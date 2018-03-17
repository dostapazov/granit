#pragma hdrstop
#include "vtdcnt.hpp"
#include <string>

 THeap heap(TSysInfo().GetPageSize()<<8);
 DWORD alloc_size = 0;

 void * operator new   (size_t sz)
 {
  void * ptr =  heap.Alloc(sz,0);
  if(ptr)
     alloc_size+=sz;
  return ptr;
 }
  void * operator new[] (size_t sz)
 {
  void * ptr =  heap.Alloc(sz,0);
  if(ptr)
     alloc_size+=sz;
  return ptr;
 }

 void   operator delete (void * ptr)
 {
  if(ptr && heap.Validate(ptr,0))
  {
    size_t sz = heap.MemSize(ptr,0);
    alloc_size-=sz;
    heap.Free(ptr,0);
  }
 }

 void   operator delete [] (void * ptr)
 {
  if(ptr && heap.Validate(ptr,0))
  {
    size_t sz = heap.MemSize(ptr,0);
    alloc_size-=sz;
    heap.Free(ptr,0);
  }
 }

 TVtdCntModem::TVtdCntModem():wt_timer(false),socket(NULL)
 {
  lock_param    = GKHB_AUTO_LOCK_OFF; 
  alloc_gkhandle();
  ZeroMemory(&mod_config,sizeof(mod_config));
  mod_config.dw_size = sizeof(mod_config);
  mod_config.modem_number = -1;
  TBaseSocket::InitWS(MAKEWORD(2,2));
  rep_id   = report_reg_event_type(L"VTDCNT",L"Модуль счётчиков ВТД");
 }

void     __fastcall TVtdCntModem::do_report(DWORD evt_type,wchar_t * rep_text,LPVOID data,DWORD dt_sz)
{
 LPBYTE buff = new BYTE[sizeof(TReportMes)+dt_sz];
 if(buff)
 {
  TReportMes &mes = *((TReportMes*)buff);
  mes.log    = rep_id;
  mes.desc   = rep_text;
  mes.source = _handle;
  mes.type   = evt_type;
  mes.data_size = dt_sz;
  if(dt_sz)
   memcpy(mes.data,data,dt_sz);
  report(RCM_MESSAGE_EVENT,(LPARAM)buff,0);
  delete [] buff;
 }
}



 DWORD       __fastcall TVtdCntModem::get_mem_used()
 {
  return alloc_size;
 }


 bool        __fastcall TVtdCntModem::read_settings()
 {
   lock();
     this->modem_number = -1;
     bool ret = TModemBase::read_settings();
     mod_config.modem_number = get_modem_number();
     TRegsReader rr(DupKey(reg_key));
     mod_config.pu_number = rr.ReadDword(REGDW_VTDMOD_PUNUM,-1,true);
     rr.ReadString(REGSTR_VTDMOD_PUNAME,mod_config.pu_name,sizeof(mod_config.pu_name)/sizeof(wchar_t),true);
     rr.ReadString(REGSTR_VTDMOD_SERVERADDR,mod_config.server_addr,KERTL_ARRAY_COUNT(mod_config.server_addr),true);
     mod_config.server_port = rr.ReadDword(REGDW_VTDMOD_SERVERPORT,0,true);
     read_devices(rr);
     cc_flags = 0;
   unlock();
   return ret;
 }

 bool        __fastcall TVtdCntModem::write_settings()
 {
   lock();
     bool ret = TModemBase::write_settings();
     TRegsWriter wr(DupKey(reg_key));
     if(cc_flags & CCFL_PUNUMBER)
       wr.WriteDword(REGDW_VTDMOD_PUNUM,mod_config.pu_number,true);
     if(cc_flags & CCFL_PUNAME)
        wr.WriteString(REGSTR_VTDMOD_PUNAME,mod_config.pu_name,true);
     if(cc_flags &CCFL_SERVER_ADDR)
        wr.WriteString(REGSTR_VTDMOD_SERVERADDR,mod_config.server_addr,true);

     if(cc_flags & CCFL_SERVER_PORT)
       wr.WriteDword(REGDW_VTDMOD_SERVERPORT,mod_config.server_port,true);
     if(cc_flags&CCFL_DEVICES)
        write_devices(wr);

   unlock();
   return ret;
 }

 char  sign[] = "CVTD";
 void __fastcall TVtdCntModem::read_devices(TRegsReader & rr)
 {

  DWORD sz = -1;
  TMemoryStream ms;
  if(rr.GetValueType(REGDB_VTDMOD_DEVICES,NULL,&sz,true) && sz)
  {
     ms.SetSize(sz);
     rr.ReadBytes(REGDB_VTDMOD_DEVICES,(LPVOID)ms.GetMemory(),sz,true);
     ms.Reserve(sz);
     ms.Rewind();
     read_devices(ms);
  }
 }

 void        __fastcall TVtdCntModem::read_devices (TStream & s)
 {
  BYTE _sgn[sizeof(sign-1)]={0};
  s.Read(_sgn,sizeof(_sgn));
  if(memcmp(_sgn,sign,sizeof(_sgn))==0)//Проверка сигнатуры
  {
   DWORD lc;
   s>>lc;
   for(DWORD i = 0;i<lc;++i)
   {
    vtd_line * line = new vtd_line;
    line->sread(s);
    this->add_line(line);
   }
  }
 }


 void __fastcall TVtdCntModem::write_devices(TRegsWriter & wr)
 {
  TMemoryStream ms;
  write_devices(ms);
  wr.WriteBytes(REGDB_VTDMOD_DEVICES,ms.GetMemory(),ms.GetSize(),true);

 }

 void        __fastcall TVtdCntModem::write_devices(TStream & s)
 {
    s.Write(sign,sizeof(sign)-1);
    DWORD lc = get_lines_count();
    s<<lc;
    lines_map::iterator ptr = lines.begin(),l_end = lines.end();
    while(ptr!=l_end)
    {
     vtd_line * line = dynamic_cast<vtd_line*>(ptr->second);
     line->swrite(s);
     ++ptr;
    }

 }


 DWORD       __fastcall TVtdCntModem::get_config_data    (DWORD mask,LPVOID buf,DWORD bsz)
 {
  DWORD ret = GKH_RET_ERROR;
  LPVTDCNTMOD_CONFIG mcfg = (LPVTDCNTMOD_CONFIG)buf;
  if(buf && bsz>=sizeof(mod_config))
  {
   lock();
   memcpy(mcfg,&mod_config,sizeof(mod_config));
   unlock();

   ret = GKH_RET_SUCCESS;
  }
  return ret;
 }

 void __fastcall TVtdCntModem::change_puname()
 {
      otd_proto_param opp;
      otd_proto       op;
      op.dw_size      = sizeof(op);
      ZeroMemory(&opp,sizeof(opp));
      opp.dw_size     = sizeof(opp);
      opp.parts       = OTD_PROTO_PART_NAME;
      opp.name_sz     = lstrlenW(mod_config.pu_name)+1;
      DWORD need_size = otd_proto_calc_size_ex(&opp,0);
      BYTE    __buf[1024];
      LPBYTE  buf     = need_size>sizeof(__buf) ? new BYTE[need_size] : __buf;
      if(buf)
        {
         otd_proto_format_ex(buf,need_size,&opp,0,&op);
         op.addr->addr = -1;
         otd_text_set(op.name,mod_config.pu_name);
         queue_rxdata(0,-1,0,FA_OTD,buf,op.proto_size,false);
         if(buf!=__buf)
           delete [] buf;
        }
 }

 bool        __fastcall TVtdCntModem::set_config_data    (DWORD mask,LPVOID buf,DWORD bsz)
 {
  bool ret = false;
  DWORD error = 0;
  LPVTDCNTMOD_CONFIG mcfg = (LPVTDCNTMOD_CONFIG)buf;
  if(check_data_size(buf,sizeof(mod_config)))
  {
   ret = true;
  if(mask & CCFL_MODEM_NUMBER) // Смена номера модема
     {
      DWORD old_num;
      if(GKH_RET_SUCCESS == set_modem_number(mcfg->modem_number,&old_num))
        {
         lock();
         mod_config.modem_number = mcfg->modem_number;
         unlock();
        }
        else
        {ret = false;error = GetLastError();}
     }
   lock();
   if((mask & CCFL_PUNUMBER) && mod_config.pu_number!= mcfg->pu_number)
      {
       mod_config.pu_number= mcfg->pu_number;
       cc_flags|=CCFL_PUNUMBER;
      }
      
   if((mask & CCFL_PUNAME) && wcsncmpi(mod_config.pu_name,mcfg->pu_name,KERTL_ARRAY_COUNT(mod_config.pu_name)))
     {
       wcsncpy(mod_config.pu_name,mcfg->pu_name,KERTL_ARRAY_COUNT(mod_config.pu_name));
       cc_flags|=CCFL_PUNAME;
       change_puname();
     }

   if((mask &CCFL_SERVER_ADDR) && wcsncmpi(mod_config.server_addr,mcfg->server_addr,KERTL_ARRAY_COUNT(mod_config.server_addr)))
   {
    wcsncpy(mod_config.server_addr,mcfg->server_addr,KERTL_ARRAY_COUNT(mod_config.server_addr));
    cc_flags|=CCFL_SERVER_ADDR;
   }
   if((mask & CCFL_SERVER_PORT) && mod_config.server_port!= mcfg->server_port)
      {
       mod_config.server_port= mcfg->server_port;
       cc_flags|=CCFL_SERVER_PORT;
      }


   unlock();
  }
  return (ret == true && !error)  ? true:false;
 }

 bool        __fastcall TVtdCntModem::compare_config_data(DWORD mask,LPVOID buf,DWORD bsz,LPDWORD ch_mask,BOOL * restart)
 {

  DWORD chmask= 0;
  BOOL  rst   = FALSE;
  LPVTDCNTMOD_CONFIG mcfg = (LPVTDCNTMOD_CONFIG)buf;
  if(check_data_size(buf,sizeof(mod_config)))
  {
  if(( mask & CCFL_MODEM_NUMBER) && mcfg->modem_number!=get_modem_number()) // Смена номера модема
     {
      chmask|=CCFL_MODEM_NUMBER,rst = TRUE;
     }

   lock();
   if((mask & CCFL_PUNUMBER) && mod_config.pu_number!= mcfg->pu_number)
      {
       chmask|=CCFL_PUNUMBER,rst = TRUE;
      }

   if((mask & CCFL_PUNAME) && wcsncmpi(mod_config.pu_name,mcfg->pu_name,KERTL_ARRAY_COUNT(mod_config.pu_name)))
     {
      chmask|=CCFL_PUNAME;
     }

   if((mask &CCFL_SERVER_ADDR) && wcsncmpi(mod_config.server_addr,mcfg->server_addr,KERTL_ARRAY_COUNT(mod_config.server_addr)))
   {
    chmask|=CCFL_SERVER_ADDR,rst = TRUE;
   }

   if((mask & CCFL_SERVER_PORT) && mod_config.server_port!= mcfg->server_port)
      {
       chmask |=CCFL_SERVER_PORT,rst = TRUE;;
      }

   unlock();
  }
  if(restart) *restart = rst;
  if(ch_mask) *ch_mask = chmask;
  return (chmask)  ? true:false;
 }

 DWORD       __fastcall TVtdCntModem::get_window_module  (wchar_t * buf,DWORD bsz)
 {
   safe_strcpyn(buf,L"vtdcntwnd.dll",bsz);
   return lstrlenW(buf);
 }

 LRESULT     __fastcall TVtdCntModem::processing         (DWORD cmd,LPARAM p1,LPARAM p2)
 {
  LRESULT ret;
  switch(cmd)
  {
   case  VTDCMD_ENUM_DEVICE : ret = enum_device((LPVTDDEV_PARAM) p1,(DWORD)p2);
         break;
   case  VTDCMD_GET_DEVICE : ret = get_device((LPVTDDEV_PARAM) p1,(DWORD)p2);
         break;
   case VTDCMD_CREATE_DEVICE: ret = create_device((LPVTDDEV_PARAM)p1);break;
   case VTDCMD_DELETE_DEVICE: ret = delete_device((DWORD)p1);break;
   case VTDCMD_UPDATE_DEVICE: ret = update_device((LPVTDDEV_PARAM)p1,p2);break;
   case VTDCMD_ENUM_GROUPS  : ret = enum_groups(LOWORD(p2),HIWORD(p2),(LPVTDDEV_GROUP)p1);break;
   case VTDCMD_GET_GROUP    : ret = get_group  (LOWORD(p2),HIWORD(p2),(LPVTDDEV_GROUP)p1);break;
   case VTDCMD_CREATE_GROUP : ret = create_group(p2,(LPVTDDEV_GROUP)p1);break;
   case VTDCMD_DELETE_GROUP : ret = delete_group(p1,p2);break;
   case VTDCMD_UPDATE_GROUP : ret = this->update_group(LOWORD(p2),HIWORD(p2),(LPVTDDEV_GROUP)p1);break;
   case VTDCMD_GET_NUMDATE_TIME: ret = get_dev_num_date_time(p1,(lpvtd_num_date_time)p2);break;
   case VTDCMD_GET_BASE_CONFIG : ret = get_dev_base_config(p1,(lpvtd_base_config)p2);break;
   case VTDCMD_GET_GROUP_DATA  : ret = get_dev_group_data((LPVTD_GET_GROUP_DATA)p1);break;

   default: ret = TModemBase::processing(cmd,p1,p2);break;
  }
  return ret;
 }

 void        __fastcall TVtdCntModem::free_line (modem_line * line)
 {
    if(line)
    {
     if(is_running())
     {
     vtd_line * v_line = dynamic_cast<vtd_line*>( line );
     if(v_line)
        {
         //TODO отправить кадр диагностики нет данных
        }
     }   
     delete line;
    }
 }


 LRESULT __fastcall TVtdCntModem::enum_device( LPVTDDEV_PARAM dev,DWORD idx)
 {
  LRESULT ret = GKH_RET_ERROR;

  lock();
   /*MODEM_LINE_PROPS mlp;
   mlp.dw_size  = sizeof(mlp);
   ret   = enum_lines(idx,&mlp);*/
   if(idx<lines.size())
   {
    lines_map::iterator i = lines.begin();
    advance(i,idx);
    vtd_line *line =  dynamic_cast<vtd_line*>(i->second);
     if(line)
     {
        ret = GKH_RET_SUCCESS;
        line->get_param(dev);
        //get_device(dev,mlp.addr.line);
     }
   }
  unlock();
  return ret;
 }

 LRESULT __fastcall TVtdCntModem::get_device ( LPVTDDEV_PARAM dp,DWORD num)
 {
  LRESULT ret = GKH_RET_ERROR;
  if(check_data_size(dp,sizeof(*dp)))
  {
   lock();
   vtd_line * dev = get_device(num);
   if(dev)
     {
      ret = GKH_RET_SUCCESS;
      dev->get_param(dp);
     }
   unlock();
  }
  return ret;
 }

 vtd_line *  __fastcall TVtdCntModem::get_device ( DWORD num)
 {
   vtd_line * ret_line = NULL;
   lock();
   if(lines.count(num))
      ret_line = dynamic_cast<vtd_line*>(lines[num]);
      else
      SetLastError(VTDERR_DEV_NOT_EXIST);
   unlock();
   return ret_line;
 }

 bool       __fastcall TVtdCntModem::check_dev_param(LPVTDDEV_PARAM dp)
 {
  bool ret = this->check_data_size(dp,sizeof(*dp));
  DWORD error = 0;
  if(ret)
  {
   if(dp->number>VTD_DEVICE_MAX_NUMBER)
     error = VTDERR_INVALID_DEVNUMBER;
     else
     {
      if(dp->type>VTD_TYPE_Y)
        error = VTDERR_INVALID_DEVTYPE;
     }
  }
  if(error)
     ret = false,SetLastError(error);
  return ret;
 }

 LRESULT    __fastcall TVtdCntModem::create_device( LPVTDDEV_PARAM dev)
 {
  // создание устройства
  LRESULT ret = GKH_RET_ERROR;
  if(check_dev_param(dev))
  {

  if(get_device(dev->number) == NULL)
  {
   lock();
   vtd_line * line = new vtd_line(*dev);
    add_line(line);
   unlock();
   if(IsRunning()) line->start();
   ret = GKH_RET_SUCCESS;
   cc_flags|=CCFL_DEVICES;
  }
  else
  SetLastError(VTDERR_DEV_ALREADY_EXIST);
  }
  return ret;
 }

 LRESULT    __fastcall TVtdCntModem::delete_device(DWORD num)
 {
   LRESULT ret = GKH_RET_ERROR;
   lock();
   if(get_line(num))
      {
       remove_line(num);
       ret = GKH_RET_SUCCESS;
       cc_flags|=CCFL_DEVICES;
      }
   unlock();   
   return ret;
 }

 LRESULT    __fastcall TVtdCntModem::update_device( LPVTDDEV_PARAM dp,DWORD number)
 {
  LRESULT ret = GKH_RET_ERROR;
  bool need_refresh = false;
  if(number!=dp->number)
    {
     if(renumber_line(number,dp->number))
        {
        number = dp->number;
        cc_flags|=CCFL_DEVICES;
        need_refresh = true;
        }

    }

  lock();
  vtd_line * vl = get_device(number);
  if(vl)
  {
    if(vl && vl->update_device(dp))
      {
       ret = GKH_RET_SUCCESS;
       cc_flags|=CCFL_DEVICES;
      }
  if(need_refresh)
     vl->refresh(-1);

  }

  unlock();
  return ret;
 }


 LRESULT    __fastcall TVtdCntModem::enum_groups (DWORD dev_num,DWORD idx,LPVTDDEV_GROUP grp)
 {
  LRESULT ret = GKH_RET_ERROR;
  if(check_data_size(grp,sizeof(*grp)))
  {
    lock();
    vtd_line * line =  get_device(dev_num);
    if(line)
    {
      if(line ->enum_groups(idx,grp) )
         ret = GKH_RET_SUCCESS;
    }
    unlock();
  }
  return ret;
 }

 LRESULT    __fastcall TVtdCntModem::get_group   (DWORD dev_num,DWORD number,LPVTDDEV_GROUP grp)
 {
  LRESULT ret = GKH_RET_ERROR;
  if(check_data_size(grp,sizeof(*grp)))
  {
    lock();
    vtd_line * line =  get_device(dev_num);
    if(line)
    {
      if(line ->get_group(number,grp) )
         ret = GKH_RET_SUCCESS;
    }
    unlock();
  }
  return ret;
 }

 LRESULT    __fastcall TVtdCntModem::create_group(DWORD dev_num,LPVTDDEV_GROUP grp)
 {
  LRESULT ret = GKH_RET_ERROR;
  if(check_data_size(grp,sizeof(*grp)))
  {
    lock();
    vtd_line * line =  get_device(dev_num);
    if(line)
    {
      if(line ->create_group(grp) )
         {
          ret = GKH_RET_SUCCESS;
          cc_flags|=CCFL_DEVICES;
         }
    }
    unlock();
  }
  return ret;
 }

 LRESULT    __fastcall TVtdCntModem::delete_group(DWORD dev_num,DWORD number)
 {
  LRESULT ret = GKH_RET_ERROR;
    lock();
    vtd_line * line =  get_device(dev_num);
    if(line)
    {
      if(line ->delete_group(number) )
        {
         ret = GKH_RET_SUCCESS;
         cc_flags|=CCFL_DEVICES;
        }
    }
    unlock();
  return ret;
 }

 LRESULT    __fastcall TVtdCntModem::update_group(DWORD dev_num,DWORD number,LPVTDDEV_GROUP grp)
 {
  LRESULT ret = GKH_RET_ERROR;
  if(check_data_size(grp,sizeof(*grp)))
  {
    lock();
    vtd_line * line =  get_device(dev_num);
    if(line)
    {
      if(line ->update_group(number,grp) )
         {
          ret = GKH_RET_SUCCESS;
          cc_flags|=CCFL_DEVICES;
         }
    }
    unlock();
  }
  return ret;
 }





 int         __fastcall TVtdCntModem::convert_rx_data(LPWORD fa,
                                                      LPBYTE in,int in_len,
                                                      LPBYTE out,DWORD out_sz)
 {
  if(*fa == FA_OTD)
  {
   if(out_sz>= (DWORD)in_len)
   {
   memcpy(out,in,in_len);
   sotd_proto sop(out,in_len);
   lock();
   sop.op.addr->pu = mod_config.pu_number;
   unlock();
   if(sop.op.kpk)
      otd_proto_protect(&sop(),in_len,OTD_DEF_POLINOM);
    return in_len;  
   }
   else
   return -in_len;   
  }
  return 0;
 }

 BOOL    __fastcall TVtdCntModem::can_start(DWORD reason,LPARAM p2)
 {
  BOOL ret   = (*this->mod_config.server_addr && mod_config.server_port!=0) ? TRUE:FALSE;
  if(!ret)
    SetLastError(VTDERR_INVALID_CONFIGURATION);
  //TODO проверка установленного протокола TCP/IP
  if(ret)
  {
    int proto[2] = {IPPROTO_TCP,0};
    WSAPROTOCOL_INFO pi[10]={0};
    DWORD sz   = sizeof(pi);
    int cnt = WSAEnumProtocols(proto,pi,&sz);
    if(cnt <1)
       {
        ret = FALSE;
        SetLastError(WSAEPROTONOSUPPORT	);
       }

  }
  return ret;
 }

 DWORD   __fastcall TVtdCntModem::start(DWORD reason,LPARAM p2)
 {
  DWORD ret = GKH_RET_ERROR;
  if(Start(0))
  {
   ret =  TModemBase::start(reason,p2);
  }
  return ret;
 }

 DWORD   __fastcall TVtdCntModem::stop (DWORD reason)
 {
  DWORD ret = GKH_RET_ERROR;
  TerminateAndWait(5000,true);
  BYTE buf[1024];
  sotd_proto sop;
  DWORD len = otd_proto_format(buf,sizeof(buf),OTD_PROTO_PART_DIAG|OTD_PROTO_PART_KPK,0,0,0,0,0,0,&sop.op);
  sop.op.addr->addr = -1;
  sop.op.addr->pu = mod_config.pu_number;

  *sop.op.diag = OTD_DIAG_PUCONNECT;
  otd_proto_protect(&sop.op,len,OTD_DEF_POLINOM);
  queue_rxdata(-1,0,FA_OTD,buf,len,false);
  ret = TModemBase::stop(reason);

  return ret;
 }

 /*Функциональность нити*/
   bool  __fastcall TVtdCntModem::BeforeExecute()
   {
    bool ret = TGKThread::BeforeExecute();


    if(ret )
    {
     term_event.SetEvent(false);
     socket_event = WSACreateEvent();

     if(socket_event!=INVALID_HANDLE_VALUE )
     {
      this->sync_set.Clear();
      sync_set+=term_event;
      sync_set+=wt_timer;
      sync_set+=socket_event;
      wt_timer.StartPeriodTimer(1000,true);
      do_connect();
     }
     else
     ret = false;
    }
    return ret;
   }

   void  __fastcall TVtdCntModem::BeforeTerminate()
   {
      if(socket)
      {
       WSAEventSelect(socket->GetSocket(),socket_event,0);
       delete socket;
       socket = NULL;
      }
      WSACloseEvent(socket_event);
      wt_timer.StopTimer();
      sync_set.Clear();
      TGKThread::BeforeTerminate();
   }

   void __fastcall TVtdCntModem::do_connect()
   {
    is_connected  = FALSE;
    not_connected_time = 0;
    lock();
    if(socket)
    {
     WSAEventSelect(socket->GetSocket(),socket_event,0);
     delete socket;
    }
     socket = new TIPSocket(true);
     WSAEventSelect(socket->GetSocket(),socket_event,FD_CONNECT | FD_CLOSE|FD_READ|FD_WRITE);
    char  addr_text[MAX_PATH];
    Unicode2Ansi(addr_text,this->mod_config.server_addr);
    TIPAddress  sa(addr_text,mod_config.server_port);
    socket->Connect(sa(),sizeof(sa));
    unlock();
   }

   int   __fastcall TVtdCntModem::Execute()
   {
    DWORD wr;
    do{
       wr = sync_set.Wait(INFINITE);
       switch(wr)
       {
        case 1:  handle_timer();
              break;
        case 2: handle_socket_events();
                break;
       }
      }while(wr && !CheckTerminateRequest());

    return 0;
   }

   void       __fastcall TVtdCntModem::handle_timer()
   {
     if(!is_connected && ++not_connected_time>5)
       {
         do_connect();
       }
       else
       {
        if(last_query_size)
         {
           //Ожидание ответа       kertl.hpp
           if(NS100_MSEC(GetTime(false)-last_query_time)>2000)
           {
            //2 секунды не было ответа
            //TODO удаляем из очереди всё вызовы от устройства
            do_send_dev_query(); //Отправляем следующий

           }
         }

        lock();
        lines_map::iterator ptr = lines.begin(),ptr_end = lines.end();
        while(ptr!=ptr_end)
        {
         vtd_line * vl = dynamic_cast<vtd_line*>(ptr->second);
         if(vl) vl->on_timer();
         ++ptr;
        }
        unlock();
       }
   }

   void       __fastcall TVtdCntModem::handle_socket_events()
   {
     WSANETWORKEVENTS ne={0};
     while(SOCKET_ERROR!=WSAEnumNetworkEvents(socket->GetSocket(),socket_event,&ne) && ne.lNetworkEvents)
     {
       if(ne.lNetworkEvents&FD_CONNECT)
         {
           on_socket_connect(ne.iErrorCode[FD_CONNECT_BIT]);
           ne.lNetworkEvents&=~FD_CONNECT;
         }

       if(ne.lNetworkEvents&FD_CLOSE)
         {
           on_socket_disconnect(ne.iErrorCode[FD_CLOSE_BIT]);
           ne.lNetworkEvents&=~FD_CLOSE;
         }

       if(ne.lNetworkEvents&FD_WRITE)
         {
           on_socket_write(ne.iErrorCode[FD_WRITE_BIT]);
           ne.lNetworkEvents&=~FD_WRITE;
         }

       if(ne.lNetworkEvents&FD_READ)
         {
           on_socket_read(ne.iErrorCode[FD_READ_BIT]);
           ne.lNetworkEvents&=~FD_READ;
         }
     }
   }

 void       __fastcall TVtdCntModem::on_socket_connect   (DWORD err)
 {
  //Обработка соединения
  if(!err)
  {
    query_queue.DropData();
    lock();
    is_connected = TRUE;
    //TODO
    //Начать опрос с первого устройства
    last_query_size = 0;
    last_query_time = 0;
    refresh_line(-1,-1);
    unlock();
  }
  else
  this->not_connected_time = 0;
 }

 void       __fastcall TVtdCntModem::send_pu_diag(DWORD diag)
 {
  BYTE buf[128];
  sotd_proto sop;
  DWORD len = otd_proto_format(buf,sizeof(buf),OTD_PROTO_PART_DIAG|OTD_PROTO_PART_KPK,0,0,0,0,0,0,&sop());
  sop.op.addr->addr = -1;
  lock();
    sop.op.addr->pu = this->mod_config.pu_number;
  unlock();
  *sop.op.diag = diag;
  otd_proto_protect(&sop(),len,OTD_DEF_POLINOM);
  queue_rxdata(-1,0,FA_OTD,buf,len,false);
 }
 
 void       __fastcall TVtdCntModem::on_socket_disconnect(DWORD err)
 {
  //Обработка рассоединения
  is_connected = FALSE;

  last_query_size = 0;
  last_query_time = 0;
  query_queue.DropData();

  /*Пробежаться по всем линиям сбросить соединение и сбросить очередь на передачу*/
  DWORD idx = 0;
  modem_line * line;
  //lock();
  do{
    line = get_line_byindex(idx++);
    if(line)
       line->connect(false);
    }while(line);
  //unlock();
  //Отправить диагностику ПУ нет связи
  send_pu_diag(OTD_DIAG_PUCONNECT);

 }

 void       __fastcall TVtdCntModem::on_socket_write     (DWORD err)
 {
  //Обработка готовности к записи
  if(!is_connected)
  {
   on_socket_connect(err);
  }

  if(last_query_size == 0)
     do_send_dev_query();
 }

 void       __fastcall TVtdCntModem::on_socket_read      (DWORD err)
 {
  //Обработка чтения
  DWORD dl = socket->CanRead();

  do{
     if(rxb.get_free_size()<dl)
         rxb.grow(dl);
     LPBYTE ptr = rxb.end();
     dl = socket->Recv(rxb.end(),rxb.get_free_size(),0);
     rxb.reserve(dl);
     notify(VTDNF_TXRX,VTDNC_RX,ptr,dl);
     dl  = socket->CanRead();
    }while(dl);

  while(process_vtd_respond())
     ;

 }

bool       __fastcall TVtdCntModem::process_vtd_respond()
{
  bool ret = false;
  DWORD dl = rxb.get_data_len();
  if(dl>=sizeof(vtd_resp_kadr))
    {
     lpvtd_resp_kadr vk = (lpvtd_resp_kadr)rxb.begin();
     DWORD      wl = vk->data_len + sizeof(*vk)+1;
     if(wl<=dl)
     {
      lock();
      //handle_respond_vtd(vk,wl);
      if(lines.count(vk->dev_number))
      {
       vtd_line * vtdl = dynamic_cast<vtd_line*>(lines[vk->dev_number]);
       lpvtd_query_kadr qk = (lpvtd_query_kadr)last_query;
       if(qk->dev_number == vk->dev_number)
          {
           vtdl->handle_respond(qk,vk);
            last_query_size = 0;
            last_query_time = 0;
            do_send_dev_query();
          }
          else
          {
          // Записать в лог несоответствие номера ответившего устройства
          wchar_t rep_text[MAX_PATH];
          wsprintfW(rep_text,L"Несовпадение номеров устройств. Ответ от %d, ожидается от %d ",vk->dev_number,qk->dev_number);
          this->do_report(REPORT_ERROR_TYPE,rep_text,0,0);
          }
      }
      unlock();
      rxb.move_to_begin(wl);
      //wait_respond = FALSE;
      ret = true;
     }
     //else
     //wait_respond = TRUE;
    }
  return ret;
}


 void        __fastcall TVtdCntModem::send_dev_query(LPVOID data,DWORD sz)
 {
   if(is_connected)
   {
     query_queue.Lock();
     query_queue.PutIntoQueue(data,sz);
     if(0==last_query_size)
        do_send_dev_query();
     query_queue.Unlock();
   }
 }

 void    __fastcall TVtdCntModem::do_send_dev_query()
 {
     last_query_size = 0;
     DWORD dev_type  = -1;
     if(query_queue.QueueCount())
       {
        query_queue.PeekFromQueue(last_query,sizeof(last_query),(int*)&last_query_size);
        if(socket->Send(last_query,last_query_size,0))
        {
        query_queue.DropFirst();
        last_query_time = GetTime(false);
        lock();
        vtd_line * dev = get_device(last_query[0]);
         if(dev)
           {
            dev->update_stats_tx(last_query_size,1,0);
            dev_type = dev->get_dev_type();
           }
         unlock();
         notify(VTDNF_TXRX,VTDNC_TX,last_query,last_query_size);
         if(dev_type!=(DWORD)-1)
           notify(VTDNF_TXRX,VTDNC_TXKADR_STD+dev_type,last_query,last_query_size);
        }
        else
        {
         last_query_size = 0;
         DWORD err = socket->GetLastError();
         if(err!=WSAEWOULDBLOCK)
            on_socket_disconnect(err);
        }
       }


 }

 LRESULT    __fastcall TVtdCntModem::get_dev_num_date_time(DWORD num,lpvtd_num_date_time dttm)
 {
  LRESULT ret = GKH_RET_ERROR;
  lock();
  vtd_line * vl = this->get_device(num);
  if(vl)
     vl->get_dev_num_date_time(dttm),ret = GKH_RET_SUCCESS;
  unlock();

  return ret;
 }

 LRESULT    __fastcall TVtdCntModem::get_dev_base_config  (DWORD num,lpvtd_base_config bc)
 {
  LRESULT ret = GKH_RET_ERROR;
  lock();
  vtd_line * vl = this->get_device(num);
  if(vl)
     vl->get_dev_base_config(bc),ret = GKH_RET_SUCCESS;
  unlock();

  return ret;
 }

 LRESULT     __fastcall    TVtdCntModem::send (LPMPROTO_HEADER mph,DWORD sz)
 {
 sotd_proto sop((LPBYTE)mph->data,mph->data_size);
  if(OTD_ADDR_ISQUERY(sop().addr) && ((DWORD)sop.op.addr->pu == this->mod_config.pu_number || sop.op.addr->pu == OTD_ADDR_MAXVALUE) )
     TVtdCntModem::change_puname();
  return TModemBase::send(mph,sz);
 }

 const char  * __fastcall TVtdCntModem::get_gkthread_name()
 {
   static char ret[MAX_PATH];
   wsprintf(ret,"VTD Module %d thread",this->mod_config.modem_number);
   return (char*)ret;
 }

 DWORD __fastcall TVtdCntModem::OnException(DWORD ExceptionCode,LPEXCEPTION_POINTERS exp)
 {
    this->full_unlock();
    while(query_queue.Unlock())
         ;

   return TGKThread::OnException(ExceptionCode,exp);
 }

 DWORD   __fastcall TVtdCntModem::on_exception  (lpexcept_record er)
 {
    this->full_unlock();
    while(query_queue.Unlock())
         ;
    return TGKModule::on_exception(er);
 }

 LRESULT    __fastcall TVtdCntModem::get_dev_group_data(LPVTD_GET_GROUP_DATA dg)
 {
  LRESULT ret = GKH_RET_ERROR;
  if(check_data_size(dg,sizeof(*dg)))
  {
   lock();
   vtd_line * vl = get_device(dg->device);
   if(vl)
      {
       ret = vl->get_group_data(dg) ? GKH_RET_SUCCESS:GKH_RET_ERROR;  
      }
   unlock();
  }
  return ret;
 }






