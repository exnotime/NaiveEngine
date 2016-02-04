#ifdef _WIN32
#ifdef GFX_DX_DLL_EXPORT
#define GFX_DX_API __declspec(dllexport)
#else
#define GFX_DX_API __declspec(dllimport)
#endif
#else
#define GFX_DX_API
#endif