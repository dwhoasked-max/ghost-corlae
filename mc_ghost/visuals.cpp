#include "ghost.h"
#include "jni_sdk.h"
#include <GL/gl.h>
#include <cmath>

// w2s via GL MODELVIEW/PROJECTION/VIEWPORT + gluProject equiv
bool Visuals::WorldToScreen(double x, double y, double z, float* sx, float* sy) {
  GLdouble mv[16], pr[16]; GLint vp[4];
  glGetDoublev(GL_MODELVIEW_MATRIX, mv);
  glGetDoublev(GL_PROJECTION_MATRIX, pr);
  glGetIntegerv(GL_VIEWPORT, vp);
  GLdouble wx=x, wy=y, wz=z, ox=0, oy=0, oz=0;
  // manual project: clip = P * MV * v
  double in[4]={wx,wy,wz,1.0}, mvout[4]={0}, clip[4]={0};
  for(int i=0;i<4;i++) mvout[i]=mv[i]*in[0]+mv[4+i]*in[1]+mv[8+i]*in[2]+mv[12+i]*in[3];
  for(int i=0;i<4;i++) clip[i]=pr[i]*mvout[0]+pr[4+i]*mvout[1]+pr[8+i]*mvout[2]+pr[12+i]*mvout[3];
  if (clip[3]==0) return false;
  double ndcx=clip[0]/clip[3], ndcy=clip[1]/clip[3];
  *sx=(float)(vp[0]+(ndcx+1.0)*vp[2]/2.0);
  *sy=(float)(vp[1]+(1.0-ndcy)*vp[3]/2.0);
  return clip[3]>0.1;
}

static void GLBox(float x,float y,float w,float h,float* c){
  glColor4f(c[0],c[1],c[2],0.9f);
  glLineWidth(1.5f);
  glBegin(GL_LINE_LOOP);
  glVertex2f(x-w/2,y-h); glVertex2f(x+w/2,y-h);
  glVertex2f(x+w/2,y); glVertex2f(x-w/2,y);
  glEnd();
}

void Visuals::OnSwapBuffers(HDC hdc){
  (void)hdc;
  if(!Ghost::espBox && !Ghost::tracers) return;
  JNIEnv* e = SDK::env();
  if(!e) return;
  jclass mcC = e->FindClass(SDK::MC);
  jobject mc = e->CallStaticObjectMethod(mcC, SDK::ids.mc_getMinecraft);
  if(!mc) return;
  jobject world = e->GetObjectField(mc, SDK::ids.mc_theWorld);
  jobject self = e->GetObjectField(mc, SDK::ids.mc_thePlayer);
  if(!world || !self) return;

  // world.getLoadedEntityList()
  jclass worldC = e->GetObjectClass(world);
  jmethodID listMid = e->GetMethodID(worldC, "func_72872_a", "()Ljava/util/List;");
  if(!listMid) listMid = e->GetMethodID(worldC, "getLoadedEntityList", "()Ljava/util/List;");
  if(!listMid) return;
  jobject list = e->CallObjectMethod(world, listMid);
  jclass listC = e->GetObjectClass(list);
  jmethodID sizeM = e->GetMethodID(listC,"size","()I");
  jmethodID getM = e->GetMethodID(listC,"get","(I)Ljava/lang/Object;");
  jint n = e->CallIntMethod(list, sizeM);

  double sx = e->GetDoubleField(self, SDK::ids.entity_posX);
  double sy = e->GetDoubleField(self, SDK::ids.entity_posY);
  double sz = e->GetDoubleField(self, SDK::ids.entity_posZ);

  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glDisable(GL_TEXTURE_2D); glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
  GLint vp[4]; glGetIntegerv(GL_VIEWPORT, vp);
  glOrtho(vp[0],vp[2],vp[3],vp[1],-1,1);
  glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();

  for(int i=0;i<n;i++){
    jobject ent = e->CallObjectMethod(list, getM, i);
    if(ent==self || !ent) continue;
    // filter: EntityLivingBase + !isDead
    jboolean dead = e->GetBooleanField(ent, SDK::ids.entity_isDead);
    if(dead) continue;
    double ex = e->GetDoubleField(ent, SDK::ids.entity_posX);
    double ey = e->GetDoubleField(ent, SDK::ids.entity_posY);
    double ez = e->GetDoubleField(ent, SDK::ids.entity_posZ);
    double dx=ex-sx, dy=ey-sy, dz=ez-sz;
    double dist = sqrt(dx*dx+dy*dy+dz*dz);
    if(dist>Ghost::espMaxDist) continue;
    float h = e->GetFloatField(ent, SDK::ids.entity_height);
    float px, py, py2;
    if(!WorldToScreen(ex,ey,ez,&px,&py)) continue;
    // top of head for box height scale by dist
    float boxH = (float)(900.0/dist * (h>0?h:1.8));
    float boxW = boxH*0.5f;
    float col[3]={1.0f,0.25f,0.25f};
    if(Ghost::espBox) GLBox(px,py,boxW,boxH,col);
    if(Ghost::tracers){
      glColor4f(1,1,1,0.6f); glBegin(GL_LINES);
      glVertex2f(vp[2]/2.0f,(float)vp[3]); glVertex2f(px,py);
      glEnd();
    }
    e->DeleteLocalRef(ent);
  }
  glPopMatrix(); glMatrixMode(GL_PROJECTION); glPopMatrix();
  glPopAttrib();
}
