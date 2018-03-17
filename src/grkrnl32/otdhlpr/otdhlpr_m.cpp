#define WIN32_LEAN_AND_MEAN
#include <windows.h>

/*
#ifdef _DEBUG
#include <otd_proto.h>


unsigned short __fastcall WordRemind(unsigned char Val,unsigned short Del,unsigned short  Remind);

extern "C"
unsigned short WINAPI CalcRemind2(void * Buffer,unsigned short len,unsigned short Del);

extern "C"
unsigned short WINAPI CalcKPK2(void * Buffer,unsigned short len,unsigned short Del)
{
 unsigned short remind  = CalcRemind2(Buffer,len,Del);

 remind = WordRemind(0,Del,remind);
 remind = WordRemind(0,Del,remind);

  //_AX = (unsigned short)(Del-remind);
  unsigned short diff =  (unsigned short)(Del-remind);

  asm xor eax,eax;
  asm mov ax,diff
  asm xchg al,ah
  #ifdef __BORLANDC__
  return _AX;
  #else
  asm mov diff,ax
  return diff;
#endif

}



void test ()
{
  char * text = "Dimka Good Boy!";
  char buff[512];
  int len = strlen(text);
  memcpy(buff,text,len);
  WORD crc1 = otd_calc_checksum(buff,len,OTD_DEF_POLINOM);
  WORD crc2 = CalcKPK2(buff,len,OTD_DEF_POLINOM);
  memcpy(buff+len,&crc1,sizeof(crc1));
  WORD crc3 = otd_calc_checksum(buff,len+sizeof(crc1),OTD_DEF_POLINOM);
  WORD crc4 = CalcKPK2(buff,len+sizeof(crc2),OTD_DEF_POLINOM);
  if(crc1 == crc2)
     crc1 = crc2;
  if(crc3 == crc4)
     crc3 = crc4;
}

#endif
*/


#pragma warn -8057
extern "C" BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fwdreason, LPVOID lpvReserved)
{
 /*#ifdef _DEBUG
 test();
 #endif
 */


 if(fwdreason == DLL_PROCESS_ATTACH)
    DisableThreadLibraryCalls(hinstDLL);
  return TRUE;
}
#pragma warn .8057



