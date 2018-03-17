#include <windows.h>
#pragma hdrstop


#include "gkipmodem_frm.h"

#pragma argsused
//---------------------------------------------------------------------------
gkhandle_wnd_map<TGkIPModemFrm> wnd_map;

LRESULT WINAPI module_main(DWORD cmd,LPARAM p1,LPARAM p2)
{
  LRESULT ret = 0;
  LPMODULE_WINDOW mw;
  TGkIPModemFrm* mf = dynamic_cast<TGkIPModemFrm*>(TGKModuleForm::get_form((HWND)p1));;
  switch(cmd)
  {
   case MWCM_CREATE_WINDOW:
    mw = (LPMODULE_WINDOW)p1;
    mf = wnd_map[mw->mod_handle];
    if(mf == NULL)
    {
     mf = new TGkIPModemFrm(Application,mw->parent_window,mw->mod_handle);
     wnd_map.add_form(mf);
    }
//    mf->PageControl1->ActivePageIndex = mw->window_type-1;
    ret  =(LRESULT)mf->Handle;
    if((BOOL)p2)
     module_main(MWCM_SHOW_WINDOW,ret,mw->modeless ? FALSE:TRUE);

   break;
   case MWCM_DESTROY_WINDOW:
   if(mf)
     {wnd_map.remove_form(mf);delete mf;}
   break;
   case MWCM_SHOW_WINDOW:
   if(mf)
     {
      if(p2)
         ret = mf->ShowModal();
         else
         mf->Show();
     }
   break;
   case MWCM_CLOSE_WINDOW:
   if(mf)
   {
    mf->ModalResult = mrCancel;
    mf->Hide();
   }
   break;
   case MWCM_SETFOCUS_WINDOW:
   if(mf)
    mf->SetFocus();
    break;

  }
  return ret;

}

 