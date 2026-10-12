#include <windows.h>
#include "menu.h"
#include "features.h"
#include "natives_stub.h"

// Replace with real SDK in VS:
// #include "ScriptHookV.h"
// void ScriptMain() { while(true){ tick; WAIT(0);} }

static bool g_running = true;

static DWORD WINAPI ScriptThread(LPVOID) {
  int player = 0;
  int ped = 0;
  // In real build: player = PLAYER::PLAYER_ID(); ped = PLAYER::PLAYER_PED_ID();
  while (g_running) {
    Menu::HandleKeys();
    if (Menu::open) Menu::Render();
    // Real build tick:
    // Features::SelfTick(player, ped);
    // Features::WeaponTick(ped);
    // Features::VehicleTick(ped);
    Sleep(10);
  }
  return 0;
}

BOOL APIENTRY DllMain(HMODULE hMod, DWORD reason, LPVOID) {
  if (reason == DLL_PROCESS_ATTACH) {
    DisableThreadLibraryCalls(hMod);
    CreateThread(nullptr, 0, ScriptThread, nullptr, 0, nullptr);
  }
  if (reason == DLL_PROCESS_DETACH) g_running = false;
  return TRUE;
}
