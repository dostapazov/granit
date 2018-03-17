//Различные определения
// Остапенко Д.В.  Май - 2000



#ifndef KE_DEFS_INC
#define KE_DEFS_INC
#ifndef RC_INVOKED


  #ifndef STRICT
  #define STRICT
  #endif

#if (__MINGW32__) ||  (__MINGW64__)
#include <stdint.h>

#endif

#ifdef __linux__

//Определяем необходимые типы данных
#include <mem.h>
#ifndef NULL
#define NULL 0
#endif



#define WINAPI    __stdcall
#define CALLBACK  WINAPI

typedef void* LPVOID ;

typedef unsigned char BYTE ;
typedef BYTE* LPBYTE;

typedef  unsigned short WORD ;
typedef  WORD* LPWORD;

typedef unsigned long DWORD ;
typedef DWORD* LPDWORD;

#define BOOL    DWORD

#define TRUE    1
#define FALSE   0

typedef long LONG;
typedef LONG* LPLONG;

typedef unsigned long  ULONG;
typedef ULONG* LPULONG;

//typedef  uint64_t QWORD;
typedef  long long QWORD;
typedef  QWORD* LPQWORD;

#define LRESULT LONG
#define LPARAM LONG

#define HANDLE    LPVOID
#define HINSTANCE LPVOID
#define HKEY      LPVOID
#define HICON     LPVOID
#define HWND      LPVOID

typedef  QWORD LARGE_INTEGER;
typedef  LARGE_INTEGER* LPLARGE_INTEGER;

typedef QWORD FILETIME ;
typedef FILETIME* LPFILETIME;

#define LANGID  WORD

#define LPSTR   char*
#define LPWSTR  wchar_t*
#define ANYSIZE_ARRAY 1

typedef struct  tagSIZE {
  LONG cx,cy;
}SIZE, *PSIZE, *LPSIZE;


#ifndef FillMemory
#define FillMemory(ptr,size,val) memset(ptr,val,size)
#endif

#ifndef ZeroMemory
#define ZeroMemory(ptr,size)     FillMemory(ptr,size,0)
#endif
#ifndef CopyMemory
#define CopyMemory(dst,src,size) memcpy(dst,src,size)
#endif

#define GlobalAlloc(flags,size)   malloc(size)
#define GlobalFree(ptr) free(ptr)

#define lstrlenW(s) wcslen(w)

 #if !defined(_SIZE_T) && !defined(_SIZE_T_DEFINED)
 #  define _SIZE_T_DEFINED
    typedef unsigned long long size_t
 #endif

#else

 #ifndef _WINDOWS_
  #ifndef __BORLANDC__
   #ifndef _WIN32_WINNT
     #define _WIN32_WINNT 0x0500 //NT 5
   #endif
  #endif
#endif

  #define __restrict
  #define bzero(x,y) ZeroMemory(x,y) 

#include  <Windows.h>
#include <tchar.h>
#endif

#ifndef DECLSPEC_NOVTABLE
#define DECLSPEC_NOVTABLE __declspec(novtable)
#endif



#ifndef winapi
#ifdef  WINAPI
#define winapi   WINAPI
#else
#define WINAPI __stdcall
#define winapi __stdcall
#endif
#endif

#ifndef EXPORT
#define EXPORT winapi
#endif


 //Делать ли ЭКСПОРТНЫЙ ВАРИАНТ
 #ifdef   KEWIN_DLL
  #ifndef KERTL_DLL
   #define KERTL_DLL
  #endif
 #endif

  //Делать ли ЭКСПОРТНЫЙ ВАРИАНТ  KeRtl
 #ifndef KERTL_DLL
    #define KERTL_CLASS    class
    #define KERTL_FUNC
  #else
    #define KERTL_CLASS    class __declspec(dllexport)
    #define KERTL_FUNC           __declspec(dllexport)
  #endif

  #define KEWIN_CLASS KERTL_CLASS
  #define KEWIN_FUNC  KERTL_FUNC


#ifndef UNICODE
#define WIDE_TEXT(arg) L##arg
#else
#define WIDE_TEXT(arg) TEXT(arg)
#endif


#define MSEC_NS100(x) (10000*(__int64)(x))
#define NS100_MSEC(x) ((x)/10000)

#define NS100_MKSEC(x) ((x)/10)
#define MKSEC_NS100(x) ((x)*10)

#ifdef __BORLANDC__
#ifndef _asm
#define _asm asm
#endif
#endif

typedef long long QWORD;
typedef QWORD* LPQWORD;
typedef QWORD* PQWORD;

#define NULL_PTR ((LPVOID)0)

 #define FLBASE_NODELETE    0x80000000  // не удалять
 #define FLBASE_RELEASED    0x40000000  // Release -отработала
 #define FLBASE_NEWALLOC    0x20000000  //  выделялась память

 #ifndef DWORD32
 #define DWORD32 DWORD
 #endif

#ifdef _DEBUG
    #define TRACE(x,y) \
    {\
     TCHAR dbg_text[2048];\
     wsprintf(dbg_text,x,y);\
     OutputDebugString(dbg_text);\
    }
#else
#define TRACE(x,y)
#endif

#define KERTL_ARRAY_COUNT(x) (sizeof(x)/sizeof(*x))
#define IS_VALID_HANDLE(h) (((HANDLE)(h)) && ((HANDLE)(h))!=(HANDLE)INVALID_HANDLE_VALUE)

#ifndef _NO_NAMESPACE
namespace KeRTL{
#endif

#ifdef __cplusplus

 template<class T>
 inline T  __fastcall MIN(T a,T b)
 {return (a < b )? a: b;};

 template<class T>
 inline T  __fastcall MAX(T one,T two)
 {return (one > two ) ? one:two;};

 template<class T>
 inline T  __fastcall ABS(T val)
 {return (val >= T(0) ) ? val: T(0)-val;};


template <typename T>
inline void __fastcall set_bit_flags( T & var,T flags, bool set)
{
  if(set)
    var|=flags;
    else
    var&=~flags;
}

template <typename T>
inline bool test_bit_flags(T var,T flags,bool all=false)
{
  if(all) return (var&flags) == flags ? true:false;
  return  (var&flags) ? true:false;
}


 //Базовый класс
 KERTL_CLASS  TKeRTLBase
 {
  protected:
    DWORD32 Flags;
    virtual void   __fastcall Release() =0;
             TKeRTLBase():Flags(0){}
    virtual ~TKeRTLBase(){};
  public:
    virtual void   __fastcall SetFlags(DWORD32 flags, bool Set);
            bool   __fastcall IsFlagsSet(DWORD32 test_flags,bool all = false);
    static  void   __fastcall FreeObject(TKeRTLBase & base);

    /*void    * operator new     (size_t size);
    void      operator  delete (void * base);*/


 };

   inline void __fastcall TKeRTLBase::FreeObject(TKeRTLBase & base)
    {
    if( NULL_PTR !=(&base) && !IsBadReadPtr(&base,sizeof(TKeRTLBase)) && !base.IsFlagsSet(FLBASE_NODELETE))
     {
      if(!base.IsFlagsSet(FLBASE_RELEASED))
        {base.Release();base.SetFlags(FLBASE_RELEASED,true);}
      /*if(base.IsFlagsSet(FLBASE_NEWALLOC))*/
         delete &base;
     }
    };

    inline void   __fastcall TKeRTLBase::SetFlags(DWORD32 flags, bool Set)
    {
      if(Set) Flags|=flags;
      else    Flags&=~flags;
    }

    inline bool   __fastcall TKeRTLBase::IsFlagsSet(DWORD32 test_flags,bool all)
    {
       return (all ? (((Flags&test_flags)==test_flags)? true:false ):((Flags&test_flags)? true:false) );
    }


#else

 #define min(a,b)\
         ((a<b) ? a : b)

 #define max(a,b)\
         ((a>b) ? a : b )
 #define ABS(a) ((a>=0) ? a:-a);        
#endif


#ifndef _NO_NAMESPACE
}
#endif
#endif //RC_INVOKED


#endif
