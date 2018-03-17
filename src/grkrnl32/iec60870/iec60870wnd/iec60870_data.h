//---------------------------------------------------------------------------

#ifndef iec60870_dataH
#define iec60870_dataH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "gkmod_frame.h"
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Buttons.hpp>
#include <iec60870_modem.h>

//---------------------------------------------------------------------------
class TIEC60870DataFrame : public TGKModuleFrame
{
__published:	// IDE-managed Components
	TRadioGroup *data_types;
	TPanel *group_panel;
	TSpeedButton *bAssignGroup;
	TEdit *otd_group;
	TStaticText *StaticText1;
	TStaticText *obj_count;
	TPanel *rc_param_panel;
	TSpeedButton *bAssignTuTr;
	TEdit *rc_object;
	TStaticText *StaticText2;
	TStaticText *StaticText3;
	TComboBox *cbRcCommand;
	TLabel *Label1;
	TSpeedButton *bRemoveGroup;
	TLabel *Label2;
	TEdit *otd_number;
	TGroupBox *GroupBox1;
	TRadioButton *rb_imp_short;
	TRadioButton *rb_imp_long;
	TRadioButton *rb_imp_persist;
	TRadioButton *rb_imp_user;
	TEdit *rc_imp_param;
	TEdit *rc_timeout;
	TStaticText *StaticText4;
	TTabControl *TabControl1;
	TListView *ASDU_Records;
	void __fastcall data_typesClick(TObject *Sender);
	void __fastcall ASDU_RecordsSelectItem(TObject *Sender, TListItem *Item, bool Selected);
	void __fastcall otd_groupChange(TObject *Sender);
	void __fastcall bAssignGroupClick(TObject *Sender);
	void __fastcall bRemoveGroupClick(TObject *Sender);
	void __fastcall bAssignTuTrClick(TObject *Sender);
	void __fastcall rb_imp_shortClick(TObject *Sender);
	void __fastcall rc_imp_paramChange(TObject *Sender);




private:	// User declarations
		 int Funassigned_count;
		 int Fassigned_count;
         iec60870_command_quality Fcurrent_cqu   ;
void     __fastcall  current_object_change (TObject * value);
DWORD    __fastcall  get_line_number();
void     __fastcall  set_unassigned(int v);
void     __fastcall  set_assigned  (int v);
void     __fastcall  calc_unassigned_count();
void     __fastcall  update_item(TListItem *item, const iec60870_record & rec );
void     __fastcall  setup_records(int otd_fa,int items_count);
void     __fastcall  setup_rc_params  (const iec60870_record & rec);
void     __fastcall  assign_otd(DWORD otd_fa,DWORD otd_group,DWORD otd_number);
bool     __fastcall  assign_rc_command(DWORD rec_number,DWORD otd_fa,DWORD rc_command);
TListItem * __fastcall create_record_item();
void     __fastcall set_current_cqu (iec60870_command_quality cqu);

public:		// User declarations
	   __fastcall TIEC60870DataFrame(TComponent* Owner);
void       __fastcall  setup_controls  ();
           __property DWORD line_number = {read = get_line_number};
           __property int unassigned_count = {read = Funassigned_count,write = set_unassigned };
           __property int   assigned_count = {read = Fassigned_count  ,write = set_assigned   };
           __property iec60870_command_quality current_cqu = {read = Fcurrent_cqu, write = set_current_cqu};
  void    __fastcall  notify_record(DWORD line_num ,LPBYTE data,DWORD data_size );
  static  int __fastcall command2index(int command);
  static  int __fastcall index2command(int idx    );


};
//---------------------------------------------------------------------------
extern PACKAGE TIEC60870DataFrame *IEC60870DataFrame;
//---------------------------------------------------------------------------

#define REC_OTD_ADDR_SUBITEM      0
#define REC_VALUE_SUBITEM         1
#define REC_FLOAT_SUBITEM         2
#define REC_IV_SUBITEM            3
#define REC_BL_SUBITEM            4
#define REC_SB_SUBITEM            5
#define REC_NP_SUBITEM            6
#define REC_OV_SUBITEM            7
#define REC_TIME_SUBITEM          8
#define REC_OTD_RC_STATE_SUBITEM  9
#define REC_ASDU_TYPE_SUBITEM     10
#define REC_MAX_SUBITEMS          11

#endif
