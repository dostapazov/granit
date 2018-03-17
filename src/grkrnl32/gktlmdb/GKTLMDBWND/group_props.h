//---------------------------------------------------------------------------


#ifndef group_propsH
#define group_propsH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TBaseProps : public TFrame
{
__published:	// IDE-managed Components
        TGroupBox *GroupBox1;
        TLabel *Label1;
        TEdit *ObjectCount;
        TUpDown *UpDown1;
        TLabel *Label2;
        TEdit *ObjFirstNumber;
        TLabel *Label3;
        TEdit *Number;
        TLabel *Label4;
        TEdit *Name;
private:	// User declarations
public:		// User declarations
        __fastcall TBaseProps(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TBaseProps *BaseProps;
//---------------------------------------------------------------------------
#endif
