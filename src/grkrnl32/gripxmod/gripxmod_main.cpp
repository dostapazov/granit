//---------------------------------------------------------------------------

#pragma hdrstop
#include "gripxmod_main.hpp"
#include <tregstry.hpp>

KeRTL::THeap heap(TSysInfo().GetPageSize()<<8);
static DWORD total_alloc_size = 0;

void * operator new(size_t sz)
{
 LPVOID ptr = heap.Alloc(sz);
 if(ptr)
    total_alloc_size+=heap.MemSize(ptr,0);
 return ptr;
}
void * operator new [](size_t sz)
{
 LPVOID ptr = heap.Alloc(sz);
 if(ptr)
    total_alloc_size+=heap.MemSize(ptr,0);
 return ptr;
}

void operator delete( void * ptr)
{
 if(ptr && heap.Validate(ptr))
    {
     total_alloc_size-=heap.MemSize(ptr,0);
     heap.Free(ptr);
    } 
}

void operator delete[]( void * ptr)
{
 if(ptr && heap.Validate(ptr))
 {
    total_alloc_size-=heap.MemSize(ptr,0);
    heap.Free(ptr);
 }   
}


LRESULT WINAPI module_main(DWORD cmd,LPARAM p1,LPARAM p2)
{
  LRESULT ret = 0;
       switch(cmd)
       {
        case GKME_CMD_GET_MODULETYPE: 
			ret = MT_MODEM;break;
        case GKME_CMD_CREATE_HANDLE :
        {
            TGRNetModule * mod =new TGRNetModule;
            ret  = (LRESULT)(*(TGKHandleBased*)(mod))();
            if(!ret)
             delete mod;
        }
        break;
       }
       return ret;
}

  TGRNetModule::TGRNetModule()
  {
   lock_param = GKHB_AUTO_LOCK_OFF;
   alloc_gkhandle((wchar_t*)NULL);
   ZeroMemory(&settings,sizeof(settings));
   settings.work_mode = GRNET_WM_UNDEFINED;
   TBaseSocket::InitWS(MAKEWORD(2,0));
  }

  void          __fastcall TGRNetModule::release(LPVOID c)
  {
    TBaseSocket::DeinitWS();
    Inherit::release(c);
  }



  DWORD        __fastcall    TGRNetModule::get_lines_limit()
  {
   if(settings.work_mode == GRNET_WM_AS_SERVER)
      return 8;
   if(settings.work_mode == GRNET_WM_AS_CLIENT)
      return 1;
      return 0;
  }


  DWORD         __fastcall TGRNetModule::get_window_module (wchar_t * buf,DWORD bsz)
  {
   wchar_t window_module [] = L"GRIPXMODWND.DLL";
   safe_strcpyn(buf,window_module,bsz);
   return sizeof(window_module);
  }

  LRESULT       __fastcall TGRNetModule::processing (DWORD cmd,LPARAM p1,LPARAM p2)
  {
   LRESULT ret ;
   switch(cmd)
   {
    case  GRNETCM_GET_CLIENT_INFO : ret = get_client_info(p1,(LPGRNET_CLIENT_INFO)p2);break;
    case  GRNETCM_GETDIAG_TABLE   : ret = this->diag_get_items((lp_grdiag_item)p1,HIWORD(p2),LOWORD(p2));break;
    case  GRNET_GET_TUCLIENT      : ret = this->get_tu_client((LPGRNET_TU_CLIENT)p1,(DWORD)p2);break;
    case  GRNET_SET_TUCLIENT      : ret = this->set_tu_client((LPGRNET_TU_CLIENT)p1,(DWORD)p2);break;
    case  GRNET_DEL_TUCLIENT      : ret = this->del_tu_client((DWORD)p1);break;
    default                       : ret = Inherit::processing(cmd,p1,p2);break;
   }
   return ret;
  }

  DWORD         __fastcall TGRNetModule::get_client_info(DWORD line_num,LPGRNET_CLIENT_INFO cli)
  {
      grnet_line * line = static_cast<grnet_line *>(get_line(line_num));
      return line ? line->get_client_info(cli):GKH_RET_ERROR;
  }


  void __fastcall TGRNetModule::apply_settings()
  {
         for(DWORD i = 0;i<MAX_MODEM_LINES_COUNT;i++)
         {
          grnet_line * line = dynamic_cast<grnet_line*>(get_line_byindex(i));
          if(line)
             line->set_tx_retry_param(settings.tx_retry_count,settings.tx_retry_delay);
            else
            break;
         }
  }


 DWORD          __fastcall TGRNetModule::get_config_data(DWORD mask,LPVOID buf,DWORD bsz)
 {
  if(buf && bsz>=sizeof(settings))
    {
     settings.modem_number = this->get_modem_number();
     memcpy(buf,&settings,sizeof(settings));
     return GKH_RET_SUCCESS;
    }
   return GKH_RET_ERROR; 

 }

 bool           __fastcall TGRNetModule::check_config_data  (DWORD mask,LPVOID buf,DWORD bsz)
 {
   LPGRNET_SETTINGS gs =(LPGRNET_SETTINGS)buf;
   if(gs->modem_number < (DWORD)MAX_MODEM_COUNT && gs->work_mode != (DWORD)GRNET_WM_UNDEFINED && *gs->server_name!=0)
      {
       if(gs->work_mode == GRNET_WM_AS_CLIENT)
          {
           if(gs->pu_number>=OTD_ADDR_MAXVALUE)  //Клиенту должен быть присвоен номер ПУ
             {
              SetLastError(MDM_ERR_INVALID_PU_NUMBER);// modemmer_err.h
              return false;
             }
          }
       return true;
      }
   SetLastError(MERR_INVALID_MODULE_CONFIG);
   return false;
 }

 bool           __fastcall TGRNetModule::set_config_data(DWORD mask,LPVOID buf,DWORD bsz)
 {
   if(buf && bsz >=sizeof(GRNET_SETTINGS))
   {
    LPGRNET_SETTINGS gs =(LPGRNET_SETTINGS)buf; 
    if(mask&GRNET_CFG_MODEMNUMBER)
      {
       DWORD old_number;
       set_modem_number(gs->modem_number,&old_number);
       settings.modem_number = get_modem_number();
      } 
    if(mask&GRNET_CFG_PUNUMBER)
       this->settings.pu_number = gs->pu_number;
    if(mask&GRNET_CFG_SERVERPARAM)
       {
        lstrcpy(settings.server_name,gs->server_name) ;
        settings.server_socket=gs->server_socket;
       }
    if(mask&GRNET_CFG_CLIENTPARAM)
      {
       lstrcpy(settings.client_name,gs->client_name );
       settings.client_socket=gs->client_socket;
      }
    if(mask&GRNET_CFG_TXPARAM)
       settings.tx_retry_count = gs->tx_retry_count,settings.tx_retry_delay= gs->tx_retry_delay;

    if(mask&GRNET_CFG_WORKMODE)
       {
        settings.work_mode = gs->work_mode;
       }
    if(mask&GRNET_CFG_PUNAME)
      {
       safe_strcpyn(settings.pu_name,gs->pu_name,sizeof(settings.pu_name)/sizeof(wchar_t));
       if( is_running() && this->settings.work_mode == GRNET_WM_AS_CLIENT && !(mask & GRNET_CFG_RESTARTMASK))
           this->refresh_line(-1,-1);
      }

     if(mask & GRNET_CFG_RESTARTMASK)
       do_init();
       else
       apply_settings();



     return true;
    }
   return false;
 }

 bool           __fastcall TGRNetModule::compare_config_data(DWORD mask,LPVOID buf,DWORD bsz,LPDWORD ch_mask,BOOL * restart)
 {
   bool ret = false;

   if(buf && bsz >=sizeof(GRNET_SETTINGS))
   {
    DWORD cm = 0;
    ret = true;
    LPGRNET_SETTINGS gs =(LPGRNET_SETTINGS)buf;
    if((mask&GRNET_CFG_MODEMNUMBER) && gs->modem_number!= get_modem_number())
        cm|= GRNET_CFG_MODEMNUMBER;
    if((mask&GRNET_CFG_WORKMODE) && gs->work_mode!= this->settings.work_mode)
        cm |=GRNET_CFG_WORKMODE;
    if((mask&GRNET_CFG_PUNUMBER) && gs->pu_number!= this->settings.pu_number)
        cm |=GRNET_CFG_PUNUMBER;
    if((mask&GRNET_CFG_PUNAME) && lstrcmpiW(gs->pu_name,settings.pu_name))
        cm |=GRNET_CFG_PUNAME;
    if((mask&GRNET_CFG_SERVERPARAM) && (lstrcmpi(gs->server_name,settings.server_name )||gs->server_socket!=settings.server_socket))
        cm |=GRNET_CFG_SERVERPARAM;
    if((mask&GRNET_CFG_CLIENTPARAM) && (lstrcmpi(gs->client_name,settings.client_name )||gs->client_socket!=settings.client_socket))
        cm |=GRNET_CFG_CLIENTPARAM;
    if((mask&GRNET_CFG_TXPARAM) &&   (gs->tx_retry_count!=settings.tx_retry_count || gs->tx_retry_delay!=settings.tx_retry_delay))
        cm |=GRNET_CFG_TXPARAM;
    if(ch_mask) *ch_mask = cm;
    if(restart && (cm&GRNET_CFG_RESTARTMASK)) *restart = true;
   }
   return ret;
 }

  bool          __fastcall TGRNetModule::read_settings  ()
  {
   bool   ret =
   Inherit::read_settings();
   KeRTL::TRegsReader rd(DupKey(this->reg_key));
   settings.modem_number  =  modem_number;
   settings.work_mode     =  rd.ReadDword(GRNET_REGDW_WORKMODE,-1,true);
   settings.pu_number     =  rd.ReadDword(GRNET_REGDW_PUNUMBER,-1,true);
   settings.server_socket =  rd.ReadDword(GRNET_REGDW_SERVER_SOCKET,0,true);
   settings.client_socket =  rd.ReadDword(GRNET_REGDW_CLIENT_SOCKET,0,true);
   settings.tx_retry_count=  rd.ReadDword(GRNET_REGDW_TX_RETRY_COUNT,5,true);
   settings.tx_retry_delay=  rd.ReadDword(GRNET_REGDW_TX_RETRY_DELAY,150,true);
   rd.ReadString(GRNET_REGSTR_SERVER_NAME,settings.server_name,sizeof(settings.server_name),true);
   rd.ReadString(GRNET_REGSTR_CLIENT_NAME,settings.client_name,sizeof(settings.client_name),true);
   rd.ReadBytes(GRNET_REGBIN_TUCLIENTS,&tu_clients,sizeof(tu_clients),true);
   rd.ReadString(GRNET_REGSTR_PUNAME,settings.pu_name,sizeof(settings.pu_name)/sizeof(wchar_t),true);
   do_init();
   return ret;
  }

  bool          __fastcall TGRNetModule::write_settings ()
  {
   Inherit::write_settings();
    KeRTL::TRegsWriter rw;
    rw.SetRootKey(DupKey(reg_key));
   if(cc_flags & CCFL_MODULE_CONFIG)
   {
    rw.WriteString(GRNET_REGSTR_SERVER_NAME,settings.server_name,true);
    rw.WriteString(GRNET_REGSTR_CLIENT_NAME,settings.client_name,true);
    if(settings.work_mode!=(DWORD)GRNET_WM_UNDEFINED)
     rw.WriteDword(GRNET_REGDW_WORKMODE,settings.work_mode,true);
    rw.WriteDword(GRNET_REGDW_PUNUMBER,settings.pu_number,true);
    rw.WriteDword(GRNET_REGDW_SERVER_SOCKET,settings.server_socket,true);
    rw.WriteDword(GRNET_REGDW_CLIENT_SOCKET,settings.client_socket,true);
    rw.WriteDword(GRNET_REGDW_TX_RETRY_COUNT,settings.tx_retry_count,true);
    rw.WriteDword(GRNET_REGDW_TX_RETRY_DELAY,settings.tx_retry_delay,true);
    rw.WriteString(GRNET_REGSTR_PUNAME,settings.pu_name,true);
    cc_flags&=~CCFL_MODULE_CONFIG;
   }
   if(cc_flags& CCFL_TU_CLIENTS)
   {
    rw.WriteBytes(GRNET_REGBIN_TUCLIENTS,&tu_clients,sizeof(tu_clients),true);
    cc_flags&=~CCFL_TU_CLIENTS;
   }
   return true;
  }

  void __fastcall TGRNetModule::do_init()
  {
   clear_lines();
   int lines_limit =this->get_lines_limit();
   for(int i = 0;i<lines_limit;i++)
      add_line(new grnet_line(i,settings,&this->diag_table));
  }



  BOOL          __fastcall TGRNetModule::can_start(DWORD reason,LPARAM p2)
  {
   BOOL ret = TModemBase::can_start(reason,p2);
   if(ret)
   {
    if(check_config_data(-1,&settings,sizeof(settings)))
    {
    int proto[2] = {NSPROTO_IPX,0};
    WSAPROTOCOL_INFO pi;
    DWORD sz   = sizeof(pi);
    int cnt = WSAEnumProtocols(proto,&pi,&sz);
    if(cnt <1)
       {
        ret = FALSE;
        SetLastError(GRNET_ERR_IPXPROTO_NOT_INSTALLED);
       }
    }
    else
    ret = FALSE;   

   }
   return ret;
  }

  DWORD         __fastcall TGRNetModule::start(DWORD reason,LPARAM start_param)
  {
   diag_reset();
   diag_notify(0,MAX_CP_COUNT);
    connected_clients = 0;
    if(settings.work_mode ==  GRNET_WM_AS_CLIENT )
       {
        
        return Inherit::start(reason,start_param);
       }
    if( settings.work_mode == GRNET_WM_AS_SERVER )
      {
       if(Start(0))
       {
        //Отправить вызов/заказ данных  по ПУ
        make_order(false);
        return Inherit::start(reason,start_param);
       } 
      }
       return false;
  }

  DWORD         __fastcall TGRNetModule::stop (DWORD reason)
  {
    if(settings.work_mode ==  GRNET_WM_AS_SERVER)
       {
        TerminateAndWait(5000);
        //Поставить в приёмную очередь отказ от заказа
        make_order(true);
       }
       else
       {
        //Поставить в приёмную очередь
       }
       return Inherit::stop(reason);
  }

  DWORD   __fastcall   TGRNetModule::get_modem_info(wchar_t * buff,DWORD buff_len)
  {
   char out[MAX_PATH];
   *out=0;
   if(this->settings.work_mode == GRNET_WM_AS_SERVER)
     {
      wsprintf(out,"IPX Сервер %s",settings.server_name);
     }
   if(this->settings.work_mode == GRNET_WM_AS_CLIENT)
     {
      wsprintf(out,"IPX клиент ");
     }
   if(*out)
    {
     int len = lstrlen(out)+1;
     if((DWORD)len<buff_len)
       Ansi2Unicode(buff,out);
       else
       len=-len;
     return DWORD (len);
    }
    else
    return TModemBase::get_modem_info(buff,buff_len);
  }

 DWORD        __fastcall TGRNetModule::get_mem_used()
 {
  return total_alloc_size;
 }

 DWORD        __fastcall TGRNetModule::get_pu_name(wchar_t * buff,DWORD bsz)
 {
  DWORD ret = 0;
  if(buff && bsz)
  {
   ret = KeRTL::MIN(bsz-1,(DWORD)lstrlenW(settings.pu_name));
   if(ret)
     safe_strcpyn(buff,settings.pu_name,ret+1);
  }
  return ret;
 }

  DWORD __fastcall TGRNetModule::get_modem_flags()
  {
   DWORD ret ;
   TLockHelper l(locker);
   if(settings.work_mode == GRNET_WM_AS_CLIENT)
      ret = MPF_DATA_SOURCE;
   else
      ret = MPF_DATA_ACCEPTOR;
   return ret;
  }













