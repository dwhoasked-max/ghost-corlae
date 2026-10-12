#pragma once
#include <cstdint>
// Y11S3 - refresh via scanner if patch shifts. Values below are pattern-relative, not hardcoded statics.
namespace R6 {
  // sigs (Y10/Y11 stable, verify in IDA)
  inline const char* SIG_GAMEMANAGER = "48 8B 0D ? ? ? ? 48 85 C9 74 ? 48 8B 01 FF 90 ? ? ? ? 48 8B C8";
  inline const char* SIG_ROUNDMANAGER = "48 8B 0D ? ? ? ? 48 8B D7 E8 ? ? ? ? 48 8B F8";
  inline const char* SIG_FOVMANAGER = "48 8B 05 ? ? ? ? 48 8B 88 ? ? ? ? 48 8B 01";
  struct GameMgr {
    uint64_t decrypted = 0;
    uint64_t entityList = 0;
    uint32_t entityCount = 0;
  };
  // decrypt params - brute-forced per build by decrypt.cpp, defaults Y11S2
  inline uint8_t rolBits = 0x1D;
  inline uint64_t xorKey = 0x5A3F2C1E9B4D8071ULL;
}
