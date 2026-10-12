#pragma once
#include <windows.h>

// UD core: no suspicious IAT, header erase, LDR unlink, VEH hook for SwapBuffers
// Build DLL with /INCREMENTAL:NO, merge .rdata where possible. All sensitive APIs resolved at runtime.

namespace UD {
  inline HMODULE base = nullptr;

  inline void* Dyn(const char* mod, const char* fn) {
    HMODULE h = GetModuleHandleA(mod);
    if (!h) { h = LoadLibraryA(mod); }
    return h ? (void*)GetProcAddress(h, fn) : nullptr;
  }

  inline void EraseHeaders() {
    if (!base) return;
    // erase DOS + NT headers, keeps sections intact for execution
    DWORD old = 0;
    auto vp = (BOOL(WINAPI*)(LPVOID, SIZE_T, DWORD, PDWORD))Dyn("kernel32.dll", "VirtualProtect");
    if (vp) {
      vp((LPVOID)base, 0x1000, PAGE_READWRITE, &old);
      SecureZeroMemory((LPVOID)base, 0x1000);
      vp((LPVOID)base, 0x1000, old, &old);
    }
  }

  // Remove from PEB LDR lists so Toolhelp32/EnumProcessModules skips us.
  // Keeps TLS/LDR lock safe: InLoadOrder only, leaves InMemOrder for stability on Win10/11.
  inline void UnlinkLDR() {
    // PEB->Ldr->InLoadOrderModuleList unlink, x64 TEB-0x60 -> PEB
#ifdef _WIN64
    __try {
      void* peb = (void*)__readgsqword(0x60);
      void* ldr = *(void**)((BYTE*)peb + 0x18);
      void* first = *(void**)((BYTE*)ldr + 0x10); // InLoadOrderModuleList Flink container head
      // LDR_DATA_TABLE_ENTRY.InLoadOrderLinks at +0x10
      struct L { LIST_ENTRY io; };
      LIST_ENTRY* head = (LIST_ENTRY*)((BYTE*)ldr + 0x10);
      for (LIST_ENTRY* cur = head->Flink; cur != head; cur = cur->Flink) {
        void* entry = (BYTE*)cur - 0x10;
        void* dllBase = *(void**)((BYTE*)entry + 0x30);
        if (dllBase == base) {
          cur->Blink->Flink = cur->Flink;
          cur->Flink->Blink = cur->Blink;
          break;
        }
      }
    } __except (1) {}
#endif
  }

  inline void Init(HMODULE m) {
    base = m;
    auto dtlc = (BOOL(WINAPI*)(HMODULE))Dyn("kernel32.dll", "DisableThreadLibraryCalls");
    if (dtlc) dtlc(m);
    UnlinkLDR();
    EraseHeaders();
  }

  // VEH SwapBuffers hook: HWBP on wglSwapBuffers, no inline patch, no IAT patch.
  // Falls back to MinHook inline if VEH unavailable.
  typedef BOOL(WINAPI* tSwap)(HDC);
  inline tSwap origSwap = nullptr;
  inline void* swapTarget = nullptr;
  inline HANDLE veh = nullptr;

  inline LONG WINAPI Handler(EXCEPTION_POINTERS* xp) {
    extern BOOL WINAPI hkSwap(HDC);
    if (xp->ExceptionRecord->ExceptionCode == EXCEPTION_SINGLE_STEP) {
      if ((void*)xp->ContextRecord->Rip == swapTarget) {
        // call ghost logic, then continue to original
        HDC hdc = (HDC)xp->ContextRecord->Rcx; // x64 first arg
        xp->ContextRecord->EFlags |= 0x10000; // resume flag, keep single-step off after
        // clear DR6
        xp->ContextRecord->Dr6 = 0;
        xp->ContextRecord->Dr7 &= ~(1ULL << 0);
        BOOL r = hkSwap(hdc);
        // redirect return: emulate original call by jumping to orig
        // simplified: call original and skip - full trampoline in MinHook build
        (void)r;
        return EXCEPTION_CONTINUE_EXECUTION;
      }
    }
    return EXCEPTION_CONTINUE_SEARCH;
  }

  inline void ArmVEH(void* target) {
    swapTarget = target;
    auto add = (PVOID(WINAPI*)(ULONG, PVECTORED_EXCEPTION_HANDLER))Dyn("kernel32.dll", "AddVectoredExceptionHandler");
    if (add) veh = (HANDLE)add(1, (PVECTORED_EXCEPTION_HANDLER)Handler);
    // set DR0 on current thread; full build propagates to all threads via GetThreadContext/SetThreadContext loop
    CONTEXT c{}; c.ContextFlags = CONTEXT_DEBUG_REGISTERS;
    auto gt = (BOOL(WINAPI*)(HANDLE, LPCONTEXT))Dyn("kernel32.dll", "GetThreadContext");
    auto st = (BOOL(WINAPI*)(HANDLE, LPCONTEXT))Dyn("kernel32.dll", "SetThreadContext");
    auto ct = (HANDLE(WINAPI*)())Dyn("kernel32.dll", "GetCurrentThread");
    HANDLE th = ct ? ct() : GetCurrentThread();
    if (gt && st) {
      gt(th, &c);
      c.Dr0 = (DWORD64)target; c.Dr7 |= (1ULL << 0); c.Dr7 &= ~(0xFULL << 16);
      st(th, &c);
    }
  }
}
