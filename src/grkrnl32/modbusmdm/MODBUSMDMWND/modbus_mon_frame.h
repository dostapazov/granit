//---------------------------------------------------------------------------

#ifndef modbus_mon_frameH
#define modbus_mon_frameH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <text_viewer.hpp>
#include <Vcl.Buttons.hpp>


typedef KeWin::TTextViewer<wchar_t> TModMon;
//---------------------------------------------------------------------------
class TMonFrame : public TFrame
{
__published:	// IDE-managed Components
	TPanel *Panel1;
	TGroupBox *MonBox;
	TSpeedButton *pause_btn;
	TSpeedButton *clr_btn;
	TCheckBox *cbShowDetails;
	TCheckBox *EnableRX;
	TCheckBox *EnableTX;
	void __fastcall FrameResize(TObject *Sender);
	void __fastcall clr_btnClick(TObject *Sender);
	void __fastcall pause_btnClick(TObject *Sender);
private:	// User declarations
protected:
TModMon         mon;
public:		// User declarations
	__fastcall TMonFrame(TComponent* Owner);
void __fastcall create_mon_instance();
void __fastcall monitor_txrx(LPVOID data,DWORD sz,DWORD code);
void __fastcall mon_details_tx(LPVOID data,DWORD sz,DWORD code);
void __fastcall mon_details_rx(LPVOID data,DWORD sz,DWORD code);
void __fastcall mon_raw_data  (LPVOID data,DWORD sz);
void __fastcall mon_header    (DWORD code,DWORD sz);


};
//---------------------------------------------------------------------------
extern PACKAGE TMonFrame *MonFrame;
//---------------------------------------------------------------------------
#endif
