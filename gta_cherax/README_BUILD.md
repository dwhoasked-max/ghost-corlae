TARGET = GTA V Story (1.68+ / Enhanced) + FiveM-safe memory path
TYPE = internal DLL, ScriptHookV + DX11 ImGui, x64
KEY = INSERT opens menu, NUMPAD5 / ENTER confirms

BUILD:
1. Install VS2022 + Windows SDK + ScriptHookV SDK (ScriptHookV.h, natives.h, types.h)
2. Create DLL x64 Release, C++17
3. Add files from this folder to project, add include path to ScriptHookV SDK + ImGui + MinHook
4. Link: d3d11.lib dxgi.lib minhook.lib
5. Output CheraxLike.asi / .dll -> copy to GTA V root (with ScriptHookV.dll + dinput8.dll)
6. Injector.exe for manual-map / LoadLibrary if not using .asi

STRUCTURE:
 dllmain.cpp:19 -> DllMain + ScriptMain thread
 menu.h:1 -> Menu state + tabs
 features.h:1 -> Self/Weapon/Vehicle/Teleport/Online/Recovery impl
 injector.cpp:1 -> LoadLibrary injector
