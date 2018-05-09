//---------------------------------------------------------------------------

#ifndef ReporterEx_frmH
#define ReporterEx_frmH
#include <Classes.hpp>
#include <ComCtrls.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include "gkmodule_form.h"
#include "ReadLogThread.h"
#include <ImgList.hpp>
#include <ExtCtrls.hpp>
#include <ToolWin.hpp>
#include <ActnList.hpp>
#include <Dialogs.hpp>
#include <System.Actions.hpp>
#include <System.ImageList.hpp>

//---------------------------------------------------------------------------
class TReporterExFrm : public TGKModuleForm
{
__published:	// IDE-managed Components
        TImageList *ImageTypes;
        TPanel *Panel1;
        TTreeView *TreeView;
        TSplitter *Splitter1;
        TPanel *Panel2;
        TMemo *MemoData;
        TListView *ListView;
        TSplitter *Splitter2;
        TMemo *MemoDesc;
        TSplitter *Splitter3;
        TStatusBar *StatusBar;
        TToolBar *ToolBar1;
        TToolButton *TbClose;
        TToolButton *ToolButton2;
        TToolButton *TbClear;
        TToolButton *ToolButton4;
        TToolButton *TbSound;
        TTimer *Timer;
        TToolButton *ToolButton1;
        TToolButton *TbProperty;
        TToolButton *ToolButton3;
        TToolButton *Tb8;
        TToolButton *Tb10;
        TToolButton *Tb16;
        TActionList *ActionList1;
        TAction *AClearAll;
        TOpenDialog *OpenDialog;
        TToolButton *ToolButton5;
        TToolButton *ToolButton6;
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall TreeViewChange(TObject *Sender, TTreeNode *Node);
        void __fastcall ListViewData(TObject *Sender, TListItem *Item);
        void __fastcall ListViewDataHint(TObject *Sender, int StartIndex,
          int EndIndex);
        void __fastcall ListViewChange(TObject *Sender, TListItem *Item,
          TItemChange Change);
        void __fastcall TbCloseClick(TObject *Sender);
        void __fastcall TbClearClick(TObject *Sender);
        void __fastcall FormResize(TObject *Sender);
        void __fastcall TbSoundClick(TObject *Sender);
        void __fastcall TimerTimer(TObject *Sender);
        void __fastcall TbPropertyClick(TObject *Sender);
        void __fastcall Tb8Click(TObject *Sender);
        void __fastcall Tb10Click(TObject *Sender);
        void __fastcall Tb16Click(TObject *Sender);
        void __fastcall Splitter2CanResize(TObject *Sender, int &NewSize,
          bool &Accept);
        void __fastcall Splitter3CanResize(TObject *Sender, int &NewSize,
          bool &Accept);
        void __fastcall AClearAllExecute(TObject *Sender);
        void __fastcall ToolButton6Click(TObject *Sender);
        void __fastcall TreeViewDragDrop(TObject *Sender, TObject *Source,
          int X, int Y);
        void __fastcall TreeViewDragOver(TObject *Sender, TObject *Source,
          int X, int Y, TDragState State, bool &Accept);
protected:
        LRESULT __fastcall on_gkhandle_notify (GKHANDLE from,LPNOTIFY_CODE nc,DWORD mask);
        void __fastcall on_module_state(GKHANDLE mod,DWORD state){};
        void __fastcall on_module_config_change(GKHANDLE mod,LPMODULE_CONFIG_DATA mcd){};
private:
        TReadLogThread *ReadLogThread;
        int Code;
        void __fastcall on_report_added(LPNOTIFY_CODE nc);
        void __fastcall on_report_refresh(LPNOTIFY_CODE nc);	// User declarations
        void __fastcall on_report_newevent(LPNOTIFY_CODE nc);
        void __fastcall ResizeView();
virtual void  __fastcall setup_form_sizes();

public:		// User declarations
        __fastcall TReporterExFrm(TComponent* Owner,HWND owner_hwnd,GKHANDLE handle = 0);
        __fastcall ~TReporterExFrm();
};
//---------------------------------------------------------------------------

int __stdcall TypeCompare(long Item1, long Item2, long ParamSort);
int __stdcall DateCompare(long Item1, long Item2, long ParamSort);
int __stdcall TimeCompare(long Item1, long Item2, long ParamSort);
int __stdcall SourceCompare(long Item1, long Item2, long ParamSort);
int __stdcall DescriptionCompare(long Item1, long Item2, long ParamSort);

#endif
