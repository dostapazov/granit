//---------------------------------------------------------------------------
#pragma hdrstop
#include <vcl.h>
#include "gkopc_client_mod_settings_frame.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "gkmod_frame"
#pragma resource "*.dfm"
TOpcModSettings *OpcModSettings;
//---------------------------------------------------------------------------
__fastcall TOpcModSettings::TOpcModSettings(TComponent* Owner)
	: TGKModuleFrame(Owner)
{
}
//---------------------------------------------------------------------------

bool     __fastcall TOpcModSettings::get_settings   (LPGKOPC_CLIENT_MOD_CONFIG mod_conf,bool from_controls)
{
   if(from_controls)
   {
     UnicodeString
     str = ModemNumber->Text.Trim();
     mod_conf->modem_number = str.IsEmpty() ? -1 : _wtoi(str.c_str());
     str = PuNumber->Text.Trim();
     mod_conf->pu_number = str.IsEmpty() ? -1 : _wtoi(str.c_str());
     str = PuName->Text;
     safe_strcpyn(mod_conf->pu_name,str.c_str(),KERTL_ARRAY_COUNT(mod_conf->pu_name));
   }
   else
   {
       char buff[GKOPCMODEM_MCD_SIZE];
       bzero(buff,sizeof(buff));
       LPMODULE_CONFIG_DATA mcd = (LPMODULE_CONFIG_DATA)buff;
       mcd->dw_sz = sizeof(buff);
       mcd->mask  = -1;
       LPGKOPC_CLIENT_MOD_CONFIG mcfg = (LPGKOPC_CLIENT_MOD_CONFIG)mcd->data;
       mcfg->dw_size = sizeof(*mcfg);
       if(mod_conf && mod_iface.get_module_config_data(mcd,sizeof(buff)) )
       {
        memcpy(mod_conf,mcfg,sizeof(*mod_conf));
        return  true;
       }
   }
  return false;
}

void     __fastcall TOpcModSettings::setup_controls (LPGKOPC_CLIENT_MOD_CONFIG mod_conf)
{
  ++in_setup;
  if(mod_conf->modem_number == (DWORD)-1)
   ModemNumber->Text = UnicodeString();
   else
   ModemNumber->Text = mod_conf->modem_number;

  if(mod_conf->pu_number == (DWORD)-1)
    PuNumber->Text    = UnicodeString();
    else
    PuNumber->Text    = mod_conf->pu_number;
  PuName->Text      = mod_conf->pu_name;
  --in_setup;
  ch_mask = 0;

}

void     __fastcall  TOpcModSettings::current_object_change (TObject * value)
{
  //TGKModuleFrame::current_object_change(value);
  if( get_settings(&mod_config,false))
      setup_controls(&mod_config);
}



void __fastcall TOpcModSettings::ModemNumberChange(TObject *Sender)
{
 if(!in_setup)
 {
  TComponent * comp = dynamic_cast<TComponent*>(Sender);
  if(comp)
     ch_mask |= comp->Tag;
 }
}
//---------------------------------------------------------------------------

bool     __fastcall  TOpcModSettings::apply_changes ()
{
       char buff[GKOPCMODEM_MCD_SIZE];
       bzero(buff,sizeof(buff));
       LPMODULE_CONFIG_DATA mcd = (LPMODULE_CONFIG_DATA)buff;
       mcd->dw_sz = sizeof(buff);
       mcd->mask  = -1;
       LPGKOPC_CLIENT_MOD_CONFIG mcfg = (LPGKOPC_CLIENT_MOD_CONFIG)mcd->data;
       mcfg->dw_size = sizeof(*mcfg);
       get_settings(mcfg,true);
       if(GKH_RET_SUCCESS == mod_iface.set_module_config_data(mcd,mcd->dw_sz))
         {
          memcpy(&mod_config,mcd,sizeof(mod_config));
          ch_mask = 0;
          return true;
         }
    return false;
}

bool     __fastcall  TOpcModSettings::undo_changes  ()
{
  setup_controls(&mod_config);
  return true;
}


