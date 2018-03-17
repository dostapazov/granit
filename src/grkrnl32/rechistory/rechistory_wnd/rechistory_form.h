//---------------------------------------------------------------------------

#ifndef rechistory_formH
#define rechistory_formH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "gkmodule_form.h"
#include <rechistory.h>
#include <System.ImageList.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.ImgList.hpp>
#include <Vcl.ToolWin.hpp>
#include <ibclass.h>
#include <Vcl.ExtCtrls.hpp>
#include <text_viewer.hpp>
#include "about_common_frame.h"
//---------------------------------------------------------------------------

typedef KeWin::TTextViewer<char> TRHMonitor;

class TRecHistroyForm : public TGKModuleForm
{
__published:	// IDE-managed Components
	TStatusBar *StatusBar1;
	TImageList *ImageList1;
	TToolBar *ToolBar1;
	TToolButton *tbStart;
	TToolButton *tbStop;
	TOpenDialog *OpenDialog1;
	TToolButton *ToolButton1;
	TToolButton *bApply;
	TToolButton *bUndo;
	TPageControl *PageControl1;
	TTabSheet *TabSheet1;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TLabel *Label4;
	TSpeedButton *tbSelLib;
	TEdit *db_path;
	TEdit *user;
	TEdit *passw;
	TEdit *cl_lib;
	TTabSheet *TabSheet2;
	TMemo *Memo1;
	TCheckBox *WrChanges;
	TCheckBox *WrAlarms;
	TPageControl *PageControl2;
	TTabSheet *TabSheet3;
	TTabSheet *TabSheet4;
	TMemo *WrSqlQuery;
	TMemo *RdSqlQuery;
	TTabSheet *MonSheet;
	TPanel *Panel1;
	TGroupBox *MonBox;
	TCheckBox *cbStore;
	TCheckBox *cbRead;
	TCheckBox *cbCommit;
	TCheckBox *cbUpdidx;
	TToolButton *ToolButton2;
	TToolButton *tbDbRestore;
	TLabel *Label5;
	TLabel *Label6;
	TEdit *CommitTimeOut;
	TEdit *CommitAfter;
	TEdit *PageSize;
	TLabel *Label7;
	TEdit *Buffers;
	TLabel *Label8;
	TLabel *Label9;
	TEdit *ArchiveDaysLong;
	TLabel *Label10;
	TToolButton *bMaintance;
	TToolButton *bStatistic;
	TToolButton *ToolButton5;
	TTabSheet *tabStats;
	TMemo *StatsMemo;
	TTabSheet *SQL_clear_hist;
	TTabSheet *SQL_idx_update;
	TMemo *SqlClearHist;
	TMemo *SqlUpdIndexes;
	TAboutCommonFrame *AboutCommonFrame1;
	TTabSheet *InstructionSheet;
	TMemo *Memo2;
	TSpeedButton *bMonClear;
	TSpeedButton *bPause;
	TLabel *Label11;
	TEdit *RecFilter;
	void __fastcall tbSelLibClick(TObject *Sender);
	void __fastcall cl_libChange(TObject *Sender);
	void __fastcall bUndoClick(TObject *Sender);
	void __fastcall bApplyClick(TObject *Sender);
	void __fastcall tbStartClick(TObject *Sender);
	void __fastcall tbStopClick(TObject *Sender);
	void __fastcall WrChangesClick(TObject *Sender);
	void __fastcall WrSqlQueryChange(TObject *Sender);
	void __fastcall PageControl2Change(TObject *Sender);
	void __fastcall cbStoreClick(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall PageControl2Resize(TObject *Sender);
	void __fastcall bMonClearClick(TObject *Sender);
	void __fastcall tbDbRestoreClick(TObject *Sender);
	void __fastcall bStatisticClick(TObject *Sender);
	void __fastcall bMaintanceClick(TObject *Sender);
private:	// User declarations
protected:
       DWORD            Fch_mask;
       void __fastcall  set_ch_mask(DWORD chm);
       REC_HIST_CONFIG settings;
       int  in_setup;
       void begin_setup(){++in_setup;}
       void end_setup  (){--in_setup;}
       TRHMonitor  * monitor;
virtual void     __fastcall on_module_state(GKHANDLE mod,DWORD state) ;
virtual void     __fastcall on_module_config_change(GKHANDLE mod,LPMODULE_CONFIG_DATA mcd);
        bool     __fastcall get_module_settings();
        void     __fastcall after_set_gkhandle();
        void     __fastcall setup_controls(LPREC_HIST_CONFIG settings);
	__fastcall TRecHistroyForm(TComponent* Owner);
void    __fastcall begin_call_gkhandle(wchar_t * cmd_name);
void    __fastcall done_call_gkhandle(LRESULT ret,wchar_t * str);
void    __fastcall create_monitor();
LRESULT __fastcall on_gkhandle_notify (GKHANDLE from,LPNOTIFY_CODE nc,DWORD mask);
void    __fastcall on_store    (LPNOTIFY_CODE nc);
void    __fastcall on_read     (LPNOTIFY_CODE nc);
void    __fastcall on_send     (LPNOTIFY_CODE nc);
void    __fastcall on_commit   (LPNOTIFY_CODE nc);
void    __fastcall on_updidx   (LPNOTIFY_CODE nc);
void    __fastcall on_opendb   (LPNOTIFY_CODE nc);
void    __fastcall on_statistic(LPNOTIFY_CODE nc);
void    __fastcall monitor_add_line(char * text);



public:		// User declarations
	__fastcall TRecHistroyForm(TComponent* Owner,HWND,GKHANDLE);
        __property DWORD ch_mask  = {read = Fch_mask, write = set_ch_mask};
  DWORD __fastcall get_notify_mask();

};
//---------------------------------------------------------------------------
extern PACKAGE TRecHistroyForm *RecHistroyForm;
//---------------------------------------------------------------------------
#define MCD_SIZE ( sizeof(MODULE_CONFIG_DATA)+ sizeof(REC_HIST_CONFIG)-sizeof(BYTE))
#endif
