
#pragma hdrstop

#ifndef _TLMOBJ_PAINTER_INTERNAL
#define _TLMOBJ_PAINTER_INTERNAL
#endif

#include <tlm_painter.hpp>
#include <freeimage.hpp>

namespace tlmpaint
{



  void      __fastcall TBmpBkgnd::release_bkgnd_bmp()
  {
    if(bkgnd_bmp)
    {
       //bkgnd_bmp = (HBITMAP)SelectObject(dc,bkgnd_bmp);
       DeleteObject(bkgnd_bmp);
       bkgnd_bmp = NULL;
    }
  }

  void      __fastcall TBmpBkgnd::release()
  {
    release_bkgnd_bmp();
    if(src_bmp)
       DeleteObject(src_bmp);
    src_bmp = NULL;
    if(dc)
       DeleteDC(dc);
       dc = NULL;

  }

  HBITMAP   __fastcall TBmpBkgnd::create_bitmap(int width,int height)
  {
    HBITMAP bmp = NULL;
    if(width && height)
    {
     HDC  dc   = CreateDCW(L"DISPLAY",0,0,0);
          bmp  = CreateCompatibleBitmap(dc,width,height);
     DeleteDC(dc);
    }

    return bmp;
  }


  void      __fastcall TBmpBkgnd::create_bitmap()
  {
       release_bkgnd_bmp();
       bkgnd_bmp  = create_bitmap(width,height);
       setup_bitmap();
  }

  void      __fastcall TBmpBkgnd::fill_bitmap ()
  {
     if(width && height)
     {
      RECT r;
      SetRect(&r,0,0,width,height);
      HBRUSH br = GetSysColorBrush(COLOR_BTNFACE);
      bkgnd_bmp = (HBITMAP)SelectObject(dc,bkgnd_bmp);
      FillRect(dc,&r,br);
      DeleteObject(br);
      bkgnd_bmp = (HBITMAP)SelectObject(dc,bkgnd_bmp);
     }
  }

  void      __fastcall TBmpBkgnd::calc_stretch_proportional_size(const SIZE src_size,SIZE & result)
  {
    if(width && height)
    {
     double kx = width;
            kx/=(double)src_size.cx;
     double ky = height;
            ky/=(double)src_size.cy;
     double k = std::min(kx,ky);
     result.cx = k*double(src_size.cx);
     result.cy = k*double(src_size.cy);
    }
    else
    {result.cx = src_size.cx; result.cy = src_size.cy;}
  }

 void __fastcall get_bitmap_size(HBITMAP hBmp, SIZE & sz)
 {
     BITMAP  _bmp;
     GetObject(hBmp,sizeof(_bmp),&_bmp);
     sz.cx = _bmp.bmWidth;
     sz.cy = _bmp.bmHeight;
 }


  inline void __fastcall select_center_begin(int &dest,int &src,const int delta )
  {
    if(delta<0)
       src-=delta;
       else
       dest+=delta;
  }

  void      __fastcall TBmpBkgnd::setup_bitmap ()
  {
   if(bkgnd_bmp)
   {

    RECT r;
    SetRect(&r,0,0,width,height);
    fill_bitmap();
    if(src_bmp)
    {
      bkgnd_bmp  = (HBITMAP)SelectObject(dc,bkgnd_bmp);

      SIZE src_size;
      SIZE dst_size;
      dst_size.cx = width;
      dst_size.cy = height;
      get_bitmap_size(src_bmp,src_size);
      int old_dc_mode = SetStretchBltMode(dc,HALFTONE); //SetStretchBltMode(dc,COLORONCOLOR);

      HDC src_dc = CreateCompatibleDC(dc);
      src_bmp    = (HBITMAP)SelectObject(src_dc,src_bmp);

      switch(this->bkgnd_mode)
      {
        case bkgnd_default               :  //Скопировать по умолчанию
                BitBlt(dc,0,0,width,height,src_dc,0,0,SRCCOPY);
        break;
        case bkgnd_center                :  //Скопировать по центру
        {
              int dest_x = 0,dest_y = 0;
              int src_x  = 0,src_y = 0;
              int w = std::min(width ,(int)src_size.cx);
              int h = std::min(height,(int)src_size.cy);
              int dx =  (width  - src_size.cx)/2;
              int dy =  (height - src_size.cy)/2;
              select_center_begin(dest_x,src_x,dx);
              select_center_begin(dest_y,src_y,dy);
              BitBlt(dc,dest_x,dest_y,w,h,src_dc,src_x,src_y,SRCCOPY);
        }
        break;
        case bkgnd_stretch               :  //Привести к размерам
              StretchBlt(dc,0,0,width,height,src_dc,0,0,src_size.cx,src_size.cy,SRCCOPY);
        break;
        case bkgnd_stretch_proportianal  :  //Привести к размерам пропорционально
        {

              calc_stretch_proportional_size(src_size,dst_size);
              int x =  (width  - dst_size.cx)/2;
              int y =  (height - dst_size.cy)/2;
              StretchBlt(dc,x,y,dst_size.cx,dst_size.cy,src_dc,0,0,src_size.cx,src_size.cy,SRCCOPY);
        }
        break;
      }
      SetStretchBltMode(dc,old_dc_mode);
      bkgnd_bmp = (HBITMAP)SelectObject(dc,bkgnd_bmp);
      src_bmp   = (HBITMAP)SelectObject(src_dc,src_bmp);
      DeleteDC(src_dc);


    }

    setup_grid_and_gray();
   }
  }


inline void    __fastcall invert_pixel(LPRGBQUAD pixel)
{
     pixel->rgbGreen    = ~pixel->rgbGreen; //0;//
     pixel->rgbRed      = ~pixel->rgbRed;   //0;//
     pixel->rgbBlue     = ~pixel->rgbBlue;  //0;//
}

void __fastcall do_grid_line(LPRGBQUAD line_ptr,int width,int grid_step)
{
  LPRGBQUAD line_end = line_ptr+width;
  do{
     invert_pixel(line_ptr);
     line_ptr+= grid_step;
    }while(line_ptr<line_end);
}

inline LPRGBQUAD __fastcall get_bmp_line(LPRGBQUAD start,int number,int width,int height)
{
 LPRGBQUAD line = start;
 if(height>0)
   {
    line+=( height-number-1 )*width;
   }
   else
   {
    line+= (number*width);
   }
 return    line;
}

void    __fastcall TBmpBkgnd::do_setup_grid(LPRGBQUAD b_arr, int bmp_width, int bmp_height)
{
  int _height = abs(bmp_height);
  for(int i = 0;i<_height;)
  {
    do_grid_line(get_bmp_line(b_arr,i,bmp_width,bmp_height),bmp_width,grid);
    i+=grid;
  }
}


void    __fastcall TBmpBkgnd::do_gray_bkgnd(LPRGBQUAD b_arr,DWORD count)
{
 /*    Приведение изображения к  серому цвету */
    LPRGBQUAD e_arr = b_arr+count;
     while(b_arr<e_arr)
      {
       //DWORD gray_value = (b_arr->rgbRed + b_arr->rgbGreen + b_arr->rgbBlue)/3;
       DWORD gray_value = (b_arr->rgbRed + 2*b_arr->rgbGreen + b_arr->rgbBlue)>>2;

       b_arr->rgbBlue  =
       b_arr->rgbRed   =
       b_arr->rgbGreen = gray_value;
       ++b_arr;
       }
}

  void      __stdcall TBmpBkgnd::setup_grid_and_gray()
  {
    if(grid || grayed)
    {
     BYTE buff[sizeof(BITMAPINFO)+3*sizeof(DWORD) ];
     bzero(buff,sizeof(buff));
     BITMAPINFO & bmp_info = *(LPBITMAPINFO)buff;
     bmp_info.bmiHeader.biSize = sizeof(bmp_info.bmiHeader);
     LPBYTE array = NULL;
     int lines_count = GetDIBits(dc,bkgnd_bmp,0,0,array,&bmp_info,DIB_RGB_COLORS);
     if(lines_count)
     {
         array = new BYTE[bmp_info.bmiHeader.biSizeImage];
         if(array)
         {
             lines_count = GetDIBits(dc,bkgnd_bmp,0,bmp_info.bmiHeader.biHeight,array,&bmp_info,DIB_RGB_COLORS);
             if(lines_count)
             {
                 if(grayed) do_gray_bkgnd((LPRGBQUAD)array,bmp_info.bmiHeader.biSizeImage/sizeof(RGBQUAD));
                 if(grid  ) do_setup_grid((LPRGBQUAD)array,bmp_info.bmiHeader.biWidth,bmp_info.bmiHeader.biHeight);
				 /*lines_count = */
				 SetDIBits(dc,bkgnd_bmp,0,bmp_info.bmiHeader.biHeight,array,&bmp_info,DIB_RGB_COLORS);
             }
          delete [] array;
         }
     }
    }
  }


  void   __fastcall TBmpBkgnd::paint   ( const HDC paint_dc, const RECT & r)
  {
     if(!dc       )  dc = CreateCompatibleDC(paint_dc);
     if(!bkgnd_bmp)  create_bitmap();

     bkgnd_bmp = (HBITMAP)SelectObject(dc,bkgnd_bmp);
     BitBlt(paint_dc,r.left,r.top,r.right - r.left,r.bottom-r.top,dc,r.left,r.top,SRCCOPY);
     bkgnd_bmp = (HBITMAP)SelectObject(dc,bkgnd_bmp);

  }

       void   __fastcall TBmpBkgnd::set_size  ( int _width, int _height)
       {
         if(width != _width || height != _height)
         {
           width   = _width;
           height  = _height;
           if(bkgnd_bmp) create_bitmap();
         }
       }

       void   __fastcall TBmpBkgnd::set_mode  ( bkgnd_mode_t mode    )
       {
           if(bkgnd_mode != mode)
           {
            bkgnd_mode = mode;
            if(bkgnd_bmp) create_bitmap();
           }
       }

       void   __fastcall TBmpBkgnd::set_bitmap(HBITMAP bmp,bool del_src)
       {
          if(src_bmp != bmp)
          {
            if(del_src && src_bmp) DeleteObject(src_bmp);
            src_bmp = bmp;
            if(bkgnd_bmp) create_bitmap();
          }
       }

       void   __fastcall TBmpBkgnd::set_grid  ( int _grid_step)
       {
         if(this->grid != _grid_step)
         {
           grid = _grid_step;
           if(bkgnd_bmp) create_bitmap();
         }
       }

       void   __fastcall TBmpBkgnd::set_grayed  ( bool _grayed)
       {
         if(this->grayed != _grayed)
         {
           grayed = _grayed;
           if(bkgnd_bmp) create_bitmap();
         }
       }

       SIZE    __fastcall TBmpBkgnd::get_src_bitmap_size  ()
       {
         SIZE sz;
         if(this->src_bmp)
         {
          get_bitmap_size(src_bmp,sz);
         }
         else
         {sz.cx = width, sz.cy = height;}
         return sz;
       }


}

