#include <windows.h>
#include "ghost.h"
#include "jni_sdk.h"
#include "loader.h"
#include "stealth.h"
#include <GL/gl.h>

typedef BOOL (WINAPI* tSwap)(HDC);
static tSwap oSwap=nullptr;
static HMODULE hMod=nullptr;
static bool running=true;

static void ResolveIDs(){
  JNIEnv* e=SDK::env(); if(!e) return;
  jclass mc=e->FindClass(SDK::MC);
  SDK::ids.mc_getMinecraft=e->GetStaticMethodID(mc,"func_71410_x","()Lnet/minecraft/client/Minecraft;");
  if(!SDK::ids.mc_getMinecraft) SDK::ids.mc_getMinecraft=e->GetStaticMethodID(mc,"getMinecraft","()Lnet/minecraft/client/Minecraft;");
  SDK::ids.mc_theWorld=e->GetFieldID(mc,"field_71441_e","Lnet/minecraft/client/multiplayer/WorldClient;");
  SDK::ids.mc_thePlayer=e->GetFieldID(mc,"field_71439_g","Lnet/minecraft/entity/player/EntityPlayerSP;");
  if(!SDK::ids.mc_theWorld){ SDK::ids.mc_theWorld=e->GetFieldID(mc,"theWorld","Lnet/minecraft/client/multiplayer/WorldClient;"); }
  if(!SDK::ids.mc_thePlayer){ SDK::ids.mc_thePlayer=e->GetFieldID(mc,"thePlayer","Lnet/minecraft/entity/player/EntityPlayerSP;"); }
  jclass ec=e->FindClass(SDK::ENTITY);
  SDK::ids.entity_posX=e->GetFieldID(ec,"field_70165_t","D"); SDK::ids.entity_posY=e->GetFieldID(ec,"field_70163_u","D");
  SDK::ids.entity_posZ=e->GetFieldID(ec,"field_70161_v","D"); SDK::ids.entity_width=e->GetFieldID(ec,"field_70130_N","F");
  SDK::ids.entity_height=e->GetFieldID(ec,"field_70131_O","F"); SDK::ids.entity_isDead=e->GetFieldID(ec,"field_70128_L","Z");
  jclass lb=e->FindClass(SDK::ENTITYLIVING);
  SDK::ids.entity_hurtTime=e->GetFieldID(lb,"field_70737_aN","I"); if(!SDK::ids.entity_hurtTime) SDK::ids.entity_hurtTime=e->GetFieldID(lb,"hurtTime","I");
  jclass pc=e->FindClass("net/minecraft/entity/player/EntityPlayerSP");
  SDK::ids.player_onGround=e->GetFieldID(pc,"field_70122_E","Z"); if(!SDK::ids.player_onGround) SDK::ids.player_onGround=e->GetFieldID(ec,"onGround","Z");
}

BOOL WINAPI hkSwap(HDC hdc){
  Aim::Tick(); Combat::Tick(); Bridge::Tick(); Clicker::Tick();
  Visuals::OnSwapBuffers(hdc);
  // RShift menu toggle (ImGui overlay in full build, console fallback here)
  static bool was=false;
  bool down=(GetAsyncKeyState(VK_RSHIFT)&0x8000)!=0;
  if(down&&!was) Ghost::menuOpen=!Ghost::menuOpen;
  was=down;
  return oSwap(hdc);
}

static DWORD WINAPI MainThread(LPVOID){
  for(int i=0;i<90 && !Loader::FindBestJVM();i++){ Sleep(1000); }
  Sleep(8000); // Lunar/Badlion takes longer: wait for world + ClassLoader
  ResolveIDs();
  Config::ApplyProfile(10);
  HMODULE opengl=GetModuleHandleA("opengl32.dll");
  void* target=GetProcAddress(opengl,"wglSwapBuffers");
  UD::origSwap=(UD::tSwap)target;
  UD::ArmVEH(target); // no IAT patch, no inline bytes - HWBP only
  oSwap=(tSwap)target;
  while(running) Sleep(100);
  return 0;
}

BOOL APIENTRY DllMain(HMODULE m,DWORD r,LPVOID){
  if(r==DLL_PROCESS_ATTACH){ UD::Init(m); hMod=m; CreateThread(0,0,MainThread,0,0,0); }
  if(r==DLL_PROCESS_DETACH) running=false;
  return TRUE;
}
