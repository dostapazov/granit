//---------------------------------------------------------------------------

#ifndef cpc152dev_formH
#define cpc152dev_formH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "gkmodule_form.h"
#include <cpc152dev.h>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ToolWin.hpp>
#include <System.ImageList.hpp>
#include <Vcl.ImgList.hpp>
#include <Vcl.ExtCtrls.hpp>
#include "cpc152_mod_frame.h"
#include "gkmod_frame.h"
#include "cpc152_dev_frame.h"
#include <VCLTee.Chart.hpp>
#include <VclTee.TeeGDIPlus.hpp>
#include <VCLTee.TeEngine.hpp>
#include <VCLTee.TeeProcs.hpp>
#include <VCLTee.Series.hpp>
#include <Vcl.Buttons.hpp>
#include "cpc152dev_spectr.h"
#include "about_common_frame.h"
#include <map>



//---------------------------------------------------------------------------
class TCpc152DevFrm : public TGKModuleForm
{
__published:	// IDE-managed Components
	TToolBar *ToolBar1;
	TToolButton *tbStart;
	TToolButton *tbStop;
	TToolButton *ToolButton3;
	TImageList *ImageList1;
	TToolButton *tbNew;
	TToolButton *tbDelete;
	TToolButton *tbApply;
	TToolButton *tbUndo;
	TToolButton *tbRefresh;
	TStatusBar *StatusBar1;
	TTreeView *Cpc152Tree;
	TSplitter *Splitter1;
	TPanel *Panel1;
	TPageControl *PageControl1;
	TTabSheet *TabSheet1;
	TTabSheet *ViewTabSheet;
	Tcpc152dev_frame *cpc152dev_frame1;
	TCpc152mod_frame *Cpc152mod_frame1;
	TTabSheet *TabSheet3;
	TMemo *Memo1;
	TTimer *ViewTimer;
	TPanel *Panel2;
	TStaticText *ConnectTime;
	TStaticText *ConnectState;
	TLabel *Label2;
	TLabel *Label1;
	TGroupBox *AnalogGrpBox;
	TBitBtn *bShowOsc;
	TListView *AnalogsList;
	TAboutCommonFrame *AboutCommonFrame1;
	TImageList *TreeImages;
	void __fastcall tbStartClick(TObject *Sender);
	void __fastcall tbStopClick(TObject *Sender);
	void __fastcall Cpc152TreeChanging(TObject *Sender, TTreeNode *Node, bool &AllowChange);
	void __fastcall Cpc152TreeChange(TObject *Sender, TTreeNode *Node);
	void __fastcall tbApplyClick(TObject *Sender);
	void __fastcall tbNewClick(TObject *Sender);
	void __fastcall tbDeleteClick(TObject *Sender);
	void __fastcall Cpc152TreeCollapsing(TObject *Sender, TTreeNode *Node, bool &AllowCollapse);
	void __fastcall bShowOscClick(TObject *Sender);
	void __fastcall tbUndoClick(TObject *Sender);
	void __fastcall PageControl1Change(TObject *Sender);
	void __fastcall ViewTimerTimer(TObject *Sender);


private:	// User declarations
protected:
int     in_setup;
TTreeNode           * root_node;
TGKModuleFrame      * Fcurrent_frame;
void       __fastcall after_set_gkhandle();
void       __fastcall done_call_gkhandle(LRESULT ret,wchar_t * str);
void       __fastcall begin_call_gkhandle(wchar_t * cmd_name);
LRESULT    __fastcall on_gkhandle_notify (GKHANDLE from,LPNOTIFY_CODE nc,DWORD mask);
void       __fastcall on_line_state      (const LPMODEM_LINE_STATE mls);
TTreeNode* __fastcall find_line_node (DWORD line_number);
void       __fastcall set_node_image_index (TTreeNode * node,DWORD line_state);
TTreeNode* __fastcall add_line_node(DWORD line_num,DWORD line_state);



void     __fastcall on_module_state(GKHANDLE mod,DWORD state);
void     __fastcall on_module_config_change(GKHANDLE mod,LPMODULE_CONFIG_DATA mcd);
bool     __fastcall get_module_settings();
void     __fastcall set_current_frame(TGKModuleFrame * value);
void     __fastcall on_frame_changed (TObject * sender);
void     __fastcall setup_lines_tree ();
         __fastcall TCpc152DevFrm    (TComponent* Owner);
virtual void __fastcall WndProc(Messages::TMessage &msg);

void     __fastcall on_spectr_destroy   (TObject * sender);
void     __fastcall setup_view_tab      ();
void     __fastcall update_connect_state();

public:		// User declarations
	__fastcall TCpc152DevFrm(TComponent* Owner,HWND,GKHANDLE);
        __property TGKModuleFrame * current_frame = {read = Fcurrent_frame, write = set_current_frame};

};
//---------------------------------------------------------------------------
extern PACKAGE TCpc152DevFrm *Cpc152DevFrm;
#define WMUSER_SETUP_LINES_TREE (WM_USER+100)
//---------------------------------------------------------------------------

#define LINE_IMAGE_CONNECT    0
#define LINE_IMAGE_CONNECTING 1
#define LINE_IMAGE_DISCONNECT 2

#endif
