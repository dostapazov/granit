#ifdef __WIN32__
#include <modem_proto.h>
#endif

#ifdef __linux
#include <gklib/modem_proto.h>
#endif

//Читаем очередной MODEM_PROTO
//Если ничего нет, то возвращаем  0;
//Если размер буфера недостаточен

int __fastcall modem_proto_buffer::peek_mproto(LPBYTE buffer,DWORD sz,bool remove)
{

 int ret = 0;
 LPMPROTO_HEADER mph = (LPMPROTO_HEADER)begin();
 //Если длина в буфере достаточна чттобы прочесть поле длины
 if(this->len>= sizeof(MPROTO_HEADER)-sizeof(mph->data))
 {
  int mp_sz =(int) MPROTO_SIZE(mph);//mph->data_size+sizeof(MPROTO_HEADER)-sizeof(mph->data[0]);
  if(mp_sz>0)
  {
  if(mp_sz<=(int)len) // если пакт принят полностью - то копируем
  {
  if(buffer && (DWORD)mp_sz<=sz)
  {
   ret = mp_sz;
   memcpy(buffer,begin(),mp_sz);
   if(remove)
      move_to_begin(mp_sz,0); //удаление
  }
  else
   ret = -mp_sz;
  }
  else
  len = 0; // Кадр не принят ждем завершения
  }
  else
   ret = -1;
 }
 return ret;
}

  int   __fastcall modem_proto_buffer::get_mproto_len()
  {
   int ret = 0;
   LPMPROTO_HEADER mph = (LPMPROTO_HEADER)begin();
   //Если длина в буфере достаточна чттобы прочесть поле длины
   if(this->len>= sizeof(MPROTO_HEADER)-sizeof(mph->data))
   {
      int mp_sz =(int) MPROTO_SIZE(mph);//mph->data_size+sizeof(MPROTO_HEADER)-sizeof(mph->data[0]);
      if(mp_sz<=(int)this->get_data_len())
      {
       ret = mp_sz;
      }

   }
   return ret;
  }

  bool  __fastcall modem_proto_buffer::move_mproto()
  {
     int len = get_mproto_len();
     if(len)
      {
       this->move_to_begin(len);
      }
      return false;
  }


DWORD __fastcall modem_proto_buffer::sync_internal(DWORD int_val)
{
 LPBYTE ptr    = begin();
 DWORD  offset = 0;
 LPMPROTO_HEADER mph;
 while(len-offset>=sizeof(MPROTO_HEADER)-sizeof(mph->data))
 {
  mph  = (LPMPROTO_HEADER)ptr;
  if(mph->internal == int_val)
     break;
  ++offset;
  ptr++;
 }
 if(offset)
    move_to_begin(offset);
  return offset ;
}


