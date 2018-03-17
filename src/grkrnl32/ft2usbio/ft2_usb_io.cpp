//---------------------------------------------------------------------------
#pragma hdrstop
#include "ft2usbio.hpp"
#include <stdio.h>

  TUsbIo::TUsbIo(wchar_t * name):TGKIOBase(name),tx_timer(false)
  {
     lock_param = GKHB_AUTO_LOCK_OFF;
     *device_name = 0;
     fthDevice = NULL;
     data_type = FT2USB_DT_GRANIT; 
     //L"CЦ-Гранит Ввод/вывод"
     wchar_t rep_templ[512];
     get_lang_string(IDS_REPORT_TITLE,rep_templ,sizeof(rep_templ)/sizeof(wchar_t));
     report_id            =   report_reg_event_type(L"GRSC_USB_IOMODULE",rep_templ);
     error_open_log_write = FALSE;
     
  }

  DWORD   __fastcall  TUsbIo::OnException(DWORD ExceptionCode,LPEXCEPTION_POINTERS exp)
  {
   DWORD  ret = TGKThread::OnException(ExceptionCode, exp);
   return ret;
  }



  LRESULT __fastcall TUsbIo::enum_devices(LPMIO_ENUM_DEVICES ed)
  {
    LRESULT ret = GKH_RET_ERROR;
    char dev_name[MAX_PATH];
    if(FT_OK == FT_ListDevices((LPVOID)ed->index,dev_name,FT_LIST_BY_INDEX|FT_OPEN_BY_SERIAL_NUMBER))
    {
     KeRTL::Ansi2Unicode(ed->dev_name,dev_name);
     ret = GKH_RET_SUCCESS;
    }
    return ret;
  }


  DWORD   __fastcall TUsbIo::get_config_data(DWORD mask,LPVOID buf,DWORD bsz)
  {
    lock();
    DWORD len;
    if(buf && (int)bsz>=(lstrlenW(device_name)+io_speed ? 10:0))
      {
       len = wsprintfW((wchar_t*)buf,L"%s",device_name);
       if(io_speed)
          wsprintfW(((wchar_t*)buf)+len,L":%d",io_speed);
      }
    unlock();
    return GKH_RET_SUCCESS;  
  }

  bool    __fastcall TUsbIo::set_config_data(DWORD mask,LPVOID buf,DWORD bsz)
  {
   lock();
   wchar_t text[MAX_PATH];
   safe_strcpy(text,(wchar_t*)buf);
   wchar_t * speed = wcschr(text,':');
   if(speed)
     {
      *speed++ = 0;
      io_speed = _wtoi(speed);
     }
   safe_strcpy(device_name,text);
   unlock();
   return true;
  }

  bool    __fastcall TUsbIo::compare_config_data(DWORD mask,LPVOID buf,DWORD bsz,LPDWORD ch_mask,BOOL * restart)
  {
    lock();
    if(lstrcmpiW(device_name,(wchar_t*)buf))
    {
     if(ch_mask)
        *ch_mask|=CCFL_MODULE_CONFIG;
     if(restart)
        *restart = TRUE;
    }
    unlock();
    return true;
  }

  int      __fastcall TUsbIo::Execute()
  {
   DWORD wr;
   char debug_str[MAX_PATH];
   rxb.set_size(FT2USB_RX_RAWDATA_SIZE<<4);
   open_device();
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
       case WR_WRTIMER: //Событие не пустая очередь на приём
             do_tx_usbdata();
             break;
       case WR_USBRX: /*Завершилось чтение*/
              handle_usb_read();
             break;
       case WAIT_TIMEOUT:
                          //OutputDebugString("USB: WaitTimeout");
                          on_timer();
                          break;
      }
       tx_watchdog();
     }while(wr && !CheckTerminateRequest());
   return 0;
  }

  void     __fastcall TUsbIo::handle_usb_read()
  {
   /*Чтение данных из  USB */
   DWORD      rd_len = 0;
   FT_STATUS  ft_status;
   do{
       rd_len = 0;
       ft_status = FT_GetQueueStatus(fthDevice,&rd_len);
       if(ft_status == FT_OK && rd_len)
       {
        BYTE tmp[4096];
        rd_len = KeRTL::MIN(rd_len,(DWORD)sizeof(tmp));
        ft_status = FT_Read(fthDevice,tmp,rd_len,&rd_len);

        total_recv_bytes +=rd_len;
        //this->report_error(L"Приняты байты из USB",tmp,rd_len,REPORT_SUCCESS_TYPE);
        rxb.add(tmp,rd_len);
        handle_rx(rxb);
       }
     }while(ft_status == FT_OK && rd_len && !CheckTerminateRequest());
  }

  void     __fastcall TUsbIo::handle_rx(rx_buffer & rxb)
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
          usb_proto_deconvert(rxb.begin()+beg_ptr,frame_len,temp,sizeof(temp),&frame_len);
          lpusb_dev_rxpacket packet = (lpusb_dev_rxpacket)temp;
           //TODO далее использовать для обработки кадров
          DWORD kadrs_count = (packet->packet_len-(sizeof(BYTE)))/sizeof(usb_dev_kadr);
          check_packet_num((DWORD)packet->packet_num);
          lpusb_dev_kadr    kadr = packet->kadrs;
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

  void     __fastcall TUsbIo::handle_kadr(lpusb_dev_kadr kadr)
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

      if(kadr->data_type ==  FT2USB_DT_GRANIT_CONNECT)
         {
          wchar_t rep_text[MAX_PATH];
          wsprintfW(rep_text,(wchar_t*)_dt,(DWORD)kadr->chanel);
          get_lang_string(IDS_REPORT_TMPL12,(wchar_t*)_dt,sizeof(_dt)/sizeof(wchar_t));
         }

  }

  void     __fastcall TUsbIo::handle_internal_kadr(lpusb_dev_kadr kadr)
  {
    if(kadr->data_type != FT2USB_DT_FILLER)
    {
     wchar_t text[MAX_PATH];
     switch( kadr->data_type)
     {
      case  FT2USB_DT_RESTART       : wsprintfW(text,L"Перезапуск");break;
      case  FT2USB_DT_BADCOMMAND    : wsprintfW(text,L"Недопустимая команда");break;
      case  FT2USB_DT_BADCHANNEL    : wsprintfW(text,L"Ошибка номера канала %d",(DWORD)kadr->data[0]);break;
      case  FT2USB_DT_INTERNAL_ERROR: wsprintfW(text,L"Внутренняя ошибка контроллера");break;
      case  FT2USB_DT_SYNCERR       : wsprintfW(text,L"Ошибка синхронизации пакет № %d", (DWORD)kadr->data[0]);break;
      default                       : wsprintfW(text,L"Неизвестный тип данных %X",(DWORD)kadr->data_type);
                                      handle_kadr(kadr);
                                      break;
     }
     this->report_error(text,kadr->data,kadr->data_len);
    }
     else
     handle_kadr(kadr);
  }

  void    __fastcall TUsbIo::check_packet_num(DWORD new_num)
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

  bool     __fastcall TUsbIo::is_ready(bool tx)
  {
   bool ret = false;
   DWORD rx_bytes,tx_bytes,events;
   if(IsValidHandle(fthDevice) )
   {
    if(FT_OK==FT_GetStatus(fthDevice,&rx_bytes,&tx_bytes,&events))
    {
    if(tx)
     return   (tx_bytes == 0) ? true:false;
     return   (rx_bytes )     ? true:false;
    }
    else
    on_timer();
    
   }
   return ret;
  }

  DWORD    __fastcall TUsbIo::queue_count()
  {
    DWORD ret = 0;
    for(int i = 0;i<4;i++)
       ret+=wr_queue[i].QueueCount();
    return ret;
  }

  bool     __fastcall TUsbIo::prepare_tx_packet(usb_dev_txpacket & tx_packet)
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
    tx_packet.kadrs[1].data_type = FT2USB_DT_FILLER;//0x0E;Заполнитель
    tx_packet.kadrs[1].data_len  = 1;
   for( i = 0;i<4; i++)
   {
      if(i!=old_ch && wr_queue[i].GetFromQueue(tx_packet.kadrs+1,sizeof(tx_packet.kadrs[1]),&data_len))
       break;
   }
   }
   return ret;
  }

  void     __fastcall TUsbIo::do_tx_usbdata()
  {
    if(is_ready(true))
    {
     usb_dev_txpacket   tx_packet;
     if( prepare_tx_packet(tx_packet))
      {
      device_send(&tx_packet);
      last_tx_time = GetTickCount();
      }
      else
      tx_timer.StopTimer();
    }
    else
    tx_timer.StopTimer();
  }

  DWORD __fastcall TUsbIo::device_send(lpusb_dev_txpacket packet)
  {
     BYTE buffer[FT2USB_TX_RAWDATA_SIZE];
     ZeroMemory(buffer,sizeof(buffer));
     *buffer = FT2USBIO_RAW_FLAG_BEGIN;
     DWORD out_len;
     packet->packet_num = tx_packet_counter++;
     this->usb_proto_convert((LPBYTE)packet,sizeof(*packet),buffer+1,sizeof(buffer)-2,&out_len);
     *(buffer+sizeof(buffer)-1) = FT2USBIO_RAW_FLAG_END;
     #ifdef _DEBUG
     DWORD ticks = GetTickCount();
     //memset(buffer,0x0F,sizeof(buffer));
     FT_STATUS   ft_status = FT_Write(fthDevice,buffer,sizeof(buffer),&out_len);
     ticks= GetTickCount()-ticks;
     Sleep(0);
     //Sleep(100);
     #else
     FT_STATUS   ft_status = FT_Write(fthDevice,buffer,sizeof(buffer),&out_len);
     #endif
     return ft_status == FT_OK ? sizeof(*packet):0;
  }

  void      __fastcall TUsbIo::device_reset()
  {
     usb_dev_txpacket   tx_packet;
     ZeroMemory(&tx_packet,sizeof (tx_packet));
     tx_packet.packet_len = sizeof(tx_packet)-sizeof(tx_packet.packet_len);
     tx_packet.kadrs[0].chanel    = 255;
     tx_packet.kadrs[0].data_len  = 5;
     tx_packet.kadrs[0].data_type = FT2USB_DT_RESET;

     tx_packet.kadrs[1].chanel    = 255;
     tx_packet.kadrs[1].data_len  = 5;
     tx_packet.kadrs[1].data_type = FT2USB_DT_FILLER; //Сброс устройства
     device_send(&tx_packet);
     last_rx_time = GetTickCount();
     //OutputDebugString("DeviceReset");

  }

  DWORD    __fastcall TUsbIo::start(DWORD reason,LPARAM p)
  {
   DWORD ret = GKH_RET_ERROR;
   error_open_log_write = FALSE;
   if(*device_name)
   {
    /*Запуск модуля ввода вывода*/
    stop_event  = CreateEvent(NULL,FALSE,FALSE,NULL);
    wr_event    = CreateEvent(NULL,FALSE,FALSE,NULL);

    fthDevice   = NULL;
    if(stop_event )
    {
      synchro_set.Clear();
      synchro_set+=stop_event;
      synchro_set+=  wr_event;
      synchro_set+=tx_timer;

      if(Start(32768))
         {
          set_module_state(MODULE_STATE_RUNNING,true);
          ret = GKH_RET_SUCCESS;
         }
    }

   }
   return ret;
  }

  DWORD    __fastcall TUsbIo::stop(DWORD reason)
  {
     DWORD ret ;
     SetEvent(stop_event);
     TerminateAndWait(2000,true);
     close_device();
     CloseHandle(wr_event);
     CloseHandle(stop_event);
     synchro_set.Clear();
     set_module_state(MODULE_STATE_RUNNING,false);
     ret = GKH_RET_SUCCESS;
     return ret;
  }

  LRESULT  __fastcall TUsbIo::read      (LPMIO_REQUEST io_req)
  {
    //Чтение
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

  LRESULT  __fastcall TUsbIo::write     (LPMIO_REQUEST io_req)
  {
    //Запись
    LRESULT ret = GKH_RET_ERROR;
    usb_dev_kadr kadr;
    bzero(&kadr,sizeof(kadr));
    DWORD   channel = io_req->channel;
    if(fthDevice)
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


bool     __fastcall TUsbIo::open_device()
{
  bool ret = false;
  char  dev_name[MAX_PATH];
  char  log_text[MAX_PATH<<1];
  lock();
  Unicode2Ansi(dev_name,device_name);
  fthDevice = NULL;
  FT_STATUS ft_stat = FT_OpenEx(dev_name,FT_OPEN_BY_SERIAL_NUMBER,&fthDevice);
  unlock();
  if(FT_OK==ft_stat)
  {
   lock();
   tx_packet_counter    = 0;
   total_recv_bytes     = 0;
   success_recv_bytes   = 0;
   error_recv_bytes     = 0;
   last_rx_time = last_tx_time   = GetTickCount();

   rx_event    = CreateEvent(NULL,FALSE,FALSE,NULL);
   synchro_set+=rx_event;
   unlock();
   Sleep(100);
   lock();
   FT_ResetDevice(fthDevice);
   //FT_ResetPort(fthDevice);  //Вешает машину
   unlock();
   Sleep(100);
   lock();
   FT_SetResetPipeRetryCount(fthDevice,150);
   //FT_SetFlowControl(fthDevice,FT_FLOW_NONE,0,0);
   FT_Purge(fthDevice,FT_PURGE_RX|FT_PURGE_TX);
   FT_SetTimeouts(fthDevice,200,200);
   FTCOMSTAT fcs;DWORD com_err;
   ft_stat = FT_W32_ClearCommError(fthDevice,&com_err,&fcs);
   ft_stat = FT_SetEventNotification(fthDevice,FT_EVENT_RXCHAR,rx_event);
   ft_stat = FT_SetDataCharacteristics(fthDevice,FT_BITS_8,FT_STOP_BITS_1,FT_PARITY_NONE);
   if(io_speed)
      ft_stat = FT_SetBaudRate(fthDevice,io_speed);
   //FT_SetUSBTimeout(
   //device_reset();
   last_packet_num = 0;
   unlock();
   Sleep(200);
   ret = true;
   call_owner(CMDIO_ERROR,0,-1);
  }
  else
  {
    lock();
    FT_Close(fthDevice);
    fthDevice = NULL;
    unlock();
    call_owner(CMDIO_ERROR,  IO_ERROR_OPEN,-1);
  }
  //"USB устройство %s - "
  char rep_templ[512];
  get_lang_string(IDS_REPORT_TMPL7,rep_templ,sizeof(rep_templ));
  int len = sprintf(log_text,rep_templ,dev_name);
  //ret == false ? " не открыто %s" :" открыто хендл = %08X"
  get_lang_string(IDS_REPORT_TMPL8 + int(ret),rep_templ,sizeof(rep_templ));
  if(ret)
      {
       len+=sprintf(log_text+len,rep_templ,(DWORD)fthDevice);
      }
     else
      {
       if(!error_open_log_write)
          {

           char ft_err[512];
           get_lang_string(IDS_FTD2_ERROR+ft_stat,ft_err   ,sizeof(ft_err   ));
           len+=sprintf(log_text+len,rep_templ,ft_err);
          }
          else
          len = 0;
          error_open_log_write = TRUE;
      }
  if(len)
     report (report_id,ft_stat ? REPORT_ERROR_TYPE:REPORT_SUCCESS_TYPE,log_text);
  if(is_ready(false))
     handle_usb_read();

  return ret;
}

bool     __fastcall TUsbIo::close_device()
{
 bool ret = false;
 char log_text[MAX_PATH];
 //lock();
 if(IsValidHandle(fthDevice))
 {
  FT_SetEventNotification(fthDevice,FT_EVENT_RXCHAR,0);
  Sleep(10);
  FT_ResetDevice(fthDevice);
  Sleep(100);
  FT_Close(fthDevice);
  Sleep(100);
  synchro_set -=rx_event;
  CloseHandle(rx_event);

  wr_queue[0].DropData();
  wr_queue[1].DropData();
  wr_queue[2].DropData();
  wr_queue[3].DropData();
  rd_queue.DropData();
  ret = true;
  //"USB устройство хендл %08X - закрыто"
  char rep_templ[512];
  get_lang_string(IDS_REPORT_TMPL10,rep_templ,sizeof(rep_templ));
  sprintf(log_text,rep_templ,(DWORD)fthDevice);
  rx_event   =  fthDevice = NULL;
  error_open_log_write = FALSE;
  ResetEvent(wr_event);
  tx_timer.StopTimer();
 }
 //unlock();
 call_owner(CMDIO_ERROR,  IO_ERROR_CLOSE,0);
 if(ret)
   report(this->report_id,REPORT_SUCCESS_TYPE,log_text);


 return ret;
}

void    __fastcall  TUsbIo::tx_watchdog()
{
 if(this->is_watchdog_enabled())
 {
   if((GetTickCount()-this->last_tx_time) > 1000)
     {
        if(this->is_ready(true))
        {
          //Для подтверждения работоспособности канала
          usb_dev_txpacket   tx_packet;

          tx_packet.packet_len = sizeof(tx_packet)-sizeof(tx_packet.packet_len);
          tx_packet.kadrs[1].chanel    = 255;
          tx_packet.kadrs[1].data_type = FT2USB_DT_FILLER;//0x0E;Заполнитель
          tx_packet.kadrs[1].data_len  = 1;
          tx_packet.kadrs[0].chanel    = 255;
          tx_packet.kadrs[0].data_type = FT2USB_DT_FILLER;//0x0E;Заполнитель
          tx_packet.kadrs[0].data_len  = 1;
          device_send(&tx_packet);
       }
   }

 }
}

void      __fastcall TUsbIo::on_timer()
{
 /*Таймер*/
 lock();
 DWORD rx_bytes,tx_bytes,events;
 FT_STATUS stat;
 wchar_t buffer[1024];
 if(IsValidHandle(fthDevice))
   {
     stat = FT_GetStatus(fthDevice,&rx_bytes,&tx_bytes,&events);
     if(stat)
        {
         wchar_t rep_templ[512];
         wchar_t ft_err   [512];
         //L"FT_GetStatus вернуло ошибку %04X переоткрытие устройства"
         get_lang_string(IDS_REPORT_TMPL11,rep_templ,sizeof(rep_templ)/sizeof(wchar_t));
         get_lang_string(IDS_FTD2_ERROR+stat,ft_err   ,sizeof(ft_err   )/sizeof(wchar_t));
         wsprintfW(buffer,rep_templ,stat,ft_err);
         report(this->report_id,REPORT_ERROR_TYPE,buffer);
         close_device();
        }
        else
        {
         if(enable_reset && GetTickCount()-last_rx_time > 60000)
           {
            device_reset();
            get_lang_string(IDS_INACTIVITY_RESET,buffer,sizeof(buffer)/sizeof(wchar_t));
            report(report_id,REPORT_WARNING_TYPE,buffer);

           }
        }
   }
   else
     open_device();
 unlock();
}


 #pragma pack(push,1)
 union W_B
 {
  WORD  w;
  struct {BYTE lo,hi;};
 };
 #pragma pack(pop)

 #define ERR_INPUT   (-1)
 #define ERR_OUTSIZE (-2)
 
 DWORD __fastcall TUsbIo::usb_proto_convert(LPBYTE in,DWORD in_len,LPBYTE out,DWORD out_size,LPDWORD out_bytes)
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


 DWORD __fastcall TUsbIo::usb_proto_deconvert(LPBYTE in,DWORD in_len,LPBYTE out,DWORD out_size,LPDWORD out_bytes)
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


  DWORD   __fastcall TUsbIo::report_error      (wchar_t * text,LPVOID data,DWORD data_sz,DWORD type)
  {
   TReportMes * mes = (TReportMes *) new BYTE [sizeof(TReportMes)+data_sz];
   if(mes)
   {
     DWORD text_len = (text ? lstrlenW(text):0)+lstrlenW(this->device_name)+32;
     mes->desc = new wchar_t[text_len];
     wsprintfW(mes->desc,L"%s - %s",device_name,text ? text:L"");
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
  




LRESULT WINAPI module_main(DWORD cmd,LPARAM p1, LPARAM p2)
{
  LRESULT ret = 0;
       switch(cmd)
       {
        case GKME_CMD_GET_MODULETYPE:
			ret = MT_GRSCIO;break;
        case GKME_CMD_CREATE_HANDLE :
        {
            TUsbIo * mod = new TUsbIo((wchar_t*)p1);
            ret  = (LRESULT)(*(TGKHandleBased*)(mod))();
            if(!ret)
             delete mod;
        }
        break;
       }
       return ret;
}






