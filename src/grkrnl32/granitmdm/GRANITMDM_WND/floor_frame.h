//---------------------------------------------------------------------------

#ifndef floor_frameH
#define floor_frameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "sc_base_frame.h"
//---------------------------------------------------------------------------
class TFloorFrame : public TSCBaseFrame
{
__published:	// IDE-managed Components
        TGroupBox *GroupBox1;
        TLabel *Label1;
        TEdit *FloorNum;
        TLabel *Label2;
        TStaticText *CpCount;
        void __fastcall FloorNumChange(TObject *Sender);
        void __fastcall flStaticClick(TObject *Sender);
private:	// User declarations
GR_FLOOR_PROPS   props;
void  __fastcall set_active    (DWORD  addr);
void  __fastcall setup_controls(GR_FLOOR_PROPS & fpr);
bool  __fastcall apply_changes(); 
bool  __fastcall undo_changes ();
public:		// User declarations
        __fastcall TFloorFrame(TComponent* Owner);
AnsiString __fastcall get_text();
void    __fastcall update_changes(LPGRSC_ACR acr);
};
//---------------------------------------------------------------------------
extern PACKAGE TFloorFrame *FloorFrame;
//---------------------------------------------------------------------------
#endif
