#ifdef ENTITYOBJECTS_DLL // ENTITYOBJECTS.dll source code will define this macro before including this header
#define ENTITYOBJECTS_API
#else
#define ENTITYOBJECTS_API __declspec( dllexport )
#endif