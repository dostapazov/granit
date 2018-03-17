//---------------------------------------------------------------------------

#ifndef cpc152_mod_frameH
#define cpc152_mod_frameH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "gkmod_frame.h"
#include <cpc152dev.h>

//---------------------------------------------------------------------------


class TCpc152mod_frame : public TGKModuleFrame
{
__published:	// IDE-managed Components
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TEdit *modem_num;
	TEdit *pu_number;
	TEdit *pu_name;
	void __fastcall pu_nameChange(TObject *Sender);
private:	// User declarations
protected:
CPC152DEV_CONFIG config;
virtual void     __fastcall  current_object_change (TObject * value) ;
        bool     __fastcall  get_module_settings();
        bool     __fastcall  apply_changes ();
        bool     __fastcall  undo_changes  ();
virtual void     __fastcall  set_ch_mask(DWORD value);
        void     __fastcall  setup_controls();
        void     __fastcall  get_conf_values(LPCPC152DEV_CONFIG mod_cfg);
        bool     __fastcall  delete_current();
public:		// User declarations
	__fastcall TCpc152mod_frame(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TCpc152mod_frame *Cpc152mod_frame;
//---------------------------------------------------------------------------
#endif
