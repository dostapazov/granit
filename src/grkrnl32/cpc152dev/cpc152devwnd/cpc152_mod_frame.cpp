//---------------------------------------------------------------------------
#pragma hdrstop
#include <vcl.h>
#include "cpc152_mod_frame.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "gkmod_frame"
#pragma resource "*.dfm"
TCpc152mod_frame *Cpc152mod_frame;
//---------------------------------------------------------------------------
__fastcall TCpc152mod_frame::TCpc152mod_frame(TComponent* Owner)
	: TGKModuleFrame(Owner)
{
 Fch_mask = 0;
 memset(&config,0,sizeof(config));
 config.dw_size = sizeof(config);
}
//---------------------------------------------------------------------------

void     __fastcall  TCpc152mod_frame::current_object_change (TObject * value)
{
  if(get_module_settings())
  {
    ++in_setup;
    modem_num->Text = config.modem_number;
    pu_number->Text = config.pu_number;
    pu_name->Text   = config.pu_name;
    if(current_tree_node && config.pu_name[0])
       current_tree_node->Text = config.pu_name;
    --in_setup;
  }
}

void     __fastcall  TCpc152mod_frame::set_ch_mask(DWORD value)
{
  TGKModuleFrame::set_ch_mask(value);
}


bool     __fastcall TCpc152mod_frame::get_module_settings()
{
 BYTE buf[sizeof(MODULE_CONFIG_DATA)+sizeof(CPC152DEV_CONFIG)] ;
 memset(buf,0,sizeof(buf));
 LPMODULE_CONFIG_DATA mcd  = (LPMODULE_CONFIG_DATA)buf;
 mcd->dw_sz = MCD_SIZE;
 LPCPC152DEV_CONFIG     cfg = (LPCPC152DEV_CONFIG)mcd->data;
 cfg->dw_size = sizeof(*cfg);
 mcd->mask = -1;
 if(mod_iface.get_module_config_data(mcd,MCD_SIZE))
 {
   memcpy(&config,cfg,sizeof(config));
   setup_controls();
   return true;
 }
 return false;
}

bool     __fastcall  TCpc152mod_frame::apply_changes ()
{
 bool ret = true;
  if(ch_mask)
  {
   char buffer[MCD_SIZE];
   LPMODULE_CONFIG_DATA mcd = (LPMODULE_CONFIG_DATA)buffer;
   mcd->dw_sz = MCD_SIZE;
   LPCPC152DEV_CONFIG  mod_cfg = (LPCPC152DEV_CONFIG)mcd->data;
   mod_cfg->dw_size = sizeof(*mod_cfg);
   mcd->mask = ch_mask;
   get_conf_values(mod_cfg);
   ret = mod_iface.set_module_config_data(mcd,MCD_SIZE);
   if(ret) ch_mask = 0;
  }
 return ret;
}

bool     __fastcall  TCpc152mod_frame::undo_changes  ()
{
  setup_controls();
  return true;
}

void     __fastcall  TCpc152mod_frame::get_conf_values(LPCPC152DEV_CONFIG mod_cfg)
{
   mod_cfg->modem_number = _wtoi(modem_num->Text.c_str());
   mod_cfg->pu_number    = _wtoi(pu_number->Text.c_str());
   int sz = KERTL_ARRAY_COUNT(mod_cfg->pu_name);
   memset(mod_cfg->pu_name,0,sz);
   safe_strcpyn(mod_cfg->pu_name,pu_name->Text.c_str(),sz-1);
}

void     __fastcall  TCpc152mod_frame::setup_controls()
{
   ++in_setup;
   if(config.modem_number)
   modem_num->Text = config.modem_number;
   else
   modem_num->Text = UnicodeString();
   if(config.pu_number)
    pu_number->Text = config.pu_number;
   else
    pu_number->Text = UnicodeString();
   pu_name->Text   = config.pu_name;
   --in_setup;
   ch_mask = 0;
}



bool     __fastcall  TCpc152mod_frame::delete_current()
{
   return false;
}

void __fastcall TCpc152mod_frame::pu_nameChange(TObject *Sender)
{
   TEdit * ed = dynamic_cast<TEdit*>(Sender);
   if(ed)
   {
     this->ch_mask |= ed->Tag;
   }
}
//---------------------------------------------------------------------------

