#include "menu.h"
#include "features.h"
#include <windows.h>

// DX11 ImGui hook target (real build):
// Present hook via kiero + MinHook, ImGui::Begin("CheraxLike")
// Simplified keyboard + console fallback so logic is testable without overlay.

void Menu::HandleKeys() {
  static bool wasDown = false;
  bool isDown = (GetAsyncKeyState(VK_INSERT) & 0x8000) != 0;
  if (isDown && !wasDown) open = !open;
  wasDown = isDown;
  if (GetAsyncKeyState(VK_F5) & 0x8000) Features::Heal();
  if (GetAsyncKeyState(VK_F6) & 0x8000) Features::ClearWanted();
  if (GetAsyncKeyState(VK_F7) & 0x8000) {
    int dummyPed = 0;
    Features::RepairVehicle(dummyPed);
  }
}

void Menu::InitDX11() { initialized = true; }
void Menu::Shutdown() { initialized = false; }

void Menu::Render() {
  // Real build: ImGui tabs SELF/WEAPON/VEHICLE/TELEPORT/ONLINE/RECOVERY
  // Checkbox(&Features::godmode) etc. + Button Heal/Clear/Teleport/Spawn
  // Teleports: Maze Bank (-75, -819, 326), Airport (-1034,-2736,20), Sandy (1980,3847,32)
  // Spawn: adder, oppressor2, deluxo, bati, zentorno via Features::SpawnVehicleByName
}
