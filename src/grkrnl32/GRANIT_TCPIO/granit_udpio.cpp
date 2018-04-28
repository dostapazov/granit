//   Important note about DLL memory management when your DLL uses the
//   static version of the RunTime Library:
//
//   If your DLL exports any functions that pass String objects (or structs/
//   classes containing nested Strings) as parameter or function results,
//   you will need to add the library MEMMGR.LIB to both the DLL project and
//   any other projects that use the DLL.  You will also need to use MEMMGR.LIB
//   if any other projects which use the DLL will be performing new or delete
//   operations on any non-TObject-derived classes which are exported from the
//   DLL. Adding MEMMGR.LIB to your project will change the DLL and its calling
//   EXE's to use the BORLNDMM.DLL as their memory manager.  In these cases,
//   the file BORLNDMM.DLL should be deployed along with your DLL.
//
//   To avoid using BORLNDMM.DLL, pass string information using "char *" or
//   ShortString parameters.
//
//   If your DLL uses the dynamic version of the RTL, you do not need to
//   explicitly add MEMMGR.LIB as this will be done implicitly for you

#pragma hdrstop
#pragma argsused
#include "granit_udpio.hpp"
#include <stdio.h>


LRESULT WINAPI module_main(DWORD cmd,LPARAM p1, LPARAM p2)
{
  LRESULT ret = 0;
       switch(cmd)
       {
        case GKME_CMD_GET_MODULETYPE:
			ret = MT_GRSCIO;break;
        case GKME_CMD_CREATE_HANDLE :
        {
            TGranitUdpIo * mod = new TGranitUdpIo((wchar_t*)p1);
            ret  = (LRESULT)(*(TGKHandleBased*)(mod))();
            if(!ret)
             delete mod;
        }
        break;
       }
       return ret;
}

  TGranitUdpIo::TGranitUdpIo(wchar_t * name):TGKIOBase(name),tx_timer(false)
  {

     lock_param = GKHB_AUTO_LOCK_OFF;
     net_mode = tcp_mode;
     *remote_addr = 0;
      remote_port = 4001;
     data_type = GRUDP_DT_GRANIT;
     //L"CЦ-Гранит Ввод/вывод"
     error_open_log_write = FALSE;
     socket = NULL;
     is_connected = FALSE;
     TBaseSocket::InitWS(MAKEWORD(2,2));
  }

  void    __fastcall  TGranitUdpIo::reg_reports()
  {
     wchar_t rep_templ[512];
     get_lang_string(IDS_REPORT_TITLE,rep_templ,sizeof(rep_templ)/sizeof(wchar_t));
     report_id =   report_reg_event_type(L"GRSC_UDP_IOMODULE",rep_templ);
  }


  DWORD   __fastcall TGranitUdpIo::get_config_data(DWORD mask,LPVOID buf,DWORD bsz)
  {
    TLockHelper l(locker);
    DWORD len = 0;
    if(buf && (int)bsz>=(lstrlenW(remote_addr)+remote_port ? 12:0))
      {
       wchar_t * out = (wchar_t*)buf;

          *out = (net_mode == tcp_mode) ? L'T':L'U';
           out[++len] = 0;
       len += wsprintfW(out+len,L"%s",remote_addr);
       if(remote_port)
         wsprintfW(out+len,L":%d",remote_port);
      }
    return GKH_RET_SUCCESS;
  }

  bool    __fastcall TGranitUdpIo::set_config_data(DWORD mask,LPVOID buf,DWORD bsz)
  {
   lock();
   wchar_t text[MAX_PATH];
   safe_strcpy(text,(wchar_t*)buf);
   wchar_t * _port = wcschr(text,':');
   if(_port)
     {
      *_port++ = 0;
      remote_port = _wtoi(_port);
     }
     int offset = 0;
     switch(*text)
     {
      case L'u': case L'U': net_mode = udp_mode;    ++offset; break;
      case L't': case L'T': net_mode = tcp_mode;    ++offset; break;
     }
     safe_strcpy(remote_addr,text+offset);
     unlock();
   return true;
  }

  bool    __fastcall TGranitUdpIo::compare_config_data(DWORD mask,LPVOID buf,DWORD bsz,LPDWORD ch_mask,BOOL * restart)
  {
    lock();
    if(lstrcmpiW(remote_addr,(wchar_t*)buf))
    {
     if(ch_mask)
        *ch_mask|=CCFL_MODULE_CONFIG;
     if(restart)
        *restart = TRUE;
    }
    unlock();
    return true;
  }

  LRESULT __fastcall TGranitUdpIo::enum_devices(LPMIO_ENUM_DEVICES ed)
  {
   return GKH_RET_ERROR;
  }



  DWORD   __fastcall  TGranitUdpIo::OnException(DWORD ExceptionCode,LPEXCEPTION_POINTERS exp)
  {
   DWORD  ret = TGKThread::OnException(ExceptionCode, exp);
   return ret;
  }

 BOOL    __fastcall TGranitUdpIo::can_start(DWORD reason,LPARAM p2)
 {
   BOOL   ret = TRUE;
    int proto[2] = {IPPROTO_TCP,0};
    WSAPROTOCOL_INFO pi[10] ={0};
    DWORD sz   = sizeof(pi);
    int cnt = WSAEnumProtocols(proto,pi,&sz);
    if(cnt <1)
       {

        SetLastError(WSAEPROTONOSUPPORT	);
        ret = FALSE;
       }

   return ret;
 }

  DWORD    __fastcall TGranitUdpIo::start      (DWORD reason,LPARAM p)
  {
   DWORD ret = GKH_RET_ERROR;
   reg_reports();
   error_open_log_write = FALSE;
   if(*remote_addr && remote_port)
   {
    /*Запуск модуля ввода вывода*/
    stop_event  = CreateEvent(NULL,FALSE,FALSE,NULL);
    wr_event    = CreateEvent(NULL,FALSE,FALSE,NULL);
    socket_destroy();

    if(stop_event )
    {
      synchro_set.Clear();
      synchro_set+=  stop_event;
      synchro_set+=  wr_event;
      synchro_set+=  tx_timer;

      if(Start(32768))
         {
          set_module_state(MODULE_STATE_RUNNING,true);
          ret = GKH_RET_SUCCESS;
         }
    }

   }
   return ret;

  }

  DWORD    __fastcall TGranitUdpIo::stop       (DWORD reason)
  {
     DWORD ret ;
     SetEvent(stop_event);
     TerminateAndWait(2000,true);
     socket_destroy();
     CloseHandle   (wr_event);
     CloseHandle   (stop_event);
     synchro_set.Clear();
     set_module_state(MODULE_STATE_RUNNING,false);
     ret = GKH_RET_SUCCESS;
     return ret;
  }

  LRESULT  __fastcall TGranitUdpIo::read       (LPMIO_REQUEST io_req)
  {
    LRESULT ret = GKH_RET_ERROR;
    if(check_data_size(io_req,sizeof(*io_req)))
    {
    if(rd_queue.QueueCount())
    {
      int len;
      if(rd_queue.PeekFromQueue(io_req,MIO_REQUEST_SIZE(io_req),&len))
         {
          rd_queue.DropFirst();
          ret = GKH_RET_SUCCESS;
         }
         else
         SetLastError(ERROR_INSUFFICIENT_BUFFER);
    }
    else
    SetLastError(ERROR_MORE_DATA);
    }
    return ret;
  }

  LRESULT  __fastcall TGranitUdpIo::write      (LPMIO_REQUEST io_req)
  {
    LRESULT ret = GKH_RET_ERROR;
    udp_dev_kadr kadr;
    bzero(&kadr,sizeof(kadr));
    DWORD   channel = io_req->channel;
    if(is_connected)
    {
    if(check_data_size(io_req,sizeof(*io_req)))
    {
     if(io_req->data_len>=4 && io_req->data_len<=sizeof(kadr.data))
     {
     memcpy(kadr.data,io_req->data,io_req->data_len);
     kadr.data_type  = data_type;
     kadr.data_len   = io_req->data_len;
     //channel         = ((kadr.data[0]>>4)&0x03);//Номер канала = 2 младших бита номера этажа
     kadr.chanel     = io_req->channel;
     wr_queue[channel].PutIntoQueue(&kadr,sizeof(kadr));
     SetEvent(wr_event);
     ret = GKH_RET_SUCCESS;
     }
     else
     SetLastError(ERROR_MESSAGE_EXCEEDS_MAX_SIZE);
    }
    }
    else
     SetLastError(ERROR_WRITE_FAULT);
    return ret;
  }

  bool     __fastcall TGranitUdpIo::is_ready   (bool tx)
  {
    return is_connected;
  }

  bool     __fastcall TGranitUdpIo::socket_create()
  {
      socket_destroy();
      if(TBaseSocket::InitWS(2))
      {
       socket = new TIPSocket(net_mode,false);
       socket->EnableReuseAddress(true);
       if(net_mode)
          socket->EnableKeepAlive(true);

       char _remote_addr[MAX_PATH];
       KeRTL::Unicode2Ansi(_remote_addr,sizeof(_remote_addr),this->remote_addr);
       memcpy(host_addr(),TIPAddress(_remote_addr,remote_port)(),sizeof(host_addr));
       tx_packet_counter    = 0;
       total_recv_bytes     = 0;
       success_recv_bytes   = 0;
       error_recv_bytes     = 0;
       last_rx_time = last_tx_time   = GetTickCount();

       socket_event    = CreateEvent(NULL,FALSE,FALSE,NULL);
       synchro_set+=socket_event;
       WSAEventSelect(socket->GetSocket(),socket_event,FD_READ|FD_CLOSE|FD_CONNECT);
       socket->Connect(host_addr(),sizeof(host_addr));
      }
      return socket ? true: false;
  }

  bool     __fastcall TGranitUdpIo::socket_destroy()
  {
     if(socket)
        {
         tx_timer.StopTimer();
         socket->DestroySocket();
         delete socket;
         socket = NULL;
         call_owner(CMDIO_ERROR,  IO_ERROR_CLOSE,0);
         }

        if(IsValidHandle(socket_event))
        {
         synchro_set -=socket_event;
         CloseHandle(socket_event);
         socket_event = NULL;
        }

        wr_queue[0].DropData();
        wr_queue[1].DropData();
        wr_queue[2].DropData();
        wr_queue[3].DropData();
        rd_queue.DropData();
        is_connected = false;

     return true;
  }


  int      __fastcall TGranitUdpIo::Execute()
  {
   DWORD wr;
   char debug_str[MAX_PATH];
   rxb.set_size(UDP_RX_RAWDATA_SIZE<<4);
   is_connected = FALSE;
   socket_create();

   do{
      wr = synchro_set.Wait(1000);
      switch(wr)
      {
       case WR_WREVENT: //Событие не пустая очередь на приём
             if(!tx_timer.IsActive())
             {
              LARGE_INTEGER li;
              li.QuadPart  = -MSEC_NS100(50);
              tx_timer.SetTimer(li,true,true);
             }
             break;
       case WR_WRTIMER: //Событие не пустая очередь на прередачу
                do_tx_udpdata();
             break;
       case WR_SOCKET_EVENT: /*Завершилось чтение*/
              handle_socket_events();
             break;
       case WAIT_TIMEOUT:
                          //OutputDebugString("USB: WaitTimeout");
                          on_timer();
                          break;
      }
       tx_watchdog();
     }while(wr && !CheckTerminateRequest());


   socket_destroy();
   return 0;
  }


  void     __fastcall TGranitUdpIo::handle_socket_events()
  {
   WSANETWORKEVENTS ne={0};

   while(socket &&  !WSAEnumNetworkEvents(socket->GetSocket(),socket_event,&ne) && ne.lNetworkEvents)
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


      if(ne.lNetworkEvents&FD_CLOSE)
      {
        handle_disconnect(ne.iErrorCode[FD_CLOSE_BIT]);
        ne.lNetworkEvents&=~FD_CLOSE;
      }

      if(ne.lNetworkEvents)
       ne.lNetworkEvents = 0;
     }
  }

  void     __fastcall TGranitUdpIo::handle_connect   (DWORD error)
  {
     is_connected = error ? FALSE:TRUE;
     if(!is_connected)
        {
         socket_destroy();
         call_owner(CMDIO_ERROR,IO_ERROR_CONNECT,-1);
        }
        else
        {
         call_owner(CMDIO_ERROR,0,-1);
         last_packet_num = 255;
        }

  }

  void     __fastcall TGranitUdpIo::handle_disconnect(DWORD error)
  {
        is_connected = FALSE;
        call_owner(CMDIO_ERROR,IO_ERROR_CONNECT,-1);
        socket_destroy();
  }


 int   __fastcall TGranitUdpIo::socket_read()
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

     if(rxb.get_free_size()<rd_len)
        rxb.grow(rd_len<<1);
     char * b = rxb.end();
     rd_len = socket->Recv(b,rd_len,0);
     rxb.add(b,rd_len);
     total_read+=rd_len;
     total_recv_bytes +=rd_len;
    }

   }while(rd_len);
   return total_read;
 }

  void     __fastcall TGranitUdpIo::handle_recv      (DWORD error)
  {
     if(error)
        handle_disconnect(0);
     else
     {
       DWORD total_read = socket_read();
       if(total_read) handle_recv(rxb);

     }
  }

  void     __fastcall TGranitUdpIo::handle_recv(rx_buffer & rxb)
  {
    DWORD   beg_ptr ,beg_ptr1 ;
    DWORD   end_ptr  = 0;

    do{
    beg_ptr = rxb.find(end_ptr,0x2A);
    if(beg_ptr<(DWORD)-1)  //Нашли начало
     {
      end_ptr = rxb.find(beg_ptr+1,0x3B);
      if(end_ptr<(DWORD)-1)
        {
          int skip_count = 0;
          while((beg_ptr1 = rxb.find(beg_ptr+1,0x2A,end_ptr))<DWORD(-1))
            {
             skip_count++;
             error_recv_bytes += (DWORD)(beg_ptr1-beg_ptr);
             beg_ptr = beg_ptr1;
            }
          if(skip_count)
          {
           wchar_t rep_text[1024];
           //L"%d кадр без завершения"
           wchar_t rep_templ[512];
           this->get_lang_string(IDS_REPORT_TMPL1+(skip_count >1 ? 1:0),rep_templ,sizeof(rep_templ)/sizeof(wchar_t));
           wsprintfW(rep_text,rep_templ,skip_count,skip_count >1 ? L"ов":L"");
           report_error(rep_text,rxb.begin(),rxb.get_data_len());
          }

          beg_ptr++;
          DWORD frame_len = end_ptr-beg_ptr;
          success_recv_bytes+= (2+frame_len);
          char temp[1024];
          udp_proto_deconvert(rxb.begin()+beg_ptr,frame_len,temp,sizeof(temp),&frame_len);
          lpudp_dev_rxpacket packet = (lpudp_dev_rxpacket)temp;
           //TODO далее использовать для обработки кадров
          DWORD kadrs_count = (packet->packet_len-(sizeof(BYTE)))/sizeof(udp_dev_kadr);
          check_packet_num((DWORD)packet->packet_num);
          lpudp_dev_kadr    kadr = packet->kadrs;
          last_rx_time      = GetTickCount();
          for(int i = 0;i<(int)kadrs_count;i++)
           {
            if(kadr->chanel<255  )
            {
             handle_kadr(kadr);
             //Отправляем
            }
            else
             {
              handle_internal_kadr(kadr);
              //handle_kadr(kadr);
             }
            kadr++;
           }

          end_ptr++;
          int slen = KeRTL::MAX(0,int(rxb.get_data_len())-int(end_ptr));
          if(!slen)
          {
           rxb.move_to_begin(end_ptr);
           end_ptr = 0;
          }
        }
        else
        {
         //Нет завершения
         rxb.move_to_begin(beg_ptr);
         if( rxb.get_free_size()==0)
             {
               error_recv_bytes+= rxb.get_data_len();
               //L"Переполнение приёмного буфера"
               wchar_t rep_templ[512];
               get_lang_string(IDS_REPORT_TMPL3,rep_templ,sizeof(rep_templ)/sizeof(wchar_t));
               report_error(rep_templ,rxb.begin(),rxb.get_data_len());
               rxb.move_to_begin(rxb.get_data_len());
             }
             beg_ptr = -1;
        }
     }
     else
     {
       error_recv_bytes+= rxb.get_data_len();
       //L"Завершение пакета без начала": L"Набор байт"
       wchar_t rep_templ[512];
       get_lang_string(rxb.find(0,0x3B)<(DWORD)-1 ? IDS_REPORT_TMPL4:IDS_REPORT_TMPL5,rep_templ,sizeof(rep_templ)/sizeof(wchar_t));
       report_error( rep_templ ,rxb.begin(),rxb.get_data_len());
       rxb.move_to_begin(rxb.get_data_len());
     }
     }while(beg_ptr<DWORD(-1) && rxb.get_data_len());
     //ZeroMemory(rxb.end(),rxb.get_free_size());
  }


  void     __fastcall TGranitUdpIo::handle_kadr(lpudp_dev_kadr kadr)
  {
    BYTE       _dt[2048];
    LPMIO_REQUEST mior = (LPMIO_REQUEST)_dt;
    ZeroMemory(mior,sizeof(*mior));
    mior->data_size = sizeof(_dt)-sizeof(*mior)+sizeof(DWORD);
    mior->channel  = kadr->chanel;
    mior->data_len = kadr->data_len;
    memcpy(mior->data,kadr->data,kadr->data_len);
   if(call_owner(CMDIO_READ_READY,(LPARAM)mior,MIO_REQUEST_LEN(mior)))
     {
      rd_queue.PutIntoQueue(mior,MIO_REQUEST_LEN(mior));
      if(rd_queue.QueueCount()>512)
          rd_queue.DropFirst();
     };

      if(kadr->data_type ==  GRUDP_DT_GRANIT_CONNECT)
         {
          wchar_t rep_text[MAX_PATH];
          wsprintfW(rep_text,(wchar_t*)_dt,(DWORD)kadr->chanel);
          get_lang_string(IDS_REPORT_TMPL12,(wchar_t*)_dt,sizeof(_dt)/sizeof(wchar_t));
         }

  }

  void     __fastcall TGranitUdpIo::handle_internal_kadr(lpudp_dev_kadr kadr)
  {
    if(kadr->data_type != GRUDP_DT_FILLER)
    {
     wchar_t text[MAX_PATH];
     switch( kadr->data_type)
     {
      case  GRUDP_DT_RESTART       : wsprintfW(text,L"Перезапуск");break;
      case  GRUDP_DT_BADCOMMAND    : wsprintfW(text,L"Недопустимая команда");break;
      case  GRUDP_DT_BADCHANNEL    : wsprintfW(text,L"Ошибка номера канала %d",(DWORD)kadr->data[0]);break;
      case  GRUDP_DT_INTERNAL_ERROR: wsprintfW(text,L"Внутренняя ошибка контроллера");break;
      case  GRUDP_DT_SYNCERR       : wsprintfW(text,L"Ошибка синхронизации пакет № %d", (DWORD)kadr->data[0]);break;
      default                      : wsprintfW(text,L"Неизвестный тип данных %X",(DWORD)kadr->data_type);
                                      handle_kadr(kadr);
                                      break;
     }
     this->report_error(text,kadr->data,kadr->data_len);
    }
     else
     handle_kadr(kadr);
  }

  void    __fastcall TGranitUdpIo::check_packet_num(DWORD new_num)
  {
    if((new_num  && (new_num - this->last_packet_num)>1) || (!new_num && last_packet_num!=255))
    {
     wchar_t rep_text[1024];
     //"пропуск принятого пакета %03d %03d"
     wchar_t rep_templ[512];
     get_lang_string(IDS_REPORT_TMPL6,rep_templ,sizeof(rep_templ)/sizeof(wchar_t));
     wsprintfW(rep_text,rep_templ,last_packet_num,new_num);
     report_error(rep_text,0,0);
    }
    last_packet_num = new_num ;
  }





  bool     __fastcall TGranitUdpIo::prepare_tx_packet(udp_dev_txpacket & tx_packet)
  {
   bool ret = false;
   ZeroMemory(&tx_packet,sizeof (tx_packet));
   tx_packet.packet_len = sizeof(tx_packet)-sizeof(tx_packet.packet_len);
   int data_len;
   DWORD    old_ch = -1;
   DWORD    i;
   //Вытащить из очередей первый кадр и запомнить номер канала
   for(  i = 0;i<4 && !ret; i++)
   {
     if(wr_queue[i].GetFromQueue(tx_packet.kadrs,sizeof(tx_packet.kadrs[0]),&data_len))
       {old_ch = i;ret = true;}

   }

   if(ret)
   {
    //здесь заполняем 2-ой кадр с номером != old_ch
    tx_packet.kadrs[1].chanel    = 255;
    tx_packet.kadrs[1].data_type = GRUDP_DT_FILLER;//0x0E;Заполнитель
    tx_packet.kadrs[1].data_len  = 1;
   for( i = 0;i<4; i++)
   {
      if(i!=old_ch && wr_queue[i].GetFromQueue(tx_packet.kadrs+1,sizeof(tx_packet.kadrs[1]),&data_len))
       break;
   }
   }
   return ret;
  }

  void     __fastcall TGranitUdpIo::do_tx_udpdata()
  {
    if(is_ready(true))
    {
     udp_dev_txpacket   tx_packet;
     if( prepare_tx_packet(tx_packet))
      {
      if(device_send(&tx_packet))
         last_tx_time = GetTickCount();
         else
         socket_destroy();
      }
      else
      tx_timer.StopTimer();
    }
    else
    {
     tx_timer.StopTimer();
     if(!socket) socket_create();
    }
  }

  DWORD __fastcall TGranitUdpIo::device_send(lpudp_dev_txpacket packet)
  {
     BYTE buffer[UDP_TX_RAWDATA_SIZE];
     ZeroMemory(buffer,sizeof(buffer));
     *buffer = GRUDPIO_RAW_FLAG_BEGIN;
     DWORD out_len;
     packet->packet_num = tx_packet_counter++;
     udp_proto_convert((LPBYTE)packet,sizeof(*packet),buffer+1,sizeof(buffer)-2,&out_len);
     *(buffer+sizeof(buffer)-1) = GRUDPIO_RAW_FLAG_END;
     if(socket)
     {
         #ifdef _DEBUG
         DWORD ticks = GetTickCount();
         //memset(buffer,0x0F,sizeof(buffer));
         int send_len = socket->SendTo(buffer,sizeof(buffer),0,host_addr(),sizeof(host_addr));
         ticks= GetTickCount()-ticks;
         Sleep(0);
         //Sleep(100);
         #else
         int send_len = socket->SendTo(buffer,sizeof(buffer),0,host_addr(),sizeof(host_addr));
         #endif
         return  send_len == (int)sizeof(buffer);
     }
     else
     return 0;
  }

  void      __fastcall TGranitUdpIo::device_reset()
  {
     udp_dev_txpacket   tx_packet;
     ZeroMemory(&tx_packet,sizeof (tx_packet));
     tx_packet.packet_len = sizeof(tx_packet)-sizeof(tx_packet.packet_len);
     tx_packet.kadrs[0].chanel    = 255;
     tx_packet.kadrs[0].data_len  = 5;
     tx_packet.kadrs[0].data_type = GRUDP_DT_RESET;

     tx_packet.kadrs[1].chanel    = 255;
     tx_packet.kadrs[1].data_len  = 5;
     tx_packet.kadrs[1].data_type = GRUDP_DT_FILLER; //Сброс устройства
     device_send(&tx_packet);
     last_rx_time = GetTickCount();
     //OutputDebugString("DeviceReset");
  }

 #define ERR_INPUT   (-1)
 #define ERR_OUTSIZE (-2)

 #pragma pack(push,1)
 union W_B
 {
  WORD  w;
  struct {BYTE lo,hi;};
 };
 #pragma pack(pop)


 DWORD __fastcall TGranitUdpIo::udp_proto_convert(LPBYTE in,DWORD in_len,LPBYTE out,DWORD out_size,LPDWORD out_bytes)
 {
  DWORD ret = 0;
  if(in && in_len)
   {
    DWORD out_len = in_len<<1;
    if(out && out_len<=out_size)
    {
     if(out_bytes) *out_bytes = out_len;
     LPBYTE end = in+in_len;
     W_B *  out_ptr = (W_B*)out;
     while(in<end)
     {
      out_ptr->lo = ((*in)&0xF0)>>4;
      out_ptr->hi = (*in)&0x0F;
      out_ptr++;
      in++;
     }
    }
    else
    ret = ERROR_INSUFFICIENT_BUFFER;
   }
   else
   ret = ERROR_INVALID_PARAMETER;
  return ret;
 }


 DWORD __fastcall TGranitUdpIo::udp_proto_deconvert(LPBYTE in,DWORD in_len,LPBYTE out,DWORD out_size,LPDWORD out_bytes)
 {
  DWORD ret = 0;
  if(in && in_len && !(in_len&1))
  {
   DWORD out_len = in_len>>1;
   if(out_len<=out_size)
   {
    if(out_bytes) *out_bytes = out_len;
    W_B * ptr = (W_B*)in,*end = ptr+out_len;
    while(ptr<end)
    {
     BYTE res  = ptr->lo&0x0F;
     res<<=4;
     res |= ptr->hi&0x0F;
     *out = res;
     out++;
     ptr++;
    }
   }
   else
    ret = ERROR_INSUFFICIENT_BUFFER;
  }
  else
  ret = ERROR_INVALID_PARAMETER;//Ошибка входных данных
  return ret;
 }


void      __fastcall TGranitUdpIo::on_timer()
{
 /*Таймер*/
 TLockHelper l(locker);
 DWORD rx_bytes,tx_bytes,events;
 wchar_t buffer[1024];
 if(socket && is_connected)
   {
         if(enable_reset && GetTickCount()-last_rx_time > 60000)
           {
            device_reset();
            get_lang_string(IDS_INACTIVITY_RESET,buffer,sizeof(buffer)/sizeof(wchar_t));
            report(report_id,REPORT_WARNING_TYPE,buffer);
           }
   }
   else
    {
     if(!socket)
        socket_create();
    }
}

void    __fastcall  TGranitUdpIo::tx_watchdog()
{
 if(this->is_watchdog_enabled())
 {
   if((GetTickCount()-this->last_tx_time) > 1000)
     {
        if(this->is_ready(true))
        {
          //Для подтверждения работоспособности канала
          udp_dev_txpacket   tx_packet;

          tx_packet.packet_len = sizeof(tx_packet)-sizeof(tx_packet.packet_len);
          tx_packet.kadrs[1].chanel    = 255;
          tx_packet.kadrs[1].data_type = GRUDP_DT_FILLER;//0x0E;Заполнитель
          tx_packet.kadrs[1].data_len  = 1;
          tx_packet.kadrs[0].chanel    = 255;
          tx_packet.kadrs[0].data_type = GRUDP_DT_FILLER;//0x0E;Заполнитель
          tx_packet.kadrs[0].data_len  = 1;
          device_send(&tx_packet);
       }
   }
 }
}


  DWORD   __fastcall TGranitUdpIo::report_error      (wchar_t * text,LPVOID data,DWORD data_sz,DWORD type)
  {
   TReportMes * mes = (TReportMes *) new BYTE [sizeof(TReportMes)+data_sz];
   if(mes)
   {
     DWORD text_len = (text ? lstrlenW(text):0)+lstrlenW(this->remote_addr)+64;
     mes->desc = new wchar_t[text_len];
     wsprintfW(const_cast<wchar_t*>(mes->desc),L"%s:%d - %s",remote_addr,remote_port,text ? text:L"");
     mes->data_size = data ? data_sz : 0;
     if(mes->data_size)
      memcpy(mes->data,data,mes->data_size);
     mes->log    = report_id;
     mes->type   = type;
     mes->source = _handle;
     LRESULT ret;
     call_gkhandle_bynameW(REPORTER_NAME_W,RCM_MESSAGE_EVENT,(LPARAM)mes,0,&ret);
    delete [] mes->desc;
    delete [] (LPBYTE)mes;
   }
   return 0;
  }



