//---------------------------------------------------------------------------

#ifndef modbus_group_frameH
#define modbus_group_frameH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "gkmod_frame.h"
#include <Vcl.ExtCtrls.hpp>
#include "..\modbusmod.h"

//---------------------------------------------------------------------------
class TModbusGroupFrm : public TGKModuleFrame
{
__published:	// IDE-managed Components
	TLabel *Label1;
	TEdit *GroupNum;
	TLabel *Label2;
	TEdit *FirstParam;
	TLabel *Label3;
	TEdit *CountParam;
	TLabel *Label4;
	TEdit *RefreshTime;
	TRadioGroup *rg1;
	TLabel *Label5;
	TEdit *DevNo;
	TLabel *Label6;
	TStaticText *otd_addr_text;
	TGroupBox *GroupBox1;
	TButton *bGetCommEvent;
	TLabel *Label7;
	TLabel *Label8;
	TStaticText *CommState;
	TStaticText *CommEvent;
	TLabel *Label13;
	TEdit *RespondTime;
	TLabel *Label14;
	TStaticText *RealRespondTime;
	TEdit *ModbusCommand;
	TLabel *Label15;
	TStaticText *RealRespondTimeMin;
	TLabel *Label16;
	TStaticText *RealRespondTimeMax;
	TStaticText *LastError;
	TLabel *Label9;
	TLabel *Label10;
	TStaticText *ErrCount;
	void __fastcall GroupNumChange(TObject *Sender);
	void __fastcall bGetCommEventClick(TObject *Sender);
	void __fastcall rg1Click(TObject *Sender);
	void __fastcall ModbusCommandChange(TObject *Sender);
private:	// User declarations
TTreeNode *   Ftree_node;
int           Fin_setup;
TNotifyEvent  Fon_changed;
DWORD         Fch_mask;
public:		// User declarations
	__fastcall TModbusGroupFrm(TComponent* Owner);
        void __fastcall set_tree_node(TTreeNode * node);
        void __fastcall set_ch_mask(DWORD mask);
        bool   __fastcall is_new_group()   {return Ftree_node && Ftree_node->Data && modbus_group->group_no == (WORD)-1 ? true : false;}
 __property TTreeNode *      tree_node   = {read = Ftree_node , write = set_tree_node};
 __property int              in_setup    = {read = Fin_setup};
 __property LPMODBUS_GROUP  modbus_group  = {read = get_modbus_group};
 __property TNotifyEvent on_changed = {read = Fon_changed, write = Fon_changed};
 __property DWORD        ch_mask    = {read = Fch_mask   , write = set_ch_mask};


 bool __fastcall modbus_group_get(LPMODBUS_GROUP grp);
 LPMODBUS_GROUP  __fastcall get_modbus_group();
        void __fastcall begin_setup   ();
        void __fastcall end_setup     ();
        void __fastcall setup_controls();
        bool __fastcall apply_changes ();
        bool __fastcall undo_changes  ();
        bool __fastcall delete_current();
        void __fastcall update_otd_addr();
        void __fastcall monitor_txrx(LPVOID data,DWORD sz,DWORD code);
static const wchar_t * group_names[];
static UnicodeString  __fastcall get_group_name(LPMODBUS_GROUP grp);
static const wchar_t * modbus_errors_text[];
};
//---------------------------------------------------------------------------
extern PACKAGE TModbusGroupFrm *ModbusGroupFrm;
//---------------------------------------------------------------------------

#endif
