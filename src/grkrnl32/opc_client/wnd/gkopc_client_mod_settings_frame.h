//---------------------------------------------------------------------------

#ifndef gkopc_client_mod_settings_frameH
#define gkopc_client_mod_settings_frameH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "gkmod_frame.h"
#include <gkopc_client.h>

//---------------------------------------------------------------------------
class TOpcModSettings : public TGKModuleFrame
{
__published:	// IDE-managed Components
	TLabel *Label1;
	TEdit *ModemNumber;
	TLabel *Label2;
	TEdit *PuNumber;
	TLabel *Label3;
	TEdit *PuName;
	void __fastcall ModemNumberChange(TObject *Sender);
private:	// User declarations
         GKOPC_CLIENT_MOD_CONFIG mod_config;
         void     __fastcall  current_object_change (TObject * value);
virtual  bool     __fastcall  apply_changes ();
virtual  bool     __fastcall  undo_changes  ();
virtual  bool     __fastcall  delete_current(){return false;}

public:		// User declarations
	__fastcall TOpcModSettings(TComponent* Owner);
        bool     __fastcall get_settings   (LPGKOPC_CLIENT_MOD_CONFIG mod_conf,bool from_controls);
        void     __fastcall setup_controls (LPGKOPC_CLIENT_MOD_CONFIG mod_conf);

};
//---------------------------------------------------------------------------
extern PACKAGE TOpcModSettings *OpcModSettings;
//---------------------------------------------------------------------------
#endif
