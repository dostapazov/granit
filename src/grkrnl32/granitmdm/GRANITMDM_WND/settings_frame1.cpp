//---------------------------------------------------------------------------

#pragma warn -8058
#include <vcl.h>
#pragma warn .8058
#pragma hdrstop

#include "settings_frame1.h"
#include <gkio_base.h>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "sc_base_frame"
#pragma resource "*.dfm"
TSCSettingsFrm *SCSettingsFrm;
//---------------------------------------------------------------------------
__fastcall TSCSettingsFrm::TSCSettingsFrm(TComponent* Owner)
        : TSCBaseFrame(Owner)
{
}
//---------------------------------------------------------------------------

void    __fastcall TSCSettingsFrm::after_set_gkhandle()
{
  undo_changes();
}

AnsiString __fastcall TSCSettingsFrm::get_text()
{
 AnsiString ret(sc_settings.pu_name);
 ret+= '-';
 ret+= "Системная централь";
 return ret;

}

#define CONFIG_DATA_SIZE (sizeof(MODULE_CONFIG_DATA)+sizeof(grsc_settings)-sizeof(BYTE))

bool    __fastcall TSCSettingsFrm::apply_changes()
{
 bool ret = false;
 if(changes_mask)
 {
   BYTE buf[CONFIG_DATA_SIZE];
   LPMODULE_CONFIG_DATA mcd =(LPMODULE_CONFIG_DATA)buf;
   mcd->dw_sz = sizeof(buf);
   mcd->mask  = changes_mask;
   make_settings(sc_settings);
   lpgrsc_settings s = (lpgrsc_settings)mcd->data;
   memcpy(s,&sc_settings,sizeof(*s));
   s->dw_size        = sizeof(*s);
   ret = mod_iface.call(MCMD_SET_MODULECONFIG_DATA,(LPARAM)mcd,sizeof(buf)) ?false:true;
 }
 return ret;

}

void             __fastcall TSCSettingsFrm::make_settings(grsc_settings & scs)
{
 scs.dw_size      = sizeof(grsc_settings);
 scs.modem_number = _wtoi(ModemNum->Text.c_str());
 scs.pu_number    = _wtoi(PuNum->Text.c_str());
 WideString wstr;
 wstr =  IOModule->Text;
 safe_strcpy(scs.io_module,wstr.c_bstr());
 wstr =  IODevice->Text;
 safe_strcpy(scs.device_name,wstr.c_bstr());
 scs.io_speed = _wtoi(IOSpeed->Text.c_str());
 scs.enable_io_watchdog = EnableWatchdog->Checked;
 wstr =   SaveFileName->Text;
 safe_strcpy(scs.save_file_name,wstr.c_bstr());
 wstr =   PuName->Text;
 safe_strcpy(scs.pu_name,wstr.c_bstr());
 
} 


bool    __fastcall TSCSettingsFrm::undo_changes ()
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
 this->changes_mask = 0;
 this->setup_controls();
 return ret;
}

void __fastcall TSCSettingsFrm::Button1Click(TObject *Sender)
{
 if(OpenDialog1->Execute())
   {
    IOModule->Text = OpenDialog1->FileName;
    changes_mask = CCFL_DEVICE;
   }
}
//---------------------------------------------------------------------------

void __fastcall TSCSettingsFrm::IODeviceDropDown(TObject *Sender)
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

void        __fastcall TSCSettingsFrm::config_change(DWORD mask,lpgrsc_settings s)
{
 if(mask & CCFL_PUNAME)
 {
  safe_strcpyn(sc_settings.pu_name,s->pu_name,sizeof(sc_settings.pu_name)/sizeof(wchar_t));
  PuName->Font->Color= clDefault;
 }

 if(mask &CCFL_PUNUMBER)
 {
  sc_settings.pu_number = s->pu_number;
  PuNum->Font->Color= clDefault;
 }

 if(mask &CCFL_DEVICE)
 {
  safe_strcpyn(sc_settings.io_module,s->io_module,sizeof(sc_settings.io_module)/sizeof(wchar_t));
  IOModule->Font->Color= clDefault;

 }
 if(mask &CCFL_DEVICE_NAME)
 {
  safe_strcpyn(sc_settings.device_name,s->device_name,sizeof(sc_settings.device_name)/sizeof(wchar_t));
  IODevice->Font->Color= clDefault;
 }
 if(mask &CCFL_DEVICE_SPEED)
 {
  IOSpeed->Text = sc_settings.io_speed;
  IOSpeed->Font->Color= clDefault;
 }

 if(mask &CCFL_MODEM_NUMBER)
 {
   sc_settings.modem_number = s->modem_number;
   ModemNum->Font->Color= clDefault;
 }

 if(mask &CCFL_FILENAME)
 {
   safe_strcpyn(sc_settings.save_file_name , s->save_file_name,sizeof(sc_settings.save_file_name)/sizeof(wchar_t));
   SaveFileName->Font->Color= clDefault;
 }

 changes_mask&=~mask;
 setup_controls();

}
void    __fastcall TSCSettingsFrm::setup_controls()
{
 if(sc_settings.modem_number < OTD_ADDR_MAXVALUE)
    ModemNum->Text = sc_settings.modem_number;
    else
    ModemNum->Clear();
 if(sc_settings.pu_number < OTD_ADDR_MAXVALUE)
    PuNum->Text    = sc_settings.pu_number;
    else
    PuNum->Clear();
 PuName->Text   = sc_settings.pu_name;
 IOSpeed->Text  = sc_settings.io_speed;
 IODevice->Text = sc_settings.device_name;
 IOModule->Text = sc_settings.io_module;
 SaveFileName->Text = sc_settings.save_file_name;
 EnableWatchdog->Checked = sc_settings.enable_io_watchdog; 

 ModemNum->Font->Color = clDefault;
 PuNum->Font->Color    = clDefault;
 PuName->Font->Color = clDefault;
 IODevice->Font->Color = clDefault;
 IOModule->Font->Color = clDefault;
 SaveFileName->Font->Color = clDefault;

}



void __fastcall TSCSettingsFrm::ModemNumChange(TObject *Sender)
{
  DWORD new_num = _wtoi(ModemNum->Text.c_str());
  if(new_num!= sc_settings.modem_number)
    {
     changes_mask |=CCFL_MODEM_NUMBER;
     ModemNum->Font->Color = clBlue;
    }
    else
    {
     changes_mask &=~CCFL_MODEM_NUMBER;
     ModemNum->Font->Color = clDefault;
    }
}
//---------------------------------------------------------------------------


void __fastcall TSCSettingsFrm::PuNumChange(TObject *Sender)
{
  DWORD new_num = _wtoi(PuNum->Text.c_str());
  if(new_num!= sc_settings.pu_number)
    {
     changes_mask |=CCFL_PUNUMBER;
     PuNum->Font->Color = clBlue;
    }
    else
    {
     changes_mask &=~CCFL_PUNUMBER;
     PuNum->Font->Color = clDefault;
    }

}
//---------------------------------------------------------------------------

void __fastcall TSCSettingsFrm::PuNameChange(TObject *Sender)
{


  if(PuName->Text.Compare(sc_settings.pu_name))
  {
     changes_mask |=CCFL_PUNAME;
     PuName->Font->Color = clBlue;

  }
  else
  {
     changes_mask &=~CCFL_PUNAME;
     PuName->Font->Color = clDefault;
  }
}
//---------------------------------------------------------------------------


void __fastcall TSCSettingsFrm::IOModuleChange(TObject *Sender)
{
  AnsiString mod_name( sc_settings.io_module);
  mod_name = mod_name.Trim();
  bool mc  = mod_name.AnsiCompare(IOModule->Text);
  changes_mask = (mc ) ? (changes_mask|CCFL_DEVICE): (changes_mask&~CCFL_DEVICE);
  IOModule->Font->Color = mc ? clBlue:clDefault;

}
//---------------------------------------------------------------------------



void __fastcall TSCSettingsFrm::SelFolderBtnClick(TObject *Sender)
{
 if(OpenDialog2->Execute())
 {
  SaveFileName->Text = OpenDialog2->FileName;
 }
}
//---------------------------------------------------------------------------

void __fastcall TSCSettingsFrm::SaveFileNameChange(TObject *Sender)
{
 AnsiString str (sc_settings.save_file_name);
 if(str.AnsiCompareIC(SaveFileName->Text))
 {
  changes_mask|=CCFL_FILENAME;
  SaveFileName->Font->Color = clBlue;
  safe_strcpyn(sc_settings.save_file_name ,WideString(SaveFileName->Text).c_bstr(),sizeof(sc_settings.save_file_name)/sizeof(wchar_t));
 }
 else
 {
  changes_mask&=CCFL_FILENAME;
  SaveFileName->Font->Color = clDefault;
 }
}
//---------------------------------------------------------------------------

void __fastcall TSCSettingsFrm::Button2Click(TObject *Sender)
{
  if(OpenDialog2->Execute())
  {
   this->mod_iface(SCCM_IMPORT_SCDB,(LPARAM)WideString(OpenDialog2->FileName).c_bstr(),0);
  }
}
//---------------------------------------------------------------------------

void __fastcall TSCSettingsFrm::EnableWatchdogClick(TObject *Sender)
{
  changes_mask |= CCFL_IOWATCHDOG;
}
//---------------------------------------------------------------------------

void __fastcall TSCSettingsFrm::IODeviceChange(TObject *Sender)
{
  AnsiString dev_name( sc_settings.device_name );
  dev_name = dev_name.Trim();
  bool dc =  dev_name.AnsiCompare(IODevice->Text);
  changes_mask = ( dc ) ? (changes_mask|CCFL_DEVICE_NAME): (changes_mask&~CCFL_DEVICE_NAME);
  IODevice->Font->Color = dc ? clBlue:clDefault;

}
//---------------------------------------------------------------------------

void __fastcall TSCSettingsFrm::IOSpeedChange(TObject *Sender)
{
  bool sc = sc_settings.io_speed!=(DWORD)_wtoi(IOSpeed->Text.c_str());
  changes_mask = ( sc) ? (changes_mask|CCFL_DEVICE_SPEED): (changes_mask&~CCFL_DEVICE_SPEED);
  IOSpeed ->Font->Color = sc ? clBlue:clDefault;
}
//---------------------------------------------------------------------------

