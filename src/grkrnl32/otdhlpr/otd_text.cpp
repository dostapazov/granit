#ifdef __linux__
#include <gklib/otd_base.h>
#include <wchar.h>

#else
#include <otd_base.h>
 #ifndef __restrict
  #define __restrict
 #endif
 #define wcsncasecmp _wcsnicmp
#endif

#include <string.h>

WORD winapi otd_text_get_length  ( lpotd_block  b)
 {
     if(OTD_BLOCK_ISTEXT(b))
      {
         lpotd_text t = (lpotd_text)b->block_data;
          return t->len;
      }
   return 0;
 }



// Инициализация блока содержащего текст
lpotd_block  WINAPI otd_text_init (LPVOID __restrict buf,int buff_sz,wchar_t * __restrict init_text)
{
 lpotd_block bl =(lpotd_block)buf;
 if(bl && buff_sz>(int)sizeof(*bl)-1)
 {
  OTD_BLOCK_INIT(bl,OTD_BLOCKTYPE_TEXT,buff_sz);
  lpotd_text ot  = (lpotd_text)bl->block_data;
  WORD max_len   = OTD_GET_TEXT_CAPASITY(bl);
  WORD text_len  = init_text ? wcslen(init_text):0;
  ot->len = KeRTL::MIN(max_len,text_len);
  memcpy(ot->text,init_text,ot->len*sizeof(wchar_t));
  return bl;
 }
 return NULL;
}

//Объединение 2 строк
int  WINAPI otd_text_merge(lpotd_block __restrict dest_block,lpotd_block __restrict src_block)
{
 if(dest_block && src_block && OTD_BLOCK_ISTEXT(src_block))
 {
  LPBYTE bptr = src_block->block_data;
  lpotd_text _src  = (lpotd_text)bptr;
  bptr = dest_block->block_data;
  lpotd_text _dest = (lpotd_text)bptr;
  DWORD max_len = OTD_GET_TEXT_CAPASITY(dest_block);
  if(max_len)
  {
    max_len-=_dest->len;
    DWORD copy_len = KeRTL::MIN(max_len,(DWORD)OTD_GET_TEXT_LENGTH(src_block));
    memcpy(_dest->text+(DWORD)_dest->len,_src->text,copy_len*sizeof(wchar_t));
    _dest->len+=(WORD)copy_len;
    return copy_len;
  }
 }
 return 0;
}

/*Копирование строки*/
int  WINAPI otd_text_copy (lpotd_block __restrict dest,lpotd_block __restrict src)
{

 if(dest && src && OTD_BLOCK_ISTEXT(dest) && OTD_BLOCK_ISTEXT(src))
 {
  lpotd_text _dest =(lpotd_text)dest->block_data;
  _dest->len = 0;
  return otd_text_merge(dest,src);
 }
 return 0;
}




int  WINAPI otd_text_compare(lpotd_block __restrict t1,lpotd_block __restrict t2,BOOL case_sens)
{
 int ret=0;
 if(t1 && t2 && OTD_BLOCK_ISTEXT(t1) && OTD_BLOCK_ISTEXT(t2))
 {
   lpotd_text _t1 = (lpotd_text)t1->block_data;
   lpotd_text _t2 = (lpotd_text)t2->block_data;
   ret = (int)(_t1->len - _t2->len);
   if(!ret)
   {
    WORD comp_len  = KeRTL::MIN(_t1->len,_t2->len);

    ret = case_sens ? wcsncasecmp(_t1->text,_t2->text,comp_len) : wcsncmp(_t1->text,_t2->text,comp_len);

   }
 }
 return ret;
}

int  WINAPI otd_text_getwstr(lpotd_block __restrict bl,wchar_t * __restrict str,int str_sz)
{
 int len = OTD_GET_TEXT_LENGTH(bl);
 if(bl && len <str_sz && str)
 {
  lpotd_text  t = (lpotd_text)bl->block_data;
  memcpy(str,t->text,len*sizeof(wchar_t));
  str[len]=0;
  return len;
 }
 return 0;
}

int  WINAPI otd_text_set    (lpotd_block bl,wchar_t * str)
{
 if(bl && bl->block_type == OTD_BLOCKTYPE_TEXT)
 {
  lpotd_text ltxt = (lpotd_text)bl->block_data;
  ltxt->len       = KeRTL::MIN((WORD)(1+wcslen(str)),(WORD)OTD_TEXT_GET_CAPASITY(bl));
  wcsncpy(ltxt->text,str,ltxt->len);
  return (int)ltxt->len;
 }
 return 0;
}




