#ifdef _WIN32
#ifdef PHYSICS_DLL_EXPORT
#define PHYSICS_API __declspec(dllexport)
#else
#define PHYSICS_API __declspec(dllimport)
#endif
#else
#define PHYSICS_API
#endif