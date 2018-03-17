#include "grantablet.hpp"
#include <gkio_base.h>


static THeap heap(TSysInfo().GetPageSize()<<8); // 512 Кб
static DWORD total_mem_alloc = 0;
void * operator new(size_t sz)
{

 void * ptr = heap.Alloc(sz);
 total_mem_alloc+= heap.MemSize(ptr,0);
 return ptr;
}

void * operator new [](size_t sz)
{
 void * ptr = heap.Alloc(sz);
 total_mem_alloc+= heap.MemSize(ptr,0);
 return ptr;
}

void operator delete( void * ptr)
{
  if(ptr && heap.Validate(ptr))
    {
     total_mem_alloc-= heap.MemSize(ptr,0);
     heap.Free(ptr);
    }
}

void operator delete[]( void * ptr)
{
  if(ptr && heap.Validate(ptr))
    {
     total_mem_alloc-= heap.MemSize(ptr,0);
     heap.Free(ptr);
    }
}

DWORD   __fastcall TGranTablet::get_mem_used()
{
  return
  total_mem_alloc;
}



LRESULT WINAPI module_main(DWORD cmd,LPARAM p1,LPARAM p2)
{
       LRESULT ret = NULL;
       switch(cmd)
       {
        case GKME_CMD_GET_MODULETYPE: ret = MT_MODEM;break;
        case GKME_CMD_CREATE_HANDLE :
        {
           {
            TGranTablet * mod = new TGranTablet;
            ret  = (LRESULT)(*(TGKHandleBased*)mod)();
            if(!ret)
              delete mod;
           }
        }
        break;
       }

       return ret;

}

  TGranTablet::TGranTablet ()
  {
    lock_param = GKHB_AUTO_LOCK_OFF;
    this->alloc_gkhandle((wchar_t*)NULL);
    ZeroMemory(&settings,sizeof(settings));
    settings.dw_size = sizeof(settings);

  }

 wchar_t   wnd_mod_name[] = L"grantabletwnd.dll";
 DWORD         __fastcall TGranTablet::get_window_module  (wchar_t * buf,DWORD bsz)
 {
  DWORD ret = KeRTL::MIN(sizeof(wnd_mod_name),(size_t)bsz);
  safe_strcpyn(buf,wnd_mod_name,ret);
  return ret;

 }

 DWORD         __fastcall TGranTablet::get_config_data(DWORD mask,LPVOID buf,DWORD bsz)
 {
   DWORD ret = GKH_RET_ERROR;
   if(check_data_size(buf,sizeof(settings)))
     {
      lock();
       ret = GKH_RET_SUCCESS;
       memcpy(buf,&settings,sizeof(settings));
      unlock();
     }
   return ret;  
 }

 bool          __fastcall TGranTablet::set_config_data(DWORD mask,LPVOID buf,DWORD bsz)
 {
  bool ret = true;
  LPGRTBL_SETTINGS p_settings = (LPGRTBL_SETTINGS)buf;
  if(check_data_size(buf,sizeof(settings)))
  {
    if(mask&CCFL_MODEM_NUMBER)
        {
         DWORD old_mn;
         if(set_modem_number(p_settings->modem_number,&old_mn)==GKH_RET_SUCCESS)
           settings.modem_number = p_settings->modem_number;
           else
           ret = false;
        } 
    lock();
    if(mask&CCFL_IOMODULE)
     safe_strcpy(settings.io_module,p_settings->io_module);
    if(mask&CCFL_IODEVICE)
     safe_strcpy(settings.io_device,p_settings->io_device);
    if(mask&CCFL_DBFILE)
     safe_strcpy(settings.db_file,p_settings->db_file);
    if(mask & CCFL_IOSPEED)
      settings.io_speed = p_settings->io_speed;
    if(mask & CCFL_IOWATCHDOG)
      settings.io_watchdog = p_settings->io_watchdog;
    if(mask & CCFL_IONOTRESET)
      settings.io_not_reset = p_settings->io_not_reset;
   unlock();
  }
  else
  ret = false;  
  return ret;
 }

 bool          __fastcall TGranTablet::compare_config_data(DWORD mask,LPVOID buf,DWORD bsz,LPDWORD ch_mask,BOOL * restart)
 {
  bool ret = true;
  LPGRTBL_SETTINGS p_settings = (LPGRTBL_SETTINGS)buf;
  DWORD cmp_mask = 0;
  BOOL  _rst = FALSE;
  if(check_data_size(p_settings,sizeof(*p_settings)))
  {
    lock();
    if(settings.modem_number!=p_settings->modem_number)
      cmp_mask|=CCFL_MODEM_NUMBER,_rst = TRUE;
    if(lstrcmpiW(settings.io_module,p_settings->io_module))
      cmp_mask|=CCFL_IOMODULE,_rst = TRUE;
    if(lstrcmpiW(settings.io_device,p_settings->io_device))
      cmp_mask|=CCFL_IODEVICE,_rst = TRUE;
    if(settings.io_speed!= p_settings->io_speed)
       cmp_mask|=CCFL_IOSPEED,_rst = TRUE;
    if(lstrcmpiW(settings.db_file,p_settings->db_file))
      cmp_mask|=CCFL_DBFILE;
    if(settings.io_watchdog!=p_settings->io_watchdog)
      cmp_mask|= CCFL_IOWATCHDOG,_rst = true;
    if(settings.io_not_reset!=p_settings->io_not_reset)
      cmp_mask|= CCFL_IONOTRESET,_rst = true;
    unlock();

  }
  else
  ret = false;
  if(ch_mask)
     * ch_mask = cmp_mask;
  if(restart)
    *restart = _rst;
  return ret;
 }

 bool          __fastcall TGranTablet::check_config_data  (DWORD mask,LPVOID buf,DWORD bsz)
 {
  return TModemBase::check_config_data(mask,buf,bsz);
 }

 bool          __fastcall TGranTablet::read_settings  ()
 {
  bool ret = TModemBase::read_settings();
  TRegsReader rr(DupKey(reg_key));
  settings.modem_number = this->get_modem_number();
  lock();
  if(!rr.ReadString(GRTBL_REGSTR_IOMODULE_W,settings.io_module,sizeof(settings.io_module)/sizeof(wchar_t),true))
     lstrcpyW(settings.io_module,L"ft2usbio.mio");
  settings.io_device[0] = 0;
  rr.ReadString(GRTBL_REGSTR_IODEVICE_W,settings.io_device,sizeof(settings.io_device)/sizeof(wchar_t),true);
  settings.io_speed  = rr.ReadDword(GRTBL_REGDW_IOSPEED_W,0,true);
  settings.db_file[0] = 0;
  rr.ReadString(GRTBL_REGSTR_DBFILENAME_W,settings.db_file,sizeof(settings.db_file)/sizeof(wchar_t),true);
  settings.io_watchdog  = rr.ReadDword(GRTBL_REGDW_IOWATCHDOG_W,1,true);
  settings.io_not_reset = rr.ReadDword(GRTBL_REGDW_IONOTRESET_W,1,true);
  unlock();
  //add_line(new TOutputFloor(11));
  read_floors(rr);
  return ret;
 }

 bool          __fastcall TGranTablet::write_settings ()
 {
  bool ret = TModemBase::write_settings();
  TRegsWriter rw(DupKey(reg_key));
  lock();
  if(cc_flags & CCFL_IOMODULE)
     rw.WriteString(GRTBL_REGSTR_IOMODULE_W,settings.io_module,true),cc_flags &= ~CCFL_IOMODULE;
  if(cc_flags & CCFL_IODEVICE)
     rw.WriteString(GRTBL_REGSTR_IODEVICE_W,settings.io_device,true),cc_flags &= ~CCFL_IODEVICE;
  if(cc_flags & CCFL_IOSPEED)
     rw.WriteDword(GRTBL_REGDW_IOSPEED_W,settings.io_speed,true),cc_flags&=~CCFL_IOSPEED;
  if(cc_flags & CCFL_DBFILE )
     rw.WriteString(GRTBL_REGSTR_DBFILENAME_W,settings.db_file,true),cc_flags &= ~CCFL_DBFILE;
  if(cc_flags & CCFL_IOWATCHDOG)
    rw.WriteDword(GRTBL_REGDW_IOWATCHDOG_W,settings.io_watchdog,true),cc_flags&=~CCFL_IOWATCHDOG;
  if(cc_flags & CCFL_IONOTRESET)
    rw.WriteDword(GRTBL_REGDW_IONOTRESET_W,settings.io_not_reset,true),cc_flags&=~CCFL_IONOTRESET;
  if(cc_flags & CCFL_OUT_FLOORS)
     write_floors(rw);
  unlock();
  return ret;
 }

 void __fastcall TGranTablet::write_floors(TRegsWriter& rw)
 {
  //Запись
  TMemoryStream strm;
  DWORD floor_count = this->get_lines_count();
  strm<<floor_count;
  for(DWORD i = 0;i<floor_count;++i)
  {
   TOutputFloor * floor = dynamic_cast<TOutputFloor *>(get_line_byindex(i));
   strm<< floor->get_number();
   floor->stream_write(strm);
  }
  rw.WriteBytes(GRTBL_REGDATA_FLOORS_W,strm.GetMemory(),strm.GetSize(),true);
 }

 void          __fastcall TGranTablet::read_floors (TRegsReader& rd)
 {
  DWORD type = 0,size = 0;
  rd.GetValueType(GRTBL_REGDATA_FLOORS_W,&type,&size,true);
  if(size)
  {
     TMemoryStream  strm(size);
     strm.Reserve(size);
     strm.SetSize(size);
     rd.ReadBytes(GRTBL_REGDATA_FLOORS_W,strm.GetMemory(),strm.GetSize(),true);
     strm.Rewind();

     DWORD floor_count;
     strm>>floor_count;
      for(DWORD i = 0;i<floor_count;++i)
      {
        DWORD floor_number;
        strm>>floor_number;
        TOutputFloor * floor = new TOutputFloor(floor_number);
        floor->stream_read(strm);
        add_line(floor);
      }

  }
 }



 DWORD         __fastcall TGranTablet::start(DWORD reason,LPARAM p2)
 {
  DWORD ret = GKH_RET_ERROR;
         if(io_iface.load(_handle,settings.io_module,NULL))
         {
          char buf[sizeof(MODULE_CONFIG_DATA)+256*sizeof(wchar_t)];
          LPMODULE_CONFIG_DATA mcd =(LPMODULE_CONFIG_DATA)buf;
          mcd->mask = CCFL_MODULE_CONFIG;
          mcd->dw_sz = sizeof(buf);
          wchar_t * text = (wchar_t*)mcd->data;
          DWORD len = wsprintfW(text,L"%s",settings.io_device);
          if(settings.io_speed)
             wsprintfW(text+len,L":%d",settings.io_speed);
          io_iface.set_module_config_data(mcd,mcd->dw_sz);
          io_iface.call(CMDIO_ENABLE_WATCHDOG,settings.io_watchdog,0);
          io_iface.call(CMDIO_ENABLE_RESET,settings.io_not_reset? FALSE:TRUE,0);
          io_iface.start(MODULE_START_REQUEST,0);
          ret = TModemBase::start(reason,p2);
         }

  return ret;
 }

 DWORD         __fastcall TGranTablet::stop (DWORD reason)
 {
  DWORD ret = GKH_RET_ERROR;
  io_iface.stop(reason);
  io_iface.close();
  ret   =  TModemBase::stop(reason);
  return ret;
 }

  void        __fastcall    TGranTablet::free_line     (modem_line * line)
  {
   delete line;
  }

  int         __fastcall    TGranTablet::convert_rx_data(LPWORD fa,LPBYTE in,int in_len,LPBYTE out,DWORD out_sz)
  {
    if(*fa!=(WORD)-1)
    {
      memcpy(out,in,in_len);
      return in_len;
    }
    return 0;
  }

  LRESULT     __fastcall    TGranTablet::send (LPMPROTO_HEADER mph,DWORD sz)
  {
   if(mph->fa == FA_OTD_MEDIUM)
   {
    LPOTDM_PROTO_HEADER omph = (LPOTDM_PROTO_HEADER)mph->data;
    if((omph->command&OTDM_RESPOND) && OTDMPROTO_CMD_ENUM_KADRS==(omph->command&OTDM_COMMAND_MASK))
    {
      if(omph->data_size)
        notify(GRTBL_NM_KADRDATA,GRTBL_NC_KADRDATA,omph->data,omph->data_size);
      if(omph->command&OTDM_ENDDATA)
        notify(GRTBL_NM_KADRDATA,GRTBL_NC_KADRDATA_END,0,0);
    }
   }

   return TModemBase::send(mph,sz);
  }

  LRESULT       __fastcall TGranTablet::processing(DWORD cmd,LPARAM p1,LPARAM p2)
  {
     LRESULT ret ;
     switch(cmd)
     {
      case CMDIO_READ_READY:
           ret =   this->floor_rx((LPMIO_REQUEST)p1,p2);
           break;
      case CMDIO_ERROR :
           ret = io_error(p1,p2);
           break;
      case GRTBL_CMD_ENUM_OUTPUT_KADR      :  ret = enum_output_kadrs((LPOUTPUT_KADR_PARAM) p1,p2);break;
      case GRTBL_CMD_GET_OUTPUT_KADR_PARAM :  ret = get_output_kadr_param((LPOUTPUT_KADR_PARAM) p1);break;
      case GRTBL_CMD_ENUM_DBKADRS          :  ret = start_enum_dbkadrs((GKHANDLE)p1);break;
      case GRTBL_CMD_CREATE_FLOOR          :  ret = create_floor(p1);break;
      case GRTBL_CMD_SET_FLOOR_NUMBER      :  ret = this->set_floor_number(p1,p2);break;
      case GRTBL_CMD_DELETE_FLOOR          :  ret = delete_floor(p1);break;
      case GRTBL_CMD_CREATE_FLOOR_OUTPUT   :  ret = create_output_kadr((LPOUTPUT_KADR_PARAM)p1);break;
      case GRTBL_CMD_SET_FLOOR_OUTPUT_PARAM:  ret = modify_output_kadr((LPOUTPUT_KADR_PARAM)p1,p2);break;
      case GRTBL_CMD_DELETE_FLOOR_OUTPUT   :  ret = this->delete_output_kadr((LPOUTPUT_KADR_PARAM)p1);break;


      default : ret = TModemBase::processing(cmd,p1,p2);break;
     }
    return ret;
  }

LRESULT       __fastcall TGranTablet::create_floor(DWORD num)
{
 LRESULT ret = GKH_RET_ERROR;
 lock();
  if(NULL==get_line(num))
   {
    TOutputFloor *floor = new TOutputFloor(num);
    if(floor)
    {
     cc_flags |= CCFL_OUT_FLOORS;
     add_line(floor);
     ret = GKH_RET_SUCCESS;
     if(is_running())
     {
        
        floor->start();
     }   
    }
   }
   else
   SetLastError(0/*ALREADY_EXIST*/);
 unlock();
 return ret;

}

LRESULT       __fastcall TGranTablet::delete_floor(DWORD num)
{
 LRESULT ret = GKH_RET_ERROR;
 lock();
 TOutputFloor * floor = dynamic_cast<TOutputFloor *>(get_line(num));
 if(floor)
  {
   floor->stop();
   remove_line(num);
   ret = GKH_RET_SUCCESS;
   cc_flags |= CCFL_OUT_FLOORS;
  }
  else
  SetLastError(MDM_ERR_NO_MODEMLINE);
 unlock();
 return ret;
}

LRESULT       __fastcall TGranTablet::set_floor_number(DWORD old_num,DWORD new_num)
{
   LRESULT ret = GKH_RET_ERROR;
   if(renumber_line(old_num,new_num) )
     {
      lock();
      cc_flags |= CCFL_OUT_FLOORS;
      unlock();
      ret = GKH_RET_SUCCESS;
     }
 return ret;     
}


LRESULT       __fastcall TGranTablet::start_enum_dbkadrs(GKHANDLE dest)
{
 set_notify_mask(dest,GRTBL_NM_KADRDATA);
 BYTE buffer[MAX_PATH];
 LPMPROTO_HEADER mph = (LPMPROTO_HEADER)buffer;
 LPOTDM_PROTO_HEADER omph;
 omph = (LPOTDM_PROTO_HEADER)mph->data;
 ZeroMemory(buffer,sizeof(*omph)+sizeof(*mph));
 omph->command = OTDMPROTO_CMD_ENUM_KADRS;
 omph->data_size = sizeof(DWORD);
 omph->data[0] = MDB_KADR_FIELD_NAME;
 mph->data_size = sizeof(*omph);
 mph->fa        = FA_OTD_MEDIUM;
 mph->addr_from.modem = this->get_modem_number();
 mph->addr_from.line  = mph->addr_from.reserv1 = -1;
 mph->flags = MPROTO_FLAG_SEND_BROADCAST;         
 queue_rxdata(mph);

 return GKH_RET_SUCCESS;
}

DWORD   __fastcall TGranTablet::floor_tx (LPBYTE data,DWORD data_len)
{
 BYTE  buff[1024];
 LPMIO_REQUEST io_req = (LPMIO_REQUEST)buff;
 DWORD need_size = sizeof(*io_req)+data_len - sizeof(io_req->data[0]);
 ZeroMemory(io_req,need_size);
 io_req->dw_size   = need_size;
 io_req->data_size = io_req->data_len = data_len;
 io_req->channel   = ((data[0]>>4)&0x03);//Номер канала = 2 младших бита номера этажа
 memcpy(io_req->data,data,data_len);

 notify(GRTBL_NM_TXRX,GRTBL_NC_TX,io_req,need_size);
 if(io_iface.write(io_req))
 {
  DWORD last_error = GetLastError();
  last_error = last_error;
  //if(last_error)
    //notify(MNF_SCTXRX,MNCODE_SCTXRX_ERROR,&last_error,sizeof(last_error));
  return 0;
 }
 return data_len;

}

DWORD   __fastcall TGranTablet::io_error(DWORD err ,DWORD param)
{
 lock();
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
   unlock();
 return GKH_RET_SUCCESS;
}

DWORD   __fastcall TGranTablet::floor_rx(LPMIO_REQUEST io_req,DWORD len)
{
  notify(GRTBL_NM_TXRX,GRTBL_NC_RX,io_req,len);
  if(io_req->channel<255)
  {
  }
  return GKH_RET_SUCCESS;
}

 LRESULT       __fastcall TGranTablet::enum_output_kadrs( LPOUTPUT_KADR_PARAM okp,DWORD index)
 {
  LRESULT ret = GKH_RET_ERROR;
  if(check_data_size(okp,sizeof(*okp)))
  {
   lock();

   TOutputFloor * floor = dynamic_cast<TOutputFloor *>(get_line(okp->floor_number));
   if(floor)
      ret = floor->enum_output_kadrs(okp,index);
   unlock();
  }
  return ret;
 }

 LRESULT       __fastcall TGranTablet::get_output_kadr_param(LPOUTPUT_KADR_PARAM  okp)
 {
  LRESULT ret = GKH_RET_ERROR;
  if(check_data_size(okp,sizeof(*okp)))
  {
   lock();
   TOutputFloor * floor = dynamic_cast<TOutputFloor *>(get_line(okp->floor_number));
   if(floor)
      ret = floor->get_output_kadr_param(okp);
      else
      SetLastError(MDM_ERR_NO_MODEMLINE);
   unlock();
  }
  return ret;
 }

 LRESULT       __fastcall TGranTablet::create_output_kadr(LPOUTPUT_KADR_PARAM kp)
 {
  LRESULT ret = GKH_RET_ERROR;
  lock();
   TOutputFloor * floor = dynamic_cast<TOutputFloor *>(get_line(kp->floor_number));
   if(floor)
   {
    if(floor->create_output_kadr(kp))
      {
       ret = GKH_RET_SUCCESS;
       cc_flags|=CCFL_OUT_FLOORS;
      } 
   }
   unlock();
  return ret;
 }

 LRESULT       __fastcall TGranTablet::delete_output_kadr(LPOUTPUT_KADR_PARAM kp)
 {
  LRESULT ret = GKH_RET_ERROR;
  lock();
   TOutputFloor * floor = dynamic_cast<TOutputFloor *>(get_line(kp->floor_number));
   if(floor)
   {
    if(floor->delete_output_kadr(kp))
    {
      ret = GKH_RET_SUCCESS;
      cc_flags|=CCFL_OUT_FLOORS;
    }
   }
   unlock();
  return ret;
 }
 
 LRESULT       __fastcall TGranTablet::modify_output_kadr(LPOUTPUT_KADR_PARAM kp,DWORD old_position)
 {
  LRESULT ret = GKH_RET_ERROR;
  lock();
   TOutputFloor * floor = dynamic_cast<TOutputFloor *>(get_line(kp->floor_number));
   if(floor)
   {
    if(floor->modify_output_kadr(kp,old_position))
      {
       ret = GKH_RET_SUCCESS;
       cc_flags|=CCFL_OUT_FLOORS;
      } 
   }
   unlock();
  return ret;
 }








