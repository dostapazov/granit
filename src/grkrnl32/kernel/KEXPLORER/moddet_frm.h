//---------------------------------------------------------------------------

#ifndef moddet_frmH
#define moddet_frmH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TModuleDefine : public TForm
{
__published:	// IDE-managed Components
        TEdit *Edit1;
        TLabel *Label1;
        TMemo *Memo1;
        TButton *Button1;
        void __fastcall Button1Click(TObject *Sender);
        void __fastcall FormDestroy(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private:	// User declarations
public:		// User declarations
        __fastcall TModuleDefine(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TModuleDefine *ModuleDefine;
//---------------------------------------------------------------------------
#endif
