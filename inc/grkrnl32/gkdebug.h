#ifndef GK_DEBUG
#ifndef _WINDOWS_
 #include <windows.h>
#endif

#ifdef _DEBUG
#define GKD_TRACE(x) \
 OutputDebugString(x)
#else
#define GKD_TRACE(x)
#endif

#endif

