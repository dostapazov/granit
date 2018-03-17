#pragma hdrstop
 #include "vtdcnt.hpp"

  void        __fastcall vtd_line::refresh_std(DWORD sub_line  )
  {
    //Обновление
    _vtd_query_kadr qk = {0};
    qk.dev_number = get_number();
    if(sub_line == 0)
      sub_line = -1;
    if(sub_line == (WORD)-1 || sub_line == (DWORD)-1 || dev_date_time.c_code.bt[0] == 0)
    {
    qk.command    = VTD_QUERY_DATE_TIME;
    qk.crc =  calc_vtd_crc((LPBYTE)&qk,sizeof(qk)-sizeof(qk.crc));
    send_dev_query(&qk,sizeof(qk));
    qk.command = VTD_QUERY_BASE_CONFIG;
    qk.crc =  calc_vtd_crc((LPBYTE)&qk,sizeof(qk)-sizeof(qk.crc));
    send_dev_query(&qk,sizeof(qk));
    }
    //lock();

    proto_pointer p_beg = groups.begin(),p_end = groups.end();
    groups.get_range(sotd_addr(0,0,OTD_FA_ANALOG1,sub_line),p_beg,p_end);
    while(p_beg<p_end)
       {
        
        refresh_grp_std(p_beg++);
       } 
    //unlock();
   }

   bool        __fastcall vtd_line::is_group_exist(DWORD num)
   {
    sotd_addr sa(0,0,OTD_FA_ANALOG1,num);
    return groups.find(sa)<groups.end() ? true:false;
   }

   void        __fastcall vtd_line::refresh_grp_std(proto_pointer ptr)
   {
    group_timer * gt = NULL;
       if(ptr->op.extension)
        gt =(group_timer*) ptr->op.extension->block_data;
   if(gt && gt->timer_counter<gt->timer_value)
       gt->timer_counter=gt->timer_value -1;
    //else
    {
    _vtd_query_kadr qk = {0};
    qk.dev_number = get_number();
    qk.Npipe      = ptr->op.addr->sb;
    if(qk.Npipe & VTD_CURR_PARAM)
       {
        //Если текущие параметры
        qk.command = VTD_QUERY_CURRENT_PARAM;
        //Должны вызываться попарно сначала параметрв труб
        //Затем потребителей
        if(!(qk.Npipe & VTD_USER_PARAM) || !is_group_exist((qk.Npipe&~VTD_USER_PARAM)))
          {
           qk.Npipe &=~VTD_CURR_PARAM;
           qk.crc =  calc_vtd_crc((LPBYTE)&qk,sizeof(qk)-sizeof(qk.crc));
           send_dev_query(&qk,sizeof(qk));
           qk.Npipe |= VTD_USER_PARAM;
           qk.crc =  calc_vtd_crc((LPBYTE)&qk,sizeof(qk)-sizeof(qk.crc));
           send_dev_query(&qk,sizeof(qk));
          }
       }
       else
       {
         qk.command = VTD_QUERY_PARAM;
         qk.Npar       = ptr->op.data->numbers.loN;
         qk.CNTpar     = OTD_GET_COUNT(&ptr->op.data->numbers);
         qk.crc =  calc_vtd_crc((LPBYTE)&qk,sizeof(qk)-sizeof(qk.crc));
         send_dev_query(&qk,sizeof(qk));

       }
       if(gt) gt->timer_counter = 0;
     }  
   }


   bool  __fastcall vtd_line::handle_respond_std(lpvtd_query_kadr qk,lpvtd_resp_kadr vk)
   {
     bool ret = true;
     switch(vk->command)
     {
      case   VTD_QUERY_PARAM         : handle_param_std(qk,vk);break;
      case   VTD_QUERY_CURRENT_PARAM : handle_curr_param_std(qk,vk);break;
      case   VTD_QUERY_DATE_TIME     :
                                      memcpy(&dev_date_time,vk->data,vk->data_len);
                                      break;
      case   VTD_QUERY_BASE_CONFIG   :
                                      memcpy(&this->base_config,vk->data,vk->data_len);
                                      break;
      default: ret = false;
     }
     return ret;
   }

   void        __fastcall vtd_line::handle_param_std(lpvtd_query_kadr qk,lpvtd_resp_kadr vk)
   {
     DWORD ch_parts = 0,lo_num = -1,hi_num = 0;
     proto_pointer grp_ptr = groups.find(sotd_addr(0,0,OTD_FA_ANALOG1,qk->Npipe));
     
     if(grp_ptr< groups.end())
     {
      /*Сбросить счётчик  таймера опроса для исключения повторного вызова от тек парамеров потребит*/
       if(grp_ptr->op.extension)
       {
        group_timer * gt =(group_timer*) grp_ptr->op.extension->block_data;
        gt->timer_counter = 0;
       }

      float * param_ptr = (float*) vk->data;
      DWORD curr_pn    = qk->Npar;
      DWORD last_pn    = curr_pn + (DWORD)vk->data_len/sizeof(float);
      DWORD new_diag   = (*grp_ptr->op.diag ) & ~OTD_DIAG_MASK;

      while(curr_pn<last_pn)
      {
       float old_value = *param_ptr;
       BYTE  old_pd = 0,new_pd = 0;// снять персональную недостоверность параметра
       otd_get_value(grp_ptr->op.data,curr_pn,&old_value,sizeof(old_value));
       otd_get_value(grp_ptr->op.personal_diag,curr_pn,&old_pd,sizeof(old_pd));


       if(*param_ptr != old_value)
          {
           otd_set_value(grp_ptr->op.data,curr_pn,param_ptr,sizeof(*param_ptr));
           ch_parts|=OTD_PROTO_PART_DATA;
           if(lo_num ==(DWORD) -1)
              lo_num = curr_pn;
            hi_num = curr_pn;  

          }

       if(old_pd!=new_pd)
        {
         otd_set_value(grp_ptr->op.personal_diag,curr_pn,&new_pd,sizeof(new_pd));
         ch_parts|=(OTD_PROTO_PART_PERSONAL_DIAG|OTD_PROTO_PART_DATA);
           if(lo_num ==(DWORD) -1)
              lo_num = curr_pn;
            hi_num = curr_pn;
        }
       ++param_ptr;
       ++curr_pn;
      }

      new_diag |= otd_scan_personal_diag(&grp_ptr->op)&~OTD_DIAG_MASK;
      DWORD diag_cm = new_diag^(*grp_ptr->op.diag);
      if(diag_cm)
         {
          ch_parts|=OTD_PROTO_PART_DIAG;
          *grp_ptr->op.diag = new_diag;
          if(diag_cm & OTD_DIAG_MASK)
            {
             lo_num = grp_ptr->op.data->numbers.loN,hi_num = grp_ptr->op.data->numbers.hiN;
             ch_parts|=OTD_PROTO_PART_DATA|OTD_PROTO_PART_PERSONAL_DIAG;

            } 

         }
       if(ch_parts)
         {
           send_group(grp_ptr,ch_parts,lo_num,hi_num);
           if(ch_parts&OTD_PROTO_PART_DATA)
           {

            DWORD number = MAKELONG(this->get_number(),(DWORD)(grp_ptr->op.addr->sb));
            owner->notify(VTDNF_DEV_CHANGED ,VTDCNT_GROUP_DATA_CHANGED,&number,sizeof(number));
           } 
         }
     }
   }

   void        __fastcall vtd_line::handle_curr_param_std(lpvtd_query_kadr qk,lpvtd_resp_kadr vk)
   {

    //Подменить адрес
    qk->Npipe|=VTD_CURR_PARAM;
    if((qk->Npipe&0x80)==0)
    {
     lpvtd_pipe_curr_param pcp = (lpvtd_pipe_curr_param) vk->data;
     float time_replace = pcp->time.hour*10000+pcp->time.min*100+pcp->time.sec;
     float * dest = (float*)&pcp->time;
     *dest = time_replace;
    }
    handle_param_std(qk,vk);

   }



