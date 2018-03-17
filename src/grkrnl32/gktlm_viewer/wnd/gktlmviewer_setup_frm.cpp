//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "gktlmviewer_setup_frm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "gkmodule_form"
#pragma resource "*.dfm"
TGKTlmViewerSetup *GKTlmViewerSetup;
//---------------------------------------------------------------------------
__fastcall TGKTlmViewerSetup::TGKTlmViewerSetup(TComponent* Owner)
	: TGKModuleForm(Owner)
{
}

__fastcall TGKTlmViewerSetup::TGKTlmViewerSetup(TComponent* Owner,HWND wnd_owner,GKHANDLE h)
       	: TGKModuleForm(Owner,wnd_owner,h)
{
}

//---------------------------------------------------------------------------

void     __fastcall TGKTlmViewerSetup::after_set_gkhandle()
{
   Fin_setup = 0;
   TGKModuleForm::after_set_gkhandle();

   //setup_controls(&settings);
   on_module_state(mod_iface(),mod_iface.get_module_state());
   set_notify(mod_iface(),MNF_COMMON);
}

//---------------------------------------------------------------------------

void     __fastcall TGKTlmViewerSetup::on_module_state(GKHANDLE mod,DWORD state)
{
  bool running = ((state & MODULE_STATE_RUNNING_MASK )== MODULE_STATE_RUNNING) ? true:false;
  bool stopped  = (state & MODULE_STATE_RUNNING_MASK ) ? false:true;
  tbStop->Enabled  = running;
  tbStart->Enabled = stopped;
  if(running)
     StatusBar1->Panels->Items[0]->Text = "Запись ведётся";
  if(stopped)
     StatusBar1->Panels->Items[0]->Text = "Запись остановлена";

}

void     __fastcall TGKTlmViewerSetup::on_module_config_change(GKHANDLE mod,LPMODULE_CONFIG_DATA mcd)
{

}


