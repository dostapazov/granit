#pragma hdrstop
#include "gripxmod_main.hpp"
#include <stdlib.h>
#include <otd_grancvt.h>





grnet_line::grnet_line(DWORD ln,GRNET_SETTINGS & gs,diag_map * _diag):modem_line(ln)
{
 tu_enabled = FALSE;
 diag    = _diag;
 pu_number     = gs.pu_number;
 work_mode     = gs.work_mode;
 client_socket = gs.client_socket;
 client_state  = 0;
 *client_name  = 0;
 if(work_mode)
    {
     client_id = -1;
     if(*gs.client_name == NULL)
       {
        char  cn[MAX_PATH];
        DWORD ln = sizeof(cn);
        GetComputerName(cn,&ln);
        if(ln) ln++;
        ln = KeRTL::MIN(ln,(DWORD)sizeof(client_name));
        cn[ln] = 0;
        safe_strcpyn(client_name,cn,ln);
       }
       else
       safe_strcpyn(client_name,gs.client_name,sizeof(client_name));
    }
    else
    client_id = ln+256;

 safe_strcpyn(server_name,gs.server_name,sizeof(server_name));
 server_socket = gs.server_socket;
 set_tx_retry_param(gs.tx_retry_count,gs.tx_retry_delay);

 rx_proto_notify = (LPNOTIFY_CODE) rx_buffer;
 rx_proto_notify->dw_size  = sizeof(rx_buffer);
 rx_proto_notify->notify_code = GRNET_NCODE_RXDATA;
 rx_kadr = (LPGRNET_KADR)rx_proto_notify->notify_data;

 tx_proto_notify =  (LPNOTIFY_CODE) tx_buffer;
 tx_proto_notify->dw_size  = sizeof(tx_buffer);
 tx_proto_notify->notify_code = GRNET_NCODE_TXDATA;
 tx_kadr = (LPGRNET_KADR)tx_proto_notify->notify_data;

 rx_len  = 0;
 tx_len  = 0;
 rx_kadr_number = tx_kadr_number = 0;
 tx_speed = rx_speed = 10000000;
}



void    __fastcall grnet_line::release()
{
 stop();
 tx_queue.DropData();
 destroy_socket();
 modem_line::release();
}

DWORD   __fastcall grnet_line::get_client_state()
{
 TLockHelper l(locker);
 DWORD ret = client_state;
 return ret;
}

bool    __fastcall grnet_line::check_client_state(DWORD state,bool exact)
{
  TLockHelper l(locker);
  DWORD mask = (client_state&state);
  bool ret =  exact ?  ((mask == state) ? true:false):((mask) ? true:false);
  return ret;
}

DWORD   __fastcall grnet_line::set_client_state(DWORD state,bool set,bool notify )
{
      this->lock();
      DWORD new_state = set ? (client_state|state):(client_state&~state);
      DWORD changes   = new_state^client_state;
      client_state    = new_state;
      this->unlock();
      if(changes)//Изменилось ли состояние
      {
        if(changes&CLIENT_STATE_WAIT_RECEIPT)
           {
            if(!set)
            {

             //TRACE("Wait ReceiptTime = %d",NS100_MSEC(GetTime()-wait_receipt_timer));
            }
            wait_receipt_timer = set ? GetTime():0;
           }

        if((changes&CLIENT_STATE_MASK) && notify && owner && owner->need_notify(GRNET_NOTIFY_CLIENT_STATE))
        {
         char buf[sizeof(NOTIFY_CODE)+sizeof(MODEM_LINE_STATE)-sizeof(DWORD)];
         LPNOTIFY_CODE nc = (LPNOTIFY_CODE)buf;
         ZeroMemory(nc,sizeof(NOTIFY_CODE)+sizeof(MODEM_LINE_STATE)-sizeof(DWORD));
         nc->dw_size     = sizeof(NOTIFY_CODE)+sizeof(MODEM_LINE_STATE)-sizeof(DWORD);
         nc->from_module =   (*owner)();
         LPMODEM_LINE_STATE mls = (LPMODEM_LINE_STATE)nc->notify_data;
         mls->dw_size = sizeof(*mls);
         mls->addr.addr  = -1;
         mls->addr.modem = owner->get_modem_number();
         mls->addr.line  = get_number();
         mls->line_state = new_state&CLIENT_STATE_MASK;
         mls->state_changes  = changes;
         owner->notify_handles(nc,GRNET_NOTIFY_CLIENT_STATE);
        }
       }
     return changes;
}

bool    __fastcall grnet_line::send(LPMPROTO_HEADER mph,DWORD sz)
{
 bool ret = false;
  /*Преобразование входных данных в команду иданные в виде протокола gratony*/
  if(mph->fa == FA_OTD)
   {
    BYTE send_buf[2048];
    int len;
    DWORD cmd;
    otd_proto op;
    op.dw_size = sizeof(op);
    if(otd_proto_parser((LPBYTE)mph->data,mph->data_size,&op,NULL))
      {
       len = otd_convert_otd2atony(&op,&cmd,send_buf,sizeof(send_buf));
       if(cmd!=(DWORD)-1)
         send_data(cmd,send_buf,len);
       ret = true;
      }
   }
   else
   SetLastError(MDM_ERR_INVALID_FA);
  return ret;
}

void    __fastcall grnet_line::connect        (bool conn)
{
 if(!conn && check_client_state(CLIENT_STATE_DISCONNECTING))
    TThread::Terminate();
 modem_line::connect(conn);
}

void    __fastcall grnet_line::on_disconnect     ()
{
  //Обрыв связи
 tu_enabled = FALSE;
 wait_receipt(false);//Сбросить ожидание квитанции
 tx_queue.DropData();
 rx_len  = 0;
 tx_len  = 0;
 rx_kadr_number = tx_kadr_number = 0;
  if(work_mode == GRNET_WM_AS_SERVER)
    {
    *client_name = 0;
     Terminate();
    }
    else
    {
    client_id = -1;
    TGRNetModule * mod =(TGRNetModule*)owner;
    mod->diag_reset();
    /*Отправить кадр готовности ПУ*/
    make_pu_ready(OTD_DIAG_PUCONNECT);
    }
    host_addr = TIpxAddress();
    modem_line::on_disconnect();
 if(check_client_state(CLIENT_STATE_DISCONNECTING))
    TThread::Terminate();
 set_client_state(CLIENT_STATE_DISCONNECTED,true);
}

void    __fastcall grnet_line::on_connect     ()
{
     //Установка связи
      set_client_state(-1,false,false);
      set_state(MODEM_LINE_STATE_TXREADY,true);
      rx_kadr_number = tx_kadr_number = 0;
      active_tu.clear();
      modem_line::on_connect();
}


DWORD __fastcall grnet_line::get_line_text  (wchar_t * text,DWORD text_sz)
{
 *text = 0;
  char lt[MAX_PATH]; DWORD len;
  len = wsprintf(lt,"%s ID %X ",*client_name ? client_name:" ",(DWORD)client_id);
  KeRTL::Ansi2Unicode(text,lt);
 return len;
}


DWORD   __fastcall grnet_line::refresh(DWORD)
{
 if(is_worked())
 {
  if(is_connected())
  {
   if(work_mode)
   {
    //если работает как клиент, то отправить вызов
    sotd_addr addr(this->pu_number,OTD_ADDR_MAXVALUE,OTD_FA_QUERY|OTD_FA_ALL,OTD_ADDR_MAXVALUE );
    send_grnet_query(&addr);
    send_pu_name();
   }
   else
   {
     //если работает как сервер, то сделать вызов
     make_query(pu_number,-1,OTD_FA_ORDER|OTD_FA_ALL,-1,false);
     send_ready(-1);
   }
  }
  else
   if(work_mode)
     begin_connection();
 } 
 return GKH_RET_SUCCESS;
}



void    __fastcall grnet_line::destroy_socket()
{
 TLockHelper l(locker);
 if(IsValidHandle((HANDLE)ipx_socket.GetSocket()))
 {
  CancelIo((HANDLE)ipx_socket.GetSocket());
  ipx_socket.DestroySocket();
  icp.Close();
 }
}

bool    __fastcall grnet_line::create_socket()
{
 destroy_socket();
 TLockHelper l(locker);
 ipx_socket.CreateIpxSocket(work_mode==GRNET_WM_AS_CLIENT ? client_socket:0,0,WSA_FLAG_OVERLAPPED);
 if(IsValidHandle((HANDLE)ipx_socket.GetSocket()))
 {
  icp.AppendFileHandle((HANDLE)ipx_socket.GetSocket(),GRNET_RX_KEY);
  return true;
 }
 return false;
}


void __fastcall grnet_line::make_pu_ready(DWORD diag)
{
   if(pu_number<OTD_ADDR_MAXVALUE)
   {
    BYTE buff[128];
    otd_proto op;
    op.dw_size = sizeof(op);
    otd_proto_format(buff,sizeof(buff),OTD_PROTO_PART_DIAG,0,0,0,0,0,0,&op);
    op.addr->addr = -1;
    op.addr->fa   = OTD_FA_ALL;
    op.addr->pu = pu_number;
    *op.diag = diag;
    queue_rxdata(FA_OTD,buff,op.proto_size,false);
   }
}


bool __fastcall grnet_line::do_start()
{
 bool ret = false;
    TGRNetModule * mod = dynamic_cast<TGRNetModule*>(get_owner());
    if(mod)
     pu_number = mod->settings.pu_number;
    if(work_mode == GRNET_WM_AS_CLIENT || (work_mode == GRNET_WM_AS_SERVER && (*client_name)))
    {
      if(create_socket())
         {
          if(work_mode == GRNET_WM_AS_CLIENT)
             make_pu_ready(OTD_DIAG_PUCONNECT);
          ret = Start(0x10000);//Для сервера нить приостанавливается

         }
    }
 return ret;
}

bool __fastcall grnet_line::do_stop()
{
  bool ret = true;

  if(GetCurrentThreadId() == Id)
  {
   TThread::Terminate();
  }
  else
  {
   Terminate();
   DWORD timeout = tx_retry_count*tx_retry_delay*20;
   ret = WaitFor(timeout);
  }
 if(work_mode == GRNET_WM_AS_CLIENT)
    make_pu_ready(OTD_DIAG_PUCONNECT);

  return ret;
}




DWORD __fastcall grnet_line::get_client_info(LPGRNET_CLIENT_INFO cli)
{
  DWORD ret = GKH_RET_ERROR;
  TLockHelper l(locker);
  if(owner && owner->check_data_size(cli,sizeof(*cli)))
  {
     memcpy  (&cli->host_addr,&host_addr,sizeof(cli->host_addr));
     memcpy  (&cli->local_addr,&ipx_socket.GetLocalAddress(), sizeof(&cli->local_addr));
     safe_strcpyn(cli->server_name,server_name,sizeof(cli->server_name)-1);
     safe_strcpyn(cli->client_name,client_name,sizeof(cli->client_name)-1);
     cli->client_id = this->client_id;
     cli->client_state = client_state;
     ret = GKH_RET_SUCCESS;


  }

  return ret;

}

 bool    __fastcall grnet_line::send_data(DWORD cmd,LPVOID data,DWORD  data_len)
 {
   BYTE            buff[1024];
   LPGRNET_KADR    kadr=(LPGRNET_KADR)buff;
   grnet_init_kadr(kadr,cmd,0,0,TRUE,data,data_len);
   tx_queue.PutIntoQueue(kadr,grnet_kadr_len(kadr));
   if(tx_queue.QueueCount() %2 == 1)
    icp.PostStatus(GRNET_TX_KEY,0,NULL);
   return true;
 }

void    __fastcall grnet_line::send_pu_name()
{
  DWORD len = 0;
  wchar_t pu_name [MAX_PATH];
  TGRNetModule* mod = dynamic_cast<TGRNetModule*>(get_owner());
  if(mod)
     {
      len = mod->get_pu_name(pu_name,sizeof(pu_name)/sizeof(wchar_t));
      pu_name[len] = 0;
     } 
 if(len)
 {
   if(work_mode == GRNET_WM_AS_CLIENT)
   {

     ++len;
     BYTE buffer[1024];
     otd_proto op;
     ZeroMemory(&op,sizeof(op));
     op.dw_size = sizeof(op);
     otd_proto_format(buffer,sizeof(buffer),OTD_PROTO_PART_NAME,0,0,0,0,0,len,&op);
     op.addr->addr = -1;
     op.addr->pu   = pu_number;
     otd_text_set(op.name,pu_name);
     queue_rxdata(FA_OTD,buffer,op.proto_size,false,0);
   }
 }
}













