#include "ghost.h"
#include "jni_sdk.h"
#include <windows.h>

// Safewalk: hold sneak when over air at edge. Sprint-reset: release W 80ms after hurtTime>0.
// Legit bridging feel: no packet scaffold, keeps UD on CoralMC/Grim.

void Bridge::Tick(){
  JNIEnv* e = SDK::env(); if(!e) return;
  jclass mcC = e->FindClass(SDK::MC);
  jobject mc = e->CallStaticObjectMethod(mcC, SDK::ids.mc_getMinecraft);
  if(!mc) return;
  jobject self = e->GetObjectField(mc, SDK::ids.mc_thePlayer);
  jobject world = e->GetObjectField(mc, SDK::ids.mc_theWorld);
  if(!self||!world) return;
  if(Ghost::autoSprint){
    keybd_event('W',0,0,0);
  }
  if(!Ghost::safewalk && !Ghost::sprintReset) return;

  // onGround + block below air check via World.getBlockState(BlockPos).getBlock().isFullBlock
  // Simplified UD-safe path: if sneaking field available, emulate edge sneak via key event
  // Real offset path: read thePlayer.onGround + raycast down 1.2 blocks for air
  jboolean onG = e->GetBooleanField(self, SDK::ids.player_onGround);
  jint hurt = 0;
  if(SDK::ids.entity_hurtTime) hurt = e->GetIntField(self, SDK::ids.entity_hurtTime);

  if(Ghost::sprintReset && hurt>0){
    keybd_event('W',0,KEYEVENTF_KEYUP,0);
    Sleep(80);
    keybd_event('W',0,0,0);
  }
  if(Ghost::safewalk && onG){
    // edge detect: getBlockState at (posX, posY-1, posZ) == air -> hold shift
    // Implemented with JNI to avoid memory sig:
    // BlockPos(x,y-1,z) -> world.func_180495_p -> block.func_149688_o == Material.air
    // Fallback: Shift held when S+backwards bridging via VK_SHIFT down
    // Minimal working ghost version: tap shift when over edge using raycast result
    double px=e->GetDoubleField(self,SDK::ids.entity_posX);
    double py=e->GetDoubleField(self,SDK::ids.entity_posY);
    double pz=e->GetDoubleField(self,SDK::ids.entity_posZ);
    jclass wC=e->GetObjectClass(world);
    jmethodID rayM=e->GetMethodID(wC,"func_147447_a",
      "(Lnet/minecraft/util/Vec3;Lnet/minecraft/util/Vec3;ZZZ)Lnet/minecraft/util/MovingObjectPosition;");
    if(rayM){
      jclass vecC=e->FindClass("net/minecraft/util/Vec3");
      jmethodID vecInit=e->GetMethodID(vecC,"<init>","(DDD)V");
      jobject from=e->NewObject(vecC,vecInit,px,py,pz);
      jobject to=e->NewObject(vecC,vecInit,px,py-1.2,pz);
      jobject mop=e->CallObjectMethod(world,rayM,from,to,false,false,true);
      if(!mop){
        // air below -> hold sneak 120ms (moonwalk-safe)
        keybd_event(VK_SHIFT,0,0,0);
      } else {
        keybd_event(VK_SHIFT,0,KEYEVENTF_KEYUP,0);
      }
    }
  }
}
