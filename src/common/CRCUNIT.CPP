//---------------------------------------------------------------------------
#include <windows.h>

#ifndef __BORLANDC__
 #define asm _asm
/*extern "C"  int WINAPI  _DllMainCRTStartup(int h ,int r,int)
{
  if(r == DLL_PROCESS_ATTACH)
	  DisableThreadLibraryCalls((HINSTANCE)h);
	return 1;
}*/

#endif



#pragma hdrstop
//---------------------------------------------------------------------------

unsigned short __fastcall WordRemind(unsigned char Val,unsigned short Del,unsigned short  Remind)
{
 unsigned long Temp=Remind;
 DWORD _Del = DWORD(Del);
 Temp<<=8;
 Temp+=(unsigned long)(Val);
 Temp= Temp%(_Del);
 Remind= (unsigned short)Temp;
 return Remind;
}



extern "C"
unsigned short WINAPI CalcRemind(void * Buffer,unsigned short len,unsigned short Del)
{
  // Расчет остатка
  // в AX - остаток
if(len && Del)
{
  asm push ecx
  asm push esi
  asm push edx
  asm push ebx
  asm pushf
  asm cld

  asm xor eax,eax
  asm mov edx,eax
  asm mov ecx,eax
  asm mov ebx,eax

  asm mov cx,len
  asm mov bx,Del
  asm mov esi,dword ptr Buffer
_REM_LOOP:
  asm mov eax,edx
  asm shl eax,8
  asm lodsb
  asm xor edx,edx
  asm div ebx
  asm loop _REM_LOOP
  asm mov eax,edx
  asm and eax,0xFFFF
  asm popf
  asm pop ebx
  asm pop edx
  asm pop esi
  asm pop ecx

#ifdef __BORLANDC__
        return _AX;

#else
        asm mov len,ax
        return len;

#endif
}
return 0;
}

extern "C"
unsigned short WINAPI  CalcKPK(void * Buffer,unsigned short len,unsigned short Del)
{
 unsigned short remind = CalcRemind(Buffer,len,Del);
   remind = WordRemind(0,Del,remind);
   remind = WordRemind(0,Del,remind);
   unsigned short diff =  (unsigned short)(Del-remind);
   return diff;

}

#ifdef __BORLANDC__
#pragma warn -asc
#endif
extern "C"
unsigned short WINAPI  CalcGranitKPK(void  *buff,unsigned short len,unsigned  short del)
{

asm{
        push esi
        push ebx
        push edx

        xor  ecx,ecx
        mov  cx,len      //;длина  посылки
        mov  esi,dword ptr buff
        xor  ebx,ebx
        mov  bx,del      //;полином делитель
        mov  edx,080h
        xor  eax,eax
        sal  ecx,1
        sal  ecx,1
        sal  ecx,1   //;байтовую длину в ECX * 8 - кол-во бит
       }
first:
        asm sal ax,1
        asm jc four
        asm test byte ptr [esi],dl
        asm jz second
five:   asm xor ax,bx
second: asm clc
        asm ror dl,1
        asm cmp dl,080h
        asm jne third
        asm clc
        asm inc esi
third:  asm loop first
        asm jmp end
four:
        asm test byte ptr [esi],dl
        asm jnz second
        asm jmp five
end:
        asm pop edx
        asm pop ebx
        asm pop esi
#ifdef __BORLANDC__
        return _AX;

#else
        asm mov len,ax
        return len;

#endif
}

/*

short sar( unsigned char * val)
{
 short ret = 0;
 if((*val)&0x01)
     ret++;
 (*val)>>=1;
 return ret;
}

short ror( unsigned char * val)
{
 int ret = sar(val);
 if(ret)
    (*val)|=0x80;
 return ret;
} 

short rol( unsigned char * val)
{
 int ret = sal(val);
 if(ret)
    (*val)|=0x0001;
 return ret;
}

unsigned short WINAPI  CalcGranitKPK2(void  *buff,unsigned short len,unsigned  short del)
{
 unsigned short  crc  = 0;
 unsigned char * array = (unsigned char *)buff;
 unsigned char   curr_byte;
 len*=8;
 unsigned char   temp = 0x80;
 unsigned char   or;
 unsigned short  sr;
 while(len>0)
 {
   len--;
   or = temp&(*array);
   sr = sal(&crc);
   if((or&& !sr) || (!or && sr))
     crc^=del;
     ror(&temp);
    if(temp == 0x80)
       array++;

 }
 return crc;
}
*/


/*
#ifndef __BORLANDC__
int WINAPI DllEntryPoint(HINSTANCE h, unsigned long r , void*)
{
  if(r == DLL_PROCESS_ATTACH)
	  DisableThreadLibraryCalls((HINSTANCE)h);
        return 1;
}
#endif
*/


#ifdef __BORLANDC__
#pragma warn .asc
#endif

