//---------------------------------------------------------------------------

#ifndef modbus_emul_formH
#define modbus_emul_formH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <ScktComp.hpp>
#include <rx_buffer.hpp>
#include <modbus_proto.h>


//---------------------------------------------------------------------------
class TForm3 : public TForm
{
__published:	// IDE-managed Components
		TEdit *TcpPort;
		TLabel *Label1;
		TLabel *Label2;
		TEdit *DevNum;
	TRadioGroup *setup_data;
		TLabel *Label3;
		TEdit *ParamCount;
		TButton *StartBtn;
		TButton *StopBtn;
	TListView *ParamList;
		TMemo *Memo1;
		TStatusBar *StatusBar1;
	TServerSocket *SrvSocket;
		void __fastcall StartBtnClick(TObject *Sender);
		void __fastcall StopBtnClick(TObject *Sender);
        void __fastcall setup_dataClick(TObject *Sender);
	void __fastcall SrvSocketClientRead(TObject *Sender, TCustomWinSocket *Socket);
	void __fastcall SrvSocketClientDisconnect(TObject *Sender, TCustomWinSocket *Socket);





private:	// User declarations
protected:
        void __fastcall  on_destroy_socket(TObject * sender);
rx_buffer *  __fastcall  get_rx_buffer(TCustomWinSocket * sender);
        void __fastcall  read_discrete(LPMODBUS_TCP_FRAME mtf,TCustomWinSocket * socket);
        void __fastcall  read_input   (LPMODBUS_TCP_FRAME mtf,TCustomWinSocket * socket);
        void  __fastcall handle_rx(TCustomWinSocket * sender,rx_buffer * buf);


public:		// User declarations
		__fastcall TForm3(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm3 *Form3;
//---------------------------------------------------------------------------
#endif
