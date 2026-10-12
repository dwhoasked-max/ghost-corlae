#include <windows.h>
#include <vector>
#include <cstdint>
// External pattern scan of RainbowSix.exe (.text) for GameManager lea
uintptr_t Scan(const char* sig, uintptr_t base, size_t size);
static bool SigCmp(BYTE* data, const char* sig) {
  for (; *sig; sig++, data++) {
    if (*sig == ' ') continue;
    if (*sig == '?') { if (*(sig+1)=='?') sig++; continue; }
    char buf[3]{sig[0],sig[1],0};
    if (*data != (BYTE)strtoul(buf,0,16)) return false;
    sig++;
  }
  return true;
}
uintptr_t Scan(const char* sig, uintptr_t base, size_t size) {
  for (size_t i=0;i<size;i++) if (SigCmp((BYTE*)(base+i),sig)) return base+i;
  return 0;
}
// lea rcx,[rip+disp] at +3 -> resolve GameManager pointer holder
uintptr_t ResolveLea(uintptr_t at) {
  int32_t disp = *(int32_t*)(at+3);
  return at+7+disp;
}
