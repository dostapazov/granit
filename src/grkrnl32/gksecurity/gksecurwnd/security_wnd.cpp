//---------------------------------------------------------------------------

#pragma hdrstop
#include <vcl.h>
#include "security_wnd.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "gkmodule_form"
#pragma resource "*.dfm"
TSecurityWnd *SecurityWnd;
//---------------------------------------------------------------------------
__fastcall TSecurityWnd::TSecurityWnd(TComponent* Owner)
	: TGKModuleForm(Owner)
{
}
//---------------------------------------------------------------------------

	__fastcall TSecurityWnd::TSecurityWnd(TComponent* Owner,HWND wnd_parent,GKHANDLE __handle)
	: TGKModuleForm(Owner,wnd_parent,__handle)
{
}



void     __fastcall TSecurityWnd::on_module_state(GKHANDLE mod,DWORD state)
{

}

void     __fastcall TSecurityWnd::on_module_config_change(GKHANDLE mod,LPMODULE_CONFIG_DATA mcd)
{

}

void     __fastcall TSecurityWnd::after_set_gkhandle()
{
  TGKModuleForm::after_set_gkhandle();
  setup_users_tree();
}

void     __fastcall TSecurityWnd::setup_users_tree  ()
{
  users_tree->Items->BeginUpdate() ;
  USER_ENTRY ue;
  ue.dwSize = sizeof(ue);
  DWORD idx = 0;

  while(mod_iface.call(SCM_ENUM_USERS,LPARAM(&ue),idx++) == GKH_RET_SUCCESS)
  {
   TTreeNode * node = users_tree->Items->AddObject(NULL,ue.user_name,(LPVOID)ue.user_id);
   node->Data = (LPVOID)ue.user_id;
  }

  users_tree->Items->EndUpdate  () ;
}


