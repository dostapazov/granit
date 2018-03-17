/*Функции преобразования протоколов*/
#pragma hdrstop
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
   op.addr->fa = atony_kadr->S&OTD_FA_ALL;
   op.addr->sb = atony_kadr->numGrp == 0x7F ? -1:atony_kadr->numGrp;
   *op.diag =    diag->get_otd_diag(op.addr->cp,op.addr->fa,op.addr->sb);
   otd_proto_protect(&op,ret,OTD_DEF_POLINOM);
  }
 return ret;
}

int     __fastcall TGRNetModule::grnet2otd(lp_gratony_kadr atony_kadr,LPBYTE dest,DWORD dest_sz)
{
 //Преобразование кадра "Гранит net "  к виду OTD
 int ret = 0;
 switch(atony_kadr->InfoType)
 {
  case tpTs   :
  case tpTit  :
                {
                 otd_proto op;
                 op.dw_size = sizeof(op);
                 ret = otd_convert_atony2otd(settings.pu_number,atony_kadr,0,dest,dest_sz,TRUE,&op);
                 //Именно |= так как
                 if(ret>0)
                 {
                  
                  *op.diag   |= this->diag_table.get_otd_diag(op.addr->cp,op.addr->fa,op.addr->sb);
                  
                  otd_proto_protect(&op,op.proto_size,OTD_DEF_POLINOM);
                  op.dw_size = sizeof(op);
                 }
                }
                break;
  case tpReady: ret = ::convert_ready(settings.pu_number,atony_kadr,dest,dest_sz,&diag_table);
               break;
  case tpAll  :
               {

                 otd_proto op;
                 otd_addr  addr;
                 op.dw_size = sizeof(op);
                 OTD_SET_ADDR(&addr,settings.pu_number,atony_kadr->numCp,OTD_FA_QUERY|atony_kadr->data[0],atony_kadr->numGrp == 0x7F ? -1:atony_kadr->numGrp );
                 ret =
                 otd_proto_format_query(dest,dest_sz,&addr,0,OTD_PROTO_PART_DATA|OTD_PROTO_PART_PERSONAL_DIAG|OTD_PROTO_PART_DIAG,(LPVOID)&op);
                 otd_proto_protect(&op,op.proto_size,OTD_DEF_POLINOM);
               }
               break;             
 }

 return ret;

}

void    __fastcall TGRNetModule::make_order    (bool undo)
{
    BYTE buffer[32];
    DWORD qmask = undo ? 0:(OTD_PROTO_PART_DIAG|OTD_PROTO_PART_DATA|OTD_PROTO_PART_PERSONAL_DIAG|OTD_PROTO_PART_KPK|OTD_PROTO_PART_PERSONAL_CHMASK);
    sotd_addr addr(settings.pu_number ,-1,OTD_FA_ORDER|OTD_FA_ALL|(undo ? 0:OTD_FA_QUERY),-1);
    otd_proto op;
    op.dw_size = sizeof(op);
    otd_proto_format_query(buffer,sizeof(buffer),&addr,0,qmask,&op);
    otd_proto_protect(&op,op.proto_size,0);
    queue_rxdata(-1,0,FA_OTD,buffer,op.proto_size,false);
}


void    __fastcall grnet_line::make_query(DWORD pu,DWORD cp,DWORD fa,DWORD grp,bool undo_query)
{
    BYTE buffer[32];
    DWORD qmask = undo_query ? 0:(OTD_PROTO_PART_DIAG|OTD_PROTO_PART_DATA|OTD_PROTO_PART_PERSONAL_DIAG|OTD_PROTO_PART_PERSONAL_CHMASK);
    if(undo_query)
       fa&=~(OTD_FA_ORDER|OTD_FA_ALL);
    sotd_addr addr(pu,cp,fa,grp);
    otd_proto op;
    op.dw_size = sizeof(op);
    otd_proto_format_query(buffer,sizeof(buffer),&addr,0,qmask,&op);
    queue_rxdata(FA_OTD,buffer,op.proto_size,false);
    
}



 void    __fastcall grnet_line::send_grnet_query(lpotd_addr addr)
 {
  //Формирование запросов
  gratony_kadr kadr;
  ZeroMemory(&kadr,sizeof(kadr));
  kadr.kadr_len = kadr.len = sizeof(kadr);
  kadr.kadr_len +=sizeof(kadr.kadr_len);
  if(addr->cp == OTD_ADDR_MAXVALUE && addr->sb == OTD_ADDR_MAXVALUE && OTD_FA_ISQUERY_ALL(addr->fa))
  {
   //Вызов данных по всем существующим КП
   send_data(CmQuery,0,0);
   return;
  }
 }
 

int     __fastcall TGRNetModule::convert_rx_data(LPWORD fa,LPBYTE in,int in_len,LPBYTE out,DWORD out_sz)
{
  /*
   Преобразование данных
  */
  int ret = 0;
  if(*fa == FA_OTD)
     {
      if(in_len<=(int)out_sz)
      {
       ret = in_len;
       memcpy(out,in,in_len);
      }
      else
      ret = -in_len;
     }
     else
  if(*fa == (WORD)-1 )
     {
      ret =  this->grnet2otd((lp_gratony_kadr)in,out,out_sz);
      *fa = FA_OTD;
     } 

  return ret;
}




 
