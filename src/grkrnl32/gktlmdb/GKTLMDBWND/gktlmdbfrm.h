//---------------------------------------------------------------------------

#ifndef gktlmdbfrmH
#define gktlmdbfrmH

//---------------------------------------------------------------------------

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "gkmodule_form.h"
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <gktlmdb.h>
#include "gktlmdbwnd.rh"
#include <ToolWin.hpp>
#include <ImgList.hpp>
#include <Menus.hpp>
#include "dblowfrm.h"
#include <Buttons.hpp>


#define MSG_TLM_CHANGES (WM_USER+512)

//---------------------------------------------------------------------------
class TGKTlmdbForm : public TGKModuleForm
{
__published:	// IDE-managed Components
        TStatusBar *StatusBar1;
        TMainMenu *MainMenu1;
        TMenuItem *N1;
        TMenuItem *MiDbLow;
        TMenuItem *MiScale;
        TMenuItem *MiDbObjects;
        TMenuItem *N5;
        TMenuItem *N6;
        TMenuItem *MiAbout;
        TToolBar *ToolBar1;
        TSpeedButton *SpeedButton1;
        TImageList *ImageList1;
        TToolButton *StartBtn;
        TToolButton *StopBtn;
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall MiDbLowClick(TObject *Sender);
        void __fastcall N6Click(TObject *Sender);
        void __fastcall StartBtnClick(TObject *Sender);
        void __fastcall StopBtnClick(TObject *Sender);
private:
protected:
void                  __fastcall after_set_gkhandle();
LRESULT               __fastcall on_gkhandle_notify(GKHANDLE from,LPNOTIFY_CODE nc,DWORD mask);
void                  __fastcall beginDBcommand(DWORD cmd,DWORD addr);
void                  __fastcall endDBcommand  (DWORD Error);

LRESULT               __fastcall gkhandle_processing(DWORD cmd,LPARAM p1,LPARAM p2);
void                  __fastcall on_module_state(GKHANDLE mod,DWORD state) ;
void                  __fastcall on_module_config_change(GKHANDLE mod,LPMODULE_CONFIG_DATA mcd);

public:		// User declarations
        __fastcall TGKTlmdbForm(TComponent* Owner,HWND own_wnd,GKHANDLE h);

};
//---------------------------------------------------------------------------
extern PACKAGE TGKTlmdbForm *GKTlmdbForm;
//---------------------------------------------------------------------------



#endif
