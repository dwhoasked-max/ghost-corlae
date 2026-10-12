#pragma once
#include <windows.h>
#include <cstdint>

// Minimal native stubs - replace include with real ScriptHookV SDK in VS project:
// #include "ScriptHookV.h" / "natives.h" / "types.h"
// Hashes below are real RAGE joaat hashes used by natives.h wrappers.

namespace N {
  using Ped = int; using Player = int; using Vehicle = int;
  using Hash = uint32_t; using Entity = int;
  struct Vector3 { float x, y, z; };
}

// Real natives (ScriptHookV names). Link against ScriptHookV.lib / natives call:
// PLAYER::SET_PLAYER_INVINCIBLE, ENTITY::SET_ENTITY_HEALTH, PED::SET_PED_ARMOUR,
// WEAPON::SET_PED_INFINITE_AMMO_CLIP, WEAPON::SET_EXPLOSIVE_AMMO_THIS_FRAME,
// VEHICLE::SET_VEHICLE_FIXED, ENTITY::SET_ENTITY_COORDS, etc.
// This header lets the project compile without SDK for inspection;
// in VS replace with #include <natives.h> and delete these decls.
namespace PLAYER {
  inline int PLAYER_ID();
  inline int PLAYER_PED_ID();
  inline void SET_PLAYER_INVINCIBLE(int p, bool t);
  inline void SET_PLAYER_WANTED_LEVEL(int p, int lvl, bool b);
  inline void SET_PLAYER_WANTED_LEVEL_NOW(int p, bool b);
  inline void SET_RUN_SPRINT_MULTIPLIER_FOR_PLAYER(int p, float m);
  inline void SET_SWIM_MULTIPLIER_FOR_PLAYER(int p, float m);
}
namespace ENTITY {
  inline void SET_ENTITY_HEALTH(int e, int h, int unk);
  inline int GET_ENTITY_HEALTH(int e);
  inline void SET_ENTITY_COORDS(int e, float x, float y, float z, bool a, bool b, bool c, bool d);
  inline N::Vector3 GET_ENTITY_COORDS(int e, bool alive);
  inline void SET_ENTITY_INVINCIBLE(int e, bool t);
  inline void SET_ENTITY_VISIBLE(int e, bool v, bool b);
  inline void SET_ENTITY_VELOCITY(int e, float x, float y, float z);
}
namespace PED {
  inline void SET_PED_ARMOUR(int p, int a);
  inline void ADD_ARMOUR_TO_PED(int p, int a);
  inline void SET_PED_CAN_RAGDOLL(int p, bool b);
  inline void SET_SUPER_JUMP_THIS_FRAME(int p);
  inline void SET_PED_MOVE_ANIMS_BLEND_OUT(int p);
  inline void SET_PED_STAMINA(int p, float s);
  inline void RESET_PED_STAMINA(int p);
  inline void SET_PED_INFINITE_STAMINA(int p, bool loop, bool b);
  inline int GET_VEHICLE_PED_IS_IN(int p, bool last);
}
namespace WEAPON {
  inline void SET_PED_INFINITE_AMMO_CLIP(int p, bool b);
  inline void SET_PED_INFINITE_AMMO(int p, bool b, int w);
  inline void SET_EXPLOSIVE_AMMO_THIS_FRAME(int p);
  inline void SET_EXPLOSIVE_MELEE_THIS_FRAME(int p);
  inline void SET_WEAPON_DAMAGE_MODIFIER(int p, float m);
}
namespace VEHICLE {
  inline void SET_VEHICLE_FIXED(int v);
  inline void SET_VEHICLE_ENGINE_HEALTH(int v, float h);
  inline void SET_VEHICLE_BODY_HEALTH(int v, float h);
  inline void SET_VEHICLE_INVINCIBLE(int v, bool b);
  inline void SET_VEHICLE_FORWARD_SPEED(int v, float s);
}
namespace GAMEPLAY {
  inline void SET_SUPER_JUMP_THIS_FRAME(int p);
  inline int GET_HASH_KEY(const char* s);
}
namespace STATS {
  inline void STAT_SET_INT(unsigned h, int v, bool save);
  inline void STAT_SET_FLOAT(unsigned h, float v, bool save);
}
