//---------------------------------------------------------------------------

#ifndef gkopc_client_line_settingsH
#define gkopc_client_line_settingsH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "gkmod_frame.h"
#include <opc_helper.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.ComCtrls.hpp>
#include "opc_config_editor.h"
#include <gkopc_client.h>
#include <Vcl.Dialogs.hpp>
#include <Vcl.Menus.hpp>

//---------------------------------------------------------------------------
class TOpcLineSetingsFrame : public TGKModuleFrame
{
__published:	// IDE-managed Components
	TPanel *Panel1;
	TEdit *cp_number;
	TSpeedButton *bSelConfigFile;
	TEdit *config_file;
	TLabel *Label5;
	TLabel *Label4;
	TGroupBox *GroupBox1;
	TLabel *Label2;
	TLabel *Label3;
	TComboBox *server_type_list;
	TComboBox *server_prog_id;
	TEdit *cp_name;
	TLabel *Label1;
	TLabel *Label6;
	TEdit *rate;
	TLabel *Label7;
	TEdit *bandwidth;
	TOpenDialog *OpenDialog1;
	TEdit *server_host;
	TLabel *Label8;
	TCheckBox *cbLineOptReplaceTime;
	TButton *Button1;
	TPageControl *PageControl1;
	TTabSheet *TabSheet1;
	TTabSheet *TabSheet2;
	TMemo *TuLuaScript;
	TMemo *TrLuaScript;
	TTabSheet *TabSheet3;
	TMemo *Memo1;
	TSpeedButton *bRestartLine;
	void __fastcall server_prog_idDropDown(TObject *Sender);
	void __fastcall server_type_listChange(TObject *Sender);
	void __fastcall server_prog_idChange(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);
        void __fastcall config_editor_destroy(TObject *Sender);
	void __fastcall cp_numberChange(TObject *Sender);
	void __fastcall bSelConfigFileClick(TObject *Sender);
	void __fastcall bRestartLineClick(TObject *Sender);

private:	// User declarations
        GKOPC_LINE_CONFIG line_config;
        opc::opc_server_type_t Fserver_type;
        opc::TOpcServerList::server_entryes_t servers;
        TOpcConfigEditor      *config_editor;

virtual  bool     __fastcall  apply_changes         ();
virtual  bool     __fastcall  undo_changes          ();
virtual  bool     __fastcall  delete_current        ();
virtual  void     __fastcall  current_object_changed(TObject * ,bool &enable_set);
virtual  void     __fastcall  current_object_change (TObject * value);
         bool     __fastcall  get_settings   (LPGKOPC_LINE_CONFIG line_conf,bool from_controls);
         void     __fastcall  clear_controls();
         void     __fastcall  setup_controls(LPGKOPC_LINE_CONFIG line_config);
         void     __fastcall  setup_scripts ();
         bool     __fastcall  apply_scripts ();

public:		// User declarations
	__fastcall TOpcLineSetingsFrame(TComponent* Owner);

        static UnicodeString __fastcall get_line_text(TGKModuleInterface & mod_iface,DWORD line_num);

};
//---------------------------------------------------------------------------
extern PACKAGE TOpcLineSetingsFrame *OpcLineSetingsFrame;
//---------------------------------------------------------------------------
#endif
