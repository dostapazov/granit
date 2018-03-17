//---------------------------------------------------------------------------

#ifndef tstfH
#define tstfH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ADODB.hpp>
#include <Db.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
        TPanel *Panel1;
        TButton *Button1;
        TButton *Button2;
        TCheckBox *CheckBox1;
        TCheckBox *HexOut;
        TLabel *Label3;
        TEdit *Edit1;
        TPanel *Panel2;
        TLabel *Label2;
        TMemo *Expr;
        TPanel *Panel3;
        TLabel *Label1;
        TMemo *Results;
        TSplitter *spl1;
        void __fastcall Button1Click(TObject *Sender);
        void __fastcall Button2Click(TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall FormDestroy(TObject *Sender);
private:	// User declarations
        LPVOID  Calc;
public:		// User declarations
        __fastcall TForm1(TComponent* Owner);
protected:
        __fastcall SetResult(LPVOID calc, long double  res);
static void __fastcall ReplaceCommas(AnsiString & text);        
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
