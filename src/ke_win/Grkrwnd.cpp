//---------------------------------------------------------------------------

#include <kertlio.hpp>
#include <gkwind.hpp>
#include <geometry.hpp>
#include <stdio.h>
#pragma hdrstop
//---------------------------------------------------------------------------

#ifndef _NO_NAMESPACE
namespace KeWin
{
#endif

     TWindow * __fastcall TWindow::CAST(LPVOID ptr)
     {
      TWindow * wnd = reinterpret_cast<TWindow *>(ptr);
      if(ptr && !IsBadReadPtr(&wnd->SecretDword,sizeof(DWORD)) && wnd->SecretDword == SECRET_DWORD)
         return wnd;
         return 0;
     }


     TWindow::TWindow(HINSTANCE hInst)
     {
      //Конструктор
      hInstance = hInst; hWnd           = 0;
      bkBrush   = 0;     ClientDC       = 0;
      Font      = 0;     SuperclassProc = 0;
      EndMessageLoopCode = 0;
      Flags = 0;
      UD = 0;
      SecretDword = SECRET_DWORD;
      Accelerator = NULL;
      dbl_dc      = NULL;
      dbl_bmp     = NULL;
      this->wnd_size.cx = wnd_size.cy = 0;
      #ifndef _NATIVE_W2K
      static bool w2k = InitW2KSupport();
      #endif
     }


     bool   __fastcall TWindow::CanClose(int)
     {return true;}


    LRESULT CALLBACK   TWindow::DefaultProc(HWND hWnd,UINT Msg,WPARAM wp,LPARAM lp)
    {
	  LRESULT ret;
          LPVOID par;
	  #ifdef _WIN64
	   par = (Msg == WM_CREATE || Msg == WM_NCCREATE || Msg == WM_INITDIALOG)? ((LPCREATESTRUCT)lp)->lpCreateParams : LPVOID(::GetWindowLongPtr((::HWND)hWnd,GWLP_USERDATA));
	  #else
	   par = (Msg == WM_CREATE || Msg == WM_NCCREATE || Msg == WM_INITDIALOG)? ((LPCREATESTRUCT)lp)->lpCreateParams : LPVOID(::GetWindowLong((::HWND)hWnd,GWL_USERDATA));
	  #endif
      TWindow * wnd = CAST(par);
      if(wnd == 0)
         {
          // Для обработки сообщений приходящих до создания окна
          // в перед Create вызывается SetMessageExtraInfo
          wnd = CAST((LPVOID)GetMessageExtraInfo());
          if(wnd && wnd->hWnd==0)
             {
              wnd->hWnd = hWnd;
              wnd->ReplaceUserData(LONG(wnd));

             }
         }
      if(wnd && ( IsWindow((::HWND)wnd->hWnd) || wnd->hWnd == NULL))
        {
         MSG  &wmsg = wnd->CurrentMsg;
         wmsg.hwnd = (::HWND)hWnd;
         wmsg.message = Msg;
         wmsg.wParam  = wp;
         wmsg.lParam  = lp;
         wmsg.time    = GetTickCount();
         GetCursorPos(&wmsg.pt);
         ret = LRESULT(wnd->MessageHandler(wmsg));
        }
        else
        ret = (LRESULT)CallWindowProc((WNDPROC)DefWindowProc,(::HWND)hWnd,Msg,wp,lp);
        if(InSendMessage())
           ReplyMessage(ret);
        return ret;
    }

    WNDPROC __fastcall TWindow::GetDefWndProc()
    {
     return (WNDPROC)DefaultProc;
    }


   bool __fastcall TWindow::RegisterWindow(HINSTANCE hi)
   {
    WNDCLASSEX wc;
    GetWndClass(wc);

    //wc.style |=CS_GLOBALCLASS	;
    wc.lpszClassName = GetClassName();
    wc.lpfnWndProc   = this->GetDefWndProc();
    wc.hInstance = (::HINSTANCE)hi;
    SetLastError(0);
    ATOM reg_result  = RegisterClassEx(&wc);
    DWORD LastError = GetLastError();
    return (reg_result || LastError == 0 || LastError == ERROR_CLASS_ALREADY_EXISTS) ? true:false; 
   }



    LRESULT   __fastcall TWindow::DefaultProcessing()
    {
    int ret;
    if(UD)
    this->ReplaceUserData(UD);
    #ifdef __BORLANDC__
     if(SuperclassProc)
       ret = CallWindowProc(SuperclassProc,CurrentMsg.hwnd,CurrentMsg.message,CurrentMsg.wParam,CurrentMsg.lParam);
       else
       ret = CallWindowProc(DefWindowProc,CurrentMsg.hwnd,CurrentMsg.message,CurrentMsg.wParam,CurrentMsg.lParam);
    #else
     if(SuperclassProc)
       ret = CallWindowProc((WNDPROC)SuperclassProc,CurrentMsg.hwnd,CurrentMsg.message,CurrentMsg.wParam,CurrentMsg.lParam);
       else
       ret = CallWindowProc((WNDPROC)DefWindowProc,CurrentMsg.hwnd,CurrentMsg.message,CurrentMsg.wParam,CurrentMsg.lParam);
     #endif
    if(UD)
     this->ReplaceUserData(UD);
     return ret;
    }

	LRESULT     __fastcall TWindow::PreProcessMessage(MSG & /*msg*/)
	{

	  return 0;
	}

	LRESULT     __fastcall TWindow::PostProcessMessage(MSG & /*msg*/)
	{
	 return 0;
	}
//#include <winuser.h>
	LRESULT   __fastcall TWindow::MessageHandler(MSG & msg)
    {
      //Процудра обработки сообщений окна
      switch(msg.message)
      {
       case WM_CREATE     :
                            {
                             DefaultProcessing();
                             LPCREATESTRUCT crst = LPCREATESTRUCT(msg.lParam);
                             this->hWnd = msg.hwnd;
                             UD = SetWindowLong(GWL_USERDATA,LONG(this));
                             if(GetClassLong((::HWND)hWnd,GCL_STYLE)&CS_OWNDC)
                                ClientDC = GetDC((::HWND)hWnd);
                             SetFont((HFONT)GetStockObject(ANSI_VAR_FONT),false);
                             return OnWMCreate (crst) ? 0 : -1;
                            }

       case WM_ERASEBKGND : return this->EraseBkgnd(HDC(msg.wParam));
       case WM_PAINT      :
                          {
                           //Отрисовка окна

                           bool _gdc(ClientDC == 0);
                           PAINTSTRUCT ps;
                           BeginPaint((::HWND)hWnd,&ps);
                           if(_gdc)
                              ClientDC = ps.hdc;
                           on_paint(ps.hdc,ps.rcPaint,bool(ps.fErase ? true:false));
                           EndPaint((::HWND)hWnd,&ps);
                            if(_gdc)
                              ClientDC = 0;
                          }

                          return 0;

       case WM_DESTROY    :
                            DeleteObject(GetFont());
                            if(dbl_dc)
                               {
                               release_dbl_buffered();
                               }

		            if(ClientDC)
                               {
                                ReleaseDC((::HWND)hWnd,(::HDC)ClientDC);
                                ClientDC = NULL;
                               }
                            OnWMDestroy();
                            break;


       case WM_SETCURSOR  : if(OnWMSetCursor(int(LOWORD(msg.lParam)),int(HIWORD(msg.lParam))))
                                return 1;
                                break;
       case WM_SETFOCUS   : if(OnWMSetFocus(HWND(msg.wParam)))
                               return  0;
                               break;
       case WM_KILLFOCUS  : if(OnWMKillFocus(HWND(msg.wParam)))
                               return  0;
                               break;


       case WM_STYLECHANGING:
       case WM_STYLECHANGED : this->OnWMStyleChange(msg.wParam == (WPARAM)GWL_EXSTYLE ? true:false,msg.message == WM_STYLECHANGED ? true:false,(LPSTYLESTRUCT)msg.lParam) ;
                              return -1;

       case WM_COMMAND    : if(OnWMCommand(HWND(msg.lParam),int(HIWORD(msg.wParam)),int(LOWORD(msg.wParam))))
                               return 0;
                            break;
       case WM_SYSCOMMAND : if(OnWMSysCommand(DWORD(msg.wParam),int(LOWORD(msg.lParam)),int(HIWORD(msg.lParam))))
                               return 0;
                            break;
       case WM_SYSKEYDOWN : DefaultProcessing();
		                    return 0;

       case WM_NOTIFY     :
		                   if(msg.lParam)
		                   return this->OnWMNotify( DWORD(msg.wParam),LPNMHDR(msg.lParam));
						   DefaultProcessing();
						   return 0;


       case WM_DRAWITEM   :
           return int(OnDrawItem(int(msg.wParam),LPDRAWITEMSTRUCT(msg.lParam)));
       case WM_MEASUREITEM:
           return int(OnMeasureItem(int(msg.wParam),LPMEASUREITEMSTRUCT(msg.lParam)));


       case WM_SETFONT    : OnWMSetFont(HFONT(msg.wParam),bool(msg.lParam ? true:false));break;
       case WM_GETFONT    : return int(OnWMGetFont());

       case WM_INITMENU        :
       case WM_INITMENUPOPUP   :
                                 return !OnWMInitMenu(HMENU(msg.wParam));
       case WM_NCCALCSIZE      : if(msg.wParam)
                                 return OnWMNCCalcSize((LPNCCALCSIZE_PARAMS)msg.lParam);
                                 else
                                 OnWMCalcSize((LPRECT)msg.lParam);
                                 return 0;

       case WM_NCCREATE        :
       case WM_NCDESTROY       :
       case WM_NCHITTEST       :
       case WM_NCPAINT         :
       case WM_NCACTIVATE      :
                               return DefaultProcessing();

       case WM_SHOWWINDOW : OnWMShow(bool(msg.wParam ? true:false),int(msg.lParam));
                            return this->DefaultProcessing();
       case WM_CLOSE      : msg.wParam = IDCANCEL;
       case WM_QUIT       :
                           if(CanClose(int(msg.wParam)))
                             EndModal( int(msg.wParam ? msg.wParam : IDCANCEL));
                           return 0;
       case WM_SIZE       :
		           on_size(int(msg.wParam),int(LOWORD(msg.lParam)),int(HIWORD(msg.lParam)));
                           return 0;
                            //break;
       case WM_MOVING     :
       case WM_SIZING     :
                          {
                           TRect wr;
                           GetWindowRect(&wr);
                           TRect &dr = *(TRect*)msg.lParam;
                           if(dr!=wr)
                             return (msg.message ==  WM_SIZING ?
                                     OnSizing(int(msg.wParam),wr,dr):
                                     OnMoving(int(msg.wParam),wr,dr)
                                     ) ? TRUE:FALSE;
                          }
                          break;
       case WM_MOVE:
                    OnMove(int(LOWORD(msg.lParam)),int(HIWORD(msg.lParam)));
                    break;
       case WM_GETMINMAXINFO :
                              DefaultProcessing();
                              OnWMGetMinmaxInfo(LPMINMAXINFO(msg.lParam));
                              return 0;
       case WM_NEXTDLGCTL: return OnWMNextDlgCtl(HWND(msg.wParam),LOWORD(msg.lParam) ? true:false);
       case WM_GETDLGCODE: return 0;

       case WM_CTLCOLORMSGBOX:
       case WM_CTLCOLOREDIT:
       case WM_CTLCOLORLISTBOX:
       case WM_CTLCOLORBTN:
       case WM_CTLCOLORDLG:
       case WM_CTLCOLORSCROLLBAR:
       case WM_CTLCOLORSTATIC:
             return int(this->OnCtlColor(HWND(msg.lParam),HDC(msg.wParam),msg.message-WM_CTLCOLORMSGBOX));
       case WM_KEYDOWN:
       case WM_KEYUP  :
       case WM_CHAR   :
                        return DefaultProcessing();
       case WM_CHILDACTIVATE    :
       case WM_WINDOWPOSCHANGING:
       case WM_WINDOWPOSCHANGED :
                        return DefaultProcessing();

                                 //return ;


       default: NotHandledMsg(msg);

      }

      return DefaultProcessing();
    }

void    __fastcall TWindow::NotHandledMsg(MSG &)
{
/*     static KeRTL::TFile f("msg.log");
	 char text[51];
     f.Write(text,wsprintf(text,"%X\n",CurrentMsg.message));
*/
}


#ifdef __BORLANDC__
    #pragma warn -8057
#endif

    DWORD        __fastcall TWindow::GetStyle(bool Ex )
    {
       if(hWnd == 0)
       return 0;
       else
       return GetWindowLong(Ex ? GWL_EXSTYLE:GWL_STYLE);
    }


    void __fastcall TWindow::Create
    ( HWND Parent,const char * Caption,RECT & r,int Id,
      HMENU Menu,DWORD Style,DWORD ExStyle,const char * ClassName)

    {
     //Создание окна
     WNDCLASSEX wc;
     ZeroMemory(&wc,sizeof(wc));


     //wc.hInstance = KeRTL::TSysInfo::IsWinNT() ?  0 : KeRTL::GetPtrModuleHandle(wc.lpfnWndProc);
     wc.cbSize = sizeof(wc);
     if(ClassName)
       {

        if(!GetClassInfoEx(0,ClassName,&wc))
           {
            GetWndClass(wc);
            wc.lpszClassName = ClassName;
           }
       }
     else
     GetWndClass(wc);
     if(!Style)
        Style = GetStyle();
     if(!ExStyle)
       ExStyle = GetStyle(true);
     SetLastError(0);
     ATOM RegResult = RegisterClassEx(&wc);
     DWORD LastError = GetLastError();
     if(RegResult || LastError == 0 || LastError == ERROR_CLASS_ALREADY_EXISTS)
      {
       // Класс успешно зарегистрирован или был зарегистрирован ранее
        int x,y,w,h;
        w = r.right  - r.left;
        h = r.bottom - r.top;
        x = r.left;
        y = r.top;

        if(w==0)
           w = CW_USEDEFAULT;
        if(h==0)
           h = CW_USEDEFAULT;
        if(w == CW_USEDEFAULT && h == CW_USEDEFAULT && x == 0)
           x = CW_USEDEFAULT;

        if(w == CW_USEDEFAULT && h == CW_USEDEFAULT && y == 0)
           y = CW_USEDEFAULT;

        SetMessageExtraInfo(LPARAM(this));
        hWnd = (HWND)CreateWindowEx(ExStyle,wc.lpszClassName,Caption,(Style&(~WS_VISIBLE)),
                              x,y,w,h,
                              (::HWND)Parent,(::HMENU)Menu,(::HINSTANCE)hInstance,this);
        SetMessageExtraInfo(0);
       if(hWnd)
        {
          SetWindowLong(GWL_ID,Id);
          if(GetWindowLong(GWL_USERDATA)!= LONG(this))
             UD = SetWindowLong(GWL_USERDATA,LONG(this));

          if(this->SuperclassProc == NULL)
            {
             if(GetWindowLong(GWL_WNDPROC)!=LONG(DefaultProc))
              SuperclassProc = (WNDPROC)SetWindowLong(GWL_WNDPROC,LONG(DefaultProc));

            }
          if(wc.style & CS_OWNDC)  //Если OWNDC
             ClientDC = GetDC((::HWND)hWnd);

             LONG wndStyle = GetWindowLong(GWL_STYLE);
             if(wndStyle != LONG(Style&~WS_VISIBLE))
                SetWindowLong(GWL_STYLE,Style);
             if(Style & WS_VISIBLE)
             ShowWindow(SW_SHOW);

		}
        else
        GetLastError();
      }
    }

#ifdef __BORLANDC__    
  #pragma warn .8057
#endif



    const char * __fastcall TWindow::GetClassName()
    {
     if(hWnd)
     {
      WNDCLASSEX  wc;
      TWindow::GetWndClass(wc);
      return wc.lpszClassName;
     }
     //const char *(__fastcall TWindow::*proc)() = &TWindow::GetClassName;
     static char class_name[32];
     sprintf(class_name,"KrnlWin::TWindow#%08X",(int)class_name);
     return class_name;
    }

    void         __fastcall TWindow::GetWndClass(WNDCLASSEXA & wc )
    {
       ZeroMemory(&wc,sizeof(wc));
       wc.cbSize        = sizeof(wc);
       if(hWnd)
       {
        char name [MAX_PATH];
        ::GetClassName((::HWND)hWnd,name,sizeof(name));
        GetClassInfoExA(0,name,&wc);
        return ;
       }

       wc.cbWndExtra    = 0;
       wc.style = CS_BYTEALIGNWINDOW|CS_BYTEALIGNCLIENT|CS_DBLCLKS|CS_VREDRAW|CS_HREDRAW|CS_OWNDC;
       wc.lpfnWndProc   = GetDefWndProc();
       //wc.hInstance     = KeRTL::GetPtrModuleHandle(wc.lpfnWndProc);
       wc.lpszClassName = GetClassName();
       wc.hCursor       = LoadCursor(0,MAKEINTRESOURCE(IDC_ARROW));
       wc.hIcon         = LoadIcon  (0,MAKEINTRESOURCE(IDI_APPLICATION));
       wc.hIconSm       = LoadIcon  (0,MAKEINTRESOURCE(IDI_APPLICATION));
       wc.hbrBackground = HBRUSH(COLOR_BTNFACE+1);
    }

//    void         __fastcall TWindow::GetWndClass(WNDCLASSEXW & wc )
//    {
//       ZeroMemory(&wc,sizeof(wc));
//       wc.cbSize        = sizeof(wc);
//       if(hWnd)
//       {
//        wchar_t name [MAX_PATH];
//        ::GetClassNameW((::HWND)hWnd,name,sizeof(name));
//        GetClassInfoExW(0,name,&wc);
//        return ;
//       }
//
//       wc.cbWndExtra    = 0;
//       wc.style = CS_BYTEALIGNWINDOW|CS_BYTEALIGNCLIENT|CS_DBLCLKS|CS_VREDRAW|CS_HREDRAW|CS_OWNDC;
//       wc.lpfnWndProc   = GetDefWndProc();
//       //wc.hInstance     = KeRTL::GetPtrModuleHandle(wc.lpfnWndProc);
//       wchar_t class_name [1024];
//       Ansi2Unicode(class_name,KERTL_ARRAY_COUNT(class_name),GetClassName());
//       wc.lpszClassName = class_name;
//       wc.hCursor       = LoadCursor(0,MAKEINTRESOURCE(IDC_ARROW));
//       wc.hIcon         = LoadIcon  (0,MAKEINTRESOURCE(IDI_APPLICATION));
//       wc.hIconSm       = LoadIcon  (0,MAKEINTRESOURCE(IDI_APPLICATION));
//       wc.hbrBackground = HBRUSH(COLOR_BTNFACE+1);
//    }
//

     bool   __fastcall TWindow::Destroy()
     {
      if((this->Flags& FLWND_ALIAS) == 0)
      {
      char class_name[MAX_PATH];
      ::GetClassName((::HWND)hWnd,class_name,sizeof(class_name));
      bool ret ( DestroyWindow((::HWND)hWnd) ? true:false);
      if(ret)
        {
         hWnd = 0;
         UnregisterClass(class_name,0);
        }
        return ret;
      }
      else
       hWnd = 0;
      return true;  
     }

     bool    __fastcall TWindow::TranslateAccelerator(MSG & msg)
     {
 #ifdef _WIN64
        TWindow * w = CAST((LPVOID)::GetWindowLongPtr(GetActiveWindow(),GWL_USERDATA));
 #else
        TWindow * w = CAST((LPVOID)::GetWindowLong(GetActiveWindow(),GWL_USERDATA));
 #endif
	 if(w && w->Accelerator)
	 {
	  return ::TranslateAccelerator((::HWND)w->hWnd,w->Accelerator,&msg) ? true:false;
	 }
		 return false;
     }

     bool    __fastcall TWindow::GetMessage(MSG & msg)
     {
      return PeekMessage(&msg,0,0,0,PM_REMOVE)? true:false;
     }

     void __fastcall TWindow::ProcessMessages()
     {
      MSG msg;
      while(GetMessage(msg) && EndMessageLoopCode==0)
      {

        if(Flags & FLWND_PREPROCESSMSG)
		  PreProcessMessage(msg);
#ifdef _WIN64
		 TWindow * w = CAST((LPVOID)::GetWindowLongPtr(msg.hwnd,GWL_USERDATA));
#else
		 TWindow * w = CAST((LPVOID)::GetWindowLong(msg.hwnd,GWL_USERDATA));
#endif
		 if(w!=this && w && (w->IsFlagsSet(FLWND_PREPROCESSMSG)))
			 w->PreProcessMessage(msg);



	  if(IsDialogMessage(GetActiveWindow(),&msg)==FALSE && IsDialogMessage(0,&msg)==FALSE && TranslateAccelerator(msg) == false)
          {

	   TranslateMessage(&msg);
           if(msg.message == WM_QUIT )
              msg.hwnd = (::HWND)hWnd;
           DispatchMessage(&msg);
          }
          else
          {
           ZeroMemory(&msg,sizeof(msg));
          }
        if(Flags & FLWND_POSTPROCESSMSG)
		  PostProcessMessage(msg);

		  if(w!=this && w && (w->IsFlagsSet(FLWND_POSTPROCESSMSG)))
			 w->PostProcessMessage(msg);

      }

     }

     int    __fastcall TWindow::MessageLoop()
     {
      this->EndMessageLoopCode = 0;
      while(EndMessageLoopCode==0 && hWnd)
      {

          if(WaitMessage())
             ProcessMessages();

      }
      return hWnd ? EndMessageLoopCode :-1;
     }

    int    __fastcall TWindow::ShowModal()
    {
     HWND parent = GetParent();
     EnableWindow((::HWND)parent,false);
     ShowWindow(SW_SHOW);
     EndMessageLoopCode = MessageLoop();
     EnableWindow((::HWND)parent,true);
     return EndMessageLoopCode;
    }


    int          __fastcall TWindow::EraseBkgnd(HDC dc)
    {
       if(bkBrush)
       {
        RECT r;
        GetClientRect(&r);
        if(IsFlagsSet(FLWND_DOUBLE_BUFFERED) && dbl_dc)
        {
         FillRect(dbl_dc,&r,bkBrush);
        }

        FillRect((::HDC)dc,&r,bkBrush);
        return (true);
       }
       return DefaultProcessing();
    }


    HDC          __fastcall TWindow::get_dbl_dc()
    {
      if(!dbl_dc)
      {
         bool release_cleint_dc = ClientDC == NULL ? true:false;
         if(release_cleint_dc)
            ClientDC = GetDC(hWnd);
            dbl_dc = CreateCompatibleDC(ClientDC);
           if(release_cleint_dc)
           {
              ReleaseDC(hWnd,ClientDC);
              ClientDC = 0;
           }
      }
      return dbl_dc;
    }

    void          __fastcall TWindow::release_dbl_buffered()
    {
      if(dbl_dc)
      {
        if(dbl_bmp)
        {
         dbl_bmp = (HBITMAP)SelectObject(dbl_dc,dbl_bmp);
         DeleteObject(dbl_bmp);
         dbl_bmp = NULL;
        }
        DeleteDC(dbl_dc);
        dbl_dc = NULL;
      }
    }

    void          __fastcall TWindow::create_dbl_buffered()
    {
       get_dbl_dc();
       HDC scr_dc = CreateDC("DISPLAY",0,0,0);
       HBITMAP bmp   = CreateCompatibleBitmap(scr_dc,wnd_size.cx,wnd_size.cy);
               bmp   = (HBITMAP)SelectObject(dbl_dc,bmp);
       if(dbl_bmp)
          DeleteObject(bmp);
          else
          dbl_bmp = bmp;
       DeleteDC    (scr_dc);
    }


    void         __fastcall TWindow::on_size(int how,int cx,int cy)
    {
     if(wnd_size.cx != cx || wnd_size.cy != cy )
     {
      wnd_size.cx = cx ; wnd_size.cy = cy;

      if(IsFlagsSet(FLWND_DOUBLE_BUFFERED))
      {
       create_dbl_buffered();
      }
      else
      {
        release_dbl_buffered();
      }
      OnSize(how,cx,cy);
     }
    }

    void __fastcall TWindow::refresh     (HDC dc,LPRECT r)
    {
      if(dbl_dc)
      {
        int x  = r ? r->left : 0;
        int y  = r ? r->top  : 0;
        int cx = r ? r->right - r->left : wnd_size.cx;
        int cy = r ? r->bottom - r->top : wnd_size.cy;
        BitBlt(dc,x,y,cx,cy,dbl_dc,x,y,SRCCOPY);
      }
//      else
//      {
//       RECT _r;
//       if(r) _r = *r;
//         else
//         GetClientRect(&_r);
//         on_paint(dc,_r,true);
//      }
    }

    int          __fastcall TWindow::on_paint(HDC dc,RECT & r,bool erase )
    {
      int ret;
      if(IsFlagsSet(FLWND_DOUBLE_BUFFERED) && get_dbl_dc())
      {
        HFONT  fnt = (HFONT )SelectObject(dbl_dc,GetFont());
        ::SetBkColor(dbl_dc,this->GetBkColor());
        ret = Paint(dbl_dc,r,erase);
        refresh(dc,&r);
        SelectObject(dbl_dc,fnt);
        
      }
      else
      ret = Paint(dc,r,erase);
      return ret;
    }

#ifdef __BORLANDC__
#pragma warn -par
#endif

    int          __fastcall TWindow::Paint(HDC dc,RECT & r,bool)
    {
     return DefaultProcessing();
    }

#ifdef __BORLANDC__
    #pragma warn .par
#endif


  HBRUSH __fastcall TWindow::SetBkBrush  (HBRUSH brush)
  {
   HBRUSH save = bkBrush;
   bkBrush = brush;
   if(ClientDC)
    ::SetBkColor((::HDC)ClientDC,GetBkColor());
   RECT r;
   GetWindowRect(&r);
   RedrawWindow((::HWND)hWnd,&r,0,RDW_ERASE|RDW_INVALIDATE);
   return save;
  }

  HBRUSH __fastcall TWindow::SetBkColor(DWORD Color )
  {
   LOGBRUSH lgbr= {BS_SOLID,Color,0};
   if(ClientDC)
    ::SetBkColor((::HDC)ClientDC,GetBkColor());
   return SetBkBrush(CreateBrushIndirect(&lgbr));

  }

  DWORD __fastcall TWindow::GetBkColor()
  {
   LOGBRUSH lgbr;
   if(bkBrush)
   GetObject(bkBrush,sizeof(lgbr),&lgbr);
   else
    lgbr.lbColor = 0;
   return lgbr.lbColor;
  }

  void   __fastcall TWindow::PostMessage(UINT msg,WPARAM wp,LPARAM lp)
  {
   ::PostMessage((::HWND)hWnd,msg,wp,lp);
  }

  LRESULT __fastcall TWindow::SendMessage (UINT msg,WPARAM wp,LPARAM lp)
  {
   return ::SendMessage((::HWND)hWnd,msg,wp,lp);
  }

  LRESULT __fastcall TWindow::SendMessageTimeout (UINT msg,WPARAM wp,LPARAM lp,bool * success,DWORD Flags,DWORD Timeout)
  {

   #ifdef _WIN64
   PDWORD ret(0);
   bool r_success (::SendMessageTimeout((::HWND)hWnd,msg,wp,lp,Flags,Timeout,(PDWORD_PTR)(&ret))? true:false);
   #else
   LRESULT ret(0);
   bool r_success (::SendMessageTimeout((::HWND)hWnd,msg,wp,lp,Flags,Timeout,(LPDWORD)(&ret))? true:false);
   #endif
   if(success)
	  *success = r_success;
   return (LRESULT)ret;
  }


#ifdef __BORLANDC__
 #pragma warn -8084
#endif

  HFONT  __fastcall TWindow::SetFont(HFONT font,bool Redraw)
  {
   HFONT OldFont = GetFont();
    SendMessage(WM_SETFONT,WPARAM(font),MAKELPARAM(Redraw,0));
   return OldFont;
  }

    BOOL CALLBACK SetChildFont(::HWND hwnd,LPARAM lParam )
    {
     SendMessage(hwnd,WM_SETFONT,WPARAM(lParam),0);
     return TRUE;
    }

#ifdef __BORLANDC__
 #pragma warn .8084
#endif

  int    __fastcall TWindow::GetCaption(char * buffer,int ccMax)
  {
   if(buffer)
   {
    *buffer = 0;
    return int(SendMessage(WM_GETTEXT,WPARAM(ccMax),LPARAM(buffer)));
   }
   return 0;
  }

  bool   __fastcall TWindow::SetCaption(char * buffer)
  {
   return SendMessage(WM_SETTEXT,0,LPARAM(buffer))? true:false;
  }

  int    __fastcall TWindow::GetID    ()
  {
   return int(GetWindowLong(GWL_ID));
  }

  HWND   __fastcall TWindow::GetParent()
  {
   return ::GetParent((::HWND)hWnd); 
  }


/* Реализация обработчиков  WM_XXXXXXX */
#ifdef __BORLANDC__
 #pragma warn -8057
#endif

    int         __fastcall TWindow::OnWMSetCursor(int TestCode,int MsgId)
    {
      return 0;
    }
#ifdef __BORLANDC__
 #pragma warn .8057
#endif

    bool         __fastcall TWindow::OnWMCreate (LPCREATESTRUCT)
    {
     return true;
    }

//    bool         __fastcall TWindow::OnWMCreate (LPCREATESTRUCTW)
//    {
//     return true;
//    }

    void         __fastcall TWindow::OnWMDestroy()
    {
     if(bkBrush)
       {
        DeleteObject(bkBrush);
        bkBrush = 0;
       }

     if(this->ClientDC)
        {
         ReleaseDC((::HWND)hWnd,(::HDC)ClientDC);
         ClientDC = 0;
        }
    }
#ifdef __BORLANDC__
 #pragma warn -8057
#endif

    bool         __fastcall TWindow::OnWMStyleChange(bool ExStyle,bool changed,LPSTYLESTRUCT ss)
    {
      if(changed)
      {
       RECT r;
       GetWindowRect(&r);
       SetSize(r.right - r.left,r.bottom - r.top-1);
       SetSize(r.right - r.left,r.bottom - r.top);
      } 
     return ss->styleOld!=ss->styleNew ? true:false;
    }
    
    bool         __fastcall TWindow::OnWMSetFocus (HWND  Lost)
    {
     return bool(DefaultProcessing()? true:false);
    }

    bool         __fastcall TWindow::OnWMKillFocus(HWND Get  )
    {
     return false;
    }

    bool         __fastcall TWindow::OnWMCommand  (HWND From,int nCode,int id)
    {
     return false;
    }

    bool         __fastcall TWindow::OnWMSysCommand(DWORD Cmd,int xpos,int ypos)
    {
     return false;
    }

    int          __fastcall TWindow::OnWMNotify(int CtrlId,LPNMHDR mnhdr)
    {
#ifdef _WIN64
     TWindow * wnd = CAST(LPVOID(::GetWindowLongPtr(mnhdr->hwndFrom,GWL_USERDATA)));
#else
     TWindow * wnd = CAST(LPVOID(::GetWindowLong(mnhdr->hwndFrom,GWL_USERDATA)));
#endif
     if(wnd )
       {
        if(wnd!=this)
        return wnd->OnWMNotify(CtrlId,mnhdr);
       }
       else
       {
        HWND n_parent = ::GetParent(mnhdr->hwndFrom);
#ifdef _WIN64
        wnd  = CAST(LPVOID(::GetWindowLongPtr((::HWND)n_parent,GWL_USERDATA)));
#else
        wnd  = CAST(LPVOID(::GetWindowLong((::HWND)n_parent,GWL_USERDATA)));
#endif
        if(wnd && wnd!=this)
          return wnd->OnWMNotify(CtrlId,mnhdr);
       }
     return 0;//DefaultProcessing();
    }

    void         __fastcall TWindow::OnWMSetFont(HFONT Font,bool Redraw)
    {
     this->Font = Font;
     //Установить по всем контролам!
#ifdef __BORLANDC__
	 EnumChildWindows((::HWND)hWnd,SetChildFont,LONG(Font));
#else
	 EnumChildWindows((::HWND)hWnd,SetChildFont,LONG(Font));
#endif
     if(ClientDC)
        SelectObject((::HDC)ClientDC,Font);
     if(Redraw)
       InvalidateRect((::HWND)hWnd,0,false);
    }

    HFONT        __fastcall TWindow::OnWMGetFont()
    {
     return Font;
    }



#ifdef __BORLANDC__ 
#pragma warn -8057
#endif

   bool         __fastcall TWindow::OnWMInitMenu(HMENU menu)
   {
    return false;
   }

    int         __fastcall TWindow::OnWMNCCalcSize(LPNCCALCSIZE_PARAMS p)
    {
     return this->DefaultProcessing();
    }

    int         __fastcall TWindow::OnWMCalcSize  (LPRECT  r)
    {
     int ret = this->DefaultProcessing();
     return ret;
    }

bool    __fastcall TWindow::OnDrawItem(int id,LPDRAWITEMSTRUCT dis)
{
 return ::SendDlgItemMessage((::HWND)hWnd,id,WM_DRAWITEM,WPARAM(id),LPARAM(dis))? true:false;
}

bool    __fastcall TWindow::OnMeasureItem(int id,LPMEASUREITEMSTRUCT mis)
{
 return ::SendDlgItemMessage((::HWND)hWnd,id,WM_MEASUREITEM,WPARAM(id),LPARAM(mis))? true:false;
}

void   __fastcall TWindow::CreateFromHWND(HWND wnd)
{
  //Создание окна на основе существующего
  if(hWnd == NULL)
  {
   hWnd = wnd;
   TWindow * wnd = CAST(LPVOID(GetWindowLong(GWL_USERDATA)));
   if(wnd)
    {
     SetFlags(FLWND_ALIAS,true);
     this->hInstance = wnd->hInstance;
     this->bkBrush   = wnd->bkBrush;
     this->Font      = wnd->Font;
     this->ClientDC  = wnd->ClientDC;
   
    } 
    else
    {
     UD = SetWindowLong(GWL_USERDATA,LONG(this));
     this->SuperclassProc = (WNDPROC )SetWindowLong(GWL_WNDPROC,LONG(DefaultProc));
	 ClientDC = GetDC((::HWND)hWnd);
    }
  }
}

void         __fastcall TWindow::OnWMShow(bool /*show*/,int /*Status*/)
{}


void         __fastcall TWindow::OnSize(int how,int /*cx*/,int /*cy*/)
{
 //По WM_SIZE
}

void         __fastcall TWindow::OnMove(int /*x*/,int /*y*/)
{
 //По WM_MOVE
}

bool         __fastcall TWindow::OnSizing(int side,TRect & old_rect,TRect & new_rect)
{
 //По WM_SIZING
 return true;
}

bool         __fastcall TWindow::OnMoving(int side,TRect & old_rect,TRect & new_rect)
{
 //По WM_SIZING
 return true;
}

void         __fastcall TWindow::OnWMGetMinmaxInfo(LPMINMAXINFO mmi)
{
}

//#include <winuser.h>

HBRUSH       __fastcall TWindow::OnCtlColor(HWND wnd,HDC dc,UINT CtrlType)
{
#ifdef _WIN64
  TWindow * w = CAST(LPVOID(::GetWindowLongPtr((::HWND)wnd,GWL_USERDATA)));
#else
  TWindow * w = CAST(LPVOID(::GetWindowLong((::HWND)wnd,GWL_USERDATA)));
#endif
  if(w )
  {
     if(w == this)
		 return this->bkBrush;
	 else
		 return w->OnCtlColor(wnd,dc,CtrlType);
  }

  return (HBRUSH)DefaultProcessing();//HBRUSH(0);
}


   void  __fastcall TWindow::SetSize  (int cx,int cy)
   {
    SetWindowPos((::HWND)hWnd,0,0,0,cx,cy,SWP_NOMOVE|SWP_NOZORDER);
   }

   void  __fastcall TWindow::SetOrigin(int x,int y )
   {
     SetWindowPos((::HWND)hWnd,0,x,y,0,0,SWP_NOSIZE|SWP_NOZORDER);
   }

    void   __fastcall TWindow::EndModal(int eCode)
    {
     this->EndMessageLoopCode = eCode ? eCode : IDCANCEL;
    }

    int          __fastcall TWindow::OnWMNextDlgCtl(HWND control,bool fHandle)
    {
     return DefaultProcessing();
    }

#ifndef _NATIVE_W2K

       TWindow::SETLAYEREDWINDOWATTRIBUTES TWindow::pfnSetLayeredWindowAttributes = 0;
       TWindow::UPDATELAYEREDWINDOW        TWindow::pfnUpdateLayeredWindow = 0;
       TWindow::LOCKSETFOREGROUNDWINDOW    TWindow::pfnLockSetForegroundWindow = 0;

       bool __fastcall TWindow::InitW2KSupport()
       {
        HMODULE user32 = GetModuleHandle("USER32.DLL");
        if(user32)
        {
           pfnSetLayeredWindowAttributes  = (SETLAYEREDWINDOWATTRIBUTES)GetProcAddress( user32,"SetLayeredWindowAttributes");
           pfnUpdateLayeredWindow         = (UPDATELAYEREDWINDOW)       GetProcAddress( user32,"UpdateLayeredWindow");
           pfnLockSetForegroundWindow     = (LOCKSETFOREGROUNDWINDOW)   GetProcAddress( user32,"LockSetForegroundWindow");
        }
        return true;
       }

#endif

bool __fastcall TWindow::SetLayeredAttributes(COLORREF crKey,BYTE bAlpha, DWORD dwFlags)
{
 #ifndef _NATIVE_W2K
  if(this->pfnSetLayeredWindowAttributes)
     return pfnSetLayeredWindowAttributes(hWnd,crKey,bAlpha,dwFlags) ?
     true:false;

     SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
     return false;
 #else
  return ::UpdateLayeredWindow(hWnd,hdcDest,pptDst,psize,hdcSrc,pptSrc,crKey,pbelnd,dwFlags) ?
         true:false;
 #endif
}

bool __fastcall TWindow::UpdateLayeredWindow(HDC hdcDst,POINT *pptDst, SIZE *psize,
                                    HDC hdcSrc,POINT *pptSrc, COLORREF crKey,
                                    BLENDFUNCTION *pblend,DWORD dwFlags)
{
 #ifndef _NATIVE_W2K
 if(this->pfnUpdateLayeredWindow)
   return pfnUpdateLayeredWindow(hWnd,hdcDst,pptDst,psize,hdcSrc,pptSrc,crKey,pblend,dwFlags) ?
          true:false;
          SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
          return false;
 #else
  return ::UpdateLayeredWindow(hWnd,hdcDst,pptDst,psize,hdcSrc,pptSrc,crKey,pbelnd,dwFlags) ?
         true:false;
 #endif
}

bool __fastcall TWindow::LockSetForegroundWindow(bool lock)
{
 #ifndef _NATIVE_W2K
 if(pfnLockSetForegroundWindow)
   return pfnLockSetForegroundWindow (lock ? LSFW_LOCK : LSFW_UNLOCK) ?
          true:false;
          SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
          return false;
 #else
  return ::LockSetForegroundWindow (lock ? LSFW_LOCK : LSFW_UNLOCK) ?
         true:false;
 #endif

}

#ifndef _NO_NAMESPACE
}//end of namespace
#endif

