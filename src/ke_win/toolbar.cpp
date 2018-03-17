#include <gkcommctrl.hpp>

#ifndef _NO_NAMESPACE
namespace KrnlWin{
#endif


  bool __fastcall TToolBar::DoCreate(HWND parent, DWORD ws, UINT wID,int nBitmaps,
                           UINT wBMID,LPCTBBUTTON lpButtons, int iNumButtons,
                           int dxButton,int dyButton,int dxBitmap,int dyBitmap)
 {
  this->CheckAndLoad(this);
  HWND w = CreateToolbarEx((::HWND)parent,ws|((parent )? WS_CHILD:0),wID,nBitmaps,
                          (::HINSTANCE)hInstance,wBMID,lpButtons,iNumButtons,
                          dxButton,dyButton,dxBitmap,dyBitmap,sizeof(TBBUTTON));
  if(w)
  {
   CreateFromHWND(w);
   return true;
  }
  return false;
 }

  bool __fastcall TToolBar::DoCreate(HWND parent,DWORD id)
  {
    this->CheckAndLoad(this);
    HWND w = ::GetDlgItem((::HWND)parent,id);
    if(w)
    CreateFromHWND(w);
    return hWnd ? true:false;
  }

 #pragma warn -8057
  int      __fastcall TToolBar::OnWMNotify(int CtrlId,LPNMHDR mnhdr)
  {
   int ret(1);
   switch(mnhdr->code)
   {
    case   TBN_GETBUTTONINFOA :
    ret = int( OnGetButtonInfo((TBNOTIFY *) mnhdr));
    break;
    case   TBN_BEGINDRAG      :
    break;
    case   TBN_ENDDRAG        :
    break;
    case   TBN_BEGINADJUST    :
    break;
    case   TBN_ENDADJUST      :
    break;
    case   TBN_RESET          :
    break;
    case   TBN_QUERYINSERT    :
     ret = int(OnQueryInsert((TBNOTIFY *) mnhdr));
    break;
    case   TBN_QUERYDELETE    :
     ret = int(OnQueryDelete((TBNOTIFY *) mnhdr));
    break;
    case   TBN_TOOLBARCHANGE  :
    break;
    case   TBN_CUSTHELP       :
    break;
    case   TBN_DROPDOWN       :
    break;
    case   TBN_GETOBJECT      :
    break;
    default :ret = 1;// TCommonControl::OnWMNotify(CtrlId,mnhdr);
   }
   return ret;
  }
 #pragma warn .8057

bool     __fastcall TToolBar::OnGetButtonInfo(TBNOTIFY * /*tbn*/)
{
 return false;
}

bool     __fastcall TToolBar::OnQueryInsert(TBNOTIFY * /*tbn*/)
{
 return true;
}

bool     __fastcall TToolBar::OnQueryDelete(TBNOTIFY * /*tbn*/)
{
 return true;
}


void __fastcall  TToolBar::AddBitmap(int bcount,DWORD bmid,HINSTANCE hInst)
{
 TBADDBITMAP adb;
 adb.hInst = (::HINSTANCE)hInst;adb.nID = bmid;
 SendMessage(TB_ADDBITMAP,bcount,LPARAM(&adb));
}

  bool __fastcall  TToolBar::AddButton(TBBUTTON & tb)
  {
   return SendMessage(TB_ADDBUTTONS,1,LPARAM(&tb)) ? true:false;
  }

  bool __fastcall  TToolBar::AddButton(int idCommand,int iBitmap, BYTE fsStyle,BYTE fsState,int iString, DWORD dwData )
  {
   TBBUTTON ab;
   ZeroMemory(&ab,sizeof(ab));
   ab.iBitmap   =iBitmap  ;
   ab.idCommand =idCommand;
   ab.fsState   =fsState  ;
   ab.fsStyle   =fsStyle   ;
   ab.dwData    =dwData   ;
   ab.iString   =iString  ;
   return AddButton(ab);
  }



#ifndef _NO_NAMESPACE
}
#endif
