#include "granit_proto.h"
#include <stdlib.h>

DWORD WINAPI check_norm_inverse(LPBYTE data,DWORD data_len)
{
 /*Проверка прямой и инверсной части*/
 data_len>>=1;
 LPBYTE inv = data+data_len;
 LPBYTE stop_ptr = inv;
 while(data<stop_ptr)
 {
  if((BYTE)(*data++)-(BYTE)(~(*inv++))) return GRPP_ERR_INVPART;

 }
 return GRPP_NOERROR;
}

/*Формирование инверсной части*/
DWORD WINAPI gr_proto_fill_inverse(LPBYTE src,LPBYTE dst,DWORD len)
{
 if(src && dst)
 {
 LPBYTE src_end = src+len;
 while(src<src_end)
    {*dst = ~*src;dst++;src++;}
 return len;
 }
 return 0;    
}

DWORD WINAPI gr_proto_val2poscode(DWORD   value,LPBYTE pc,DWORD pc_size)
{
 /*Преобразование числа в позиционный код*/
 div_t dt = div((int)value,8);
 if(pc && pc_size && dt.quot<(int)pc_size)
   {
    ZeroMemory(pc,pc_size);
    pc[dt.quot] = (0x80>>dt.rem);
    return pc_size;
   }
   return 0;
}

DWORD WINAPI gr_proto_poscode2val(LPBYTE pc,DWORD pc_size)
{
 //Преобразование позиционного кода в число
 DWORD byte = 0,bit = 0;
 BYTE  val ;
 for(;pc && byte<pc_size;byte++)
    {
     val = *pc;
     if(val)
      {
       while(bit<8 && !(val&0x80))
         {val<<=1;bit++;}
       return DWORD(byte*8+bit);
      }
      pc++;
    }

  return -1;
}

//#ifdef __BORLANDC__
//#pragma warn -8070
//#pragma warn -8057
//#endif


//__declspec(naked) DWORD WINAPI gr_get_ts_old(LPBYTE data,DWORD number)
//{
//  _asm{
//      push  ebp
//      mov   ebp,esp;
//      push  esi
//      push  edx
//      push  ecx
//
//      xor   edx,edx
//      xor   eax,eax
//      mov   eax,DWORD ptr [ebp+12];
//      mov   ecx,16
//      div   cx       ;// EDX - номер бита
//      shl   eax,1    ;// EAX - номер байта
//      mov   esi,dword ptr [ebp+8]
//      add   esi,eax
//      mov   ecx,edx
//      mov   edx,08000h
//      shr   edx,cl
//      lodsw
//      xchg  al,ah
//      and   eax,edx
//      jz    __1
//      mov   eax,1
//      }
//__1:
//_asm{
//      pop   ecx
//      pop   edx
//      pop   esi
//
//      pop  ebp
//      ret 0x08;
//  }
//}

 DWORD WINAPI gr_get_ts(LPBYTE data,DWORD number)
 {
   div_t div_res = div(number,8);
   data+=div_res.quot;
   BYTE mask  = 0x80>>div_res.rem;
   return ((*data)&mask) ? TRUE : FALSE;
 }


//#ifdef __BORLANDC__
//#pragma warn -8070
//#pragma warn -8057
//#endif
//
//__declspec(naked) void  WINAPI gr_set_ts(DWORD value,LPBYTE data,DWORD number)
//{
//
//  _asm{
//      push  ebp
//      mov   ebp,esp;
//      push  esi
//      push  edi
//      push  edx
//      push  ecx
//
//      xor   edx,edx
//      xor   eax,eax
//      mov   eax,DWORD ptr [ebp+16];
//      mov   ecx,16
//      div   cx                      ;// EDX - номер бита  EAX - номер байта
//      mov   esi,dword ptr [ebp+12]
//      shl   eax,1
//      add   eax,esi
//      mov   esi,eax
//      mov   edi,eax
//      lodsw
//      xchg  ah,al
//      mov   ecx,edx
//      mov   edx,08000h
//      shr   edx,cl
//      test  DWORD ptr [ebp+8],-1
//      jz    set_zero
//      or    eax,edx
//      jmp   short  __1
//set_zero:
//      not   edx;
//      and   eax,edx
//      }
//
//__1:
//_asm{
//      xchg  ah,al
//      stosw
//
//      pop   ecx
//      pop   edx
//      pop   edi
//      pop   esi
//
//      pop  ebp
//      ret 0x0C;
//  }
//}

 void  WINAPI gr_set_ts(DWORD value,LPBYTE data,DWORD number)
 {

   div_t   div_res = div(number,8);
   data+=div_res.quot;
   BYTE   v = 0x80>>div_res.rem;
   if(value)
      (*data)|=v;
      else
      (*data)&=~v;
 }


BOOL WINAPI zero_check(LPBYTE kadr,DWORD kadr_len)
{
 LPBYTE kadr_end = kadr+kadr_len;
 while(kadr<kadr_end)
   {
    if(*kadr) return TRUE;
    ++kadr;
   }
 return FALSE;
}


DWORD WINAPI gr_proto_parser(LPBYTE kadr,DWORD kadr_len,lpgr_proto gp)
{
 //Разбор кадра на составные части
 DWORD ret = GRPP_NOERROR;
 if(kadr && kadr_len  && !IsBadReadPtr(kadr,kadr_len))
 {
   if(gp && gp->dw_size == sizeof(*gp))
   {
     ZeroMemory(gp,sizeof(*gp));
     gp->dw_size  = sizeof(*gp);
     gp->kadr_len = kadr_len;
     gp->cp   = (lpgr_proto_cp)kadr;
     gp->kpk  = (LPWORD)(kadr+kadr_len-sizeof(*gp->kpk));
     if(zero_check(kadr,kadr_len))
     {
     if(gr_calc_kpk(kadr,(WORD)kadr_len,(WORD)STD_GRANIT_POLINOM)==0)
     {
      kadr        +=sizeof(*gp->cp);
      gp->kadr_len =sizeof(*gp->cp)+sizeof(*gp->kpk);
      gp->type  = (lpgr_proto_type)kadr;
      kadr        +=sizeof(*gp->type);
      gp->kadr_len+=sizeof(*gp->type);
      /*Определение дальнейшего*/
      if(gp->type->ktype<=GRKT_ERROR && gp->type->ktype!= 0x02 && gp->type->ktype!= 0x06 && gp->type->ktype!= 0x07)
      {
        if(gp->type->ktype&GRKT_DATAIN)
        {
           gp->info = (lpgr_proto_info)kadr;
           gp->kadr_len+=sizeof(*gp->info);
           kadr+=sizeof(*gp->info);
        }
           gp->data_len = (DWORD)((LPBYTE)gp->kpk-kadr);
           gp->kadr_len+=gp->data_len;

           if(gp->data_len)
              {
               gp->data = kadr;
               if(gp->info && gp->info->inf == GRINF_TS)
                 ret = check_norm_inverse(gp->data,gp->data_len);
              }
      }
      else
       ret = GRPP_ERR_INVALID_TYPE, gp->kadr_len = kadr_len;

     }
     else
     ret = GRPP_ERR_KPK;
    }
    else
     ret = GRPP_ZERO_KADR;
   }
   else
   ret = GRPP_ERR_STRUCT_SIZE;
 }
 else
 ret = GRPP_ERR_INVALID_BUFFER;
 return ret;
}

/*Формирование  кадра протокола гранит*/
DWORD  WINAPI   gr_proto_format (BYTE cp,BYTE type,BYTE info,
                                 LPBYTE buff,DWORD bsz,
                                 lpgr_proto gp)
{
  DWORD ret       = GRPP_NOERROR;
  if(buff && bsz>=GRSC_MIN_FRAME_SIZE)
  {
   if(gp && gp->dw_size == sizeof(*gp))
   {
    ZeroMemory(gp,sizeof(*gp));gp->dw_size = sizeof(*gp);

    gp->cp        = (lpgr_proto_cp)buff;
    gp->cp->cp_number = cp;
    buff         += sizeof(*gp->cp);
    gp->kadr_len += sizeof(*gp->cp);
    gp->type      = (lpgr_proto_type)buff;
    gp->type->afb_type = type;
    buff         += sizeof(*gp->type);
    gp->kadr_len += sizeof(*gp->type);
    /*Проверка типа кадра и возможности наличия данных*/
    DWORD pdl = 0; /*Длина данных протокола*/
    switch(gp->type->ktype)
    {
     case GRKT_CALL    :/*Все вызовы  кроме ВФА имеют 8 байт данных*/
     if(gp->type->afb<GRKTAFB_CALLTS || gp->type->afb > GRKTAFB_CALLTII )
        pdl = 8;
     break;
     case GRKT_DATAOUT :
     case GRKT_DATAIN  :
     {
      /*Размер данных зависит от типа данных*/
       gp->info = (lpgr_proto_info)buff;
       gp->info->grp_inf = info;
       gp->kadr_len+=sizeof(*gp->info);
       buff+=sizeof(*gp->info);
       pdl = 16;/*16 тит, тии, 8 ТС + 8~ТС */
     }
     break;
     case  GRKT_NODATA: // 2 байта 0
     case  GRKT_ERROR :
                       pdl = 2;
     break;
    }
    if(gp->kadr_len+pdl+sizeof(WORD)<=bsz)
    {
      gp->data_len = pdl;
      gp->kadr_len+= gp->data_len+sizeof(WORD);
      gp->data = buff;
      buff+=pdl;
      gp->kpk =(LPWORD)buff;
     *gp->kpk = 0;
    }
    else
    ret = GRPP_ERR_INVALID_BUFFER;   
   }
   else
   ret = GRPP_ERR_STRUCT_SIZE;
  }
  else
  ret = GRPP_ERR_INVALID_BUFFER;
 return ret;
}

/*Формирование кадра ТУ*/
/*Возвращает длину кадра*/
DWORD  WINAPI    gr_proto_tu          (BYTE   cp,BYTE tuop,BYTE tu_group,BYTE tu_obj,LPBYTE buff,DWORD bsz)
{
 gr_proto gp;
 gp.dw_size = sizeof(gp);
 if(gr_proto_format(cp,GRPROTO_MKTYPE(GRKT_CALL,tuop),0,buff,bsz,&gp)==GRPP_NOERROR)
  {
    lpgr_proto_data_tu dtu = (lpgr_proto_data_tu)gp.data;
    gr_proto_val2poscode(tu_group,dtu->pc_group,sizeof(dtu->pc_group));
    gr_proto_val2poscode(tu_obj,dtu->pc_object,sizeof(dtu->pc_object));
    BYTE inv_afb[2];
    gr_proto_val2poscode (tuop-1,inv_afb,sizeof(inv_afb));
    gr_proto_fill_inverse(inv_afb,dtu->pc_invafb,sizeof(inv_afb));
    gr_proto_fill_inverse(dtu->pc_group,dtu->pc_invgroup,sizeof(dtu->pc_group));
    gr_proto_fill_inverse(dtu->pc_object,dtu->pc_invobject,sizeof(dtu->pc_invobject));
    gr_proto_protect(&gp);
    return gp.kadr_len;
  }
 return 0;
}

//
//#ifndef __BORLANDC__
// #define _asm __asm
//#endif
//
//WORD WINAPI gr_calc_kpk_old (void  *buff  ,unsigned short len,unsigned short del)
//{
//#ifdef __WIN32__
//_asm{
//        push esi
//        push ebx
//        push edx
//
//        xor  ecx,ecx
//        mov  cx,len      //;длина  посылки
//        mov  esi,dword ptr buff
//        xor  ebx,ebx
//        mov  bx,del      //;полином делитель
//        mov  edx,080h
//        xor  eax,eax
//        sal  ecx,1
//        sal  ecx,1
//        sal  ecx,1   //;байтовую длину в ECX * 8 - кол-во бит
//       }
//first:
//        _asm sal ax,1
//        _asm jc four
//        _asm test byte ptr [esi],dl
//        _asm jz second
//five:   _asm xor ax,bx
//second: _asm clc
//        _asm ror dl,1
//        _asm cmp dl,080h
//        _asm jne third
//        _asm clc
//        _asm inc esi
//third:  _asm loop first
//        _asm jmp end
//four:
//        _asm test byte ptr [esi],dl
//        _asm jnz second
//        _asm jmp five
//end:
//        _asm pop edx
//        _asm pop ebx
//        _asm pop esi
//        _asm xchg al,ah
// #ifdef __BORLANDC__
//        return _AX;
// #else
//        _asm mov len,ax
//        return len;
// #endif
//#endif
// return 0;
//}


inline bool __fastcall __sal(WORD & val)
{
  bool ret = (val&0x8000) ? true : false;
  val<<=1;
  return ret;
}

inline bool __fastcall __test(BYTE b1,BYTE b2)
{
   return (b1&b2) ? true : false;
}

inline bool __fastcall __cmp(BYTE b1,BYTE b2)
{
  return b1 == b2 ? true : false;
}

WORD WINAPI gr_calc_kpk(void  *buff  ,unsigned short len,unsigned short del)
{
  LPBYTE ptr = (LPBYTE)buff;
  LPBYTE end = ptr+len;
  WORD ax   = 0;
  while(ptr<end)
  {
   BYTE dl   = 0x80;
   while(dl)
   {
    if(__sal(ax) != __test(*ptr,dl))
        ax^=del;
     dl>>=1;
   }
   ++ptr;
  }
  ax = ((ax>>8)&0xFF)|((ax<<8 )&0xFF00);
  return ax;
}

DWORD  WINAPI   gr_proto_protect(lpgr_proto gp)
{
 DWORD ret = GRPP_NOERROR;
 if(gp && gp->dw_size == sizeof(*gp))
 {
   if(gp->kpk)
     *gp->kpk = gr_calc_kpk        (gp->cp,(WORD)gp->kadr_len-sizeof(WORD),(WORD)STD_GRANIT_POLINOM);
     else
     ret = GRPP_ERR_INVALID_BUFFER;
 }
 else
 ret = GRPP_ERR_STRUCT_SIZE;
 return ret;
}


