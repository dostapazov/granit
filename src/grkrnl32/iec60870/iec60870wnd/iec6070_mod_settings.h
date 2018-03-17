//---------------------------------------------------------------------------

#ifndef iec6070_mod_settingsH
#define iec6070_mod_settingsH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "gkmod_frame.h"
#include <iec60870_modem.h>
//---------------------------------------------------------------------------
class TIEC60870ModSettings : public TGKModuleFrame
{
__published:	// IDE-managed Components
	TLabel *Label1;
	TEdit *modem_number;
	TLabel *Label2;
	TEdit *pu_number;
	TLabel *Label3;
	TEdit *pu_name;
	void __fastcall modem_numberChange(TObject *Sender);
private:	// User declarations
         IEC60870_MOD_CONFIG mod_config;
         bool     __fastcall  get_settings(LPIEC60870_MOD_CONFIG mcfg,bool from_controls);
         void     __fastcall  setup_controls(LPIEC60870_MOD_CONFIG mcfg);
virtual  void     __fastcall  current_object_change (TObject * value) ;
virtual  bool     __fastcall  apply_changes ();
virtual  bool     __fastcall  undo_changes  ();
public:		// User declarations
	__fastcall TIEC60870ModSettings(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TIEC60870ModSettings *IEC60870ModSettings;
//---------------------------------------------------------------------------
#endif
