// kertl.dll
// Вспомогательные функции
// Остапенко Д.В. Азов 5 02 2007 Today Wages

#include <kertl.hpp>
#include <string.h>

#ifndef _NO_NAMESPACE
namespace KeRTL
{
#endif

KERTL_FUNC void __fastcall secunds2HMS(DWORD secs, char * text,int tsz)
{
 if(tsz>=10)
 {
  div_t dt = div(secs,60);
  int sec  = dt.rem;
  dt       = div(dt.quot,60);
  wsprintf(text,"%02d:%02d:%02d",dt.quot,dt.rem,sec);
 }
} 

KERTL_FUNC void __fastcall secunds2HMS(DWORD secs, wchar_t * text,int tsz)
{
 if(tsz>=10)
 {
  div_t dt = div(secs,60);
  int sec  = dt.rem;
  dt       = div(dt.quot,60);
  wsprintfW(text,L"%02d:%02d:%02d",dt.quot,dt.rem,sec);
 }
}

int  __fastcall str2addr(char * str,LPBYTE addr,DWORD addr_sz,int delim)
{
 int ret = 0;
 if(str && addr && addr_sz)
 {
   memset(addr,-1,addr_sz);
     while(str && *str && addr_sz-- )
     {
       *addr = (BYTE)atoi(str);
       str = strchr(str,delim);
       if(str)str++;
       addr++;ret++;
     }
 }
 return ret;
}

int  __fastcall str2addr(wchar_t * str,LPBYTE addr,DWORD addr_sz,int delim)
{
 int ret = 0;
 if(str && addr && addr_sz)
 {
   memset(addr,-1,addr_sz);
     while(str && *str && addr_sz-- )
     {
       *addr = (BYTE)_wtoi(str);
       str = wcschr(str,delim);
       if(str)str++;
       addr++;ret++;
     }
 }
 return ret;
}



#ifndef _NO_NAMESPACE
}//End of namespace
#endif

