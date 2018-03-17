//---------------------------------------------------------------------------

#ifndef kadr_painterH
#define kadr_painterH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <mediumdb.h>
#include <kadrs_table.hpp>
#include <tlm_painter.h>
#include <gkmodules.hpp>
#include <pictdlgs.h>
#include <tlm_painter.hpp>

//---------------------------------------------------------------------------

typedef WideString __fastcall (__closure * get_text_method        )(DWORD text_id);
typedef WideString __fastcall (__closure * get_name_method        )(DWORD rec_id,bool full);
typedef bool       __fastcall (__closure * get_record_method      )(DWORD rec_id,MDB_RECORD & sr);
typedef void       __fastcall (__closure * set_kadr_change_method )(DWORD kadr_id,DWORD mask);
typedef void       __fastcall (__closure * on_select_entry_method )(mdb_kadr_entry * entry,bool added);
typedef void       __fastcall (__closure * on_over_entry_method   )(mdb_kadr_entry * entry);


class TKadrPainter : public TForm
{
__published:	// IDE-managed Components
        void __fastcall FormResize(TObject *Sender);
        void __fastcall FormDestroy(TObject *Sender);
        void __fastcall FormPaint(TObject *Sender);
        void __fastcall FormMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
        void __fastcall FormMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
        void __fastcall FormMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
        void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall FormKeyPress(TObject *Sender, System::WideChar &Key);

private:	// User declarations
bool                     Fdiag_off;
get_text_method          Fget_text;
get_name_method          Fget_name;
get_record_method        Fget_record;

set_kadr_change_method   Fset_kadr_change;
on_select_entry_method   Fon_select_entry;
on_over_entry_method     Fon_over_entry;

WideString    __fastcall stub_get_text(DWORD rid)        ;
WideString    __fastcall stub_get_name(DWORD rid,bool )  ;
bool          __fastcall stub_get_record(DWORD rec_id,MDB_RECORD& sr){sr.rec_id = -1;return false;}



single_kadr     *   Fkadr_data;
tlmpaint::TBmpBkgnd bkgnd;
//HBITMAP             bkgnd_bmp_src; //Источник фона
//HBITMAP             bkgnd_bmp;     //Изображение адартированное для  отрисовки
HDC                 mem_dc;
BOOL                grayed_bkgnd;
DWORD               Fgrid_step;
BOOL                Fhide_bkgnd;
BOOL                show_grid;

//TGKModuleLoader     painter_dll;
tlmpaint::painter_dll * painter_dll;
DWORD flash_counter;
UnicodeString       Fbkgnd_image;
mdb_kadr_entry      * Fover_entry;
BOOL                rmb_down;
BOOL                lmb_down;
POINT               spot;
POINT               spot1;
BOOL                mouse_sel_mode;

bool                is_captured;
bool                select_on_down;
void     __fastcall set_capture(bool val) { if(is_captured!=val){is_captured = val; if(val) SetCapture(Handle);else ReleaseCapture();}}

 void         __fastcall set_diag_off(bool val);
 bool         __fastcall is_bkgnd_grayed();
 //bool         __fastcall do_setup_bkgnd();
 //bool         __fastcall do_setup_grid(LPRGBQUAD b_arr,DWORD width, DWORD height);
 //void         __fastcall do_gray_bkgnd(LPRGBQUAD b_arr,LPRGBQUAD e_arr);
 void         __fastcall release_bkgnd();

void      __fastcall prepare_paint_param(TLMOBJ_PAINT_PARAM &pp);
void      __fastcall paint_entry        (kadr_entryes_table::iterator ptr,TLMOBJ_PAINT_PARAM &pp);
void      __fastcall paint_entry        (kadr_entryes_table::iterator ptr,TLMOBJ_PAINT_PARAM &pp,BOOL erase);
void      __fastcall repaint_entry(kadr_entryes_table::iterator ptr);
void      __fastcall update_intersect(kadr_entryes_table::iterator ptr,bool before);

virtual void __fastcall CreateParams(Vcl::Controls::TCreateParams &Params);
void      __fastcall WndProc(Messages::TMessage &Message);
public:		// User declarations
         __fastcall TKadrPainter(TComponent* Owner);
void     __fastcall set_kadr_data(single_kadr * kd);
void     __fastcall set_get_text  (get_text_method   __gm);
void     __fastcall set_get_name  (get_name_method   __gm);
void     __fastcall set_get_record(get_record_method __gm);
LONG     __fastcall get_kadr_width();
void     __fastcall set_kadr_width(LONG );
LONG     __fastcall get_kadr_height();
void     __fastcall set_kadr_height(LONG);
void     __fastcall set_bkgnd_image( UnicodeString img);
kadr_entryes_table::iterator  __fastcall find_entry(int x,int y);

DWORD    __fastcall get_changes();
void     __fastcall set_changes(DWORD mask);

__property LONG    kadr_width        = {read = get_kadr_width ,write = set_kadr_width };
__property LONG    kadr_height       = {read = get_kadr_height,write = set_kadr_height};
__property single_kadr *            kadr_data         = {read = Fkadr_data    ,write = set_kadr_data    };
__property get_text_method          get_text          = {read = Fget_text,write = set_get_text};
__property get_name_method          get_name          = {read = Fget_name,write = set_get_name};
__property get_record_method        get_record        = {read = Fget_record,write = set_get_record};
__property set_kadr_change_method   set_kadr_change   = {read = Fset_kadr_change ,write = Fset_kadr_change};
__property UnicodeString            bkgnd_image       = {read = Fbkgnd_image,write = set_bkgnd_image};
__property on_select_entry_method   on_select_entry   = {read = Fon_select_entry, write = Fon_select_entry };
__property on_over_entry_method     on_over_entry     = {read = Fon_over_entry  , write = Fon_over_entry   };
__property bool                     captured          = {read = is_captured,write = set_capture};
__property DWORD                    changes           = {read = get_changes,write = set_changes};
__property mdb_kadr_entry *         over_entry        = {read = Fover_entry};
__property int                      selected_count    = {read = get_selected_count};
__property mdb_kadr_entry *         selected[int idx] = {read = get_selected};
__property kadr_entryes_table::iterator  mouse_entry[int x][int y] ={read = find_entry};
__property bool                     diag_off          = {read = Fdiag_off, write = set_diag_off};


mdb_kadr_entry * __fastcall get_selected(int idx) {return kadr_data->get_selected(0);}
int              __fastcall get_selected_count()  {return kadr_data->sel_count();    }

void   __fastcall on_kadr_change  (DWORD mask);
void   __fastcall on_record_change(DWORD mask,DWORD rec_id);

void   __fastcall resize_entry(mdb_kadr_entry & entry,SIZE & delta);
void   __fastcall move_entry  (mdb_kadr_entry & entry,SIZE & delta);
void   __fastcall resize_sel_entryes(SIZE & delta);
void   __fastcall move_sel_entryes  (SIZE & delta);
void   __fastcall set_sel_text_color(DWORD color);
void   __fastcall set_sel_bk_color  (DWORD color);
int    __fastcall set_sel_paint_var (DWORD paint_var,bool discrete = true);
void   __fastcall set_sel_prec_digits (int prec,int digits);
void   __fastcall arrange_sel_left();
void   __fastcall arrange_sel_top ();
void   __fastcall arrange_sel_size();
void   __fastcall arrange_sel_grid(bool horizontal);
void   __fastcall do_arrange_grid(mdb_kadr_entry * entry);
void   __fastcall reset_size();
void   __fastcall move_outsiding();
void   __fastcall select_all();
bool   __fastcall enum_paint_variant(bool discrete,int idx,UnicodeString & s);


void   __fastcall select_in_rect(RECT & r,bool add);


void   __fastcall paint_entry(DWORD idx,BOOL erase = FALSE);
bool   __fastcall update_size(DWORD idx);
void   __fastcall select_entry(DWORD offs,bool multi);

HDC __fastcall get_client_dc(){return Canvas->Handle;}
__property     HDC ClientDC = {read = get_client_dc};
DWORD      __fastcall get_bkgnd_mode();

bool __fastcall get_grid_visible();
void __fastcall set_grid_visible(bool v);
void __fastcall set_grid_step(DWORD v);
void __fastcall set_hide_bkgnd(BOOL v);

__property    bool   grid_visible = {read = get_grid_visible,write = set_grid_visible};
__property    DWORD  grid_step    = {read = Fgrid_step,write = set_grid_step};
__property    BOOL   hide_bkgnd   = {read = Fhide_bkgnd,write = set_hide_bkgnd};
DWORD __fastcall get_selected_recid();
void  __fastcall reload_bkgnd_image();
};
//---------------------------------------------------------------------------
extern PACKAGE TKadrPainter *KadrPainter;
//---------------------------------------------------------------------------



#endif
