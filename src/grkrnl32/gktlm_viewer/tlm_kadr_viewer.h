//---------------------------------------------------------------------------

#ifndef tlm_kadr_viewerH
#define tlm_kadr_viewerH
//---------------------------------------------------------------------------
#include "gktlm_viewer_form.h"
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ToolWin.hpp>
#include <System.ImageList.hpp>
#include <Vcl.ImgList.hpp>
#include <System.Actions.hpp>
#include <Vcl.ActnList.hpp>
#include <Vcl.Menus.hpp>
#include "arch_list.h"


//---------------------------------------------------------------------------

/*#define TC_CMD_SET_BKGND     (WM_USER+100)
#define TC_CMD_SET_ENTRYES   (WM_USER+101)
#define TC_CMD_UPDATE_KADR   (WM_USER+102)
#define TC_CMD_SET_CAPTION   (WM_USER+103)
#define TC_CMD_REPAINT_KADR  (WM_USER+104)
#define TC_CMD_SET_KADR      (WM_USER+105)
*/

class TTlmKadrViewer : public TTlmViewerForm
{
__published:	// IDE-managed Components
	TStatusBar *StatusBar1;
	TToolBar *ToolBar1;
	TToolButton *tbGetArchive;
	TImageList *ImageList1;
	TPopupMenu *PopupMenu1;
	TActionList *ActionList1;
	TAction *actShowHistory;
	TMenuItem *N1;
	TAction *actSelectAll;
	TMenuItem *actSelectAll1;
	TToolButton *tbKvit;
	TToolButton *tbClean;
	TAction *actKvit;
	TAction *actClean;
	TToolButton *ToolButton1;
	TToolButton *ToolButton2;
	TToolButton *ToolButton4;
	TAction *actRefresh;
	TToolButton *tbShowHidden;
	TToolButton *ToolButton5;
	TToolButton *bArchList;
	TMenuItem *N2;
	TMenuItem *N3;
	TAction *actSelAllDiscrete;
	TAction *actSelAnalogs;
	TToolButton *bRequestOscilogramm;
	TToolButton *ToolButton7;
	TMenuItem *miRequestOscillogramm;
	TToolButton *ToolButton6;
	TToolButton *ToolButton8;
	TToolButton *ToolButton9;
	TToolButton *ToolButton10;
	TAction *actTuBegin;
	TAction *actTuOnMore;
	TAction *actTuOffLess;
	TAction *actTuCancel;
	TToolButton *ToolButton11;
	TToolButton *ToolButton12;
	TToolButton *tbNoGrayBkgnd;
	TToolButton *tbFixed;
	TMenuItem *N4;
	TMenuItem *N5;
	TMenuItem *N6;
	TMenuItem *N7;
	TMenuItem *actTuCancel1;
	TMenuItem *N8;
	TMenuItem *N9;
	TMenuItem *N10;
	TAction *actRecordInfo;
	TMenuItem *N11;
	TAction *actWorkInOut;
	TAction *actClearTUError;
	TMenuItem *N12;
	TMenuItem *N13;
	TAction *actNewTrend;
	TMenuItem *N14;
	void __fastcall actShowHistoryExecute(TObject *Sender);
	void __fastcall actSelectAllExecute(TObject *Sender);
	void __fastcall actCleanExecute(TObject *Sender);
	void __fastcall actKvitExecute(TObject *Sender);
	void __fastcall actRefreshExecute(TObject *Sender);
	void __fastcall FormActivate(TObject *Sender);
	void __fastcall FormDeactivate(TObject *Sender);
	void __fastcall tbShowHiddenClick(TObject *Sender);
	void __fastcall bArchListClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall actSelAllDiscreteExecute(TObject *Sender);
	void __fastcall actSelAnalogsExecute(TObject *Sender);
	void __fastcall bRequestOscilogrammClick(TObject *Sender);
	void __fastcall actTuOnMoreExecute(TObject *Sender);
	void __fastcall actTuOffLessExecute(TObject *Sender);
	void __fastcall actTuCancelExecute(TObject *Sender);
	void __fastcall actTuBeginExecute(TObject *Sender);
	void __fastcall tbNoGrayBkgndClick(TObject *Sender);
	void __fastcall tbFixedClick(TObject *Sender);
	void __fastcall actRecordInfoExecute(TObject *Sender);
	void __fastcall actWorkInOutExecute(TObject *Sender);
	void __fastcall actClearTUErrorExecute(TObject *Sender);
	void __fastcall PainterPanelStartDrag(TObject *Sender, TDragObject *&DragObject);
	void __fastcall PainterPanelEndDrag(TObject *Sender, TObject *Target, int X, int Y);
	void __fastcall PainterPanelDragDrop(TObject *Sender, TObject *Source, int X, int Y);
	void __fastcall actNewTrendExecute(TObject *Sender);



private  :	// User declarations
protected:
         TArchList * arch_list;
         UnicodeString          temp_file_name ;
         KeRTL::TFileStream    *temp_file_stream;
         KeRTL::TStorage       *temp_data_storage;
         DWORD Fcurrent_selected,Fcurrent_over;
         bool                  Fdrag_active;

         void  __fastcall CreateParams        (TCreateParams & cp);
virtual  void __fastcall  WndProc             (Winapi::Messages::TMessage & msg);
virtual DWORD __fastcall on_select_entry (TTlmPainter * p,DWORD entry  );
virtual DWORD __fastcall on_mouse_over   (TTlmPainter * p,DWORD entry  );
virtual BOOL  __fastcall on_mouse_buttons(TTlmPainter * p,Tmouse_data * md);
virtual BOOL  __fastcall on_mouse_move   (TTlmPainter * p,Tmouse_data * md);

       void   __fastcall update_kadr     (DWORD kadr_id,DWORD mask);
       void   __fastcall list_view_on_data(TObject *Sender, TListItem *Item);
       void   __fastcall arch_list_tab_change(TObject *Sender);
       void   __fastcall arch_list_deactivate(TObject *Sender);
       void   __fastcall arch_list_view_click (TObject *Sender);
       void   __fastcall arch_list_del        (TObject *Sender);
       void   __fastcall clear_temp_storage();
       void   __fastcall check_archive_present();
       void   __fastcall enable_tutr_acts     ();
       void   __fastcall disable_tutr_acts    (bool all);
       void   __fastcall select_tutr(DWORD rec_id,bool _select);
       void   __fastcall repaint_changed_records();
       void   __fastcall fill_opened_kadrs(TListItems * Items);



	      __fastcall TTlmKadrViewer        (TComponent* Owner);
public   :// User declarations
	      __fastcall TTlmKadrViewer        (TComponent* Owner,TGKTlmViewer * vm);
      void    __fastcall request_history       (UnicodeString hist_caption,LONGLONG beg_tm, LONGLONG end_tm,DWORD alarm,TListItems * items = NULL);
      void    __fastcall request_history_all   (UnicodeString hist_caption,LONGLONG beg_tm, LONGLONG end_tm,DWORD alarm);
      void    __fastcall request_arch_list     (TObject * sender);
      void    __fastcall rename_arch_click     (TObject * sender);
      void    __fastcall handle_arch_list      (LPOTDM_KADR_ARCHIVE_RESPOND kar,BOOL end);

      void    __fastcall do_clean              (Tselected_list & lst);
      void    __fastcall do_receipt            (Tselected_list & lst);
virtual void  __fastcall size_for_kadr_size    (LONG cx, LONG cy, int & form_width,int &form_height);
        void  __fastcall get_record_status_text(DWORD rec_id,UnicodeString & str);
        void  __fastcall update_status_bar     ();
        void  __fastcall set_current_selected  (DWORD rec_id);
        void  __fastcall set_current_over      (DWORD rec_id);
              __property DWORD current_selected = {read = Fcurrent_selected,write = set_current_selected};
              __property DWORD current_over     = {read = Fcurrent_over    ,write = set_current_over    };
              __property bool  drag_active      = {read = Fdrag_active     ,write = Fdrag_active };
       void   __fastcall get_selected_recid (Tselected_list & rec_ids);
       void   __fastcall get_selected_recid (TListItems * items,Tselected_list & rec_ids);


};
//---------------------------------------------------------------------------
extern PACKAGE TTlmKadrViewer *TlmKadrViewer;
//---------------------------------------------------------------------------

class TRecDrag:public TDragObjectEx
{
  TTlmKadrViewer * Fkv;
  public:
  __fastcall virtual TRecDrag(TTlmKadrViewer * _kv):TDragObjectEx(),Fkv(_kv)
  {
   Fkv->drag_active = true;
  }
  __fastcall virtual ~TRecDrag()
  {
    Fkv->drag_active = false;
    Fkv->PainterPanel->EndDrag(this->Dropped);
    Fkv->get_painter()->reset_mbuttons();
  }
  __property TTlmKadrViewer * kadr_viewer = {read = Fkv};
};

#endif
