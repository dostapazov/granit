#include <windows.h>
#include <freeimage.hpp>
#include "grkrnl32_main.hpp"
#include <KeRTLio.hpp>
#include <dll_procs.hpp>
#include <kfname.hpp>

#define GKEXPLORER_MODAL 0

 char * mod_name = NULL;
 char * mod_work = NULL;
 char * mod_stop = NULL;
 
 ULONG  TGKAppWindow::create_taskbar_msg = 0;
 TGKAppWindow::TGKAppWindow(GKHANDLE khandle)
             :
             kernel_handle(khandle)
             ,tray_menu(MAKEINTRESOURCE(IDM_TRAY_MENU))
 {
   *instance = 0;
   create_taskbar_msg = RegisterWindowMessageW(L"TaskbarCreated");
   this->mw_handle = NULL;
 }

 void __fastcall TGKAppWindow::set_instance(const char * _inst)
 {
   strcpy(this->instance,_inst);
 }

 void     __fastcall TGKAppWindow::init_tray()
 {
  ZeroMemory(&tray,sizeof(tray));
  tray.cbSize = sizeof(tray);
  tray.hWnd   = (::HWND)this->hWnd;
  tray.uID    = GKAPP_TRAY_UID;
  tray.uCallbackMessage = GKAPP_TRAY_MSG;
  tray.hIcon  = tray_icons[0];
 }

 void     __fastcall TGKAppWindow::tray_icon(HICON ic)
 {
  tray.hIcon  = ic;
  tray.uFlags = NIF_ICON;
  Shell_NotifyIcon(NIM_MODIFY,&tray);
 }

 #pragma warn -8004
 void     __fastcall TGKAppWindow::tray_text(char * text)
 {
  int len = snprintf(tray.szTip,sizeof(tray.szTip)  ,"%s:%s",instance,text);
  tray.uFlags = NIF_TIP;

  /*    len = snprintf(tray.szTip,sizeof(tray.szTip)  ,"%s",text);
      len = snprintf(tray.szInfo,sizeof(tray.szInfo),"%s",text);
  snprintf(tray.szInfoTitle,sizeof(tray.szInfoTitle),"[%s]",this->instance);

  tray.uTimeout  = 1000;
  tray.uFlags = NIF_INFO ;
  */
  Shell_NotifyIcon(NIM_MODIFY,&tray);
 }
 #pragma warn .8004


 void     __fastcall TGKAppWindow::tray_put()
 {
  tray.uFlags = NIF_ICON|NIF_MESSAGE;
  Shell_NotifyIcon(NIM_ADD,&tray);
 }

 void     __fastcall TGKAppWindow::tray_remove()
 {
  Shell_NotifyIcon(NIM_DELETE,&tray);
 }

 void     __fastcall TGKAppWindow::OnTrayTimer()
 {

  HICON icon ;
  char  text[64]  ;
  TGKModuleInterface mi(kernel_handle,true);
  DWORD state = mi.get_module_state();
  bool running = (state&MODULE_STATE_RUNNING_MASK)==MODULE_STATE_RUNNING ? true:false;
  strcpy(text,mod_name);
  if(running)
     strcat(text,mod_work);
     else
      strcat(text,mod_stop);

  if(running && (++tray_timer_counter)&1)
  {
   icon = tray_icons[1];
   if(ISGRKERNEL_STATE_WARN(state))
      {
       icon=tray_icons[2];
      }
   if(ISGRKERNEL_STATE_ERROR(state))
      icon=tray_icons[3];
  }
  else
   icon = tray_icons[0];
        tray_icon(icon);
        tray_text(text);
        

 }

 int      __fastcall TGKAppWindow::handle_tray(MSG & msg)
 {
  switch(LOWORD(msg.lParam))
  {
   case WM_RBUTTONDOWN   : show_menu  (msg.pt.x,msg.pt.y);break;
   case WM_LBUTTONDBLCLK : PostMessage(WM_COMMAND,CM_SETUP,(LPARAM)hWnd);break;
  }
  return 0;
 }
void     __fastcall TGKAppWindow::show_menu(int x,int y)
{
 POINT pt;
 pt.x = x,pt.y=y;
 SetForeground();
 BringToTop();
 SetActive();
 __try{
      //tray_menu.TrackEx(pt,0,hWnd,0);
      tray_menu.Track(pt,0,hWnd,NULL);
      }
   __except(EXCEPTION_CONTINUE_SEARCH){};

}

 bool     __fastcall TGKAppWindow::OnWMCommand(HWND From,int nCode,int id)
 {
  switch(id)
  {
   case    CM_SETUP:
          {

           if(this->mw_handle == NULL)
            {
                 if(!mw_handle || !mwl.is_loaded())
                 {
                     GetProcessWorkingSetSize(GetCurrentProcess(),&min_working_sz,&max_working_sz);
                     MODULE_WINDOW mw = {0};
                     mw.dw_size = sizeof(mw);
                     mw.mod_handle = this->kernel_handle;
                     mw.parent_window = hWnd;
                     mwl.load_module("gkexplorer.dll");
                     mw_handle = mwl.create_window(&mw,FALSE);
                 }
       __try{
				  #if GKEXPLORER_MODAL == 1
				   mwl.show_window(mw_handle,TRUE);
				   mwl.destroy_window(mw_handle);
                                   mw_handle = NULL;
				   mwl.unload_module();
				  #else
				   mwl.show_window(mw_handle,FALSE);
				  #endif

			}
       __except(grkrnl32_except_filter(GetExceptionInformation(),"Исключение окна конфигурации"))
                {
                 mwl.destroy_window(mw_handle);
                 mw_handle = 0;
                 mwl.unload_module();
                }
            }
            else
            mwl.setfocus_window(mw_handle);

          }
           break;
   case    CM_APP_RESTART:
   if(mw_handle)
      {
       mwl.destroy_window(mw_handle);
       mw_handle = 0;
      }
   EndModal(CM_APP_RESTART);
   break;

   case    CM_EXIT:
   if(mw_handle)
      {
       mwl.close_window(mw_handle);
       //mwl.destroy_window(mw_handle);
       mw_handle = 0;
       //mwl.unload_module();
      }
   EndModal(IDCANCEL);
   break;
   default : return TWindow::OnWMCommand(From,nCode,id);
  }
  return true;
 }

 bool     __fastcall TGKAppWindow::OnWMCreate(LPCREATESTRUCT ps)
 {
  if(TWindow::OnWMCreate(ps))
    {
      char str[MAX_PATH];
      ::HINSTANCE hInst = GetModuleHandle(0);
      TGKModuleInterface(this->kernel_handle,true).get_module_name(str,sizeof(str));
      mod_name  =newstr(str); 
      LoadString(hInst,IDS_WORK,str,sizeof(str));
      mod_work = newstr(str);
      LoadString(hInst,IDS_STOP,str,sizeof(str));
      mod_stop = newstr(str);
      tray_icons[0] = CopyIcon(LoadIcon(hInst,MAKEINTRESOURCE(IDI_SEM_OFF)));
      tray_icons[1] = CopyIcon(LoadIcon(hInst,MAKEINTRESOURCE(IDI_SEM_GREEN)));
      tray_icons[2] = CopyIcon(LoadIcon(hInst,MAKEINTRESOURCE(IDI_SEM_YELLOW)));
      tray_icons[3] = CopyIcon(LoadIcon(hInst,MAKEINTRESOURCE(IDI_SEM_RED)));
      init_tray();
      tray_put(),
      SetTimer((::HWND)hWnd,TRAY_TIMER_ID,333,NULL);
      return true;
    }
    return false;
 }
 void     __fastcall TGKAppWindow::OnWMDestroy()
 {
  tray_remove();
  DestroyIcon(tray_icons[3]);
  DestroyIcon(tray_icons[2]);
  DestroyIcon(tray_icons[1]);
  DestroyIcon(tray_icons[0]);
  TWindow::OnWMDestroy();
 }


 LRESULT     __fastcall TGKAppWindow::MessageHandler(MSG & msg)
 {
  if(msg.message == create_taskbar_msg)
   {tray_put();return 0;}
  if(msg.message == GKAPP_TRAY_MSG)
    return handle_tray(msg);
  if(msg.message == WM_TIMER)
    {
     if(msg.wParam == TRAY_TIMER_ID)
        {
         __try
         {
         OnTrayTimer();
         }
        __except(grkrnl32_except_filter(GetExceptionInformation(),"Exception TrayTimer Handler",EXCEPTION_CONTINUE_SEARCH))
        {}

         return 0;
       }
     return TWindow::MessageHandler(msg);
    }

  if(msg.message == WGKM_WND_HIDE)
    {
	  if(mwl.is_loaded())
	     mwl.destroy_window(mw_handle);
      mw_handle = NULL;
      return 0;
    }

  if(msg.message == WGKM_WND_DESTROYED)
    {
     mw_handle = NULL;
     SetProcessWorkingSetSize(GetCurrentProcess(),min_working_sz,min_working_sz);
     return 0;
    }


  return TWindow::MessageHandler(msg);
 }

 void     __fastcall TGKAppWindow::ProcessMessages()
 {
   if(mwl.is_loaded() && mw_handle && mwl.process_messages())
     return;

     TWindow::ProcessMessages();
 }

 int __fastcall TGKAppWindow::MessageLoop()
 {
   if(!mw_handle && mwl.is_loaded())
       mwl.unload_module();
   return TWindow::MessageLoop();
 }

int __fastcall gkapp_window(GKHANDLE kernel,BOOL show_setup,const char * instance)
{
     SetThreadLocale(MAKELCID(LANG_RUSSIAN,SUBLANG_NEUTRAL));
     int ret ;
     TGKAppWindow * wnd = new TGKAppWindow(kernel);
     wnd->set_instance(instance);
     char  caption   [MAX_PATH];
     char  wnd_class [MAX_PATH];
     _make_name(caption  ,KERTL_ARRAY_COUNT(caption  ),_T("Гранит - Н")     ,instance,_T(' '));
     _make_name(wnd_class,KERTL_ARRAY_COUNT(wnd_class),GKAPP_WINDOW_NAME,instance,_T('$'));
     TRect r(10,10,300,300);
     wnd->Create(0,caption,r,-1,0,WS_OVERLAPPEDWINDOW,WS_EX_TOOLWINDOW,wnd_class);

     wnd->ProcessMessages();
     if(show_setup)
       {
        ::HWND hwnd =(::HWND)(*wnd)();
        PostMessage(hwnd,WM_COMMAND,CM_SETUP,(LPARAM)hwnd);
       }
     ret = wnd->MessageLoop();
     wnd->Destroy();
     delete wnd;
     return ret == IDCANCEL ? 0:ret;
}



//TModule freeimg;
//
//HBITMAP __fastcall DibToDdb(FIBITMAP * fi_bmp,int width,int height)
//{
// HDC ScreenDC = 0;
// HDC MemDC    = 0;
// HBITMAP Bmp  = 0;
// if(!freeimg.GetInstance()) return NULL;
// dll_proc1<FIBITMAP *,LPBITMAPINFO> pFreeImage_GetInfo(freeimg.GetInstance(),"_FreeImage_GetInfo@4");
// dll_proc1<FIBITMAP *,char*>        pFreeImage_GetBits(freeimg.GetInstance(),"_FreeImage_GetBits@4");
// if( fi_bmp)
// {
//  ScreenDC = CreateDC("DISPLAY",0,0,0);
//  if(ScreenDC)
//  {
//   MemDC    = CreateCompatibleDC(ScreenDC);
//   if(MemDC)
//   {
//     LPBITMAPINFO bi = pFreeImage_GetInfo(fi_bmp);
//     if(!width)  width  = bi->bmiHeader.biWidth;
//     if(!height) height = bi->bmiHeader.biHeight;
//     Bmp = CreateCompatibleBitmap(ScreenDC,width,height);
//     if(Bmp)
//     {
//      SetBitmapDimensionEx(Bmp,width,height,NULL);
//      Bmp =(HBITMAP) SelectObject(MemDC,Bmp);
//      SetStretchBltMode(MemDC,COLORONCOLOR);
//      StretchDIBits(
//              MemDC,0,0,width,height,0,0,
//              bi->bmiHeader.biWidth,
//              bi->bmiHeader.biHeight,
//              pFreeImage_GetBits(fi_bmp),bi,DIB_RGB_COLORS,SRCCOPY
//              );
//      Bmp = (HBITMAP)SelectObject(MemDC,Bmp);
//     }
//   }
//  }
// }
//
// if(ScreenDC)
//    DeleteDC(ScreenDC);
// if(MemDC)
//    DeleteDC(MemDC);
// return Bmp;
//}
//
//  HBITMAP __fastcall  load_image(const wchar_t * image_name)
//  {
//    HBITMAP  ret     = NULL;
//    if( !freeimg.GetInstance()) return NULL;
//    dll_proc1<const wchar_t*,FREE_IMAGE_FORMAT>                 pFreeImage_GetFileTypeU(freeimg.GetInstance(),"_FreeImage_GetFileTypeU@8");
//    dll_proc1<FIBITMAP *>                                       pFreeImage_Unload      (freeimg.GetInstance(),"_FreeImage_Unload@4");
//    dll_proc3<FREE_IMAGE_FORMAT,const wchar_t *,int,FIBITMAP *> pFreeImage_LoadU       (freeimg.GetInstance(),"_FreeImage_LoadU@12");
//    FREE_IMAGE_FORMAT  img_fmt = pFreeImage_GetFileTypeU(image_name);
//    if(img_fmt != FIF_UNKNOWN)
//    {
//     FIBITMAP *fbmp = pFreeImage_LoadU(img_fmt,image_name,0);
//     if(fbmp)
//       {
//        ret = DibToDdb(fbmp,0,0);
//        pFreeImage_Unload(fbmp);
//       }
//    }
//    return  ret;
//  }
//
//

  void __fastcall splash_list(std::vector<std::wstring> & fvect,const wchar_t * templ)
  {
    TKFName<wchar_t> fname;
    fname.set_fullname(templ);
    WIN32_FIND_DATAW fd;
    HANDLE hFind = FindFirstFileW(templ,&fd);
    if(hFind!=INVALID_HANDLE_VALUE)
    {
     do
     {
      std::wstring str =  fname.get_path();
      str+= fd.cFileName;
      fvect.push_back(str);
     }while(FindNextFileW(hFind,&fd));
     FindClose(hFind);
    }
  }

  TSplashWindow::TSplashWindow(GKHANDLE k_handle):TWindow()
  {
   std::vector<std::wstring>  fvect;
   splash_list(fvect,L"splash*.bmp");
   splash_list(fvect,L"splash*.jpg");
   splash_list(fvect,L"splash*.jpeg");
   splash_list(fvect,L"splash*.png");
   splash_list(fvect,L"..\\splash*.bmp");
   splash_list(fvect,L"..\\splash*.jpg");
   splash_list(fvect,L"..\\splash*.jpeg");
   splash_list(fvect,L"..\\splash*.png");
   if(fvect.size())
   {
    //freeimg.Load( L"..\\freeimage32.dll");
    int    val = GetTickCount()%fvect.size();
    splash_bmp  = freeimage::TFreeImage::load_image(fvect.at(val).c_str());// LoadBitmap((::HINSTANCE)hInstance,MAKEINTRESOURCE(IBM_SPLASH+val));
    BITMAPINFO bmpi;
    GetObject(splash_bmp,sizeof(bmpi),&bmpi);
   //TSize sz(288,190);
    TSize sz(bmpi.bmiHeader.biWidth,bmpi.bmiHeader.biHeight);
    int x = (GetSystemMetrics(SM_CXSCREEN)-sz.cx)/2;
    int y = (GetSystemMetrics(SM_CYSCREEN)-sz.cy)/2;
    TRect r(x,y,sz.cx,sz.cy);
    Create(0,0,r,-1,NULL,WS_VISIBLE|WS_POPUP,WS_EX_TOOLWINDOW|WS_EX_TOPMOST,NULL);
   //r = *&TRect(0,176,288,14);
    #ifdef _WIN64
    r = TRect(0,sz.cy-16,sz.cx,14);
    #else
    r = *&TRect(0,sz.cy-16,sz.cx,14);
    #endif
    stext.DoCreate(hWnd,NULL,r,-1);
    lock_param = 1;
    alloc_gkhandle((wchar_t*)NULL);
    add_module(k_handle);
   }

  };

bool     __fastcall TSplashWindow::OnWMCreate(LPCREATESTRUCT cs)
{
 if(TWindow::OnWMCreate(cs))
  {
   TRect clr = GetClientRect();
   HRGN  region = CreateRoundRectRgn(0,0,clr.Width(),clr.Height(),30,30);
   SetWindowRgn((::HWND)hWnd,region,FALSE);
   return true;
  }
  return false;
}

LRESULT      __fastcall TSplashWindow::MessageHandler(MSG &msg)
{
 LRESULT ret = 0 ;
 switch(msg.message )
   {

    case WM_TIMER: ShowWindow(SW_HIDE);//close_gkhandle(_handle);
    break;
    default : ret = TWindow::MessageHandler(msg);break;
   }
  return ret;
}


 LRESULT  __fastcall  TSplashWindow::processing(DWORD cmd,LPARAM p1,LPARAM p2)
 {
  if(cmd == (DWORD)MCMD_NOTIFY_CODE && p2 ==(LPARAM)MNF_COMMON)
  {
   LPNOTIFY_CODE nc = (LPNOTIFY_CODE)p1;
   if(nc && nc->dw_size>= sizeof(*nc))
     {
      switch(nc->notify_code)
      {
       case MNCODE_MODULE_STATE: module_state(nc->from_module,nc->notify_data[0]);
       break;
       case MNCODE_ADDED_MODULE:
                                 add_module((GKHANDLE)nc->notify_data[0]);
       break;
       case MNCODE_REMOVED_MODULE:
                                 remove_handle((GKHANDLE)nc->notify_data[0]);
       break;
      }
     }
   return GKH_RET_SUCCESS;
  }

  return TGKHandleBased::processing(cmd,p1,p2);
 }

 #define SLEEP_TIME 100

 void     __fastcall  TSplashWindow::module_state(GKHANDLE  h,DWORD st)
 {
    TGKModuleInterface mi(h,true);
    char text[MAX_PATH<<1];
    lstrcpy(text,"Модуль - ");
    int len = lstrlen(text);
    mi.get_module_name(text+len,sizeof(text) - len);
    bool stop_pending  = (st&MODULE_STATE_STOP_PENDING)  ? true:false;
    bool start_pending = (st&MODULE_STATE_START_PENDING) ? true:false;
    bool running       = (!start_pending && !stop_pending && (st&MODULE_STATE_RUNNING))       ? true:false;
    bool stopped       = (st == 0) ? true:false;
    if(start_pending)
     lstrcat(text,"  запускается ...");
    if(stop_pending)
     lstrcat(text,"  останавливается ...");
    if(running)
     lstrcat(text,"  работает");
    if(stopped)
     lstrcat(text,"  останавлен");
    print_text(text);
    Sleep(SLEEP_TIME);
 }


 void     __fastcall  TSplashWindow::add_module(GKHANDLE  h)
 {
  if(m_list.get_interface_index(h)==(int)GKH_RET_ERROR)
   {
    TGKModuleInterface * mi = new TGKModuleInterface(h,true);
    m_list.add(mi);
    mi->set_notify(_handle,MNF_COMMON,TRUE);
    char text[MAX_PATH<<1];
    lstrcpy(text,"Модуль - ");
    int len = lstrlen(text);
    mi->get_module_name(text+len,sizeof(text) - len);
    lstrcat(text,"  загружается ...");
    print_text(text);
    Sleep(SLEEP_TIME);
   }
 }

 void     __fastcall  TSplashWindow::remove_handle(GKHANDLE  h)
 {
    TGKModuleInterface * mi = m_list.remove(h);
    if(mi)
       {
        char text[MAX_PATH<<1];
        lstrcpy(text,"Модуль - ");
        int len = lstrlen(text);
        mi->get_module_name(text+len,sizeof(text) - len);
        lstrcat(text,"  выгружен.");
        print_text(text);
        Sleep(SLEEP_TIME);
        delete mi;
       }

 }

void     __fastcall TSplashWindow::release(LPVOID)
{
  while(m_list.get_modules_count())
  {
   TGKModuleInterface * mi = m_list.remove(0);
   if(mi)
   {
    mi->set_notify(_handle,-1,FALSE);
    delete mi;
   }
  }
  this->Destroy();
}


int      __fastcall TSplashWindow::EraseBkgnd(HDC dc)
{
 TRect  r = GetClientRect();
 return Paint(dc,r,true);
}

  int      __fastcall TSplashWindow::Paint(HDC dc,RECT & r,bool)
  {
   ::HDC src_dc = CreateCompatibleDC((::HDC)dc);
   BITMAPINFO bmpi;
   GetObject(splash_bmp,sizeof(bmpi),&bmpi);
   HBITMAP old_bmp =(HBITMAP) SelectObject(src_dc,splash_bmp);
   TRect clr = GetClientRect();
   BitBlt((::HDC)dc,r.left,r.top,r.right,r.bottom,src_dc,0,0,SRCCOPY);
   //StretchBlt((::HDC)dc,r.left,r.top,r.right,r.bottom,src_dc,0,0,bmpi.bmiHeader.biWidth,bmpi.bmiHeader.biHeight,COLORONCOLOR);
   SelectObject(src_dc,old_bmp);
   DeleteDC(src_dc);
   return 0;
  }

  void     __fastcall  TSplashWindow::print_text(char * text)
  {
   KillTimer((::HWND)hWnd,100);
   char out_text[MAX_PATH<<1];
   wsprintf(out_text,"  %s",text);
   stext.SetCaption(out_text);
   SetTimer((::HWND)hWnd,100,SLEEP_TIME<<1,NULL);
  }







