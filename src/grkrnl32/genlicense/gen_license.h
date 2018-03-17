//---------------------------------------------------------------------------

#ifndef gen_licenseH
#define gen_licenseH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Buttons.hpp>
#include <tlicense.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.ComCtrls.hpp>
//---------------------------------------------------------------------------


class TGenLicenseForm : public TForm
{
__published:	// IDE-managed Components
	TLabel *Label1;
	TEdit *ModuleName;
	TSpeedButton *SpeedButton1;
	TSpeedButton *bGenTrialLicense;
	TSpeedButton *bOpenLicence;
	TOpenDialog *ModOpenDlg;
	TOpenDialog *LicenseOpenDlg;
	TGroupBox *GroupBox1;
	TLabel *Label2;
	TEdit *CompName;
	TLabel *Label3;
	TEdit *VolumeInfo;
	TLabel *Label4;
	TEdit *MemInfo;
	TLabel *Label5;
	TEdit *UID;
	TCheckBox *IsActive;
	TLabel *Label6;
	TEdit *WorkedTimes;
	TLabel *Label7;
	TEdit *ExpiredTime;
	TSpeedButton *bCheckLicense;
	TStatusBar *StatusBar1;
	TSpeedButton *bIncTime;
	TSpeedButton *bActivate;
	TSpeedButton *SpeedButton2;
	TLabel *Label8;
	TEdit *Comment;
	void __fastcall SpeedButton1Click(TObject *Sender);
	void __fastcall bGenTrialLicenseClick(TObject *Sender);
	void __fastcall ModuleNameChange(TObject *Sender);
	void __fastcall bOpenLicenceClick(TObject *Sender);
	void __fastcall bCheckLicenseClick(TObject *Sender);
	void __fastcall bIncTimeClick(TObject *Sender);
	void __fastcall bActivateClick(TObject *Sender);
	void __fastcall SpeedButton2Click(TObject *Sender);
private:	// User declarations
protected:
       TLicense lic;
        void __fastcall get_license_info();
        void __fastcall clear_controls();
public:		// User declarations
	__fastcall TGenLicenseForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TGenLicenseForm *GenLicenseForm;
//---------------------------------------------------------------------------
#endif
