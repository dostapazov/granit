//---------------------------------------------------------------------------

#pragma hdrstop

#include "ready_painter.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)


  void __fastcall  get_max_char_size(HDC dc,HFONT font,wchar_t * str_ptr,SIZE & sz)
  {
    sz.cx = sz.cy = 0;
    tlmpaint::TGdiSwap fs(dc,font);
    while(str_ptr && *str_ptr)
     {
       SIZE _sz;
       GetTextExtentPoint32W(dc,str_ptr,1,&_sz);
       sz.cx = std::max(sz.cx,+_sz.cx);
       sz.cy = std::max(sz.cy,+_sz.cy);
       ++str_ptr;
     }
  }

  void  __fastcall     ready_painter_t::reset_paint_objects()
  {

    bk_brush            = (HBRUSH)NULL;
    bk_stable_brush     = (HBRUSH)NULL;
    border_pen          = (HPEN)  NULL;
    border_pen_selected = (HPEN)  NULL;
  }

  HBRUSH        __fastcall ready_painter_t::get_bk_brush(bool stable)
  {
    tlmpaint::TGdiObject<HBRUSH> & brush = stable ? this->bk_stable_brush : bk_brush;
    if(!brush()) brush = CreateSolidBrush( stable ? this->ready_colors->bkgnd_stable : this->ready_colors->bkgnd);
    return brush();
  }

  HPEN          __fastcall ready_painter_t::get_border_pen(bool selected)
  {
    tlmpaint::TGdiObject<HPEN> & pen = selected ? border_pen_selected : border_pen;
    if(!pen()) pen  = CreatePen(PS_SOLID,2,selected ? ready_colors->border_selected : ready_colors->border);
    return pen();
  }

  void          __fastcall ready_painter_t::do_erase(RECT  & r,HBITMAP bmp)
  {

  }

  void __fastcall  ready_painter_t::ready_paint_border(HDC dc,const RECT & r,const bool stable,const bool selected)
  {

    tlmpaint::TGdiSwap brsw (dc,get_bk_brush(stable));
    tlmpaint::TGdiSwap pensw(dc,get_border_pen(selected));
    RoundRect(dc,r.left,r.top,r.right,r.bottom,10,10);
  }

  void __fastcall ready_painter_t::ready_paint_name  (HDC dc,const RECT & r,const UnicodeString text,const DWORD diag ,const bool flash_erase)
  {
    //Отрисовка имени
    if(!flash_erase )
    {
      COLORREF color =  diag ? ready_colors->name_bad : ready_colors->name_good;
      tlmpaint::TGdiSwap        fsw(dc,font);
      tlmpaint::TTextBkModeSwap bkm(dc,TRANSPARENT);
      tlmpaint::TTextColorSwap  tcs(dc,color);
      SIZE sz;
      GetTextExtentPoint32W(dc,text.c_str(),text.Length(),&sz);
      int x = r.left+(rect_width(r)-sz.cx)/2;
      int y = r.top+2;
      TextOutW(dc,x,y,text.c_str(),text.Length());
    }
  }

  wchar_t   ready_painter_t::state_chars[] = L" uanHMCy";

  void __fastcall ready_painter_t::ready_paint_state (HDC dc,const RECT & r,const DWORD ready_state ,const bool flash_erase)
  {
    //Отрисовка состояние
    if(!state_char_size.cx)  get_max_char_size(dc,state_font,state_chars,state_char_size);
    tlmpaint::TGdiSwap        fs(dc,state_font);
    tlmpaint::TTextBkModeSwap bk(dc,TRANSPARENT);
    int x = r.left + rect_width(r)  - 4 - 7*state_char_size.cx;
    int y = r.top  + rect_height(r) - 3 - state_char_size.cy;
    COLORREF  color = ready_colors->bkgnd;

    for(int i = 1;i<8;i++)
    {
      wchar_t out_ch = state_chars[0];
      switch (i)
      {
      case 1:   //История
          if( ready_state & rds_alarm_history) { out_ch = this->state_chars[1]; color = ready_colors->state_on;}
          break;
      case 2:   //ТИТ аварийный
          if( ready_state & rds_analog_alarm)  { out_ch = this->state_chars [2];color = ready_colors->state_alarm;}
          break;
      case 3:   //ТИТ предупредельный
          if( ready_state & rds_analog_danger) { out_ch = this->state_chars [3];color = ready_colors->state_danger;}
          break;
      case 4:   //Несоответствие отбражается есть наличие
          if( (ready_state & rds_notnormal ) && (!(ready_state & rds_notnormal_flashing) || !flash_erase))
            {
              out_ch = this->state_chars [4];
              if(ready_state&rds_notnormal_alarm) color = ready_colors->state_alarm; else
              {
                if(ready_state&rds_notnormal_danger) color = ready_colors->state_danger;else
                   color = (ready_state&rds_notnormal_on) ? ready_colors->state_on : ready_colors->state_off;
              }
            }
          break;
      case 5:  //Много
          if( (ready_state & rds_many ) && (!(ready_state & rds_many_flashing) || !flash_erase))
            {
              out_ch = this->state_chars [5];
              if(ready_state&rds_many_alarm) color = ready_colors->state_alarm; else
              {
                if(ready_state&rds_many_danger) color = ready_colors->state_danger;else
                   color = (ready_state&rds_many_on) ? ready_colors->state_on : ready_colors->state_off;
              }
            }
      break;
      case 6:  //Возврат в соответствие
          if( (ready_state & rds_backnormal ) && (!(ready_state & rds_backnormal_flashing) || !flash_erase))
            {
              out_ch = this->state_chars [6];
              if(ready_state&rds_backnormal_alarm) color = ready_colors->state_alarm; else
              {
                if(ready_state&rds_backnormal_danger) color = ready_colors->state_danger;else
                   color = (ready_state&rds_backnormal_on) ? ready_colors->state_on : ready_colors->state_off;
              }
            }
      break;
      case 7: //Ту-тр
           if((ready_state & rds_rc)  )
             {
               if(!(ready_state&rds_rc_active ) || !flash_erase )
                  {
                    out_ch = this->state_chars [7];
                    color = (ready_state & rds_rc) == rds_rc_error ? ready_colors->state_danger : ready_colors->tutr;
                  }
             }
      break;
      }

      if(out_ch != L' ')
      {
       tlmpaint::TTextColorSwap tc(dc,color);
       TextOutW(dc,x,y,&out_ch,1);
      }
      x+=state_char_size.cx;
    }
  }

  void __fastcall  ready_painter_t::ready_paint    (HDC dc,mdb_kadr_entry & entry,bool erase,bool flash_erase,HBITMAP bk_bmp)
  {
    single_kadr * skadr = this->get_kadr(entry.rec_id);
    RECT r;entry.get_rect(r);
    if(!erase && skadr)
    {
      ready_paint_border(dc,r,skadr->is_stable(),entry.is_selected());
      ready_paint_name  (dc,r,get_ready_text(*skadr),skadr->kadr.kadr_diag,(skadr->ready_state&rds_name_flashing)&&flash_erase);
      ready_paint_state (dc,r,skadr->ready_state,flash_erase);
    }
    else
    do_erase(r,bk_bmp);
  }

  MDB_GSIZE __fastcall  ready_painter_t::ready_calc_size(HDC dc,mdb_kadr_entry & entry)
  {
    // Вычисление размера кадра на строке готовности
    // Ширина + 2 пикселей
    // Высота имени кадра + высота состояния + 2
    MDB_GSIZE sz;
    SIZE      txtsz;
    single_kadr * skadr = this->get_kadr(entry.rec_id);
    if(!state_char_size.cx)
       get_max_char_size(dc,state_font,state_chars,state_char_size);

    tlmpaint::TGdiSwap font      (dc,this->font);
    UnicodeString  ready_str = this->get_ready_text(*skadr);
    GetTextExtentPoint32W(dc,ready_str.c_str(),ready_str.Length(),&txtsz);
    sz.cx = std::max(txtsz.cx,state_char_size.cx*7)+8;
    sz.cy = txtsz.cy + state_char_size.cy+6 ;
    return sz;
  }

  UnicodeString __fastcall     ready_painter_t::get_ready_text (single_kadr & skadr)
  {
      UnicodeString str;
      if(Fon_get_text)
         {
           str = Fon_get_text(skadr.kadr.sname_id);
           if(str.IsEmpty())
              str = Fon_get_text(skadr.kadr.name_id);
         }
      if(str.IsEmpty())
       str.printf(L"Kadr ID-%d",skadr.kadr.kadr_id);
      return str;
  }


