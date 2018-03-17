//---------------------------------------------------------------------------

#ifndef ipmodem_wrkH
#define ipmodem_wrkH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <IdBaseComponent.hpp>
#include <IdComponent.hpp>
#include <IdTCPClient.hpp>
#include <IdTCPConnection.hpp>
#include <text_viewer.hpp>
#include <modem_proto.h>
#include <ExtCtrls.hpp>
#include <System.Win.ScktComp.hpp>

#include <otd.h>
#include <otdm_proto.h>


typedef KeWin::TTextViewer<char> TRecvMonitor;

//---------------------------------------------------------------------------
class TIpModemSampleFrm : public TForm
{
__published:	// IDE-managed Components
        TStatusBar *StatusBar1;
        TGroupBox *MonitorBox;
        TGroupBox *GroupBox1;
        TBitBtn *StartBtn;
        TBitBtn *StopBtn;
        TTimer *Timer1;
        TClientSocket *ClientSocket1;
        TPageControl *PageControl1;
        TTabSheet *TabSheet1;
        TTabSheet *TabSheet2;
        TCheckBox *CheckBox8;
        TCheckBox *CheckBox1;
        TCheckBox *CheckBox2;
        TCheckBox *CheckBox3;
        TCheckBox *CheckBox4;
        TCheckBox *CheckBox5;
        TCheckBox *CheckBox9;
        TButton *Button1;
        TButton *Button2;
        TButton *Button4;
        TEdit *AddrMask;
        TCheckBox *CheckBox7;
        TEdit *CallPeriod;
        TCheckBox *CheckBox10;
        TCheckBox *CheckBox6;
        TButton *Button3;
        TButton *Button5;
        TButton *Button6;
        TEdit *KadrID;
        TLabel *Label3;
        TButton *Button7;
        TButton *Button8;
        TButton *Button9;
        TButton *Button10;
	TGroupBox *grp1;
	TEdit *ServerName;
	TLabel *Label2;
	TEdit *ServerPort;
	TTabSheet *TabSheet3;
	TButton *Button11;
	TButton *Button12;
	TButton *Button13;
	TButton *Button14;
	TListView *ArchList;
        void __fastcall CheckBox8Click(TObject *Sender);
        void __fastcall StopBtnClick(TObject *Sender);
        void __fastcall StartBtnClick(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall FormResize(TObject *Sender);
        void __fastcall Button3Click(TObject *Sender);
        void __fastcall Button1Click(TObject *Sender);
        void __fastcall CheckBox7Click(TObject *Sender);
        void __fastcall CallPeriodChange(TObject *Sender);
        void __fastcall Timer1Timer(TObject *Sender);
        void __fastcall ClientSocket1Read(TObject *Sender,
          TCustomWinSocket *Socket);
        void __fastcall ClientSocket1Connect(TObject *Sender,
          TCustomWinSocket *Socket);
        void __fastcall ClientSocket1Disconnect(TObject *Sender,
          TCustomWinSocket *Socket);
        void __fastcall Timer2Timer(TObject *Sender);
        void __fastcall ClientSocket1Error(TObject *Sender,
          TCustomWinSocket *Socket, TErrorEvent ErrorEvent,
          int &ErrorCode);
        void __fastcall PageControl1Change(TObject *Sender);
        void __fastcall Button5Click(TObject *Sender);
        void __fastcall Button6Click(TObject *Sender);
        void __fastcall Button7Click(TObject *Sender);
        void __fastcall Button8Click(TObject *Sender);
        void __fastcall Button9Click(TObject *Sender);
private:	// User declarations
DWORD current_proto_id;
DWORD  call_parts;
TRecvMonitor     recv_mon;
modem_proto_buffer rxbuff;
void __fastcall handle_recv();
DWORD kadrs_count;
DWORD bytes_count;
DWORD pkt_num;
int   no_tx_timer;
int   no_rx_timer;
void  __fastcall do_send(LPMPROTO_HEADER mph);
void  __fastcall create_recvmon();
void __fastcall update_status_line();
void __fastcall send_synchro();
/*
virtual int  __fastcall GetDesignDpi(void) ;
virtual int  __fastcall GetParentCurrentDpi(void);
virtual void __fastcall ScaleForPPI(int NewPPI);
virtual void __fastcall ScaleControlsForDpi(int NewPPI);
virtual void __fastcall ScaleForCurrentDpi();
*/
public:		// User declarations
		__fastcall TIpModemSampleFrm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TIpModemSampleFrm *IpModemSampleFrm;
//---------------------------------------------------------------------------
/*
inline int  __fastcall TIpModemSampleFrm::GetDesignDpi(void)
{
 return this->PixelsPerInch;
}

inline int  __fastcall TIpModemSampleFrm::GetParentCurrentDpi(void)
{
 TForm * cf = dynamic_cast<TForm*>(this->Parent);
 return cf ? cf->PixelsPerInch : GetDesignDpi();
}

inline void __fastcall TIpModemSampleFrm::ScaleForPPI(int NewPPI)
{

}
inline void __fastcall TIpModemSampleFrm::ScaleControlsForDpi(int NewPPI)
{

}

inline void __fastcall TIpModemSampleFrm::ScaleForCurrentDpi()
{

}
*/
#endif
