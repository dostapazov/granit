//---------------------------------------------------------------------------

#ifndef gk_aboutH
#define gk_aboutH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <jpeg.hpp>
//---------------------------------------------------------------------------
class TExplAbout : public TForm
{
__published:	// IDE-managed Components
        TTimer *Timer1;
        TImage *Image1;
        TBitBtn *BitBtn1;
        void __fastcall FormShow(TObject *Sender);
        void __fastcall Timer1Timer(TObject *Sender);
        void __fastcall FormKeyPress(TObject *Sender, char &Key);
private:	// User declarations
void     __fastcall WndProc(Messages::TMessage &Message);
void     __fastcall AfterConstruction(void);
public:		// User declarations
        __fastcall TExplAbout(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TExplAbout *ExplAbout;
//---------------------------------------------------------------------------
#endif
