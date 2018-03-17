//---------------------------------------------------------------------------

#ifndef test_formH
#define test_formH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "tlm_painter_form.h"
#include <System.ImageList.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.ExtDlgs.hpp>
#include <Vcl.ImgList.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.ToolWin.hpp>
//---------------------------------------------------------------------------
class TTlmTestFrom : public TTlmPainterForm
{
__published:	// IDE-managed Components
	TOpenPictureDialog *OpenPictureDialog1;
	TPopupMenu *PopupMenu1;
	TMenuItem *N1;
	TMenuItem *N3;
	TMenuItem *N4x41;
	TMenuItem *N8x81;
	TMenuItem *N16x161;
	TMenuItem *N2;
	TMenuItem *N4;
	TMenuItem *N5;
	TMenuItem *N6;
	TMenuItem *N7;
	TMenuItem *N8;
	TTimer *Timer1;
	TToolButton *ToolButton1;
	TToolButton *ToolButton2;
	TToolButton *ToolButton3;
	TToolButton *ToolButton4;
	void __fastcall N7Click(TObject *Sender);
	void __fastcall ToolButton1Click(TObject *Sender);
	void __fastcall ToolButton4Click(TObject *Sender);
	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall ToolButton3Click(TObject *Sender);
	void __fastcall N8Click(TObject *Sender);
	void __fastcall N16x161Click(TObject *Sender);
private:	// User declarations
        UnicodeString name_str;
  void  init();
  void __fastcall  set_current_over(DWORD over);
public:		// User declarations
	__fastcall TTlmTestFrom(TComponent* Owner);
  virtual const wchar_t * __fastcall get_name     (MDB_RECORD & rec,bool short_name) const;
  virtual       bool      __fastcall get_record(DWORD rec_id,MDB_RECORD & rec,bool * rec_visible) const;

};
//---------------------------------------------------------------------------
extern PACKAGE TTlmTestFrom *TlmTestFrom;
//---------------------------------------------------------------------------
#endif
