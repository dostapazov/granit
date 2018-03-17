#pragma hdrstop
#include "modemmer_main.hpp"
#include <stdio.h>
#include <otd_proto.h>
#include <control_proto.h>
#include <alloc.h>



static THeap heap;
static DWORD total_mem_alloc = 0;

void * operator new [](size_t sz)
{
 void * ptr = NULL;
 if(!heap.IsValid())
     heap.Create(TSysInfo().GetPageSize()<<8);
 if(heap.IsValid())
 {
  ptr = heap.Alloc(sz);
 }
 else
 {
   //ptr = _aligned_malloc(sz,sizeof(DWORD));;
   ptr = malloc(sz);
 }
 return ptr;
}

void operator delete( void * ptr)
{
 __try{
  if(ptr && _USEDENTRY == heapchecknode(ptr))
  {

   free(ptr);
   return;
  }
  if(ptr && heap.IsValid() && heap.Validate(ptr))
    {
     heap.Free(ptr);
    }
    else
    {

    }
 }
   __finally
   {
   }

}

void operator delete[]( void * ptr)
{
 __try
 {
  if(ptr && heap.IsValid() && heap.Validate(ptr))
    {
     heap.Free(ptr);
    }
   else
   {
    if(ptr && _USEDENTRY == heapchecknode(ptr)) _aligned_free(ptr);
   }
  }
  __finally
  {}
}



module_starter<TModemmer> ms(MT_MODEMMER);

LRESULT WINAPI module_main(DWORD cmd ,LPARAM p1,LPARAM p2)
{
     return ms(cmd,p1,p2);
}


 TModemmer::TModemmer():TGKModulesSet(),dfa(this)
 {
  lock_param = GKHB_AUTO_LOCK_OFF;
  wchar_t evt_page_title[MAX_PATH];
  LoadStringW(this->lang_dll_instance,IDS_MODULE_NAME,evt_page_title,sizeof(evt_page_title)/sizeof(wchar_t));
  event_type = report_reg_event_type(MODEMMER_HANDLE_NAME_W,evt_page_title);
  alloc_gkhandle(MODEMMER_HANDLE_NAME_W);
  modems.set_modemmer(this);
 }

TModemmer::~TModemmer()
{

}

void     __fastcall TModemmer::modemmer_report(DWORD type,wchar_t * text)
{
 report(event_type,type,text);
}

wchar_t wm_name[] = L"MODEMMERWND.DLL";

DWORD  __fastcall TModemmer::get_window_module (wchar_t * buf,DWORD bsz)
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

#pragma warn -8057

 DWORD       __fastcall TModemmer::get_config_data(DWORD mask,LPVOID buf,DWORD bsz)
 {
  return  GKH_RET_SUCCESS;
 }

 bool        __fastcall TModemmer::set_config_data(DWORD mask,LPVOID buf,DWORD bsz)
 {
  return true;
 }

 bool        __fastcall TModemmer::compare_config_data(DWORD mask,LPVOID buf,DWORD bsz,LPDWORD ch_mask,BOOL * restart)
 {
  if(restart) *restart = FALSE;
  if(ch_mask) *ch_mask = 0;
  return true;
 }

#pragma warn .8057

 DWORD    __fastcall TModemmer::start(DWORD reason,LPARAM p2)
 {
   DWORD ret;
    if(reason!= MODULE_START_RECONFIGURE)
      {
       dfa.start();
      }
   dfa.reg_handler((*this)(),FA_CONTROL);
   int mref_count = modems.get_modems_count();
   for(int i = 0;i<mref_count;i++)
    {
     modem_ref * mr = modems.get_modem_ref_byidx(i);
     if(mr)
      mr->start();
    }
   ret = TGKModulesSet::start(reason,p2);

   DWORD re_idx = 0;
   REDIRECT_ENTRY re;
   re.dw_size = sizeof(re);
   while(redir_table.enum_entry(re_idx++,&re))
     {
      modem_ref * mr = modems.get_modem_ref(re.to_addr.modem);
      if(mr && !(re.options&MDM_REDIR_DISABLED))
         mr->get_interface()->call(MDMCM_BEGIN_REDIRECT,(LPARAM)&re,0);
     }

   return ret;
 }
 DWORD    __fastcall TModemmer::stop(DWORD reason)
 {
   /*Останов */

   DWORD lock_count = full_unlock();

   DWORD ret = TGKModulesSet::stop(reason);
   modems.stop();

   if(reason == MODULE_STOP_PROGRAM_SHUTDOWN || reason == MODULE_STOP_RELEASE  )
    {
     dfa.stop();

    }

    dfa.unreg_handler((*this)(),FA_CONTROL);
    restore_lock(lock_count);
    return ret;
 }

 TGKModuleInterface  * __fastcall TModemmer::get_interface(GKHANDLE h,DWORD type)
 {
   if(IS_MOD_MODEM(type) /*== MT_MODEM*/)
     return new TModemInterface(h);
     return TGKModulesSet::get_interface(h,type);
 }

 void                  __fastcall TModemmer::free_interface(TGKModuleInterface* iface)
 {
    TGKModulesSet::free_interface(iface);
 }

bool        __fastcall TModemmer::accept_loaded_module(TGKModuleInterface * iface,DWORD type)
{
      bool ret = TGKModulesSet::accept_loaded_module(iface,type);
      if(ret && IS_MOD_MODEM(type) /*== MT_MODEM*/)
      {
         TModemInterface * mi = static_cast<TModemInterface*>(iface);
         DWORD number = mi->get_number();
         if(number < MAX_MODEM_COUNT)
            {
             //mi->set_notify(_handle,MNF_COMMON,TRUE); ?Вопрос пока
             modem_ref * mr = new modem_ref(this,mi);
             modems.add(number,mr);
             if(is_running())
                mr->start();
            }
            else
            free_modems.add(iface);
      }
      return ret;

}

bool        __fastcall TModemmer::remove_module(GKHANDLE h,bool erase)
{
 TGKModuleInterface * iface =  modules_list.at(modules_list.get_interface_index(h));
 if(iface && IS_MOD_MODEM(iface->get_type())/* == MT_MODEM*/)
   {
     //iface->set_notify(_handle,MNF_COMMON,FALSE);
     DWORD modem_number = iface->call(MODEMCM_GET_NUMBER,0,0);
     if(modem_number < MAX_MODEM_COUNT )
     {
       modem_ref * mref  = modems.get_modem_ref(modem_number);
     if(mref)
       {
        modems.remove(modem_number);
        mref->stop();
        delete mref;
       }
     }
     else
     free_modems.remove(iface);
   }
 return TGKModulesSet::remove_module(h,erase);
}

void        __fastcall TModemmer::release(LPVOID arg)
{
 
   modems.release();
   TGKModulesSet::release(arg);
}


 LRESULT  __fastcall TModemmer::change_modem_number(LPMODEM_CHANGE_NUMBER mcn,BOOL no_call)
 {
  /*Процедура изменения номера модема*/
  LRESULT ret = GKH_RET_ERROR;
  DWORD error = 0;
  if(check_data_size(mcn,sizeof(*mcn),true))
  {
   if(no_call)
   {
     /*Эта часть вызвана из модема*/
     if(mcn->new_number>=1 && mcn->new_number<=250)
     {
     if(  modems.get_modem_ref(mcn->new_number)==NULL)//Проверка того, что требуемый номер не занят
     {
        modem_ref * mref = NULL;
        if(mcn->old_number<MAX_MODEM_COUNT)
        {
         mref  =  modems.remove(mcn->old_number);
         mref->stop();
        }
        else
        {

         TModemInterface * iface = reinterpret_cast<TModemInterface*>(free_modems.remove(free_modems.get_interface_index(mcn->modem)));
         if(iface)
            mref = new modem_ref(this,iface);
        }
        if(mref)
          {
           if(mcn->new_number<MAX_MODEM_COUNT)
               {
                mref->update_modem_number(mcn->new_number);
                modems.add(mcn->new_number,mref);
                mref->start();
                ret = GKH_RET_SUCCESS;
               }
              else
              {
               free_modems.add(mref->get_interface());
               error = MDM_ERR_INVALID_MODEM_NUMBER ; //modemmer_err.h
               delete mref;
              }

          }
     }
     else
     error = (MDM_ERR_USED_MODEM_NUMBER);//MERR_USED_MODEM_NUMBER
    }
    else
    error = MDM_ERR_INVALID_MODEM_NUMBER;

     wchar_t evt_templ[256];//L"Смена номера модема с %d на %d  "
     wchar_t evt_text[MAX_PATH];
     get_lang_string(IDS_MODEMMER_REP_CHNUMBER,evt_templ,sizeof(evt_templ)/sizeof(wchar_t));
     int len = wsprintfW(evt_text,evt_templ,mcn->old_number,mcn->new_number);
     if(error)
         get_error_text(error,evt_text+len,MAX_PATH-len,NULL);
     report(event_type,ret ? REPORT_ERROR_TYPE:REPORT_SUCCESS_TYPE,evt_text);

     if(ret == GKH_RET_SUCCESS)
     {
           #define NC_MODEM_NUMBER_SZ (sizeof(NOTIFY_CODE)+sizeof(MODEM_CHANGE_NUMBER)-sizeof(DWORD))
           char * buffer    [NC_MODEM_NUMBER_SZ];
           LPNOTIFY_CODE nc = (LPNOTIFY_CODE)buffer;
           nc->dw_size = NC_MODEM_NUMBER_SZ;
           nc->from_module = _handle;
           nc->notify_code = MNCODE_MODEM_NUMBER ;
           memcpy(nc->notify_data,mcn,sizeof(*mcn));
           notify_handles(nc,MNF_MODEM);
     }
   }
   else
   {
    if(mcn->new_number>=1 && mcn->new_number<=250)
    {
    TGKModuleInterface * mod = modules_list.at(modules_list.get_interface_index(mcn->modem));
    if(mod)
    {
      ret = mod->call(MODEMCM_SET_NUMBER,mcn->new_number,(LPARAM)&mcn->old_number);
    }
    }
    else
     error =MDM_ERR_INVALID_MODEM_NUMBER; 

   }
  }
  if(error)
   SetLastError(error);
  return ret;
 }

 LRESULT  __fastcall TModemmer::processing(DWORD cmd,LPARAM p1,LPARAM p2)
 {
  LRESULT ret = GKH_RET_ERROR;
  if(MCMD_TYPE(cmd) == MT_MODEMMER && cmd == MDMCM_FA_HANDLE)
  {
    return handle_fa((LPMPROTO_HEADER)p1,(*this)());
  }

     switch(cmd)
     {
     case MODEMCM_LINE_STATE_CHANGE  :  ret = handle_line_state((LPMODEM_LINE_STATE)p2);                            break;
     case MDMCM_CHANGE_MODEM_NUMBER  :  ret = change_modem_number((LPMODEM_CHANGE_NUMBER)p1,(BOOL)p2);              break;
     case MDMCM_REGISTER_HANDLER     :  ret = dfa.reg_handler((GKHANDLE)p1,(DWORD)p2)    ? GKH_RET_SUCCESS:ret;     break;
     case MDMCM_UNREGISTER_HANDLER   :  ret = dfa.unreg_handler((GKHANDLE)p1,(DWORD)p2)  ? GKH_RET_SUCCESS:ret;     break;
     case MDMCM_ENUM_HANDLER         :  if(dfa.enum_handlers(DWORD(p1),(LPFA_HANDLER) p2)) ret = GKH_RET_SUCCESS;   break;
     case MODEMCM_GET_MODEM_PROPS    :  ret = get_modem_props((LPMODEM_PROPS)p1,_handle);                           break;
     case MDMCM_GET_MODEM_PROPS      :  ret = get_modem_props((LPMODEM_PROPS)p1,(GKHANDLE)p2);                      break;
     case MDMCM_ENUM_MODEMS          :  ret = enum_modems(p1,(LPMODEM_ADDR)p2);                                     break;
     case MDMCM_ENUM_FREEMODEMS      :  ret = enum_freemodems(p1,(GKHANDLE*)p2);                                    break;
     case MDMCM_ENUM_MODEM_LINES     :  ret = enum_modem_lines(p1,(LPMODEM_LINE_PROPS)p2);                          break;
     case MODEMCM_GET_LINE_PROPS     :  ret = get_int_line_props(p1,(LPMODEM_LINE_PROPS)p2);                        break;
     case MDMCM_GET_LINE_PROPS       :  ret = get_line_props(p1,(LPMODEM_LINE_PROPS)p2);                            break;
     case MODEMCM_GET_LINE_STATS     :  ret = get_line_stats(modem_addr(0,0,p1,-1).addr,(LPMODEM_LINE_STATS)p2);    break;
     case MDMCM_GET_LINE_STATS       :  ret = get_line_stats((DWORD)p1,(LPMODEM_LINE_STATS)p2);                     break;
     case MDMCM_GET_MODEM_HANDLE     :  ret = (LRESULT) get_modem_handle((DWORD)p1);                                break;
     case MDMCM_TXQUEUE_TIMER        :  ret = modems.on_send_timeout(p1,p2);                                        break;
     case MDMCM_ENUM_REDIRENTRY      :  ret = redirect_enum((DWORD)p1,(LPREDIRECT_ENTRY)p2);                        break;
     case MDMCM_ENABLE_REDIRECT      :  ret = redirect_enable((BOOL)p1);                                            break;
     case MDMCM_ISREDIRECT_ENABLED   :  ret = redirect_is_enabled((LPBOOL)p1);                                      break;
     case MDMCM_REDIRENTRY_REMOVE    :  ret = redirect_remove((DWORD)p1);                                           break;
     case MDMCM_REDIRENTRY_ADD       :  ret = redirect_add((LPREDIRECT_ENTRY)p1);                                   break;
     case MDMCM_REDIRENTRY_UPDATE    :  ret = redirect_update((LPREDIRECT_ENTRY)p1,(LPDWORD)p2);                    break;
     case MDMCM_SEND                 :  ret = send((LPMPROTO_HEADER) p1,(DWORD)p2);                                 break;
     case MDMCM_REG_INTERNAL_MLINE   :  ret = this->reg_internal_line((GKHANDLE)p1,(DWORD)p2);                      break;

     default:  ret = TGKModulesSet::processing(cmd,p1,p2);
     }
     return ret;
 }


 void     __fastcall TModemmer::txrx_notify(LPMPROTO_HEADER mph,DWORD code)
 {
       DWORD mph_size = mph->data_size+sizeof(*mph)-sizeof(mph->data[0]);
       notify(MNF_MODEMMER,code,mph,mph_size);
 }

 LRESULT  __fastcall TModemmer::send(LPMPROTO_HEADER mph,DWORD timeout)
 {
  DWORD   ret = 0;
  if(mph->addr_to.sta == 0 &&  mph->addr_to.modem == 0)
  {
     if(mph->addr_to.line)
     {

      GKHANDLE hl = int_lines.get_intline_handle(mph->addr_to.line);
      if(hl)
       {
        if(!mph->pkt_num)
        {
         mph->pkt_num = int_lines.get_next_kadr_num(mph->addr_to.line,false);
         mproto_protect(mph);
        }

        if(!mph->header_cs)
            {
             mproto_protect(mph);
             //mph->header_cs= otd_calc_checksum(mph,sizeof(*mph)-sizeof(mph->data[0])-sizeof(mph->header_cs),OTD_DEF_POLINOM);
            }
        int_lines.update_tx(mph->addr_from.line,1,mproto_size(mph));
        txrx_notify(mph,MNCODE_MODEMMER_TX);
        ret = call_gkhandle(hl,MODEMCM_SEND,(LPARAM)mph,0);
        if(ret == (DWORD)GKH_RET_ERROR)
          {
           DWORD err =  GetLastError();
           if(err)
           {
            wchar_t text[512];
            get_error_text(err,text,KERTL_ARRAY_COUNT(text));
            report(this->event_type,REPORT_ERROR_TYPE,text);
           }
          }
          else
          {
           int_lines.update_rx(mph->addr_to.line,1,mproto_size(mph));
           txrx_notify(mph,MNCODE_MODEMMER_RX);
          }
       }
     }
     else
      {
       ret = dfa.handle(mph,MPROTO_SIZE(mph));
       if(mph->addr_from.sta == 0 &&  mph->addr_from.modem == 0 )
          int_lines.update_tx(mph->addr_from.line,1,mproto_size(mph));

       txrx_notify(mph,MNCODE_MODEMMER_RX);

      }

  }
    else
    ret =  modems.send(mph,timeout);
  return  ret;
 }

#pragma warn -8057

 LRESULT  __fastcall TModemmer::route   (LPMPROTO_HEADER mph,DWORD sz)
 {
  LRESULT ret = GKH_RET_ERROR;
  return  ret;
 }

#pragma warn .8057

 LRESULT  __fastcall TModemmer::redirect(LPMPROTO_HEADER mph,DWORD sz,BOOL * enable_handle)
 {
  /*Перенаправление*/
   LRESULT ret = GKH_RET_SUCCESS;
   ma_list list;
   DWORD opt = 0;
   if(redir_table.is_redirect_enabled() && redir_table.make_redirect_list(mph,list,&opt))
   {
    modem_addr save_addr(mph->addr_to);
    mph->flags|=  MPH_REDIRECTED;
    while(list.size())
    {
     mph->addr_to.addr = list.front().addr;
     list.pop_front();
     notify(MNF_MODEMMER,MNCODE_MODEMMER_REDIRECT,mph,sz);
     
     send(mph,-1);

    }
    mph->addr_to.addr = save_addr.addr;
    mph->flags&= ~MPH_REDIRECTED;

    if(enable_handle)
      *enable_handle = (opt&MDM_REDIR_REDIRONLY) ? FALSE:TRUE;

   }
   return ret;
 }

 LRESULT  __fastcall TModemmer::input_handler(LPMPROTO_HEADER mph,DWORD sz)
 {
   LRESULT ret = GKH_RET_ERROR;
   BOOL enable_handle = TRUE;
   if( mph && sz)
   {

     DWORD rsz = sz-sizeof(*mph)+sizeof(mph->data[0]);
     if(rsz!=(DWORD)mph->data_size)
       {
        mph->data_size = rsz;
        #ifdef _DEBUG
        DebugBreak();
        #endif
       }

     notify(MNF_MODEMMER,MNCODE_MODEMMER_RX,mph,sz);
     
     if(mph->addr_to.sta == 0 && mph->addr_to.modem == 0)//Нулевой адрес значит нам
     {
     mph->internal = (DWORD)_handle;
     //ret  = redirect(mph,sz,&enable_handle);
     if(enable_handle)
     {
       if( dfa.handle(mph,sz))
          ret = GKH_RET_SUCCESS;
        //txrx_notify(mph,MNCODE_MODEMMER_RX);
    }
    }
    else
    send(mph,0);//Переслать
   }
   return ret;
 }

 LRESULT __fastcall TModemmer::handle_line_state(LPMODEM_LINE_STATE mls)
 {
    LRESULT ret = GKH_RET_ERROR;

    if(this->check_data_size(mls,sizeof(MODEM_LINE_STATE)))
    {
     ret = GKH_RET_SUCCESS;
     modems.handle_line_state(mls);
     /*
      if(need_notify(MNF_MODEM) && (mls->state_changes&(MODEM_LINE_STATE_EXIST|MODEM_LINE_STATE_CONNECT|MODEM_LINE_STATE_WORKED)))
      {
       //Напоминание об изменении состоянии линии
       #define NFML_SIZE (sizeof(NOTIFY_CODE)+sizeof(MODEM_LINE_STATE)-sizeof(DWORD))
       BYTE buffer[NFML_SIZE];
       LPNOTIFY_CODE nc = (LPNOTIFY_CODE)buffer;
       nc->dw_size      = NFML_SIZE;
       nc->from_module  = _handle;
       nc->notify_code  = MNCODE_LINE_STATE;
       memcpy(nc->notify_data,mls,sizeof(*mls));
       notify_handles(nc,MNF_MODEM);
      }
    */
    }
    
  return ret;
 }

/********** Интерфейсные функции ********************************/

 LRESULT  __fastcall TModemmer::get_modem_props (LPMODEM_PROPS mp,GKHANDLE h)
 {
  LRESULT ret = GKH_RET_ERROR;
  if(check_data_size(mp,sizeof(*mp)))
  {
   if(is_same_gkhandle(h,_handle))
   {
        mp->lines_limit = MODEM_ADDR_MAXVALUE-5;
        mp->lines_count = this->int_lines.get_lines_count();

        wchar_t * int_name = L"Внутренний модем ";
        mp->modem_info_len = KeRTL::MIN((DWORD)wcslen(int_name),mp->modem_info_size);
        wcsncpy(mp->modem_info,int_name,mp->modem_info_len);
        mp->modem_info[mp->modem_info_len] = 0;
        return GKH_RET_SUCCESS ;

   }

   if(mp->addr.modem<MAX_MODEM_COUNT)
   {
    if(!mp->addr.modem)
    {
      h   = int_lines.get_intline_handle(mp->addr.line);
      if(h)
       ret = call_gkhandle(h,MODEMCM_GET_MODEM_PROPS,(LPARAM)mp,(LPARAM)h);
    }
     else
     {
     //modems.lock();
     modem_ref * mr = modems.get_modem_ref(mp->addr.modem);
     if(mr)
      {
       TModemInterface * modem_iface = mr->get_interface();
       if(modem_iface)
       {
        //modems.unlock();
        ret = modem_iface->get_props(mp);
        return ret;
       }
      }
      //modems.unlock();
     }

   }
   else
   {
    free_modems.lock();
    TModemInterface * mi = reinterpret_cast<TModemInterface*>(free_modems.at(free_modems.get_interface_index(h)));
    if(mi)
    ret = mi->get_props(mp);
    free_modems.unlock();
   }
  }
  return ret;
 }

 LRESULT  __fastcall  TModemmer::enum_modems(DWORD idx,LPMODEM_ADDR ma)
 {
  LRESULT ret = GKH_RET_SUCCESS;
  if(!idx)
  {
   ma->sta   = 0;
   ma->modem = 0;
   ma->line  = -1;
   ma->reserv1 = -1;
  }
  else
  ret = modems.get_modem_addr(idx-1,ma) ? GKH_RET_SUCCESS:GKH_RET_ERROR;
  return ret;

 }

 LRESULT  __fastcall TModemmer::enum_freemodems(DWORD idx,GKHANDLE * handle)
 {
   LRESULT ret = GKH_RET_ERROR;
   if(handle)
   {
   *handle = NULL;
   free_modems.lock();
   TGKModuleInterface * iface = free_modems.at(idx);
   if(iface)
     {
      *handle = (*iface)();
      ret = GKH_RET_SUCCESS;
     }
     free_modems.unlock();
    }

   return ret;
 }

 LRESULT  __fastcall TModemmer::enum_modem_lines(DWORD idx,LPMODEM_LINE_PROPS mlp)
 {
  LRESULT ret = GKH_RET_ERROR;
  if(!mlp->addr.modem )
  {
   ret = int_lines.enum_lines(idx,mlp);
  }
  else
  {
      modems.lock();
      modem_ref * mref = modems.get_modem_ref(mlp->addr.modem);
      if(mref)
      {
         ret = mref->get_interface()->enum_lines(idx,mlp);
      }
      modems.unlock();
  }
  return ret;
 }

 LRESULT     __fastcall TModemmer::get_int_line_props (DWORD addr,LPMODEM_LINE_PROPS mlp)
 {
   LRESULT ret = GKH_RET_ERROR;
    if(check_data_size(mlp,sizeof(*mlp)))
       ret = int_lines.get_line_props(addr,mlp);
    return ret;
 }

 LRESULT  __fastcall TModemmer::get_line_props  (DWORD addr,LPMODEM_LINE_PROPS mlp)
 {
  LRESULT ret = GKH_RET_ERROR;
  MODEM_ADDR ma;ma.addr = addr;
  if(!ma.modem )
  {
    ret = get_int_line_props((DWORD)ma.line,mlp);
  }
  else
  {
      modems.lock();
      modem_ref * mref = modems.get_modem_ref(ma.modem);
      if(mref)
         ret = mref->get_interface()->get_line_props(ma.line,mlp);
      modems.unlock();
  }
  return ret;
 }


 LRESULT     __fastcall TModemmer::get_line_stats(DWORD addr,LPMODEM_LINE_STATS mls)
 {
  LRESULT ret = GKH_RET_ERROR;
  MODEM_ADDR ma;ma.addr = addr;
  if(!ma.modem )
  {
    if(check_data_size(mls,sizeof(*mls)))
    {
      ret = int_lines.get_line_stats((DWORD)ma.line,mls);
    }
  }
  else
  {
   modems.lock();
   modem_ref * mref = modems.get_modem_ref(ma.modem);
   if(mref)
      ret = mref->get_interface()->get_line_stats(ma.line,mls);
   modems.unlock();
  }
  return ret;
 }

 GKHANDLE    __fastcall TModemmer::get_modem_handle(DWORD modem)
 {
  GKHANDLE ret = (GKHANDLE)GKH_RET_ERROR;
  if(!modem )
      ret = _handle;
  else
  {
   modem_ref * mref = modems.get_modem_ref(modem);
   if(mref)
     ret = (*mref->get_interface())();
  }
  return ret;
 }

 bool    __fastcall TModemmer::read_settings  ()
 {
  //Чтение конфигурации
  redir_table.read(this->reg_key);
  return TGKModulesSet::read_settings();
 }

 bool    __fastcall TModemmer::write_settings ()
 {
  if(cc_flags&=CCFL_REDIR_TABLE)
   {
    redir_table.write(reg_key);
    cc_flags&=~CCFL_REDIR_TABLE;
   }
   return TGKModulesSet::write_settings();
 }

 LRESULT  __fastcall TModemmer::redirect_enable(BOOL enable)
 {
   if(redir_table.redirect_enable(enable))
   {
      cc_flags|=CCFL_REDIR_TABLE;
   }
  return GKH_RET_SUCCESS;
 }

 LRESULT  __fastcall TModemmer::redirect_is_enabled(LPBOOL enable)
 {
  LRESULT ret = enable ? GKH_RET_SUCCESS:GKH_RET_ERROR;
  if(enable)
  {
   *enable = redir_table.is_redirect_enabled();
  }
  return ret;
 }

 LRESULT  __fastcall TModemmer::redirect_add (LPREDIRECT_ENTRY re)
 {
   LRESULT ret = GKH_RET_ERROR;
   if(check_data_size(re,sizeof(*re)))
   {
    ret = redir_table.add(re);
    if(ret !=(LRESULT)-1)
     {
      cc_flags|=CCFL_REDIR_TABLE;
      redirect_notify(MNCODE_REDIRECT_ADD,ret);
      modem_ref * mr = modems.get_modem_ref(re->to_addr.modem);
      if(mr && !(re->options&MDM_REDIR_DISABLED))
         {
          TModemInterface * mi = mr->get_interface();
          if(mi->is_running())
             mi->call(MDMCM_BEGIN_REDIRECT,(LPARAM)re,0);
         }
     }
   }
   return ret;
 }

 LRESULT  __fastcall TModemmer::redirect_remove(DWORD index)
 {
   LRESULT ret = GKH_RET_ERROR;
   REDIRECT_ENTRY re;
   re.dw_size = sizeof(re);
   if(redir_table.enum_entry(index,&re))
   {
   ret = redir_table.remove(index);
   if(ret !=(LRESULT)-1)
     {
      lock();
      cc_flags|=CCFL_REDIR_TABLE;
      unlock();
      redirect_notify(MNCODE_REDIRECT_REMOVE,index);
      //TODO сообщить модему о завершении перенаправления
      modem_ref * mr = modems.get_modem_ref(re.to_addr.modem);
      if(mr && !(re.options&MDM_REDIR_DISABLED))
         {
          TModemInterface * mi = mr->get_interface();
          mi->call(MDMCM_END_REDIRECT,(LPARAM)&re,0);
         }
   }
   }
   return ret;

 }

 LRESULT  __fastcall TModemmer::redirect_update (LPREDIRECT_ENTRY re,LPDWORD index)
 {
   LRESULT  ret = GKH_RET_ERROR;
   if(check_data_size(re,sizeof(re)) && index)
   {
     if(redirect_remove(*index)==GKH_RET_SUCCESS)
     {
      ret = redirect_add(re);
      if(ret!=GKH_RET_ERROR)
         {*index = ret ;ret = GKH_RET_SUCCESS;}
     }
   }
   return ret;

 }

LRESULT  __fastcall TModemmer::redirect_enum(DWORD index,LPREDIRECT_ENTRY re)
{

 LRESULT ret = GKH_RET_ERROR;
 if(check_data_size(re,sizeof(*re)) && redir_table.enum_entry(index,re))
 {
   ret = GKH_RET_SUCCESS;
 }
 return ret;
}

 void     __fastcall TModemmer::redirect_notify (DWORD code,DWORD index)
 {

  if(need_notify(MNF_MODEMMER_REDIRECT))
  {
   NOTIFY_CODE nc;
   nc.dw_size = sizeof(nc);
   nc.notify_code = code;
   nc.notify_data[0] = index;
   notify_handles(&nc,MNF_MODEMMER_REDIRECT);
  }
 }

 DWORD  __fastcall TModemmer::get_mem_used()
 {
  return heap.IsValid() ? heap.GetMemoryUsed() : GKH_RET_ERROR;
  //total_mem_alloc;
 }

 LRESULT __fastcall TModemmer::reg_internal_line(GKHANDLE h,DWORD ln)
 {
   //Регистрация внутренней линии
   LRESULT ret;
   MODEM_LINE_STATE mls;
   mls.dw_size = sizeof(mls);
   mls.addr.addr = -1;
   mls.addr.sta  = 0;
   mls.addr.modem= 0;
   mls.addr.line = ln;
   mls.state_changes = MODEM_LINE_STATE_EXIST|MODEM_LINE_STATE_WORKED|MODEM_LINE_STATE_CONNECT;
   mls.line_state    = 0;
   if(ln == (DWORD)-1)
    {
     ret = int_lines.reg_intline(h);
     if(ret && ret<MODEM_ADDR_MAXVALUE)
        {
         mls.line_state = mls.state_changes;
         mls.addr.line  = ret;
         notify(MNF_MODEM,MNCODE_LINE_STATE,&mls,sizeof(mls));
        }
    }
    else
    {
     ret = int_lines.unreg_intline(h,ln);
     if(ret)
       {
        //Уведомление
        notify(MNF_MODEM,MNCODE_LINE_STATE,&mls,sizeof(mls));
       }
    }

   return ret;
 }

DWORD   __fastcall TModemmer::on_exception     (lpexcept_record er)
{
 modems.unlock(true);
 free_modems.unlock(true);
 full_unlock();
 return TGKModulesSet::on_exception(er);
}

 LRESULT __fastcall TModemmer::handle_fa(LPMPROTO_HEADER mph,GKHANDLE /*h_from*/)
 {
   LRESULT ret = GKH_RET_ERROR;
   lpctrl_proto_header lph = NULL;
     if(mproto_check(mph) && mph->fa == FA_CONTROL && mph->data_size>=sizeof(*lph))
     {
        lph =  (lpctrl_proto_header)mph->data;

        if(!CTRL_PROTO_IS_RESPOND(lph->command))
        {
         switch(CTRL_PROTO_COMMAND(lph->command))
         {
          case CTRL_PROTO_CMD_TIMESYNC:  ret = ctrl_time_sync(mph);break;
          default :
          break;
         }
        }
     }
   return ret;
 }

 LRESULT __fastcall TModemmer::ctrl_time_sync(LPMPROTO_HEADER mph)
 {
    lpctrl_proto_header lph =  (lpctrl_proto_header)mph->data;
    lpctrl_proto_times lpt = (lpctrl_proto_times)lph->data;
    lph->command|= CTRL_PROTO_FLAG_RESPOND;
    lpt->remote_time = GetTime(lpt->utc ? true:false);
    mph->addr_to.addr = mph->addr_from.addr;
    ctrl_proto_protect(lph);
    mproto_protect(mph);
    send(mph,500);
    return GKH_RET_SUCCESS;

 }

