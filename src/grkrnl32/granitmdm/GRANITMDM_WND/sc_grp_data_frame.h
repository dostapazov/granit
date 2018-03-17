//---------------------------------------------------------------------------

#ifndef sc_grp_data_frameH
#define sc_grp_data_frameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "sc_base_data_frame.h"
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TSCGroupDataFrame : public TSCBaseDataFrame
{
__published:	// IDE-managed Components
        TListView *ObjectList;
        TBitBtn *TuOnMore;
        TBitBtn *TuOffLess;
        TBitBtn *TrUndo;
        TGroupBox *CalcValueBox;
        TLabel *Label6;
        TLabel *Label7;
        TLabel *Label8;
        TLabel *Label9;
        TStaticText *Result;
        TStaticText *PDiagText;
        TEdit *ScaleBegin;
        TEdit *ScaleEnd;
        TEdit *Kvants;
        TLabel *Label10;
        TStaticText *GrpTimerText;
        TLabel *Label15;
        TStaticText *RespondTime;
        TLabel *Label16;
        TStaticText *Chdbc_counter;
        TLabel *Label17;
        TStaticText *Nchd_counter;
        TLabel *Label11;
        TStaticText *NoRespondCounter;
        TBitBtn *ResetTuError;
        void __fastcall KvantsChange(TObject *Sender);
        void __fastcall ObjectListSelectItem(TObject *Sender,
          TListItem *Item, bool Selected);
        void __fastcall ScaleBeginChange(TObject *Sender);
        void __fastcall TuOnMoreClick(TObject *Sender);
        void __fastcall TuOffLessClick(TObject *Sender);
        void __fastcall TrUndoClick(TObject *Sender);
        void __fastcall ObjectListChanging(TObject *Sender,
          TListItem *Item, TItemChange Change, bool &AllowChange);
        void __fastcall ResetTuErrorClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TSCGroupDataFrame(TComponent* Owner);
protected:

         void  __fastcall setup_data     (otd_proto & op);
         void  __fastcall update_dyn_data();
         void  __fastcall set_active     (DWORD _addr);
         void  __fastcall prepare_object_list(otd_proto & op);
         void  __fastcall setup_param( otd_data & od );
         void  __fastcall setup_pdiag( otd_data & od );
         void  __fastcall setup_pcm  ( otd_data & od );




};
//---------------------------------------------------------------------------
extern PACKAGE TSCGroupDataFrame *SCGroupDataFrame;
//---------------------------------------------------------------------------
#endif
