//---------------------------------------------------------------------------

#ifndef kadr_painter_frameH
#define kadr_painter_frameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "kadr_painter.h"
#include <ComCtrls.hpp>
#include <ToolWin.hpp>
#include <ImgList.hpp>
#include <Dialogs.hpp>
#include <Menus.hpp>
#include <ExtCtrls.hpp>
#include <System.ImageList.hpp>
#ifdef __CODEGEARC__
#include <System.ImageList.hpp>
#endif
#include "paint_props.h"


//---------------------------------------------------------------------------
class TKadrPainterFrame : public TForm
{
__published:	// IDE-managed Components
        TStatusBar *StatusBar1;
        TToolBar *ToolBar1;
        TToolButton *ApplyBtn;
        TToolButton *UndoBtn;
        TToolButton *ToolButton3;
        TToolButton *DeleteBtn;
        TImageList *ImageList1;
        TToolButton *SelectBkgndBtn;
        TToolButton *ToolButton6;
        TPopupMenu *PopupMenu1;
        TToolButton *ToolButton1;
        TLabel *BkgndColor;
        TToolButton *ToolButton2;
        TLabel *TextColor;
        TColorDialog *ColorDialog1;
        TPanel *Panel1;
        TComboBox *DiscretePaint;
        TComboBox *AnalogPaint;
        TPanel *Panel2;
        TLabel *Label5;
        TLabel *Label6;
        TEdit *Edit1;
        TUpDown *Precision;
        TEdit *Edit2;
        TUpDown *Digits;
        TMenuItem *miArrangeTop;
        TMenuItem *miArrangeLeft;
        TMenuItem *miSetOneSize;
        TPopupMenu *PopupMenu2;
        TMenuItem *N1;
        TMenuItem *N2;
        TMenuItem *N3;
	TMenuItem *miEnableGrid;
        TMenuItem *x41;
        TMenuItem *x81;
        TMenuItem *miEditInDB;
        TMenuItem *N6;
        TMenuItem *miSetNormalSize;
        TMenuItem *N8;
        TMenuItem *N9;
        TMenuItem *N10;
	TMenuItem *miArrangeGridY;
	TMenuItem *miArrangeGridX;
	TMenuItem *N4;
	TMenuItem *N5;
	TMenuItem *N7;
	TMenuItem *miBkgndNoChange;
	TMenuItem *miBkgndCentered;
	TMenuItem *miBkgndKadrSize;
	TMenuItem *miBkgndKadrSizeProportional;
	TToolButton *DiagOff;
	TToolButton *ToolButton5;
        void __fastcall FormShow(TObject *Sender);
        void __fastcall BkgndColorClick(TObject *Sender);
        void __fastcall TextColorClick(TObject *Sender);
        void __fastcall AnalogPaintChange(TObject *Sender);
        void __fastcall DiscretePaintChange(TObject *Sender);
        void __fastcall PrecisionClick(TObject *Sender, TUDBtnType Button);
        void __fastcall miArrangeTopClick(TObject *Sender);
        void __fastcall miArrangeLeftClick(TObject *Sender);
        void __fastcall miSetOneSizeClick(TObject *Sender);
        void __fastcall N3Click(TObject *Sender);
        void __fastcall miEnableGridClick(TObject *Sender);
        void __fastcall miSetNormalSizeClick(TObject *Sender);
        void __fastcall N8Click(TObject *Sender);
        void __fastcall N9Click(TObject *Sender);
        void __fastcall FormResize(TObject *Sender);
	void __fastcall miArrangeGridYClick(TObject *Sender);
	void __fastcall N4Click(TObject *Sender);
	void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall SelectBkgndBtnClick(TObject *Sender);
	void __fastcall FormActivate(TObject *Sender);
	void __fastcall miBkgndKadrSizeProportionalClick(TObject *Sender);
	void __fastcall DiagOffClick(TObject *Sender);
        void __fastcall WndProc(Messages::TMessage &Message);

protected:

set_kadr_change_method   Fset_kadr_change;
on_select_entry_method   Fon_select_entry;
TTreeNode              * Ftree_node;
TPaintProps            * entry_paint_props;

TKadrPainter   *  Fkadr_painter;
void    __fastcall over_entry_handler      (mdb_kadr_entry * entry);
void    __fastcall select_entry_handler    (mdb_kadr_entry * entry,bool add);
void    __fastcall set_kadr_change_handler (DWORD kadr_id,DWORD mask);
int     __fastcall get_bkgnd_mode();
void    __fastcall set_bkgnd_mode(int);

public:		// User declarations
virtual __fastcall TKadrPainterFrame(TComponent* Owner);
virtual __fastcall TKadrPainterFrame(TComponent* Owner,TTreeNode * node,single_kadr * sk);
        __property TKadrPainter   * kadr_painter = {read = Fkadr_painter,write = Fkadr_painter};
        __property on_select_entry_method   on_select_entry = {read = Fon_select_entry, write = Fon_select_entry };
        __property set_kadr_change_method   set_kadr_change = {read = Fset_kadr_change,write  = Fset_kadr_change };
        __property TTreeNode              * tree_node       = {read = Ftree_node};
        __property int                    bkgnd_mode        = {read = get_bkgnd_mode,write = set_bkgnd_mode};

        int  __fastcall calc_frame_width (int kadr_width){return kadr_width;}
        int  __fastcall calc_frame_height(int kadr_height){return kadr_height+ToolBar1->Height+StatusBar1->Height;}
        void __fastcall set_kadr_size (int w,int h);


void   __fastcall on_kadr_change   (DWORD mask);
void   __fastcall on_record_change (DWORD mask,DWORD rec_id);
void   __fastcall show_entry_info(mdb_kadr_entry * entry);
void   __fastcall setup_entry_param(mdb_kadr_entry * entry);
void   __fastcall update_caption();
void   __fastcall get_paint_variants(bool discrete);
void   __fastcall add_paint_variant(bool discrete,int idx,UnicodeString & name);


};
//---------------------------------------------------------------------------
extern PACKAGE TKadrPainterFrame *KadrPainterFrame;
//---------------------------------------------------------------------------
#endif
