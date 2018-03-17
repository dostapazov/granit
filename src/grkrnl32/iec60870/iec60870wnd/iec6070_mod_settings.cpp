//---------------------------------------------------------------------------
#pragma hdrstop
#include <vcl.h>
#include "iec6070_mod_settings.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "gkmod_frame"
#pragma resource "*.dfm"
TIEC60870ModSettings *IEC60870ModSettings;
//---------------------------------------------------------------------------
__fastcall TIEC60870ModSettings::TIEC60870ModSettings(TComponent* Owner)
	: TGKModuleFrame(Owner)
{
 Fin_setup = 0;
}
//---------------------------------------------------------------------------

bool     __fastcall  TIEC60870ModSettings::get_settings(LPIEC60870_MOD_CONFIG mcfg,bool from_controls)
{
  if(from_controls)
  {
     mcfg->modem_number = _wtoi(this->modem_number->Text.c_str());
     mcfg->pu_number    = _wtoi(this->pu_number->Text.c_str());
     safe_strcpyn(mcfg->pu_name,this->pu_name->Text.c_str(),sizeof(mcfg->pu_name)/sizeof(mcfg->pu_name[0]));
     return true;
  }
  else
  {
     BYTE buf[IEC60870_MCD_SIZE] ;
     memset(buf,0,sizeof(buf));
     LPMODULE_CONFIG_DATA mcd  = (LPMODULE_CONFIG_DATA)buf;
     mcd->dw_sz = IEC60870_MCD_SIZE;
     LPIEC60870_MOD_CONFIG      src_cfg = (LPIEC60870_MOD_CONFIG)mcd->data;
     src_cfg->dw_size = sizeof(*mcfg);
     mcd->mask = -1;
     if(mod_iface.get_module_config_data(mcd,mcd->dw_sz))
     {
       memcpy(mcfg,src_cfg,sizeof(*mcfg));
       return true;
     }
  }
  return false;
}

void     __fastcall  TIEC60870ModSettings::setup_controls(LPIEC60870_MOD_CONFIG mcfg)
{
  ++in_setup;
    if(mcfg->modem_number && mcfg->modem_number < OTD_ADDR_MAXVALUE)
       modem_number->Text = mcfg->modem_number;
       else
       modem_number->Text = UnicodeString();

    if(mcfg->pu_number && mcfg->pu_number < OTD_ADDR_MAXVALUE)
       pu_number->Text    = mcfg->pu_number;
       else
       pu_number->Text = UnicodeString();

       pu_name->Text      = mcfg->pu_name;
  --in_setup;
}


void     __fastcall  TIEC60870ModSettings::current_object_change (TObject * value)
{
  //TGKModuleFrame::current_object_change(value);
  if( get_settings(&mod_config,false))
      setup_controls(&mod_config);
}

bool     __fastcall  TIEC60870ModSettings::apply_changes ()
{
     BYTE buf[IEC60870_MCD_SIZE] ;
     memset(buf,0,sizeof(buf));
     LPMODULE_CONFIG_DATA mcd  = (LPMODULE_CONFIG_DATA)buf;
     mcd->dw_sz = IEC60870_MCD_SIZE;
     LPIEC60870_MOD_CONFIG      src_cfg = (LPIEC60870_MOD_CONFIG)mcd->data;
     src_cfg->dw_size = sizeof(*src_cfg);
     mcd->mask = ch_mask;
     get_settings(src_cfg,true);
     if(mod_iface.set_module_config_data(mcd,mcd->dw_sz))
       {
        mod_config = *src_cfg;
        return true;
       }
 return false;
}

bool     __fastcall  TIEC60870ModSettings::undo_changes  ()
{
  setup_controls(&this->mod_config);
  ch_mask = 0;
  return true;
}

void __fastcall TIEC60870ModSettings::modem_numberChange(TObject *Sender)
{
  TComponent * comp = dynamic_cast<TComponent *>(Sender);
  if(comp)
     ch_mask |= comp->Tag;
}
//---------------------------------------------------------------------------

