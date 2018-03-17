#pragma hdrstop
#define _TLMOBJ_PAINTER_INTERNAL
#include <tlm_painter.h>
#include <gkwind.hpp>




extern HDC        MemoryDC ;
extern HDC        temp_dc;


void __fastcall paint_question_sign(LPTLMOBJ_PAINT_PARAM paint_param,void * /*pv*/)
{
/*Отрисовка знака вопроса*/
      //int old_mode       = SetBkMode(paint_param->paint_dc,TRANSPARENT);
      COLORREF old_color    = SetTextColor(paint_param->paint_dc,RGB(255,0,0));
      COLORREF old_bk_color = SetBkColor  (paint_param->paint_dc,RGB(0,0,0));
      KeWin::TSize txt_sz ;
      GetTextExtentPoint(paint_param->paint_dc,"?",1,&txt_sz);
      KeWin::TPoint pt(paint_param->position.x,paint_param->position.y);
      KeWin::TSize  sz(paint_param->size.cx,paint_param->size.cy);
      KeWin::TRect r( pt,sz);
      ExtTextOutW (paint_param->paint_dc,r.left+(r.Width()-txt_sz.cx)/2,r.top+(r.Height()-txt_sz.cy)/2,ETO_CLIPPED,&r,L"?",1,0);
      //SetBkMode(paint_param->paint_dc,old_mode);
      SetTextColor(paint_param->paint_dc,old_color);
      SetBkColor  (paint_param->paint_dc,old_bk_color);
}



//DWORD __fastcall get_lamp_index(LPMDB_SHORT_RECORD sr,DWORD normal_value,DWORD switch_count)
//{
//  DWORD image_idx = 0;
//  if(sr->diag)
//    image_idx = 7;
//    else
//    {
//     if(sr->state & MDBR_STATE_WORKOUT)
//     image_idx = 6;
//     else
//     {
//       if(switch_count)
//       {
//       if(switch_count>2)
//         image_idx+=3;
//         else
//         image_idx+=1;
//       if(normal_value!=(DWORD)sr->value)
//         image_idx++;
//       }
//       else
//       if(normal_value!=(DWORD)sr->value)
//           image_idx+=5;
//     }
//    }
//  if(sr->state & MDBR_STATE_TUFLAG)
//   {
//    image_idx +=16;
//    if(sr->state & MDBR_STATE_TU)
//      image_idx+= 8;
//      else
//    {
//     if(sr->state & MDBR_STATE_TUERROR)
//      image_idx+= 16;
//    }
//   }
//   else
//   {
//    if(sr->state & (MDBR_STATE_ALARM|MDBR_STATE_DANGER))
//     image_idx+=8;
//   }
//  return image_idx;
//}

enum discrete_index_t  { di_normal = 0,di_one = 1,di_back_normal = 2, di_many = 3,di_many_back_normal=4,di_not_normal = 5,di_workout = 6 ,di_tutr = 7,di_bad_diag = 8};

DWORD __fastcall get_lamp_index(LPMDB_RECORD sr,DWORD normal_value,DWORD switch_count)
{
  DWORD image_idx = 0;
  if(sr->diag)
    image_idx = di_bad_diag;
    else
    {
     if(sr->state & MDBR_STATE_WORKOUT)
     image_idx = di_workout;
     else
     {
       if(switch_count)
       {
       if(switch_count>2)
         image_idx=di_many;
         else
         image_idx=1;
       if(normal_value!=(DWORD)sr->value)
            image_idx = (switch_count > 2) ? di_many : di_one;
            else
            image_idx = (switch_count > 2) ? di_many_back_normal : di_back_normal;

       }
       else
       if(normal_value!=(DWORD)sr->value)
           image_idx=di_not_normal;
     }
    }
    if( sr->state & (MDBR_STATE_TUTR_ACTIVE|MDBR_STATE_TUTR_PREPARE) )
       image_idx = di_tutr;
  return image_idx;
}



SIZE __fastcall  calc_sname_size(LPTLMOBJ_PAINT_PARAM paint_param)
{
 SIZE ret = {0,0};
 if((paint_param->paint_options&PAINT_OPT_ADD_SHORTNAME_MASK) && paint_param->short_name && *paint_param->short_name)
 {
 SIZE space;
 GetTextExtentPointW(paint_param->paint_dc,L" ",1,&space);
 GetTextExtentPointW(paint_param->paint_dc,paint_param->short_name,lstrlenW(paint_param->short_name),&ret);
 if(paint_param->paint_options&(PAINT_OPT_ADD_RIGHT_SHORTNAME|PAINT_OPT_ADD_LEFT_SHORTNAME))
   ret.cx+=space.cx;
 else
  ret.cy+=space.cy;
 }
 return ret;
}

DWORD __fastcall d_paint_lamp  (LPTLMOBJ_PAINT_PARAM paint_param,BOOL calc_size ,void * _pv)
{


  Tpaint_variant &pv = *(Tpaint_variant*)_pv;
  if(!MemoryDC)
     MemoryDC = CreateCompatibleDC(paint_param->paint_dc);

  RECT r;
  SIZE sname_size = calc_sname_size(paint_param); //Размер короткого имени
  if(calc_size || !paint_param->size.cx || !paint_param->size.cy)
  {
   /*Вычисление размера*/
   paint_param->size.cx = pv.sz.cx;
   paint_param->size.cy = pv.sz.cy;
   if( paint_param->paint_options&(PAINT_OPT_ADD_RIGHT_SHORTNAME|PAINT_OPT_ADD_LEFT_SHORTNAME))
    {
      paint_param->size.cy   = KeRTL::MAX(paint_param->size.cy,sname_size.cy);
      paint_param->size.cx +=  sname_size.cx;

    }
    else
    {
     if( paint_param->paint_options&(PAINT_OPT_ADD_TOP_SHORTNAME|PAINT_OPT_ADD_BOTTOM_SHORTNAME))
      {
        paint_param->size.cy   = KeRTL::MAX(paint_param->size.cy,sname_size.cy);
        paint_param->size.cx +=  sname_size.cx;
      }
    }
        paint_param->size.cx+=4;/*Под выделение*/
        paint_param->size.cy+=4;
  }

   if(!calc_size)
   {

   set_rect(r,paint_param->position,paint_param->size);
   HANDLE old_dcbmp;
   bool dqs = false;

   if(!paint_param->erase)
   {
    paint_param->erase = true;
    d_paint_lamp(paint_param,FALSE,_pv);
    paint_param->erase = false;

    DWORD lamp_idx    = get_lamp_index(&paint_param->record,paint_param->normal_value,paint_param->changed);
    if(paint_param->record.diag && pv.elements_count<9) dqs = true;
    DWORD multi_position = pv.multi_position;
     if(pv.switched)
        multi_position = 2;
        else
        multi_position = KeRTL::MAX((DWORD)1,multi_position);


    if(lamp_idx >= (DWORD(pv.elements_count/multi_position)))
       lamp_idx = 0;
    if(multi_position>1)
    {
     lamp_idx *=multi_position;
     lamp_idx+=KeRTL::MIN(multi_position-1,(DWORD)paint_param->record.value);
    }
    DWORD lamp_offset = lamp_idx*pv.sz.cx;

    RECT bmp_rect;
    set_rect(bmp_rect,0,0,pv.sz.cx,pv.sz.cy);

    if(pv.mask)
    {
    /*Пробиваем маску*/
     old_dcbmp = SelectObject(MemoryDC,pv.mask);
     BitBlt(paint_param->paint_dc,paint_param->position.x+2,paint_param->position.y+2,pv.sz.cx,pv.sz.cy,MemoryDC,lamp_offset,0,SRCAND);
    }


    if(!temp_dc)  temp_dc   = CreateCompatibleDC(MemoryDC);
    HBITMAP       sw_bitmap = create_bitmap(pv.sz.cx,pv.sz.cy);

    DWORD    bk_color = paint_param->bkgnd_color;
    DWORD    color    = paint_param->color;
    if(multi_position<2)
    {
      if( color == bk_color)
         color = (~color)&0xFFFFFF;
    }


    HBRUSH   sw_br = CreateSolidBrush((DWORD) paint_param->record.value ? color:bk_color);

    HBITMAP old_tem_dc_bitmap = (HBITMAP)SelectObject(temp_dc,sw_bitmap);
    FillRect(temp_dc,&bmp_rect,sw_br); //Залили цветом отображения
    SelectObject(MemoryDC,pv.image);/*Сформировали картинку*/
    BitBlt(temp_dc,0,0,pv.sz.cx,pv.sz.cy,MemoryDC,lamp_offset,0,SRCAND);
    BitBlt(paint_param->paint_dc,paint_param->position.x+2,paint_param->position.y+2,pv.sz.cx,pv.sz.cy,temp_dc ,0,0,SRCPAINT);

    SelectObject(temp_dc,old_tem_dc_bitmap);
    DeleteObject(sw_br);
    DeleteObject(sw_bitmap);

   }
   else
   {
    old_dcbmp = SelectObject(MemoryDC,paint_param->bkgnd_bitmap);
    BitBlt(paint_param->paint_dc,r.left,r.top,paint_param->size.cx+2,paint_param->size.cy+2,MemoryDC,r.left,r.top,SRCCOPY);
   }

   SelectObject(MemoryDC,old_dcbmp);
   if(dqs) paint_question_sign(paint_param,_pv);
  }


return 0;
}


/*Отрисовка выключателем*/

DWORD __fastcall d_paint_switch(LPTLMOBJ_PAINT_PARAM paint_param,BOOL calc_size,void * _pv)
{
  Tpaint_variant & pv = *(Tpaint_variant*)_pv;
  SIZE sname_size = calc_sname_size(paint_param); //Размер короткого имени
  if(calc_size || !paint_param->size.cx || !paint_param->size.cy)
  {
   /*Вычисление размера*/
   paint_param->size.cx = pv.sz.cx;
   paint_param->size.cy = pv.sz.cy;
   if( paint_param->paint_options&(PAINT_OPT_ADD_RIGHT_SHORTNAME|PAINT_OPT_ADD_LEFT_SHORTNAME))
    {
      paint_param->size.cy   = KeRTL::MAX(paint_param->size.cy,sname_size.cy);
      paint_param->size.cx +=  sname_size.cx;

    }
    else
    {
     if( paint_param->paint_options&(PAINT_OPT_ADD_TOP_SHORTNAME|PAINT_OPT_ADD_BOTTOM_SHORTNAME))
      {
        paint_param->size.cy   = KeRTL::MAX(paint_param->size.cy,sname_size.cy);
        paint_param->size.cx +=  sname_size.cx;
      }
    }
        paint_param->size.cx+=4;/*Под выделение*/
        paint_param->size.cy+=4;

  }
   if(!calc_size)
   {
    if(!MemoryDC)
       MemoryDC = CreateCompatibleDC(paint_param->paint_dc);

   RECT r;
   set_rect(r,paint_param->position,paint_param->size);
   DWORD image_idx = DWORD(paint_param->record.value ? 1:0);
   if(paint_param->record.state&MDBR_STATE_TUTR_ACTIVE)
      image_idx+=2;
   if(paint_param->paint_variant == PAINT_DISCRETE_AS_VSWITCH)
     image_idx+=4;
     image_idx = KeRTL::MIN(image_idx,(DWORD)pv.elements_count-1);
     DWORD image_offset = image_idx*pv.sz.cx;

   HANDLE old_dcbmp;
   old_dcbmp = SelectObject(MemoryDC,paint_param->bkgnd_bitmap);
   BitBlt(paint_param->paint_dc,r.left,r.top,paint_param->size.cx+2,paint_param->size.cy+2,MemoryDC,r.left,r.top,SRCCOPY);
   //Восстанавливаем часть фона под рисунком выключателя
   SelectObject(MemoryDC,old_dcbmp);

   if(!paint_param->erase  ) /*Если не мигаем и не стираем*/
   {

    if(pv.mask)
    {
     pv.mask = (HBITMAP)SelectObject(MemoryDC,pv.mask);
     BitBlt(paint_param->paint_dc,paint_param->position.x+2,paint_param->position.y+2,
            pv.sz.cx,pv.sz.cy,MemoryDC,image_offset,0,SRCAND);
     pv.mask = (HBITMAP)SelectObject(MemoryDC,pv.mask);
    }
    /*Пробили маску в изображении*/


    HDC           temp_dc   = CreateCompatibleDC(paint_param->paint_dc);
    HBITMAP       sw_bitmap = create_bitmap(pv.sz.cx,pv.sz.cy);

    RECT bmp_rect;
    set_rect(bmp_rect,0,0,pv.sz.cx,pv.sz.cy);
    old_dcbmp  = SelectObject(temp_dc,pv.image);
    HBRUSH   br = CreateSolidBrush(paint_param->color);
    FillRect(temp_dc,&bmp_rect,br);
    DeleteObject(br);
    SelectObject(temp_dc,old_dcbmp);

    old_dcbmp = SelectObject(MemoryDC,pv.image);

    BitBlt(temp_dc,0,0,pv.sz.cx,pv.sz.cy,MemoryDC,image_offset,0,SRCAND);

    BitBlt(paint_param->paint_dc,paint_param->position.x+2,paint_param->position.y+2,
           pv.sz.cx,pv.sz.cy,temp_dc,0,0,SRCPAINT);

    SelectObject(MemoryDC,old_dcbmp);
    DeleteObject(sw_bitmap);
    DeleteDC(temp_dc);
    if(paint_param->record.diag)
      {
       //Поместить знак вопроса
       paint_question_sign(paint_param,_pv);
      }

   }

  }
 return 0;
}



DWORD __fastcall d_paint_sname (LPTLMOBJ_PAINT_PARAM paint_param,BOOL calc_size ,void * pv)
{
   wchar_t text[128];
   if(!paint_param->short_name || !(*paint_param->short_name))
   {
     swprintf(text,L"%1u",(1&(DWORD)paint_param->record.value));
     paint_param->short_name = text;
   }
   if(paint_param->short_name &&  *paint_param->short_name)
   {
   SIZE sz;
   GetTextExtentPointW(paint_param->paint_dc,paint_param->short_name,lstrlenW(paint_param->short_name),&sz);
   if(calc_size || !paint_param->size.cx || !paint_param->size.cy)
   {
    //Вычисление размера
     paint_param->size.cx = sz.cx+4;
     paint_param->size.cy = sz.cy+4;
   }
   
   if(!calc_size  )
   {
    if(!MemoryDC)
       MemoryDC = CreateCompatibleDC(paint_param->paint_dc);



    HANDLE old_dcbmp;
    old_dcbmp = SelectObject(MemoryDC,paint_param->bkgnd_bitmap);
    BitBlt(paint_param->paint_dc,paint_param->position.x,paint_param->position.y,
                                 paint_param->size.cx+2,paint_param->size.cy+2,
                                 MemoryDC,paint_param->position.x,paint_param->position.y,SRCCOPY);
    SelectObject(MemoryDC,old_dcbmp);

    if(!paint_param->erase)
       {
        DWORD bk_color = paint_param->bkgnd_color;
        DWORD color    = paint_param->color;
        if(color == bk_color)
           color = (~color)&0xFFFFFF;

        if((DWORD)paint_param->record.value)
        {
         if(paint_param->record.state&(MDBR_STATE_DANGER|MDBR_STATE_ALARM))
              color = RGB(255,0,0);
        }
        else
         color = bk_color;
        RECT r ;
        DWORD old_color = SetTextColor(paint_param->paint_dc,color);
        int old_mode    = SetBkMode(paint_param->paint_dc,TRANSPARENT);

        set_rect(r,paint_param->position,paint_param->size);
        grow_rect(r,-2,-2);
        ExtTextOutW(paint_param->paint_dc,r.left+2,r.top+2,0,&r,paint_param->short_name,lstrlenW(paint_param->short_name),NULL);
        SetTextColor(paint_param->paint_dc,old_color);
        SetBkMode(paint_param->paint_dc,old_mode);
        if(paint_param->record.diag)
           paint_question_sign(paint_param,pv);
       }

   }
  }
   else
   d_paint_lamp(paint_param,calc_size,pv);
  return 0;
}


