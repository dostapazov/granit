#pragma hdrstop
#include "cpc152dev.hpp"
#include <math.hpp>
#include <otd_arch_proto.h>



int  __fastcall cpc152dev_line::do_handle_recv   (LPMPROTO_HEADER mph)
{
       int error = 0;
       if(mph->fa == FA_DEVICE_RAW_CPC152)
       {
           lpcpc152proto_hdr phdr = (lpcpc152proto_hdr)mph->data;
           bool is_respond        = phdr->fa&CPC152PROTO_FA_FLAG_RESPOND;
           bool is_end            = phdr->fa&CPC152PROTO_FA_FLAG_END;
           switch(phdr->fa&CPC152PROTO_FA_MASK)
           {
            case CPC152PROTO_FA_SCAN_DATA:      error  =   handle_cpc152_scans(phdr,is_end,is_respond);     break;
            case CPC152PROTO_FA_SYNCTIME:       error  =   handle_time_sync(mph,phdr);break;
            case CPC152PROTO_FA_NEW_ALARM        :
             if(!is_respond)    error  =  handle_alarms_list(phdr,true,true);
                 break;
            case CPC152PROTO_FA_GET_ALARMS_LIST  :
            if(is_respond)     error  =   handle_alarms_list(phdr,is_end,false);
                 break;
            case CPC152PROTO_FA_GET_ALARM_FILE   :    error  = handle_alarm_file(phdr,is_end);    break;
            default:
            mph->fa &=0xFF; break;
           }
       }
     return error;
   }


int  __fastcall cpc152dev_line::handle_time_sync(LPMPROTO_HEADER mph,lpcpc152proto_hdr phdr)
{
    //print_string("Запрос на синхронизацию времени");
    phdr->fa |= CPC152PROTO_FA_FLAG_RESPOND|CPC152PROTO_FA_FLAG_END;
    LPQWORD pqw =  (LPQWORD)phdr->data;
    pqw [1] = KeRTL::GetTime(false);
    do_send(mph);
    return GKH_RET_SUCCESS;
}


int  __fastcall alarm_print_time(wchar_t * text,int bsz,QWORD timestamp)
{
  SYSTEMTIME st;
  FileTimeToSystemTime((LPFILETIME)&timestamp,&st);
  return snwprintf(text,bsz,L" %hd-%02hd-%02hd %02hd:%02hd:%02hd.%03hd",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond,st.wMilliseconds);
}

int  __fastcall cpc152dev_line::handle_alarms_list   (lpcpc152proto_hdr phdr,bool is_end,bool is_new)
{
  LPQWORD qptr      = (LPQWORD)phdr->data;
  LPQWORD qptr_end  = qptr+phdr->count;
  while(qptr<qptr_end)
  {
     alarms_list.push_back(*qptr);
     wchar_t text[512];
     int len = snwprintf(text,KERTL_ARRAY_COUNT(text),L"КП %d:получен %s архив ID-%Ld ",line_config.cp_number,is_new ? L"новый":L"существующий",*qptr);
     alarm_print_time(text+len,KERTL_ARRAY_COUNT(text)-len,*qptr);
     this->do_report(REPORT_INFORMATION_TYPE,text);
     ++qptr;
  }

  if(is_end && alarms_list.size() && !current_alarm)
  {
    request_alarm_archive(alarms_list.at(0),false);
  }
 return GKH_RET_SUCCESS;
}

int   __fastcall cpc152dev_line::handle_alarm_file    (lpcpc152proto_hdr phdr,bool is_end)
{
  int error = 0;
  LPBYTE   ptr = phdr->data;
  LPQWORD  alarm_id = (LPQWORD  )ptr;
  ptr+=sizeof(*alarm_id);

  sotd_proto * sop = NULL;
  if(*alarm_id == this->current_alarm)
  {
  lpcpc152scan_data sd = (lpcpc152scan_data)ptr;
  if(!alarm_times[0]) alarm_times[0] = sd->tm_beg;
  for(int i = 0;i<phdr->count && current_alarm;i++)
  {
   sd = (lpcpc152scan_data)ptr;
   int cur_len = (sd->dlen+sizeof(*sd)-sizeof(sd->data[0]));
   cur_len+=sizeof(WORD);//kpk
   DWORD check_sum = otd_calc_checksum(ptr,cur_len,OTD_DEF_POLINOM);
   if(!check_sum)
   {
     alarm_times[1] = sd->tm_beg;
     ptr+= cur_len;
     if(sd->dev_num&CPC152SCAN_DATA_DISCRETE)
        sop = handle_alarm_discrete_scan(current_alarm,sd,false);
     else
        sop = handle_alarm_analog_scan  (current_alarm,sd,false);
   }
   else
   {
     //Ошибка контрольной суммы
     //Прервать обработку
       wchar_t text[512];
       int len = snwprintf(text,KERTL_ARRAY_COUNT(text),L"КП %d:ошибка контрольной суммы при приёме ID-%Ld ",line_config.cp_number,current_alarm);
       alarm_print_time(text+len,KERTL_ARRAY_COUNT(text)-len,current_alarm);
       //do_report(REPORT_ERROR_TYPE,text);
       error = GKH_RET_ERROR;
   }
  }
     if(error || is_end && current_alarm)
     {
       wchar_t text[512];
       int len = snwprintf(text,KERTL_ARRAY_COUNT(text),L"КП %d:завершен прием архива ID-%Ld ",line_config.cp_number,current_alarm);
       len    += alarm_print_time(text+len,KERTL_ARRAY_COUNT(text)-len,current_alarm);
       if(error)
          snwprintf(text+len,KERTL_ARRAY_COUNT(text)-len,L" c ошибками ");
       do_report(REPORT_INFORMATION_TYPE,text);

       //Отправка завершения архива
       message_end_alarm_recv(alarm_times);
       request_alarm_archive(current_alarm,true);
       alarms_list.erase(alarms_list.begin());

       if(alarms_list.size())
          request_alarm_archive(alarms_list.at(0),false);
          error = 0;
     }

  }
  return error;

}

void  __fastcall cpc152dev_line::message_end_alarm_recv(QWORD * alarm_times)
{
       BYTE buff[1024];
       sotd_proto sop;
       int      proto_sz   =
       otd_arch_proto_format_list((LPBYTE)buff,sizeof(buff),get_cpc152controller()->get_pu_number(),line_config.cp_number,
                                  OTD_ARCH_CMD_ALARM_ARCHIEVES_GET|OTD_ARCH_FLAG_END_RESPOND,
                                  2,alarm_times,0,NULL,&sop.op);



       queue_rxdata(FA_OTD_ALARM_ARCHIVE,(LPBYTE)buff,proto_sz,false);

}

int  __fastcall cpc152dev_line::handle_cpc152_scans (lpcpc152proto_hdr phdr,bool is_end,bool is_respond)
{
  int error  = CPC152DEV_LINE_HANDLE_SUCCESS;
  LPBYTE ptr = phdr->data;
  for(int i = 0;i<phdr->count;i++)
  {
   lpcpc152scan_data sd = (lpcpc152scan_data)ptr;
   int cur_len = (sd->dlen+sizeof(*sd)-sizeof(sd->data[0]));
   cur_len+=sizeof(WORD);//kpk
   DWORD check_sum = otd_calc_checksum(ptr,cur_len,OTD_DEF_POLINOM);
   if(!check_sum)
   {
    if(sd->dev_num& CPC152SCAN_DATA_DISCRETE )
      error = handle_discrete_scan(sd,is_respond);
      else
      error = handle_analog_scan(sd);
      owner->notify(MNF_NOTIFY_CPC152_SCAN_DATA,line_config.cp_number,sd,cur_len);
    ptr+= cur_len;
   }
   else
   {
     wchar_t err_text[512];
     snwprintf(err_text,KERTL_ARRAY_COUNT(err_text),L"КП-%d.Ошибка контрольной суммы %02X",line_config.cp_number,check_sum);
     do_report(REPORT_ERROR_TYPE,err_text);
     error = CPC152DEV_LINE_HANDLE_ERROR_CHECKSUM;

   }
  }

  update_stats_rx(0,1,0);
  return error;
}


void __fastcall cpc152dev_line::start_write_oscilogramm(DWORD lenght)
{
 if(otd_locker) otd_locker->Lock();
  local_alarm_times[0] = last_scan_time;
  local_alarm_times[1] = last_scan_time+MSEC_NS100(lenght);
 if(otd_locker) otd_locker->Unlock();
}

int   __fastcall cpc152dev_line::handle_discrete_scan(lpcpc152scan_data sd,bool is_respond)
{
   int   error = CPC152DEV_LINE_HANDLE_SUCCESS;
   wchar_t text[128];
   int dev_num = sd->dev_num&CPC152SCAN_DATA_DEVNUM_MASK;
   int grp_num = sd->group_num;
   Tcpc152controller * ctrl = get_cpc152controller();
   DWORD   changes = 0;
   QWORD   _local_alarm_times[2] = {0};

   if(ctrl)
   {
       if(dev_num<CPC152_MAX_SLOTS)
       {
         if(grp_num < CPC152_MAX_GROUPS)
         {
           cpc152dev_t & dev = discrete_devs[dev_num];
           sotd_proto  & sop = dev.groups   [grp_num];
           last_scan_time =  sd->tm_beg ;
           if(!sop().addr)
              {
                //Создать группу
                ctrl->create_group(sop,true,this->line_config.cp_number,devgrp2otdgroup(true,dev_num,grp_num),CPC152_MAX_PARAM_DISCRETE,0);
                BYTE val = 1;
                memset(sd->data+3,-1,3);
                changes = -1;
              }
           //Начинаем обрабатывать
           DWORD new_diag       = (sd->dev_num&CPC152SCAN_DATA_NOTWORKED) ? OTD_DIAG_MODRESPOND : OTD_DIAG_GOOD;
           new_diag = 0;
           TLockHelper l(*otd_locker);
           memcpy(_local_alarm_times,local_alarm_times,sizeof(_local_alarm_times));
           if((*sop.op.diag)^new_diag)
             {
               *sop.op.diag = new_diag;
               changes|=OTD_PROTO_PART_DIAG;
               if(!new_diag)
                  changes|=OTD_PROTO_PART_DATA;
             }

           int bcount = 3;
           LPBYTE src = sd->data;
           LPBYTE cm  = sd->data+bcount;
           for(int i = 0,param_number = 0 ; i<bcount ;i++)
           {
             BYTE mask  = 1;
             for(int j = 0;j<8;j++,param_number++)
             {
               if(mask&(*cm))
               {
                BYTE val = mask&(*src) ? 1 : 0;
                if(otd_proto_setvalue(&sop(),param_number,&val,sizeof(val),0,TRUE))
                   changes|=OTD_PROTO_PART_DATA|OTD_PROTO_PART_PERSONAL_CHMASK;
               }
               mask<<=1;
             }
             ++src;
             ++cm;
           }
           if(changes || is_respond)
           {
            *sop.op.time_stamp = sd->tm_beg;//KeRTL::GetTime(false);;
            changes|= OTD_PROTO_PART_TIMESTAMP;
            otd_proto_protect(&sop.op,0,OTD_DEF_POLINOM);
            queue_rxdata(FA_OTD,(LPBYTE)sop.op.addr,sop.op.proto_size,false);
            BYTE zv = 0;
            otd_fill(sop.op.personal_chmask,&zv,sizeof(zv));
           }
         }
         else
         {
           snwprintf(text,KERTL_ARRAY_COUNT(text),L"КП-%d.Номер группы дискретного устройства слишком большой %d-%d",line_config.cp_number,dev_num,grp_num);
           do_report(REPORT_ERROR_TYPE,text);

         }
       }
       else
       {
        snwprintf(text,KERTL_ARRAY_COUNT(text),L"КП-%d.Номер дискретного устройства слишком большой %d",line_config.cp_number,dev_num);
        do_report(REPORT_ERROR_TYPE,text);
       }
     if(_local_alarm_times[1])
       {
         if(sd->tm_beg>_local_alarm_times[1])
            {
             message_end_alarm_recv(_local_alarm_times);
             bzero(local_alarm_times,sizeof(local_alarm_times));
            }
            else
            handle_alarm_discrete_scan(-1,sd,false);
       }
   }
  return error;
}




void  __fastcall cpc152dev_line::check_analog_seq_number(int sqn)
{
   wchar_t text[128];
   if(analog_seq>-1)
   {
      int seq_dif = sqn - analog_seq;
      if(seq_dif>1)
      {
        if(!(analog_seq == (int)0xFFFF && sqn == 0))
        {
           snwprintf(text,KERTL_ARRAY_COUNT(text),L"КП-%d.Пропуск аналоговых сканов с %d по %d",line_config.cp_number,analog_seq,sqn);
           do_report(REPORT_ERROR_TYPE,text);
        }

      }
   }
   analog_seq = sqn;
}

int  __fastcall cpc152dev_line::handle_analog_scan(lpcpc152scan_data sd)
{
   int error = CPC152DEV_LINE_HANDLE_SUCCESS;
   wchar_t text[128];
   BYTE buf1[1024];int blen1 = 0;
   BYTE buf2[2048];int blen2 = 0;
   QWORD  _local_alarm_times[2] = {0};
   Tcpc152controller * ctrl = get_cpc152controller();
   if(ctrl)
   {
       check_analog_seq_number(sd->seq_number);
       int dev_num = sd->dev_num&CPC152SCAN_DATA_DEVNUM_MASK;
       int grp_num = sd->group_num;
       if(dev_num<CPC152_MAX_SLOTS)
	   {
		 if(grp_num < CPC152_MAX_GROUPS)
		 {
		   DWORD ch_base = 0,ch_ext = 0;
		   cpc152dev_t & dev = analog_devs  [dev_num];
		   sotd_proto  & sop = dev.groups   [grp_num];
       		   short * vptr = (short*)sd->data;
                   int param_count = sd->dlen/sizeof(*vptr);
		   short * vptr_end = vptr + param_count;
                   last_scan_time =  sd->tm_beg ;
		   lpanalog_ext_t aext;

		   DWORD changes = 0;
		   if(!sop().addr)
			  {
				//Создать группу
				//В extenson хранится адрес параметров расчета каналов
				//Для аварийных архивов используем как LPOTD_ARCH_HEADER
				ctrl->create_group(sop,false,line_config.cp_number,devgrp2otdgroup(false,dev_num,grp_num),param_count,0);
				BYTE val = 1;
				otd_fill(sop.op.personal_chmask,&val,sizeof(val));
				aext = new analog_ext_t(dev_num,grp_num,dft_enabled.dft_enabled[dev_num][grp_num]);
                                aext->init(line_config.samples_count);
                                aext->param_count = param_count;
				ctrl->create_group(aext->ext_result,false,line_config.cp_number,sop.op.addr->sb+100,param_count*3,0,true);
				dev.dev_ext[grp_num] = aext;
				ch_base = ch_ext = -1;
			  }
			  else
			  aext = (lpanalog_ext_t)dev.dev_ext[grp_num];

              //Начинаем обрабатывать
                         int scan_time_lenght = (sd->tm_end - sd->tm_beg);
                         int scan_param_time  = scan_time_lenght/param_count;
                         if(aext->avg_scan_time_lenght)
                            {
                             aext->avg_scan_time_lenght = (aext->avg_scan_time_lenght+scan_time_lenght)/2;
                             aext->avg_scan_time_param  = (aext->avg_scan_time_param+scan_param_time)/2;
                            }
                            else
                            {
                             aext->avg_scan_time_lenght =   scan_time_lenght;
                             aext->avg_scan_time_param  =   scan_param_time;
                            }
			 int param_number = 0;
			 while(vptr<vptr_end)
			 {
			  aext->add_param_scan(param_number,(double)*vptr);
			  ++param_number;
			  ++vptr;
			 }

			 if(!((++aext->scan_counter)%aext->scan_count))
			 {
                           if(aext->get_locker().TryLock())
                           {
                            recalc_analog_params(aext,sop,ch_base,ch_ext);

                            icp.PostStatus((LPARAM)aext,sizeof(*aext),NULL);
                            aext->get_locker().unlock();
                           }
                           else
                           {
                            aext->overtime_count++;
                           }
			 }

               memcpy(_local_alarm_times,local_alarm_times,sizeof(_local_alarm_times));
               DWORD new_diag       = (sd->dev_num&CPC152SCAN_DATA_NOTWORKED) ? OTD_DIAG_MODRESPOND : OTD_DIAG_GOOD;
               new_diag = 0;
               TLockHelper l(*otd_locker);

               if((*sop.op.diag)^new_diag)
                 {
                   *sop.op.diag = new_diag;
                   *aext->ext_result.op.diag = new_diag;
                   ch_ext = ch_base =OTD_PROTO_PART_DIAG;
                 }

                 if(ch_base+ch_ext)
                  {
                      BYTE zv = 0;
                      if(ch_base)
                         {
                          *sop.op.time_stamp     = sd->tm_beg;//KeRTL::GetTime(false);//;
                          *sop.op.time_stamp_end = *sop.op.time_stamp;//sd->tm_end;
                          otd_proto_protect(&sop.op,0,OTD_DEF_POLINOM);
                          blen1 = sop.op.proto_size;
                          memcpy(buf1,sop.op.addr,blen1);
                          otd_fill(sop.op.personal_chmask,&zv,sizeof(zv));
                         }

                        if(ch_ext)
                         {
                                          *aext->ext_result.op.time_stamp     = sd->tm_beg;
                                          *aext->ext_result.op.time_stamp_end = sd->tm_end;
                                          otd_proto_protect(&aext->ext_result.op,0,OTD_DEF_POLINOM);
                          blen2 = aext->ext_result.op.proto_size;
                          memcpy(buf2,aext->ext_result.op.addr,blen2);
                          otd_fill(aext->ext_result.op.personal_chmask,&zv,sizeof(zv));
                         }


                      }
         }
         else
         {
           snwprintf(text,KERTL_ARRAY_COUNT(text),L"КП-%d.Номер группы дискретного устройства слишком большой %d-%d",line_config.cp_number,dev_num,grp_num);
           do_report(REPORT_ERROR_TYPE,text);
         }
       }
       else
       {
        snwprintf(text,KERTL_ARRAY_COUNT(text),L"КП-%d.Номер дискретного устройства слишком большой %d",line_config.cp_number,dev_num);
        do_report(REPORT_ERROR_TYPE,text);
       }
     if(_local_alarm_times[1])
       {
         if(sd->tm_beg>_local_alarm_times[1])
            {
             message_end_alarm_recv(_local_alarm_times);
             bzero(local_alarm_times,sizeof(local_alarm_times));
            }
            else
            handle_alarm_analog_scan(-1,sd,false);
       }

   }

  if(blen1)
   queue_rxdata(FA_OTD,(LPBYTE)buf1,blen1,false);
  if(blen2)
   queue_rxdata(FA_OTD,(LPBYTE)buf2,blen2,false);
   return error;
}




void  __fastcall cpc152dev_line::setup_analog_calc_params()
{
   Tcpc152controller * mod = get_cpc152controller();

   for(int i = 0;i<(int)KERTL_ARRAY_COUNT(analog_devs);i++)
   {
      cpc152dev_t & dev = analog_devs[i];
      for(int j = 0;j<(int)KERTL_ARRAY_COUNT(dev.groups);j++)
      {

		 lpanalog_ext_t aext = (lpanalog_ext_t)dev.dev_ext[j];
		 if(aext)
		   aext->init(line_config.samples_count);
      }
   }

    if(dft.get_dft_frame() !=  line_config.dft_frame_size )
        dft.setup(line_config.dft_frame_size,line_config.samples_freq,line_config.freq,line_config.freq_gate,line_config.dft_percent);
}

 sotd_proto*  __fastcall cpc152dev_line::handle_alarm_discrete_scan (QWORD alarm_id,lpcpc152scan_data sd,bool end)
 {
   int dev_num = sd->dev_num&CPC152SCAN_DATA_DEVNUM_MASK;
   int grp_num = sd->group_num;
   Tcpc152controller * ctrl = get_cpc152controller();
   DWORD   changes = 0;
   sotd_proto *  src_sop = NULL;
   if(ctrl)
   {
       if(dev_num<CPC152_MAX_SLOTS)
       {
         if(grp_num < CPC152_MAX_GROUPS)
         {
           cpc152dev_t & dev = discrete_devs[dev_num];
           src_sop = &dev.groups   [grp_num];
           otd_locker->lock();
           if(src_sop->op.addr)
           {
             BYTE buf[256];
             memcpy(buf,src_sop->op.addr,src_sop->op.proto_size);
             otd_locker->unlock();
             sotd_proto sop;
             sop.assign(buf,src_sop->op.proto_size);
             if(sop.op.extension)
             {
                 LPOTD_ARCH_HEADER ah = (LPOTD_ARCH_HEADER) sop.op.extension->block_data;
                 LPOTD_ARCH_LIST   al = (LPOTD_ARCH_LIST)ah->data;
                 if(end)
                 ah->cmd = OTD_ARCH_CMD_ALARM_ARCHIEVES_GET_AND_ERASE|OTD_ARCH_FLAG_END_RESPOND;
                 else
                 ah->cmd = 0;
                 ah->data_size  = sizeof(*al);
                 al->count      = 1;
                 al->archive[0] = alarm_id;
             }

             DWORD new_diag       = (sd->dev_num&CPC152SCAN_DATA_NOTWORKED) ? OTD_DIAG_MODRESPOND : OTD_DIAG_GOOD;
             *sop.op.diag = new_diag;

           int bcount = 3;
           LPBYTE src = sd->data;
           for(int i = 0,param_number = 0 ; i<bcount ;i++)
           {
             BYTE mask  = 1;
             for(int j = 0;j<8;j++,param_number++)
             {
                BYTE val = mask&(*src) ? 1 : 0;
                otd_proto_setvalue(&sop(),param_number,&val,sizeof(val),0,FALSE);
                mask<<=1;
             }
             ++src;
           }
            *sop.op.time_stamp = sd->tm_beg;
            BYTE zv = 1;
            otd_fill(sop.op.personal_chmask,&zv,sizeof(zv));
            otd_proto_protect(&sop.op,0,OTD_DEF_POLINOM);
            queue_rxdata(FA_OTD_ALARM_ARCHIVE,(LPBYTE)sop.op.addr,sop.op.proto_size,false);
           }
           else
           otd_locker->unlock();
         }
       }
   }
   return src_sop;
 }

sotd_proto  *  __fastcall cpc152dev_line::handle_alarm_analog_scan   (QWORD alarm_id,lpcpc152scan_data sd,bool end)
 {
   Tcpc152controller * ctrl = get_cpc152controller();
   sotd_proto  * src_sop = NULL;
   if(ctrl)
   {
       int dev_num = sd->dev_num&CPC152SCAN_DATA_DEVNUM_MASK;
       int grp_num = sd->group_num;
       if(dev_num<CPC152_MAX_SLOTS)
       {
         if(grp_num < CPC152_MAX_GROUPS)
         {
           cpc152dev_t & dev = analog_devs  [dev_num];
           src_sop = &dev.groups   [grp_num];
           otd_locker->Lock();
           if(src_sop->op.addr)
           {
             BYTE buf[4096];
             memcpy(buf,src_sop->op.addr,src_sop->op.proto_size);
             otd_locker->unlock();
             sotd_proto sop;
             sop.assign(buf,src_sop->op.proto_size);
             if(sop.op.extension)
             {
                 LPOTD_ARCH_HEADER ah = (LPOTD_ARCH_HEADER) sop.op.extension->block_data;
                 LPOTD_ARCH_LIST   al = (LPOTD_ARCH_LIST)ah->data;
                 if(end)
                 ah->cmd = OTD_ARCH_CMD_ALARM_ARCHIEVES_GET_AND_ERASE|OTD_ARCH_FLAG_END_RESPOND;
                 else
                 ah->cmd = 0;
                 ah->data_size  = sizeof(*al);
                 al->count      = 1;
                 al->archive[0] = alarm_id;
             }
             short * vptr = (short*)sd->data;
             short * vptr_end = vptr + sd->dlen/sizeof(*vptr);
             int param_number = 0;
             while(vptr<vptr_end)
             {
              otd_set_value(sop.op.data,param_number,vptr,sizeof(*vptr));
              ++param_number;
              ++vptr;
             }
             *sop.op.time_stamp     = sd->tm_beg;
             *sop.op.time_stamp_end = sd->tm_end;
             BYTE zv = 1;
             otd_fill(sop.op.personal_chmask,&zv,sizeof(zv));
             otd_proto_protect(&sop.op,0,OTD_DEF_POLINOM);
             queue_rxdata(FA_OTD_ALARM_ARCHIVE,(LPBYTE)sop.op.addr,sop.op.proto_size,false);
           }
           else
           otd_locker->unlock();
         }
       }
   }
   return src_sop;
 }

void  __fastcall cpc152dev_line::request_alarm_archive(QWORD alarm_id,bool erase)
{
      char buff[512];
      wchar_t text[512];
      LPMPROTO_HEADER mph = (LPMPROTO_HEADER)buff;
      lpcpc152proto_hdr phdr = (lpcpc152proto_hdr)mph->data;
      LPQWORD arch_id = (LPQWORD)phdr->data;
      *arch_id        = alarm_id;
      phdr->count = 1;
      if(erase)
      {
         phdr->fa      = CPC152PROTO_FA_ERASE_ALARM_FILE;
         if(current_alarm == alarm_id)
            current_alarm = 0;
          snwprintf(text,KERTL_ARRAY_COUNT(text),L"КП %d:команда на удаление архива %Ld отправлена",line_config.cp_number,alarm_id);

      }
      else
      {
          current_alarm   = alarm_id;
          phdr->fa    = CPC152PROTO_FA_GET_ALARM_FILE;
          snwprintf(text,KERTL_ARRAY_COUNT(text),L"КП %d:запрос на получение архива %Ld отправлен",line_config.cp_number,alarm_id);
      }
      do_report(REPORT_INFORMATION_TYPE,text);
      phdr->fa    |= CPC152PROTO_FA_FLAG_END;
      mproto_init   (mph,0,FA_DEVICE_RAW_CPC152,cpc152proto_hdr_calc_size(sizeof(alarm_id)));
      do_send(mph);
      alarm_recv_timer = 0;
      alarm_times[0] = alarm_times[1] = 0;
}

bool __fastcall cpc152dev_line::get_analog_stats       (LPCPC152_ANALOG_STATS stats)
{
    bool ret = false;
  if(owner->check_data_size(stats,sizeof(*stats)))
  {
    if(otd_locker)  otd_locker->lock();
    if(stats->dev_num<CPC152_MAX_SLOTS && stats->grp_num < CPC152_MAX_GROUPS)
    {
     DWORD ch_base = 0,ch_ext = 0;
     cpc152dev_t & dev = analog_devs  [stats->dev_num];
     lpanalog_ext_t aext = (lpanalog_ext_t)dev.dev_ext[stats->grp_num];
     if(aext)
       {
          ret = true;
          stats->param_count    = aext->param_count;
          stats->overtimes      = aext->overtime_count;
          stats->avg_scan_param_time = aext->avg_scan_time_param;
          stats->avg_scan_time  = aext->avg_scan_time_lenght;
       }
    }

    if(otd_locker)  otd_locker->unlock();
  }
    return ret;
}

bool __fastcall cpc152dev_line::enum_analogs           (LPCPC152_ANALOG_STATS stats)
{
   bool ret = owner->check_data_size(stats,sizeof(*stats));
   if(ret)
   {
       ret = false;
       proto_pointer b_ptr,e_ptr;
       sotd_addr sa(-1);
       sa.cp     = line_config.cp_number;
       sa.fa     = OTD_FA_ANALOG;
       Tcpc152controller * c = get_cpc152controller();

       if(c)
       {
           c->otd_lock();
           int range_count = c->get_otd_range(sa,b_ptr,e_ptr);
           int curr_idx    = 0;
           while(b_ptr<e_ptr)
           {
             if(b_ptr->op.addr->fa == OTD_FA_ANALOG)
               {
                 if(curr_idx == stats->enum_idx)
                  {
                     stats->otd_grp_num =  b_ptr->op.addr->sb;
                     otdgroup2devgrp(false,stats->otd_grp_num,stats->dev_num,stats->grp_num);
                     ret =  get_analog_stats(stats);
                     break;
                  }
                ++curr_idx;
               }
            ++b_ptr;
           }
           c->otd_unlock();
       }
   }
  return ret;
}







