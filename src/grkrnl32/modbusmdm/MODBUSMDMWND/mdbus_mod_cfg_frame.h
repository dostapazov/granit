//---------------------------------------------------------------------------

#ifndef mdbus_mod_cfg_frameH
#define mdbus_mod_cfg_frameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "gkmod_frame.h"
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include "..\modbusmod.h"
//---------------------------------------------------------------------------
class TModbusModConfig : public TGKModuleFrame
{
__published:	// IDE-managed Components
        TLabel *Label1;
        TLabel *Label2;
        TLabel *Label5;
        TEdit *Edit1;
        TUpDown *ModemNumber;
        TEdit *PuName;
        TEdit *Edit2;
        TUpDown *PuNumber;
        void __fastcall Edit1Change(TObject *Sender);
        void __fastcall Edit2Change(TObject *Sender);
        void __fastcall PuNameChange(TObject *Sender);
private  :	// User declarations
protected:
        DWORD      Fch_mask;
        int        in_setup;
        MODBUS_MOD_CONFIG   mod_config;
        void     __fastcall  begin_setup();
        void     __fastcall  end_setup  ();
        void     __fastcall  setup_controls(LPMODBUS_MOD_CONFIG mc);
        void     __fastcall  after_set_gkhandle();
        bool     __fastcall  on_enum_handle(GKHANDLE parent,GKHANDLE h,LPVOID data);
        bool     __fastcall  get_mod_config(LPMODBUS_MOD_CONFIG mcfg);
        bool     __fastcall  set_mod_config(LPMODBUS_MOD_CONFIG mcfg,DWORD mask);
        void     __fastcall  set_ch_mask(DWORD val);

public   :		// User declarations
        __fastcall TModbusModConfig(TComponent* Owner);
        __property DWORD        ch_mask    = {read = Fch_mask,write = set_ch_mask};
 bool   __fastcall  apply_changes();
 bool   __fastcall  undo_changes();
};
//---------------------------------------------------------------------------
extern PACKAGE TModbusModConfig *ModbusModConfig;
//---------------------------------------------------------------------------
#endif
