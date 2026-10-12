#include "ghost.h"
#include <windows.h>
#include <cstdio>

// Profiles like Vape: Legit / Semi / Closet / 10=BEST CoralMC
void Config::ApplyProfile(int p) {
  Ghost::profile = p;
  if (p == 10) { // BEST CoralMC Grim+staff safe
    Ghost::aimAssist=true; Ghost::aimFov=32; Ghost::aimSmooth=20; Ghost::aimRange=4.3f;
    Ghost::aimOnlyClick=true; Ghost::aimCheckBlock=true;
    Ghost::reach=true; Ghost::reachMin=3.08f; Ghost::reachMax=3.28f;
    Ghost::velocity=true; Ghost::velH=87; Ghost::velV=100; Ghost::velChance=85;
    Ghost::clicker=true; Ghost::cpsMin=9; Ghost::cpsMax=13; Ghost::breakBlocks=true;
    Ghost::rClicker=false;
    Ghost::safewalk=true; Ghost::sprintReset=true; Ghost::autoSprint=false;
    Ghost::noSlow=true; Ghost::sprint=true; Ghost::wTap=true;
    Ghost::espBox=true; Ghost::tracers=false; Ghost::nametags2D=true;
    Ghost::espMaxDist=80; Ghost::antiBot=true; Ghost::hitBoxes=false;
    return;
  }
  if (p == 0) { // legit CoralMC
    Ghost::aimFov=28; Ghost::aimSmooth=22; Ghost::aimRange=4.2f;
    Ghost::reachMin=3.05f; Ghost::reachMax=3.25f;
    Ghost::velH=88; Ghost::velV=100; Ghost::velChance=80;
    Ghost::cpsMin=8; Ghost::cpsMax=12;
    Ghost::espBox=true; Ghost::tracers=false; Ghost::hitBoxes=false;
  } else if (p == 1) { // semi
    Ghost::aimFov=45; Ghost::aimSmooth=16; Ghost::aimRange=4.5f;
    Ghost::reachMin=3.1f; Ghost::reachMax=3.45f;
    Ghost::velH=80; Ghost::velV=100; Ghost::velChance=100;
    Ghost::cpsMin=9; Ghost::cpsMax=14;
    Ghost::espBox=true; Ghost::tracers=true;
  } else { // closet
    Ghost::aimFov=60; Ghost::aimSmooth=12; Ghost::aimRange=5.0f;
    Ghost::reachMin=3.2f; Ghost::reachMax=3.6f;
    Ghost::velH=72; Ghost::velV=92; Ghost::velChance=100;
    Ghost::cpsMin=10; Ghost::cpsMax=15;
    Ghost::espBox=true; Ghost::tracers=true; Ghost::hitBoxes=true;
  }
}
void Config::Save(const wchar_t* path) {
  FILE* f=nullptr; _wfopen_s(&f,path,L"w");
  if(!f) return;
  fwprintf(f,L"profile=%d\naim=%d %f %f\nreach=%d %f %f\nvel=%d %f %f %d\nclick=%d %d %d\n",
    Ghost::profile,Ghost::aimAssist,Ghost::aimFov,Ghost::aimSmooth,
    Ghost::reach,Ghost::reachMin,Ghost::reachMax,
    Ghost::velocity,Ghost::velH,Ghost::velV,Ghost::velChance,
    Ghost::clicker,Ghost::cpsMin,Ghost::cpsMax);
  fclose(f);
}
void Config::Load(const wchar_t* path) {
  FILE* f=nullptr; _wfopen_s(&f,path,L"r");
  if(!f){ ApplyProfile(10); return; }
  int pr=0,am=0,rc=0,vl=0,cl=0;
  fwscanf_s(f,L"profile=%d\n", &pr);
  Config::ApplyProfile(pr);
  fclose(f);
}
