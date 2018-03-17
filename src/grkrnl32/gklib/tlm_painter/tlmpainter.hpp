#ifndef __TLM_PAINTER_HPP
#define __TLM_PAINTER_HPP
#include <gkwind.hpp>
#include <mediumdb.h>
#include <vector>
#include <algorithm>
#include <keclosure.hpp>
#include <string>
#include <gkmodules.hpp>
#include <tlm_painter.h>
#include <modem_proto.h>
#include <tlm_painter.hpp>




using namespace KeWin ;
using namespace KeRTL ;
using namespace std;

#pragma pack (push,1)

class mdb_record_ex:public mdb_record
{
 protected :
 WORD          normal_value ;//Начальное нормальное  состояние
 public:
 mdb_record_ex():mdb_record(){changes_count = 0;normal_value = 0;}
 mdb_record_ex(DWORD _rec_id):mdb_record(){ rec_id = _rec_id; changes_count = 0;normal_value = 0;}
 mdb_record_ex(const MDB_RECORD & r):mdb_record(r){changes_count = 0; normal_value = get_init_nv();}
 mdb_record_ex(const mdb_record_ex & rec):mdb_record(){*this = rec;};
 mdb_record_ex & operator = (const MDB_RECORD & rec);
 mdb_record_ex & operator = (mdb_record_ex & rec){memcpy(this,&rec,sizeof(*this));return *this;};

 WORD  __fastcall inc_changes();
 DWORD  __fastcall get_changes() const {return (!is_worked() || (state&MDBR_STATE_STATIC) ) ? 0 : changes_count;}
 DWORD  __fastcall get_normal_value() const {return normal_value;}
 bool   __fastcall is_normal_state() const;
 BOOL   __fastcall is_flashed();
 void   __fastcall receipt   ();
 void   __fastcall unreceipt ();
 void   __fastcall clean     ();
 void   __fastcall reset_changes(){changes_count = 0;}
 BOOL   __fastcall is_visible();
};

class mdb_kadr_ex:public mdb_kadr
{

 public:
 mdb_kadr_ex():mdb_kadr(){}
 mdb_kadr_ex(DWORD _kadr_id):mdb_kadr() {memset(this,0,sizeof(*this));kadr_id = _kadr_id;}
 mdb_kadr_ex(const MDB_KADR    & src):mdb_kadr(src) {}
 mdb_kadr_ex(const mdb_kadr_ex & src){*this = src;}
 mdb_kadr_ex & operator = (const mdb_kadr_ex & src){memcpy(this,&src,sizeof(*this)); return * this;}
};
#pragma pack(pop)


inline bool operator == (const mdb_kadr_ex & k1, const mdb_kadr_ex & k2)
{
  return k1.kadr_id == k2.kadr_id ? true : false;
}

inline bool operator < (const mdb_kadr_ex & k1, const mdb_kadr_ex & k2)
{
  return k1.kadr_id < k2.kadr_id ? true : false;
}


inline  mdb_record_ex & mdb_record_ex::operator = (const MDB_RECORD & rec)
{
 *((mdb_record*)this) = rec;
 changes_count = 0;
 normal_value = get_init_nv();
 return * this;
}

inline  bool operator == (const mdb_record_ex  & rec1,const mdb_record_ex  & rec2)
{
 return  rec1.rec_id == rec2.rec_id;
}

inline  bool operator < (const mdb_record_ex  & rec1,const mdb_record_ex  & rec2)
{
 return  rec1.rec_id < rec2.rec_id;
}

inline bool __fastcall mdb_record_ex::is_normal_state() const
 {
  if(is_worked() && is_discrete())
   {
    return ((DWORD)value == (DWORD) (normal_value)) ? true:false;
   }
   return true;
 }

inline  void   __fastcall mdb_record_ex::receipt()
{
 if(is_discrete() )
 {
  if(0==(state&MDBR_STATE_NORECEIPT) || is_normal_state())
    {
     if(!changes_count )
        {
         //Вторая фаза квитирования
          normal_value = (DWORD)value;
         if(normal_value) state |= MDBR_STATE_DYN_NORMAL;
            else
            state &= ~MDBR_STATE_DYN_NORMAL;
        }
    }
 }

 changes_count = 0;
 state &=~MDBR_STATE_FLASH;
}

inline  void   __fastcall mdb_record_ex::unreceipt()
{
 if(is_discrete() && !changes_count && is_normal_state())
 {
          normal_value = !(DWORD)value;
         if(normal_value) state |= MDBR_STATE_DYN_NORMAL;
            else
            state &= ~MDBR_STATE_DYN_NORMAL;
 }
}


inline  void   __fastcall mdb_record_ex::clean()
{
 if(is_discrete() )
 {
  //if(0==(state&MDBR_STATE_NORECEIPT) )
    {
     changes_count = 0 ;
     state &=~MDBR_STATE_FLASH;
    }
 }
}

inline  BOOL   __fastcall mdb_record_ex::is_visible()
{
 if(is_discrete())
 {
  if((state&MDBR_STATE_HIDDEN) && changes_count == 0 && is_normal_state() && diag == MDBR_DIAG_GOOD)
      return  FALSE;
 }
 return TRUE;
}

inline WORD  __fastcall mdb_record_ex::inc_changes()
{
 if(this->is_worked())
 {
  if(!(++changes_count))
       changes_count = 2;
  if(is_discrete())
  {

   //if(((changes_count&1) && is_normal_state()) || (!(changes_count&1) && !is_normal_state()))
   //     ++changes_count;
   if(!(state&MDBR_STATE_STATIC))
         state|=MDBR_STATE_FLASH;
  }
 }
 else
  changes_count = 0;
  return changes_count;
}

inline  BOOL   __fastcall mdb_record_ex::is_flashed()
{
 return is_worked() && ( state&(MDBR_STATE_FLASH|MDBR_STATE_TUTR_ACTIVE)) ? TRUE:FALSE;
}



/*Класс отрисовки объектов*/
class   TTlmPainter;

typedef TClosureMethod<LPVOID,TTlmPainter*,DWORD  ,wstring>          getter_text;
typedef TClosureMethod<LPVOID,TTlmPainter*,mdb_record_ex * ,DWORD>   getter_record;
typedef TClosureMethod<LPVOID,TTlmPainter*,DWORD  ,DWORD>            entry_change_notify;

#define  MOUSE_BUTTON_LEFT   1
#define  MOUSE_BUTTON_RIGHT  2
#define  MOUSE_BUTTON_MIDDLE 4

struct   Tmouse_data
{
 DWORD mbuttons;
 DWORD keys;
 POINT pt;
 bool  down;
};



typedef  TClosureMethod<void *,TTlmPainter*,Tmouse_data*,BOOL > Tmouse_closure;

struct   Tkeyboard_data
{
 DWORD    v_key;
 DWORD    key_data;
};

typedef  TClosureMethod<void *,TTlmPainter*,Tkeyboard_data*,BOOL > Tkeyboard_closure;




typedef vector<mdb_kadr_entry>  Tentryes_list;
typedef vector<DWORD>           Tselected_list;


/*Как выравнивать*/
enum   how_arrange {arr_left,
                    arr_top,
                    arr_right,
                    arr_bottom,
                    arr_size_width,
                    arr_size_height,
                    arr_size_both
                    };




class TTlmPainter:public TWindow
{
 protected:

 BOOL            editable; //Признак того, что редактируемый
 BOOL            show_grid; //Показывать сетку
 LONG            grid_step; //Шаг сетки;
 BOOL            hide_bkgnd;//Скрыть фоновое изображение;
 BOOL            Fshow_hidden;

 wstring          paint_dll_name;
 wstring          bkgnd_image;
 tlmpaint::TBmpBkgnd bkgnd;
 //HBITMAP         bkgnd_bmp_src; //Источник фона
 //HBITMAP         bkgnd_bmp;     //Изображение адартированное для  отрисовки
                                //Фона
 HDC             mem_dc;

 Tmouse_closure     on_mouse_buttons_callback;
 Tmouse_closure     on_mouse_move_callback;
 Tkeyboard_closure  on_keydown_closure;

 DWORD           mouse_over;     //Смещение в списке над которым проходит курсор
 DWORD           mouse_buttons;  // Нажатые кнопки
 POINT           spot_pt;        // смещение от верхнего левого угла
 POINT           spot_pt1;
 BOOL            on_down_select;// признак выбора по нажатию кнопки
 BOOL            captured;      // признак захвата мыши


 mdb_kadr*       kadr; // Указатель на кадр
 Tentryes_list   ent_list;//Список элементов
 Tselected_list  sel_list;//Список индексов выделенных
 DWORD           sel_leader;

 getter_text         _get_text;
 getter_record       _get_record;
 entry_change_notify _on_select_notify;
 entry_change_notify _on_mouse_over;
 entry_change_notify _on_moving_sizing;



 LRESULT      __fastcall MessageHandler(MSG &);
 void         __fastcall GetWndClass(WNDCLASSEX & );
 bool         __fastcall OnWMCreate (LPCREATESTRUCT);
 void         __fastcall OnSize(int how,int cx,int cy);
 void         __fastcall OnWMDestroy();

 void         __fastcall on_keydown(DWORD v_key,DWORD key_data );
 void         __fastcall on_lmouse(POINT & pt,bool down,DWORD keys);
 void         __fastcall on_rmouse(POINT & pt,bool down,DWORD keys);
 void         __fastcall on_mouse_move(POINT & pt,DWORD keys);
 void         __fastcall paint_select_rect(HDC dc,LPPOINT  pt);


 int          __fastcall EraseBkgnd(HDC dc);
 DWORD        __fastcall GetStyle(bool Ex = false);
 HBRUSH       __fastcall OnCtlColor(HWND wnd,HDC dc,UINT CtrlType);
 bool         __fastcall is_bkgnd_grayed();
 void         __fastcall release_bkgnd();

 int          __fastcall Paint(HDC dc,RECT & r,bool);

 void         __fastcall paint_entry  (HDC dc,Tentryes_list::iterator ptr,bool sel,bool erase,bool flash = false);
 void         __fastcall paint_entry  (HDC dc,DWORD idx,bool erase = false);
 void         __fastcall paint_entryes(HDC dc,RECT & r,bool erase = false);
 bool         __fastcall is_selected  (Tentryes_list::iterator ptr);
 bool         __fastcall select_entry (DWORD offs,bool added_select);
 void         __fastcall select_in_rect(RECT &r,bool add);


 virtual
 void         __fastcall on_mouse_over_entry(DWORD offs);
 DWORD        __fastcall set_mouse_over_entry(DWORD offs);

 void         __fastcall move_size_entry(Tentryes_list::iterator ptr,int dx,int dy,bool sizing);
 void         __fastcall move_size_entry(DWORD offset,int dx,int dy,bool sizing);

 void         __fastcall calc_min_entry_size(Tentryes_list::iterator ptr,KeWin::TSize & min_size);
 void         __fastcall arrange_left  (mdb_kadr_entry & leader,mdb_kadr_entry & entry);
 void         __fastcall arrange_top   (mdb_kadr_entry & leader,mdb_kadr_entry & entry);
 void         __fastcall arrange_right (mdb_kadr_entry & leader,mdb_kadr_entry & entry);
 void         __fastcall arrange_bottom(mdb_kadr_entry & leader,mdb_kadr_entry & entry);
 void         __fastcall arrange_size  (mdb_kadr_entry & leader,mdb_kadr_entry & entry,how_arrange ha);
 const char * __fastcall GetClassName();
 public:
 TTlmPainter(mdb_kadr * _kadr):editable(FALSE),
                              Fshow_hidden(FALSE),
//                              bkgnd_bmp_src(NULL),
//                              bkgnd_bmp(NULL),
                              kadr(_kadr),
                              not_gray_bkgnd(FALSE)
 {};

 void   __fastcall Create
    ( HWND Parent,const char * Caption,RECT & r,int Id = -1,
      HMENU Menu = 0,DWORD Style=0,DWORD ExStyle=0,const char * ClassName=0);
 bool         __fastcall set_bkgnd(const wstring & img_name,bool force = false);
 DWORD        __fastcall set_bkgnd_mode(DWORD mode);
 mdb_kadr *   __fastcall set_kadr(mdb_kadr * kadr,DWORD ch_mask = -1);
 DWORD        __fastcall get_bkgnd_mode();
 void         __fastcall set_getter_text     (getter_text::closure_method,LPVOID user_data);
 void         __fastcall set_getter_record   (getter_record::closure_method,LPVOID user_data);
 void         __fastcall set_on_select       (entry_change_notify::closure_method,LPVOID user_data);
 void         __fastcall set_on_moving_sizing(entry_change_notify::closure_method,LPVOID user_data);
 void         __fastcall set_on_mouse_over   (entry_change_notify::closure_method,LPVOID user_data);

 bool         __fastcall do_setup_bkgnd();

 DWORD        __fastcall add_entry         (mdb_kadr_entry & e);
 DWORD        __fastcall set_entry         (DWORD idx,mdb_kadr_entry & e);
 DWORD        __fastcall set_entryes_count (DWORD count);
 DWORD        __fastcall remove_entry(mdb_kadr_entry & e);
 DWORD        __fastcall update_entry(mdb_kadr_entry & e);
 void         __fastcall move_size_selected(int dx,int dy,bool sizing);
 void         __fastcall arrange_selected(how_arrange ha);
 void         __fastcall enable_edit(bool enable);
 void         __fastcall enable_show_grid(bool show);
 void         __fastcall set_grid_step(DWORD step);
 DWORD        __fastcall get_grid_step();
 bool         __fastcall is_edit_enable(){return this->editable ? true:false;}
 bool         __fastcall is_selected  (DWORD offs);
 DWORD        __fastcall get_selected_count();
 DWORD        __fastcall get_selected();
 DWORD        __fastcall get_selected(Tselected_list & sel_list);
 void         __fastcall set_selected(Tselected_list & sel_list,bool repaint = true);
 bool         __fastcall get_kadr_entry(DWORD offs,mdb_kadr_entry &);
 void         __fastcall clear_entryes();
 void         __fastcall get_record(DWORD rec_id,mdb_record_ex & rec);
 wstring      __fastcall get_wtext(DWORD id);
 DWORD        __fastcall get_entry_count(){return ent_list.size();}
 void         __fastcall flash_repaint();
 DWORD        __fastcall get_selected_recid(DWORD num);
 DWORD        __fastcall get_selected_recid(Tselected_list & lst);

 void         __fastcall release_bkgnd_bitmaps();
 void         __fastcall restore_bkgnd_bitmaps();
 void         __fastcall set_mouse_buttons_callback  (Tmouse_closure::closure_method cm,LPVOID user_data);
 void         __fastcall set_mouse_move_callback     (Tmouse_closure::closure_method cm,LPVOID user_data);
 void         __fastcall set_keydown_closure(Tkeyboard_closure::closure_method cm,LPVOID user_data);
 void         __fastcall select_all  (bool repaint = true);
 void         __fastcall select_clean  (bool repaint = true);
 void         __fastcall select_discrete(bool repaint = true);
 void         __fastcall select_analogs (bool repaint = true);
 void         __fastcall select_next(bool next);

 DWORD        __fastcall get_entry_rec_id  (DWORD entry_num);

 DWORD        __fastcall find_entry(POINT & pt);
 DWORD        __fastcall find_entry(DWORD x_coord,DWORD y_coord);
 void         __fastcall calc_min_entry_size(mdb_kadr_entry &e ,KeWin::TSize & min_size);
 void         __fastcall SetFlags(DWORD32 flags, bool Set);
 bool         __fastcall lower_entry(DWORD rec_id,DWORD number, Tentryes_list::iterator & ptr);
 bool         __fastcall lower_entry(DWORD rec_id, Tentryes_list::iterator & ptr) {return lower_entry(rec_id,0,ptr);}
 DWORD        __fastcall range_entry(DWORD rec_id,Tentryes_list::iterator & lo_ptr,Tentryes_list::iterator & hi_ptr);
 void         __fastcall repaint_record(DWORD rec_id);
 void         __fastcall show_hidden(BOOL show);
 void         __fastcall reset_mbuttons(){this->mouse_buttons = 0; mouse_over = -1;}

 static       TGKModuleLoader painter_dll;
 static       DWORD           flash_counter;
 void         __fastcall set_not_gray_bkgnd(BOOL ngray);
 BOOL         __fastcall get_not_gray_bkgnd(){return not_gray_bkgnd;}
 BOOL         __fastcall get_show_hidden(){return Fshow_hidden;}
 protected:
              BOOL            not_gray_bkgnd;



};

inline  DWORD        __fastcall TTlmPainter::find_entry(DWORD x_coord,DWORD y_coord)
{
 KeWin::TPoint pt(x_coord,y_coord);
 return find_entry(pt);
}


 inline  bool         __fastcall TTlmPainter::is_selected(Tentryes_list::iterator ptr)
 {
  DWORD offs = std::distance(ent_list.begin(),ptr);
  //(DWORD(ptr)-DWORD(ent_list.begin()))/sizeof(*ptr);
  return is_selected(offs);
 }


 inline void         __fastcall TTlmPainter::set_on_mouse_over   (entry_change_notify::closure_method cm,LPVOID user_data)
 {
   _on_mouse_over.SetClosure(user_data,cm,this,NULL);
 }

 inline void         __fastcall TTlmPainter::on_mouse_over_entry(DWORD offs)
 {
  _on_mouse_over(this,offs);
 }

 inline DWORD        __fastcall TTlmPainter::set_mouse_over_entry(DWORD offs)
 {
  DWORD old_offs = mouse_over;
  if(mouse_over!=offs)
  {
    on_mouse_over_entry(offs);
    mouse_over = offs;
  }
  return old_offs;
 }

 inline  void         __fastcall TTlmPainter::move_size_entry(DWORD offset,int dx,int dy,bool sizing)
 {
  if(offset<ent_list.size())
    {
     Tentryes_list::iterator ptr = ent_list.begin();
     advance(ptr,offset);
     move_size_entry(ptr,dx,dy,sizing);
    }
 }

inline   void         __fastcall TTlmPainter::enable_show_grid(bool show)
{
 BOOL sg = show ? TRUE:FALSE;
 if(show_grid!=sg)
    {
     show_grid = sg;
     bkgnd.set_grid(show_grid ? this->grid_step : 0);
     Repaint(true);
    }

}

inline   void         __fastcall TTlmPainter::set_grid_step(DWORD step)
{
 if(grid_step != (LONG)step)
 {
    if(step)
      {
       grid_step = step;
       if(show_grid)
       {
        bkgnd.set_grid(grid_step);
        Repaint();
       }
      }
    else
    enable_show_grid(false);

 }
}

inline   DWORD        __fastcall TTlmPainter::get_grid_step()
{
 return grid_step;
}


inline   void         __fastcall TTlmPainter::set_on_select
                      (entry_change_notify::closure_method cm,LPVOID user_data)
{
 _on_select_notify.SetClosure(user_data,cm,this,NULL);
}

inline   void         __fastcall TTlmPainter::set_on_moving_sizing
                        (entry_change_notify::closure_method cm,LPVOID user_data)
{
 _on_moving_sizing.SetClosure(user_data,cm,this,NULL);

}

inline  DWORD        __fastcall TTlmPainter::get_selected_count()
{
 return this->sel_list.size();
}

#endif
