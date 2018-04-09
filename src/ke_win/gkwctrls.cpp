
#include <gkwctrls.hpp>
#pragma hdrstop
//winuser.h
#ifndef _NO_NAMESPACE
namespace KeWin
{
#endif


 void   __fastcall TWinControl::Create
    ( HWND Parent,const wchar_t * Caption,RECT & r,int Id  ,
      HMENU Menu ,DWORD Style,DWORD ExStyle,const wchar_t * ClassName)
 {

    WNDCLASSEXW wc;
    ZeroMemory(&wc,sizeof(wc));
    wc.cbSize = sizeof(wc);
    GetClassInfoExW(0,ClassName ? ClassName : GetClassName(),&wc);
    SuperclassProc = wc.lpfnWndProc;
    wchar_t c_name [MAX_PATH];
    lstrcpyW(c_name,L"KEWIN_");
    lstrcatW(c_name,ClassName ? ClassName:GetClassName());
    wc.lpfnWndProc = (WNDPROC)DefaultProc;
    wc.hInstance = 0;
    wc.lpszClassName = c_name;
    RegisterClassExW(&wc);
    TWindow::Create(Parent,Caption,r,Id,Menu,Style,ExStyle,c_name);
 }

  LRESULT __fastcall TWinControl::MessageHandler(MSG & msg)
  {
   if(msg.message == WM_PAINT )
      return DefaultProcessing();
   if(msg.message == WM_GETDLGCODE)
      return GetDlgCode();   
      return TWindow::MessageHandler(msg);
     
  }

  HBRUSH __fastcall TWinControl::OnCtlColor(HWND,HDC dc,UINT)
  {
    ::SetTextColor((::HDC)dc,TextColor);
    ::SetBkMode((::HDC)dc,TRANSPARENT);
    return bkBrush;
  }


  bool  __fastcall TWinControl::OnWMCreate(LPCREATESTRUCTW cs)
  {
   HFONT Font;
   HWND p = GetParent();
   if(p)
     Font = HFONT(::SendMessage((::HWND)p,WM_GETFONT,0,0));
   if(!Font)
       Font =(HFONT) GetStockObject(ANSI_VAR_FONT);
   this->SetFont(Font,true);
   return  TWindow::OnWMCreate(cs);
  }

   COLORREF __fastcall TWinControl::SetTextColor(COLORREF new_color)
      {
       COLORREF old = TextColor;
       TextColor = new_color;
       if(TextColor && !bkBrush)
          SetBkColor(GetSysColor(COLOR_BTNFACE));
       return old;
      }





#ifndef _NO_NAMESPACE
//namespace KeWin
}
#endif
