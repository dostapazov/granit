//---------------------------------------------------------------------------
#pragma hdrstop
#include <vcl.h>
#include "mod_config_frame.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "gkmod_frame"
#pragma resource "*.dfm"
TVtdModConfig *VtdModConfig;
//---------------------------------------------------------------------------
__fastcall TVtdModConfig::TVtdModConfig(TComponent* Owner)
        : TGKModuleFrame(Owner)
{
  Fon_changed = NULL;
}
//---------------------------------------------------------------------------

void    __fastcall TVtdModConfig::set_ch_mask(DWORD mask)
{
 if(Fch_mask!= mask)
  {
   Fch_mask = mask;
   if(on_changed && Visible)
      on_changed(this);
  }
}

void    __fastcall TVtdModConfig::after_set_gkhandle()
{
 ZeroMemory(&mod_config,sizeof(mod_config));
 mod_config.dw_size = sizeof(mod_config);
 get_config(&mod_config);
 setup(&mod_config);
 ch_mask = 0;
}


bool    __fastcall TVtdModConfig::get_config(LPVTDCNTMOD_CONFIG mc)
{
 BYTE buffer [sizeof(MODULE_CONFIG_DATA)+sizeof(*mc)];
 LPMODULE_CONFIG_DATA mcd =(LPMODULE_CONFIG_DATA)buffer;
 mcd->dw_sz = sizeof(buffer);
 mcd->mask  = -1;
 LPVTDCNTMOD_CONFIG _mod_config =(LPVTDCNTMOD_CONFIG)mcd->data;
 _mod_config->dw_size = sizeof(*_mod_config);
 if(GKH_RET_SUCCESS == mod_iface.get_module_config_data(mcd,sizeof(buffer)))
 {
  if(mc)
  {

   memcpy(mc,_mod_config,sizeof(*_mod_config));
   if(mc->modem_number ==(DWORD)-1)
      mc->modem_number = 255;
   if(mc->pu_number ==(DWORD)-1)
      mc->pu_number = 255;
   return true;
  }
 }
 return false;
}

void    __fastcall TVtdModConfig::setup(LPVTDCNTMOD_CONFIG mc)
{
  VTDCNTMOD_CONFIG mod_config = {0};
  mod_config.dw_size = sizeof(mod_config);
  if(!mc)
  {
     mc = &mod_config;
     get_config(mc);
  }
 ModemNumber->Position = KeRTL::MIN(mc->modem_number,(DWORD)255);
 PuNumber->Position    = KeRTL::MIN(mc->pu_number,(DWORD)255);
 PuName->Text          = mc->pu_name;
 ServerAddr->Text      = mc->server_addr;
 ServerPort->Text      = mc->server_port ? AnsiString(mc->server_port):AnsiString();
 ch_mask = 0;
}

bool  __fastcall TVtdModConfig::apply_changes()
{
 DWORD ret = 0;
 BYTE buffer [sizeof(MODULE_CONFIG_DATA)+sizeof(VTDCNTMOD_CONFIG)] = {0};
 LPMODULE_CONFIG_DATA mcd =(LPMODULE_CONFIG_DATA)buffer;
 mcd->dw_sz = sizeof(buffer);
 mcd->mask  = -1;
 LPVTDCNTMOD_CONFIG _mod_config =(LPVTDCNTMOD_CONFIG)mcd->data;
 _mod_config->dw_size = sizeof(*_mod_config);
 _mod_config->modem_number = ModemNumber->Position;
 _mod_config->pu_number    = PuNumber->Position;
 if(PuName->Text.Length())
   wcsncpy(_mod_config->pu_name,WideString(PuName->Text).c_bstr(),KERTL_ARRAY_COUNT(_mod_config->pu_name));
 if(ServerAddr->Text.Length())
   wcsncpy(_mod_config->server_addr,WideString(ServerAddr->Text).c_bstr(),KERTL_ARRAY_COUNT(_mod_config->server_addr));
 _mod_config->server_port = atoi(ServerPort->Text.c_str());
 
 if(mod_iface.set_module_config_data(mcd,sizeof(buffer)))
 {
  ret = false;
 }
 else
 ch_mask = 0;

 return ret;
}

bool  __fastcall TVtdModConfig::undo_changes()
{

 setup(&mod_config);
 return true;
}


void __fastcall TVtdModConfig::Edit1Change(TObject *Sender)
{
  //Изменение номера модема
  DWORD mn = atoi(Edit1->Text.c_str());
  if(mn != mod_config.modem_number)
  {
  ch_mask |=CCFL_MODEM_NUMBER;
  }
  else
  ch_mask &=~CCFL_MODEM_NUMBER;

}
//---------------------------------------------------------------------------

void __fastcall TVtdModConfig::Edit2Change(TObject *Sender)
{
  //Номер ПУ
  DWORD pn = atoi(Edit2->Text.c_str());
  if(pn != mod_config.pu_number)
  {
  ch_mask |=CCFL_PUNUMBER;
  }
  else
  ch_mask &=~CCFL_PUNUMBER;
}
//---------------------------------------------------------------------------

void __fastcall TVtdModConfig::PuNameChange(TObject *Sender)
{
 if(AnsiString(mod_config.pu_name).AnsiCompareIC( PuName->Text) )
 {
 ch_mask|= CCFL_PUNAME;
 }
 else
 ch_mask&=~CCFL_PUNAME;
}
//---------------------------------------------------------------------------

void __fastcall TVtdModConfig::ServerAddrChange(TObject *Sender)
{
 if(AnsiString(mod_config.server_addr).AnsiCompareIC(ServerAddr->Text))
 {
 ch_mask|= CCFL_SERVER_ADDR;
 }
 else
 ch_mask&=~CCFL_SERVER_ADDR;
}
//---------------------------------------------------------------------------

void __fastcall TVtdModConfig::ServerPortChange(TObject *Sender)
{
   int port_num = atoi(ServerPort->Text.c_str());
   if(mod_config.server_port!=port_num)
   {
    ch_mask|= CCFL_SERVER_PORT;
   }
   else
   ch_mask&=~CCFL_SERVER_PORT;


}
//---------------------------------------------------------------------------

