//---------------------------------------------------------------------------

#ifndef trend_formsH
#define trend_formsH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "tchart_frame.h"
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ToolWin.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <System.ImageList.hpp>
#include <Vcl.ImgList.hpp>
#include <Vcl.Buttons.hpp>

class TTlmKadrForm;
class TGkAwpModule;

//---------------------------------------------------------------------------
class TTrendForm : public TForm
{
__published:	// IDE-managed Components
	TToolBar *ToolBar1;
	TStatusBar *StatusBar1;
	TTChartFrame *chart_frame;
	TTimer *Timer1;
	TImageList *ImageList1;
	TSpeedButton *tbHideLegend;
	TSpeedButton *tbAxisAutomatic;
	TStaticText *StaticText1;
	TEdit *ShearFreq;
	TToolButton *ToolButton1;
	TToolButton *ToolButton2;
	TToolButton *ToolButton3;
	TStaticText *StaticText2;
	TEdit *TimeLong;
	TToolButton *ToolButton4;
	TToolButton *ToolButton5;
	TStaticText *StaticText3;
	TEdit *Edit1;
	TUpDown *LineWidth;
	TSpeedButton *tbSetStairs;
	void __fastcall TChartFrame1Chart1DragOver(TObject *Sender, TObject *Source, int X,
          int Y, TDragState State, bool &Accept);
	void __fastcall TChartFrame1Chart1DragDrop(TObject *Sender, TObject *Source, int X,
          int Y);
	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall tbHideLegendClick(TObject *Sender);
	void __fastcall tbAxisAutomaticClick(TObject *Sender);
	void __fastcall ToolButton1Click(TObject *Sender);
	void __fastcall LineWidthChangingEx(TObject *Sender, bool &AllowChange, int NewValue,
          TUpDownDirection Direction);
	void __fastcall tbSetStairsClick(TObject *Sender);
private:	// User declarations
        TGkAwpModule     *module;
        int points_limit ;
        void __fastcall set_time_long(int sec);
	__fastcall TTrendForm(TComponent* Owner);
public:		// User declarations
virtual __fastcall TTrendForm(TComponent* Owner,TGkAwpModule     *module);
        void __fastcall add_selected(TTlmKadrForm * kfrm);
};
//---------------------------------------------------------------------------
extern PACKAGE TTrendForm *TrendForm;
//---------------------------------------------------------------------------
#endif
