#include <base64endecode.h>
#include <stdlib.h>

#pragma pack (push,1)
union word_bt
{
 unsigned short word;
 struct {unsigned char lo_b,hi_b;};
};

#pragma pack(pop)

char base64_tbl[64]= { 'A','B','C','D','E','F','G','H', 'I','J','K','L','M','N','O','P', 'Q','R','S','T','U','V','W','X', 'Y','Z','a','b','c','d','e','f', 'g','h','i','j','k','l','m','n', 'o','p','q','r','s','t','u','v', 'w','x','y','z','0','1','2','3', '4','5','6','7','8','9','+','/'};
DWORD WINAPI Base64Encode(LPVOID in_buff,DWORD in_sz,LPVOID out_buff,DWORD out_buff_sz)
{
  DWORD ret(0);
  word_bt wb;
  wb.word = 0;
  LPBYTE src = (LPBYTE)in_buff;
  LPBYTE dst = (LPBYTE)out_buff;
  int bit = 0;
  for(DWORD i = 0;i<in_sz && int(out_buff_sz)>0;i++)
  {
    wb.lo_b = *src++;
    for(int j = 0;j<8;j++)
    {
     wb.word<<=1;
     bit++;
     if(bit==6)
       {
        wb.hi_b&=0x3F;
        *(dst++) = base64_tbl[wb.hi_b];
        ret++;out_buff_sz--;
        bit -= bit;
       }
    }

  }
  if(bit)
  {
   wb.word<<=6-bit;
   *(dst++) = base64_tbl[wb.hi_b&0x3F];
   ret++;
  }
  while(ret%4)
    {
     *(dst++)= '=';
      ret++;
    }
  return ret;
}

DWORD __fastcall get_base64index(DWORD ch )
{
 LPBYTE p_beg=(LPBYTE)base64_tbl,p_end = (LPBYTE)p_beg+64;
 while(p_beg<p_end)
 {
  if(DWORD(*p_beg)==ch)
     break;
     p_beg++;
 }
 return DWORD(p_beg)-DWORD(base64_tbl);
}

DWORD WINAPI Base64Decode(LPVOID in_buff,DWORD in_sz,LPVOID out_buff,DWORD out_buff_sz)
{
 //Вычисяем необходимый размер временного буфера

 DWORD ret(0);
 LPBYTE src = (LPBYTE)in_buff;
 LPBYTE dst = (LPBYTE)out_buff;
 word_bt wb;
 wb.word       = 0;
 DWORD     bit = 0;
 for(DWORD i = 0;i<in_sz && *src!='=' && int(out_buff_sz)>0;i++)
 {
   wb.lo_b |=  get_base64index(*(src++))<<2;
   for(int j = 0;j<6;j++)
   {
    wb.word<<=1;bit++;
    if(bit==8)
    {
     *(dst++) = wb.hi_b;
     ret++;bit = 0;out_buff_sz--;
    }
   }
 }
 return ret;
}

 
