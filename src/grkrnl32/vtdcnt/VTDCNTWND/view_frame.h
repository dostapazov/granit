//---------------------------------------------------------------------------

#ifndef view_frameH
#define view_frameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "gkmod_frame.h"
#include "..\vtdcnt.h"
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>

//---------------------------------------------------------------------------
class TViewFrame : public TGKModuleFrame
{
__published:	// IDE-managed Components
        TPanel *ViewDevPanel;
        TPanel *ViewGrpPanel;
        TLabel *Label1;
        TLabel *PlantDevNumber;
        TLabel *Label3;
        TLabel *DevDtTm;
        TLabel *Label4;
        TLabel *LastRepDt;
        TLabel *Label5;
        TLabel *PrevRepDt;
        TGroupBox *GroupBox1;
        TListView *DevListView;
        TListView *ViewGrpList;
        TLabel *Label2;
private:	// User declarations
protected:
DWORD Fnumber;
void   __fastcall setup_device(DWORD dn);
void   __fastcall setup_group (DWORD dn,DWORD gn,bool update);
void   __fastcall     setup_curr_user_param(sotd_proto  & sop);
void   __fastcall     setup_curr_param(sotd_proto& sop);
void   __fastcall     setup_pipe_param(sotd_proto& sop);

public:		// User declarations
        __fastcall TViewFrame(TComponent* Owner);
void   __fastcall after_set_gkhandle(){};
void   __fastcall set_number(DWORD);

__property DWORD number = {read = Fnumber,write = set_number};


};
//---------------------------------------------------------------------------
extern PACKAGE TViewFrame *ViewFrame;
//---------------------------------------------------------------------------
#endif
