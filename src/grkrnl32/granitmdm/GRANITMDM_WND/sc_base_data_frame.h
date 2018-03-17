//---------------------------------------------------------------------------

#ifndef sc_base_data_frameH
#define sc_base_data_frameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "sc_base_frame.h"
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TSCBaseDataFrame : public TSCBaseFrame
{
__published:	// IDE-managed Components
        TGroupBox *GroupBox1;
        TStaticText *FullDiagText;
        TLabel *Label1;
        TLabel *Label2;
        TLabel *Label3;
        TStaticText *PartDiagText;
        TStaticText *StateText;
        TLabel *Label4;
        TStaticText *TimeStampText;
        TLabel *Label5;
        TTimer *DynTimer;
        void __fastcall DynTimerTimer(TObject *Sender);
private:
        DWORD FDiagDword;
        __int64 FTimeStamp;
        void __fastcall SetDiagDword(DWORD value);
        void __fastcall SetTimeStamp(__int64 value);	// User declarations
public:		// User declarations
        __fastcall TSCBaseDataFrame(TComponent* Owner);
static AnsiString __fastcall get_diag_text(DWORD diag,bool prn_val = true);
static AnsiString __fastcall get_part_diag_text(DWORD diag,bool prn_val = true);
static AnsiString __fastcall get_state_text(DWORD diag,bool prn_val = true);
static AnsiString __fastcall get_pdiag_text(DWORD diag,bool prn_val = true);
void  __fastcall set_active     (DWORD _addr);
void  __fastcall set_inactive   (){DynTimer->Enabled = false;TSCBaseFrame::set_inactive();}
protected:
           void  __fastcall update_data_changes(LPGRSC_ACR acr);
virtual    void  __fastcall setup_data     (otd_proto & op);
virtual    void  __fastcall update_dyn_data(){};

__property DWORD DiagDword  = { read=FDiagDword, write=SetDiagDword };
__property __int64 TimeStamp  = { read=FTimeStamp, write=SetTimeStamp };
};
//---------------------------------------------------------------------------
extern PACKAGE TSCBaseDataFrame *SCBaseDataFrame;
//---------------------------------------------------------------------------
#endif
