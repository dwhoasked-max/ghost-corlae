#pragma once
#include "natives_stub.h"

struct Features {
  // Self
  inline static bool godmode = false;
  inline static bool infiniteStamina = true;
  inline static bool superJump = false;
  inline static bool fastRun = false;
  inline static bool invisible = false;
  inline static bool noRagdoll = false;
  inline static bool clearWantedTick = true;
  inline static float runMult = 1.49f;

  // Weapon
  inline static bool infAmmo = false;
  inline static bool noReload = false;
  inline static bool explosiveAmmo = false;
  inline static bool explosiveMelee = false;
  inline static float damageMult = 1.0f;

  // Vehicle
  inline static bool vehGod = false;
  inline static float vehBoostSpeed = 120.0f;

  // Online / grief - vehicle thrower
  inline static bool vehicleGun = false;
  inline static bool deleteGun = false;
  inline static float throwPower = 150.0f;

  // UD profile: story = full natives, online = memory-safe only
  inline static bool onlineSafeMode = true;

  static void SelfTick(int player, int ped);
  static void WeaponTick(int ped);
  static void VehicleTick(int ped);
  static void Heal();
  static void ClearWanted();
  static void RepairVehicle(int ped);
  static void BoostVehicle(int ped);
  static void TeleportTo(float x, float y, float z);
  static void TeleportToWaypoint();
  static void TeleportForward(float dist);
  static void SpawnVehicleByName(const char* name);
  static void ThrowClosestVehicle(float power);
  static void ApplyUDProfile();
};
