//---------------------------------------------------------------------------
#pragma hdrstop
#include <vcl.h>
#include "gkexplorer_form.h"
#pragma argsused

//---------------------------------------------------------------------------

LRESULT WINAPI module_main(DWORD cmd,LPARAM p1,LPARAM p2)
{
  LRESULT ret = 0;
  LPMODULE_WINDOW mw;
  TGKExplorerFrm * ef;
  ef = dynamic_cast<TGKExplorerFrm*>(TGKModuleForm::get_form((HWND)p1));
  switch(cmd)
  {
   case GKME_CMD_GET_MODULETYPE : ret  = MT_WINDOW;break;

   case MWCM_CREATE_WINDOW:
    mw = (LPMODULE_WINDOW)p1;
    ef = new TGKExplorerFrm(Application,mw->parent_window,mw->mod_handle);
    ret  =(LRESULT)ef->Handle;
    if((BOOL)p2)
     module_main(MWCM_SHOW_WINDOW,ret,mw->modeless ? FALSE:TRUE);

   break;
   case MWCM_DESTROY_WINDOW:
   if(ef)
     delete ef;
   break;
   case MWCM_SHOW_WINDOW:
   if(ef)
     {
      if(p2)
         ret = ef->ShowModal();
         else
         ef->Show();
     }
   break;
   case MWCM_CLOSE_WINDOW:
   if(ef)
   {
    ef->ModalResult = mrCancel;
    ef->Hide();
   }
   break;
   case MWCM_SETFOCUS_WINDOW:
   if(ef)
   {
    if(IsIconic(ef->Handle))
      ShowWindow(ef->Handle,SW_SHOWNORMAL);
    BringWindowToTop(ef->Handle);
    SetForegroundWindow(ef->Handle);
    ef->SetFocus();
   }
    break;
    case MWCM_PROCESS_MESSAGES:
    Application->ProcessMessages();
    ret = TRUE;
    break;

  }
  return ret;

}

