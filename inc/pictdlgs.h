//---------------------------------------------------------------------------
#ifndef openpictH
#define openpictH
//---------------------------------------------------------------------------
 #ifdef __cplusplus
  extern "C"{
 #endif

 DWORD  WINAPI  OpenPicture(HWND Owner, char * fName,DWORD fSz,char * title,char * initDir,POINT * TopLeft ,BOOLEAN);
 HANDLE WINAPI  LoadImageFromFile(char * FileName);
 void WINAPI    SaveDibPicture(HWND Owner,HANDLE hDib,char * title,char * InitDir,POINT * pos);
 void WINAPI    SaveImageToBmp(HANDLE hDib,char * fname);
 void WINAPI    SaveImageToTiff(HANDLE hDib,char * fname,BOOLEAN Lzw,BOOLEAN HorDiff);
 void WINAPI    SaveImageToJpeg(HANDLE hDib,char * fname,int QualityPercent);
 void WINAPI    FreeImage(HANDLE);
 HBITMAP WINAPI DibToDdb(HANDLE,int w,int h,BOOLEAN freeDib);
 HANDLE  WINAPI ConvertToTrueColor(HANDLE Src,BOOLEAN Copy);
 #ifdef __cplusplus
  }
 #endif

#endif
