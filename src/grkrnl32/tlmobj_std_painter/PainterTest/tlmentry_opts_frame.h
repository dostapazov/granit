//---------------------------------------------------------------------------

#ifndef tlmentry_opts_frameH
#define tlmentry_opts_frameH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.Buttons.hpp>
//---------------------------------------------------------------------------
class Ttlm_entry_opts_frame : public TFrame
{
__published:	// IDE-managed Components
	TGridPanel *GridPanel1;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TLabel *Label4;
	TRadioGroup *RadioGroup1;
	TLabel *Label5;
	TPanel *Panel1;
	TEdit *Edit1;
	TUpDown *UpDown1;
	TLabel *Label6;
	TPanel *Panel2;
	TEdit *Edit2;
	TUpDown *UpDown2;
	TColorBox *ColorBox1;
	TComboBox *ComboBox1;
	TColorBox *ColorBox2;
	TSpeedButton *SpeedButton2;
	TSpeedButton *SpeedButton3;
	TSpeedButton *SpeedButton1;
private:	// User declarations
public:		// User declarations
	__fastcall Ttlm_entry_opts_frame(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE Ttlm_entry_opts_frame *tlm_entry_opts_frame;
//---------------------------------------------------------------------------
#endif
