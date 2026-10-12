// LoadLibrary injector for CheraxLike.dll - x64, SeDebugPrivilege
#include <windows.h>
#include <tlhelp32.h>
#include <string>
#include <cstdio>

static DWORD FindPid(const wchar_t* exe) {
  DWORD pid = 0;
  HANDLE s = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  PROCESSENTRY32W pe{ sizeof(pe) };
  if (Process32FirstW(s, &pe)) do {
    if (!_wcsicmp(pe.szExeFile, exe)) { pid = pe.th32ProcessID; break; }
  } while (Process32NextW(s, &pe));
  CloseHandle(s);
  return pid;
}

int wmain(int argc, wchar_t** argv) {
  const wchar_t* target = L"GTA5.exe";
  std::wstring dll = L"C:\\CheraxLike\\CheraxLike.dll";
  if (argc >= 3) { target = argv[1]; dll = argv[2]; }
  DWORD pid = FindPid(target);
  if (!pid) { printf("target not found\n"); return 1; }
  HANDLE h = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
  if (!h) { printf("open %lu\n", GetLastError()); return 1; }
  size_t sz = (dll.size() + 1) * sizeof(wchar_t);
  LPVOID m = VirtualAllocEx(h, 0, sz, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
  WriteProcessMemory(h, m, dll.c_str(), sz, 0);
  HANDLE t = CreateRemoteThread(h, 0, 0,
    (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleW(L"kernel32"), "LoadLibraryW"),
    m, 0, 0);
  WaitForSingleObject(t, INFINITE);
  VirtualFreeEx(h, m, 0, MEM_RELEASE);
  CloseHandle(t); CloseHandle(h);
  printf("injected %ls -> %ls\n", dll.c_str(), target);
  return 0;
}
