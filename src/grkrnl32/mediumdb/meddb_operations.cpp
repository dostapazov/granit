#pragma hdrstop
#include "mediumdb.hpp"
#include <otd.h>
#include <stdio.h>


  void       __fastcall med_db::do_timer   (int msec)
  {
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       wr_lock    l (locker);
     #endif
     tutr_actives_t::iterator ptr = tutr_actives.begin();
     tutr_actives_t::iterator end = tutr_actives.end  ();
     while(ptr!=end)
     {
      tutr_record_active & rca = ptr->second;
      rca.timeout-= msec;
      if(rca.timeout<=0)
         {
          tutr_select(&rca.ma,ptr->first,false);
          ptr = tutr_actives.begin();
          end = tutr_actives.end();
         }
         else
         ++ptr;
     }

    if(addr_mask_timeout>0)
       {
        addr_mask_timeout-= msec;
        if(addr_mask_timeout<=0) addr_mask.addr = 0;
       }

  }


  DWORD      __fastcall med_db::tutr_select        (const LPMODEM_ADDR ma,DWORD rec_id,bool select)
  {
    DWORD ret    = GKH_RET_ERROR;
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       wr_lock    l (locker);
     #endif


    DWORD offset = records.find_by_id(rec_id);
    if(offset!= (DWORD )-1 && offset<records.size())
    {
       mdb_record & rec = records.at(offset);

       if(rec.state & MDBR_STATE_TUFLAG)
       {
           OTDM_PROTO_TUOPERATION tuop;
           tuop.rec_id     = rec_id;
           tuop.op_code    = OTD_TUTR_CMDDESELECT;
           DWORD error     = 0;

           if(select)
           {
             if(!tutr_actives.count(rec.rec_id))
             {
              tutr_actives[rec.rec_id] = tutr_record_active(*ma,10000);
              rec.state |= MDBR_STATE_TUTR_PREPARE;
              record_changed(rec,MDBR_FIELD_STATE);
              tuop.op_code    = OTD_TUTR_CMDENABLE;
              ret = GKH_RET_SUCCESS;
             }
           }
           else
           {
             if(tutr_actives.count(rec.rec_id))
             {
              rec.state &= ~(MDBR_STATE_TUTR_ACTIVE|MDBR_STATE_TUTR_PREPARE);
              tutr_actives.erase(rec.rec_id);
              record_changed(rec,MDBR_FIELD_STATE);
              ret = GKH_RET_SUCCESS;
             }
           }

           handle_record_changes() ;
           module->mproto_send(FA_OTD_MEDIUM,*ma,OTDMPROTO_CMD_TUOPERATION|OTDM_RESPOND,error,&tuop,sizeof(tuop),true,false);
       }
       else
       {
         if(rec.state &(MDBR_STATE_TUTR_ACTIVE|MDBR_STATE_TUTR_ERROR|MDBR_STATE_TUTR_PREPARE))
            {
              rec.state &= ~(MDBR_STATE_TUTR_ACTIVE|MDBR_STATE_TUTR_ERROR|MDBR_STATE_TUTR_PREPARE);
              record_changed(rec,MDBR_FIELD_STATE);
              handle_record_changes() ;
            }
       }

    }
    return ret;
  }

 DWORD      __fastcall med_db::record_operation(const LPMODEM_ADDR ma,const LPOTDM_PROTO_TUOPERATION tuop)
 {
   DWORD ret = GKH_RET_ERROR;
   if(tuop->op_code & OTD_TUTR_CMDMASK)
   {

      switch(tuop->op_code)
      {
       case OTD_TUTR_CMDSELECT:
            ret = tutr_select(ma,tuop->rec_id,true);
       break;
       case OTD_TUTR_CMDDESELECT:
             ret = tutr_select(ma,tuop->rec_id,false);
       break;
      }

   }
   else
   {
    if(tuop->op_code)
       ret = record_operation(tuop->rec_id,tuop->op_code,tuop->new_value);
   }
   return ret;
 }

 DWORD      __fastcall med_db::record_operation(DWORD rec_id,DWORD op_code,float new_value)
 {
  DWORD  ret = GKH_RET_ERROR;
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       wr_lock    l (locker);
     #endif
  mdb_record & rec = records.get_record(rec_id);
  if(rec.rec_id == rec_id && rec.addr.param_number!=(DWORD)-1 && ((rec.state&MDBR_STATE_TUFLAG) || rec.is_pseudo()))
  {
    //TODO Запись в лог об операции ТУ/ТР
    if(tutr_actives.count(rec_id))
       {
         tutr_actives.erase   (rec_id);
         rec.state &= ~(MDBR_STATE_TUTR_PREPARE|MDBR_STATE_TUTR_ERROR);
         rec.state |= (op_code == OTD_TUOP_ON) ?  MDBR_STATE_TUTR_ON_MORE : MDBR_STATE_TUTR_OFF_LESS;
         record_changed(rec_id,MDBR_FIELD_STATE);
         report_tu_operation  (rec,op_code);

       }

      if(rec.is_pseudo())
       {
         handle_record_changes();
         rec.state &=~MDBR_STATE_TUTR;
         record_changed(rec_id,MDBR_FIELD_STATE);
         new_value = rec.value;
         if(rec.is_discrete() && op_code != OTD_TUTROP_CANCEL)
           {
             new_value =  (op_code == OTD_TUOP_ON) ? 1.0:0.0;
           }

          if(fabs(rec.value-new_value) > .0001)
          {
           rec.otd_val = rec.calc_kvants(new_value);
           rec.value   = new_value;
           rec.time    = GetTime();
           calc_record(rec,rec.time);
           record_changed(rec,MDBR_FIELD_OTDVALUE|MDBR_FIELD_VALUE|MDBR_FIELD_TIME);
           handle_record_changes();
          }
        }
        else
        {
         BYTE buffer[1024];
         LPMPROTO_HEADER mph = (LPMPROTO_HEADER)buffer;
         ZeroMemory(mph,sizeof(*mph));
         mph->fa = FA_OTD;
         WORD cmd_attr  =  OTD_TUTR_CMDATTR_CHECK_SUCCESS | (rec.is_inverse_scale() ? OTD_TUTR_CMDATTR_INVERSE : 0);
         mph->data_size =  otd_proto_format_tutr((LPBYTE)mph->data,sizeof(buffer)-sizeof(*mph),&rec.addr.addr,rec.addr.param_number,op_code,cmd_attr,0,0,0,NULL);
         module->send(mph);
        }

      handle_record_changes();
  }
  //unlock();
  return ret;
 }

 DWORD      __fastcall med_db::record_request_oscillogramm(DWORD rec_id,DWORD osc_lenght)
 {
  //Запрос осцилограммы
  DWORD  ret = GKH_RET_ERROR;
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       rd_lock    l (locker);
     #endif
  mdb_record & rec = records.get_record(rec_id);
  if(rec.rec_id == rec_id && rec.addr.param_number!=(DWORD)-1 )
  {
    BYTE buffer[1024];
    LPMPROTO_HEADER mph = (LPMPROTO_HEADER)buffer;
    ZeroMemory(mph,sizeof(*mph));
    mph->fa = FA_OTD;
    sotd_proto sop;
    lpotd_oscillgramm_param oscparam;
    otd_proto_param op;
    bzero(&op,sizeof(op));
    op.dw_size  = sizeof(op);
    op.ext_size = sizeof(*oscparam);
    op.ext_type = OTD_BLOCKTYPE_START_OSCILLOGRAMM;
    op.parts    = OTD_PROTO_PART_PROTOEXTENSION|OTD_PROTO_PART_KPK;
    mph->data_size = otd_proto_format_ex((LPBYTE)mph->data,sizeof(buffer)-sizeof(*mph),&op,0,&sop.op);

    oscparam = (lpotd_oscillgramm_param)sop.op.extension->block_data;
    oscparam-> param_addr = rec.addr;
    oscparam->osc_lenght  = osc_lenght;
    sop.op.addr->addr = rec.addr.addr.addr;
    sop.op.addr->fa  |= OTD_FA_SETUP;
    otd_proto_protect(&sop.op,sop.op.proto_size,OTD_DEF_POLINOM);
    module->send(mph);

  }
  //unlock();
  return ret;

 }


 void       __fastcall med_db::report_tu_operation(mdb_record & rec,DWORD op_code)
 {
  wchar_t rep_text[MAX_PATH<<2];
  wchar_t rec_name[MAX_PATH];
  wchar_t rep_templ[MAX_PATH];
  wchar_t op_code_text[64];
  bool    is_discrete  = (rec.state&MDBR_STATE_DISCRETE) ? true:false;
  module->get_lang_string( is_discrete ? MDB_RCSTR_TU_OPERATION:MDB_RCSTR_TR_OPERATION,rep_templ,sizeof(rep_templ)/sizeof(wchar_t));
  module->get_lang_string( MDB_RCSTR_TU_OPNONE +(int)!is_discrete+op_code,op_code_text,sizeof(op_code_text)/sizeof(wchar_t));
  get_name(rec.rec_id,true,rec_name,MAX_PATH);

  if(!rec_name[0])
    get_name(rec.rec_id,false,rec_name,MAX_PATH);
    if(is_discrete)
      swprintf(rep_text,rep_templ,op_code_text,(DWORD)rec.addr.addr.pu,(DWORD)rec.addr.addr.cp,(DWORD)rec.addr.addr.fa,(DWORD)rec.addr.addr.sb,(DWORD)rec.addr.param_number,rec_name);
      else
      swprintf(rep_text,rep_templ,op_code_text,rec.value,(DWORD)rec.addr.addr.pu,(DWORD)rec.addr.addr.cp,(DWORD)rec.addr.addr.fa,(DWORD)rec.addr.addr.sb,(DWORD)rec.addr.param_number,rec_name);
    module->mdb_report_tutr(REPORT_SUCCESS_TYPE,rep_text,0,0);
 }

 void       __fastcall med_db::record_clear_tu_error(DWORD rec_id)
 {
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       wr_lock    l (locker);
     #endif
    DWORD offs = records.find_by_id(rec_id);
    if(offs != DWORD(-1))
      {
        mdb_record & rec = records.at(offs);
        if(rec.state & MDBR_STATE_TUTR_ERROR)
        {
         rec.state &= ~MDBR_STATE_TUTR_ERROR;
         record_changed(rec,MDBR_FIELD_STATE,0);
         handle_record_changes();
        }
      }

 }

 void       __fastcall med_db::record_work_inout( LPOTDM_PROTO_WORK_INOUT wio)
 {
     #if( FASTMUTEX_LOCKER != 0)
      TLockHelper l (locker);
     #else
       wr_lock    l (locker);
     #endif
    DWORD offs = records.find_by_id(wio->rec_id);
    if(offs != DWORD(-1))
      {
        mdb_record & rec = records.at(offs);
        if(wio->worked)
           rec.state &= ~MDBR_STATE_WORKOUT;
           else
           rec.state |= MDBR_STATE_WORKOUT;
         record_changed(rec,MDBR_FIELD_STATE,0);
         handle_record_changes();
      }

 }
