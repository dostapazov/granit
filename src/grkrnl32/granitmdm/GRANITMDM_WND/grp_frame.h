//---------------------------------------------------------------------------

#ifndef grp_frameH
#define grp_frameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "sc_base_frame.h"
#include <ComCtrls.hpp>
#include <Buttons.hpp>

//---------------------------------------------------------------------------
class TGroupFrame : public TSCBaseFrame
{
__published:	// IDE-managed Components
        TGroupBox *GroupBox1;
        TLabel *Label1;
        TEdit *GrpNum;
        TComboBox *GrpFa;
        TLabel *Label2;
        TStaticText *ParamCount;
        TLabel *Label3;
        TLabel *Label4;
        TEdit *ModNum;
        TGroupBox *GroupBox2;
        TLabel *Label6;
        TEdit *WorkTimer;
        TLabel *Label7;
        TEdit *NoWorkTimer;
        TGroupBox *GroupBox3;
        TEdit *ObjNumber;
        TSpeedButton *AddTuEntry;
        TSpeedButton *DelTuEntry;
        TSpeedButton *SetTuEntry;
        TListView *TuTrList;
        TComboBox *TuCmd;
        TComboBox *TuGroup;
        TComboBox *TuObj;
        TComboBox *TuTimeout;
        TCheckBox *TsInv;
        TCheckBox *NoCheckTU;
        TComboBox *TuHoldTime;
        TLabel *Label8;
        TLabel *Label9;
        TLabel *Label10;
        TCheckBox *RAviaRC;
        TLabel *Label11;
        TComboBox *TuSelTime;
        TLabel *Label12;
        TComboBox *TuModule;
        TLabel *Label13;
        TEdit *RespondSmooth;
        TLabel *Label5;
        TEdit *RespondTimer;
        TCheckBox *ApplyTimersToAll;
        void __fastcall RespondTimerChange(TObject *Sender);
        void __fastcall GrpFaChange(TObject *Sender);
        void __fastcall flStaticClick(TObject *Sender);
        void __fastcall ModNumChange(TObject *Sender);
        void __fastcall TuCmdChange(TObject *Sender);
        void __fastcall AddTuEntryClick(TObject *Sender);
        void __fastcall TuTrListSelectItem(TObject *Sender,
          TListItem *Item, bool Selected);
        void __fastcall SetTuEntryClick(TObject *Sender);
        void __fastcall DelTuEntryClick(TObject *Sender);
        void __fastcall RespondSmoothChange(TObject *Sender);
        void __fastcall ApplyTimersToAllClick(TObject *Sender);
private  :	// User declarations
protected:
GR_GRP_PROPS   props;
void           __fastcall setup_controls(GR_GRP_PROPS & prp);
void           __fastcall setup_tu_list ();
void           __fastcall setup_tu_item(TListItem * item,GRSC_TU_ENTRY &tue);
void           __fastcall setup_tu_controls(GRSC_TU_ENTRY & tue);
bool           __fastcall on_tu_entry_added  (DWORD idx);
void           __fastcall on_tu_entry_set    (DWORD idx);
void           __fastcall on_tu_entry_removed(DWORD idx);

public:		// User declarations
        __fastcall TGroupFrame(TComponent* Owner);
bool    __fastcall apply_changes() ;
bool    __fastcall undo_changes () ;
void    __fastcall set_active   (DWORD );
void    __fastcall set_inactive();
AnsiString __fastcall get_text();
bool    __fastcall make_tu_entry(GRSC_TU_ENTRY &tue,bool & new_entry);
void    __fastcall update_changes(LPGRSC_ACR acr);
};
//---------------------------------------------------------------------------
extern PACKAGE TGroupFrame *GroupFrame;
//---------------------------------------------------------------------------
#endif
