//---------------------------------------------------------------------------

#ifndef text_editorH
#define text_editorH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <System.ImageList.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ImgList.hpp>
#include <Vcl.ToolWin.hpp>
//---------------------------------------------------------------------------
class TTextEditor : public TForm
{
__published:	// IDE-managed Components
	TStatusBar *StatusBar1;
	TToolBar *ToolBar1;
	TMemo *Memo1;
	TToolButton *ToolButton1;
	TToolButton *ToolButton2;
	TImageList *ImageList1;
	void __fastcall ToolButton1Click(TObject *Sender);
	void __fastcall ToolButton2Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TTextEditor(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TTextEditor *TextEditor;
//---------------------------------------------------------------------------
#endif
