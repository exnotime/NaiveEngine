#ifdef _WIN32
#ifdef SOUND_DLL_EXPORT
#define SOUND_API __declspec(dllexport)
#else
#define SOUND_API __declspec(dllimport)
#endif
#else
#define SOUND_API
#endif