#ifndef __FREE_IMAGE32_HPP__
#define __FREE_IMAGE32_HPP__
#include <freeimage32.h>



namespace freeimage
{
  class TFreeImage
  {
    public:
    static HBITMAP __fastcall load_image(const char    * image_name);
    static HBITMAP __fastcall load_image(const wchar_t * image_name);
    static HBITMAP __fastcall DibToDdb(FIBITMAP * fi_bmp,int width,int height);
  };

}
#endif
