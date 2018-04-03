#pragma hdrstop
#ifndef _TLMOBJ_PAINTER_INTERNAL
#define _TLMOBJ_PAINTER_INTERNAL
#endif
#include "tlm_painter.hpp"

  enum discrete_index_t  { di_normal = 0,di_one = 1,di_back_normal = 2, di_many = 3,di_many_back_normal=4,di_not_normal = 5,di_workout = 6 ,di_tutr = 7,di_bad_diag = 8};

  int   __fastcall discrete_painter_t::get_index(const LPMDB_RECORD  sr)
  {
    //Определение индекса по состоянию дисретного сигнала
   int image_idx = 0;
   if(sr->diag)
    image_idx = di_bad_diag;
    else
    {
     if(is_record_workout(sr))
        image_idx = di_workout;
     else
     {
      if(!is_record_normal(sr))  image_idx=di_not_normal;
      if(!(sr->state&MDBR_STATE_STATIC))
      {
       DWORD changes_count = record_changes_count(sr);
       if(changes_count)
       {
       if(!is_record_normal(sr))
            image_idx = (changes_count > MDBR_CH_COUNT_MANY) ? di_many : di_one;
            else
            image_idx = (changes_count > MDBR_CH_COUNT_MANY) ? di_many_back_normal : di_back_normal;
       }
      }
     }
    }
    if(sr->state & (MDBR_STATE_TUTR_ACTIVE|MDBR_STATE_TUTR_PREPARE))
      image_idx = di_tutr;
    return image_idx;
  }


DWORD __fastcall get_lamp_index(LPMDB_RECORD sr)
{
    //Определение индекса по состоянию дисретного сигнала
  int image_idx = 0;
  if(sr->diag)
    image_idx = di_bad_diag;
    else
    {
     if(is_record_workout(sr))
        image_idx = di_workout;
     else
     {
       if(sr->changes_count)
       {
       if(sr->changes_count>2)
         image_idx=di_many;
         else
         image_idx=1;
       if(!is_record_normal(sr))
            image_idx = (sr->changes_count > 2) ? di_many : di_one;
            else
            image_idx = (sr->changes_count > 2) ? di_many_back_normal : di_back_normal;
       }
       else
       if(!is_record_normal(sr))
           image_idx=di_not_normal;
     }
    }
    if(sr->state & (MDBR_STATE_TUTR_ACTIVE|MDBR_STATE_TUTR_PREPARE))
      image_idx = di_tutr;
  return image_idx;
}




void __fastcall  do_colored_dibits( LPDWORD array,DWORD dw_count,COLORREF on_white, COLORREF  on_black,bool handle_black)
{
        const COLORREF white = RGB(255,255,255);
        const COLORREF black = RGB(0,0,0);
        COLORREF _on_white = RGB(GetBValue(on_white),GetGValue(on_white),GetRValue(on_white));
        COLORREF _on_black = RGB(GetBValue(on_black),GetGValue(on_black),GetRValue(on_black));
        LPDWORD  e_arr = array+dw_count;
        while(array<e_arr)
        {
          switch(*array)
          {
           case white:    *array = _on_white;break;
           case black:    if(handle_black) *array = _on_black;break;
          }
          ++array;
        }
}

void __fastcall  do_change_white_colors( LPDWORD array,DWORD dw_count,COLORREF on_white)
{
        const COLORREF white = RGB(255,255,255);
        const COLORREF black = RGB(0,0,0);
        COLORREF _on_white = RGB(GetBValue(on_white),GetGValue(on_white),GetRValue(on_white));

        LPDWORD  e_arr = array+dw_count;
        while(array<e_arr)
        {
          if(*array == white) *array = _on_white;
          ++array;
        }
}


void __fastcall  do_change_line_colors( LPDWORD array,DWORD dw_count,COLORREF on_white,COLORREF on_black)
{
        const COLORREF white = RGB(255,255,255);
        const COLORREF black = RGB(0,0,0);
        COLORREF white_replace = RGB(GetBValue(on_white),GetGValue(on_white),GetRValue(on_white));
        COLORREF black_replace = RGB(GetBValue(on_black),GetGValue(on_black),GetRValue(on_black));
        LPDWORD  b_arr = array;
        LPDWORD  e_arr = array+dw_count;
        LPDWORD  m_arr = array+(dw_count>>1);
        bool in_white_from_start = false;
        bool in_white_from_end   = false;

        while(b_arr<m_arr)
        {
          --e_arr;
          if(*b_arr == white)
            { *b_arr = white_replace; in_white_from_start = true;}
            else
            {
            if(in_white_from_start && *b_arr == black)
              *b_arr = black_replace;
            }

          if(*e_arr == white)
            { *e_arr = white_replace; in_white_from_end   = true;}
            else
            {
            if(in_white_from_end   && *e_arr == black)
               *e_arr = black_replace;
            }
          ++b_arr;
        }
       if((dw_count&1) && *b_arr == white )
       {
          *b_arr = white_replace;
       }
}




//  void            discrete_painter_t::colored_temp_bmp(const HDC dc,const Tpaint_variant * pvar,COLORREF on_white,COLORREF on_black)
//  {
//      int line_count = get_temp_bmp_bits(dc,pvar);
//      if(line_count)
//      {
//         //do_colored_dibits((LPDWORD)pvar->temp_bmp_rgb,pvar->temp_bmp_info.bmiHeader.biSizeImage/sizeof(DWORD),on_white,on_black,pvar->multi_position ? false : true);
//         do_colored_dibits((LPDWORD)pvar->temp_bmp_rgb,pvar->temp_bmp_info.bmiHeader.biSizeImage/sizeof(DWORD),on_white,on_black,false);
//         set_temp_bmp_bits(dc,pvar);
//      }
//  }

  void            discrete_painter_t::colored_temp_bmp(const HDC dc,const Tpaint_variant * pvar,COLORREF on_white,COLORREF on_black)
  {
      int line_count = get_temp_bmp_bits(dc,pvar);
      if(line_count)
      {
         LPDWORD ptr         = (LPDWORD)pvar->temp_bmp_rgb;
         DWORD pix_count     = pvar->temp_bmp_info.bmiHeader.biSizeImage/sizeof(DWORD);
         if(pvar->multi_position >1)
            do_colored_dibits( ptr,pix_count,on_white,on_black,false);
            else
            {
             DWORD pix_per_line = pix_count/line_count;
             for(int i = 0;i<line_count;i++)
                {
                 //do_colored_dibits( ptr,pix_per_line,on_white,on_black,true);
                 do_change_line_colors(ptr,pix_per_line,on_white,on_black);
                 ptr+=pix_per_line;
                }
            }
         set_temp_bmp_bits(dc,pvar);
      }
  }



  void  __fastcall discrete_painter_t::paint    (const LPTLMOBJ_PAINT_PARAM1  pparam,const Tpaint_variant * pvar)
  {
       COLORREF color    = ((int)pparam->record.value) ? pparam->entry.options.color : pparam->entry.options.bkgnd_color;
       COLORREF bk_color = ((int)pparam->record.value) ? pparam->entry.options.bkgnd_color : pparam->entry.options.color;
       if(pparam->entry.options.line_width)
           paint_line(pparam,color);
        else
        {
         erase(pparam,pvar);
         RECT dest_rect,src_rect;
         obj_painter_t::get_rect(dest_rect,pparam);
         set_rect(src_rect,0,0,pvar->sz.cx,pvar->sz.cy);
         centering_rects(dest_rect,src_rect);
           if(!pvar->elements_count)
               {
                 //name_paint(pparam,color,&dest_rect );
                 const wchar_t * name = pparam->short_name;
                 wchar_t result[16];
                 if(!name || !*name)
                    {
                     get_value_text(pparam,false,result,16,NULL);
                     pparam->short_name = result;
                    }

                 name_paint(pparam,color,NULL );
                 return;
               }


          DWORD img_idx = get_index(&pparam->record);
          DWORD multipos = pvar->multi_position;
          DWORD value    = abs((int)pparam->record.value);
          if(multipos && img_idx >= (DWORD(pvar->elements_count/multipos)))
             img_idx = 0;
          if(multipos>1)
          {
           img_idx *=multipos;
           img_idx+=KeRTL::MIN(multipos-1,value);
          }
          LONG  x_offset = img_idx*(pvar->sz.cx);
          //Смещение в картинке источнике
           init_temp_bmp(pvar,pvar->sz.cx,pvar->sz.cy);
           tlmpaint::TGdiSwap mem_dc (pvar->get_mem_dc(pparam->paint_dc),pvar->temp_bmp);

           {
            tlmpaint::TGdiSwap tmp_dc (pvar->get_temp_dc(pparam->paint_dc),pvar->image);
            BitBlt (mem_dc(),0,0,pvar->sz.cx,pvar->sz.cy,tmp_dc(),x_offset,0,SRCCOPY);
           }
           //Залили соответствующим цветом временный битмап
           colored_temp_bmp(pvar->get_temp_dc(pparam->paint_dc),pvar,color , bk_color);

           tlmpaint::TGdiSwap tmp_dc (pvar->get_temp_dc(pparam->paint_dc),pvar->mask);

           //Пробили маску
           BitBlt(pparam->paint_dc,dest_rect.left,dest_rect.top,rect_width(dest_rect),rect_height(dest_rect),
                  tmp_dc(),src_rect.left+x_offset,src_rect.top,SRCAND
                 );

           //Вывели изображение из tmp_bmp;
           BitBlt(pparam->paint_dc,dest_rect.left,dest_rect.top,rect_width(dest_rect),rect_height(dest_rect),
                  mem_dc(),src_rect.left,src_rect.top,SRCPAINT
                 );

        }
       if(pparam->record.diag && !pparam->entry.options.part_of_background)
          paint_question_sign(pparam);
  }

void __fastcall discrete_painter_t::calc_size (const LPTLMOBJ_PAINT_PARAM1  pparam, const Tpaint_variant * pvar,LPSIZE sz)
{
   if(pvar)
      {
       if(pvar->elements_count)
       {
        sz->cx = pvar->sz.cx ;
        sz->cy = pvar->sz.cy ;
       }
       else
        {
         const wchar_t * name = pparam->short_name;
         wchar_t result[16];
         if(!name || !*name)
            {
             get_value_text(pparam,false,result,16,NULL);
             pparam->short_name = result;
            }
         name_size(pparam,*sz);
        }
       sz->cx+= 2+pvar->frame_pen_width*2;
       sz->cy+= 2+pvar->frame_pen_width*2;
      }
}


