//---------------------------------------------------------------------------

//#include <vcl.h>
#include <windows.h>
#include "GKScaleFrm.h"
#pragma hdrstop

#pragma argsused

gkhandle_wnd_map<TGKScaleFrm> wnd_map;

#pragma warn -8004
LRESULT WINAPI module_main(DWORD cmd,LPARAM p1,LPARAM p2)
{
  LRESULT ret = 0;
  LPMODULE_WINDOW mw;
  TGKScaleFrm * mf = NULL;
  switch(cmd)
  {
   case MWCM_CREATE_WINDOW:
    mw = (LPMODULE_WINDOW)p1;
    mf = wnd_map[mw->mod_handle];
    if(mf == NULL)
    {
     mf = new TGKScaleFrm(Application,mw->parent_window,mw->mod_handle,mw->window_type);
     wnd_map.add_form(mf);
    }
    ret  =(LRESULT)mf;
    if((BOOL)p2)
     module_main(MWCM_SHOW_WINDOW,ret,mw->modeless ? FALSE:TRUE);

   break;
   case MWCM_DESTROY_WINDOW:
   mf = static_cast<TGKScaleFrm*>(LPVOID(p1));
   if(mf)
     {wnd_map.remove_form(mf);
     delete mf;}
   break;
   case MWCM_SHOW_WINDOW:
   mf = static_cast<TGKScaleFrm*>(LPVOID(p1));
   if(mf)
     {
      if(p2)
         ret = mf->ShowModal();
         else
         mf->Show();
     }
   break;
   case MWCM_CLOSE_WINDOW:
   mf = static_cast<TGKScaleFrm*>(LPVOID(p1));
   if(mf)
   {
    mf->ModalResult = mrCancel;
    mf->Hide();
   }
   break;
   case MWCM_SETFOCUS_WINDOW:
   mf = static_cast<TGKScaleFrm*>(LPVOID(p1));
   if(mf)
    mf->SetFocus();
    break;
  }
  return ret;
}
#pragma warn .8004

//---------------------------------------------------------------------------
