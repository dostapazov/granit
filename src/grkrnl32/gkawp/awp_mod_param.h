//---------------------------------------------------------------------------

#ifndef awp_mod_paramH
#define awp_mod_paramH
//---------------------------------------------------------------------------
#include "gkawpmodule.hpp"

#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.ComCtrls.hpp>

//---------------------------------------------------------------------------

#include <otdm_proto.h>
#include <otd_proto_medium.h>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.CategoryButtons.hpp>
#include <Vcl.Grids.hpp>
#include <Vcl.ValEdit.hpp>
#include <Vcl.ButtonGroup.hpp>
#include "gkawpmodule.hpp"


typedef void __fastcall (__closure * Trequerst_kadr_list_proc)(modem_addr );
typedef void __fastcall (__closure * Tenum_layouts_proc      )(TStrings * strs  );
typedef void __fastcall (__closure * Tlayout_proc            )(UnicodeString lname);

using namespace System;

class TAwpModuleParams : public TForm
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
	TLabel *Label2;
	TEdit *ExportFolder;
	TSpeedButton *bSelExporFolder;
	TLabel *Label3;
	TEdit *ExportViewer;
	TSpeedButton *bSelViewer;
	TOpenDialog *SelViewerDlg;
	TGroupBox *GroupBox1;
	TLabel *Label4;
	TEdit *HistoryTimeBefore;
	TEdit *HistoryTimeAfter;
	TLabel *Label5;
	TColorDialog *ColorDialog1;
	TLabel *Label8;
	TEdit *OrderPeriod;
	TGroupBox *GroupBox2;
	TLabel *Label6;
	TLabel *Label7;
	THotKey *HotKeyReceiptAll;
	THotKey *HotKeyClearAll;
	TSpeedButton *bSelAll;
	TSpeedButton *bUnselAll;
	TSpeedButton *bSelInvert;
	TBitBtn *bApply;
	TLabel *Label12;
	TOpenDialog *SoundOpenDlg;
	TFontDialog *ReadyFont;
	TComboBox *Source;
	TTabSheet *ReadySheet;
	TGroupBox *GroupBox8;
	TLabel *Label14;
	TLabel *Label15;
	TComboBox *MonitorSelect;
	TEdit *Edit1;
	TUpDown *ReadySpacer;
	TGroupBox *GroupBox5;
	TButtonGroup *ReadyColorsEdit;
	TSpeedButton *bRefreshKadr;
	TCheckBox *ready_eq_size;
	TLabel *Label11;
	TEdit *StableReadyTimer;
	TLabel *Label13;
	TTimer *Timer1;
	TPaintBox *ReadyTest;
	TSpeedButton *SpeedButton3;
	TSpeedButton *SpeedButton4;
	TFontDialog *StateFont;
	TLabel *Label16;
	TEdit *FlashPeriod;
	TLabel *Label19;
	TLabel *Label20;
	TCheckBox *ready_show_rc_error;
	TGroupBox *GroupBox4;
	TLabel *Label9;
	TLabel *Label10;
	TSpeedButton *bSelDisparitySnd;
	TSpeedButton *bSelBackSnd;
	TLabel *Label17;
	TSpeedButton *bSelAlarmSnd;
	TLabel *Label18;
	TSpeedButton *bSelDangerSnd;
	TEdit *snd_disparity;
	TEdit *snd_back;
	TEdit *snd_alarm;
	TEdit *snd_danger;
	TGroupBox *GroupBox3;
	TSpeedButton *bReadLayout;
	TSpeedButton *bSaveCurrentLayout;
	TComboBox *Layouts;
	void __fastcall bSelViewerClick(TObject *Sender);
	void __fastcall bSelDataFolderClick(TObject *Sender);
	void __fastcall bSelExporFolderClick(TObject *Sender);
	void __fastcall SourceClick(TObject *Sender);
	void __fastcall bOkClick(TObject *Sender);
	void __fastcall bCancelClick(TObject *Sender);
	void __fastcall LayoutsChange(TObject *Sender);
	void __fastcall bSelDisparitySndClick(TObject *Sender);
	void __fastcall bSelAllClick(TObject *Sender);
	void __fastcall bUnselAllClick(TObject *Sender);
	void __fastcall bSelInvertClick(TObject *Sender);
	void __fastcall bApplyClick(TObject *Sender);
	void __fastcall ReadyColorsEditDrawButton(TObject *Sender, int Index, TCanvas *Canvas,Types::TRect &Rect, TButtonDrawState State);
	void __fastcall ReadyColorsEditButtonClicked(TObject *Sender, int Index);
	void __fastcall bRefreshKadrClick(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall ReadyTestPaint(TObject *Sender);
	void __fastcall SpeedButton3Click(TObject *Sender);
	void __fastcall SpeedButton4Click(TObject *Sender);
	void __fastcall KadrListDeletion(TObject *Sender, TListItem *Item);
	void __fastcall KadrListChange(TObject *Sender, TListItem *Item, TItemChange Change);
	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall bSaveCurrentLayoutClick(TObject *Sender);
	void __fastcall bReadLayoutClick(TObject *Sender);
	void __fastcall LayoutsDropDown(TObject *Sender);
	void __fastcall ReadyFontApply(TObject *Sender, HWND Wnd);


private:	// User declarations
        READY_COLORS        Fready_colors;
        int                 Fcolors_changed;
        bool                Ffont_changed;
        bool                Forders_changed;
        TGkAwpModule      * module;
        single_kadr         sk;
protected:
     Trequerst_kadr_list_proc Fon_request_kadr_list;
     Tenum_layouts_proc       Fenum_layouts;
     Tlayout_proc             Fopen_layout;
     Tlayout_proc             Fsave_layout;
     virtual void __fastcall  WndProc(Winapi::Messages::TMessage &Message);
             void __fastcall  handle_enum_kadrs(LPMODEM_ADDR ma, LPOTDM_PROTO_STATE ps);
             bool __fastcall  select_sound(UnicodeString & result);
   single_kadr *  __fastcall  get_single_kadr(DWORD kadr_id){return &sk;}
   UnicodeString  __fastcall  get_ready_text(DWORD /*str_id*/);

	__fastcall TAwpModuleParams(TComponent* Owner);

public:		// User declarations
virtual __fastcall TAwpModuleParams(TComponent* Owner,TGkAwpModule      * _mod);
        __property Trequerst_kadr_list_proc on_request_kadr_list = {read = Fon_request_kadr_list, write = Fon_request_kadr_list};
        __property                  int     colors_changed       = {read = Fcolors_changed};
        __property                  bool    font_changed         = {read = Ffont_changed  };
        __property                  bool    orders_changed       = {read = Forders_changed};
        __property Tenum_layouts_proc       enum_layouts         = {read = Fenum_layouts  ,write = Fenum_layouts };
        __property Tlayout_proc             open_layout          = {read = Fopen_layout   ,write = Fopen_layout  };
        __property Tlayout_proc             save_layout          = {read = Fsave_layout   ,write = Fsave_layout  };


         void   __fastcall kadr_list_add   (LPMODEM_ADDR ma,LPOTDM_PROTO_KADR pkadr,DWORD data_size);
 READY_COLORS   __fastcall get_ready_colors()       {Fcolors_changed = 0; return Fready_colors; }
         void   __fastcall set_ready_colors(const READY_COLORS & ready_colors){ Fready_colors = ready_colors;Fcolors_changed = 0; }
         TColor __fastcall get_ready_color (int idx);
         void   __fastcall set_ready_color (int idx, TColor color);
         void   __fastcall set_modem_addr  (modem_addr ma);
    modem_addr  __fastcall get_modem_addr  ();
    void        __fastcall set_parameters  (TAWParameters & params);
    void        __fastcall get_parameters  (TAWParameters & params);
LPOTDM_PROTO_KADR __fastcall get_kadr      (int idx,bool &ordered);
};
//---------------------------------------------------------------------------
extern PACKAGE TAwpModuleParams *AwpModuleParams;
//---------------------------------------------------------------------------
#endif
