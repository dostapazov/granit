//---------------------------------------------------------------------------

#ifndef gkipmodem_form_radH
#define gkipmodem_form_radH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include "gkmodule_form.h"
#include <Vcl.Buttons.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include "..\gkipmodem_mod.h"

//---------------------------------------------------------------------------
class TGKIpModemForm : public TGKModuleForm
{
__published:	// IDE-managed Components
	TPageControl *PageControl1;
	TTabSheet *SettingsPage;
	TLabel *Label3;
	TLabel *Label1;
	TLabel *Label2;
	TEdit *ServerAddr;
	TEdit *PortNumber;
	TGroupBox *AddressBox;
	TListView *AddressList;
	TCheckBox *AllAddress;
	TBitBtn *ApplyBtn;
	TBitBtn *UndoBtn;
	TEdit *ModemNumber;
	TRadioButton *AsClient;
	TRadioButton *AsServer;
	TBitBtn *StartBtn;
	TBitBtn *StopBtn;
	TTabSheet *TabSheet2;
	TListView *LinesList;
	TTabSheet *TabSheet1;
	TLabel *Label4;
	TLabel *Label30;
private:	// User declarations
public:		// User declarations
	__fastcall TGKIpModemForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TGKIpModemForm *GKIpModemForm;
//---------------------------------------------------------------------------
#endif
