//---------------------------------------------------------------------------

#ifndef gkopc_client_frmH
#define gkopc_client_frmH
//---------------------------------------------------------------------------

#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "gkmodule_form.h"
#include <modemmer.h>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ToolWin.hpp>
#include <System.ImageList.hpp>
#include <Vcl.ImgList.hpp>
#include <gkopc_client.h>
#include <Vcl.ExtCtrls.hpp>
#include "gkmod_frame.h"
#include "gkopc_client_mod_settings_frame.h"
#include "gkopc_client_line_settings.h"
#include "gkopcitem_view_frame.h"
#include "about_common_frame.h"
//#include <opc_helper.hpp>

//---------------------------------------------------------------------------
class TGKOpcClientFrm : public TGKModuleForm
{
__published:	// IDE-managed Components
	TStatusBar *StatusBar1;
	TToolBar *ToolBar1;
	TToolButton *bStart;
	TToolButton *bStop;
	TToolButton *ToolButton1;
	TToolButton *tbApply;
	TToolButton *tbUndo;
	TImageList *ImageList1;
	TTreeView *LinesTree;
	TSplitter *Splitter1;
	TPanel *Panel1;
	TToolButton *tbNew;
	TToolButton *tbDelete;
	TToolButton *ToolButton2;
	TPageControl *PageControl1;
	TTabSheet *ConfigSheet;
	TOpcModSettings *OpcModSettings1;
	TOpcLineSetingsFrame *OpcLineSetingsFrame1;
	TTabSheet *ViewOpcItemsSheet;
	TGKOpcItemsViewFrame *DataFrame;
	TTabSheet *AboutSheet;
	TAboutCommonFrame *AboutCommonFrame1;
	TMemo *Memo1;
	TToolButton *bRefresh;
	TTabSheet *MonSheet;
	TMemo *opc_mon;
	void __fastcall bStartClick(TObject *Sender);
	void __fastcall bStopClick(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall LinesTreeChanging(TObject *Sender, TTreeNode *Node, bool &AllowChange);
	void __fastcall LinesTreeChange(TObject *Sender, TTreeNode *Node);
	void __fastcall tbNewClick(TObject *Sender);
	void __fastcall tbApplyClick(TObject *Sender);
	void __fastcall tbDeleteClick(TObject *Sender);
	void __fastcall tbUndoClick(TObject *Sender);
	void __fastcall LinesTreeDeletion(TObject *Sender, TTreeNode *Node);
	void __fastcall PageControl1Change(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall bRefreshClick(TObject *Sender);

private:	// User declarations

        int                     in_setup;
        TTreeNode              * root_node;
        TGKModuleFrame         * Fcurrent_frame;
virtual void     __fastcall on_module_state(GKHANDLE mod,DWORD state);
virtual void     __fastcall on_module_config_change(GKHANDLE mod,LPMODULE_CONFIG_DATA mcd);
virtual void     __fastcall after_set_gkhandle();
        void     __fastcall done_call_gkhandle(LRESULT ret,const wchar_t * str);
        void     __fastcall begin_call_gkhandle(const wchar_t * cmd_name);
        void     __fastcall set_current_frame(TGKModuleFrame * value);
        void     __fastcall on_frame_changed (TObject * sender);

	__fastcall TGKOpcClientFrm(TComponent* Owner);
virtual void __fastcall WndProc(Messages::TMessage & msg);
        void __fastcall setup_lines();
        void __fastcall add_opc_line(LPMODEM_LINE_PROPS mlp );
    LRESULT  __fastcall on_gkhandle_notify (GKHANDLE from,LPNOTIFY_CODE nc,DWORD mask);

        void __fastcall enable_monitor(bool _enable);
public:		// User declarations
	__fastcall TGKOpcClientFrm(TComponent* Owner, HWND parent, GKHANDLE mod_handle );
        void     __fastcall end_setup  (){if((--in_setup)<0) in_setup = 0;}
        void     __fastcall begin_setup(){++in_setup;}
     __property TGKModuleFrame * current_frame = {read = Fcurrent_frame, write = set_current_frame};


};
//---------------------------------------------------------------------------
extern PACKAGE TGKOpcClientFrm *GKOpcClientFrm;

#define WMUSER_SETUP_LINES_TREE (WM_USER+100)

//---------------------------------------------------------------------------
#endif
