//---------------------------------------------------------------------------

#ifndef arch_listH
#define arch_listH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TArchList : public TForm
{
__published:	// IDE-managed Components
	TBitBtn *ViewBtn;
	TTabControl *TabControl1;
	TListView *ListView1;
	TPanel *Panel1;
	TBitBtn *DelBtn;
	TBitBtn *ClearBtn;
	TPanel *Panel2;
	TDateTimePicker *beg_date;
	TDateTimePicker *end_date;
	TBitBtn *bRefresh;
	TLabel *Label1;
	TLabel *Label2;
	TBitBtn *bRename;
	void __fastcall ListView1DblClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TArchList(TComponent* Owner);
    int __fastcall get_tab_index(){return TabControl1->TabIndex;}
    __property int is_all = {read = get_tab_index};
    UnicodeString __fastcall get_arch_name();
    bool          __fastcall get_protect  ();
    void          __fastcall set_arch_name(UnicodeString text);
    void          __fastcall set_protect  (bool prot);
 };
//---------------------------------------------------------------------------
extern PACKAGE TArchList *ArchList;
//---------------------------------------------------------------------------
#endif
