//---------------------------------------------------------------------------

#ifndef iec60870_frmH
#define iec60870_frmH
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
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.ImgList.hpp>
#include "gkmod_frame.h"
#include "iec6070_mod_settings.h"
#include "iec60870_line_param.h"
#include "about_common_frame.h"
#include "iec60870_data.h"
#include "mon_frame.h"




//---------------------------------------------------------------------------
class TIec60870Frm : public TGKModuleForm
{
__published:	// IDE-managed Components
	TStatusBar *StatusBar1;
	TCoolBar *CoolBar1;
	TToolBar *ToolBar1;
	TToolButton *bStart;
	TToolButton *bStop;
	TToolButton *ToolButton3;
	TImageList *ImageList1;
	TTreeView *ModemTree;
	TSplitter *Splitter1;
	TPanel *Panel1;
	TPageControl *PageControl1;
	TTabSheet *ParamsSheet;
	TIEC60870ModSettings *IEC60870ModSettings1;
	TToolButton *tbNew;
	TToolButton *tbDelete;
	TToolButton *tbApply;
	TToolButton *ToolButton5;
	TToolButton *tbUndo;
	TTabSheet *MonSheet;
	TTabSheet *ViewSheet;
	TIEC60870LineParam *IEC60870LineParam1;
	TImageList *TreeImages;
	TToolButton *tbRefresh;
	TToolButton *ToolButton1;
	TTabSheet *AboutSheet;
	TAboutCommonFrame *AboutCommonFrame1;
	TIEC60870MonFrame *MonFrame;
	TMemo *Memo1;
	TToolButton *ToolButton2;
	TToolButton *ToolButton4;
	TIEC60870DataFrame *DataFrame;
	void __fastcall bStartClick(TObject *Sender);
	void __fastcall bStopClick(TObject *Sender);
	void __fastcall ModemTreeChange(TObject *Sender, TTreeNode *Node);
	void __fastcall ModemTreeChanging(TObject *Sender, TTreeNode *Node, bool &AllowChange);
	void __fastcall PageControl1Change(TObject *Sender);
	void __fastcall tbDeleteClick(TObject *Sender);
	void __fastcall tbApplyClick(TObject *Sender);
	void __fastcall tbUndoClick(TObject *Sender);
	void __fastcall tbNewClick(TObject *Sender);
	void __fastcall ModemTreeCompare(TObject *Sender, TTreeNode *Node1, TTreeNode *Node2,
          int Data, int &Compare);
	void __fastcall ModemTreeDeletion(TObject *Sender, TTreeNode *Node);
	void __fastcall tbRefreshClick(TObject *Sender);
	void __fastcall ToolButton2Click(TObject *Sender);

private:	// User declarations
  TTreeNode        * root_node;
  int                in_setup;
  TGKModuleFrame   * Fcurrent_frame;

protected:
void     __fastcall after_set_gkhandle();
void     __fastcall done_call_gkhandle(LRESULT ret,const wchar_t * str);
void     __fastcall begin_call_gkhandle(const wchar_t * cmd_name);
LRESULT  __fastcall on_gkhandle_notify (GKHANDLE from,LPNOTIFY_CODE nc,DWORD mask);
void     __fastcall set_current_frame(TGKModuleFrame * value);
void     __fastcall setup_lines();
TTreeNode *     __fastcall add_iec60870_line(const LPMODEM_LINE_PROPS mlp);
       void     __fastcall upd_iec60870_line(TTreeNode * node,const LPMODEM_LINE_PROPS mlp);


void     __fastcall on_modem_number  (const LPMODEM_CHANGE_NUMBER mns);
void     __fastcall on_line_state    (const LPMODEM_LINE_STATE mls);

void     __fastcall on_module_state(GKHANDLE mod,DWORD state) ;
void     __fastcall on_module_config_change(GKHANDLE mod,LPMODULE_CONFIG_DATA mcd);
bool     __fastcall get_module_settings ();
void     __fastcall on_frame_changed    (TObject * sender);
TTreeNode* __fastcall find_line_node(DWORD line_num);



         __fastcall TIec60870Frm(TComponent* Owner);
        __property TGKModuleFrame * current_frame = {read = Fcurrent_frame, write = set_current_frame};

virtual void __fastcall WndProc(Messages::TMessage &msg);
virtual void __fastcall setup_form_sizes();

public:		// User declarations
	__fastcall TIec60870Frm(TComponent* Owner,HWND,GKHANDLE);


};
//---------------------------------------------------------------------------
extern PACKAGE TIec60870Frm *Iec60870Frm;
//---------------------------------------------------------------------------
#define WMUSER_SETUP_LINES_TREE (WM_USER+100)
#endif
