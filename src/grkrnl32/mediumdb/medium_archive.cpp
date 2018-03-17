#pragma hdrstop
#include "Mediumdb.hpp"
#include <otd.h>
#include <modem_proto.h>
#include <algorithm>
#pragma warn -8017
#include <values.h>
#pragma warn .8017


 bool __fastcall TArchiveThread::BeforeExecute  ()
 {
     bool ret = TGKThread::BeforeExecute();
     if(ret)
     {
       term_event = owner->get_terminate_event().Dup(false);
       ret = IS_VALID_HANDLE(term_event);
     }
   return ret;
 }

 void __fastcall TArchiveThread::BeforeTerminate()
 {
   CloseHandle(term_event);
   term_event = INVALID_HANDLE_VALUE;
   input_queue.DropData();
   TGKThread::BeforeTerminate();
 }

 int  __fastcall TArchiveThread::Execute()
 {
  TSynchroSet ss;
  ss+=term_event;
  ss+=input_queue.GetEvent();
  while(ss.Wait(INFINITE))
  {
   handle_data();
  }
  return 0;
 }

// void __fastcall TArchiveThread::handle_data()
// {
//    BYTE    __buffer[8192];
//    int    bsz = sizeof(__buffer);
//    LPBYTE  buff = __buffer;
//    LPMPROTO_HEADER mph = (LPMPROTO_HEADER) buff;
//    while(input_queue.QueueCount())
//    {
//     int out_sz = 0;
//     if(!input_queue.PeekFromQueue(buff,bsz,&out_sz))
//       {
//         bsz = input_queue.TopDataSize();
//         if(buff != __buffer) delete [] buff;
//         buff = new BYTE[bsz];
//         mph = (LPMPROTO_HEADER) buff;
//         input_queue.PeekFromQueue(buff,bsz,&out_sz);
//       }
//
//     switch(mph->fa)
//     {
//     case FA_OTD_ALARM_ARCHIVE:
//      {
//       sotd_proto sop ((LPBYTE)mph->data,mph->data_size);
//       handle_otd(sop.op);
//      }
//      break;
//     case FA_OTD_MEDIUM_ALARM_ARCHIVE:
//          handle_medium_query(mph);
//      break;
//     }
//     input_queue.DropFirst();
//    }
//    if(buff != __buffer) delete [] buff;
// }

 void __fastcall TArchiveThread::handle_data()
 {
	BYTE    __buffer[8192];
	int    bsz = sizeof(__buffer);
	LPBYTE  buff = __buffer;
	LPMPROTO_HEADER mph = (LPMPROTO_HEADER) buff;
	while(input_queue.QueueCount())
	{
	 int out_sz = 0;
	 if(!input_queue.GetFromQueue(buff,bsz,&out_sz))
	   {
		 bsz = input_queue.TopDataSize();
                 bsz<<=1;
		 if(buff != __buffer) delete [] buff;
		 buff = new BYTE[bsz];
		 mph = (LPMPROTO_HEADER) buff;
		 input_queue.GetFromQueue(buff,bsz,&out_sz);
	   }
	 switch(mph->fa)
	 {
	 case FA_OTD_ALARM_ARCHIVE:
	  {
	   sotd_proto sop ((LPBYTE)mph->data,mph->data_size);
	   handle_otd_arch(true,sop.op);
	  }
	  break;
	 case FA_OTD_MEDIUM_ALARM_ARCHIVE:
		  handle_medium_query(mph);
	  break;
	 }
	}

	if(buff != __buffer)
	   delete [] buff;
 }


 void __fastcall TArchiveThread::handle_otd_arch(bool alarm,otd_proto & op)
 {
   int lo_num = 0;
   int hi_num = 0;
   trecords changed_recs;
   if(op.extension)
   {
    handle_otd_arch_cmd(alarm,op);
   }

   if(op.data && op.time_stamp)
   {
    __int64 tm_stamp = *op.time_stamp;
    __int64 tm_delta = 0;
    if(op.time_stamp_end && (tm_delta = ((*op.time_stamp_end) - (*op.time_stamp)))>0 )
       tm_delta/= OTD_GET_COUNT(&op.data->numbers);
       else
       tm_delta = 0;

    recs.reserve(op.data->numbers.hiN+1);
    if(op.personal_chmask)
    {
     lo_num = op.personal_chmask->numbers.loN;
     hi_num = op.personal_chmask->numbers.hiN;
    }
    else
    {
     lo_num = op.data->numbers.loN;
     hi_num = op.data->numbers.hiN;
    }
    med_db & mdb = owner->get_med_db();
    mdb_record rec;
    while(lo_num<=hi_num)
    {
      DWORD val = 0;
      if(op.personal_chmask)
        otd_get_value(op.personal_chmask,lo_num,&val,sizeof(val));
        else
        val = 1;
     if(val)
     {
      otd_get_value(op.data,lo_num,&val,sizeof(val));
      rec.addr.addr.addr    = op.addr->addr;
      rec.addr.param_number = lo_num;

      if(mdb.get_record((DWORD)-1,&rec) == GKH_RET_SUCCESS)
         {
          DWORD changed = 0;
          trecords::iterator    rec_ptr = std::lower_bound(recs.begin(),recs.end(),rec,mdb_record_id_less());
          if(!(rec_ptr<recs.end() && rec_ptr->rec_id == rec.rec_id))
             {
              rec_ptr          = recs.insert(rec_ptr,rec);
              rec_ptr->percent = 0;
              rec_ptr->value   = MINFLOAT;//values.h
              rec_ptr->state   = 0;
              changed = 1;
             }

          if(rec_ptr->is_signed_kvants())
          rec_ptr->otd_val = rec_ptr->get_signed_kvants(val);
          else
          rec_ptr->otd_val = val;
          rec_ptr->diag    = 0;
          rec_ptr->percent = 0;
          WORD old_state = rec_ptr->state;
          changed|=mdb.calc_record(*rec_ptr,*op.time_stamp);
          rec_ptr->time = tm_stamp;
          changed|= MDBR_FIELD_STATE|MDBR_FIELD_DIAG|MDBR_FIELD_TIME|MDBR_FIELD_OTDVALUE;
          if(((old_state^rec_ptr->state)&MDBR_STATE_ALARM) && (rec_ptr->state&MDBR_STATE_ALARM))
            {
              //запись стала аварийной
              std::less<DWORD> less_cmp;

              trecords_index::iterator alarm_ptr = std::lower_bound(this->alarm_recs.begin(),alarm_recs.end(),rec_ptr->rec_id,less_cmp);
              if( !(alarm_ptr<alarm_recs.end() && *alarm_ptr == rec_ptr->rec_id))
                    alarm_recs.insert(alarm_ptr,rec_ptr->rec_id);
            }
          changed_recs.push_back(*rec_ptr);

         }
     }
     tm_stamp+=tm_delta;
     ++lo_num;
    }
   }
   DWORD ch_count = changed_recs.size();
   if(ch_count) //Рассылка по заказчикам
     {
      std::sort(changed_recs.begin(),changed_recs.end(),mdb_record_id_less());
      send_order(changed_recs);
     }
 }


 void __fastcall TArchiveThread::handle_otd_arch_cmd (bool alarm,otd_proto & op)
 {

   LPOTD_ARCH_HEADER ah = (LPOTD_ARCH_HEADER)op.extension->block_data;

   bool   is_respond     = OTD_ARCH_CMD_IS_RESPOND(ah->cmd);
   bool   is_begin       = OTD_ARCH_CMD_IS_BEGIN(ah->cmd);
   bool   is_end         = OTD_ARCH_CMD_IS_END(ah->cmd);

   if(is_respond )
   {
     //Завершился приём архива.
     //Нарастить счётчик принятых архивов в кадрах в которых были изменения
     switch(OTD_ARCH_CMD(ah->cmd))
     {
      case OTD_ARCH_CMD_ALARM_ARCHIEVES_GET           :
      case OTD_ARCH_CMD_ALARM_ARCHIEVES_GET_AND_ERASE :
      if(is_begin)
      {
       is_begin = is_begin;
      }
      if(is_end)
      {
       LPOTD_ARCH_LIST al = (LPOTD_ARCH_LIST)ah->data;
       owner->get_med_db().archives_increment(op.addr,alarm,al->archive[0],al->count>1 ? al->archive[1] : al->archive[0] ,recs,alarm_recs);
       erase_records(op.addr);
      }
      break;
     }
   }

 }


void __fastcall TArchiveThread::erase_records  (lpotd_addr  addr)
{
    trecords::iterator rec_beg,rec_end;
    rec_beg = recs.begin();

    while(rec_beg<recs.end())
    {
     if(rec_beg->addr.addr.pu == addr->pu && rec_beg->addr.addr.cp == addr->cp)
       {
        trecords_index::iterator alarm_ptr = std::lower_bound(this->alarm_recs.begin(),alarm_recs.end(),rec_beg->rec_id,std::less<DWORD>());
        if(alarm_ptr<alarm_recs.end() && *alarm_ptr == rec_beg->rec_id)
           alarm_recs.erase(alarm_ptr);
        recs.erase(rec_beg);
       }
       else
       ++rec_beg;
    }

}


void __fastcall TArchiveThread::handle_medium_query(LPMPROTO_HEADER mph)
{
   LPOTDM_PROTO_HEADER omph = (LPOTDM_PROTO_HEADER)mph->data;
   if(!(omph->command&OTDM_RESPOND))
   {
       switch(omph->command&OTDM_COMMAND_MASK)
       {
         case OTDMPROTO_CMD_GETKADR:
               if(omph->command&OTDM_SUBSCRIBE)
               {
                handle_order(mph->addr_from,omph->command,omph->data[0],omph->data[1],(omph->command&OTDM_ENDDATA) ? true : false);
               }
         break;
       }
   }
}

 void  __fastcall TArchiveThread::handle_order (MODEM_ADDR & to,DWORD cmd,DWORD kadr_id,DWORD mask,bool finish)
 {
    if(mask == 0)
    {
     orders.remove_order(to,kadr_id); // Отписка от всех кадров
     wchar_t rep_text[MAX_PATH];
     wsprintfW(rep_text,L"Отказ от подписки на аварийные архивы по всем кадрам. Modem %03d.%03d",(DWORD)to.modem,(DWORD)to.line);
     owner->mdb_report(REPORT_INFORMATION_TYPE,rep_text,0,0);
    }
    else
     {
      bool is_valid_kadr = ((kadr_id == (DWORD)-1) || (owner->get_med_db().kadr_exists(kadr_id))) ? true:false;
      if(is_valid_kadr)
         orders.add_order(to,kadr_id,mask);//Подписка на все записи всех кадров
      else
      owner->mproto_send(FA_OTD_MEDIUM_ALARM_ARCHIVE,to,cmd,OTDMPROTO_ERR_NO_KADR,&kadr_id,sizeof(kadr_id),false,finish);
     }
 }

 void __fastcall TArchiveThread::send_order(trecords & recs)
 {

     trecords::iterator rec_beg = recs.begin();
     trecords::iterator rec_end = recs.end();
     trecords::iterator rec_ptr = rec_beg;
     med_db & mdb = owner->get_med_db();
     entry_index   kadrs; // Список кадров по
     record_idlist kadr_recs;
     sended_recs   sr;    //Список уже отправленных записей

     while(rec_ptr<rec_end)
     {
      //Получили список кадров
      DWORD rec_id = rec_ptr->rec_id;
      if(mdb.kadrs_get_range(rec_id,kadrs))
         rec_id = rec_id;
         else
         rec_id+=1;
      ++rec_ptr;
     }
     rec_ptr = rec_beg;
     entry_index::iterator k_beg = kadrs.begin(),k_end = kadrs.end();
     while(k_beg<k_end)
     {
      mdb.kadr_make_record_list(*k_beg,kadr_recs);
      std::sort(kadr_recs.begin(),kadr_recs.end(),std::less<DWORD>());
      order_vector::iterator order_lo,order_hi;
      if(orders.get_modem_range(*k_beg,order_lo,order_hi))
           do_send_order(order_lo,order_hi,recs,kadr_recs);
      else
      {
       //Случай когда подписка на все
       if(orders.get_modem_range(-1,order_lo,order_hi))
           do_send_order(order_lo,order_hi,recs,kadr_recs);
      }
     ++k_beg;
     }

     kadrs.clear();
 }

 void __fastcall TArchiveThread::do_send_order  (order_vector::iterator lo,order_vector::iterator hi,trecords & recs,record_idlist & kadr_recs)
 {
    BYTE   buffer[4096+sizeof(MDB_RECORD)];
    //LPMPROTO_HEADER mph = (LPMPROTO_HEADER)buffer;
    DWORD data_len = 0;
    bool start  = true ;
    trecords::iterator rec_beg = recs.begin(),rec_end = recs.end();

    LPOTDM_PROTO_RECORDS  precs = (LPOTDM_PROTO_RECORDS) buffer;
    LPBYTE sr_ptr  = (LPBYTE) precs->records;
    precs->rec_count = 0;
    __int64 save_timestamp = 0;
    while(rec_beg<rec_end)
    {
     DWORD rec_id = rec_beg->rec_id;
     if(std::binary_search(kadr_recs.begin(),kadr_recs.end(),rec_beg->rec_id))
       {
        if(data_len>=(sizeof(buffer)- sizeof(MDB_RECORD)-sizeof(*precs)))
         {
             data_len+=sizeof(precs->rec_count);
             mproto_send(lo,hi,OTDMPROTO_CMD_RECORDS|OTDM_ORDER,0,buffer,data_len,start,false);
             start = false;
             data_len = 0;
             precs->rec_count = 0;
             sr_ptr  = (LPBYTE) precs->records;
         }
       LPOTDM_PROTO_RECORD proto_rec = (LPOTDM_PROTO_RECORD)sr_ptr;
       proto_rec->mask = MDBR_FIELD_VALUE|MDBR_FIELD_DIAG|MDBR_FIELD_STATE|MDBR_FIELD_TIME|MDBR_FIELD_OTDVALUE;
       sr_ptr   +=sizeof(proto_rec->mask);
       data_len +=sizeof(proto_rec->mask);
       mdb_record & src_record = *rec_beg;
       if(proto_rec->mask & MDBR_FIELD_TIME)
       {
        if(save_timestamp!=src_record.time)
           save_timestamp = src_record.time;
        else
          proto_rec->mask&=~MDBR_FIELD_TIME;
       }

       DWORD rec_len = otdm_pack_record(sr_ptr,sizeof(buffer)-data_len,proto_rec->mask,&src_record);
       sr_ptr  +=rec_len;
       data_len+=rec_len;
       ++precs->rec_count;
       }
     ++rec_beg;
    }

   if(data_len && precs->rec_count)
    {
      data_len+=sizeof(precs->rec_count);
      mproto_send(lo,hi,OTDMPROTO_CMD_RECORDS|OTDM_ORDER,0,buffer,data_len,start,true);
    }
 }

  void __fastcall TArchiveThread::mproto_send  ( order_vector::iterator lo
                                                ,order_vector::iterator hi
                                                ,DWORD cmd,DWORD err
                                                ,LPVOID data,DWORD data_sz
                                                ,bool start,bool finish
                                                )
  {
    while(lo<hi)
    {
      owner->mproto_send(FA_OTD_MEDIUM_ALARM_ARCHIVE,lo->dest_addr,cmd,err,data,data_sz,start,finish);
      ++lo;
    }
  }



