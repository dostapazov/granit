#include <gkcommctrl.hpp>

#ifndef _NO_NAMESPACE
namespace KrnlWin
{
#endif

 DWORD           TCommonControl::CtrlLoaded  = 0;
 DLLVERSIONINFO2 TCommonControl::version = {0};
 
 static bool   __fastcall __GetDllVersion(HMODULE dll,DLLVERSIONINFO2 * dv)
 {
  if(dll)
  {
   typedef HRESULT (CALLBACK * DLLGETVERSION)(LPVOID);
   DLLGETVERSION DllGetVersion = (DLLGETVERSION)GetProcAddress(dll,"DllGetVersion");
   if(DllGetVersion)
    {
     dv->info1.cbSize = sizeof(dv->info1);
     return DllGetVersion(dv) == NOERROR ? true:false;
    }
  }
  return false;
 }

 DWORD __fastcall TCommonControl::GetDllVersion()
 {
  return MAKELONG(version.info1.dwMinorVersion,version.info1.dwMajorVersion);
 }

 void          __fastcall TCommonControl::CheckAndLoad(TCommonControl * ctrl)
 {
   if( (CtrlLoaded & ctrl->GetControlFlag())==0)
   {
     HMODULE DllHinstance = GetModuleHandle("comctl32.dll");
     static bool version_avail = __GetDllVersion(DllHinstance,&version);
     typedef BOOL (WINAPI * InitProc)(LPVOID);
     InitProc init_proc = (InitProc)GetProcAddress(DllHinstance,"InitCommonControlsEx");
     if(init_proc)
     {
       INITCOMMONCONTROLSEX icex;
       icex.dwSize = sizeof(icex);
       icex.dwICC  = ctrl->GetControlFlag();
       if(init_proc(&icex))
         CtrlLoaded|=icex.dwICC;

     }
     else
     {
      InitCommonControls();
      CtrlLoaded =DWORD(-1);
     }
   }
 }

TCommonControl::TCommonControl(HINSTANCE hInst):TWindow(hInst)
{
}

 #pragma warn -8057

 int   __fastcall TCommonControl::OnWMNotify(int CtrlId,LPNMHDR mnhdr)
 {
  int ret ;
  switch(mnhdr->code)
  {
   case NM_CHAR:
         if(_OnChar.HaveMethod())
            ret = _OnChar.CallMethod(this,mnhdr);
            else
            ret = OnChar(LPNMCHAR(mnhdr));
   break;
   case NM_KEYDOWN:
         if(_OnKey.HaveMethod())
            ret = _OnKey.CallMethod(this,mnhdr);
            else
         ret = OnKey(LPNMKEY(mnhdr));
   break ;
   case NM_RCLICK:
   case NM_RDBLCLK:
   case NM_CLICK:
   case NM_DBLCLK:
         if(_OnClick.HaveMethod())
            ret = _OnClick.CallMethod(this,mnhdr);
            else
        ret = OnClick(mnhdr);
   break;
   case NM_HOVER:
         if(_OnHover.HaveMethod())
            ret = _OnHover.CallMethod(this,mnhdr);
            else
        ret = OnHover(mnhdr);
   break;
   case NM_SETFOCUS:
   case NM_KILLFOCUS:
         if(_OnSetFocus.HaveMethod())
            ret = _OnSetFocus.CallMethod(this,mnhdr);
            else
        ret = OnSetFocus(mnhdr, mnhdr->code == NM_SETFOCUS ? true:false);
   break;
   case NM_NCHITTEST:
         if(_OnNCHitTest.HaveMethod())
            ret = _OnNCHitTest.CallMethod(this,mnhdr);
            else
        ret = OnNCHitTest(LPNMMOUSE(mnhdr));
   break;
   case NM_OUTOFMEMORY:
         if(_OnOutOfMemory.HaveMethod())
            ret = _OnOutOfMemory.CallMethod(this,mnhdr);
            else
        ret = OnOutOfMemory(mnhdr);
   break;
   case NM_RELEASEDCAPTURE:
         if(_OnReleasedCapture.HaveMethod())
            ret = _OnReleasedCapture.CallMethod(this,mnhdr);
            else
        ret = OnReleasedCapture(mnhdr);
   break;
   case NM_RETURN:
         if(_OnReturn.HaveMethod())
            ret = _OnReturn.CallMethod(this,mnhdr);
            else
        ret = OnReturn(mnhdr);
   break;
   case NM_SETCURSOR:
         if(_OnSetCursor.HaveMethod())
            ret = _OnSetCursor.CallMethod(this,mnhdr);
            else
        ret = OnSetCursor(LPNMMOUSE(mnhdr));
   break;
   case        NM_CUSTOMDRAW:
               ret = DefaultProcessing();
               break;
   default:
	       ret = DefaultProcessing();


  }
  return ret;
 }
 #pragma warn .8057

  LRESULT      __fastcall TCommonControl::MessageHandler(MSG & msg)
  {
   if(msg.message >= WM_MOUSEFIRST && msg.message<=WM_MOUSELAST)
	    return TWindow::MessageHandler(msg);
   switch(msg.message)
   {
    case WM_CREATE :
    case WM_DESTROY:
    case WM_NOTIFY :
    case WM_DRAWITEM   :
    case WM_MEASUREITEM:
    
	
	return TWindow::MessageHandler(msg);
   }
    return DefaultProcessing();
  }


#ifndef _NO_NAMESPACE
//namespace KrnlWin
}
#endif

 
