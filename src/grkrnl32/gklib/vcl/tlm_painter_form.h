//---------------------------------------------------------------------------

#ifndef tlm_painter_formH
#define tlm_painter_formH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <System.ImageList.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.ExtDlgs.hpp>
#include <Vcl.ImgList.hpp>
#include <Vcl.ToolWin.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.ButtonGroup.hpp>
#include <memory>
#include <tlm_painter.hpp>
#include <kadrs_table.hpp>
#include <text_table.hpp>

#define WM_SHOW_REFRESH       (WM_USER+1000)
#define WM_RESET_KADR         (WM_USER+1001)
#define WM_FLASH_KADR         (WM_USER+1002)
#define WM_PAINTER_USER_FIRST (WM_USER+2000)

typedef bool           __fastcall (__closure * Tget_record_closure)(DWORD rec_id,MDB_RECORD & rec,bool * is_visible);
typedef UnicodeString  __fastcall (__closure * Tget_text_closure  )(DWORD str_id);

//---------------------------------------------------------------------------
class TTlmPainterForm : public TForm
{
__published:	// IDE-managed Components
	TToolBar *ToolBar1;
	TStatusBar *StatusBar1;
	TPaintBox *paint_area;
	TImageList *TbImageList;
	TPopupMenu *kadr_popup;
	TMenuItem *miSelectAll;
	TMenuItem *miSelectDiscrete;
	TMenuItem *miSelectAnalogs;
	TMenuItem *miSelGroup;
	TMenuItem *miInvertSelect;
	TMenuItem *miSelectDiscreteInvert;
	TMenuItem *miSelectAnalogsInvert;
	void __fastcall FormResize(TObject *Sender);
	void __fastcall paint_areaMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall paint_areaPaint(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall paint_areaMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,int X, int Y);
	void __fastcall miSelectAllClick(TObject *Sender);
	void __fastcall miSelectDiscreteClick(TObject *Sender);
	void __fastcall miSelectAnalogsClick(TObject *Sender);
	void __fastcall paint_areaMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);


protected:	// User declarations

      typedef tlmpaint::paint_action_t<TTlmPainterForm> paint_action_t;
      typedef tlmpaint::index_action_t<kadr_entryes_table,paint_action_t> index_paint_action_t;
      typedef tlmpaint::index_action_t<kadr_entryes_table,tlmpaint::move_action_t>  index_move_action_t;
      typedef tlmpaint::index_action_t<kadr_entryes_table,tlmpaint::size_action_t>  index_size_action_t;
      Tget_record_closure    Fon_get_record;
      Tget_text_closure      Fon_get_text;
      UnicodeString          _gts;
      int                    Fpaint_context;
      int                    Fpaint_locker;
      tlmpaint::TBmpBkgnd    bkgnd;
      std::auto_ptr<TBitmap> bk_bmp;

      Tmouse_state           mstate;
      single_kadr          * Fkadr;
      bool                   Fcaptured;
      DWORD                  Fcurrent_over;
      bool                   Feditable;
      bool                   Ffixed;
      bool                   Fmulty_selected;
      POINT                  Fsel_rect_lt;
      POINT                  Fsel_rect_br;
      DWORD                  Fflash_counter;

      TBitmap * __fastcall get_bitmap();
      void __fastcall      CreateParams(TCreateParams & params);

      virtual void __fastcall  show_entry_info       (mdb_kadr_entry *entry){}
      virtual void __fastcall  update_status_bar     ();
      virtual void __fastcall  paint_bkgnd           ();

      virtual void __fastcall  paint_entryes         (bool erase,bool flash);
      virtual void __fastcall  paint_selected_entryes(bool erase,bool flash);
      virtual void __fastcall  set_flash_counter     (DWORD  fc);

      virtual void __fastcall  on_mouse_down      (TMouseButton Button,TShiftState Shift, int X, int Y);
      virtual void __fastcall  on_mouse_up        (TMouseButton Button,TShiftState Shift, int X, int Y);
      virtual void __fastcall  on_mouse_move      (TShiftState Shift, int X, int Y );
      virtual void __fastcall  captured_mouse_move(TShiftState Shift, int X, int Y );

      virtual void __fastcall  do_move_selected(int dx,int dy);
      virtual void __fastcall  do_size_selected(int dx,int dy);
      virtual void __fastcall  set_current_over(DWORD val);
      virtual void __fastcall  paint_select_rect(HDC dc,POINT & spot_pt,LPPOINT  new_pt,LPPOINT old_pt);
      virtual int  __fastcall  get_captured_delta(){return 4;}
      virtual bool __fastcall  is_entry_selectable(mdb_kadr_entry * entry){return entry ? true : false;}
              void __fastcall  set_captured (bool val);
              void __fastcall  set_kadr_size(int cx,int cy);

      __property bool  captured     = {read = Fcaptured    , write = set_captured};
      __property DWORD current_over = {read = Fcurrent_over, write = set_current_over};

      void __fastcall WndProc(TMessage & msg);
      TWinControl* __fastcall get_owner_control();
virtual  void      __fastcall reset_kadr  (DWORD mask = -1);
         void      __fastcall paint_lock    (){++Fpaint_locker;}
         void      __fastcall paint_unlock  ();
public:		// User declarations
virtual __fastcall TTlmPainterForm(TComponent* Owner);
      __property Tget_record_closure  on_get_record  = {read = Fon_get_record ,write = Fon_get_record   };
      __property Tget_text_closure    on_get_text    = {read = Fon_get_text   ,write = Fon_get_text     };
      __property int                  paint_context  = {read = Fpaint_context ,write = set_paint_context};
      __property bool                 bk_grayed      = {read = get_bk_grayed  ,write = set_bk_grayed    };
      __property bool                 editable       = {read = Feditable      ,write = Feditable        };

                void      __fastcall set_paint_context(int pctx){Fpaint_context = pctx;refresh_kadr();}
  virtual       bool      __fastcall get_record   (DWORD rec_id,MDB_RECORD & rec,bool * rec_visible) const;
  virtual const wchar_t * __fastcall get_name     (MDB_RECORD & rec,bool short_name) const;
          UnicodeString   __fastcall get_record_name(DWORD  rec_id,bool short_name) const;
  virtual       LPVOID    __fastcall get_paint_variant(const MDB_KADR_ENTRY & entry) const{return NULL;}
  virtual UnicodeString   __fastcall get_text     (DWORD str_id) const;
                void      __fastcall set_bk_mode  (tlmpaint::TBmpBkgnd::bkgnd_mode_t mode);
                void      __fastcall set_bk_image (UnicodeString image_name);
                void      __fastcall set_bk_grid  (int grid_size);
   virtual      void      __fastcall set_kadr     (single_kadr * kadr);
        single_kadr*      __fastcall get_kadr     (){return Fkadr;}
                enum class sel_mode_t {deselect,select,toggle,clear};
                void      __fastcall  select_entry    (DWORD offset,sel_mode_t sel);
        virtual void      __fastcall  select_changed  (DWORD offset,bool selected){refresh_kadr();};
                void      __fastcall  selections_clear();
                void      __fastcall  select_in_rect (bool add);

                void      __fastcall  refresh_kadr    ();
                void      __fastcall  update_kadr     ();
virtual   UnicodeString   __fastcall  get_kadr_caption();
virtual   UnicodeString   __fastcall  get_image_file  ();
                int       __fastcall  get_bk_grid     (){return bkgnd.get_grid() ;}
                bool      __fastcall  get_bk_grayed   (){return bkgnd.is_grayed();}
                void      __fastcall  set_bk_grayed   (bool val){if(bk_grayed != val) {bkgnd.set_grayed(val); paint_bkgnd();} }
};
//---------------------------------------------------------------------------
extern PACKAGE TTlmPainterForm *TlmPainterForm;
//---------------------------------------------------------------------------


inline     void      __fastcall  TTlmPainterForm::selections_clear ()
{
    select_entry(-1,sel_mode_t::clear);
}

inline TWinControl* __fastcall TTlmPainterForm::get_owner_control   ()
{
   return dynamic_cast<TWinControl*>(Owner);
}

inline void      __fastcall TTlmPainterForm::paint_unlock  ()
{
    if(Fpaint_locker)
    {
      if(!(--Fpaint_locker)) refresh_kadr();
    }
}

inline void      __fastcall  TTlmPainterForm::refresh_kadr    ()
{
   if(!Fpaint_locker)
   {
    paint_area->Repaint();
    update_status_bar  ();
   }
}

inline void      __fastcall  TTlmPainterForm::update_kadr    ()
{
   paint_area->Update();
}

#endif
