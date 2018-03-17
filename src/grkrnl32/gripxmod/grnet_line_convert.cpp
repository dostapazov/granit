/*Функции преобразования протоколов*/

//#include "gripxmod.hpp"
#include "gripxmod_main.hpp"
#include <stdlib.h>
#include <otd_grancvt.h>


DWORD   __fastcall convert_ready(DWORD pu_number,lp_gratony_kadr atony_kadr,LPBYTE dest,DWORD dest_sz,diag_map * diag)
{
  otd_proto op;
  op.dw_size = sizeof(op);
  DWORD ret = otd_proto_format(dest,dest_sz,OTD_PROTO_PART_DIAG|OTD_PROTO_PART_KPK,0,0,0,0,0,0,&op);
  if(ret)
  {
   op.addr->pu = pu_number;
   op.addr->cp = atony_kadr->numCp;
   op.addr->fa = OTD_FA_ALL;
   op.addr->sb = atony_kadr->numGrp == 0x7F ? -1:atony_kadr->numGrp;
   *op.diag =   diag->get_otd_diag(op.addr);
   otd_proto_protect(&op,ret,OTD_DEF_POLINOM);
  }
 return ret; 
}

int     __fastcall grnet_line::grnet2otd(lp_gratony_kadr atony_kadr,LPBYTE dest,DWORD dest_sz)
{
 //Преобразование кадра "Гранит net "  к виду OTD
 int ret = 0;
 TGRNetModule * mod  =(TGRNetModule *)owner;
 switch(atony_kadr->InfoType)
 {
  case tpTs   :
  case tpTit  :
                {
                 otd_proto op;
                 op.dw_size = sizeof(op);
                 ret = otd_convert_atony2otd(pu_number,atony_kadr,0,dest,dest_sz,TRUE,&op);
                 //Именно |= так как
                 *op.diag   |= this->diag->get_otd_diag(op.addr);
                 otd_proto_protect(&op,op.proto_size,OTD_DEF_POLINOM);
                 op.dw_size = sizeof(op);
                }
                break;
  case tpReady: ret = ::convert_ready(pu_number,atony_kadr,dest,dest_sz,diag);
               break;
  case tpAll  :
               {

                 otd_proto op;
                 otd_addr  addr;
                 op.dw_size = sizeof(op);
                 OTD_SET_ADDR(&addr,pu_number,atony_kadr->numCp,OTD_FA_QUERY|atony_kadr->data[0],atony_kadr->numGrp == 0x7F ? -1:atony_kadr->numGrp );
                 ret =
                 otd_proto_format_query(dest,dest_sz,&addr,0,OTD_PROTO_PART_DATA|OTD_PROTO_PART_PERSONAL_DIAG|OTD_PROTO_PART_DIAG,(LPVOID)&op);
                 otd_proto_protect(&op,op.proto_size,OTD_DEF_POLINOM);
               }
               break;             
 }

 return ret;

}
void __fastcall grnet_line::make_grnet_query(DWORD cp,DWORD fa,DWORD grp)
{
    gratony_kadr kadr;
    ZeroMemory(&kadr,sizeof(kadr));
    kadr.kadr_len = sizeof(kadr);
    kadr.len = kadr.kadr_len-sizeof(WORD);
    kadr.channel  = get_number();
    kadr.numCp    = (BYTE)cp;
    kadr.numGrp   = (BYTE)grp;
    kadr.InfoType =  tpAll;
    kadr.data[0]  =  fa;
    queue_rxdata((LPBYTE)&kadr,kadr.kadr_len,false);
}    



 void    __fastcall grnet_line::make_queryes(lpotd_addr addr)
 {
  /*Формирование запросов*/
  gratony_kadr kadr;
  ZeroMemory(&kadr,sizeof(kadr));
  kadr.kadr_len = kadr.len = sizeof(kadr);
  if(addr->cp == OTD_ADDR_MAXVALUE && addr->sb == OTD_ADDR_MAXVALUE && OTD_FA_ISQUERY_ALL(addr->fa))
  {
   /*Вызов данных по всем существующим КП*/
   send_data(CmQuery,0,0);
   return;
  }
 }




 
