#include <gkcommctrl.hpp>
#include <stdio.h>

#ifndef _NO_NAMESPACE
namespace KrnlWin
{
#endif

 TToolInfoA::TToolInfoA(HWND Ctrl,char  * Text,DWORD Flags,HINSTANCE hInst  ,bool subClass)
 {
  TOOLINFOA & tinfo = *this;
  ZeroMemory(&tinfo,sizeof(tinfo));
  tinfo.cbSize   = sizeof(tinfo);
  tinfo.uFlags   = TTF_IDISHWND | Flags |((subClass) ? TTF_SUBCLASS:0);
  tinfo.uId      = UINT(Ctrl);
  tinfo.hinst    = (::HINSTANCE)hInst;
  tinfo.lpszText = Text;
 }

 TToolInfoA::TToolInfoA(int Ctrl,TRect & r,char  * Text,DWORD Flags,HINSTANCE hInst  ,bool subClass)
 {
  TOOLINFOA & tinfo = *this;
  ZeroMemory(&tinfo,sizeof(tinfo));
  tinfo.cbSize   = sizeof(tinfo);
  tinfo.uFlags   = Flags |((subClass) ? TTF_SUBCLASS:0);
  tinfo.uId      = UINT(Ctrl);
  tinfo.hinst    = (::HINSTANCE)hInst;
  tinfo.lpszText = Text;
  SetRect(r);
 }

 TToolInfoW::TToolInfoW(HWND Ctrl,wchar_t  * Text,DWORD Flags,HINSTANCE hInst  ,bool subClass)
 {
  TOOLINFOW & tinfo = *this;
  ZeroMemory(&tinfo,sizeof(tinfo));
  tinfo.cbSize   = sizeof(tinfo);
  tinfo.uFlags   = TTF_IDISHWND | Flags |((subClass) ? TTF_SUBCLASS:0);
  tinfo.uId      = UINT(Ctrl);
  tinfo.hinst    = (::HINSTANCE)hInst;
  tinfo.lpszText = Text;
 }

 TToolInfoW::TToolInfoW(int Ctrl,TRect & r,wchar_t  * Text,DWORD Flags,HINSTANCE hInst  ,bool subClass)
 {
  TOOLINFOW & tinfo = *this;
  ZeroMemory(&tinfo,sizeof(tinfo));
  tinfo.cbSize   = sizeof(tinfo);
  tinfo.uFlags   = Flags |((subClass) ? TTF_SUBCLASS:0);
  tinfo.uId      = UINT(Ctrl);
  tinfo.hinst    = (::HINSTANCE)hInst;
  tinfo.lpszText = Text;
  SetRect(r);
 }


bool __fastcall TToolTips::DoCreate(DWORD Style)
{
 CheckAndLoad(this);
 TRect r;
 this->Create(0,0,r,-1,0,Style|WS_POPUP,WS_EX_TOOLWINDOW,TOOLTIPS_CLASSW);
 return hWnd!=0 ? true:false;
}

bool  __fastcall TToolTips::AppendControl(HWND Ctrl,char * Text,DWORD Flags,HINSTANCE hInst ,bool subClass)
{
 bool ret ( false);
 if(Ctrl)
 {
  SendMessage(TTM_ACTIVATE,TRUE,0);
  TToolInfoA tinfo(Ctrl,Text,Flags,hInst,subClass);
  tinfo.SetWindow(hWnd);
  ret = SendMessage(TTM_ADDTOOLA,0,LPARAM(&tinfo)) ? true:false;
 }
 return ret;
}

bool  __fastcall TToolTips::AppendControl(HWND parent,int  id,char * Text,DWORD Flags,HINSTANCE hInst ,bool subClass)
{
  HWND w;
  w = ::GetDlgItem((::HWND)parent,id);
  return AppendControl(w,Text,Flags,hInst,subClass);
}

bool  __fastcall TToolTips::AppendControl(HWND Ctrl,wchar_t * Text,DWORD Flags,HINSTANCE hInst ,bool subClass)
{
 bool ret ( false);
 if(Ctrl)
 {
  SendMessage(TTM_ACTIVATE,TRUE,0);
  TOOLINFOW tinfo;
  ZeroMemory(&tinfo,sizeof(tinfo));
  tinfo.cbSize   = sizeof(tinfo);
  tinfo.uFlags   = TTF_IDISHWND | TTF_CENTERTIP |  Flags |((subClass) ? TTF_SUBCLASS:0);
  tinfo.uId      = UINT(Ctrl);
  tinfo.hwnd     = (::HWND)hWnd;
  tinfo.hinst    = (::HINSTANCE)hInst;
  tinfo.lpszText = Text;
  ret = SendMessage(TTM_ADDTOOLW,0,LPARAM(&tinfo)) ? true:false;
 }
 return ret;
}

bool  __fastcall TToolTips::AppendControl(HWND parent,int  id,wchar_t * Text,DWORD Flags,HINSTANCE hInst ,bool subClass)
{
  HWND w;
  w = ::GetDlgItem((::HWND)parent,id);
  return AppendControl(w,Text,Flags,hInst,subClass);
}

  bool     __fastcall TToolTips::RemoveControl(HWND Ctrl)
  {
   TToolInfoA info;
   if(GetToolInfo(Ctrl,info))
     return SendMessage(TTM_DELTOOL,0,LPARAM(&info)) ? true:false;
    return false; 
  }


  bool     __fastcall TToolTips::GetToolInfo(HWND ctrl,TToolInfoA & i)
  {
    i.uFlags = TTF_IDISHWND;
    i.uId    = UINT(ctrl);
    return SendMessage(TTM_GETTOOLINFOA,0,LPARAM(&i))? true:false;
  }

  bool     __fastcall TToolTips::GetToolInfo(HWND ctrl,TToolInfoW & i)
  {
    i.uFlags = TTF_IDISHWND;
    i.uId    = UINT(ctrl);
    return SendMessage(TTM_GETTOOLINFOW,0,LPARAM(&i))? true:false;
  }

  LRESULT      __fastcall TToolTips::MessageHandler(MSG & msg)
  {
   if(msg.message == WM_PAINT)
    return DefaultProcessing();
   return TWindow::MessageHandler(msg);
  }

 bool __fastcall TToolTips::GetTipText(TToolInfoA &)
 {return false;}
 bool __fastcall TToolTips::GetTipText(TToolInfoW &)
 {return false;}


int          __fastcall TToolTips::OnWMNotify(int CtrlId,LPNMHDR mnhdr)
{
 int ret(0);

 switch( mnhdr->code)
 {
  case TTN_GETDISPINFOA:
  case TTN_GETDISPINFOW:
       {
       LPNMTTDISPINFOA diA = (LPNMTTDISPINFOA) mnhdr;
       LPNMTTDISPINFOW diW = (LPNMTTDISPINFOW) mnhdr;
       if(mnhdr->code == TTN_GETDISPINFOA)
       {
        TToolInfoA info;
        SendMessage(TTM_GETCURRENTTOOLA,0,LPARAM(&info));
        info.lpszText = diA->szText;
        if(__FnTipTextA.HaveMethod()==false || false == __FnTipTextA.CallMethod(this,&info))
          this->GetTipText(info);


       }
       else
       {
        TToolInfoW info;
        SendMessage(TTM_GETCURRENTTOOLW,0,LPARAM(&info));
        info.lpszText = diW->szText;
        if(__FnTipTextW.HaveMethod()==false || false == __FnTipTextW.CallMethod(this,&info))
          this->GetTipText(info);
          else
          info.lpszText = diW->szText;

       }
       }
       break;
       default: ret = TCommonControl::OnWMNotify(CtrlId,mnhdr);
 }
 return ret;
}

DWORD __fastcall TToolTips::CustomDraw(NMTTCUSTOMDRAW * )
{
 return DefaultProcessing();
}


#ifndef _NO_NAMESPACE
//namespace KrnlWin
}
#endif
