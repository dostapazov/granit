// kertl.dll
// Вспомогательные функции
// Остапенко Д.В. ASDU 15 March 2000 Today Wages

#include <kertl.hpp>
#include <string.h>

#ifndef _NO_NAMESPACE
namespace KeRTL
{
#endif

 KERTL_FUNC DWORD __fastcall GetKeRTLVersion()
 {
  return  KERTL_VERSION;
 }

  HINSTANCE  __fastcall GetPtrModuleHandle(LPVOID ptr)
  {
      MEMORY_BASIC_INFORMATION bi;
      bi.AllocationBase = 0;
      if(VirtualQuery(ptr,&bi,sizeof(bi))==sizeof(bi))
         return HINSTANCE(bi.AllocationBase);

         return 0;

  }

  bool    __fastcall GetPtrModuleName(LPVOID  ptr,char * dest,int ccMax)
  {
	  return  GetModuleFileNameA(GetPtrModuleHandle(ptr),dest,ccMax) ? true:false;
  }

  bool    __fastcall GetPtrModuleName(LPVOID  ptr,wchar_t * dest,int ccMax)
  {
	  return  GetModuleFileNameW(GetPtrModuleHandle(ptr),dest,ccMax) ? true:false;
  }

  char *     __fastcall GetFileNameFromPath(LPCSTR  FullPath)
  {
   int len(FullPath ? lstrlen(FullPath) : 0);
   char * ptr = (char*)FullPath+len;
   while(ptr>FullPath)
   {
     if(*ptr =='\\')
         {ptr++;break;}
         else
         ptr--;
   }
   return ptr;
  }

  wchar_t *  __fastcall GetFileNameFromPath(LPCWSTR  FullPath)
  {
   int len(FullPath ? lstrlenW(FullPath) : 0);
   wchar_t * ptr = (wchar_t*)FullPath+len;
   while(ptr>FullPath)
   {
     if(*ptr =='\\')
         {ptr++;break;}
         else
         ptr--;
   }
   return ptr;
  }


 int        __fastcall Unicode2Ansi(LPSTR out,LPCWSTR in)
 {
   int i =lstrlenW(in);
   WideCharToMultiByte(CP_ACP,WC_COMPOSITECHECK,in,i ,out,i,0,0);
   *(out+i)=0;
   return i;
 }

 int        __fastcall Ansi2Unicode(LPWSTR out,LPCSTR in)
 {
   int i =lstrlenA(in);
  MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,in,i,out,i);
  *(out+i)=0;
  return i;
 }

 KERTL_FUNC int     __fastcall Unicode2Ansi(LPSTR  out,int sz, LPCWSTR in)
 {
   int i =lstrlenW(in);
   i = WideCharToMultiByte(CP_ACP,WC_COMPOSITECHECK,in,i ,out,sz,0,0);
   *(out+i)=0;
   return i;
 }

 KERTL_FUNC int     __fastcall Ansi2Unicode(LPWSTR out,int sz, LPCSTR  in)
 {
   int i =lstrlenA(in);
  i = MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,in,i,out,sz);
  *(out+i)=0;
  return i;

 }

 KERTL_FUNC __int64    __fastcall GetTime(bool System )
 {
   SYSTEMTIME stime;
   FILETIME   ftime;
   LARGE_INTEGER li;
   if(System)
     GetSystemTime(&stime);
      else
       GetLocalTime(&stime);
    SystemTimeToFileTime(&stime,&ftime);
    li.LowPart = ftime.dwLowDateTime;
    li.HighPart= ftime.dwHighDateTime;
    return li.QuadPart;

 }

 KERTL_FUNC int        __fastcall GetErrorMessage(DWORD Error,char * buffer,int ccMax,HINSTANCE fromModule )
 {
   return FormatMessageA(fromModule ? FORMAT_MESSAGE_FROM_HMODULE:FORMAT_MESSAGE_FROM_SYSTEM,
                        fromModule,
                        Error,
                        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                        buffer,
                        ccMax,
                        0);

 }

KERTL_FUNC int        __fastcall GetErrorMessage(DWORD Error,char * buffer,int ccMax,char * module )
{
 return GetErrorMessage(Error ,buffer,ccMax, (module && lstrlenA(module)) ? GetModuleHandle(module):0 );
}

KERTL_FUNC int        __fastcall GetErrorMessage(DWORD Error,wchar_t * buffer,int ccMax,HINSTANCE fromModule )
{
   return FormatMessageW(fromModule ? FORMAT_MESSAGE_FROM_HMODULE:FORMAT_MESSAGE_FROM_SYSTEM,
                        fromModule,
                        Error,
                        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                        buffer,
                        ccMax,
                        0);
}


KERTL_FUNC int  __fastcall ShowErrorMessage(HWND parent,DWORD ErrCode,int Flags,wchar_t * caption,HINSTANCE from_mod)
{
      //KeRTL.hpp
      wchar_t buf[MAX_PATH<<1];
      KeRTL::GetErrorMessage(ErrCode,buf,KERTL_ARRAY_COUNT(buf),from_mod);
      return MessageBoxW(parent,buf,(caption && *caption) ? caption:L"Error",Flags);
}


KERTL_FUNC BOOL __fastcall EnableAnimation(BOOL e)
{
 ANIMATIONINFO ai;
 bool prev;
 ZeroMemory(&ai,sizeof(ai));
 ai.cbSize = sizeof(ai);
 SystemParametersInfo(SPI_GETANIMATION,ai.cbSize,&ai,0);
 prev = ai.iMinAnimate ? true:false;
 ai.iMinAnimate = e;
 SystemParametersInfo(SPI_SETANIMATION, ai.cbSize, &ai, SPIF_UPDATEINIFILE );
 return prev;
}

#ifndef _NO_NAMESPACE
}// end of namespace KeRTL
#endif
