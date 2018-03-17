//---------------------------------------------------------------------------

#ifndef security_wndH
#define security_wndH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "gkmodule_form.h"
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.ToolWin.hpp>
//---------------------------------------------------------------------------
class TSecurityWnd : public TGKModuleForm
{
__published:	// IDE-managed Components
	TStatusBar *StatusBar1;
	TToolBar *ToolBar1;
	TPanel *Panel1;
	TSplitter *Splitter1;
	TPanel *Panel2;
private:	// User declarations
	__fastcall TSecurityWnd(TComponent* Owner);
virtual void     __fastcall on_module_state(GKHANDLE mod,DWORD state) ;
virtual void     __fastcall on_module_config_change(GKHANDLE mod,LPMODULE_CONFIG_DATA mcd);
public:		// User declarations
	__fastcall TSecurityWnd(TComponent* Owner,HWND wnd_parent,GKHANDLE __handle);

};
//---------------------------------------------------------------------------
extern PACKAGE TSecurityWnd *SecurityWnd;
//---------------------------------------------------------------------------
#endif
