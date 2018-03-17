#include "gkipclient.hpp"

 struct data_kadrs
 {
  DWORD an_kadr_id;
  DWORD bit_kadr_id;
  DWORD int_kadr_id;
 };

 TGKIPClient::TGKIPClient ():client_event(NULL),client_socket(NULL)
 {
  dev_config_parser config("dataserv_config.txt");
  config.get_server_param(server_name,sizeof(server_name),server_port);
  data.reserve(config.dev_count()+1);
  for(int i = 0;i<config.dev_count();i++)
   {
    device_config dc = {0};
    config.get_device(i,dc);
    //opc_data::iterator ptr =
    TDataForOPCServer entry(dc.device_name,dc.an_tag_count,dc.bit_tag_count,dc.int_tag_count,dc.tag_name_prefix);
    opc_data::iterator ptr = data.insert(data.end(),entry);

    data_kadrs * dk = (data_kadrs*)ptr->Ext.Reserved;
    dk->an_kadr_id = dc.an_tag_kadrid;
    dk->bit_kadr_id = dc.bit_tag_kadrid;
    dk->int_kadr_id = dc.int_tag_kadrid;

   }

  data.insert(data.end(),TDataForOPCServer()); // Завершить нулевым

 }

 TGKIPClient::~TGKIPClient()
 {

 }

 bool __fastcall TGKIPClient::BeforeExecute()
 {
  wait_terminate        = FALSE;
  client_connected      = FALSE;
  client_disconnecting  = 0;
  terminate_event.SetEvent(false);
  synchro_set.Clear();
  synchro_set+=terminate_event();
  client_socket = new TIPSocket(true);
  if(client_socket && client_socket->GetSocket()!=INVALID_SOCKET)
  {
  /*Подготовка гнезда к работе*/
  client_event = WSACreateEvent();
  do_connect();
  return TThread::BeforeExecute();
  }
  return false;
 }

 void __fastcall TGKIPClient::BeforeTerminate()
 {
  do_disconnect();
  synchro_set-=client_event;
  WSACloseEvent(client_event);
  if(client_socket)
     delete client_socket;
  TThread::BeforeTerminate();
 }

 int  __fastcall TGKIPClient::Execute()
 {
  while(!CheckTerminateRequest())
  {
   DWORD wr = synchro_set.Wait(1000);
   switch(wr)
   {
    case 0:  wait_terminate = TRUE;
             do_disconnect();break;
    case 1: // обработка событий по сети
           this->handle_socket_events();break;

    case  WAIT_TIMEOUT:
          //Обработка тайм-аута
          if(client_disconnecting)
          {
             if(++client_disconnecting >2)
               TThread::Terminate();           
          }
          else
          {
           if(!client_connected)
              do_connect();
          }
    break;
   }
  }
  return 0;
 }

 void    __fastcall TGKIPClient::do_connect()
 {
   if(!client_socket)
      client_socket = new TIPSocket(true);
   WSAEventSelect(client_socket->GetSocket(),client_event,FD_CONNECT|FD_READ|FD_CLOSE);
   synchro_set+=client_event;
   client_disconnecting = 0;
   TIPAddress addr(server_name,server_port);
   client_socket->Connect(addr(),sizeof(addr));
 }

 void    __fastcall TGKIPClient::do_disconnect()
 {
   if(client_connected)
   {
    ++client_disconnecting;
    client_socket->WSASendDisconnect(NULL);
    //TODO снять достоверность по всем объектам
   }
   else
   TThread::Terminate();
 }

 void    __fastcall TGKIPClient::handle_socket_events()
 {
   WSANETWORKEVENTS  events = {0};
   WSAEnumNetworkEvents(client_socket->GetSocket(),client_event,&events);
   if(events.lNetworkEvents&FD_CONNECT)
     {
       //Соединение
        on_socket_connect(events.iErrorCode[FD_CONNECT_BIT]);

     }

   if(events.lNetworkEvents&FD_CLOSE)
     {
       //Сервер разорвал соединение
       on_socket_disconnect(events.iErrorCode[FD_CLOSE_BIT]);
     }

   if(events.lNetworkEvents&FD_READ)
     {
      // На гнезде получены данные и готовы к чтению
       on_socket_read(events.iErrorCode[FD_READ_BIT]);
     }

 }

 void    __fastcall TGKIPClient::on_socket_read      (DWORD err)
 {
  DWORD rd_size;
  client_socket->IoctlSocket(FIONREAD,&rd_size);
  while(rd_size)
  {
    if(in_buffer.get_free_size()<rd_size)
       in_buffer.grow(rd_size);
    client_socket->Recv(in_buffer.end(),rd_size,0);
    in_buffer.add(in_buffer.end(),rd_size);
    client_socket->IoctlSocket(FIONREAD,&rd_size);
  }

  int mp_len;
  do{
    in_buffer.sync_internal(0xAAAA5555);
    mp_len = in_buffer.get_mproto_len();
    if(mp_len)
    {
     handle_mproto((LPMPROTO_HEADER)in_buffer.begin());
     in_buffer.move_mproto();
    }

    }while(mp_len);
 }

 void    __fastcall TGKIPClient::on_socket_connect   (DWORD err)
 {
  if(!err)
  {
  client_disconnecting = 0;
  client_connected     = TRUE;
   //Отправка подписок на кадры
   send_kadr_orders();
  }
  else
  do_connect();
 }

 void    __fastcall TGKIPClient::on_socket_disconnect(DWORD err)
 {
  client_connected      = FALSE;
  TDataForOPCServer * data = this->get_data();
  while(data && data->structVersionNumber)
  {
   data->dataIsValid = FALSE;

   TAnTag * atag = data->anTagsArray, * atag_end = atag+data->anTagsCount;
   while(atag<atag_end)
     {
      exposeAnTag(atag,atag->value,QualityBad,atag->time);
      ++atag;
     }
   TBitTag * btag = data->bitTagsArray, * btag_end = btag+data->bitTagsCount;
   while(btag<btag_end)
     {
      exposeBitTag(btag,btag->value,QualityBad,btag->time);
      ++btag;
     }
   TIntTag * itag = data->intTagsArray, * itag_end = itag+data->intTagsCount;
   while(itag<itag_end)
     {
      exposeIntTag(itag,itag->value,QualityBad,itag->time);
      ++itag;
     }
   ++data;
  }

  client_socket->Disconnect();
  delete client_socket;
  client_socket = 0;

  if(client_disconnecting)
    {
     client_disconnecting  = 0;
     if(wait_terminate)
        {
         TThread::Terminate();
         wait_terminate = FALSE;
        }
    }
   else
    do_connect();//Сервер разорвал соединение делаем попвтку восстановить

 }

 void __fastcall TGKIPClient::send_kadr_orders()
 {
  // очистим соответвия кадров
  kadr_links.clear();
  obj_links.clear();

  BYTE buffer[MAX_PATH];
  LPMPROTO_HEADER mph =(LPMPROTO_HEADER)buffer;
  LPOTDM_PROTO_HEADER omph;
  omph = (LPOTDM_PROTO_HEADER)mph->data;
  ZeroMemory(mph,sizeof(*mph)+sizeof(*omph));
  mph->fa = FA_OTD_MEDIUM;
  mph->internal   = (LPVOID)0xAAAA5555;
  omph->command   = OTDMPROTO_CMD_GETKADR|OTDM_ORDER;
  omph->data[1]   =  -1 ; // Всё по кадру
  omph->data_size = sizeof(DWORD)<<1;
  mph->data_size  = sizeof(*omph)+sizeof(DWORD);
  opc_data::iterator ptr = data.begin(),end = data.end();
  while(ptr<end && ptr->structVersionNumber)
  {
    data_kadrs * dk = (data_kadrs*)ptr->Ext.Reserved;
    if(dk->an_kadr_id)
    {
     if(kadr_links.count(dk->an_kadr_id)==0)
       {
        omph->data[0]   = dk->an_kadr_id;
        send_data(buffer,MPROTO_SIZE(mph));
       }
     kadr_links.insert(TKadrLinks::value_type(dk->an_kadr_id,ptr));
    }

    if(dk->bit_kadr_id)
    {
     if(kadr_links.count(dk->bit_kadr_id)==0)
       {
        omph->data[0]   = dk->bit_kadr_id;
        send_data(buffer,MPROTO_SIZE(mph));
       }
     kadr_links.insert(TKadrLinks::value_type(dk->bit_kadr_id,ptr));
    }

    if(dk->int_kadr_id)
    {
     if(kadr_links.count(dk->int_kadr_id)==0)
       {
        omph->data[0]   = dk->int_kadr_id;
        send_data(buffer,MPROTO_SIZE(mph));
       }
     kadr_links.insert(TKadrLinks::value_type(dk->int_kadr_id,ptr));
    }
   ++ptr;
  }

 }

 int     __fastcall TGKIPClient::send_data(LPBYTE data,DWORD len)
 {
   int send_sz ;
   do{
       send_sz = client_socket->Send(data,len,0);
     }while(!send_sz && client_socket->GetLastError() == WSAEWOULDBLOCK);

   return send_sz;
 }

 void __fastcall TGKIPClient::handle_mproto(LPMPROTO_HEADER mph)
 {
  if(mph->fa == FA_OTD_MEDIUM)
  {
    LPOTDM_PROTO_HEADER omph;
    omph = (LPOTDM_PROTO_HEADER)mph->data;
    if(omph->command & OTDM_RESPOND)
    {
     if(!omph->error)
     {
       switch(omph->command & OTDM_COMMAND_MASK)
       {
        case OTDMPROTO_CMD_GETKADR         : handle_kadr((LPOTDM_PROTO_KADR)omph->data);break;
        case OTDMPROTO_CMD_GETKADR_ENTRYES : handle_kadr_entryes((LPOTDM_PROTO_KADR_ENTRYES)omph->data);break;
        case OTDMPROTO_CMD_GETKADR_RECORDS : handle_kadr_records((LPOTDM_PROTO_KADR_RECORDS)omph->data);break;
        case OTDMPROTO_CMD_RECORDS         : handle_records((LPOTDM_PROTO_RECORDS) omph->data);break;
       }
     }
     else
      ;//handle_error()
    }
  }
 }


 void    __fastcall TGKIPClient::handle_kadr(LPOTDM_PROTO_KADR pk)
 {

 }

 void    __fastcall TGKIPClient::handle_kadr_entryes(LPOTDM_PROTO_KADR_ENTRYES pke)
 {
    LPMDB_KADR_ENTRY ke = pke->entryes;
    for(DWORD  i = 0;i< pke->entryes_count;i++,ke++)
    {
     TObjLinks::value_type val(ke->rec_id, obj_num_kid( ke->kadr_id,ke->number));
     obj_links.insert(val);
     
    }
 }

 void    __fastcall TGKIPClient::handle_kadr_records(LPOTDM_PROTO_KADR_RECORDS pkr)
 {
   LPMDB_SHORT_RECORD rec  = pkr->records;
   for(DWORD i = 0; i<pkr->rec_count;i++,rec++)
   {
    TObjLinks::iterator  rec_ptr = obj_links.lower_bound(rec->rec_id),
                         rec_ptr_end = obj_links.upper_bound(rec->rec_id);
    while(rec_ptr != rec_ptr_end)
    {
      TKadrLinks::iterator klptr     = kadr_links.lower_bound(rec_ptr->second.kadr_id),
                           klptr_end = kadr_links.upper_bound(rec_ptr->second.kadr_id) ;
      while(klptr!=klptr_end)
      {
       TDataForOPCServer * opc_data = klptr->second;
       opc_data->dataIsValid = TRUE;
       data_kadrs * dk = (data_kadrs*)opc_data->Ext.Reserved;

       if(rec_ptr->second.kadr_id == dk->an_kadr_id && (int)rec_ptr->second.number< opc_data->anTagsCount )
          {
           LPFILETIME ft = (LPFILETIME) &rec->time;
           exposeAnTag(opc_data->anTagsArray+rec_ptr->second.number,rec->value,rec->diag ?QualityBad: QualityGood ,*ft);
          }

       if(rec_ptr->second.kadr_id == dk->bit_kadr_id && (int)rec_ptr->second.number< opc_data->bitTagsCount )
          {
           LPFILETIME ft = (LPFILETIME) &rec->time;
           exposeBitTag(opc_data->bitTagsArray+rec_ptr->second.number,((BYTE)rec->value) ? true:false,rec->diag ?QualityBad: QualityGood ,*ft);
          }

       if(rec_ptr->second.kadr_id == dk->int_kadr_id && (int)rec_ptr->second.number< opc_data->intTagsCount )
          {
           LPFILETIME ft = (LPFILETIME) &rec->time;
           exposeIntTag(opc_data->intTagsArray+rec_ptr->second.number,((DWORD)rec->value) ? true:false,rec->diag ?QualityBad: QualityGood ,*ft);
          }

       ++klptr;
      }
     ++rec_ptr;
    };

   }
 }

DWORD WINAPI otdm_unpack_record(LPVOID buff,DWORD mask, LPMDB_RECORD  rec);
 void    __fastcall TGKIPClient::handle_records(LPOTDM_PROTO_RECORDS krecs)
 {
  /*Обработка изменений*/
       LPBYTE src_ptr = (LPBYTE)krecs->records;
       mdb_record rec;
       for(DWORD i = 0;i<krecs->rec_count;i++)
       {
        LPOTDM_PROTO_RECORD proto_rec = (LPOTDM_PROTO_RECORD) src_ptr;
        src_ptr+=sizeof(proto_rec->mask);
        src_ptr+= otdm_unpack_record(src_ptr,proto_rec->mask,&rec);
        TObjLinks::iterator  rec_ptr = obj_links.lower_bound(rec.Id),
                             rec_ptr_end = obj_links.upper_bound(rec.Id);
        while(rec_ptr != rec_ptr_end)
        {
          TKadrLinks::iterator klptr     = kadr_links.lower_bound(rec_ptr->second.kadr_id),
                               klptr_end = kadr_links.upper_bound(rec_ptr->second.kadr_id) ;
          while(klptr!=klptr_end)
          {
           TDataForOPCServer * opc_data = klptr->second;
           opc_data->dataIsValid = TRUE;
           data_kadrs * dk = (data_kadrs*)opc_data->Ext.Reserved;

           if(rec_ptr->second.kadr_id == dk->an_kadr_id && (int)rec_ptr->second.number< opc_data->anTagsCount )
              {
               TAnTag * an_tag = opc_data->anTagsArray+rec_ptr->second.number;
               mdb_record  record = rec;
               if(!(proto_rec->mask&MDBR_FIELD_VALUE))
                    record.value = an_tag->value;
               if(!(proto_rec->mask&MDBR_FIELD_DIAG))
                    record.diag = an_tag->quality == QualityGood ? 0:-1;
               if(!(proto_rec->mask&MDBR_FIELD_TIME))
                    record.time = GetTime(false);
               LPFILETIME ft = (LPFILETIME) &rec.time;

               exposeAnTag(an_tag,record.value,record.diag ?QualityBad: QualityGood ,*ft);
              }

           if(rec_ptr->second.kadr_id == dk->bit_kadr_id && (int)rec_ptr->second.number< opc_data->bitTagsCount )
              {
               TBitTag * bit_tag = opc_data->bitTagsArray+rec_ptr->second.number;
               mdb_record  record = rec;
               if(!(proto_rec->mask&MDBR_FIELD_VALUE))
                    record.value = (float)bit_tag->value;
               if(!(proto_rec->mask&MDBR_FIELD_DIAG))
                    record.diag = bit_tag->quality == QualityGood ? 0:-1;
               if(!(proto_rec->mask&MDBR_FIELD_TIME))
                    record.time = GetTime(false);
               LPFILETIME ft = (LPFILETIME) &rec.time;

               exposeBitTag(bit_tag,((BYTE)record.value) ? true:false,record.diag ?QualityBad: QualityGood ,*ft);
              }

           if(rec_ptr->second.kadr_id == dk->int_kadr_id && (int)rec_ptr->second.number< opc_data->intTagsCount )
              {
               TIntTag * int_tag = opc_data->intTagsArray+rec_ptr->second.number;
               mdb_record  record = rec;
               if(!(proto_rec->mask&MDBR_FIELD_VALUE))
                    record.value = int_tag->value;
               if(!(proto_rec->mask&MDBR_FIELD_DIAG))
                    record.diag = int_tag->quality == QualityGood ? 0:-1;
               if(!(proto_rec->mask&MDBR_FIELD_TIME))
                    record.time = GetTime(false);
               LPFILETIME ft = (LPFILETIME) &rec.time;

               exposeIntTag(int_tag,((DWORD)record.value) ? true:false,record.diag ?QualityBad: QualityGood ,*ft);
              }

           ++klptr;
          }
         ++rec_ptr;
        };


        
       }

 }

union multi_ptr
{
 LPVOID    v_ptr;
 LPBYTE    b_ptr;
 LPWORD    w_ptr;
 LPDWORD   dw_ptr;
 float   * float_ptr;
 __int64 * int64_ptr;
 lpotd_param_addr addr_ptr;
};

DWORD WINAPI otdm_unpack_record(LPVOID buff,DWORD mask, LPMDB_RECORD  rec)
{
 DWORD ret = 0;
 if(rec)
 {
 multi_ptr ptr;
 ptr.v_ptr = buff;

 rec->Id = *ptr.dw_ptr  ;
 ++ptr.dw_ptr;
 ret+=sizeof(DWORD);

 if(!(mask & MDBR_RECORD_REMOVED))
 {
  if(mask & MDBR_FIELD_VALUE)
  {
    rec->value = *ptr.float_ptr   ;
   ++ptr.float_ptr ;
   ret+=sizeof(*ptr.float_ptr);
  }

  if(mask & MDBR_FIELD_DIAG)
  {
   rec->diag = *ptr.w_ptr    ;
   ++ptr.w_ptr ;
   ret+=sizeof(*ptr.w_ptr);
  }

  if(mask & MDBR_FIELD_STATE)
  {
   rec->state = *ptr.w_ptr   ;
   ++ptr.w_ptr ;
   ret+=sizeof(*ptr.w_ptr);
  }

  if(mask & MDBR_FIELD_TIME)
  {
    rec->time  = *ptr.int64_ptr  ;
   ++ptr.int64_ptr;
   ret+=sizeof(*ptr.int64_ptr);
  }

  if(mask & MDBR_FIELD_MINVALUE)
  {
   rec->min_value =*ptr.float_ptr   ;
   ++ptr.float_ptr ;
   ret+=sizeof(*ptr.float_ptr);
  }

  if(mask & MDBR_FIELD_MAXVALUE)
  {
   rec->max_value = *ptr.float_ptr   ;
   ++ptr.float_ptr ;
   ret+=sizeof(*ptr.float_ptr);
  }

  if(mask & MDBR_FIELD_NAME)
  {
   rec->name = *ptr.dw_ptr   ;
   ++ptr.dw_ptr ;
   ret+=sizeof(*ptr.dw_ptr);
  }

  if(mask & MDBR_FIELD_SNAME)
  {
   rec->sname = *ptr.dw_ptr  ;
   ++ptr.dw_ptr ;
   ret+=sizeof(*ptr.dw_ptr);
  }

  if(mask & MDBR_FIELD_ADDR)
  {
   rec->addr = *ptr.addr_ptr   ;
   ++ptr.addr_ptr ;
   ret+=sizeof(*ptr.addr_ptr);
  }

  if(mask & MDBR_FIELD_OTDVALUE)
  {
    rec->otd_val = *ptr.dw_ptr  ;
   ++ptr.dw_ptr ;
   ret+=sizeof(*ptr.dw_ptr);
  }

  if(mask & MDBR_FIELD_OTDPD)
  {
   rec->otd_pd = *ptr.dw_ptr  ;
   ++ptr.dw_ptr ;
   ret+=sizeof(*ptr.dw_ptr);
  }

  if(mask & MDBR_FIELD_OPTIONS)
  {
   rec->options = *ptr.dw_ptr   ;
   ++ptr.dw_ptr ;
   ret+=sizeof(*ptr.dw_ptr);
  }
  if(mask & MDBR_FIELD_KVALUE)
  {
   rec->k_value = *ptr.float_ptr  ;
   ++ptr.float_ptr ;
   ret+=sizeof(*ptr.float_ptr);
  }
  if(mask & MDBR_FIELD_OFFVALUE)
  {
   rec->off_value = *ptr.float_ptr  ;
   ++ptr.float_ptr ;
   ret+=sizeof(*ptr.float_ptr);
  }

  if(mask & MDBR_FIELD_SCALEID)
  {
   rec->scale_id = *ptr.dw_ptr  ;
   ++ptr.dw_ptr ;
   ret+=sizeof(*ptr.dw_ptr);
  }

  if(mask & MDBR_FIELD_PERCENT)
  {
   rec->percent = *ptr.dw_ptr   ;
   ++ptr.dw_ptr ;
   ret+=sizeof(*ptr.dw_ptr);
  }
  if(mask & MDBR_FIELD_LASTVALUE)
  {
   rec->last_value = *ptr.float_ptr   ;
   ++ptr.float_ptr ;
   ret+=sizeof(*ptr.float_ptr);
   rec->last_time = *ptr.int64_ptr   ;
   ++ptr.int64_ptr ;
   ret+=sizeof(*ptr.int64_ptr);
  }

  if(mask & MDBR_FIELD_SECURITY)
  {
   rec->security = *ptr.dw_ptr  ;
   ++ptr.dw_ptr ;
   ret+=sizeof(*ptr.dw_ptr);
  }


 }
 }
 return ret;

}






