#include "grkrnl32_main.hpp"

extern TCHAR * commands[];

/******************************************************************************/
/*                              Часть сервиса                                 */
/******************************************************************************/

wchar_t service_name[] = L"grkernel_svc32";
SERVICE_STATUS_HANDLE    svc_stat_handle = NULL;
SERVICE_STATUS           svc_stat;

HANDLE                   svc_thread      = NULL;
DWORD                    svc_thread_id   = -1;
HANDLE                   svc_term_event  = NULL;
GKHANDLE                 kernel_handle   = NULL;
LPVOID                   mw_handle       = NULL;
TGKModuleWindowLoader    mwl;
wchar_t                  svc_instance[MAX_PATH];


void __fastcall get_service_name(wchar_t * buf,int sz)
{
    snwprintf(buf,sz,L"%s-%s",service_name,svc_instance);
}

int  __fastcall svc_install(::HINSTANCE hi1 )
{
  DWORD error = 0;
  SetLastError(error);
  wchar_t image[MAX_PATH<<2];
  if(GetModuleFileNameW(hi1,image,sizeof(image)/sizeof(wchar_t)))
  {
   lstrcatW(image,L" -service -instance=");
   lstrcatW(image,svc_instance);
   //Ansi2Unicode(image+wcslen(image),KERTL_ARRAY_COUNT(image)-wcslen(image),svc_instance);

   SC_HANDLE scm = OpenSCManagerW(NULL,NULL,GENERIC_WRITE);
   if(scm && INVALID_HANDLE_VALUE!=scm)
   {
     wchar_t svc_name[MAX_PATH];
     get_service_name(svc_name,KERTL_ARRAY_COUNT(svc_name));

     SC_HANDLE sch = OpenServiceW(scm,svc_name,GENERIC_ALL|SC_MANAGER_CREATE_SERVICE);

     if(!sch || INVALID_HANDLE_VALUE == sch)
     {
     wchar_t svc_dsp_name[256];
     if(!LoadStringW(hi1,IDS_SERVICE_DISPLAY,svc_dsp_name,sizeof(svc_dsp_name)/sizeof(wchar_t)))
        svc_dsp_name[0] = 0;
        lstrcatW(svc_dsp_name,L"-");
        lstrcatW(svc_dsp_name,svc_instance);
     wchar_t depend[] = L"\0\0";
                sch = CreateServiceW(scm,svc_name,
                                    svc_dsp_name,
                                    SERVICE_ALL_ACCESS,
                                    SERVICE_WIN32_OWN_PROCESS,
                                    SERVICE_AUTO_START,
                                    SERVICE_ERROR_NORMAL,
                                    image,
                                    NULL,
                                    NULL,
                                    depend,
                                    NULL,
                                    NULL
                                    );
     if(sch && INVALID_HANDLE_VALUE!=sch)
      {
       //Сервис установлен
       error = NO_ERROR;

      }
      else
      {
       error = GetLastError();
      }
    }

    CloseServiceHandle(sch);
    CloseServiceHandle(scm);
   }

  }else
    error = GetLastError();

  return error;
}


int  __fastcall svc_delete()
{
   int    error ;
   SC_HANDLE scm = OpenSCManagerW(NULL,NULL,GENERIC_WRITE|GENERIC_READ );
   if(scm && INVALID_HANDLE_VALUE!=scm)
   {
     wchar_t svc_name[MAX_PATH];
     get_service_name(svc_name,KERTL_ARRAY_COUNT(svc_name));

     SC_HANDLE sch = OpenServiceW(scm,svc_name,GENERIC_WRITE|GENERIC_EXECUTE|DELETE	);
     if(sch && INVALID_HANDLE_VALUE != sch)
     {
        error = DeleteService(sch) ? 0:GetLastError();
        CloseServiceHandle(sch);
     }else
       error = GetLastError();
       CloseServiceHandle(scm);
   }else
     error = GetLastError();

  return error;
}

int  __fastcall svc_check_running()
{
   int    error ;
   SC_HANDLE scm = OpenSCManagerW(NULL,NULL,GENERIC_WRITE|GENERIC_READ );
   if(scm && INVALID_HANDLE_VALUE!=scm)
   {
     wchar_t svc_name[MAX_PATH];
     get_service_name(svc_name,KERTL_ARRAY_COUNT(svc_name));

     SC_HANDLE sch = OpenServiceW(scm,svc_name,GENERIC_WRITE|GENERIC_EXECUTE|SERVICE_QUERY_STATUS);
     if(sch && INVALID_HANDLE_VALUE != sch)
     {
        svc_stat.dwCurrentState = -1;
             if(!QueryServiceStatus(sch,&svc_stat))
                error = GetLastError();
                else
                error =  (svc_stat.dwCurrentState == SERVICE_RUNNING || svc_stat.dwCurrentState == SERVICE_START_PENDING)  ? 0 : -1;

        CloseServiceHandle(sch);
     }else
       error = GetLastError();
       CloseServiceHandle(scm);
   }else
     error = GetLastError();
  return error;
}

int  __fastcall svc_start ()
{
   int    error ;
   SC_HANDLE scm = OpenSCManagerW(NULL,NULL,GENERIC_WRITE|GENERIC_READ );
   if(scm && INVALID_HANDLE_VALUE!=scm)
   {
     wchar_t svc_name[MAX_PATH];
     get_service_name(svc_name,KERTL_ARRAY_COUNT(svc_name));

     SC_HANDLE sch = OpenServiceW(scm,svc_name,GENERIC_WRITE|GENERIC_EXECUTE|SERVICE_QUERY_STATUS);
     if(sch && INVALID_HANDLE_VALUE != sch)
     {
        svc_stat.dwCurrentState = -1;
        error = StartService(sch,0,NULL) ? 0:GetLastError();
        while(!error && svc_stat.dwCurrentState!= SERVICE_RUNNING)
            {
            Sleep(100);
             if(!QueryServiceStatus(sch,&svc_stat))
                error = GetLastError();
            }

        CloseServiceHandle(sch);
     }else
       error = GetLastError();
       CloseServiceHandle(scm);
   }else
     error = GetLastError();
  return error;
}


int  __fastcall svc_stop  ()
{
   int    error ;
   SC_HANDLE scm = OpenSCManagerW(NULL,NULL,GENERIC_WRITE|GENERIC_READ );
   if(scm && INVALID_HANDLE_VALUE!=scm)
   {
     wchar_t svc_name[MAX_PATH];
     get_service_name(svc_name,KERTL_ARRAY_COUNT(svc_name));

     SC_HANDLE sch = OpenServiceW(scm,svc_name,GENERIC_WRITE|GENERIC_EXECUTE|SERVICE_QUERY_STATUS);
     if(sch && INVALID_HANDLE_VALUE != sch)
     {

        error = ControlService(sch,SERVICE_CONTROL_STOP,&svc_stat) ? 0:GetLastError();
        //Ожидание завершения службы
        while(!error && svc_stat.dwCurrentState!= SERVICE_STOPPED)
           {
            Sleep(100);
            if(!QueryServiceStatus(sch,&svc_stat))
                error = GetLastError();
                
          }      

        CloseServiceHandle(sch);
     }else
       error = GetLastError();
       CloseServiceHandle(scm);
   }else
     error = GetLastError();

  return error;
}

int  __fastcall svc_config()
{
   int    error = 0;

   SC_HANDLE scm = OpenSCManagerW(NULL,NULL,GENERIC_WRITE|GENERIC_READ );
   if(scm && INVALID_HANDLE_VALUE!=scm)
   {
     wchar_t svc_name[MAX_PATH];
     get_service_name(svc_name,KERTL_ARRAY_COUNT(svc_name));
     SC_HANDLE sch = OpenServiceW(scm,svc_name,GENERIC_WRITE|GENERIC_EXECUTE|SERVICE_USER_DEFINED_CONTROL|SERVICE_QUERY_STATUS);
     if(sch && INVALID_HANDLE_VALUE != sch)
     {
          while(QueryServiceStatus(sch,&svc_stat) && svc_stat.dwCurrentState == SERVICE_START_PENDING )
             Sleep(250);
           if(svc_stat.dwCurrentState == SERVICE_RUNNING)
             error = ControlService(sch,GKSVC_CONTROL_CONFIG,&svc_stat) ? 0:GetLastError();
        CloseServiceHandle(sch);
     }else
       error = GetLastError();
       CloseServiceHandle(scm);
   }else
     error = GetLastError();

  return error;
}


int WINAPI svc_entry()
{
   HANDLE curr_process = GetCurrentProcess();
   SetProcessPriorityBoost(curr_process,TRUE);
   SetPriorityClass(curr_process,REALTIME_PRIORITY_CLASS);
   int ret = -1;
   SERVICE_TABLE_ENTRYW _svc_entry[2] = {0};
   _svc_entry[0].lpServiceName = service_name;
   _svc_entry[0].lpServiceProc = GKSvcMain;

   HANDLE mutex = create_app_mutex(svc_instance);
   if(GetLastError()!=ERROR_ALREADY_EXISTS)
      {
       ret = StartServiceCtrlDispatcherW(_svc_entry);
      }
   CloseHandle(mutex);
   return ret;
}

void    __fastcall  dosvc_stop()
{
  if(mw_handle)
   {
    SendMessage((::HWND)mw_handle,WM_SYSCOMMAND,SC_CLOSE,SC_CLOSE);
    for(int i = 0;i<64 && !PostThreadMessage(svc_thread_id,WM_QUIT,0,0);)
        i++;
   }

  SetEvent(svc_term_event);
  svc_stat.dwCurrentState  = SERVICE_STOP_PENDING ;
  svc_stat.dwCheckPoint    = 1;
  svc_stat.dwWaitHint      = 2000;
  SetServiceStatus(svc_stat_handle,&svc_stat);
}

#pragma warn .8057

#pragma warn -8057
DWORD   WINAPI      GKSvcHandlerEx(DWORD c_code,DWORD,LPVOID,LPVOID context)
{
 DWORD ret = 0;
 switch(c_code)
 {

  case SERVICE_CONTROL_SHUTDOWN   :
  case SERVICE_CONTROL_STOP       :
                                   dosvc_stop();
                                   break;
  case SERVICE_CONTROL_INTERROGATE:
                                  svc_stat.dwCheckPoint    ++;
                                  SetServiceStatus(svc_stat_handle,&svc_stat);
                                  break;
  case GKSVC_CONTROL_CONFIG       :
                                   if(!mw_handle)
                                     {
                                     while(!PostThreadMessage(svc_thread_id,GKSVC_THREAD_MSG,GKSVC_CONTROL_CONFIG,0) && ret<32)
                                           ret++;
                                     }
                                     else
                                     {
                                      mwl.setfocus_window(mw_handle);
                                     }
                                  break;
 }
 return ret;
}
#pragma warn .8057

#pragma warn .8057

void   WINAPI      GKSvcHandler(DWORD c_code)
{
 GKSvcHandlerEx(c_code,0,NULL,NULL);
}


#pragma warn -8057

void    WINAPI    GKSvcMain(DWORD argc,wchar_t * argv[])
{
  // Независимость от версии
  dll_proc3 <wchar_t*,LPHANDLER_FUNCTION_EX,LPVOID,SERVICE_STATUS_HANDLE> reg_ctrl_ex;
  dll_proc2 <wchar_t*,LPHANDLER_FUNCTION,SERVICE_STATUS_HANDLE> reg_ctrl;
  if(reg_ctrl_ex.assign(GetModuleHandle(_T("ADVAPI32.DLL")),"RegisterServiceCtrlHandlerExW"))
     svc_stat_handle = reg_ctrl_ex(service_name,GKSvcHandlerEx,NULL);
     else
     {

      if(reg_ctrl.assign(GetModuleHandle(_T("ADVAPI32.DLL")),"RegisterServiceCtrlHandlerW"))
        svc_stat_handle = reg_ctrl(service_name,GKSvcHandler);
     }
  ZeroMemory(&svc_stat,sizeof(svc_stat));
  svc_stat.dwServiceType  = SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS;
  svc_stat.dwCurrentState = SERVICE_START_PENDING ;
  svc_stat.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
  svc_stat.dwWaitHint      = 2000;
  SetServiceStatus(svc_stat_handle,&svc_stat);

  dll_proc1<DWORD> allow_set_foreground_window(GetModuleHandle(_T("USER.DLL")),"AllowSetForegroundWindow");
  allow_set_foreground_window(GetCurrentProcessId());

  svc_term_event = CreateEventW (NULL,FALSE,FALSE,(wchar_t*)NULL);
  svc_thread     = CreateThread(NULL,8192,GKSvcThread,NULL,0,&svc_thread_id);

  svc_stat.dwCurrentState = SERVICE_RUNNING;
  svc_stat.dwCheckPoint    = 0;
  svc_stat.dwWaitHint      = 1000;
  SetServiceStatus(svc_stat_handle,&svc_stat);

  while(WaitForSingleObject(svc_thread,2000) == WAIT_TIMEOUT)
  {
   if(svc_stat.dwCurrentState  == SERVICE_STOP_PENDING)
   {
    //Мы перешли в режим завершения  но не завершились  за 20 сек
    if(++svc_stat.dwCheckPoint>5)
      {
       TerminateThread(svc_thread,-1);
       break;
      }
   }
  }

  svc_stat.dwCurrentState = SERVICE_STOPPED;
  SetServiceStatus(svc_stat_handle,&svc_stat);
  CloseHandle(svc_thread);
  CloseHandle(svc_term_event);
}
#pragma warn .8057


DWORD __fastcall svc_thread_msg()
{
 MSG msg;
 while(PeekMessage(&msg,NULL,0,-1,TRUE))
 {
  if(msg.message == GKSVC_THREAD_MSG && msg.wParam == GKSVC_CONTROL_CONFIG)
  {
   if(!mw_handle)
   {
    #ifdef _WIN64
    SIZE_T
    #else
    DWORD
    #endif
    min_wss,max_wss;
    GetProcessWorkingSetSize(GetCurrentProcess(),&min_wss,&max_wss);
    TGKModuleInterface    iface(kernel_handle,true);
    MODULE_WINDOW mw        = {0};
    mwl.load_module(L"gkexplorer.dll");
    mw.dw_size       = sizeof(mw);
    mw.mod_handle    = kernel_handle;
    mwl.create_window(&mw,FALSE);
    mw_handle =  mwl.create_window(&mw,FALSE);
   __try{
              //svc_stat.dwControlsAccepted &=~SERVICE_ACCEPT_STOP;
              //SetServiceStatus(svc_stat_handle,&svc_stat);
              mwl.show_window(mw_handle,TRUE);
              mwl.destroy_window(mw_handle);
              mw_handle = NULL;
              //svc_stat.dwControlsAccepted |=SERVICE_ACCEPT_STOP;
              //SetServiceStatus(svc_stat_handle,&svc_stat);
        }
   __except(grkrnl32_except_filter(GetExceptionInformation(),"Исключение окна конфигурации")) {}

            mwl.unload_module();
            mw_handle = NULL;
            SetProcessWorkingSetSize(GetCurrentProcess(),min_wss,max_wss);
   }
   else
     mwl.setfocus_window(mw_handle);
  }
 }
 return 0;
}

DWORD  WINAPI   GKSvcThread(LPVOID)
{
 MSG msg;
 BOOL do_work = TRUE;
 PeekMessage(&msg,NULL,0,-1,FALSE);
 //Загрузка и запуск ядра
 TGKModuleLoader    kldr;
 kernel_handle   = kernel_load(kldr,svc_instance,true);
 if(kernel_handle)
    {
     TGKModuleInterface iface(kernel_handle,true);
       //В данном случае при запуске 2-ой параметр == 0
       //Говорит о том, что не служба
       iface.start(MODULE_START_PROGRAM_LAUNCH,TRUE);

         while(do_work)
         {
          DWORD wait_code = MsgWaitForMultipleObjects(1,&svc_term_event,FALSE,INFINITE,QS_ALLINPUT);
          switch(wait_code)
          {
           case 0 : do_work = FALSE;break; // Завершение
           case 1 : svc_thread_msg();break;
          }
         }
      kernel_stop_unload(kldr,kernel_handle);
      kernel_handle = NULL;
   }
 return 0;
}


int WINAPI run_as_service(::HINSTANCE hi1,TCHAR * cmd_line)
{
  int   ret    = -1;
  TCHAR tmp[MAX_PATH];
  get_instance(tmp,KERTL_ARRAY_COUNT(tmp),cmd_line);
  #ifndef UNICODE
  Ansi2Unicode(svc_instance,KERTL_ARRAY_COUNT(svc_instance),tmp);
  bool  inst   = strstr(cmd_line,commands[APP_CMDLINE_SVC_INSTALL]) ? true:false;
  bool  del    = strstr(cmd_line,commands[APP_CMDLINE_SVC_DELETE ]) ? true:false;
  bool  start  = strstr(cmd_line,commands[APP_CMDLINE_SVC_START  ]) ? true:false;
  bool  stop   = strstr(cmd_line,commands[APP_CMDLINE_SVC_STOP   ]) ? true:false;
  bool  config = strstr(cmd_line,commands[APP_CMDLINE_CONFIG     ]) ? true:false;
  #else
  bool  inst   = wcsstr(cmd_line,commands[APP_CMDLINE_SVC_INSTALL]) ? true:false;
  bool  del    = wcsstr(cmd_line,commands[APP_CMDLINE_SVC_DELETE ]) ? true:false;
  bool  start  = wcsstr(cmd_line,commands[APP_CMDLINE_SVC_START  ]) ? true:false;
  bool  stop   = wcsstr(cmd_line,commands[APP_CMDLINE_SVC_STOP   ]) ? true:false;
  bool  config = wcsstr(cmd_line,commands[APP_CMDLINE_CONFIG     ]) ? true:false;
  #endif


  if(del)
  {
    ret = svc_delete();
  }
  else
  {
  if(!inst && !start && !stop && !config)
     {

      ret = svc_entry(); //Запущен из SCM
     }
     else
     {
     if(inst)
         ret = svc_install(hi1);
     if(start)
         ret = svc_start();
     if(config)
        ret = svc_config();
     if(stop)
         ret = svc_stop();
     }
  }

  return ret;
}

