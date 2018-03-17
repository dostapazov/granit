#include <windows.h>
#include <freeimage.hpp>
#include <dll_procs.hpp>

#ifdef __WIN32__
#pragma link "freeimage32.lib"
#endif

#ifdef _WIN64
#pragma link "freeimage64.a"
#endif


namespace freeimage
{

  HBITMAP __fastcall TFreeImage::load_image(const char * image_name)
  {
    HBITMAP  ret     = NULL;
    FREE_IMAGE_FORMAT  img_fmt = FreeImage_GetFileType(image_name);
    if(img_fmt != FIF_UNKNOWN)
    {
     FIBITMAP *fbmp = FreeImage_Load(img_fmt,image_name,0);
     if(fbmp)
       {
        ret = DibToDdb(fbmp,0,0);
        FreeImage_Unload(fbmp);
       }
    }
    return  ret;
  }

  HBITMAP __fastcall TFreeImage::load_image(const wchar_t * image_name)
  {
    HBITMAP  ret     = NULL;
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

HBITMAP __fastcall TFreeImage::DibToDdb(FIBITMAP * fi_bmp,int width,int height)
{
 HDC ScreenDC = 0;
 HDC MemDC    = 0;
 HBITMAP Bmp  = 0;
 if(fi_bmp)
 {
  ScreenDC = CreateDC("DISPLAY",0,0,0);
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

}//end of namespace
