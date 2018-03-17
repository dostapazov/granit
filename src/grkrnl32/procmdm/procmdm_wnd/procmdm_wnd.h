//---------------------------------------------------------------------------

#ifndef procmdm_wndH
#define procmdm_wndH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "gkmodule_form.h"
#include "..\processmdm.h"
#include <ComCtrls.hpp>
#include <ImgList.hpp>
#include <ToolWin.hpp>
#include <Buttons.hpp>
#include "FldrBrws.h"
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
class TProcessMdmWnd : public TGKModuleForm
{
__published:	// IDE-managed Components
        TPageControl *PageControl1;
        TStatusBar *StatusBar1;
        TTabSheet *TabSheet1;
        TTabSheet *TabSheet2;
        TLabel *Label1;
        TLabel *Label2;
        TLabel *Label3;
        TEdit *ExeName;
        TEdit *CmdLine;
        TEdit *WorkDir;
        TToolBar *ToolBar1;
        TToolButton *StartBtn;
        TToolButton *StopBtn;
        TToolButton *ToolButton3;
        TToolButton *ApplyBtn;
        TToolButton *UndoBtn;
        TImageList *ImageList1;
        TLabel *Label4;
        TEdit *ModemNumberEdit;
        TUpDown *ModemNumber;
        TSpeedButton *SpeedButton1;
        TSpeedButton *SpeedButton2;
        TLabel *Label5;
        TTabSheet *TabSheet3;
        TOpenDialog *OpenDialog1;
        TFolderBrowser *FolderBrowser1;
        TCheckBox *AutoRestart;
        TCheckBox *CmdShow;
        TLabel *Label30;
        void __fastcall SpeedButton2Click(TObject *Sender);
        void __fastcall SpeedButton1Click(TObject *Sender);
        void __fastcall ExeNameChange(TObject *Sender);
        void __fastcall WorkDirChange(TObject *Sender);
        void __fastcall CmdLineChange(TObject *Sender);
        void __fastcall UndoBtnClick(TObject *Sender);
        void __fastcall ApplyBtnClick(TObject *Sender);
        void __fastcall ModemNumberChangingEx(TObject *Sender,
          bool &AllowChange, short NewValue, TUpDownDirection Direction);
        void __fastcall StopBtnClick(TObject *Sender);
        void __fastcall StartBtnClick(TObject *Sender);
        void __fastcall AutoRestartClick(TObject *Sender);
        void __fastcall CmdShowClick(TObject *Sender);
private:	// User declarations
        PMSETTINGS   settings;
        DWORD Fchmask;
void     __fastcall on_module_state(GKHANDLE mod,DWORD state) ;
void     __fastcall on_module_config_change(GKHANDLE mod,LPMODULE_CONFIG_DATA mcd);
void     __fastcall after_set_gkhandle();
bool     __fastcall get_settings();
void     __fastcall setup_config_controls();
void     __fastcall Setchmask(DWORD value);
void     __fastcall handle_call_result(LRESULT ret);

public:		// User declarations
        __fastcall TProcessMdmWnd(TComponent* Owner,HWND parent,GKHANDLE mod);
protected:
        __property DWORD chmask  = { read=Fchmask, write=Setchmask };
};
//---------------------------------------------------------------------------
extern PACKAGE TProcessMdmWnd *ProcessMdmWnd;
//---------------------------------------------------------------------------
#endif
