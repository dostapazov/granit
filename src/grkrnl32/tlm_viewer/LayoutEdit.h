//---------------------------------------------------------------------------

#ifndef LayoutEditH
#define LayoutEditH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TLaoutEdit : public TForm
{
__published:	// IDE-managed Components
        TListView *ListView1;
        TSpeedButton *BUp;
        TSpeedButton *BDn;
        TBitBtn *BitBtn1;
        TBitBtn *BitBtn2;
        TSpeedButton *BDel;
        void __fastcall ListView1SelectItem(TObject *Sender,
          TListItem *Item, bool Selected);
        void __fastcall BDelClick(TObject *Sender);
        void __fastcall BUpClick(TObject *Sender);
        void __fastcall BDnClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TLaoutEdit(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TLaoutEdit *LaoutEdit;
//---------------------------------------------------------------------------
#endif
