#include <KeRtl.hpp>
#include <gkwctrl.hpp>

namespace KrnlWin
{
    WNDPROC __fastcall TWinControl::GetDefWndProc()
    {
      return this->SuperclassProc ? SuperclassProc : DefaultProc;
    }

   void    __fastcall TWinControl::GetWndClass(WNDCLASSEX & wc)
   {
      ZeroMemory(&wc,sizeof(wc));
      wc.cbSize = sizeof(wc);
      GetClassInfoEx(0,GetCtrlClassName(),&wc);
      wc.style  &= ~CS_GLOBALCLASS;
      wc.style  |= CS_PARENTDC;
      wc.lpszClassName = GetClassName();
      SuperclassProc   = wc.lpfnWndProc;
      wc.lpfnWndProc   = TWindow::DefaultProc;
   }

   TWinControl::TWinControl(HINSTANCE hInst):TWindow(hInst)
   {}


    bool    __fastcall TWinControl::OnWMCreate(LPCREATESTRUCT cs)
    {
        HFONT fnt = (HFONT)::SendMessage(GetParent(),WM_GETFONT,0,0);
        SetFont(fnt,true);
        return TWindow::OnWMCreate(cs);
    }

    LRESULT     __fastcall TWinControl::MessageHandler(MSG & msg)
    {
     switch(msg.message)
     {
      case WM_NOTIFY     :
                          return TWindow::MessageHandler(msg);
      case WM_PAINT      :return DefaultProcessing();                    
      case WM_DRAWITEM   :
           return int(OnDrawItem(int(msg.wParam),LPDRAWITEMSTRUCT(msg.lParam)));
      case WM_MEASUREITEM:
           return int(OnMeasureItem(int(msg.wParam),LPMEASUREITEMSTRUCT(msg.lParam)));
      }

     return TWindow::MessageHandler(msg);
    }

     void    __fastcall TWinControl::Create
      ( HWND Parent,const char * Caption,RECT & r,int Id ,
        HMENU Menu ,DWORD Style,DWORD ExStyle,const char * ClassName)
     {
      hWnd = 0;
      if(!ClassName)
          ClassName = GetCtrlClassName();

      if(Parent && Id!=-1)
      {
        hWnd = ::GetDlgItem(Parent,Id);
        if(hWnd)
        {
         char cl_name[MAX_PATH];
         CharUpperBuff(cl_name,::GetClassName(hWnd,cl_name,sizeof(cl_name)));
         if(lstrcmpA(cl_name,GetCtrlClassName()))
           {
            //Контрол существует но имена WINDOW-классов не совпадают
            //Вообщето это исключение но мы пока бум создавать новый контрол
            hWnd = 0;//Позволяем работать дальше
           }
           else
           {
            //То что надо
            SetWindowLong(GWL_USERDATA,LONG(this));
            SuperclassProc = (WNDPROC)GetWindowLong(GWL_WNDPROC);
            //CreateFromHWND(hWnd);
           }
        }
      }

      if(!hWnd)
      {
       TWindow::Create(Parent,Caption,r,Id,Menu,Style,ExStyle,ClassName);
      }

      if(hWnd)
       {
        if(!SuperclassProc)
           SuperclassProc = (WNDPROC)GetWindowLong(GWL_WNDPROC);
        SetWindowLong(GWL_ID,LONG(Id));
       }

     }

#ifdef __BORLANDC__
#pragma warn -8057
#endif

bool    __fastcall TWinControl::OnDrawItem(int id,LPDRAWITEMSTRUCT dis)
{
	return DefaultProcessing() ? true:false;
}

bool    __fastcall TWinControl::OnMeasureItem(int id,LPMEASUREITEMSTRUCT mis)
{
 return DefaultProcessing()? true:false;
}

#ifdef __BORLANDC__
#pragma warn .8057
#endif
/*Кнопка*/

  TButton::TButton(HINSTANCE hInst):TWinControl(hInst){}

  const char * __fastcall TButton::GetClassName()
  {return "TBUTTON";}
  const char * __fastcall TButton::GetCtrlClassName()
  {return "BUTTON";}

  void  __fastcall TButton::SetImage(LPVOID img,bool tBitmap)
  {
   LONG Style = ::GetWindowLong(hWnd,GWL_STYLE);
   if(img==0)
     Style&=~(BS_BITMAP|BS_ICON);
     else
     Style|= (tBitmap? BS_BITMAP:BS_ICON)|BS_TEXT;
     ::SetWindowLong(hWnd,GWL_STYLE,Style);
     SendMessage(BM_SETIMAGE,WPARAM( tBitmap ? IMAGE_BITMAP:IMAGE_ICON),LPARAM(img));
  }

   bool  __fastcall TButton::DoCreate(HWND Parent,char * Caption, RECT & r,int id,bool Def)
   {
           Create(Parent,Caption,r,id,0,
                  WS_VISIBLE|WS_CHILD|WS_TABSTOP|
                  (Def? BS_DEFPUSHBUTTON:BS_PUSHBUTTON)|
                  BS_TEXT|BS_NOTIFY,
                  0,GetClassName()
                 );
     return bool(hWnd  ? true:false);
   }

/************************* RADIOBUTTON  ***************************************/

    const char * __fastcall TRadioButton::GetClassName()
    {return "TRadioButton";}

    bool __fastcall TRadioButton::DoCreate(HWND Parent,char * Caption,RECT & r,int id)
    {
     Create(Parent,Caption,r,id,0,
            WS_VISIBLE|WS_CHILD|WS_TABSTOP|BS_AUTORADIOBUTTON|BS_TEXT,0,GetClassName()); 
     return bool(hWnd ? true:false);
    }

/*************************    STATIC    ***************************************/

    const char * __fastcall TStatic::GetClassName()
    {return "TSTATIC";}
    const char * __fastcall TStatic::GetCtrlClassName()
    {return "STATIC";}

    bool  __fastcall TStatic::DoCreate(HWND parent,char * caption,RECT & r,int id)
    {
     Create(parent,caption,r,id,0,WS_VISIBLE|WS_CHILD|WS_TABSTOP|SS_SUNKEN|SS_CENTER,0,GetClassName());
     return bool(hWnd ? true:false);
    }


  LPVOID       __fastcall TStatic::GetImage (LPDWORD Type)
  {
    DWORD checked_type = *Type;
    LPVOID image(0);
    if(hWnd)
    {
    if(checked_type)
     {
      image =(LPVOID) SendMessage(STM_GETIMAGE,WPARAM(*Type),0);
     }

      *Type = -1;

      if(image == 0 && checked_type!=IMAGE_BITMAP)
       {
         image = (LPVOID)SendMessage(STM_GETIMAGE,WPARAM(IMAGE_BITMAP),0);
         if(image)
          *Type = IMAGE_BITMAP;
       }

      if(image == 0 && checked_type!=IMAGE_CURSOR)
       {
         image = (LPVOID)SendMessage(STM_GETIMAGE,WPARAM(IMAGE_CURSOR),0);
         if(image)
          *Type = IMAGE_CURSOR;
       }

      if(image == 0 && checked_type!=IMAGE_ICON)
       {
         image = (LPVOID)SendMessage(STM_GETIMAGE,WPARAM(IMAGE_ICON),0);
         if(image)
          *Type = IMAGE_ICON;
       }

      if(image == 0 && checked_type!=IMAGE_ENHMETAFILE)
       {
         image = (LPVOID)SendMessage(STM_GETIMAGE,WPARAM(IMAGE_ENHMETAFILE),0);
         if(image)
          *Type = IMAGE_ENHMETAFILE;
       }

    }
    return image;
  }

}
