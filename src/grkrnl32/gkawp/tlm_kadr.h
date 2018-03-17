//---------------------------------------------------------------------------

#ifndef tlm_kadrH
#define tlm_kadrH
//---------------------------------------------------------------------------
#include "gkawpmodule.hpp"
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "tlm_painter_form.h"
#include <System.ImageList.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.ImgList.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.ToolWin.hpp>
#include <System.Actions.hpp>
#include <Vcl.ActnList.hpp>
//---------------------------------------------------------------------------

#define WM_RC_ENABLE           (WM_PAINTER_USER_FIRST+0)
#define WM_RC_STATE_CHANGE     (WM_PAINTER_USER_FIRST+1)
#define WM_HISTORY_PRESENT     (WM_PAINTER_USER_FIRST+2)
#define WM_HANDLE_KADR_ARCHIVE (WM_PAINTER_USER_FIRST+4)

class TGkAwpModule;
class TArchList;

class TTlmKadrForm : public TTlmPainterForm
{
__published:	// IDE-managed Components
	TToolButton *tbRefresh;
	TToolButton *ToolButton2;
	TToolButton *tbReceipt;
	TToolButton *tbClean;
	TToolButton *tbShowHidden;
	TToolButton *ToolButton6;
	TToolButton *ToolButton7;
	TToolButton *tbNoGrayBkgnd;
	TToolButton *tbFixed;
	TToolButton *ToolButton10;
	TToolButton *ToolButton11;
	TToolButton *ToolButton12;
	TToolButton *ToolButton13;
	TToolButton *ToolButton14;
	TToolButton *tbAlarmHistoryList;
	TToolButton *tbRequestOscilogramm;
	TToolButton *ToolButton17;
	TToolButton *tbShowHistory;
	TMenuItem *miRecordDetails;
	TMenuItem *N1;
	TMenuItem *miWorkInOut;
	TActionList *KadrActions;
	TAction *actWorkInout;
	TAction *actTuErrorClear;
	TMenuItem *miTuErrorClear;
	TAction *actShowHistory;
	TMenuItem *N2;
	TMenuItem *N3;
	TAction *actRcBegin;
	TMenuItem *miRcCommon;
	TMenuItem *miTuBegin;
	TAction *actRcOnMore;
	TAction *actRcOffLess;
	TMenuItem *miRcOnMore;
	TMenuItem *miRcOffLess;
	TAction *actRcUndo;
	TMenuItem *miRcUndo;
	TMenuItem *miNewTrend;
	TPopupMenu *CautionsPopup;
	TToolButton *ToolButton1;
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall tbShowHiddenClick(TObject *Sender);
	void __fastcall tbRefreshClick(TObject *Sender);
	void __fastcall tbNoGrayBkgndClick(TObject *Sender);
	void __fastcall tbFixedClick(TObject *Sender);
	void __fastcall miRecordDetailsClick(TObject *Sender);
	void __fastcall tbReceiptClick(TObject *Sender);
	void __fastcall actWorkInoutExecute(TObject *Sender);
	void __fastcall actTuErrorClearExecute(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall actShowHistoryExecute(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall actRcBeginExecute(TObject *Sender);
	void __fastcall actRcUndoExecute(TObject *Sender);
	void __fastcall actRcOnMoreExecute(TObject *Sender);
	void __fastcall actRcOffLessExecute(TObject *Sender);
	void __fastcall miNewTrendClick(TObject *Sender);
	void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall tbAlarmHistoryListClick(TObject *Sender);
	void __fastcall CautionsPopupPopup(TObject *Sender);
	void __fastcall tbRequestOscilogrammClick(TObject *Sender);

private:	// User declarations
     TGkAwpModule          * module;
     TArchList             * arch_list;
     bool                   drag_active;
     UnicodeString          temp_file_name ;
     KeRTL::TFileStream    *temp_file_stream;
     KeRTL::TStorage       *temp_data_storage;
  virtual       void      __fastcall AfterConstruction();
  virtual       void      __fastcall BeforeDestruction();

  virtual       bool      __fastcall get_record   (DWORD rec_id,MDB_RECORD & rec,bool * rec_visible) const;
  virtual UnicodeString   __fastcall get_text     (DWORD str_id) const;
  virtual UnicodeString   __fastcall get_image_file();
  virtual       void      __fastcall reset_kadr  (DWORD mask = -1);

  virtual       void      __fastcall set_current_over(DWORD val);
  virtual       void      __fastcall show_entry_info        (mdb_kadr_entry *entry);
                void      __fastcall get_record_status_text (mdb_record & rec,UnicodeString & str);
  virtual       void      __fastcall paint_entryes(bool erase,bool flash);

  virtual       bool      __fastcall is_entry_selectable(mdb_kadr_entry * entry);
  virtual       void      __fastcall select_changed   (DWORD offset,bool selected);
                void      __fastcall fill_opened_kadrs(TListItems * items);
                void      __fastcall request_history(UnicodeString period_text,QWORD tm_beg,QWORD tm_end,DWORD alarm,TListItems * items);
                void      __fastcall request_history(UnicodeString hist_capt,UnicodeString period_text,QWORD tm_beg,QWORD tm_end,DWORD alarm,records_ids_t & rec_ids);

                void      __fastcall remote_control_select  (DWORD rec_id,bool _select);
                void      __fastcall remote_control_enable  (DWORD rec_id,bool enable);
                void      __fastcall remote_control_undo    (DWORD rec_id);
                void      __fastcall remote_control_on_more (DWORD rec_id);
                void      __fastcall remote_control_off_less(DWORD rec_id);
                void      __fastcall remote_control_state_change(DWORD rec_id);

                void      __fastcall start_drag         (TObject *Sender, TDragObject *&DragObject);
  virtual       void      __fastcall captured_mouse_move(TShiftState Shift, int X, int Y );

                void      __fastcall WndProc(TMessage & msg);
                bool      __fastcall get_disable_bk_grayed();
                void      __fastcall set_disable_bk_grayed(bool val);
                void      __fastcall set_history_present  (bool pr ){tbAlarmHistoryList->Enabled = pr;actShowHistory->Enabled = pr;}
                void      __fastcall clear_temp_storage   ();

                //Список архивов
                void      __fastcall arch_list_tab_change  (TObject *Sender);
                void      __fastcall arch_list_view_click  (TObject *Sender);
                void      __fastcall arch_list_deactivate  (TObject *Sender);
                void      __fastcall arch_list_del         (TObject *Sender);
                void      __fastcall arch_list_handle      (LPOTDM_KADR_ARCHIVE_RESPOND kar,BOOL end);
                void      __fastcall arch_list_view_on_data(TObject *Sender, TListItem *Item);
                void      __fastcall arch_list_request     (TObject * sender);
                void      __fastcall arch_list_rename      (TObject * sender);
                void      __fastcall handle_kadr_archives  (LPOTDM_KADR_ARCHIVE_RESPOND respond,bool end_data);
                DWORD     __fastcall make_caution_list     (records_t & list);
                DWORD     __fastcall make_caution_menu     (TMenuItem * items);
                void      __fastcall on_caution_click      (TObject * sender);



	__fastcall TTlmKadrForm(TComponent* Owner);

public:		// User declarations
virtual __fastcall TTlmKadrForm(TComponent* Owner,TGkAwpModule * _module);
        __property bool    fixed              = {read = Ffixed  ,write = Ffixed};
        __property bool    disable_bk_grayed  = {read = get_disable_bk_grayed  ,write = set_disable_bk_grayed};
           void __fastcall end_drag();
};
//---------------------------------------------------------------------------
extern PACKAGE TTlmKadrForm *TlmKadrForm;
//---------------------------------------------------------------------------

class TRecDrag:public TDragObjectEx
{
  TTlmKadrForm   * Fkf;
  public:
  __fastcall virtual TRecDrag(TTlmKadrForm * _kf):TDragObjectEx(),Fkf(_kf)
  {
   //Fkv->drag_active = true;
  }
  __fastcall virtual ~TRecDrag()
  {
    //Fkv->drag_active = false;
    Fkf->ToolBar1->EndDrag(Dropped);
    Fkf->end_drag();
    //Fkf->get_painter()->reset_mbuttons();
  }
  __property TTlmKadrForm * kadr_form   = {read = Fkf};
};


#define TLMKDR_MSG_FIRST (WM_USER+2000)

#endif
