/*******************************************************************************
 *
 * Модуль    : gkmodules
 * Описание  : Реализация базового класса определяющего модули
 * Автор     : Остапенко Д.В.
 * Версия    : 1
 * Создан    : 28.06.2002
 *
 ******************************************************************************/

/*
 Небольшое примечание:
 В случае  Неинициализированных указателей нет смысла даже проверять на
 IsBadRead/WritePtr т.к. этот указатель вполне может указывать на область стека
 со всеми вытекающими отсюда последствиями...
*/


#pragma hdrstop
#include <gkmodules.hpp>
#include <stdlib.h>
#include <tregstry.hpp>
#include <gkreporter.h>
#include <gkdebug.h>
#include <malloc.h>
#include <alloc.h>
#include <kfname.hpp>
#include <kertlio.hpp>

    KeRTL::TFASTMutex  TGKModule::resource_locker;
    HINSTANCE          TGKModule::dll_instance      = NULL;
    HINSTANCE          TGKModule::lang_dll_instance = NULL;
    LANGID             TGKModule::current_lang      = LANG_RUSSIAN;

    HINSTANCE          TGKModule::common_error_instance = NULL;

#if defined _MSC_VER || __CODEGEARC__
 
	 LPVOID TGKModule::_link_module_entry_guaranty = (LPVOID)gkmodule_entry;
#endif

         BOOL TGKModule::_is_debug_build = FALSE;


DWORD     __fastcall TGKModule::is_running()
{
  DWORD ret = FALSE;
  TLockHelper l(locker);
  if((module_state&(MODULE_STATE_RUNNING|MODULE_STATE_START_PENDING|MODULE_STATE_STOP_PENDING)) == MODULE_STATE_RUNNING)
       ret = TRUE;
  return ret;
}

 bool __fastcall  TGKModule::check_running()
 {
  if(!is_running())
      {::SetLastError(MERR_MODULE_NOTRUNNING); return false;}
    return true;  
 }

 DWORD  __fastcall TGKModule::set_module_state(DWORD state_mask,bool set)
 {
   DWORD old_state = module_state;
   DWORD new_state;
   lock();
   if(state_mask&MODULE_STATE_RUNNING)
       module_state &=~(MODULE_STATE_PAUSED|MODULE_STATE_START_PENDING|MODULE_STATE_STOP_PENDING);
   if(set)
     new_state = module_state|state_mask;
     else
     new_state = module_state&~state_mask;
     module_state = new_state;
   unlock();
     if(old_state!=new_state && need_notify(MNF_COMMON))
     {
        notify(MNF_COMMON,MNCODE_MODULE_STATE,&new_state,sizeof(new_state));
     }
     return old_state;
 }

 bool __fastcall  TGKModule::check_data_size(LPVOID ptr,DWORD cb_sz,bool exact)
 {
  /*!IsBadReadPtr(ptr,cb_sz) &&*/ // remove
  if(ptr &&  ( exact ? *((LPDWORD)ptr)==cb_sz : *((LPDWORD)ptr)>=cb_sz ) )
     return true;
     SetLastError(MERR_INVALID_BUFFER_SIZE);
    return false;
 }

 bool    __fastcall  TGKModule::check_data_size(LPVOID ptr,DWORD ptr_size,size_t cb_sz,bool exact)
 {
  if(ptr && ptr_size>=cb_sz && ( exact ? *((LPDWORD)ptr)==cb_sz : *((LPDWORD)ptr)>=cb_sz ) )
     return true;
   SetLastError(MERR_INVALID_BUFFER_SIZE);
   return false;
 }


 void __fastcall  TGKModule::init_instance(HINSTANCE dll_inst)
 {
  dll_instance      = dll_inst;
  lang_dll_instance = dll_inst;
  set_language(LANGIDFROMLCID(GetThreadLocale()));
 }

bool __fastcall  TGKModule::set_language(LANGID lang)
{
  bool ret = false;
  resource_locker.Lock();
 if(lang!= current_lang)
 {
  wchar_t lang_dll_name[MAX_PATH<<1];
  wchar_t drive[MAX_PATH];
  wchar_t dir  [MAX_PATH];
  wchar_t name [MAX_PATH];
  wchar_t ext  [MAX_PATH];
  wchar_t locale_name[MAX_PATH];
  GetModuleFileNameW(dll_instance,lang_dll_name,sizeof(lang_dll_name)/sizeof(wchar_t));
  _wsplitpath(lang_dll_name,drive,dir,name,ext);
  *ext = '.';
  *(ext+1) = 0;
  GetLocaleInfoW(MAKELCID(lang,SORT_DEFAULT),LOCALE_SABBREVLANGNAME,locale_name,sizeof(locale_name)/sizeof(wchar_t));
  safe_strcpy(ext+1,locale_name);
  _wmakepath(lang_dll_name,drive,dir,name,ext);
  if(GetFileAttributesW(lang_dll_name) == DWORD(-1))
     {
      //Файла с полным расширением не существует - убираем последний симвод ENG -> EN, RUS -> RU
      lang_dll_name[lstrlenW(lang_dll_name)-1] = 0;
     }
  HINSTANCE hInst = LoadLibraryW(lang_dll_name);
  if(hInst)
  {
    DisableThreadLibraryCalls(hInst);
	ret = true;
   if(lang_dll_instance)
     FreeLibrary(lang_dll_instance);
     lang_dll_instance = hInst;
     current_lang = lang;
  }
   //Теперь загружаем DLL со списком общих ощибок для всех модулей
   {
    //Последовательность действий
    // 1. Ищем DLL gkcomerr.XXX где XXX - аббревиатура языка
    safe_strcpy(name,L"gkcomerr");
    safe_strcpy(ext+1,locale_name);
    _wmakepath(lang_dll_name,0,0,name,ext);
    if(GetFileAttributesW(lang_dll_name)==DWORD(-1))
       {
        lang_dll_name[lstrlenW(lang_dll_name)-1] = 0;
        if(GetFileAttributesW(lang_dll_name)==DWORD(-1))
          _wmakepath(lang_dll_name,0,0,name,L"dll");
       }
     HINSTANCE comm_error = LoadLibraryW(lang_dll_name);
     if(comm_error)
     {
        DisableThreadLibraryCalls(comm_error);
        if(common_error_instance)
           FreeLibrary(common_error_instance);
          common_error_instance = comm_error;
     }
   }
 }
 else
  ret = true;

  resource_locker.Unlock();
  return ret;
}

//освобождение модуля
void    __fastcall TGKModule::release (LPVOID)
{
  if(is_running()) //Если работает то остановить
    stop(MODULE_STOP_RELEASE);
  if(cc_flags)
     write_settings();
  notify_handles_list.lock();
  notify_handles_list.free();
  notify_handles_mask.clear();
  notify_handles_list.unlock();

  if(reg_key)
  {
   RegCloseKey(reg_key);
   reg_key    = NULL;
  }
  if(reg_key_name)
     delete [] reg_key_name;
  if(config_folder)
     delete [] config_folder;
  /*

  Ошибка при нескольких модулях в одной DLL
    
  if(lang_dll_instance&& lang_dll_instance!=dll_instance)
     FreeLibrary(lang_dll_instance);
  if(common_error_instance && common_error_instance!=dll_instance)
      {
       if(FreeLibrary(common_error_instance))
          common_error_instance = NULL;
      }*/

}


//Инициализация модуля - установка владельца
bool    __fastcall TGKModule::init_module(LPMODULE_INIT mi)
{
   lock();
   owner   = mi->owner;
   reg_key = DupKey(mi->reg_key);
   config_folder = newstr(mi->config_folder);
   reg_key_name  = newstr(mi->reg_key_name);
   unlock();
   license_type = check_license();
   read_settings   ();
   return  true;
}



//-----------------------------------------------------------------------------
//Procedure: processing
//Author:    Администратор
//Date:      27-июн-2002
//Arguments: DWORD cmd,LPARAM p1,LPARAM p2
//-----------------------------------------------------------------------------

LRESULT __fastcall TGKModule::processing(DWORD cmd,LPARAM p1,LPARAM p2)
{
 LRESULT ret(GKH_RET_ERROR);
 switch (cmd)
 {
  case MCMD_MODULE_INIT   :
       if(check_data_size(LPVOID(p1),sizeof(MODULE_INIT)) && init_module(LPMODULE_INIT(p1)))
          ret =  GKH_RET_SUCCESS;
       break;
  case MCMD_SETLANGUAGE   :
       ret = set_language(LANGID(p1)) ? GKH_RET_SUCCESS:GKH_RET_ERROR;
       break;

  case MCMD_GETMODULE_INFO:
        {
         if(check_data_size(LPVOID(p1),sizeof(MODULE_INFO)))
            {
             ret = get_module_info(LPMODULE_INFO(p1));
            }
        }
        break;

  case MCMD_GETCOMPARE_KEY:
       ret = get_compare_key();
       break;
  case MCMD_COMPARE_KEYS:
       ret = compare_keys(p1,p2);
       break;
  case MCMD_GETOWNER_HANDLE:     ret = LRESULT(owner);break;

  case MCMD_GETCONFIG_PARAMS:    if(get_config_params(LPCONFIG_PARAMS(p1))) ret = GKH_RET_SUCCESS;
       break;

  case  MCMD_GET_MODULECONFIG_DATA :
        ret =  (p1&&p2) ?
               (LRESULT)get_config_data(((LPMODULE_CONFIG_DATA)p1)->mask,((LPMODULE_CONFIG_DATA)p1)->data,DWORD(p2))
               :LRESULT(GKH_RET_ERROR);
  break;
  case  MCMD_SET_MODULECONFIG_DATA :
        ret =   do_set_config_data((LPMODULE_CONFIG_DATA)p1,p2);  break;
  case MCMD_IS_MODULE_RUNNING :  ret = is_running() ? TRUE:FALSE; break;

  case MCMD_FLUSH_CONFIG      :  lock();
                                 if(cc_flags)
                                    write_settings();
                                 ret = GKH_RET_SUCCESS;
                                 unlock();
                                 break;

  case  MCMD_NOTIFY_CODE:
        ret  = on_notify_code(LPNOTIFY_CODE(p1),DWORD(p2));
       break;

  case  MCMD_SET_NOTIFY_MASK:
        ret = set_notify_mask(GKHANDLE(p1),DWORD(p2));break;
  case  MCMD_RESET_NOTIFY_MASK:
        ret = reset_notify_mask(GKHANDLE(p1),DWORD(p2));break;
  case MCMD_GET_WINDOW_MODULE:
          ret = _get_window_module(p1,p2);
          break;
  case MCMD_GETERROR_TEXT:
        if(get_error_text(DWORD(p1),LPERROR_TEXT(p2)) )
           ret = GKH_RET_SUCCESS;
           else
           {
            if(owner)
              {
               DWORD lock_count = full_unlock();
               ret = TGKModuleInterface(owner,true).call(cmd,p1,p2);
               restore_lock(lock_count);
              } 
           }  
       break;
  case MCMD_START:
             if(!check_running())
             {

             set_module_state(MODULE_STATE_START_PENDING,true);
             LPARAM sp =  p2 ? p2: start_param;
             if(can_start(DWORD(p1),sp))
                {
                 ret = start(DWORD(p1),sp);
                }
                report_start(ret);
                set_module_state(MODULE_STATE_START_PENDING,false);
             }

        break;
  case MCMD_PAUSE:
          ret = pause(DWORD(p1),p2 ? TRUE:FALSE);
        break;
  case MCMD_STOP :
          if(check_running())
          {
          if(can_stop(DWORD(p1)))
          {
           set_module_state(MODULE_STATE_STOP_PENDING,true);
           ret = stop(DWORD(p1));
           report_stop(ret);
          }
           set_module_state(MODULE_STATE_STOP_PENDING,false);
          }
        break;
  case MCMD_LICENSE_GEN_REQUEST:
       ret = this->license_gen_request((wchar_t*)p1);
  break;
  case MCMD_LICENSE_ACTIVATE   :
        ret = license_activate((wchar_t*)p1,(DWORD)p2) ? GKH_RET_SUCCESS : GKH_RET_ERROR;
  break;
  case MCMD_LICENSE_INCTIME:
       ret = license_inc_time(p1);
  break;

  default:
         SetLastError(MWRN_UNKNOWN_MODULE_COMMAND);
 }
 return ret;
}

UINT   __fastcall TGKModule::report_reg_event_type(const char * type_name, const char * event_name, ReportDataToStrA pfn)
{
  LRESULT ret;
  call_gkhandle_bynameW(GRKERNEL_HANDLE_NAME,RCM_REGISTER_MSG_TYPE_A,(LPARAM)type_name,(LPARAM)event_name,&ret);
  if (pfn) call_gkhandle_bynameW(GRKERNEL_HANDLE_NAME,RCM_SET_REGISTER_FUNC,(LPARAM)ret,(LPARAM)pfn,0);
  return (UINT) ret;
}

UINT   __fastcall TGKModule::report_reg_event_type(const wchar_t * type_name, const wchar_t * event_name, ReportDataToStrW pfn)
{
  LRESULT ret;
  call_gkhandle_bynameW(GRKERNEL_HANDLE_NAME,RCM_REGISTER_MSG_TYPE_W,(LPARAM)type_name,(LPARAM)event_name,&ret);
  if (pfn) call_gkhandle_bynameW(GRKERNEL_HANDLE_NAME,RCM_SET_REGISTER_FUNC,(LPARAM)ret,(LPARAM)pfn,0);
  return (UINT) ret;
}

void   __fastcall TGKModule::report(DWORD cmd,LPARAM p1,LPARAM p2)
{
 call_gkhandle_bynameW(GRKERNEL_HANDLE_NAME,cmd,p1,p2,0);
}

void   __fastcall TGKModule::report(const char * s1,const char * s2)
{
 report(RCM_MESSAGE_A,LPARAM(s1),LPARAM(s2));
}

void   __fastcall TGKModule::report(const wchar_t * s1,const wchar_t * s2)
{
 report(RCM_MESSAGE_W,LPARAM(s1),LPARAM(s2));
}

void   __fastcall TGKModule::report(UINT log,int type,const char * desc)
{
  wchar_t *s=0;
  if(desc){s = new wchar_t[strlen(desc)+1]; /*KeRTL::*/Ansi2Unicode(s,desc);}
  report(log,type,s);
  if(s) delete [] s;
}

void   __fastcall TGKModule::report(UINT log,int type,const wchar_t * desc)
{
  TReportMes mes;
  mes.log=log;
  mes.source=_handle;
  mes.type=type;
  mes.desc=desc;
  report(RCM_MESSAGE_EVENT,LPARAM(&mes),0);
}

DWORD    __fastcall TGKModule::on_exception(lpexcept_record er)
{
 //Обработчик исключительных ситуаций
 except_record _er;
 CopyMemory(&_er,er,sizeof(_er));
 report(RCM_GKH_EXCEPTION,LPARAM(&_er),0);
 if(er->cmd == MCMD_GETMODULE_INFO)
   {
      resource_locker.FullUnlock();
   }
  return TGKHandleBased::on_exception(er);
}

LRESULT  __fastcall TGKModule::do_set_config_data(LPMODULE_CONFIG_DATA mcd,DWORD bsz)
{
 LRESULT ret   = GKH_RET_ERROR;
 DWORD   error = 0;
 if(check_data_size(mcd,sizeof(*mcd)))
    {
     DWORD     ch_mask = 0;BOOL restart = FALSE;
     BOOL      is_running = this->check_running();

     if(check_config_data(mcd->mask,mcd->data,bsz))
     {
     if(compare_config_data(mcd->mask,mcd->data,bsz,&ch_mask,&restart) && ch_mask)
       {
        if(ch_mask)
        {

        if(is_running && restart)
          stop(MODULE_STOP_RECONFIGURE);
          if(set_config_data(ch_mask,mcd->data,bsz))
             {
              notify(MNF_COMMON,MNCODE_MODULE_CONFIG,mcd,mcd->dw_sz);
              ret = GKH_RET_SUCCESS;
             } 
             else
             error = GetLastError();
          lock();
          cc_flags |= CCFL_MODULE_CONFIG|ch_mask;
          write_settings();
          unlock();
          if(is_running && restart && ret == GKH_RET_SUCCESS)
           start(MODULE_START_RECONFIGURE,start_param);
        }
        else
        ret = GKH_RET_SUCCESS;   
       }
      }
    }
    if(error)
       SetLastError(error);
    return ret;
}

DWORD    __fastcall TGKModule::get_module_info(LPMODULE_INFO mi)
{
  DWORD ret(GKH_RET_SUCCESS);
  if(mi->dw_size>= sizeof(*mi))
  {
   if(mi->mi_mask&MI_FL_TYPE)
    mi->type = get_type();

   if(mi->mi_mask&MI_FL_VERSION)
    {
    mi->version = get_version();
     if(_is_debug_build)
       mi->version|=0x80000000;
    }

  if(mi->mi_mask&MI_FL_COMPARE_KEY)
      mi->compare_key = get_compare_key();

   if(mi->mi_mask&MI_FL_DLL_NAME)
   {
    *mi->dll_name = 0;
    ret = GetModuleFileNameW(dll_instance,mi->dll_name,mi->dll_name_cbsz) ? ret : GKH_RET_ERROR;
   }
   if(mi->mi_mask&MI_FL_LANGDLL_NAME)
   {
    *mi->lang_dll_name = 0;
    ret = GetModuleFileNameW(lang_dll_instance,mi->lang_dll_name,mi->lang_dll_name_cbsz) ? ret : GKH_RET_ERROR;
   }

   //lock();
   if(mi->mi_mask&MI_FL_LANGID)
    mi->lang_id = current_lang;
   if(mi->mi_mask&MI_FL_MODULE_STATE)
      mi->module_state = this->module_state;

   if(mi->mi_mask&MI_FL_MODULE_TIMES)
   {
    mi->start_time.dwLowDateTime  = start_time.LowPart;
    mi->start_time.dwHighDateTime = start_time.HighPart;
    mi->stop_time.dwLowDateTime   = stop_time.LowPart;
    mi->stop_time.dwHighDateTime  = stop_time.HighPart;
   }
   if((mi->mi_mask&MI_FL_LICENSE)  )
   {
    mi->license_type = license_type;
    if(mi->reg_info_cbsz && mi->reg_info)
    {
     switch(mi->license_type)
     {
       case MOD_LICENSE_REGISTERED :
       print_rct(mi->reg_info,mi->reg_info_cbsz,IDS_COMMON_MOD_REGISTERED);
       break;
       case MOD_LICENSE_UNREGISTERED:
       print_rct(mi->reg_info,mi->reg_info_cbsz,IDS_COMMON_MOD_UNREGISTERED);
       break;
       default:
       {
         SYSTEMTIME st;
         __int64 remind_time  = license_inc_time(0);
         remind_time *=1000;
         __int64 expired_time = GetTime(false)+ MSEC_NS100(remind_time);

         FileTimeToSystemTime((LPFILETIME)&expired_time,&st);
         print_rct(mi->reg_info,mi->reg_info_cbsz,IDS_COMMON_MOD_TRIAL,(int)st.wYear,(int)st.wMonth,(int)st.wDay,(int)st.wHour,(int)st.wMinute);
       }
       break;
     }
    }
   }
   //unlock();
 lock_resource();
   if(mi->mi_mask&MI_FL_ICON)
     mi->icon = LoadIcon(lang_dll_instance,MAKEINTRESOURCE(IDI_MODULE_ICON));

   if(mi->mi_mask&MI_FL_ICONSM)
     mi->icon = LoadIcon(lang_dll_instance,MAKEINTRESOURCE(IDI_MODULE_ICONSM));

   if(mi->mi_mask&MI_FL_PROVIDER)
    {
     *mi->provider = 0;
     ret = LoadStringW(lang_dll_instance,IDS_MODULE_PROVIDER,mi->provider,mi->provider_cbsz) ? ret:GKH_RET_ERROR;
    }

   if(mi->mi_mask&MI_FL_DESCRIPTION)
   {
    *mi->description = 0;
    ret = LoadStringW(lang_dll_instance,IDS_MODULE_DESCRIPT,mi->description,mi->description_cbsz) ? ret:GKH_RET_ERROR;
   }

   if(mi->mi_mask&MI_FL_MODULE_NAME)
   {
    *mi->module_name =0;
    ret = LoadStringW(lang_dll_instance,IDS_MODULE_NAME,mi->module_name,mi->module_name_cbsz) ? ret:GKH_RET_ERROR;
   }

   unlock_resource();
   if(mi->mi_mask&MI_FL_MEMUSED)
    mi->mem_used = this->get_mem_used();

  }
  else
  {SetLastError(MERR_INVALID_BUFFER_SIZE);ret = GKH_RET_ERROR;}
  return ret;
}

bool     __fastcall TGKModule::get_config_params(LPCONFIG_PARAMS cp )
{
  bool ret(false);
  if(check_data_size(cp,sizeof(cp)))
  {
    lock();
    cp->owner   = this->owner;
    cp->reg_key = reg_key;
    ret = true;
    if(cp->config_folder && cp->config_folder_size!=0)
    {
    if(config_folder )
    {
    if( cp->config_folder_size>DWORD(lstrlenW(this->config_folder)))
       safe_strcpy(cp->config_folder,config_folder);
    }
    else
    cp->config_folder[0] = 0;   
       
    }
    if(cp->reg_key_name_size && cp->reg_key_name)
      {
       cp->reg_key_name[0] = 0;
       if(this->reg_key_name) lstrcpynW(cp->reg_key_name,reg_key_name,cp->reg_key_name_size);
      }
    unlock();
  }
  return ret;
}


bool    __fastcall TGKModule::read_settings  ()
{
 return true;
}

bool    __fastcall TGKModule::write_settings ()
{
 return true;
}

#ifdef __BORLANDC__
 #pragma warn -8057
#endif

BOOL    __fastcall TGKModule::can_start(DWORD reason,LPARAM p2)
{
 BOOL ret = TRUE;
 if(owner)
 {
 MODULE_INFO mi;
 ZeroMemory(&mi,sizeof(mi));
 mi.mi_mask  = MI_FL_MODULE_STATE;
 mi.dw_size = sizeof(mi);
 TGKModuleInterface (owner,true).get_module_info(&mi);
 license_type = check_license();
 if(license_type != (DWORD)MOD_LICENSE_UNREGISTERED )
  {
   if(!check_running())
    {
     if( (mi.module_state&(MODULE_STATE_START_PENDING|MODULE_STATE_RUNNING))==0 )
        {
         ret = false;
         SetLastError( MERR_OWNER_NOTRUNNING);
        }
     }
  }
  else
  ret = FALSE;
 }
 return ret;
}


BOOL    __fastcall TGKModule::can_stop (DWORD reason)
{

 return check_running() ? TRUE:FALSE;
}

#ifdef __BORLANDC__
 #pragma warn .8057
#endif

DWORD   __fastcall TGKModule::start(DWORD reason,LPARAM p2)
{
 GKD_TRACE("Module start begin");
 start_param = p2;
 if(reason == MODULE_START_REQUEST)
 {
	if(reg_key)
	{
	TRegsWriter rw;
	rw.SetRootKey(DupKey(reg_key));
	rw.WriteDword(MREG_VALUE_RUNNING,1,true);
	}
 }

 if(reason!=MODULE_START_RECONFIGURE)
 {
 SYSTEMTIME st;
 GetSystemTime(&st);
 lock();
  SystemTimeToFileTime(&st,(LPFILETIME)&start_time);
  this->stop_time.QuadPart = start_time.QuadPart;
 unlock();
 }
 set_module_state(MODULE_STATE_RUNNING,true);
 GKD_TRACE("Module start end");

 return 0;
}

DWORD   __fastcall TGKModule::stop (DWORD reason)
{
 if(reason == MODULE_STOP_REQUEST )
 {
   if(reg_key)
   {
    TRegsWriter rw;
    rw.SetRootKey(DupKey(reg_key));
    rw.WriteDword(MREG_VALUE_RUNNING,0,true);
   }
 if(reason!=MODULE_STOP_RECONFIGURE)
 {
  SYSTEMTIME st;
  GetSystemTime(&st);
  lock();
  SystemTimeToFileTime(&st,(LPFILETIME)&stop_time);
  unlock();
 }
 }
 set_module_state(MODULE_STATE_RUNNING|MODULE_STATE_STOP_PENDING|MODULE_STATE_PAUSED,false);
 license.close();
 return 0;

}

#ifdef __BORLANDC__
 #pragma warn -8057
#endif

DWORD   __fastcall TGKModule::pause(DWORD reason,BOOL p)
{

 return 0;
}

LRESULT __fastcall TGKModule::on_notify_code(LPNOTIFY_CODE ncd,DWORD mask)
{

  return check_data_size(ncd,sizeof(*ncd)) ;
}

#ifdef __BORLANDC__
 #pragma warn .8057
#endif


void    __fastcall TGKModule::notify(DWORD mask,DWORD code,LPVOID data,DWORD data_len)
{
  if(need_notify(mask))
  {
   BOOL          need_free = FALSE;
   BYTE          buffer[4096];
   LPNOTIFY_CODE nc;
   DWORD need_sz = sizeof(*nc)+((data && data_len> sizeof(nc->notify_data[0]))? data_len-sizeof(nc->notify_data[0]):0);
   if(need_sz<sizeof(buffer))
    nc = (LPNOTIFY_CODE)buffer;
   else
   {need_free = TRUE; nc =  (LPNOTIFY_CODE)new BYTE[need_sz]; }
   if(nc)
   {
    nc->dw_size = need_sz;
    nc->notify_code = code;
    nc->from_module = _handle;
    if(data)
        CopyMemory(nc->notify_data,data,data_len);
       else
        nc->notify_data[0] = 0;
      notify_handles(nc,mask);
      if(need_free && nc)
          delete [] (LPBYTE)nc;
   }
  }
}


void   __fastcall TGKModule::notify_handles(LPNOTIFY_CODE nc,DWORD mask)
{
  DWORD save_error = GetLastError();
  GKHANDLE nc_list[256];
  int nc_list_count     = 0;
  GKHANDLE bad_nc [256];
  int      bad_nc_count = 0;

  GKHANDLE         nc_handle;
  int count;

  if(nc)
  {
  if(nc->from_module == NULL)
	 nc->from_module = _handle;
  notify_handles_list.lock();
  count    = notify_handles_list.count();
  vector<DWORD>::iterator ptr = notify_handles_mask.begin();
  for(int i = 0;i<count;i++,ptr++)
	  {
	   if((*ptr)&mask)
		  {
           nc_handle = notify_handles_list.at(i);
           nc_list[nc_list_count++]=nc_handle;
          }
      }
  notify_handles_list.unlock();
  }

 DWORD lock_count = this->full_unlock();

 for(int i = 0;i<nc_list_count;i++)
 {

  nc_handle = nc_list[i];
  SetLastError(0);
  LRESULT ret_code = call_gkhandle(nc_handle,MCMD_NOTIFY_CODE,LPARAM(nc),mask);
  if(ret_code != GKH_RET_SUCCESS)
  {
            //При ислючении  и или невалидном GKHANDLE - удаляем из списка напоминания
  wchar_t rep_text[1024];
  wchar_t h_name  [128];
  wchar_t my_name [128];
  wchar_t templ   [128];
  wchar_t reason  [128];

  get_gkhandle_nameW(nc_handle,h_name,sizeof(h_name)/sizeof(wchar_t));
  get_gkhandle_nameW(_handle,my_name,sizeof(my_name)/sizeof(wchar_t));
  get_common_string(IDS_COMMON_NOTIFY_TEMPL,templ,sizeof(templ)/sizeof(wchar_t));
  get_common_string(ret_code == GKH_RET_EXCEPTION ? IDS_COMMON_EXCEPTION:IDS_COMMON_INVALID_GKHANDLE,reason,sizeof(reason)/sizeof(wchar_t));
  wsprintfW(rep_text,templ,my_name ,(DWORD)nc_handle,h_name,reason);
  report(rep_text,0);
   bad_nc[bad_nc_count++] = nc_handle;
  }
  }

 for(int j = 0;j<bad_nc_count;j++)
 {
  reset_notify_mask(bad_nc[j],-1);
 }

 if(save_error)
    SetLastError(save_error);
 restore_lock(lock_count);
}

bool    __fastcall TGKModule::need_notify(DWORD n_flag)
{
 bool ret ;
 notify_handles_list.lock();
 ret = (setting_notify_flags&n_flag) ? true:false;
 notify_handles_list.unlock();
 return ret;
} 

DWORD   __fastcall TGKModule::set_notify_mask  (GKHANDLE h,DWORD mask)
{
 DWORD ret = GKH_RET_SUCCESS;
 notify_handles_list.lock();
 int idx = this->notify_handles_list.index(h);
 if(idx<0)
 {
  //Еще тагого нет
  idx = notify_handles_list.add(h);
  notify_handles_mask.insert(notify_handles_mask.begin()+idx,mask);
 }
 else
   (*(notify_handles_mask.begin()+idx))|=mask;
 setting_notify_flags|=mask;
 notify_handles_list.unlock();
 return ret;
}

DWORD   __fastcall TGKModule::reset_notify_mask(GKHANDLE h,DWORD mask)
{
 //Сброс битов маски напоминания
 DWORD ret = GKH_RET_ERROR;
 notify_handles_list.lock();

 int idx = this->notify_handles_list.index(h);
 if(idx>=0)
 {
   ret = GKH_RET_SUCCESS;
   vector<DWORD>::iterator  m_ptr = notify_handles_mask.begin()+idx,end;
   (*m_ptr)&=~mask;
   if(!(*m_ptr))
   {
    notify_handles_list.remove(h);
    notify_handles_mask.erase(m_ptr);
   }
  /*Обновление общей маски*/
   m_ptr = notify_handles_mask.begin();
   end   = notify_handles_mask.end();
   setting_notify_flags = 0;
   while(m_ptr<end)
   {
    setting_notify_flags|=(*m_ptr);
    m_ptr++;
   }
 }
 notify_handles_list.unlock();
 return ret;
}






void __fastcall TGKModule::SetLastError(DWORD error)
{
 /*
  Пока заглушка.
  В дальнейшем предполагается передача ошибки МОДУЛЮ с именем хендла Reporter
 */
 ::SetLastError(error);
}


//  Values are 32 bit values layed out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+

typedef union _WIN32_ERROR_CODE
{
  DWORD error;
  struct
  {
   DWORD code:16;
   DWORD facility:12;
   DWORD reserved:1;
   DWORD customer:1;
   DWORD severety:2;
  };
}WIN32_ERROR_CODE,*LPWIN32_ERROR_CODE;

bool __fastcall TGKModule::get_error_text(DWORD error,LPERROR_TEXT et)
{
  bool ret = false;
  if(check_data_size(et,sizeof(*et)))
  {

    LPWIN32_ERROR_CODE wec = LPWIN32_ERROR_CODE(&error);
    DWORD     flags = et->args ? FORMAT_MESSAGE_ARGUMENT_ARRAY:0;
    LPCVOID   src   = NULL;
    lock_resource();
    switch(wec->facility)
    {
    case FACILITY_GKCOMMON:
      src = common_error_instance;
      flags|=FORMAT_MESSAGE_FROM_HMODULE;
      break;
      case 0:
      flags|=FORMAT_MESSAGE_FROM_SYSTEM;
      break;

      default:
      src = this->lang_dll_instance;
      flags|=FORMAT_MESSAGE_FROM_HMODULE;
    }

     ret = FormatMessageW(flags,src,error,MAKELANGID(current_lang, SUBLANG_DEFAULT),et->error_text,et->error_text_size,et->args ? &et->args:NULL) ? true:false;
     unlock_resource();
  }
  return ret;
}

bool   __fastcall TGKModule::get_error_text(DWORD error_code,wchar_t *err_text,DWORD err_text_sz,LPVOID args)
{
 ERROR_TEXT et;
 et.dw_size = sizeof(et);
 et.error_text = err_text;
 et.error_text_size = err_text_sz;
 et.args = (va_list)args;
 return get_error_text(error_code,&et);
}

DWORD  __fastcall TGKModule::get_config_item(LPMODULE_CONFIG_ITEM)
{
  return GKH_RET_ERROR;
}

DWORD  __fastcall TGKModule::set_config_item(LPMODULE_CONFIG_ITEM)
{
  return GKH_RET_ERROR;
}

LRESULT __fastcall TGKModule::_get_window_module (LPARAM p1,LPARAM p2)
{
 wchar_t * name_buf = (wchar_t*)p1;
 DWORD     nbsz     = (DWORD)   p2;
 DWORD     ret      = 0;
 if(name_buf && nbsz && !IsBadWritePtr(name_buf,nbsz))
  {
    TRegsReader rr(DupKey(reg_key));
    DWORD val_size;
    if(rr.GetValueType(MREG_VALUE_WINDOW_MODULE,NULL,&val_size,true))
    {
     if(val_size<=nbsz)
        ret = rr.ReadString(MREG_VALUE_WINDOW_MODULE,name_buf,nbsz,true);
        else
        ret = -(int)val_size;
    }
    else
    ret = (LRESULT)get_window_module(name_buf,nbsz);

  }
 return    ret;
}

 void    __fastcall TGKModule::report_start(DWORD start_ret)
 {
   DWORD error = GetLastError();
   wchar_t name[MAX_PATH];
   wchar_t rep_text[MAX_PATH<<2];
   get_lang_string(IDS_MODULE_NAME,name,sizeof(name)/sizeof(wchar_t));
   DWORD rep_len = get_common_string(start_ret ? IDS_COMMON_STARTFAIL:IDS_COMMON_STARTSUCCESS,rep_text,sizeof(rep_text)/sizeof(wchar_t));
   if(start_ret)
     {
      rep_text[rep_len++] = ' ';
      get_error_text(error,rep_text+rep_len,sizeof(rep_text)/sizeof(wchar_t)-rep_len);
     }
   report(0,start_ret ? REPORT_ERROR_TYPE:REPORT_SUCCESS_TYPE,rep_text);
   SetLastError(error);
 }

 void    __fastcall TGKModule::report_stop (DWORD stop_ret)
 {

   DWORD error = GetLastError();
   wchar_t rep_text[MAX_PATH<<2];
   DWORD rep_len = get_common_string(stop_ret ? IDS_COMMON_STOPFAIL:IDS_COMMON_STOPSUCCESS,rep_text,sizeof(rep_text)/sizeof(wchar_t));
   if(stop_ret)
     {
      rep_text[rep_len++] = ' ';
      this->get_error_text(error,rep_text+rep_len,sizeof(rep_text)/sizeof(wchar_t)-rep_len);
     }
   report(0,stop_ret ? REPORT_ERROR_TYPE:REPORT_SUCCESS_TYPE,rep_text);
   SetLastError(error);
 }

int    TGKModule::print_rct(char * result,int res_sz,DWORD str_id,... )
{
   va_list  args;
   va_start(args,str_id);
   char templ[MAX_PATH<<1];
   int  len = get_lang_string(str_id,templ,sizeof(templ));
   if(!len)
       len = get_common_string(str_id,templ,sizeof(templ));
   if(len)
   {
    templ[len] = 0;
    len = vsnprintf(result,res_sz,templ,args);
   }
   va_end(args);
   return len;

}

int    TGKModule::print_rct(wchar_t * result,int res_sz,DWORD str_id,... )
{
   va_list  args;
   va_start(args,str_id);
   wchar_t templ[MAX_PATH<<1];
   int  len = get_lang_string(str_id,templ,sizeof(templ)/sizeof(wchar_t));
   if(!len)
       len = get_common_string(str_id,templ,sizeof(templ)/sizeof(wchar_t));
   if(len)
   {
    templ[len] = 0;
    len = vsnwprintf(result,res_sz,templ,args);
   }
   va_end(args);
   return len;

}

/* Процедуры для Привязки к компьютеру */


void   __fastcall coder(wchar_t * str,int len)
{
 #pragma pack (push,1)
  union wrd
  {
    struct {BYTE a,b;} bt ;
    WORD   w;
  } ;
  #pragma pack(pop)

  wrd * ptr = (wrd*)str;
  wrd * ptr_end = ptr+len;
  while(ptr<ptr_end)
  {
    std::swap(ptr->bt.a,ptr->bt.b);
    ptr->w = ~ptr->w;
    ++ptr;
  }
}

void  __fastcall get_volume_info(wchar_t * vol_name,int len)
{
  BYTE serial_no[4] = {0};
  DWORD max_size   = sizeof(serial_no);
  GetVolumeInformationW(NULL,vol_name,len,(LPDWORD)serial_no,&max_size,NULL,NULL,0);
  swprintf(vol_name+wcslen(vol_name),L"-%02X-%02X-%02X-%02X",(DWORD)serial_no[0],(DWORD)serial_no[1],(DWORD)serial_no[2],(DWORD)serial_no[3]);
}

void  __fastcall get_memory_info(wchar_t * text,int len)
{
   MEMORYSTATUSEX ms;
   ms.dwLength = sizeof(ms);
   GlobalMemoryStatusEx(&ms);
   BYTE bt[4] = {0};
   memcpy(bt,&ms.ullTotalPhys,sizeof(bt));
   snwprintf(text,len,L"-%02X-%02X-%02X-%02X",(DWORD)bt[0],(DWORD)bt[1],(DWORD)bt[2],(DWORD)bt[3]);
}


//void __fastcall get_module_name();

static const wchar_t * crlf= L"\n\r";
void __fastcall TGKModule::get_comp_info_string(wstring & s,wchar_t * module_name,wchar_t * vol_name,wchar_t * mem_info)
{

 s = L"Module=";
 s+=module_name;
 s+=crlf;
 s+=L"Volume=";
 s+=vol_name;
 s+=crlf;
 s+=L"Memory=";
 s+=mem_info;
 if(s.length()&1)
     s+='*';
}

void __fastcall TGKModule::get_comp_info_string(wstring & s)
{
  wchar_t mod_info[MAX_PATH<<1];
  wchar_t vol_info[MAX_PATH];
  wchar_t mem_info[MAX_PATH];

  MODULE_INFO mi;
  mi.dw_size = sizeof(mi);
  mi.mi_mask = MI_FL_MODULE_NAME;
  mi.module_name_cbsz = KERTL_ARRAY_COUNT(mod_info);
  mi.module_name = mod_info;
  get_module_info(&mi);
  get_volume_info(vol_info,KERTL_ARRAY_COUNT(vol_info));
  get_memory_info(mem_info,KERTL_ARRAY_COUNT(mem_info));
  get_comp_info_string(s,mod_info,vol_info,mem_info);
}

#pragma warn -8057
LRESULT __fastcall TGKModule::license_gen_request(LPCWSTR  file_name)
{
 return  GKH_RET_ERROR;
}
#pragma warn .8057

bool __fastcall TGKModule::open_license()
{
  TLockHelper l(this->locker);
  if(!license.is_opened())
     {
      wchar_t buffer[2048];
      GetModuleFileNameW(dll_instance,buffer,KERTL_ARRAY_COUNT(buffer));
      return license.is_valid_license(buffer);
     }
  return true;
}

/*
   MOD_LICENSE_TRIAL              1
   MOD_LICENSE_REGISTERED         0
   MOD_LICENSE_UNREGISTERED      -1
*/

DWORD __fastcall TGKModule::check_license()
{
  DWORD error = MERR_UNREGISTERED_MODULE_COPY;
  DWORD ret   = MOD_LICENSE_UNREGISTERED;
  TLockHelper l(this->locker);
  if(open_license())
  {
   if(!license.is_license_active())
      {
        //Лицензия не активирована
        if(license.time_remind())
           {
            ret = MOD_LICENSE_TRIAL;
           }
           else
           error = MERR_EVAL_PERIOD_EXPIRED;
      }
      else
      {
       ret = MOD_LICENSE_REGISTERED;
      }
  }
  SetLastError(error);
  license.close();
  return ret;
}

/*
bool __fastcall TGKModule::check_valid_copy()
{

  DWORD error = MERR_UNREGISTERED_MODULE_COPY;
  wchar_t * command = L"First Time Launch*";
  wstring s;
  get_comp_info_string(s);

  wchar_t buffer[2048];
  GetModuleFileNameW(dll_instance,buffer,KERTL_ARRAY_COUNT(buffer));
  TKFName<wchar_t> key_file_name(buffer);
  key_file_name.set_ext(L".lic");
  KeRTL::TFile key_file(key_file_name(),OPEN_EXISTING);
  if(key_file.IsValid())
  {
    DWORD rd;
    memset(buffer,0,sizeof(buffer));
    key_file.Read(buffer,sizeof(buffer),rd);
    coder((wchar_t*)s.data(),s.length());
    if(rd == s.length() && !memcmp(s.data(),buffer,rd))
       {
        expired_time = MOD_LICENSE_REGISTERED;
        return true;
       }
       else
       {
        expired_time = MOD_LICENSE_UNREGISTERED;
        coder((wchar_t*)buffer,rd/sizeof(wchar_t));
        if(!memcmp((wchar_t*)buffer,command,wcslen(command)<<1))
        {
          char * ptr = (char*)memchr(buffer,'*',rd);
          if(ptr)
            {
             ptr+=2;rd+=2;
             __int64 curr_time = GetTime(false);
             __int64 * i_ptr = (__int64*)ptr ;
             if(*ptr)
             {
              __int64 diff    = NS100_MSEC(curr_time - *i_ptr);
              __int64 expire  = 30*24*3600;
                      expire*=1000;
              if(diff < expire )
                   {
                    expire-=diff;
                    expired_time = curr_time+ MSEC_NS100(expire);
                    return true;
                   }
                   else
                   {
                    expired_time = curr_time-MSEC_NS100(diff);
                    error = MERR_EVAL_PERIOD_EXPIRED;
                   }
             }
             else
             {
              *i_ptr = curr_time;
               rd+=sizeof(__int64);
               coder((wchar_t*)buffer,rd/sizeof(wchar_t));
               key_file.SeekWr(0,FILE_BEGIN);
               key_file.SeekRd(0,FILE_BEGIN);
               key_file.Write(buffer,rd);
               return true;
              }
            }
        }
        else
        error = MERR_UNREGISTERED_MODULE_COPY;
     }
  }
   else
   {
    expired_time = MOD_LICENSE_UNREGISTERED;
    #ifdef _DEBUG
    error = MERR_UNREGISTERED_MODULE_COPY;
    if(IsDebuggerPresent())
    {
     expired_time = MOD_LICENSE_UNREGISTERED;
     wcscpy ((wchar_t*)buffer,command);
     int len = wcslen((wchar_t*)buffer);
     coder((wchar_t*)buffer,len);
     key_file.Open(key_file_name(),OPEN_ALWAYS);
     key_file.Write(buffer,len*sizeof(wchar_t));
    }
    #endif
   }

  if(error )
    SetLastError(error);
  return false;
}
*/

 BOOL      __fastcall    TGKModule::license_is_active ()
 {
  TLockHelper l(this->locker);
  return license.is_license_active();
 }

 DWORD     __fastcall    TGKModule::license_get_remind()
 {
   TLockHelper l(this->locker);
   return license.time_remind();
 }

 BOOL      __fastcall    TGKModule::license_activate  (wchar_t * activation_key,DWORD len)
 {
   TLockHelper l(this->locker);
   BOOL ret = FALSE;
   if(open_license())
   {
    ret = license.activate_license(activation_key,len) ? TRUE : FALSE;
    if(ret)
      license_type = MOD_LICENSE_REGISTERED;
   }
   return ret;
 }

  DWORD   __fastcall TGKModule::license_inc_time(DWORD v)
  {
    TLockHelper l(this->locker);
    DWORD ret = 0;
    if(license_type != MOD_LICENSE_REGISTERED )
    {
     if(this->open_license())
     {
      if(license.is_license_active())
         return -1;
         else
         {
          ret = license.time_increase(v);
          if(!ret )
           {
            license_type = MOD_LICENSE_UNREGISTERED;
            stop(MODULE_STOP_REQUEST);
           }
         }
     }
    }
   license.close();
   return ret;
  }





