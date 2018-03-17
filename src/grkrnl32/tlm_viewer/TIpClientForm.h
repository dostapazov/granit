//---------------------------------------------------------------------------

#ifndef TIpClientFormH
#define TIpClientFormH
//---------------------------------------------------------------------------
#include "tlm_net_client.hpp"
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>

//---------------------------------------------------------------------------
class TIPClient : public TForm
{
__published:	// IDE-managed Components
        TLabel *Label1;
        TLabel *Label2;
        TEdit *ServerPort;
        TTimer *Timer1;
        TButton *Button1;
        TLabel *Label3;
        TPanel *Panel1;
        TLabel *Label4;
        TLabel *Label5;
        TLabel *Label6;
        TLabel *Label7;
        TLabel *connect_state;
        TLabel *RxBytes;
        TLabel *RxKadrs;
        TLabel *TxBytes;
        TLabel *TxKadrs;
        TComboBox *ServerAddr;
        void __fastcall Timer1Timer(TObject *Sender);
        void __fastcall ServerAddrDropDown(TObject *Sender);
private:	// User declarations
       bool   __fastcall  is_connected();
       void   __fastcall  connect(bool act);
       DWORD  __fastcall  send_data(LPMPROTO_HEADER mph);
       DWORD  __fastcall  send_command(bool begin,bool finish,DWORD cmd,DWORD err,LPVOID data,DWORD data_size );

       void   __fastcall  handle_recv();
       void   __fastcall  handle_recv(LPMPROTO_HEADER mph);
       void   __fastcall  handle_get_kadr(bool start, bool end, LPOTDM_PROTO_KADR kadr_data);
       void   __fastcall  handle_get_image(bool start, bool end,LPVOID data);
       void   __fastcall  update_rx_status();

       bool   __fastcall add_kadr(MDB_KADR & kadr,DWORD & ch_flags);
       bool   __fastcall get_kadr(DWORD id,mdb_kadr & kadr,bool by_offs);
       kadr_list::size_type  __fastcall find_kadr(DWORD id);

       tlm_client        * Fclient;


public:		// User declarations
        __fastcall TIPClient(TComponent* Owner);
        __property bool  Connected      = {read = is_connected,write = connect };
        __property tlm_client * client  = {read = Fclient,write = Fclient};



};
//---------------------------------------------------------------------------
extern PACKAGE TIPClient *IPClient;
//---------------------------------------------------------------------------
#endif
