#pragma once
#include <windows.h>

struct Ghost {
  // visuals - Vape style
  inline static bool espBox = true;
  inline static bool tracers = false;
  inline static bool nametags2D = true;
  inline static bool chams = false;
  inline static bool itemESP = false;
  inline static float espMaxDist = 80.0f;
  inline static bool antiBot = true; // filter NPC/tablist bots

  // aim - Vape AimAssist
  inline static bool aimAssist = true;
  inline static float aimFov = 45.0f;      // vape uses large fov + distance check
  inline static float aimSmooth = 18.0f;
  inline static float aimRange = 4.5f;
  inline static bool aimOnlyClick = true;
  inline static bool aimCheckBlock = true;  // no aim through walls
  inline static bool aimTargetInvisible = false;

  // combat - Vape Combat tab
  inline static bool reach = true;
  inline static float reachMin = 3.1f, reachMax = 3.45f;
  inline static bool velocity = true;
  inline static float velH = 82.0f, velV = 100.0f; // % taken
  inline static int velChance = 100;
  inline static bool noSlow = true;
  inline static bool sprint = true;   // keep sprint
  inline static bool wTap = true;
  inline static bool hitBoxes = false;
  inline static float hitExpand = 0.12f;

  // bridging / movement
  inline static bool safewalk = true;
  inline static bool sprintReset = true;
  inline static bool autoSprint = false;
  inline static bool fastPlace = false;

  // clicker - Vape Left/Right
  inline static bool clicker = true;
  inline static int cpsMin = 9, cpsMax = 14;
  inline static bool rClicker = false;
  inline static int rCpsMin = 12, rCpsMax = 16;
  inline static bool breakBlocks = true; // only click entities/blocks legit

  inline static bool menuOpen = false;
  inline static int profile = 0; // 0 legit / 1 semi / 2 closet
};

namespace Visuals { void OnSwapBuffers(HDC hdc); bool WorldToScreen(double x,double y,double z,float* sx,float* sy); bool IsBot(void* env, jobject ent, jobject tabList); }
namespace Aim { void Tick(); }
namespace Bridge { void Tick(); }
namespace Clicker { void Tick(); }
namespace Combat { void Tick(); double GetReach(); void OnHurt(); void ApplyNoSlow(void* env, jobject player); }
namespace Config { void Save(const wchar_t* path); void Load(const wchar_t* path); void ApplyProfile(int p); }
