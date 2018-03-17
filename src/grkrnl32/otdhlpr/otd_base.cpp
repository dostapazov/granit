#ifdef __linux__
#include <gklib/otd_base.h>
#include <gklib/otd_errors.h>
#else
#include <otd_base.h>
#include <otd_errors.h>
#endif

#include <stdlib.h>

div_t WINAPI fast_divP2(DWORD value,DWORD denom)
{
  div_t ret ;
  ret.rem = value&(denom-1);
  if(value>=denom)
      ret.quot = value>>3;
     else
      ret.quot = 0;
  return ret;
}

DWORD WINAPI otd_calc_data_size(DWORD type,DWORD count)
{
 /*Вычисление размера данных определённого типа и кол-ва*/
 if(OTD_GET_TYPE(type) == OTD_BIT_TYPE)
   return OTD_DATA_CALC_DISCRETE_SIZE(type,count);
 return OTD_DATA_CALC_SIZE(type,count);
}

DWORD WINAPI _get_bit_value(LPBYTE bytes,DWORD idx)
{
 div_t dt = fast_divP2(idx,8);
 bytes+=dt.quot;
 DWORD ret = ((*bytes)&(0x80>>dt.rem)) ? 1:0;
 return ret;
}

void  WINAPI _set_bit_value(LPBYTE bytes,DWORD idx,DWORD value)
{
 div_t dt  = div(idx,8);
 bytes+=dt.quot;
 BYTE mask = 0x80>>dt.rem;
 if(value)
    (*bytes)|=mask;
    else
    (*bytes)&= ~mask;
}




#define GET_REAL_INDEX(n,i) (i-(n)->loN)
//Получение/Установка значения
int  WINAPI otd_get_value(lpotd_data array,DWORD number,LPVOID value,size_t sz)
{

  if(array && OTD_CHECK_NUMBER(&array->numbers,number) && value && sz)
  {
   DWORD real_idx = GET_REAL_INDEX(&array->numbers,number);
   if(array->otd_type == OTD_DISCRETE)
    {
     DWORD v = _get_bit_value  (array->data,real_idx);
     memcpy(value,&v,KeRTL::MIN(sz,sizeof(v)));
    }
    else
    {
      size_t native_size = (size_t)OTD_DATA_CALC_SIZE(array->otd_type,1);
      sz = KeRTL::MIN(sz,native_size);
      memcpy(value,array->data+(native_size*real_idx),sz);
    }
      return 0;
  }
  return -1;
}

int  WINAPI otd_set_value(lpotd_data array,DWORD number,LPVOID value,size_t sz)
{
   int ret = -1;
   if(array && OTD_CHECK_NUMBER(&array->numbers,number) && sz)
   {
    ret=0;
    DWORD real_idx = GET_REAL_INDEX(&array->numbers,number);
    if(array->otd_type == OTD_DISCRETE)
       _set_bit_value  (array->data,real_idx,*LPBYTE(value));
    else
    {
      size_t native_size = (size_t)OTD_DATA_CALC_SIZE(array->otd_type,1);
      sz = KeRTL::MIN(sz,native_size);
      memcpy(array->data+(native_size*real_idx),value,sz);

    }
   }
   return ret;
}

int WINAPI otd_inc_value(lpotd_data array,DWORD number,BOOL no_zero)
{
 int ret = -1;
 if(array && OTD_CHECK_NUMBER(&array->numbers,number))
   {
     if(array->otd_type&OTD_BIT_TYPE)
     {
      bool v;
      otd_get_value(array,number,&v,sizeof(v));
      if(no_zero || !v)
         v=!v;
      ret = otd_set_value(array,number,&v,sizeof(v));
     }
     else
     {
      switch(array->otd_type)
      {
       case OTD_ANALOG_BYTE  :{LPBYTE  ptr  = (LPBYTE)array->data;ptr+=GET_REAL_INDEX(&array->numbers,number);  if(++(*ptr)== 0 && no_zero ) (*ptr)++; };break;
       case OTD_ANALOG_WORD  :{LPWORD  ptr  = (LPWORD)array->data;ptr+=GET_REAL_INDEX(&array->numbers,number);  if(++(*ptr)== 0 && no_zero ) (*ptr)++; };break;
       case OTD_ANALOG_DWORD :{LPDWORD ptr  = (LPDWORD)array->data;ptr+=GET_REAL_INDEX(&array->numbers,number); if(++(*ptr)== 0 && no_zero ) (*ptr)++; };break;
#ifdef _WINDOWS_
       case OTD_ANALOG_QWORD: {unsigned __int64 *  ptr  = (unsigned __int64 *)array->data;ptr+=GET_REAL_INDEX(&array->numbers,number); if(++(*ptr)== 0 && no_zero ) (*ptr)++; };break;
#else
       case OTD_ANALOG_QWORD: { __uint64 *  ptr  = ( __uint64 *)array->data;ptr+=GET_REAL_INDEX(&array->numbers,number); if(++(*ptr)== 0 && no_zero ) (*ptr)++; };break;
#endif
      }
      ret = 0;
     }
   }
 return ret;
}

int WINAPI otd_dec_value(lpotd_data array,DWORD number,BOOL no_zero)
{

 int ret = -1;
 if(array && OTD_CHECK_NUMBER(&array->numbers,number))
   {
     if(array->otd_type&OTD_BIT_TYPE)
     {
      bool v;
      otd_get_value(array,number,&v,sizeof(v));
      v=!v;
      ret = otd_set_value(array,number,&v,sizeof(v));
     }
     else
     {
      switch(array->otd_type)
      {
       case OTD_ANALOG_BYTE  :{LPBYTE  ptr  = (LPBYTE)array->data;ptr+=GET_REAL_INDEX(&array->numbers,number); if(--(*ptr)== 0 && no_zero ) (*ptr)--; };break;
       case OTD_ANALOG_WORD  :{LPWORD  ptr  = (LPWORD)array->data;ptr+=GET_REAL_INDEX(&array->numbers,number); if(--(*ptr)== 0 && no_zero ) (*ptr)--; };break;
       case OTD_ANALOG_DWORD :{LPDWORD ptr  = (LPDWORD)array->data;ptr+=GET_REAL_INDEX(&array->numbers,number); if(--(*ptr)== 0 && no_zero ) (*ptr)--; };break;
#ifdef _WINDOWS_
       case OTD_ANALOG_QWORD: {unsigned __int64 *  ptr  = (unsigned __int64 *)array->data;ptr+=GET_REAL_INDEX(&array->numbers,number); if(--(*ptr)== 0 && no_zero ) (*ptr)--; };break;
#else
       case OTD_ANALOG_QWORD: { __uint64 *  ptr  = ( __uint64 *)array->data;ptr+=GET_REAL_INDEX(&array->numbers,number); if(--(*ptr)== 0 && no_zero ) (*ptr)--; };break;
#endif
      }
      ret = 0;
     }
   }
 return ret;
}

int WINAPI otd_zero_value(lpotd_data array,DWORD number)
{
 __int64 v = 0;
 return otd_set_value(array,number,&v,sizeof(v));
}

int  WINAPI otd_copy     (lpotd_data dest,lpotd_data src,int start,int count)
{
 //Копирование из одного массива в другой
 int copy_count(0);
 if(dest && src)
 {
 count+=start;
 int src_sz = OTD_DATA_CALC_SIZE(src->otd_type,1);
 int dst_sz = OTD_DATA_CALC_SIZE(dest->otd_type,1);
 char buf[256];
 char * value;
 if(dst_sz>(int)sizeof(buf) || src_sz>(int)sizeof(buf))
   value = new char[KeRTL::MAX(dst_sz,src_sz)];
   else
   value = buf;

 for(int i = start;i<count;i++)
 {
   if(!otd_get_value(src,i,(LPVOID*)value,src_sz) && !otd_set_value(dest,i,value,OTD_DATA_CALC_SIZE(dest->otd_type,1)))
      copy_count++;

 }
 if(value && value!=buf)
   delete [] (char*) value;
 }

 return copy_count;
}


void WINAPI otd_clear    (lpotd_data data)
{
 __int64 v = 0;
 LPBYTE  value = (LPBYTE)&v;
 if(data)
 {
 DWORD val_sz = OTD_DATA_CALC_SIZE(data->otd_type,1);
 if(val_sz>sizeof(v))
 {
	 value = (LPBYTE)GlobalAlloc(GPTR,val_sz);
	 if(!value)
	   value = (LPBYTE)&v,val_sz= sizeof(v);

 }

 for(int i = data->numbers.loN;i<=data->numbers.hiN;i++)
     otd_set_value(data,i,LPVOID(value),val_sz);

 if(value && value!=(LPBYTE)&v)
	 GlobalFree(value);
 }
}

int  WINAPI otd_fill      (lpotd_data array ,LPVOID val,DWORD val_sz)
{
  if(array && val && val_sz)
  {
   int count = OTD_GET_COUNT(&array->numbers);
   for(int i = 0;i<count;i++)
      otd_set_value(array,i+array->numbers.loN,val,val_sz);
   return count;
  }
 return  0;
}

