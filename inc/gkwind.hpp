//-----------------------------------------------------------------------------
// Интерфрейс класса "Окна" для реализации GRKRNL32
// GranitKernel32 V 1.0
// Остапенко Д.В. СДТУ 1993-2000
//-----------------------------------------------------------------------------
#ifndef GRKE_WINDOW_INC
#define GRKE_WINDOW_INC
#include <ke_defs.h>

#ifndef _Windows_

 #ifndef STRICT
  //#define STRICT
 #endif
#include <KeRtl.hpp>

 #ifndef _Windows_
  #define WIN32_LEAN_AND_MEAN
  #include <Windows.h>
  #undef WIN32_LEAN_AND_MEAN
 #endif
#endif

#include <geometry.hpp>

//-----------------------------------------------------------------------------

/*
  Определение _NO_NAMESPACE отменяет пространство имен KeRTL, KrnlWin
  Определение _NATIVE_W2K   Реализует нормальную поддержку Win2k
*/

#ifdef _WIN64
  #define GWL_USERDATA  GWLP_USERDATA
  #define GWL_WNDPROC   GWLP_WNDPROC

  #define DWL_MSGRESULT DWLP_MSGRESULT
  #define DWL_DLGPROC   DWLP_DLGPROC



#endif

#pragma pack(push,8)

#ifndef _NO_NAMESPACE
#define KrnlWin KeWin
using namespace KeRTL;

namespace KeWin
{
using namespace KeRTL;
/*typedef LPVOID  HWND;
typedef LPVOID  HDC;
typedef LPVOID  HINSTANCE;
typedef LPVOID  HMENU;
typedef LPVOID  HFONT;*/
#endif

  #define SECRET_DWORD 0x6E698775
	//Флаги окна
  #define FLWND_NODELETE       FLBASE_NODELETE
  #define FLWND_ALIAS           0x00000001

  #define FLWND_PREPROCESSMSG   0x00000002
  #define FLWND_POSTPROCESSMSG  0x00000004
  #define FLWND_DOUBLE_BUFFERED 0x00000008

  /*Флажки определяющие поведение дочернего окна при изменении размеров родительского*/
  #define FLWND_ANCHOR_LEFT    0x00000010
  #define FLWND_ANCHOR_TOP     0x00000020
  #define FLWND_ANCHOR_RIGHT   0x00000040
  #define FLWND_ANCHOR_BOTTOM  0x00000080


  KEWIN_FUNC void __fastcall TrackWndSizePosition(HWND parent, HWND control,TRect & old_r,TRect & new_r,DWORD flags,bool redraw = false);

  KEWIN_CLASS  TWindow:public TKeRTLBase
  {
    protected:
    DWORD         SecretDword;
    HINSTANCE     hInstance;
    HWND          hWnd;
    int           EndMessageLoopCode;
    HBRUSH        bkBrush;
    HFONT         Font;
    HDC           ClientDC;
    MSG           CurrentMsg;
    WNDPROC       SuperclassProc;
    LONG          UD;
    HACCEL        Accelerator;
    SIZE          wnd_size;

    /*Для двойной буфферизации отрисовки*/
    HDC           dbl_dc;
    HBITMAP       dbl_bmp;
    static LRESULT CALLBACK DefaultProc(HWND wnd,UINT Msg,WPARAM wp,LPARAM lp);
            void    __fastcall ReplaceUserData(LONG nud);
    virtual bool    __fastcall GetMessage(MSG & msg);
    public:
    virtual void    __fastcall ProcessMessages();
    virtual LRESULT __fastcall MessageHandler(MSG &);
    protected:
    virtual void    __fastcall NotHandledMsg(MSG &);
    virtual LRESULT __fastcall PreProcessMessage(MSG &msg);
    virtual LRESULT __fastcall PostProcessMessage(MSG & msg);
    virtual LRESULT __fastcall DefaultProcessing();
    virtual bool    __fastcall TranslateAccelerator(MSG & msg);
	

    virtual const char * __fastcall GetClassName();
    virtual void         __fastcall GetWndClass(WNDCLASSEXA & );
    //virtual void         __fastcall GetWndClass(WNDCLASSEXW & );
    virtual DWORD        __fastcall GetStyle(bool Ex = false);

    virtual WNDPROC      __fastcall GetDefWndProc();
    virtual int          __fastcall EraseBkgnd(HDC dc);
    virtual int          __fastcall Paint(HDC dc,RECT & r,bool);
            int          __fastcall on_paint(HDC dc,RECT & r,bool );
            void         __fastcall on_size(int how,int cx,int cy);
    virtual bool         __fastcall OnWMCreate (LPCREATESTRUCTA);
    virtual void         __fastcall OnWMDestroy();
    virtual bool         __fastcall OnWMStyleChange(bool ExStyle,bool change,LPSTYLESTRUCT);
    virtual int          __fastcall OnWMSetCursor(int TestCode,int MsgId);
    virtual bool         __fastcall OnWMSetFocus (HWND  Lost);
    virtual bool         __fastcall OnWMKillFocus(HWND Get  );
    virtual bool         __fastcall OnWMCommand  (HWND From,int nCode,int id);
    virtual bool         __fastcall OnWMSysCommand(DWORD Cmd,int xpos,int ypos);
    virtual int          __fastcall OnWMNotify(int CtrlId,LPNMHDR mnhdr);
    virtual void         __fastcall OnWMSetFont(HFONT Font,bool Redraw);
    virtual HFONT        __fastcall OnWMGetFont();
    virtual bool         __fastcall OnWMInitMenu(HMENU menu);
    virtual int          __fastcall OnWMNCCalcSize(LPNCCALCSIZE_PARAMS);
    virtual int          __fastcall OnWMCalcSize  (LPRECT  r);
    virtual void         __fastcall OnWMGetMinmaxInfo(LPMINMAXINFO mmi); 
    virtual bool         __fastcall OnDrawItem(int id,LPDRAWITEMSTRUCT dis);
    virtual void         __fastcall OnWMShow(bool show,int Status);
    virtual bool         __fastcall OnMeasureItem(int id,LPMEASUREITEMSTRUCT mis);
    virtual void         __fastcall OnSize(int how,int cx,int cy);
    virtual bool         __fastcall OnSizing(int side ,TRect & old_rect,TRect & new_rect);
    virtual void         __fastcall OnMove  (int x,int y);
    virtual bool         __fastcall OnMoving(int side, TRect & old_rect,TRect & new_rect);
    virtual int          __fastcall OnWMNextDlgCtl(HWND control,bool fHandle);
    virtual HBRUSH       __fastcall OnCtlColor(HWND wnd,HDC dc,UINT CtrlType);
            void         __fastcall Release(){Destroy();}
            HDC          __fastcall get_dbl_dc    ();
            void         __fastcall release_dbl_buffered();
            void         __fastcall create_dbl_buffered();
    public:
                               TWindow(HINSTANCE = GetModuleHandle(0));
    virtual                   ~TWindow();
    virtual void   __fastcall Create
    ( HWND Parent,const char * Caption,RECT & r,int Id = -1,
      HMENU Menu = 0,DWORD Style=0,DWORD ExStyle=0,const char * ClassName=0);
    virtual bool   __fastcall Destroy();
    virtual int    __fastcall MessageLoop();
    virtual int    __fastcall ShowModal();
    virtual void   __fastcall CreateFromHWND(HWND wnd);
    virtual bool   __fastcall CanClose(int);
    virtual void   __fastcall EndModal(int eCode);
            int    __fastcall GetEndMessageLoopCode(){return EndMessageLoopCode;}
            HINSTANCE __fastcall GetInstance();
            void      __fastcall SetAccelerator(HACCEL accel){Accelerator = accel;}

            bool   __fastcall ShowWindow(int how);
            bool   __fastcall IsVisible();

            void    __fastcall PostMessage(UINT msg,WPARAM,LPARAM);
            LRESULT __fastcall SendMessage(UINT msg,WPARAM,LPARAM);
            LRESULT __fastcall SendMessageTimeout(UINT msg,WPARAM,LPARAM,bool * success,DWORD Flags = SMTO_NORMAL,DWORD TimeOut = 1000);

            HBRUSH __fastcall SetBkBrush  (HBRUSH brush);
            HBRUSH __fastcall SetBkColor(DWORD Color );
            DWORD  __fastcall GetBkColor();
            HICON  __fastcall GetIcon(bool Small);
            HICON  __fastcall SetIcon(HICON icon,bool Small);
            HFONT  __fastcall GetFont();
            HFONT  __fastcall SetFont(HFONT font,bool Redraw);
            int    __fastcall GetCaption(char * buffer,int ccMax);
            bool   __fastcall SetCaption(char * buffer);
            int    __fastcall GetID    ();
            HWND   __fastcall GetParent();
            void   __fastcall EnableDelete(bool);
            HWND   __fastcall SetFocus();
            HWND operator ()();
            void  __fastcall GetWindowRect(LPRECT r);
            void  __fastcall GetClientRect(LPRECT r);
            TRect __fastcall GetWindowRect();
            TRect __fastcall GetClientRect();
            #ifdef _WIN64
            LONG_PTR  __fastcall GetWindowLong(int idx);
            LONG_PTR  __fastcall SetWindowLong(int idx,LONG_PTR v);
            #else
            LONG  __fastcall GetWindowLong(DWORD idx);
            LONG  __fastcall SetWindowLong(DWORD idx,LONG);
            #endif
            void  __fastcall SetSize  (int cx,int cy);
            void  __fastcall SetOrigin(int x,int y);
            HWND  __fastcall GetDlgItem(int);
            LONG  __fastcall SendDlgItemMessage(int,UINT,WPARAM,LPARAM);
            static TWindow * __fastcall CAST(LPVOID ptr);
            bool __fastcall RegisterWindow(HINSTANCE hi);
            HWND __fastcall SetActive();
            bool __fastcall BringToTop();
            bool __fastcall SetForeground();
            void __fastcall Repaint(bool EraseBkgnd = true,TRect * r = NULL);
            HDC  __fastcall GetClientDC(){return ClientDC;}
            HDC  __fastcall get_paint_dc() {return dbl_dc ? dbl_dc : ClientDC;}
            void __fastcall refresh     (HDC dc,LPRECT r = NULL);

            //WIN2K Support LayeredWindow
#ifndef _NATIVE_W2K
		   static bool __fastcall InitW2KSupport();
       protected:

typedef
BOOL (WINAPI * SETLAYEREDWINDOWATTRIBUTES)
     (HWND hwnd,COLORREF crKey, BYTE bAlpha,  DWORD dwFlags);

typedef
BOOL (WINAPI * UPDATELAYEREDWINDOW)
     (HWND hwnd, HDC hdcDst, POINT *pptDst, SIZE *psize,HDC hdcSrc,POINT *pptSrc,
      COLORREF crKey,BLENDFUNCTION *pblend,DWORD dwFlags);
typedef BOOL (WINAPI * LOCKSETFOREGROUNDWINDOW)(UINT uLockCode );

      static SETLAYEREDWINDOWATTRIBUTES pfnSetLayeredWindowAttributes;
      static UPDATELAYEREDWINDOW        pfnUpdateLayeredWindow;
      static LOCKSETFOREGROUNDWINDOW    pfnLockSetForegroundWindow;

#endif
public:
bool __fastcall SetLayeredAttributes(COLORREF crKey,BYTE bAlpha, DWORD dwFlags);

bool __fastcall UpdateLayeredWindow(HDC hdcDst,POINT *pptDst, SIZE *psize,
                                    HDC hdcSrc,POINT *pptSrc, COLORREF crKey,
                                    BLENDFUNCTION *pblend,DWORD dwFlags);

static bool __fastcall LockSetForegroundWindow(bool lock);
};




  #define BEGIN_MSG_MAP\
  virtual int __fastcall MessageHandler(MSG & msg)\
  {\
          switch(msg.message)\
          {


  #define MSG_HANDLER(Message,Method)   case Message: return (LRESULT)Method(msg);


  #define MSG_HANDLER_WPLP(Message,Method,type1,type2) case Message: return (LRESULT)Method(msg.message,(type1)(msg.wParam),(type2)(msg.lParam));

  #define END_MSG_MAP(Parent)\
          default: \
             return Parent::MessageHandler(msg);\
          }\
        }

  #define DECLARE_MSG_MAP \
          virtual int __fastcall MessageHandler(MSG & msg);


  #define IMPLEMENT_MSG_MAP(ClassName) \
  int __fastcall ClassName::MessageHandler(MSG & msg)\
  {\
          switch(msg.message)\
          {



#ifndef _NATIVE_W2K
#ifdef _MSC_VER
//Расширенные стили
//из winuser.h

#define WS_EX_LAYERED           0x00080000
#define WS_EX_NOINHERITLAYOUT   0x00100000L // Disable inheritence of mirroring by children
#define WS_EX_LAYOUTRTL         0x00400000L // Right to left mirroring
#define WS_EX_NOACTIVATE        0x08000000L

#define LWA_COLORKEY            0x00000001
#define LWA_ALPHA               0x00000002
#define ULW_COLORKEY            0x00000001
#define ULW_ALPHA               0x00000002
#define ULW_OPAQUE              0x00000004

#define LSFW_LOCK       1
#define LSFW_UNLOCK     2

#endif
#endif
/*inline implementation*/

  inline void   __fastcall TWindow::EnableDelete(bool e)
  { SetFlags(FLWND_NODELETE,!e);};

  inline HWND              TWindow::operator()(){return hWnd;}

  inline bool   __fastcall TWindow::ShowWindow(int how)
  {return ::ShowWindow((::HWND)hWnd,how) ? true : false;}

  inline bool   __fastcall TWindow::IsVisible()
  {return IsWindowVisible((::HWND)hWnd)? true : false;}

  inline HWND   __fastcall TWindow::SetFocus()
  {  return ::SetFocus((::HWND)hWnd); }

  inline void  __fastcall TWindow::GetWindowRect(LPRECT r)
  {  if(r) ::GetWindowRect((::HWND)hWnd,r);  }

  inline void  __fastcall TWindow::GetClientRect(LPRECT r)
  {  if(r) ::GetClientRect((::HWND)hWnd,r);  }

  inline TRect __fastcall TWindow::GetWindowRect()
  {
   TRect r; GetWindowRect(&r);return r;
  }

  inline TRect __fastcall TWindow::GetClientRect()
  {
   TRect r; GetClientRect(&r);return r;
  }

#ifdef _WIN64
inline  LONG_PTR  __fastcall TWindow::GetWindowLong(int idx)
  {
   return ::GetWindowLongPtr((::HWND)hWnd,idx);
  }

inline   LONG_PTR  __fastcall TWindow::SetWindowLong(int idx,LONG_PTR v)
  {
   return ::SetWindowLongPtr((::HWND)hWnd,idx,v);
  }
#else
  inline LONG  __fastcall TWindow::GetWindowLong(DWORD idx)
  {return ::GetWindowLong((::HWND)hWnd,idx);}

  inline LONG  __fastcall TWindow::SetWindowLong(DWORD idx,LONG v)
  {return ::SetWindowLong((::HWND)hWnd,idx,v);}
#endif

  inline HWND  __fastcall TWindow::GetDlgItem(int i)
  { return ::GetDlgItem((::HWND)hWnd,i); }

  inline LONG  __fastcall TWindow::SendDlgItemMessage(int i ,UINT m,WPARAM w,LPARAM l)
  { return ::SendDlgItemMessage((::HWND)hWnd,i,m,w,l);  }


inline  TWindow::~TWindow()
{
      if(hWnd && (Flags& (FLWND_NODELETE|FLWND_ALIAS))==0)
        Destroy();
}

inline HINSTANCE __fastcall TWindow::GetInstance()
{ return hInstance;}

inline void    __fastcall TWindow::ReplaceUserData(LONG nud)
{
 #ifdef _WIN64
 UD = SetWindowLong(GWLP_USERDATA,nud);
 #else
 UD = SetWindowLong(GWL_USERDATA,nud);
 #endif
}

inline HWND __fastcall TWindow::SetActive()
{
 return SetActiveWindow((::HWND)hWnd);
}

inline bool __fastcall TWindow::BringToTop()
{
 return BringWindowToTop((::HWND)hWnd) ? true:false;
}

inline bool __fastcall TWindow::SetForeground()
{
 return SetForegroundWindow((::HWND)hWnd) ? true:false;
}

inline   HICON  __fastcall TWindow::GetIcon(bool Small)
  {
   return HICON(SendMessage(WM_GETICON,WPARAM(Small ? ICON_SMALL : ICON_BIG),0));
  }

inline   HICON   __fastcall TWindow::SetIcon(HICON icon,bool Small)
  {
   return HICON(SendMessage(WM_SETICON,WPARAM(Small ? ICON_SMALL : ICON_BIG),LPARAM(icon)));
  }

inline   HFONT  __fastcall TWindow::GetFont()
  {
    return HFONT (SendMessage(WM_GETFONT,0,0));
  }

inline void __fastcall TWindow::Repaint(bool EraseBkgnd ,TRect * r )
{
 InvalidateRect((::HWND)hWnd,r,EraseBkgnd ? TRUE:FALSE);
}




#ifndef _NO_NAMESPACE
}//end of namespace
#endif

#pragma pack(pop)

#endif

