//---------------------------------------------------------------------------

#ifndef iec60870_line_paramH
#define iec60870_line_paramH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "gkmod_frame.h"
#include "iec60870_modem.h"
//---------------------------------------------------------------------------
class TIEC60870LineParam : public TGKModuleFrame
{
__published:	// IDE-managed Components
	TLabel *Label2;
	TEdit *CpNumber;
	TLabel *Label3;
	TEdit *cp_name;
	TLabel *Label1;
	TEdit *host_addr;
	TLabel *Label4;
	TEdit *host_port;
	TLabel *Label5;
	TComboBox *obj_addr_size;
	TLabel *Label6;
	TEdit *transmit_delay;
	TLabel *Label7;
	TEdit *W_count;
	TLabel *Label8;
	TEdit *K_count;
	TLabel *Label9;
	TEdit *rx_timeout;
	TLabel *Label10;
	TEdit *dev_addr;
	TLabel *Label11;
	TEdit *common_addr;
	void __fastcall CpNumberChange(TObject *Sender);
private:	// User declarations

         IEC60870_LINE_CONFIG line_config;
         bool     __fastcall  get_settings  (LPIEC60870_LINE_CONFIG mcfg,bool from_controls);
         void     __fastcall  setup_controls(const LPIEC60870_LINE_CONFIG mcfg);
virtual  void     __fastcall  current_object_change (TObject * value) ;
virtual  void     __fastcall  current_object_changed(TObject * ,bool &enable_set){ enable_set = !is_create_mode(); }
virtual  bool     __fastcall  apply_changes   ();
virtual  bool     __fastcall  undo_changes    ();
virtual  bool     __fastcall  delete_current  ();
         bool     __fastcall  is_create_mode  ();
         DWORD    __fastcall  get_line_number ();

public:		// User declarations
	__fastcall TIEC60870LineParam(TComponent* Owner);
        __property DWORD line_number = {read = get_line_number};
         void     __fastcall  update_line(TTreeNode * node);
         void     __fastcall  update_line(TTreeNode * node,LPMODEM_LINE_PROPS mlp);
static   void     __fastcall  set_node_image_index(TTreeNode * node,DWORD line_state);

};
//---------------------------------------------------------------------------
extern PACKAGE TIEC60870LineParam *IEC60870LineParam;

//---------------------------------------------------------------------------

#define LINE_IMAGE_CONNECT    0
#define LINE_IMAGE_CONNECTING 1
#define LINE_IMAGE_DISCONNECT 2


#endif
