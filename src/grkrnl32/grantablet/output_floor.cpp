#include "grantablet.hpp"
#include <granit_proto.h>
#include <stdio.h>
#include <list>

TOutputFloor::TOutputFloor(DWORD number):modem_line(number)
{
}

 DWORD         __fastcall TOutputFloor::stream_write(TStream & strm)
 {
  //Запись содержимого в поток

  DWORD ret = 0 ;
  lock();
  strm<<(DWORD) this->kadrs.size();
  output_kadr_vector::iterator k_ptr = kadrs.begin(),k_end = kadrs.end();
  while(k_ptr<k_end)
  {
   strm<<k_ptr->kadr.kadr_id<<k_ptr->output_position<<k_ptr->output_mode;
   ++k_ptr;
   ++ret;
  }
  unlock();
  return ret;
 }

 DWORD         __fastcall TOutputFloor::stream_read (TStream & strm)
 {
   //Чтение содержимого из потока
   clear_floor();
   DWORD count = 0;
   lock();
   strm>>count;
   kadrs.resize(count,output_kadr(0,0,0));
    output_kadr_vector::iterator k_ptr = kadrs.begin(),k_end = kadrs.end();
    while(k_ptr<k_end)
    {
       strm>>k_ptr->kadr.kadr_id>>k_ptr->output_position>>k_ptr->output_mode;
       ++k_ptr;
    }
    unlock();
   return count; 
 }


 DWORD __fastcall TOutputFloor::get_line_flags()
 {
   return MLPF_RXABLE|MLPF_TXABLE;
 }

 DWORD __fastcall TOutputFloor::get_line_text  (wchar_t * text,DWORD text_sz)
 {
  wchar_t _text[MAX_PATH];
  DWORD len = wsprintfW(_text,L"Этаж вывода № %d",get_number());
  safe_strcpyn(text,_text,text_sz);
  return KeRTL::MIN(len,text_sz);
 }

 DWORD __fastcall TOutputFloor::get_max_frame_size()
 {
  return 2048;
 }

 DWORD __fastcall TOutputFloor::get_tx_queue_size()
 {
  return 0;
 }

 bool  __fastcall TOutputFloor::send        (LPMPROTO_HEADER mph,DWORD sz)
 {
  bool ret = true;
  if(mph->fa == FA_OTD_MEDIUM)
  {
   LPOTDM_PROTO_HEADER omph = (LPOTDM_PROTO_HEADER)mph->data;
   bool data_begin = omph->command&OTDM_BEGINDATA;
   bool data_end   = omph->command&OTDM_ENDDATA;
   switch(omph->command&OTDM_COMMAND_MASK)
   {
    case OTDMPROTO_CMD_GETKADR:
         if(omph->error == OTDMPROTO_NO_ERROR)
           handle_kadr((LPOTDM_PROTO_KADR)omph->data);
    break;
    case OTDMPROTO_CMD_GETKADR_ENTRYES:
         if(omph->error == OTDMPROTO_NO_ERROR)
          handle_kadr_entryes((LPOTDM_PROTO_KADR_ENTRYES)omph->data,data_begin,data_end);
    break;
    case OTDMPROTO_CMD_GETKADR_RECORDS:
         if(omph->error == OTDMPROTO_NO_ERROR)
         handle_records((LPOTDM_PROTO_KADR_RECORDS)omph->data);
    break;
    case OTDMPROTO_CMD_RECORDS:
       if(omph->error == OTDMPROTO_NO_ERROR)
        handle_record_changes((LPOTDM_PROTO_RECORDS) omph->data);
    break;
   }

  }
  else
  ret = false;
  return ret;
 }


 output_kadr_vector::iterator __fastcall TOutputFloor::lower_kadr(DWORD kid,DWORD position)
 {
  output_kadr_vector::iterator  ptr = kadrs.begin(),end = kadrs.end();
  ptr = std::lower_bound(ptr,end,output_kadr(kid,0,0));
  return ptr;

 }

 output_kadr_vector::iterator __fastcall TOutputFloor::find_kadr(DWORD kid,DWORD position)
 {
  output_kadr_vector::iterator  ptr = kadrs.begin(),end = kadrs.end();
  ptr = std::lower_bound(ptr,end,output_kadr(kid,0,position));
  if(kadrs.size() &&  ptr<end && ptr->kadr.kadr_id == kid && ptr->output_position == position)
     return ptr;
   return end;

 }

 output_kadr_vector::iterator __fastcall TOutputFloor::find_kadr_by_position(DWORD pos)
 {
  //Поиск описателя выводимого кадра по номеру позиции
  output_kadr_vector::iterator  ptr = kadrs.begin(),end = kadrs.end();
  while(ptr<end)
  {
   if(ptr->output_position == pos)
      break;
     else
     ++ptr;
  }
  return ptr;

 }

 record_vector::iterator      __fastcall TOutputFloor::lower_record(DWORD rec_id)
 {
  mdb_short_record sr(rec_id);
  record_vector::iterator ptr = std::lower_bound(records.begin(),records.end(),sr);
  return ptr;
 }

 record_vector::iterator      __fastcall TOutputFloor::find_record(DWORD rec_id)
 {
  record_vector::iterator ptr = lower_record(rec_id);
  if(records.size() && ptr->rec_id == rec_id)
     return ptr;
   return records.end();
 }


 void      __fastcall TOutputFloor::handle_kadr(LPOTDM_PROTO_KADR kdr)
 {

 }

 void          __fastcall TOutputFloor::handle_kadr_entryes(LPOTDM_PROTO_KADR_ENTRYES entryes,bool beg_data,bool end_data)
 {
  lock();
  output_kadr_vector::iterator  ptr = lower_kadr(entryes->kadr_id,0);
  while(ptr<kadrs.end() && ptr->kadr.kadr_id == entryes->kadr_id)
  {
    if(beg_data)
      ptr->entry_list.clear();

   LPMDB_KADR_ENTRY  list = entryes->entryes;
   for(DWORD i = 0;i<entryes->entryes_count;++i,++list)
   {
     ptr->entry_list.insert(ptr->entry_list.end(),mdb_kadr_entry(*list));
   }
   do_output_kadr(ptr);
   ++ptr;
  }
  unlock();
 }

 void          __fastcall TOutputFloor::handle_records(LPOTDM_PROTO_KADR_RECORDS k_recs)
 {
   lock();
   for(DWORD i = 0; i< k_recs->rec_count;++i)
   {
     mdb_short_record sr(k_recs->records[i]);
     record_vector::iterator r_ptr = lower_record(sr.rec_id);
     if(r_ptr<records.end() && r_ptr->rec_id == sr.rec_id)
        *r_ptr = sr;
     else
      records.insert(r_ptr,sr);   
   }
   do_output_kadr(k_recs->kadr_id);
   unlock();
 }




 void          __fastcall TOutputFloor::handle_record_changes(LPOTDM_PROTO_RECORDS records)
 {
       LPBYTE src_ptr = (LPBYTE)records->records;
       mdb_record rec;
       changes_list ch_list;
       for(DWORD i = 0;i<records->rec_count;i++)
       {
        LPOTDM_PROTO_RECORD proto_rec = (LPOTDM_PROTO_RECORD) src_ptr;
        src_ptr+=sizeof(proto_rec->mask);
        DWORD ch_mask = proto_rec->mask;
        src_ptr+= otdm_unpack_record(src_ptr,sizeof(rec),ch_mask,&rec);

        lock();
        srecord_list::iterator rec_ptr = lower_record(rec.rec_id);
        if(rec_ptr<this->records.end() && rec_ptr->rec_id == rec.rec_id)
         {
           if(ch_mask&MDBR_FIELD_VALUE)
            rec_ptr->value = rec.value;
           if(ch_mask&MDBR_FIELD_DIAG)
            rec_ptr->diag = rec.diag;
           if(ch_mask&MDBR_FIELD_STATE)
            rec_ptr->state = rec.state;
         }
         else
         {
          //ТАКОЙ ЗАПИСИ НЕТ
          if(ch_mask  & (MDBR_FIELD_VALUE | MDBR_FIELD_DIAG))
             this->records.insert(rec_ptr,mdb_short_record(rec));
         }

        if(ch_mask&(MDBR_FIELD_VALUE | MDBR_FIELD_DIAG))
          {
            ch_list.push_back(rec.rec_id);
          }
       unlock();

      }
      ch_list.sort();
      ch_list.unique();

      if(ch_list.size())
      {
        lock();
        output_kadr_vector::iterator k_ptr = kadrs.begin(),k_end = kadrs.end();
        while(k_ptr<k_end)
        {
         output_kadr_changes(*k_ptr,ch_list);
         ++k_ptr;
        }
        unlock();
      }

 }

 bool         __fastcall  TOutputFloor::output_kadr_changes(output_kadr & o_kdr,changes_list  & ch_list)
 {
  bool ret = false;
  lock();
  if(o_kdr.entry_list.size())
  {
  changes_list::iterator ch_beg = ch_list.begin(),ch_end = ch_list.end();
  while(!ret && ch_beg!=ch_end)
  {
   mdb_kadr_entry l_entry(o_kdr.kadr.kadr_id,*ch_beg,0);
   mdb_kadr_entry h_entry(o_kdr.kadr.kadr_id,*ch_beg,-1);
   kadr_entry_list::iterator l_ptr = o_kdr.entry_list.begin(),h_ptr = o_kdr.entry_list.end();
   l_ptr = std::lower_bound(l_ptr,h_ptr,l_entry);
   if(l_ptr<h_ptr && l_ptr->rec_id == *ch_beg)
     {
      switch(o_kdr.output_mode)
      {
       case OUTPUT_MODE_ANALOG:
         h_ptr = std::upper_bound(l_ptr,h_ptr,h_entry);
          while(l_ptr<h_ptr)
          do_output_analog_value(&o_kdr,l_ptr++);
       break;
       case OUTPUT_MODE_DISCRETE:
          do_output_kadr_discrete(&o_kdr);
          ret = true;
       break;
       case OUTPUT_MODE_READY:
          do_output_kadr_ready(&o_kdr);
          ret = true;
       break;

      }
     }
   ++ch_beg;
  }
  }
  unlock();
  return ret;
 }


 void          __fastcall TOutputFloor::do_output_kadr(DWORD kadr_id)
 {
  lock();
  output_kadr_vector::iterator k_ptr = lower_kadr(kadr_id,0);
  while(k_ptr<kadrs.end() && k_ptr->kadr.kadr_id == kadr_id)
   {
    do_output_kadr(k_ptr);
    ++k_ptr;
   }
  unlock();
 }

 void  __fastcall TOutputFloor::do_output_kadr(DWORD kadr_id,DWORD pos)
 {
   lock();
   output_kadr_vector::iterator k_ptr = find_kadr(kadr_id,pos);
   do_output_kadr(k_ptr);
   unlock();
 }

 void          __fastcall TOutputFloor::do_output_kadr(output_kadr_vector::iterator k_ptr)
 {
   lock();
   if(k_ptr<this->kadrs.end())
   {
    switch(k_ptr->output_mode)
    {
     case OUTPUT_MODE_READY    :
          do_output_kadr_ready(k_ptr);
      break;
     case OUTPUT_MODE_DISCRETE :
          do_output_kadr_discrete(k_ptr);
      break;
     case OUTPUT_MODE_ANALOG   :
          do_output_kadr_analog(k_ptr);
     break;
    }
   }
   unlock();
 }
 

void __fastcall  TOutputFloor::do_output_kadr_ready(output_kadr_vector::iterator k_ptr)
{
  gr_proto_out_ready out_ready;
  ZeroMemory(&out_ready,sizeof(out_ready));

  FillMemory(out_ready.ready_state,sizeof(out_ready.ready_state),-1);
  FillMemory(out_ready.ready_slaking,sizeof(out_ready.ready_slaking),-1);
  out_ready.ready_slaking[0]&=0x3F;
  out_ready.floor     = GRPROTO_MK_CPNUM(get_number(),0);
  out_ready.type_afb  = GRPROTO_MKTYPE(GRKT_DATAOUT,k_ptr->output_position);

  kadr_entry_list::iterator beg = k_ptr->entry_list.begin(),end = k_ptr->entry_list.end();
  while(beg<end)
   {
    if(beg->number<30)
    {
     record_vector::iterator rec_ptr = find_record(beg->rec_id);
     if(rec_ptr<this->records.end() )
      {
       gr_set_ts(rec_ptr->diag==MDBR_DIAG_BAD ? 1:0 ,out_ready.ready_state,beg->number);
       DWORD sl_value = 0;
       if(rec_ptr->diag == MDBR_DIAG_PARTIAL)
          sl_value = 1;
       if(rec_ptr->state& (MDBR_STATE_FLASH))
            gr_set_ts(1,out_ready.ready_flash,beg->number);
          rec_ptr->state&=~MDBR_STATE_FLASH;
         gr_set_ts(sl_value,out_ready.ready_slaking,beg->number+2);
      }
    }
    ++beg;
   }
  if(k_ptr->kadr.discrete_changes || k_ptr->kadr.alarm_changes)
  {
   out_ready.ready_slaking[0]|= 0x80;
  }
  DWORD send_len = sizeof(out_ready);
  out_ready.kpk = gr_calc_kpk(&out_ready,send_len-sizeof(out_ready.kpk),STD_GRANIT_POLINOM );
  floor_tx((LPBYTE)&out_ready,send_len);
  k_ptr->kadr.discrete_changes = k_ptr->kadr.alarm_changes = k_ptr->kadr.kadr_changes = 0;
}

void __fastcall  TOutputFloor::do_output_kadr_discrete(output_kadr_vector::iterator k_ptr)
{
  /*Вывод ТС*/
  gr_proto_out_ts  out_ts;
  out_ts.floor     = GRPROTO_MK_CPNUM(get_number(),0);
  out_ts.type_afb  = GRPROTO_MKTYPE(GRKT_DATAOUT,k_ptr->output_position);
  ZeroMemory(out_ts.bits,sizeof(out_ts.bits));
  kadr_entry_list::iterator beg = k_ptr->entry_list.begin(),end = k_ptr->entry_list.end();
  while(beg<end)
   {
    if(beg->number<64)
    {
     record_vector::iterator rec_ptr = find_record(beg->rec_id);
     if(rec_ptr<this->records.end() )
      {
       gr_set_ts((DWORD)rec_ptr->value,out_ts.bits,beg->number);
      }
    }  
    ++beg;
   }
  for(int i = 0;i<sizeof(out_ts.inv_bits);++i)
      out_ts.inv_bits[i] = ~out_ts.bits[i];
  out_ts.kpk  = gr_calc_kpk(&out_ts,sizeof(out_ts)-sizeof(out_ts.kpk),STD_GRANIT_POLINOM );
  floor_tx((LPBYTE)&out_ts,sizeof(out_ts));    
}

void __fastcall  TOutputFloor::do_output_kadr_analog(output_kadr_vector::iterator k_ptr)
{
  kadr_entry_list::iterator beg = k_ptr->entry_list.begin(),end = k_ptr->entry_list.end();
  while(beg<end)
  {
   if(beg->number<64)
    {
     do_output_analog_value(k_ptr,beg);
    }
   ++beg;
  }

}

void __fastcall  setup_tiout_data(LPBYTE data,DWORD n_param,
                                  DWORD  digit0,DWORD  digit1,DWORD  digit2,
                                  bool slaking = false,bool reverse = false)
{
 data[2] = (digit0&0x0F)|((digit1<<4)&0xF0);
 data[1] = digit2&0x0F;
 if(slaking)
   data[1]|= 0x20;
 if(reverse)
   data[1]|= 0x10;


 data[1]|= ((n_param&3)<<6)&0xC0;
 data[0] = (n_param>>2)&0x0F;
 data[0]|= 0x30;
}


void __fastcall  TOutputFloor::do_output_analog_value(output_kadr_vector::iterator k_ptr,
                                                      kadr_entry_list::iterator entry)
{

  BYTE            buffer[8];
  gr_proto_out_ti &out_ti = *(lpgr_proto_out_ti)buffer;
  ZeroMemory(&out_ti,sizeof(out_ti));
  out_ti.floor             = GRPROTO_MK_CPNUM(get_number(),0);
  out_ti.type_afb          = GRPROTO_MKTYPE(GRKT_DATAOUT,k_ptr->output_position);
  bool slaking = false;
  bool reverse = false;
  DWORD digit0,digit1,digit2;

  record_vector::iterator rec_ptr = find_record(entry->rec_id);
  if(rec_ptr<this->records.end() )
  {
    digit0 = digit1 = digit2 = 0;
    char fmt    [32];
    char result [128];
    sprintf(fmt,"%%03.%d%c",entry->options.prec,'f');
    char *src = result-1+sprintf(result,fmt,rec_ptr->value);
    int count = 0;
    while(result<=src && count<3)
    {
      if(isdigit(*src))
      {
       switch(count)
       {
        case 0: digit0 = *src - '0';break;
        case 1: digit1 = *src - '0';break;
        case 2: digit2 = *src - '0';break;
       }
       ++count;
      }
      --src;
    }
    if(rec_ptr->diag)
      slaking = true;
    if(rec_ptr->value<.0)
       reverse = true; //Отрицателльное значение  

  }
  else
  {
   slaking = 1;
   digit0 = digit1 = digit2 = 2;

  }
  setup_tiout_data(out_ti.data,entry->number,digit0,digit1,digit2,slaking,reverse);
  out_ti.kpk  = gr_calc_kpk(&out_ti,sizeof(out_ti)-sizeof(out_ti.kpk),STD_GRANIT_POLINOM );
  floor_tx((LPBYTE)&out_ti,sizeof(out_ti));

}



 bool  __fastcall TOutputFloor::do_start     ()
 {
  bool ret = true;
  clear_floor();
  ret = Start(16384,false,NULL);
  return ret;
 }

  void __fastcall  TOutputFloor::release()
  {
   lock();
   clear_floor();
   kadrs.clear();
   unlock();
   modem_line::release();
   
  }

 void __fastcall  TOutputFloor::clear_floor()
  {
   lock();
   records.clear();
   output_kadr_vector::iterator ptr = kadrs.begin(),end = kadrs.end();
   while(ptr<end)
    {
     ptr->entry_list.clear();
     ++ptr;
    }
   unlock();
  }


 bool  __fastcall TOutputFloor::do_stop      ()
 {
  bool ret = true;
  TerminateAndWait(2000,true);
  return ret;
 }

 void  __fastcall TOutputFloor::on_connect   ()
 {
  clear_floor();
  modem_line::on_connect();

 }
 void  __fastcall TOutputFloor::on_disconnect()
 {
  modem_line::on_disconnect();
 }

 bool  __fastcall TOutputFloor::BeforeExecute()
 {
  terminate_event.SetEvent(false);
  return TGKThread::BeforeExecute();
 }

 void  __fastcall TOutputFloor::BeforeTerminate()
 {
  TGKThread::BeforeTerminate();
 }

 int   __fastcall TOutputFloor::Execute()
 {
  TSynchroSet ss;
  ss+=terminate_event();
  DWORD wr;
  do{
     wr = ss.Wait(1000);
     switch(wr)
     {
      case WAIT_TIMEOUT: on_timeout();break;
     }
    }while(wr && !CheckTerminateRequest());
  return 0;
 }


void  __fastcall TOutputFloor::on_timeout()
{
 lock();
 if(is_connected())
 {
  if(current_kadr<kadrs.size())
  {
    output_kadr & kadr = kadrs.at(current_kadr++);
    if(kadr.entry_list.size())
    {
     /*Переодический обход кадров с обновлением щита*/
     do_output_kadr(&kadr);
    }
    else
    /*Формирование подписки на */
    make_order(kadr.kadr.kadr_id);
  }
  else
  current_kadr = 0;
 }
 unlock();
}

DWORD __fastcall TOutputFloor::refresh(DWORD sbl)
{
  lock();
   output_kadr_vector::iterator k_ptr = kadrs.begin(),k_end= kadrs.end();
   while(k_ptr<k_end)
   {
    if(k_ptr->output_position == sbl)
       do_output_kadr(k_ptr);
    ++k_ptr;
   }
  unlock();
  return GKH_RET_SUCCESS;
}


void          __fastcall TOutputFloor::make_order(DWORD kid)
{
    BYTE buffer[128];
    LPOTDM_PROTO_HEADER  omph = (LPOTDM_PROTO_HEADER) buffer;
    DWORD sz = sizeof(*omph)+sizeof(DWORD);
    omph->data_size = sizeof(DWORD)<<1;
    omph->command =  OTDMPROTO_CMD_GETKADR|OTDM_ORDER;
    omph->error   = 0;
    omph->data[0] = kid;
    omph->data[1] = MDB_KADR_FIELD_ENTRYES |MDB_KADR_FIELD_VALUES;
    queue_rxdata(FA_OTD_MEDIUM,buffer,sz,false);
}

DWORD         __fastcall TOutputFloor::floor_tx    (LPBYTE data,DWORD data_len)
{
 TGranTablet * gtbl = dynamic_cast<TGranTablet*>(owner);
 if(gtbl)
   return gtbl->floor_tx(data,data_len);
 return 0;
}

 LRESULT       __fastcall TOutputFloor::enum_output_kadrs( LPOUTPUT_KADR_PARAM okp,DWORD index)
 {
  LRESULT ret = GKH_RET_ERROR;
  lock();
  if(index<this->kadrs.size())
   {
    output_kadr & kdr = kadrs.at(index);
    okp->output_position = kdr.output_position;
    okp->output_mode     = kdr.output_mode;
    okp->output_kadr_id  = kdr.kadr.kadr_id;
    okp->output_entry_count = kdr.entry_list.size();
    okp->kadr_name_len   = lstrlenW(kdr.kadr_name);
    safe_strcpyn(okp->kadr_name,kdr.kadr_name,okp->kadr_name_size);
    ret = GKH_RET_SUCCESS;
   }
   else
   SetLastError(0/*ERR_NO_MORE_KADRS*/);
  unlock();
  return ret;
 }

 LRESULT       __fastcall TOutputFloor::get_output_kadr_param(LPOUTPUT_KADR_PARAM  okp)
 {
  LRESULT ret = GKH_RET_ERROR;
  lock();
   for(DWORD i = 0;i<kadrs.size();++i)
      {
       if(kadrs.at(i).output_position == okp->output_position)
         {
          ret = enum_output_kadrs(okp,i);
          i = kadrs.size();
         }
      }
  unlock();
  return ret;

 }



 bool          __fastcall TOutputFloor::create_output_kadr(LPOUTPUT_KADR_PARAM kp)
 {
  bool ret = false;
  lock();
  if(find_kadr(kp->output_kadr_id,kp->output_position)==kadrs.end())
  {
   kadrs.insert(lower_kadr(kp->output_kadr_id,kp->output_position),output_kadr(kp->output_kadr_id,kp->output_mode,kp->output_position));
   ret = true;
  }
  unlock();
  return ret;
 }

 bool          __fastcall TOutputFloor::delete_output_kadr(LPOUTPUT_KADR_PARAM kp)
 {
  bool ret = false;
  lock();
  if(kadrs.size())
  {
   output_kadr_vector::iterator ptr = find_kadr(kp->output_kadr_id,kp->output_position);
   if(ptr<kadrs.end())
   {
    ptr->entry_list.clear();
    ptr->entry_list.resize(0);
    kadrs.erase(ptr);
    ret = true;
   }
  }
  unlock();
  return ret;
 }


 bool          __fastcall TOutputFloor::modify_output_kadr(LPOUTPUT_KADR_PARAM kp,DWORD old_position)
 {
  bool ret = false;
  lock();
  if(kadrs.size())
  {
   output_kadr_vector::iterator ptr = find_kadr_by_position(old_position);
   if(ptr<kadrs.end())
   {
    ptr->output_mode     = kp->output_mode;
    ptr->output_position = kp->output_position;
    if(ptr->kadr.kadr_id != kp->output_kadr_id)
     {
      ptr->kadr.kadr_id    = kp->output_kadr_id;
      ptr->entry_list.clear();
      this->make_order(kp->output_kadr_id);
     }
     else
     do_output_kadr(ptr);
    if(old_position!= kp->output_position)
    {
     sort(kadrs.begin(),kadrs.end());
    }
    ret = true;
   }
  }
  unlock();
  return ret;
 }







