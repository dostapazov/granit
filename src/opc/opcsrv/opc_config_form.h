//---------------------------------------------------------------------------

#ifndef opc_config_formH
#define opc_config_formH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Buttons.hpp>
#include <System.Win.ScktComp.hpp>

#include <lightopc.hpp>
#include "opc_srv_config.h"

#ifndef CONFIG_DLL
 #define CONFIG_DLL_EXPORT __declspec(dllimport)
#else
 #define CONFIG_DLL_EXPORT __declspec(dllexport)
#endif

extern "C" CONFIG_DLL_EXPORT BOOL WINAPI show_config(GUID * guid);

//---------------------------------------------------------------------------
class TConfigForm : public TForm
{
__published:	// IDE-managed Components
	TPageControl *pgc1;
	TTabSheet *ts1;
	TTabSheet *ts2;
	TGroupBox *srv_param;
	TLabel *lbl1;
	TLabel *lbl2;
	TEdit *otd_srv_addr;
	TEdit *otd_srv_port;
	TStatusBar *StatusBar1;
	TListView *AvailList;
	TClientSocket *ClSock;
	TSpeedButton *SpeedButton1;
	TEdit *GuidText;
	TLabel *Label1;
	TBitBtn *bGUIDGen;
	TLabel *Label2;
	TEdit *ProgID;
	TLabel *Label3;
	TEdit *OpcSrvName;
	TBitBtn *bRegister;
	TBitBtn *bUnregister;
	TBitBtn *bWriteConfig;
	TStaticText *StaticText1;
	TEdit *RefreshRate;
	TStaticText *StaticText2;
	TEdit *MinRefreshRate;
	TStaticText *StaticText3;
	TEdit *MaxQueueCount;
	TStaticText *StaticText4;
	TEdit *BranchSep;
	TStaticText *StaticText5;
	TEdit *TagsCount;
	TStaticText *StaticText6;
	TEdit *NoClientsWorkTime;
	TRadioGroup *TagNameMode;
	TCheckBox *UseConsole;
	void __fastcall SpeedButton1Click(TObject *Sender);
	void __fastcall ClSockConnect(TObject *Sender, TCustomWinSocket *Socket);
	void __fastcall ClSockDisconnect(TObject *Sender, TCustomWinSocket *Socket);
	void __fastcall ClSockRead(TObject *Sender, TCustomWinSocket *Socket);
	void __fastcall bGUIDGenClick(TObject *Sender);
	void __fastcall bRegisterClick(TObject *Sender);
	void __fastcall bUnregisterClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall bWriteConfigClick(TObject *Sender);
	void __fastcall ClSockError(TObject *Sender, TCustomWinSocket *Socket, TErrorEvent ErrorEvent,
          int &ErrorCode);




private:	// User declarations
void __fastcall send_request_enum_kadr(TCustomWinSocket * Socket);
void __fastcall handle_receive        (TCustomWinSocket * Socket);
void __fastcall handle_enum_kadrs     (void * data,DWORD data_size);
bool __fastcall is_srv_registered     ();
void __fastcall setup_controls        ();
          TOpcSrvConfig   config_data;
      unsigned long pkt_num;
public:		// User declarations
	__fastcall TConfigForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TConfigForm *ConfigForm;
//---------------------------------------------------------------------------
#endif
