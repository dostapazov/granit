//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "gen_license.h"
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TGenLicenseForm *GenLicenseForm;
//---------------------------------------------------------------------------
__fastcall TGenLicenseForm::TGenLicenseForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TGenLicenseForm::SpeedButton1Click(TObject *Sender)
{
 if(ModOpenDlg->Execute())
   {
    ModuleName->Text = ModOpenDlg->FileName;
    clear_controls();
   }
}
//---------------------------------------------------------------------------


void __fastcall TGenLicenseForm::bGenTrialLicenseClick(TObject *Sender)
{
   AnsiString str = InputBox(L"Длительность пробного периода",L"Количество дней",L"30");
   if(str.Length())
   {
    if(!lic.generate_timed_license(ModuleName->Text.c_str(),str.ToInt(),Comment->Text.c_str()));
      {
        UnicodeString err_str = L"Ошибка создания файла лицензии";
        MessageDlg(err_str,mtError,TMsgDlgButtons()<<mbOK,0);
      }
    lic.close();
   }
}
//---------------------------------------------------------------------------

void __fastcall TGenLicenseForm::ModuleNameChange(TObject *Sender)
{
  bGenTrialLicense->Enabled = !ModuleName->Text.IsEmpty();
}
//---------------------------------------------------------------------------

void __fastcall TGenLicenseForm::get_license_info()
{
       wchar_t buf[MAX_PATH];
       if(lic.get_lic_computer_name(buf,KERTL_ARRAY_COUNT(buf)))
         CompName->Text = buf;
       if(lic.get_lic_volume_name(buf,KERTL_ARRAY_COUNT(buf)))
         VolumeInfo->Text = buf;
       if(lic.get_lic_mem_info(buf,KERTL_ARRAY_COUNT(buf)))
         MemInfo->Text = buf;
       if(lic.get_lic_uid_str(buf,KERTL_ARRAY_COUNT(buf)))
         UID->Text = buf;
         IsActive->Checked = lic.is_license_active();
       lic.get_module_name(buf,sizeof(buf));
       ModuleName->Text = buf;
       DWORD t1,t2;
       lic.get_times(&t1,&t2);
       WorkedTimes->Text = t1;
       ExpiredTime->Text = t2;
       DWORD len = lic.get_comment(buf,KERTL_ARRAY_COUNT(buf));
       buf[len]=0;
       Comment->Text = buf;
}

void __fastcall TGenLicenseForm::bOpenLicenceClick(TObject *Sender)
{
  if(LicenseOpenDlg->Execute(Handle))
  {
    if(lic.open_license(LicenseOpenDlg->FileName.c_str()))
    {
      get_license_info();

    }
  }
}

//---------------------------------------------------------------------------
void __fastcall TGenLicenseForm::bCheckLicenseClick(TObject *Sender)
{
  if(LicenseOpenDlg->Execute(Handle))
  {
    if(lic.open_license(LicenseOpenDlg->FileName.c_str()))
    {
      wchar_t mod_name[MAX_PATH];
      lic.get_module_name(mod_name,KERTL_ARRAY_COUNT(mod_name));
      if(lic.is_valid_license(mod_name))
      {
       get_license_info();
      }
    }
  }
}
//---------------------------------------------------------------------------

void __fastcall TGenLicenseForm::bIncTimeClick(TObject *Sender)
{
  if(lic.is_opened())
  {
    lic.time_increase(100);
    get_license_info ();
  }
}
//---------------------------------------------------------------------------

void __fastcall TGenLicenseForm::bActivateClick(TObject *Sender)
{
  UnicodeString key_str;
  if(lic.is_opened() && InputQuery(L"Activation",L"Input key string",key_str))
  {
   if(lic.activate_license(key_str.c_str(),key_str.Length()))
      get_license_info();
  }

}
//---------------------------------------------------------------------------

void __fastcall TGenLicenseForm::clear_controls()
{
    UnicodeString s;
    CompName->Text = s;
    VolumeInfo->Text = s;
    MemInfo->Text = s;
    UID->Text = s;
    WorkedTimes->Text = s;
    ExpiredTime->Text = s;
    Comment->Text = s;

}


void __fastcall TGenLicenseForm::SpeedButton2Click(TObject *Sender)
{
  lic.close();
  clear_controls();
}
//---------------------------------------------------------------------------

