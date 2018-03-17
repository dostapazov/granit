//---------------------------------------------------------------------------

#ifndef modemmer_frm_embH
#define modemmer_frm_embH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "gkmodule_form.h"
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <modemmer.h>
#include <Buttons.hpp>
#include <ImgList.hpp>
#include <ToolWin.hpp>
#include <Dialogs.hpp>
#include <text_viewer.hpp>
#include <Menus.hpp>
#include <modem.hpp>
#include <System.ImageList.hpp>


//---------------------------------------------------------------------------

#define MSG_MODEMMER_LINE (WM_USER+0x200)
#define MSG_MODEMMER_TXRX (WM_USER+0x201)

struct  modem_data
{
  modem_addr      addr;
  GKHANDLE        hModem;
  modem_data(GKHANDLE modem,DWORD mn,DWORD ln):hModem(modem),addr(mn,ln){};
  modem_data(GKHANDLE modem,MODEM_ADDR ma):hModem(modem),addr(ma){};
  modem_data():hModem(NULL),addr(-1){};
  modem_data & operator = (modem_data & md){hModem = md.hModem;addr.addr = md.addr.addr;return * this;}
};

inline modem_data  __fastcall get_modem_data(TTreeNode * node)
{
 modem_data md;
  if(node->Data)
  md = *((modem_data*)node->Data);
 return md;
}

typedef KeWin::TTextViewer<char> TModemmerMonitor;

class TModemmerFrm : public TGKModuleForm
{
__published:	// IDE-managed Components
        TTreeView *ModemsTree;
        TPageControl *PageControl1;
        TStatusBar *StatusBar1;
        TTabSheet *TabSheet1;
        TTabSheet *TabSheet2;
        TGroupBox *ModemBox;
        TLabel *Label2;
        TLabel *ModemMaxLinesCount;
        TLabel *Label3;
        TLabel *ModemLinesCount;
        TGroupBox *LineBox;
        TLabel *Label4;
        TStaticText *LostConnection;
        TTimer *Timer1;
        TToolBar *ToolBar1;
        TToolButton *StartBtn;
        TToolButton *StopBtn;
        TToolButton *ToolButton3;
        TToolButton *PropBtn;
        TToolButton *ViewBtn;
        TImageList *ImageList1;
        TToolButton *ToolButton6;
        TPanel *MnPanel;
        TLabel *Label1;
        TEdit *Edit1;
        TUpDown *ModemNumber;
        TRadioGroup *MonitorParam;
        TToolButton *RefreshBtn;
        TGroupBox *GroupBox1;
        TStaticText *LineWorkTime;
        TStaticText *RxQuality;
        TStaticText *RxSpeed;
        TStaticText *RxErrorsCount;
        TStaticText *RxSpeedMiddle;
        TStaticText *RxBytesCount;
        TStaticText *RxKadrsCount;
        TStaticText *RxSpeedMin;
        TStaticText *RxSpeedMax;
        TGroupBox *GroupBox2;
        TStaticText *TxQuality;
        TStaticText *TxSpeed;
        TStaticText *TxErrorsCount;
        TStaticText *TxSpeedMiddle;
        TStaticText *TxBytesCount;
        TStaticText *TxKadrsCount;
        TStaticText *TxSpeedMin;
        TStaticText *TxSpeedMax;
        TLabel *Label5;
        TLabel *Label6;
        TLabel *Label7;
        TLabel *Label8;
        TLabel *Label9;
        TLabel *Label18;
        TLabel *Label19;
        TLabel *Label20;
        TLabel *Label10;
        TLabel *Label11;
        TLabel *Label12;
        TLabel *Label13;
        TLabel *Label14;
        TLabel *Label15;
        TLabel *Label16;
        TLabel *Label21;
        TLabel *Label22;
        TToolButton *AddModemBtn;
        TToolButton *RemoveModemBtn;
        TToolButton *ToolButton4;
        TOpenDialog *OpenDialog1;
        TGroupBox *MonitorBox;
        TTabSheet *RedirectSheet;
        TListView *RedirectList;
        TBitBtn *RedirNewBtn;
        TBitBtn *RedirDeleteBtn;
        TBitBtn *RedirEditBtn;
        TCheckBox *RedirectEnable;
        TGroupBox *FilterBox;
        TLabel *Label23;
        TEdit *FilterModem;
        TLabel *Label24;
        TEdit *FilterLine;
        TEdit *FilterFA;
        TLabel *Label25;
        TToolButton *NewWindowBtn;
        TMainMenu *MainMenu1;
        TMenuItem *N1;
        TMenuItem *N7;
        TMenuItem *MiWndCopy;
        TMenuItem *MiAbout;
        TMenuItem *N10;
        TMenuItem *MiClose;
        TStaticText *ModemmerRxKadrs;
        TStaticText *ModemmerRxBytes;
        TLabel *Label17;
        TLabel *Label26;
        TLabel *Label27;
        TStaticText *ModemmerTxKadrs;
        TStaticText *ModemmerTxBytes;
        TLabel *Label28;
        TLabel *Label29;
        TStaticText *TxQueueSize;
        TEdit *RedirFrom;
        TEdit *RedirTo;
        TCheckBox *RedirOnly;
        TEdit *RedirFA;
        TCheckBox *RedirDisabled;
        TSplitter *Splitter1;
        TPopupMenu *PopupMenu1;
        TMenuItem *MiAddModem;
        TMenuItem *MiRemoveModem;
        TMenuItem *N5;
        TMenuItem *MiStartModem;
        TMenuItem *MiStopModem;
        TMenuItem *N6;
        TMenuItem *MiSettings;
        TMenuItem *MiView;
        TMenuItem *N2;
        TMenuItem *MiRefresh;
        TTabSheet *TabSheet3;
        TLabel *Label30;
        TLabel *Label31;
        TToolButton *ToolButton1;
        TToolButton *ToolButton2;
        TGroupBox *GroupBox3;
        TCheckBox *RxMonEnable;
        TCheckBox *TxMonEnable;
        TCheckBox *RedirMonEnable;
        TCheckBox *PausedMon;
        TGroupBox *GroupBox4;
        TLabel *Label32;
        TCheckBox *MonitorData;
        TEdit *MonLinesCount;
        TBitBtn *ClearMonitor;
        TToolButton *SetMNBtn;
        TLabel *Label33;
        TLabel *ModemFlags;
        void __fastcall FormShow(TObject *Sender);
        void __fastcall FormHide(TObject *Sender);
        void __fastcall ModemsTreeChange(TObject *Sender, TTreeNode *Node);
        void __fastcall PageControl1Change(TObject *Sender);
        void __fastcall Timer1Timer(TObject *Sender);
        void __fastcall SetModemNumberBtnClick(TObject *Sender);
        void __fastcall StartBtnClick(TObject *Sender);
        void __fastcall StopBtnClick(TObject *Sender);
        void __fastcall ModemNumberChanging(TObject *Sender,
          bool &AllowChange);
        void __fastcall Edit1Change(TObject *Sender);
        void __fastcall PropBtnClick(TObject *Sender);
        void __fastcall ViewBtnClick(TObject *Sender);
        void __fastcall RefreshBtnClick(TObject *Sender);
        void __fastcall AddModemBtnClick(TObject *Sender);
        void __fastcall RemoveModemBtnClick(TObject *Sender);
        void __fastcall FormDestroy(TObject *Sender);
        void __fastcall FormResize(TObject *Sender);
        void __fastcall MonitorParamClick(TObject *Sender);
        void __fastcall FilterModemChange(TObject *Sender);
        void __fastcall NewWindowBtnClick(TObject *Sender);
        void __fastcall MiCloseClick(TObject *Sender);
        void __fastcall MiAboutClick(TObject *Sender);
        void __fastcall RxMonEnableClick(TObject *Sender);
        void __fastcall TxMonEnableClick(TObject *Sender);
        void __fastcall ClearMonitorClick(TObject *Sender);
        void __fastcall PausedMonClick(TObject *Sender);
        void __fastcall RedirectListSelectItem(TObject *Sender,
          TListItem *Item, bool Selected);
        void __fastcall RedirDeleteBtnClick(TObject *Sender);
        void __fastcall RedirectEnableClick(TObject *Sender);
        void __fastcall RedirFromChange(TObject *Sender);
        void __fastcall RedirNewBtnClick(TObject *Sender);
        void __fastcall RedirEditBtnClick(TObject *Sender);
        void __fastcall RedirOnlyClick(TObject *Sender);
        void __fastcall RedirMonEnableClick(TObject *Sender);
        void __fastcall ModemsTreeDeletion(TObject *Sender,
          TTreeNode *Node);
        void __fastcall Splitter1Moved(TObject *Sender);
        void __fastcall ModemsTreeDblClick(TObject *Sender);
        void __fastcall N1Click(TObject *Sender);
        void __fastcall ModemsTreeKeyPress(TObject *Sender, char &Key);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall ToolButton1Click(TObject *Sender);
        void __fastcall MonLinesCountChange(TObject *Sender);


private:	// User declarations
bool              filter_enabled,rx_mon_enabled,tx_mon_enabled,redir_mon_enabled;
DWORD             filter_modem,filter_line,filter_fa;
TModemmerMonitor* monitor;
MODEM_ADDR        selected_addr;
GKHANDLE          selected_modem_handle;
DWORD             line_connect_time;

void        __fastcall end_module_command(GKHANDLE mod,DWORD ret);
void        __fastcall on_modem_change();
void        __fastcall on_line_change ();
void        __fastcall after_set_gkhandle();
LRESULT     __fastcall on_gkhandle_notify(GKHANDLE from,LPNOTIFY_CODE nc,DWORD mask);
void        __fastcall on_modemmer_txrx     (LPMPROTO_HEADER mph,DWORD sz,DWORD code);
void        __fastcall on_modem_line_state  (LPMODEM_LINE_STATE mls);
void        __fastcall on_modem_change_number(LPMODEM_CHANGE_NUMBER);
TTreeNode * __fastcall add_modem_node(GKHANDLE modem,MODEM_ADDR &addr);
TTreeNode * __fastcall add_line_node (TTreeNode * modem,LPMODEM_LINE_PROPS mlp);
TTreeNode * __fastcall add_line_node (TTreeNode * modem,MODEM_ADDR & addr);
void        __fastcall update_line_stats();
void        __fastcall update_line_state(MODEM_ADDR & ma ,DWORD state);
void        __fastcall update_modem_state(GKHANDLE h_modem,DWORD state);
void        __fastcall monitor_string(AnsiString & s,DWORD color = -1);
void        __fastcall monitor_data(DWORD fa, LPBYTE data,DWORD data_size);
bool        __fastcall check_monitor_filter(LPBYTE b2,DWORD len,DWORD fa);
void        __fastcall on_module_state(GKHANDLE mod,DWORD state) ;
void        __fastcall on_module_config_change(GKHANDLE mod,LPMODULE_CONFIG_DATA mcd);
void        __fastcall setup_redirect_list();
bool        __fastcall make_redirect_entry(LPREDIRECT_ENTRY re);
void        __fastcall on_redir_change(DWORD index,bool add);
void        __fastcall AddRedirectItem(DWORD idx,LPREDIRECT_ENTRY re);

virtual __fastcall TModemmerFrm(TComponent* Owner):TGKModuleForm(Owner){}
public:		// User declarations
        __fastcall TModemmerFrm(TComponent* Owner,HWND owner_hwnd,GKHANDLE handle);
protected:
TTreeNode * worked_modems;
TTreeNode * free_modems;
TTreeNode * __fastcall find_modem(DWORD number,GKHANDLE handle);
TTreeNode * __fastcall find_modem(GKHANDLE mod);
TTreeNode * __fastcall find_modem_line(TTreeNode * modem_node,DWORD line);
TTreeNode * __fastcall find_node (MODEM_ADDR & addr);

LPMODEM_PROPS      __fastcall get_modem_props(GKHANDLE hModem,LPBYTE buffer,DWORD bsz);
LPMODEM_LINE_PROPS __fastcall get_line_props(GKHANDLE hModem,DWORD line,LPBYTE buffer,DWORD bsz);

void __fastcall Tree_setup_modems();
void __fastcall Tree_setup_freemodems();
void __fastcall Tree_setup_modem_lines(TTreeNode * node,MODEM_ADDR & addr);
void    __fastcall on_added_module  (GKHANDLE owner,GKHANDLE mod,DWORD mod_type);
void    __fastcall on_removed_module(GKHANDLE owner,GKHANDLE mod,DWORD mod_type);
HWND __fastcall get_owner_window();

};
//---------------------------------------------------------------------------
extern PACKAGE TModemmerFrm *ModemmerFrm;
//---------------------------------------------------------------------------

void __fastcall monitor_otd       (TModemmerMonitor * monitor, LPBYTE data, DWORD data_size);
void __fastcall monitor_otd_medium(TModemmerMonitor * monitor, LPBYTE data, DWORD data_size);

#endif
