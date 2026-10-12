#include <windows.h>
#include <cstdio>
#include <cstdint>
#include "game.h"
uintptr_t Scan(const char*,uintptr_t,size_t);
uintptr_t ResolveLea(uintptr_t);
uint64_t DecryptGameManager(HANDLE,uintptr_t,uintptr_t);
int main(){
  HWND w=FindWindowA(0,"Rainbow Six");
  if(!w){ printf("open R6S Y11S3 first\n"); return 1; }
  DWORD pid=0; GetWindowThreadProcessId(w,&pid);
  HANDLE h=OpenProcess(PROCESS_VM_READ|PROCESS_QUERY_INFORMATION,FALSE,pid);
  HMODULE mods[1]; DWORD need=0; EnumProcessModules(h,mods,8,&need);
  uintptr_t base=(uintptr_t)mods[0];
  MODULEINFO mi{}; GetModuleInformation(h,mods[0],&mi,19);
  uintptr_t at=Scan(R6::SIG_GAMEMANAGER,base,mi.SizeOfImage);
  if(!at){ printf("sig fail - update SIG_GAMEMANAGER\n"); return 1; }
  uintptr_t holder=ResolveLea(at);
  uint64_t mgr=DecryptGameManager(h,holder,base);
  if(!mgr){ printf("decrypt fail rol=%d\n",R6::rolBits); return 1; }
  uint64_t list=0; uint32_t cnt=0;
  ReadProcessMemory(h,(void*)(mgr+0x1C8),&list,8,0);
  ReadProcessMemory(h,(void*)(mgr+0x1D0),&cnt,4,0);
  printf("Y11S3 GameManager=0x%llx list=0x%llx count=%u holder=0x%llx\n",mgr,list,cnt,holder);
  for(uint32_t i=0;i<cnt;i++){
    uint64_t ent=0; ReadProcessMemory(h,(void*)(list+i*8),&ent,8,0);
    uint32_t hp=0; ReadProcessMemory(h,(void*)(ent+0x1A8),&hp,4,0);
    printf("[%u] ent=0x%llx hp=%u\n",i,ent,hp);
  }
  return 0;
}
