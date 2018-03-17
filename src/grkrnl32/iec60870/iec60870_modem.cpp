#pragma hdrstop
#include "iec60870_modem.hpp"

THeap         Tiec60870modem::mod_heap(TSysInfo().GetPageSize()<<8);
DWORD         Tiec60870modem::total_mem_alloc  = 0;

void __fastcall check_heap(THeap & mod_heap)
{
  if(!mod_heap.IsValid())
      mod_heap.Create(TSysInfo().GetPageSize()<<8);
}

void * __fastcall Tiec60870modem::mem_alloc(size_t size)
{

   check_heap(mod_heap);
   void * ptr = NULL;
   if(size)
     {
      ptr = mod_heap.Alloc(size);
      total_mem_alloc +=  mod_heap.MemSize(ptr);
     }
   return ptr;
}

void   __fastcall Tiec60870modem::mem_free (void * ptr )
{
   check_heap(mod_heap);
   if(ptr && mod_heap.Validate(ptr))
   {
      total_mem_alloc -=  mod_heap.MemSize(ptr);
      mod_heap.Free(ptr);
   }
}

DWORD   __fastcall Tiec60870modem::get_mem_used()
{
  return total_mem_alloc;
}

 Tiec60870modem::Tiec60870modem()
 {
   lock_param    = GKHB_AUTO_LOCK_OFF;
   alloc_gkhandle();
   rep_id = report_reg_event_type(L"IEC60870",L"IEC60870-Модем");
 }


void __fastcall Tiec60870modem::free_line(modem_line * line)
{
  if(line)
     {
      line->stop();
      delete line;
     }
}

BYTE      __fastcall Tiec60870modem::get_pu_number ()
{
  TLockHelper l(this->locker);
  return mod_config.pu_number;
}

int      __fastcall Tiec60870modem::convert_rx_data(LPWORD fa,LPBYTE in,int in_len,LPBYTE out,DWORD out_sz)
{
  if(*fa == FA_OTD )
  {
   if(out_sz>= (DWORD)in_len)
   {
    if(in_len)
    {
    memcpy(out,in,in_len);
    sotd_proto sop(out,in_len);
    sop.op.addr->pu = get_pu_number();
    if(sop.op.kpk)
       otd_proto_protect(&sop(),in_len,OTD_DEF_POLINOM);
     return in_len;
    }
   }
   else
    return -in_len;
  }
  return 0;
}

 DWORD    __fastcall Tiec60870modem::get_config_data    (DWORD mask,LPVOID buf,DWORD bsz)
 {
     if(check_data_size(buf,bsz,sizeof(mod_config)))
     {
      LPIEC60870_MOD_CONFIG dest_mod_config = (LPIEC60870_MOD_CONFIG)buf;
      TLockHelper l(locker);
      dest_mod_config->modem_number = this->get_modem_number();
      dest_mod_config->pu_number    = this->mod_config.pu_number;
      safe_strcpyn(dest_mod_config->pu_name,mod_config.pu_name,KERTL_ARRAY_COUNT(dest_mod_config->pu_name));
      return GKH_RET_SUCCESS;
     }
    return GKH_RET_ERROR;
 }

 bool     __fastcall Tiec60870modem::set_config_data    (DWORD mask,LPVOID buf,DWORD bsz)
 {
  if( check_data_size(buf,bsz,sizeof(mod_config)) )
  {
   LPIEC60870_MOD_CONFIG src_mod_config = (LPIEC60870_MOD_CONFIG)buf;
   TLockHelper l(locker);
   if(mask & CCFL_MODEM_NUMBER)
      {
        if(TModemBase::set_modem_number(src_mod_config->modem_number,NULL))
           return false;
         mod_config.modem_number = src_mod_config->modem_number;
      }
   if(mask&CCFL_PUNUMBER)
      mod_config.pu_number = src_mod_config->pu_number;
   if(mask&CCFL_PUNAME)
      {
       safe_strcpyn(mod_config.pu_name , src_mod_config->pu_name,KERTL_ARRAY_COUNT(mod_config.pu_name));
       if(is_running())
       {
         //TODO Отправить измененное имя КП
         do_recv_pu_data(true);
       }
      }
   return true;
  }
  return false;
 }

 bool     __fastcall Tiec60870modem::compare_config_data(DWORD mask,LPVOID buf,DWORD bsz,LPDWORD ch_mask,BOOL * restart)
 {
   bool ret = false;
   if(check_data_size(buf,bsz,sizeof(mod_config)))
   {
    LPIEC60870_MOD_CONFIG src_mod_config = (LPIEC60870_MOD_CONFIG)buf;
    DWORD _chmask      = 0;
    bool  need_restart = false;
    TLockHelper l(locker);
    if((mask & CCFL_MODEM_NUMBER) && this->get_modem_number()!=src_mod_config->modem_number)
         {
          need_restart = true;
          _chmask |= CCFL_MODEM_NUMBER;
         }

    if((mask & CCFL_PUNUMBER) && mod_config.pu_number != src_mod_config->pu_number)
         {
          need_restart = true;
          _chmask |= CCFL_PUNUMBER;
         }
    if((mask & CCFL_PUNAME) && _wcsnicmp(src_mod_config->pu_name,mod_config.pu_name,KERTL_ARRAY_COUNT(mod_config.pu_name)))
         {
          _chmask |= CCFL_PUNAME;
         }

    if(restart) *restart = need_restart;
    if(ch_mask) *ch_mask = _chmask;
    ret = true;
   }
   return ret;
 }

 bool     __fastcall Tiec60870modem::read_settings      ()
 {
   bool ret = TModemBase::read_settings();
   if(ret)
   {
    mod_config.modem_number = get_modem_number();
    TRegsReader rd(DupKey(reg_key));
    if(rd.IsRootOnly())
    {
      TLockHelper l (locker);
      mod_config.pu_number = rd.ReadDword(REGDW_PUNUMBER,-1,true);
      mod_config.pu_name[rd.ReadString(REGSTR_PUNAME,mod_config.pu_name,KERTL_ARRAY_COUNT(mod_config.pu_name),true)] = 0;
      read_lines(rd);
    }
    else
     ret = false;
   }
   return ret;
 }

 void     __fastcall Tiec60870modem::read_lines  (TRegsReader & rd)
 {
    clear_lines();
    KeRTL::TMemoryStream ms;
    DWORD sz   = 0;
    DWORD type = 0;
    if(rd.GetValueType(REGBIN_LINES,&type,&sz,true))
    {
      ms.SetSize(sz);
      rd.ReadBytes(REGBIN_LINES,ms.GetMemory(),sz,true);
      ms.Seek(0,KeRTL::TStream::smFromBegin);
      DWORD lines_count = 0;
      DWORD stream_ver  = 0;
      ms>>lines_count>>stream_ver;
      while(lines_count)
      {
        Tiec60870line * line = new Tiec60870line;
        line->read(ms,stream_ver);
        if(!add_line(line)) free_line(line);
        --lines_count;

      }
    }
  #ifdef _DEBUG
    if(!get_lines_count())
    {
     IEC60870_LINE_CONFIG lc;
     lc.cp_number = 51;
     wcscpy(lc.host_addr,L"192.168.0.51");
     wcscpy(lc.cp_name  ,L"Телеканал M/M2 № 51");
     //_line_add(&lc);
    }
  #endif
 }

 void     __fastcall Tiec60870modem::write_lines (TRegsWriter & rw)
 {
   KeRTL::TMemoryStream ms;
   TLockHelper l(locker);
   DWORD lcnt = this->get_lines_count();
   ms<<lcnt;
   ms<<(DWORD)IEC60870_MOD_STREAM_VER;
   DWORD i = 0;
   while(i<lcnt)
   {
    Tiec60870line * line = dynamic_cast<Tiec60870line *>( get_line_byindex(i) );
    if(line) line->write(ms);
    ++i;
   }
   rw.WriteBytes(REGBIN_LINES,ms.GetMemory(),ms.GetSize(),true);
 }


 bool     __fastcall Tiec60870modem::write_settings     ()
 {
   bool ret = TModemBase::write_settings();
   if(ret)
   {
     TRegsWriter rw(DupKey(reg_key));
     ret = rw.IsRootOnly();
     if(ret)
     {
      if(cc_flags&CCFL_PUNUMBER)
      rw.WriteDword (REGDW_PUNUMBER,mod_config.pu_number,true);
      if(cc_flags&CCFL_PUNAME)
      rw.WriteString(REGSTR_PUNAME,mod_config.pu_name,true);
      if(cc_flags&CCFL_LINES)
         write_lines(rw);
     }
   }
   return ret;
 }

 BOOL     __fastcall Tiec60870modem::check_tcp_proto    ()
 {
    int proto[2] = {IPPROTO_TCP,0};
    WSAPROTOCOL_INFO pi[10] ={0};
    DWORD sz   = sizeof(pi);
    int cnt = WSAEnumProtocols(proto,pi,&sz);
    if(cnt <1)
       {

        SetLastError(WSAEPROTONOSUPPORT	);
        return FALSE;
       }
  return TRUE;
 }

 BOOL     __fastcall Tiec60870modem::check_pu_number    ()
 {
  TLockHelper l(locker);
  if(mod_config.pu_number && mod_config.pu_number<OTD_ADDR_MAXVALUE)
     return TRUE;
     //SetLastError(ERR_PU_NUMBER);
   return FALSE;
 }

 BOOL     __fastcall Tiec60870modem::can_start          (DWORD reason,LPARAM p2)
 {

     if(TBaseSocket::InitWS(MAKEWORD(2,2)) && check_pu_number() && check_tcp_proto())
       return TModemBase::can_start(reason,p2);
       else
       TBaseSocket::DeinitWS();
     return FALSE;
 }

 DWORD    __fastcall Tiec60870modem::start              (DWORD reason,LPARAM p2)
 {
     pu_diag = OTD_DIAG_NODATA|OTD_DIAG_CPMASK;
     DWORD ret =  TModemBase::start(reason,p2);
     if(GKH_RET_SUCCESS == ret)
        do_recv_pu_data(true);
     return ret;
 }

 DWORD    __fastcall Tiec60870modem::stop               (DWORD reason)
 {
     pu_diag = OTD_DIAG_NODATA|OTD_DIAG_CPMASK;
     do_recv_pu_data(false);
     DWORD ret =  TModemBase::stop(reason);
     if(ret == GKH_RET_SUCCESS)
        TBaseSocket::DeinitWS();
     return ret;
 }

 LRESULT  __fastcall Tiec60870modem::processing         (DWORD cmd,LPARAM p1,LPARAM p2)
 {
   LRESULT ret;
   switch(cmd)
   {
    case  CMD_IEC60870_GET_LINE : ret = _line_get((LPIEC60870_LINE_CONFIG)p1,p2)  ;break;
    case  CMD_IEC60870_ADD_LINE : ret = _line_add((LPIEC60870_LINE_CONFIG)p1)     ;break;
    case  CMD_IEC60870_UPD_LINE : ret = _line_upd((LPIEC60870_LINE_CONFIG)p1,p2)  ;break;
    case  CMD_IEC60870_DEL_LINE : ret = _line_del(p1)                             ;break;

    case  CMD_IEC60870_LINE_GET_ASDU_RECORDS_COUNT      : ret = get_asdu_records_count(p1,p2);break;
    case  CMD_IEC60870_LINE_ENUM_ASDU_RECORDS           : ret = enum_asdu_records    (p1,p2)  ;break;
    case  CMD_IEC60870_LINE_GET_ASDU_RECORD             : ret = get_asdu_records     (p1,p2)  ;break;
    case  CMD_IEC60870_LINE_ASDU_RECORD_BIND_OTD        : ret = bind_record_otd      (p1,p2)  ;break;
    case  CMD_IEC60870_LINE_GROUP_CREATE                : ret = otd_create_group     (p1,p2)  ;break;
    case  CMD_IEC60870_LINE_GROUP_MAX_NUMBER            : ret = get_group_max_number (p1,p2)  ;break;
    case  CMD_IEC60870_LINE_ASDU_RECORD_BIND_OTD_RC_CMD : ret = bind_record_otd_rccmd(p1,p2)  ;break;

     default : ret = TModemBase::processing(cmd,p1,p2);
     break;
   }
   return ret;
 }

 DWORD    __fastcall Tiec60870modem::get_window_module  (wchar_t * buf,DWORD bsz)
 {
    static wchar_t * module_wnd_name = L"iec60870wnd.dll";
    safe_strcpyn(buf,module_wnd_name,bsz);
    int ret = lstrlenW(buf);
    return ret;
 }

 void    __fastcall  Tiec60870modem::do_recv_pu_data(bool inc_name )
 {
      TLockHelper l(locker);
      otd_proto_param opp;
      otd_proto       op;
      op.dw_size      = sizeof(op);
      ZeroMemory(&opp,sizeof(opp));
      opp.dw_size     = sizeof(opp);
      opp.parts       = OTD_PROTO_PART_DIAG;
      if(inc_name)
      {
       opp.parts       |= OTD_PROTO_PART_NAME;
       opp.name_sz     = lstrlenW(mod_config.pu_name)+1;
      }
      BYTE   buffer [1024];
      DWORD need_size = otd_proto_calc_size_ex(&opp,0);
      LPBYTE  buf;
      buf = (need_size>sizeof(buffer)) ?    new BYTE[need_size] :   buffer;
      if(buf)
      {
      	 otd_proto_format_ex(buf,need_size,&opp,0,&op);
      	 op.addr->addr = -1;
      	 op.addr->pu   = mod_config.pu_number;
         *op.diag      = get_pu_diag(false);
      	 otd_text_set(op.name,mod_config.pu_name);
      	 queue_rxdata(0,-1,0,FA_OTD,buf,op.proto_size,false);
      	 if(buf!=buffer) delete [] buf;
       }
  }

  DWORD __fastcall Tiec60870modem::get_pu_diag(bool rescan)
  {
    if(rescan) scan_pu_diag();
    return   pu_diag;
  }

   DWORD __fastcall Tiec60870modem::scan_pu_diag  ()
   {
     return pu_diag;
   }

   LRESULT __fastcall Tiec60870modem::_line_get(LPIEC60870_LINE_CONFIG ln_conf,DWORD line_num)
   {
    LRESULT ret = GKH_RET_ERROR;
    if(check_data_size(ln_conf,sizeof(*ln_conf)))
    {
     Tiec60870line * line = get_iec60870_line(line_num);
     if(line && line->get_config(*ln_conf))
        ret =  GKH_RET_SUCCESS ;
    }
    return  ret;
   }


   LRESULT __fastcall Tiec60870modem::_line_add (LPIEC60870_LINE_CONFIG ln_conf)
   {
    LRESULT ret = GKH_RET_ERROR;
    if(check_data_size(ln_conf,sizeof(*ln_conf)))
    {
      Tiec60870line * line = new Tiec60870line(*ln_conf);
      if(!add_line(line))
         delete line;
         else
         {
          if(this->is_running())   line->start();
          ret = GKH_RET_SUCCESS;
          cc_flags |=  CCFL_LINES;
         }
    }


    return  ret;
   }

   LRESULT __fastcall Tiec60870modem::_line_upd (LPIEC60870_LINE_CONFIG ln_conf,DWORD line_num)
   {
    LRESULT ret = GKH_RET_ERROR;
    if(check_data_size(ln_conf,sizeof(*ln_conf)))
    {
     Tiec60870line * line = get_iec60870_line(line_num);
     if(line && line->set_config(*ln_conf))
         {
          ret =  GKH_RET_SUCCESS ;
          cc_flags |=  CCFL_LINES;
         }
    }
    return  ret;
   }

   LRESULT __fastcall Tiec60870modem::_line_del (DWORD line_num)
   {
    LRESULT ret = GKH_RET_ERROR;
    if(remove_line(line_num) )
    {
      ret =  GKH_RET_SUCCESS ;
      cc_flags |=  CCFL_LINES;
    }
    return  ret;
   }

 void       __fastcall Tiec60870modem::do_report(DWORD evt_type,wchar_t * rep_text,LPVOID data,DWORD dt_sz)
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


 LRESULT __fastcall  Tiec60870modem::get_asdu_records_count(DWORD line_number,DWORD type)
 {
  Tiec60870line * line = dynamic_cast<Tiec60870line *>( get_line(line_number) );
  if(line)
  {
     return line->get_asdu_records_count(type);
  }
  return GKH_RET_ERROR;

 }

 LRESULT __fastcall  Tiec60870modem::enum_asdu_records     (DWORD line_number,LPARAM p2)
 {
  Tiec60870line * line = dynamic_cast<Tiec60870line *>( get_line(line_number) );
  if(line)
  {
     lpiec60870_record lprec = static_cast<lpiec60870_record>((LPVOID)p2);
     if(this->check_data_size(lprec,sizeof(*lprec),true))
        return line->enum_asdu_records(lprec);
  }
  return GKH_RET_ERROR;
 }

 LRESULT __fastcall  Tiec60870modem::get_asdu_records      (DWORD line_number,LPARAM p2)
 {
  Tiec60870line * line = dynamic_cast<Tiec60870line *>( get_line(line_number) );
  if(line)
  {
     lpiec60870_record lprec = static_cast<lpiec60870_record>((LPVOID)p2);
     if(this->check_data_size(lprec,sizeof(*lprec),true))
        return line->get_asdu_record(lprec);
  }
  return GKH_RET_ERROR;
 }

 LRESULT __fastcall  Tiec60870modem::otd_create_group      (DWORD line_number,LPARAM p2)
 {
  Tiec60870line * line = dynamic_cast<Tiec60870line *>( get_line(line_number) );
  if(line)
  {
   LPIEC60870_LINE_GROUP_PARAM lgc = static_cast<LPIEC60870_LINE_GROUP_PARAM>((LPVOID)p2);
   if(check_data_size(lgc,sizeof(*lgc)))
   {
    return line->otd_create_group(lgc);
   }
  }
  return GKH_RET_ERROR;
 }

 LRESULT __fastcall  Tiec60870modem::get_group_max_number(DWORD line_number, LPARAM p2)
 {
  Tiec60870line * line = dynamic_cast<Tiec60870line *>( get_line(line_number) );
  if(line)
  {
   LPIEC60870_LINE_GROUP_PARAM lgc = static_cast<LPIEC60870_LINE_GROUP_PARAM>((LPVOID)p2);
   if(check_data_size(lgc,sizeof(*lgc)))
   {
     DWORD max_number = 0;
     line->get_group_min_max_numbers(lgc->otd_fa,lgc->otd_group,NULL,&max_number);
     return max_number;
   }
  }
  return GKH_RET_ERROR;

 }

 LRESULT __fastcall  Tiec60870modem::bind_record_otd       (DWORD line_number,LPARAM p2    )
 {
  Tiec60870line * line = dynamic_cast<Tiec60870line *>( get_line(line_number) );
  if(line)
  {
     lpiec60870_record lprec = static_cast<lpiec60870_record>((LPVOID)p2);
     if(this->check_data_size(lprec,sizeof(*lprec),true))
        {
         cc_flags |=  CCFL_LINES;
         return line->record_bind(lprec);
        }
  }
  return GKH_RET_ERROR;
 }

 LRESULT __fastcall  Tiec60870modem::bind_record_otd_rccmd(DWORD line_number,LPARAM p2    )
 {
  Tiec60870line * line = dynamic_cast<Tiec60870line *>( get_line(line_number) );
  if(line)
  {
     lpiec60870_record lprec = static_cast<lpiec60870_record>((LPVOID)p2);
     if(this->check_data_size(lprec,sizeof(*lprec),true))
        {
         cc_flags |=  CCFL_LINES;
         return line->record_bind_rccmd(lprec);
        }
  }
  return GKH_RET_ERROR;
 }



