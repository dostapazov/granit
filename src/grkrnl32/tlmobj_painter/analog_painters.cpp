#pragma hdrstop
#ifndef _TLMOBJ_PAINTER_INTERNAL
#define _TLMOBJ_PAINTER_INTERNAL
#endif
#include "tlm_painter.hpp"

void  __fastcall analog_text_painter_t::calc_size     (const LPTLMOBJ_PAINT_PARAM1  pparam, const Tpaint_variant * pvar,LPSIZE sz)
{
     wchar_t text[MAX_PATH];
     int len = get_value_text(pparam,pparam->entry.options.digits >1 ? true : false,text,KERTL_ARRAY_COUNT(text));
     GetTextExtentPoint32W(pparam->paint_dc,text,len,sz);
     if(!simple_text) sz->cx+=4,sz->cy += 4;
}



void __fastcall analog_text_painter_t::paint(const LPTLMOBJ_PAINT_PARAM1  pparam,const Tpaint_variant * pvar)
{
    int       old_bk_mode = SetBkMode ( pparam->paint_dc,OPAQUE);
    COLORREF  bk_color = pparam->entry.options.bkgnd_color;
    COLORREF  text_color   = pparam->entry.options.color;
     if(pparam->record.state&MDBR_STATE_DANGER)
        text_color = danger_color;
     if(pparam->record.state&MDBR_STATE_ALARM)
        text_color = alarm_color;

     if(text_color   ==  bk_color)
        bk_color=0xFFFFFF&(~bk_color);
      int   bk_text_mode = TRANSPARENT;


     wchar_t result[MAX_PATH];
     int len = get_value_text(pparam,pparam->entry.options.digits>1 ? true : false,result,KERTL_ARRAY_COUNT(result));
     RECT r;
     get_rect(r,pparam);
    if(!simple_text)
      {
        tlmpaint::TGdiObject<HBRUSH> br(CreateSolidBrush(bk_color));
        FillRect (pparam->paint_dc,&r,br());
      }
      else
      bk_text_mode = OPAQUE;

      bk_color   = SetBkColor  ( pparam->paint_dc, bk_color  );
      text_color = SetTextColor( pparam->paint_dc, text_color);

       RECT tr;
       SIZE sz;
       tr = r;
       GetTextExtentPoint32W(pparam->paint_dc,result,len,&sz);
       set_rect(r,tr.left,tr.top,sz.cx,sz.cy);
       centering_rects(r,tr);
       r = tr;

       bk_text_mode = SetBkMode(pparam->paint_dc,bk_text_mode);
       ExtTextOutW(pparam->paint_dc,r.left,r.top,0,&r,result,len,NULL);
       SetBkMode(pparam->paint_dc,bk_text_mode);
    SetTextColor(pparam->paint_dc, text_color );
    SetTextColor(pparam->paint_dc, bk_color   );
    SetBkMode   (pparam->paint_dc, old_bk_mode);
    if(pparam->record.diag)
        paint_question_sign(pparam);
}

/*********************** Отрисовка баров ****************************************/

void  __fastcall analog_bar_painter_t::split_rect    (bool vertical,int split_line,RECT & r1,RECT & r2)
{
 r2 = r1;
 if(vertical)
 {
  grow_rect(r1,0,-split_line);
  r2.top    = r1.bottom;
 }
 else
 {
  split_line = r1.right - r1.left - split_line;
  grow_rect (r2,-split_line,0);
  r1.left   = r2.right;
 }
}

void  __fastcall analog_bar_painter_t::calc_bar_param(bool vertical,RECT & r,MDB_RECORD & rec
                                                      ,int & pixels
                                                      ,int & val_pos
                                                      ,int & min_danger_pos
                                                      ,int & max_danger_pos
                                                      ,int & min_alarm_pos
                                                      ,int & max_alarm_pos
                                                      ,int & zero_pos)
{
 pixels         = vertical ? (r.bottom - r.top) : (r.right - r.left);
 float k        = fabs(rec.max_value - rec.min_value);
 if(fabs(k)>.00001)k = (float)pixels/k;
 else
 k = 1;
 val_pos        = k*fabs(rec.value-rec.min_value);

 min_danger_pos = k*fabs(rec.min_danger - rec.min_value);
 max_danger_pos = k*fabs(rec.max_danger - rec.min_value);

 min_alarm_pos  = k*fabs(rec.min_alarm  - rec.min_value);
 max_alarm_pos  = k*fabs(rec.max_alarm  - rec.min_value);

 if(rec.max_value*rec.min_value < .0)
    zero_pos = k*(fabs(rec.min_value));
 else
    zero_pos = 0;
}

void  __fastcall analog_bar_painter_t::get_rect(RECT & r,const LPTLMOBJ_PAINT_PARAM1  pparam)
{
       obj_painter_t::get_rect(r,pparam);
       grow_rect(r,-2,-2);
       move_rect(r,1,1);
}

void __fastcall analog_bar_painter_t::calc_size (const LPTLMOBJ_PAINT_PARAM1  pparam, const Tpaint_variant * pvar,LPSIZE sz)
{
  analog_text_painter_t::calc_size(pparam,pvar,sz);
  if(vertical)
    {
      sz->cx+=16;
      sz->cy*=4;
    }
    else
    {
      sz->cy+=16;
      sz->cx*=2;
    }

}

void __fastcall analog_bar_painter_t::paint(const LPTLMOBJ_PAINT_PARAM1  pparam,const Tpaint_variant * pvar)
{
       int pixels,val_pos,min_dpos,max_dpos,min_apos,max_apos,zero_point;
       RECT r;
       get_rect(r,pparam);

       calc_bar_param(vertical,r,pparam->record,pixels,val_pos,min_dpos,max_dpos,min_apos,max_apos,zero_point);
       COLORREF bk_color = pparam->entry.options.bkgnd_color;
       COLORREF    color = pparam->entry.options.color;
       if(color == bk_color) bk_color = 0xFFFFFFFF&(~bk_color);
       RECT    vr,dr,ar;

       split_rect(vertical,val_pos,r,vr);
       dr = vr;
       ar = vr;

       HBRUSH  danger_brush  = NULL;
       HBRUSH  alarm_brush   = NULL;

       COLORREF alarm_color  = obj_painter_t::alarm_color;
       COLORREF danger_color = obj_painter_t::danger_color;


       if(pparam->record.state&MDBR_STATE_ALARM)
       {
        if(max_apos<val_pos)
        {
         alarm_brush = CreateSolidBrush(alarm_color);
         split_rect (vertical,max_apos,ar,vr);
        }
        else
        color = danger_color = alarm_color;
       }

       if(pparam->record.state&MDBR_STATE_DANGER)
       {
        if(max_dpos<val_pos)
          {
           danger_brush = CreateSolidBrush(danger_color);
           split_rect(vertical,max_dpos,dr,vr);
          }
          else
          color = danger_color;
       }



       {
        tlmpaint::TGdiObject<HBRUSH> bk_br(CreateSolidBrush(bk_color));
        tlmpaint::TGdiObject<HBRUSH> val_br (CreateSolidBrush(color));
        FillRect(pparam->paint_dc,&r,bk_br());
        FillRect(pparam->paint_dc,&vr,val_br());
       }

       if(danger_brush)
       {
        FillRect(pparam->paint_dc,&dr,danger_brush);
        DeleteObject(danger_brush);
       }

       if(alarm_brush )
       {
        FillRect(pparam->paint_dc,&ar,alarm_brush);
        DeleteObject(alarm_brush );
       }
       if(zero_point)
       {
        get_rect(r,pparam);
        POINT save_point;
        tlmpaint::TRopSwap rs (pparam->paint_dc,R2_NOTXORPEN);
        if(vertical)
        {
          MoveToEx(pparam->paint_dc,r.left,r.bottom - zero_point,&save_point );
          LineTo  (pparam->paint_dc,r.right,r.bottom - zero_point);
        }
        else
         {
           MoveToEx(pparam->paint_dc,r.left+zero_point,r.top,&save_point);
           LineTo  (pparam->paint_dc,r.left+zero_point,r.bottom  );

         }

          MoveToEx(pparam->paint_dc,save_point.x,save_point.y,NULL);
       }
       if(simple_text)
          analog_text_painter_t::paint(pparam,pvar);
       if(pparam->record.diag)
        paint_question_sign(pparam);
}

/*Отрисовка LED - индикаторами*/

void __fastcall analog_led_painter_t::release()
{
  analog_text_painter_t::release();
  release_temp_bitmaps();
  if(image_dst_dc) {DeleteDC(image_dst_dc); image_dst_dc = NULL;}
  if(mask_dst_dc ) {DeleteDC(mask_dst_dc ); mask_dst_dc  = NULL;}

}

void __fastcall analog_led_painter_t::release_temp_bitmaps()
{
  if(image_bitmap) {DeleteObject(image_bitmap); image_bitmap = NULL;}
  if(mask_bitmap ) {DeleteObject(mask_bitmap ); mask_bitmap  = NULL;}
  image_width = 0;
}


void __fastcall analog_led_painter_t::create_temp_bitmaps(int width,int height)
{
  if(image_width < width  )
    {
      release_temp_bitmaps();
      image_width  = width;
      image_bitmap = create_bitmap(width,height);
      mask_bitmap  = create_bitmap(width,height);
    }
}


void __fastcall analog_led_painter_t::get_colors(const LPTLMOBJ_PAINT_PARAM1  pparam,DWORD & color,DWORD & bkgnd_color)
{
      if(pparam->record.state&MDBR_STATE_ALARM)
          color = obj_painter_t::alarm_color;
          else
          {
           if(pparam->record.state&MDBR_STATE_DANGER)
              color = obj_painter_t::danger_color;
              else
              color    = pparam->entry.options.color;
          }

      bkgnd_color = pparam->entry.options.bkgnd_color;
      if(color  == bkgnd_color)
          bkgnd_color=0xFFFFFF&(~bkgnd_color);

}


void __fastcall analog_led_painter_t::calc_size     (const LPTLMOBJ_PAINT_PARAM1  pparam, const Tpaint_variant * pvar,LPSIZE sz)
{
   wchar_t res[256];
   int len = get_value_text(pparam,pparam->entry.options.digits >1 ? true : false,res,256,NULL);
   sz->cx = len*(pvar->sz.cx/5);
   sz->cy = pvar->sz.cy/5;
}

void __fastcall analog_led_painter_t::paint(const LPTLMOBJ_PAINT_PARAM1  pparam,const Tpaint_variant * pvar)
{
      wchar_t vtext[256];
      int point_pos;
      int len          = get_value_text(pparam,pparam->entry.options.digits>1 ? true : false,vtext,KERTL_ARRAY_COUNT(vtext),&point_pos);
      int digit_width  = pvar->sz.cx;
      int digit_height = pvar->sz.cy;

      int led_width = get_digits_width(vtext,len,digit_width);

      DWORD  color;
      DWORD  bk_color;
      get_colors(pparam,color,bk_color);
      RECT     rect_bmp;
      set_rect (rect_bmp,0,0,led_width,digit_height);
      create_temp_bitmaps(led_width,digit_height   );



      tlmpaint::TGdiObject<HBRUSH> bk_brush (CreateSolidBrush(bk_color));
      tlmpaint::TGdiObject<HBRUSH> led_brush(CreateSolidBrush(color   ));

      tlmpaint::TGdiSwap dst_image_dc (get_image_dc(pparam->paint_dc),this->image_bitmap);
      tlmpaint::TGdiSwap dst_mask_dc  (get_mask_dc (pparam->paint_dc),this->mask_bitmap );

      FillRect(dst_image_dc(),&rect_bmp,led_brush());
      FillRect(dst_mask_dc (),&rect_bmp,bk_brush ());

      tlmpaint::TGdiSwap src_image_dc (pvar->get_temp_dc(pparam->paint_dc),pvar->image);
      tlmpaint::TGdiSwap src_mask_dc  (pvar->get_mem_dc (pparam->paint_dc),pvar->mask );

      int digit_offset = 0;
      int dst_offset   = 0;
      for(int i = 0;i<len;i++)
      {
        int cw        = 0;
        int digit_idx = get_digit_idx(vtext[i],cw,digit_width);
        if(digit_idx>=0)
          {
            digit_offset   =  digit_idx*digit_width;
            BitBlt(dst_mask_dc (),dst_offset,0,cw,digit_height,src_mask_dc (),digit_offset,0,SRCAND);
            BitBlt(dst_image_dc(),dst_offset,0,cw,digit_height,src_image_dc(),digit_offset,0,SRCAND);
            dst_offset += cw;
          }
      }

      BitBlt(dst_mask_dc (),0,0,rect_width(rect_bmp),rect_height(rect_bmp),dst_image_dc(),0,0,SRCPAINT);

      RECT paint_rect;
      get_rect(paint_rect,pparam);

      int old_stretch_mode = SetStretchBltMode(pparam->paint_dc,COLORONCOLOR);
      StretchBlt( pparam->paint_dc
                 ,paint_rect.left,paint_rect.top
                 ,rect_width(paint_rect),rect_height(paint_rect)
                 ,dst_mask_dc ()
                 ,0,0,rect_bmp.right,rect_bmp.bottom
                 ,SRCCOPY
                 );
      SetStretchBltMode(pparam->paint_dc,old_stretch_mode);

      if(pparam->record.diag)
         this->paint_question_sign(pparam,&paint_rect);
}

int __fastcall analog_led_painter_t::get_digit_idx(wchar_t ch,int & width,int digit_width)
{
  width = digit_width;
  switch(ch)
  {
   case '-' : return 10;
   case '.' : width/=5;width+=10;return 11;
   case ' ' : return 12;
  }
  return ch-'0';
}

int   __fastcall analog_led_painter_t::get_digits_width(wchar_t * vtext,int len,int digit_width)
{
  int ret = 0;
  int cw  = 0;
  wchar_t * end = vtext+len;
  while(vtext<end)
  {
   get_digit_idx(*vtext,cw,digit_width);
   ret+=cw;
   ++vtext;
  }
 return ret;
}


void  __fastcall analog_led_painter_t::get_rect(RECT & r,const LPTLMOBJ_PAINT_PARAM1  pparam)
{
       obj_painter_t::get_rect(r,pparam);
       grow_rect(r,-2,-2);
       move_rect(r,1,1);
}



