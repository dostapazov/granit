//---------------------------------------------------------------------------

#ifndef mdb_currstateH
#define mdb_currstateH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "gkmodule_form.h"
#include <ComCtrls.hpp>
#include <mediumdb.h>
#include <ExtCtrls.hpp>
#include <vector_index.hpp>
namespace vi =  vector_index;
//typedef vi::indexed_vector<mdb_record,>




//---------------------------------------------------------------------------
class TMDBCurrentState : public TGKModuleForm
{
__published:	// IDE-managed Components
        TListView *RecordsList;
        TGroupBox *GroupBox1;
        TLabel *Label1;
        TLabel *OtdAddr;
        TLabel *FullName;
        TLabel *Label2;
        TLabel *OtdValue;
        TLabel *Label3;
        TLabel *OtdPd;
        TLabel *Label4;
        TLabel *TimeDiff;
        TLabel *Label5;
        TLabel *LastValue;
        TLabel *Percent;
        TLabel *MinMax;
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall RecordsListColumnClick(TObject *Sender,
          TListColumn *Column);
        void __fastcall RecordsListSelectItem(TObject *Sender,
          TListItem *Item, bool Selected);
        void __fastcall RecordsListMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
private:	// User declarations
otd_addr   Ffilter_addr;
bool       Fonly_combined;
public:		// User declarations
         __fastcall TMDBCurrentState(TComponent* Owner,GKHANDLE handle);
void     __fastcall on_module_config_change(GKHANDLE mod,LPMODULE_CONFIG_DATA mcd);
void     __fastcall on_module_state(GKHANDLE mod,DWORD state);
LRESULT  __fastcall on_gkhandle_notify (GKHANDLE from,LPNOTIFY_CODE nc,DWORD mask);
void     __fastcall on_record_changed  (DWORD id,DWORD mask);
void     __fastcall on_record_created  (DWORD id);
void     __fastcall on_record_removed  (DWORD id);
void     __fastcall after_set_gkhandle();
void     __fastcall setup_list_item(TListItem * item,mdb_record & rec,DWORD mask);
void     __fastcall setup_list_view(bool by_addr);
void     __fastcall setup_additional(mdb_record & rec,DWORD mask);
__property otd_addr filter_addr = {read = Ffilter_addr,write = set_filter_addr};
__property bool     only_combined = {read = Fonly_combined,write = Fonly_combined};
void    __fastcall set_filter_addr(otd_addr  addr);
void    __fastcall on_select_record(TListItem *Item, bool Selected);
bool    __fastcall select_item_byid(DWORD rec_id);

};
//---------------------------------------------------------------------------
extern PACKAGE TMDBCurrentState *MDBCurrentState;
//---------------------------------------------------------------------------
#endif
