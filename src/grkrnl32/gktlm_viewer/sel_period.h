//---------------------------------------------------------------------------

#ifndef sel_periodH
#define sel_periodH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TSelPeriod : public TForm
{
__published:	// IDE-managed Components
	TGroupBox *GroupBox1;
	TDateTimePicker *BegTime;
	TGroupBox *Окончание;
	TDateTimePicker *EndTime;
	TBitBtn *BitBtn1;
	TBitBtn *BitBtn2;
	TDateTimePicker *BegDate;
	TDateTimePicker *EndDate;
	TRadioGroup *RadioGroup1;
	TListView *OpenedKadrs;
	TBitBtn *BitBtn3;
	void __fastcall FormResize(TObject *Sender);
	void __fastcall BitBtn3Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
	   __fastcall TSelPeriod(TComponent* Owner);
 LONGLONG  __fastcall get_begin_timestamp();
 void      __fastcall set_begin_timestamp(LONGLONG val);
 LONGLONG  __fastcall get_end_timestamp  ();
 void      __fastcall set_end_timestamp  (LONGLONG val);
 static
 LONGLONG  __fastcall get_timestamp      (TDateTime  dt,TDateTime  tm);
           __property LONGLONG beg_time = {read = get_begin_timestamp, write = set_begin_timestamp};
           __property LONGLONG end_time = {read = get_end_timestamp  , write = set_end_timestamp  };

 UnicodeString __fastcall get_period_text();

};
//---------------------------------------------------------------------------
extern PACKAGE TSelPeriod *SelPeriod;
//---------------------------------------------------------------------------
#endif
