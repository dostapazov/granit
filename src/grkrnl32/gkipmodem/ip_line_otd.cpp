#pragma hdrstop
#include "otd_proto.h"
#include "gkipmodem_mod.hpp"
#include <stdio.h>

#define IPML_ALARM_RECV_ARCH_TIMEOUT  5
#define IPML_ALARM_RECV_LIST_TIMEOUT  1800


void  __fastcall   ip_line::otd_recv_prehandle(LPMPROTO_HEADER mph)
 {
            sotd_proto sop ((LPBYTE)mph->data,mph->data_size);
            //Только клиент подменяет адреса
            // В случае если ПУ = 0;
            if(!sop.op.kpk && otd_proto_check(&sop.op,0,OTD_DEF_POLINOM) )
            {
             //Ошибка КПК
             //Записать в ЛОГ
             return;
            }
            bool recalc_kpk = false;
            lpotd_addr addr = sop.op.addr;
            if( (client_options&IPM_CLOPT_OTD_REPLACE_PUNUMBER_ALWAYS) &&
                ( addr->pu == 0 || (client_options&IPM_CLOPT_OTD_REPLACE_PUNUMBER_ALWAYS ) == IPM_CLOPT_OTD_REPLACE_PUNUMBER_ALWAYS)
              )
             {
             addr->pu = owner->get_modem_number();
             if(addr->cp == 0xFF)
                addr->cp = line_number;
             recalc_kpk = true;

             }

            if(sop.op.kpk && recalc_kpk)
              otd_proto_protect(&sop.op,0,OTD_DEF_POLINOM);

            if(client_options&IPM_CLOPT_OTD_ALARM_ARCHIVE_SUPPORT )
            {
              if(!otd_alarms_active   )
              {
                 otd_alarms_active = TRUE;
                 alarms_list_request();
              }

            }

            if((mph->fa == FA_OTD_ALARM_ARCHIVE) )
             {
              alarms_archive_handle(mph,sop);
                 //return;
             }

            queue_rxdata(mph->fa,(LPBYTE)mph->data,mph->data_size,false);


 }

 bool __fastcall ip_line::alarms_archive_handle(LPMPROTO_HEADER /*mph*/,sotd_proto & sop)
 {
   //Возврат true чтобы не помещать в очередь приема
   bool   ret = false;

   if(sop.op.extension)
   {
     if(sop.op.extension->block_size==sizeof(QWORD))
     {
      LPQWORD arch_id = (LPQWORD)sop.op.extension->block_data;
      if(alarm_current != *arch_id  )
         ret = true;//Какой-то другой архив
         else
         alarm_timeout = IPML_ALARM_RECV_ARCH_TIMEOUT;
     }
     else
     {
      DWORD error =  otd_arch_proto_get_error(&sop.op);
      LPOTD_ARCH_HEADER ah = (LPOTD_ARCH_HEADER)sop.op.extension->block_data;
      alarm_handle_command(sop.op.addr ,ah,error);

      ret = true;
     }

    return ret;
   }

   return ret;
 }

int  __fastcall ip_line::alarm_print_time(wchar_t * text,int bsz,QWORD timestamp)
{
  SYSTEMTIME st;
  FileTimeToSystemTime((LPFILETIME)&timestamp,&st);
  return snwprintf(text,bsz,L" %hd-%02hd-%02hd %02hd:%02hd:%02hd.%03hd",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond,st.wMilliseconds);
}

void __fastcall ip_line::alarm_handle_command(lpotd_addr addr,LPOTD_ARCH_HEADER ah,DWORD error)
{
  wchar_t buf[1024];
  //bool   is_respond     = OTD_ARCH_CMD_IS_RESPOND(ah->cmd);
  bool   is_end         = OTD_ARCH_CMD_IS_END(ah->cmd);

  LPOTD_ARCH_LIST  al = (LPOTD_ARCH_LIST)ah->data;
  TLockHelper l(locker);
  bool is_new_arch = false;

  switch(OTD_ARCH_CMD(ah->cmd))
  {
   case OTD_ARCH_CMD_ALARM_ARCHIEVES_CREATED:
        is_end      = true;
        is_new_arch = true;
   case OTD_ARCH_CMD_ALARM_ARCHIEVES_LIST:
        alarm_add_archieves(is_new_arch,addr,al,is_end );
   break;
   case OTD_ARCH_CMD_ALARM_ARCHIEVES_GET_AND_ERASE:
   case OTD_ARCH_CMD_ALARM_ARCHIEVES_GET:
        this->alarm_arch_get(addr,error,ah,al,is_end);
    break;
  }
}

void  __fastcall ip_line::alarm_arch_get(lpotd_addr addr,DWORD error,LPOTD_ARCH_HEADER /*ah*/,LPOTD_ARCH_LIST al,bool is_end)
{
         alarm_timeout = IPML_ALARM_RECV_ARCH_TIMEOUT;
         if(is_end)
         {
         //Отправить уведомление о завершении приёма архива
         wchar_t buf[512];
            if(alarm_current == al->archive[0])
            {
               int modem_number = owner->get_modem_number();
               if(error)
               {
                int len = snwprintf(buf,KERTL_ARRAY_COUNT(buf),L"Модем %03d  линия %03d : ПУ %03d КП %03d Ошибка запроса архива %u "
                                                              ,modem_number,line_number
                                                              ,(int)addr->pu,(int)addr->cp
                                                              ,error);
                alarm_print_time(buf+len,KERTL_ARRAY_COUNT(buf)-len,alarm_current);
                ipm_arch_report(REPORT_ERROR_TYPE,buf);
                alarm_current = 0;
                alarms_list_request();

               }
               else
               {
               int len = snwprintf(buf,KERTL_ARRAY_COUNT(buf),L"Модем %03d  линия %03d : ПУ %03d КП %03d Прием архива аварии завершен "
                                                             , modem_number,line_number
                                                             ,(int)addr->pu,(int)addr->cp);
               alarm_print_time(buf+len,KERTL_ARRAY_COUNT(buf)-len,alarm_current);
               ipm_arch_report(REPORT_INFORMATION_TYPE,buf);
                 {
                  this->alarms_list.erase(alarms_list.begin());
                  alarm_current = 0;
                  alarm_start_download();
                 }
               }
             }
           alarm_start_download();
          }
          else
          {
           is_end = true;
          }

}


 void  __fastcall ip_line::alarm_add_archieves (bool new_arch,lpotd_addr addr,LPOTD_ARCH_LIST al,bool is_end )
 {
   LPQWORD a_ptr = (LPQWORD)al->archive;
   LPQWORD a_end = a_ptr+al->count;
   wchar_t buf[512];
   int modem_number = owner->get_modem_number();
   while (a_ptr<a_end)
   {
    alarms_list.push_back(*a_ptr);
    int len = snwprintf(buf,KERTL_ARRAY_COUNT(buf),L"Модем %03d  линия %03d : ПУ %03d КП %03d %s архив аварии "
                                                  , modem_number,line_number
                                                  ,(int)addr->pu,(int)addr->cp
                                                  ,new_arch ? L"новый" : L"существующий"
                                                  );
    alarm_print_time(buf+len,KERTL_ARRAY_COUNT(buf)-len,*a_ptr);
    ipm_arch_report(REPORT_INFORMATION_TYPE,buf,0,0);
    ++a_ptr;
   }
   if(is_end && !this->alarm_current)
   {
     //start down load archive
     alarm_start_download();
   }

 }

 void __fastcall ip_line::alarm_start_download()
 {
  if(this->alarms_list.size() && !alarm_current)
  {
   alarm_timeout = IPML_ALARM_RECV_ARCH_TIMEOUT;
   alarm_current = alarms_list.at (0);

   wchar_t buf[1024];
   LPMPROTO_HEADER mph = (LPMPROTO_HEADER)buf;
   DWORD sz = sizeof(buf)-sizeof(*mph)+sizeof(mph->data[0]);

   sotd_proto sop;
   DWORD cmd = (this->client_options & IPM_CLOPT_OTD_ALARM_ARCHIVE_ERASE) ? OTD_ARCH_CMD_ALARM_ARCHIEVES_GET_AND_ERASE : OTD_ARCH_CMD_ALARM_ARCHIEVES_GET;
   if(otd_arch_proto_format_list((LPBYTE)mph->data,sz,-1,-1,cmd,1,&alarm_current,0,&sz,&sop.op))
   {

    modem_addr ma;
    ma.line   = line_number;
    ma.modem  = owner->get_modem_number();
    mproto_init   (mph,0,FA_OTD_ALARM_ARCHIVE,sz);
    mproto_protect(mph);
    send   (mph,mproto_size(mph));
    int len = snwprintf(buf,KERTL_ARRAY_COUNT(buf),L"Модем %03d  линия %03d : запрос архива аварии отправлен",(int)ma.modem,(int)ma.line);
    alarm_print_time(buf+len,KERTL_ARRAY_COUNT(buf)-len,alarm_current);
    ipm_arch_report(REPORT_INFORMATION_TYPE,buf);
    alarm_timeout = IPML_ALARM_RECV_ARCH_TIMEOUT;
   }
  }
  else
  alarm_timeout = IPML_ALARM_RECV_LIST_TIMEOUT;
 }

 void  __fastcall ip_line::alarms_list_request  ()
 {
  // Отправка запроса на список аварийных архивов
       BYTE buf[1024];
       LPMPROTO_HEADER mph = (LPMPROTO_HEADER)buf;
       DWORD sz    = sizeof(buf)-sizeof(*mph)+sizeof(mph->data[0]);
       sotd_proto sop;
       if(otd_arch_proto_format((LPBYTE)mph->data,sz,-1,-1,OTD_ARCH_CMD_ALARM_ARCHIEVES_LIST,0,NULL,0,&sz,&sop.op))
       {
        MODEM_ADDR ma;
        GKIPM_LINE_ADDRS la;
        get_line_addrs(&la);
        ma.addr    = 0;
        ma.modem   = la.line_number;
        mproto_init(mph,0,FA_OTD_ALARM_ARCHIVE,sop.op.proto_size,&ma);
        mproto_protect(mph);
        this->send(mph,mproto_size(mph));
        //TRACE("Send alarm request");
        alarm_current = 0;
        alarms_list.clear();
       }
       alarm_timeout = IPML_ALARM_RECV_LIST_TIMEOUT;
       alarms_list.clear();
 }

 void __fastcall ip_line::alarm_timer()
 {
   if(alarm_timeout >0)
   {
     if(--alarm_timeout<=0)
     {
      alarms_list_request();
     }
   }
   else
   alarm_timeout = IPML_ALARM_RECV_LIST_TIMEOUT;

 }


