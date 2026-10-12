#include "ghost.h"
#include <windows.h>
#include <random>

void Clicker::Tick(){
  if(!Ghost::clicker) return;
  static DWORD nextUp=0,nextDown=0;
  static std::mt19937 rng(GetTickCount());
  bool down = (GetAsyncKeyState(VK_LBUTTON)&0x8000)!=0;
  // only in-game, menu closed, window focused
  if(!down || Ghost::menuOpen) return;
  DWORD now=GetTickCount();
  if(now>=nextDown){
    // humanized CPS between min/max + 12ms jitter
    std::uniform_int_distribution<int> cps(Ghost::cpsMin,Ghost::cpsMax);
    std::uniform_int_distribution<int> jit(-12,12);
    int c=cps(rng);
    int delay=1000/max(1,c)+jit(rng);
    mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);
    nextDown=now+max(30,delay*2);
    nextUp=now+max(20,delay/2);
    // re-press handled by user still holding; extra click via down event
    if(now>=nextUp) mouse_event(MOUSEEVENTF_LEFTDOWN,0,0,0,0);
  }
}
