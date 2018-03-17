//---------------------------------------------------------------------------

#ifndef comtrade_paramH
#define comtrade_paramH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.ComCtrls.hpp>
//---------------------------------------------------------------------------
class TComtradeParam : public TForm
{
__published:	// IDE-managed Components
	TLabeledEdit *ctName;
	TLabel *Label1;
	TComboBox *ctFreq;
	TLabel *Label2;
	TBitBtn *BitBtn1;
	TBitBtn *bOk;
	TLabeledEdit *ctFile;
	TCheckBox *CheckBox1;
	TOpenDialog *OpenDialog1;
	TEdit *ProgramName;
	TSpeedButton *bSelProgram;
	TComboBox *ctFreqSample;
	TCheckBox *cbAdjustSampleTime;
	TLabel *Label3;
	TEdit *Edit1;
	TUpDown *ResampleValue;
	void __fastcall ctNameChange(TObject *Sender);
	void __fastcall bSelProgramClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TComtradeParam(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TComtradeParam *ComtradeParam;
//---------------------------------------------------------------------------
#endif
