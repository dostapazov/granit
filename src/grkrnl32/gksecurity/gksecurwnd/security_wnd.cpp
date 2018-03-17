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

