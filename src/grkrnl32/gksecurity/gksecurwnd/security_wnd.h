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
#include <gksecurity.h>

//---------------------------------------------------------------------------
class TSecurityWnd : public TGKModuleForm
{
__published:	// IDE-managed Components
	TStatusBar *StatusBar1;
	TToolBar *ToolBar1;
	TPageControl *PageControl1;
	TTabSheet *users_sheet;
	TTreeView *users_tree;
	TSplitter *Splitter1;
private:	// User declarations
	__fastcall TSecurityWnd(TComponent* Owner);
virtual void     __fastcall on_module_state(GKHANDLE mod,DWORD state) ;
virtual void     __fastcall on_module_config_change(GKHANDLE mod,LPMODULE_CONFIG_DATA mcd);
virtual void     __fastcall after_set_gkhandle();
        void     __fastcall setup_users_tree  ();
public:		// User declarations
	__fastcall TSecurityWnd(TComponent* Owner,HWND wnd_parent,GKHANDLE __handle);

};
//---------------------------------------------------------------------------
extern PACKAGE TSecurityWnd *SecurityWnd;
//---------------------------------------------------------------------------
#endif
