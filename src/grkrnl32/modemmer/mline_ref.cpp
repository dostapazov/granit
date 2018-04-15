/*
 Ссылка на линию модема
 Содержит  состояние, свойства линии и очередь на передачу
*/
#pragma hdrstop
#include "modemmer_main.hpp"
#include <stdio.h>

/*
  modem_line_ref - содержит очередь на передачу данных в линию
  Данные храняться в виде [_send_timeout][MPROTO_HEADER]
*/


 modem_line_ref::modem_line_ref(LPMODEM_LINE_PROPS prp)
 {
  TLockHelper l(locker);
  this->reserve(32);
  CopyMemory(&props,prp,sizeof(props));
  tx_kadr_number = 0;

 }

 bool  __fastcall modem_line_ref::can_send()
 {
  bool ret = false;
  if(props.line_flags & MLPF_TXABLE)
  {
   DWORD  mask = /*MODEM_LINE_STATE_CONNECT|*/MODEM_LINE_STATE_EXIST|MODEM_LINE_STATE_WORKED;
   ret =  (props.line_state&mask) == mask ? true:false;
  }
  return ret;
 }

 bool  __fastcall modem_line_ref::istx_ready()
 {
  return (props.line_state&MODEM_LINE_STATE_TXREADY) ? true:false;
 }


 struct _send_timeout
 {
  __int64 queued_time;//Время постановки в очередь
  __int64 timeout;    //Таймаут
 };

 __int64 __fastcall get_remind_time(_send_timeout * sto,__int64 curr_time)
 {
  __int64 time_elapsed = curr_time - sto->queued_time;
  __int64 remind       = sto->timeout - time_elapsed;
  return remind;  
 }

 int   __fastcall modem_line_ref::queue_data(LPMPROTO_HEADER mph,__int64 q_time,__int64 timeout_value)
 {
   int ret = -1;
   if(mph)
   {
    DWORD mp_size     = mproto_size(mph);
    LPSIMPLEDATA  sd = alloc_data_buffer(mp_size+sizeof(_send_timeout));
    if(sd)
    {
     LPBYTE      ptr = sd->data;
     _send_timeout * id  = (_send_timeout*)ptr;
     ptr+=sizeof(_send_timeout);
     id->queued_time    = q_time;
     id->timeout        = timeout_value;
     CopyMemory (ptr,mph,mp_size);
     TLockHelper l(this->locker);

     ret = size();
     if(!ret  ) //Без таймера отправки
         push(sd);
      else
      {
       push(sd);
       /*
       vector<LPSIMPLEDATA>::iterator beg = this->begin();
       vector<LPSIMPLEDATA>::iterator end = this->end();
       while(beg<end)
       {
        //Вставить в очередь перед тем , где таймаут еще терпит
        _send_timeout * sto = (_send_timeout*)(*beg)->data;
        if(get_remind_time(sto,q_time)>timeout_value)
           break;
        beg++;
       }
       insert(beg,sd);
       */
      }
      ret++;

    }
    else
    GetLastError();
   }
   return ret;
 }

 int   __fastcall modem_line_ref::get_data  (LPMPROTO_HEADER mph,DWORD sz)
 {
   int ret;
   Lock();
    ret = peek_data(mph,sz);
    if(ret>=0)
      DropFirst();
   Unlock();
   return ret;
 }

 int   __fastcall modem_line_ref::peek_data  (LPMPROTO_HEADER mph,DWORD sz)
 {
  int ret = -1;
  Lock();
  if(queue_count())
  {
    LPSIMPLEDATA sd = *this->begin();
    if(sd->size<=(int)sz)
    {
     ret = sd->size;
     CopyMemory(mph,sd->data+sizeof(_send_timeout),sd->size);
    }
  }
  Unlock();
  return ret; 
 }


 DWORD   __fastcall modem_line_ref::erase_timeouts(__int64 curr_time)
 {
   DWORD cnt = 0;
   Lock();
       vector<LPSIMPLEDATA>::iterator _beg = this->begin();
       vector<LPSIMPLEDATA>::iterator _end = this->end();
       while(_beg<_end)
       {
        LPSIMPLEDATA  sd = *_beg;
        _send_timeout *sto = (_send_timeout*)sd->data;
        if(get_remind_time(sto,curr_time)<0 )
          {
           erase(_beg);
           delete [](BYTE*)sd;
           cnt++;
           _end = this->end();
          }
        else  
        _beg++;
       }
   Unlock();
  return cnt; 
 }

 bool  __fastcall modem_line_ref::drop_data(DWORD timer_id)
 {
   bool ret = false;
   Lock();
       vector<LPSIMPLEDATA>::iterator beg = this->begin();
       vector<LPSIMPLEDATA>::iterator end = this->end();
       while(beg<end)
       {
        LPSIMPLEDATA  sd = *beg;
        _send_timeout *id = (_send_timeout*)sd->data;

        if(id->queued_time == timer_id)
          {
           delete [](char*)sd;
           erase(beg);
           ret = true;
          }
        beg++;
       }
   Unlock();
   return ret;
 }


 
