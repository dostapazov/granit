
//---------------------------------------------------------------------------
#pragma hdrstop
#include "modbusmod.hpp"
#include <tregstry.hpp>

using namespace KeRTL;



  TModbusModem::TModbusModem()
               :TModemBase()

  {
   ZeroMemory(&dev_cfg,sizeof(dev_cfg));
   dev_cfg.dw_size    = sizeof(dev_cfg);
   lock_param    = GKHB_AUTO_LOCK_OFF;
   alloc_gkhandle();
   rep_id = report_reg_event_type(L"MODBUS",L"MODBUS-Модем");
  }

  void        __fastcall    TModbusModem::free_line     (modem_line * m_line)
  {
   if(m_line)
   {
    m_line->stop();
    modbus_line * ml = dynamic_cast<modbus_line*>(m_line);
    if(ml)
      {
       delete ml;
      }
      else
      delete m_line;
   }
  }


  void     __fastcall TModbusModem::do_report(DWORD evt_type,wchar_t * rep_text,LPVOID data,DWORD dt_sz)
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

  bool        __fastcall TModbusModem::read_settings()
  {
   bool ret = false;
   TLockHelper l(locker);
   ret = TModemBase::read_settings();
   if(ret)
   {
     dev_cfg.modem_number = get_modem_number();
     TRegsReader rd(DupKey(reg_key));
     ret = rd.IsRootOnly();
     if(ret)
     {
      dev_cfg.pu_number = rd.ReadDword(REGDW_MODBUS_PUNUMBER,0,true);
      dev_cfg.pu_name    [0] = 0;
      dev_cfg.pu_name[rd.ReadString(REGSTR_MODBUS_PUNAME,dev_cfg.pu_name,KERTL_ARRAY_COUNT(dev_cfg.pu_name),true)] = 0;

      KeRTL::TMemoryStream ms;
      DWORD type,stream_size = 0;
      rd.GetValueType(REGBIN_MODBUS_POINTS,&type,&stream_size,true);

      ms.SetSize(stream_size);
      rd.ReadBytes(REGBIN_MODBUS_POINTS,ms.GetMemory(),stream_size,true);
      ms.Seek(0,KeRTL::TStream::smFromBegin );
      read_lines(ms);
      cc_flags = 0;
     }
   }
   return ret;
  }

  bool        __fastcall TModbusModem::write_settings()
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
      rw.WriteDword(REGDW_MODBUS_PUNUMBER,dev_cfg.pu_number,true);

      if(cc_flags & CCFL_PUNAME  )
       rw.WriteString(REGSTR_MODBUS_PUNAME,dev_cfg.pu_name,true);

     }
    if(cc_flags& CCFL_POINTS)
      {
       KeRTL::TMemoryStream  ms;
       write_lines(ms);
       rw.WriteBytes(REGBIN_MODBUS_POINTS,ms.GetMemory(),ms.GetPosition(),true);
      }
      cc_flags = 0;
   }

   return ret;
  }


  #pragma warn -8057
  DWORD       __fastcall TModbusModem::get_config_data    (DWORD mask,LPVOID buf,DWORD bsz)
  {
    DWORD ret = GKH_RET_ERROR;
    LPMODBUS_MOD_CONFIG mcfg = (LPMODBUS_MOD_CONFIG)buf;
    if(buf && bsz>=sizeof(dev_cfg))
    {
     lock();
     memcpy(mcfg,&dev_cfg,sizeof(dev_cfg));
     unlock();

     ret = GKH_RET_SUCCESS;
    }
    return ret;
  }



  bool        __fastcall TModbusModem::set_config_data    (DWORD mask,LPVOID buf,DWORD bsz)
  {
   bool   ret   = false;
   DWORD  error ;
   LPMODBUS_MOD_CONFIG mcfg = (LPMODBUS_MOD_CONFIG)buf;
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
        dev_cfg.pu_number = mcfg->pu_number;
     if(mask & CCFL_PUNAME)
     {
       safe_strcpyn(dev_cfg.pu_name , mcfg->pu_name,KERTL_ARRAY_COUNT(dev_cfg.pu_name));
       if(is_running()) do_recv_puname();
     }
   }
   return ret;
  }



  bool        __fastcall TModbusModem::compare_config_data(DWORD mask,LPVOID buf,DWORD bsz,LPDWORD ch_mask,BOOL * restart)
  {
   bool  ret = false;
   BOOL  rst   = FALSE;
   DWORD chmask = 0;
   LPMODBUS_MOD_CONFIG mcfg = (LPMODBUS_MOD_CONFIG)buf;
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

#pragma warn .8057

 void __fastcall TModbusModem::do_recv_puname()
 {
      lock();
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
       unlock(); 
 }

 int         __fastcall TModbusModem::convert_rx_data(LPWORD fa,LPBYTE in,int in_len,LPBYTE out,DWORD out_sz)
 {
  if(*fa == FA_OTD )
  {

   if(out_sz>= (DWORD)in_len)
   {
    if(in_len)
    {
    memcpy(out,in,in_len);
    sotd_proto sop(out,in_len);
    lock();
    sop.op.addr->pu = dev_cfg.pu_number;
    unlock();
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

  BOOL    __fastcall TModbusModem::can_start(DWORD reason,LPARAM p2)
  {
    //TODO
    BOOL ret = TModemBase::can_start(reason,p2);
    if(dev_cfg.pu_number)
    {

    }
    else
    {
     //TODO установить нормальную ошибку
     SetLastError(-1),ret = FALSE;
    }
    return ret ;
  }

  DWORD   __fastcall TModbusModem::start(DWORD reason,LPARAM p2)
  {
    DWORD ret = GKH_RET_ERROR;
    ret =  TModemBase::start(reason,p2);
    if(ret == GKH_RET_SUCCESS)
    {
      do_recv_pu_diag(0);
      do_recv_puname ( );
    }
    return ret;
  }

  DWORD   __fastcall TModbusModem::stop (DWORD reason)
  {
     DWORD ret = GKH_RET_SUCCESS;
     do_recv_pu_diag(OTD_DIAG_PUCONNECT);
     wchar_t ev_name[MAX_PATH];
     modbus_line::get_term_event_name(this,ev_name,KERTL_ARRAY_COUNT(ev_name));
     TEvent(ev_name,EVENT_ALL_ACCESS).PulseEvent();
     ret =  TModemBase::stop(reason);
     return ret;
  }


 DWORD       __fastcall TModbusModem::get_window_module  (wchar_t * buf,DWORD bsz)
 {
   safe_strcpyn(buf,L"modbus_mdm_wnd.dll",bsz);
   return lstrlenW(buf);
 }

 LRESULT     __fastcall TModbusModem::processing         (DWORD cmd,LPARAM p1,LPARAM p2)
 {
   LRESULT ret = GKH_RET_ERROR;
   switch(cmd)
   {
    case MBCM_ENUM_POINT :
    ret = point_enum((LPMODBUS_POINT)p1,(DWORD)p2);
    break;
    case MBCM_GET_POINT:
    ret = point_get((LPMODBUS_POINT)p1);
    break;
    case MBCM_CREATE_POINT:
    ret = point_create((LPMODBUS_POINT)p1);
    break;
    case MBCM_REMOVE_POINT:
    ret = point_remove((DWORD)p1);
    break;
    case MBCM_UPDATE_POINT:
    ret = point_update((LPMODBUS_POINT)p1);
    break;
    case MBCM_ENUM_GROUP   :
         ret = group_enum((LPMODBUS_GROUP)p1,p2);
    break;
    case MBCM_CREATE_GROUP :
    case MBCM_GET_GROUP    :
    case MBCM_UPDATE_GROUP :
    case MBCM_DELETE_GROUP :
    {
         LPMODBUS_GROUP mg=(LPMODBUS_GROUP)p1;
         if(check_data_size(mg,sizeof(*mg)))
           ret = group_cr_get_upd_del(cmd,mg);
    }
    break;
    case MBCM_SERVICE_FETCH_COMMEVENT_CTRL:
    case MBCM_SERVICE_FETCH_COMMEVENT_LOG :
          ret = fetch_comm_event(p1,p2,cmd ==MBCM_SERVICE_FETCH_COMMEVENT_LOG ? true:false);
    break;

    default :
    ret = TModemBase::processing(cmd,p1,p2);
    break;
   }
   return ret;
 }




  LRESULT __fastcall TModbusModem::point_enum   (LPMODBUS_POINT mdev,DWORD idx)
  {
   if(this->check_data_size(mdev,sizeof(*mdev)))
   {
   TLockHelper l(locker);
   if(idx < lines.size())
   {
    lines_map::iterator i = lines.begin();
    advance(i,idx);
    modbus_line * line =  dynamic_cast<modbus_line*>(i->second);
    mdev->point_no =  line->get_number();
    return point_get(mdev);
   }
   }
   return  GKH_RET_ERROR;
  }

  LRESULT __fastcall TModbusModem::point_get    (LPMODBUS_POINT mdev)
  {
    if(this->check_data_size(mdev,sizeof(*mdev)))
    {
      TLockHelper l(locker);
      if(lines.count(mdev->point_no))
      {
       modbus_line * line = dynamic_cast<modbus_line *>(lines[mdev->point_no]);
       if(line)
         {
          line->get_point(mdev);
          return GKH_RET_SUCCESS;
         }
      }

      SetLastError(MBERR_DEV_NOT_EXIST);
    }
   return GKH_RET_ERROR;
  }


  LRESULT __fastcall TModbusModem::point_create (LPMODBUS_POINT mdev)
  {

    if(check_data_size(mdev,sizeof(*mdev)))
    {

      if(mdev->point_no >= OTD_ADDR_MAXVALUE)
        SetLastError(MBERR_INVALID_DEVNUMBER);
      else
      {
      TLockHelper l(locker);
      if(lines.count(mdev->point_no))
       SetLastError(MBERR_DEV_ALREADY_EXIST);
       else
       {
        modbus_line * line = new modbus_line(mdev);
        if(add_line(line))
        {
          cc_flags |= CCFL_POINTS;
          return GKH_RET_SUCCESS;
        }
       }
      }
    }
    return GKH_RET_ERROR;
  }

  LRESULT __fastcall TModbusModem::point_remove (DWORD dev_no)
  {
   if(remove_line(dev_no))
     {
      cc_flags |= CCFL_POINTS;
      return GKH_RET_SUCCESS;
     }
   SetLastError(MBERR_DEV_NOT_EXIST);
   return GKH_RET_ERROR;
  }

  LRESULT __fastcall TModbusModem::point_update (LPMODBUS_POINT mdev)
  {
   TLockHelper l(locker);
   modbus_line * dev = this->get_modbus_line(mdev->point_no);
   if(dev)
   {
     if(dev->set_config(mdev))
     {
      cc_flags |= CCFL_POINTS;
      return GKH_RET_SUCCESS;
     }
   }
   else
   SetLastError(MBERR_DEV_NOT_EXIST);
   return GKH_RET_ERROR;
  }

  modbus_line * __fastcall TModbusModem::get_modbus_line(DWORD point_no)
  {
     modbus_line * line = dynamic_cast<modbus_line*>(get_line(point_no));
     return line;
  }


  LRESULT __fastcall TModbusModem::group_enum    (LPMODBUS_GROUP mbg,DWORD idx)
  {
    LRESULT ret = GKH_RET_SUCCESS;
    TLockHelper l(locker);
    modbus_line * dev = get_modbus_line(mbg->point_no);
    if(dev)
    {
       ret = dev->group_enum(mbg,idx);
    }
    else
    {
     ret = GKH_RET_ERROR;
     SetLastError(MBERR_DEV_NOT_EXIST);
    }
    return ret;
  }

  LRESULT __fastcall TModbusModem::group_cr_get_upd_del  (DWORD cmd,LPMODBUS_GROUP mbg)
  {
    LRESULT ret = GKH_RET_SUCCESS;
    TLockHelper l(locker);
    modbus_line * line = get_modbus_line(mbg->point_no);
    if(line)
    {
      switch(cmd)
      {
       case MBCM_CREATE_GROUP :
        ret = line->group_create(mbg);
       break;
       case MBCM_UPDATE_GROUP :
        ret = line->group_update(mbg);
       break;
       case MBCM_GET_GROUP    :
        ret = line->group_get(mbg);
       break;
       case MBCM_DELETE_GROUP :
        ret = line->group_delete(mbg);
       break;
      }
      if(ret == GKH_RET_SUCCESS && cmd != MBCM_GET_GROUP) cc_flags |= CCFL_POINTS;
    }
    else
    {
     ret = GKH_RET_ERROR;
     SetLastError(MBERR_DEV_NOT_EXIST);
    }
    return ret;
  }





  #define MODBUS_STREAM_VER  1

  int     __fastcall TModbusModem::write_lines (TStream & s)
  {
   TLockHelper l(locker);
   DWORD sv  = MODBUS_STREAM_VER;
   DWORD cnt = lines.size();
   s<<sv<<cnt;
   lines_map::iterator i = lines.begin(),end = lines.end();
   while(i!=end)
   {
     modbus_line * line = dynamic_cast<modbus_line *>(i->second);
     if(line)
        line->write(s);
     ++i;
   }
   return s.GetPosition();
  }

  int     __fastcall TModbusModem::read_lines  (TStream & s)
  {
   DWORD sv,cnt;
   if(!s.Eos())
   {
    s>>sv>>cnt;
    TLockHelper l(locker);
    while(lines.size())
    {
     remove_line(lines.begin()->first);
    }

    while(cnt>0)
    {
     modbus_line * line = new modbus_line;
     line->read(s);
     add_line(line);
     --cnt;
    }
   }
   return s.GetPosition();
  }

 void       __fastcall TModbusModem::do_recv_pu_diag(DWORD diag)
 {
  BYTE buf[128];
  sotd_proto sop;
  DWORD len = otd_proto_format(buf,sizeof(buf),OTD_PROTO_PART_DIAG|OTD_PROTO_PART_KPK,0,0,0,0,0,0,&sop());
  sop.op.addr->addr = -1;
  lock();
    sop.op.addr->pu  = this->dev_cfg.pu_number;
  unlock();
  *sop.op.diag = diag;
  otd_proto_protect(&sop(),len,OTD_DEF_POLINOM);
  queue_rxdata(-1,0,FA_OTD,buf,len,false);
 }

 LRESULT     __fastcall  TModbusModem::send (LPMPROTO_HEADER mph,DWORD sz)
 {
  sotd_proto sop((LPBYTE)mph->data,mph->data_size);
  {
  TLockHelper l(locker);
  if(OTD_ADDR_ISQUERY(sop().addr) && ((DWORD)sop.op.addr->pu == dev_cfg.pu_number || sop.op.addr->pu == OTD_ADDR_MAXVALUE) )
     {
      do_recv_puname();
     }
  }

  return TModemBase::send(mph,sz);
 }


LRESULT    __fastcall TModbusModem::fetch_comm_event(DWORD line_no,DWORD dev_no,bool log)
{
  LRESULT ret = GKH_RET_SUCCESS;
  TLockHelper l(locker);
  modbus_line * ml = get_modbus_line(line_no);
  if(ml)
    {
      ml->fetch_comm_event(dev_no,log);
    }
    else
    ret = GKH_RET_ERROR;

 return ret;
}






