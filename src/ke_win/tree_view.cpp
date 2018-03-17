#include <gkcommctrl.hpp>

#ifndef _NO_NAMESPACE
namespace KrnlWin{
#endif

 bool __fastcall TTreeView::DoCreate(HWND parent,DWORD id)
 {
    this->CheckAndLoad(this);
    HWND w = ::GetDlgItem((::HWND)parent,id);
    if(w)
    CreateFromHWND(w);
    return hWnd ? true:false;

 }

 bool __fastcall TTreeView::DoCreate(HWND parent,TRect & r,DWORD Style,DWORD ExStyle, DWORD id)
 {
   Create((::HWND)parent,0,r,id,0,Style|(parent ? WS_CHILD:0),ExStyle, WC_TREEVIEW );
   return hWnd ? true:false;
 }

  #pragma warn -8057
 int      __fastcall TTreeView::OnWMNotify(int CtrlId,LPNMHDR mnhdr)
 {
  int ret(0);
  switch(mnhdr->code)
  {
    case TVN_BEGINDRAGA      :
    case TVN_BEGINDRAGW      :
         if(this->_OnBeginDrag.HaveMethod())
            this->_OnBeginDrag.CallMethod(this,(LPNMTREEVIEW)mnhdr);
            else
            OnBeginDrag((LPNMTREEVIEW)mnhdr);
    break;
    case TVN_BEGINLABELEDITA :
    case TVN_BEGINLABELEDITW :
         if(this->_OnBeginEditLabel.HaveMethod())
            ret = this->_OnBeginEditLabel.CallMethod(this,(LPNMTVDISPINFO)mnhdr) ? FALSE:TRUE;
            else
            ret = OnBeginLabelEdit((LPNMTVDISPINFO)mnhdr) ? FALSE:TRUE;
    break;
    case TVN_ENDLABELEDITA   :
    case TVN_ENDLABELEDITW   :
         if(this->_OnEndLabelEdit.HaveMethod())
            ret = this->_OnEndLabelEdit.CallMethod(this,(LPNMTVDISPINFO)mnhdr) ? FALSE:TRUE;
            else
            ret = OnEndLabelEdit((LPNMTVDISPINFO)mnhdr) ? FALSE:TRUE;
    break;

    case TVN_DELETEITEMA    :
    case TVN_DELETEITEMW    :
         if(this->_OnDeleteItem.HaveMethod())
            this->_OnDeleteItem.CallMethod(this,(LPNMTREEVIEW)mnhdr);
            else
            OnDeleteItem((LPNMTREEVIEW)mnhdr);
    break;
    case TVN_GETDISPINFOA   :
    case TVN_GETDISPINFOW   :
         if(this->_OnGetDispInfo.HaveMethod())
                  _OnGetDispInfo.CallMethod(this,(LPNMTVDISPINFO)mnhdr);
                  else
                  this->OnGetDispInfo((LPNMTVDISPINFO)mnhdr);

    break;
    case TVN_SETDISPINFOA   :
    case TVN_SETDISPINFOW   :
         if(this->_OnSetDispInfo.HaveMethod())
                  _OnSetDispInfo.CallMethod(this,(LPNMTVDISPINFO)mnhdr);
                  else
                  this->OnSetDispInfo((LPNMTVDISPINFO)mnhdr);
    break;

    case TVN_ITEMEXPANDEDA  :
    case TVN_ITEMEXPANDEDW  :
    if(this->_OnExpanded.HaveMethod())
             _OnExpanded.CallMethod(this,(LPNMTREEVIEW)mnhdr);
             else
             OnExpanded((LPNMTREEVIEW)mnhdr);
    break;
    case TVN_ITEMEXPANDINGA :
    case TVN_ITEMEXPANDINGW :
    if(this->_OnExpanding.HaveMethod())
             ret = _OnExpanding.CallMethod(this,(LPNMTREEVIEW)mnhdr) ? FALSE:TRUE;
             else
             ret = OnExpanding((LPNMTREEVIEW)mnhdr) ? FALSE:TRUE ;

    break;
    case TVN_KEYDOWN        :
         if(this->_OnKeyDown.HaveMethod())
                  _OnKeyDown.CallMethod(this,(LPNMTVKEYDOWN)mnhdr);
                  else
                  OnKeyDown((LPNMTVKEYDOWN)mnhdr);
    break;
    case TVN_SELCHANGEDA    :
    case TVN_SELCHANGEDW    :
         if(this->_OnSelChanged.HaveMethod())
                  _OnSelChanged.CallMethod(this,(LPNMTREEVIEW)mnhdr);
                  else
                  OnSelChanged((LPNMTREEVIEW)mnhdr);

    break;
    case TVN_SELCHANGINGA   :
    case TVN_SELCHANGINGW   :
         if(this->_OnSelChanging.HaveMethod())
                  ret = _OnSelChanging.CallMethod(this,(LPNMTREEVIEW)mnhdr) ? FALSE:TRUE;
                  else
                  ret = OnSelChanging((LPNMTREEVIEW)mnhdr)? FALSE:TRUE;
    break;
    default: ret = 0;//TCommonControl::OnWMNotify(CtrlId,mnhdr);
    break;
   }
  return ret;
 }
 #pragma warn .8057




#ifndef _NO_NAMESPACE
}
#endif


 
