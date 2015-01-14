#ifdef XMLCONVERTER_DLL // XMLCONVERTER.dll source code will define this macro before including this header
#define XMLCONVERTER_API
#else
#define XMLCONVERTER_API __declspec( dllexport )
#endif