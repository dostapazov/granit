//---------------------------------------------------------------------------

#ifndef ready_painterH
#define ready_painterH
//---------------------------------------------------------------------------
#include <tlm_painter.hpp>
#include <System.hpp>


class ready_painter_t
{
public:
typedef single_kadr *  __fastcall (__closure * get_kadr_callback)(DWORD kadr_id);
typedef UnicodeString  __fastcall (__closure * get_text_callback)(DWORD str_id );

private:
  static wchar_t   state_chars[];
  HFONT            font;
  HFONT            state_font;
  READY_COLORS *   ready_colors;
  SIZE             state_char_size;


  tlmpaint::TGdiObject<HBRUSH> bk_brush;
  tlmpaint::TGdiObject<HBRUSH> bk_stable_brush;
  tlmpaint::TGdiObject<HPEN>   border_pen;
  tlmpaint::TGdiObject<HPEN>   border_pen_selected;
  get_kadr_callback            Fon_get_kadr;
  get_text_callback            Fon_get_text;


  void          __fastcall do_erase      (RECT & r,HBITMAP bk_bmp);
  HBRUSH        __fastcall get_bk_brush  (bool stable);
  HPEN          __fastcall get_border_pen(bool selected);

  void          __fastcall ready_paint_border(HDC dc,const RECT & r,const bool stable,const bool selected);
  void          __fastcall ready_paint_name  (HDC dc,const RECT & r,const UnicodeString text,const DWORD diag,const bool flash_erase);
  void          __fastcall ready_paint_state (HDC dc,const RECT & r,const DWORD ready_state ,const bool flash_erase);

  ready_painter_t(){Fon_get_text = NULL; Fon_get_kadr = NULL;}
  public:
  ready_painter_t  (HFONT _font, HFONT _state_font,READY_COLORS * _rdy_clrs)
                   :font(_font),state_font(_state_font),ready_colors(_rdy_clrs)
  {   state_char_size.cx = state_char_size.cy = 0;Fon_get_text = NULL; Fon_get_kadr = NULL;  }
  ready_painter_t  (const ready_painter_t  & src){(*this) = src;}
  ready_painter_t  & operator = (const ready_painter_t  & src);
 ~ready_painter_t  (){}

  void          __fastcall     ready_paint    (HDC dc,mdb_kadr_entry & entry,bool erase,bool flash_erase,HBITMAP bk_bmp);
  MDB_GSIZE     __fastcall     ready_calc_size(HDC dc,mdb_kadr_entry & entry);
  UnicodeString __fastcall     get_ready_text (single_kadr & skadr);
  single_kadr * __fastcall     get_kadr       (DWORD kadr_id){ return (Fon_get_kadr) ? on_get_kadr(kadr_id) : NULL; };
          void  __fastcall     reset_paint_objects();
  __property get_kadr_callback  on_get_kadr = {read =Fon_get_kadr , write =Fon_get_kadr };
  __property get_text_callback  on_get_text = {read =Fon_get_text , write =Fon_get_text };
};

inline  ready_painter_t  & ready_painter_t::operator = (const ready_painter_t  & src)
  {
   font = src.font;state_font = src.state_font;ready_colors = src.ready_colors;
   state_char_size   = src.state_char_size;
   this->on_get_kadr = src.on_get_kadr;
   this->on_get_text = src.on_get_text;
   return *this;
  }

#endif
