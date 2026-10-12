#pragma once
#include "jni_sdk.h"
#include <vector>

// Lunar/Badlion attach: multi-VM scan + TCCL loadClass fallback
// Lunar 1.8.9 = same obf (field_71441_e etc) but different ClassLoader + process name
// Badlion 1.8.9 = same obf, embedded JRE, extra BL anticheat thread (ghost-safe, no packets)

namespace Loader {
  inline const wchar_t* kTargets[] = {
    L"javaw.exe", L"java.exe",
    L"LunarClient.exe", L"lunarclient.exe",
    L"Badlion Client.exe", L"BadlionClient.exe", L"badlion.exe"
  };

  inline bool FindBestJVM() {
    JavaVM* vms[8]; jsize n = 0;
    if (JNI_GetCreatedJavaVMs(vms, 8, &n) != JNI_OK || n == 0) return false;
    // pick VM where Minecraft class resolves via FindClass or TCCL
    for (int i = 0; i < n; i++) {
      JNIEnv* e = nullptr;
      vms[i]->GetEnv((void**)&e, JNI_VERSION_1_6);
      if (!e) vms[i]->AttachCurrentThread((void**)&e, 0);
      if (!e) continue;
      jclass mc = e->FindClass(SDK::MC);
      if (mc) { SDK::jvm = vms[i]; return true; }
      e->ExceptionClear();
      // TCCL fallback: Thread.currentThread().getContextClassLoader().loadClass()
      jclass threadC = e->FindClass("java/lang/Thread");
      if (!threadC) { e->ExceptionClear(); continue; }
      jmethodID curT = e->GetStaticMethodID(threadC, "currentThread", "()Ljava/lang/Thread;");
      jmethodID getCL = e->GetMethodID(threadC, "getContextClassLoader", "()Ljava/lang/ClassLoader;");
      jobject th = e->CallStaticObjectMethod(threadC, curT);
      jobject cl = e->CallObjectMethod(th, getCL);
      jclass clC = e->GetObjectClass(cl);
      jmethodID load = e->GetMethodID(clC, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
      jstring name = e->NewStringUTF(SDK::MC);
      jobject mc2 = e->CallObjectMethod(cl, load, name);
      if (!e->ExceptionCheck() && mc2) { SDK::jvm = vms[i]; return true; }
      e->ExceptionClear();
    }
    SDK::jvm = vms[0];
    return true;
  }
}
