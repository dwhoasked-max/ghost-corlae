#include "ghost.h"
#include "jni_sdk.h"
#include <cmath>
#include <windows.h>

static float wrapDeg(float a){ while(a>180)a-=360; while(a<-180)a+=360; return a; }

void Aim::Tick(){
  if(!Ghost::aimAssist) return;
  if(Ghost::aimOnlyClick && !(GetAsyncKeyState(VK_LBUTTON)&0x8000)) return;
  JNIEnv* e = SDK::env(); if(!e) return;
  jclass mcC = e->FindClass(SDK::MC);
  jobject mc = e->CallStaticObjectMethod(mcC, SDK::ids.mc_getMinecraft);
  if(!mc) return;
  jobject self = e->GetObjectField(mc, SDK::ids.mc_thePlayer);
  jobject world = e->GetObjectField(mc, SDK::ids.mc_theWorld);
  if(!self||!world) return;

  // self yaw/pitch: Entity.rotationYaw/rotationPitch (float)
  jclass entC = e->FindClass(SDK::ENTITY);
  jfieldID yawF = e->GetFieldID(entC,"rotationYaw","F");
  jfieldID pitchF = e->GetFieldID(entC,"rotationPitch","F");
  if(!yawF){ jclass lc=e->FindClass(SDK::ENTITYLIVING);
    yawF=e->GetFieldID(lc,"field_70759_as","F"); pitchF=e->GetFieldID(lc,"field_70761_aq","F"); }
  float yaw = e->GetFloatField(self,yawF);
  float pitch = e->GetFloatField(self,pitchF);

  double sx=e->GetDoubleField(self,SDK::ids.entity_posX);
  double sy=e->GetDoubleField(self,SDK::ids.entity_posY)+1.62;
  double sz=e->GetDoubleField(self,SDK::ids.entity_posZ);

  jclass worldC=e->GetObjectClass(world);
  jmethodID listMid=e->GetMethodID(worldC,"func_72872_a","()Ljava/util/List;");
  if(!listMid) listMid=e->GetMethodID(worldC,"getLoadedEntityList","()Ljava/util/List;");
  if(!listMid) return;
  jobject list=e->CallObjectMethod(world,listMid);
  jclass listC=e->GetObjectClass(list);
  auto sizeM=e->GetMethodID(listC,"size","()I");
  auto getM=e->GetMethodID(listC,"get","(I)Ljava/lang/Object;");
  int n=e->CallIntMethod(list,sizeM);

  float bestScore=Ghost::aimFov; jobject best=nullptr; float bestYaw=yaw,bestPitch=pitch;
  for(int i=0;i<n;i++){
    jobject ent=e->CallObjectMethod(list,getM,i);
    if(!ent||ent==self) continue;
    if(e->GetBooleanField(ent,SDK::ids.entity_isDead)) continue;
    double ex=e->GetDoubleField(ent,SDK::ids.entity_posX);
    double ey=e->GetDoubleField(ent,SDK::ids.entity_posY)+1.0;
    double ez=e->GetDoubleField(ent,SDK::ids.entity_posZ);
    double dx=ex-sx,dy=ey-sy,dz=ez-sz;
    double d=sqrt(dx*dx+dy*dy+dz*dz);
    if(d>Ghost::aimRange+14) continue; // visuals range, hit range gated by click
    float ty=(float)(atan2(-dx,dz)*180.0/3.14159);
    float tp=(float)(-asin(dy/d)*180.0/3.14159);
    float dyaw=fabs(wrapDeg(ty-yaw)), dpitch=fabs(tp-pitch);
    float score=(float)(dyaw+dpitch*0.8);
    if(score<bestScore){ bestScore=score; best=ent; bestYaw=ty; bestPitch=tp; }
    else e->DeleteLocalRef(ent);
  }
  if(best){
    float ny=yaw+wrapDeg(bestYaw-yaw)/Ghost::aimSmooth;
    float np=pitch+(bestPitch-pitch)/Ghost::aimSmooth;
    e->SetFloatField(self,yawF,ny);
    e->SetFloatField(self,pitchF,np);
    e->DeleteLocalRef(best);
  }
}
