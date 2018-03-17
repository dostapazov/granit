//---------------------------------------------------------------------------

#ifndef tlm_obj_painter_setupH
#define tlm_obj_painter_setupH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "gkmodule_form.h"
#include <System.ImageList.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ImgList.hpp>
#include <Vcl.ToolWin.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.ExtDlgs.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <tlm_painter.h>
//---------------------------------------------------------------------------
class TTlmObjPainterSetup : public TGKModuleForm
{
__published:	// IDE-managed Components
	TToolBar *ToolBar1;
	TStatusBar *StatusBar1;
	TLabel *Label1;
	TComboBox *ComboBox1;
	TToolButton *ToolButton1;
	TToolButton *bAddVariant;
	TToolButton *bUpdVariant;
	TOpenPictureDialog *Opd1;
	TGroupBox *ParamBox;
	TCheckBox *cbObjValue;
	TColorDialog *ColorDlg;
	TLabel *Label5;
	TLabel *Label6;
	TPaintBox *OffColor;
	TPaintBox *OnColor;
	TGroupBox *GroupBox1;
	TImage *Image1;
	TGroupBox *GroupBox2;
	TImage *Image2;
	TImageList *ImageList1;
	TCheckBox *cbAbnormal;
	TRadioGroup *RadioGroup1;
	TPanel *Panel1;
	TLabel *Label2;
	TEdit *eWidth;
	TLabel *Label4;
	TEdit *eCount;
	TCheckBox *cbTuTr;
	TCheckBox *cbWorkOut;
	TLabel *Label3;
	TComboBox *multipos_size;
	TPaintBox *TryBox;
	TEdit *ObjValueText;
	TUpDown *ObjValue;
	TCheckBox *cbBadDiag;
	void __fastcall bSelImageClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall eWidthChange(TObject *Sender);
	void __fastcall OffColorClick(TObject *Sender);
	void __fastcall OffColorPaint(TObject *Sender);
	void __fastcall cbObjValueClick(TObject *Sender);
	void __fastcall cbSwitchedClick(TObject *Sender);
	void __fastcall bAddVariantClick(TObject *Sender);
	void __fastcall ComboBox1Change(TObject *Sender);
	void __fastcall ComboBox1Select(TObject *Sender);
	void __fastcall eCountChange(TObject *Sender);
	void __fastcall bUpdVariantClick(TObject *Sender);
	void __fastcall TryBoxPaint(TObject *Sender);
	void __fastcall multipos_sizeChange(TObject *Sender);
	void __fastcall ObjValueClick(TObject *Sender, TUDBtnType Button);
	void __fastcall cbAbnormalClick(TObject *Sender);
	void __fastcall ObjValueTextChange(TObject *Sender);
private:	// User declarations

protected:
LPARAM              paint_variant;
TLMOBJ_PAINT_PARAM  paint_param;

AnsiString          image_file_name;
AnsiString          mask_file_name;

BOOL     __fastcall get_variant (DWORD idx,mdb_paint_variant & pv );
void     __fastcall read_variants();



void     __fastcall on_module_state(GKHANDLE mod,DWORD state);
void     __fastcall on_module_config_change(GKHANDLE mod,LPMODULE_CONFIG_DATA mcd);
void     __fastcall after_set_gkhandle();
void     __fastcall paint_tlm_object();
int      __fastcall get_element_width();
int      __fastcall get_element_height();
int      __fastcall get_multipos();
void     __fastcall set_multipos(int multipus,bool switched);

void     __fastcall set_image     (UnicodeString fName);
void     __fastcall set_image_mask(UnicodeString fName);



public:		// User declarations
	__fastcall TTlmObjPainterSetup(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TTlmObjPainterSetup *TlmObjPainterSetup;
//---------------------------------------------------------------------------
#endif
