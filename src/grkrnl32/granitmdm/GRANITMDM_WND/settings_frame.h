//---------------------------------------------------------------------------


#ifndef settings_frameH
#define settings_frameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <granit_modem.h>
#include <Dialogs.hpp>
#include "sc_base_frame.h"


//---------------------------------------------------------------------------

typedef void __fastcall ( __closure * on_changes_event)(DWORD ch_mask);

class TSettingsFrame : public TSCBaseFrame
{
__published:	// IDE-managed Components
        TGroupBox *SettingsBox;
        TLabel *Label1;
        TEdit *PuNum;
        TLabel *Label2;
        TEdit *ModemNum;
        TLabel *Label3;
        TEdit *PuName;
        TLabel *Label4;
        TEdit *IOModule;
        TButton *Button1;
        TLabel *Label5;
        TComboBox *IODevice;
        TOpenDialog *OpenDialog1;
        void __fastcall Button1Click(TObject *Sender);
        void __fastcall IODeviceDropDown(TObject *Sender);
private:
        grsc_settings  sc_settings;
        void    __fastcall after_set_gkhandle();
virtual  bool  __fastcall apply_changes() ;
virtual  bool  __fastcall undo_changes () ;

public:		// User declarations
        __fastcall TSettingsFrame(TComponent* Owner);
        void    __fastcall set_gkhandle(GKHANDLE h){mod_iface.open(h,true);after_set_gkhandle();}
grsc_settings & __fastcall get_settings() {return sc_settings;}
        void    __fastcall config_change(DWORD mask,lpgrsc_settings s);


        
};
//---------------------------------------------------------------------------
extern PACKAGE TSettingsFrame *SettingsFrame;
//---------------------------------------------------------------------------
#endif
