
#pragma comment (lib, "opengl32")

#define LUA_BUILD_AS_DLL
#ifdef _DEBUG
#pragma comment (lib, "SOILdebug")
#pragma comment(lib, "D:/bullet-2.82-r2704/build/lib/Debug/BulletDynamics_Debug")
#pragma comment(lib, "D:/bullet-2.82-r2704/build/lib/Debug/BulletCollision_Debug")
#pragma comment(lib, "D:/bullet-2.82-r2704/build/lib/Debug/LinearMath_Debug")
#pragma comment(lib, "D:/bullet-2.82-r2704/build/lib/Debug/BulletWorldImporter_Debug")
#pragma comment(lib, "D:/bullet-2.82-r2704/build/lib/Debug/BulletFileLoader_Debug")
#pragma comment(lib, "LuaDebug")
#else
#pragma comment (lib, "SOIL")
#pragma comment(lib, "D:/bullet-2.82-r2704/build/lib/Release/BulletDynamics")
#pragma comment(lib, "D:/bullet-2.82-r2704/build/lib/Release/BulletCollision")
#pragma comment(lib, "D:/bullet-2.82-r2704/build/lib/Release/LinearMath")
#pragma comment(lib, "D:/bullet-2.82-r2704/build/lib/Release/BulletWorldImporter")
#pragma comment(lib, "D:/bullet-2.82-r2704/build/lib/Release/BulletFileLoader")
#pragma comment(lib, "LuaRelease")
#endif

#pragma comment(lib, "glfw3dll")
