#include <stdio.h>
#include <tregstry.hpp>
#include <kfname.hpp>

#pragma hdrstop

#include "granit_modem.hpp"
#include "granit.hpp"

char sig[] = "[GranitN-SC v01.00]****";

 TSCGranitModem::TSCGranitModem()
 {
   lock_param = GKHB_AUTO_LOCK_OFF;
   alloc_gkhandle((wchar_t*)NULL);
   ZeroMemory(&settings,sizeof(settings));
   settings.dw_size=sizeof(settings);
   settings.modem_number = -1;
   settings.pu_number    = -1;
   this->reg_reports() ;
 };

  void    __fastcall TSCGranitModem::reg_reports()
  {
   wchar_t rep_title[MAX_PATH];
   get_lang_string(IDS_SC_EVENT_LOGTITLE,rep_title,sizeof(rep_title)/sizeof(wchar_t));
   grsc_event_rep = report_reg_event_type(L"GRSC_EVENTS",rep_title);
   get_lang_string(IDS_SC_TUTR_LOGTITLE,rep_title,sizeof(rep_title)/sizeof(wchar_t));
   grsc_tutr_rep  = report_reg_event_type(L"GRSC_TUTR",rep_title);
  }


 int     __fastcall TSCGranitModem::convert_rx_data(LPWORD /*fa*/,LPBYTE in,int in_len,LPBYTE out,DWORD out_sz)
 {
  int ret;
  if((DWORD)in_len<=out_sz)
  {
   memcpy(out,in,in_len);
   otd_proto op;
   op.dw_size = sizeof(op);
   if(otd_proto_parser(out,out_sz,&op,NULL))
   {
	{
	 TLockHelper l(locker);
	 op.addr->pu = this->settings.pu_number;
	}
	if(op.kpk)
           otd_proto_protect(&op,op.proto_size,OTD_DEF_POLINOM);
   } 
   ret = op.proto_size;
  }
  else
  ret = -in_len;
  return ret;
 }

 static wchar_t wm_name[] = L"granitmdmwnd.dll";

 DWORD  __fastcall TSCGranitModem::get_window_module (wchar_t * buf,DWORD bsz)
 {

   int len = lstrlenW(wm_name);
   if(buf && int(bsz) > len)
    {
     SetLastError(0);
     safe_strcpy(buf,wm_name);
    }
    else
    {SetLastError(MERR_INVALID_BUFFER_SIZE);len=-len;}
   return DWORD(len);
 }

 void        __fastcall  TSCGranitModem::release(LPVOID arg)
 {
  io_iface.stop(MODULE_STOP_RELEASE);
  io_iface.close();
  TModemBase::release(arg);
 }


 void    __fastcall TSCGranitModem::config_change(DWORD floor_num)
 {
   TLockHelper l(locker);
   cc_flags|=(CCFL_FLOOR0>>(floor_num & 0x3));
 }

 DWORD   __fastcall TSCGranitModem::get_config_data(DWORD /*mask*/,LPVOID buf,DWORD bsz)
 {
  DWORD ret = GKH_RET_ERROR;
  lpgrsc_settings gms  = (lpgrsc_settings)buf;
  if(buf && bsz>= sizeof(*gms) && !IsBadWritePtr(buf,sizeof(*gms)))
    {
	  TLockHelper l(locker);
	  memcpy(gms,&settings,sizeof(*gms));
      ret = GKH_RET_SUCCESS;
    }
  return ret;
 }

 bool    __fastcall TSCGranitModem::check_config_data  (DWORD mask,LPVOID buf,DWORD /*bsz*/)
 {
  lpgrsc_settings gms  = (lpgrsc_settings)buf;
  if((mask&CCFL_MODEM_NUMBER) && gms->modem_number == 0)
     return false;
  if((mask&CCFL_PUNUMBER) && gms->pu_number>=OTD_ADDR_MAXVALUE)
      return false;
  if((mask & CCFL_DEVICE) && !(*gms->io_module) &&!(*gms->device_name))
      return false;
  /*if((mask & CCFL_FILENAME) && !(*gms->save_file_name))
      return false;*/
  return true;
 }

 bool    __fastcall TSCGranitModem::set_config_data(DWORD mask,LPVOID buf,DWORD /*bsz*/)
 {
  //Установка новых данных настроек
  bool ret = true;
  DWORD error = 0;
  lpgrsc_settings gms  = (lpgrsc_settings)buf;
  if(mask & CCFL_MODEM_NUMBER)
     {
      DWORD old_num;
      if(GKH_RET_SUCCESS == set_modem_number(gms->modem_number,&old_num))
        {
		 TLockHelper l(locker);
		 settings.modem_number = gms->modem_number;
        }
        else
        {ret = false;error = GetLastError();}
     }
  if(mask&CCFL_PUNUMBER)
    {
	 TLockHelper l(locker);
	 settings.pu_number = gms->pu_number;

    }
    if(mask & CCFL_DEVICE)
    {
	 TLockHelper l(locker);
	 safe_strcpy(settings.io_module,gms->io_module);

	}
	if(mask & CCFL_DEVICE_NAME)
	{
	 TLockHelper l(locker);
	 safe_strcpy(settings.device_name,gms->device_name);

	}
	if(mask & CCFL_DEVICE_SPEED)
	{
	 TLockHelper l(locker);
	 settings.io_speed = gms->io_speed;

	}

    if(mask&CCFL_PUNAME)
    {
	 {
	  TLockHelper l(locker);
	  safe_strcpy(settings.pu_name,gms->pu_name);
     }
	 if(is_running())
	 {
	  otd_proto_param opp;
      otd_proto       op;
      op.dw_size      = sizeof(op);
      ZeroMemory(&opp,sizeof(opp));
      opp.dw_size     = sizeof(opp);
      opp.parts       = OTD_PROTO_PART_NAME;
      opp.name_sz     = lstrlenW(gms->pu_name)+1;
      DWORD need_size = otd_proto_calc_size_ex(&opp,0);
      LPBYTE  buf     = new BYTE[need_size];
      if(buf)
        {
         otd_proto_format_ex(buf,need_size,&opp,0,&op);
         op.addr->addr = -1;
         otd_text_set(op.name,gms->pu_name);
         queue_rxdata(0,-1,0,FA_OTD,buf,op.proto_size,false);
         delete [] buf;
        }
     }
    }
    if(mask&CCFL_FILENAME)
    {
	 TLockHelper l(locker);
	 lstrcpynW(settings.save_file_name,gms->save_file_name,sizeof(settings.save_file_name)/sizeof(wchar_t));
	 cc_flags|=CCFL_FLOORS;
	}

	if(mask&CCFL_IOWATCHDOG)
	{
	 int enable_io_watchdog;
	 {
	  TLockHelper l(locker);
	  enable_io_watchdog = settings.enable_io_watchdog=gms->enable_io_watchdog;
	 }
	 io_iface.call(CMDIO_ENABLE_WATCHDOG,enable_io_watchdog,0);
	}


    cc_flags|=mask;
  if(error)
    SetLastError(error);
  return ret;
 }

 bool    __fastcall TSCGranitModem::compare_config_data(DWORD /*mask*/,LPVOID buf,DWORD /*bsz*/,LPDWORD ch_mask,BOOL * restart)
 {
  lpgrsc_settings gms  = (lpgrsc_settings)buf;
  DWORD _chmask  = 0;
  BOOL  _restart = FALSE;
  if(check_data_size(gms,sizeof(*gms)))
    {
     TLockHelper l(locker);
     if(gms->modem_number!=settings.modem_number)
      {
        _chmask  |= CCFL_MODEM_NUMBER;
        _restart = TRUE;
      }

     if(gms->pu_number !=settings.pu_number)
     {
        _chmask  |= CCFL_PUNUMBER;
        _restart = TRUE;
     }

     if( lstrcmpiW(gms->io_module,settings.io_module) )
     {
        _chmask  |= CCFL_DEVICE;
        _restart = TRUE;
     }
     if(lstrcmpiW(gms->device_name,settings.device_name) )
     {
        _chmask  |= CCFL_DEVICE_NAME;
        _restart = TRUE;
     }
     if(gms->io_speed!=settings.io_speed)
     {
        _chmask  |= CCFL_DEVICE_SPEED;
        _restart = TRUE;
     }

     if(lstrcmpW(gms->pu_name,settings.pu_name))
     {
       _chmask  |= CCFL_PUNAME;
     }

     if(lstrcmpW(gms->save_file_name,settings.save_file_name))
     {
       _chmask  |= CCFL_FILENAME;
       _restart = TRUE;
     }
     if(gms->enable_io_watchdog != settings.enable_io_watchdog)
       _chmask|= CCFL_IOWATCHDOG;
    }
  if(restart) *restart = _restart;
  if(ch_mask) *ch_mask = _chmask;
  return _chmask ? true:false;
 }

 bool    __fastcall TSCGranitModem::read_settings()
 {
  Inherit::read_settings();
  KeRTL::TRegsReader rr (DupKey(reg_key));
  settings.modem_number = get_modem_number();
  settings.pu_number = rr.ReadDword(REGDW_GRSCM_PUNUMBER_W,-1,true);
  settings.enable_io_watchdog = rr.ReadDword(REGDW_GRSC_IOWATCHDOG,1,true);
  rr.ReadString(REGSTR_GRSC_IOMODULE_W,settings.io_module,sizeof(settings.io_module)/sizeof(wchar_t),true);
  rr.ReadString(REGSTR_GRSC_DEVICENAME_W,settings.device_name,sizeof(settings.device_name)/sizeof(wchar_t),true);
  rr.ReadString(REGSTR_GRSC_PUNAME_W,settings.pu_name,sizeof(settings.pu_name)/sizeof(wchar_t),true);
  rr.ReadString(REGSTR_GRSC_FILENAME_W,settings.save_file_name,sizeof(settings.save_file_name)/sizeof(wchar_t),true);
  settings.io_speed =  rr.ReadDword(REGDW_GRSC_IOSPEED,0,true);

  if(!(*settings.io_module))
      safe_strcpy(settings.io_module,L"ft2usbio.mio");
  read_floors();
  return true;
 }

 bool    __fastcall TSCGranitModem::write_settings()
 {
    Inherit::write_settings();
    TLockHelper l(locker);
    if(cc_flags)
    {
     TRegsWriter wr(DupKey(reg_key),(wchar_t*)NULL);
     if(cc_flags&CCFL_PUNUMBER)
     {
      wr.WriteDword(REGDW_GRSCM_PUNUMBER_W,get_pu_number(),true);
      cc_flags&=~CCFL_PUNUMBER;
     }
     if(cc_flags&CCFL_DEVICE)
      {
        wr.WriteString(REGSTR_GRSC_IOMODULE_W ,settings.io_module,true);
        cc_flags&=~CCFL_DEVICE;
      }
     if(cc_flags&CCFL_DEVICE_NAME)
      {
        wr.WriteString(REGSTR_GRSC_DEVICENAME_W ,settings.device_name,true);
        cc_flags&=~CCFL_DEVICE_NAME;
      }
     if(cc_flags&CCFL_DEVICE_SPEED)
      {
        wr.WriteDword(REGDW_GRSC_IOSPEED,settings.io_speed,true);
        cc_flags&=~CCFL_DEVICE_SPEED;
      }
     if(cc_flags&CCFL_PUNAME)
     {
      wr.WriteString(REGSTR_GRSC_PUNAME_W,settings.pu_name,true);
      cc_flags&=~CCFL_PUNAME;
     }
     if(cc_flags&CCFL_FLOORS)
       write_floors();

     if(cc_flags&CCFL_FILENAME)
     {
       wr.WriteString(REGSTR_GRSC_FILENAME_W,settings.save_file_name,true);
       cc_flags&=~CCFL_FILENAME;
     }
     if(cc_flags&CCFL_IOWATCHDOG)
     {
      wr.WriteDword (REGDW_GRSC_IOWATCHDOG,settings.enable_io_watchdog,true);
      cc_flags&=~CCFL_IOWATCHDOG;
     }
    }
    return true;
 }



 DWORD   __fastcall TSCGranitModem::start(DWORD reason,LPARAM start_param)
 {
  DWORD  ret = GKH_RET_ERROR;
  reg_reports();
         if(io_iface.load(_handle,settings.io_module,NULL))
         {
          char buf[sizeof(MODULE_CONFIG_DATA)+256*sizeof(wchar_t)];
          LPMODULE_CONFIG_DATA mcd =(LPMODULE_CONFIG_DATA)buf;
          mcd->mask = CCFL_MODULE_CONFIG;
          mcd->dw_sz = sizeof(buf);
		  wchar_t * text = (wchar_t*)mcd->data;
		  {
		   TLockHelper l (locker);
		   DWORD len = wsprintfW(text,L"%s",settings.device_name);
		   if(settings.io_speed)
			  wsprintfW(text+len,L":%d",settings.io_speed);
		  }
          io_iface.set_module_config_data(mcd,mcd->dw_sz);
          io_iface.call(CMDIO_ENABLE_WATCHDOG,settings.enable_io_watchdog,0);
          io_iface.start(MODULE_START_REQUEST,0);
          ret  = Inherit::start(reason,start_param);
         }
  return ret;
 }

 DWORD   __fastcall TSCGranitModem::stop (DWORD reason)
 {
  DWORD  ret;
         {
		  io_iface.stop(MODULE_STOP_REQUEST);
		  io_iface.close();
		  ret  = Inherit::stop(reason);
		 }
  return ret;
 }

 DWORD   __fastcall    TSCGranitModem::get_pu_number()
 {
  DWORD ret;
  TLockHelper l(locker);
  ret = settings.pu_number;

  return  ret;
 }



 LRESULT __fastcall TSCGranitModem::processing(DWORD cmd,LPARAM p1,LPARAM p2)
 {
   LRESULT ret ;
   switch(cmd)
   {

    case CMDIO_READ_READY:
         ret =   sc_rx((LPMIO_REQUEST)p1,p2);
         break;
    case CMDIO_ERROR :
         ret = io_error(p1,p2);
         break;     
    case MODEMCM_REFRESH_LINE  :
         {
         sotd_addr addr((DWORD)p1);
         refresh(-1,-1,-1);ret = GKH_RET_SUCCESS;
         }
         break;
    case SCCM_REFRESH   :
         {
         sotd_addr addr((DWORD)p1);
         refresh(addr.cp,addr.fa,addr.sb);ret = GKH_RET_SUCCESS;
         }break;
    case SCCM_GET_PROPS : ret = get_props((DWORD)p1,(LPVOID )p2);     
         break;
    case SCCM_SET_PROPS : ret = set_props((DWORD)p1,(LPVOID )p2);
         break;
    case SCCM_ENUM_CP   : ret = enum_cp ((DWORD) p1,(LPGR_CP_PROPS) p2);
         break;
    case SCCM_ENUM_GRP  : ret = enum_grp((DWORD) p1,(LPGR_GRP_PROPS) p2);
         break;
    case SCCM_CREATE_FLOOR: ret = sc_create_floor((DWORD)p1,(LPGR_FLOOR_PROPS)p2);
         break;
    case SCCM_CREATE_CP   : ret = sc_create_cp   ((DWORD)p1,(LPGR_CP_PROPS)p2);
         break;
    case SCCM_CREATE_GROUP: ret = sc_create_group((DWORD)p1,(LPGR_GRP_PROPS)p2);
         break;
    case SCCM_REMOVE      : ret = sc_remove((DWORD)p1);
         break;
    case SCCM_GET_OTDDATA : ret = get_otd_data(p1,(LPSC_OTDDATA)p2);
         break;
    case SCCM_GET_DYNAMIC : ret = get_dynamic (p1,(LPSC_OTDDATA)p2);
         break;
    case SCCM_TEST_TUTR   : ret = test_tutr((LPSC_TEST_TUTR)p1);
         break;

    case SCCM_RESET_TU_ERROR: ret = reset_tu_error((DWORD)p1,(DWORD)p2);
         break;

    case SCCM_GET_TUENTRY : ret = get_tu_entry((LPGRSC_TU_ENTRY)p1);
         break;
    case SCCM_ADD_TUENTRY : ret = add_tu_entry((LPGRSC_TU_ENTRY)p1);
         break;
    case SCCM_SET_TUENTRY : ret = set_tu_entry((LPGRSC_TU_ENTRY)p1);
         break;
    case SCCM_DEL_TUENTRY : ret = del_tu_entry((LPGRSC_TU_ENTRY)p1);
         break;
    case SCCM_ENUM_TUENTRY: ret = enum_tu_entry((LPGRSC_TU_ENTRY)p1,(DWORD)p2);
         break;
    case SCCM_IMPORT_SCDB : ret = import_floors((wchar_t*) p1); break;

    default : ret = Inherit::processing(cmd,p1,p2);
         break;
   }
   return ret;
 }

void    __fastcall TSCGranitModem::refresh(DWORD cp,DWORD fa,DWORD /*group*/)
{
  if(cp<OTD_ADDR_MAXVALUE)
  {
   sc_floor * floor = this->get_floor(GRPROTO_GET_FLOOR_NUM(cp));
   if(floor)
      floor->floor_refresh(cp,fa);
  }
  else
  TModemBase::refresh_line(-1,-1);
}

DWORD   __fastcall TSCGranitModem::sc_tx (LPBYTE data,DWORD data_len)
{
 BYTE  buff[1024];
 LPMIO_REQUEST io_req = (LPMIO_REQUEST)buff;
 DWORD need_size = sizeof(*io_req)+data_len - sizeof(io_req->data[0]);
 ZeroMemory(io_req,need_size);
 io_req->dw_size   = need_size;
 io_req->data_size = io_req->data_len = data_len;
 io_req->channel   = ((data[0]>>4)&0x03);//Номер канала = 2 младших бита номера этажа
 memcpy(io_req->data,data,data_len);
 notify(MNF_SCTXRX,MNCODE_SCTX,io_req,need_size);
 if(io_iface.write(io_req))
 {
  DWORD last_error = GetLastError();
  if(last_error)
    notify(MNF_SCTXRX,MNCODE_SCTXRX_ERROR,&last_error,sizeof(last_error));
  return 0;
 }
 return data_len;

}

DWORD   __fastcall TSCGranitModem::io_error(DWORD err ,DWORD param)
{
 if(err )
 {
   if(err ==  IO_ERROR_CLOSE)
   {
   //Отправить кадр диагностики ПУ
   BYTE kadr[128];
   otd_proto op;
   op.dw_size = sizeof(op);
   otd_proto_format(kadr,sizeof(kadr),OTD_PROTO_PART_DIAG|OTD_PROTO_PART_KPK,0,0,0,0,0,0,&op);
   op.addr->addr = -1;
   op.addr->pu   = settings.pu_number;
   *op.diag      = OTD_DIAG_NODATA|OTD_DIAG_PUCONNECT|OTD_PART_DIAG_PARAM;
   otd_proto_protect(&op,op.proto_size,OTD_DEF_POLINOM);
   queue_rxdata(0,0,0,FA_OTD,kadr,op.proto_size,false);
   param = -1;
   }
 }

   TLockHelper l(locker);
   modem_line * line ;
   DWORD index = 0;

   for(line = get_line_byindex(index);line;index++)
	   {
		 if(param == (DWORD)-1 || param == (line->get_number()&0x03))
		 {
		  line->connect(err ? false:true);
		 }
		line = get_line_byindex(index);
	   }

 return GKH_RET_SUCCESS;
}

DWORD   __fastcall TSCGranitModem::sc_rx(LPMIO_REQUEST io_req,DWORD len)
{
  notify(MNF_SCTXRX,MNCODE_SCRX,io_req,len);
  gr_proto gp;  
  gp.dw_size = sizeof(gp);
  if(io_req->channel<255)
  {
  DWORD proto_error = gr_proto_parser((LPBYTE)io_req->data,io_req->data_len,&gp);
  log_rx_error(io_req->channel,gp,proto_error);
  if(!proto_error || proto_error == (DWORD)GRPP_ERR_INVPART)
  {
    DWORD floor_number = gp.cp->floor;
    //lock(); //was comment
    sc_floor * floor = dynamic_cast<sc_floor*>(get_line(floor_number));
    if(floor)
       floor->floor_rx((LPBYTE)io_req->data,io_req->data_len);
            //Передача на обработку КП
    //unlock();//was comment

  }
  }
  return GKH_RET_SUCCESS;
}

    void      __fastcall  TSCGranitModem::log_rx_error(DWORD channel,gr_proto & gp,DWORD proto_error)
    {
      int error = KeRTL::MIN(std::abs((long)proto_error),(long)GRPP_MAX_ERROR);
      wchar_t buff[MAX_PATH];
      int len = this->get_lang_string(IDS_SC_CHANNEL,buff,sizeof(buff)/sizeof(wchar_t));
      len += wsprintfW(buff+len,L" %d :",channel);
      if(error)
      {
       ERROR_TEXT et;
       et.dw_size    = sizeof(et);
       et.error_text = buff+len;
       et.error_text_size = sizeof(buff)/sizeof(wchar_t)-len;
       et.args = NULL;
       this->get_error_text(GRSCERR_GRPROTO_SIZE+error-1,&et);
       event_report(REPORT_ERROR_TYPE,buff,gp.cp,gp.kadr_len);
      }
    }

 void    __fastcall TSCGranitModem::report_modem(DWORD rep_id,int rep_type,wchar_t * text,LPVOID data ,DWORD data_sz )
 {
   char  mes_buf[2048];
   TReportMes  * mes = (TReportMes*)mes_buf;
   wchar_t mes_text[4096];
   mes->desc = mes_text;
   mes->data_size = KeRTL::MIN((DWORD)sizeof(mes_buf)-sizeof(*mes),data_sz);
   {
    TLockHelper l(locker);
	swprintf(mes_text,L"%s : %s",settings.pu_name,text);
   }
   mes->log    = rep_id;
   mes->type   = rep_type;
   mes->source = _handle;
   if(data && mes->data_size)
      memcpy(mes->data,data,mes->data_size);
   LRESULT ret;
   call_gkhandle_bynameW(REPORTER_NAME_W,RCM_MESSAGE_EVENT,(LPARAM)mes,0,&ret);
 }

  sc_floor * __fastcall TSCGranitModem::get_floor(DWORD num)
  {
   sc_floor * ret = NULL;
   if(num<get_lines_limit())
     ret = dynamic_cast<sc_floor*>(get_line(num)) ;
    if(!ret)
      SetLastError(GRSCERR_OBJECT_NOT_EXIST);
   return ret;
  }

  LRESULT __fastcall TSCGranitModem::get_props(DWORD addr,LPVOID ptr)
  {
   LRESULT ret = GKH_RET_ERROR;
   sotd_addr sa(addr);
   if(sa.addrtype() != OTD_ADDR_TYPE_BROADCAST)
    {
     sc_floor * floor = get_floor(GRPROTO_GET_FLOOR_NUM(sa.cp));
     if(floor)
        ret = floor->get_props(addr,ptr);
    }
   return  ret;
  }

  LRESULT __fastcall TSCGranitModem::set_props(DWORD addr,LPVOID ptr)
  {
   LRESULT ret = GKH_RET_ERROR;
   sotd_addr sa(addr);
   if(sa.addrtype() != OTD_ADDR_TYPE_BROADCAST)
    {
     sc_floor * floor = get_floor(GRPROTO_GET_FLOOR_NUM(sa.cp));
     if(floor)
        {
         ret = floor->set_props(addr,ptr);
         if(sa.addrtype() == OTD_ADDR_TYPE_CP && !GRPROTO_GET_LU_NUM(sa.cp))
         {
          LPGR_FLOOR_PROPS  prp = (LPGR_FLOOR_PROPS )ptr;
          if(ptr && check_data_size(prp,sizeof(*prp)) && (prp->mask&GRSC_FPRP_ADDR) )
          {

           DWORD floor_num = GRPROTO_GET_FLOOR_NUM(prp->addr.cp);
           if(this->renumber_line(floor->get_number(),floor_num))
              config_change(floor_num);
          }
         }
        }
    }

   return  ret;
  }

  LRESULT __fastcall TSCGranitModem::enum_cp (DWORD index,LPGR_CP_PROPS  cpp)
  {
   LRESULT ret = GKH_RET_ERROR;
   if(check_data_size(cpp,sizeof(cpp)))
   {
   DWORD fn    = GRPROTO_GET_FLOOR_NUM(cpp->addr.cp);
    sc_floor * floor = get_floor(fn);
    if(floor)
       ret = floor->enum_cp(index,cpp);
   }
   return  ret;
  }

  LRESULT __fastcall TSCGranitModem::enum_grp(DWORD index,LPGR_GRP_PROPS grp)
  {
   LRESULT ret = GKH_RET_ERROR;
   if(check_data_size(grp,sizeof(*grp)))
   {
   DWORD fn    = GRPROTO_GET_FLOOR_NUM(grp->addr.cp);
    sc_floor * floor = get_floor(fn);
    if(floor)
       {
        ret = floor->enum_grp(index,grp);
       }
   }
   return  ret;
  }

 LRESULT __fastcall TSCGranitModem::sc_remove(DWORD addr)
 {
  LRESULT ret = GKH_RET_ERROR;
  sotd_addr sa(addr);
  sc_floor * floor = this->get_floor(GRPROTO_GET_FLOOR_NUM(sa.cp));
  if(floor)
  {
    if(GRPROTO_GET_LU_NUM(sa.cp))
        ret = floor->remove(addr);
       else
       {
        floor->stop();
        config_change(floor->get_number());
        remove_line(floor->get_number());
        ret = GKH_RET_SUCCESS;
       }
  }
  return ret;
 }

 LRESULT __fastcall TSCGranitModem::sc_create_floor(DWORD addr ,LPGR_FLOOR_PROPS  fpr)
 {
  DWORD ret   = GKH_RET_ERROR;
  DWORD error = 0;
  if(check_data_size(fpr,sizeof(*fpr)))
  {
  if(this->get_lines_count()< this->get_lines_limit())
  {
  DWORD floor_num = GRPROTO_GET_FLOOR_NUM(sotd_addr(addr).cp);
  if(floor_num<128)
  {
   //TLockHelper l(locker);
   //TODO проверить пересечения этажей по каналам
  sc_floor * floor = get_floor(floor_num);
  if(!floor)
   {
    floor = new sc_floor(floor_num);
    if(floor)
    {
     floor->set_props(addr,fpr);
     add_line(floor);
     if(is_running())
        floor->start();
     ret = GKH_RET_SUCCESS;
    }
   }
    else
    error = GRSCERR_OBJECT_ALREADY_EXIST;
  }
  else
   error =  GRSCERR_FLOOR_NUMBER;
  }
  else
   error = GRSCERR_FLOOR_COUNT_EXCEED;
  }
   if(error)
     SetLastError(error);
  return ret;
 }

 LRESULT __fastcall TSCGranitModem::sc_create_cp   (DWORD addr,LPGR_CP_PROPS     cpp)
 {

  DWORD ret   = GKH_RET_ERROR;
  DWORD error = 0;
  if(check_data_size(cpp,sizeof(*cpp)))
  {
  DWORD floor_num = GRPROTO_GET_FLOOR_NUM(sotd_addr(addr).cp);
  sc_floor * floor = get_floor((floor_num));
  if(floor)
    ret = floor->create_cp(sotd_addr(addr).cp,cpp);
  }
   if(error)
     SetLastError(error);
  return ret;
 }

 LRESULT __fastcall TSCGranitModem::sc_create_group(DWORD addr,LPGR_GRP_PROPS    grp)
 {
  DWORD  ret = GKH_RET_ERROR;
  DWORD error = 0;
  if(check_data_size(grp,sizeof(*grp)))
  {
  DWORD floor_num = GRPROTO_GET_FLOOR_NUM(sotd_addr(addr).cp);
  sc_floor * floor = get_floor(floor_num);
  if(floor)
    ret = floor->create_group(sotd_addr(addr).cp,grp);
  }
   if(error)
     SetLastError(error);

  return ret;
 }

 LRESULT __fastcall TSCGranitModem::get_dynamic (DWORD addr,LPVOID dyn_data)
 {
  LRESULT ret      = GKH_RET_ERROR;
  DWORD floor_num  = GRPROTO_GET_FLOOR_NUM(sotd_addr(addr).cp);
  sc_floor * floor = get_floor(floor_num);
  if(floor)
    ret = floor->get_dynamic(addr,dyn_data);
  return ret;
 }

 LRESULT __fastcall TSCGranitModem::get_otd_data(DWORD addr,LPSC_OTDDATA scod)
 {
  LRESULT ret = GKH_RET_ERROR;
  DWORD error = 0;
  if(check_data_size(scod,sizeof(*scod)))
  {
  DWORD floor_num = GRPROTO_GET_FLOOR_NUM(sotd_addr(addr).cp);
  sc_floor * floor = get_floor(floor_num);
  if(floor)
    ret = floor->get_otd_data(addr,scod);
  }
   if(error)
     SetLastError(error);

  return ret;
 }

 LRESULT __fastcall TSCGranitModem::get_tu_entry(LPGRSC_TU_ENTRY tue)
 {
  LRESULT ret = GKH_RET_ERROR;
  DWORD error = 0;
  if(check_data_size(tue,sizeof(*tue)))
  {
    DWORD floor_num = GRPROTO_GET_FLOOR_NUM(tue->addr.cp);
    sc_floor * floor = get_floor(floor_num);
    if(floor)
      {
       ret = floor->get_tu_entry(tue);
      }
  }
   if(error)
     SetLastError(error);
  return ret;
 }

 LRESULT __fastcall TSCGranitModem::add_tu_entry(LPGRSC_TU_ENTRY tue)
 {
  LRESULT ret = GKH_RET_ERROR;
  DWORD error = 0;
  if(check_data_size(tue,sizeof(*tue)))
  {
    DWORD floor_num = GRPROTO_GET_FLOOR_NUM(tue->addr.cp);
    sc_floor * floor = get_floor(floor_num);
    if(floor)
      {
       ret = floor->add_tu_entry(tue);
      }
  }
   if(error)
     SetLastError(error);
  return ret;
 }

 LRESULT __fastcall TSCGranitModem::set_tu_entry(LPGRSC_TU_ENTRY tue)
 {
  LRESULT ret = GKH_RET_ERROR;
  DWORD error = 0;
  if(check_data_size(tue,sizeof(*tue)))
  {
    DWORD floor_num = GRPROTO_GET_FLOOR_NUM(tue->addr.cp);
    sc_floor * floor = get_floor(floor_num);
    if(floor)
      {
       ret = floor->set_tu_entry(tue);
      }
  }
   if(error)
     SetLastError(error);
  return ret;
 }

 LRESULT __fastcall TSCGranitModem::del_tu_entry(LPGRSC_TU_ENTRY tue)
 {
  LRESULT ret = GKH_RET_ERROR;
  DWORD error = 0;
  if(check_data_size(tue,sizeof(*tue)))
  {
    DWORD floor_num = GRPROTO_GET_FLOOR_NUM(tue->addr.cp);
    sc_floor * floor = get_floor(floor_num);
    if(floor)
      {
       ret = floor->del_tu_entry(tue);
      }
  }
   if(error)
     SetLastError(error);
  return ret;
 }

 LRESULT __fastcall TSCGranitModem::enum_tu_entry(LPGRSC_TU_ENTRY tue,DWORD idx)
 {
  LRESULT ret = GKH_RET_ERROR;
  DWORD error = 0;
  if(check_data_size(tue,sizeof(*tue)))
  {
    DWORD floor_num = GRPROTO_GET_FLOOR_NUM(tue->addr.cp);
    sc_floor * floor = get_floor(floor_num);
    if(floor)
      {
       ret = floor->enum_tu_entry(tue,idx);
      }
  }
   if(error)
     SetLastError(error);
  return ret;
 }


/*
//Старый вариант дублирование вызовов по линиям
LRESULT    __fastcall TSCGranitModem::send        (LPMPROTO_HEADER mph,DWORD sz)
 {
  if(mph && mph->fa == FA_OTD && mph->data_size)
  {
    sotd_addr sa(*(LPDWORD)mph->data);
    DWORD  floor_num =  GRPROTO_GET_FLOOR_NUM(sa.cp);
    if(sa.pu == OTD_ADDR_MAXVALUE || (DWORD)sa.pu == this->get_pu_number())
    {
    //Проверка номера ПУ или для всех ПУ
    if(sa.cp == OTD_ADDR_MAXVALUE)
      mph->addr_to.line = MODEM_ADDR_MAXVALUE;
    else
      {
       if(mph->addr_to.line != floor_num)
         return GKH_RET_SUCCESS;
      }
      return Inherit::send(mph,sz);
    }
  }
  return GKH_RET_SUCCESS;
 }
*/


 LRESULT    __fastcall TSCGranitModem::send        (LPMPROTO_HEADER mph,DWORD sz)
 {
  if(mph && mph->fa == FA_OTD && mph->data_size>=(sizeof(otd_addr)+sizeof(WORD)))
  {
    sotd_addr sa(*(LPDWORD)mph->data);
    /*Проверка номера ПУ или для всех ПУ*/
    if(sa.pu == OTD_ADDR_MAXVALUE || (DWORD)sa.pu == this->get_pu_number())
    {
      if(OTD_ADDR_ISQUERY(&sa))
      {
       DWORD parts;
       if(sa.cp == OTD_ADDR_MAXVALUE && otd_proto_get_verpart((LPBYTE)mph->data,mph->data_size,NULL,&parts) && (parts&OTD_PROTO_PART_NAME))
       {
         BYTE buffer[MAX_PATH];
         otd_proto op={0};
		 op.dw_size = sizeof(op);
		 {
			 TLockHelper l(locker);
			 otd_proto_format(buffer,sizeof(buffer),OTD_PROTO_PART_NAME,0,0,0,0,0,lstrlenW(settings.pu_name),&op);
			 otd_text_set(op.name,settings.pu_name);
			 op.addr->addr = -1;
			 op.addr->pu   = settings.pu_number;
		 }
         queue_rxdata(-1,0,FA_OTD,buffer,op.proto_size,false);
       }
      }
      
      DWORD  floor_num =  GRPROTO_GET_FLOOR_NUM(sa.cp);
      if(floor_num == (DWORD)mph->addr_to.line || sa.cp==OTD_ADDR_MAXVALUE)
         return Inherit::send(mph,sz);
    }
  }
  return GKH_RET_SUCCESS;
 }




 LRESULT      __fastcall TSCGranitModem::test_tutr   (LPSC_TEST_TUTR tt)
 {
  LRESULT ret = GKH_RET_ERROR;
  if(check_data_size(tt,sizeof(*tt)))
  {
  DWORD floor_num = GRPROTO_GET_FLOOR_NUM(tt->addr.cp);
  sc_floor * floor = get_floor(floor_num);
  if(floor)
    ret = floor->test_tutr(tt);
  }
  return ret;
 }

LRESULT __fastcall TSCGranitModem::reset_tu_error(DWORD addr,DWORD obj)
{

  LRESULT ret = GKH_RET_ERROR;
  otd_addr  sa;
  sa.addr = addr;
  DWORD floor_num = GRPROTO_GET_FLOOR_NUM(sa.cp);
  sc_floor * floor = get_floor(floor_num);
  
  if(floor)
    ret = floor->reset_tu_error(sa,obj);
  return ret;
}





 void       __fastcall TSCGranitModem::write_floors()
 {
   /*Запись этажей*/
   if(!(*settings.save_file_name))
    {
     settings.save_file_name;
     check_file_exist cfe;
     get_uni_filename((wchar_t*)SC_FILENAME_TEMPL,settings.save_file_name,(int)(sizeof(settings.save_file_name)/sizeof(wchar_t)),cfe);
     cc_flags |= CCFL_FILENAME; 
    }

   wchar_t * name_ptr = KeRTL::GetFileNameFromPath(settings.save_file_name);
   if(name_ptr)
   {
    wchar_t save_char = *name_ptr;
    *name_ptr = 0;
    KeRTL::ForceDirs(settings.save_file_name);
    *name_ptr = save_char;
   }

   KeRTL::TFileStream  fs(settings.save_file_name);
   fs.Rewind();
   fs.Write(sig,lstrlenA(sig));
   for(DWORD i = 0;i<get_lines_limit();i++)
   {
     sc_floor * floor = get_floor(i);
     if(floor)
     {
      char floor_sep[32];
      int len = wsprintfA(floor_sep,"[ Floor %03d ]",floor->get_number());
      fs<<len;
      fs.Write(floor_sep,len);
      fs<<floor->get_stream_ver();
      floor->write(fs);
     }
    }
    fs.SetSize(fs.GetPosition());
   cc_flags&=~CCFL_FLOORS;

 }

 DWORD      __fastcall TSCGranitModem::import_floors(wchar_t * fname)
 {
  DWORD ret = GKH_RET_ERROR;
  bool running = this->is_running();
  if(fname && * fname)
  {
  if(running)
     processing(MCMD_STOP,MODULE_STOP_RECONFIGURE,0);
   {
	TLockHelper l(locker);
	ret = do_import_floors(fname);
	if(ret == GKH_RET_SUCCESS)
	{
	 cc_flags|=CCFL_FLOORS;
	 write_floors();
	}
   }

  if(running)
     processing(MCMD_START,MODULE_START_RECONFIGURE,start_param);
  }
  else
  SetLastError(ERROR_FILE_NOT_FOUND);
  return ret;
 }

 DWORD      __fastcall TSCGranitModem::do_import_floors(wchar_t * fname)
 {
   DWORD ret = GKH_RET_ERROR;
   if(fname && * fname)
   {
   clear_lines();
   KeRTL::TFileStream  fs(fname,true);
   if(fs.IsGood())
   {
   fs.Rewind();
   char check_sig[sizeof(sig)];
   int len = lstrlenA(sig);
   fs.Read(check_sig,len);
   if(memcmp(check_sig,sig,len)==0)
   {
   for(DWORD i = 0;!fs.Eos() && i<get_lines_limit();i++)
   {
       char floor_sep[32];
       int len;
       fs>>len;
       if(len==0x0D)
       {
       fs.Read(floor_sep,len);
       sc_floor * floor = new sc_floor(0);
       DWORD ver ;
       fs>>ver;
       floor->read(fs,ver);
       add_line   (floor);
       }
       else
       fs.Seek(0,TStream::smFromEnd);
    }
    ret = GKH_RET_SUCCESS;
   }
  }
  }
  return ret;
 }

 void       __fastcall TSCGranitModem::read_floors()
 {
   do_import_floors(settings.save_file_name);
 }



















