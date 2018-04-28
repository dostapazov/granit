#pragma hdrstop
#include "cpc152dev.hpp"
#include <alloc.h>

extern "C" void tss_cleanup_implemented(void)
{
 //stub only for boost::thread
}

static THeap heap(TSysInfo().GetPageSize()<<8); // 512 Кб
static DWORD total_mem_alloc = 0;

void * operator new(size_t sz)
{
 void * ptr = NULL;
 if(heap.IsValid())
 {
  ptr =  heap.Alloc(sz);
 }
 else
 ptr = malloc(sz);
 return ptr;
}

void * operator new [](size_t sz)
{
 void * ptr = NULL;

 if(heap.IsValid())
 {
  ptr = heap.Alloc(sz);
 }
 else
 ptr = malloc(sz);
 return ptr;
}

void operator delete( void * ptr)
{
  if(ptr && heap.Validate(ptr))
    {

     heap.Free(ptr);
    }
    else
    {
    if(ptr && _USEDENTRY == heapchecknode(ptr)) free(ptr);
    }
}

void operator delete[]( void * ptr)
{

  if(ptr && heap.Validate(ptr))
    {
     heap.Free(ptr);
    }
   else
   {
    if(ptr && _USEDENTRY == heapchecknode(ptr)) free(ptr);
   }
}

DWORD   __fastcall Tcpc152controller::get_mem_used()
 {
  return heap.IsValid() ? heap.GetMemoryUsed() : GKH_RET_ERROR;
  //total_mem_alloc;

  /*heapinfo hi;
  heapwalk(&hi);
  return hi.in_use;*/
 }


  Tcpc152controller::Tcpc152controller ()
  {
   ZeroMemory(&dev_cfg,sizeof(dev_cfg));
   dev_cfg.dw_size    = sizeof(dev_cfg);
   lock_param    = GKHB_AUTO_LOCK_OFF;
   alloc_gkhandle();
   reg_reports();
   TBaseSocket::InitWS(MAKEWORD(2,2));
  }

  void    __fastcall Tcpc152controller::reg_reports()
  {
   rep_id = report_reg_event_type(L"CPC152DEV",L"CPC152 Контроллер");
  }


  void       __fastcall Tcpc152controller::do_report(DWORD evt_type,wchar_t * rep_text,LPVOID data,DWORD dt_sz)
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


  void          __fastcall Tcpc152controller::free_line          (modem_line * ml)
  {
    cpc152dev_line * cline = dynamic_cast<cpc152dev_line *>(ml);
    if(cline)
       {
         cline->stop();
         delete cline;
       }
       else
       delete ml;
  }

  int           __fastcall Tcpc152controller::convert_rx_data    (LPWORD fa,LPBYTE in,int in_len,LPBYTE out,DWORD out_sz)
  {
     memcpy(out,in,in_len);
     return in_len;
  }

  DWORD         __fastcall Tcpc152controller::get_config_data    (DWORD mask,LPVOID buf,DWORD bsz)
  {
    DWORD ret = GKH_RET_ERROR;
    LPCPC152DEV_CONFIG mcfg = (LPCPC152DEV_CONFIG)buf;
    if(buf && bsz>=sizeof(dev_cfg))
    {
     TLockHelper l  (locker);
	 memcpy(mcfg,&dev_cfg,sizeof(dev_cfg));
     ret = GKH_RET_SUCCESS;
    }
    return ret;
  }

  bool          __fastcall Tcpc152controller::set_config_data    (DWORD mask,LPVOID buf,DWORD bsz)
  {
   bool   ret   = false;
   DWORD  error ;
   LPCPC152DEV_CONFIG mcfg = (LPCPC152DEV_CONFIG)buf;
   if(this->check_data_size(buf,sizeof(dev_cfg)))
   {
    ret = true;
    TLockHelper l  (locker);
    if(mask & CCFL_MODEM_NUMBER)
     {
      DWORD old_num;
      if(GKH_RET_SUCCESS == set_modem_number(mcfg->modem_number,&old_num))
        {
         dev_cfg.modem_number = mcfg->modem_number;
        }
        else
        {
        error = GetLastError();
        SetLastError(error);
        ret = false;
        }
     }

     if(mask & CCFL_PUNUMBER)
		{
         dev_cfg.pu_number = mcfg->pu_number;
        }
     if(mask & CCFL_PUNAME)
     {
       safe_strcpyn(dev_cfg.pu_name , mcfg->pu_name,KERTL_ARRAY_COUNT(dev_cfg.pu_name));
       if(is_running()) do_recv_puname();
     }
   }
   return ret;

  }

  void    __fastcall Tcpc152controller::do_recv_puname()
  {
      TLockHelper l(locker);
      otd_proto_param opp;
      otd_proto       op;
      op.dw_size      = sizeof(op);
      ZeroMemory(&opp,sizeof(opp));
      opp.dw_size     = sizeof(opp);
      opp.parts       = OTD_PROTO_PART_NAME;
      opp.name_sz     = lstrlenW(dev_cfg.pu_name)+1;
      BYTE   buffer [1024];
      DWORD need_size = otd_proto_calc_size_ex(&opp,0);
      LPBYTE  buf;
      if(need_size>sizeof(buffer))
       buf     = new BYTE[need_size];
       else
       buf = buffer;
      if(buf)
		{
		 otd_proto_format_ex(buf,need_size,&opp,0,&op);
		 op.addr->addr = -1;
		 op.addr->pu   = dev_cfg.pu_number;
		 otd_text_set(op.name,dev_cfg.pu_name);
		 queue_rxdata(0,-1,0,FA_OTD,buf,op.proto_size,false);
		 if(buf!=buffer)
			delete [] buf;
		}
  }


  bool          __fastcall Tcpc152controller::compare_config_data(DWORD mask,LPVOID buf,DWORD bsz,LPDWORD ch_mask,BOOL * restart)
  {
   bool  ret = false;
   BOOL  rst   = FALSE;
   DWORD chmask = 0;
   LPCPC152DEV_CONFIG mcfg = (LPCPC152DEV_CONFIG )buf;
   if(check_data_size(buf,sizeof(dev_cfg)))
   {
	ret = true;
	if(( mask & CCFL_MODEM_NUMBER) && mcfg->modem_number!=get_modem_number()) // Смена номера модема
	   { chmask|=CCFL_MODEM_NUMBER,rst = TRUE;}

	  TLockHelper l(locker);
	  if(( mask & CCFL_PUNUMBER) && mcfg->pu_number != dev_cfg.pu_number)
		  chmask|=CCFL_PUNUMBER,rst = TRUE;

	  if(( mask & CCFL_PUNAME) && lstrcmpW(mcfg->pu_name,dev_cfg.pu_name))
		  chmask|=CCFL_PUNAME ;
   }

   if(restart) *restart = rst;
   if(ch_mask) *ch_mask = chmask;
   return ret;
  }

  bool          __fastcall Tcpc152controller::read_settings()
  {
   bool ret = false;
   TLockHelper l(locker);
   ret = TModemBase::read_settings();
   if(ret)
   {
	 memset(&dev_cfg,0,sizeof(dev_cfg));
	 dev_cfg.dw_size = sizeof(dev_cfg);
	 dev_cfg.modem_number = get_modem_number();
	 TRegsReader rd(DupKey(reg_key));
	 ret = rd.IsRootOnly();
	 if(ret)
	 {
	  dev_cfg.pu_number = rd.ReadDword(REGDW_CPC152_PUNUMBER,0,true);
	  dev_cfg.pu_name    [0] = 0;
	  dev_cfg.pu_name[rd.ReadString(REGSTR_CPC152_PUNAME,dev_cfg.pu_name,KERTL_ARRAY_COUNT(dev_cfg.pu_name),true)] = 0;

	  KeRTL::TMemoryStream ms;
	  DWORD type,stream_size = 0;
	  rd.GetValueType(REGBIN_CPC152_LINES,&type,&stream_size,true);

	  ms.SetSize(stream_size);
	  rd.ReadBytes(REGBIN_CPC152_LINES,ms.GetMemory(),stream_size,true);
	  ms.Seek(0,KeRTL::TStream::smFromBegin );
	  read_lines(ms);
	  cc_flags = 0;
	 }
   }
   return ret;
  }

  bool          __fastcall Tcpc152controller::write_settings()
  {
   bool ret = false;
   TLockHelper l(locker);
   ret = TModemBase::write_settings();
   if(ret && cc_flags)
   {
	 TRegsWriter rw(DupKey(reg_key));
	 ret = rw.IsRootOnly();
	 if(ret)
	 {
	  if(cc_flags & CCFL_PUNUMBER)
	  rw.WriteDword(REGDW_CPC152_PUNUMBER,dev_cfg.pu_number,true);

	  if(cc_flags & CCFL_PUNAME  )
	   rw.WriteString(REGSTR_CPC152_PUNAME,dev_cfg.pu_name,true);

	 }
	if(cc_flags& CCFL_LINES)
	  {
	   KeRTL::TMemoryStream  ms;
	   write_lines(ms);
	   rw.WriteBytes(REGBIN_CPC152_LINES,ms.GetMemory(),ms.get_using_size(),true);
	  }
	  cc_flags = 0;
   }

   return ret;
  }


  #define CPC152_LINES_STREAM_VER_0 0
  #define CPC152_LINES_STREAM_VER_1 1
  #define CPC152_LINES_STREAM_VER_2 2
  #define CPC152_LINES_STREAM_VER_3 3
  #define CPC152_LINES_STREAM_VER   4

  void    __fastcall Tcpc152controller::read_lines (KeRTL::TMemoryStream & ms)
  {
    ms.Seek(0,KeRTL::TStream::smFromBegin);
    int ver = 0,lines_count = 0;
    ms>>ver>>lines_count;
    CPC152_LINE_CONFIG lc;
    lc.dw_size = sizeof(lc);
    lc.mask = -1;

    for(int i = 0;i<lines_count && !ms.Eos();i++)
    {
       ms>>lc.cp_number>>lc.ip_port;
       wchar_t * ptr;
       ptr =  lc.ip_addr;
       ms.read_string(ptr);
       ptr = lc.cp_name;
	   ms.read_string(ptr);
	   ms>>lc.freq>>lc.samples_freq>>lc.samples_count>>lc.resamping>>lc.dft_frame_size;
       if(ver>CPC152_LINES_STREAM_VER_0)
       {
        ms>>lc.freq_gate>>lc.dft_percent;
        if(ver>CPC152_LINES_STREAM_VER_1 )
          ms>>lc.calc_varint;
        if(ver>CPC152_LINES_STREAM_VER_2)
          {
           ms>>lc.corr_enable>>lc.corr_max_kvants>>lc.corr_must_kvants>>lc.corr_zero;
          }
          else
          lc.corr_enable = lc.corr_max_kvants = lc.corr_must_kvants = lc.corr_zero = 0;
       }
       else
       {
         lc.freq_gate   = 5.0;
         lc.dft_percent = 99.9;
       }

	   cpc152dev_line * line = new cpc152dev_line(lc.cp_number);
	   line->config_set(&lc);
	   line->alarm_start_read(ms);
           if(ver>CPC152_LINES_STREAM_VER_3)
              line->dft_enabled_read(ms);
	   add_line(line);

    }

  }



  void    __fastcall Tcpc152controller::write_lines(KeRTL::TMemoryStream & ms)
  {
     CPC152_LINE_CONFIG lc;
     lc.dw_size = sizeof(lc);
     ms.Seek(0,KeRTL::TStream::smFromBegin);
     int lines_count = 0;
     ms<<(int)CPC152_LINES_STREAM_VER<<(int)lines_count;
     TLockHelper l(locker);
     lines_map::iterator i   = lines.begin();
     lines_map::iterator end = lines.end  ();
     while(i!=end)
     {
       cpc152dev_line * line = dynamic_cast<cpc152dev_line*>(i->second);
       if(line)
         {
          line->config_get(&lc);
          ms<<lc.cp_number<<lc.ip_port;
          ms.write_string(lc.ip_addr);
          ms.write_string(lc.cp_name);
		  ms<<lc.freq<<lc.samples_freq<<lc.samples_count<<lc.resamping<<lc.dft_frame_size<<lc.freq_gate<<lc.dft_percent<<lc.calc_varint
                    <<lc.corr_enable<<lc.corr_max_kvants<<lc.corr_must_kvants<<lc.corr_zero;
          line->alarm_start_write(ms);
          line->dft_enabled_write(ms);
          ++lines_count;
         }
       ++i;
     }
     ms.Seek(sizeof(int),TStream::smFromBegin);
     ms<<lines_count;
  }


  cpc152dev_line * __fastcall Tcpc152controller::get_cpc152controller_line(DWORD dev_num)
  {
    return dynamic_cast<cpc152dev_line*>(this->get_line(dev_num) );
  }


BOOL    __fastcall Tcpc152controller::can_start(DWORD reason,LPARAM p2)
{
  BOOL ret = TModemBase::can_start(reason,p2);
  if(ret)
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
  }
  return ret;

}

  DWORD   __fastcall Tcpc152controller::start    (DWORD reason,LPARAM p2)
  {
    reg_reports();
    do_recv_puname();
    return TModemBase::start(reason,p2);
  }
  DWORD   __fastcall Tcpc152controller::stop     (DWORD reason)
  {
    return TModemBase::stop(reason);
  }

  DWORD   __fastcall Tcpc152controller::get_window_module  (wchar_t * buf,DWORD bsz)
  {
   safe_strcpyn(buf,L"cpc152dev_wnd.dll",bsz);
   return lstrlenW(buf);
  }

  LRESULT __fastcall Tcpc152controller::processing         (DWORD cmd,LPARAM p1,LPARAM p2)
  {
    LRESULT ret = GKH_RET_ERROR;
    switch(cmd)
    {
     case CMD_CPC152DEV_ADD_LINE:
     ret = line_add((LPCPC152_LINE_CONFIG)p1);
     break;
     case CMD_CPC152DEV_GET_LINE:
     ret = line_get((LPCPC152_LINE_CONFIG)p1);
     break;
     case CMD_CPC152DEV_UPD_LINE:
     ret = line_upd((LPCPC152_LINE_CONFIG)p1,p2);
     break;
     case CMD_CPC152DEV_DEL_LINE:
		ret = this->line_del(p1);
	 break;
	 case CMD_CPC152_LINE_ALARM_START_ENUM:
		ret = alarm_start_enum(p1,(LPCPC152_ALARM_START)p2);
	 break;
	 case CMD_CPC152_LINE_ALARM_START_ADD:
		ret = alarm_start_add(p1,(LPCPC152_ALARM_START)p2);
	 break;
	 case CMD_CPC152_LINE_ALARM_START_DEL:
		ret = alarm_start_del(p1,(LPCPC152_ALARM_START)p2);
	 break;
     case CMD_CPC152DEV_WRITE_OSCILOGRAMM:
          ret = start_write_oscilogramm(p1,p2);
     break;
     case CMD_CPC152DEV_ENUM_ANALOGS :
          ret = enum_analogs(p1,(LPCPC152_ANALOG_STATS)p2);
     break;
     case CMD_CPC152DEV_GET_ANALOG_STATS:
          ret = get_analog_stats(p1,(LPCPC152_ANALOG_STATS)p2);
     case CMD_CPC152_LINE_GET_DFT_ENABLED :
          ret = get_line_dft_enabled(p1,(LPCPC152_DFT_ENABLED)p2);
          break;
     case CMD_CPC152_LINE_SET_DFT_ENABLED :
          ret = set_line_dft_enabled(p1,(LPCPC152_DFT_ENABLED)p2);
          break;
     break;
	 default : ret = TModemBase::processing(cmd,p1,p2);
	 break;
    }
    return  ret;
  }

  LRESULT   __fastcall Tcpc152controller::start_write_oscilogramm(int lnum , int lenght)
  {
    LRESULT ret = GKH_RET_ERROR;
    cpc152dev_line * dev_line  = get_cpc152controller_line(lnum);
    if(dev_line)
    {
      ret = GKH_RET_SUCCESS;
      dev_line->start_write_oscilogramm(lenght);
    }
    return ret;
  }

  DWORD   __fastcall Tcpc152controller::line_add(LPCPC152_LINE_CONFIG lc)
  {
   DWORD ret = GKH_RET_ERROR;
   if(check_data_size(lc,sizeof(*lc)))
   {
    cpc152dev_line * line = new cpc152dev_line(lc->cp_number);
    if(add_line(line))
     {
      ret = line->config_set(lc);
      if(ret == GKH_RET_SUCCESS)
       {
        cc_flags |= CCFL_LINES;
        if(this->is_running())
          line->start();
       }
        else
         remove_line(lc->cp_number);
     }
     else
     delete line;

   }
   return ret;
  }

  DWORD   __fastcall Tcpc152controller::line_get(LPCPC152_LINE_CONFIG lc)
  {
   DWORD ret = GKH_RET_ERROR;
   if(check_data_size(lc,sizeof(*lc)))
   {
    ret = GKH_RET_ERROR;
    cpc152dev_line * dev_line  = get_cpc152controller_line(lc->cp_number);
    if(dev_line)
       ret = dev_line->config_get(lc);
   }
   return ret;
  }

  DWORD   __fastcall Tcpc152controller::line_upd(LPCPC152_LINE_CONFIG lc,DWORD old_num)
  {
   DWORD ret = GKH_RET_ERROR;
   if(check_data_size(lc,sizeof(*lc)))
   {
    cpc152dev_line * line = this->get_cpc152controller_line(old_num);
    if(line)
       ret = line->config_set(lc);
     if(ret == GKH_RET_SUCCESS)
      cc_flags |= CCFL_LINES;
   }
   return ret;
  }

  DWORD   __fastcall Tcpc152controller::line_del(DWORD line_num)
  {
   DWORD ret = GKH_RET_SUCCESS;
   if(remove_line(line_num))
    cc_flags |= CCFL_LINES;
    else
    DWORD ret = GKH_RET_ERROR;
   return ret;
  }

  int        __fastcall Tcpc152controller::create_group (sotd_proto & sop,bool discrete,int cp_num,int group_num,int param_count,int ext_size,bool float_type)
  {
    otd_proto_param op;
    bzero(&op,sizeof(op));
    op.dw_size = sizeof(op);
    op.parts = OTD_PROTO_PART_KPK|OTD_PROTO_PART_DIAG|OTD_PROTO_PART_DATA|
               OTD_PROTO_PART_PERSONAL_CHMASK|OTD_PROTO_PART_TIMESTAMP
               ;
    if(!discrete)
       op.parts |= OTD_PROTO_PART_TIMESTAMP_END;
    if(ext_size)
    {
     op.ext_size = ext_size;
     op.parts   |= OTD_PROTO_PART_PROTOEXTENSION;
     op.ext_type = OTD_BYTE;
    }

    op.cm_param.first = op.dt_param.first = 0;
    op.cm_param.count = op.dt_param.count = param_count;

    op.dt_param.type  = discrete ? OTD_DISCRETE : (float_type ? OTD_FLOAT : OTD_ANALOG_WORD );
    op.cm_param.type  = OTD_BYTE;
    DWORD need_sz = otd_proto_calc_size_ex(&op,0);
    LPBYTE ptr = new BYTE[need_sz];
    otd_proto_format_ex(ptr,need_sz,&op,0,&sop());
    sop.op.addr->pu  = this->dev_cfg.pu_number;
    sop.op.addr->cp  = cp_num;
    sop.op.addr->fa  = discrete ? OTD_FA_DISCRETE : (float_type ? OTD_FA_ANALOG : OTD_FA_ANALOG);
    sop.op.addr->sb  = group_num;
	*sop.op.diag     = OTD_DIAG_NODATA;
    TLockHelper  l (otd_locker);
	proto_pointer pp = otd_data.insert(sop());
	return (int) std::distance(otd_data.begin(),pp);
  }

  int        __fastcall Tcpc152controller::get_otd_range(otd_addr & addr, proto_pointer & ptr_beg,proto_pointer & ptr_end)
  {
     return otd_data.get_range(addr,ptr_beg,ptr_end);
  }


  void        __fastcall Tcpc152controller::otd_set_diag  (BYTE cp_number,DWORD diag)
  {
	//Установка диагностики
	sotd_addr addr(-1);
	addr.pu = this->dev_cfg.pu_number;
	addr.cp = cp_number;
	TLockHelper l(otd_locker);
	proto_pointer ptr_beg,ptr_end;

	if(otd_data.get_range(addr,ptr_beg,ptr_end))
	{
	 while(ptr_beg<ptr_end)
	 {
	   if(otd_addr_include(ptr_beg->op.addr,&addr) && ptr_beg->op.diag)
		  *ptr_beg->op.diag = diag;
	   ++ptr_beg;
	 }
	}

  }


  LRESULT __fastcall Tcpc152controller::alarm_start_enum(DWORD ln_idx,LPCPC152_ALARM_START ast)
  {
   LRESULT ret = GKH_RET_ERROR;
   cpc152dev_line * cp = get_cpc152controller_line(LOWORD(ln_idx));
   if(cp)
	  {
	   if(cp->alarm_start_enum(HIWORD(ln_idx),ast ) )
		  ret = GKH_RET_SUCCESS;
	  }
   return  ret;
  }

  LRESULT __fastcall Tcpc152controller::alarm_start_add (DWORD ln,LPCPC152_ALARM_START ast)
  {
   LRESULT ret = GKH_RET_ERROR;
   cpc152dev_line * cp = get_cpc152controller_line(LOWORD(ln));
   if(cp)
	  {
	   if(cp->alarm_start_add( ast ) )
	   {
		  ret = GKH_RET_SUCCESS;
    	  cc_flags |= CCFL_LINES;

	   }
	  }
   return  ret;
  }

  LRESULT __fastcall Tcpc152controller::alarm_start_del (DWORD ln,LPCPC152_ALARM_START ast)
  {
   LRESULT ret = GKH_RET_ERROR;
   cpc152dev_line * cp = get_cpc152controller_line(LOWORD(ln));
   if(cp)
	  {
	   if(cp->alarm_start_del( ast ) )
		  {
		   ret = GKH_RET_SUCCESS;
		   cc_flags |= CCFL_LINES;
		  }
	  }
   return  ret;
  }

  LRESULT __fastcall Tcpc152controller::get_analog_stats(DWORD ln,LPCPC152_ANALOG_STATS  sts)
  {
   LRESULT ret = GKH_RET_ERROR;
   cpc152dev_line * cp = get_cpc152controller_line(LOWORD(ln));
   if(cp)
   {
     if(cp->get_analog_stats(sts))
        ret = GKH_RET_SUCCESS;
   }
   return ret;
  }

  LRESULT __fastcall Tcpc152controller::enum_analogs    (DWORD ln,LPCPC152_ANALOG_STATS  sts)
  {
   LRESULT ret = GKH_RET_ERROR;
   cpc152dev_line * cp = get_cpc152controller_line(LOWORD(ln));
   if(cp)
   {
     if(cp->enum_analogs(sts))
        ret = GKH_RET_SUCCESS;
   }
   return ret;
  }

  LRESULT __fastcall Tcpc152controller::get_line_dft_enabled(DWORD ln ,LPCPC152_DFT_ENABLED data)
  {
   LRESULT ret = GKH_RET_ERROR;
   cpc152dev_line * cp = get_cpc152controller_line(LOWORD(ln));
   if(cp && check_data_size(data,sizeof(*data),true))
   {
    cp->get_dft_enabled(data);
    ret = GKH_RET_SUCCESS;
   }
   return ret;
  }

  LRESULT __fastcall Tcpc152controller::set_line_dft_enabled(DWORD ln ,LPCPC152_DFT_ENABLED data)
  {
   LRESULT ret = GKH_RET_ERROR;
   cpc152dev_line * cp = get_cpc152controller_line(LOWORD(ln));
   if(cp && check_data_size(data,sizeof(*data),true))
   {
    if(cp->set_dft_enabled(data))
      {
       ret = GKH_RET_SUCCESS;
       cc_flags |=CCFL_LINES;
      }
   }
   return ret;

  }

 LRESULT    __fastcall Tcpc152controller::send        (LPMPROTO_HEADER mph,DWORD sz)
 {
  if(mph && mph->fa == FA_OTD && mph->data_size>=(sizeof(otd_addr)+sizeof(WORD)))
  {
    sotd_addr sa(*(LPDWORD)mph->data);
    /*Проверка номера ПУ или для всех ПУ*/
    if(sa.pu == OTD_ADDR_MAXVALUE || sa.pu == this->get_pu_number())
    {
      if(OTD_ADDR_ISQUERY(&sa))
      {
       DWORD parts;
       otd_proto_get_verpart((LPBYTE)mph->data,mph->data_size,NULL,&parts);
       if(parts&OTD_PROTO_PART_NAME)
          do_recv_puname();
      }
    }
  }

  TModemBase::send(mph,sz);
  return GKH_RET_SUCCESS;
 }


