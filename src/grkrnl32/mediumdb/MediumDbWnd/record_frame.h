//---------------------------------------------------------------------------


#ifndef record_frameH
#define record_frameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <mediumdb.h>
#include <gkmodules.hpp>
#include <Vcl.ExtCtrls.hpp>

//---------------------------------------------------------------------------
class TRecordFrame : public TFrame
{
__published:	// IDE-managed Components
        TGroupBox *CommonRecordParam;
        TLabel *Label1;
        TEdit *OtdAddr;
        TLabel *Label2;
        TLabel *Label3;
        TLabel *Label4;
        TLabel *ID;
        TCheckBox *StateWorkOut;
	TCheckBox *OptAlarm;
        TLabel *ScaleLabel;
        TComboBox *ScaleId;
        TLabel *Label9;
        TEdit *Percent;
	TCheckBox *OptDanger;
        TCheckBox *StateHasTuTr;
        TCheckBox *StateDiscreteValue;
        TGroupBox *ScaleParam;
        TLabel *Label7;
        TLabel *Label8;
        TLabel *Label10;
	TEdit *ValueMin;
	TEdit *ValueMax;
        TEdit *Kvants;
        TGroupBox *RecordParam;
        TCheckBox *StateDiscreteStatic;
        TCheckBox *StateDiscreteHidden;
        TCheckBox *StateDiscreteFixed;
        TCheckBox *StateDiscreteNormal;
        TCheckBox *StateDiscreteTU;
        TCheckBox *StateDiscreteTUERR;
	TGroupBox *DangerBox;
	TEdit *MaxDanger;
	TEdit *MinDanger;
	TGroupBox *AlarmBox;
	TEdit *MaxAlarm;
	TEdit *MinAlarm;
	TCheckBox *optInvScale;
	TTimer *adjust_timer;
	TCheckBox *optSigned;
	TCheckBox *OptPseudo;
	TCheckBox *OptCaution;
	TComboBox *ShortName;
	TComboBox *FullName;
	TGroupBox *CombinedRecordParam;
	TPageControl *PageControl1;
	TTabSheet *TabSheet1;
	TTabSheet *FormulaSheet;
	TPanel *Panel1;
	TSpeedButton *CombObjAdd;
	TSpeedButton *CombObjRemove;
	TLabel *CombEntryName;
	TEdit *CombObjId;
	TCheckBox *OptCombineCalcByLast;
	TListView *CombineList;
	TMemo *Formula;
	TTabSheet *TabSheet2;
	TMemo *Memo1;
	TLabel *Label5;
	TLabel *Label6;
	TLabel *Label11;
	TLabel *Label12;
        void __fastcall CombineListSelectItem(TObject *Sender,
          TListItem *Item, bool Selected);
        void __fastcall CombineListDragOver(TObject *Sender,
          TObject *Source, int X, int Y, TDragState State, bool &Accept);
        void __fastcall CombineListDragDrop(TObject *Sender,
          TObject *Source, int X, int Y);
        void __fastcall OtdAddrChange(TObject *Sender);
        void __fastcall ShortNameChange(TObject *Sender);
        void __fastcall FullNameChange(TObject *Sender);
        void __fastcall FormulaChange(TObject *Sender);
        void __fastcall CombineListDeletion(TObject *Sender,
          TListItem *Item);
        void __fastcall CombineListInsert(TObject *Sender,
          TListItem *Item);
        void __fastcall CombObjRemoveClick(TObject *Sender);
        void __fastcall CombObjIdChange(TObject *Sender);
        void __fastcall CombObjAddClick(TObject *Sender);
        void __fastcall CombineListCompare(TObject *Sender,
          TListItem *Item1, TListItem *Item2, int Data, int &Compare);
        void __fastcall PercentChange(TObject *Sender);
        void __fastcall CombineListResize(TObject *Sender);
        void __fastcall StateWorkOutClick(TObject *Sender);
        void __fastcall ScaleIdChange(TObject *Sender);
        void __fastcall OptAlarmClick(TObject *Sender);
        void __fastcall ValueMinChange(TObject *Sender);
        void __fastcall ScaleIdDropDown(TObject *Sender);
        void __fastcall MaxDangerChange(TObject *Sender);
        void __fastcall MinDangerChange(TObject *Sender);
        void __fastcall StateDiscreteStaticClick(TObject *Sender);
	void __fastcall MinAlarmChange(TObject *Sender);
	void __fastcall MaxAlarmChange(TObject *Sender);
	void __fastcall optInvScaleClick(TObject *Sender);
	void __fastcall StateDiscreteValueClick(TObject *Sender);
	void __fastcall adjust_timerTimer(TObject *Sender);
	void __fastcall optSignedClick(TObject *Sender);
	void __fastcall OptCombineCalcByLastClick(TObject *Sender);
	void __fastcall OptPseudoClick(TObject *Sender);
	void __fastcall ShortNameCloseUp(TObject *Sender);
	void __fastcall ShortNameDropDown(TObject *Sender);
	void __fastcall ShortNameSelect(TObject *Sender);
	void __fastcall OptCautionClick(TObject *Sender);
private:	// User declarations
        DWORD         Fapply_mask;
        BOOL          setup_active;
        mdb_record    Frecord;
        mdb_record  __fastcall Getrecord();
        void        __fastcall Setrecord(mdb_record & value);
        TGKModuleInterface     mod_iface;
        TGKModuleInterface     scale;
        GKHANDLE    __fastcall get_gkhandle(){return mod_iface();}
        void        __fastcall set_gkhandle(GKHANDLE h);

        void        __fastcall setup_controls(DWORD mask);
        void        __fastcall setup_combine_param();
        void        __fastcall setup_combine_list();
        void        __fastcall combine_list_add(DWORD id);

        DWORD       __fastcall set_record_name(DWORD id,UnicodeString & str,bool short_name);
        void        __fastcall Setapply_mask(DWORD value);
        DWORD       __fastcall Getapply_mask();
        bool        __fastcall Getcreate_mode();
        sotd_param_addr
             __fastcall  make_record_addr();
       void  __fastcall  adjust_alarms_dangers();
       void  __fastcall  start_adjust_timer   ();
       float __fastcall  get_min_danger();
       float __fastcall  get_max_danger();
       float __fastcall  get_min_alarm ();
       float __fastcall  get_max_alarm ();
       float __fastcall  get_min_value ();
       float __fastcall  get_max_value ();

       void __fastcall  set_min_danger(float val);
       void __fastcall  set_max_danger(float val);
       void __fastcall  set_min_alarm (float val);
       void __fastcall  set_max_alarm (float val);
       void __fastcall  set_min_value (float val);
       void __fastcall  set_max_value (float val);


public:		// User declarations
        __fastcall TRecordFrame(TComponent* Owner);
        void       __fastcall update_record(DWORD rec_id,DWORD mask);
        DWORD __fastcall apply_changes(DWORD mask);
        DWORD __fastcall undo_changes (DWORD mask);

        __property mdb_record  record    = { read = Getrecord ,write = Setrecord};
        __property GKHANDLE   gk_handle = { read = get_gkhandle,write = set_gkhandle};
        __property DWORD apply_mask  = { read=Getapply_mask, write=Setapply_mask };
        __property bool create_mode  = { read=Getcreate_mode };
        WideString   __fastcall get_record_name(DWORD id,bool short_name);
        static void  __fastcall set_float(TEdit * edit,float val);
        static float __fastcall get_float(TEdit * edit);
               void  __fastcall get_mdb_strings(TStrings * strings);


};
//---------------------------------------------------------------------------
extern PACKAGE TRecordFrame *RecordFrame;
//---------------------------------------------------------------------------

#endif
