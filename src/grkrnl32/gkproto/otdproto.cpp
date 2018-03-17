/***************************************************************
 *
 * Модуль    : otdproto
 * Описание  : Вспомогательные функции работы с ОТД - данными
 * Автор     :Остапенко Д.В.
 * Версия    :1
 * Создан    :13.06.2002 15:46:34
 *
 ****************************************************************/


#include <kertl.hpp>
#include "otd_proto.h"

#ifdef __BORLANDC__
#pragma argsused
#endif
//---------------------------------------------------------------------------


   //Вычисление количества байт необходимых для param_count битовых параметров
   inline DWORD __fastcall _bytes_for_bitparam(DWORD param_count)
   {
    if(int(param_count)>0)
    {
     div_t d = div(int(param_count),8);
     return d.quot+(d.rem ? 1:0);
    }
    return 0;
   }

   DWORD WINAPI otd_bytes_for_bitparam(DWORD param_count)
   {
    return _bytes_for_bitparam(param_count);
   }

   bool __fastcall set_bit(LPBYTE array,DWORD sz,int bit_num,bool val)
   {
    div_t d = div(bit_num,8);
    if(DWORD(d.quot+((d.rem) ? 1:0))<=sz)
     {
      BYTE   mask  = 0x80>>d.rem;
      LPBYTE ptr = array+d.quot;
      bool ret = ((*ptr)&mask) ? true:false;
      if(val)
      (*ptr)|=mask;
      else
      (*ptr)&=~mask;
      return ret;
     }
     return false;
   }

   BOOL WINAPI otd_set_bit(LPBYTE array,DWORD asz,DWORD bit_num,BOOL val)
   {
    return BOOL(set_bit(array,asz,int(bit_num),val ? true:false));
   }


   bool __fastcall get_bit(LPBYTE array,DWORD sz,int bit_num )
   {
    div_t d = div(bit_num,8);
    if(DWORD(d.quot+((d.rem) ? 1:0))<=sz)
     {
      BYTE   mask  = 0x80>>d.rem;
      LPBYTE ptr = array+d.quot;
      bool ret = ((*ptr)&mask) ? true:false;
      return ret;
     }
     return false;

   }

   BOOL WINAPI otd_get_bit(LPBYTE array,DWORD asz,DWORD bit_num)
   {
    return BOOL (get_bit(array,asz,int(bit_num)));
   }

   DWORD WINAPI otd_parser   (LPBYTE src,DWORD cb,LPOTD_PARSED pd,DWORD psz)
   {
    DWORD version = 0;
    if(pd && psz == sizeof((*pd)) && src && cb>=sizeof(*pd->hdr))
    {
      ZeroMemory(pd,sizeof(*pd));
      pd->hdr = LPOTD_HEADER(src);
      src+=sizeof(*pd->hdr);
      pd->cb_size = sizeof(*pd->hdr);

      if(pd->hdr->flags&OTD_FL_VERSION)
         {
          //Версия
          pd->version = (LPWORD)src;
          src+=sizeof(WORD);
          version = *pd->version;
          pd->cb_size+=sizeof(WORD);
         }

      //Данные
      int param_count = pd->hdr->nums.nums == WORD(-1) ? 0 : OTDPARAMS_COUNT(&pd->hdr->nums);

      if(pd->hdr->addr.fa&OTDFA_BIT_EXTFA)
       {
        //Расширение ФА
        pd->extfa = src;
        pd->extfa_size = sizeof(DWORD);
        src+=sizeof(DWORD);
        pd->cb_size+=pd->extfa_size;
       }

      if(param_count >0 && (pd->hdr->flags&OTD_FL_DATA))
      {
         pd->data =    src;
         pd->data_size = pd->hdr->flags&OTD_FL_DATASIZE_MASK;
         if(pd->data_size)
         {
           pd->data_size = param_count*((1<<pd->data_size)/2);
         }
         else
         {
          //Размер данных выравнивается до  байта
          pd->data_size = ((param_count%8 == 0) ? 0:1) + param_count/8;
         }
         src+=pd->data_size;
         pd->cb_size+=pd->data_size;
      }

      if(param_count>0 && (pd->hdr->flags& OTD_FL_CHANGEMASK))
      {
       //Маска изменений
       pd->chmask      = src;
       pd->chmask_size = sizeof(WORD)+ (((param_count%8) == 0 ? 0:1) + param_count/8);
       src+= pd->chmask_size;
       pd->cb_size+=pd->chmask_size;

      }

      if(param_count>0 && (pd->hdr->addr.fa&OTDFA_BIT_EXTDIAG))
       {
        //Расширение Диагностики
        pd->extdiag = src;
        pd->extdiag_size = param_count + sizeof(WORD);
        src             +=(param_count + sizeof(WORD));
        pd->cb_size+=pd->extdiag_size;

       }

       if(pd->hdr->flags & OTD_FL_CONTROL)
       {
        pd->ctrl = src;
        pd->ctrl_size = sizeof(OTD_CTRL_0);
        src          +=sizeof(OTD_CTRL_0);
        pd->cb_size+=pd->ctrl_size;
       }

       if(pd->hdr->flags & OTD_FL_STATE)
       {
        pd->state      = src;
        pd->state_size = sizeof(OTD_STATE_0);
        src           +=sizeof(OTD_STATE_0);
        pd->cb_size+=pd->state_size;
       }

       pd->kpk = LPWORD(src);
       pd->cb_size+=sizeof(WORD);
       return pd->cb_size;
    }
    return FALSE;
   }

   DWORD WINAPI otd_maker    (LPOTD_PARSED op,DWORD psz,LPBYTE dst,DWORD cb)
   {
    //Формирование кадра
	LPWORD wptr;

    if(op && psz >= sizeof(*op) && dst && cb>=op->cb_size)
    {
     op->cb_size = 0;
     int param_count = op->hdr->nums.nums == WORD(-1) ? 0 : OTDPARAMS_COUNT(&op->hdr->nums);
     LPOTD_HEADER hdr = (LPOTD_HEADER)dst;
     CopyMemory (dst,op->hdr,sizeof(*op->hdr));//Скопировали заголовок
     dst+=sizeof(*op->hdr);
     op->cb_size+=sizeof(*op->hdr);
     hdr->flags = 0;
     if(op->version)
     {                  //Версия протокола
      hdr->flags|=OTD_FL_VERSION;
	  wptr = LPWORD(dst);
      *(wptr) = *op->version;
      dst+=sizeof(WORD);
      op->cb_size+=sizeof(WORD);
     }

     if(op->extfa)
      {                //Расширение ФА
       hdr->addr.fa|=OTDFA_BIT_EXTFA;
       CopyMemory(dst,op->extfa,op->extfa_size);
       dst+=op->extfa_size;
       op->cb_size+=op->extfa_size;
      }
       else
       hdr->addr.fa&=~OTDFA_BIT_EXTFA;

     if(op->data && op->data_size && param_count>0)
     {            //Данные
      hdr->flags |=(OTD_FL_DATA|(op->hdr->flags & OTD_FL_DATASIZE_MASK));
      CopyMemory(dst,op->data,op->data_size);
      dst +=op->data_size;
      op->cb_size+=op->data_size;
     }


     if(op->chmask && op->chmask_size && param_count>0)
      {      //Маска изменений
       hdr->flags|=OTD_FL_CHANGEMASK;
       CopyMemory(dst,op->chmask,op->chmask_size);
       dst +=op->chmask_size;
       op->cb_size+=op->chmask_size;
      }


     if(op->extdiag && op->extdiag_size &&  param_count>0)
     {
       hdr->addr.fa|=OTDFA_BIT_EXTDIAG;
       CopyMemory(dst,op->extdiag,op->extdiag_size);
       dst +=op->extdiag_size;
       op->cb_size+=op->extdiag_size;
     }
     else
       hdr->addr.fa&=~OTDFA_BIT_EXTDIAG;

     if(op->ctrl && op->ctrl_size)
        {
         hdr->flags|=OTD_FL_CONTROL;
         CopyMemory(dst,op->ctrl,op->ctrl_size);
         dst +=op->ctrl_size;
         op->cb_size+=op->ctrl_size;
        }
     if(op->state && op->state_size)
        {
         hdr->flags|=OTD_FL_STATE  ;
         CopyMemory(dst,op->state,op->state_size);
         dst +=op->state_size;
         op->cb_size+=op->state_size;
        }
      (*(LPWORD)dst) = WORD(0);
      op->cb_size+=sizeof(WORD);

     return op->cb_size;//Вернуть длину
    }
    return FALSE;
   }

   DWORD WINAPI otd_get_param(LPOTD_PARSED op,DWORD psz,DWORD pnum)
   {
    //Взять значение параметра
    DWORD ret(0);
    if(psz == sizeof(*op) && op && op->data && op->hdr)
    {
      //int param_count = op->hdr->nums.nums == WORD(-1) ? 0 : OTDPARAMS_COUNT(&op->hdr->nums);
      if(op->hdr->nums.lo_num<= BYTE(pnum) && op->hdr->nums.hi_num>=BYTE(pnum))
      {
        if(op->hdr->flags&OTD_FL_DATASIZE_MASK)
        {
          switch(op->hdr->flags&OTD_FL_DATASIZE_MASK)
          {
           case 1: ret = DWORD(*(op->data+pnum-DWORD(op->hdr->nums.lo_num)));break;
           case 2: ret = DWORD(*(op->data+(pnum-DWORD(op->hdr->nums.lo_num))*sizeof(WORD))) ;break;
           case 3: ret = DWORD(*(op->data+(pnum-DWORD(op->hdr->nums.lo_num))*sizeof(DWORD)));break;

          }
        }
        else
        {
         div_t v = div(pnum-DWORD(op->hdr->nums.lo_num),8);
         //v.quot -> номер байта
         //v.rem  -> номер бита
         BYTE  param_mask = 0x80>>v.rem;
         ret = DWORD(((*(op->data+v.quot)&param_mask))? 1:0);
        }
      }

    }
    return ret;
   }

   DWORD WINAPI otd_set_param(LPOTD_PARSED op,DWORD psz,DWORD pnum,DWORD param)
   {
    //Установить параметр
    DWORD ret(0);
    if(psz == sizeof(*op) && op && op->data && op->hdr)
    {

      if(op->hdr->nums.lo_num<= BYTE(pnum) && op->hdr->nums.hi_num>=BYTE(pnum))
      {
        if(op->hdr->flags&OTD_FL_DATASIZE_MASK)
        {
          switch(op->hdr->flags&OTD_FL_DATASIZE_MASK)
          {
           case 1: {LPBYTE  dst =           op->data+pnum-DWORD(op->hdr->nums.lo_num); ret = DWORD(*dst);*dst = BYTE(param);} break;
           case 2: {LPWORD  dst = (LPWORD  )op->data+(pnum-DWORD(op->hdr->nums.lo_num))*sizeof(WORD );ret = DWORD(*dst);*dst = WORD(param); };break;
           case 3: {LPDWORD dst = (LPDWORD )op->data+(pnum-DWORD(op->hdr->nums.lo_num))*sizeof(DWORD);ret = DWORD(*dst);*dst = (param); };break;

          }
        }
        else
        {
         div_t v = div(pnum-DWORD(op->hdr->nums.lo_num),8);
         //v.quot -> номер байта
         //v.rem  -> номер бита
         BYTE   param_mask = 0x80>>v.rem;
         BYTE   _param_mask = ~param_mask;
         LPBYTE dst = op->data+v.quot;
         ret = DWORD((*dst)&param_mask? 1:0);
         *dst&=_param_mask;
         if(param)
           *dst|=param_mask;
        }
      }

    }
    return ret;
   }

   //Копирование данных
   DWORD WINAPI otd_copy_data(LPOTD_PARSED dst,DWORD dst_sz,LPOTD_PARSED src,DWORD src_sz,BOOL compare)
   {
     DWORD changes_count(0);
     if(dst && dst_sz>=sizeof(*dst) && src && src_sz>=sizeof(*src))
     {

      if((dst->hdr->addr.fa&OTDFA_MASK_TYPE) == (src->hdr->addr.fa&OTDFA_MASK_TYPE))
      {
       LPOTD_CHMASK chm =  (compare && dst->chmask && dst->chmask_size >sizeof(WORD)) ? LPOTD_CHMASK(dst->chmask) :NULL;
       int begin_param = KeRTL::MAX(dst->hdr->nums.lo_num,src->hdr->nums.lo_num);
       int end_param = KeRTL::MIN(dst->hdr->nums.hi_num,src->hdr->nums.hi_num);
       if(chm)
         memset(dst->chmask,0,dst->chmask_size);
       for(;begin_param<end_param;begin_param ++)
       {
        DWORD p1 = otd_get_param(dst,sizeof(*dst),begin_param);
        DWORD p2 = otd_get_param(src,sizeof(*src),begin_param);
        if(p1!=p2)
          {
           changes_count++;
           otd_set_param(dst,sizeof(*dst),begin_param,p2);
           if(chm)
              otd_set_bit  (LPBYTE(chm->changes_mask),dst->chmask_size,begin_param,1);
          }
       }
       if(chm)
          chm->changes_count = WORD(changes_count);
      }
     }
     return changes_count;
   }


int   WINAPI otd_compare_addr(DWORD addr1,DWORD addr2)
{
  OTDADDR a1,a2;
  a1.addr = addr1;
  a2.addr = addr2;
  a1.fa  &= OTDFA_MASK_TYPE;
  a2.fa  &= OTDFA_MASK_TYPE;
  if(a1.addr-a2.addr)
  {
   int ret = int(a1.pu)- int(a2.pu);
   if(!ret)
   {
      ret = int(a1.kp)- int(a2.kp);
      if(!ret)
      {
       ret = int(a1.fa)- int(a2.fa);
       if(!ret)
        ret = int(a1.sb)- int(a2.sb);
      }
   }
   return ret;
  }
  return 0;
}

static DWORD __fastcall  _get_addr_element(char* s)
{
 if(s== 0 || *s=='%' || *s == '*' || *s == 0)
  return -1;
  DWORD  v = atoi(s);
  return v>=0 ? v:-1;
}

   //Преобразование  строки типа "1.49.2.0.3 " в адрес
   DWORD WINAPI otd_str2addr (LPOTDADDR addr,char    * str_addr)
   {
     char * s = (char *)str_addr;
     addr->addr = -1;
     addr->pu = _get_addr_element(s);
     s = strchr(s,'.');
     if(s)
     {
      s++;
      addr->kp = _get_addr_element(s);
       s = strchr(s,'.');
       if(s)
       {
        s++;
        addr->fa = _get_addr_element(s);
         s = strchr(s,'.');
         if(s)
         {
          s++;
          addr->sb = _get_addr_element(s);
         }
         s = strchr(s,'.');
         if(s)
           return _get_addr_element(s+1);

       }

     }
       return -1;
   }

   DWORD WINAPI otd_wstr2addr(LPOTDADDR addr,wchar_t * src)
   {

     char ansi_str[256];
     *ansi_str = 0;
     if(src)
     {
      KeRTL::Unicode2Ansi(ansi_str,src);
     }
     return otd_str2addr(addr,ansi_str);
   }

   inline int __fastcall _put_addr_element(BYTE el,char * out,char all_mask)
   {
    if(el<255)
      return wsprintf(out,"%d",DWORD(el));
      *out = all_mask;
      out++;
      *out = 0;
      return 1;
   }

   //Преобразование адреса в строку
   BOOL  WINAPI otd_addr2str (LPOTDADDR addr,char    * dst,DWORD sz,char  all_mask)
   {
    if(dst && sz>=4)
    {
    int t_len = 0,len;
    len =_put_addr_element(addr->pu,dst,all_mask);
    t_len+=len;
    dst  +=len;
    *dst = '.';
     dst++;
    if(t_len<int(sz))
    {
     len =_put_addr_element(addr->kp,dst,all_mask);
     t_len+=len;
     dst  +=len;
     *dst = '.';
      dst++;
      if(t_len<int(sz))
      {
       len =_put_addr_element(addr->fa,dst,all_mask);
       t_len+=len;
       dst  +=len;
       *dst = '.';
       dst++;
        if(t_len<int(sz))
          {
           len =_put_addr_element(addr->sb,dst,all_mask);
           dst  +=len;
           *dst = 0;;
          }

      }

    }
    }
    return false;

   }

   BOOL  WINAPI otd_addr2wstr(LPOTDADDR addr,wchar_t * dst,DWORD sz,wchar_t all_mask)
   {
     char ansi_dst[128];
     if(otd_addr2str(addr,ansi_dst,sizeof(ansi_dst),all_mask))
     {
      if(lstrlenA(ansi_dst)+1<=int(sz))
       {
        KeRTL::Ansi2Unicode(dst,ansi_dst);
        return TRUE;
       } 
     }
     return FALSE;
   }



