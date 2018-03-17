//---------------------------------------------------------------------------

#ifndef test_mdm_frmH
#define test_mdm_frmH
//---------------------------------------------------------------------------
#include <text_viewer.hpp>
#include <vcl.h>
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <modem_proto.h>
#include <otd.h>
#include <ExtCtrls.hpp>

#define MSG_RECV  (WM_APP+100)

typedef KeWin::TTextViewer<char> TRecvMonitor;
//---------------------------------------------------------------------------
class TAppMdm : public TForm
{
__published:	// IDE-managed Components
        TGroupBox *GroupBox1;
        TGroupBox *MonitorBox;
        TStatusBar *StatusBar1;
        TCheckBox *CheckBox1;
        TCheckBox *CheckBox2;
        TCheckBox *CheckBox3;
        TCheckBox *CheckBox4;
        TCheckBox *CheckBox5;
        TButton *Button1;
        TButton *Button2;
        TEdit *AddrMask;
        TCheckBox *CheckBox7;
        TTimer *Timer1;
        TEdit *CallPeriod;
        TCheckBox *CheckBox6;
        TButton *Button3;
        TCheckBox *CheckBox8;
        TButton *Button4;
        TCheckBox *CheckBox9;
        void __fastcall FormResize(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall CheckBox1Click(TObject *Sender);
        void __fastcall Button1Click(TObject *Sender);
        void __fastcall FormDestroy(TObject *Sender);
        void __fastcall CheckBox7Click(TObject *Sender);
        void __fastcall Timer1Timer(TObject *Sender);
        void __fastcall CallPeriodChange(TObject *Sender);
        void __fastcall Button3Click(TObject *Sender);
private:	// User declarations
HANDLE             std_input;
HANDLE             std_output;
HANDLE             terminate_event;
HANDLE             recv_thread;
DWORD              recv_kadrs;
DWORD              send_kadrs;
DWORD              send_bytes;
DWORD              recv_bytes;
TRecvMonitor       recv_mon;
modem_proto_buffer rxbuff;
void  __fastcall    send(LPBYTE buff,DWORD len);
void  __fastcall    recv();
static  DWORD WINAPI recv_thread_func(TAppMdm * frm);

DWORD call_parts;
        DWORD Flast_error;

bool __fastcall check_for_terminate();

void  __fastcall create_recvmon();
        void __fastcall Set_last_error(DWORD value);
public:		// User declarations
        __fastcall TAppMdm(TComponent* Owner);
void    __fastcall update_recv_stat();        
void    __fastcall update_send_stat();        
protected:
        BEGIN_MESSAGE_MAP
                VCL_MESSAGE_HANDLER(MSG_RECV, TMessage, OnMsgRecv)
        END_MESSAGE_MAP(TForm)
        void __fastcall OnMsgRecv(TMessage & msg);
        __property DWORD last_error  = { read=Flast_error, write=Set_last_error };
};
//---------------------------------------------------------------------------
extern PACKAGE TAppMdm *AppMdm;
//---------------------------------------------------------------------------
#endif
