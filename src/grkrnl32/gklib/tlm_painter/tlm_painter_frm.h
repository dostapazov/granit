//---------------------------------------------------------------------------

#ifndef tlm_painter_frmH
#define tlm_painter_frmH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "tlmpainter.hpp"
#include <Menus.hpp>
#include <ComCtrls.hpp>
#include <ToolWin.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
#include <ImgList.hpp>
#include <System.ImageList.hpp>
//#include <ImageList.hpp>

//---------------------------------------------------------------------------
class TForm2 : public TForm
{
__published:	// IDE-managed Components
        TMainMenu *MainMenu1;
        TMenuItem *N1;
        TMenuItem *N2;
        TStatusBar *StatusBar1;
        TPanel *Panel1;
        TCoolBar *CoolBar1;
        TToolBar *ToolBar1;
        TToolButton *ToolButton1;
        TSpeedButton *SpeedButton1;
        TSpeedButton *SpeedButton2;
        TSpeedButton *SpeedButton3;
        TImageList *ImageList1;
        TToolButton *ToolButton2;
        TToolButton *ToolButton3;
        TToolButton *ToolButton4;
        TPopupMenu *PopupMenu1;
        TMenuItem *N21;
        TMenuItem *N41;
        TMenuItem *N61;
        TMenuItem *N81;
        TMenuItem *N3;
        TToolButton *ToolButton5;
        TToolButton *ToolButton6;
        TPopupMenu *PopupMenu2;
        TMenuItem *N4;
        TMenuItem *N5;
        TMenuItem *N6;
        TMenuItem *N7;
        TMenuItem *N8;
        TMenuItem *N9;
        TMenuItem *N10;
        TMenuItem *N11;
        void __fastcall FormShow(TObject *Sender);
        void __fastcall N2Click(TObject *Sender);
        void __fastcall FormDestroy(TObject *Sender);
        void __fastcall Panel1Resize(TObject *Sender);
        void __fastcall SpeedButton1Click(TObject *Sender);
        void __fastcall SpeedButton2Click(TObject *Sender);
        void __fastcall SpeedButton3Click(TObject *Sender);
        void __fastcall ToolButton1Click(TObject *Sender);
        void __fastcall EnableGridClick(TObject *Sender);
        void __fastcall ToolButton5Click(TObject *Sender);
        void __fastcall N11Click(TObject *Sender);
private:	// User declarations
TTlmPainter     painter;
public:		// User declarations
        __fastcall TForm2(TComponent* Owner);
static string  __stdcall get_text       (LPVOID data,TTlmPainter*,DWORD);
static wstring __stdcall get_wtext      (LPVOID data,TTlmPainter*,DWORD);
static DWORD  __stdcall  get_record      (LPVOID data,TTlmPainter*,mdb_record_ex * );
static DWORD  __stdcall  on_select_entry (LPVOID data,TTlmPainter*,DWORD offs);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm2 *Form2;
//---------------------------------------------------------------------------
#endif
