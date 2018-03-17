//---------------------------------------------------------------------------

#ifndef cpc152_dev_frameH
#define cpc152_dev_frameH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "gkmod_frame.h"
#include "cpc152dev.h"
#include <Vcl.Buttons.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <vector>
#include <algorithm>
//---------------------------------------------------------------------------
class Tcpc152dev_frame : public TGKModuleFrame
{
__published:	// IDE-managed Components
	TLabel *Label1;
	TEdit *ip_addr;
	TLabel *Label2;
	TEdit *cp_number;
	TLabel *Label3;
	TEdit *cp_name;
	TGroupBox *GroupBox1;
	TLabel *Label4;
	TEdit *ip_port;
	TLabel *Label5;
	TEdit *Freq;
	TLabel *Label6;
	TEdit *SampleFreq;
	TLabel *Label7;
	TEdit *SamplesCount;
	TLabel *Label8;
	TEdit *FrameSize;
	TUpDown *UpDown1;
	TLabel *Label12;
	TEdit *FrqGate;
	TLabel *Label13;
	TEdit *AmplPercent;
	TRadioGroup *CalcVariant;
	TGroupBox *GroupBox3;
	TLabel *ms;
	TEdit *OscLenght;
	TLabel *Label14;
	TButton *CreateOscilogramm;
	TCheckBox *CorrEnable;
	TGroupBox *CorrectionBox;
	TLabel *Label15;
	TEdit *CorrMaxKvants;
	TLabel *Label16;
	TEdit *CorrMustKvants;
	TLabel *Label17;
	TEdit *CorrZero;
	TPageControl *page_control;
	TTabSheet *dft_enabled_page;
	TTabSheet *TabSheet2;
	TGroupBox *GroupBox2;
	TLabel *Label9;
	TLabel *Label10;
	TLabel *Label11;
	TSpeedButton *bAdd;
	TSpeedButton *bRemove;
	TEdit *sDev;
	TEdit *sGrp;
	TEdit *sNum;
	TRadioGroup *sState;
	TListView *AlarmList;
	TComboBox *dev_num;
	TLabel *Label18;
	TLabel *Label19;
	TComboBox *grp_num;
	TListView *lv_dft_enabled;
	TSpeedButton *SpeedButton1;
	TSpeedButton *SpeedButton2;
	void __fastcall ip_addrChange(TObject *Sender);
	void __fastcall UpDown1ChangingEx(TObject *Sender, bool &AllowChange, int NewValue,
          TUpDownDirection Direction);
	void __fastcall AlarmListData(TObject *Sender, TListItem *Item);
	void __fastcall AlarmListChange(TObject *Sender, TListItem *Item, TItemChange Change);
	void __fastcall sDevChange(TObject *Sender);
	void __fastcall bAddClick(TObject *Sender);
	void __fastcall bRemoveClick(TObject *Sender);
	void __fastcall CreateOscilogrammClick(TObject *Sender);
	void __fastcall CalcVariantClick(TObject *Sender);
	void __fastcall CorrEnableClick(TObject *Sender);
	void __fastcall page_controlChange(TObject *Sender);
	void __fastcall lv_dft_enabledData(TObject *Sender, TListItem *Item);
	void __fastcall grp_numChange(TObject *Sender);
	void __fastcall lv_dft_enabledDblClick(TObject *Sender);
	void __fastcall lv_dft_enabledKeyPress(TObject *Sender, System::WideChar &Key);
	void __fastcall SpeedButton1Click(TObject *Sender);


private:	// User declarations
protected:

		 CPC152_LINE_CONFIG line_config;
                 CPC152_DFT_ENABLED dft_enabled;
		 typedef std::vector<CPC152_ALARM_START> alarm_start_t;
		 alarm_start_t  alarm_starts;
 		 alarm_start_t  alarm_starts_deleted;



virtual  bool     __fastcall  apply_changes  ();
virtual  bool     __fastcall  undo_changes   ();
         void     __fastcall  setup_controls (LPCPC152_LINE_CONFIG lc);
         void     __fastcall  get_conf_values(LPCPC152_LINE_CONFIG lc);
virtual  bool     __fastcall  delete_current ();
virtual  void     __fastcall  current_object_changed(TObject * ,bool &enable_set);
virtual  void     __fastcall  current_object_change (TObject * value);
         void     __fastcall  read_alarm_starts();
         bool     __fastcall  is_dft_enabled (int dev,int group,int param);
         void     __fastcall  set_dft_enabled(int dev,int group,int param,bool enable);


public:		// User declarations
                  __fastcall Tcpc152dev_frame(TComponent* Owner);
            int   __fastcall get_cp_number();
            void  __fastcall on_alarm_start_notify( LPCPC152_ALARM_START ast, bool add);

static   bool     __fastcall read_line_config(TGKModuleInterface * mod_iface,LPCPC152_LINE_CONFIG lc,LPCPC152_DFT_ENABLED dft_enabled = NULL);
static UnicodeString __fastcall get_line_text   (LPCPC152_LINE_CONFIG lc);
};
//---------------------------------------------------------------------------
extern PACKAGE Tcpc152dev_frame *cpc152dev_frame;
//---------------------------------------------------------------------------
#endif
