#pragma hdrstop
#include "modbusmod.hpp"
#include <functional>


  __fastcall modbus_line::modbus_line(LPMODBUS_POINT mdev):modem_line(mdev ? mdev->point_no : -1)
  {

   memset(&modbus_point,0,sizeof(modbus_point));
   modbus_point.dw_size = sizeof(modbus_point);
   if(mdev)
      memcpy(&modbus_point,mdev,sizeof(modbus_point));
   term_event = NULL;
   line_event = NULL;
   socket     = NULL;
   comm_port  = NULL;
   tx_queue   = NULL;
  }

  void  __fastcall modbus_line::do_report(DWORD evt_type,wchar_t * rep_text,LPVOID data,DWORD dt_sz)
  {
    TModbusModem * mm = dynamic_cast<TModbusModem *>(owner);
     if(mm)
      mm->do_report(evt_type,rep_text,data,dt_sz);
  }

  DWORD __fastcall modbus_line::get_line_text     (wchar_t * text,DWORD text_sz)
  {

    wchar_t tmp[MAX_PATH] = {0};
    wchar_t line_str[MAX_PATH];
    if(!owner->get_lang_string(IDS_LINE_TEMPL,line_str,KERTL_ARRAY_COUNT(line_str)))
       {
        Ansi2Unicode(line_str,"Устройство ");
        //safe_strcpy(line_str,L"Устройство ");
       }
    DWORD len = swprintf(tmp,text_sz-1,L"%s № %d",line_str,get_number());
    tmp[len++] = 0;
    len = KeRTL::MIN(len,text_sz);
    wcsncpy(text,tmp,len);
    return len;
  }

 void        __fastcall modbus_line::do_recv_group(lpotd_proto op,DWORD parts,DWORD lo_num,DWORD hi_num,DWORD to)
 {
          BYTE buff[2048];
          otd_proto_param opp;
          opp.dw_size = sizeof(opp);
          otd_proto_getparam(op,&opp);
          opp.parts  = parts;
          opp.pd_param.first = opp.cm_param.first = opp.dt_param.first = lo_num;
          opp.pd_param.count = opp.cm_param.count = opp.dt_param.count = hi_num-lo_num+1;
          DWORD len = otd_proto_getparts_ex(buff,sizeof(buff),&opp,op,0);
          if(!len)
              len = otd_proto_getparts_ex(buff,sizeof(buff),&opp,op,0);
          sotd_proto sop(buff,len);
          sop.op.addr->cp = get_number();
          if(len)
             queue_rxdata(FA_OTD,buff,len,false,to);
 }

 #pragma warn -8057
  bool  __fastcall modbus_line::send        (LPMPROTO_HEADER mph,DWORD sz)
  {
    if(mph->fa == FA_OTD)
    {
      update_modemmer_tx(sz,1);
      sotd_proto sop((LPBYTE)mph->data,mph->data_size);
      if(OTD_ADDR_ISQUERY(sop().addr) && ((DWORD)sop.op.addr->cp == this->modbus_point.point_no || sop.op.addr->cp == OTD_ADDR_MAXVALUE) )
      {
        //если вызов
         DWORD cp_diag = 0;
         DWORD parts = ((*sop.op.ver_parts)&OTD_PROTO_PARTSMASK)|OTD_PROTO_PART_KPK;
         TLockHelper l(locker);
         proto_pointer b_ptr = groups.begin(),e_ptr = groups.end();
         sotd_addr sa(*sop().addr);
         sa.pu = 0;
         sa.cp = modbus_point.point_no;
         sa.fa&=OTD_FA_ALL;
         groups.get_range(sa,b_ptr,e_ptr);
         while(b_ptr<e_ptr)
         {
          do_recv_group(&b_ptr->op,parts,0,-1,mph->addr_from.addr);
          if(*(b_ptr->op.diag)&OTD_DIAG_MASK)
             cp_diag|= OTD_PART_DIAG_PARAM;
          ++b_ptr;
         }

        if((sop.op.addr->fa&OTD_FA_ALL)==OTD_FA_ALL)
        {
         //вызов по всему КП
         if(!this->is_connected())
           cp_diag|=OTD_DIAG_CPCONNECT;
         if(owner)
           {
            TModbusModem * modem = dynamic_cast<TModbusModem*>(owner);
            if(!modem || !modem->is_running())
               cp_diag|= OTD_DIAG_PUCONNECT;
           }
         BYTE buff[1024];
         sotd_proto sop;
         DWORD len = otd_proto_format(buff,sizeof(buff),parts,0,0,0,0,0,
                                      (parts&OTD_PROTO_PART_NAME)? lstrlenW(modbus_point.point_name):0,&sop.op);
         if(sop.op.diag)
            *sop.op.diag = cp_diag;
         if(sop.op.name )
           otd_text_set(sop.op.name,modbus_point.point_name);
         sop.op.addr->addr = -1;
         sop.op.addr->pu   = 0;
         sop.op.addr->cp   = modbus_point.point_no;
         otd_proto_protect(&sop.op,len,OTD_DEF_POLINOM);
         queue_rxdata(FA_OTD,buff,len,false,mph->addr_from.addr);
        }
      }
      else
       {

         if(OTD_FA_ISCTRL(sop().addr->fa))
         {
          otd_addr addr(*sop.op.addr);
          addr.fa &=OTD_FA_DATA_MASK;
          addr.pu = 0;
          TLockHelper l(locker);
          proto_pointer p = groups.find(addr);
          if(p<groups.end())
          {
            LPMODBUS_GROUP  grp = (LPMODBUS_GROUP)p->op.extension->block_data;

            lpotd_tutr tutr = (lpotd_tutr) sop.op.data->data;
            BYTE tmp[128];
            int command,param;
            if(addr.fa == OTD_FA_DISCRETE )
            {
             command = MODBUS_FORCE_SINGLE_COIL ;
             param   = tutr->command == OTD_TUOP_ON ? MODBUS_FORCE_ON : MODBUS_FORCE_OFF;
            }
            else
            {
             command = MODBUS_PRESET_SINGLE_REGISTER;
             param   = tutr->param;
            }
            int need_len = 0;
            int len = modbus_format_preset(tmp,sizeof(tmp),grp->dev_no,command,sop.op.data->numbers.loN,param,&need_len);
            tx_queue->PutIntoQueue(tmp,len,true);
          }
         }
        //unlock();
       }
   }
   return true;
  }

 #pragma warn .8057

  void  __fastcall modbus_line::get_point(LPMODBUS_POINT mpt)
  {
   memcpy(mpt,&modbus_point,sizeof(*mpt));
  }

  void _write_string(TStream &s,const wchar_t * str)
  {
  DWORD len = str ? lstrlenW(str):0;
  s.Write(&len,sizeof(len));
  s.Write((void*)str,len*sizeof(wchar_t));
 }

 int _read_string (TStream& s,wchar_t *& str)
 {
     DWORD len(0);
     s.Read(&len,sizeof(len));
     if(str == 0 && len)
       str = new wchar_t[(len+1)];
     if(len)
     {
      s.Read(str,len*sizeof(wchar_t));
      str[len] = 0;
     }
     else
     str = 0;
     return len;
 }

    #define MBLINE_STREAM_VER 1

     void  __fastcall modbus_line::write (TStream & s)
     {
       s<<(DWORD)MBLINE_STREAM_VER;
       TLockHelper l(locker);
       s.Write(&modbus_point,sizeof(modbus_point));
       s<<(DWORD)groups.size();
       proto_pointer b = groups.begin(),e = groups.end();
       while(b<e)
       {
        LPMODBUS_GROUP mg = (LPMODBUS_GROUP)b->op.extension->block_data;
        s<<mg->group_no<<mg->command<<mg->dev_no<<mg->start_no<<mg->count<<mg->refresh_time<<mg->respond_time;
        ++b;
       }
     }


     void  __fastcall modbus_line::read  (TStream & s)
     {
       DWORD sver = 0;
       s>>sver;
       TLockHelper l(locker);
       s.Read(&modbus_point,sizeof(modbus_point));
       DWORD cnt = 0;
       s>>cnt;
       MODBUS_GROUP mg ;
       mg.dw_size = sizeof(mg);
       mg.point_no  = modbus_point.point_no;
       for(DWORD i = 0;i< cnt;i++)
       {
         s>>mg.group_no>>mg.command>>mg.dev_no>>mg.start_no>>mg.count>>mg.refresh_time>>mg.respond_time;
         mg.point_no = modbus_point.point_no;
         group_create(&mg);
       }
       set_line_number(modbus_point.point_no);
       set_diag(OTD_DIAG_NODATA|OTD_DIAG_CPCONNECT,false);
     }

     bool  __fastcall modbus_line::set_config(LPMODBUS_POINT pt)
     {
      bool restart = false;
      bool upd_name     = false;

      {
      TLockHelper l(locker);
      if(wcscmp(pt->point_name,modbus_point.point_name))
          upd_name = true;
      if(modbus_point.point_no != pt->point_no)
          restart = true;
      if(modbus_point.media_type ==MODBUS_MEDIA_TYPE_COMPORT  && memcmp(&modbus_point.comport_settings,&pt->comport_settings,sizeof(modbus_point.comport_settings)))
         restart = true;

      if((modbus_point.media_type == MODBUS_MEDIA_TYPE_ETHERNET || modbus_point.media_type == MODBUS_MEDIA_TYPE_COMPORT_VIA_ETHERNET) &&
         (wcscmp(modbus_point.host_params.host_addr,pt->host_params.host_addr) || modbus_point.host_params.host_port != pt->host_params.host_port)
         )
         restart = true;

      if(pt->media_type != modbus_point.media_type)
           restart = true;
      }

      bool wrk = is_worked();
      DWORD tid = GetID();
      if(wrk && restart)
      {
       if(tid!=GetCurrentThreadId())
           {
            int lc =owner->full_unlock();
            stop();
            owner->restore_lock(lc);
           }
      }

      memcpy(&modbus_point,pt,sizeof(modbus_point));
      Sleep(100);

      if(wrk)
      {
          if(restart)
          {
           if(tid!=GetCurrentThreadId())
               start();
          }
          else
          {
          if(upd_name)
             do_recv_point_name();
          }
      }
      return true;
     }

     LRESULT __fastcall modbus_line::group_enum   (LPMODBUS_GROUP mbg,DWORD idx)
     {
        LRESULT ret = GKH_RET_ERROR;
        if(idx>=this->groups.size())
            SetLastError(ERROR_MORE_DATA);
           else
           {
             ret = GKH_RET_SUCCESS;
             proto_pointer p = groups.begin();
             std::advance(p,idx);
             LPMODBUS_GROUP  grp =(LPMODBUS_GROUP)p->op.extension->block_data;
             memcpy(mbg,grp,sizeof(*mbg));
           }

        return  ret;
     }

     LRESULT __fastcall modbus_line::do_create_group(LPMODBUS_GROUP mbg)
     {                                                   LRESULT ret = GKH_RET_ERROR;

         sotd_addr addr(0,modbus_point.point_no,get_otd_fa(mbg->command),mbg->group_no);
         otd_proto_param opp = {0};
         opp.dw_size = sizeof(opp);
         opp.parts = OTD_PROTO_PART_KPK |
                     OTD_PROTO_PART_DIAG|
                     OTD_PROTO_PART_DATA|
                     OTD_PROTO_PART_PERSONAL_DIAG |
                     OTD_PROTO_PART_PERSONAL_CHMASK|
                     OTD_PROTO_PART_TIMESTAMP|
                     OTD_PROTO_PART_PROTOEXTENSION;
        opp.dt_param.type  = addr.fa == OTD_FA_DISCRETE ? OTD_DISCRETE : OTD_ANALOG_SHORT;
        opp.pd_param.type  = OTD_BYTE;
        opp.cm_param.type  = OTD_DISCRETE;
        opp.dt_param.first = opp.pd_param.first = opp.cm_param.first = mbg->start_no;
        opp.dt_param.count = opp.pd_param.count = opp.cm_param.count = mbg->count;
        opp.ext_size       = sizeof(*mbg);
        DWORD nsz = otd_proto_calc_size_ex(&opp,0);
        LPBYTE buf = new BYTE[nsz];
        if(buf)
        {
         ret = GKH_RET_SUCCESS;
         sotd_proto sop;
         otd_proto_format_ex(buf,nsz,&opp,0,&sop.op);
         sop.op.addr->addr    = addr.addr;
         LPMODBUS_GROUP  grp =(LPMODBUS_GROUP)sop.op.extension->block_data;
         memcpy(grp,mbg,sizeof(*grp));
         grp->point_no = modbus_point.point_no;
         grp->requested = FALSE;
         grp->timer    = 0;
         *sop.op.diag = OTD_DIAG_PUMASK|OTD_DIAG_CPMASK;


         proto_pointer ptr = groups.insert(sop());
         set_diag(ptr,-1,OTD_DIAG_MODRESPOND,OTD_PDIAG_NODATA,false);
          if(owner)
          {
           DWORD n_data = MAKELONG (modbus_point.point_no,grp->group_no);
           owner->notify(MBNF_DEV_CHANGED,MBNC_GROUP_CREATED,&n_data,sizeof(n_data));
          }

        }
        else
        SetLastError(E_OUTOFMEMORY);
       return ret;
     }

     LRESULT __fastcall modbus_line::group_create (LPMODBUS_GROUP mbg)
     {
        LRESULT ret = GKH_RET_SUCCESS;
        //создание группы
        sotd_addr addr(0,mbg->point_no,get_otd_fa(mbg->command),mbg->group_no);
        TLockHelper l(locker);
        proto_pointer ptr = groups.find(addr);
        if(ptr<groups.end())
          owner->SetLastError(MBERR_GROUP_ALREADY_EXIST);
          else
          {
           ret = do_create_group(mbg);
          }

        return  ret;
     }

     LRESULT __fastcall modbus_line::group_get    (LPMODBUS_GROUP mbg)
     {
        LRESULT ret = GKH_RET_SUCCESS;
        sotd_addr addr(0,mbg->point_no,get_otd_fa(mbg->command),mbg->group_no);
        TLockHelper l(locker);
        proto_pointer ptr = groups.find(addr);
        if(ptr<groups.end())
        {
         LPMODBUS_GROUP  _mbg = (LPMODBUS_GROUP) ptr->op.extension->block_data;
         memcpy(mbg,_mbg,sizeof(*mbg));

        }
        else
        {
         ret = false;
         owner->SetLastError(MBERR_GROUP_NOT_EXIST);
        }

        return  ret;
     }

     LRESULT __fastcall modbus_line::group_update (LPMODBUS_GROUP mbg)
     {
        LRESULT ret = GKH_RET_ERROR;
        sotd_addr addr(0,mbg->point_no,get_otd_fa(mbg->command),mbg->group_no);
        TLockHelper l(locker);
        proto_pointer ptr = groups.find(addr);
        if(ptr<groups.end())
        {
         LPMODBUS_GROUP  _mbg = (LPMODBUS_GROUP) ptr->op.extension->block_data;
         if(_mbg->start_no != mbg->start_no || _mbg->count!= mbg->count)
           {
            //Удалить группу
            group_delete(mbg);
            ret = group_create(mbg);
            //Создать группу
           }
           else
           {

            if(_mbg->dev_no != mbg->dev_no || _mbg->refresh_time != mbg->refresh_time || _mbg->respond_time != mbg->respond_time)
               {
                ret = GKH_RET_SUCCESS;
                _mbg->dev_no = mbg->dev_no;
                _mbg->refresh_time = mbg->refresh_time;
                _mbg->respond_time = mbg->respond_time;
                _mbg->timer        = 0;
               }
           }


        }
        else
        SetLastError(MBERR_GROUP_NOT_EXIST);
        return  ret;
     }

     WORD  __fastcall modbus_line::get_otd_fa   (DWORD reg_no)
     {
        WORD ret = OTD_FA_FROM_MODBUS_CMD(reg_no);
         switch(reg_no )
         {
          case MODBUS_READ_DISCRETE_OUTPUT:
          case MODBUS_READ_DISCRETE_INPUT :
               ret = OTD_FA_DISCRETE;
          break;
          case MODBUS_READ_HOLDING_REGISTERS :
          case MODBUS_READ_INPUT_REGISTERS   :
               ret = OTD_FA_ANALOG;
          break;

         }
       return ret;
     }

     LRESULT __fastcall modbus_line::group_delete    (LPMODBUS_GROUP mbg)
     {
        LRESULT ret = GKH_RET_SUCCESS;
        sotd_addr addr(0,mbg->point_no,get_otd_fa(mbg->command),mbg->group_no);
        TLockHelper l(locker);
        proto_pointer ptr = groups.find(addr);

        if(ptr<groups.end())
        {
          //TODO
          //Отправить диагностику
          if(requested == &ptr->op)
             requested = NULL;
          if(this->is_worked())
          {
           *ptr->op.diag = OTD_DIAG_REMOVED;
           BYTE pd = OTD_PDIAG_NODATA;
           otd_fill(ptr->op.personal_diag,&pd,sizeof(pd));
           do_recv_group(&ptr->op,OTD_PROTO_PART_DIAG|OTD_PROTO_PART_PERSONAL_DIAG,0,-1);
          }
          groups.erase(ptr);
        }
        else
        {
         ret = GKH_RET_ERROR;
         owner->SetLastError(MBERR_GROUP_NOT_EXIST);
        }
        return  ret;
     }

     void  __fastcall modbus_line::next_refresh_from(proto_pointer & p)
     {
       TLockHelper l(locker);
       proto_pointer e = groups.end();
       ++p;
       if(p>=e)
          p = groups.begin();
       while(p<e)
       {
        LPMODBUS_GROUP grp = (LPMODBUS_GROUP)p->op.extension->block_data;
        if(grp && grp->timer<=0)
           {
           refresh(p);
           p = e;
           }
           else
           ++p;
       }

     }

     void  __fastcall modbus_line::on_timer   ()
     {
      if(is_connected())
      {
       TLockHelper l(locker);
       if(requested)
       {
          //Вызов отправлен ждем
          lpotd_proto  op = requested;
          LPMODBUS_GROUP grp = (LPMODBUS_GROUP)op->extension->block_data;
          grp->timer -= TIMER_RESOLUTION;
          if(grp->timer<=0)
          {
           //Нет ответа на вызов
           set_diag(op,OTD_DIAG_MODRESPOND,-1,0,true);
           grp->requested = 0;
           grp->timer = grp->refresh_time;
           requested = NULL;
           update_stats_rx(0,0,1);
           if(tx_queue->QueueCount())
              tx_queue->GetEvent().SetEvent(true);
           proto_pointer p = groups.find(*op->addr);
           next_refresh_from(p);
          }
       }

         proto_pointer p = groups.begin(),e = groups.end();
         while( p < e)
         {
          LPMODBUS_GROUP grp = (LPMODBUS_GROUP)p->op.extension->block_data;
          if(!grp->requested)
          {
           if(grp->timer<=0)
           {
            if(!requested)
                refresh(p);
                else
                grp->timer = 0;
           }
           else
            grp->timer -= TIMER_RESOLUTION;
          }
          ++p;
         }
        }

     }

     void  __fastcall modbus_line::on_connect   ()
     {
       modem_line::on_connect();
       watch_dog_time  = 0;
       watch_dog_timer = 5000;

       if(tx_queue )tx_queue->DropData();
       wchar_t buff[MAX_PATH];
       swprintf_s(buff,sizeof(buff),L"Соединение установлено модем %d линия %d",(int)owner->get_modem_number(),(int)get_number());
       do_report(REPORT_SUCCESS_TYPE,buff,0,0);

       frame_number = 0;

       set_state(MODEM_LINE_STATE_TXREADY,true);
       rxb.clear();
       TLockHelper l(locker);
       proto_pointer p = groups.begin(), e = groups.end();
       requested = NULL;
       while(p<e)
       {
         LPMODBUS_GROUP grp =(LPMODBUS_GROUP) p->op.extension->block_data;
         grp->requested = false;
         grp->timer     = 0;
         ++p;
       }
     }

     void  __fastcall modbus_line::on_disconnect()
     {
       set_diag(OTD_DIAG_NODATA|OTD_DIAG_CPCONNECT,true);
       set_state(MODEM_LINE_STATE_CONNECT|MODEM_LINE_STATE_TXREADY,false);
       watch_dog_time  = 0;
       watch_dog_timer = 2000;
       if(tx_queue )
       {
        tx_queue->DropData();
       }
       modem_line::on_disconnect();
       requested = NULL;
        wchar_t buff[MAX_PATH];
        swprintf_s(buff,sizeof(buff),L"Соединение разорвано модем %d линия %d",(int)owner->get_modem_number(),(int)get_number());
        do_report(REPORT_ERROR_TYPE,buff,0,0);


     }



    DWORD __fastcall modbus_line::refresh(DWORD sbl)
    {
      proto_pointer p;
      TLockHelper l(locker);
      if(sbl == DWORD(-1))
      {
       p = groups.begin();
       proto_pointer e = groups.end();
       while(p<e)
       {
         LPMODBUS_GROUP grp = (LPMODBUS_GROUP)p->op.extension->block_data;
         grp->timer = 0;
         grp->requested = false;
         if(!requested)
             refresh(p);
         ++p;
       }
      }
      else
      {
       if(sbl<groups.size())
          {
            p = groups.begin();
            std::advance(p,sbl);
            LPMODBUS_GROUP grp = (LPMODBUS_GROUP)p->op.extension->block_data;
            grp->timer = 0;
            grp->requested = false;
            if(!requested)
               refresh(p);
          }
      }
      return GKH_RET_SUCCESS;
    }

     void  __fastcall modbus_line::refresh(proto_pointer & p)
     {
         //Отправить вызов на устройство
       if(!requested && p < groups.end())
       {
        LPMODBUS_GROUP grp = (LPMODBUS_GROUP) p->op.extension->block_data;
        if(grp )
        {
         grp->requested = true;
         char temp[1024];
         int need_sz = 0;
         int len = modbus_format_query(temp,KERTL_ARRAY_COUNT(temp),grp->dev_no,grp->command,grp->start_no,grp->count,&need_sz);
         grp->timer = grp->respond_time;
         grp->requested = 1; // Установить признак отправки вызова
         tx_queue->PutIntoQueue(temp,len);
         requested = &p->op;
        }
       }
     }


    int   __fastcall modbus_line::get_term_event_name(TGKModule * mod,wchar_t * name,int sz)
    {
     return swprintf(name,sz,MODBUS_TERMINATE_EVENT,(DWORD)(*mod)());
    }

     bool  __fastcall modbus_line::BeforeExecute     ()
     {
       bool   ret = TGKThread::BeforeExecute();
       requested = NULL;
       wchar_t event_name[MAX_PATH];
       get_term_event_name(owner,event_name,KERTL_ARRAY_COUNT(event_name));
       term_event = new TEvent(true,false,event_name);
       tx_queue   = new TEventSimpleDataQueue;
       if(term_event && term_event->IsValid())
       {
       if(modbus_point.media_type)
          ret = before_socket_execute();
          else
          ret = before_comport_execute();
       }
       return ret;
     }

     void  __fastcall modbus_line::BeforeTerminate   ()
     {
       set_state(MODEM_LINE_STATE_WORKED,false);
       if(modbus_point.media_type)
          before_socket_terminate();
          else
          before_comport_terminate();
       if(term_event)
          delete term_event;
       term_event = 0;
       delete tx_queue;
       tx_queue = NULL;
       requested = NULL;
       TGKThread::BeforeTerminate();
     }

     int   __fastcall modbus_line::Execute           ()
     {
       int ret;
       set_state(MODEM_LINE_STATE_WORKED,true);
       if(modbus_point.media_type)
          ret = socket_execute ();
          else
          ret = comport_execute();
      return ret;
     }

 bool  __fastcall modbus_line::do_start()
 {
  //SetFlags(KERTL_THREAD_ENABLERESTART,true);
  return Start(0x2000);
 }

 bool  __fastcall modbus_line::do_stop ()
 {
  SetFlags(KERTL_THREAD_ENABLERESTART,false);
  return TerminateAndWait(3000,true);
 }



     bool  __fastcall modbus_line::socket_connect   ()
     {
       socket_disconnect();
       set_state(MODEM_LINE_STATE_CONNECTING,true);
       line_event = new TEvent(WSACreateEvent(),false);
       socket     = new TIPSocket(true);
       socket->EnableReuseAddress(true);
       if(line_event && line_event->IsValid() && socket && socket->GetSocket())
       {
        WSAEventSelect(socket->GetSocket(),(*line_event)(),FD_READ|FD_CLOSE|FD_WRITE|FD_CONNECT);
        char text[MAX_PATH];

        Unicode2Ansi(text,modbus_point.host_params.host_addr);
        TIPAddress addr(text,modbus_point.host_params.host_port);
        if(socket->Connect(addr(),sizeof(addr)))
        return true;
        else
        return socket->GetLastError()  == WSAEWOULDBLOCK ? true : false;
       }
       return false;
     }

     void  __fastcall modbus_line::socket_disconnect()
     {
       set_state(MODEM_LINE_STATE_DISCONNECTING,true);
       if(socket)
         {
           socket->Disconnect();
           socket->DestroySocket();
           delete socket;
           socket = NULL;
         }
       if(line_event)
          {
           delete line_event;
           line_event = 0;
          }
        watch_dog_time  = 0;
        watch_dog_timer = 2000;
     }

     bool  __fastcall modbus_line::before_socket_execute ()
     {
       if(TBaseSocket::InitWS(MAKEWORD(2,2)))
          return socket_connect();
        return false;
     }

     void  __fastcall modbus_line::before_socket_terminate ()
     {
       socket_disconnect();
       TBaseSocket::DeinitWS();
     }

     void  __fastcall modbus_line::socket_handle_event()
     {
      WSANETWORKEVENTS ne={0};
     while(socket &&  !WSAEnumNetworkEvents(socket->GetSocket(),(*line_event)(),&ne) && ne.lNetworkEvents)
     {

      watch_dog_time = 0;
      if(ne.lNetworkEvents&FD_CONNECT)
         {
          socket_handle_connect(ne.iErrorCode[FD_CONNECT_BIT]);
          ne.lNetworkEvents&=~(FD_CONNECT|FD_WRITE);
         }


      if(ne.lNetworkEvents&FD_CLOSE)
      {
        socket_handle_disconnect(ne.iErrorCode[FD_CLOSE_BIT]);
        ne.lNetworkEvents&=~FD_CLOSE;
      }

      if(ne.lNetworkEvents&FD_READ)
        {
         socket_handle_recv(ne.iErrorCode[FD_READ_BIT]);
         ne.lNetworkEvents&=~FD_READ;
        }

       if(ne.lNetworkEvents&FD_WRITE)
       {
        socket_handle_send(ne.iErrorCode[FD_WRITE_BIT]);
        ne.lNetworkEvents&=~FD_WRITE;
       }

      if(ne.lNetworkEvents)
      {
       ne.lNetworkEvents = 0;
      }
     }
 }

 void  __fastcall modbus_line::socket_handle_connect   (DWORD err)
 {
   if(err)
      {
       char text[1024];
       GetErrorMessage(err,text,sizeof(text),(HINSTANCE)NULL);
       GetErrorMessage(err,text,sizeof(text),(HINSTANCE)NULL);
       socket_handle_disconnect(err);
      }
      else
      connect(true);
 }

     void  __fastcall modbus_line::socket_handle_disconnect(DWORD err)
     {
      err = err;
      socket_disconnect();
      connect(false);
     }

     void  __fastcall modbus_line::do_socket_send          (LPVOID buf,int len,DWORD notify)
     {
            int send_len = socket->Send(buf,len,0);
            if(send_len == len || socket->GetLastError() == WSAEWOULDBLOCK)
            {
               update_stats_tx(len,1,0);
               if(this->modbus_point.media_type == MODBUS_MEDIA_TYPE_COMPORT_VIA_ETHERNET)
               Sleep(KeRTL::MAX(DWORD(10), modbus_point.comport_settings.port_delay));
                else
               Sleep(1);
               owner->notify(MBNF_TXRX,notify,buf,len);
            }
            else
            socket_handle_disconnect(socket->GetLastError());
     }

     void  __fastcall modbus_line::socket_send_queue       ()
     {

        tx_queue->GetEvent().SetEvent(false);
        BYTE tmp[2048];
        LPMODBUS_TCP_FRAME mtf =(LPMODBUS_TCP_FRAME)tmp;
        ++frame_number;
        mtf->hdr.frame_number = frame_number;
        mtf->hdr.proto_id = 0;

        int len = 0;
        //if(tx_queue->PeekFromQueue(&mtf->modbus_frame,sizeof(tmp)-sizeof(mtf->hdr),&len))
        if(tx_queue->GetFromQueue(&mtf->modbus_frame,sizeof(tmp)-sizeof(mtf->hdr),&len))
        {
         mtf->hdr.frame_len = modbus_word(len);
         if(socket)
           {
            switch(modbus_point.media_type )
            {
             case MODBUS_MEDIA_TYPE_COMPORT_VIA_ETHERNET:
                  modbus_protect_crc(&mtf->modbus_frame,len,sizeof(tmp)-sizeof(mtf->hdr)-len,&len);
                  do_socket_send(&mtf->modbus_frame,len,MBNC_TX);
             break;
             case MODBUS_MEDIA_TYPE_ETHERNET:
                   len = modbus_get_tcp_length(mtf);
                   do_socket_send(mtf,len,MBNC_TXTCP);
             break;
            }
           }
        }
     }

     wchar_t * __fastcall get_modbus_error_text(int e_code)
     {
      static wchar_t * errors_name[] =
      {
       L"?Unknown? "
       ,L"ILLEGAL_FUNCTION"
       ,L"ILLEGAL DATA_ADDRESS"
       ,L"ILLEGAL_DATA_VALUE"
       ,L"SLAVE_DEVICE_FAILURE"
       ,L"ACKNOWLEDGE"
       ,L"SLAVE_DEVICE_BUSY"
       ,L"NEGATIVE_ACKNOWLEDGE"
       ,L"MEMORY_PARITY_ERROR"
      };

      return errors_name[e_code<9 ?e_code : 0];

     }

      bool  __fastcall modbus_line::handle_modbus_respond(LPMODBUS_FRAME mf,DWORD len)
      {
         //tx_queue->DropFirst();

         int resp_err = modbus_is_respond_error(mf);
         if(mf->data.command&MODBUS_RESPOND_ERROR_FLAG)
           {
            resp_err = (int)mf->data.data[0];
           }
         watch_dog_time = 0;
         len = modbus_get_respond_length(mf);
         TLockHelper l(locker);
         lpotd_proto op =requested;
         if(op )
         {

          LPMODBUS_GROUP grp =(LPMODBUS_GROUP) op->extension->block_data;

         if(!resp_err)
            {
             grp->last_error = resp_err;
             grp->real_respond_time = grp->respond_time - grp->timer;
             grp->real_respond_time_min = std::min(grp->real_respond_time,grp->real_respond_time_min ? grp->real_respond_time_min : grp->real_respond_time);
             grp->real_respond_time_max = std::max(grp->real_respond_time,grp->real_respond_time_max);

             if(len && mf->addr == grp->dev_no && mf->data.command == grp->command)
             {

              DWORD ch_parts = 0,lo_num = -1,hi_num = 0;
              DWORD new_diag   = (*op->diag ) & ~OTD_DIAG_MASK;

              for(WORD i = 0;i<grp->count;i++)
              {

                int  val = 0;
                int  old_val = 0;
                BYTE old_pd = 0,new_pd = 0;// снять персональную недостоверность параметра
                //Взяли старые значание
                otd_get_value(op->personal_diag,i+grp->start_no,&old_pd,sizeof(old_pd));
                otd_get_value(op->data,i+grp->start_no,&old_val,sizeof(old_val));
                int get_result ;
                if(op->addr->fa == OTD_FA_DISCRETE)
                   get_result = modbus_get_discrete(&mf->data,i,&val);
                   else
                   get_result = modbus_get_analog(&mf->data,i,&val);
                if(old_val!=val)
                {
                 if(get_result)
                 {
                  otd_set_value(op->data,i+grp->start_no,&val,sizeof(val));
                  ch_parts|=OTD_PROTO_PART_DATA;
                  if(lo_num ==(DWORD) -1)lo_num = i;
                      hi_num = i;
                 }
                 else
                 new_pd = OTD_PDIAG_NODATA;

                }
                if(old_pd!=new_pd)
                {
                  otd_set_value(op->personal_diag,i+grp->start_no,&new_pd,sizeof(new_pd));
                  ch_parts|=(OTD_PROTO_PART_PERSONAL_DIAG|OTD_PROTO_PART_DATA);
                   if(lo_num ==(DWORD) -1) lo_num = i;
                    hi_num = i;

                }
              }

              new_diag |= otd_scan_personal_diag(op)&~OTD_DIAG_MASK;
              DWORD diag_cm = new_diag^(*op->diag);
              if(diag_cm)
                 {
                  ch_parts|=OTD_PROTO_PART_DIAG;
                  *op->diag = new_diag;
                  if(diag_cm & OTD_DIAG_MASK)
                    {
                     lo_num = op->data->numbers.loN,hi_num = op->data->numbers.hiN;
                     ch_parts|=OTD_PROTO_PART_DATA|OTD_PROTO_PART_PERSONAL_DIAG;
                    }
                 }

               if(ch_parts)
                 {
                   do_recv_group(op,ch_parts,lo_num,hi_num);
                   if(ch_parts&OTD_PROTO_PART_DATA)
                   {

                    DWORD number = MAKELONG(this->get_number(),(DWORD)(op->addr->sb));
                    owner->notify(MBNF_DEV_CHANGED ,MB_GROUP_DATA_CHANGED,&number,sizeof(number));
                   }
                 }
             }
            }
            else
            {
             if(grp->last_error!=(DWORD)resp_err)
             {
             //Записать в лог
             wchar_t text[MAX_PATH];
             int slc = locker.FullUnlock();
             swprintf_s(text,KERTL_ARRAY_COUNT(text),L"Модем %d линия %d устройство %d ошибка %d(%s) команда %d "
             ,owner->get_modem_number(),get_number(),mf->addr,resp_err,get_modbus_error_text(resp_err),mf->data.command);
             do_report(REPORT_ERROR_TYPE,text,0,0);
             locker.RestoreLock(slc);
             grp->last_error = resp_err;
             ++grp->error_count;
             }
             set_diag(op,OTD_DIAG_GOOD,-1,OTD_PDIAG_ERROR,true);

            }
            grp->requested  = FALSE;
            grp->timer      = grp->refresh_time;
            requested       = NULL;
            proto_pointer p = groups.find(*op->addr);
            next_refresh_from(p);
         }
         else
         {

         }

         if(tx_queue->QueueCount())
            tx_queue->GetEvent().SetEvent(true);

         return resp_err ? false : true;
      }



   void  __fastcall modbus_line::set_diag(proto_pointer & p,DWORD mod_diag,int number,DWORD diag_state,bool rcv)
   {
      set_diag(&p->op,mod_diag,number,diag_state,rcv);
   }

   void  __fastcall modbus_line::set_diag(lpotd_proto op,DWORD mod_diag,int number,DWORD diag_state,bool rcv)
    {
      if(number <0)
        otd_fill(op->personal_diag,&diag_state,sizeof(diag_state));
        else
         otd_set_value(op->personal_diag,number,&diag_state,sizeof(diag_state));
      *op->diag =mod_diag |otd_scan_personal_diag(op);
      if(rcv)
         do_recv_group(op, OTD_PROTO_PART_DIAG |OTD_PROTO_PART_PERSONAL_DIAG,0,-1);
    }

   void __fastcall modbus_line::set_diag(DWORD diag,bool send)
   {
    if(diag& OTD_DIAG_MASK)
    {
      int cnt = 0;
      proto_pointer b_ptr = groups.begin(),e_ptr = groups.end();
      DWORD  pd = OTD_PDIAG_NODATA;
      while(b_ptr<e_ptr)
      {
       otd_fill(b_ptr->op.personal_diag,&pd,sizeof(pd));
       if((*b_ptr->op.diag) ^ (diag|OTD_PART_DIAG_PARAM) )
           {
           *b_ptr->op.diag = diag|OTD_PART_DIAG_PARAM;
           ++cnt;
           }
          ++b_ptr;
      }

      if(send && cnt)
      {
        BYTE buf[1024];
        sotd_proto sop;
        DWORD len = otd_proto_format(buf,sizeof(buf),OTD_PROTO_PART_DIAG|OTD_PROTO_PART_KPK,0,0,0,0,0,0,&sop.op);
        sop.op.addr->addr = -1;
        sop.op.addr->pu = 0;
        sop.op.addr->cp = get_number();
        *sop.op.diag = diag;
        otd_proto_protect(&sop.op,len,OTD_DEF_POLINOM);
        queue_rxdata(FA_OTD,buf,len,false);
      }
    }

   }

     void  __fastcall modbus_line::do_handle_comport_frame ()
     {
       LPMODBUS_FRAME mf;
       do
       {
        mf = (LPMODBUS_FRAME)rxb.begin();
        int buf_len = rxb.get_data_len();
        if(buf_len >(int) sizeof(*mf))
        {
         int resp_len = modbus_get_respond_length(mf)+sizeof(WORD)+sizeof(*mf);
         if(resp_len<=buf_len)
           {
            if(modbus_check_crc(mf,resp_len))
            {
             owner->notify(MBNF_TXRX,MBNC_RX,mf,resp_len);
             handle_modbus_respond(mf,resp_len);

             rxb.move_to_begin(resp_len,0);
            }
            else
            ;//disconnect
           }
           else
           break;
        }
       }while(rxb.get_data_len()>sizeof(*mf));
     }

     void  __fastcall modbus_line::do_handle_ethernet_frame()
     {
          do{
             LPMODBUS_TCP_FRAME mtf =  (LPMODBUS_TCP_FRAME)rxb.begin();
             if(rxb.get_data_len()> sizeof(*mtf))
             {
               DWORD frame_len = (DWORD)modbus_get_tcp_length(mtf);
               if(rxb.get_data_len() >= frame_len)
               {
                update_stats_rx(0,1,handle_modbus_respond(&mtf->modbus_frame,modbus_word(mtf->hdr.frame_len))==true ? 0 : 1);
                owner->notify(MBNF_TXRX,MBNC_RXTCP,mtf,modbus_get_tcp_length(mtf));
                rxb.move_to_begin(frame_len,0);
               }
             }
            }while(rxb.get_data_len()>sizeof(MODBUS_TCP_FRAME));
     }



     void  __fastcall modbus_line::socket_handle_recv      (DWORD err)
     {
       if(err)
         socket_handle_disconnect(err);
       else
       {
           DWORD rd_len = 0;
           do
           {
            if(socket && socket->GetSocket() )
               socket->IoctlSocket(FIONREAD,&rd_len);
               //else
               //DebugBreak();
            if(rd_len)
            {
             if(rxb.get_free_size()<rd_len) rxb.grow(rd_len);
             char * b = rxb.end();
             rd_len = socket->Recv(b,rd_len,0);
             rxb.add(b,rd_len);
             update_stats_rx(rd_len,0,0);

            }
           }while(rd_len);

           //Вычитали из гнезда
           switch(this->modbus_point.media_type)
           {
            case MODBUS_MEDIA_TYPE_COMPORT_VIA_ETHERNET:
            do_handle_comport_frame();
            break;
            case MODBUS_MEDIA_TYPE_ETHERNET:
            do_handle_ethernet_frame();
            break;
           }
       }
     }

     void  __fastcall modbus_line::socket_handle_send      (DWORD err)
     {
        if(err)
          {
           update_stats_tx(0,0,1);
           socket_handle_disconnect(err);
          }
          else
          {
          if(tx_queue)
             tx_queue->GetEvent().SetEvent(true);
          }
     }



     void  __fastcall modbus_line::socket_timer()
     {
       if(this->is_connected())
        {
         on_timer();
        }
        watch_dog_time +=TIMER_RESOLUTION;
        if(watch_dog_time>=watch_dog_timer)
         {
             socket_connect();
         }
     }


     int   __fastcall modbus_line::socket_execute()
     {
       do{
          TSynchroSet ss;
          ss+=(*term_event)();
          ss+=tx_queue->GetEvent();
          if(line_event) ss+=(*line_event)();
          DWORD wait_result = ss.Wait(TIMER_RESOLUTION,false);
          switch(wait_result)
          {
            case 0: Terminate();break;
            case 1: socket_send_queue  ();break;
            case 2: socket_handle_event();break;
            case WAIT_TIMEOUT:
                 socket_timer();
             break;
          }
         }while(!CheckTerminateRequest());
        return  0;
     }


     bool  __fastcall modbus_line::before_comport_execute(){return false;}
     void  __fastcall modbus_line::before_comport_terminate(){};
     int   __fastcall modbus_line::comport_execute(){return 0;};

     DWORD __fastcall modbus_line::OnException(DWORD ExceptionCode,LPEXCEPTION_POINTERS p)
     {

       return TGKThread::OnException(ExceptionCode,p);
     }


    void    __fastcall modbus_line::fetch_comm_event(DWORD dev_no,bool log)
    {
     if(is_connected())
     {
      char temp[MAX_PATH];
      LPMODBUS_FRAME mf = (LPMODBUS_FRAME)temp;
      mf->addr = dev_no;
      mf->data.command = log ? MODBUS_FETCH_COMMEVENT_LOG : MODBUS_FETCH_COMMEVENT_CTRL;
      int len = sizeof(*mf)-1;
      tx_queue->PutIntoQueue(temp,len);
      if(tx_queue->QueueCount()>1)
         tx_queue->GetEvent().SetEvent(false);
     }
    }

     void  __fastcall modbus_line::do_recv_point_name()
     {
      BYTE buf[4096];
      otd_proto op;
      op.dw_size = sizeof(op);
      int len = otd_proto_format(buf,sizeof(buf),OTD_PROTO_PART_NAME|OTD_PROTO_PART_KPK,0,0,0,0,0,wcslen(modbus_point.point_name)+2,&op);
      otd_text_set(op.name,modbus_point.point_name);
      op.addr->addr = -1;
      op.addr->cp   = this->modbus_point.point_no;
      otd_proto_protect(&op,len,OTD_DEF_POLINOM);
      queue_rxdata(FA_OTD,buf,len,false);
     }





