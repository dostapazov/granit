#include "grnet_dos.h"
#include <crcunit.h>




void   __fastcall grnet_protect  (LPGRNET_KADR kadr)
{
 if(kadr && kadr->data_len<=GRNET_DATA_LEN)
 {
  kadr->secret_dword  = NetSecret;
  kadr->check_sum     = 0;
  kadr->check_sum     =  CalcKPK(kadr,(WORD)grnet_kadr_len(kadr),DEF_POLINOM_DEL);
 }
}

BOOL   __fastcall grnet_check    (LPGRNET_KADR kadr)
{
 if(kadr && kadr->data_len<=GRNET_DATA_LEN)
 {
   DWORD save_cs    = kadr->check_sum;
   kadr->check_sum = 0;
   kadr->check_sum = CalcKPK(kadr,(WORD)grnet_kadr_len(kadr),DEF_POLINOM_DEL);
   if(kadr->check_sum == save_cs)
      return TRUE;
      else
      kadr->check_sum = save_cs;
 }
 return false;
}

BOOL   __fastcall grnet_init_kadr (LPGRNET_KADR kadr,DWORD cmd,WORD kadr_num,WORD id,BOOL need_receipt,LPVOID data,DWORD data_len)
{
 if(kadr && data_len<GRNET_DATA_LEN)
 {
  kadr->secret_dword = NetSecret;
  kadr->receipt      = 0;
  kadr->need_receipt = need_receipt;
  kadr->command      = cmd;
  kadr->id           = id;
  kadr->retry_num    = 0;
  kadr->kadr_num     = kadr_num;
  if(data && data_len)
  {
   memcpy(kadr->data,data,data_len);
   kadr->data_len = (WORD)data_len;
  }
  else
  kadr->data_len = 0; 
  grnet_protect(kadr);
  return TRUE;
 }
 return FALSE;
}


