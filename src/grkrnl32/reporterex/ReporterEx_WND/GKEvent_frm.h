//---------------------------------------------------------------------------

#ifndef GKEvent_frmH
#define GKEvent_frmH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "gkmodule_form.h"
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <GKReporterEx.h>
//---------------------------------------------------------------------------
class TGKEventFrm : public TGKModuleForm
{
__published:	// IDE-managed Components
        TPanel *Panel;
        TButton *BtClose;
        TBitBtn *BtUp;
        TBitBtn *BtDown;
        TLabel *Label1;
        TLabel *LDate;
        TLabel *Label3;
        TLabel *LTime;
        TLabel *Label5;
        TLabel *LType;
        TLabel *Label7;
        TLabel *LSource;
        TMemo *MDesc;
        TMemo *MData;
        TLabel *Label9;
        TLabel *LData;
        TBitBtn *BtCopy;
        void __fastcall BtCloseClick(TObject *Sender);
        void __fastcall BtUpClick(TObject *Sender);
        void __fastcall BtDownClick(TObject *Sender);
private:
        AnsiString __fastcall ToStringByte(LPVOID Data);	// User declarations
protected:
        void __fastcall on_module_state(GKHANDLE mod,DWORD state){};
        void __fastcall on_module_config_change(GKHANDLE mod,LPMODULE_CONFIG_DATA mcd){};
public:		// User declarations
        __fastcall TGKEventFrm(TComponent* Owner,HWND owner_hwnd,GKHANDLE handle = 0);
        void __fastcall SetUp(TListItem *item, LPVOID func);
};
//---------------------------------------------------------------------------
extern PACKAGE TGKEventFrm *GKEventFrm;
//---------------------------------------------------------------------------
#endif
