
#ifdef __linux__
#include <lin_ke_defs.h>
#else
#include <kertl.hpp>
#endif

#ifndef _NO_NAMESCPACE
namespace KeRTL{
#endif

unsigned short __fastcall WordRemind(unsigned char Val,unsigned short Del,unsigned short  Remind)
{
 unsigned long Temp=Remind;
 DWORD _Del = DWORD(Del);
 Temp<<=8;
 Temp+=(unsigned long)(Val);
 Temp= Temp%(_Del);
 Remind= (unsigned short)Temp;
 return Remind;
}




WORD WINAPI CalcRemind(void * Buffer,unsigned short len,unsigned short Del)
{
  // Расчет остатка
  DWORD   ret = 0;
  if(Buffer && len)
  {
  LPBYTE ptr  = (LPBYTE)Buffer;
  LPBYTE eptr = ptr+len;
  do{
     ret |=(DWORD)((*ptr)&0xFF);
     ret%= (DWORD)Del;
     if(++ptr<eptr)
       ret = 0xFFFFFF00&(ret<<8);
    }while  (ptr<eptr);
  }
  return (unsigned short)ret;
}


WORD WINAPI  calc_kpk(void * Buffer,unsigned short len,unsigned short Del)
{
 unsigned short remind = CalcRemind(Buffer,len,Del);
 remind = WordRemind(0,Del,remind);
 remind = WordRemind(0,Del,remind);

  //_AX = (unsigned short)(Del-remind);
  unsigned short diff =  remind ?  (unsigned short)(Del-remind) : 0;
  diff = ((diff>>8)&0x00FF) | ((diff<<8)&0xFF00);
  return diff;
}

#ifndef _NO_NAMESCPACE
} // end of namespace
#endif

