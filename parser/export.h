#ifdef PARSER_DLL // PARSER.dll source code will define this macro before including this header
#define PARSER_API
#else
#define PARSER_API __declspec( dllexport )
#endif