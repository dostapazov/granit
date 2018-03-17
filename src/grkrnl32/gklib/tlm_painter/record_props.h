//---------------------------------------------------------------------------

#ifndef record_propsH
#define record_propsH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ComCtrls.hpp>
#include <mediumdb.h>

//---------------------------------------------------------------------------
class TRecordProps : public TForm
{
__published:	// IDE-managed Components
	TTreeView *PropsTree;
private:	// User declarations
      LPMDB_RECORD Flp_rec;
      void __fastcall update_view(TTreeNode * node);

public:		// User declarations
	__fastcall TRecordProps(TComponent* Owner);
        __property LPMDB_RECORD lp_rec = {read = Flp_rec, write = set_record};
        void __fastcall set_record(LPMDB_RECORD _lp_rec);
};
//---------------------------------------------------------------------------
extern PACKAGE TRecordProps *RecordProps;
//---------------------------------------------------------------------------
#endif
