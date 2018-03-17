//---------------------------------------------------------------------------

#ifndef mediumdb_frmH
#define mediumdb_frmH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "gkmodule_form.h"
#include <ComCtrls.hpp>
#include <ToolWin.hpp>
#include <ExtCtrls.hpp>
#include <mediumdb.h>
#include <Buttons.hpp>
#include <ImgList.hpp>
#include "record_frame.h"
#include "mdb_currstate.h"
#include "kadrs_editor.h"
#include <Menus.hpp>
#include <System.ImageList.hpp>
#include "about_common_frame.h"
#ifdef __CODEGEARC__
#include <System.ImageList.hpp>
#endif



//---------------------------------------------------------------------------
class TMediumDBFrm : public TGKModuleForm
{
__published:	// IDE-managed Components
        TToolBar *ToolBar1;
        TStatusBar *StatusBar1;
        TTreeView *MdbTree;
        TSplitter *Splitter1;
        TToolButton *StartBtn;
        TToolButton *StopBtn;
        TToolButton *ToolButton4;
        TImageList *ImageList1;
        TToolButton *ApplyBtn;
        TToolButton *UndoBtn;
        TToolButton *ToolButton5;
        TToolButton *ToolButton6;
        TToolButton *CreateBtn;
        TToolButton *DeleteBtn;
        TPageControl *PageControl1;
	TTabSheet *ParamsSheet;
	TTabSheet *AboutSheet;
	TToolButton *tbShowKadrsEditor;
        TMemo *Memo1;
        TTabSheet *CurrStateSheet;
        TToolButton *ToolButton3;
        TToolButton *ToolButton7;
        TPanel *StatePanel;
        TMainMenu *MainMenu1;
        TMenuItem *N1;
        TMenuItem *N2;
        TMenuItem *miStart;
        TMenuItem *miStop;
        TMenuItem *N4;
        TMenuItem *miClose;
        TMenuItem *N7;
        TMenuItem *miAbout;
        TMenuItem *miNew;
        TMenuItem *miDelete;
        TMenuItem *miNewByMask;
        TPanel *SettingsPanel;
        TEdit *EditMdbPath;
        TLabel *Label1;
        TMenuItem *N3;
        TMenuItem *N5;
        TMenuItem *N6;
        TMenuItem *N8;
        TMenuItem *N9;
        TMenuItem *miCollapseAll;
        TMenuItem *miFindByAddr;
        TMenuItem *miFindById;
        TGroupBox *GroupBox1;
        TLabel *RecCountInfo;
        TLabel *Label2;
        TLabel *Label3;
        TLabel *NextRecInfo;
        TLabel *Label4;
        TLabel *Label5;
        TLabel *Label6;
        TLabel *KadrsCountInfo;
        TLabel *NextKadrInfo;
        TLabel *NamesCountInfo;
        TMenuItem *N10;
        TLabel *Label7;
        TLabel *CombCountInfo;
	TMenuItem *miCreateSet;
	TMenuItem *miImportCSV;
	TMenuItem *miExportCSV;
	TMenuItem *N11;
	TAboutCommonFrame *AboutCommonFrame1;
	TTabSheet *OrdersSheet;
	TTreeView *OrdersTree;
	TToolButton *tbCombinesEdit;
	TRecordFrame *RecordFrame1;
        void __fastcall MdbTreeCollapsing(TObject *Sender, TTreeNode *Node,
          bool &AllowCollapse);
        void __fastcall MdbTreeExpanding(TObject *Sender, TTreeNode *Node,
          bool &AllowExpansion);
        void __fastcall MdbTreeCollapsed(TObject *Sender, TTreeNode *Node);
        void __fastcall MdbTreeChange(TObject *Sender, TTreeNode *Node);
        void __fastcall MdbTreeMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall ApplyBtnClick(TObject *Sender);
        void __fastcall UndoBtnClick(TObject *Sender);
        void __fastcall DeleteBtnClick(TObject *Sender);
        void __fastcall CreateBtnClick(TObject *Sender);
        void __fastcall MdbTreeChanging(TObject *Sender, TTreeNode *Node,
          bool &AllowChange);
        void __fastcall StopBtnClick(TObject *Sender);
        void __fastcall StartBtnClick(TObject *Sender);
        void __fastcall RecordFrame1PercentChange(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall tbShowKadrsEditorClick(TObject *Sender);
        void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
        void __fastcall ToolButton3Click(TObject *Sender);
        void __fastcall PageControl1Changing(TObject *Sender,
          bool &AllowChange);
        void __fastcall MdbTreeAddition(TObject *Sender, TTreeNode *Node);
        void __fastcall miAboutClick(TObject *Sender);
        void __fastcall miCloseClick(TObject *Sender);
        void __fastcall miNewByMaskClick(TObject *Sender);
        void __fastcall EditMdbPathChange(TObject *Sender);
        void __fastcall N5Click(TObject *Sender);
        void __fastcall MdbTreeMouseUp(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall MdbTreeMouseMove(TObject *Sender,
          TShiftState Shift, int X, int Y);
        void __fastcall miCollapseAllClick(TObject *Sender);
        void __fastcall miFindByAddrClick(TObject *Sender);
        void __fastcall miFindByIdClick(TObject *Sender);
        void __fastcall N10Click(TObject *Sender);
	void __fastcall miCreateSetClick(TObject *Sender);
	void __fastcall miImportCSVClick(TObject *Sender);
	void __fastcall miExportCSVClick(TObject *Sender);
	void __fastcall PageControl1Change(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall tbCombinesEditClick(TObject *Sender);



private:	// User declarations
MDB_SETTINGS settings;
TMDBCurrentState * current_states;
TTreeNode        * db_root;
TTreeNode        * db_root_combine;
DWORD              FDragRecordId;
DWORD              Fapply_mask;
bool               Fcreate_mode;
bool               Lmb;
bool               Rmb;
TMediumDBFrm     * comb_frm;
static             TKadrsEditor     * kadrs_editor;


void     __fastcall add_current_states();
void     __fastcall apply_settings();

sotd_param_addr
         __fastcall get_node_addr  (TTreeNode * node);
bool     __fastcall get_node_record(TTreeNode *node, mdb_record & rec);

TTreeNode * __fastcall find_by_id  (TTreeNode * root,DWORD id,bool make_visible = false);
TTreeNode * __fastcall find_by_addr(otd_param_addr & addr,bool make_visible = false);
TTreeNode * __fastcall find_pu     (otd_param_addr & addr);
TTreeNode * __fastcall find_cp     (TTreeNode * root,otd_param_addr & addr);
TTreeNode * __fastcall find_sb     (TTreeNode * root,otd_param_addr & addr);
TTreeNode * __fastcall find_obj    (TTreeNode * root,otd_param_addr & addr);

void     __fastcall on_module_config_change(GKHANDLE mod,LPMODULE_CONFIG_DATA mcd);
void     __fastcall on_module_state(GKHANDLE mod,DWORD state);
LRESULT  __fastcall on_gkhandle_notify (GKHANDLE from,LPNOTIFY_CODE nc,DWORD mask);
void     __fastcall on_record_removed(DWORD id);
void     __fastcall on_record_created(DWORD id);
void     __fastcall on_record_changed(DWORD id,DWORD mask);
void     __fastcall after_set_gkhandle();

void     __fastcall mdb_tree_combines_enable   (bool value);
void     __fastcall mdb_tree_records_enable    (bool value);
bool     __fastcall mdb_tree_combines_enabled  (){return db_root_combine  ? true : false;}
bool     __fastcall mdb_tree_records_enabled   (){return db_root          ? true : false;}

void     __fastcall setup_mdb_info();
void     __fastcall setup_orders_tree();


void     __fastcall expand_node(TTreeNode * parent,sotd_param_addr & pa);
void     __fastcall expand_combine();
void     __fastcall set_node_text(TTreeNode * node, mdb_record & rec);
TTreeNode *
         __fastcall add_node(TTreeNode * parent,mdb_record & rec);
        void __fastcall Setapply_mask(DWORD value);
        void __fastcall Setcreate_mode(bool value);

        void __fastcall comb_frm_close(TObject *Sender, TCloseAction & ca);

virtual void __fastcall BeforeDestruction();


public:		// User declarations
        __fastcall TMediumDBFrm(TComponent* Owner,HWND,GKHANDLE);
        __property DWORD DragRecordId  = { read=FDragRecordId, write=FDragRecordId };
        __property DWORD apply_mask    = { read=Fapply_mask, write=Setapply_mask };
        __property bool create_mode    = { read=Fcreate_mode, write=Setcreate_mode };
        __property bool records_enable = { read=mdb_tree_records_enabled , write= mdb_tree_records_enable   };
        __property bool combine_enable = { read=mdb_tree_combines_enabled , write= mdb_tree_combines_enable };
bool    __fastcall select_node_by_id(DWORD);
static UnicodeString    __fastcall get_text(TGKModuleInterface & mod_iface,DWORD id);
static DWORD            __fastcall set_text(TGKModuleInterface & mod_iface,DWORD id,UnicodeString  str);


static LRESULT          __fastcall on_notify_paintvar(TGKModuleInterface & mod_iface,LPNOTIFY_CODE nc);

static bool    __fastcall get_record     (TGKModuleInterface & mod_iface,DWORD Id, mdb_record & rec);
static bool    __fastcall get_record     (TGKModuleInterface & mod_iface,sotd_param_addr & addr, mdb_record & rec);
       bool    __fastcall get_record     (DWORD Id, mdb_record & rec){return get_record(mod_iface,Id,rec);}
       bool    __fastcall get_record     (sotd_param_addr & addr, mdb_record & rec){return get_record(mod_iface,addr,rec);}



};
//---------------------------------------------------------------------------
extern PACKAGE TMediumDBFrm *MediumDBFrm;
//---------------------------------------------------------------------------
#endif
