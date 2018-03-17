#pragma hdrstop
#include "modemmer_main.hpp"
#include <algorithm>
#include <functional>
#include <grkernel.h>

 char * TFaHandler::thread_name = "Modemmer::FA Handler Thread";

 TDfa::TDfa(TModemmer * _modemmer):modemmer(_modemmer),stop_event(true,false,MODEMMER_FAHANDLERS_TERMINATE_NAME_A),running(false)
 {
  ZeroMemory(handlers,sizeof(handlers));
  wchar_t kinstance[MAX_PATH];
  LRESULT ret = GKH_RET_ERROR;
  call_gkhandle_bynameW(GRKERNEL_HANDLE_NAME,CMGK_GET_INSTANCE,(LPARAM)kinstance,KERTL_ARRAY_COUNT(kinstance),&ret);
  snwprintf(stop_event_name,sizeof(stop_event_name),L"%s%s",kinstance,MODEMMER_FAHANDLERS_TERMINATE_NAME_W);
  stop_event.Close();
  stop_event = *&TEvent(true,false,stop_event_name);
 }

 void  __fastcall TDfa::start  ()
 {
  if(!running)
  {
   running = true;
   stop_event.SetEvent(false);
   for(int i = 0;i<MAX_FA_COUNT;i++)
   {
    locker.Lock();
    TFaHandler * fah = this->handlers[i];
    locker.Unlock();
    if(fah)
       fah->Start(0);
   }
  }
 }

 void  __fastcall TDfa::stop   ()
 {
  if(running)
  {
   running = false;
   stop_event.SetEvent(true);
   for(int i = 0;i<MAX_FA_COUNT;i++)
   {
    locker.Lock();
    TFaHandler * fah = this->handlers[i];
    locker.Unlock();
    if(fah)
       fah->TerminateAndWait(100,false);
   }
  }
 }

 void  __fastcall TDfa::release()
 {
  //Удаление всех созданных TFaHandler
  stop();
   for(int i = 0;i<MAX_FA_COUNT;i++)
   {
    locker.Lock();
    TFaHandler * fah = handlers[i];
    locker.Unlock();
    if(fah)
       delete fah,handlers[i] = NULL;

   }

 }

 bool  __fastcall TDfa::reg_handler(GKHANDLE gkh  ,DWORD fa)
 {
   /*Регистрация обработчика*/
   int ret = -1;
   if(fa<MAX_FA_COUNT)
   {
     locker.Lock();
     TFaHandler * fah = handlers[fa];
     locker.Unlock();
     if(fah)
     {
      ret = fah->reg_handler(gkh);
     }
      else
      {
        locker.Lock();
        handlers[fa] = fah = new TFaHandler(modemmer,stop_event_name, gkh);
        locker.Unlock();
        ret = 1;
      }

     if(this->running && ret>0 && !fah->IsRunning())
       fah->Start(0);
   }
   return ret >0 ? true:false;
 }

 bool  __fastcall TDfa::unreg_handler(GKHANDLE gkh,DWORD fa)
 {
   bool ret = false;
   if(fa<MAX_FA_COUNT)
   {
     locker.Lock();
     TFaHandler * fah = handlers[fa];
     locker.Unlock();
     if(fah)
        {
         if(running && fah->unreg_handler(gkh) == 0)
         {
          fah->Terminate();
          fah->push_data(fah,0);
         }
        }
     else
      SetLastError(MDM_ERR_NO_FA_HANDLER);
   }
   else
   SetLastError(MDM_ERR_INVALID_FA);
   return ret;
 }

 bool  __fastcall TDfa::handle(LPMPROTO_HEADER mp_hdr,DWORD sz)
 {
  bool ret = false;
  if(mp_hdr->fa <MAX_FA_COUNT)
    {
     locker.Lock();
     TFaHandler * fah = handlers[mp_hdr->fa];
     locker.Unlock();
     if(fah && fah->get_handlers_count())
        {

         fah->push_data(mp_hdr,sz);
         ret = true;
        }
     else
      SetLastError(MDM_ERR_NO_FA_HANDLER);
    }
    else
     SetLastError(MDM_ERR_INVALID_FA);
  return ret;
 }

 void    __fastcall TDfa::get_fa_text   (LPFA_HANDLER lpfah)
 {
  int len = snwprintf(lpfah->fa_text,KERTL_ARRAY_COUNT(lpfah->fa_text),L" %02X ",lpfah->fa);
  wchar_t * text = NULL;
  switch(lpfah->fa)
  {
   case FA_CONTROL                  : text = L"Управляющий протокол";break;
   case FA_OTD                      : text = L"ОТД-низкого  уровня";break;
   case FA_OTD_MEDIUM               : text = L"ОТД-среднего уровня";break;
   case FA_OTD_ALARM_ARCHIVE        : text = L"ОТД- аварийные архивы низкого  уровня";break;
   case FA_OTD_MEDIUM_ALARM_ARCHIVE : text = L"ОТД- аварийные архивы среднего уровня";break;
   case FA_SECURITY                 : text = L"Система безопасности и контроля прав" ;break;
  }
  if(text)
     wcsncpy(lpfah->fa_text+len,text,KERTL_ARRAY_COUNT(lpfah->fa_text)-len);
 }

 bool    __fastcall TDfa::enum_handlers(DWORD idx,LPFA_HANDLER lpfah)
 {
  bool   ret = false;
  get_fa_text(lpfah);
  locker.Lock();
  TFaHandler * fah = handlers[lpfah->fa];
  locker.Unlock();
  if(fah)
  {
    ret = fah->enum_handlers(idx,lpfah);
  }
  else
  SetLastError(MDM_ERR_NO_FA_HANDLER);
  return ret;
 }




 bool   __fastcall TFaHandler::BeforeExecute()
 {
  bool ret = TGKThread::BeforeExecute();
  ret = ret && terminate_event.IsValid() ? true:false;
  TerminateRequest = 0;
  pkt_num = 0;
  return ret;

 }

 int    __fastcall TFaHandler::Execute()
 {
  TSynchroSet ss;
  ss+=terminate_event();
  ss+=handle_event();
  wait_time = 2000;
  do{
     DWORD wr = ss.Wait(wait_time);
     switch(wr)
     {
      case  0 : InterlockedIncrement(&TerminateRequest); break;
      case  1 : do_handle();wait_time = 2000;break;
      default : if(data_queue.QueueCount())
                   {
                    do_handle();
                    wait_time = 100;
                   }
                   ;break;
     }
    }while(!TerminateRequest);
  return 0;
 }

 void   __fastcall TFaHandler::BeforeTerminate()
 {
   data_queue.Lock();
   this->data_queue.DropData();
   data_queue.Unlock();
   TGKThread::BeforeTerminate();
 }

 void __fastcall TFaHandler::do_handle()
 {
  //Собственно передача на обработку всем зарегестрированным обработчикам
  BYTE   _pre_buff[4096];
  LPBYTE buffer  = _pre_buff;
  int    buff_sz = sizeof(_pre_buff);
  while(!TerminateRequest && data_queue.QueueCount())
  {
    int sz = data_queue.TopDataSize();
    if(sz)
    {
    if(sz> buff_sz)
    {
     buff_sz = sz;
     if(buffer!=_pre_buff)
       delete [] buffer;
     buffer = new BYTE[buff_sz];
    }
     data_queue.GetFromQueue(buffer,buff_sz,&sz);
     LPMPROTO_HEADER mph = (LPMPROTO_HEADER)buffer;
    if(0 == ++pkt_num ) ++pkt_num;
     mph->pkt_num        = pkt_num;
     mproto_protect    (mph);
     call_handlers(buffer,sz);
     //modemmer->txrx_notify(mph,MNCODE_FA_HANDLER);
    }
    else
     TerminateRequest = true;
  }

  if(buffer && buffer!=_pre_buff)
    delete[] buffer;
 }

 void    __fastcall TFaHandler::call_handlers(LPBYTE data,DWORD len)
 {
     if(data && len)
     {
      GKHANDLE handlers[1024];
      int      h_count;
      reg_locker.Lock();
      h_count = std::min((int)KERTL_ARRAY_COUNT(handlers),(int)registered_handlers.size());
      std::copy(registered_handlers.begin(),registered_handlers.end(),handlers);
      reg_locker.Unlock();
      GKHANDLE *  ptr = handlers,*end = ptr+h_count;
      while(ptr<end)
      {
       if(call_gkhandle(*ptr,MDMCM_FA_HANDLE,(LPARAM)data,len))//Если обработчик вернул значение отличное от  GKH_RET_SUCCESS
          {
           unreg_handler(*ptr);
          }
       ptr++;
      }

     }
 }


// void    __fastcall TFaHandler::call_handlers(LPBYTE data,DWORD len)
// {
//     if(data && len)
//     {
//      reg_locker.Lock();
//      vector<GKHANDLE>::iterator ptr = this->registered_handlers.begin(),end = registered_handlers.end();
//      while(ptr<end)
//      {
//       if(call_gkhandle(*ptr,MDMCM_FA_HANDLE,(LPARAM)data,len))//Если обработчик вернул значение отличное от  GKH_RET_SUCCESS
//          registered_handlers.erase(ptr),end = registered_handlers.end();//Удаляем его из списка
//       else
//       ptr++;
//      }
//      reg_locker.Unlock();
//     }
// }




 int   __fastcall TFaHandler::reg_handler  ( GKHANDLE h  )
 {
   int ret;
   reg_locker.Lock();
   vector<GKHANDLE>::iterator ptr = registered_handlers.begin(),e = registered_handlers.end();
   ptr = std::lower_bound(ptr,e,h,gkhandle_less());
   if(!(*ptr) || !(ptr<e) || !gkhandle_equal()(*ptr,h))
     {
      registered_handlers.insert(ptr,h);
      ret = registered_handlers.size();
     }
     else
     SetLastError(MDM_WRN_HANDLER_EXIST);

   reg_locker.Unlock();
   return ret;
 }

 int   __fastcall TFaHandler::unreg_handler( GKHANDLE h  )
 {
   int ret = -1;
   reg_locker.Lock();
   vector<GKHANDLE>::iterator ptr = registered_handlers.begin(),e = registered_handlers.end();
   ptr = std::lower_bound(ptr,e,h,gkhandle_less());
   if(*ptr && ptr<e && gkhandle_equal()(*ptr,h))
     {
      registered_handlers.erase(ptr);
      ret = registered_handlers.size();
     }
   reg_locker.Unlock();
   return ret;
 }

int    __fastcall TFaHandler::get_handlers_count()
{
 int ret ;
 reg_locker.Lock();
 ret = (int)registered_handlers.size();
 reg_locker.Unlock();
 return ret;
}

 int   __fastcall TFaHandler::push_data(LPVOID data,DWORD sz)
 {
   int ret ;
   data_queue.Lock();
   if(sz)
     {
      data_queue.PutIntoQueue(data,sz);
      if(data_queue.QueueCount()==1)
        handle_event.SetEvent(true);
     }
   else
   handle_event.SetEvent(true);
   ret = data_queue.QueueCount();
   data_queue.Unlock();
   return ret;
 }

 bool   __fastcall TFaHandler::enum_handlers(DWORD idx,LPFA_HANDLER fah)
 {

  TLockHelper l(this->reg_locker);
  if(idx<registered_handlers.size())
    {
     fah->module = registered_handlers[idx];
     return true;
    }
    else
    SetLastError(MDM_ERR_NO_FA_HANDLER);
  return false;
 }




