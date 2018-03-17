//---------------------------------------------------------------------------

#ifndef record_propsH
#define record_propsH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "gkmodule_form.h"
#include "record_frame.h"
#include <Vcl.Buttons.hpp>
#include <Vcl.ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TRecordProps : public TGKModuleForm
{
__published:	// IDE-managed Components
	TPanel *Panel1;
	TSpeedButton *SpeedButton1;
	TSpeedButton *SpeedButton2;
	TRecordFrame *RecordFrame1;
	void __fastcall FormDeactivate(TObject *Sender);
	void __fastcall SpeedButton1Click(TObject *Sender);
	void __fastcall SpeedButton2Click(TObject *Sender);
private:	// User declarations
	__fastcall TRecordProps(TComponent* Owner);
void     __fastcall on_module_state(GKHANDLE mod,DWORD state) {};
void     __fastcall on_module_config_change(GKHANDLE mod,LPMODULE_CONFIG_DATA mcd){};

public:		// User declarations
virtual __fastcall TRecordProps(TComponent* Owner,GKHANDLE handle);
};
//---------------------------------------------------------------------------
extern PACKAGE TRecordProps *RecordProps;
//---------------------------------------------------------------------------
#endif
