//---------------------------------------------------------------------------

#ifndef grantablet_wndH
#define grantablet_wndH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "gkmodule_form.h"
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <ToolWin.hpp>
#include <ImgList.hpp>
#include "..\grantablet.h"
#include <text_viewer.hpp>
#include <gkio_base.h>
#include <otdm_proto.h>
#include <Buttons.hpp>
#include <Grids.hpp>
#include <granit_proto.h>


typedef KeWin::TTextViewer<char>  txrx_monitor;
//---------------------------------------------------------------------------
class TGranTabledFrm : public TGKModuleForm
{
__published:	// IDE-managed Components
        TStatusBar *StatusBar1;
        TToolBar *ToolBar1;
        TTreeView *OutFloorsTree;
        TSplitter *Splitter1;
        TPageControl *PageControl1;
        TTabSheet *SettigsSheet;
        TTabSheet *TabSheet2;
        TPanel *SettingsPanel;
        TLabel *Label1;
        TEdit *ModemNumber;
        TToolButton *StartBtn;
        TToolButton *StopBtn;
        TImageList *ImageList1;
        TLabel *Label2;
        TEdit *IoModule;
        TLabel *Label3;
        TLabel *Label4;
        TEdit *DeviceSpeed;
        TComboBox *DeviceName;
        TToolButton *ToolButton1;
        TToolButton *CreateBtn;
        TToolButton *ApplyBtn;
        TToolButton *UndoBtn;
        TToolButton *DeleteBtn;
        TCheckBox *IoWatchDog;
        TLabel *Label5;
        TEdit *DbFile;
        TPanel *KadrPanel;
        TLabel *Label6;
        TEdit *OutputPosition;
        TComboBox *KadrDescript;
        TLabel *Label7;
        TRadioGroup *OutputMode;
        TMemo *Memo1;
        TPanel *FloorPanel;
        TLabel *Label8;
        TEdit *FloorNumber;
        TTabSheet *MonSheet;
        TGroupBox *MonitorBox;
        TUpDown *OutputPositionNum;
        TUpDown *FloorNum;
        TSpeedButton *TxRxMonClear;
        TSpeedButton *TxRxMonWork;
        TTabSheet *OutParamsSheet;
        TStringGrid *OutputParams;
        TSpeedButton *SpeedButton3;
        TMemo *Memo2;
        TLabel *Label9;
        TCheckBox *IoNotReset;
        TLabel *Label30;
        void __fastcall OutFloorsTreeChange(TObject *Sender,
          TTreeNode *Node);
        void __fastcall DeviceNameDropDown(TObject *Sender);
        void __fastcall ModemNumberChange(TObject *Sender);
        void __fastcall IoModuleChange(TObject *Sender);
        void __fastcall DeviceNameChange(TObject *Sender);
        void __fastcall DeviceSpeedChange(TObject *Sender);
        void __fastcall IoWatchDogClick(TObject *Sender);
        void __fastcall ApplyBtnClick(TObject *Sender);
        void __fastcall DbFileChange(TObject *Sender);
        void __fastcall StartBtnClick(TObject *Sender);
        void __fastcall StopBtnClick(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall MonSheetResize(TObject *Sender);
        void __fastcall FormDestroy(TObject *Sender);
        void __fastcall CreateBtnClick(TObject *Sender);
        void __fastcall OutFloorsTreeChanging(TObject *Sender,
          TTreeNode *Node, bool &AllowChange);
        void __fastcall DeleteBtnClick(TObject *Sender);
        void __fastcall UndoBtnClick(TObject *Sender);
        void __fastcall OutputPositionChange(TObject *Sender);
        void __fastcall FloorNumberChange(TObject *Sender);
        void __fastcall TxRxMonClearClick(TObject *Sender);
        void __fastcall OutputParamsMouseMove(TObject *Sender,
          TShiftState Shift, int X, int Y);
        void __fastcall PageControl1Changing(TObject *Sender,
          bool &AllowChange);
        void __fastcall IoNotResetClick(TObject *Sender);
private:	// User declarations
txrx_monitor    * txrx_mon;
TTreeNode       * root;
GRTBL_SETTINGS  settings;
DWORD           Fconfig_changes;
BOOL            in_setup;
BOOL            create_mode;
bool         __fastcall get_config_param(LPGRTBL_SETTINGS psett);
bool         __fastcall apply_module_settings();
bool         __fastcall apply_floor_param();
bool         __fastcall apply_kadr_param();
void         __fastcall get_kadr_param(OUTPUT_KADR_PARAM &kp );
void         __fastcall enum_floors();
void         __fastcall enum_output_kadrs(TTreeNode * floor);
TTreeNode *  __fastcall add_output_kadr_node(TTreeNode * floor,LPOUTPUT_KADR_PARAM okp);
TTreeNode *  __fastcall add_floor_node(TTreeNode * root,DWORD num);
void         __fastcall on_txrx_notify(LPMIO_REQUEST io_req,DWORD len,bool tx);
void         __fastcall on_enum_kadr( LPOTDM_PROTO_KADR kadr_data,bool end);

void         __fastcall setup_out_params_ts( lpgr_proto_out_ts data);
void         __fastcall setup_out_params_ti( lpgr_proto_out_ti data);
void         __fastcall setup_out_params_ready(lpgr_proto_out_ready data);
void         __fastcall clear_output_param_grid();


public:		// User declarations
         __fastcall TGranTabledFrm(TComponent* Owner,HWND parent,GKHANDLE _handle);

LRESULT  __fastcall on_gkhandle_notify (GKHANDLE from,LPNOTIFY_CODE nc,DWORD mask);
void     __fastcall on_module_state(GKHANDLE mod,DWORD state) ;
void     __fastcall on_module_config_change(GKHANDLE mod,LPMODULE_CONFIG_DATA mcd);
void     __fastcall after_set_gkhandle();
void     __fastcall set_config_changes(DWORD value);
void     __fastcall setup_settings_panel();
void     __fastcall setup_floor_panel();
void     __fastcall setup_kadr_panel ();
         __property DWORD config_changes = {read = Fconfig_changes,write = set_config_changes};

};
//---------------------------------------------------------------------------
extern PACKAGE TGranTabledFrm *GranTabledFrm;
//---------------------------------------------------------------------------
#endif
