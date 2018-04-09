//---------------------------------------------------------------------------

#pragma warn -8058
#include <vcl.h>
#pragma warn .8058
#pragma hdrstop
#include "sc_base_data_frame.h"
#include "grscfrm_rh.h"
#include "gkmodule_form.h"


//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "sc_base_frame"
#pragma resource "*.dfm"

TSCBaseDataFrame *SCBaseDataFrame;

//---------------------------------------------------------------------------
__fastcall TSCBaseDataFrame::TSCBaseDataFrame(TComponent* Owner)
        : TSCBaseFrame(Owner)
{
  FDiagDword = 0;
}
//---------------------------------------------------------------------------

AnsiString __fastcall TSCBaseDataFrame::get_diag_text(DWORD diag,bool prn_val)
{
  AnsiString s;
  DWORD mask = OTD_DIAG_NODATA;
  if(prn_val)
   s.printf("%04X ",(DWORD)HIWORD(diag));
  for(int i = 0;i<16;i++)
  {
   if(diag & mask)
   {
    UnicodeString rcs = load_rcstring(ModuleInstance,STR_DIAG_NODATA+i);

    s+= rcs;
    break;
   }
   mask>>=1;
  }
  return s;
}

AnsiString __fastcall TSCBaseDataFrame::get_part_diag_text(DWORD value,bool prn_val)
{
 AnsiString s;
 if(prn_val)
  s.printf("%04X ",(value&(OTD_DIAG_PART_MASK|OTD_DIAG_STATE_MASK)));
 DWORD  mask = OTD_PART_DIAG_PARAM ;
  for(int i = 0;i<8;i++)
  {
   if(value & mask)
     {
      s+= load_rcstring(ModuleInstance,STR_PART_DIAG_PARAM+i);
      break;
     }
    mask>>=1;
  }

 return s;
}

AnsiString __fastcall TSCBaseDataFrame::get_state_text(DWORD value,bool prn_val)
{
 AnsiString s;
 if(prn_val)
 s.printf("%04X ",(value&OTD_STATE_MASK));
 DWORD  mask = OTD_STATE_RESERV1;
  for(int i = 0;i<8;i++)
  {
   if(value & mask)
     {
      s+=load_rcstring(ModuleInstance,STR_STATE_CALL_SENDED+i);
      break;
     }
    mask>>=1;
  }
 return s;
}

AnsiString __fastcall TSCBaseDataFrame::get_pdiag_text(DWORD value,bool prn_val)
{
 AnsiString s;
 if(prn_val)
  s.printf("%02X ",value);
 DWORD  mask = OTD_PDIAG_NODATA;
  for(int i = 0;i<8;i++)
  {
   if(value & mask)
     {
      s+=load_rcstring(ModuleInstance,STR_PDIAG_NODATA+i);
     }
    mask>>=1;
  }
 return s;
}


void __fastcall TSCBaseDataFrame::SetDiagDword(DWORD value)
{
        //TODO: Add your source code here
        FDiagDword = value;
        char text[MAX_PATH];
        int     len;
        DWORD   mask;
        TColor  text_color ;

        FullDiagText->Caption = get_diag_text(value);
        FullDiagText->Font->Color = (value&OTD_DIAG_MASK) ?  clRed:clDefault;
        PartDiagText->Caption = get_part_diag_text(value);
        PartDiagText->Font->Color = (value&OTD_DIAG_PART_MASK) ? (TColor)0x003797AF:clDefault;
        StateText->Caption = get_state_text(value);
}

void __fastcall TSCBaseDataFrame::SetTimeStamp(__int64 value)
{
        //TODO: Add your source code here
    FTimeStamp  = value;
    TCHAR date_time[MAX_PATH];
    date_time[0] = 0;
    if(value)
    {
      SYSTEMTIME st;
      FileTimeToSystemTime((LPFILETIME)&value,&st);
      int dtl = GetDateFormat(LOCALE_USER_DEFAULT,DATE_SHORTDATE,&st,NULL,date_time,sizeof(date_time)/sizeof(TCHAR));
      dtl--;
      *(date_time+dtl) = ' ';
      dtl++;
      dtl+=GetTimeFormat(LOCALE_USER_DEFAULT,0,&st,_T("HH':'mm':'ss"),date_time+dtl,sizeof(date_time)/sizeof(TCHAR)-dtl);
      dtl--;
      wsprintf(date_time+dtl,_T(",%03d"),(DWORD)st.wMilliseconds);

    }
    TimeStampText->Caption = date_time;
}



void  __fastcall TSCBaseDataFrame::set_active     (DWORD _addr)
{
  TSCBaseFrame::set_active(_addr);
  update_dyn_data();
  DynTimer->Enabled = true;
}


void  __fastcall TSCBaseDataFrame::update_data_changes(LPGRSC_ACR acr)
{
 BYTE buffer[4096];
 LPSC_OTDDATA  scod = (LPSC_OTDDATA)buffer;
 scod->dw_size = sizeof(*scod);
 scod->parts   = acr->parts;
 scod->lo_num  = KeRTL::MIN(acr->loN,acr->hiN);
 scod->hi_num  = KeRTL::MAX(acr->loN,acr->hiN);
 scod->data_size  = sizeof(buffer)-sizeof(*scod)+sizeof(BYTE);
 scod->data_len   = 0;
 if(mod_iface.call(SCCM_GET_OTDDATA,addr.addr,(LPARAM)scod)==GKH_RET_SUCCESS)
 {
  otd_proto  op;
  DWORD real_sz;
  op.dw_size = sizeof(op);
  if(otd_proto_parser(scod->data,scod->data_len,&op,&real_sz))
     setup_data(op);
 }
}

void  __fastcall TSCBaseDataFrame::setup_data     (otd_proto & op)
{
  if(op.diag)
     this->DiagDword = *op.diag;
  if(op.time_stamp)
     this->TimeStamp = *((__int64*)op.time_stamp);
}


void __fastcall TSCBaseDataFrame::DynTimerTimer(TObject *Sender)
{
 update_dyn_data();
}
//---------------------------------------------------------------------------

