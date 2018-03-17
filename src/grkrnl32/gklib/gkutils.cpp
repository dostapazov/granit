#include <gkmodules.h>
#include <kertl.hpp>
using namespace KeRTL;

LPMODULE_CONFIG_ITEM WINAPI setup_module_config_item(LPVOID buffer,DWORD bsz,
                                                     BYTE i0,BYTE i1,BYTE i2,BYTE i3,wchar_t * iname,
                                                     DWORD item_type,LPVOID item,DWORD item_sz,
                                                     wchar_t * comment)
{

 if(item_sz)
 {
  int iname_len    = (iname   && *iname)   ? (lstrlenW(iname)+1):0;
  int comment_len  = (comment && *comment) ? (lstrlenW(comment)+1):0;
  if((iname_len+comment_len+item_sz+sizeof(MODULE_CONFIG_ITEM)-1)<bsz)
  {
   LPMODULE_CONFIG_ITEM mci = LPMODULE_CONFIG_ITEM(buffer);
   INIT_MCITEM(mci,bsz);
   mci->i0 = i0,mci->i1 = i1,mci->i2 = i2,mci->i3 = i3;
   mci->item_name_sz = iname_len*sizeof(wchar_t);
   mci->item_type    = (WORD)item_type;
   mci->item_sz      = (WORD)item_sz;
   mci->item_comment_sz = comment_len*sizeof(wchar_t);
   if(iname_len)
     safe_strcpy(GET_MCITEM_NAME(mci),iname);
   memcpy(GET_MCITEM_VALUE(mci),item,item_sz);

   if(comment_len)
     safe_strcpy(GET_MCITEM_COMMENT(mci),comment);
    return mci;
  }
 }
 return NULL;
}
