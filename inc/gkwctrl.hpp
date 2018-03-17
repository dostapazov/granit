
// Заголовок определения управляющих элементов
// GranitKernel32 V 1.0
// Остапенко Д.В. СДТУ 1993-2000

#ifndef  GRKE_STD_WINCTRLS_INC
 #define GRKE_STD_WINCTRLS_INC

 #ifndef GRKE_WINDOW_INC
  #include <gkwind.hpp>
 #endif

 #pragma pack (push,8)

namespace KrnlWin
{
  KEWIN_CLASS TWinControl:public TWindow
  {
   protected:
        WNDPROC      __fastcall GetDefWndProc();
virtual LRESULT      __fastcall MessageHandler(MSG &);
        bool         __fastcall OnWMCreate(LPCREATESTRUCT cs);
        void         __fastcall GetWndClass(WNDCLASSEX & );
virtual const char * __fastcall GetCtrlClassName()=0;
virtual bool         __fastcall OnDrawItem(int id,LPDRAWITEMSTRUCT dis);
virtual bool         __fastcall OnMeasureItem(int id,LPMEASUREITEMSTRUCT mis);

                         TWinControl(HINSTANCE);
        void         __fastcall Create
                     ( HWND Parent,const char * Caption,RECT & r,int Id = -1,
                       HMENU Menu = 0,DWORD Style=0,DWORD ExStyle=0,const char * ClassName=0
                     );


  };

 KEWIN_CLASS TButton:public TWinControl
 {
    const char * __fastcall GetClassName();
    const char * __fastcall GetCtrlClassName();

   public:
     TButton(HINSTANCE = 0);
     bool        __fastcall DoCreate(HWND Parent,char * Caption, RECT & r,int id,bool Def = false);
     DWORD       __fastcall GetBtnState();
     void        __fastcall SetBtnState(DWORD);
     void        __fastcall Click();

     HICON       __fastcall GetIconImage();
     HBITMAP     __fastcall GetBitmapImage();
     void        __fastcall SetIconImage(HICON);
     void        __fastcall SetBitmapImage(HBITMAP);

     LPVOID      __fastcall GetImage(bool tBitmap);
     void        __fastcall SetImage(LPVOID ,bool tBitmap);
     void        __fastcall SetBtnStyle(DWORD,bool redraw);
 };

 KEWIN_CLASS TRadioButton:public TButton
 {
    const char * __fastcall GetClassName();
    public:
    TRadioButton(HINSTANCE = 0);
    bool __fastcall DoCreate(HWND Parent,char * Caption,RECT & r,int id=-1);

 };

 KEWIN_CLASS TStatic:public TWinControl
 {
    const char * __fastcall GetClassName();
    const char * __fastcall GetCtrlClassName();
    public:
    TStatic(HINSTANCE = 0);
    bool         __fastcall DoCreate(HWND parent,char * caption,RECT & r,int id=-1);
    LPVOID       __fastcall SetImage(DWORD Type,LPVOID image);
    LPVOID       __fastcall SetBitmap(HBITMAP bitmap);
    LPVOID       __fastcall SetIcon  (HICON   icon  );
    LPVOID       __fastcall SetEMF   (HENHMETAFILE cursor);
    LPVOID       __fastcall SetCursor(HCURSOR bitmap);
    LPVOID       __fastcall GetImage (LPDWORD Type);
 };

 KEWIN_CLASS TEdit:public TWinControl
 {
    const char * __fastcall GetClassName();
    const char * __fastcall GetCtrlClassName();
  public:
    TEdit(HINSTANCE = 0);
    bool         __fastcall DoCreate(HWND parent,char * caption,RECT & r,int id=-1);

 };

/****************               Button inlines      ***************************/

inline     DWORD       __fastcall TButton::GetBtnState()
{ return DWORD(SendMessage(BM_GETSTATE,0,0));}

inline     void        __fastcall TButton::SetBtnState(DWORD State)
{SendMessage(BM_SETSTATE,WPARAM(State),0);}

inline     void        __fastcall TButton::Click()
{SendMessage(BM_CLICK,WPARAM(0),LPARAM(0));}


inline     LPVOID      __fastcall TButton::GetImage(bool tBitmap)
{return (LPVOID)SendMessage(BM_GETIMAGE,WPARAM( tBitmap ? IMAGE_BITMAP:IMAGE_ICON),LPARAM(0));}


inline     HICON       __fastcall TButton::GetIconImage()
{return HICON(GetImage(false));}

inline     HBITMAP     __fastcall TButton::GetBitmapImage()
{return HBITMAP(GetImage(true));}

inline     void        __fastcall TButton::SetIconImage(HICON icon)
{SetImage(icon,false);}
inline     void        __fastcall TButton::SetBitmapImage(HBITMAP bmp)
{SetImage(bmp,true);}

inline     void        __fastcall TButton::SetBtnStyle(DWORD style,bool redraw)
{
SendMessage(BM_SETSTYLE,WPARAM(style),LPARAM(redraw));
}

/* TRadioButton */
inline    TRadioButton::TRadioButton(HINSTANCE inst):TButton(inst)
    {}


/*  Static  */
inline TStatic::TStatic(HINSTANCE inst ):TWinControl(inst)
{}

inline LPVOID __fastcall TStatic::SetImage(DWORD type,LPVOID image)
{
  LPVOID ret =
  (LPVOID)SendMessage(STM_SETIMAGE,WPARAM(type),LPARAM(image));
  return ret;
}
inline LPVOID       __fastcall TStatic::SetBitmap(HBITMAP bitmap)
{return (LPVOID)SetImage(IMAGE_BITMAP,bitmap);}

inline LPVOID       __fastcall TStatic::SetIcon  (HICON   icon  )
{return (LPVOID)SetImage(IMAGE_ICON  ,icon  );}

inline LPVOID       __fastcall TStatic::SetEMF   (HENHMETAFILE emf)
{return (LPVOID)SetImage(IMAGE_ENHMETAFILE,emf);}

inline LPVOID       __fastcall TStatic::SetCursor(HCURSOR cursor)
{return (LPVOID)SetImage(IMAGE_CURSOR,cursor);}


}


#pragma pack(pop)
#endif

