#pragma hdrstop
#include <tlm_painter.hpp>
#include <pictdlgs.h>
#include <kertl.hpp>
#include <freeimage32.h>
#include <float.h>

#pragma link "freeimage32.lib"


  COLORREF obj_painter_t::danger_color  = RGB(0xFF,0x80,0);
  COLORREF obj_painter_t::alarm_color   = RGB(0xFF,0x00,0);

  void __fastcall _rectangle(HDC dc,const RECT & r)
  {
         POINT old_position;
         MoveToEx(dc,r.left,r.top,&old_position);
         LineTo  (dc,r.right,r.top);
         LineTo  (dc,r.right,r.bottom);
         LineTo  (dc,r.left ,r.bottom);
         LineTo  (dc,r.left ,r.top);
         MoveToEx(dc,old_position.x,old_position.y,NULL);
  }

 void __fastcall _rectangle1(HDC dc,const RECT & r)
 {
   POINT sel_points[5];
   sel_points[0].x = r.right;
   sel_points[0].y = r.top;
   sel_points[1].x = r.right;
   sel_points[1].y = r.bottom;
   sel_points[2].x = r.left;
   sel_points[2].y = r.bottom;
   sel_points[3].x = r.left;
   sel_points[3].y = r.top;
   sel_points[4].x = r.right;
   sel_points[4].y = r.top;
   Polyline(dc,sel_points,5);

 }

  void __fastcall obj_painter_t::paint_line (const LPTLMOBJ_PAINT_PARAM1  pparam,COLORREF color)
  {
    MDB_KADR_ENTRY & entry = pparam->entry;
    int line_width = entry.options.line_width;
    if(line_width)
    {
     tlmpaint::TGdiObject<HPEN> pen(CreatePen(PS_SOLID,line_width,color));
     tlmpaint::TGdiSwap  dc_pen(pparam->paint_dc,pen());
     int x1,y1,x2,y2;

     if(entry.size.cy > entry.size.cx)
      {
        //Вертикальная линия
        int delta = (entry.size.cx-line_width/2)/2;
        x1 = entry.position.x+delta;
        y1 = entry.position.y;
        x2 = x1;
        y2 = y1+entry.size.cy;

      }
      else
      {
        int delta = (entry.size.cy-line_width/2)/2;
        y1 = entry.position.y+delta;
        x1 = entry.position.x;
        y2 = y1;
        x2 = x1+entry.size.cx;
      }
      POINT save_point;
      MoveToEx(pparam->paint_dc,x1,y1,&save_point);
      LineTo  (pparam->paint_dc,x2,y2);
      MoveToEx(pparam->paint_dc,save_point.x,save_point.y,&save_point);
    }
  }

  #define FRAME_ROP  R2_WHITE

  void __fastcall obj_painter_t::frame_entry(const LPTLMOBJ_PAINT_PARAM1  pparam,const Tpaint_variant * pvar)
  {
   RECT r ;
   obj_painter_t::get_rect(r,pparam);
   tlmpaint::TGdiSwap ps(pparam->paint_dc,pvar->get_frame_pen());
   tlmpaint::TRopSwap rsw(pparam->paint_dc,FRAME_ROP);
   int w = rect_width (r);
   int h = rect_height(r);
   _rectangle1(ps(),r);
  }




 void __fastcall obj_painter_t::get_rect(RECT & r,const LPTLMOBJ_PAINT_PARAM1  pparam)
 {
  int x  = pparam->entry.position.x;
  int y  = pparam->entry.position.y;
  int cx = pparam->entry.size.cx;
  int cy = pparam->entry.size.cy;
  if(cx<0) {x+=cx;cx = -cx;}
  if(cy<0) {y+=cy;cy = -cy;}
  set_rect(r,x,y,cx,cy);
 }


  void obj_painter_t::operator ()(const LPTLMOBJ_PAINT_PARAM1  pparam,const Tpaint_variant * pvar)
  {

    if(pparam->erase)
       erase(pparam,pvar);
    else
    {
     if(pparam->font)    pparam->font = (HFONT)SelectObject(pparam->paint_dc,pparam->font);
     paint(pparam,pvar);
     if(pparam->short_name && *pparam->short_name && pparam->entry.options.paint_flags&PAINT_FLAGS_SHORT_NAMES)
         name_paint_add(pparam);
     if(pparam->font)    pparam->font = (HFONT)SelectObject(pparam->paint_dc,pparam->font);
    }

    if( !(pparam->erase&TLMOBJ_PAINT_FLAG_ERASE) && (pparam->entry.state & MDB_KADR_ENTRY_STATE_SELECTED))
         frame_entry(pparam,pvar);
  }

  LRESULT  __fastcall obj_painter_t::do_calc_size (const LPTLMOBJ_PAINT_PARAM1  pparam, const Tpaint_variant * pvar,LPSIZE sz)
  {
    if(pparam->font)    pparam->font = (HFONT)SelectObject(pparam->paint_dc,pparam->font);
     calc_size(pparam,pvar,sz);
    if(pparam->font)    pparam->font = (HFONT)SelectObject(pparam->paint_dc,pparam->font);
    return GKH_RET_SUCCESS;
  }


  void __fastcall obj_painter_t::name_add_get_rect(const LPTLMOBJ_PAINT_PARAM1  pparam,RECT & dest_rect)
  {
    RECT src_rect;
    get_rect(src_rect,pparam);

    SIZE sz;
    obj_painter_t::name_size(pparam,sz);

    DWORD paint_name_flags = pparam->entry.options.paint_flags&PAINT_FLAGS_SHORT_NAMES;
    switch(paint_name_flags)
    {
    case PAINT_FLAG_SHORT_NAME_LEFT   :    rect_left(src_rect,sz,&dest_rect)  ; break;
    case PAINT_FLAG_SHORT_NAME_TOP    :    rect_upper(src_rect,sz,&dest_rect) ; break;
    case PAINT_FLAG_SHORT_NAME_BOTTOM :    rect_bottom(src_rect,sz,&dest_rect); break;
    case PAINT_FLAG_SHORT_NAME_RIGHT  :
    default                           :    rect_right(src_rect,sz,&dest_rect) ; break;
    }
  }

  void __fastcall obj_painter_t::name_paint_add(const LPTLMOBJ_PAINT_PARAM1  pparam)
  {
    RECT dest_rect;
    DWORD color ;
    name_add_get_rect(pparam,dest_rect);
    if(pparam->record.state&MDBR_STATE_ALARM)
        color = obj_painter_t::alarm_color;
        else
        {
          if(pparam->record.state&MDBR_STATE_DANGER)
           color = obj_painter_t::danger_color;
           else
           color =  pparam->entry.options.color;
        }
    if((pparam->record.state & MDBR_STATE_DISCRETE) && 0 == int(pparam->record.value))
        color = pparam->entry.options.bkgnd_color;
    name_paint(pparam,color,&dest_rect);
  }

  void __fastcall obj_painter_t::erase(HDC dc,HBITMAP bmp,RECT & r ,const Tpaint_variant * pvar)
  {
   grow_rect(r,4,4);
   move_rect(r,-2,-2);
   HDC temp_dc = pvar->get_temp_dc(dc);
   bmp = (HBITMAP)SelectObject(temp_dc,bmp);

   BitBlt(dc,
          r.left,r.top,r.right-r.left,r.bottom-r.top,
          temp_dc,r.left,r.top,SRCCOPY
         );

   SelectObject(temp_dc,bmp);

  }

 void __fastcall obj_painter_t::erase(const LPTLMOBJ_PAINT_PARAM1  pparam,const Tpaint_variant * pvar)
 {
  RECT r;
  get_rect(r,pparam);
  erase(pparam->paint_dc,pparam->bkgnd_bmp,r,pvar);
  if(pparam->entry.options.paint_flags&PAINT_FLAGS_SHORT_NAMES)
    {
      name_add_get_rect(pparam,r);
      erase(pparam->paint_dc,pparam->bkgnd_bmp,r,pvar);
    }
 }

  void __fastcall obj_painter_t::name_size(const LPTLMOBJ_PAINT_PARAM1  pparam,SIZE & sz)
  {
    const wchar_t * name = pparam->short_name;
    if(!name || !*name) name = L"record";
    GetTextExtentPoint32W(pparam->paint_dc, name,lstrlenW(pparam->short_name),&sz);
  }

  void __fastcall obj_painter_t::name_paint(const HDC dc,const RECT & r, const COLORREF color,const wchar_t * name)
  {
     int old_bk_mode     = SetBkMode   (dc,TRANSPARENT);
     COLORREF old_color  = SetTextColor(dc,color);
     ExtTextOutW(dc,r.left,r.top,ETO_CLIPPED,&r,name,lstrlenW(name),NULL);
     SetTextColor(dc,old_color);
     SetBkMode   (dc,old_bk_mode);
  }

 void __fastcall obj_painter_t::name_paint(const LPTLMOBJ_PAINT_PARAM1  pparam, const  COLORREF color,const RECT * r )
 {
  RECT prect;
  if(r)
     prect = *r;
     else
     {
       SetRect(&prect,pparam->entry.position.x,pparam->entry.position.y,pparam->entry.position.x+pparam->entry.size.cx,pparam->entry.position.y+pparam->entry.size.cy);
       SIZE nsz;
       name_size(pparam,nsz);
       RECT nr;
       set_rect(nr,0,0,nsz.cx,nsz.cy);
       centering_rects(prect,nr);
     }

     name_paint(pparam->paint_dc,prect,color,pparam->short_name);

     if(pparam->record.diag)
        paint_question_sign(pparam,&prect);
 }

void __fastcall obj_painter_t::paint_question_sign(LPTLMOBJ_PAINT_PARAM1 paint_param,LPRECT r )
{
      RECT  prect;
      if(r) prect = *r; else  get_rect(prect,paint_param);

      if(paint_param->question_sign)
      {
      /*Отрисовка знака вопроса*/
       int old_mode          = SetBkMode(paint_param->paint_dc,OPAQUE/*TRANSPARENT*/);
       COLORREF old_color    = SetTextColor(paint_param->paint_dc,RGB(255,0,0));
       COLORREF old_bk_color = SetBkColor  (paint_param->paint_dc,RGB(0,0,0));
       SIZE txt_sz ;

       GetTextExtentPointW(paint_param->paint_dc,&paint_param->question_sign,1,&txt_sz);
       ExtTextOutW (paint_param->paint_dc,prect.left+(rect_width(prect)-txt_sz.cx)/2,prect.top+(rect_height(prect)-txt_sz.cy)/2,ETO_CLIPPED,&prect,&paint_param->question_sign,1,0);
       SetBkMode(paint_param->paint_dc,old_mode);
       SetTextColor(paint_param->paint_dc,old_color);
       SetBkColor  (paint_param->paint_dc,old_bk_color);
      }
      else
      {
        tlmpaint::TGdiObject<HPEN> pen(  CreatePen(PS_DOT,1,RGB(255,0,0)) );
        tlmpaint::TGdiSwap swp(paint_param->paint_dc,pen());
        POINT pt;
        MoveToEx(paint_param->paint_dc,prect.left,prect.top,&pt);
        LineTo  (paint_param->paint_dc,prect.right,prect.bottom);
        MoveToEx(paint_param->paint_dc,prect.left,prect.bottom,NULL);
        LineTo  (paint_param->paint_dc,prect.right,prect.top);
        MoveToEx(paint_param->paint_dc,pt.x,pt.y,NULL);
      }
}

#define X_SPACE 6
#define Y_SPACE 6


void __fastcall obj_painter_t::rect_upper(const RECT & src,const SIZE & sz, LPRECT result)
{
       POINT pt;
       pt.x = src.left+rect_width(src)/2;
       pt.y = src.top; //Середина верхней грани
       pt.x -= (sz.cx>>1);
       pt.y-= Y_SPACE+sz.cy;
       set_rect(*result,pt,sz);
}

void __fastcall obj_painter_t::rect_bottom(const RECT & src,const SIZE & sz, LPRECT result)
{
       POINT pt;
       pt.x = src.left+rect_width(src)/2;
       pt.y = src.bottom; //Середина нижней грани
       pt.x -= (sz.cx>>1);
       pt.y+= Y_SPACE;
       set_rect(*result,pt,sz);
}

void __fastcall obj_painter_t::rect_right(const RECT & src,const SIZE & sz, LPRECT result)
{
  POINT pt;
  pt.x = src.right + X_SPACE;
  pt.y = src.top - sz.cy/2 + rect_height(src)/2; //Середина правой грани
  set_rect(*result,pt,sz);
}

void __fastcall obj_painter_t::rect_left(const RECT & src,const SIZE & sz, LPRECT result)
{
  POINT pt;
  pt.x = src.left - X_SPACE;
  pt.y = src.top - sz.cy/2 + rect_height(src)/2; //Середина левой грани
  pt.x -= sz.cx;
  set_rect(*result,pt,sz);
}

HBITMAP __fastcall obj_painter_t::create_bitmap(DWORD width,DWORD height)
 {
    HDC ScreenDC = CreateDCW(L"DISPLAY",0,0,0);
    HBITMAP bitmap = CreateCompatibleBitmap(ScreenDC,width,height);
    DeleteDC(ScreenDC);
    return bitmap;
 }

HBITMAP __fastcall obj_painter_t::clone_bitmap(HBITMAP src_bitmap)
 {
    BITMAP ibmp;
    if(GetObject(src_bitmap,sizeof(ibmp),&ibmp))
    {
     tlmpaint::TDC ScreenDC(CreateDCW(L"DISPLAY",0,0,0));
     HBITMAP ret = CreateCompatibleBitmap(ScreenDC(),ibmp.bmWidth,ibmp.bmHeight);
     if(ret)
     {
      tlmpaint::TDC src_dc(CreateCompatibleDC(ScreenDC()));
      tlmpaint::TDC dst_dc(CreateCompatibleDC(ScreenDC()));
      tlmpaint::TGdiSwap dst (dst_dc(),ret);
      tlmpaint::TGdiSwap src (src_dc(),src_bitmap);
      BitBlt(dst(),0,0,ibmp.bmWidth,ibmp.bmHeight
            ,src(),0,0,SRCCOPY) ;
     }
     return  ret;
    }
    return NULL;
 }


  void __fastcall obj_painter_t::centering_rects (RECT & dest, RECT & src)
  {
    int w  = rect_width     (dest);
    int h  = rect_height    (dest);
    int dx = w - rect_width (src );
    int dy = h - rect_height(src );
    if(dx<0)
      {
       src.left   -= dx/2;
       src.right  += dx/2;
      }
      else
      {
       dest.left  += dx/2;
       dest.right -= dx/2;
      }

    if(dy<0)
      {
        src.top     -= dy/2;
        src.bottom  += dy/2;
      }
      else
      {
        dest.top    += dy/2;
        dest.bottom -= dy/2;

      }
  }

void    __fastcall obj_painter_t::get_bitmap_size(HBITMAP bmp,SIZE & sz)
{
   BITMAP i_bmp;
   sz.cx = sz.cy = 0;
   if(GetObject(bmp,sizeof(i_bmp),&i_bmp))
   {
     sz.cx = i_bmp.bmWidth;
     sz.cy = i_bmp.bmHeight;
   }
}
HBITMAP __fastcall DibToDdb(FIBITMAP * fi_bmp,int width,int height)
{
 HDC ScreenDC = 0;
 HDC MemDC    = 0;
 HBITMAP Bmp  = 0;

 if(fi_bmp)
 {
  ScreenDC = CreateDC(L"DISPLAY",0,0,0);
  if(ScreenDC)
  {
   MemDC    = CreateCompatibleDC(ScreenDC);
   if(MemDC)
   {
     LPBITMAPINFO bi = FreeImage_GetInfo(fi_bmp);
     if(!width)  width  = bi->bmiHeader.biWidth;
     if(!height) height = bi->bmiHeader.biHeight;
     Bmp = CreateCompatibleBitmap(ScreenDC,width,height);
     if(Bmp)
     {
      SetBitmapDimensionEx(Bmp,width,height,NULL);
      Bmp =(HBITMAP) SelectObject(MemDC,Bmp);
      SetStretchBltMode(MemDC,COLORONCOLOR);
      StretchDIBits(
              MemDC,0,0,width,height,0,0,
              bi->bmiHeader.biWidth,
              bi->bmiHeader.biHeight,
              FreeImage_GetBits(fi_bmp),bi,DIB_RGB_COLORS,SRCCOPY
              );
      Bmp = (HBITMAP)SelectObject(MemDC,Bmp);
     }
   }
  }
 }

 if(ScreenDC)
    DeleteDC(ScreenDC);
 if(MemDC)
    DeleteDC(MemDC);
 return Bmp;
}
  HBITMAP __fastcall obj_painter_t::load_image(const wchar_t * image_name)
  {
    HBITMAP  ret     = NULL;
    wchar_t * res_bmp_id = RESOURCE_BITMAP;
    wchar_t * res_ptr = wcsstr(image_name,res_bmp_id);
    if( res_ptr )
      {
       res_ptr+=wcslen(res_bmp_id);
       ret = load_image(ModuleInstance,_wtoi(res_ptr));
       return ret;
      }
    FREE_IMAGE_FORMAT  img_fmt = FreeImage_GetFileTypeU(image_name);
    if(img_fmt != FIF_UNKNOWN)
    {
     FIBITMAP *fbmp = FreeImage_LoadU(img_fmt,image_name,0);
     if(fbmp)
       {
        ret = DibToDdb(fbmp,0,0);
        FreeImage_Unload(fbmp);
       }
    }
    return  ret;
  }


HBITMAP __fastcall obj_painter_t::load_image(const char * img_name)
{
   int len = strlen(img_name)+1;
   std::wstring _img_name(len,0);
   KeRTL::Ansi2Unicode(const_cast<std::wstring::pointer>(_img_name.data()),len,img_name);
   return load_image(_img_name.c_str());
}


  bool  __fastcall  obj_painter_t::init_temp_bmp   (const Tpaint_variant * pvar,int cx,int cy)
  {
       if(!pvar->temp_bmp)
        {
         ((Tpaint_variant*)pvar)->temp_bmp = create_bitmap(cx,cy);
         if(pvar->temp_bmp_rgb) delete [] pvar->temp_bmp_rgb;
         bzero(&((Tpaint_variant*)pvar)->temp_bmp_info,sizeof(pvar->temp_bmp_info));
        }
        else
        {
          SIZE sz;
          get_bitmap_size(pvar->temp_bmp,sz);
          if(sz.cx != cx || sz.cy != cy)
             {
              pvar->release_temp_bmp();
              init_temp_bmp(pvar,cx,cy);
             }
        }
     return pvar->temp_bmp ? true:false;
  }

  int __fastcall obj_painter_t::get_temp_bmp_bits (const HDC dc,const Tpaint_variant *pv)
  {
    int lines_count = 0;
    Tpaint_variant * pvar = (Tpaint_variant*)pv ;
    if(pvar )
    {
     if(!pvar->temp_bmp && !init_temp_bmp(pvar,pvar->sz.cx,pvar->sz.cy))
         return false;

      if(!pvar->temp_bmp_info.bmiHeader.biSize || !pvar->temp_bmp_rgb)
      {
        if(pvar->temp_bmp_rgb)
           {
            delete [] pvar->temp_bmp_rgb;
            pvar->temp_bmp_rgb = NULL;
           }


        bzero(&pvar->temp_bmp_info,sizeof(pvar->temp_bmp_info));
        pvar->temp_bmp_info.bmiHeader.biSize = sizeof(pvar->temp_bmp_info.bmiHeader);
        lines_count = GetDIBits(dc,pvar->temp_bmp,0,0,NULL,&pvar->temp_bmp_info,DIB_RGB_COLORS);
        if(lines_count) pvar->temp_bmp_rgb = new BYTE [pvar->temp_bmp_info.bmiHeader.biSizeImage];
      }

      if( pvar->temp_bmp_rgb)
      {
        LPBYTE array,array1;
        array = array1 = pvar->temp_bmp_rgb;
        lines_count = GetDIBits(dc,pvar->temp_bmp,0,pvar->temp_bmp_info.bmiHeader.biHeight,array,&pvar->temp_bmp_info,DIB_RGB_COLORS);
        if(lines_count)
           pvar->temp_bmp_rgb = array;
        //lines_count = GetDIBits(dc,pvar->temp_bmp,0,pvar->temp_bmp_info.bmiHeader.biHeight,array,&pvar->temp_bmp_info,DIB_RGB_COLORS);
        //pvar->temp_bmp_rgb = array;

       }
     }
    return  pvar->temp_bmp_rgb ? lines_count : 0;
  }

  bool __fastcall obj_painter_t::set_temp_bmp_bits (const HDC dc,const Tpaint_variant * pvar)
  {
    int lines_count = 0;
    if(pvar && pvar->temp_bmp_rgb)
    {
     lines_count = SetDIBits(dc,pvar->temp_bmp,0,pvar->temp_bmp_info.bmiHeader.biHeight,pvar->temp_bmp_rgb,&pvar->temp_bmp_info,DIB_RGB_COLORS);
    }
    return lines_count ? true : false;
  }





HBITMAP __fastcall obj_painter_t::load_image(HINSTANCE inst,DWORD res_id)
{
 HBITMAP bmp = NULL;
 HBITMAP res_bmp = LoadBitmap(inst,MAKEINTRESOURCE(res_id));
 if(res_bmp)
 {
    BITMAP bmp_info;
    GetObject(res_bmp,sizeof(bmp_info),&bmp_info);
    HDC ScreenDC = CreateDC(L"DISPLAY",0,0,0);
    bmp = CreateCompatibleBitmap(ScreenDC,bmp_info.bmWidth,bmp_info.bmHeight);
    HDC src_dc = CreateCompatibleDC(ScreenDC);
    HDC dst_dc = CreateCompatibleDC(ScreenDC);
    res_bmp = (HBITMAP) SelectObject(src_dc,res_bmp);
        bmp = (HBITMAP) SelectObject(dst_dc,bmp);

    BitBlt(dst_dc,0,0,bmp_info.bmWidth,bmp_info.bmHeight,src_dc,0,0,SRCCOPY);

    res_bmp = (HBITMAP) SelectObject(src_dc,res_bmp);
        bmp = (HBITMAP) SelectObject(dst_dc,bmp);
    DeleteDC(dst_dc);
    DeleteDC(src_dc);
    DeleteDC(ScreenDC);
    DeleteObject(res_bmp);
 }
 return  bmp;
}

int __fastcall obj_painter_t::get_value_text(LPTLMOBJ_PAINT_PARAM1 paint_param, bool fill_zero,wchar_t * result,DWORD result_sz,int * point_pos )
{
 int len = 0;
 if(!is_record_discrete(&paint_param->record))
 {
     wchar_t format[MAX_PATH];
     float max_value  = KeRTL::MAX(fabs(paint_param->record.max_value),fabs(paint_param->record.min_value));
     if(!(DWORD)max_value)
        max_value  = fabs(paint_param->record.value);

     DWORD max_digits = 1+(DWORD)log10(std::max(max_value,float(1.0)));
           max_digits = std::max((DWORD)paint_param->entry.options.digits,max_digits);
     DWORD prec = paint_param->entry.options.prec;
     max_digits +=(prec);// на точку
     char space_ch    = fill_zero ? '0':' ';
     swprintf(format,L"%c%c%d.%df",'%',space_ch,max_digits,prec);

     int float_class = _fpclass(paint_param->record.value);
     if(!(float_class & ~(_FPCLASS_NN|_FPCLASS_PN|_FPCLASS_PZ|_FPCLASS_NZ)) )
     {
       len = 0;
       result[len] = L' ';
       if(paint_param->record.min_value<0 && paint_param->record.value>=.0)  //На знак минуса
          len = 1;

         len  += snwprintf(result+len,result_sz-len,format,paint_param->record.value);
         if(point_pos){
             wchar_t * pp = wcschr(result,'.');
             if(pp)
               *point_pos = (pp-result)/sizeof(*result);
               else
               *point_pos = -1;

         }
     }
     else
     {
      std::fill(result,result+result_sz,L'-');
      len = max_digits;
     }
 }
 else
 {
  len = snwprintf(result,result_sz,L"%s",DWORD(paint_param->record.value) ? L" ON":L"OFF");
 }

 return len;
}




