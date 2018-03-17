//---------------------------------------------------------------------------

#ifndef sc_data_frameH
#define sc_data_frameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "sc_base_frame.h"
#include <ComCtrls.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TSCDataFrame : public TSCBaseFrame
{
__published:	// IDE-managed Components
        TGroupBox *GroupBox1;
        TLabel *Label1;
        TListView *ListView1;
        TStaticText *Diag;
        TGroupBox *GroupBox2;
        TLabel *Label2;
        TLabel *Label3;
        TLabel *Label4;
        TLabel *Label5;
        TEdit *ScaleBegin;
        TEdit *ScaleEnd;
        TEdit *Kvants;
        TStaticText *Result;
        TLabel *Label6;
        TStaticText *TimeStamp;
        TBitBtn *TrUndo;
        TBitBtn *TuOnMore;
        TBitBtn *TuOffLess;
        TLabel *Label7;
        TStaticText *dyn_timer;
        TTimer *Timer1;
        void __fastcall KvantsChange(TObject *Sender);
        void __fastcall ListView1Change(TObject *Sender, TListItem *Item,
          TItemChange Change);
        void __fastcall TuOnMoreClick(TObject *Sender);
        void __fastcall TuOffLessClick(TObject *Sender);
        void __fastcall TrUndoClick(TObject *Sender);
        void __fastcall Timer1Timer(TObject *Sender);
private:	// User declarations
bool        __fastcall setup_data(SC_OTDDATA & scod,bool upd);
AnsiString  __fastcall get_diag_text(DWORD dw_diag);
void        __fastcall prepare_list_view(otd_proto & op);
void        __fastcall setup_param( otd_data & od );
void        __fastcall setup_pdiag( otd_data & od );
void        __fastcall setup_pcm  ( otd_data & od );

public:		// User declarations
        __fastcall TSCDataFrame(TComponent* Owner);
void  __fastcall set_active   (DWORD _addr);
void  __fastcall set_inactive();
void  __fastcall update_changes     (LPGRSC_ACR acr);
void  __fastcall update_data_changes(LPGRSC_ACR acr);
bool  __fastcall update_data  (WORD lo,WORD hi);



};
//---------------------------------------------------------------------------
extern PACKAGE TSCDataFrame *SCDataFrame;
//---------------------------------------------------------------------------
#endif
