//---------------------------------------------------------------------------

#ifndef gkuser_tools_formH
#define gkuser_tools_formH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <System.Win.ScktComp.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <otd_proto.h>
#include <modem_proto.h>
#include <gksecur_proto.h>

//---------------------------------------------------------------------------
class TForm2 : public TForm
{
__published:	// IDE-managed Components
	TPanel *pnl1;
	TGroupBox *grp1;
	TLabel *lbl1;
	TLabel *Label1;
	TSpeedButton *conn_btn;
	TEdit *gkServer;
	TEdit *gkSrvPort;
	TGroupBox *grp2;
	TLabel *Label2;
	TEdit *user_name;
	TLabel *Label3;
	TEdit *user_password;
	TSpeedButton *login_btn;
	TClientSocket *csocket;
	TStaticText *txt1;
	void __fastcall conn_btnClick(TObject *Sender);
	void __fastcall csocketConnect(TObject *Sender, TCustomWinSocket *Socket);
	void __fastcall csocketDisconnect(TObject *Sender, TCustomWinSocket *Socket);
	void __fastcall login_btnClick(TObject *Sender);
	void __fastcall csocketError(TObject *Sender, TCustomWinSocket *Socket, TErrorEvent ErrorEvent,
          int &ErrorCode);
	void __fastcall csocketRead(TObject *Sender, TCustomWinSocket *Socket);
private:	// User declarations

SESSION_ID   local_ssid,remote_ssid;
modem_proto_buffer  rxm;
void __fastcall handle_recv(LPMPROTO_HEADER mph,int len);
public:		// User declarations
	__fastcall TForm2(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm2 *Form2;
//---------------------------------------------------------------------------
#endif
