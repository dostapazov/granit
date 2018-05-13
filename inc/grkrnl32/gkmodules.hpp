/***************************************************************
 *
 *
 * Описание  :Интерфейс класса реализующего модуль
 * Автор     :Остапенко Д.В.
 * Версия    :1
 * Создан    :19.06.2002 13:21:17
 *
 ****************************************************************/
#ifndef GKMODULES_INC_HPP
#define GKMODULES_INC_HPP


#ifndef  _GKHANDLE_INC_HPP
 #include <gkhandle.hpp>
#endif

 #include <gkmodules.h>
 #include <gkreporter.h>
 #include <dll_procs.hpp>
 #include <kertl.hpp>
 #include <kethreads.hpp>
 #include <vector>
 #include <stdio.h>
 #include <tlicense.hpp>


using namespace std;
using namespace KeRTL;

//array deleter for unique_ptr
template <typename T>
struct array_deleter_t
{
  void operator()(T* __ptr) {delete [] __ptr;}
};

//Базовый класс для реализации модулей

class TGKModule:public TGKHandleBased
{
#if defined _MSC_VER || __CODEGEARC__

 private:
	 static LPVOID _link_module_entry_guaranty;
#endif // __CODEGEARC__
         static BOOL   _is_debug_build;

 protected:
 GKHANDLE       owner;
 HKEY           reg_key;
 wchar_t      * reg_key_name;
 wchar_t      * config_folder;
 DWORD          cc_flags;    //Флаги наличия изменений в настройках
 DWORD          module_state;//Состояние модуля
 TGKHandlesList notify_handles_list;//Список хендлов для напоминания об изменении состояния модуля
 vector<DWORD>  notify_handles_mask;//Маски событий для напоминания
 DWORD          setting_notify_flags;//Сумма всех флагов напоминания
 LARGE_INTEGER  start_time;//Время запуска модуля
 LARGE_INTEGER  stop_time; //Время останова модуля
 LPARAM         start_param;
 TLicense       license;
 DWORD          license_type;

 virtual   DWORD   __fastcall get_version    (){return (DWORD)MAKELONG(1,1);};
 virtual   WORD    __fastcall get_type       () = 0;
 virtual   LPARAM  __fastcall get_compare_key(){return get_type();}
 virtual   int     __fastcall compare_keys(LPARAM p1,LPARAM p2){return LPARAM(int(p1)-int(p2));}

 virtual   int     __fastcall enum_components(LPMODULE_COMPONENT,int index){return -1;}

 virtual   bool    __fastcall read_settings  ();
 virtual   bool    __fastcall write_settings ();

 virtual   BOOL    __fastcall can_start(DWORD reason,LPARAM p2);
 virtual   BOOL    __fastcall can_stop (DWORD reason);
 virtual   DWORD   __fastcall start(DWORD reason,LPARAM p2);
 virtual   DWORD   __fastcall stop (DWORD reason);
 virtual   DWORD   __fastcall pause(DWORD reason,BOOL p);

 virtual   bool    __fastcall init_module  (LPMODULE_INIT mi);
 virtual   void    __fastcall release      (LPVOID);

 virtual   LRESULT __fastcall on_notify_code(LPNOTIFY_CODE,DWORD mask);
           LRESULT __fastcall processing    (DWORD cmd,LPARAM p1,LPARAM p2);
           DWORD   __fastcall on_exception  (lpexcept_record er);
 public:
           void    __fastcall notify(DWORD mask,DWORD code,LPVOID data,DWORD data_len);
           void    __fastcall notify_handles(LPNOTIFY_CODE,DWORD mask);
           bool    __fastcall need_notify(DWORD n_flag);
 protected:
           DWORD   __fastcall set_notify_mask  (GKHANDLE h,DWORD mask);
           DWORD   __fastcall reset_notify_mask(GKHANDLE h,DWORD mask);

 virtual   DWORD   __fastcall get_mem_used(){return -1;}
 virtual   bool    __fastcall get_error_text(DWORD error_code,LPERROR_TEXT et);
           LRESULT __fastcall _get_window_module (LPARAM p1,LPARAM p2);
 virtual   DWORD   __fastcall get_window_module  (wchar_t * buf,DWORD bsz){if(buf && bsz) *buf = 0;return 0;};
 virtual   DWORD   __fastcall get_config_data(DWORD mask,LPVOID buf,DWORD bsz)=0;
 virtual   bool    __fastcall set_config_data(DWORD mask,LPVOID buf,DWORD bsz)=0;
 virtual   bool    __fastcall compare_config_data(DWORD mask,LPVOID buf,DWORD bsz,LPDWORD ch_mask,BOOL * restart)=0;
 virtual   bool    __fastcall check_config_data  (DWORD mask,LPVOID buf,DWORD bsz){return true;};
           LRESULT __fastcall do_set_config_data(LPMODULE_CONFIG_DATA mcd,DWORD bsz);
 virtual   DWORD   __fastcall get_config_item(LPMODULE_CONFIG_ITEM);
 virtual   DWORD   __fastcall set_config_item(LPMODULE_CONFIG_ITEM);
 virtual   DWORD   __fastcall set_module_state(DWORD state_mask,bool set);
 virtual   DWORD   __fastcall check_license();
           void    __fastcall get_comp_info_string(wstring & s);
           void    __fastcall get_comp_info_string(wstring & s,wchar_t * module_name,wchar_t * vol_name,wchar_t * mem_info);
           LRESULT __fastcall license_gen_request(LPCWSTR  file_name);
           bool    __fastcall open_license();
 virtual   DWORD   __fastcall license_inc_time(DWORD v);
           LRESULT __fastcall module_save_registry( const wchar_t * file_name );


 public:
 TGKModule();
 static    void    __fastcall report(DWORD cmd,LPARAM p1,LPARAM p2);
 static    void    __fastcall report(const char * s1,const char * s2);
 static    void    __fastcall report(const wchar_t * s1,const wchar_t * s2);
 static    UINT    __fastcall report_reg_event_type(const char    * type_name, const char * event_name = NULL, ReportDataToStrA pfn = NULL);
 static    UINT    __fastcall report_reg_event_type(const wchar_t * type_name, const wchar_t * event_name = NULL, ReportDataToStrW pfn = NULL);

           void    __fastcall report(UINT log,int type,const char * desc);
           void    __fastcall report(UINT log,int type,const wchar_t * desc);
           void    __fastcall report_start(DWORD start_ret);
           void    __fastcall report_stop (DWORD stop_ret);
         DWORD     __fastcall is_running();
 void              __fastcall SetLastError(DWORD error);
 static int    __fastcall get_common_string(DWORD str_id,wchar_t  *buff,int bsz);
 static int    __fastcall get_common_string(DWORD str_id,char     *buff,int bsz);

 static int    __fastcall get_lang_string(DWORD str_id,wchar_t  *buff,int bsz);
 static int    __fastcall get_lang_string(DWORD str_id,char     *buff,int bsz);
 static int               print_rct(char    * result,int res_sz,DWORD str_id,... ); // Печать по шаблону из ресурсов
 static int               print_rct(wchar_t * result,int res_sz,DWORD str_id,... );

 bool   __fastcall get_error_text(DWORD error_code,wchar_t *err_text,DWORD err_text_sz,LPVOID args = NULL);
 protected:

           DWORD    __fastcall get_module_info(LPMODULE_INFO);
           bool     __fastcall get_config_params(LPCONFIG_PARAMS);

 protected:
 static     HINSTANCE          dll_instance;
 static     HINSTANCE          lang_dll_instance;
 static     LANGID             current_lang;
 static     HINSTANCE          common_error_instance;
 static     KeRTL::TFASTMutex  resource_locker;

 public   :
 bool    __fastcall  check_running();
 bool    __fastcall  check_locked();
 LRESULT __fastcall  call_owner(DWORD cmd,LPARAM p1, LPARAM p2);

 static    void __fastcall  init_instance  (HINSTANCE dll_inst);
 static    bool __fastcall  set_language   (LANGID lang);

 static    HINSTANCE __fastcall    get_module_instance(){return dll_instance;}
 static    HINSTANCE __fastcall    get_lang_instance(){return lang_dll_instance;}
 static    bool      __fastcall    lock_resource  ();
 static    void      __fastcall    unlock_resource();
           bool      __fastcall    check_data_size(LPVOID ptr,DWORD cb_sz,bool exact=false);
           bool      __fastcall    check_data_size(LPVOID ptr,DWORD ptr_size,size_t cb_sz,bool exact=false);

           BOOL      __fastcall    license_is_active ();
           DWORD     __fastcall    license_get_remind();
           BOOL      __fastcall    license_activate  (wchar_t * activation_key,DWORD len);

};


class TGKModuleInterface:public TGKHandleInterface
{
 protected:
 HINSTANCE    module_dll;
 void         __fastcall init_module_dll();
 public:
  TGKModuleInterface():module_dll(NULL)   {};
  TGKModuleInterface(const char   * name):TGKHandleInterface(name),module_dll(NULL)  {init_module_dll();};
  TGKModuleInterface(const wchar_t* name):TGKHandleInterface(name),module_dll(NULL)  {init_module_dll();};

  TGKModuleInterface(GKHANDLE     handle,bool _no_close):TGKHandleInterface(handle,_no_close),module_dll(NULL)
  {init_module_dll();}

  //Этот конструктор не делает копию хендла но загружает dll

  TGKModuleInterface(GKHANDLE     handle)
  {_handle = handle;init_module_dll();}

  virtual ~TGKModuleInterface(){close();};
  bool       __fastcall open(GKHANDLE     handle,bool no_close = false);
  bool       __fastcall open(char    * name);
  bool       __fastcall open(wchar_t * name);
  int        __fastcall close();
  bool       __fastcall get_module_info(LPMODULE_INFO mi);
  DWORD      __fastcall get_type       ();
  HICON      __fastcall get_icon       (bool sm = false);
  DWORD      __fastcall get_version    ();
  DWORD      __fastcall get_compare_key();
  bool       __fastcall get_dll_name   (wchar_t * dname,DWORD cbsz);
  GKHANDLE   __fastcall get_owner_handle();
  bool       __fastcall is_running();
  DWORD      __fastcall get_module_state();

  bool       __fastcall module_init (LPMODULE_INIT mi);
  bool       __fastcall module_init (GKHANDLE owner,HKEY reg_key,wchar_t * rk_name,wchar_t * config_folder);
  bool       __fastcall set_language(LANGID lid);

  LRESULT    __fastcall start(DWORD  reason,LPARAM p2);
  LRESULT    __fastcall stop (DWORD reason);
  LRESULT    __fastcall pause(DWORD reason,bool p);
  bool       __fastcall set_module_config_data(LPMODULE_CONFIG_DATA mcd,DWORD size){return GKH_RET_SUCCESS == call(MCMD_SET_MODULECONFIG_DATA,(LPARAM)mcd,size) ? true : false;}
  bool       __fastcall get_module_config_data(LPMODULE_CONFIG_DATA mcd,DWORD size){return GKH_RET_SUCCESS == call(MCMD_GET_MODULECONFIG_DATA,(LPARAM)mcd,size) ? true : false;}
  int        __fastcall compare(TGKModuleInterface & mi) ;
  bool       __fastcall get_config_params(LPCONFIG_PARAMS);
  bool       __fastcall get_error_text     (DWORD error,wchar_t * txt_buff,DWORD buff_sz);
  bool       __cdecl    get_error_text_args(DWORD error,wchar_t * txt_buff,DWORD buff_sz,...);
  bool       __fastcall get_module_name(wchar_t * buff,int sz);
  bool       __fastcall get_module_name(char * buff,int sz);
  bool       __fastcall set_notify(GKHANDLE notify_handle,DWORD notify_mask,BOOL set);
//  bool       __fastcall get_config_data(LPMODULE_CONFIG_DATA mcd,DWORD sz);
//  bool       __fastcall set_config_data(LPMODULE_CONFIG_DATA mcd,DWORD sz);
  void       __fastcall flush_config();
};


inline int        operator < ( TGKModuleInterface & i1,TGKModuleInterface & i2)
{   return i1.compare(i2);}

//Загрузчик модулей
class TGKModuleLoader
{
  protected:
  HINSTANCE  module_dll;
  dll_proc3<DWORD,LPARAM,LPARAM,LRESULT> entry_main;
  public:
   TGKModuleLoader ():module_dll(NULL){}
  ~TGKModuleLoader (){unload_module();}
  bool      __fastcall load_module  (const wchar_t * dll_name);
  bool      __fastcall load_module  (const char    * dll_name);
  void      __fastcall unload_module();
  HINSTANCE __fastcall get_dll_instance(){return module_dll;}
  GKHANDLE  __fastcall create_module(wchar_t * name = 0,LPARAM p = 0);
  LRESULT   __fastcall module_entry(DWORD c,LPARAM p1,LPARAM p2){return entry_main(c,p1,p2);};
  LRESULT              operator()  (DWORD c,LPARAM p1,LPARAM p2){return entry_main(c,p1,p2);};
  bool      __fastcall is_loaded(){return module_dll ? true:false;}
  bool      __fastcall is_module(){return entry_main.is_valid();}
  DWORD     __fastcall get_module_type(){return entry_main.is_valid() ? entry_main(GKME_CMD_GET_MODULETYPE,0,0) : 0;}
};

class TGKModuleWindowLoader:protected TGKModuleLoader
{
 public:
 TGKModuleWindowLoader(){};
 TGKModuleWindowLoader(char    * dll_name);
 TGKModuleWindowLoader(wchar_t * dll_name);
 ~TGKModuleWindowLoader(){unload_module();}
 #ifdef _WIN64
  using TGKModuleLoader::load_module;
  using TGKModuleLoader::unload_module;
  using TGKModuleLoader::is_loaded;
  using TGKModuleLoader::get_dll_instance;
 #else
 TGKModuleLoader::load_module;
 TGKModuleLoader::unload_module;
 TGKModuleLoader::is_loaded;
 TGKModuleLoader::get_dll_instance;
 #endif
 LPVOID   __fastcall create_window   (LPMODULE_WINDOW mw,BOOL show);
 LPVOID   __fastcall create_window   (HWND parent,GKHANDLE handle,DWORD type,BOOL modal,LPRECT area,BOOL show);
 DWORD    __fastcall destroy_window  (LPVOID wnd_handle);
 DWORD    __fastcall close_window    (LPVOID wnd_handle);
 DWORD    __fastcall show_window     (LPVOID wnd_handle,BOOL modal = TRUE);
 DWORD    __fastcall setfocus_window (LPVOID wnd_handle);
 LRESULT  __fastcall process_messages();
};


//Список модулей
class TGKModulesList
{
 protected:
 std::vector<TGKModuleInterface*> modules;
 KeRTL::TFASTMutex locker;
 public:
 bool   __fastcall lock  ();
 bool   __fastcall unlock(bool full = false);
 TGKModulesList(){};
 ~TGKModulesList(){modules.clear();}
 int                 __fastcall get_modules_count();
 TGKModuleInterface* __fastcall at    (DWORD idx);
 bool                __fastcall add   (TGKModuleInterface * i);
 bool                __fastcall remove(TGKModuleInterface * i);
 TGKModuleInterface *__fastcall remove(int idx);
 TGKModuleInterface *__fastcall remove(GKHANDLE h){return remove(get_interface_index(h));};
 int                 __fastcall get_interface_index(GKHANDLE h);
 TGKModuleInterface * operator[]( DWORD idx);
 void                __fastcall  resort();

};

//Набор модулей

class TGKModulesSet:public TGKModule
{
  protected:
   TGKModulesList     modules_list;
   virtual
   TGKModuleInterface * __fastcall get_interface(GKHANDLE h,DWORD module_type);
   virtual
   void                 __fastcall free_interface(TGKModuleInterface* mldr);
   LRESULT              __fastcall processing   (DWORD cmd,LPARAM p1,LPARAM p2);
   virtual
   bool                 __fastcall check_module_type(WORD mt) {return false;};
   void                 __fastcall release( LPVOID c );
   TGKModuleInterface*  __fastcall do_load_module(wchar_t * image_name);
   void                 __fastcall load_modules ();
   bool                 __fastcall load_module  (wchar_t   * key_name);
   bool                 __fastcall load_module  (HKEY rgk,wchar_t   * key_name);
   void                 __fastcall unload_modules();
   bool                 __fastcall init_loaded_module(TGKModuleInterface * iface,HKEY module_reg_key,wchar_t * rk_name,  wchar_t * config_folder);
   virtual
   bool                 __fastcall accept_loaded_module(TGKModuleInterface * iface,DWORD type);
   virtual
   void                 __fastcall pre_load_modules();
   void                 __fastcall notify_module_load(GKHANDLE h,DWORD mod_type,BOOL load);

  public:
   TGKModulesSet():modules_list (* new TGKModulesList){};
   int                  __fastcall get_modules_count  ();
   GKHANDLE             __fastcall enum_modules       (int idx);

   bool                 __fastcall init_module        (LPMODULE_INIT mi);
   DWORD                __fastcall start(DWORD reason,LPARAM p2);
   DWORD                __fastcall stop (DWORD reason);
   DWORD                __fastcall pause(DWORD reason,BOOL p);
   virtual
   GKHANDLE             __fastcall append_module(wchar_t * image_path,bool perm);
   virtual
   LRESULT              __fastcall replace_module(GKHANDLE prev,wchar_t * new_image);
   virtual
   bool                 __fastcall remove_module(GKHANDLE h,bool erase = false);

};


//Запускатель модуля. Использовать в module_main
template <typename ModClass>
class module_starter
{
 DWORD      module_type;
 public:
 module_starter(DWORD mt):module_type(mt){}
 LRESULT operator()(DWORD cmd,LPARAM p1,LPARAM p2);
};

#pragma warn -8057
template <typename ModClass>
LRESULT module_starter<ModClass>::operator()(DWORD cmd,LPARAM p1,LPARAM p2)
{
       LRESULT ret = NULL;
       switch(cmd)
       {
        case GKME_CMD_GET_MODULETYPE: ret = module_type;break;
        case GKME_CMD_CREATE_HANDLE :
        {
           {
            ModClass * mod = new ModClass;
            ret  = (LRESULT)(*(TGKHandleBased*)mod)();
            if(!ret)
              delete mod;
           }
        }
        break;
        case GKME_CMD_UNLOAD_MODULE: ret = TRUE; break;
       }
       return ret;
}
#pragma warn .8057


//Нить

class TGKThread:public KeRTL::TThread
{
 protected:
 bool __fastcall BeforeExecute();
 void __fastcall BeforeTerminate();
 protected:
 TGKModule * owner_module;
 TGKThread():owner_module(NULL){};
 TGKThread(TGKModule * own):owner_module(own){};
 DWORD __fastcall OnException(DWORD ExceptionCode,LPEXCEPTION_POINTERS);
 public:
 virtual const char  * __fastcall get_gkthread_name(){return "GKThread";}
};

inline bool __fastcall TGKThread::BeforeExecute()
 {

  #ifdef _DEBUG
//    TCHAR text[1024];
//    wsprintf(text,_T("Thread %s ID %X  started"),get_gkthread_name(),Id);
//    OutputDebugString(text);
  #endif
  return true;
 }

inline void __fastcall TGKThread::BeforeTerminate()
 {
  #ifdef _DEBUG
//    TCHAR  text[1024];
//    wsprintf(text,_T("Thread %s ID %X  terminated"),get_gkthread_name(),Id);
//    OutputDebugString(text);
  #endif
 }


//  Inlines  Implementations   **

inline LRESULT __fastcall  TGKModule::call_owner(DWORD cmd,LPARAM p1, LPARAM p2)
{
 LRESULT ret  = GKH_RET_ERROR;
 if(owner)
 {
//  int lc = locker.GetLockCount();
//  if(lc) locker.FullUnlock() ;
  ret = call_gkhandle(owner,cmd,p1,p2);
//  if(lc) locker.RestoreLock(lc) ;
 }
 return ret;
}


inline    bool __fastcall   TGKModule::lock_resource()
{ return resource_locker.Lock();}

inline    void __fastcall   TGKModule::unlock_resource()
{ resource_locker.Unlock();}

inline int    __fastcall TGKModule::get_common_string(DWORD str_id,wchar_t *buff,int bsz)
{
 lock_resource();
 int ret = 0;
 if(common_error_instance && buff && bsz)
   ret = LoadStringW(common_error_instance,str_id,buff,bsz);
 unlock_resource();
 return ret;
}

inline int    __fastcall TGKModule::get_common_string(DWORD str_id,char *buff,int bsz)
{
 lock_resource();
 int ret = LoadStringA(common_error_instance,str_id,buff,bsz);
 unlock_resource();
 return ret;
}

inline int    __fastcall TGKModule::get_lang_string(DWORD str_id,wchar_t *buff,int bsz)
{
 lock_resource();
 int ret = LoadStringW(lang_dll_instance,str_id,buff,bsz);
 unlock_resource();
 buff[ret] = 0;
 return ret;
}

inline int    __fastcall TGKModule::get_lang_string(DWORD str_id,char *buff,int bsz)
{
 lock_resource();
 int ret = LoadStringA(lang_dll_instance,str_id,buff,bsz);
 unlock_resource();
 buff[ret] = 0;
 return ret;
}


inline  TGKModule::TGKModule():
        owner(NULL),reg_key(NULL),config_folder(NULL),module_state(0),
        cc_flags(0),setting_notify_flags(0),reg_key_name(0),start_param(0)
        {
         start_time.QuadPart = 0;stop_time.QuadPart = 0;
         #ifdef _DEBUG
         _is_debug_build = TRUE;
         #endif
        };

inline bool    __fastcall  TGKModule::check_locked()
{
 return GetCurrentThreadId() == get_owner_threadid(_handle) ? true:false;
}



inline bool       __fastcall TGKModuleInterface::open(char    * name)
{
 return TGKHandleInterface::open(name);
}

inline bool       __fastcall TGKModuleInterface::open(wchar_t * name)
{
 return TGKHandleInterface::open(name);
}


inline bool       __fastcall TGKModuleInterface::open(GKHANDLE     handle,bool _no_close)
{
 if(TGKHandleInterface::open(handle,_no_close))
   {
    if(!_no_close)
        init_module_dll();
     return true;   
   }
  return false; 
}

inline int        __fastcall    TGKModuleInterface::close()
{
 int ret = 0;
 ret = TGKHandleInterface::close();
 if(!no_close && module_dll && module_dll!=GetModuleHandle(0))
 {
        wchar_t mod_name[2048];
	stop(MODULE_STOP_RELEASE);
	HINSTANCE saved_module = module_dll;
	module_dll = NULL;
    __try{
          if(saved_module)
              {
               GetModuleFileNameW(saved_module,mod_name,KERTL_ARRAY_COUNT(mod_name));
	       FreeLibrary(saved_module);
              }
     }
     __finally{
         ret = 0;
     }
 }

 return ret;
}

inline void       __fastcall TGKModuleInterface::init_module_dll()
{
 if(no_close)
    module_dll = NULL;
 else
 {
 wchar_t     dll_name[MAX_PATH<<2];
 MODULE_INFO mi;
 mi.dw_size      = sizeof(mi);
 mi.mi_mask      = MI_FL_DLL_NAME;
 mi.dll_name_cbsz=sizeof(dll_name)/sizeof(wchar_t);
 mi.dll_name     = dll_name;
 *dll_name       = 0;
 get_module_info(&mi);
 module_dll = LoadLibraryW(dll_name);
 }
}

inline   bool       __fastcall TGKModuleInterface::set_notify(GKHANDLE notify_handle,DWORD notify_mask,BOOL set)
{
 return call(set ? MCMD_SET_NOTIFY_MASK:MCMD_RESET_NOTIFY_MASK,(LPARAM)notify_handle,notify_mask) ? false:true;
}

inline   bool       __fastcall TGKModuleInterface::get_module_info(LPMODULE_INFO mi)
{ return call(MCMD_GETMODULE_INFO,LPARAM(mi),0) == GKH_RET_SUCCESS ? true:false;}

inline   bool       __fastcall TGKModuleInterface::is_running()
{ return call(MCMD_IS_MODULE_RUNNING,0,0) ? true:false; }


inline   DWORD      __fastcall TGKModuleInterface::get_type       ()
{
 MODULE_INFO mi;
 mi.dw_size = sizeof(mi);
 mi.mi_mask = MI_FL_TYPE;
 return call(MCMD_GETMODULE_INFO,LPARAM(&mi),0) == GKH_RET_SUCCESS ?mi.type:GKH_RET_ERROR;
}

inline   DWORD      __fastcall TGKModuleInterface::get_module_state()
{

 MODULE_INFO mi;
 mi.dw_size = sizeof(mi);
 mi.mi_mask = MI_FL_MODULE_STATE;
 mi.module_state = 0;
 call(MCMD_GETMODULE_INFO,LPARAM(&mi),0) ;
 return mi.module_state;
}

inline   HICON      __fastcall TGKModuleInterface::get_icon       (bool sm)
{
 MODULE_INFO mi;
 mi.dw_size = sizeof(mi);
 mi.mi_mask = (sm) ? MI_FL_ICONSM:MI_FL_ICON;
 return call(MCMD_GETMODULE_INFO,LPARAM(&mi),0) == GKH_RET_SUCCESS ?mi.icon:NULL;
}

inline   DWORD      __fastcall TGKModuleInterface::get_version    ()
{
 MODULE_INFO mi;
 mi.dw_size = sizeof(mi);
 mi.mi_mask = MI_FL_VERSION;
 return call(MCMD_GETMODULE_INFO,LPARAM(&mi),0) == GKH_RET_SUCCESS ?mi.version:GKH_RET_ERROR;
}

inline   DWORD      __fastcall TGKModuleInterface::get_compare_key()
{
 MODULE_INFO mi;
 mi.dw_size = sizeof(mi);
 mi.mi_mask = MI_FL_COMPARE_KEY;
 return call(MCMD_GETMODULE_INFO,LPARAM(&mi),0) == GKH_RET_SUCCESS ?mi.compare_key:GKH_RET_ERROR;
}
inline   bool       __fastcall TGKModuleInterface::get_dll_name   (wchar_t * dname,DWORD cbsz)
{
 MODULE_INFO mi;
 mi.dw_size = sizeof(mi);
 mi.mi_mask = MI_FL_DLL_NAME;
 mi.dll_name      = dname;
 mi.dll_name_cbsz = cbsz;
 return call(MCMD_GETMODULE_INFO,LPARAM(&mi),0) == GKH_RET_SUCCESS ?true:false;
}

inline bool       __fastcall TGKModuleInterface::get_module_name(wchar_t * buff,int sz)
{
 MODULE_INFO mi;
 mi.dw_size = sizeof(mi);
 mi.mi_mask = MI_FL_MODULE_NAME;
 mi.module_name   = buff;
 mi.module_name_cbsz = sz;
 return call(MCMD_GETMODULE_INFO,LPARAM(&mi),0) == GKH_RET_SUCCESS ?true:false;
}

inline bool       __fastcall TGKModuleInterface::get_module_name(char * buff,int sz)
{
 bool ret;
 MODULE_INFO mi;
 mi.dw_size = sizeof(mi);
 mi.mi_mask = MI_FL_MODULE_NAME;
 mi.module_name   = new wchar_t [sz];
 mi.module_name_cbsz = sz;
 ret = (mi.module_name && call(MCMD_GETMODULE_INFO,LPARAM(&mi),0) == GKH_RET_SUCCESS) ? true:false;
 if(ret)
    KeRTL::Unicode2Ansi(buff,mi.module_name);
 if(mi.module_name)
    delete [] mi.module_name;
 return ret;   
}


inline   GKHANDLE   __fastcall TGKModuleInterface::get_owner_handle()
{ return GKHANDLE(call(MCMD_GETOWNER_HANDLE,0,0));}

inline   bool       __fastcall TGKModuleInterface::module_init (LPMODULE_INIT mi)
{
 return call(MCMD_MODULE_INIT,LPARAM(mi),0) == GKH_RET_SUCCESS ? true:false;
}

inline bool       __fastcall TGKModuleInterface::get_config_params(LPCONFIG_PARAMS cp)
{
 return call(MCMD_GETCONFIG_PARAMS,LPARAM(cp),0) == GKH_RET_SUCCESS ? true:false;
}


inline   bool       __fastcall TGKModuleInterface::set_language(LANGID lid)
{
 return call(MCMD_SETLANGUAGE,LPARAM(lid),0) == GKH_RET_SUCCESS ? true:false;
}

inline   LRESULT    __fastcall TGKModuleInterface::start(DWORD reason,LPARAM p2)
{
  return call(MCMD_START,LPARAM(reason),p2) ;
}

inline   LRESULT    __fastcall TGKModuleInterface::stop (DWORD reason)
{
  return call(MCMD_STOP,reason,0) ;
}
inline   LRESULT    __fastcall TGKModuleInterface::pause(DWORD reason,bool p)
{
  return call(MCMD_PAUSE,reason,LPARAM(p)) ;
}

inline  int        __fastcall TGKModuleInterface::compare( TGKModuleInterface & i)
{
 MODULE_INFO mi1,mi2;
 mi1.dw_size = mi2.dw_size = sizeof(MODULE_INFO);
 mi1.mi_mask    = mi2.mi_mask    = MI_FL_TYPE|MI_FL_COMPARE_KEY;
 get_module_info(&mi1);
 i.get_module_info(&mi2);
 if(mi1.type == mi2.type)
  return int(call(MCMD_COMPARE_KEYS,mi1.compare_key,mi2.compare_key));
 return  int(mi1.type) - int(mi2.type);
}

inline bool       __fastcall TGKModuleInterface::module_init (GKHANDLE owner,HKEY reg_key,wchar_t * rk_name,wchar_t * config_folder)
{
 MODULE_INIT mi;
 mi.dw_size = sizeof(mi);
 mi.owner = owner;mi.reg_key = reg_key;mi.config_folder = config_folder;
 mi.config_folder_size = config_folder ? lstrlenW(config_folder):0;
 mi.reg_key_name_size  = rk_name ?       lstrlenW(rk_name):0;
 mi.reg_key_name       = rk_name;
 return module_init(&mi);
}


inline bool       __fastcall TGKModuleInterface::get_error_text(DWORD error,wchar_t * txt_buff,DWORD buff_sz)
{
 ERROR_TEXT et;
 et.dw_size = sizeof(et);et.error_text = txt_buff;et.error_text_size = buff_sz;et.args = 0;
 return call(MCMD_GETERROR_TEXT,error,LPARAM(&et)) == GKH_RET_SUCCESS? true:false;
}

inline bool       __cdecl  TGKModuleInterface::get_error_text_args(DWORD error,wchar_t * txt_buff,DWORD buff_sz,...)
{
 ERROR_TEXT et;
 et.dw_size = sizeof(et);et.error_text = txt_buff;et.error_text_size = buff_sz;
 va_start(et.args,buff_sz);
  bool ret = call(MCMD_GETERROR_TEXT,error,LPARAM(&et))== GKH_RET_SUCCESS ? true:false;
 va_end(et.args);
 return ret;
}

//inline bool __fastcall TGKModuleInterface::get_config_data(LPMODULE_CONFIG_DATA mcd,DWORD sz)
//{
// return call(MCMD_GET_MODULECONFIG_DATA,(LPARAM)mcd,sz) == GKH_RET_SUCCESS ? true:false;
//}
//
//inline bool __fastcall TGKModuleInterface::set_config_data(LPMODULE_CONFIG_DATA mcd,DWORD sz)
//{
// return call(MCMD_SET_MODULECONFIG_DATA,(LPARAM)mcd,sz) == GKH_RET_SUCCESS ? true:false;
//}

inline  void       __fastcall TGKModuleInterface::flush_config()
{ call(MCMD_FLUSH_CONFIG,0,0);}



inline  bool   __fastcall TGKModulesList::lock  ()
{ return locker.Lock();}

inline  bool   __fastcall TGKModulesList::unlock(bool full)
{return locker.Unlock(full);}

inline  int                 __fastcall TGKModulesList::get_modules_count()
{ int ret;lock();ret = modules.size();unlock();return ret;}

inline  TGKModuleInterface* __fastcall TGKModulesList::at    (DWORD idx)
{
 TGKModuleInterface * ret ;
 lock();
  ret = idx<modules.size()? modules.at(idx):NULL;
 unlock();
 return ret;
}

inline TGKModuleInterface *__fastcall TGKModulesList::remove(int idx)
{
 TGKModuleInterface * ret = NULL;
 lock();
 if(idx>=0 && idx<(int)modules.size())
   { ret = at(idx);modules.erase(modules.begin()+idx);}

 unlock();
 return ret;
}

inline bool         __fastcall TGKModulesList::remove(TGKModuleInterface * i)
{
 bool ret = false;
  lock();
  if(i)
  {
   ret = remove(get_interface_index((*i)())) ? true:false;
  }
  unlock();
 return ret;
}


inline  TGKModuleInterface * TGKModulesList::operator[](DWORD idx)
{
 return at(idx);
}


inline    int             __fastcall TGKModulesSet::get_modules_count()
{    return modules_list.get_modules_count();   }

inline    GKHANDLE        __fastcall TGKModulesSet::enum_modules (int idx)
{
     TGKModuleInterface * ir = modules_list[idx];
     return ir ? (*ir)():NULL;
}


inline TGKModuleWindowLoader::TGKModuleWindowLoader(char    * dll_name)
 {
  load_module(dll_name);
 }

inline TGKModuleWindowLoader::TGKModuleWindowLoader(wchar_t * dll_name)
 {
  load_module(dll_name);
 }


inline LRESULT  __fastcall TGKModuleWindowLoader::process_messages()
{
 return (LRESULT) entry_main(MWCM_PROCESS_MESSAGES,0,0);
}

inline  LPVOID   __fastcall TGKModuleWindowLoader::create_window (LPMODULE_WINDOW mw,BOOL show)
 {
  return (LPVOID) entry_main(MWCM_CREATE_WINDOW,LPARAM(mw),show);
 }

inline   LPVOID   __fastcall TGKModuleWindowLoader::create_window (HWND parent,GKHANDLE handle,DWORD type,BOOL modal,LPRECT area, BOOL show)
 {
  MODULE_WINDOW mw;
  mw.dw_size = sizeof(mw);
  mw.window_type = type;
  mw.parent_window = parent;
  if(area)  CopyMemory(&mw.region,area,sizeof(*area));
  else ZeroMemory(&mw.region,sizeof(mw.region));
  mw.modeless = modal ? FALSE:TRUE;
  mw.mod_handle = handle;
  return create_window(&mw,show);
 }

inline   DWORD    __fastcall TGKModuleWindowLoader::destroy_window(LPVOID wnd_handle)
 {
  return (DWORD) entry_main(MWCM_DESTROY_WINDOW,LPARAM(wnd_handle),0);
 }
inline   DWORD    __fastcall TGKModuleWindowLoader::close_window(LPVOID wnd_handle)
 {
  return (DWORD) entry_main(MWCM_CLOSE_WINDOW,LPARAM(wnd_handle),0);
 }

inline   DWORD    __fastcall TGKModuleWindowLoader::show_window (LPVOID wnd_handle,BOOL modal )
{
 return (DWORD) entry_main(MWCM_SHOW_WINDOW,LPARAM(wnd_handle),modal);
}

inline   DWORD    __fastcall TGKModuleWindowLoader::setfocus_window (LPVOID wnd_handle)
{
 return (DWORD) entry_main(MWCM_SETFOCUS_WINDOW,LPARAM(wnd_handle),0);
}

inline DWORD __fastcall TGKThread::OnException(DWORD ExceptionCode,LPEXCEPTION_POINTERS exp)
{
  TGKModule::report(RCM_EXCEPTION,(LPARAM)exp,(LPARAM)get_gkthread_name());
 #ifndef __BORLANDC__
  return TThread::OnException(ExceptionCode,exp);
 #else 
  return KeRTL::TThread::OnException(ExceptionCode,exp);
 #endif 
}


#endif


