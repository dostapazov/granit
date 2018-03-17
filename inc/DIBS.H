#if !defined ( __DIBS_FUNC_INC__ )
 #define __DIBS_FUNC_INC__

  #ifndef _WINDOWS_
   #define STRICT
   #include <Windows.h>
  #endif

  #include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ScanBytes(w,bpp)  ((int)((w*bpp+31)&(~31))/8)


const int TiffOK                        = 0,
          TiffCantOpen                  = 1,
          TiffNotImplement              = 2,
          TiffNoTiff                    = 3,
          TiffInvalidBitsPerSample      = 4,
          TiffInvalidSamples            = 5,
          TiffInvalidTag                = 6,
          TiffUnknownCompression        = 7,
          TiffInvalidDib                = 8
          ;



typedef void (* AppShowMessage)(const char * msg);

HANDLE __fastcall CreateDib(int w,int h,int BitPerPixel=8,bool Rgb=true,WORD memAttr = GMEM_MOVEABLE);
HANDLE __fastcall CreateDibFromHeader(BITMAPINFOHEADER * Header,WORD memAttr = GMEM_MOVEABLE);
void   __fastcall SetupGrayScale(RGBQUAD * First,int NumEntries = 256);

HANDLE __fastcall LoadTiff(const char * fName,int &Error);
bool   __fastcall SaveAsTiff( HANDLE dib,const char * fName,bool lzw=true,bool Pred = false);

HANDLE LoadJpeg(const char * file,AppShowMessage = 0);
bool SaveAsJpeg(HANDLE Handle,const char * filename ,int Quality = 100,AppShowMessage = 0);
BOOL   __fastcall SaveAsBitmap(HANDLE hDib,const char * fName);
HANDLE __fastcall LoadFromBitmap(const char * fName);


extern "C" HANDLE WINAPI ConvertToTrueColor(HANDLE Src,BOOLEAN Copy);
extern "C" HBITMAP WINAPI DibToDdb(HANDLE hDib,int width,int height,BOOLEAN FreeDib);

long __fastcall   NumColors(unsigned short bitCount);
unsigned short    NumBits(long colors);


#define INVALID_BITMAP_HEADER  -1
#define NO_HANDLE              -2

struct TBitmapInfo
{
 BITMAPINFO    * Info;
 RGBQUAD       * Colors;
 unsigned long * Mask;
 unsigned char * Bits;
 unsigned long   NumClrs;
 unsigned long   Mode;
 HANDLE        hBitmap;
#ifdef __cplusplus
       TBitmapInfo(HANDLE hBmp);
       TBitmapInfo();
  int __fastcall InfoFromHandle();
      ~TBitmapInfo();
#endif

};


int WINAPI InfoFromBitmapHandle(::TBitmapInfo *,HANDLE);

#ifdef __cplusplus
}
#endif



#ifdef __cplusplus
inline  TBitmapInfo::TBitmapInfo(HANDLE hBmp)
{
 ZeroMemory(this,sizeof(*this));
 InfoFromBitmapHandle(this,hBmp);
}

inline  TBitmapInfo::TBitmapInfo()
{
 ZeroMemory(this,sizeof(*this));
}


inline  TBitmapInfo::~TBitmapInfo()
{
 if(hBitmap)
  GlobalUnlock(hBitmap);
}
#endif

#endif

