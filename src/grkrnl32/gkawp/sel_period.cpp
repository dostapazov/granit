//---------------------------------------------------------------------------

#pragma hdrstop
#include <vcl.h>

#include "sel_period.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TSelPeriod *SelPeriod;
//---------------------------------------------------------------------------
__fastcall TSelPeriod::TSelPeriod(TComponent* Owner)
	: TForm(Owner)
{
 EndDate->Date      = TDateTime::CurrentDate();
 EndTime->DateTime  = TDateTime::CurrentTime();
 BegTime->DateTime  = EndTime->DateTime;
 BegDate->Date      = EndDate->Date-1;

}
//---------------------------------------------------------------------------

LONGLONG  __fastcall TSelPeriod::get_begin_timestamp()
 {
  return get_timestamp(BegDate->Date,BegTime->DateTime);
 }

LONGLONG  __fastcall TSelPeriod::get_end_timestamp  ()
 {
  return get_timestamp(EndDate->Date,EndTime->DateTime);
 }

LONGLONG  __fastcall TSelPeriod::get_timestamp      (TDateTime  dt,TDateTime  tm)
 {
  SYSTEMTIME st;
  dt.DecodeDate(&st.wYear,&st.wMonth,&st.wDay);
  tm.DecodeTime(&st.wHour,&st.wMinute,&st.wSecond,&st.wMilliseconds);
  LONGLONG ret;
  SystemTimeToFileTime(&st,(LPFILETIME)&ret);
  return ret;
 }

 void      __fastcall TSelPeriod::set_end_timestamp  (LONGLONG val)
 {
  if(end_time != val)
    {
     SYSTEMTIME st;
     FileTimeToSystemTime((LPFILETIME)&val,&st);
     EndDate->Date = TDateTime(st.wYear,st.wMonth,st.wDay);
     EndTime->Time = TDateTime(st.wHour,st.wMinute,st.wSecond,st.wMilliseconds);
    }
 }

 void      __fastcall TSelPeriod::set_begin_timestamp  (LONGLONG val)
 {
  if(beg_time != val)
    {
     SYSTEMTIME st;
     FileTimeToSystemTime((LPFILETIME)&val,&st);
     BegDate->Date = TDateTime(st.wYear,st.wMonth,st.wDay);
     BegTime->Time = TDateTime(st.wHour,st.wMinute,st.wSecond,st.wMilliseconds);
    }
 }


 UnicodeString __fastcall TSelPeriod::get_period_text()
 {
   UnicodeString str;
   str.printf(L"ѕериод с %s %s  по  %s %s"
                ,BegDate->Date.DateString().c_str(),BegTime->DateTime.TimeString().c_str()
                ,EndDate->Date.DateString().c_str(),EndTime->DateTime.TimeString().c_str()
             );
   return str;
 }

void __fastcall TSelPeriod::FormResize(TObject *Sender)
{
    OpenedKadrs->Columns->Items[0]->Width = OpenedKadrs->Width - GetSystemMetrics(SM_CXVSCROLL);
}
//---------------------------------------------------------------------------


void __fastcall TSelPeriod::BitBtn3Click(TObject *Sender)
{
       Height += BitBtn3->Tag ? -222 : 222;
       BitBtn3->Caption = !BitBtn3->Tag ? L"—крыть" : L"≈щЄ";
       BitBtn3->Tag = !bool(BitBtn3->Tag);
}
//---------------------------------------------------------------------------

