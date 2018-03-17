//---------------------------------------------------------------------------

#pragma hdrstop
#include <vcl.h>
#include "dblowfrm.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

wnd_module_starter<TDBLowForm> wms(MT_WINDOW|MT_TLMDB);


LRESULT WINAPI module_main(DWORD cmd,LPARAM p1,LPARAM p2)
{
  return wms(cmd,p1,p2);

//  LRESULT ret = 0;
//  LPMODULE_WINDOW mw;
//  TDBLowForm * dbf = dynamic_cast<TDBLowForm*>(TGKModuleForm::get_form((HWND)p1));
//  switch(cmd)
//  {
//   case GKME_CMD_GET_MODULETYPE : ret  = MT_WINDOW|MT_TLMDB;break;
//   case MWCM_CREATE_WINDOW:
//    mw = (LPMODULE_WINDOW)p1;
//    if(DBLowForm == NULL)
//    {
//     DBLowForm = new TDBLowForm(Application,mw->parent_window,mw->mod_handle);
//    }
//    dbf = DBLowForm;
//    //dbf->ViewMode = mw->window_type;
//    ret  =(LRESULT)dbf->Handle;
//    if((BOOL)p2)
//     module_main(MWCM_SHOW_WINDOW,ret,mw->modeless ? FALSE:TRUE);
//
//   break;
//   case MWCM_DESTROY_WINDOW:
//   if(dbf)
//     {delete dbf;DBLowForm=NULL;}
//   break;
//   case MWCM_SHOW_WINDOW:
//   if(dbf)
//     {
//      if(p2)
//          ret = dbf->ShowModal();
//         else
//         dbf->Show();
//     }
//   break;
//   case MWCM_CLOSE_WINDOW:
//   if(dbf)
//   {
//    dbf->ModalResult = mrCancel;
//    dbf->Hide();
//   }
//   break;
//   case MWCM_SETFOCUS_WINDOW:
//   if(dbf)
//     dbf->SetFocus();
//    break;
//
//  }
//  return ret;

}
