#pragma hdrstop
#include "vtdcnt.hpp"

WORD __fastcall vtd_line::calc_vtd_crc(LPBYTE buf,DWORD len)
{
int i;
WORD     crc = 0xFFFF;
while (len--)
{
crc ^= *buf++;
for (i = 0; i < 8; ++i)
if (crc & 0x01) { crc >>= 1; crc ^= 0xA001; }
else crc >>= 1;
}
return crc;
}

  vtd_line::vtd_line(VTDDEV_PARAM & param):modem_line(param.number)
  {
   memcpy(&dev_param,&param,sizeof(dev_param));
   ZeroMemory(&dev_date_time,sizeof(dev_date_time));
   ZeroMemory(&base_config,sizeof(base_config));
  }

   DWORD __fastcall vtd_line::get_line_text  (wchar_t * text,DWORD text_sz)
   {
     wchar_t buf[MAX_PATH];
     DWORD len;
     switch(dev_param.type)
     {
      case VTD_TYPE_STD : len = wsprintfW(buf,L"Счетчик ВТД %d [%s]",dev_param.number,dev_param.name);break;
      case VTD_TYPE_VG  : len = wsprintfW(buf,L"Счетчик ВТД-В,Г %d [%s]",dev_param.number,dev_param.name);break;
      case VTD_TYPE_Y   : len = wsprintfW(buf,L"Счетчик ВТД-У %d [%s]",dev_param.number,dev_param.name);break;
     }
     len = KeRTL::MIN(text_sz,len+1);
     safe_strcpyn(text,buf,len);
     return len;
   }


  void  __fastcall vtd_line::set_line_number(DWORD new_num)
  {
    dev_param.number = new_num;
    DWORD lock_cnt = 0;
    if(owner)
       lock_cnt= owner->full_unlock();
    modem_line::set_line_number(new_num);
    if(lock_cnt)
       owner->restore_lock(lock_cnt);

  }

   bool  __fastcall vtd_line::do_start ()
   {

    return modem_line::do_start();
   }

   bool  __fastcall vtd_line::do_stop  ()
   {
    set_diag(OTD_DIAG_PUCONNECT);
    return modem_line::do_stop();
   }

   void  __fastcall vtd_line::on_connect()
   {
    modem_line::on_connect();
   }

   void  __fastcall vtd_line::on_disconnect()
   {
    ZeroMemory(&dev_date_time,sizeof(dev_date_time));
    ZeroMemory(&base_config,sizeof(base_config));
    set_diag(OTD_DIAG_NODATA|OTD_DIAG_CPCONNECT,true);
    modem_line::on_disconnect();
   }

   void __fastcall vtd_line::set_diag(DWORD diag,bool send)
   {
    if(diag& OTD_DIAG_MASK)
    {
      proto_pointer b_ptr = groups.begin(),e_ptr = groups.end();

      DWORD  pd = OTD_PDIAG_NODATA;
      while(b_ptr<e_ptr)
      {
       otd_fill(b_ptr->op.personal_diag,&pd,sizeof(pd));
       *b_ptr->op.diag = diag|OTD_PART_DIAG_PARAM;
       ++b_ptr;
      }
      if(send)
      {
        BYTE buf[1024];
        sotd_proto sop;
        DWORD len = otd_proto_format(buf,sizeof(buf),OTD_PROTO_PART_DIAG|OTD_PROTO_PART_KPK,0,0,0,0,0,0,&sop.op);
        sop.op.addr->addr = -1;
        sop.op.addr->pu = 0;
        sop.op.addr->cp = this->dev_param.number;
        *sop.op.diag = diag;
        otd_proto_protect(&sop.op,len,OTD_DEF_POLINOM);
        queue_rxdata(FA_OTD,buf,len,false);
      }
    }

   }

   bool  __fastcall vtd_line::send        (LPMPROTO_HEADER mph,DWORD sz)
   {
    if(mph->fa == FA_OTD)
    {
      sotd_proto sop((LPBYTE)mph->data,mph->data_size);
      if(OTD_ADDR_ISQUERY(sop().addr) && ((DWORD)sop.op.addr->cp == dev_param.number || sop.op.addr->cp == OTD_ADDR_MAXVALUE) )
      {
        //если вызов
        DWORD cp_diag = 0;
        DWORD parts = ((*sop.op.ver_parts)&OTD_PROTO_PARTSMASK)|OTD_PROTO_PART_KPK;
         //lock();
         proto_pointer b_ptr = groups.begin(),e_ptr = groups.end();
         sotd_addr sa(*sop().addr);
         sa.pu = sa.cp = 0;
         sa.fa&=OTD_FA_ALL;
         groups.get_range(sa,b_ptr,e_ptr);
         while(b_ptr<e_ptr)
         {
          send_group(b_ptr,parts,0,-1,mph->addr_from.addr);
          if(*(b_ptr->op.diag)&OTD_DIAG_MASK)
             cp_diag|= OTD_PART_DIAG_PARAM;
          ++b_ptr;
         }

        if((sop.op.addr->fa&OTD_FA_ALL)==OTD_FA_ALL)
        {
         //вызов по всему КП
         if(!this->is_connected())
           cp_diag|=OTD_DIAG_CPCONNECT;
         if(owner)
           {
            TVtdCntModem * modem = dynamic_cast<TVtdCntModem*>(owner);
            if(!modem || !modem->chek_server_connect())
               cp_diag|= OTD_DIAG_PUCONNECT;
           }
         BYTE buff[1024];
         sotd_proto sop;
         DWORD len = otd_proto_format(buff,sizeof(buff),parts,0,0,0,0,0,
                                      (parts&OTD_PROTO_PART_NAME)?lstrlenW(dev_param.name):0,&sop.op);
         if(sop.op.diag)
            *sop.op.diag = cp_diag;
         if(sop.op.name )
           otd_text_set(sop.op.name,dev_param.name);
         sop.op.addr->addr = -1;
         sop.op.addr->pu   = 0;
         sop.op.addr->cp   = dev_param.number;
         otd_proto_protect(&sop.op,len,OTD_DEF_POLINOM);
         queue_rxdata(FA_OTD,buff,len,false,mph->addr_from.addr);
        }

        //unlock();
      }
    }
    return true;
   }

 void        __fastcall vtd_line::send_group(proto_pointer ptr,DWORD parts,DWORD lo_num,DWORD hi_num,DWORD to)
 {
          BYTE buff[2048];
          otd_proto_param opp;
          opp.dw_size = sizeof(opp);
          otd_proto_getparam(&ptr->op,&opp);
          opp.parts  = parts;
          opp.pd_param.first = opp.cm_param.first = opp.dt_param.first = lo_num;
          opp.pd_param.count = opp.cm_param.count = opp.dt_param.count = hi_num-lo_num+1;
          DWORD len = otd_proto_getparts_ex(buff,sizeof(buff),&opp,&ptr->op,0);
          sotd_proto sop(buff,len);
          sop.op.addr->cp = get_number();
          queue_rxdata(FA_OTD,buff,len,false,to);

 }

 DWORD __fastcall vtd_line::refresh(DWORD sbl)
  {
    switch(this->dev_param.type)
    {
      case VTD_TYPE_STD : refresh_std(sbl);break;
      case VTD_TYPE_VG  : refresh_vg(sbl);break;
      case VTD_TYPE_Y   : refresh_y (sbl);break;
    }
    return GKH_RET_SUCCESS;
   }

   bool  __fastcall vtd_line::handle_respond(lpvtd_query_kadr qk,lpvtd_resp_kadr vk)
   {
     bool ret = true;
     connect(true);
     DWORD kadr_len = sizeof(*vk)+vk->data_len+1;
     DWORD err = 0;
     if(calc_vtd_crc((LPBYTE)vk,kadr_len)==0)
     {
      owner->notify(VTDNF_TXRX,VTDNC_RXKADR_STD+dev_param.type,vk,kadr_len);
      switch(this->dev_param.type)
      {
      case VTD_TYPE_STD : handle_respond_std(qk,vk);break;
      case VTD_TYPE_VG  : handle_respond_vg(qk,vk);break;
      case VTD_TYPE_Y   : handle_respond_y(qk,vk);break;
      default:
      break;
      }
     }
     else
     {
      ++err;
      //TODO записать в лог
      ret = false;
     }
     update_stats_rx(kadr_len,1,err);
     return ret;
   }

   bool       __fastcall vtd_line::update_device(LPVTDDEV_PARAM dp)
   {
    if(wcsncmpi(dev_param.name,dp->name,sizeof(dev_param.name)/sizeof(wchar_t)))
      {
       wcsncpy(dev_param.name,dp->name,sizeof(dev_param.name)/sizeof(wchar_t));


      }
      return true;
   }


   void __fastcall vtd_line::send_dev_query(LPVOID data,DWORD sz)
   {
     if(check_state(MODEM_LINE_STATE_WORKED,false) && owner)
        ((TVtdCntModem*)owner)->send_dev_query(data,sz);
   }

   bool        __fastcall vtd_line::enum_groups (DWORD idx,LPVTDDEV_GROUP grp)
   {
    bool ret = false;
    //lock();
    if(idx<groups.size())
    {
     proto_pointer ptr = groups.begin();
     std::advance(ptr,idx);
     grp->group_number = ptr->op.addr->sb;
     grp->first        = ptr->op.data->numbers.loN;
     grp->count        = OTD_GET_COUNT(&ptr->op.data->numbers);
     grp->query_period = *((LPDWORD)ptr->op.extension->block_data);
     ret = true;
    }
    //unlock();
    return ret;
   }

   bool        __fastcall vtd_line::get_group   (DWORD number,LPVTDDEV_GROUP grp)
   {
    bool ret = false;
    //lock();
    sotd_addr addr(0,0,OTD_FA_ANALOG1,number);
    proto_pointer ptr = groups.find(addr);
    if(ptr < groups.end())
    {
     ret = true;
     if(grp)
     {
     grp->group_number = ptr->op.addr->sb;
     grp->first        = ptr->op.data->numbers.loN;
     grp->count        = OTD_GET_COUNT(&ptr->op.data->numbers);
     grp->query_period = *((LPDWORD)ptr->op.extension->block_data);
     }

    }
    //unlock();
    return ret;
   }


   bool        __fastcall vtd_line::create_group(LPVTDDEV_GROUP grp)
   {
    bool ret = false;
    //lock();
     // Создание группы
     otd_proto_param opp = {0};
     opp.dw_size = sizeof(opp);
     opp.parts = OTD_PROTO_PART_KPK |
                 OTD_PROTO_PART_DIAG|
                 OTD_PROTO_PART_DATA|
                 OTD_PROTO_PART_PERSONAL_DIAG |
                 OTD_PROTO_PART_PERSONAL_CHMASK|
                 OTD_PROTO_PART_TIMESTAMP|
                 OTD_PROTO_PART_PROTOEXTENSION;
    opp.dt_param.type  = OTD_FLOAT;
    opp.pd_param.type  = OTD_BYTE;
    opp.cm_param.type  = OTD_BYTE;
    opp.dt_param.first = opp.pd_param.first = opp.cm_param.first = grp->first;
    opp.dt_param.count = opp.pd_param.count = opp.cm_param.count = grp->count;
    opp.ext_size       = sizeof(group_timer);
    DWORD nsz = otd_proto_calc_size_ex(&opp,0);
    LPBYTE buf = new BYTE[nsz];
    if(buf)
    {
     ret = true;
     sotd_proto sop;
     otd_proto_format_ex(buf,nsz,&opp,0,&sop.op);
     sop.op.addr->addr = sotd_addr(0,0,OTD_FA_ANALOG1,grp->group_number).addr;
     group_timer * gt =(group_timer*)sop.op.extension->block_data;
     gt->timer_value   = grp->query_period;
     gt->timer_counter = 0;
     groups.insert(sop());
      if(owner)
      {
       DWORD n_data = MAKELONG(this->line_number,grp->group_number);
       owner->notify(VTDNF_DEV_CHANGED,VTDNC_GROUP_CREATED,&n_data,sizeof(n_data));
       if(owner->is_running())
          refresh(grp->group_number);
       
      }
    }
    //unlock();
    return ret;
   }
   bool        __fastcall vtd_line::delete_group(DWORD number)
   {
    bool ret = false;
    //lock();
    sotd_addr addr(0,0,OTD_FA_ANALOG1,number);
    proto_pointer ptr = groups.find(addr);
    if(ptr < groups.end())
    {
     /*
      Отправить диагностику группы
     */
     *ptr->op.diag = OTD_DIAG_NODATA;
     this->send_group(ptr,OTD_PROTO_PART_DIAG,0,0);
     delete [] (LPBYTE)ptr->op.addr;
     groups.erase(ptr);
     ret = true;

      if(owner)
      {
       DWORD n_data = MAKELONG(this->line_number,number);
       owner->notify(VTDNF_DEV_CHANGED,VTDNC_GROUP_DELETED,&n_data,sizeof(n_data));
      }
    }
    //unlock();
    return ret;
   }

   bool        __fastcall vtd_line::update_group(DWORD number,LPVTDDEV_GROUP grp)
   {
    bool ret = false;
    //lock();
     if(number!=grp->group_number)
     {
       if(!get_group(grp->group_number,NULL))
       {
        delete_group(number);
        ret = create_group(grp);
       }
     }
     else
     {
      //Изменить параметры группы
      if(owner)
      {
       DWORD n_data = MAKELONG(this->line_number,number);
       proto_pointer ptr = groups.find(sotd_addr(0,0,OTD_FA_ANALOG1,grp->group_number));
       if(ptr<groups.end() && ptr->op.extension)
       {
         group_timer * gt =(group_timer*)ptr->op.extension->block_data;
         gt->timer_value = grp->query_period;
         if(gt->timer_value>=gt->timer_counter)
           this->refresh(grp->group_number);
         ret = true;  
       }
       owner->notify(VTDNF_DEV_CHANGED,VTDNC_GROUP_UPDATED,&n_data,sizeof(n_data));
      }

     }
    //unlock();
    return ret;
   }

   void        __fastcall vtd_line::swrite(TStream & s)
   {
    //Функция записи в поток
    s.Write(&dev_param,sizeof(dev_param));
    DWORD grp_count = groups.size();
    s<< grp_count;
    VTDDEV_GROUP dg;
    dg.dw_size = sizeof(dg);
    for(DWORD i = 0;i<grp_count;i++)
     {
      enum_groups(i,&dg);
      s.Write(&dg,sizeof(dg));
     }
   }

   void        __fastcall vtd_line::sread (TStream & s)
   {
     s.Read(&dev_param,sizeof(dev_param));
     line_number = dev_param.number;
     DWORD grp_count;
     s>>grp_count;
      VTDDEV_GROUP dg;

     for(DWORD i = 0;i<grp_count;i++)
     {
      s.Read(&dg,sizeof(dg));
      this->create_group(&dg);
     }


   }

   void __fastcall vtd_line::on_timer()
   {
     proto_pointer ptr = groups.begin(),p_end = groups.end();
     while(ptr<p_end)
     {
      group_timer * gt =(group_timer *)(ptr->op.extension ? ptr->op.extension->block_data: NULL);
      if(gt && ++gt->timer_counter>=gt->timer_value)
      {
         switch(this->dev_param.type)
         {
          case VTD_TYPE_VG:break;
          case VTD_TYPE_Y:break;
          default: refresh_grp_std(ptr);break;
         }
         gt->timer_counter = 0;
      }
      ++ptr;
     }

   }



 /*Пока функции заглушки реализации других типов счётчиков*/
   void        __fastcall vtd_line::refresh_vg (DWORD mask )
   {
   }
   void        __fastcall vtd_line::refresh_y  (DWORD mask )
   {
   }


   bool  __fastcall vtd_line::handle_respond_vg(lpvtd_query_kadr qk,lpvtd_resp_kadr vk)
   {
    return false;
   }

   bool  __fastcall vtd_line::handle_respond_y(lpvtd_query_kadr qk,lpvtd_resp_kadr vk)
   {
    return false;
   }

   bool __fastcall vtd_line::get_group_data(LPVTD_GET_GROUP_DATA dg)
   {
    sotd_addr addr(0,0,OTD_FA_ANALOG1,dg->group);
    dg->data_len = groups.get_parts_addr(addr,dg->parts,0,-1,dg->data,dg->dw_size-sizeof(*dg)+1);
    return dg->data_len ? true:false;
   }






