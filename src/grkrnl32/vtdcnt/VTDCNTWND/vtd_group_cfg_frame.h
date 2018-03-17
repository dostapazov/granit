//---------------------------------------------------------------------------

#ifndef vtd_group_cfg_frameH
#define vtd_group_cfg_frameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "gkmod_frame.h"
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include "..\vtdcnt.h"

//---------------------------------------------------------------------------
class TVtdGroupConfig : public TGKModuleFrame
{
__published:	// IDE-managed Components
        TLabel *Label1;
        TEdit *NumberEdit;
        TComboBox *ParamKind;
        TUpDown *GrpNumber;
        TPanel *VTDPanel;
        TLabel *Label4;
        TLabel *Label8;
        TEdit *Edit3;
        TUpDown *FirstParam;
        TEdit *Edit4;
        TUpDown *ParamCount;
        TLabel *Label3;
        TEdit *QueryPeriod;
        TLabel *Label11;
        TLabel *Label2;
        TLabel *GroupNumber;
        void __fastcall ParamKindChange(TObject *Sender);
        void __fastcall NumberEditChange(TObject *Sender);
        void __fastcall QueryPeriodChange(TObject *Sender);
        void __fastcall Edit3Change(TObject *Sender);
private:	// User declarations
VTDDEV_GROUP       grp_param;
DWORD              dev_number;
TNotifyEvent       Fon_changed;
DWORD              Fch_mask;

DWORD  __fastcall get_number();
void   __fastcall set_number(DWORD num);

void   __fastcall set_ch_mask(DWORD);
void   __fastcall setup_controls();

void   __fastcall get_param_from_controls(LPVTDDEV_GROUP gparam);
void   __fastcall after_set_gkhandle();
void   __fastcall update_grp_num();

public:		// User declarations
        __fastcall TVtdGroupConfig(TComponent* Owner);

__property DWORD        ch_mask    = {read = Fch_mask,write = set_ch_mask};
__property TNotifyEvent on_changed = {read = Fon_changed,write = Fon_changed};
__property DWORD        number     = {read = get_number,write = set_number};

bool   __fastcall apply_changes();
bool  __fastcall undo_changes();
AnsiString __fastcall get_text(DWORD number);
bool   __fastcall get_param(LPVTDDEV_GROUP gparam,DWORD num = -1);


};
//---------------------------------------------------------------------------
extern PACKAGE TVtdGroupConfig *VtdGroupConfig;
//---------------------------------------------------------------------------
#endif
