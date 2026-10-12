#include "features.h"
#include "menu.h"

void Features::SelfTick(int player, int ped) {
  if (godmode) {
    PLAYER::SET_PLAYER_INVINCIBLE(player, true);
    ENTITY::SET_ENTITY_HEALTH(ped, 328, 0);
    PED::SET_PED_ARMOUR(ped, 100);
  } else {
    PLAYER::SET_PLAYER_INVINCIBLE(player, false);
  }
  if (infiniteStamina) {
    PED::SET_PED_INFINITE_STAMINA(ped, false, true);
    PED::RESET_PED_STAMINA(ped);
  }
  if (superJump) GAMEPLAY::SET_SUPER_JUMP_THIS_FRAME(player);
  if (fastRun) PLAYER::SET_RUN_SPRINT_MULTIPLIER_FOR_PLAYER(player, runMult);
  else PLAYER::SET_RUN_SPRINT_MULTIPLIER_FOR_PLAYER(player, 1.0f);
  if (invisible) ENTITY::SET_ENTITY_VISIBLE(ped, false, false);
  else ENTITY::SET_ENTITY_VISIBLE(ped, true, false);
  PED::SET_PED_CAN_RAGDOLL(ped, !noRagdoll);
  if (clearWantedTick) {
    PLAYER::SET_PLAYER_WANTED_LEVEL(player, 0, false);
    PLAYER::SET_PLAYER_WANTED_LEVEL_NOW(player, false);
  }
}

void Features::WeaponTick(int ped) {
  if (infAmmo) {
    WEAPON::SET_PED_INFINITE_AMMO_CLIP(ped, true);
    WEAPON::SET_PED_INFINITE_AMMO(ped, true, 0);
  } else {
    WEAPON::SET_PED_INFINITE_AMMO_CLIP(ped, false);
  }
  if (explosiveAmmo) WEAPON::SET_EXPLOSIVE_AMMO_THIS_FRAME(ped);
  if (explosiveMelee) WEAPON::SET_EXPLOSIVE_MELEE_THIS_FRAME(ped);
  WEAPON::SET_WEAPON_DAMAGE_MODIFIER(ped, damageMult);
}

void Features::VehicleTick(int ped) {
  int veh = PED::GET_VEHICLE_PED_IS_IN(ped, false);
  if (veh && vehGod) {
    VEHICLE::SET_VEHICLE_INVINCIBLE(veh, true);
    VEHICLE::SET_VEHICLE_ENGINE_HEALTH(veh, 1000.0f);
    VEHICLE::SET_VEHICLE_BODY_HEALTH(veh, 1000.0f);
  }
}

void Features::Heal() {
  int p = PLAYER::PLAYER_PED_ID();
  ENTITY::SET_ENTITY_HEALTH(p, 328, 0);
  PED::SET_PED_ARMOUR(p, 100);
}

void Features::ClearWanted() {
  int pl = PLAYER::PLAYER_ID();
  PLAYER::SET_PLAYER_WANTED_LEVEL(pl, 0, false);
  PLAYER::SET_PLAYER_WANTED_LEVEL_NOW(pl, false);
}

void Features::RepairVehicle(int ped) {
  int v = PED::GET_VEHICLE_PED_IS_IN(ped, false);
  if (v) VEHICLE::SET_VEHICLE_FIXED(v);
}

void Features::BoostVehicle(int ped) {
  int v = PED::GET_VEHICLE_PED_IS_IN(ped, false);
  if (v) VEHICLE::SET_VEHICLE_FORWARD_SPEED(v, vehBoostSpeed);
}

void Features::TeleportTo(float x, float y, float z) {
  int ped = PLAYER::PLAYER_PED_ID();
  int v = PED::GET_VEHICLE_PED_IS_IN(ped, false);
  int e = v ? v : ped;
  // UD-safe TP: single SET_ENTITY_COORDS, no tick loop, preserves velocity = 0
  ENTITY::SET_ENTITY_VELOCITY(e, 0, 0, 0);
  ENTITY::SET_ENTITY_COORDS(e, x, y, z, false, false, false, false);
}

void Features::TeleportForward(float dist) {
  // Real build: GET_ENTITY_FORWARD_VECTOR + GET_ENTITY_COORDS + SET_ENTITY_COORDS
  // Keeps Z from ground probe to avoid fall-through / detection by position sanity
  int ped = PLAYER::PLAYER_PED_ID();
  N::Vector3 c = ENTITY::GET_ENTITY_COORDS(ped, true);
  TeleportTo(c.x, c.y + dist, c.z);
}

void Features::ThrowClosestVehicle(float power) {
  // Real build: GET_CLOSEST_VEHICLE(coords, radius) + APPLY_FORCE_TO_ENTITY
  // + SET_ENTITY_VELOCITY toward crosshair. Memory path: CVehicle + CPhysics velocity write.
  int ped = PLAYER::PLAYER_PED_ID();
  N::Vector3 c = ENTITY::GET_ENTITY_COORDS(ped, true);
  (void)c; (void)power;
  // Example with SDK:
  // int veh = VEHICLE::GET_CLOSEST_VEHICLE(c.x, c.y, c.z, 30.0f, 0, 70);
  // if (veh) ENTITY::SET_ENTITY_VELOCITY(veh, 0, power, power * 0.5f);
}

void Features::ApplyUDProfile() {
  // onlineSafeMode disables flagged natives: money STAT_SET, explosive ammo spam,
  // rapid TP loop. Story mode allows all.
  if (onlineSafeMode) {
    explosiveAmmo = false;
    // keep: godmode, inf ammo clip (client), TP single-shot, vehicle throw single-shot
  }
}

void Features::SpawnVehicleByName(const char* name) {
  // Real impl: STREAMING::REQUEST_MODEL(hash) loop + CREATE_VEHICLE
  // hash = GAMEPLAY::GET_HASH_KEY(name); kept here for SDK swap-in
  (void)name;
}
