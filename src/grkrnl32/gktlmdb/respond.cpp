#pragma hdrstop
/*Нить ответа на вызов*/
#include "gktlmdb_mod.hpp"


 void           __fastcall release_storage(otd_storage & ost)
 {
   proto_pointer ptr = ost.begin(),end = ost.end();
   while(ptr<end)
   {
    delete [] (LPBYTE)ptr->op.addr;
    ++ptr;
   }
   ost.clear();
 }

 bool           __fastcall TRespond::begin_work(DWORD p,LPVOID arg1,LPVOID arg2)
 {
  dest.addr = p;
  src = reinterpret_cast<main_dblow*>(arg1);
  lpotd_proto op  = reinterpret_cast<lpotd_proto>(arg2);
  if(op)
    {
     addr.addr = op->addr->addr;

     parts = (*op->ver_parts)&OTD_PROTO_PARTSMASK;
    }

  return  true;
 }

 void           __fastcall TRespond::do_work()
 {

  if(src && src->get_module())
  {
   BOOL is_call      =  (addr.fa&OTD_FA_CALL) ? TRUE:FALSE;
   BYTE buffer[2048+sizeof(MPROTO_HEADER)];
   DWORD tm = GetTickCount();
   modem_addr ma(dest.addr);
   DWORD mon_len =
   wsprintf(buffer,"Обработка %s от модема %03d.%03d адрес %03d.%03d.%03d.%03d parts %04X",
            is_call ? "вызова" : "подписки",
            (DWORD)ma.modem,(DWORD)ma.line,
            (DWORD)addr.pu,(DWORD)addr.cp,(DWORD)addr.fa&OTD_FA_ALL,(DWORD)addr.sb,
            parts
            )+1;

   src->get_module()->notify(TLMDB_DBLOW_MONITOR,TLMDB_DBLOW_MONITOR_RESPOND,buffer,mon_len);

   DWORD s_count = 0;
   LPMPROTO_HEADER mph =  (LPMPROTO_HEADER)buffer;
   ZeroMemory(mph,sizeof(*mph));
   mph->fa = FA_OTD;
   mph->addr_to.addr = dest.addr;
   #if defined (_DEBUG) && !defined(FASTMUTEX_LOCKER)
   TMrSw::enable_dbo(true);
   #endif
   __try{
   __try{
   otd_storage ost;  // TODO Надо делать отдельную  копию т.к возможно
                    // удаление данных из src и тогда указатели станут невалидными
   parts&=~OTD_PROTO_PART_PERSONAL_CHMASK;
   src->copy_to(addr,ost,parts|OTD_PROTO_PART_DIAG,0,-1);
   proto_pointer  ptr = ost.begin(),end = ost.end();
   while(ptr<end && !need_terminate())
    {
      //TODO - Отправлять otd_proto::personal_diag в случае установленного OTD_PART_DIAG_PARAM | OTD_DIAG_STATE_TUTR_ERROR|OTD_STATE_TUTR_MASK
      DWORD respond_parts = parts;
      //if(!is_call && ptr->op.diag && !((*ptr->op.diag)&(OTD_PART_DIAG_PARAM | OTD_DIAG_STATE_TUTR_ERROR|OTD_STATE_TUTR_MASK)))
      //    respond_parts&=~OTD_PROTO_PART_PERSONAL_DIAG;

      if(sotd_addr(ptr->op.addr->addr).is_include(addr))
        {
         mph->data_size = ost.get_parts(ptr,respond_parts|(OTD_PROTO_PART_KPK|OTD_PROTO_FLAG_QUERYRESPOND),0,-1,(LPBYTE)mph->data,sizeof(buffer)-sizeof(*mph));
         sotd_proto sop((LPBYTE)mph->data,mph->data_size );
         if(sop.op.kpk)
            otd_proto_protect(&sop.op,0,OTD_DEF_POLINOM);
         if(mph->data_size > 6)
          {
           src->get_module()->send(mph);
           ++s_count;
          } 
        }
        
      ++ptr;
    }
     release_storage(ost);
    }
    __except(OnException(GetExceptionCode(),GetExceptionInformation())){}
    }
    __finally{
         #if defined (_DEBUG) && !defined(FASTMUTEX_LOCKER)
                TMrSw::enable_dbo(false);
         #endif
              }
   tm = GetTickCount()-tm;
   mon_len = wsprintf(buffer,"Отправлено кадров  %03d  Время обработки %03d ms Id - нити %X",s_count,tm,Id)+1;
   src->get_module()->notify(TLMDB_DBLOW_MONITOR,TLMDB_DBLOW_MONITOR_RESPOND,buffer,mon_len);

  }
 }

 DWORD          __fastcall TRespond::OnException(DWORD ExceptionCode,LPEXCEPTION_POINTERS exps)
 {
   TGKModule::report(RCM_EXCEPTION,(LPARAM)exps,(LPARAM)"GKTLMDB:RespondThread");
   while(src->wr_unlock())
        ;
   while(src->rd_unlock())
        ;
   return TThread::OnException(ExceptionCode,exps);
 }

