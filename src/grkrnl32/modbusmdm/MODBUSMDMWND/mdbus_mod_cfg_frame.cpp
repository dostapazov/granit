//---------------------------------------------------------------------------

#pragma hdrstop
#include <vcl.h>

#include "mdbus_mod_cfg_frame.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "gkmod_frame"
#pragma resource "*.dfm"
TModbusModConfig *ModbusModConfig;
//---------------------------------------------------------------------------
__fastcall TModbusModConfig::TModbusModConfig(TComponent* Owner)
        : TGKModuleFrame(Owner)
{
  in_setup    = 0;
  Fon_changed = NULL;
  Fch_mask    = 0;
}
//---------------------------------------------------------------------------
void     __fastcall  TModbusModConfig::after_set_gkhandle()
{
 mod_config.dw_size = sizeof(mod_config);
 if(get_mod_config(&mod_config))
     setup_controls(&mod_config);
}


void     __fastcall  TModbusModConfig::begin_setup   ()
{
 ++in_setup;
}

void     __fastcall  TModbusModConfig::end_setup     ()
{
 in_setup = std::max(--in_setup,0);
}


int __fastcall idx2ComSpeed(int speed)
{
 int i = 0;
 switch(speed)
 {
  case 0  : i =  CBR_1200   ;break;
  case 1  : i =  CBR_2400   ;break;
  case 2  : i =  CBR_4800   ;break;
  case 3  : i =  CBR_9600   ;break;
  case 4  : i =  CBR_19200  ;break;
  case 5  : i =  CBR_38400  ;break;
  case 6  : i =  CBR_57600  ;break;
  case 7  : i =  CBR_115200 ;break;
 }
 return i;
}


void     __fastcall  TModbusModConfig::setup_controls(LPMODBUS_MOD_CONFIG mc)
{
 if(mc)
 {
  begin_setup();
  ModemNumber->Position =  mc->modem_number;
  PuNumber->Position    =  mc->pu_number;
  PuName->Text          =  AnsiString(mc->pu_name);

  end_setup  ();
  ch_mask = 0;
 }
}


bool     __fastcall  TModbusModConfig::on_enum_handle(GKHANDLE parent,GKHANDLE h,LPVOID data)
{
 return false;
}

bool     __fastcall  TModbusModConfig::get_mod_config(LPMODBUS_MOD_CONFIG mcfg)
{
 bool ret = false;
 if(mcfg)
 {
   char buffer[MODBUS_CONFIG_BUFF_SIZE];
   LPMODULE_CONFIG_DATA mcd = (LPMODULE_CONFIG_DATA)buffer;
   mcd->dw_sz = sizeof(MODBUS_CONFIG_BUFF_SIZE);
   LPMODBUS_MOD_CONFIG  mmod_cfg = (LPMODBUS_MOD_CONFIG)mcd->data;
   mmod_cfg->dw_size = sizeof(*mmod_cfg);
   mcd->mask = -1;
   if( mod_iface.get_module_config_data(mcd,MODBUS_CONFIG_BUFF_SIZE))
   {
    *mcfg = *mmod_cfg;
    ret = true;
   }
 }
 return ret;
}

bool     __fastcall  TModbusModConfig::set_mod_config(LPMODBUS_MOD_CONFIG mcfg,DWORD mask)
{
 mcfg->dw_size      = sizeof(*mcfg);
 mcfg->modem_number = ModemNumber->Position;
 mcfg->pu_number    = PuNumber   ->Position;
 safe_strcpyn(mcfg->pu_name,WideString(PuName->Text).c_bstr(),KERTL_ARRAY_COUNT(mcfg->pu_name));
 

 return true;

}


 bool   __fastcall  TModbusModConfig::undo_changes()
 {
  bool ret = true;
  if(ch_mask)
  {
   setup_controls(&mod_config);
  }
  return ret;
 }

 bool     __fastcall  TModbusModConfig::apply_changes()
 {
  bool ret = true;
  if(ch_mask)
  {
   char buffer[MODBUS_CONFIG_BUFF_SIZE];
   LPMODULE_CONFIG_DATA mcd = (LPMODULE_CONFIG_DATA)buffer;
   mcd->dw_sz = MODBUS_CONFIG_BUFF_SIZE;
   LPMODBUS_MOD_CONFIG  mmod_cfg = (LPMODBUS_MOD_CONFIG)mcd->data;
   mmod_cfg->dw_size = sizeof(*mmod_cfg);
   mcd->mask = ch_mask;
   set_mod_config(mmod_cfg,-1);
   ret = mod_iface.set_module_config_data(mcd,MODBUS_CONFIG_BUFF_SIZE);
   if(ret) ch_mask = 0;
  }
  return ret;
 }

 void     __fastcall  TModbusModConfig::set_ch_mask(DWORD val)
 {
   if(Fch_mask != val)
    {
     Fch_mask = val;
     if(on_changed) on_changed(this);
    }
 }




void __fastcall TModbusModConfig::Edit1Change(TObject *Sender)
{
  if(!in_setup)
     {
      int new_value = atoi(Edit1->Text.c_str());
      if(int(mod_config.modem_number) !=new_value)
        ch_mask |=  CCFL_MODEM_NUMBER;
        else
        ch_mask &= ~CCFL_MODEM_NUMBER;
     }
}
//---------------------------------------------------------------------------

void __fastcall TModbusModConfig::Edit2Change(TObject *Sender)
{
  if(!in_setup)
     {
      int new_value = atoi(Edit2->Text.c_str());
      if(int(mod_config.pu_number) !=new_value)
        ch_mask |=  CCFL_PUNUMBER;
        else
        ch_mask &= ~CCFL_PUNUMBER;
     }
}
//---------------------------------------------------------------------------

void __fastcall TModbusModConfig::PuNameChange(TObject *Sender)
{
  if(!in_setup)
  {
   if(AnsiString(mod_config.pu_name).AnsiCompare(PuName->Text))
    ch_mask |= CCFL_PUNAME;
    else
    ch_mask &= ~CCFL_PUNAME;
  }

}
//---------------------------------------------------------------------------






