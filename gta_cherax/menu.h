#pragma once
#include <windows.h>
#include <string>
#include <vector>

namespace Menu {
  enum Tab { SELF = 0, WEAPON, VEHICLE, TELEPORT, ONLINE, RECOVERY, SETTINGS };
  inline bool open = false;
  inline Tab current = SELF;
  inline bool initialized = false;

  void InitDX11();
  void Shutdown();
  void Render();
  void HandleKeys();
}
