//Modem implementation OPC client functional
//Ostapenko D.V. Norilsk - Azov 1993-2017


#pragma hdrstop
#include "gkopc_client.hpp"
#include <otd_diag.h>
#include <opcda_i.c>
#include <opcenum_i.c>

#pragma argsused

THeap         TGkOpcModem::mod_heap(TSysInfo().GetPageSize()<<8);
DWORD         TGkOpcModem::total_mem_alloc  = 0;

void __fastcall check_heap(THeap & mod_heap)
{
  if(!mod_heap.IsValid())
      mod_heap.Create(TSysInfo().GetPageSize()<<8);
}

void * __fastcall TGkOpcModem::mem_alloc(size_t size)
{
   //void * ptr =  CoTaskMemAlloc(size);
   check_heap(mod_heap);
   void * ptr = NULL;
   if(size)
     {
      ptr = mod_heap.Alloc(size);
      total_mem_alloc +=  mod_heap.MemSize(ptr);
     }
   return ptr;
}

void   __fastcall TGkOpcModem::mem_free (void * ptr )
{
//    if(ptr) CoTaskMemFree(ptr);

   check_heap(mod_heap);
   if(ptr && mod_heap.Validate(ptr))
   {
      total_mem_alloc -=  mod_heap.MemSize(ptr);
      mod_heap.Free(ptr);
   }
}

DWORD   __fastcall TGkOpcModem::get_mem_used()
{
  return total_mem_alloc;
}


void * operator new      (size_t sz)  {return TGkOpcModem::mem_alloc(sz);}
void * operator new   [] (size_t sz)  {return TGkOpcModem::mem_alloc(sz);}
void   operator delete   (void * ptr) {TGkOpcModem::mem_free(ptr);}
void   operator delete[] (void * ptr) {TGkOpcModem::mem_free(ptr);}




LRESULT WINAPI module_main(DWORD cmd,LPARAM p1,LPARAM p2)
{
  static module_starter<TGkOpcModem> ms(MT_MODEM_OPC);
  if(cmd==GKME_CMD_UNLOAD_MODULE) return 1;
     return ms(cmd,p1,p2);
}

  TGkOpcModem::TGkOpcModem(DWORD number ):
  TModemBase(number),rep_id(-1),pu_diag(OTD_DIAG_MASK)
  {

   lock_param    = GKHB_AUTO_LOCK_OFF;
   alloc_gkhandle();
   reg_reports();

  }

  void        __fastcall    TGkOpcModem::reg_reports()
  {
   rep_id = report_reg_event_type(L"GKOPC_CLIENT",L"OPC-CLIENT Modem");
  }

  void        __fastcall    TGkOpcModem::free_line     (modem_line * line)
  {
   if(line)
      {
       line->stop();
       delete line;
      }

  }


BYTE      __fastcall TGkOpcModem::get_pu_number ()
{
  TLockHelper l(this->locker);
  return  this->mod_config.pu_number;
}

int         __fastcall    TGkOpcModem::convert_rx_data(LPWORD fa,LPBYTE in,int in_len,LPBYTE out,DWORD out_sz)
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


  LRESULT     __fastcall    TGkOpcModem::processing(DWORD cmd,LPARAM p1,LPARAM p2)
  {
   LRESULT ret;
     switch(cmd)
     {
      case CMD_OPC_GET_LINE: ret = _line_get_config((LPGKOPC_LINE_CONFIG)p1)    ; break;
      case CMD_OPC_ADD_LINE: ret = _line_add       ((LPGKOPC_LINE_CONFIG)p1)    ; break;
      case CMD_OPC_UPD_LINE: ret = _line_upd_config((LPGKOPC_LINE_CONFIG)p1,p2) ; break;
      case CMD_OPC_DEL_LINE: ret = _line_del(p1)                                ; break;
      case CMD_OPC_RESTART_LINE :  ret = _line_restart(p1);                     ; break;
      case CMD_OPC_LINE_GET_TU_SCRIPT : ret = _line_get_tu_script(p1,p2)        ; break;
      case CMD_OPC_LINE_SET_TU_SCRIPT : ret = _line_set_tu_script(p1,p2)        ; break;
      case CMD_OPC_LINE_GET_TR_SCRIPT : ret = _line_get_tr_script(p1,p2)        ; break;
      case CMD_OPC_LINE_SET_TR_SCRIPT : ret = _line_set_tr_script(p1,p2)        ; break;

      case CMD_OPC_LINE_GET_ITEM     : ret = _line_get_item    (p1,p2);         ; break;
      case CMD_OPC_LINE_RELEASE_ITEM : ret = _line_release_item(p1,p2);         ; break;

      default : ret = TModemBase::processing(cmd,p1,p2); break;
     }
   return ret;
  }

  void        __fastcall    TGkOpcModem::release(LPVOID arg)
  {
    TModemBase::release(arg);
  }

  BOOL        __fastcall    TGkOpcModem::can_start(DWORD reason,LPARAM start_param)
  {
   BOOL ret = TModemBase::can_start(reason,start_param);
   return ret;
  }

  DWORD       __fastcall    TGkOpcModem::start(DWORD reason,LPARAM start_param)
  {
   reg_reports();
   DWORD ret = TModemBase::start(reason,start_param);
   if(ret == GKH_RET_SUCCESS)
      {
       pu_diag = 0;
       do_recv_pu_data(true);
      }
   return ret;
  }

  DWORD       __fastcall    TGkOpcModem::stop (DWORD reason)
  {

   pu_diag = OTD_DIAG_MASK;
   do_recv_pu_data(false);
   DWORD ret = TModemBase::stop(reason);
       do_recv_pu_data(false);
   return ret;
  }

  bool        __fastcall    TGkOpcModem::read_settings()
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

  bool        __fastcall    TGkOpcModem::write_settings()
  {
   bool ret = TModemBase::write_settings();
   if(ret)
   {
    TRegsWriter rw(DupKey(reg_key));
    if(rw.IsRootOnly())
    {
      TLockHelper l (locker);
      if(cc_flags & CCFL_PUNUMBER)
       rw.WriteDword(REGDW_PUNUMBER,mod_config.pu_number,true);
      if(cc_flags & CCFL_PUNAME)
       rw.WriteString(REGSTR_PUNAME,mod_config.pu_name,true);
      if(cc_flags & CCFL_LINES)
       write_lines(rw);
    }


   }
   return ret;
  }

  int         __fastcall    TGkOpcModem::read_lines   (KeRTL::TRegsReader & rd)
  {
    int ret = 0;
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
      ms>>stream_ver>>lines_count;
      while(lines_count)
      {
        opc_line * line = new opc_line;
        line->read(ms,stream_ver);
        if(!add_line(line)) free_line(line);
        --lines_count;
      }
    }

    return ret;
  }

  int         __fastcall    TGkOpcModem::write_lines   (KeRTL::TRegsWriter & rw)
  {
    int ret = 0;
    KeRTL::TMemoryStream ms;
    ms.set_inc_by(8192);
    ms<<(DWORD)	OPC_MOD_STREAM_VER;
    TLockHelper l(locker);
    DWORD       ln_count = get_lines_count() ;
    ms<<(DWORD) ln_count;
    for(DWORD i = 0;i<ln_count;i++)
     {
          opc_line * line = dynamic_cast<opc_line *>( get_line_byindex(i) );
          if(line) line->write(ms);
     }
    rw.WriteBytes(REGBIN_LINES,ms.GetMemory(),ms.GetSize(),true);
    return ret;
  }

  DWORD       __fastcall    TGkOpcModem::get_config_data(DWORD mask,LPVOID buf,DWORD bsz)
  {
    DWORD ret = 0;
    if(check_data_size(buf,bsz,sizeof(mod_config)))
    {
      LPGKOPC_CLIENT_MOD_CONFIG dst_config = (LPGKOPC_CLIENT_MOD_CONFIG) buf;
      TLockHelper l(locker);
      dst_config->modem_number = this->mod_config.modem_number;
      dst_config->pu_number    = this->mod_config.pu_number;
      wcsncpy(dst_config->pu_name,mod_config.pu_name,KERTL_ARRAY_COUNT(dst_config->pu_name));
    }
    return ret;
  }

  bool        __fastcall    TGkOpcModem::set_config_data(DWORD mask,LPVOID buf,DWORD bsz)
  {
   bool ret = false;
   if(check_data_size(buf,bsz,sizeof(mod_config)))
   {
      LPGKOPC_CLIENT_MOD_CONFIG src_config = (LPGKOPC_CLIENT_MOD_CONFIG) buf;
       if(mask & CCFL_MODEM_NUMBER)
         {
           if(set_modem_number(src_config->modem_number,NULL))
             return false;
             else
             mod_config.modem_number = src_config->modem_number;
         }
      TLockHelper l(locker);
      if(mask & CCFL_PUNUMBER)
       {
        mod_config.pu_number = src_config->pu_number;
       }

      if(mask & CCFL_PUNAME)
       {
        wcsncpy(mod_config.pu_name,src_config->pu_name,KERTL_ARRAY_COUNT(mod_config.pu_name));
        if(is_running())
             do_recv_pu_data(true);
       }


   }
   return ret;
  }

  bool        __fastcall    TGkOpcModem::compare_config_data(DWORD mask,LPVOID buf,DWORD bsz,LPDWORD ch_mask,BOOL * restart)
  {
   bool ret = false ;
   if(check_data_size(buf,bsz,sizeof(mod_config)))
   {
    LPGKOPC_CLIENT_MOD_CONFIG src_config = (LPGKOPC_CLIENT_MOD_CONFIG) buf;
    DWORD __chmask  = 0;
    BOOL  __restart = FALSE;
    {
    TLockHelper l(locker);
    if((mask & CCFL_MODEM_NUMBER) && src_config->modem_number != mod_config.modem_number )
       {
         __chmask |= CCFL_MODEM_NUMBER;
         __restart = TRUE;
       }
    if((mask & CCFL_PUNUMBER) && src_config->pu_number != mod_config.pu_number )
       {
         __chmask |= CCFL_PUNUMBER;
         __restart = TRUE;
       }

    if((mask & CCFL_PUNAME) && wcsncmp(src_config->pu_name , mod_config.pu_name,KERTL_ARRAY_COUNT(mod_config.pu_name) ) )
       {
         __chmask |= CCFL_PUNAME;
       }
    }
    ret = __chmask ? true : false;
    if(ch_mask) *ch_mask = __chmask;
    if(restart) *restart = __restart;

   }
   return ret;
  }

  DWORD       __fastcall    TGkOpcModem::get_window_module  (wchar_t * buf,DWORD bsz)
  {
    static wchar_t  mod_wnd[] = L"gkopc_client_wnd.dll";
    DWORD len = wcslen(mod_wnd);
    if(buf && bsz>len)
        safe_strcpyn(buf,mod_wnd,bsz);
       else
       len = 0;
    return len;
  }


 void    __fastcall  TGkOpcModem::do_recv_pu_data(bool inc_name )
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
       opp.name_sz     = wcslen(mod_config.pu_name)+1;
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

  DWORD __fastcall TGkOpcModem::get_pu_diag(bool rescan)
  {
    if(rescan) scan_pu_diag();
    return   pu_diag;
  }

   DWORD __fastcall TGkOpcModem::scan_pu_diag  ()
   {
     return pu_diag;
   }


  LRESULT __fastcall TGkOpcModem::_line_get_config(LPGKOPC_LINE_CONFIG line_config)
  {
    LRESULT ret = GKH_RET_ERROR;
    if(check_data_size(line_config,sizeof(*line_config)))
    {
       opc_line * line = __get_opc_line (line_config->line_num);
       if(line)
        {
         line->get_line_config(line_config);
         ret = GKH_RET_SUCCESS;
        }

    }
    return  ret;
  }

  LRESULT __fastcall TGkOpcModem::_line_add       (LPGKOPC_LINE_CONFIG line_config)
  {
    LRESULT ret = GKH_RET_ERROR;
    if(check_data_size(line_config,sizeof(*line_config)))
    {
      opc_line * line = new opc_line(line_config->line_num);
      if(add_line(line))
        {
         line->set_line_config(line_config);
         cc_flags |= CCFL_LINES;
         if(this->is_running())
            line->start();
         ret = GKH_RET_SUCCESS;
        }
        else
        delete line;
    }
    return  ret;
  }

  LRESULT __fastcall TGkOpcModem::_line_upd_config(LPGKOPC_LINE_CONFIG line_config,DWORD old_num)
  {
    LRESULT ret = GKH_RET_ERROR;
    if(check_data_size(line_config,sizeof(*line_config)))
    {
      opc_line * line = this->__get_opc_line(old_num);
      if(line && line->set_line_config(line_config))
         {
          cc_flags |= CCFL_LINES;
          ret = GKH_RET_SUCCESS;
         }

    }
    return  ret;
  }

  LRESULT __fastcall TGkOpcModem::_line_del(DWORD line_num)
  {
    LRESULT ret = GKH_RET_ERROR;
    if(remove_line(line_num) )
    {
      ret =  GKH_RET_SUCCESS ;
      cc_flags |=  CCFL_LINES;
    }
    return  ret;
  }

  LRESULT __fastcall TGkOpcModem::_line_restart   (DWORD line_num)
  {
    LRESULT ret = GKH_RET_ERROR;
    if(this->is_running())
       {
         opc_line * line = this->__get_opc_line(line_num);
         if(line)
         {
           line->stop();
           line->start();
           ret = GKH_RET_SUCCESS;
         }
       }
    return ret;
  }



  void       __fastcall TGkOpcModem::do_report(DWORD evt_type,wchar_t * rep_text,LPVOID data,DWORD dt_sz)
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

  LRESULT    __fastcall TGkOpcModem::_line_get_item        (LPARAM p1,LPARAM p2)
  {
        opc_line * line = __get_opc_line (LOWORD(p1));
        if(line)
        {
          return line->get_opc_item(HIWORD(p1),(gkopc_item**)p2);
        }
     return GKH_RET_ERROR;
  }

  LRESULT    __fastcall TGkOpcModem::_line_release_item    (LPARAM ln,LPARAM p2)
  {
    opc_line * line = __get_opc_line (ln);
     if(line)
      {
        return line->release_opc_item((gkopc_item *)p2) ;
      }
     return GKH_RET_ERROR;
  }

  LRESULT __fastcall TGkOpcModem::_line_get_tu_script(DWORD line_num,LPARAM p)
  {
    LPSCRIPT_TEXT st = (LPSCRIPT_TEXT)p;
    LRESULT  ret     = GKH_RET_ERROR;
    if(check_data_size(st,sizeof(*st)))
      {
       opc_line * line = __get_opc_line (line_num);
       if(line)
        {
          ret = line->get_script(0,st);
          this->cc_flags |=CCFL_LINES;
         }
      }
    return ret;
  }

  LRESULT __fastcall TGkOpcModem::_line_set_tu_script(DWORD line_num,LPARAM p)
  {
    LPSCRIPT_TEXT st = (LPSCRIPT_TEXT)p;
    LRESULT  ret     = GKH_RET_ERROR;
    if(check_data_size(st,sizeof(*st)))
      {
       opc_line * line = __get_opc_line (line_num);
       if(line)
        {
          ret = line->set_script(0,st);
          this->cc_flags |=CCFL_LINES;
         }
      }
    return ret;
  }

  LRESULT __fastcall TGkOpcModem::_line_get_tr_script(DWORD line_num,LPARAM p)
  {
    LPSCRIPT_TEXT st = (LPSCRIPT_TEXT)p;
    LRESULT  ret     = GKH_RET_ERROR;
    if(check_data_size(st,sizeof(*st)))
      {
       opc_line * line = __get_opc_line (line_num);
       if(line)
       {
          ret = line->get_script(1,st);
          this->cc_flags |=CCFL_LINES;
       }
      }
    return ret;
  }

  LRESULT __fastcall TGkOpcModem::_line_set_tr_script(DWORD line_num,LPARAM p)
  {
    LPSCRIPT_TEXT st = (LPSCRIPT_TEXT)p;
    LRESULT  ret     = GKH_RET_ERROR;
    if(check_data_size(st,sizeof(*st)))
      {
       opc_line * line = __get_opc_line (line_num);
       if(line)
          {
           ret = line->set_script(1,st);
           this->cc_flags |=CCFL_LINES;
          }
      }
    return ret;
  }

LRESULT __fastcall TGkOpcModem::send       (LPMPROTO_HEADER mph,DWORD sz)
{
  if(mph && mph->fa == FA_OTD && mph->data_size>=(sizeof(otd_addr)+sizeof(WORD)))
  {
    sotd_addr sa(*(LPDWORD)mph->data);
    /*Проверка номера ПУ или для всех ПУ*/
    if(sa.pu == OTD_ADDR_MAXVALUE || sa.pu == get_pu_number())
    {
      if(OTD_ADDR_ISQUERY(&sa))
       do_recv_pu_data(true);
    }
  }

  return TModemBase::send(mph,sz);
}










