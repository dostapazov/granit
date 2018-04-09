
#ifdef __linux__
#include <lin_ke_defs.h>
#include <gklib/modbus_proto.h>
#include <gklib/otd_base.h>
#include <stdlib.h>
#else
#include <keRTL.hpp>
#include <windows.h>
#include <modbus_proto.h>
#include <otd_base.h>
#endif

#include <ctype.h>


unsigned int __fastcall hexchar2digit(char ch)
{

  ch = toupper(ch);
  if(ch>='0' && ch<='9')
     return (unsigned int)(ch-'0');
  ch&=~0x20;
  if(ch>='A' && ch<='F')
     return (unsigned int)(ch-'A'+10);
  return -1;
}

char __fastcall digit2hexchar(unsigned int dig)
{
  if(dig < 10)
     return (char)('0'+dig);
  return (char)'A'-10+dig;
}



unsigned short EXPORT modbus_calc_crc(unsigned char * buff,int blen)
{
  unsigned short crc = 0xFFFF;
  if(buff )
  {
    unsigned char * b_end  = buff + blen;
    while(buff<b_end)
    {
     unsigned short val = (unsigned short)(*buff);
     ++buff;
     crc = crc ^ val;
     for(int i = 0;i<8;i++)
     {
      if(crc&0x0001)
      {
       crc = (crc>>1)^(unsigned short)0xA001;
      }
      else
      crc = crc>>1;
     }
    }
  }
  return crc;
}

int EXPORT modbus_protect_crc        (void * data  ,int data_len,int buff_size,int * new_len)
{
 unsigned char  *p_data  = (unsigned char*)data;
 unsigned short *crc_ptr = (unsigned short *)(p_data+data_len);
 if(new_len)
    *new_len = data_len;

 if(data && data_len && buff_size>=int(data_len+sizeof(*crc_ptr)))
  {
  *crc_ptr = modbus_calc_crc(p_data,data_len);
  if(new_len)
     *new_len += sizeof(*crc_ptr);
  return TRUE;
 }
 else
 return FALSE;
}

int EXPORT modbus_check_crc     (void * data  ,int data_len)
{
     return data && data_len && 0 == modbus_calc_crc((unsigned char*)data,data_len) ? TRUE : FALSE;
}


inline unsigned char make_byte(unsigned char hi_4bits,unsigned char lo_4bits)
{
  return (unsigned char) (lo_4bits&0x0F)|((hi_4bits<<4)&0xF0);
}

int EXPORT modbus_ascii2bin(char * src,int src_len,unsigned char * dst,int dst_len,int hi_val_last)
{
 int res = 0;
 unsigned char lo_val ;
 unsigned char hi_val = 0;

 for (int i = 0;src && dst && i<src_len && res < dst_len;i++)
 {
   if(i&1)
      {
       lo_val =(unsigned char) hexchar2digit(*src);
       if(hi_val_last)
          *dst = make_byte(lo_val,hi_val);
        else
         *dst = make_byte(hi_val,lo_val);

       ++dst;
       ++res;
      }
      else
      hi_val =(unsigned char) hexchar2digit(*src);
   ++src;
 }

 return res;
}

inline unsigned char lo_4bits(unsigned char v)
{
 return v&0x0F;
}

inline unsigned char hi_4bits(unsigned char v)
{
 return (v>>4)&0x0F;
}


int EXPORT modbus_bin2ascii(unsigned char * src,int src_len,char * dst,int dst_len,int hi_val_last)
{
 //ѕреобразование в ascii формат
  int res = 0;

 if(src && src_len && dst &&  dst_len)
 {
  unsigned char * dst_ptr = (unsigned char *)dst;
  unsigned char lo_val,hi_val;
  for(int i = 0;i<src_len && res<dst_len;i++)
  {

   lo_val =  lo_4bits(*src);
   hi_val =  hi_4bits(*src);

   if(res<dst_len)
      {
       *dst_ptr = digit2hexchar(hi_val_last ? lo_val : hi_val );
       ++dst_ptr;
       ++res;
      }

   if(res<dst_len)
      {
       *dst_ptr = digit2hexchar(hi_val_last ? hi_val : lo_val);
       ++dst_ptr;
       ++res;
      }
   ++src;
  }
 }
 return res;
}

int EXPORT modbus_format_query(void * dst,int dst_len,int addr, int command,int v_start, int v_count,int * need_len)
{
  int need_sz = sizeof(MODBUS_FRAME)-1;
  need_sz += (sizeof(MODBUS_SHORT)<<1);
  if(need_len) *need_len = need_sz;
  if(dst && need_sz<=dst_len)
  {
   LPMODBUS_FRAME mf = (LPMODBUS_FRAME)dst;
   mf->addr = (unsigned char)addr;
   mf->data.command = (unsigned char) command;
   LPMODBUS_SHORT ms = (LPMODBUS_SHORT) mf->data.data;
   ms->val = modbus_word(v_start);
   ++ms;
   ms->val = modbus_word(v_count);
   return need_sz;
  }
  return  0;
}

int EXPORT modbus_format_preset     (void * dst,int dst_len,int addr,int command,int number,unsigned short value,int * need_len)
{
 int need_size = sizeof(MODBUS_FRAME)-1+(sizeof(unsigned short)<<1);
 if(need_len) *need_len = need_size;
 if(dst && need_size<= dst_len)
 {
   LPMODBUS_FRAME mf = (LPMODBUS_FRAME)dst;
   mf->addr = (unsigned char)addr;
   mf->data.command = (unsigned char) command;
   LPMODBUS_SHORT ms = (LPMODBUS_SHORT) mf->data.data;
   ms->val = modbus_word(number);
   ++ms;
   ms->val = modbus_word(value);
   return need_size;
 }
 return 0;
}

int EXPORT modbus_get_discrete(LPMODBUS_DATA  data,int number,int * state)
{
  int  len =  (int)data->data[0];
  div_t offs = fast_divP2(number,8);
  if(state )
  {
   if(offs.quot<len)
   {
    unsigned char * discrete_states = data->data+offs.quot+1;
    unsigned char mask = 0x01<<offs.rem;
     *state = ((*discrete_states) & mask) ? 1 : 0;
    return TRUE;
   }
   else
   *state = -1;
  }

  return FALSE;
}

int EXPORT modbus_set_discrete(LPMODBUS_DATA  data,int number,int  state)
{
   int  len =  (int)data->data[0];
   div_t offs = fast_divP2(number,8);
   if(offs.quot<len)
   {
    unsigned char * discrete_states = data->data+offs.quot+1;
    unsigned char mask = 0x01<<offs.rem;
    if(state)
      (*discrete_states) |=  mask;
      else
      (*discrete_states) &= ~mask;

    return TRUE;
   }
   return FALSE;
}


int EXPORT modbus_get_analog        (LPMODBUS_DATA data ,int number,int * astate)
{
 if(astate)
 {
   int len = (int)data->data[0];
   if(((number+1)<<1)<=len) //2 байта на параметр
   {
    LPMODBUS_SHORT ms = (LPMODBUS_SHORT)(data->data+1);
    ms+=number;
    *astate = modbus_get_short(ms);
    return TRUE;
   }
 }
   return FALSE;
}

int EXPORT modbus_set_analog        (LPMODBUS_DATA data ,int number,int  astate)
{

   int len = (int)data->data[0];
   if(((number+1)<<1)<=len) //2 байта на параметр
   {
    LPMODBUS_SHORT ms = (LPMODBUS_SHORT)(data->data+1);
    ms+=number;
    ms->val =  modbus_word(astate);
    return TRUE;
   }
   return FALSE;
}



int EXPORT modbus_format_respond    (void * dst,int dst_len,int addr,int command,int count,int bits,int * need_len)
{
 //‘ормирование ответа
  int need_sz  = sizeof(MODBUS_FRAME);
  div_t dt     = fast_divP2(count*bits,8);
  int data_len = dt.quot+(dt.rem ? 1 : 0);
  need_sz     += data_len;

  if(need_len) *need_len = need_sz;
  if(dst && need_sz<=dst_len)
  {
   LPMODBUS_FRAME mf = (LPMODBUS_FRAME)dst;
   mf->addr = (unsigned char)addr;
   mf->data.command = (unsigned char) command;
   mf->data.data[0] = data_len;
   return need_sz;
  }
  return  0;
}







