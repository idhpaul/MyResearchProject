#ifdef DLLEXPORT
#define MYDLLTYPE __declspec(dllexport)
#else
#define MYDLLTYPE __declspec(dllimport)
#endif

extern "C" MYDLLTYPE int AddInteger(int a, int b);
