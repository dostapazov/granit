#pragma hdrstop
#include "cpc152dev.hpp"
#include <winerror.h>





//  int generate_sin(fftw_complex * out,double freq,double discr,double ampl,int pcount,bool positive)
//  {
//	//Генерация синусоиды частотой freq
//	//Дискретностью discr
//	double PI2     = M_PI * 2.0;
//	double period  = discr/freq;
//	double delta_angle   = PI2/period;
//	double angle = 0;
//	int count = period*pcount;
//	count*=2;
//	for(int i = 0;i<count;i++)
//	{
//	  double val = sin(angle);
//	  if(positive)
//		 val = fabs(val);
//	  *out[0] = val*ampl;
//	  ++out;
//	  angle+=delta_angle;
//	  if(angle> PI2)
//		 angle-=PI2;
//	}
//	return count;
//  }




  cpc152dev_line::cpc152dev_line(DWORD ln):
         modem_line(ln)
        ,terminate_event(true,false)
        ,timer (false)
        ,socket(NULL)
        ,network_event(NULL)
        ,otd_locker(NULL)
        ,analog_sample_size(0)

  {
    memset(&line_config,0,sizeof(line_config));
	line_config.dw_size = sizeof(line_config);
    memset(this->discrete_devs,0,sizeof(discrete_devs));
    memset(this->analog_devs,0,sizeof(analog_devs));
  }


  DWORD __fastcall cpc152dev_line::get_line_text     (wchar_t * text,DWORD text_sz)
  {
   DWORD ret = 0;
   ret = snwprintf(text,text_sz,L"КП %d:%s",line_config.cp_number,line_config.cp_name);
   return ret;
  }

  DWORD    __fastcall cpc152dev_line::config_get(LPCPC152_LINE_CONFIG lc)
  {
    memcpy(lc,&line_config,sizeof(*lc));
    return GKH_RET_SUCCESS;
  }

  DWORD    __fastcall cpc152dev_line::config_set(LPCPC152_LINE_CONFIG lc)
  {
     DWORD ret = GKH_RET_SUCCESS;
     bool need_restart = (line_config.ip_port != lc->ip_port || wcscmp(line_config.ip_addr,lc->ip_addr)) ? true : false;
     if(need_restart && this->is_worked())
        this->stop();
        else
        need_restart = false;

     memcpy(&line_config,lc,sizeof(line_config));
     setup_analog_calc_params();
     if(need_restart)
        start();
    return ret;
  }

  bool  __fastcall cpc152dev_line::analog_calc_start_thread         ()
  {
        bool ret = false;
        analog_params_calc_thread     = NULL;
        analog_params_calc_thread     = new boost::thread(boost::bind(&cpc152dev_line::analog_params_calc_thread_proc,this));
        ret  =  analog_params_calc_thread ? true:false;
        return ret;

  }

  void  __fastcall cpc152dev_line::analog_calc_stop_thread         ()
  {

    if(analog_params_calc_thread)
    {
     icp.PostStatus(0,0,NULL);
     analog_params_calc_thread->join();
     delete analog_params_calc_thread;
    }
    analog_params_calc_thread = NULL;
  }


  bool  __fastcall cpc152dev_line::BeforeExecute         ()
  {

    bool   ret    = TGKThread::BeforeExecute();
    if(!otd_locker)
        otd_locker = &get_cpc152controller()->otd_get_locker();

    send_queue.DropData();
    line_state&= ~MODEM_LINE_STATE_RXDATA;

    mpb.move_to_begin(mpb.get_data_len(),0);
    pkt_num = 0;
    connect_error_log = false;
    no_rx_timer = 0;
    no_tx_timer = 0;

    terminate_event.SetEvent(false);
    ss.Clear();
    ss+=terminate_event();
    ss+=send_queue.GetEvent();
    ss+=timer();
    if(ret && analog_calc_start_thread())
    {
        Sleep(20);//Задержка из-за гонок в уведомлении modemmer'a об изменении состояния линии
        do_connect();
    }
    timer.StartPeriodTimer(1000,true);
    return ret;
  }

  void  __fastcall cpc152dev_line::BeforeTerminate       ()
  {
   analog_calc_stop_thread ();
   TGKThread::BeforeTerminate();
   timer.StopTimer();
   if(socket)
      {
       if(is_connected())
          {
            WSASendDisconnect(socket->GetSocket(),NULL);
            connect(false);
           }
       else
       {
          if(socket)
          {
           socket->Disconnect();
           socket->DestroySocket();
           delete socket;
          }
       }
      socket = NULL;
    }
   if(network_event)
      WSACloseEvent(network_event);
   network_event = NULL;
   ss.Clear();
   set_state(MODEM_LINE_STATE_WORKED,false,true);
  }


  int   __fastcall cpc152dev_line::Execute               ()
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

 bool  __fastcall cpc152dev_line::do_start  ()
 {
  bool ret = false;
  SetFlags(KERTL_THREAD_SELFDESTRUCT,true);
  local_alarm_times[0] = local_alarm_times[1] = 0;
  ret = Start();
  return  ret;
 }

 bool  __fastcall cpc152dev_line::do_stop   ()
 {
  bool ret ;
  SetFlags(KERTL_THREAD_SELFDESTRUCT,false);
  ret = TerminateAndWait(3000,true);
  return  ret;
 }


 void  __fastcall cpc152dev_line::handle_network_events()
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


//      if(ne.lNetworkEvents&FD_OOB)
//        {
//         handle_oob(ne.iErrorCode[FD_OOB_BIT]);
//         ne.lNetworkEvents&=~FD_OOB;
//        }
      if(ne.lNetworkEvents)
      {
       ne.lNetworkEvents = 0;
      }

     }
 }

 void  __fastcall cpc152dev_line::handle_disconnect(DWORD err)
 {
    wchar_t rep_text[MAX_PATH];
    int len = snwprintf(rep_text,KERTL_ARRAY_COUNT(rep_text),L"КП %d.Рассоединение c %s",line_config.cp_number,line_config.ip_addr);
    if(err)
      {
       len += snwprintf(rep_text+len,KERTL_ARRAY_COUNT(rep_text)-len,L" Ошибка %d ",err);
       GetErrorMessage(err,rep_text + len,KERTL_ARRAY_COUNT(rep_text)-len,NULL);
      }
    //owner->print_rct(rep_text,sizeof(rep_text),IDS_REP_DISCONNECT,owner->get_modem_number(),get_number(),inet_ntoa(host_addr.sin_addr),err);
    do_report(REPORT_ERROR_TYPE,rep_text);
    connect(false);
 }

 #pragma warn .8057

 void  __fastcall cpc152dev_line::handle_connect(DWORD err)
 {
    wchar_t rep_text[MAX_PATH<<1];
    if(err)
    {
     if(connect_error_log!=err)
     {
      //owner->print_rct(rep_text,sizeof(rep_text),IDS_REP_CONNECT_ERROR,owner->get_modem_number(),get_number(),err,server_addr);
       int len = snwprintf(rep_text,KERTL_ARRAY_COUNT(rep_text),L"КП %d.Ошибка соединения %d c %s ",line_config.cp_number,err,line_config.ip_addr);
       GetErrorMessage(err,rep_text + len,KERTL_ARRAY_COUNT(rep_text)-len,NULL);
       do_report(REPORT_ERROR_TYPE,rep_text);
       connect_error_log = err;
     }
    }
    else
    {
      snwprintf(rep_text,KERTL_ARRAY_COUNT(rep_text),L"КП %d.Установлено соединение c %s",line_config.cp_number,line_config.ip_addr);
      do_report(REPORT_INFORMATION_TYPE,rep_text);
      connect_error_log = FALSE;
    }
    connect(err ? false:true);
 }

 void  __fastcall cpc152dev_line::handle_recv(DWORD err)
 {
   wchar_t rep_text [MAX_PATH<<1];

   if(err)
   {
    //owner->print_rct(rep_text,sizeof(rep_text),IDS_REP_RECV_ERROR,owner->get_modem_number(),get_number(), err);
    //strcpy(rep_templ,);
    snwprintf(rep_text,KERTL_ARRAY_COUNT(rep_text),L"%03d.%03d Ошибка при приёме %d",owner->get_modem_number(),get_number(), err);
    do_report(REPORT_ERROR_TYPE,rep_text);
    update_stats_rx(0,0,1);
   }
   else
   no_rx_timer = 0;

   DWORD total_read = read_from_socket();
   /*Вычитали всё из гнезда, теперь начинаем разбор полётов*/
   while(total_read && this->is_connected())
   {
   update_stats_rx(total_read,0,0);
   DWORD rd_len;

   do{
      err = CPC152DEV_LINE_HANDLE_SUCCESS;
      DWORD sync_offs =  mpb.sync_internal();
      if(sync_offs)
       {
         snwprintf(rep_text,KERTL_ARRAY_COUNT(rep_text),L"КП-%d.Рассинхронизация",line_config.cp_number);
         do_report(REPORT_INFORMATION_TYPE,rep_text);
         update_stats_rx(0,0,1);
       }

      rd_len = mpb.get_mproto_len();
      if(rd_len)
      {

        LPMPROTO_HEADER mph = (LPMPROTO_HEADER)mpb.begin();
        //DWORD cv = otd_calc_checksum(mph,sizeof(*mph)-sizeof(mph->data[0]),OTD_DEF_POLINOM);
        if(!mproto_check(mph))
        {
         //owner->print_rct(rep_text,sizeof(rep_text),IDS_REP_CHECKSUM_ERROR,owner->get_modem_number(),get_number(),mph->header_cs);
          snwprintf(rep_text,KERTL_ARRAY_COUNT(rep_text),L"КП-%d.Ошибка контрольной суммы заголовка кадра",line_config.cp_number);

         do_report(REPORT_ERROR_TYPE,rep_text);
         err ++;
        }

       update_stats_rx(0,1,err);
       if(!err)
      {
       if(last_pkt_num)
         {
          if(mph->pkt_num!= 1 && last_pkt_num!= (WORD)-1  &&  (abs((int)mph->pkt_num-(int)last_pkt_num)>1) )
           {

              //owner->print_rct(rep_text,sizeof(rep_text),IDS_REP_LOSTKADR_ERROR,owner->get_modem_number(),get_number(),(DWORD)last_pktnum+1,(DWORD)mph->pkt_num-1);

              snwprintf(rep_text,KERTL_ARRAY_COUNT(rep_text),L"КП-%d.Потеря кадров c %d по %d",line_config.cp_number,(DWORD)last_pkt_num+1,(DWORD)mph->pkt_num-1);
              do_report(REPORT_ERROR_TYPE,rep_text);
              update_stats_rx(0,0,1);
              //TODO вызов опроса дискретных данных
              get_cpc152controller()->otd_set_diag(this->line_config.cp_number,OTD_DIAG_CPMASK);
              request_discrete();
            }
         }

        last_pkt_num = mph->pkt_num;

       if(mph->fa) // Исключить тестовую посылку проверки канала связи
         {
          //queue_rxdata(mph->fa,(LPBYTE)mph->data,mph->data_size,false);
          err = do_handle_recv(mph);
         }
       }
       if(!err)
       {
           mpb.move_mproto();
           sync_offs =  mpb.sync_internal();
           if(sync_offs)
            {
             snwprintf(rep_text,KERTL_ARRAY_COUNT(rep_text),L"КП-%d.Рассинхронизация",line_config.cp_number);
             do_report(REPORT_INFORMATION_TYPE,rep_text);
             update_stats_rx(0,0,1);

            }
       }
       else
       connect(false);
      }
     }while(rd_len && !err && this->is_connected());
     total_read = read_from_socket();
   }
 }

 int   __fastcall cpc152dev_line::read_from_socket()
 {
  //Процедура чтения данных из гнезда
   DWORD rd_len;
   DWORD total_read = 0;
   do
   {
    socket->IoctlSocket(FIONREAD,&rd_len);
    if(rd_len)
    {
     total_read+= rd_len;
     if(mpb.get_free_size()<rd_len)
        mpb.grow(CPC152_LINE_BUFFSZ);
     char * b = mpb.end();
     rd_len = socket->Recv(b,rd_len,0);
     mpb.add(b,rd_len);
     total_read+=rd_len;
    }

   }while(rd_len);
   return total_read;
 }





 void  __fastcall cpc152dev_line::send_processing()
 {
   BYTE   buf[CPC152_LINE_BUFFSZ];
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
          owner->get_error_text(last_error,error_text,KERTL_ARRAY_COUNT(error_text),NULL);
          wchar_t rep_text[MAX_PATH<<1];
          //DWORD len = owner->print_rct(rep_text,sizeof(rep_text),IDS_REP_TRANSMIT_ERROR,owner->get_modem_number(),get_number(),inet_ntoa(host_addr.sin_addr),last_error);
          snwprintf(rep_text,KERTL_ARRAY_COUNT(rep_text),L"КП-%d.Рассоединение: Ошибка %d при передаче (%s)",line_config.cp_number,last_error,error_text);
          do_report(REPORT_ERROR_TYPE,rep_text);
          connect(false);
         }
         else
         {
           send_queue.GetEvent().SetEvent(false);
           Sleep(1);
           sending = true;
         }
    }
 }

 void __fastcall cpc152dev_line::on_timer()
{
 wchar_t rep_text[MAX_PATH];
 if(!is_connected())
 {
  if(!check_state(MODEM_LINE_STATE_CONNECTING,false) && !is_connected() &&  (++no_tx_timer)%3 == 0)
      do_connect();
 }
 else
 {
  send_keep_alive();
  if((++no_rx_timer)%10==0)
   {
      //не было принято ни одного кадра в течении 10 сек
      //" Рассоединение Не работоспособен тракт приёма"

     //owner->print_rct(rep_text,sizeof(rep_text),IDS_REP_NO_RX_ERROR,owner->get_modem_number(),get_number(),inet_ntoa(host_addr.sin_addr));
     snwprintf(rep_text,KERTL_ARRAY_COUNT(rep_text),L"КП %d : Рассоединение : не работает тракт приёма",line_config.cp_number);
     do_report(REPORT_ERROR_TYPE,rep_text);
     connect(false);
   }

   if(current_alarm && ++alarm_recv_timer>3)
   {
      snwprintf(rep_text,KERTL_ARRAY_COUNT(rep_text),L"КП %d : Таймаут приёма архива.Повторный вызов списка.",line_config.cp_number);
      do_report(REPORT_ERROR_TYPE,rep_text);
      current_alarm = 0;
      alarms_list.clear();
      this->request_alarm_list();

   }
 }
}

 void  __fastcall cpc152dev_line::send_keep_alive()
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
     wchar_t rep_text[MAX_PATH];
     snwprintf(rep_text,KERTL_ARRAY_COUNT(rep_text),L"КП-%d.Рассоединение Не работоспособен тракт передачи",line_config.cp_number);
     do_report(REPORT_ERROR_TYPE,rep_text);
     connect(false);
    }
   }
 }

 bool   __fastcall cpc152dev_line::do_send(LPMPROTO_HEADER mph)
 {

  mph->pkt_num    = get_packet_num();
  mph->internal   = MP_INTERNAL_DEFAULT;
  mproto_protect(mph);
  try{
      TLockHelper l(send_queue.get_locker());
      send_queue.PutIntoQueue(mph,mproto_size(mph));
     }
     catch(...)
     {

       wchar_t rep_text[512];
       snwprintf(rep_text,KERTL_ARRAY_COUNT(rep_text),L"КП-%d.Исключение при работе с очередью передачи",line_config.cp_number);
       do_report(REPORT_ERROR_TYPE,rep_text);
       connect(false);
     }
  return true;
 }


void  __fastcall cpc152dev_line::do_connect()
 {
  no_rx_timer = no_tx_timer = 0;
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
   set_state(MODEM_LINE_STATE_WORKED|MODEM_LINE_STATE_CONNECTING,true,true);
   socket->SetRecvBuffSize(CPC152_LINE_BUFFSZ);
   char ip_addr[512];
   Unicode2Ansi(ip_addr,KERTL_ARRAY_COUNT(ip_addr),line_config.ip_addr);
   host_addr = *&TIPAddress(ip_addr,line_config.ip_port);
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

  void  __fastcall cpc152dev_line::on_disconnect()
 {
   if(local_alarm_times[1])
   {
     this->message_end_alarm_recv(local_alarm_times);
   }
   local_alarm_times[0] = local_alarm_times[1] = 0;
   modem_line::on_disconnect();
   send_queue.DropData();
   mpb.clear();
   sending     = FALSE;
   current_alarm = 0;
   alarms_list.clear();
   last_pkt_num  = 0;
   get_cpc152controller()->otd_set_diag(this->line_config.cp_number,OTD_DIAG_CPMASK);
   SetThreadPriority(tpHighest);

 }

 void  __fastcall cpc152dev_line::on_connect        ()
 {
   set_state(MODEM_LINE_STATE_CONNECT|MODEM_LINE_STATE_TXREADY,true,true);
   modem_line::on_connect();
   mpb.clear();
   current_alarm = 0;
   analog_seq = discrete_seq = -1;
   last_pkt_num = 0;
   alarms_list.clear();
   request_discrete  ();
   alarm_starts_send_all();
   request_alarm_list();

 }




  void __fastcall cpc152dev_line::handle_otd_query(sotd_proto & sop)
  {
    request_discrete();
    if(otd_locker) otd_locker->lock();
    Tcpc152controller * ctrl =  get_cpc152controller();
    if(ctrl)
    {
        proto_pointer ptr_beg,ptr_end;
        sotd_addr saddr(*sop.op.addr);
        BYTE ln    = get_number();
        if(saddr.cp == ln || saddr.cp == OTD_ADDR_MAXVALUE)
        {
            saddr.cp = ln;
            if(ctrl->get_otd_range(saddr,ptr_beg,ptr_end))
            {
             update_modemmer_tx(sop.op.proto_size,1);
             while(ptr_beg<ptr_end)
             {
               if(otd_addr_include(ptr_beg->op.addr,&saddr) )
               queue_rxdata(FA_OTD,(LPBYTE)ptr_beg->op.addr,ptr_beg->op.proto_size,false);
               ++ptr_beg;
             }
            }
        }
    }
    if(otd_locker) otd_locker->unlock();
  }

  bool  __fastcall cpc152dev_line::send        (LPMPROTO_HEADER mph,DWORD sz)
  {
	if(mph->fa == FA_OTD)
	{
	      sotd_proto sop;
	      sop.assign((LPBYTE)mph->data,mph->data_size);
          if(OTD_ADDR_ISQUERY(sop.op.addr))
          {

           handle_otd_query(sop);
          }
          else
          {
            if(sop.op.addr->cp == line_config.cp_number && OTD_ADDR_ISSETUP(sop.op.addr) )
            {
              if(sop.op.extension)
              {
                update_modemmer_tx(sz,1);
                if(sop.op.extension->block_type == OTD_BLOCKTYPE_ALARM )
                {
                 lpotd_alarm_param ap = (lpotd_alarm_param)sop.op.extension->block_data;
                 int  count = sop.op.extension->block_size/sizeof(*ap);
                 handle_alarm_params(count,ap);
                }
                if(sop.op.extension->block_type == OTD_BLOCKTYPE_START_OSCILLOGRAMM)
                {
                  lpotd_oscillgramm_param oscp = (lpotd_oscillgramm_param)sop.op.extension->block_data;
                  if(oscp->param_addr.addr.cp == line_config.cp_number)
                  {
                    start_write_oscilogramm(oscp->osc_lenght);
                  }
                }
              }
              else
              {
               wchar_t   text[MAX_PATH];
               {
                TLockHelper l(locker);
                snwprintf(text,KERTL_ARRAY_COUNT(text),L"КП %d в команде установки отсутствуют данные op.extension = NULL ",line_config.cp_number);
               }
               do_report(REPORT_ERROR_TYPE,text);
              }
            }
          }
	}

    if(mph->fa == FA_OTD_ALARM_ARCHIVE)
    {
      //Обработка запроса на создание осцилограммы
    }
	return true;
  }

 void     __fastcall cpc152dev_line::do_report(DWORD type,char * descr )
 {
  Tcpc152controller * own = this->get_cpc152controller();
  if(own)
    own->do_report(type,descr);
 }

 void     __fastcall cpc152dev_line::do_report(DWORD type,wchar_t * descr )
 {
  Tcpc152controller * own = get_cpc152controller();
  if(own)
    own->do_report(type,descr);
 }


void  __fastcall cpc152dev_line::request_discrete ()
{
   BYTE buf[256];
   LPMPROTO_HEADER mph    = (LPMPROTO_HEADER)buf;
   lpcpc152proto_hdr phdr = (lpcpc152proto_hdr)mph->data;
   phdr->count = 0;
   phdr->fa    = CPC152PROTO_FA_REQUEST_DISCRETE;
   mproto_init(mph,0,FA_DEVICE_RAW_CPC152,cpc152proto_hdr_calc_size(0));
   do_send(mph);
}

void  __fastcall cpc152dev_line::request_alarm_list()
{
	char buff[256];
	LPMPROTO_HEADER mph = (LPMPROTO_HEADER)buff;
	lpcpc152proto_hdr phdr = (lpcpc152proto_hdr)mph->data;
	phdr->count = 0;
	phdr->fa    = CPC152PROTO_FA_GET_ALARMS_LIST;
	mproto_init   (mph,0,FA_DEVICE_RAW_CPC152,cpc152proto_hdr_calc_size(0));
	do_send(mph);
}

  void      __fastcall cpc152dev_line::alarm_start_write(KeRTL::TStream & s)
  {
	int sz = alarm_starts.size();
	s<<sz;
	if(sz)
	{
	 TLockHelper l(locker);
	 alarm_start_t::iterator beg = alarm_starts.begin(),end = alarm_starts.end();
	 while(beg<end)
	 {

	  s.Write(&*beg,sizeof(alarm_start_t::value_type));
	  ++beg;
	 }
	}
  }

  void      __fastcall cpc152dev_line::dft_enabled_write(KeRTL::TStream & s)
  {
   s.Write(&dft_enabled,sizeof(dft_enabled));
  }

  void      __fastcall cpc152dev_line::dft_enabled_read (KeRTL::TStream & s)
  {
   s.Read(&dft_enabled,sizeof(dft_enabled));
  }


  void      __fastcall cpc152dev_line::alarm_start_read (KeRTL::TStream & s)
  {
	int sz = 0;
	s>>sz;
	for(int i = 0;i<sz;i++)
	{
	  CPC152_ALARM_START ast;
	  s.Read(&ast,sizeof(ast));
	  if(!ast.external)
		 alarm_start_add(&ast);
	}
  }

  bool      __fastcall cpc152dev_line::alarm_start_enum (DWORD idx,LPCPC152_ALARM_START dst)
  {
	bool ret = false;
	if(owner->check_data_size(dst,sizeof(*dst)))
	{
	 TLockHelper l(locker);
	 if(idx<alarm_starts.size())
	   {
		*dst = alarm_starts.at(idx);
		ret = true;
	   }
	}
	return ret;
  }

  bool      __fastcall cpc152dev_line::alarm_start_add  (LPCPC152_ALARM_START src)
  {
	bool ret = false;
	if(owner->check_data_size(src,sizeof(*src)))
	{
	 TLockHelper l(locker);
	 alarm_start_t::iterator ptr = alarm_starts.begin(),end = alarm_starts.end();
	 ptr = std::lower_bound(ptr,end,*src);
	 if(ptr<end)
	 {
	   if(*ptr == *src)
		  {
		   if(!src->external  && ptr->external )
		   {
			ptr->external = 0;
            return true;
           }
		   SetLastError(ERROR_ALREADY_EXISTS);
		   return ret;
		  }
	 }
	   alarm_starts.insert(ptr,*src);
	   ret = true;
      if(owner)
        owner->notify(MNF_NOTIFY_CPC152_ALARM_START_ADD,line_config.cp_number,src,sizeof(*src));
	   if(this->is_connected())
	   {
        //TODO
		//Отправить на контроллер
        alarm_start_send(src,false);
	   }
	}
	return ret;
  }

  bool      __fastcall cpc152dev_line::alarm_start_del  (LPCPC152_ALARM_START src)
  {
	bool ret = false;
	if(owner->check_data_size(src,sizeof(*src)))
	{
	 TLockHelper l(locker);
	 alarm_start_t::iterator ptr = alarm_starts.begin(),end = alarm_starts.end();
	 ptr = std::lower_bound(ptr,end,*src);
	 if(ptr<end && *ptr == *src )
	 {

	  ret = true;
      if(!src->external || src->external == ptr->external)
      {
          alarm_starts.erase(ptr);
          //l.unlock();
          if(owner)
          owner->notify(MNF_NOTIFY_CPC152_ALARM_START_DEL,line_config.cp_number,src,sizeof(*src));
          //l.lock();
          if(is_connected())
          {
            //TODO
            //Отправить на контроллер
            alarm_start_send(src,true);
          }
      }
	 }
	 else
	  SetLastError(ERROR_NOT_FOUND);

	}
	return ret;

  }

 void  __fastcall cpc152dev_line::handle_alarm_param (lpotd_alarm_param ap)
 {
   if(ap && ap->param_addr.addr.fa == OTD_FA_DISCRETE)
   {
     int dev_num,grp_num;
     otdgroup2devgrp (true,ap->param_addr.addr.sb,dev_num,grp_num);
     CPC152_ALARM_START ast(dev_num,grp_num,ap->param_addr.param_number,ap->value,TRUE);

     switch(ap->cmd&(~OTD_ALARM_CMD_MASK))
     {
      case OTD_ALARM_CMD_CLEAR:
           alarm_start_clear_external(&ast);
      break;
      case OTD_ALARM_CMD_SET :
           alarm_start_add(&ast);
      break;
      case OTD_ALARM_CMD_DELETE :
           alarm_start_del(&ast);
      break;
     }

   }
 }

 void  __fastcall cpc152dev_line::handle_alarm_params(int count,lpotd_alarm_param ap)
 {
   lpotd_alarm_param ap_end = ap+count;
   while(ap<ap_end)
   {
       handle_alarm_param(ap);
	   ++ap;
   }
 }

void      __fastcall cpc152dev_line::alarm_start_clear_external(LPCPC152_ALARM_START src)
{
   int idx = 0;
   CPC152_ALARM_START ast;
   while(alarm_start_enum(idx,&ast))
   {
    if(ast.external &&
      (ast.dev_num   == src->dev_num   || src->dev_num   == (WORD)-1) &&
      (ast.grp_num   == src->grp_num   || src->grp_num   == (WORD)-1) &&
      (ast.param_num == src->param_num || src->param_num == (WORD)-1)
      )
      {
       alarm_start_del(&ast);
      }
      else
      ++idx;
   }
}

void __fastcall cpc152dev_line::alarm_start_send(LPCPC152_ALARM_START ast, bool erase)
{
     char buff[512];
     LPMPROTO_HEADER mph = (LPMPROTO_HEADER)buff;
     lpcpc152proto_hdr phdr = (lpcpc152proto_hdr)mph->data;
     lptalarm_condition  ac = (lptalarm_condition )phdr->data;
     phdr->count     = 1;
     phdr->fa        = CPC152PROTO_FA_ADD_CONDITION;
     if(erase)
        phdr->fa |= CPC152PROTO_FA_FLAG_ERASE;

     ac->dev_num     = ast->dev_num;
     ac->grp_num     = ast->grp_num;
     ac->param_num   = ast->param_num;
     ac->alarm_value = ast->value;
     ac->more        = ast->value ? false : true;
     mproto_init   (mph,0,FA_DEVICE_RAW_CPC152,cpc152proto_hdr_calc_size(sizeof(*ac)));
     do_send(mph);

}

  void  __fastcall cpc152dev_line::alarm_starts_send_all()
  {
    CPC152_ALARM_START  ast(-1,-1,-1,MAXINT,0);
    alarm_start_send(&ast,true);
    TLockHelper l(locker);
    alarm_start_t::iterator ptr = alarm_starts.begin(),end = alarm_starts.end();
    while(ptr<end)
    {
     ast = *ptr;
     alarm_start_send(&ast,false);
     ++ptr;
    }
  }

  bool __fastcall cpc152dev_line::get_dft_enabled(LPCPC152_DFT_ENABLED data)
  {
   memcpy(data->dft_enabled,dft_enabled.dft_enabled,sizeof(data->dft_enabled));
   return true;
  }

  bool __fastcall cpc152dev_line::set_dft_enabled(LPCPC152_DFT_ENABLED data)
  {
   memcpy(dft_enabled.dft_enabled,data->dft_enabled,sizeof(dft_enabled.dft_enabled));
   Tcpc152controller * ctrl = get_cpc152controller();
   if(ctrl)
   {
    TLockHelper l(ctrl->otd_get_locker());
    for(int i = 0;i<CPC152_MAX_SLOTS;i++)
    {
     cpc152dev_t & dev = analog_devs[i];
     for(int j = 0;j<CPC152_MAX_GROUPS;j++)
     {
      lpanalog_ext_t aext = (lpanalog_ext_t) dev.dev_ext[j];
      if(aext)
         aext->dft_enabled   = dft_enabled.dft_enabled[i][j];
     }
    }
    return true;
   }
   return false;
  }



