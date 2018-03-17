//---------------------------------------------------------------------------

#ifndef sc_cp_data_frameH
#define sc_cp_data_frameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "sc_base_data_frame.h"
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TSCCpDataFrame : public TSCBaseDataFrame
{
__published:	// IDE-managed Components
        TGroupBox *GroupBox2;
        TListView *ListView1;
        TListView *TuList;
        TStaticText *GrpPDDiag;
        TLabel *Label8;
        TStaticText *TxByteTime;
        TLabel *Label9;
        TStaticText *TxDeliverTime;
        TLabel *Label10;
        TStaticText *RxTimer;
        TStaticText *StaticText1;
        TStaticText *StaticText2;
        void __fastcall ListView1SelectItem(TObject *Sender,
          TListItem *Item, bool Selected);
private:	// User declarations
public:		// User declarations
        __fastcall TSCCpDataFrame(TComponent* Owner);
  void  __fastcall set_active         (DWORD _addr);
  void  __fastcall setup_data         (otd_proto & op);
  void  __fastcall update_data_changes(LPGRSC_ACR acr);
  bool  __fastcall add_tu_entry   (DWORD idx);
  void  __fastcall remove_tu_entry(DWORD idx);
  void  __fastcall change_tu_entry(DWORD idx);
  void  __fastcall update_dyn_data();

};
//---------------------------------------------------------------------------
extern PACKAGE TSCCpDataFrame *SCCpDataFrame;
//---------------------------------------------------------------------------
#endif
