#ifndef _GKAPP_WND_INC
#define _GKAPP_WND_INC

#define CM_SETUP        101
#define CM_EXIT         102
#define CM_APP_RESTART  103

#include <gkmodules.h>

#define IDM_TRAY_MENU 101

#define IDI_SEM_OFF    1
#define IDI_SEM_GREEN  2
#define IDI_SEM_YELLOW 3
#define IDI_SEM_RED    4

#define IDI_SERVICE_ICON_START   5
#define IDI_SERVICE_ICON_CONFIG  6
#define IDI_SERVICE_ICON_STOP    7

#define IBM_SPLASH     100
#define IBM_SPLASH1    101
#define IBM_SPLASH2    102
#define IBM_SPLASH3    103
#define IBM_SPLASH4    104
#define IBM_SPLASH5    105
#define IBM_SPLASH6    106
#define IBM_SPLASH7    107
#define IBM_SPLASH8    108

#define GKAPP_TRAY_MSG (WM_USER+256)
#define GKAPP_TRAY_UID  101
#define TRAY_TIMER_ID   100

#define IDS_SELKERNEL         1000
#define IDS_MAINLOOP_EXCEPT   1001
#define IDS_WORK              1002
#define IDS_STOP              1003

#define IDS_SERVICE_DISPLAY   2000



#ifndef RC_INVOKED

#include <gkmodules.hpp>
#include <tregstry.hpp>
#include <gkcomdlg.hpp>
#include <gkwind.hpp>
#include <gkwctrls.hpp>
#include <gkmenu.hpp>
#include <grkernel.h>


using namespace KeWin;
using namespace KeRTL;

DWORD grkrnl32_except_filter(LPEXCEPTION_POINTERS exp,const char *text,DWORD = EXCEPTION_EXECUTE_HANDLER);
int __fastcall gkapp_window(GKHANDLE kernel,BOOL show_setup,const TCHAR * instance);

#define GKAPP_WINDOW_NAME_A  "GKAPP_HIDDEN_WINDOW"
#define GKAPP_WINDOW_NAME_W  L"GKAPP_HIDDEN_WINDOW"
#ifdef UNICODE
#define GKAPP_WINDOW_NAME GKAPP_WINDOW_NAME_W
#else
#define GKAPP_WINDOW_NAME GKAPP_WINDOW_NAME_A
#endif
#define GKAPP_MUTEX_NAME     L"GRKRNL32_MAIN_MUTEX"

#define APP_CMDLINE_COLON       0
#define APP_CMDLINE_RESTART     1
#define APP_CMDLINE_SERVICE     2
#define APP_CMDLINE_NOSPLASH    3
#define APP_CMDLINE_CONFIG      4
#define APP_CMDLINE_SHUTDOWN    5
#define APP_CMDLINE_SVC_INSTALL 6
#define APP_CMDLINE_SVC_DELETE  7
#define APP_CMDLINE_SVC_START   8
#define APP_CMDLINE_SVC_STOP    9
#define APP_CMDLINE_INSTANCE    10


#define GKSVC_CONTROL_CONFIG    130

#define GKSVC_THREAD_MSG        (WM_USER+777)




class TSplashWindow:public TGKHandleBased,public TWindow
{
 protected:
 HBITMAP        splash_bmp;
 TGKModulesList m_list;
 TStatic        stext;
 void     __fastcall release(LPVOID);
 bool     __fastcall OnWMCreate(LPCREATESTRUCT);
 int      __fastcall EraseBkgnd(HDC dc);
 int      __fastcall Paint(HDC dc,RECT & r,bool);
 LRESULT  __fastcall MessageHandler(MSG &);
 public:
  TSplashWindow(GKHANDLE k_handle);

 LRESULT  __fastcall  processing(DWORD cmd,LPARAM p1,LPARAM p2);
 void     __fastcall  add_module  (GKHANDLE );
 void     __fastcall  module_state(GKHANDLE,DWORD st);
 void     __fastcall  remove_handle(GKHANDLE );
 void     __fastcall  print_text(TCHAR * text);
};


class TGKAppWindow:public TWindow
{
 protected:
 TCHAR instance[256];
 #ifdef _WIN64
 SIZE_T
 #else
 DWORD
 #endif
 min_working_sz,max_working_sz;

 HICON    tray_icons    [4];
 GKHANDLE kernel_handle;
 NOTIFYICONDATAW  tray;
 static   ULONG  create_taskbar_msg;
 TPopupMenu      tray_menu;
 TGKModuleWindowLoader mwl;
 LPVOID   mw_handle;
 DWORD    tray_timer_counter;
 void     __fastcall init_tray();
 void     __fastcall tray_put();
 void     __fastcall tray_remove();
 void     __fastcall tray_icon  (HICON ic);
 void     __fastcall tray_text  (wchar_t * text);
 int      __fastcall handle_tray(MSG & msg);
 bool     __fastcall OnWMCreate (LPCREATESTRUCTW);
 void     __fastcall OnWMDestroy();
 LRESULT  __fastcall MessageHandler(MSG &);
 bool     __fastcall OnWMCommand  (HWND From,int nCode,int id);
 void     __fastcall OnTrayTimer();
 void     __fastcall show_menu(int x,int y);
 public:
 TGKAppWindow(GKHANDLE khandle);
 int      __fastcall MessageLoop();
 void     __fastcall ProcessMessages();
 void     __fastcall set_instance(const TCHAR * _inst);

};

void __fastcall get_instance(char    * inst,int sz,LPSTR     cmd);
void __fastcall get_instance(wchar_t * inst,int sz,wchar_t * cmd);
int      __fastcall run_kernel (bool restart,bool service,bool config,TCHAR * cfg_command,TCHAR * instance,bool shutdown,bool splash);
GKHANDLE __fastcall kernel_load(TGKModuleLoader & kldr,wchar_t * instance,bool service);
GKHANDLE __fastcall kernel_load(TGKModuleLoader & kldr, char   * instance,bool service);
void     __fastcall kernel_stop_unload(TGKModuleLoader & kldr, GKHANDLE kernel);

int  __fastcall   svc_install(::HINSTANCE hi1 );
int  __fastcall   svc_delete ();
int  __fastcall   svc_start  ();
int  __fastcall   svc_stop   ();
int  __fastcall   svc_config ();
int    WINAPI     svc_entry  ();
int  __fastcall   svc_check_running();
void   WINAPI     GKSvcMain  (DWORD argc,wchar_t * argv[]);
DWORD  WINAPI     GKSvcThread(LPVOID);
HANDLE __fastcall create_app_mutex(wchar_t * instance);
HANDLE __fastcall create_app_mutex(char    * instance);
HWND   __fastcall find_app_window (char    * instance);
HWND   __fastcall find_app_window (wchar_t * instance);
int    __fastcall _make_name(wchar_t * result,int res_sz,const wchar_t * prefix,const wchar_t * suffix,wchar_t split = L'$');
int    __fastcall _make_name(char    * result,int res_sz,const char    * prefix,const char    * suffix,char    split = '$');
void   __fastcall get_service_name(wchar_t * buf,int sz);
extern wchar_t                  svc_instance[MAX_PATH];





#endif


#endif
