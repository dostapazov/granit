#pragma hdrstop
#include "gkipmodem_mod.hpp"
#include "otd_proto.h"
#include <stdio.h>

#define GKIPLINE_SECRET_DWORD  MP_INTERNAL_DEFAULT

ip_line::ip_line(DWORD number,DWORD cl_opts)
        :modem_line(number)
        ,terminate_event(true,false)
        ,timer (false)
        ,socket(NULL)
        ,network_event(NULL)
        ,connect_error_log(FALSE)
        ,client_options(cl_opts)
        ,otd_alarms_active(FALSE)
        ,alarm_current ( 0 )
{
 server_addr[0]    = 0;
 mpb.grow(IPLINE_BUFFSZ);
 last_pktnum = 0;
 sending     = FALSE;
}

ip_line::ip_line(DWORD number,char * _server_addr,DWORD _server_port,DWORD cl_opt)
        :modem_line(number)
        ,terminate_event(true,false)
        ,timer (false)
        ,socket(NULL)
        ,network_event(NULL)
        ,server_port(_server_port)
        ,connect_error_log(FALSE)
        ,client_options(cl_opt)
{
 strcpy(server_addr,_server_addr);
 last_pktnum       = 0;
 mpb.grow(IPLINE_BUFFSZ);
 sending     = FALSE;
}


 DWORD __fastcall ip_line::get_line_text  (wchar_t * text,DWORD text_sz)
 {
   wchar_t _text[MAX_PATH<<1];
   DWORD len;
    wchar_t _server_addr[MAX_PATH];
    _server_addr[0] = 0;
    if(server_addr[0])
    KeRTL::Ansi2Unicode(_server_addr,server_addr);
   if(is_connected())
   {
    wchar_t name[MAX_PATH<<1];
    len = 0;
    if(!server_addr[0])
     owner->print_rct(name,sizeof(name)/sizeof(*name),IDS_LINE_NO,get_number());
     //len  = swprintf(name,L"Линия №-%03d",get_number());
            swprintf(name+len,L"%s %d.%d.%d.%d : %d",_server_addr,
                                        (DWORD)host_addr.sin_addr.S_un.S_un_b.s_b1,
                                        (DWORD)host_addr.sin_addr.S_un.S_un_b.s_b2,
                                        (DWORD)host_addr.sin_addr.S_un.S_un_b.s_b3,
                                        (DWORD)host_addr.sin_addr.S_un.S_un_b.s_b4,
                                        (DWORD)htons(host_addr.sin_port));
    len = swprintf(_text,L"%s",name);
   }
   else
   {
    if(server_addr[0])
      len = owner->print_rct(_text,sizeof(_text)/sizeof(*_text),IDS_CONNECT_WITH,_server_addr);
    //len = swprintf(_text,L"Соединение с %s",_server_addr);
    else
      len = owner->print_rct(_text,sizeof(_text)/sizeof(*_text),IDS_FREE_LINE);
    //len = swprintf(_text,L"Свободая IP линия");
   }
   len = KeRTL::MIN(len+1,text_sz);
  if(text && len)
    lstrcpynW(text,_text,len);
    else
    len = 0;
  return len;  
 }

 bool  __fastcall ip_line::do_start  ()
 {
  bool ret = false;
  SetFlags(KERTL_THREAD_SELFDESTRUCT,(server_addr[0] == 0) ? true:false);
  //lock();
  if(server_addr[0] || socket)
    ret = Start();
  //unlock();
  return  ret;
 }

 bool  __fastcall ip_line::do_stop   ()
 {
  bool ret ;

  SetFlags(KERTL_THREAD_SELFDESTRUCT,false);
  ret = TerminateAndWait(3000,true);
  return  ret;
 }

 void  __fastcall ip_line::on_connect()
 {
    alarm_timeout = 10;
    mpb.move_to_begin(mpb.get_data_len(),0);
    if(!mpb.get_free_size())
        mpb.grow(IPLINE_BUFFSZ);
    last_pktnum = 0;
    pkt_num      = 0;
    sending      = FALSE;
    modem_line::on_connect();
    char rep_text  [MAX_PATH<<1];

    if(*server_addr)
      {
       owner->print_rct(rep_text,sizeof(rep_text),IDS_REP_CONNECT_ESTABLISH,owner->get_modem_number(),this->get_number(),server_addr);
      }
     else
     {
      //"%03d.%03d : Подключение клиента  %s"
      owner->print_rct(rep_text,sizeof(rep_text),IDS_REP_CLIENT_CONNECTED,owner->get_modem_number(),this->get_number(),inet_ntoa(host_addr.sin_addr));
      //wsprintf(rep_text,"%03d.%03d : Подключение клиента  %s",owner->get_modem_number(),this->get_number(),inet_ntoa(host_addr.sin_addr));
     }
    ipm_report(REPORT_INFORMATION_TYPE,rep_text,0,0);

 }

 void  __fastcall ip_line::on_disconnect()
 {
   modem_line::on_disconnect();
   send_queue.DropData();
   otd_alarms_active = FALSE;
   alarm_current = 0;
   alarms_list.clear();
   sending     = FALSE;
 }


 void  __fastcall ip_line::do_connect()
 {
  no_rx_timer = no_tx_timer = 0;
  otd_alarms_active = FALSE;
  alarm_timeout     = 10;
  if(socket)
     delete socket;
  if(network_event)
   {
    ss -=network_event;
    WSACloseEvent(network_event);
    network_event = NULL;
   }
  socket = new TIPSocket(true);
  if(socket)
  {
   set_state(MODEM_LINE_STATE_CONNECTING,true,true);
   //socket->SetSendBuffSize(IPLINE_BUFFSZ);
   socket->GetSendBuffSize();
   socket->SetRecvBuffSize(IPLINE_BUFFSZ);

   host_addr = *&TIPAddress(server_addr,server_port);
   if(host_addr.sin_addr.S_un.S_addr)
   {
    socket->EnableReuseAddress(true);
    socket->EnableKeepAlive(true);
    socket->GetSocketError();
    network_event = WSACreateEvent();
    ss+=network_event;
    WSAEventSelect(socket->GetSocket(),network_event,FD_READ|FD_CLOSE|FD_WRITE|FD_CONNECT);
    socket->WSAConnect(host_addr(),sizeof(host_addr),NULL,NULL,NULL,NULL);
   }
  }
 }

 bool  __fastcall ip_line::accept_connection(SOCKET s,TIPAddress & host)
 {
  bool ret ;
  host_addr = host;
  if(socket) { delete socket; socket = NULL;}
  socket = new TIPSocket(s);
  ret = socket ? true:false;
  if(ret)
  {
   socket->SetRecvBuffSize(IPLINE_BUFFSZ);
   socket->GetSendBuffSize();
   socket->SetSendBuffSize(IPLINE_BUFFSZ);

   socket->EnableReuseAddress(true);
   ret =   start();
  }
  connect(ret);
  return ret;
 }

 DWORD __fastcall ip_line::OnException(DWORD ExceptionCode,LPEXCEPTION_POINTERS exp)
 {
   send_queue.full_unlock();
   sending     = FALSE;
   return TGKThread::OnException(ExceptionCode,exp);
 }


 bool  __fastcall ip_line::BeforeExecute()
 {
  bool ret = TThread::BeforeExecute();
  send_queue.DropData();
  mpb.move_to_begin(mpb.get_data_len(),0);
  if(ret && (server_addr[0] || socket))
  {
   terminate_event.SetEvent(false);
   ss.Clear();
   ss+=terminate_event();
   ss+=send_queue.GetEvent();
   ss+=timer();

   if(server_addr[0])
   {
    Sleep(20);//Задержка из-за гонок в уведомлении modemmer'a об изменении состояния линии
    do_connect();
   }
   else
   {
    network_event = WSACreateEvent();
    ss+=network_event;
    ret = SOCKET_ERROR == WSAEventSelect(socket->GetSocket(),network_event,FD_WRITE|FD_READ|FD_CLOSE|FD_OOB|FD_CONNECT) ? false:true;
   }
  }
  no_rx_timer = 0;
  no_tx_timer = 0;
  this->locker.FullUnlock();
  timer.StartPeriodTimer(1000,true);
  set_state(MODEM_LINE_STATE_WORKED,true,true);
  return ret;
 }

 int   __fastcall ip_line::Execute()
 {
  DWORD wr;
  do{
      wr = WSAWaitForMultipleEvents(ss.GetWaitingCount(),ss(),FALSE,INFINITE,FALSE);
      switch(wr)
      {
       case 1:
               ResetEvent(ss[1]);
               send_processing();
               break;
       case 2: on_timer();
       break;
       case 3: handle_network_events();
       break;
      }
    }while(wr && !CheckTerminateRequest() );

  return 0;
 }

 void  __fastcall ip_line::BeforeTerminate()
 {
   timer.StopTimer();
   if(socket)
      {
       if(is_connected())
          {
           //if(!server_addr[0])
              WSASendDisconnect(socket->GetSocket(),NULL);
              connect(false);
           Sleep(100);
          }

      if(socket)
      {
       socket->Disconnect();
       socket->DestroySocket();
       delete socket;
       socket = NULL;
      }
    }
   if(network_event)
      WSACloseEvent(network_event);
   network_event = NULL;
   ss.Clear();
   set_state(MODEM_LINE_STATE_WORKED,false,true);

 }

 void  __fastcall ip_line::handle_network_events()
 {
   WSANETWORKEVENTS ne={0};

   while(!WSAEnumNetworkEvents(socket->GetSocket(),network_event,&ne) && ne.lNetworkEvents)
     {
      if(ne.lNetworkEvents&FD_CONNECT)
         {
          handle_connect(ne.iErrorCode[FD_CONNECT_BIT]);
          ne.lNetworkEvents&=~FD_CONNECT;
         }

      if(ne.lNetworkEvents&FD_READ)
        {
         handle_recv(ne.iErrorCode[FD_READ_BIT]);
         ne.lNetworkEvents&=~FD_READ;
        }

      if(ne.lNetworkEvents&FD_WRITE)
      {
       DWORD send_error = ne.iErrorCode[FD_WRITE_BIT];
       sending = false;
       ne.lNetworkEvents&=~FD_WRITE;
       no_tx_timer = 0;
       if(!send_error )
       {
        if(send_queue.QueueCount())
           SetEvent(ss[1]);
       }
       else
       {
         this->on_disconnect();
       }
      }

      if(ne.lNetworkEvents&FD_CLOSE)
      {
        handle_disconnect(ne.iErrorCode[FD_CLOSE_BIT]);
        ne.lNetworkEvents&=~FD_CLOSE;
      }


      if(ne.lNetworkEvents&FD_OOB)
        {
         handle_oob(ne.iErrorCode[FD_OOB_BIT]);
         ne.lNetworkEvents&=~FD_OOB;
        }
      if(ne.lNetworkEvents)
      {
       ne.lNetworkEvents = 0;
      }

     }
 }

 int   __fastcall ip_line::read_from_socket()
 {
  //Процедура чтения данных из гнезда
   DWORD rd_len = 0;
   DWORD total_read = 0;
   do
   {
    socket->IoctlSocket(FIONREAD,&rd_len);
    if(rd_len)
    {
     total_read+= rd_len;
     if(mpb.get_free_size()<rd_len)
        mpb.grow(IPLINE_BUFFSZ);
     char * b = mpb.end();
     rd_len = socket->Recv(b,rd_len,0);
     mpb.add(b,rd_len);
     total_read+=rd_len;
    }
    else
    rd_len = rd_len;

   }while(rd_len);
   return total_read;
 }

void    __fastcall ip_line::send_query_arch_list()
{
  if(is_connected())
  {

  }
}

void    __fastcall ip_line::set_client_options(DWORD opts)
{
  if(client_options!=opts)
  {
   int opts_up = opts&(client_options^opts);
   client_options = opts;
   if(opts_up)
   {
       if(opts_up&IPM_CLOPT_OTD_ALARM_ARCHIVE_SUPPORT)
          send_query_arch_list();
   }
  }

}


 void  __fastcall ip_line::handle_recv(DWORD err)
 {
   char rep_text [MAX_PATH<<1];

   if(err)
   {
    owner->print_rct(rep_text,sizeof(rep_text),IDS_REP_RECV_ERROR,owner->get_modem_number(),get_number(), err);
    //strcpy(rep_templ,"%03d.%03d Ошибка при приёме %d");
    //wsprintf(rep_text,rep_templ,owner->get_modem_number(),get_number(), err);
    ipm_report(REPORT_ERROR_TYPE,rep_text,server_addr);
    update_stats_rx(0,0,1);
   }
   else
   no_rx_timer = 0;

   DWORD total_read = read_from_socket();
   /*Вычитали всё из гнезда, теперь начинаем разбор полётов*/
   while(total_read)
   {
   update_stats_rx(total_read,0,0);
   DWORD rd_len;
   do{
      err = 0;
      DWORD sync_offs =  mpb.sync_internal(GKIPLINE_SECRET_DWORD);
      if(sync_offs)
       {
         //"%03d.%03d Ошибка контрольной суммы %d байт"
         owner->print_rct(rep_text,sizeof(rep_text),IDS_REP_SYNC_ERROR,owner->get_modem_number(),get_number(),sync_offs);
         ipm_report(REPORT_INFORMATION_TYPE,rep_text);
         update_stats_rx(0,0,1);
       }

      rd_len = mpb.get_mproto_len();
      if(rd_len)
      {

       LPMPROTO_HEADER mph = (LPMPROTO_HEADER)mpb.begin();
       if(mph->header_cs )
       {
        //DWORD cv = otd_calc_checksum(mph,sizeof(*mph)-sizeof(mph->data[0]),OTD_DEF_POLINOM);
        if(!mproto_check(mph))
        {
         owner->print_rct(rep_text,sizeof(rep_text),IDS_REP_CHECKSUM_ERROR,owner->get_modem_number(),get_number(),mph->header_cs);
         ipm_report(REPORT_ERROR_TYPE,rep_text,mph,mproto_size(mph));
         err ++;
        }
       }
       else
       {
        if(client_options & IPM_CLOPT_COMPATIBLE_MODE)
           mproto_protect(mph);
        else
        {
            owner->print_rct(rep_text,sizeof(rep_text),IDS_REP_ZERO_CHECKSUM_ERROR,owner->get_modem_number(),get_number());
            ipm_report(REPORT_ERROR_TYPE,rep_text,mph,mproto_size(mph));
            err++;
        }
       }

       update_stats_rx(0,1,err);

      if(!err)
      {
       if(last_pktnum)
         {
          if(mph->pkt_num!= 1 && last_pktnum!= (WORD)-1  &&  (abs((int)mph->pkt_num-(int)last_pktnum)>1) )
           {

              owner->print_rct(rep_text,sizeof(rep_text),IDS_REP_LOSTKADR_ERROR,owner->get_modem_number(),get_number(),(DWORD)last_pktnum+1,(DWORD)mph->pkt_num-1);
              ipm_report(REPORT_ERROR_TYPE,rep_text,0,0);
              update_stats_rx(0,0,1);
            }
         }

        last_pktnum = mph->pkt_num;

       if(mph->fa) // Исключить тестовую посылку проверки канала связи
         {
//          if((client_options&IPM_CLOPT_OTD_SUPPORT) && (mph->fa == FA_OTD || mph->fa == FA_OTD_ALARM_ARCHIVE))
//          {
//            otd_recv_prehandle(mph);
//          }
//          else
           queue_rxdata(mph->fa,(LPBYTE)mph->data,mph->data_size,false);
         }
       }
       mpb.move_mproto();
      }
     }while(rd_len);
     total_read = read_from_socket();
   }
 }

 #pragma warn -8057

 void  __fastcall ip_line::handle_oob(DWORD err)
 {
   BYTE rbuf[4096];
   while(socket->Recv(rbuf,sizeof(rbuf),MSG_OOB))
       ;
 }

 void  __fastcall ip_line::handle_disconnect(DWORD err)
 {
    char rep_text[MAX_PATH];
    owner->print_rct(rep_text,sizeof(rep_text),IDS_REP_DISCONNECT,owner->get_modem_number(),get_number(),inet_ntoa(host_addr.sin_addr),err);
    //sprintf(rep_text,"%03d.%03d  Хост %s разовал соединение %d",owner->get_modem_number(),get_number(),inet_ntoa(host_addr.sin_addr),err);

    ipm_report(REPORT_ERROR_TYPE,rep_text);
    connect(false);
 }

 #pragma warn .8057

 void  __fastcall ip_line::handle_connect(DWORD err)
 {
    if(err)
    {
     if(!connect_error_log)
     {
      char rep_text[MAX_PATH];
      owner->print_rct(rep_text,sizeof(rep_text),IDS_REP_CONNECT_ERROR,owner->get_modem_number(),get_number(),err,server_addr);
      //sprintf(rep_text,"Ошибка соединения %d c %s",err,server_addr);
      ipm_report(REPORT_ERROR_TYPE,rep_text);
      connect_error_log = TRUE;
     }
    }
    else
    connect_error_log = FALSE;
    connect(err ? false:true);
 }

void          __fastcall ip_line::set_line_addrs(LPGKIPM_LINE_ADDRS addrs)
{
  strncpy(this->server_addr,addrs->host_name,sizeof(server_addr));
  this->server_port =  addrs->host_port;
}

void          __fastcall ip_line::get_line_addrs(LPGKIPM_LINE_ADDRS addrs)
{
  addrs->line_number = this->get_number();
  if(server_addr[0])
  sprintf(addrs->host_name,"%s",server_addr);
  else
   owner->get_lang_string(IDS_CONNECTION_WAIT,addrs->host_name,sizeof(addrs->host_name));
  //sprintf(addrs->host_name,"Ожидание подключения",server_addr);
  addrs->host_port = this->server_port;
 if( socket )
 {
  ZeroMemory(&addrs->local_addr,sizeof(addrs->local_addr));
  if(is_connected())
  {
    socket->GetSocketName((sockaddr*)&addrs->local_addr);
    if(!server_addr[0])
    {
        sprintf(addrs->host_name,"%d.%d.%d.%d : %d",
        (DWORD)host_addr.sin_addr.S_un.S_un_b.s_b1,
        (DWORD)host_addr.sin_addr.S_un.S_un_b.s_b2,
        (DWORD)host_addr.sin_addr.S_un.S_un_b.s_b3,
        (DWORD)host_addr.sin_addr.S_un.S_un_b.s_b4,DWORD(htons(host_addr.sin_port))
        );
    }
  }

 }
}

 void     __fastcall ip_line::ipm_report(DWORD type,char * descr,LPVOID data ,DWORD data_sz )
 {
  TGkIPModem * own = dynamic_cast<TGkIPModem *>(get_owner());
  if(own)
    own->ipm_report(false,type,descr,data,data_sz);
 }

 void     __fastcall ip_line::ipm_report(DWORD type,wchar_t * descr,LPVOID data ,DWORD data_sz )
 {
  TGkIPModem * own = dynamic_cast<TGkIPModem *>(get_owner());
  if(own)
    own->ipm_report(false,type,descr,data,data_sz);
 }


 void     __fastcall ip_line::ipm_arch_report(DWORD type,char * descr,LPVOID data ,DWORD data_sz )
 {
  TGkIPModem * own = dynamic_cast<TGkIPModem *>(get_owner());
  if(own)
    own->ipm_report(true,type,descr,data,data_sz);
 }

 void     __fastcall ip_line::ipm_arch_report(DWORD type,wchar_t * descr,LPVOID data ,DWORD data_sz )
 {
  TGkIPModem * own = dynamic_cast<TGkIPModem *>(get_owner());
  if(own)
    own->ipm_report(true,type,descr,data,data_sz);
 }

/*
 Отправка данных
 Подготовка данных  

*/

 #pragma warn -8057

 bool  __fastcall ip_line::send        (LPMPROTO_HEADER mph,DWORD sz)
 {

   DWORD  mp_size = MPROTO_SIZE(mph);
   LPBYTE ptr     = (LPBYTE)mph;
   DWORD total_size = 0;
   DWORD total_kadrs = 0;

   while(mp_size)
   {
	DWORD put_sz = KeRTL::MIN((DWORD)IPLINE_BUFFSZ,mp_size);
	do_send(mph);
	ptr+=put_sz;
	mp_size-=put_sz;
	total_size+=put_sz;
	++total_kadrs;
   }

   update_modemmer_tx(total_size,total_kadrs);
   return true;
 }

 DWORD  __fastcall ip_line::send_data(LPBYTE ptr, DWORD sz)
 {
  return 0;
 }

 bool   __fastcall ip_line::do_send(LPMPROTO_HEADER mph)
 {

  mph->pkt_num    = get_packet_num();
  mph->internal   = GKIPLINE_SECRET_DWORD;
  mproto_protect(mph);
  try{
	  //TLockHelper l(send_queue.get_locker());
	  send_queue.PutIntoQueue(mph,mproto_size(mph));
	 }
	 catch(...)
	 {
       send_queue.full_unlock();
	   ipm_report(REPORT_ERROR_TYPE,"Исключение при работе с очередью ",0,0);
	   connect(false);
	 }
  return true;
 }

 #pragma warn .8057

 void  __fastcall ip_line::send_processing()
 {
   BYTE   buf[IPLINE_BUFFSZ];
   LPMPROTO_HEADER mph = (LPMPROTO_HEADER)buf;
   DWORD last_error    = 0;
   int   send_len = 0;
   int   data_len = 0;
   while(socket && send_queue.QueueCount() && !sending)
   {
     send_queue.PeekFromQueue(buf,sizeof(buf),&data_len);
     send_len = socket->Send(mph,data_len,0);
     if(send_len==data_len)
       {
        send_queue.DropFirst();
        update_stats_tx(send_len,1,0);
        last_error = 0;
        no_tx_timer = 0;
       }
       else
       {
         last_error = socket->GetLastError();
         break;
       }
   }

   if(last_error   )
    {
         if(last_error!=WSAEWOULDBLOCK)
         {
          wchar_t error_text[MAX_PATH];
          owner->get_error_text(last_error,error_text,sizeof(error_text)/sizeof(wchar_t),NULL);
          char rep_text[MAX_PATH<<1];
          DWORD len = owner->print_rct(rep_text,sizeof(rep_text),IDS_REP_TRANSMIT_ERROR,owner->get_modem_number(),get_number(),inet_ntoa(host_addr.sin_addr),last_error);
          //DWORD len = sprintf(rep_text,"%s Рассоединение: Ошибка %d при передаче  ",inet_ntoa(host_addr.sin_addr),last_error);
          Unicode2Ansi(rep_text+len,error_text);
          ipm_report(REPORT_ERROR_TYPE,rep_text);
          connect(false);
         }
         else
         {
           send_queue.GetEvent().SetEvent(false);
           Sleep(0);
           sending = true;
         }
    }
 }


void __fastcall ip_line::on_timer()
{
 if(!is_connected())
 {

  if(server_addr[0])
  {
    if(!check_state(MODEM_LINE_STATE_CONNECTING,false) && !is_connected() &&  (++no_tx_timer)%5 == 0)
        do_connect();
  }
     else
      Terminate();
 }
 else
 {
  send_keep_alive();
  if((++no_rx_timer)%10==0)
   {
    if(this->client_options & IPM_CLOPT_COMPATIBLE_MODE)
        no_rx_timer = 0;
      else
      {
          //не было принято ни одного кадра в течении 10 сек
          //" Рассоединение Не работоспособен тракт приёма"
         char rep_text[MAX_PATH];
         connect(false);
         owner->print_rct(rep_text,sizeof(rep_text),IDS_REP_NO_RX_ERROR,owner->get_modem_number(),get_number(),inet_ntoa(host_addr.sin_addr));
         ipm_report(REPORT_ERROR_TYPE,rep_text,0,0);

      }
   }
   alarm_timer();
 }
}

 void  __fastcall ip_line::send_keep_alive()
 {
   if((++no_tx_timer)%3==0 )
   {
    if(!send_queue.QueueCount())
    {
    MPROTO_HEADER mph = {0};
    mph.addr_from.line  = this->get_number();
    mph.addr_from.modem = owner->get_modem_number();
    mph.addr_from.reserv1 = -1;
    mph.data_size = sizeof(mph.data);
    do_send(&mph);
    }
    else
    {
     //send_processing();
     char rep_text[MAX_PATH];
     owner->print_rct(rep_text,sizeof(rep_text),IDS_REP_KPA_ERROR,owner->get_modem_number(),get_number(),inet_ntoa(host_addr.sin_addr));
     //wsprintf(rep_text,"%s Рассоединение Не работоспособен тракт передачи",inet_ntoa(host_addr.sin_addr));
     ipm_report(REPORT_ERROR_TYPE,rep_text,0,0);
     connect(false);
    }
   }
 }


