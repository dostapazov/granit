#define WIN32_LEAN_AND_MEAN
#define STRICT
#include <WINDOWS.H>
#include <hdlcunit.h>

#pragma warn -par
BOOL WINAPI DllEntryPoint(HINSTANCE hInst,DWORD dwReason,LPVOID reserved)
{return TRUE;}
#pragma warn .par

#pragma warn -asc
__declspec(dllexport) unsigned short WINAPI NeedBytes(unsigned short len)
{
 asm xor dx,dx
 asm mov ax,len
 asm or ax,ax
 asm jz  __end
 asm mov cx,5
 asm div cx
 asm cmp dx,0
 asm jz  _mul
 asm inc al
_mul:
 asm inc cx
 asm mul cx
__end:
 asm inc ax   ;// Reserv space to flags
 asm inc ax

 return _AX;
}

__declspec(dllexport) unsigned short WINAPI MirrorBytes(char * buffer,unsigned short len)
{


 #ifdef __FLAT__
 asm push edx
 asm push ecx
 asm push esi
 asm push edi
 asm mov eax,buffer
 asm mov esi,eax
 asm mov edi,eax
 asm xor ecx,ecx
 asm xor edx,edx
 #else
 asm push dx
 asm push cx
 asm push si
 asm push di
 asm push ds
 asm push es
 asm mov ax,word ptr [buffer+2]
 asm mov ds,ax
 asm mov es,ax
 asm mov ax,word ptr[buffer]
 asm mov si,ax
 asm mov di,ax
 #endif
 asm mov ax,len
 asm mov cx,ax
//Mirroring  bytes ds:si -> pointer to buffer ,cx = len
//return len
 asm jcxz _END
 asm push cx
 asm pushf
 asm cld
_MAIN_LOOP:
 asm lodsb
 asm push cx
 asm mov cx,8
 asm xor ah,ah
_MIRROR:
 asm shl al,1
 asm rcr ah,1
 asm loop _MIRROR
 asm pop cx
 asm mov al,ah
 asm stosb
 asm loop _MAIN_LOOP
 asm pop ax
 asm popf
_END:
#ifdef __FLAT__
 asm pop edi
 asm pop esi
 asm pop ecx
 asm pop edx

#else
 asm pop es
 asm pop ds
 asm pop di
 asm pop si
 asm pop cx
 asm pop dx
#endif

 return _AX;
}



__declspec(dllexport) unsigned short WINAPI InsertBitStaffings(char * dest,unsigned short destlen,
                                  char * src,unsigned short srclen,
                                  char BeginFlag,char EndFlag)
{
 DWORD Res=0;
 Bytes2HDLC((LPBYTE)dest,destlen,(LPBYTE)src,srclen,(BYTE)BeginFlag,(BYTE)EndFlag,&Res,5);
 return (WORD)Res;
}

//Remove bitstaffings
__declspec(dllexport) unsigned short  WINAPI RemoveBitStaffings(char * dest,char * src,unsigned short len)
{
 DWORD Res = 0;
 HDLC2Bytes((LPBYTE)dest,len,(LPBYTE)src,len,&Res,5);
 return Res;
}

//calculates number of bit-staffings to need insert
unsigned short WINAPI BitStaffingsCount (char * src,unsigned short len)
{
 #ifdef __FLAT__
  asm push edi
  asm push ecx
  asm xor  ecx,ecx
  asm mov  esi,dword ptr src
 #else
  asm push ds
  asm push di
  asm push cx
  asm lds  si,dword ptr src
 #endif
 asm push bx
 asm push dx
 asm pushf
 asm cld

 asm xor dx,dx
 asm xor bx,bx

 asm mov cx,len
 asm jcxz _END
__MAIN_LOOP:
 asm push cx
 asm lodsb
 asm mov cx,8
_SHIFT_LOOP:
 asm shl al,1
 asm jnc _RESET_COUNT
 asm adc bx,0
 asm cmp bx,5
 asm jb _NEXT_SHIFT
 asm inc dx
_RESET_COUNT:
 asm xor bx,bx
_NEXT_SHIFT:
 asm loop _SHIFT_LOOP

 asm pop cx
 asm loop __MAIN_LOOP
_END:
 asm mov ax,dx

 asm popf
 asm pop dx
 asm pop bx
 #ifdef __FLAT__
  asm pop ecx
  asm pop edi
 #else
  asm pop cx
  asm pop di
  asm pop ds
 #endif

 return _AX;

}

#pragma warn .asc

#pragma warn -aus

 //Вставка Битстаффингов
__declspec(dllexport) BOOL WINAPI Bytes2HDLC(LPBYTE dest,DWORD destSz,LPBYTE src,DWORD srcSz,
                        DWORD BeginFlag,DWORD EndFlag,LPDWORD ResCount,DWORD lim )
 {
     WORD DST_BYTE   = 0;
     WORD DST_BIT    = 0;
     WORD SRC_BYTE   = 0;
     DWORD OneCount  = 0; //Кол-во единиц
     DWORD i,j;

   BOOL Ret = (BOOL)(destSz>2);
   *ResCount = 0;
   if(Ret)
   {
    *dest++ = (BYTE)BeginFlag;
    (*ResCount)++;
     for(i = 0;(i<srcSz) && ((*ResCount) < destSz);i++)
     {
      SRC_BYTE = (WORD)(*src++);
       for(j = 0;j<8;j++)
        {
          SRC_BYTE<<=1;
          DST_BYTE<<=1;
          DST_BIT++;
          if(SRC_BYTE&0x100)
            {
             OneCount++;
             DST_BYTE|=1;
            }
            else
            OneCount=0;

__OUT_BYTE:
           if(*ResCount < destSz)
           {
              if(DST_BIT == 8)
               {
                 *dest++ = DST_BYTE;
                 (*ResCount)++;
                 DST_BYTE = 0;
                 DST_BIT  = 0;
               }
           }
            else
             {Ret = FALSE;break;}


              if(OneCount>=lim)
              {
               DST_BYTE<<=1;
               DST_BIT++;
               OneCount = 0;
               goto __OUT_BYTE;
              }

        }
     }

     if(Ret)
     {
       //Завершающий флаг
        SRC_BYTE = (BYTE)(EndFlag);
         for(i = 0;i<8;i++)
         {
           if(*ResCount<destSz)
           {
              if(DST_BIT == 8)
               {
                 *dest++ = DST_BYTE;
                 (*ResCount)++;
                 DST_BYTE = 0;
                 DST_BIT  = 0;
               }
             DST_BYTE<<=1;
             SRC_BYTE<<=1;
             DST_BYTE|= (WORD)((SRC_BYTE&0x100) ? 1:0);
             DST_BIT++;
           }
            else
            {
             Ret = FALSE;break;
            }
         }
      if(Ret)
      {
        while(DST_BIT <8)
             {
              DST_BYTE<<=1;
              DST_BYTE|=1;
              DST_BIT++;
             }
       if(*ResCount<destSz)
         {
            *dest++ = DST_BYTE;
            (*ResCount)++;
            DST_BYTE = 0;
            DST_BIT  = 0;
         }
          else
           Ret = FALSE;
      }
     }
   }

   return Ret;
 }

__declspec(dllexport) BOOL WINAPI HDLC2Bytes(LPBYTE dest,DWORD destSz,LPBYTE src,DWORD srcSz,LPDWORD ResCount,DWORD lim )
 {
    DWORD i,j;
    WORD  DST_BYTE = 0;
    WORD  DST_BIT  = 0;
    WORD  SRC_BYTE = 0;
    DWORD OneCount = 0;
    
   BOOL Ret = (BOOL)(destSz>1);
   *ResCount=0;
   if(Ret)
   {

    for(i = 0;i<srcSz && Ret;i++)
    {
     SRC_BYTE = *src++;
      for(j=0;j<8;j++)
      {
       SRC_BYTE<<=1;
       DST_BYTE<<=1;
       DST_BIT++;
       if(SRC_BYTE&0x100)
         {
          DST_BYTE|=1;
          OneCount++;
         }
         else
         {
          if(OneCount>lim)
            {
             //Это конец!!!
             OneCount=lim+1;
             return TRUE;
            }
            else
             {
               if(OneCount == lim)
                 {
                 DST_BYTE>>=1;
                 DST_BIT--;
                 }
                 OneCount = 0;

             }

         }
          if(OneCount>lim)
            {
             //Это конец!!!
             OneCount=lim+1;
             return TRUE;
            }


        if(DST_BIT == 8)
        {
         if(*ResCount <destSz)
         {
             *dest++=DST_BYTE;
            (*ResCount)++;
             DST_BIT  = 0;
             DST_BYTE = 0;
         }
          else
           {Ret = FALSE;break;}

        }
      }
    }
   }
   return Ret;
 }
#pragma warn .aus

