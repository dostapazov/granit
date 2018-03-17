//---------------------------------------------------------------------------
#include <windows.h>
#pragma warn -8058
#include <vcl.h>
#pragma warn .8058
#pragma hdrstop
#include "grscfrm.h"

gkhandle_wnd_map<TSCGranFrm>  wnd_map;

LRESULT WINAPI module_main(DWORD cmd,LPARAM p1,LPARAM p2)
{
  LRESULT ret = 0;
  LPMODULE_WINDOW mw;
  TSCGranFrm* mf = dynamic_cast<TSCGranFrm*>(TGKModuleForm::get_form((HWND)p1));
  switch(cmd)
  {
   case GKME_CMD_GET_MODULETYPE : ret  = MT_WINDOW|MT_MODEM;break;
   case MWCM_CREATE_WINDOW:
    mw = (LPMODULE_WINDOW)p1;
    mf = wnd_map[mw->mod_handle];
    if(mf == NULL)
    {
     mf = new TSCGranFrm(Application,mw->parent_window,mw->mod_handle);
     wnd_map.add_form(mf);
    }
    /*Настройка или просмотр*/
    mf->PageControl1->ActivePageIndex = mw->window_type-1;
    mf->PageControl1->ActivePage      = mf->PageControl1->Pages[mw->window_type-1];
    if(mf->PageControl1->OnChange)
       mf->PageControl1->OnChange(mf);
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
         {
          if(mf->Visible)
          {
           if(IsIconic(mf->Handle))
              ShowWindow(mf->Handle,SW_RESTORE);
          }
          else
            mf->Show();
          mf->SetFocus();   
         } 
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

