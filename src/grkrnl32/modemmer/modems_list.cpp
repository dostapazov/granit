/*
  Реализация modems_list - список модемов
             modem_ref   - ссылка на модем, интерфейс с модемом
             modem_line_ref - ссылка на линию связи модемя 

*/

#pragma hdrstop
#include "modemmer_main.hpp"
#include <otd_proto.h>

void         __fastcall modems_list::stop()
{
 lock();
 iterator beg   = this->begin();
 iterator end   = this->end();
 while(beg!=end)
     {
      modem_ref * mr = beg->second;
      mr->stop();
      beg++;
     }
 unlock();
}

void         __fastcall modems_list::release()
{
 lock();
 iterator beg   = this->begin();
 iterator end   = this->end();
 while(beg!=end)
     {
      modem_ref * mr = beg->second;
      mr->stop();
      delete mr;
      beg++;
     }
 clear();
 unlock();
}

modem_ref *  __fastcall modems_list::get_modem_ref(DWORD number)
{
 modem_ref * ret = NULL;
 TLockHelper l(locker);
 if(count(number))
    ret = (*this)[number];
    else
     SetLastError(MDM_ERR_NO_MODEM);
 return ret;
}

modem_ref *  __fastcall modems_list::get_modem_ref_byidx(DWORD idx)
{
 modem_ref * ret = NULL;
 TLockHelper l(locker);
 if(idx<size())
    {
     iterator i = begin();
     advance(i,idx);
     ret = i->second;
    }
 return ret;
}


bool  __fastcall modems_list::add(DWORD number,modem_ref * ref)
{
 bool ret = false;
 TLockHelper l(locker);
 if(count(number)==0 || (*this)[number]==NULL)
    {
     (*this)[number] = ref;
    }
 return ret;
}

 modem_ref *  __fastcall modems_list::remove(DWORD number)
 {
  modem_ref * ret = NULL;
  TLockHelper l(locker);
  iterator i = find(number);
  if(i!= this->end() && i->first == number )
      {
       ret = i->second;
       erase(i);
       if(ret)
         ret->stop();
      }
  return ret;
 }

void  __fastcall modems_list::handle_line_state(LPMODEM_LINE_STATE mls)
{
 DWORD modem_number = (DWORD)mls->addr.modem;
 modem_ref * mref ;
 TLockHelper l(locker);
 if(count((DWORD)mls->addr.modem))
 {
 mref = (*this)[modem_number];
 if(mref)
    mref->post_line_state(mls->addr.addr,mls->line_state);
    else
    erase(modem_number);
 }
 else
 GetLastError();
}

LRESULT __fastcall modems_list::on_send_timeout(DWORD addr,DWORD id)
{
  modem_ref * mr = get_modem_ref((DWORD)modem_addr(addr).modem);
  if(mr)
    mr->post_timer(addr,id);
  return GKH_RET_ERROR;  
}

LRESULT __fastcall modems_list::send(LPMPROTO_HEADER mph,DWORD wait)
{
  /*Через модеммер*/
  LRESULT ret = GKH_RET_SUCCESS;
  DWORD   save_modem  = mph->addr_to.modem;
  DWORD   begin = mph->addr_to.modem;
  DWORD   end   = mph->addr_to.modem;

  if(mph->addr_to.modem == MODEM_ADDR_MAXVALUE)
     begin = 0,wait = 0;
     else

     end++;
  while(begin<end)
  {
   modem_ref * mr = get_modem_ref(begin);
   if(mr)
      {
       mph->addr_to.modem = begin;
       ret = mr->send(mph,wait);
      }
      else
      SetLastError(MDM_ERR_NO_MODEM);

    begin++;
  }
   mph->addr_to.modem = save_modem;
  return ret;
}

BOOL  __fastcall modems_list::get_modem_addr(DWORD idx,LPMODEM_ADDR addr)
{
 addr->addr = -1;
 addr->sta  =  0;
 BOOL ret = FALSE;
 lock();
 if(idx<this->size())
  {
   iterator ptr = begin();
   advance(ptr,idx);
   if(ptr->second)
      {
       addr->modem = ptr->first;
       ret = TRUE;
      }
  }
 unlock();
 return ret;
}







