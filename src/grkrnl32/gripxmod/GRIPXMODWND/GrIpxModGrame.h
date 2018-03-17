//---------------------------------------------------------------------------

#ifndef GrIpxModGrameH
#define GrIpxModGrameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "gkmodule_form.h"
#include <ComCtrls.hpp>
#include <ToolWin.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <gripxmod.h>
//---------------------------------------------------------------------------
class TGrIpxModuleWnd : public TGKModuleForm
{
__published:	// IDE-managed Components
        TPageControl *PageControl1;
        TTabSheet *TabSheet2;
        TTabSheet *TabSheet1;
        TGroupBox *GroupBox1;
        TLabel *Label2;
        TLabel *Label3;
        TEdit *ServerName;
        TEdit *ServerSocket;
        TGroupBox *GroupBox2;
        TLabel *Label1;
        TLabel *Label4;
        TLabel *Label5;
        TLabel *Label6;
        TEdit *PUNumberEdit;
        TUpDown *PUNumber;
        TEdit *RetryCountEdit;
        TUpDown *RetryCount;
        TEdit *RetryTimeoutEdit;
        TBitBtn *ApplyBtn;
        TBitBtn *UndoBtn;
        TRadioButton *RadioButton1;
        TRadioButton *RadioButton2;
        TGroupBox *GroupBox3;
        TLabel *Label7;
        TLabel *Label8;
        TEdit *ClientName;
        TEdit *ClientSocket;
        TTreeView *ClientsTree;
        TSplitter *Splitter1;
        TMemo *Memo1;
        TRadioButton *RadioButton3;
        void __fastcall RadioButton1Click(TObject *Sender);
        void __fastcall UndoBtnClick(TObject *Sender);
        void __fastcall ApplyBtnClick(TObject *Sender);
        void __fastcall RetryTimeoutEditChange(TObject *Sender);
        void __fastcall PageControl1Change(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall FormHide(TObject *Sender);
private:	// User declarations
void     __fastcall after_set_gkhandle();
void     __fastcall set_settings();
void     __fastcall apply_settings();
LRESULT  __fastcall on_gkhandle_notify (GKHANDLE from,LPNOTIFY_CODE nc,DWORD mask);
void     __fastcall monitor_grnet_kadr (LPGRNET_KADR grnet_kadr,DWORD len, BOOL tx);
void     __fastcall monitor_string(AnsiString & s);
void     __fastcall monitor_gratony_kadr( lp_gratony_kadr  kadr);
public:		// User declarations
        __fastcall TGrIpxModuleWnd(TComponent* Owner,HWND p_wnd,GKHANDLE handle);
};
//---------------------------------------------------------------------------
extern PACKAGE TGrIpxModuleWnd *GrIpxModuleWnd;
//---------------------------------------------------------------------------
#endif
