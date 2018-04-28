//---------------------------------------------------------------------------

#ifndef dblowfrmH
#define dblowfrmH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "gkmodule_form.h"
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <ToolWin.hpp>
#include <gktlmdb.h>
#include <ImgList.hpp>
#include <Menus.hpp>
#include <text_viewer.hpp>
#include <Buttons.hpp>
#include <Dialogs.hpp>
#include <System.ImageList.hpp>
#include "about_common_frame.h"

typedef KeWin::TTextViewer<char>  db_monitor;
#pragma warn -8027

//---------------------------------------------------------------------------
class TDBLowForm : public TGKModuleForm
{
__published:	// IDE-managed Components
        TTreeView *DbTree;
        TToolBar *ToolBar1;
        TSplitter *Splitter1;
        TPageControl *PageControl1;
        TTabSheet *TabSheet1;
        TTabSheet *TabSheet2;
        TListView *ListView1;
        TLabel *Label2;
        TStaticText *TimeStamp;
        TGroupBox *GroupBox1;
        TLabel *Label1;
        TStaticText *FullDiagText;
        TLabel *Label3;
        TStaticText *PartDiagText;
        TLabel *Label4;
        TStaticText *StateDiagText;
        TPanel *Settings_Panel;
        TImageList *DiagImages;
        TToolButton *RefreshAddrBtn;
        TLabel *Label6;
        TEdit *FilterText;
        TMainMenu *MainMenu1;
        TMenuItem *MiAbout;
        TToolButton *StartBtn;
        TToolButton *StopBtn;
        TStatusBar *StatusBar1;
	TTabSheet *AboutSheet;
        TPanel *pu_panel;
        TPanel *sb_panel;
        TPanel *cp_panel;
        TButton *Button1;
        TButton *Button2;
        TStaticText *PDiagText;
        TEdit *ModLoNum;
        TLabel *Label5;
        TStaticText *ModuleType;
        TCheckBox *ModStatic;
        TEdit *ModHiNum;
        TLabel *Label7;
        TLabel *Label8;
        TLabel *Label9;
        TEdit *CpName;
        TCheckBox *CpStatic;
        TLabel *Label10;
        TEdit *PuName;
        TCheckBox *PuStatic;
        TToolButton *ApplyBtn;
        TToolButton *UndoBtn;
        TToolButton *ToolButton4;
        TToolButton *ToolButton1;
        TToolButton *ToolButton2;
        TMemo *Memo1;
        TTabSheet *TabSheet4;
        TGroupBox *MonitorBox;
        TBitBtn *ClearMon;
        TGroupBox *GroupBox2;
        TEdit *ScaleBeg;
        TLabel *Label11;
        TLabel *Label12;
        TEdit *ScaleEnd;
        TLabel *Label13;
        TEdit *Result;
        TLabel *Label14;
        TEdit *Kvants;
        TButton *Button3;
        TCheckBox *MonPause;
        TCheckBox *MonChanges;
        TCheckBox *MonCallOrder;
        TLabel *Label15;
        TEdit *DBFileName;
        TSpeedButton *SpeedButton1;
        TOpenDialog *OpenDialog1;
	TTabSheet *orders_sheet;
	TListView *orders_list;
	TAboutCommonFrame *AboutCommonFrame1;
        void __fastcall DbTreeChange(TObject *Sender, TTreeNode *Node);
        void __fastcall RefreshAddrBtnClick(TObject *Sender);
        void __fastcall FilterTextChange(TObject *Sender);
        void __fastcall PageControl1Change(TObject *Sender);
        void __fastcall StartBtnClick(TObject *Sender);
        void __fastcall StopBtnClick(TObject *Sender);
        void __fastcall DbTreeCollapsing(TObject *Sender, TTreeNode *Node,
          bool &AllowCollapse);
        void __fastcall DbTreeChanging(TObject *Sender, TTreeNode *Node,
          bool &AllowChange);
        void __fastcall Button1Click(TObject *Sender);
        void __fastcall Button2Click(TObject *Sender);
        void __fastcall ListView1SelectItem(TObject *Sender,
          TListItem *Item, bool Selected);
        void __fastcall ToolButton2Click(TObject *Sender);
        void __fastcall PageControl1Resize(TObject *Sender);
        void __fastcall PageControl1Changing(TObject *Sender,
          bool &AllowChange);
        void __fastcall Splitter1Moved(TObject *Sender);
        void __fastcall ClearMonClick(TObject *Sender);
        void __fastcall ScaleBegChange(TObject *Sender);
        void __fastcall Button3Click(TObject *Sender);
        void __fastcall ApplyBtnClick(TObject *Sender);
        void __fastcall ModLoNumChange(TObject *Sender);
        void __fastcall UndoBtnClick(TObject *Sender);
        void __fastcall PuNameChange(TObject *Sender);
        void __fastcall CpNameChange(TObject *Sender);
        void __fastcall FormKeyPress(TObject *Sender, char &Key);
        void __fastcall SpeedButton1Click(TObject *Sender);
        void __fastcall DBFileNameChange(TObject *Sender);
        void __fastcall CpStaticClick(TObject *Sender);
	void __fastcall MiAboutClick(TObject *Sender);
private:
DWORD         FDiagDword;	// User declarations
otd_addr      filter_addr;
TTreeNode   * db_root;
db_monitor    monitor;
        DWORD Fapply_mask;
void        __fastcall monitor_handler(DWORD code,TCHAR * text);
void        __fastcall activate_monitor(bool active);
void        __fastcall WndProc(Messages::TMessage &Message);
void        __fastcall on_module_state(GKHANDLE mod,DWORD state) ;
void        __fastcall on_module_config_change(GKHANDLE mod,LPMODULE_CONFIG_DATA mcd);
LRESULT     __fastcall on_gkhandle_notify (GKHANDLE from,LPNOTIFY_CODE nc,DWORD mask);
void        __fastcall after_set_gkhandle();
void        __fastcall setup_db_tree();
TTreeNode * __fastcall find   (DWORD addr);
TTreeNode * __fastcall find_pu(DWORD addr);
TTreeNode * __fastcall find_cp(DWORD addr,TTreeNode * parent = 0);
TTreeNode * __fastcall find_sb(DWORD addr,TTreeNode * parent = 0);
TTreeNode * __fastcall add_cp (DWORD addr);
TTreeNode * __fastcall add_pu (DWORD addr);
TTreeNode * __fastcall add_sb (DWORD addr);
TTreeNode * __fastcall add_tree_item(otd_proto & op);
int         __fastcall get_tlmmodule_rctext(lpotd_addr adr, int type,TCHAR * name,int bsz);
void        __fastcall on_dblow_change(DWORD code,LPDBLOW_CHANGES_STRUCT dcs);
int         __fastcall get_dbentry_data(DWORD addr,DWORD parts,DWORD low,DWORD hi,LPBYTE buffer,DWORD bsz,sotd_proto & sop);
void        __fastcall show_param_panel(TTreeNode * node,bool show);
void        __fastcall update_view(DWORD addr,DWORD parts,DWORD low,DWORD hi,bool upd);
void        __fastcall setup_view(sotd_proto & sop);
TListItem * __fastcall create_param_item(DWORD number);
void        __fastcall setup_timestamp(LPFILETIME ftm);
void        __fastcall setup_diag     (LPDWORD diag   );
bool        __fastcall setup_param    (lpotd_data data);
void        __fastcall setup_pd       (lpotd_data data);
void        __fastcall setup_cp_pd    (lpotd_data data);
void        __fastcall setup_pu_pd    (lpotd_data data);
void        __fastcall setup_cm       (lpotd_data data);

void        __fastcall setup_pu_panel(otd_addr & addr);
void        __fastcall setup_cp_panel(otd_addr & addr);
void        __fastcall setup_sb_panel(otd_addr & addr);
void        __fastcall setup_settings_panel();


void        __fastcall SetDiagDword   (DWORD value);
UnicodeString  __fastcall get_pdiag_text (DWORD value,bool prn_val = true);
UnicodeString  __fastcall get_state_text (DWORD value,bool prn_val = true);
UnicodeString  __fastcall get_diag_text  (DWORD diag,bool prn_val= true);
UnicodeString  __fastcall get_part_diag_text(DWORD value,bool prn_val= true);
void        __fastcall set_node_image   (TTreeNode * node,DWORD diag);
void        __fastcall update_node_image(TTreeNode * node);
void        __fastcall update_node_text (TTreeNode * node);

void        __fastcall calc_value();
void        __fastcall Setapply_mask(DWORD value);
void        __fastcall apply_sb(otd_addr & addr);
void        __fastcall apply_cp(otd_addr & addr);
void        __fastcall apply_pu(otd_addr & addr);
void        __fastcall apply_settings();



public:		// User declarations
        __fastcall TDBLowForm(TComponent* Owner,HWND parent ,GKHANDLE handle);
static int WINAPI node_comparer(LPARAM p1,LPARAM p2,LPARAM p3);
protected:
        void __fastcall OnWMGetMinmaxInfo(TMessage & msg);
        void __fastcall OnWMMoving(TMessage & msg);
        __property DWORD DiagDword  = { read=FDiagDword ,write = SetDiagDword};
        __property DWORD apply_mask  = { read=Fapply_mask, write=Setapply_mask };
        BEGIN_MESSAGE_MAP
                VCL_MESSAGE_HANDLER(WM_GETMINMAXINFO, TMessage, OnWMGetMinmaxInfo)
                VCL_MESSAGE_HANDLER(WM_MOVING, TMessage, OnWMMoving)
        END_MESSAGE_MAP(TGKModuleForm)
void __fastcall enum_orders();
};

#pragma warn .8027

//---------------------------------------------------------------------------
extern PACKAGE TDBLowForm *DBLowForm;
//---------------------------------------------------------------------------

inline int WINAPI TDBLowForm::node_comparer(LPARAM p1,LPARAM p2,LPARAM p3)
{
 sotd_addr a1((DWORD)((TTreeNode *)p1)->Data);
 sotd_addr a2((DWORD)((TTreeNode *)p2)->Data);
 return otd_addr_cmp(&a1,&a2);
}

#endif
