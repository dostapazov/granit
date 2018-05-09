//---------------------------------------------------------------------------

#ifndef grscfrmH
#define grscfrmH
//---------------------------------------------------------------------------
#ifndef RC_INVOKED

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "gkmodule_form.h"
#include <granit_modem.h>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Menus.hpp>
#include <ToolWin.hpp>
#include <ImgList.hpp>
#include "sc_monitor.h"
#include "cp_frame.h"
#include "sc_base_frame.h"
#include "floor_frame.h"
#include "settings_frame1.h"
#include "grp_frame.h"
#include "sc_data_frame.h"
#include "sc_base_data_frame.h"
#include "sc_grp_data_frame.h"
#include "sc_cp_data_frame.h"
#include <System.ImageList.hpp>
#include "about_common_frame.h"

#define MSG_SHOW_MONITOR      (WM_USER+525)
#define MSG_ACTIVATE_MON_PAGE (WM_USER+526)

class TGroupFrame;

//---------------------------------------------------------------------------
#pragma warn -8027

class TSCGranFrm : public TGKModuleForm
{
__published:	// IDE-managed Components
        TStatusBar *StatusBar1;
        TMainMenu *MainMenu1;
        TMenuItem *N1;
        TMenuItem *N2;
        TMenuItem *miAbout;
        TMenuItem *miClose;
        TToolBar *ToolBar1;
        TTreeView *SCTree;
        TSplitter *Splitter1;
        TPageControl *PageControl1;
        TTabSheet *SettingsPage;
        TTabSheet *ViewPage;
        TToolButton *MonitorBtn;
        TImageList *ImageList1;
        TToolButton *ApplyBtn;
        TToolButton *UndoBtn;
        TToolButton *ToolButton4;
        TToolButton *ToolButton1;
        TToolButton *StartBtn;
        TToolButton *StopBtn;
        TToolButton *CreateBtn;
        TToolButton *DelBtn;
        TToolButton *NewWndBtn;
        TMenuItem *N3;
        TMenuItem *miStart;
        TMenuItem *miStop;
        TToolButton *RefreshBtn;
        TCPFrame *CPFrame;
        TFloorFrame *FloorFrame;
        TSCSettingsFrm *SCSettingsFrm1;
        TGroupFrame *GroupFrame1;
        TMenuItem *N4;
        TMenuItem *MiCreate;
        TMenuItem *MiDelete;
        TMenuItem *N6;
        TMenuItem *MiApply;
        TMenuItem *MiUndo;
        TSCGroupDataFrame *GrpDataFrm;
        TSCCpDataFrame *CpDataFrm;
        TTabSheet *TabSheet1;
        TLabel *Label31;
        TToolButton *FlushConfigBtn;
        TToolButton *ToolButton3;
        TTabSheet *MonitorSheet;
	TAboutCommonFrame *AboutCommonFrame1;
        void __fastcall miCloseClick(TObject *Sender);
        void __fastcall MonitorBtnClick(TObject *Sender);
        void __fastcall NewWndBtnClick(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall RefreshBtnClick(TObject *Sender);
        void __fastcall SCTreeChange(TObject *Sender, TTreeNode *Node);
        void __fastcall SCTreeChanging(TObject *Sender, TTreeNode *Node,
          bool &AllowChange);
        void __fastcall StartBtnClick(TObject *Sender);
        void __fastcall StopBtnClick(TObject *Sender);
        void __fastcall DelBtnClick(TObject *Sender);
        void __fastcall CreateBtnClick(TObject *Sender);
        void __fastcall PageControl1Changing(TObject *Sender,
          bool &AllowChange);
        void __fastcall ApplyBtnClick(TObject *Sender);
        void __fastcall UndoBtnClick(TObject *Sender);
        void __fastcall PageControl1Change(TObject *Sender);
        void __fastcall Splitter1Moved(TObject *Sender);
        void __fastcall miAboutClick(TObject *Sender);
        void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
        void __fastcall FormDestroy(TObject *Sender);
        void __fastcall FlushConfigBtnClick(TObject *Sender);
private  :	// User declarations
TSCBaseFrame* prop_frame;
TSCBaseFrame* data_frame;
TSCMonitor  * sc_monitor;
TTreeNode   * sc_root;
DWORD         Fch_mask;
DWORD         selected_type;
        bool Fcreate_mode;

        void __fastcall on_monitor_destroy(TObject * sender);
        void __fastcall Setch_mask(DWORD value);
        void __fastcall Setcreate_mode(bool value);
protected:
void     __fastcall SelectFrames(otd_addr & addr);
void     __fastcall on_module_state(GKHANDLE mod,DWORD state) ;
void     __fastcall on_module_config_change(GKHANDLE mod,LPMODULE_CONFIG_DATA mcd);
LRESULT  __fastcall on_gkhandle_notify (GKHANDLE from,LPNOTIFY_CODE nc,DWORD mask);
void     __fastcall after_set_gkhandle();
void     __fastcall on_floor_state( LPMODEM_LINE_STATE mls);

void     __fastcall set_node_image(TTreeNode * node);
void     __fastcall set_node_image(TTreeNode * node,DWORD diag);
void     __fastcall on_sc_added       (LPGRSC_ACR acr);
void     __fastcall on_sc_changed     (LPGRSC_ACR acr);
void     __fastcall on_sc_removed     (LPGRSC_ACR acr);
void     __fastcall on_sc_data_changed(LPGRSC_ACR acr);

grsc_settings&  __fastcall get_settings( );
void        __fastcall init_form();
TTreeNode * __fastcall find_floor_node(DWORD num);
void        __fastcall enum_floors(TTreeNode * root);
TTreeNode * __fastcall add_floor(TTreeNode * root,DWORD num);

void        __fastcall enum_cp     (TTreeNode * floor);
TTreeNode * __fastcall add_cp      (GR_CP_PROPS & cpp,TTreeNode * floor = NULL);
TTreeNode * __fastcall add_cp      (DWORD addr);
TTreeNode * __fastcall find_cp_node(DWORD num,TTreeNode * fllor = NULL);

void        __fastcall enum_group     (TTreeNode * cp);
TTreeNode * __fastcall add_group      (GR_GRP_PROPS & grp,TTreeNode * cp = NULL);
TTreeNode * __fastcall add_group      (DWORD addr);
TTreeNode * __fastcall find_group_node(DWORD addr,TTreeNode * cp_node = NULL);

TTreeNode * __fastcall find_node      (DWORD addr);

void        __fastcall setup_form_sizes();

void        __fastcall on_changes(TSCBaseFrame * sender){ch_mask = sender->changes_mask;}
        BEGIN_MESSAGE_MAP
                VCL_MESSAGE_HANDLER(WM_SYSCOMMAND, TMessage, OnSysCommand)
                VCL_MESSAGE_HANDLER(MSG_SHOW_MONITOR, TMessage, OnMsgShowMonitor)
        END_MESSAGE_MAP(TGKModuleForm)
public   :	// User declarations
 virtual  __fastcall TSCGranFrm(TComponent* Owner);
 virtual  __fastcall TSCGranFrm(TComponent* Owner,HWND owner_hwnd,GKHANDLE handle);
        __property DWORD ch_mask  = { read=Fch_mask, write=Setch_mask };
        __property bool create_mode  = { read=Fcreate_mode, write=Setcreate_mode };
void    __fastcall sort_nodes(TTreeNode * node);
void     __fastcall begin_call_gkhandle(const TCHAR * cmd_name);
void     __fastcall done_call_gkhandle(LRESULT ret,const TCHAR * str = NULL);
        void __fastcall OnSysCommand(TMessage & msg);
        void __fastcall OnMsgShowMonitor(TMessage & msg);

typedef TGKModuleForm Inherit;
};

#pragma warn .8027
//---------------------------------------------------------------------------
extern PACKAGE TSCGranFrm *SCGranFrm;
//---------------------------------------------------------------------------

int WINAPI node_comparer(LPARAM p1,LPARAM p2,LPARAM p3)
{
 sotd_addr a1((DWORD)((TTreeNode *)p1)->Data);
 sotd_addr a2((DWORD)((TTreeNode *)p2)->Data);
 return otd_addr_cmp(&a1,&a2);
}

#endif
#endif
