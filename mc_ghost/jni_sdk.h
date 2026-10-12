#pragma once
#include <jni.h>
#include <windows.h>

// 1.8.9 obf names (vanilla). For Lunar/Badlion replace with deobf map.
namespace SDK {
  inline const char* MC = "net/minecraft/client/Minecraft";
  inline const char* ENTITY = "net/minecraft/entity/Entity";
  inline const char* ENTITYLIVING = "net/minecraft/entity/EntityLivingBase";
  inline const char* PLAYER = "net/minecraft/entity/player/EntityPlayer";
  inline const char* WORLD = "net/minecraft/client/multiplayer/WorldClient";
  inline const char* AXISBB = "net/minecraft/util/AxisAlignedBB";

  struct IDs {
    jfieldID mc_theWorld = 0;
    jfieldID mc_thePlayer = 0;
    jfieldID mc_gameSettings = 0;
    jfieldID mc_renderManager = 0;
    jfieldID entity_posX = 0, entity_posY = 0, entity_posZ = 0;
    jfieldID entity_lastX = 0, entity_lastY = 0, entity_lastZ = 0;
    jfieldID entity_width = 0, entity_height = 0;
    jfieldID entity_isDead = 0, entity_hurtTime = 0;
    jfieldID player_motionX = 0, player_motionZ = 0;
    jfieldID player_onGround = 0, player_sneaking = 0;
    jmethodID mc_getMinecraft = 0;
    jmethodID world_loadedEntityList = 0; // via getLoadedEntityList
    jmethodID entity_getDistanceTo = 0;
  };
  inline IDs ids;
  inline JavaVM* jvm = nullptr;
  inline JNIEnv* env(JNIEnv* e = nullptr) {
    if (e) return e;
    JNIEnv* out = nullptr;
    jvm->GetEnv((void**)&out, JNI_VERSION_1_6);
    if (!out) jvm->AttachCurrentThread((void**)&out, 0);
    return out;
  }
}
