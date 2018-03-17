//---------------------------------------------------------------------------

#ifndef GrIpxModFrameH
#define GrIpxModFrameH
//---------------------------------------------------------------------------

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "gkmodule_form.h"
#include <ComCtrls.hpp>
#include <ToolWin.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <gripxmod.h>
#include <text_viewer.hpp>
#include <ImgList.hpp>
#include <Graphics.hpp>
#include <System.ImageList.hpp>
#include "about_common_frame.h"

//---------------------------------------------------------------------------

#define MSG_GRNET_NOTIFY  (WM_USER+1024)
#ifdef __BORLANDC__
#pragma warn -8027
#endif

class TGrIpxModuleWnd : public TGKModuleForm
{
__published:	// IDE-managed Components
        TPageControl *PageControl1;
        TTabSheet *TabSheet2;
        TTabSheet *TabSheet1;
        TGroupBox *Server;
        TLabel *Label2;
        TLabel *Label3;
        TEdit *ServerName;
        TEdit *ServerSocket;
        TGroupBox *Common;
        TLabel *Label1;
        TEdit *PUNumberEdit;
        TUpDown *PUNumber;
        TRadioButton *RadioButton1;
        TRadioButton *RadioButton2;
        TGroupBox *Client;
        TLabel *Label7;
        TLabel *Label8;
        TEdit *ClientName;
        TEdit *ClientSocket;
        TTreeView *ClientsTree;
        TSplitter *Splitter1;
        TRadioButton *dummy;
        TTabSheet *TabSheet3;
        TStatusBar *StatusBar1;
        TPanel *Panel1;
        TRadioGroup *RadioGroup1;
        TGroupBox *GroupBox4;
        TLabel *Label9;
        TLabel *Label10;
        TLabel *Label11;
        TEdit *filtrCpEdit;
        TEdit *filtrGrpEdit;
        TGroupBox *MonitorBox;
        TBitBtn *ApplyBtn;
        TBitBtn *UndoBtn;
        TGroupBox *TxParam;
        TLabel *Label4;
        TEdit *RetryCountEdit;
        TUpDown *RetryCount;
        TLabel *Label5;
        TEdit *RetryTimeoutEdit;
        TLabel *Label6;
        TLabel *Label12;
        TEdit *ModemNumberEdit;
        TListView *DiagView;
        TImageList *ImageList1;
        TComboBox *filterFaEdit;
        TGroupBox *GroupBox1;
        TLabel *Label14;
        TGroupBox *GroupBox2;
        TLabel *Label16;
        TLabel *Label13;
        TLabel *Label15;
        TLabel *Label17;
        TLabel *Label18;
        TLabel *tit_exist;
        TLabel *tit_nowork;
        TLabel *ts_exist;
        TLabel *ts_nowork;
        TCheckBox *RxEnableMonitor;
        TCheckBox *TxEnableMonitor;
        TSpeedButton *RefreshBtn;
        TCheckBox *PausedMonitor;
        TSpeedButton *ClearMonBtn;
        TToolBar *ToolBar1;
        TToolButton *StartBtn;
        TToolButton *StopBtn;
        TToolButton *ToolButton2;
        TToolButton *CopyWndBtn;
        TPaintBox *PaintBox1;
        TGroupBox *ServerTUTR;
        TListView *TuClientsList;
        TEdit *TuClientName;
        TEdit *TuClientSocket;
        TSpeedButton *SpeedButton2;
        TSpeedButton *SpeedButton3;
        TToolButton *ToolButton3;
        TCheckBox *TuEnable;
        TTabSheet *TabSheet4;
        TLabel *Label30;
        TLabel *Label31;
        TEdit *PuName;
        TLabel *Label19;
	TAboutCommonFrame *AboutCommonFrame1;
        void __fastcall RadioButton1Click(TObject *Sender);
        void __fastcall UndoBtnClick(TObject *Sender);
        void __fastcall ApplyBtnClick(TObject *Sender);
        void __fastcall RetryTimeoutEditChange(TObject *Sender);
        void __fastcall PageControl1Change(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall FormHide(TObject *Sender);
        void __fastcall RadioGroup1Click(TObject *Sender);
        void __fastcall filtrCpEditChange(TObject *Sender);
        void __fastcall filtrGrpEditChange(TObject *Sender);
        void __fastcall ClientsTreeChange(TObject *Sender,
          TTreeNode *Node);
        void __fastcall FormDestroy(TObject *Sender);
        void __fastcall FormResize(TObject *Sender);
        void __fastcall PUNumberEditChange(TObject *Sender);
        void __fastcall ServerNameChange(TObject *Sender);
        void __fastcall ClientSocketChange(TObject *Sender);
        void __fastcall ModemNumberEditChange(TObject *Sender);
        void __fastcall StartBtnClick(TObject *Sender);
        void __fastcall StopBtnClick(TObject *Sender);
        void __fastcall DiagViewChange(TObject *Sender, TListItem *Item,
          TItemChange Change);
        void __fastcall DiagViewCompare(TObject *Sender, TListItem *Item1,
          TListItem *Item2, int Data, int &Compare);
        void __fastcall filterFaEditChange(TObject *Sender);
        void __fastcall RefreshBtnClick(TObject *Sender);
        void __fastcall RxEnableMonitorClick(TObject *Sender);
        void __fastcall TxEnableMonitorClick(TObject *Sender);
        void __fastcall ClearMonBtnClick(TObject *Sender);
        void __fastcall CopyWndBtnClick(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall TuClientsListSelectItem(TObject *Sender,
          TListItem *Item, bool Selected);
        void __fastcall SpeedButton2Click(TObject *Sender);
        void __fastcall SpeedButton3Click(TObject *Sender);
        void __fastcall TuEnableBtnClick(TObject *Sender);
        void __fastcall PuNameChange(TObject *Sender);
private:	// User declarations
TTreeNode       * RootNode;
BOOL            settings_up;
GRNET_SETTINGS  current_settings;
DWORD           settings_change_mask;
KeWin::TTextViewer<char> * monitor;
DWORD     monitor_switch;
DWORD     filtrCp,filtrGrp,filtrFa;
DWORD     selected_client_id;
DWORD     selected_line_number;
void     __fastcall set_form_caption();
void     __fastcall after_set_gkhandle();
void     __fastcall set_settings(LPGRNET_SETTINGS gs);
void     __fastcall setup_settings();
void     __fastcall setup_tu_clients();
void     __fastcall apply_settings();
void     __fastcall on_client_state(LPMODEM_LINE_STATE mls);
void     __fastcall on_line_state  (LPMODEM_LINE_STATE mls);
LRESULT  __fastcall on_gkhandle_notify (GKHANDLE from,LPNOTIFY_CODE nc,DWORD mask);
void     __fastcall on_module_state(GKHANDLE mod,DWORD state) ;
void     __fastcall on_module_config_change(GKHANDLE mod,LPMODULE_CONFIG_DATA mcd);
void     __fastcall monitor_grnet_kadr (LPGRNET_KADR grnet_kadr,DWORD len, BOOL tx);
void     __fastcall monitor_string(char * text1,char * text2 = NULL,DWORD color = -1);
void     __fastcall monitor_gratony_kadr( lp_gratony_kadr  kadr,DWORD color);
bool     __fastcall check_monitor_filtr(LPGRNET_KADR grnet_kadr,DWORD len);
TTreeNode * __fastcall add_line (DWORD line_number); 
void     __fastcall scan_lines();
TTreeNode * __fastcall find_line_node(DWORD num);
void     __fastcall scan_diag();
void     __fastcall handle_diag(DWORD cp_num,WORD diag,WORD exist);
void     __fastcall update_tu_client(DWORD idx);
bool     __fastcall is_tu_enabled(char * name,WORD sock_num);
bool     __fastcall enable_tu_client(char * name,WORD socket,bool enable);
void     __fastcall begin_command(char * text);
void     __fastcall end_command  (DWORD cmd_ret);

public:		// User declarations
        __fastcall TGrIpxModuleWnd(TComponent* Owner,HWND p_wnd,GKHANDLE handle);
protected:
        void __fastcall OnGrnetNotify(GKHANDLE from,LPNOTIFY_CODE nc,DWORD mask);
        BEGIN_MESSAGE_MAP
                VCL_MESSAGE_HANDLER(MSG_GRNET_NOTIFY, TMessage, OnMsgGrnetNotify)
        END_MESSAGE_MAP(TGKModuleForm)
        void __fastcall OnMsgGrnetNotify(TMessage & msg);
        void __fastcall Flash(DWORD st);        
};
//---------------------------------------------------------------------------
extern PACKAGE TGrIpxModuleWnd *GrIpxModuleWnd;
//---------------------------------------------------------------------------
#define GRNET_PARAM_PAGE      0
#define GRNET_MONITOR_PAGE    1
#define GRNET_DIAG_PAGE       2
#define GRNET_TUTR_PAGE       3


#ifdef __BORLANDC__
#pragma warn .8027
#endif


#endif
