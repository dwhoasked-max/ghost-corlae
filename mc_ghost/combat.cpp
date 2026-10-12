#include "ghost.h"
#include "jni_sdk.h"
#include <random>

// Vape-like Combat: Reach + Velocity + NoSlow + Sprint + WTap + HitBoxes
// 1.8.9 vanilla paths, Grim-safe percentages.

static std::mt19937 g_rng(GetTickCount());

double Combat::GetReach() {
  std::uniform_real_distribution<double> d(Ghost::reachMin, Ghost::reachMax);
  return d(g_rng);
}

void Combat::OnHurt() {
  // velocity: scale motion on hurtTime==max, chance roll
  if (!Ghost::velocity) return;
  JNIEnv* e = SDK::env(); if (!e) return;
  jclass mcC = e->FindClass(SDK::MC);
  jobject mc = e->CallStaticObjectMethod(mcC, SDK::ids.mc_getMinecraft);
  if (!mc) return;
  jobject self = e->GetObjectField(mc, SDK::ids.mc_thePlayer);
  if (!self) return;
  jint hurt = e->GetIntField(self, SDK::ids.entity_hurtTime);
  if (hurt <= 0) return;
  std::uniform_int_distribution<int> ch(0, 99);
  if (ch(g_rng) >= Ghost::velChance) return;
  double mx = e->GetDoubleField(self, SDK::ids.player_motionX);
  double my = 0, mz = 0;
  // motionY/Z via Entity fields (pos deltas already in IDs for X, reuse for Z)
  jclass ec = e->GetObjectClass(self);
  jfieldID mY = e->GetFieldID(ec, "field_70181_x", "D"); // motionY
  jfieldID mZ = e->GetFieldID(ec, "field_70179_y", "D"); // motionZ (obf order X/Y/Z)
  if (!mY) mY = e->GetFieldID(ec, "motionY", "D");
  if (!mZ) mZ = e->GetFieldID(ec, "motionZ", "D");
  if (mY) my = e->GetDoubleField(self, mY);
  if (mZ) mz = e->GetDoubleField(self, mZ);
  e->SetDoubleField(self, SDK::ids.player_motionX, mx * (Ghost::velH / 100.0));
  if (mY) e->SetDoubleField(self, mY, my * (Ghost::velV / 100.0));
  if (mZ) e->SetDoubleField(self, mZ, mz * (Ghost::velH / 100.0));
}

void Combat::ApplyNoSlow(void* venv, jobject player) {
  // NoSlow: clear isBlocking slowdown by spoofing itemInUseCount=0 + sprint keep
  // Vanilla: EntityPlayer.isBlocking() -> itemInUseCount>0. Zero it each tick while moving.
  JNIEnv* e = (JNIEnv*)venv;
  jclass pc = e->GetObjectClass(player);
  jfieldID useCount = e->GetFieldID(pc, "field_71069_bz", "I");
  if (!useCount) useCount = e->GetFieldID(pc, "itemInUseCount", "I");
  if (useCount && Ghost::noSlow) {
    // only spoof movement calc, keep block animation server-side
    // read keys: if W held + blocking, zero count for 1 tick
    if (GetAsyncKeyState('W') & 0x8000) e->SetIntField(player, useCount, 0);
  }
  if (Ghost::sprint) {
    jmethodID setSprint = e->GetMethodID(pc, "func_70031_b", "(Z)V");
    if (!setSprint) setSprint = e->GetMethodID(pc, "setSprinting", "(Z)V");
    if (setSprint) e->CallVoidMethod(player, setSprint, true);
  }
}

void Combat::Tick() {
  if (!Ghost::reach && !Ghost::velocity && !Ghost::noSlow && !Ghost::sprint && !Ghost::wTap) return;
  JNIEnv* e = SDK::env(); if (!e) return;
  jclass mcC = e->FindClass(SDK::MC);
  jobject mc = e->CallStaticObjectMethod(mcC, SDK::ids.mc_getMinecraft);
  if (!mc) return;
  jobject self = e->GetObjectField(mc, SDK::ids.mc_thePlayer);
  jobject ctrl = nullptr;
  // playerController for attackEntity override (reach)
  jfieldID pcF = e->GetFieldID(mcC, "field_71442_b", "Lnet/minecraft/client/multiplayer/PlayerControllerMP;");
  if (!pcF) pcF = e->GetFieldID(mcC, "playerController", "Lnet/minecraft/client/multiplayer/PlayerControllerMP;");
  if (pcF) ctrl = e->GetObjectField(mc, pcF);
  if (!self) return;

  ApplyNoSlow(e, self);
  OnHurt();

  // Reach: replace vanilla 3.0 attack with extended raycast attack
  // Hook point is EntityRenderer.getMouseOver (3.0D). Here we pre-attack closest in custom reach
  // on LMB so CoralMC hit registers while vanilla ray still shows 3.0.
  if (Ghost::reach && (GetAsyncKeyState(VK_LBUTTON) & 0x8000)) {
    jobject world = e->GetObjectField(mc, SDK::ids.mc_theWorld);
    if (!world || !ctrl) return;
    double sx = e->GetDoubleField(self, SDK::ids.entity_posX);
    double sy = e->GetDoubleField(self, SDK::ids.entity_posY) + 1.62;
    double sz = e->GetDoubleField(self, SDK::ids.entity_posZ);
    jclass wC = e->GetObjectClass(world);
    jmethodID listM = e->GetMethodID(wC, "func_72872_a", "()Ljava/util/List;");
    if (!listM) listM = e->GetMethodID(wC, "getLoadedEntityList", "()Ljava/util/List;");
    if (!listM) return;
    jobject list = e->CallObjectMethod(world, listM);
    jclass lC = e->GetObjectClass(list);
    auto szM = e->GetMethodID(lC, "size", "()I");
    auto gM = e->GetMethodID(lC, "get", "(I)Ljava/lang/Object;");
    int n = e->CallIntMethod(list, szM);
    double want = GetReach();
    jobject best = nullptr; double bestD = want;
    for (int i = 0; i < n; i++) {
      jobject ent = e->CallObjectMethod(list, gM, i);
      if (!ent || ent == self) continue;
      if (e->GetBooleanField(ent, SDK::ids.entity_isDead)) continue;
      double ex = e->GetDoubleField(ent, SDK::ids.entity_posX);
      double ey = e->GetDoubleField(ent, SDK::ids.entity_posY);
      double ez = e->GetDoubleField(ent, SDK::ids.entity_posZ);
      double dx = ex - sx, dy = ey - sy, dz = ez - sz;
      double d = sqrt(dx*dx + dy*dy + dz*dz);
      if (d < bestD && d > 3.0) { if (best) e->DeleteLocalRef(best); best = ent; bestD = d; }
      else e->DeleteLocalRef(ent);
    }
    if (best) {
      jclass cC = e->GetObjectClass(ctrl);
      jmethodID atk = e->GetMethodID(cC, "func_78764_a", "(Lnet/minecraft/entity/player/EntityPlayer;Lnet/minecraft/entity/Entity;)V");
      if (!atk) atk = e->GetMethodID(cC, "attackEntity", "(Lnet/minecraft/entity/player/EntityPlayer;Lnet/minecraft/entity/Entity;)V");
      if (atk) e->CallVoidMethod(ctrl, atk, self, best);
      e->DeleteLocalRef(best);
    }
  }

  // HitBoxes expand: Entity.setEntityBoundingBox expand client-side
  if (Ghost::hitBoxes) {
    // applied in visuals tick to loaded entities: bb = bb.expand(expand, expand*0.5, expand)
    // kept here as flag for Visuals::OnSwapBuffers expansion path
  }
}
