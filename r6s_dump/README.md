TARGET = R6S Y11S3 (Vulkan, BattlEye), RainbowSix.exe x64
METHOD = external DMA-safe + internal pattern, GameManager decrypt + EntityList walk
BUILD VS2022 x64 Release C++17, link ntdll.lib

FILES:
 game.h:1 -> offsets struct for Y11S3 (update via dumper)
 scanner.cpp:1 -> pattern scan RainbowSix.exe
 decrypt.cpp:1 -> GameManager decrypt (rol/xor chain)
 reader.cpp:1 -> EntityList + health/team/pos + w2s
 main.cpp:1 -> dump + validate
