/*-----------------------------------------------------------------------------
 Unit Name: grkrnl32
 Author : Остапенко Д.В.
 Purpose: Исполняемый модуль ядра "Гранит-Н"
 Его задача только лишь загрузить DLL-модуль ядра.

-----------------------------------------------------------------------------*/

#include "grkrnl32_main.hpp"

#if _MSC_VER >=1200
#ifndef _DEBUG
#pragma comment(linker,"/opt:nowin98")
#endif
#endif

int     WINAPI           run_as_service(::HINSTANCE hi1,TCHAR* cmd_line);

TCHAR intro_text[] =
 _T("Granit Kernel 32-bit Version\n\r")
 _T("STDU NorilskEnergo 1993-2002\n\r")
 ;
char except_text[MAX_PATH] = {0};
const wchar_t reg_key[] =
#ifdef COMERCIAL_BUILD
L"Software\\КомПА\\ОУИК_Центр\\" ;
#else
 L"Software\\SDTU\\GRKERNEL32\\" ;
#endif

const TCHAR *commands[] =
{
 _T(":")
,_T("restart")
,_T("service")
,_T("nosplash")
,_T("config")
,_T("shutdown")
,_T("install")
,_T("delete"),_T("start"),_T("stop"),_T("instance=")
};


#pragma warn -8057
int __fastcall select_kernel_module(TCHAR * instance,bool service)
{
  HKEY rroot =  service ?  HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER;
  KeRTL::TRegsWriter rgw(rroot);
  if(rgw.OpenKey(reg_key,true))
     return rgw.WriteString(MREG_VALUE_IMAGEPATH,L"gkernel.gkm") ? 1:0;
 return -1;
}
#pragma warn .8057

DWORD grkrnl32_except_filter(LPEXCEPTION_POINTERS exp,const char * text,DWORD ret_code)
{
 LRESULT reporter_ret;
 except_record er;
 ZeroMemory(&er,sizeof(er));
 er.dw_size = sizeof  (er);

 CopyMemory(&er.except_rec,exp->ExceptionRecord,sizeof(*exp->ExceptionRecord));
 CopyMemory(&er.except_ctx,exp->ContextRecord,sizeof(*exp->ContextRecord));
 #ifdef _WIN64
 CopyMemory(er.esp_dump,(LPVOID)(er.except_ctx.R13-sizeof(er.esp_dump)),sizeof(er.esp_dump));
 CopyMemory(er.ebp_dump,(LPVOID)(er.except_ctx.R12-sizeof(er.ebp_dump)),sizeof(er.ebp_dump));
 #else
 CopyMemory(er.esp_dump,(LPVOID)(er.except_ctx.Esp-sizeof(er.esp_dump)),sizeof(er.esp_dump));
 CopyMemory(er.ebp_dump,(LPVOID)(er.except_ctx.Ebp-sizeof(er.ebp_dump)),sizeof(er.ebp_dump));
 #endif
 #ifdef UNICODE
 call_gkhandle_byname(REPORTER_NAME_W,RCM_GKH_EXCEPTION,(LPARAM)&er,(LPARAM)text,&reporter_ret);
 #else
 call_gkhandle_byname(REPORTER_NAME_A,RCM_GKH_EXCEPTION,(LPARAM)&er,(LPARAM)text,&reporter_ret);
 #endif
 return ret_code;
}

GKHANDLE __fastcall kernel_load(TGKModuleLoader & kldr, char  * instance,bool service)
{
 int len = strlen(instance)+1;
 wchar_t * text = new wchar_t[len];
 Ansi2Unicode(text,len,instance);
 GKHANDLE ret = kernel_load(kldr,text,service);
 delete [] text;
 return ret;
}

GKHANDLE __fastcall kernel_load(TGKModuleLoader & kldr,wchar_t * instance,bool service)
{
 GKHANDLE kHandle = NULL;
 wchar_t dll_name[MAX_PATH<<1];
 wchar_t key_name[MAX_PATH];
 _make_name(key_name,KERTL_ARRAY_COUNT(key_name),reg_key,instance,L'\\');
 HKEY rroot =  service ?  HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER ;

 KeRTL::TRegsRdWr rg(rroot);
 if(rg.OpenKey(key_name,true))
 {
   if(!rg.ReadString(MREG_VALUE_IMAGEPATH,dll_name,sizeof(dll_name)/sizeof(wchar_t)))
    {
     safe_strcpy(dll_name,L"gkernel.gkm");
     rg.WriteString(MREG_VALUE_IMAGEPATH,dll_name);
    }

  if(kldr.load_module(dll_name) && kldr.get_module_type() == MT_KERNEL)
  {
    kHandle = kldr.create_module();
    TGKModuleInterface iface(kHandle,true);
    dll_name[0] = 0;//Строку в которую прочитаем путь к папке для сохранения настроек
    rg.ReadString(MREG_VALUE_CONFIGFOLDER,dll_name,sizeof(dll_name)/sizeof(wchar_t));
    MODULE_INIT mi;
    mi.dw_size = sizeof(mi);
    mi.owner   = 0;
    mi.reg_key = rg();

    mi.reg_key_name      = newstr((wchar_t*)key_name);
    mi.reg_key_name_size = lstrlenW(key_name);
    mi.config_folder = 0;//dll_name;
    mi.config_folder_size = 0;// lstrlenW(dll_name);
    if(iface.module_init(&mi))
     {
      iface.set_language(LANGIDFROMLCID(GetThreadLocale()));
      iface.call(CMGK_SET_INSTANCE,(LPARAM)instance,wcslen(instance));
     }
    if(mi.reg_key_name)
       delete [] mi.reg_key_name;
  }
 }
 return   kHandle;
}


void     __fastcall kernel_stop_unload(TGKModuleLoader & kldr, GKHANDLE kernel)
{
 TGKModuleInterface iface(kernel,true);
 iface.stop(MODULE_STOP_PROGRAM_SHUTDOWN);
 close_gkhandle(kernel);
 kldr.unload_module();
}


void __fastcall get_instance(TCHAR * inst,int sz,TCHAR * cmd)
{
 if(inst && sz)
 {
   safe_strcpyn(inst,_T("DefaultInstance"),sz);
   TCHAR * ptr = safe_strstr(cmd,commands[APP_CMDLINE_INSTANCE]);
 if(ptr)
    {
     ptr+=safe_strlen(commands[APP_CMDLINE_INSTANCE]);
     TCHAR  * dest = inst;
     while(*ptr>' ' && sz>1)
        {
         *dest = *ptr;
          ++dest;++ptr;
          --sz;
        }
       *dest = 0;
	}

 }
}

#pragma warn -8057
int __fastcall run_kernel(HINSTANCE,HINSTANCE, TCHAR * cmd ,int cmd_show,bool service,bool config)
{
 bool  restart       = safe_strstr(cmd,commands[APP_CMDLINE_RESTART])  ? true:false;
 TCHAR * config_module = NULL;
 bool  shutdown      = safe_strstr(cmd,commands[APP_CMDLINE_SHUTDOWN]) || safe_strstr(cmd,commands[APP_CMDLINE_SVC_STOP]) ? true:false;
 bool  splash        = false;
 TCHAR  instance[MAX_PATH];
 #ifdef UNICODE
 get_instance(instance,KERTL_ARRAY_COUNT(svc_instance),cmd);
 #else
 get_instance(instance,KERTL_ARRAY_COUNT(instance),cmd);
 Ansi2Unicode(svc_instance,KERTL_ARRAY_COUNT(svc_instance),instance);
 #endif

 if((!service || config) && !restart && !safe_strstr(cmd,commands[APP_CMDLINE_NOSPLASH]) )
	 splash = true;

 return run_kernel (restart,service,config,config_module,instance,shutdown,splash);
}
#pragma warn .8057


int __fastcall _make_name(wchar_t * result,int res_sz,const wchar_t * prefix,const wchar_t * suffix,wchar_t split)
{
  return snwprintf(result,res_sz,L"%s%c%s",prefix,split,suffix);
}

int __fastcall _make_name(char * result,int res_sz,const char * prefix,const char * suffix,char split)
{
  return snprintf(result,res_sz,"%s%c%s",prefix,split,suffix);
}

HANDLE __fastcall  create_app_mutex(char  * instance)
{
  int len = strlen(instance)+1 ;
  wchar_t *text = new wchar_t[len];
  Ansi2Unicode(text,len,instance);
  HANDLE mut = create_app_mutex(text);
  delete [] text;
  return mut;
}

HANDLE __fastcall  create_app_mutex(wchar_t * instance)
{
  wchar_t mut_name[MAX_PATH];
  _make_name(mut_name,KERTL_ARRAY_COUNT(mut_name),GKAPP_MUTEX_NAME,instance,L'$');
  HANDLE mut = CreateMutexW(NULL,TRUE,mut_name);
  return mut;
}

HWND   __fastcall find_app_window (wchar_t    * instance)
{
 wchar_t wnd_name[MAX_PATH];
 _make_name(wnd_name,KERTL_ARRAY_COUNT(wnd_name),GKAPP_WINDOW_NAME_W,instance,L'$');
 return FindWindowW(wnd_name,NULL);

//  int len = wcslen(instance)+1 ;
//  char   * text = new char[len];
//  Unicode2Ansi(text,len,instance);
//  HWND ret = find_app_window(text);
//  delete [] text;
//  return ret;

}

HWND   __fastcall find_app_window (char * instance)
{
 char wnd_name[MAX_PATH];
 _make_name(wnd_name,KERTL_ARRAY_COUNT(wnd_name),GKAPP_WINDOW_NAME_A,instance,'$');
 return FindWindowA(wnd_name,NULL);
}

#pragma warn -8057
int __fastcall run_kernel(bool restart,bool service,bool config,TCHAR * cfg_command,TCHAR * instance,bool shutdown,bool splash)
{
 LRESULT ret_code(-1);
 DWORD LastError ;
 DWORD svc_running = 0;
 svc_running = svc_check_running()== 0 ? TRUE : FALSE;
 if(svc_running &&  config )
 {
	svc_stop();
 }

 SetLastError(0);
 HANDLE mutex = create_app_mutex(instance);

 LastError = GetLastError();
 if(mutex)
 {
 if( LastError == ERROR_ALREADY_EXISTS)
 {
  HWND app_wnd = find_app_window(instance);
  ret_code = 0;
  if(restart )
   {
    WaitForSingleObject(mutex,INFINITE);
    LastError = 0;
   }
   else
   {
   if(config && app_wnd)
     ::PostMessage(app_wnd,WM_COMMAND,CM_SETUP,0);
   if(shutdown && app_wnd)
     ::PostMessage(app_wnd,WM_COMMAND,CM_EXIT,0);
   }
 }

 if(LastError == 0 && !shutdown)
 {
 HANDLE curr_process = GetCurrentProcess();
 SetProcessPriorityBoost(curr_process,TRUE);
 #ifndef _DEBUG
 SetPriorityClass(curr_process,HIGH_PRIORITY_CLASS  /*REALTIME_PRIORITY_CLASS*/);
 #endif
  /*
   Далее предполагается анализ кода возврата
   И выполнение некоторых действий
   Например перезагрузка по новой при смене ядра
  */
 TGKModuleLoader    kldr;
 TSplashWindow    * swnd = NULL;
 GKHANDLE kHandle = kernel_load(kldr,instance,service);
 if(kHandle)
 {
 if(splash && (!service || config))
    swnd = new TSplashWindow(kHandle);
    TGKModuleInterface iface(kHandle,true);
    iface.start(MODULE_START_PROGRAM_LAUNCH,( service&&(!config)) );
   try{
        ret_code = gkapp_window(kHandle,config ? TRUE:FALSE,instance);
        //if(swnd)
          //swnd->ShowWindow(SW_SHOW);
      }
   __except(grkrnl32_except_filter(GetExceptionInformation(),except_text))
    {
     ret_code = GKH_RET_EXCEPTION;
    }
    kernel_stop_unload(kldr,kHandle);
    if(swnd)
     close_gkhandle((*(TGKHandleBased*)swnd)());
 }
 ReleaseMutex(mutex);
 }
 CloseHandle(mutex);
 }
 else
 {
   ret_code = 0;
   if(config)
     svc_config();
 }
 if(shutdown)
    ret_code = 0;
 if(svc_running)
    svc_start();
 return ret_code;
}
#pragma warn .8057

#pragma warn -8057
void __fastcall restart(int rcode,::HINSTANCE hi1,TCHAR * cmd_line)
{
      #ifdef _DEBUG
      //if(rcode == GKH_RET_EXCEPTION)
      //     DebugBreak();
      #endif
      TCHAR exe_name[MAX_PATH<<2];
      DWORD len = GetModuleFileName(hi1,exe_name,sizeof(exe_name));
      exe_name[len++] = ' ';
      exe_name[len]   = 0;
      const TCHAR * cmd_pref = _T(" -");

      TCHAR * new_cmd_line = exe_name+len;
      if(*cmd_line)
         lstrcat(new_cmd_line,cmd_line);

         if(!safe_strstr(cmd_line,commands[APP_CMDLINE_NOSPLASH]))
           {
            safe_strcat(new_cmd_line,cmd_pref);
            safe_strcat(new_cmd_line,commands[APP_CMDLINE_NOSPLASH]);
           }

         if(!safe_strstr(cmd_line,commands[APP_CMDLINE_RESTART]))
            {
            safe_strcat(new_cmd_line,cmd_pref);
            safe_strcat(new_cmd_line,commands[APP_CMDLINE_RESTART]);
            }


	  TProcess proc;
	  proc.Create(NULL,exe_name,NULL,NULL);
}

#pragma warn .8057


int __fastcall run_as_app(::HINSTANCE hi1,::HINSTANCE hi2, TCHAR * cmd_line ,int cmd_show,bool service,bool config)
{
  int ret;
  //SetThreadLocale( MAKELCID( MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT), SORT_DEFAULT) );
  do{
	 ret =  run_kernel(hi1,hi2,cmd_line,cmd_show,service,config);
	 TCHAR  instance[MAX_PATH];
	 get_instance(instance,KERTL_ARRAY_COUNT(instance),cmd_line);

	 switch (ret)
	 {
	  case  -1: cmd_show = -1;
				{
				 TCHAR instance[MAX_PATH];
				 get_instance(instance,sizeof(instance),cmd_line);
                 ret = select_kernel_module(instance,service) <= 0 ? 0:ret ;break;
                }
      case  GKH_RET_EXCEPTION:
      case  CM_APP_RESTART   :
            restart(ret,hi1,cmd_line);
            ret = 0;
            break;

     }
    }while(ret );//В случае исключения тоже завершаемся
    return ret;
}



//если в командной строке есть ключ - service то надо запускать как сервис
int WINAPI wWinMain(::HINSTANCE hi1,::HINSTANCE hi2, TCHAR * cmd_line ,int cmd_show)
{
  #ifndef _DEBUG
  /*if(IsDebuggerPresent())
     {
      LPBYTE ptr = NULL;
      for(;;)
        strcpy(ptr,"Under debugger");

     }
  */
  #endif
  int ret;
  //Установка текущего каталога
  TCHAR curr_path[MAX_PATH<<1];
  if(GetPtrModuleName(hi1,curr_path,KERTL_ARRAY_COUNT(curr_path)))
   {
     TCHAR * fn = GetFileNameFromPath(curr_path);
     if(fn)
     {
      *fn = 0;
      get_instance(fn,KERTL_ARRAY_COUNT(curr_path)-safe_strlen(curr_path),cmd_line);
      #ifdef UNICODE
      safe_strcpyn(svc_instance,fn,KERTL_ARRAY_COUNT(svc_instance));
      #else
      Ansi2Unicode(svc_instance,KERTL_ARRAY_COUNT(svc_instance),fn);
      #endif
      safe_strcat(curr_path,_T("\\"));
      ForceDirs(curr_path);
      SetCurrentDirectory(curr_path);
     }
   }
  LoadStringA(0,IDS_MAINLOOP_EXCEPT,except_text,KERTL_ARRAY_COUNT(except_text));
  CharLowerBuff(cmd_line,lstrlen(cmd_line));
  bool service = safe_strstr(cmd_line,commands[APP_CMDLINE_SERVICE]) ? true : false;
  bool config  = safe_strstr(cmd_line,commands[APP_CMDLINE_CONFIG] ) ? true : false;
  if(service && !config)
    {
     ret = run_as_service(hi1,cmd_line);
     if(ret)
     {
       char err_text[1024];
       GetErrorMessage(ret,err_text,sizeof(err_text),(HINSTANCE)NULL);
       //MessageBox(NULL,err_text,"ошибка",MB_OK);
     }
	}
    else
	 ret = run_as_app(hi1,hi2,cmd_line,cmd_show,service,config);
  return ret;
}

#pragma warn -8057
LRESULT WINAPI module_main(DWORD cmd ,LPARAM,LPARAM)
{
	return 1;
}
#pragma warn .8057




