//---------------------------------------------------------------------------

#ifndef vtd_line_cfg_frameH
#define vtd_line_cfg_frameH
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
class TVtdLineConfig : public TGKModuleFrame
{
__published:	// IDE-managed Components
        TLabel *Label1;
        TEdit *Edit1;
        TLabel *Label2;
        TEdit *DevName;
        TLabel *Label15;
        TComboBox *DevType;
        TUpDown *DeviceNumber;
        void __fastcall Edit1Change(TObject *Sender);
        void __fastcall DevTypeChange(TObject *Sender);
        void __fastcall DevNameChange(TObject *Sender);
private:	// User declarations

VTDDEV_PARAM       device_param;
TNotifyEvent       Fon_changed;
DWORD              Fch_mask;
void   __fastcall set_ch_mask(DWORD);
void   __fastcall after_set_gkhandle();
void   __fastcall set_device_number(DWORD num);
DWORD  __fastcall get_device_number();
void   __fastcall get_device_param(LPVTDDEV_PARAM device_param);
void   __fastcall setup_controls(LPVTDDEV_PARAM device_param);

public:		// User declarations
        __fastcall TVtdLineConfig(TComponent* Owner);
bool  __fastcall apply_changes();
bool  __fastcall undo_changes();
__property DWORD        ch_mask    = {read = Fch_mask,write = set_ch_mask};
__property TNotifyEvent on_changed = {read = Fon_changed,write = Fon_changed};
__property DWORD        number     = {read = get_device_number,write = set_device_number};

};
//---------------------------------------------------------------------------
extern PACKAGE TVtdLineConfig *VtdLineConfig;
//---------------------------------------------------------------------------
#endif
