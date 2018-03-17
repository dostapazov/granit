//---------------------------------------------------------------------------

#pragma warn -8058
#include <vcl.h>
#pragma warn .8058
#pragma hdrstop

#include "settings_frame.h"
#include <gkio_base.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TSettingsFrame::TSettingsFrame(TComponent* Owner)
        : TSCBaseFrame(Owner)
{
}
//---------------------------------------------------------------------------
#define CONFIG_DATA_SIZE (sizeof(MODULE_CONFIG_DATA)+sizeof(grsc_settings)-sizeof(BYTE))

void    __fastcall TSettingsFrame::after_set_gkhandle()
{
 undo_changes();
 
}


bool    __fastcall   TSettingsFrame::apply_changes()
{
 bool ret = false;
 if(changes_mask)
 {
   BYTE buf[CONFIG_DATA_SIZE];
   LPMODULE_CONFIG_DATA mcd =(LPMODULE_CONFIG_DATA)buf;
   mcd->dw_sz = sizeof(buf);
   mcd->mask  = changes_mask;
   lpgrsc_settings s = (lpgrsc_settings)mcd->data;
   memcpy(s,&sc_settings,sizeof(*s));
   s->dw_size        = sizeof(*s);
   ret = mod_iface.call(MCMD_SET_MODULECONFIG_DATA,(LPARAM)mcd,sizeof(buf)) ?false:true;
 }
 return ret;

}

bool    __fastcall   TSettingsFrame::undo_changes()
{
 bool ret = false;
 BYTE buff[CONFIG_DATA_SIZE];
 LPMODULE_CONFIG_DATA mcd = (LPMODULE_CONFIG_DATA)buff;
 mcd->dw_sz = CONFIG_DATA_SIZE;
 mcd->mask  = -1;
 lpgrsc_settings  st =(lpgrsc_settings) mcd->data;
 sc_settings.modem_number = sc_settings.pu_number = -1;
 sc_settings.io_module[0] = sc_settings.device_name[0] = sc_settings.pu_name[0] = 0;
 if(mod_iface.get_module_config_data(mcd,mcd->dw_sz))
    {
     memcpy(&sc_settings,st,sizeof(*st));
     ret = true;
    } 
 ModemNum->Text = st->modem_number;
 PuNum->Text    = st->pu_number;
 PuName->Text   = st->pu_name;
 IOModule->Text = st->io_module;
 IODevice->Text = st->device_name;
 this->changes_mask = 0;
 return ret;
}


void        __fastcall TSettingsFrame::config_change(DWORD mask,lpgrsc_settings s)
{

}

void __fastcall TSettingsFrame::Button1Click(TObject *Sender)
{
 if(OpenDialog1->Execute())
   {
    IOModule->Text = OpenDialog1->FileName;
    changes_mask = CCFL_DEVICE; 
   }
}
//---------------------------------------------------------------------------
void __fastcall TSettingsFrame::IODeviceDropDown(TObject *Sender)
{
   IODevice->Items->Clear();
   TGKIoInterface  io;
   if(io.load(mod_iface(),IOModule->Text.c_str(),NULL))
   {
    MIO_ENUM_DEVICES ed;
    ed.dw_size  = sizeof(ed);
    ed.index = 0;
    while(io.enum_devices(&ed) == GKH_RET_SUCCESS)
    {
     IODevice->Items->Add(ed.dev_name);
     ed.index++;
    }
   }

        
}
//---------------------------------------------------------------------------
