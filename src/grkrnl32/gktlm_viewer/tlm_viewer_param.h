//---------------------------------------------------------------------------

#ifndef tlm_viewer_paramH
#define tlm_viewer_paramH
//---------------------------------------------------------------------------
#include "gktlm_viewer_mod.hpp"

#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.ComCtrls.hpp>

//---------------------------------------------------------------------------

#include <otdm_proto.h>
//#include <otd_proto_medium.h>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.CategoryButtons.hpp>
#include <Vcl.Grids.hpp>
#include <Vcl.ValEdit.hpp>
#include <Vcl.ButtonGroup.hpp>

#define WNDCM_ADD_KADR  WM_USER+1000

typedef void __fastcall (__closure * requerst_kadr_list_proc)(modem_addr );
using namespace System;

class TTlmViewerParams : public TForm
{
__published:	// IDE-managed Components
	TPageControl *PageControl1;
	TBitBtn *bOk;
	TBitBtn *bCancel;
	TTabSheet *TabSheet1;
	TListView *KadrList;
	TTabSheet *TabSheet2;
	TLabel *Label1;
	TEdit *DataFolder;
	TSpeedButton *bSelDataFolder;
	TBitBtn *BitBtn1;
	TLabel *Label2;
	TEdit *ExportFolder;
	TSpeedButton *bSelExporFolder;
	TLabel *Label3;
	TEdit *ExportViewer;
	TSpeedButton *bSelViewer;
	TOpenDialog *SelViewerDlg;
	TCheckBox *cbDoubleBuffered;
	TGroupBox *GroupBox1;
	TLabel *Label4;
	TEdit *HistoryTimeBefore;
	TEdit *HistoryTimeAfter;
	TLabel *Label5;
	TColorDialog *ColorDialog1;
	TTabSheet *TabSheet3;
	TLabel *Label8;
	TEdit *OrderPeriod;
	TGroupBox *GroupBox2;
	TLabel *Label6;
	TLabel *Label7;
	THotKey *HotKeyReceiptAll;
	THotKey *HotKeyClearAll;
	TGroupBox *GroupBox3;
	TSpeedButton *bReadLayout;
	TSpeedButton *bSaveCurrentLayout;
	TComboBox *Layouts;
	TGroupBox *GroupBox4;
	TLabel *Label9;
	TEdit *snd_disparity;
	TLabel *Label10;
	TEdit *snd_back;
	TSpeedButton *bSelDisparitySnd;
	TSpeedButton *bSelBackSnd;
	TSpeedButton *SpeedButton1;
	TSpeedButton *SpeedButton2;
	TSpeedButton *SpeedButton3;
	TBitBtn *bApply;
	TLabel *Label12;
	TOpenDialog *SoundOpenDlg;
	TFontDialog *FontDialog1;
	TComboBox *Source;
	TTabSheet *TabSheet4;
	TGroupBox *GroupBox8;
	TLabel *Label14;
	TLabel *Label15;
	TLabel *Label16;
	TComboBox *MonitorSelect;
	TStaticText *ReadyFont;
	TEdit *Edit1;
	TUpDown *ReadySpacer;
	TGroupBox *GroupBox5;
	TButtonGroup *ReadyColorsEdit;
	TLabel *Label11;
	TEdit *StableReadyTimer;
	TLabel *Label13;
	void __fastcall bSelViewerClick(TObject *Sender);
	void __fastcall bSelDataFolderClick(TObject *Sender);
	void __fastcall bSelExporFolderClick(TObject *Sender);
	void __fastcall SourceClick(TObject *Sender);
	void __fastcall bOkClick(TObject *Sender);
	void __fastcall bCancelClick(TObject *Sender);
	void __fastcall LayoutsChange(TObject *Sender);
	void __fastcall bSelDisparitySndClick(TObject *Sender);
	void __fastcall bSelBackSndClick(TObject *Sender);
	void __fastcall SpeedButton1Click(TObject *Sender);
	void __fastcall SpeedButton2Click(TObject *Sender);
	void __fastcall SpeedButton3Click(TObject *Sender);
	void __fastcall bApplyClick(TObject *Sender);
	void __fastcall ReadyColorsEditDrawButton(TObject *Sender, int Index, TCanvas *Canvas,Types::TRect &Rect, TButtonDrawState State);
	void __fastcall ReadyColorsEditButtonClicked(TObject *Sender, int Index);
	void __fastcall ReadyFontDblClick(TObject *Sender);

private:	// User declarations
        READY_COLORS Fready_colors;
        int          Fcolors_changed;
        bool         Ffont_changed;
protected:
     requerst_kadr_list_proc Fon_request_kadr_list;
     virtual void __fastcall WndProc(Winapi::Messages::TMessage &Message);
             void __fastcall kadr_list_add(LPOTDM_PROTO_KADR pk,bool ordered);
     bool __fastcall select_sound(UnicodeString & result);

public:		// User declarations
	__fastcall TTlmViewerParams(TComponent* Owner);
        __property requerst_kadr_list_proc on_request_kadr_list = {read = Fon_request_kadr_list, write = Fon_request_kadr_list};
        __property                  int     colors_changed      = {read = Fcolors_changed};
        __property                  bool    font_changed        = {read = Ffont_changed};
   void __fastcall kadr_list_add(DWORD kadr_id,UnicodeString sname,UnicodeString name,bool ordered);
 READY_COLORS __fastcall get_ready_colors()       {Fcolors_changed = 0; return Fready_colors; }
         void __fastcall set_ready_colors(const READY_COLORS & ready_colors){ Fready_colors = ready_colors;Fcolors_changed = 0; }
         TColor __fastcall get_ready_color(int idx);
         void   __fastcall set_ready_color(int idx, TColor color);
         void   __fastcall set_modem_addr (modem_addr ma);
    modem_addr  __fastcall get_modem_addr ();
    void        __fastcall set_parameters (Tviewer_parameters & params);
    void        __fastcall get_parameters (Tviewer_parameters & params);
};
//---------------------------------------------------------------------------
extern PACKAGE TTlmViewerParams *TlmViewerParams;
//---------------------------------------------------------------------------
#endif
