#pragma hdrstop
#include <kertl.hpp>
#include <string>

#ifndef _NO_NAMESPACE
namespace KeRTL
{
#endif


KERTL_FUNC  char __fastcall digit2hexchar(DWORD dig)
{
  if(dig < 10)
     return (char)('0'+dig);
  return (char)'A'-10+dig;
}

KERTL_FUNC  DWORD __fastcall hexchar2digit(char ch)
{

  CharUpperBuffA(&ch,1);
  if(ch>='0' && ch<='9')
     return DWORD(ch-'0');
  ch&=~0x20;
  if(ch>='A' && ch<='F')
     return DWORD(ch-'A'+10);
  return -1;
}

KERTL_FUNC  DWORD __fastcall hexchar2digit(wchar_t wch)
{
  CharUpperBuffW(&wch,1);
  char ch = wch&0xFF;
  if(ch>='0' && ch<='9')
     return DWORD(ch-'0');
  ch&=~0x20;
  if(ch>='A' && ch<='F')
     return DWORD(ch-'A'+10);
  return -1;
}

bool is_hex_digit(char val)
{
  CharUpperBuffA((char*)&val,1);
  return ((val >='0' && val <='9') || (val>='A' && val<='F')) ? true:false;
}

bool is_hex_digit(wchar_t val)
{
 CharUpperBuffW((wchar_t*)&val,1);
  return ((val >='0' && val <='9') || (val>='A' && val<='F')) ? true:false;
}

KERTL_FUNC  DWORD __fastcall hextoi(const char * str)
{
 DWORD ret = 0;
 while(str && *str && is_hex_digit(*str))
 {
  ret<<=4;
  ret|=hexchar2digit(*str);
  str++;
 }
 return ret;
}

KERTL_FUNC  DWORD __fastcall hextoi(const wchar_t * str)
{
 DWORD ret = 0;
 while(str && *str && is_hex_digit(*str))
 {
  ret<<=4;
  ret|=hexchar2digit(*str);
  str++;
 }
 return ret;
}

union s_byte
{
 struct{
        BYTE lo4:4;
        BYTE hi4:4;
       };
 BYTE  bt;
};

 KERTL_FUNC DWORD      __fastcall octtoi(const wchar_t* ch)
 {
  DWORD ret = 0;
  while(ch && *ch)
  {
   ret<<=3;
   ret+=KeRTL::MIN((DWORD)7,(DWORD)(*ch)-'0');
   ch++;
  }
  return ret;
 }

 KERTL_FUNC DWORD      __fastcall octtoi(const char   * ch)
 {
  DWORD ret = 0;
  while(ch && *ch)
  {
   ret<<=3;
   ret+=KeRTL::MIN((DWORD)7,(DWORD)(*ch)-'0');
   ch++;
  }
  return ret;
 }



KERTL_FUNC  int __fastcall bytes2hexstr(LPBYTE ma,int ma_sz,char * outs,int out_sz,char delim )
{
 //ѕреобразование в строку
 int ret = 0;
 int need_out_sz = (ma_sz<<1) + ma_sz*int(delim!=0)+1;
 if(need_out_sz<=out_sz && outs)
 {
 s_byte * bt = (s_byte*)ma;
 for(int i = 0; i < ma_sz;i++,bt++)
 {
  *(outs++) = digit2hexchar(bt->hi4);
  *(outs++) = digit2hexchar(bt->lo4);
  ret+=2;
  if(delim)
    {*(outs++) = delim;ret++;}
 }
  if(delim)
     {--outs;ret--;}
 *(outs) = 0;
 }
 return ret;
}

template <typename T_CHAR>
T_CHAR * __fastcall _rtl_itoa_radix(int value,T_CHAR * out,int radix)
{
  if(out)
  {
   *out = 0;
   if(radix>1 && radix <65)
   {
   T_CHAR * p = out;
   T_CHAR   buff[64];
   T_CHAR * p_buf = buff;
   div_t dt;
   if(value<0)
    {
     *p++ = '-';
     dt.quot = -value;
     }
     else
      dt.quot = value;
    //«аполнение временного массива остатками от делени€ по основаню счислени€
    while(dt.quot)
    {
      dt = div(dt.quot,radix);
      *p_buf++ = (T_CHAR)dt.rem;
    }

    // опирование в обратном пор€дке с приведением к цифро-буквенному виду
    while(p_buf>buff)
    {
     value = *--p_buf;
     if(value<10)
        *p = (T_CHAR)value+(T_CHAR)'0';
        else
        {
        if(value<37)
        {
         if(value == 36)
           *p = (T_CHAR)'^';
            else
           *p = (T_CHAR)(value-10)+(T_CHAR)'A';
        }
        else
          {
            if(value == 63)
            *p = (T_CHAR)'~';
            else
            *p = (T_CHAR)(value-37)+(T_CHAR)'a';
          }
        }
       ++p;
    }
    *p = 0;
   }
  }
  return out;
}

KERTL_FUNC char * __fastcall itoa_radix(int value, char * out,int radix)
{
 return _rtl_itoa_radix(value,out,radix);
}

KERTL_FUNC wchar_t * __fastcall itoa_radix(int value, wchar_t * out,int radix)
{
 return _rtl_itoa_radix(value,out,radix);
}


/*ѕреобразование строки в число по основанию*/
template <typename T_CHAR>
int __fastcall _rtl_atoi_radix(T_CHAR * __str,int radix)
{
 //int len = __str ? (sizeof(__str[0]) == 1 ? strlen((char*)__str):wcslen((wchar_t*)__str)):0;
 int ret = 0;
 int len = 0;
 T_CHAR * in = __str;
 while(radix<=64 && ((*in>=T_CHAR('0') && *in<= T_CHAR('z')) || *in==T_CHAR('~')))
     ++in,++len;
 if(len)
 {


  int pow = 1;
  for(int i = 0;i<len;i++)
  {
   --in;
   int ch = radix>35? int(*in):toupper(*in);
   if(ch>=(T_CHAR)'0' && ch<=(T_CHAR)'9')
      ch -=(T_CHAR)'0';
   else
   {
   if(ch == (T_CHAR)'-')
     {ret = -ret;continue;}
   if(ch<(T_CHAR)'a')
   {
    if(ch == (T_CHAR)'^')
      ch = (T_CHAR)36;
    else
    {
     ch-=(T_CHAR)'A';
     ch+=(T_CHAR)10;
    }
   }
   else
   {
    if(ch == '~')
      ch = 63;
      else
      {
       ch-='a';
       ch+=37;
      }
   }

   }
   ret+= ch*pow;
   pow*=radix;
  }
 }
 return ret;
}

KERTL_FUNC int __fastcall atoi_radix(char * __str,int radix)
{
 return _rtl_atoi_radix(__str,radix);
}

KERTL_FUNC int __fastcall atoi_radix(wchar_t * __str,int radix)
{
 return _rtl_atoi_radix(__str,radix);
}




#ifndef _NO_NAMESPACE
}//End of namespace
#endif

