//---------------------------------------------------------------------------

#ifndef record_infoH
#define record_infoH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ComCtrls.hpp>
//---------------------------------------------------------------------------
class TRecordInfo : public TForm
{
__published:	// IDE-managed Components
	TListView *ListView1;
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private:	// User declarations
public:		// User declarations
	__fastcall TRecordInfo(TComponent* Owner);
TListItem * __fastcall get_item(int idx)  { return idx < ListView1->Items->Count ? ListView1->Items->Item[idx] : NULL;}
};
//---------------------------------------------------------------------------
extern PACKAGE TRecordInfo *RecordInfo;
//---------------------------------------------------------------------------
#define RECINFO_RECID   0
#define RECINFO_OTDADDR 1
#define RECINFO_TYPE    2
#define RECINFO_SNAME   3
#define RECINFO_NAME    4
#define RECINFO_VALUE   5
#define RECINFO_TIME    6
#define RECINFO_DIAG    7
#define RECINFO_WORK    8
#define RECINFO_TU      9
#define RECINFO_STATE   10
#define RECINFO_CH_CNT  11
#define RECINFO_OPTIONS 12

#endif
