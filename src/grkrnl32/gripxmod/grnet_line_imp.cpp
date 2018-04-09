/*
   Реализация линии как клиента
*/

#pragma hdrstop
#include "gripxmod_main.hpp"
#include <stdlib.h>
#include <otd_proto.h>


void __fastcall grnet_line::begin_connection()
{
  work_mode = TRUE;
  host_addr = TIpxAddress(server_socket,true);
  GRNET_BEGIN_CONNECTION  bc;
  ZeroMemory(&bc,sizeof(bc));
  safe_strcpyn(bc.server_name,server_name,sizeof(bc.server_name));
  safe_strcpyn(bc.client_name,client_name,sizeof(bc.client_name));
  bc.ID           = -1;
  bc.client_level =  1; 
  tx_kadr_number  =  0;
  grnet_init_kadr(tx_kadr,CmConnect,tx_kadr_number,client_id,TRUE,&bc,sizeof(bc));
  ipx_socket.EnableBroadcast(true);
  set_client_state(-1,false);
  set_client_state(CLIENT_STATE_FIND_SERVER,true);
  this->ipx_socket.Receive(rx_kadr,1024,&rx_overlapped);
  do_send_kadr(tx_kadr);
  /*Подготвка  кадра начала соединения*/
}

 void    __fastcall grnet_line::connect_establish(LPGRNET_BEGIN_CONNECTION bc,TIpxAddress & remote_addr,BOOL _tu_enabled)
 {
    tu_enabled = _tu_enabled;
    host_addr = remote_addr;
    if(work_mode == GRNET_WM_AS_CLIENT)
    {
    if(bc->error_connection == 0)
    {
      client_id = bc->ID;
      ipx_socket.EnableBroadcast(false);
      set_client_state(-1,false);
      connect(true);
     }
    }

    if(work_mode == GRNET_WM_AS_SERVER)
    {
      bc->ID = client_id;
      bc->error_connection = 0;
      char buff[1024];
      LPGRNET_KADR kadr =(LPGRNET_KADR)buff; 
      grnet_init_kadr(kadr,CmConnect,0,client_id,FALSE,bc,sizeof(*bc));
      kadr->receipt = 1;
      grnet_protect(kadr);
      tx_kadr_number = 1;
      rx_kadr_number = 1;
      if(!is_connected())
      {
       safe_strcpyn(server_name,bc->server_name,sizeof(server_name)-1);
       safe_strcpyn(client_name,bc->client_name,sizeof(client_name)-1);
       set_client_state(-1,false);
       start();
       net_send((LPBYTE)kadr,grnet_kadr_len(kadr));
       tx_kadr->retry_num  = 0;
       connect(true);

      }
      else
      net_send((LPBYTE)kadr,grnet_kadr_len(kadr));
    }
 }




int      __fastcall grnet_line::net_send(LPBYTE data,DWORD len)
{
  /*Физическая передача данных*/

  WSABUF wb;
  wb.len = len;
  wb.buf = (char*)data;
  ipx_socket.GetSocketError();
  ipx_socket.SendTo(data,len,MSG_DONTROUTE,(sockaddr*)&host_addr,sizeof(host_addr));
  //ipx_socket.WSASendTo(&wb,1,&len,MSG_DONTROUTE,(sockaddr*)&host_addr,sizeof(host_addr),NULL/*&tx_overlapped*/,NULL);
  //Sleep(0);
  update_stats_tx(len,0,0);
  return len;

}

void     __fastcall grnet_line::do_send_kadr(LPGRNET_KADR kadr)
{
  /*Передача кадра*/
  DWORD ticks = GetTickCount();
  grnet_protect(kadr);
  DWORD len = grnet_kadr_len(kadr);
  net_send((LPBYTE)kadr,len);
  memcpy(tx_kadr,kadr,len);
  update_stats_tx(0,1,0);
  if(kadr->need_receipt)
     wait_receipt(true);
 /*Напоминание об отправке данных*/
 if(owner->need_notify(GRNET_NOTIFY_TXDATA))
  {
    tx_proto_notify->notify_code = GRNET_NCODE_TXDATA;
    tx_proto_notify->dw_size  = sizeof(NOTIFY_CODE)+len-sizeof(tx_proto_notify->notify_data);
    owner->notify_handles(tx_proto_notify,GRNET_NOTIFY_TXDATA);
  }
  #ifdef _DEBUG
//  ticks = GetTickCount()-ticks;
//  TRACE("DoSendKadr time %d ms",ticks)
  #endif
}

void    __fastcall grnet_line::repeat_send_kadr()
{
  //Повторная передача кадра

  wait_receipt(false);
  DWORD retry_limit = tx_retry_count;
  if(check_client_state(CLIENT_STATE_DISCONNECTING))
     retry_limit = KeRTL::MAX(retry_limit/2,(DWORD)3);
  if(rx_kadr->command == CmConnect) //При соединении  не больше 3 попыток
     retry_limit = KeRTL::MIN(retry_limit,(DWORD)3);   
  if(++tx_kadr->retry_num < (WORD)retry_limit)
    {
      update_stats_tx(0,0,1);
      grnet_protect(tx_kadr);
      net_send((LPBYTE)tx_kadr,grnet_kadr_len(tx_kadr));
      set_state(MODEM_LINE_STATE_TXREADY,false);
      wait_receipt(true);
    }
    else
     connect(false);
}

bool    __fastcall grnet_line::CheckTerminateRequest()
{
  //Проверка состояния савершения
  //Если не установлено соединение, то завершаемся сразу
  //В противном случае завершаемся при получении квитанции на коменду CmDisconnect
    return TThread::CheckTerminateRequest();
}

void    __fastcall grnet_line::Terminate()
{
  icp.PostStatus(-1,-1,NULL);

}

bool    __fastcall grnet_line::BeforeExecute()
{
  ZeroMemory(&rx_overlapped,sizeof(rx_overlapped));
  ZeroMemory(&tx_overlapped,sizeof(tx_overlapped));

  if(work_mode == GRNET_WM_AS_CLIENT)
     begin_connection();
     else
     ipx_socket.Receive(rx_kadr,1024,&rx_overlapped);
  return TGKThread::BeforeExecute();
}

int     __fastcall grnet_line::Execute()
{
 DWORD key,bytes;
 LPOVERLAPPED   ovrlp;
 do{
    key = -1;
    DWORD  time_out = tx_retry_delay ;
    if(!check_client_state(CLIENT_STATE_WAIT_RECEIPT))
       time_out = KeRTL::MAX((DWORD)2000,time_out*tx_retry_count);
       /*При отсутствии активности устанавливается таймаут для проверки связи*/
    if(icp.GetStatus(key,ovrlp,time_out,&bytes))
    {
     
     switch(key)
     {
      case -1           :  disconnect();break;
      case GRNET_RX_KEY:
                        if(ovrlp == &rx_overlapped)
                        {
                         do{
                            handle_receive(bytes);
                            if(ipx_socket.CanRead())
                               bytes = ipx_socket.Receive(rx_kadr,1024,NULL);
                               else
                               bytes = 0;
                           }while(bytes &&!CheckTerminateRequest());
                           ipx_socket.Receive(rx_kadr,1024,&rx_overlapped);
                        }break;
     case  GRNET_TX_KEY:
       {
        if(is_connected() && !check_client_state(CLIENT_STATE_WAIT_RECEIPT))
            {
             if(tx_queue.QueueCount()>1)
               //TRACE("TX_KEY queue count %d",tx_queue.QueueCount());
             send_next_kadr();
            } 

       }
       break;
     }
     if(get_wait_receipt_time()>tx_retry_delay)
        repeat_send_kadr();
    }
    else
     on_timeout();

   }while(!CheckTerminateRequest());
   connect(false);
   set_state(MODEM_LINE_STATE_WORKED,false);
   return 0;
}

void    __fastcall grnet_line::BeforeTerminate()
{
  TLockHelper l(locker);
  CancelIo((HANDLE)ipx_socket.GetSocket());
  destroy_socket();
  TGKThread::BeforeTerminate();
}


void    __fastcall grnet_line::on_timeout()
{
 if(check_client_state(CLIENT_STATE_WAIT_RECEIPT))
   {
     repeat_send_kadr();
   }
   else
   {
    if(this->is_connected())
        check_connect();
        else
        {
         if(work_mode==GRNET_WM_AS_CLIENT)
            {
             set_client_state(-1,false);
             create_socket();
             begin_connection();
            }
         }   
   }
}


void __fastcall grnet_line::check_connect()
{
 /*
  Отправка кадра проверки связи
 */
 GRNET_KADR kadr;
 grnet_init_kadr(&kadr,CmCheckConnect,next_tx_kadr_number(),client_id,TRUE,0,0);
 do_send_kadr(&kadr);
}

void __fastcall grnet_line::disconnect()
{
  //Функция рассоединения /*Дождаться завершения*/
 tx_queue.DropData();

 if(is_connected())
 {
  set_client_state(-1,false);
  set_client_state(CLIENT_STATE_DISCONNECTING,true);
  GRNET_KADR dc_kadr;
  grnet_init_kadr(&dc_kadr,CmDisconnect,next_tx_kadr_number(),client_id,TRUE,0,0);
  do_send_kadr(&dc_kadr);
 }
 else
   TThread::Terminate();
}


inline bool __fastcall is_new_kadr(WORD old_num,WORD new_num)
{
 return (old_num < new_num || (old_num == 0xFFFF && new_num<old_num)) ? true:false;
}

void   __fastcall  grnet_line::handle_receive(DWORD bytes)
{
  if(bytes)
  {
   rx_len+=bytes;
   update_stats_rx(bytes,0,0);

   DWORD errors = 0;
   if(grnet_check(rx_kadr))
    {
     /*Ввести проверку равенства адресов host_addr && ipx_socket.RemoteAddr*/
      if(rx_kadr->receipt)
         {
          handle_receipt(rx_kadr);
         }
         else
         {
          if(rx_kadr->need_receipt)
             receipt_kadr(rx_kadr);
          if(is_new_kadr(rx_kadr_number,rx_kadr->kadr_num))
             {
              rx_kadr_number = rx_kadr->kadr_num;
              update_stats_rx(0,1,0);
              handle_grnet_command(rx_kadr->command,rx_kadr->data,rx_kadr->data_len);
              if(rx_kadr->retry_num)
                 errors++;
             }
             else
             errors++;
            TModemBase * owner = get_owner();
            if(!check_client_state(CLIENT_STATE_DISCONNECTING) && owner && owner->need_notify(GRNET_NOTIFY_RXDATA)  )
              {
               rx_proto_notify->notify_code = errors ? GRNET_NCODE_RXDATA_ERROR:GRNET_NCODE_RXDATA;
               rx_proto_notify->dw_size  = sizeof(NOTIFY_CODE)+bytes-sizeof(rx_proto_notify->notify_data);
               owner->notify_handles(rx_proto_notify,GRNET_NOTIFY_RXDATA);
              }
         }
    }
    else
    {
     errors = 1;
     DebugBreak();
    } 
    update_stats_rx(0,0,errors);
  }
}


 void    __fastcall grnet_line::handle_receipt(LPGRNET_KADR kadr)
 {
   /*Обработка квитанции*/
   if(kadr->kadr_num == tx_kadr->kadr_num)
   {
    wait_receipt(false);
    switch(kadr->command)
    {
     case CmConnect   :if(kadr->data_len == sizeof(GRNET_BEGIN_CONNECTION))
                          connect_establish((LPGRNET_BEGIN_CONNECTION)rx_kadr->data,(TIpxAddress&)ipx_socket.GetRemoteAddress(),false);
                       break;
     case CmDisconnect:
                       connect_lost--;//Не было потери связи так как рассоединение нормальное
                       connect(false);
                       
                       break;
     default          :
                       break;
    }

    if(is_connected())
    {
     if(tx_queue.QueueCount())
        {
         send_next_kadr();
         //TRACE("SendNext kadr remain %d",tx_queue.QueueCount())
        }
       else
        {
         set_state(MODEM_LINE_STATE_TXREADY,true);
         //TRACE("Send Queue Empty %d",0)
        }
    }
   }
   else
   {
    //TRACE("mismatch kadr numbers %d - %d",(kadr->kadr_num,tx_kadr->kadr_num));
   }
 }

 void __fastcall grnet_line::send_next_kadr()
 {
     DWORD ticks = GetTickCount();
     tx_queue.GetFromQueue(tx_kadr,sizeof(tx_buffer)-sizeof(tx_proto_notify)+sizeof(tx_proto_notify->notify_data),(int*)&this->tx_len);
     tx_kadr->kadr_num = next_tx_kadr_number();
     tx_kadr->need_receipt = 1;

     /*if(tx_kadr->command == CmRetranslation && tx_queue.QueueCount()>256)
      {
       lp_gratony_kadr ak = (lp_gratony_kadr)tx_kadr->data;
       if(ak->InfoType ==tpTit || ak->InfoType ==tpTii)
          tx_kadr->need_receipt = 0;
      }*/
     tx_kadr->retry_num    = 0;
     tx_kadr->id           = client_id;
     grnet_protect(tx_kadr);
     do_send_kadr (tx_kadr);
     #ifdef _DEBUG
//     ticks = GetTickCount()-ticks;
//     TRACE("send_next_kadr time %d",ticks);
     #endif
 }

 void    __fastcall grnet_line::receipt_kadr(LPGRNET_KADR kadr)
 {
  GRNET_KADR rcpt;
  memcpy(&rcpt,kadr,sizeof(rcpt));
  rcpt.data_len = 0;
  rcpt.receipt = 1;
  grnet_protect(&rcpt);
  net_send((LPBYTE)&rcpt,grnet_kadr_len(&rcpt));
 }


 void    __fastcall grnet_line::handle_grnet_command(DWORD cmd,LPBYTE data,DWORD len)
 {

  lp_gratony_kadr kadr = (lp_gratony_kadr)data;
  switch(cmd)
  {
   case     CmDisconnect  :
                            connect_lost--;
                            connect(false);
                            break;
   case     CmCheckConnect: break; /*Проверка соединения*/
   case     CmQueryTU     : tu_query(kadr);break;
   case     CmBeginTU     : tu_begin(kadr);break;
   case     CmResetTU     : tu_reset(kadr);break;
   case     CmTuOn        : tu_on_more(kadr); break;
   case     CmTuOff       : tu_off_less(kadr);break;
   case     CmTrNorm      : break;
   case     CmTuCancel    : tu_cancel(kadr);break;
   case     CmEnableTU    :
   case     CmDisableTU   :
   case     CmErrorTU     :
   case     CmActivateTR  :
            break;
   case     CmQuery       :
               make_query(pu_number,-1,OTD_FA_ORDER|OTD_FA_ALL,-1,false);
               send_ready(-1);
               break;
   case     CmQueryCp     :
            make_query(pu_number,kadr->numCp,OTD_FA_ORDER|OTD_FA_ALL,-1,false);
            send_ready(kadr->numCp);
            break;
   case	    CmQueryFa     :
            make_query(pu_number,kadr->numCp,OTD_FA_ORDER|kadr->InfoType,-1,false);
            send_ready(kadr->numCp);
            break;
   case	    CmQueryGroup  :
            make_query(pu_number,kadr->numCp,OTD_FA_ORDER|kadr->InfoType,kadr->numGrp,false);
            send_ready(kadr->numCp);
            break;
   case     CmRetranslation:
            /*Передача данных*/
             handle_tlm_data((lp_gratony_kadr)data,len);
            break;
  }
 }

 void    __fastcall grnet_line::handle_tlm_diag(DWORD begin_cp,LPWORD diag_data,DWORD cp_count)
 {
   /*Обработка кадра диагностики*/
   TGRNetModule * mod = (TGRNetModule *)this->owner;
   BYTE buf[128];
   otd_proto op;
   op.dw_size = sizeof(op);
   otd_proto_format(buf,sizeof(buf),OTD_PROTO_PART_DIAG|OTD_PROTO_PART_KPK,0,0,0,0,0,0,&op);
   op.addr->pu   = pu_number;

   for(DWORD cp_num = begin_cp, i = 0;i<cp_count;i++,diag_data++,cp_num++)
   {
     op.addr->cp = cp_num;
     DWORD changes = diag->set_grdiag(cp_num,*diag_data);
     if(changes )
     {
      //изменилось состояние диагностики
      mod->diag_notify(cp_num,1);
     }
      grdiag_item gd_item ;
      diag->get_grdiag(cp_num,&gd_item);
      if((gd_item.state&GRDIAG_CP))
      {
       //КП неработоспособно
       op.addr->fa  = OTD_FA_ALL;
       op.addr->sb  = OTD_ADDR_MAXVALUE;
       *op.diag     = OTD_DIAG_CPRESPOND|OTD_DIAG_CPCONNECT;
       otd_proto_protect(&op,op.proto_size,OTD_DEF_POLINOM);
       queue_rxdata(FA_OTD,buf,op.proto_size,false);
      }
      else
      {

       if((gd_item.mask&(GRDIAG_TIT_MASK|GRDIAG_TS_MASK)) == 0) //Нет ни одной группы
       {
         /*Нет Групп*/
          op.addr->fa  = OTD_FA_ALL;
          op.addr->sb  = OTD_ADDR_MAXVALUE;
         *op.diag = diag->get_otd_diag(op.addr->cp,-1,-1);
          otd_proto_protect(&op,op.proto_size,OTD_DEF_POLINOM);
          queue_rxdata(FA_OTD,buf,op.proto_size,false);


       }
       else
       {
       //Бежим по изменившимся группам
       DWORD mask = 0x10001;
       DWORD   i    ;
       //ТС

       op.addr->fa = OTD_FA_DISCRETE;
       for(i = 0;i<8;i++)
       {

         if(gd_item.mask&mask)
         {
           op.addr->sb = i;
           *op.diag = diag->get_otd_diag(op.addr->cp,op.addr->fa,op.addr->sb);
           otd_proto_protect(&op,op.proto_size,OTD_DEF_POLINOM);
           queue_rxdata(FA_OTD,buf,op.proto_size,false);
         }
         mask<<=1;
       }
       //ТИТ
       op.addr->fa = OTD_FA_ANALOG;
       for(i = 0;i<7;i++)
       {
         if(gd_item.mask&mask)
         {
           op.addr->sb = i;
           *op.diag = this->diag->get_otd_diag(cp_num,OTD_FA_ANALOG,i);
           otd_proto_protect(&op,op.proto_size,OTD_DEF_POLINOM);
           queue_rxdata(FA_OTD,buf,op.proto_size,false);
         }
         mask<<=1;
       }
       }
      }

   }
 }

 void    __fastcall grnet_line::handle_tlm_data(lp_gratony_kadr kadr,DWORD len)
 {
  TGRNetModule * mod = (TGRNetModule *)this->owner;
  switch(kadr->InfoType)
  {
   case tpReady:
                  {
                   DWORD beg_cp    =  kadr->numCp;
                   DWORD cp_count  = (kadr->len - sizeof(*kadr)+sizeof(kadr->data)+sizeof(kadr->kadr_len))/sizeof(WORD);
                   handle_tlm_diag(beg_cp,(LPWORD)kadr->data,cp_count);
                  }break;

   // здесь при помещении принятых данных в очередь
   //ФА = -1 чтобы покозать, что данные в формате ATONY
   case tpTs :
              if(diag->mark_exist (kadr->numCp,tpTs,kadr->numGrp))
                 mod->diag_notify(kadr->numCp,1);
               queue_rxdata(-1,(LPBYTE)kadr,len,false);

              break;
   case tpTit:
               if(diag->mark_exist(kadr->numCp,tpTit,kadr->numGrp))
                 mod->diag_notify(kadr->numCp,1);
               queue_rxdata(-1,(LPBYTE)kadr,len,false);
               break;

  }

 }

void  __fastcall grnet_line::send_ready (DWORD cp,bool always)
{
    /*Отправка диакностики*/
    DWORD end_cp = cp;
    if(cp<MAX_CP_COUNT)
       end_cp++;
       else
       cp = 0,end_cp = MAX_CP_COUNT;

    BYTE  buffer[sizeof(gratony_kadr)+1];
    DWORD data_len = sizeof(buffer);
    ZeroMemory(buffer,sizeof(buffer));
    gratony_kadr & kadr = *(lp_gratony_kadr)buffer;
    kadr.kadr_len =
    kadr.len      = data_len-sizeof(kadr.kadr_len);
    kadr.InfoType = tpReady;
    kadr.numGrp   = -1;
    grdiag_item gdi;
    for(;cp < end_cp;cp++)
    {
    this->diag->get_grdiag(cp,&gdi);
    if(always || gdi.mask & GRDIAG_CP)
    {
    kadr.numCp    = cp;
    *((LPWORD)kadr.data) = gdi.state;
        kadr.channel  = get_number();
        send_data(CmRetranslation,buffer,data_len);
    }
   }
}




 
