//---------------------------------------------------------------------------

#ifndef cp_frameH
#define cp_frameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "sc_base_frame.h"
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TCPFrame : public TSCBaseFrame
{
__published:	// IDE-managed Components
        TGroupBox *GroupBox1;
        TLabel *Label1;
        TEdit *LuNum;
        TLabel *Label2;
        TEdit *CpName;
        TStaticText *CpNum;
        TLabel *Label5;
        TCheckBox *CpTestFlag;
        TLabel *Label3;
        TCheckBox *CpLogErrors;
        TCheckBox *CpNotReceipt;
        TLabel *Label4;
        TEdit *swt;
        TLabel *Label6;
        TCheckBox *NumberedTS;
        void __fastcall LuNumChange(TObject *Sender);
        void __fastcall CpNameChange(TObject *Sender);
        void __fastcall CpTestFlagClick(TObject *Sender);
        void __fastcall swtChange(TObject *Sender);
private:	// User declarations
GR_CP_PROPS  cpr;
void   __fastcall  setup_controls(GR_CP_PROPS & cpr);
void   __fastcall  format_props  (GR_CP_PROPS & cpr);

public:		// User declarations
        __fastcall TCPFrame(TComponent* Owner);
virtual  bool  __fastcall apply_changes() ;
virtual  bool  __fastcall undo_changes () ;
virtual  void  __fastcall set_active   (DWORD );
virtual  void  __fastcall set_inactive();
AnsiString     __fastcall get_text();
void           __fastcall update_changes(LPGRSC_ACR acr);

};
//---------------------------------------------------------------------------
extern PACKAGE TCPFrame *CPFrame;
//---------------------------------------------------------------------------
#endif
