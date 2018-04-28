#pragma hdrstop
#include "otd_dblow.hpp"

 #pragma warn -8057
  DWORD  __fastcall otd_dblow::handle(otd_proto & op, modem_addr & from,LPDWORD lo_obj,LPDWORD hi_obj)
  {
     DWORD ret;
     wr_locker l(*this);
     if(OTD_FA_ISCTRL(op.addr->fa))
       ret = 0;//this->handle_ctrl(op,from);
     else
     {

     if(sotd_addr(op.addr->addr).addrtype() == OTD_ADDR_TYPE_SB)
        ret = handle_sb(op,lo_obj,hi_obj);
        else
        ret = handle_group(op,lo_obj,hi_obj);
      if(ret)
         this->item_changed(this->find(*op.addr),ret,*lo_obj,*hi_obj);
     }
    return ret;
  }
 #pragma warn .8057

  DWORD  __fastcall otd_dblow::handle_name(proto_pointer ptr,otd_block & name )
  {
      DWORD ret = 0;
      if(ptr->op.name &&  otd_text_compare(ptr->op.name,&name,TRUE))
        {
         otd_text_copy(ptr->op.name,&name);
         ret = OTD_PROTO_PART_NAME;
        }
    return ret;
  }

  DWORD  __fastcall otd_dblow::handle_group(otd_proto & op,LPDWORD lo_obj,LPDWORD hi_obj)
  {
   //Обработка ПУ
   DWORD ret     = 0;
   proto_pointer  ptr = this->find(*op.addr);
   if(ptr>=otd_list.end() && op.name)
   {
    ptr = create_group(*op.addr);
    ret|=OTD_PROTO_PART_DIAG;
   }
   if(ptr<otd_list.end())
    {
      if(op.name )
          ret|= handle_name(ptr,*op.name);

     if(op.diag)
        ret|=handle_diag(ptr,*op.diag,lo_obj,hi_obj);

     }
     else
     {
      if(op.addr->pu == OTD_ADDR_MAXVALUE && (*op.diag&OTD_DIAG_PUMASK))
       {
        proto_pointer l_ptr = this->begin(),h_ptr = end();
        while(l_ptr< h_ptr)
        {
         sotd_addr sa(*op.addr);
         if(sa.is_include(*l_ptr->op.addr,true))
            {
             ret|=handle_diag(l_ptr,*op.diag,lo_obj,hi_obj);
             this->item_changed(l_ptr,OTD_PROTO_PART_DIAG,*lo_obj,*hi_obj);
            } 
         ++l_ptr;
        }
       }
     }
    
   return ret;
  }

 #pragma warn -8057
  DWORD  __fastcall otd_dblow::handle_ctrl(otd_proto & op,modem_addr & from,LPDWORD,LPDWORD)
  {
   //Обработка команда управление
   DWORD ret = 0;
   return ret;
  }


  DWORD   __fastcall otd_dblow::handle_diag(proto_pointer ptr,DWORD diag,LPDWORD lo_ch,LPDWORD hi_ch)
  {
    DWORD ret = 0;
    DWORD ch  = (*ptr->op.diag)^diag;
    if(ch)
    {
      DWORD set_mask   = diag&ch;
      DWORD reset_mask = (~diag)&ch;
      ch = 0;
      if(reset_mask)
        ch |= set_diag(ptr,reset_mask,false);
      if(set_mask)
        ch |= set_diag(ptr,set_mask,true);
      //*ptr->op.diag = diag;
      if(ch&OTD_DIAG_FULL_MASK)
         ret|=OTD_PROTO_PART_PERSONAL_DIAG;
      if(ch&OTD_DIAG_MASK)
        ret |= OTD_PROTO_PART_DIAG;

    }
    return ret;
  }
 #pragma warn .8057




  DWORD  __fastcall otd_dblow::handle_sb(otd_proto & op,LPDWORD lo_obj,LPDWORD hi_obj)
  {
       /*Обработка одного объекта*/
       DWORD ret = 0;
    __try{
       proto_pointer ptr = find(*op.addr);
       if(!otd_list.size() || ptr>=otd_list.end() )
       {
        //не нашли объект
        if(op.data && op.data->numbers.loN<=op.data->numbers.hiN)
        {
        //Добавляем в случае есть данные
        DWORD offset =  create(op);
        ptr = otd_list.begin();
        advance(ptr,offset);
        if(ptr<otd_list.end())
        ret |= ((*op.ver_parts)&OTD_PROTO_PARTSMASK);
        #ifdef _DEBUG
        else
        DebugBreak();
        #endif
        }
        else
        return ret;
       }

       if(otd_list.size() && ptr<otd_list.end())
        {

         if(ptr->op.dw_size == sizeof(otd_proto))
         {
          DWORD ln = -1,hn = 0;
          BOOL  need_recreate = FALSE;
          if( op.data )
          {
           ln =(DWORD) KeRTL::MIN (op.data->numbers.loN, ptr->op.data->numbers.loN);
           hn =(DWORD) KeRTL::MAX (op.data->numbers.hiN , ptr->op.data->numbers.hiN);
          }
          if( op.personal_diag )
          {
           ln =(DWORD) KeRTL::MIN (op.personal_diag->numbers.loN, ptr->op.data->numbers.loN);
           hn =(DWORD) KeRTL::MAX (op.personal_diag->numbers.hiN , ptr->op.data->numbers.hiN);
          }
          if( op.personal_chmask )
          {
           ln =(DWORD) KeRTL::MIN (op.personal_chmask->numbers.loN , ptr->op.data->numbers.loN);
           hn =(DWORD) KeRTL::MAX (op.personal_chmask->numbers.hiN , ptr->op.data->numbers.hiN);
          }

          need_recreate = (ln<(DWORD)ptr->op.data->numbers.loN || hn>(DWORD)ptr->op.data->numbers.hiN) ? TRUE:FALSE;

          if(need_recreate &&  is_dynamic(ptr))
           {
           //Необходимо пересоздать

           #ifdef _DEBUG
           //DebugBreak();
           #endif
           this->recreate(ptr,ln,hn,NULL);


           }
          //Время берем или из кадра или берем текущее
          __int64 chtime = op.time_stamp ? (*(__int64*)(op.time_stamp)) : 0;
          if(0 == chtime) chtime = GetTime();
           DWORD diag = op.diag ? *op.diag:0;
           sotd_addr cp_addr( get_owner_addr(*op.addr) );
           sotd_addr pu_addr( get_owner_addr(cp_addr));
           //1) Найти ПУ
           if((diag&OTD_DIAG_PUMASK)==0)
           {
            proto_pointer pu = find(pu_addr);

            if(pu< end())
            {
             if(set_pu_diag(pu,OTD_DIAG_MASK,false))
               item_changed(pu,OTD_PROTO_PART_DIAG,0,-1);
            }

            if((diag&OTD_DIAG_CPMASK)==0)
            {
              proto_pointer cp = find(cp_addr);
              if(cp<otd_list.end())
              {
               if(set_cp_diag(cp,OTD_DIAG_MASK,false))
                 item_changed(cp,OTD_PROTO_PART_DIAG,0,-1);
              }
            }

           }
           ret = handle_sb(ptr,op,chtime,lo_obj, hi_obj);
         }
        }
      }
      __except(EXCEPTION_EXECUTE_HANDLER)
      {
       ret = 0;
      }

    return ret;
  }



  DWORD __fastcall otd_dblow::handle_sb(proto_pointer ptr,otd_proto & src,__int64 chtime,LPDWORD lo_obj,LPDWORD hi_obj)
  {
   otd_proto & dest = ptr->op;
   DWORD changes = 0;
   DWORD diag = *ptr->op.diag;
   DWORD lo_ch = -1;
   DWORD hi_ch = 0;
   if(src.diag)
   diag    = *src.diag;
   else
   diag   = 0;
   DWORD loN = dest.data->numbers.loN,hiN = dest.data->numbers.hiN;

    if(src.data)
       {
        loN = KeRTL::MAX((DWORD)src.data->numbers.loN,loN);
        hiN = KeRTL::MIN((DWORD)src.data->numbers.hiN,hiN);
       }
     else
    if(src.personal_diag)
      {
        loN = KeRTL::MAX((DWORD)src.personal_diag->numbers.loN,loN);
        hiN = KeRTL::MIN((DWORD)src.personal_diag->numbers.hiN,hiN);
      }
      
    otd_clear(ptr->op.personal_chmask);//Очистить маску изменений
    sotd_addr own_addr (*src.addr);
    own_addr = own_addr.owner_addr();
    own_addr= own_addr.owner_addr();


   //if(!(diag&OTD_DIAG_MASK))
   {
    bool obj_changes = false;
    while(loN<=hiN)
    {
     QWORD old_val  = 0,new_val = 0;
     DWORD old_pd   = 0,new_pd = 0;
     if( otd_get_value(dest.data         ,loN,&old_val,sizeof(old_val)))
        continue;
        otd_get_value(dest.personal_diag,loN,&old_pd ,sizeof(old_pd ));
     if(otd_get_value(src.data          ,loN,&new_val,sizeof(new_val)))
        new_val = old_val;



     if(otd_get_value(src.personal_diag ,loN,&new_pd ,sizeof(new_pd)))
        new_pd  = (old_pd&~OTD_PDIAG_NODATA);
     if(new_val!=old_val)
        {
         otd_set_value(dest.data,loN,&new_val,sizeof(new_val));
         otd_inc_value(dest.personal_chmask,loN,TRUE);
         changes|=OTD_PROTO_PART_DATA|OTD_PROTO_PART_PERSONAL_CHMASK;
         obj_changes = true;
        }

        
        
     if(new_pd !=old_pd )
        {
         obj_changes = true;
         if(((*dest.diag^new_pd)&OTD_STATE_MASK))
             {
              changes|=OTD_PROTO_PART_DIAG;
             }
         if((new_pd^old_pd) & OTD_PDIAG_MASK )
         {
            changes|=OTD_PROTO_PART_DIAG;
            if(0==(new_pd&OTD_PDIAG_MASK))
             changes|=OTD_PROTO_PART_DATA; //При восстановлении готовности восстанавливаются и данные
         }

         otd_set_value(dest.personal_diag,loN,&new_pd ,sizeof(new_pd ));
         otd_inc_value(dest.personal_chmask,loN,TRUE);
         otd_inc_value(dest.personal_chmask,loN,TRUE);
         changes|=OTD_PROTO_PART_PERSONAL_DIAG|OTD_PROTO_PART_PERSONAL_CHMASK;
        }
        else
         {
          if(old_pd && obj_changes)
            {
             changes     |=OTD_PROTO_PART_PERSONAL_DIAG ;
            }
         }


        if(obj_changes)
        {
         if(lo_ch == DWORD(-1))
            lo_ch = loN;
         hi_ch = loN;
         obj_changes = false;
        }
     ++loN;
    }

   }
   //else
   if((diag&OTD_DIAG_MASK) && dest.personal_diag)
   {
    //Данные недостоверны
    loN = dest.personal_diag->numbers.loN;
    hiN = dest.personal_diag->numbers.hiN;
    while(loN<=hiN)
    {
     DWORD pd = 0;
     otd_get_value(dest.personal_diag,loN,&pd,sizeof(pd));
     if(!(pd&OTD_PDIAG_NODATA)) //Установить всю персональную диагностику НЕТ ДАННЫХ
     {
      pd|=OTD_PDIAG_NODATA;
      otd_set_value(dest.personal_diag,loN,&pd,sizeof(pd));
      otd_inc_value(dest.personal_chmask,loN,TRUE);
      otd_inc_value(dest.personal_chmask,loN,TRUE);
      diag|=OTD_PART_DIAG_PARAM;
      changes|=OTD_PROTO_PART_DIAG|OTD_PROTO_PART_PERSONAL_DIAG|OTD_PROTO_PART_PERSONAL_CHMASK;
      if(lo_ch == DWORD(-1))
         lo_ch = loN;
       else
         lo_ch = std::min(lo_ch,loN);
       hi_ch = std::max(hi_ch,loN);
     }
     ++loN;
    }
   }

    if(lo_obj) *lo_obj = lo_ch;
    if(hi_obj) *hi_obj = hi_ch;

    if((*dest.diag^diag)&OTD_DIAG_MASK)
         {
          changes|=OTD_PROTO_PART_DIAG;
          if(0==(diag&OTD_DIAG_MASK))
           changes|=OTD_PROTO_PART_DATA;
         }


    if( changes&(OTD_PROTO_PART_DIAG|OTD_PROTO_PART_PERSONAL_DIAG))
    {
     DWORD ch_diag = (*dest.diag)^diag;
     if(dest.diag && (ch_diag || (changes&OTD_PROTO_PART_PERSONAL_DIAG)))
     {
     *dest.diag = diag;
     *dest.diag = otd_scan_personal_diag(&dest);
     }
     /*if(ch_diag&OTD_DIAG_CPMASK && !(diag&OTD_DIAG_CPMASK))
         set_diag(own_addr,((~diag)&(OTD_DIAG_FULL_MASK))|OTD_DIAG_NODATA|OTD_PART_DIAG_PARAM,false);*/
       update_diag(own_addr);
    }



     if(changes && dest.time_stamp)
       {
        (*(__int64*)dest.time_stamp) = chtime ? chtime:GetTime();
         changes|=OTD_PROTO_PART_TIMESTAMP;
         //TODO - обновить персональную диагностику
         //if(changes &(OTD_PROTO_PART_DIAG|OTD_PROTO_PART_PERSONAL_DIAG))
          // changes|=update_diag(dest);
       }

     if(src.name && dest.name && otd_text_compare(dest.name,src.name,TRUE))
       {
        otd_text_copy(dest.name,src.name);
        changes|=OTD_PROTO_PART_NAME;
       }

    return changes;
  }








