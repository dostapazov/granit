//---------------------------------------------------------------------------

#ifndef settings_frame1H
#define settings_frame1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "sc_base_frame.h"
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
class TSCSettingsFrm : public TSCBaseFrame
{
__published:	// IDE-managed Components
        TGroupBox *SettingsBox;
        TLabel *Label1;
        TLabel *Label2;
        TLabel *Label3;
        TLabel *Label4;
        TLabel *Label5;
        TEdit *PuNum;
        TEdit *ModemNum;
        TEdit *PuName;
        TEdit *IOModule;
        TButton *Button1;
        TComboBox *IODevice;
        TOpenDialog *OpenDialog1;
        TLabel *Label6;
        TEdit *SaveFileName;
        TButton *SelFolderBtn;
        TOpenDialog *OpenDialog2;
        TButton *Button2;
        TCheckBox *EnableWatchdog;
        TLabel *Label7;
        TEdit *IOSpeed;
        void __fastcall Button1Click(TObject *Sender);
        void __fastcall IODeviceDropDown(TObject *Sender);
        void __fastcall ModemNumChange(TObject *Sender);
        void __fastcall PuNumChange(TObject *Sender);
        void __fastcall PuNameChange(TObject *Sender);
        void __fastcall IOModuleChange(TObject *Sender);
        void __fastcall SelFolderBtnClick(TObject *Sender);
        void __fastcall SaveFileNameChange(TObject *Sender);
        void __fastcall Button2Click(TObject *Sender);
        void __fastcall EnableWatchdogClick(TObject *Sender);
        void __fastcall IODeviceChange(TObject *Sender);
        void __fastcall IOSpeedChange(TObject *Sender);
private:	// User declarations
        grsc_settings  sc_settings;
        void    __fastcall after_set_gkhandle();
        bool    __fastcall apply_changes() ;
        bool    __fastcall undo_changes () ;
        void    __fastcall setup_controls();

public:		// User declarations
        __fastcall TSCSettingsFrm(TComponent* Owner);
grsc_settings & __fastcall get_settings() {return sc_settings;}
void             __fastcall make_settings(grsc_settings & scs) ;
        void    __fastcall config_change(DWORD mask,lpgrsc_settings s);
     AnsiString __fastcall get_text();        
        

};
//---------------------------------------------------------------------------
extern PACKAGE TSCSettingsFrm *SCSettingsFrm;
//---------------------------------------------------------------------------
#endif
