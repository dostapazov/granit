//---------------------------------------------------------------------------

#ifndef modbus_mdm_formH
#define modbus_mdm_formH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ImgList.hpp>
#include <ToolWin.hpp>
#include <ExtCtrls.hpp>
#include "gkmod_frame.h"
#include "mdbus_mod_cfg_frame.h"
#include "modbus_device_frame.h"
#include <System.ImageList.hpp>
#include "modbus_group_frame.h"
#include "modbus_mon_frame.h"

#include "gkmodule_form.h"
#include "..\modbusmod.h"
#include "about_common_frame.h"

//---------------------------------------------------------------------------
typedef void ( __closure __fastcall * Tmon_func)(void * data,DWORD sz,DWORD code);
class TModbusModemForm : public TGKModuleForm
{
__published:	// IDE-managed Components
        TStatusBar *StatusBar1;
        TToolBar *ToolBar1;
        TToolButton *StartBtn;
        TToolButton *StopBtn;
        TTreeView *TreeView1;
        TImageList *ImageList1;
        TSplitter *Splitter1;
        TToolButton *ToolButton1;
        TToolButton *tbNew;
        TToolButton *tbDel;
        TToolButton *ApplyBtn;
        TToolButton *UndoBtn;
        TToolButton *ToolButton6;
	TPageControl *pgc1;
	TTabSheet *TabSheet1;
	TTabSheet *MonSheet;
	TModbusModConfig *ModbusModConfig1;
	TModbusDevFrame *ModbusDevFrame1;
	TModbusGroupFrm *ModbusGroupFrm1;
	TTabSheet *TabSheet2;
	TAboutCommonFrame *AboutCommonFrame1;
	TLabel *Label1;

        void __fastcall StartBtnClick(TObject *Sender);
        void __fastcall StopBtnClick(TObject *Sender);
        void __fastcall Splitter1Moved(TObject *Sender);
        void __fastcall TreeView1Change(TObject *Sender, TTreeNode *Node);
        void __fastcall ApplyBtnClick(TObject *Sender);
        void __fastcall UndoBtnClick(TObject *Sender);
        void __fastcall tbNewClick(TObject *Sender);
        void __fastcall FormHide(TObject *Sender);
        void __fastcall tbDelClick(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
	void __fastcall TreeView1Deletion(TObject *Sender, TTreeNode *Node);
	void __fastcall TreeView1Compare(TObject *Sender, TTreeNode *Node1, TTreeNode *Node2,
          int Data, int &Compare);
	void __fastcall pgc1Change(TObject *Sender);
	void __fastcall pgc1Changing(TObject *Sender, bool &AllowChange);
	void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
private  :	// User declarations
protected:
     Tmon_func  Fmon_func;
     TMonFrame  *MonFrame1;
     TGKModuleFrame * Fcurrent_frame;
     void     __fastcall on_module_state(GKHANDLE mod,DWORD state);
     void     __fastcall on_module_config_change(GKHANDLE mod,LPMODULE_CONFIG_DATA mcd);
     LRESULT  __fastcall on_gkhandle_notify (GKHANDLE from,LPNOTIFY_CODE nc,DWORD mask);

     void     __fastcall after_set_gkhandle();
     void     __fastcall setup_dev_tree();
     void     __fastcall scan_groups(TTreeNode * device);

     void     __fastcall set_current_frame(TGKModuleFrame * frame);
     void     __fastcall on_config_change(TObject * sender);
              __property TGKModuleFrame * current_frame = {read = Fcurrent_frame,write = set_current_frame};

     void     __fastcall free_node(TTreeNode * Node);
public   :// User declarations
            __fastcall TModbusModemForm(TComponent* Owner,HWND _hwnd,GKHANDLE _gkh);
       void __fastcall show_error(AnsiString cmd, DWORD err);
TTreeNode * __fastcall get_nearest_node(TTreeNode * curr);
       void __fastcall set_mon_func(Tmon_func mf);
       __property Tmon_func mon_func = {read = Fmon_func, write = set_mon_func };



};
//---------------------------------------------------------------------------
extern PACKAGE TModbusModemForm *ModbusModemForm;
//---------------------------------------------------------------------------
#endif
