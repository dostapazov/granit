//---------------------------------------------------------------------------

#ifndef gkipmodem_frmH
#define gkipmodem_frmH
//---------------------------------------------------------------------------
#define _WINSOCKAPI_
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "gkmodule_form.h"
#include "..\gkipmodem_mod.h"
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <wsockdef.h>
#include <Buttons.hpp>
#include "about_common_frame.h"


//---------------------------------------------------------------------------
class TGkIPModemFrm : public TGKModuleForm
{
__published:	// IDE-managed Components
        TPageControl *PageControl1;
        TTabSheet *SettingsPage;
        TTabSheet *TabSheet2;
        TLabel *Label1;
        TEdit *PortNumber;
        TGroupBox *AddressBox;
        TCheckBox *AllAddress;
        TListView *AddressList;
        TBitBtn *ApplyBtn;
        TBitBtn *UndoBtn;
        TStatusBar *StatusBar1;
        TLabel *Label2;
        TEdit *ModemNumber;
        TRadioButton *AsClient;
        TRadioButton *AsServer;
        TEdit *ServerAddr;
        TLabel *Label3;
        TBitBtn *StartBtn;
        TBitBtn *StopBtn;
        TListView *LinesList;
        TTabSheet *TabSheet1;
        TLabel *Label4;
	TGroupBox *ClientBox;
	TListView *ClientList;
	TSpeedButton *bChangeLine;
	TSpeedButton *bNewLine;
	TSpeedButton *bDelLine;
	TGroupBox *ClOpts;
	TAboutCommonFrame *AboutCommonFrame1;
	TCheckBox *OptCompatibleMode;
        void __fastcall AllAddressClick(TObject *Sender);
        void __fastcall PortNumberChange(TObject *Sender);
        void __fastcall UndoBtnClick(TObject *Sender);
        void __fastcall ApplyBtnClick(TObject *Sender);
        void __fastcall AsClientClick(TObject *Sender);
        void __fastcall AsServerClick(TObject *Sender);
        void __fastcall ServerAddrChange(TObject *Sender);
        void __fastcall StartBtnClick(TObject *Sender);
        void __fastcall StopBtnClick(TObject *Sender);
        void __fastcall PageControl1Resize(TObject *Sender);
        void __fastcall ModemNumberChange(TObject *Sender);
	void __fastcall ClientListSelectItem(TObject *Sender, TListItem *Item, bool Selected);
	void __fastcall bChangeLineClick(TObject *Sender);
	void __fastcall bDelLineClick(TObject *Sender);
	void __fastcall bNewLineClick(TObject *Sender);
	void __fastcall OptCompatibleModeClick(TObject *Sender);

private:	// User declarations
void       __fastcall after_set_gkhandle();
void       __fastcall on_module_state(GKHANDLE mod,DWORD state);
void       __fastcall on_module_config_change(GKHANDLE mod,LPMODULE_CONFIG_DATA mcd);

GKIPM_SETTINGS   settings;
DWORD            Fch_mask;
int              in_setup;
void __fastcall  set_ch_mask(DWORD chm);
void __fastcall  setup_addr_list();
bool __fastcall  get_module_settings();
void __fastcall  setup_settings_controls();
void __fastcall  setup_lines_list();
bool __fastcall  IsBindingAddr(DWORD addr);
void __fastcall  get_settings_from_control(GKIPM_SETTINGS & ipms);
void __fastcall  on_modem_number(LPMODEM_CHANGE_NUMBER mcn);
void __fastcall  on_line_state  (LPMODEM_LINE_STATE mls);
void __fastcall  begin_command(char * cmd);
void __fastcall  end_command  (DWORD ret_code);
LRESULT       __fastcall on_gkhandle_notify (GKHANDLE from,LPNOTIFY_CODE nc,DWORD mask);
bool          __fastcall  get_line_addr(DWORD num,GKIPM_LINE_ADDRS & la);
TListItem *   __fastcall  add_line(DWORD num,GKIPM_LINE_ADDRS & la);
TListItem *   __fastcall  find_line       (TListView * lst,DWORD num);
void          __fastcall  setup_client_opts(DWORD opts);
DWORD         __fastcall  get_client_opts  ();
void          __fastcall  setup_form_sizes(){}


public:		// User declarations
virtual        __fastcall TGkIPModemFrm(TComponent* Owner):TGKModuleForm(Owner){};
virtual        __fastcall TGkIPModemFrm(TComponent* Owner,HWND owner_hwnd,GKHANDLE handle = 0);
protected:
        __property DWORD ch_mask  = {read = Fch_mask, write = set_ch_mask};

};
//---------------------------------------------------------------------------
extern PACKAGE TGkIPModemFrm *GkIPModemFrm;
//---------------------------------------------------------------------------
#endif
