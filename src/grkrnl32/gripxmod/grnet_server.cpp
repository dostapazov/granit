#pragma hdrstop
#include "gripxmod_main.hpp"

  void          __fastcall TGRNetModule::Terminate()
  {
    TThread::Terminate();
    server_icp_port.PostStatus(0,0,0);
  }


  void        __fastcall     TGRNetModule::line_state_changed(DWORD number,DWORD line_state,DWORD changes)
  {
   if(changes&MODEM_LINE_STATE_CONNECT)
     {
      if(line_state&MODEM_LINE_STATE_CONNECT)
        {
         connected_clients++;
         if(diag_table.set_grdiag(0,0x7FFF))
            this->diag_notify(0,1);
         
        }
        else
        {
         if(--connected_clients == 0)
            diag_reset();
        } 
     }

   TModemBase::line_state_changed(number,line_state,changes);
  }

  bool          __fastcall TGRNetModule::create_server_socket()
  {
    bool ret;
    TLockHelper l(locker);
    if( server_socket.GetSocket())
    {
    CancelIo((HANDLE)server_socket.GetSocket());
    server_socket.DestroySocket();
    server_icp_port.Close();
    }
    server_socket.CreateIpxSocket(settings.server_socket,0,WSA_FLAG_OVERLAPPED);
    server_icp_port.AppendFileHandle((HANDLE)server_socket.GetSocket(),GRNET_RX_KEY);
    ret = server_socket.GetSocket() ? true:false;

    return ret;
  }

  bool          __fastcall TGRNetModule::BeforeExecute()
  {
   if(create_server_socket())
     return TGKThread::BeforeExecute();
   return false;  
  }

  int           __fastcall TGRNetModule::Execute()
  {
    char buffer[1024];
    OVERLAPPED rx_ovrlp;
    DWORD key;
    DWORD rx_len;
    LPOVERLAPPED ovrlp;
    ZeroMemory(&rx_ovrlp,sizeof(OVERLAPPED));
    server_socket.Receive(buffer,sizeof(buffer),&rx_ovrlp);
    while(!CheckTerminateRequest())
    {
      if(server_icp_port.GetStatus(key,ovrlp,(10000*(connected_clients ? 1:6)),&rx_len))
      {
       if(key == GRNET_RX_KEY)
         {
           handle_receive((LPGRNET_KADR )buffer,rx_len);
           server_socket.Receive(buffer,sizeof(buffer),&rx_ovrlp);
         }
      }
      else
      {

       if(connected_clients)
       {
        /*Отправить готовности*/
        
        send_line_ready(-1,-1);
       }
       else
       {
        //Таймаут для пересоздания гнезда и icp
        /*Если нет ни одного клиента*/
        create_server_socket();
        ZeroMemory(&rx_ovrlp,sizeof(OVERLAPPED));
        server_socket.Receive(buffer,sizeof(buffer),&rx_ovrlp);
       }
       
       
      }
    }

    diag_reset();
    CancelIo((HANDLE)server_socket.GetSocket());
    server_icp_port.Close();
    server_socket.DestroySocket();
    return 0;
  }

  void          __fastcall TGRNetModule::handle_receive(LPGRNET_KADR kadr,DWORD len)
  {
   if(len && grnet_check(kadr) && !kadr->receipt && kadr->command == CmConnect)
   {
    LPGRNET_BEGIN_CONNECTION bc  = (LPGRNET_BEGIN_CONNECTION)kadr->data;
     lock();
     int cmp_res = strnicmp(bc->server_name,settings.server_name,sizeof(settings.server_name));
     unlock();
   if(cmp_res==0)
    {

    if(bc->client_level <= 1)
    {
       TIpxAddress  client_addr = server_socket.GetRemoteAddress();     
       grnet_line * client = find_client(bc,client_addr);
       if(client)
       {
         client->connect_establish(bc,client_addr,this->is_client_tu_enabled(bc->client_name,htons(client_addr.sa_socket)));
         return;
       }
       else
        bc->error_connection =CONNECT_SPACE_ERROR;

     }
      else
        bc->error_connection = CONNECT_LEVEL_ERROR;

       kadr->receipt = 1;
       grnet_protect(kadr);
       server_socket.Send(kadr,grnet_kadr_len(kadr),0);
     }  
   }
  }

grnet_line * __fastcall TGRNetModule::get_client_byidx(DWORD idx)
{
 grnet_line * ret = dynamic_cast<grnet_line*>(get_line_byindex(idx));
 return ret; 
}

grnet_line *  __fastcall TGRNetModule::find_client(LPGRNET_BEGIN_CONNECTION bc,const TIpxAddress & addr)
{
  grnet_line * ret     ;
  grnet_line * current ;
  DWORD idx = 0;
  DWORD tu_idx = this->find_tu_client(bc->client_name,htons(addr.sa_socket));
  GRNET_CLIENT_INFO ci;
  ci.dw_size = sizeof(ci);

  if(tu_idx<GRNET_MAX_TU_CLIENTS)
    {
      ret =  dynamic_cast<grnet_line*>(get_line_byindex(tu_idx));
      if(ret->is_connected())
      {
        ret->get_client_info(&ci);
//        if(strncmpi(bc->client_name,ci.client_name,sizeof(bc->client_name)) == 0 && memcmp(&addr,&ci.host_addr,sizeof(sockaddr_ipx))==0)
//           return ret;
        if(_strnicoll(bc->client_name,ci.client_name,sizeof(bc->client_name)) == 0 && memcmp(&addr,&ci.host_addr,sizeof(sockaddr_ipx))==0)
           return ret;
      }
      else
      return ret;
    }

   ret = NULL;

  do{
     current = dynamic_cast<grnet_line*>(get_line_byindex(idx));
     if(current )
     {
       grnet_tu_client gtc;
       get_tu_client(&gtc,idx); // Пропуск зарезервированных клиентов под разрешение ТУ
       if(!(*gtc.name))
       {
       if(current->is_connected())
       {
        current->get_client_info(&ci);
        if(memcmp(&addr,&ci.host_addr,sizeof(sockaddr_ipx))==0 &&
            strnicmp(bc->client_name,ci.client_name,sizeof(ci.client_name)) == 0)
            ret = current;
       }
       else
        ret = current;
      
      }
     }
     idx++;
    }while(current && !ret);
    return  ret ;
}

grnet_line * __fastcall TGRNetModule::find_worked_client(char * name,DWORD sock_num)
{
  grnet_line * ret = NULL;
  DWORD idx  = 0;
  if(*name)
  {
  for(ret = get_client_byidx(idx);ret;ret = get_client_byidx(++idx))
  {
   if(ret->is_connected())
   {
        GRNET_CLIENT_INFO ci;
        ci.dw_size = sizeof(ci);
        ret->get_client_info(&ci);
        if(strnicmp(name,ci.client_name,sizeof(ci.client_name)) == 0 && (WORD)sock_num == ci.host_addr.sa_socket)
           break;
   }
  }
  }
  return ret;
}


 LRESULT   __fastcall TGRNetModule::send(LPMPROTO_HEADER mph,DWORD sz)
 {
  /*Отправка данных*/
  LRESULT ret   = GKH_RET_ERROR;
  DWORD   error = 0;
  SetLastError(0);
  if(mph->fa == FA_OTD)
  {
   /*
    Работаем только с OTD
    Данные разбиваются на кадр диагностики и кадр данных если они есть.
   */

   DWORD      op_sz;
   otd_proto  op;
   op.dw_size = sizeof(op);
   if(otd_proto_parser((LPBYTE)mph->data,mph->data_size,&op,&op_sz) &&
      ((DWORD)op.addr->pu == settings.pu_number || settings.pu_number>=OTD_ADDR_MAXVALUE || op.addr->pu == OTD_ADDR_MAXVALUE))
       {
        DWORD begin_line = mph->addr_to.line;
        DWORD end_line   = mph->addr_to.line;
        if(begin_line < MODEM_ADDR_MAXVALUE)
           end_line++;
           else
           {begin_line = 0;end_line = get_lines_limit();}
        while(begin_line<end_line)
        {
         grnet_line * line = dynamic_cast<grnet_line*>(get_line(begin_line++));
         if(line && line->is_connected())
         {
          line->update_modemmer_tx(sz,1);
          ret = send_otd(line,&op);
         }
         else
         error = MDM_ERR_LINE_NOT_CONNECT;
        }
       }
       else
       error = -1;
    
  }
  else
   error =  MDM_ERR_INVALID_FA;
  if(error)
    SetLastError(error);
  return ret;
 }

LRESULT __fastcall TGRNetModule::send_otd(grnet_line * line,lpotd_proto op)
{

 LRESULT ret =  GKH_RET_SUCCESS;
 if( OTD_FA_ISQUERY(op->addr->fa))
    send_otd_query(line,op);
    else
  if(OTD_FA_ISCTRL(op->addr->fa))
     send_otd_ctrl(line,op);
    else
    send_otd_data (line,op);
 if(!line->check_state(MODEM_LINE_STATE_TXREADY,false))
 {   //line->set_state(MODEM_LINE_STATE_TXREADY,false,false);
  SetLastError(MDM_ERR_SEND_BUSY);
 } 
 return  ret;
};

void    __fastcall TGRNetModule::send_otd_query(grnet_line * line,lpotd_proto op)
{
   /*Отправка вызова*/
   DWORD cmd = CmQuery;
   DWORD data_len = 0;
   gratony_kadr kadr;
   kadr.kadr_len = sizeof(kadr);
   kadr.len      = kadr.kadr_len;
   kadr.channel  = line->get_number();
   kadr.InfoType = tpAll;
   if(op->addr->cp< OTD_ADDR_MAXVALUE)
   {
      kadr.numCp   = op->addr->cp;
      kadr.numGrp  = op->addr->sb;
      kadr.data[0] = OTD_FA_ISQUERY_ALL(op->addr->fa) ? -1: op->addr->fa;
      data_len = sizeof(kadr);
      cmd      = CmQuery;
      if(op->addr->sb<OTD_ADDR_MAXVALUE)
      {
       cmd = CmQueryCp;
       if(kadr.data[0]<0xFF)
         cmd = CmQueryFa;
       if(kadr.numGrp<0x7F)
         cmd = CmQueryGroup;
      }
   }

   line->send_data(cmd,&kadr,data_len);

 }

inline void __fastcall set_gran_ts(LPWORD array,DWORD num,WORD val)
{
 div_t dt = div(num,16);
 array+=dt.quot;
 WORD mask = (0x8000>>dt.rem);
 if(val)
  (*array)|=mask;
  else
  (*array)&=~mask;
}



 void    __fastcall TGRNetModule::send_otd_data (grnet_line * line,lpotd_proto op)
 {
  /*Отправка данных  Оперативно технологических*/

    DWORD data_len ;
    BYTE  buffer[256];
    DWORD otd_diag = op->diag ? *op->diag : 0;
    DWORD gr_fa = op->addr->fa ;
    gratony_kadr & kadr = *(lp_gratony_kadr)buffer;
    ZeroMemory(buffer,sizeof(buffer));
    data_len      = sizeof(kadr)+1;
    kadr.kadr_len =                                      
    kadr.len      = data_len-sizeof(kadr.kadr_len);
    kadr.channel  = line->get_number();
    kadr.InfoType = tpReady;
    kadr.numCp    = op->addr->cp;
    kadr.numGrp   = op->addr->sb;
    kadr.P        = ((*op->ver_parts)&OTD_PROTO_FLAG_QUERYRESPOND) ? TRUE:TRUE;
    /*Здесь также может быть и готовность КП*/
    /*ПРи addr.fa == 0x0F*/
    if(op->addr->cp < OTD_ADDR_MAXVALUE && op->addr->sb <OTD_ADDR_MAXVALUE && op->addr->fa <OTD_FA_ALL)
    {
    /*Передача готовности по модулю*/
    /*TODO
      Сделать обновления готовности по изменениям
    */
    
    DWORD diag_change = diag_table.set_otd_diag(op->addr,(otd_diag&OTD_DIAG_MASK));
    if(diag_change )
    {
     //Разослать всем клиентам
     if(diag_table.set_grdiag(0,0x7FFF))
       {
        send_line_ready(-1,0);
        diag_notify(0,1);
       }
        send_line_ready(-1,op->addr->cp);

    }

    if(op->addr->fa<=tpTit && op->data)
    {
    kadr.InfoType = gr_fa;
    if(gr_fa == tpTs)
    {
     LPWORD ts_array = (LPWORD)kadr.data;
     data_len = sizeof(kadr)+7; // 8 байт ТС
     DWORD begin = op->data->numbers.loN;
     DWORD end   = op->data->numbers.hiN+1;
     while(begin<end)
      {
       WORD  val = 0;
       otd_get_value(op->data,begin,&val,sizeof(val));
       set_gran_ts(ts_array,begin,val);
       begin++;
      }

    }
    else
    {
      LPBYTE tit_array = (LPBYTE)kadr.data;
      data_len = sizeof(kadr)+15; // 16 байт ТИТ
      DWORD begin = op->data->numbers.loN;
      DWORD end   = op->data->numbers.hiN+1;
       while(begin<end)
         otd_get_value(op->data,begin++,tit_array++,sizeof(*tit_array));
    }
    kadr.kadr_len =
    kadr.len = data_len-sizeof(kadr.kadr_len);
    line->send_data(CmRetranslation,buffer,data_len+1);//Данные
    }
    if(diag_change)
       diag_notify(op->addr->cp,1);

    if(op->addr->fa<=tpTit && op->personal_diag)
    {
     /*Отправка состояний выбранности ТУ/ТР*/
     DWORD begin = op->personal_diag->numbers.loN;
     DWORD end   = op->personal_diag->numbers.hiN+1;
     object_tu otu;
     ZeroMemory(&otu,sizeof(otu));
     otu.kp = op->addr->cp;
     otu.sb = op->addr->sb;
     otu.tlm_type = op->addr->fa&OTD_FA_DATA_MASK;
     while(begin<end)
      {
       DWORD  val = 0;
       DWORD  cm  = 2;
       otd_get_value(op->personal_diag,begin,&val,sizeof(val));
       otd_get_value(op->personal_chmask,begin,&cm,sizeof(cm));
       begin++;
       otu.obj    = begin;
       DWORD net_cmd = 0;

       if(cm)
       {

       if((val&OTD_PSTATE_TUTR_MASK))
       {

       if(
          ((val&OTD_PSTATE_TUTR_MASK) == OTD_PSTATE_TUTR_PREPARE)
           &&
           !line->get_tuobj_active_state(*op->addr,begin) // если еще не в списке
           )
          {
           net_cmd = CmBeginTU;
           //Внести в список активных ТУ
           line->set_tuobj_active_state(*op->addr,begin,OTD_PSTATE_TUTR_PREPARE);
          }
       }
       else
        {
        if(val & (OTD_PDIAG_TUTR_FAIL|OTD_PDIAG_TUTR_DESCRIPT))
           line->send_data(CmTuFalse,&otu,sizeof(otu));
         if(!(cm&1)
           &&
           line->get_tuobj_active_state(*op->addr,begin) // если уже в списке
           )
         {
            //Только если есть в списке
            net_cmd = CmResetTU;
            line->set_tuobj_active_state(*op->addr,begin,0);
         }
        }
       }
       else
       {
        /*if(val & (OTD_PDIAG_TUTR_ERROR|OTD_PDIAG_TUTR_DESCRIPT))
           line->send_data(CmTuFalse,&otu,sizeof(otu));*/
       }
       if(net_cmd)
         line->send_data(net_cmd,&otu,sizeof(otu));
      }
    }
    return ;
   }

   if(op->diag )
   {
    send_common_diag(op->addr->cp,*op->diag);
   }
 }

 void   __fastcall TGRNetModule::send_common_diag(DWORD cp,DWORD diag)
 {

   diag&=(OTD_DIAG_NODATA|OTD_DIAG_PUCONNECT|OTD_DIAG_PURESPOND|OTD_DIAG_CPCONNECT|OTD_DIAG_CPRESPOND);
   if(diag&OTD_DIAG_MASK)
   {
   DWORD end_cp = cp;
   if(cp<OTD_ADDR_MAXVALUE)
     end_cp++;
     else
     cp = 1 ,end_cp = MAX_CP_COUNT;
     grdiag_item gdi;
   for(;cp<end_cp;cp++)
   {
     DWORD dc;
     if(!diag)
       {
        diag_table.get_grdiag(cp,&gdi);
        dc = diag_table.set_grdiag(cp,(gdi.state&=~GRDIAG_CP));
       }
       else
       dc = diag_table.set_grdiag(cp,-1);
     if(dc)
       {
        send_line_ready(-1,cp,true);
        diag_notify(cp,1);
       }
   }
   }
 }


 void    __fastcall TGRNetModule::send_otd_ctrl (grnet_line * line,lpotd_proto op)
 {
  //Отправка управления
  otd_tutr tutr;
  if(op->data)
  {
    DWORD beg = (DWORD)op->data->numbers.loN,end = (DWORD)op->data->numbers.hiN;
     object_tu otu;
     ZeroMemory(&otu,sizeof(otu));
     otu.kp = op->addr->cp;
     otu.sb = op->addr->sb;
     otu.tlm_type = op->addr->fa&OTD_FA_DATA_MASK;

    while(beg<=end)
    {
     otd_get_value(op->data,beg++,&tutr,sizeof(tutr));
     if(tutr.command&OTD_TUTR_CMDENABLE)
     {
      otu.obj = beg;
      line->send_data(CmEnableTU,&otu,sizeof(otu));
     } 
    }
  }
 }


void    __fastcall TGRNetModule::send_line_ready(DWORD line_num,DWORD cp,bool always)
{
      DWORD begin_line =  line_num,end_line   =  line_num;
      
      if(begin_line<MODEM_ADDR_MAXVALUE)
         end_line++;
         else
         begin_line = 0,end_line = this->get_lines_count();

     for(;begin_line<end_line;begin_line++)
     {
      lock();
      send_line_ready((grnet_line *)get_line(begin_line),cp,always);
      unlock();
     }
}

void    __fastcall TGRNetModule::send_line_ready(grnet_line * line,DWORD cp,bool always)
{
      if(line && line->check_state(MODEM_LINE_STATE_CONNECT,false))
         line->send_ready(cp,always);
}


 LRESULT __fastcall TGRNetModule::get_tu_client(LPGRNET_TU_CLIENT gtc,DWORD idx)
 {
  LRESULT ret = GKH_RET_ERROR;
  gtc->name[0]  = 0;
  gtc->sock_num = 0;
  TLockHelper l(locker);
  if(idx<GRNET_MAX_TU_CLIENTS)
     {
        grnet_tu_client tc(tu_clients[idx]);
        safe_strcpyn(gtc->name,tc.name,sizeof(gtc->name));
        gtc->sock_num = tc.sock_num;
      ret = GKH_RET_SUCCESS;
     }
  return ret;
 }

 LRESULT __fastcall TGRNetModule::set_tu_client(LPGRNET_TU_CLIENT gtc,DWORD idx)
 {
  LRESULT ret = GKH_RET_ERROR;

  if(idx<GRNET_MAX_TU_CLIENTS && find_tu_client(gtc->name,gtc->sock_num)>GRNET_MAX_TU_CLIENTS)
     {
      grnet_line * line = find_worked_client(tu_clients[idx].name,htons((WORD)tu_clients[idx].sock_num));
      if(line)
         line->disconnect();
      lock();
      tu_clients[idx] = grnet_tu_client (gtc->name,gtc->sock_num);
      ret = GKH_RET_SUCCESS;
      cc_flags |=CCFL_TU_CLIENTS;
      unlock();
      notify(MNF_MODEM,GRNET_NCODE_TUCLIENTS,&idx,sizeof(idx));
      line = find_worked_client(tu_clients[idx].name,htons((WORD)tu_clients[idx].sock_num));
      if(line)
         line->disconnect();
     }
  return ret;
 }

 LRESULT __fastcall TGRNetModule::del_tu_client(DWORD idx)
 {
  return set_tu_client(&grnet_tu_client(),idx);
 }

 DWORD   __fastcall TGRNetModule::find_tu_client(char * name,DWORD sock_num)
 {

  grnet_tu_client tc(name,sock_num);
  DWORD   ret = -1;
  TLockHelper l(locker);
  for(int i = 0;i<GRNET_MAX_TU_CLIENTS && ret>GRNET_MAX_TU_CLIENTS;i++)
   {
    if(tc.name[0] && tc.compare(this->tu_clients[i])==0)
        ret = i;
   }
  return ret;

 }

 BOOL __fastcall TGRNetModule::is_client_tu_enabled(char * name,DWORD sock_num)
 {
    return find_tu_client(name,sock_num) < GRNET_MAX_TU_CLIENTS  ? true:false;
 }



