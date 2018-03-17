#pragma hdrstop
#include <tlm_painter.h>
#include <stdio.h>
#include <math.h>
#include <geometry.hpp>

#define DANGER_COLOR RGB(0xFF,0x80,0)
#define ALARM_COLOR  RGB(0xFF,0,0)

void __fastcall paint_question_sign(LPTLMOBJ_PAINT_PARAM paint_param,void  * pv);

/*Отрисовка*/

int __fastcall get_value_text(LPTLMOBJ_PAINT_PARAM paint_param, bool fill_zero,wchar_t * result,DWORD result_sz,int * point_pos = NULL)
{
 wchar_t format[MAX_PATH];
 float max_value  = KeRTL::MAX(fabs(paint_param->record.max_value),fabs(paint_param->record.min_value));
 if(!(DWORD)max_value)
    max_value  = fabs(paint_param->record.value);
 DWORD max_digits = !(DWORD)max_value ? 1:1+(DWORD)log10(max_value);
       max_digits = KeRTL::MAX((DWORD)HIWORD(paint_param->precision),max_digits);
 DWORD prec = LOWORD(paint_param->precision);
 max_digits +=(prec+1);// на точку
 char space_ch    = fill_zero || HIWORD(paint_param->precision) ? '0':' ';

 if(paint_param->record.min_value<.0)
 swprintf(format,L"%c %c%d.%df",'%',space_ch,1+max_digits,prec);
 else
 swprintf(format,L"%c%c%d.%df",'%',space_ch,max_digits,prec);
 int len = snwprintf(result,result_sz,format,paint_param->record.value);
 if(point_pos){
     wchar_t * pp = wcschr(result,'.');
     if(pp)
       *point_pos = (pp-result)/sizeof(*result);
       else
       *point_pos = -1;

 }
 return len;
}

DWORD __fastcall a_paint_text(LPTLMOBJ_PAINT_PARAM paint_param,BOOL calc_size,void * pv)
{

 wchar_t result[MAX_PATH];
 int len = get_value_text(paint_param,false,result,KERTL_ARRAY_COUNT(result));

 SIZE sz;
 //UINT old_align = SetTextAlign(paint_param->paint_dc,TA_CENTER|TA_BASELINE|TA_UPDATECP);
 GetTextExtentPoint32W(paint_param->paint_dc,result,len,&sz);
 if(calc_size || !paint_param->size.cx || !paint_param->size.cx)
 {
    if(calc_size || !paint_param->size.cx) paint_param->size.cx = sz.cx+4;
    if(calc_size || !paint_param->size.cy) paint_param->size.cy = sz.cy+4;
 }


 if(!calc_size)
 {
   RECT r;
   set_rect(r,paint_param->position,paint_param->size);
   if(paint_param->erase)
     {
      HDC MemoryDC = CreateCompatibleDC(paint_param->paint_dc);
      HGDIOBJ old_dcbmp = SelectObject(MemoryDC,paint_param->bkgnd_bitmap);
      BitBlt(paint_param->paint_dc,r.left,r.top,paint_param->size.cx,paint_param->size.cy,MemoryDC,r.left,r.top,SRCCOPY);
      SelectObject(MemoryDC,old_dcbmp);
      DeleteDC(MemoryDC);
     }
   else
   {
   int bk_mode        = SetBkMode(paint_param->paint_dc,OPAQUE);
   DWORD old_bk_color = SetBkColor(paint_param->paint_dc,paint_param->bkgnd_color);
   DWORD text_color   = paint_param->color;
   if(paint_param->record.state&MDBR_STATE_DANGER)
      text_color = DANGER_COLOR;
   if(paint_param->record.state&MDBR_STATE_ALARM)
      text_color = ALARM_COLOR;

   if(text_color      == paint_param->bkgnd_color)
      text_color=0xFFFFFF&(~text_color);

   if(paint_param->record.diag)
   {
    RGBQUAD * rgb  = (RGBQUAD*)&text_color;
    //DWORD gray_val = (rgb->rgbRed + 2*rgb->rgbGreen + rgb->rgbBlue)>>2;
    rgb->rgbBlue /=2,  rgb->rgbGreen /=2, rgb->rgbRed /=2 ;
   }

   DWORD old_color    = SetTextColor(paint_param->paint_dc,text_color);
   ExtTextOutW(paint_param->paint_dc,paint_param->position.x+2,paint_param->position.y+2,ETO_OPAQUE|ETO_CLIPPED,&r,result,len,NULL);
   if(paint_param->record.diag)
       paint_question_sign(paint_param,pv);
   SetBkColor(paint_param->paint_dc,old_bk_color);
   SetTextColor(paint_param->paint_dc,old_color);
   SetBkMode(paint_param->paint_dc,bk_mode);
   }
 }
 //SetTextAlign(paint_param->paint_dc,old_align);
 return 0;
}

void  __fastcall split_rect    (bool vertical,int split_line,RECT & r1,RECT & r2)
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

void  __fastcall calc_bar_param(bool vertical,RECT & r,MDB_RECORD & rec
                                                      ,int & pixels
                                                      ,int & val_pos
                                                      ,int & min_danger_pos
                                                      ,int & max_danger_pos
                                                      ,int & min_alarm_pos
                                                      ,int & max_alarm_pos
                                                      ,int & zero_pos)
{
 pixels         = vertical ? (r.bottom - r.top) : (r.right - r.left);
 float k        = fabs(std::max(rec.value,rec.max_value) - std::min(rec.min_value,rec.value));
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

DWORD __fastcall a_paint_bar (bool vertical,LPTLMOBJ_PAINT_PARAM paint_param,BOOL calc_size,void * pv)
{
  if(calc_size || paint_param->size.cx<4 || paint_param->size.cy<4)
  {
     if(paint_param->size.cx<4)
         paint_param->size.cx = vertical ? 32 : 64;
     if(paint_param->size.cy<4)
         paint_param->size.cy = vertical ? 64 : 32;
  }

  if(!calc_size)
  {
  RECT r;
  set_rect(r,paint_param->position,paint_param->size);
  if(paint_param->erase)
  {
      HDC MemoryDC = CreateCompatibleDC(paint_param->paint_dc);
      HGDIOBJ old_dcbmp = SelectObject(MemoryDC,paint_param->bkgnd_bitmap);
      BitBlt(paint_param->paint_dc,r.left,r.top,paint_param->size.cx,paint_param->size.cy,MemoryDC,r.left,r.top,SRCCOPY);
      SelectObject(MemoryDC,old_dcbmp);
      DeleteDC(MemoryDC);
  }
  else
  {
   int pixels,val_pos,min_dpos,max_dpos,min_apos,max_apos,zero_point;
   calc_bar_param(vertical,r,paint_param->record,pixels,val_pos,min_dpos,max_dpos,min_apos,max_apos,zero_point);



   DWORD   danger_color = DANGER_COLOR;
   DWORD   alarm_color  = ALARM_COLOR;
   DWORD   color = paint_param->color != paint_param->bkgnd_color ? paint_param->color : 0xFFFFFF&(~paint_param->color);

   HBRUSH  bk_brush     = CreateSolidBrush(paint_param->bkgnd_color);

   HBRUSH  val_brush    = NULL ;
   HBRUSH  danger_brush = NULL;
   HBRUSH  alarm_brush  = NULL;
   RECT    vr,dr,ar;

   split_rect(vertical,val_pos,r,vr);
   dr = vr;
   ar = vr;

   if(paint_param->record.state&MDBR_STATE_ALARM)
   {
    if(max_apos<val_pos)
    {
     alarm_brush = CreateSolidBrush(alarm_color);
     split_rect (vertical,max_apos,ar,vr);
    }
    else
    color = danger_color = alarm_color;
   }

   if(paint_param->record.state&MDBR_STATE_DANGER)
   {
    if(max_dpos<val_pos)
      {
       danger_brush = CreateSolidBrush(danger_color);
       split_rect(vertical,max_dpos,dr,vr);
      }
      else
      color = danger_color;
   }

   val_brush =  CreateSolidBrush(color);

   FillRect(paint_param->paint_dc,&r,bk_brush);
   FillRect(paint_param->paint_dc,&vr,val_brush);

   if(danger_brush)
   {
    FillRect(paint_param->paint_dc,&dr,danger_brush);
    DeleteObject(danger_brush);
   }

   if(alarm_brush )
   {
    FillRect(paint_param->paint_dc,&ar,alarm_brush);
    DeleteObject(alarm_brush );
   }

   DeleteObject(val_brush);
   DeleteObject(bk_brush );

   if(zero_point)
   {
    set_rect(r,paint_param->position,paint_param->size);
    POINT save_point;
    int old_rop = SetROP2(paint_param->paint_dc,R2_NOTXORPEN );
    if(vertical)
    {
      MoveToEx(paint_param->paint_dc,r.left,r.bottom - zero_point,&save_point );
      LineTo  (paint_param->paint_dc,r.right,r.bottom - zero_point);
    }
    else
     {
       MoveToEx(paint_param->paint_dc,r.left+zero_point,r.top,&save_point);
       LineTo  (paint_param->paint_dc,r.left+zero_point,r.bottom  );

     }

      MoveToEx(paint_param->paint_dc,save_point.x,save_point.y,NULL);
      SetROP2(paint_param->paint_dc,old_rop );
   }
   if(paint_param->record.diag)
    paint_question_sign(paint_param,pv);


  }
 }
 return 0;
}

DWORD __fastcall a_paint_hbar(LPTLMOBJ_PAINT_PARAM paint_param,BOOL calc_size,void * pv)
{
  return a_paint_bar(false,paint_param,calc_size,pv);
}


DWORD __fastcall a_paint_vbar(LPTLMOBJ_PAINT_PARAM paint_param,BOOL calc_size,void *pv)
{
  return a_paint_bar(true,paint_param,calc_size,pv);
}


DWORD __fastcall a_paint_pie(LPTLMOBJ_PAINT_PARAM paint_param,BOOL calc_size,void * pv)
{
   if(paint_param->size.cx<16)
       paint_param->size.cx = 16;
   if(paint_param->size.cy<16)
       paint_param->size.cx = 16;
   if(!calc_size)
   {
     RECT r;
     set_rect(r,paint_param->position,paint_param->size);
    if(paint_param->erase)
    {
      HDC MemoryDC = CreateCompatibleDC(paint_param->paint_dc);
      HGDIOBJ old_dcbmp = SelectObject(MemoryDC,paint_param->bkgnd_bitmap);
      BitBlt(paint_param->paint_dc,r.left,r.top,paint_param->size.cx,paint_param->size.cy,MemoryDC,r.left,r.top,SRCCOPY);
      SelectObject(MemoryDC,old_dcbmp);
      DeleteDC(MemoryDC);

    }
    else
    {
      grow_rect(r,-2,-2);
      move_rect(r,2,2);
      float amplitude   = fabs(paint_param->record.max_value- paint_param->record.min_value);
      if(amplitude)
      {
      HPEN    pen = CreatePen(PS_SOLID,1,paint_param->color);
      HBRUSH  br   = CreateSolidBrush(paint_param->color);
      HBRUSH  bkbr = CreateSolidBrush(paint_param->bkgnd_color);
      pen = (HPEN)  SelectObject (paint_param->paint_dc,pen);
      br  = (HBRUSH)SelectObject (paint_param->paint_dc,br);
      float value       = fabs(paint_param->record.value- paint_param->record.min_value);
      float angle_k     = 360.0/amplitude;
      float angle_value = value*angle_k;
      grow_rect(r,-2,-2);
      move_rect(r,2,2);
      bkbr =(HBRUSH) SelectObject(paint_param->paint_dc,bkbr);
      Ellipse(paint_param->paint_dc,r.left,r.top,r.right,r.bottom);
      bkbr = (HBRUSH)SelectObject(paint_param->paint_dc,bkbr);

      if(angle_value)
      {
        float StartA  = 0;
        float EndA    = angle_value;
        float midX, midY, stX, stY, endX, endY;
        float sX, sY, eX, eY;

         midX =  float(r.right-r.left)/2.0;
         midY =  float(r.bottom-r.top)/2.0;

       sX = cos(StartA);
       sY = sin(StartA);
       eX = cos((EndA / 180.0) * M_PI);
       eY = sin((EndA / 180.0) * M_PI);

       stX = floor(sX * 100.0);
       stY = floor(sY * 100.0);
       endX = ceil(eX * 100.0);
       endY = ceil(eY * 100.0);


      //FillRect(paint_param->paint_dc,&r,bkbr);
      Pie(paint_param->paint_dc, r.left,
                               r.top,
                               r.right,
                               r.bottom,
                               r.right-midX + stX, r.bottom-midY - stY,
                               r.right-midX + endX, r.bottom-midY - endY);
      }                       

      pen = (HPEN)  SelectObject (paint_param->paint_dc,pen);
      br  = (HBRUSH)SelectObject (paint_param->paint_dc,br);
      DeleteObject( bkbr );
      DeleteObject( br );
      DeleteObject(pen);
      if(paint_param->record.diag)
         paint_question_sign(paint_param,pv);

    }
   else
   return a_paint_text(paint_param,calc_size,pv);

    }


   }
   return 0;
}


HBITMAP  bmp_digits_mask = NULL;
HBITMAP  bmp_digits      = NULL;
BITMAP   digits_info      ;
int      digit_width     = 0;


int __fastcall get_digit_idx(wchar_t ch,int & width)
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

int   __fastcall get_digits_width(wchar_t * vtext,int len)
{
  int ret = 0;
  int cw  = 0;
  wchar_t * end = vtext+len;
  while(vtext<end)
  {
   get_digit_idx(*vtext,cw);
   ret+=cw;
   ++vtext;
  }
 return ret;
}

DWORD __fastcall a_paint_digits(LPTLMOBJ_PAINT_PARAM paint_param,BOOL get_extent,void * pv)
{
 if(!bmp_digits_mask)
     bmp_digits_mask = LoadBitmap( ModuleInstance,MAKEINTRESOURCE(IDBMP_LED_DIGITS_MASK));

 if(!bmp_digits     )
     {
      bmp_digits      = LoadBitmap( ModuleInstance,MAKEINTRESOURCE(IDBMP_LED_DIGITS));
      GetObject(bmp_digits,sizeof(digits_info),&digits_info);
      digit_width = digits_info.bmWidth/13;
     }

 wchar_t vtext[256];
 int point_pos;
 int len = get_value_text(paint_param,false,vtext,KERTL_ARRAY_COUNT(vtext),&point_pos);

 if(!get_extent)
    {

    DWORD  color    = paint_param->color;
      if(paint_param->record.state&MDBR_STATE_DANGER)
       color = DANGER_COLOR;
      if(paint_param->record.state&MDBR_STATE_ALARM)
       color = ALARM_COLOR;
      if(color      == paint_param->bkgnd_color)
        color=0xFFFFFF&(~color);

      HBRUSH bk_br    = CreateSolidBrush(paint_param->bkgnd_color);
      HBRUSH fg_br    = CreateSolidBrush(color);

      //Создаем bitmap для формирования цифр
      RECT     rect_bmp;
      set_rect(rect_bmp,0,0,get_digits_width(vtext,len),(int)digits_info.bmHeight);

      HBITMAP      bk_bmp = create_bitmap(rect_bmp.right,rect_bmp.bottom);
      HBITMAP      fg_bmp = create_bitmap(rect_bmp.right,rect_bmp.bottom);


      HDC  mem_dc1     = CreateCompatibleDC(paint_param->paint_dc);
      HDC  mem_dc2     = CreateCompatibleDC(paint_param->paint_dc);

      HGDIOBJ old_memdc1_bmp = SelectObject(mem_dc1,bmp_digits_mask);
      HGDIOBJ old_memdc2_bmp = SelectObject(mem_dc2,bmp_digits);

      HDC  temp_dc1    = CreateCompatibleDC(mem_dc1);
      HDC  temp_dc2    = CreateCompatibleDC(mem_dc2);


      HGDIOBJ old_tempdc1_bmp = SelectObject(temp_dc1,bk_bmp);
      FillRect    (temp_dc1,&rect_bmp,bk_br);

      HGDIOBJ old_tempdc2_bmp =SelectObject(temp_dc2,fg_bmp);
      FillRect    (temp_dc2,&rect_bmp,fg_br);



      int digit_offset = 0;
      int dst_offset   = 0;
      for(int i = 0;i<len;i++)
      {
        int cw        = 0;
        int digit_idx = get_digit_idx(vtext[i],cw);
        if(digit_idx>=0)
          {
            digit_offset   =  digit_idx*digit_width;
            BitBlt(temp_dc1,dst_offset,0,cw,digits_info.bmHeight,mem_dc1,digit_offset,0,SRCAND);
            BitBlt(temp_dc2,dst_offset,0,cw,digits_info.bmHeight,mem_dc2,digit_offset,0,SRCAND);
            dst_offset += cw;
          }
      }
      BitBlt(temp_dc1,0,0,rect_bmp.right,rect_bmp.bottom,temp_dc2,0,0,SRCPAINT);

      int old_stretch_mode = SetStretchBltMode(paint_param->paint_dc,COLORONCOLOR);
      StretchBlt(paint_param->paint_dc
                 ,paint_param->position.x+1,paint_param->position.y+1,paint_param->size.cx-1,paint_param->size.cy-1
                 ,temp_dc1
                 ,0,0,rect_bmp.right,rect_bmp.bottom
                 ,SRCCOPY
                 );
      SetStretchBltMode(paint_param->paint_dc,old_stretch_mode);
      if(paint_param->record.diag)
        paint_question_sign(paint_param,pv);


      SelectObject(mem_dc1,old_memdc1_bmp);
      SelectObject(mem_dc2,old_memdc2_bmp);
      SelectObject(temp_dc1,old_tempdc1_bmp);
      SelectObject(temp_dc2,old_tempdc2_bmp);

      DeleteObject(fg_br  );
      DeleteObject(bk_br  );
      DeleteObject(bk_bmp);
      DeleteObject(fg_bmp);

      DeleteDC(temp_dc1);
      DeleteDC(temp_dc2);

      DeleteDC(mem_dc1 );
      DeleteDC(mem_dc2 );
    }

 return 0;
}



