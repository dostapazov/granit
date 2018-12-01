//---------------------------------------------------------------------------
#include <windows.h>

#ifndef __BORLANDC__
 #define asm _asm
//extern "C"  int WINAPI  _DllMainCRTStartup(int,int,int)
//{return 1;}

#endif

//Расчет 

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

#ifdef __BORLANDC__
#pragma warn - 8002
#endif

extern "C"
//__declspec(dllexport)
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

#ifdef __BORLANDC__
#pragma warn - 8002
#endif

extern "C"
//__declspec(dllexport)
unsigned short WINAPI  CalcKPK(void * Buffer,unsigned short len,unsigned short Del)
{
 unsigned short remind = CalcRemind(Buffer,len,Del);
   remind = WordRemind(0,Del,remind);
   remind = WordRemind(0,Del,remind);

  //_AX = (unsigned short)(Del-remind);
  unsigned short diff =  (unsigned short)(Del-remind);
  asm mov ax,diff
  asm xchg al,ah
  #ifdef __BORLANDC__
  return _AX;
  #else
  asm mov diff,ax
	  return diff;
#endif

}

#ifdef __BORLANDC__
#pragma warn -asc
#endif
extern "C"
//__declspec(dllexport)
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
        sal  ecx,1
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
four:   asm test byte ptr [esi],dl
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


#ifdef __BORLANDC__
#pragma warn .asc
#endif

