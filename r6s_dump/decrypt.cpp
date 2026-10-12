#include <windows.h>
#include <cstdint>
#include "game.h"
// Y10-Y11 GameManager decrypt: enc = *(holder); dec = rol(enc ^ key, bits) + imagebase ^ secondKey
// Brute-forces rol 1-63 + validates entityCount 1-12 to survive Y11S3 patch shifts.
static uint64_t Rol64(uint64_t v, int b){ b&=63; return (v<<b)|(v>>(64-b)); }
uint64_t DecryptGameManager(HANDLE h, uintptr_t holder, uintptr_t imageBase) {
  uint64_t enc=0; ReadProcessMemory(h,(void*)holder,&enc,8,0);
  for(int r=1;r<64;r++){
    uint64_t d = Rol64(enc ^ R6::xorKey, r) + imageBase;
    uint64_t mgr=0; if(!ReadProcessMemory(h,(void*)d,&mgr,8,0)) continue;
    // mgr+0x1C8 entityList ptr, +0x1D0 count (Y11 layout, verify)
    uint64_t list=0; uint32_t cnt=0;
    ReadProcessMemory(h,(void*)(mgr+0x1C8),&list,8,0);
    ReadProcessMemory(h,(void*)(mgr+0x1D0),&cnt,4,0);
    if(cnt>=1 && cnt<=12 && (list>>32)!=0){
      R6::rolBits=(uint8_t)r;
      return mgr;
    }
  }
  // fallback: try secondKey xor path (Y11S3 alt chain)
  for(int r=1;r<64;r++){
    uint64_t d = (Rol64(enc,r) ^ R6::xorKey) - imageBase;
    uint64_t mgr=0; if(!ReadProcessMemory(h,(void*)d,&mgr,8,0)) continue;
    uint32_t cnt=0; ReadProcessMemory(h,(void*)(mgr+0x1D0),&cnt,4,0);
    if(cnt>=1 && cnt<=12){ R6::rolBits=(uint8_t)r; return mgr; }
  }
  return 0;
}
