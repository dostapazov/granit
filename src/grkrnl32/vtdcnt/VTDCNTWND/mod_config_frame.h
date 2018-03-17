//---------------------------------------------------------------------------

#ifndef mod_config_frameH
#define mod_config_frameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "gkmod_frame.h"
#include <ComCtrls.hpp>
#include "..\vtdcnt.h"
//---------------------------------------------------------------------------
class TVtdModConfig : public TGKModuleFrame
{
__published:	// IDE-managed Components
        TLabel *Label1;
        TEdit *Edit1;
        TUpDown *ModemNumber;
        TLabel *Label2;
        TEdit *PuName;
        TLabel *Label3;
        TEdit *ServerAddr;
        TLabel *Label4;
        TEdit *ServerPort;
        TLabel *Label5;
        TEdit *Edit2;
        TUpDown *PuNumber;
        void __fastcall Edit1Change(TObject *Sender);
        void __fastcall Edit2Change(TObject *Sender);
        void __fastcall PuNameChange(TObject *Sender);
        void __fastcall ServerAddrChange(TObject *Sender);
        void __fastcall ServerPortChange(TObject *Sender);
private:	// User declarations
TNotifyEvent       Fon_changed;
DWORD              Fch_mask;
void    __fastcall set_ch_mask(DWORD)  ;

void    __fastcall after_set_gkhandle();
bool    __fastcall get_config(LPVTDCNTMOD_CONFIG mc);

public:		// User declarations
VTDCNTMOD_CONFIG   mod_config;
        __fastcall      TVtdModConfig(TComponent* Owner);
void    __fastcall      setup(LPVTDCNTMOD_CONFIG mc = NULL);
bool  __fastcall apply_changes();
bool  __fastcall undo_changes();
__property DWORD        ch_mask    = {read = Fch_mask,write = set_ch_mask};
__property TNotifyEvent on_changed = {read = Fon_changed,write = Fon_changed};
};
//---------------------------------------------------------------------------
extern PACKAGE TVtdModConfig *VtdModConfig;
//---------------------------------------------------------------------------
#endif
