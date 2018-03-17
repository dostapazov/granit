//---------------------------------------------------------------------------

#ifndef gktlmviewer_setup_frmH
#define gktlmviewer_setup_frmH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "gkmodule_form.h"
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ToolWin.hpp>
#include "../gktlm_viewer.h"
#include <System.ImageList.hpp>
#include <Vcl.ImgList.hpp>
//---------------------------------------------------------------------------
class TGKTlmViewerSetup : public TGKModuleForm
{
__published:	// IDE-managed Components
	TStatusBar *StatusBar1;
	TToolBar *ToolBar1;
	TPageControl *PageControl1;
	TTabSheet *TabSheet1;
	TToolButton *tbStart;
	TToolButton *tbStop;
	TImageList *ImageList1;
	TToolButton *ToolButton1;
	TToolButton *ToolButton2;
	TToolButton *ToolButton3;
	TToolButton *tbApply;
	TToolButton *tbUndo;
	TToolButton *ToolButton6;
private:	// User declarations
protected:
         int Fin_setup;
void     __fastcall after_set_gkhandle();
void     __fastcall on_module_state(GKHANDLE mod,DWORD state);
void     __fastcall on_module_config_change(GKHANDLE mod,LPMODULE_CONFIG_DATA mcd);

public:		// User declarations
        __fastcall TGKTlmViewerSetup(TComponent* Owner,HWND wnd_owner,GKHANDLE h);
	__fastcall TGKTlmViewerSetup(TComponent* Owner);
   void __fastcall begin_setup(){++Fin_setup;}
   void __fastcall end_setup  (){if(--Fin_setup<0) Fin_setup = 0;}
        __property int in_setup = {read = Fin_setup};
};
//---------------------------------------------------------------------------
extern PACKAGE TGKTlmViewerSetup *GKTlmViewerSetup;
//---------------------------------------------------------------------------
#endif
