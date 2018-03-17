//---------------------------------------------------------------------------

#ifndef sc_monitorH
#define sc_monitorH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "gkmodule_form.h"
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <ToolWin.hpp>
#include <text_viewer.hpp>
#include <granit_modem.h>
#include <ImgList.hpp>
#include <Menus.hpp>
#include <KeStrm.hpp>
#include <Dialogs.hpp>
#include <gkio_base.h>
#include <System.ImageList.hpp>



//---------------------------------------------------------------------------
class TSCMonitor : public TGKModuleForm
{
__published:	// IDE-managed Components
        TGroupBox *ParamBox;
        TToolBar *ToolBar1;
        TGroupBox *MonitorBox;
        TToolButton *MonClearBtn;
        TGroupBox *GroupBox1;
        TCheckBox *RxEnable;
        TLabel *Label1;
        TEdit *RxCP;
        TLabel *Label2;
        TComboBox *RxTYPE;
        TLabel *Label3;
        TEdit *RxAFB;
        TLabel *Label4;
        TEdit *RxGRP;
        TLabel *Label5;
        TComboBox *RxInf;
        TGroupBox *GroupBox2;
        TLabel *Label6;
        TLabel *Label7;
        TLabel *Label8;
        TEdit *TxCP;
        TComboBox *TxType;
        TEdit *TxAFB;
        TCheckBox *TxEnable;
        TImageList *ImageList1;
        TToolButton *MonPauseBtn;
        TToolButton *FilterBtn;
        TToolButton *ToolButton1;
        TToolButton *ToolButton3;
        TToolButton *ToolButton4;
        TPopupMenu *PopupMenu1;
        TMenuItem *miFALL;
        TMenuItem *miFFILTR;
        TMenuItem *miFFLOW;
        TLabel *Label9;
        TLabel *Label10;
        TCheckBox *DetailEnabled;
        TToolButton *ToolButton6;
        TEdit *FlowFilterText;
        TToolButton *ToolButton7;
        TToolButton *ToolButton2;
        TPopupMenu *MonSaveMenu;
        TMenuItem *SaveToFile;
        TMenuItem *N2;
        TMenuItem *CopyToClipboard;
        TOpenDialog *OpenDialog1;
        TButton *Button1;
        TCheckBox *NoErrorsPrint;
        TColorDialog *ColorDialog1;
        TMenuItem *N1;
        TMenuItem *N3;
        TMenuItem *N4;
        TMenuItem *N5;
        TMenuItem *N6;
        TFontDialog *FontDialog1;
        TMenuItem *N7;
        TToolButton *ToolButton8;
        TCheckBox *ShowSysChannel;
        void __fastcall FormResize(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall MonClearBtnClick(TObject *Sender);
        void __fastcall MonPauseBtnClick(TObject *Sender);
        void __fastcall FilterBtnClick(TObject *Sender);
        void __fastcall ToolButton1Click(TObject *Sender);
        void __fastcall miFALLClick(TObject *Sender);
        void __fastcall RxEnableClick(TObject *Sender);
        void __fastcall TxEnableClick(TObject *Sender);
        void __fastcall RxCPChange(TObject *Sender);
        void __fastcall RxTYPEChange(TObject *Sender);
        void __fastcall RxGRPChange(TObject *Sender);
        void __fastcall RxInfChange(TObject *Sender);
        void __fastcall RxAFBChange(TObject *Sender);
        void __fastcall TxCPChange(TObject *Sender);
        void __fastcall TxAFBChange(TObject *Sender);
        void __fastcall TxTypeChange(TObject *Sender);
        void __fastcall SaveToFileClick(TObject *Sender);
        void __fastcall CopyToClipboardClick(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall FormHide(TObject *Sender);
        void __fastcall Button1Click(TObject *Sender);
        void __fastcall N3Click(TObject *Sender);
        void __fastcall N4Click(TObject *Sender);
        void __fastcall N5Click(TObject *Sender);
        void __fastcall N6Click(TObject *Sender);
        void __fastcall FormDestroy(TObject *Sender);
        void __fastcall N7Click(TObject *Sender);
        void __fastcall FontDialog1Apply(TObject *Sender, HWND Wnd);
private:	// User declarations
bool       settings_change;
bool       mon_success;
bool       mon_pause;
int        filter_mode;
int        fmon_radix;
BOOL       frx_enable;
DWORD      frx_cp,frx_type,frx_afb,frx_group,frx_inf;
BOOL       ftx_enable;
DWORD      ftx_cp,ftx_type,ftx_afb;
otd_addr   filter_addr;
KeWin::TTextViewer<TCHAR>  * monitor;

COLORREF   mon_bkgnd_color;
COLORREF   rx_text_color;
COLORREF   tx_text_color;
COLORREF   err_text_color;
LOGFONT    log_font;

void __fastcall read_settings();
void __fastcall write_settings();
void __fastcall replace_moncolor(COLORREF old_color,COLORREF new_color);



void     __fastcall on_module_state(GKHANDLE mod,DWORD state) ;
void     __fastcall on_module_config_change(GKHANDLE mod,LPMODULE_CONFIG_DATA mcd);
void     __fastcall after_set_gkhandle();
LRESULT  __fastcall on_gkhandle_notify (GKHANDLE from,LPNOTIFY_CODE nc,DWORD mask);
void     __fastcall monitor_bytes(LPBYTE data,DWORD len,DWORD color);
void     __fastcall monitor_data(bool tx,LPMIO_REQUEST io_req,DWORD sz);
bool     __fastcall check_filter(gr_proto & gp,bool tx);
void     __fastcall monitor_details(gr_proto & gp,DWORD color);
void     __fastcall monitor_error(DWORD  err);
void     __fastcall separate_line(DWORD color);
void     __fastcall BeforeDestruction(void);
void     __fastcall get_lines(KeRTL::TStream & stream);
        void  __fastcall SetRxCPNumber(DWORD value);
        DWORD __fastcall GetRxCPNumber();
        void  __fastcall SetRxGRPNumber(DWORD value);
        DWORD __fastcall GetRxGRPNumber();
        void  __fastcall SetRxAFBNumber(DWORD value);
        DWORD __fastcall GetRxAFBNumber();
        void  __fastcall SetTxCPNumber(DWORD value);
        DWORD __fastcall GetTxCPNumber();
        void  __fastcall SetTxAFBNumber(DWORD value);
        DWORD __fastcall GetTxAFBNumber();
        void  __fastcall create_monitor_view();

public:		// User declarations
        __fastcall TSCMonitor(TComponent* Owner,GKHANDLE handle);
void    __fastcall change_filter(DWORD addr);
void    __fastcall add_string(char * str,DWORD color = 0);
protected:
        __property DWORD RxCPNumber  = { read=GetRxCPNumber, write=SetRxCPNumber };
        __property DWORD RxGRPNumber  = { read=GetRxGRPNumber, write=SetRxGRPNumber };
        __property DWORD RxAFBNumber  = { read=GetRxAFBNumber, write=SetRxAFBNumber };
        __property DWORD TxCPNumber  = { read=GetTxCPNumber, write=SetTxCPNumber };
        __property DWORD TxAFBNumber  = { read=GetTxAFBNumber, write=SetTxAFBNumber };

        typedef TGKModuleForm Inherit;

};
//---------------------------------------------------------------------------
extern PACKAGE TSCMonitor *SCMonitor;
//---------------------------------------------------------------------------

#define SCMON_FALL     0
#define SCMON_FFILTER  1
#define SCMON_FFLOW    2

#define REG_KEY_SCMON        L"SCMonitor"
#define REG_DW_MONBKGND      L"MonBkgndColor"
#define REG_DW_TXTEXTCOLOR   L"TxTextColor"
#define REG_DW_RXTEXTCOLOR   L"RxTextColor"
#define REG_DW_ERRTEXTCOLOR  L"ErrTextColor"
#define REG_VAL_MONFONT      L"MonintorFont"

#endif
