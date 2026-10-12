TARGET = Minecraft Java 1.8.9 (CoralMC), javaw.exe x64
TYPE = internal ghost DLL, JNI + OpenGL hook, ImGui menu (RShift)
FEATURES = visuals only, aim assist, bridging assist, autoclicker

BUILD VS2022 x64 Release C++17:
1. Include: jni.h (JDK8/17), MinHook, ImGui, OpenGL
2. Link: opengl32.lib minhook.lib jvm.lib
3. Output GhostCoral.dll
4. Inject with injector.cpp (LoadLibraryW into javaw.exe) AFTER world join
5. Menu: RSHIFT toggle, all default OFF for ghost legit look

FILES:
 dllmain.cpp:1 -> attach, JVM fetch, hook wglSwapBuffers
 jni_sdk.h:1 -> 1.8.9 field/method IDs
 visuals.cpp:1 -> ESP box/tracer/chams via GL
 aim.cpp:1 -> aim assist FOV+smooth
 bridge.cpp:1 -> safewalk/scaffold/sprint
 clicker.cpp:1 -> LMB autoclicker w/ randomization
