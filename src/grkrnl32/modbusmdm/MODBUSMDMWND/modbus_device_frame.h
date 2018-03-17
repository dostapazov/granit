//---------------------------------------------------------------------------

#ifndef modbus_device_frameH
#define modbus_device_frameH
//---------------------------------------------------------------------------

#include <Classes.hpp>
#include <Controls.hpp>
#include <ComCtrls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "gkmod_frame.h"
#include <ExtCtrls.hpp>
#include "..\modbusmod.h"

//---------------------------------------------------------------------------
class TModbusDevFrame : public TGKModuleFrame
{
__published:	// IDE-managed Components
        TRadioGroup *media_type;
	TGroupBox *GroupBox1;
	TEdit *DevName;
	TLabel *Label2;
	TEdit *DevNumber;
	TLabel *Label1;
	TPanel *pnl1;
	TLabel *Label4;
	TEdit *host_addr;
	TLabel *Label6;
	TEdit *host_port;
	TPanel *pnl0;
	TRadioGroup *ParityControl;
	TRadioGroup *StopBits;
	TRadioGroup *ComPortSpeed;
	TUpDown *ComPortNumber;
	TEdit *Edit3;
	TLabel *Label3;
	TLabel *Label5;
	TEdit *ComPortDelayTCP;
	TEdit *ComPortDelay;
	TLabel *Label7;
        void __fastcall DevNumberChange(TObject *Sender);
        void __fastcall media_typeClick(TObject *Sender);
        void __fastcall host_addrChange(TObject *Sender);
private:	// User declarations

int           in_setup;
DWORD         Fch_mask;
TNotifyEvent  Fon_changed;
TTreeNode   * Ftree_node;

public:		// User declarations
        __fastcall TModbusDevFrame(TComponent* Owner);
MODBUS_POINT &   __fastcall get_point();
         void   __fastcall setup_controls();
         void   __fastcall begin_setup(){++in_setup;}
         void   __fastcall end_setup () {if(--in_setup<0) in_setup = 0;}
         void   __fastcall set_ch_mask(DWORD val);
         bool   __fastcall apply_changes();
         bool   __fastcall undo_changes();
         bool   __fastcall delete_current();
         void   __fastcall read_point();

         void   __fastcall set_point_no(DWORD point_no);
        DWORD   __fastcall get_point_no() {return &get_point() ? get_point().point_no : -1 ;} ;

         void   __fastcall set_tree_node(TTreeNode * node);
         bool   __fastcall is_new_point(){return !Ftree_node || !Ftree_node->Data || modbus_point.point_no == (DWORD)-1 ? true : false;}

        __property TNotifyEvent on_changed = {read = Fon_changed, write = Fon_changed};
        __property DWORD        point_no   = {read = get_point_no , write = set_point_no };
        __property DWORD        ch_mask    = {read = Fch_mask   , write = set_ch_mask};
        __property TTreeNode * tree_node   = {read = Ftree_node , write = set_tree_node};
        __property MODBUS_POINT & modbus_point = {read = get_point};

static int __fastcall ComSpeed2idx(int speed);
static int __fastcall idx2ComSpeed(int speed);
};
//---------------------------------------------------------------------------
extern PACKAGE TModbusDevFrame *ModbusDevFrame;
//---------------------------------------------------------------------------
#endif
