//---------------------------------------------------------------------------


#ifndef base_propsH
#define base_propsH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <gktlmdb.h>
//---------------------------------------------------------------------------
typedef void __fastcall (__closure * changes_notify)(bool );
class TBaseProps : public TFrame
{
__published:	// IDE-managed Components
        TLabel *Label3;
        TEdit *Number;
        TLabel *Label4;
        TEdit *Name;
        TLabel *Label1;
        TEdit *Edit1;
        TUpDown *ObjCount;
        TLabel *Label2;
        TEdit *ObjFirstNumber;
        void __fastcall NumberChange(TObject *Sender);
        void __fastcall ObjFirstNumberChange(TObject *Sender);
private:	// User declarations
        sotd_addr   addr;
        changes_notify Fon_prop_changes;
protected:
         void __fastcall call_changes_notify(bool arg);          
public:		// User declarations
              __fastcall TBaseProps(TComponent* Owner);
virtual  void __fastcall setup(otd_proto * op,bool create = false,lpotd_addr own_addr = NULL);
              __property changes_notify on_prop_changes  = { read=Fon_prop_changes, write=Fon_prop_changes };

};
//---------------------------------------------------------------------------
extern PACKAGE TBaseProps *BaseProps;
//---------------------------------------------------------------------------

inline    void __fastcall TBaseProps::call_changes_notify(bool arg)
{
 if(Fon_prop_changes)
    Fon_prop_changes(arg);
}

#endif
