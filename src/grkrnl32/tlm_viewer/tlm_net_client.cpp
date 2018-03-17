#include "tlm_net_client.hpp"
#include <algorithm>
#include <functional>
#include <stdio.h>





 tlm_client::~tlm_client()
 {
  TerminateAndWait(2000,true);
 }

 DWORD  __fastcall tlm_client::OnException(DWORD ExceptionCode,LPEXCEPTION_POINTERS exp)
 {
  this->locker.FullUnlock();
  return TThread::OnException(ExceptionCode,exp);
 }


 bool   __fastcall tlm_client::start_work(const char * srv_addr,int srv_port)
 {
   recv_buffer.set_size(128000);
   if(!server_addr.length() || stricmp(server_addr.c_str(),srv_addr) || srv_port!= server_port)
   {
    server_addr = srv_addr;
    server_port = srv_port;
    opened_kadrs.clear();
    names.clear();
    kadrs.clear();
    k_entryes.clear();
    records.clear();
   }
   return this->Start();
 }

 void   __fastcall tlm_client::connect_to_server()
 {
   if(connected == CS_NOTCONNECTED)
   {
   if(this->client_socket)
      {
       client_socket;
       client_socket = 0;
      }
   if(net_event)
   {
      events_set-=net_event;
      WSACloseEvent(net_event);
      net_event = NULL;
   }

   client_socket  = new TIPSocket(true);
   if(client_socket)
   {
    net_event      = WSACreateEvent();
    WSAEventSelect (client_socket->GetSocket(),net_event,FD_READ|FD_WRITE|FD_CONNECT|FD_CLOSE);
    events_set+=net_event;
    TIPAddress host_addr(server_addr.c_str(),server_port);
    client_socket->Connect(host_addr(),sizeof(host_addr));
    connected = CS_CONNECTING;
   }
  }
 }

 void   __fastcall tlm_client::disconnect_from_server()
 {
   this->client_socket->Disconnect();
   this->client_socket->DestroySocket();
   connected = FALSE;
 }

 wstring __fastcall tlm_client::get_term_event_name()
 {
  wchar_t name[MAX_PATH];
  swprintf(name,L"%04X_%s",GetCurrentProcessId(),TLM_CLIENT_COMMON_TEMINATE);
  return wstring(name);

 }

 bool   __fastcall tlm_client::BeforeExecute()
 {
   bool   ret = TThread::BeforeExecute();
   recv_buffer.move_to_begin(recv_buffer.get_data_len(),0);


   terminate_event = OpenEventW(SYNCHRONIZE,TRUE,get_term_event_name().c_str());
   events_set.Clear();
   events_set  += terminate_event;
   connected = CS_NOTCONNECTED;
   connect_to_server();
   return ret;
 }

 int    __fastcall tlm_client::Execute()
 {
  while(!CheckTerminateRequest())
  {
   DWORD  event_code = events_set.Wait(100);
   switch(event_code)
   {
    case  WR_TERMINATE_CODE:
         TThread::Terminate();  break;
    case  WR_NETWORK_CODE:  // Сетевое событие
          handle_network_events();
          break;
    case  WAIT_TIMEOUT:
          on_timeout();
          break;

   }
   if(this->client_socket == NULL)
      DebugBreak();
  }
  return 0;
 }

 void   __fastcall tlm_client::BeforeTerminate()
 {
  if(client_socket)
  {
   disconnect_from_server();
   delete client_socket;
   client_socket = 0;
   WSACloseEvent(this->net_event);
   net_event = NULL;
   store_server_data();
   
  }
  if(this->terminate_event)
   CloseHandle(terminate_event);
   terminate_event = NULL;
 }

 void   __fastcall tlm_client::handle_network_events()
 {
  WSANETWORKEVENTS ne = {0};
  WSAEnumNetworkEvents(client_socket->GetSocket(),net_event,&ne);
  if(ne.lNetworkEvents&FD_READ)
     on_net_read(ne.iErrorCode[FD_READ_BIT]);

  if(ne.lNetworkEvents&FD_WRITE)
     on_net_write(ne.iErrorCode[FD_WRITE_BIT]);

  if(ne.lNetworkEvents&FD_CLOSE)
     on_net_disconnect(ne.iErrorCode[FD_CLOSE_BIT]);

  if(ne.lNetworkEvents&FD_CONNECT)
     on_net_connect(ne.iErrorCode[FD_CONNECT_BIT]);



 }

 void   __fastcall tlm_client::handle_rx()
 {
    //Обработка принятых данных
    DWORD sync_offset = recv_buffer.sync_internal(0xAAAA5555);
    while(recv_buffer.get_mproto_len())
    {
      LPMPROTO_HEADER mph = (LPMPROTO_HEADER) recv_buffer. begin();
      if(mph->fa == FA_OTD_MEDIUM)
      {
        LPOTDM_PROTO_HEADER omph = (LPOTDM_PROTO_HEADER)mph->data;
        ++rx_kadrs_count;
        handle_mproto(omph);
      }
     recv_buffer.move_mproto();

     if(recv_buffer.sync_internal(0xAAAA5555))
      {
       //DebugBreak();
      }
     }

 }

void   __fastcall tlm_client::handle_mproto(LPOTDM_PROTO_HEADER omph)
{
 if(omph->command & OTDM_RESPOND )
 {
   bool beg_resp = (omph->command&OTDM_BEGINDATA) ? true:false;
   bool end_resp = (omph->command&OTDM_ENDDATA) ? true:false;
   DWORD cmd = omph->command&OTDM_COMMAND_MASK;
   switch(cmd)
   {
    case OTDMPROTO_CMD_ENUM_KADRS:
    case OTDMPROTO_CMD_GETKADR:
         //Обработка перечисления кадров
         handle_enum_kadrs(omph,end_resp);
         break;
    /*case OTDMPROTO_CMD_ENUM_KADRS:
         //Обработка перечисления кадров
         handle_enum_kadrs(omph,end_resp);
         break;*/

    case OTDMPROTO_CMD_GETKADR_IMAGE:
         handle_image(omph,beg_resp,end_resp);
         break;
    case OTDMPROTO_CMD_GETKADR_ENTRYES:
         handle_entryes(omph,beg_resp,end_resp);
         break;
    case OTDMPROTO_CMD_GETKADR_RECORDS:
         handle_records(omph,beg_resp,end_resp);
         break;
    case OTDMPROTO_CMD_RECORDS:
         handle_records_changed(omph,beg_resp,end_resp);
         break;
    case OTDMPROTO_CMD_GET_RECORD_SNAME:
         handle_record_names(omph,true);
    break;
    case OTDMPROTO_CMD_GET_RECORD_NAME :
         handle_record_names(omph,false);
    break;
    default:
    DebugBreak();
    break;
   }
 }
}

 void   __fastcall tlm_client::handle_image     (LPOTDM_PROTO_HEADER omph,bool beg,bool end)
 {
  if(beg)
  {
   //Начало приёма кадра
   file.Close();
   LPOTDM_PROTO_IMAGE_BEGIN img_beg = (LPOTDM_PROTO_IMAGE_BEGIN) omph->data;
   mdb_kadr kdr;
   if(get_kadr(img_beg->kadr_id,kdr,false) && kdr.image_id!=(DWORD)-1)
   {
    string str(data_dir);
    str+=TLMV_IMAGES_FOLDER;

    wstring wstr(str.length(),0);
    //Преобразовать к Unicode
    transform(str.begin(),str.end(),wstr.begin(),char2mb());

    wstr+=img_beg->image_name;
    set_text(kdr.image_id,wstr);


    file.Open(wstr.c_str(),OPEN_ALWAYS);
    if(file.IsValid())
    {
     image_for_kadr = kdr.kadr_id;
     file.SetSize(img_beg->image_size);
     file.SeekWr(0,FILE_BEGIN);
     LPFILETIME f_time = (LPFILETIME)& kdr.image_time;
     SetFileTime(file(),f_time,f_time,f_time);
    }
    else
    image_for_kadr = -1;

   }
  }
  else
  {
  if(image_for_kadr!=(DWORD)-1)
  {
    LPOTDM_PROTO_IMAGE_CHUNK chunk = (LPOTDM_PROTO_IMAGE_CHUNK)omph->data;
    file.Write(chunk->chunk_data,chunk->chunk_size);
    if(end)
     {
      file.Close();
      if(this->opened_kadrs.count(image_for_kadr))
      {
       //Кадр открыт
       SendMessage(opened_kadrs[image_for_kadr],TC_CMD_SET_BKGND,0,(LPARAM)this->get_kadr_image_file(image_for_kadr).c_str());
      }
     } 
  }
  }
 }


void   __fastcall tlm_client::handle_enum_kadrs(LPOTDM_PROTO_HEADER omph,bool end)
{
 LPOTDM_PROTO_KADR kadr = (LPOTDM_PROTO_KADR )omph->data;

  wchar_t * kname = kadr->names;
  DWORD ch  ;
  if(add_kadr(kadr->kadr,ch))
     ch &=~MDB_KADR_FIELD_IMAGE;


  //request_kadr_image(kadr->kadr.kadr_id);

  if(kadr->mask&MDB_KADR_FIELD_SNAME)
    {
     wstring name;
     name  = kname;
     set_text(kadr->kadr.sname_id,name);
     kname +=lstrlenW(kname)+1;
     ch|=MDB_KADR_FIELD_SNAME;
    }
  if(kadr->mask&MDB_KADR_FIELD_NAME)
    {
     wstring name = kname;
     set_text(kadr->kadr.name_id,name);
     ch|=MDB_KADR_FIELD_NAME;
    }
    
  HWND kadr_wnd = NULL;
  read_lock();
  open_kadr_map::iterator ptr = opened_kadrs.find(kadr->kadr.kadr_id);
  if(ptr!=opened_kadrs.end())
    kadr_wnd = ptr->second;
  read_unlock();
  if(kadr_wnd)
     ::SendMessage(kadr_wnd,TC_CMD_REPAINT_KADR,ch,(LPARAM)&kadr->kadr);
  if(ch&MDB_KADR_FIELD_IMAGE)
     request_kadr_image(kadr->kadr.kadr_id);
}

 int    __fastcall compare_entryes(LPMDB_KADR_ENTRY  e1,LPMDB_KADR_ENTRY e2)
 {
  return memcmp(e1,e2,sizeof(MDB_KADR_ENTRY));
 }

 void   __fastcall tlm_client::handle_entryes   (LPOTDM_PROTO_HEADER omph,bool beg,bool end)
 {
  /*Обработка записей содержимого кадра*/

  int changes_count = 0;
  LPOTDM_PROTO_KADR_ENTRYES ent = (LPOTDM_PROTO_KADR_ENTRYES)omph->data;
  write_lock();
  DWORD kadr_id = ent->kadr_id;
  if(find_kadr(ent->kadr_id)<this->kadrs.size())
  {
     entryes_list & ent_list = k_entryes[ent->kadr_id];
     ent_list.resize(ent->entryes_count);
     entryes_list::iterator dest_ptr = ent_list.begin(), dest_end = ent_list.end();
     LPMDB_KADR_ENTRY src_ptr = ent->entryes;

     while(dest_ptr<dest_end)
     {
      if(compare_entryes(&*dest_ptr,src_ptr))
         {
          *dest_ptr = *src_ptr;
          ++changes_count;
         }
      ++dest_ptr;++src_ptr;
     }
  }
  write_unlock();
  if(end)
     update_opened_kadr_entryes(kadr_id);
 }

 void   __fastcall tlm_client::handle_records   (LPOTDM_PROTO_HEADER omph,bool beg,bool end)
 {

  LPOTDM_PROTO_KADR_RECORDS kr = (LPOTDM_PROTO_KADR_RECORDS) omph->data;
  LPMDB_SHORT_RECORD b_ptr = kr->records, e_ptr = b_ptr+kr->rec_count;
  write_lock();
  while(b_ptr<e_ptr)
  {
   mdb_record rec(*b_ptr);
   DWORD mask = -1;
   mask &=~(MDBR_FIELD_NAME|MDBR_FIELD_SNAME);
   if(add_record(rec,mask))
     {
      //Добавлена запись
      //TODO    Обновить имена
      //Вызвать имена записи
      send_command(true,true,OTDMPROTO_CMD_GET_RECORD_NAME,0,&rec.rec_id,sizeof(rec.rec_id));
      send_command(true,true,OTDMPROTO_CMD_GET_RECORD_SNAME,0,&rec.rec_id,sizeof(rec.rec_id));
     }
   ++b_ptr;

  }
  write_unlock();
 }

void   __fastcall tlm_client::handle_record_names(LPOTDM_PROTO_HEADER omph,bool short_name)
 {
  LPOTDM_PROTO_RECORD_NAME rn = (LPOTDM_PROTO_RECORD_NAME) omph->data;
  write_lock();
  DWORD rec_offs = this->find_record(rn->rec_id);
  if(rec_offs<this->records.size())
  {
   rec_list::iterator ptr = records.begin();
   advance(ptr,rec_offs);
   wstring name(rn->text,rn->len);
   if(short_name)
     ptr->sname = rn->name_id;
     else
     ptr->name  = rn->name_id;
   set_text(rn->name_id,name );
  }
  write_unlock();
 }


void   __fastcall tlm_client::handle_records_changed(LPOTDM_PROTO_HEADER omph,bool beg,bool end)
{
 LPOTDM_PROTO_RECORDS p_recs = (LPOTDM_PROTO_RECORDS) omph->data;
 if(p_recs->rec_count)
 {
  LPBYTE src_ptr = (LPBYTE)p_recs->records;
  mdb_record rec;
  write_lock();
  for(DWORD i = 0;i<p_recs->rec_count;i++)
  {
   LPOTDM_PROTO_RECORD proto_rec = (LPOTDM_PROTO_RECORD) src_ptr;
   DWORD mask = proto_rec->mask;
   src_ptr+=sizeof(proto_rec->mask);
   src_ptr+= otdm_unpack_record(src_ptr,omph->data_size,proto_rec->mask,&rec);
   add_record(rec,mask);
   //TODO обработать изменения имен

  }
  write_unlock();
 }
}



int  __fastcall get_grow_size_for(int  sz)
{
  #define GROW_CHUNK 8192
  div_t dt;
  dt = div(sz,GROW_CHUNK);
  return GROW_CHUNK*dt.quot + GROW_CHUNK-dt.rem;
}

 void   __fastcall tlm_client::on_net_read      (DWORD err)
 {
   DWORD recv_len  ;
   do
   {
   client_socket->IoctlSocket(FIONREAD,&recv_len);
   if(recv_len)
   {
     if(recv_buffer.get_free_size()<recv_len)
         handle_rx();
     LPBYTE ptr = recv_buffer.end();
     recv_len = client_socket->Recv(ptr,recv_len,0);
     recv_buffer.add(ptr,recv_len);
     rx_bytes_count+=recv_len;
     //Sleep(0);
    }
   
   }while(recv_len);
  handle_rx();
 }

 void   __fastcall tlm_client::on_net_write     (DWORD err)
 {
   kpa_timer = 0;
 }

 void   __fastcall tlm_client::on_net_connect   (DWORD err)
 {
  if(err)
     {
      connected = 0;
      //Сбросить Готовности;
      set_bad_ready();
     } 
     else
     {
      //Создание каталога для сохранения данных

      data_dir = TLMV_DATA_FOLDER;
      data_dir+=server_addr;
      replace(data_dir.begin(),data_dir.end(),'.','_');
      data_dir+='\\';
      string img_dir = data_dir;
      img_dir+=TLMV_IMAGES_FOLDER;
      ForceDirs((char*)img_dir.c_str());
      restore_server_data();
      connected = CS_CONNECTED;
      packet_counter = 0;
      send_command(true,true,OTDMPROTO_CMD_ENUM_KADRS,0,NULL,NULL);
      recv_buffer.move_to_begin(recv_buffer.get_data_len());
      rx_bytes_count =  rx_kadrs_count =  rx_errors_count = tx_bytes_count =
      tx_kadrs_count = 0;
      //TODO очистка содержимого
      //TODO восстановление заказов
      restore_orders();
      kpa_timer = 0;

     }
 }

 void   __fastcall tlm_client::on_net_disconnect(DWORD err)
 {
   this->connected = FALSE;
 }

 void   __fastcall tlm_client::on_timeout()
 {
  if(!connected)
    {
     this->connect_to_server();
    }
    else
    {
     //Проверка связи каждые 3 секунды
     kpa_timer +=100;
     if(kpa_timer > 3000)
        {
         send_keep_alive();
        }
    }
 }

void        __fastcall tlm_client::send_keep_alive()
{

    MPROTO_HEADER mph = {0};
    mph.addr_from.reserv1 = -1;
    mph.data_size = sizeof(mph.data);
    if(!send_data(&mph))
      disconnect_from_server();
      
}

DWORD       __fastcall tlm_client::send_command(bool begin,bool finish,DWORD cmd,DWORD err,LPVOID data,DWORD data_size )
{
 DWORD ret = 0;
 BYTE   buffer[16384];
 LPMPROTO_HEADER     mph;
 LPOTDM_PROTO_HEADER omph;
 DWORD  need_size = sizeof(MPROTO_HEADER)-sizeof(mph->data[0])+sizeof(*omph)-sizeof(omph->data[0])+data_size;
 if(need_size<sizeof(buffer))
   mph = (LPMPROTO_HEADER) buffer;
  else
   mph = (LPMPROTO_HEADER) new BYTE[need_size];
 if(mph)
 {
 ZeroMemory(mph,need_size);
 mph->addr_to.addr = -1;
 mph->data_size = need_size+(sizeof(mph->data)-sizeof(*mph));
 mph->fa = FA_OTD_MEDIUM;
 omph =(LPOTDM_PROTO_HEADER) mph->data;
 omph->command    = cmd&(OTDM_COMMAND_MASK|OTDM_ORDER);
 //omph->command   |= OTDM_RESPOND;
 if(begin)
   omph->command  |= OTDM_BEGINDATA;
 if(finish)
  omph->command  |= OTDM_ENDDATA;
  if(err)
  omph->command  |= OTDM_ERROR;
  omph->error     = err;
  omph->data_size = data_size;
  memcpy(omph->data,data,data_size);

  ret = send_data(mph);
  ++tx_kadrs_count;

  if( buffer!=(LPBYTE)mph)
      delete  [](LPBYTE)mph;
  }
  return ret;
}

int   __fastcall tlm_client::send_tu(DWORD rec_id,DWORD op_code)
{
  OTDM_PROTO_TUOPERATION tuop;
  tuop.rec_id  = rec_id;
  tuop.op_code = op_code;
  return (int) send_command(true,true,OTDMPROTO_CMD_TUOPERATION,0,&tuop,sizeof(tuop));

}


DWORD  __fastcall tlm_client::send_data(LPMPROTO_HEADER mph)
{
 DWORD ret = -1;
 mph->pkt_num  = ++packet_counter;
 mph->internal =  MP_INTERNAL_DEFAULT;
 mproto_protect(mph);
 DWORD send_len = MPROTO_SIZE(mph);
 read_lock();
 tx_bytes_count+=send_len;
 ret =  client_socket->Send(mph,send_len,0);
 read_unlock();
 kpa_timer = 0;

 return ret;
}

void   __fastcall tlm_client::request_kadr_image(DWORD kadr_id)
{
  //TODO проверить есть ли
  send_command(true,true,OTDMPROTO_CMD_GETKADR_IMAGE,0,&kadr_id,sizeof(kadr_id));
}

 void  __fastcall tlm_client::send_order_kadr(DWORD id,bool order)
 {
  DWORD order_data[2];
  order_data[0] = id;
  order_data[1] = order ? -1:0;

   send_command(true,true,OTDMPROTO_CMD_GETKADR |OTDM_ORDER,0,order_data,sizeof(order_data));

 }

 void   __fastcall tlm_client::store_server_data()
 {
  //Сохранение данных сервера
  string file_name = data_dir;
  file_name+="kadrnames.lst";
  TFile file;
  file.Open(file_name.c_str(),OPEN_ALWAYS);
  if(file.IsValid())
  {
   file.SeekWr(0,FILE_BEGIN);
   file<<(DWORD)names.size();
   names_map::iterator beg = names.begin(),end = names.end();
   while(beg!=end)
   {
    DWORD len = beg->second.length();
    file<<(DWORD)beg->first<<len;
    if(len)
    file.Write((LPVOID)beg->second.c_str(),len<<1);
    ++beg;
   }
  }
 }

 void   __fastcall tlm_client::restore_server_data()
 {
  //Восстановление данных сервера
  names.clear();
  string file_name = data_dir;
  file_name+="kadrnames.lst";
  TFile file;
  file.Open(file_name.c_str(),OPEN_EXISTING );
  if(file.IsValid())
  {
   DWORD count;
   file>>count;
   while(count--)
   {
    DWORD id,len;
    file>>id>>len;
    if(len)
    {
     wstring & ws = names[id];
     ws.resize(len+1,0);
     file.Read((LPVOID)ws.c_str(),len<<1,len);
     len=len+1;
    }
   }
  }

 }

 void   __fastcall tlm_client::restore_orders()
 {
   open_kadr_map::iterator beg = opened_kadrs.begin(),end = opened_kadrs.end();
   while(beg!=end)
   {
    this->send_order_kadr(beg->first);
    ++beg;
   }
 }

void __fastcall tlm_client::set_bad_ready()
{
 write_lock();
 rec_list::iterator beg_rec = this->records.begin(),end_rec = records.end();
 while(beg_rec<end_rec)
 {
  beg_rec->diag = MDBR_DIAG_BAD;
  ++beg_rec;
 }
 kadr_list::iterator beg_kdr = kadrs.begin(),end_kdr = kadrs.end();
 while(beg_kdr<end_kdr)
 {
  if(beg_kdr->kadr_diag!=MDBR_DIAG_BAD)
     {
      beg_kdr->kadr_diag = MDBR_DIAG_BAD;
      beg_kdr->discrete_changes = 1;
     }
     else
     beg_kdr->discrete_changes = 0;

  ++beg_kdr;
 }
 write_unlock();
 refresh_opened_kadrs();

}

void __fastcall tlm_client::refresh_opened_kadrs(bool always )
{
 open_kadr_map::iterator okb = opened_kadrs.begin(),oke = opened_kadrs.end();
 mdb_kadr kdr;
 while(okb!=oke)
 {
  DWORD  kdr_id = okb->first;
  HWND   wnd    = okb->second;
  get_kadr(kdr_id,kdr,false);
  if(kdr.discrete_changes || always)
     ::SendMessage(wnd,TC_CMD_REPAINT_KADR,MDB_KADR_FIELD_DIAG,(LPARAM)&kdr);
  ++okb;
 }
}








