//---------------------------------------------------------------------------

#ifndef GKScaleFrmH
#define GKScaleFrmH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "gkmodule_form.h"
#include <ComCtrls.hpp>
#include <ActnList.hpp>
#include <Menus.hpp>
#include <StdActns.hpp>
#include <TIndexedFile.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
#include <Dialogs.hpp>
#include <ToolWin.hpp>
#include <Grids.hpp>
#include <ImgList.hpp>
#include <scale.h>
#include <System.ImageList.hpp>

//---------------------------------------------------------------------------
class TGKScaleFrm : public TGKModuleForm
{
__published:	// IDE-managed Components
        TToolBar *ToolBar1;
        TTreeView *Tree;
        TSplitter *Splitter1;
        TPageControl *Pages;
        TTabSheet *TabEdit;
        TTabSheet *TabSetup;
        TOpenDialog *OpenDialog;
        TLabel *Label1;
        TEdit *EdFileName;
        TSpeedButton *SBFileName;
        TCheckBox *CBIndex;
        TLabel *Label2;
        TStaticText *STSize;
        TLabel *Label3;
        TStaticText *STUnused;
        TStaticText *STScaleFile;
        TLabel *Label4;
        TButton *BtLoad;
        TButton *BtSaveAs;
        TButton *BtSet;
        TTabSheet *TabGraph;
        TStringGrid *Grid;
        TRadioGroup *RGInput;
        TLabeledEdit *EdMaxValue;
        TLabeledEdit *EdMinValue;
        TPanel *Panel1;
        TPaintBox *PaintBox;
        TRadioGroup *RGPercent;
        TSplitter *Splitter2;
        TGroupBox *GroupBox2;
        TLabel *Label7;
        TLabel *Label8;
        TGroupBox *GroupBox3;
        TLabel *Label9;
        TLabel *Label10;
        TListView *ListView;
        TGroupBox *GroupBox4;
        TLabel *Label11;
        TLabel *Label12;
        TLabel *Label13;
        TLabel *Label14;
        TEdit *MinWarn;
        TEdit *MaxWarn;
        TEdit *MinAlarm;
        TEdit *MaxAlarm;
        TStaticText *max_kvant;
        TStaticText *min_kvant;
        TStaticText *min_scale;
        TLabel *Label15;
        TStaticText *STCount;
        TButton *BtShrink;
        TImageList *ImageList1;
        TToolButton *TBNew;
        TToolButton *TBDelete;
        TToolButton *ToolButton3;
        TToolButton *TBApplay;
        TToolButton *TBCancel;
        TToolButton *TBSep;
        TStaticText *max_scale;
        TLabel *Label16;
        TStaticText *STid;
        TToolButton *TBAdd;
        TToolButton *TBDel;
        TToolButton *TBUndo;
        TToolButton *TBSave;
        TToolButton *TBExp;
        TLabel *Label6;
        TEdit *s_name;
        void __fastcall Splitter2Moved(TObject *Sender);
        void __fastcall Splitter2CanResize(TObject *Sender, int &NewSize,
          bool &Accept);
        void __fastcall TabGraphResize(TObject *Sender);
        void __fastcall RGInputClick(TObject *Sender);
        void __fastcall SBFileNameClick(TObject *Sender);
        void __fastcall BtLoadClick(TObject *Sender);
        void __fastcall EdFileNameExit(TObject *Sender);
        void __fastcall BtSaveAsClick(TObject *Sender);
        void __fastcall BtSetClick(TObject *Sender);
        void __fastcall BtShrinkClick(TObject *Sender);
        void __fastcall TBNewClick(TObject *Sender);
        void __fastcall TBDeleteClick(TObject *Sender);
        void __fastcall TBApplayClick(TObject *Sender);
        void __fastcall TBCancelClick(TObject *Sender);
        void __fastcall TreeChange(TObject *Sender, TTreeNode *Node);
        void __fastcall MinWarnExit(TObject *Sender);
        void __fastcall MaxWarnExit(TObject *Sender);
        void __fastcall MinAlarmExit(TObject *Sender);
        void __fastcall MaxAlarmExit(TObject *Sender);
        void __fastcall s_nameChange(TObject *Sender);
        void __fastcall PagesChange(TObject *Sender);
        void __fastcall ListViewDblClick(TObject *Sender);
        void __fastcall RGPercentClick(TObject *Sender);
        void __fastcall GridSelectCell(TObject *Sender, int ACol, int ARow,
          bool &CanSelect);
        void __fastcall Splitter1Moved(TObject *Sender);
        void __fastcall TBAddClick(TObject *Sender);
        void __fastcall TBDelClick(TObject *Sender);
        void __fastcall TBSaveClick(TObject *Sender);
        void __fastcall TBUndoClick(TObject *Sender);
        void __fastcall TBExpClick(TObject *Sender);
        void __fastcall PagesChanging(TObject *Sender, bool &AllowChange);
        void __fastcall PaintBoxPaint(TObject *Sender);
        void __fastcall EdFileNameChange(TObject *Sender);
        void __fastcall TreeChanging(TObject *Sender, TTreeNode *Node,
          bool &AllowChange);
        void __fastcall GridClick(TObject *Sender);
        void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
        void __fastcall EdMinValueChange(TObject *Sender);
        void __fastcall EdMaxValueChange(TObject *Sender);
        void __fastcall PaintBoxMouseMove(TObject *Sender,
          TShiftState Shift, int X, int Y);
        void __fastcall PaintBoxMouseUp(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall MinWarnChange(TObject *Sender);
        void __fastcall MaxWarnChange(TObject *Sender);
        void __fastcall MinAlarmChange(TObject *Sender);
        void __fastcall MaxAlarmChange(TObject *Sender);
	void __fastcall GridSetEditText(TObject *Sender, int ACol, int ARow, const UnicodeString Value);

protected:
        void __fastcall on_module_state(GKHANDLE mod,DWORD state){};
        void __fastcall on_module_config_change(GKHANDLE mod,LPMODULE_CONFIG_DATA mcd) {};
        LRESULT __fastcall on_gkhandle_notify (GKHANDLE from,LPNOTIFY_CODE nc,DWORD mask);
private:	// User declarations
        TIndexedFile *extfile;                  //внешний файл для редактирования
        DWORD mask;                             //маска изменений
        TScale scl;
        TSegment tseg;
        DWORD incalc;
        void __fastcall FillTree();
        void __fastcall FillSizes();
        bool __fastcall NameExists(AnsiString name);
        void __fastcall ClearFields();
        void __fastcall FillFields(DWORD id, bool old=true);
        void __fastcall SetMask(DWORD m, bool set=true);
        void __fastcall FillGraph();
        void __fastcall ReFillSegments();
        void __fastcall SelectGridRow(AnsiString str);
        AnsiString __fastcall ConstructName(AnsiString ed, DWORD id);
        void __fastcall SetSegmentMask(bool set=true);
        void __fastcall on_scale_id(DWORD id);
        void __fastcall on_scale_refresh();
        bool __fastcall CalcLineSegs(DWORD from);
public:		// User declarations
        __fastcall TGKScaleFrm(TComponent* Owner,HWND owner_hwnd,GKHANDLE handle,DWORD win_type);
        __fastcall ~TGKScaleFrm();
};
//---------------------------------------------------------------------------
extern PACKAGE TGKScaleFrm *GKScaleFrm;
//---------------------------------------------------------------------------

#define NEW_SCALE_NAME          "Создание ..."
#define S_NAME_CHANGE           0x0001
#define MIN_WARN_CHANGE         0x0002
#define MAX_WARN_CHANGE         0x0004
#define MIN_ALARM_CHANGE        0x0008
#define MAX_ALARM_CHANGE        0x0010
#define SEGMENTS_CHANGE         0x0020

#define LINE_UPDATE             0x0000
#define LINE_MIN_X              0x0001
#define LINE_MAX_X              0x0002
#define LINE_MIN_Y              0x0004
#define LINE_MAX_Y              0x0008

#endif
