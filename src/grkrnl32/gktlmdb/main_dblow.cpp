#pragma hdrstop
#include "gktlmdb_mod.hpp"
#include <stdio.h>

//DWORD  __fastcall main_dblow::get_punumber (modem_addr & ma)
//{
// DWORD ret;
// rd_lock();
//  ret = modem_pu.get_pu(ma,0);
// rd_unlock();
// return ret;
//}

otd_addr  __fastcall main_dblow::get_otd_addr (modem_addr & ma)
{
 otd_addr ret;
 rd_lock();
  ret.addr = modem_pu.get_otd(ma).addr;
 rd_unlock();
 return ret;
}

DWORD   __fastcall main_dblow::modem_connect(modem_addr & ma,bool connect,LPMODEM_PROPS mp)
{
  /*Линия модема сменила состояние */
  DWORD ret = -1;
  TGKTlmDB * save_mod = module;
    wchar_t  rep_text[MAX_PATH<<1];
    wchar_t  rep_templ[MAX_PATH];
  if(connect)
   {
     if(mp && mp->flags&MPF_DATA_SOURCE)
     {

      module->get_lang_string(IDS_LOG_MODEM_CONNECTSRC,rep_templ,sizeof(rep_templ)/sizeof(wchar_t));
      wsprintfW(rep_text,rep_templ,(DWORD)ma.modem,(DWORD)ma.line);
      module->db_report(REPORT_INFORMATION_TYPE,rep_text,0,0);
      module->refresh_pu(ma.addr,-1);
     }
   }
   else
   {
    wr_lock();
    modem_range_t range = modem_pu.get_modem_range(ma);
    while (range.first != range.second)
    {
        otd_addr addr = range.first->second;
        if(addr.addr!=(DWORD)0)
        {
        set_diag(addr,OTD_DIAG_PUCONNECT,true);
        DBLOW_CHANGES_STRUCT dcs;
        dcs.parts = OTD_PROTO_PART_DIAG|OTD_PROTO_PART_PERSONAL_DIAG;
        dcs.loN   =  0;
        dcs.hiN   = -1;
        dcs.addr  = addr.addr;
        module->notify(TLM_DBLOW_NOTIFY_CHANGE,TLM_DBLOW_CODE_CHANGED,&dcs,sizeof(dcs));
        module->send_otd_changes(addr,OTD_PROTO_PART_DIAG,0,-1);
        module->get_lang_string(IDS_LOG_MODEM_DISCONNECTSRC,rep_templ,sizeof(rep_templ)/sizeof(wchar_t));
        wsprintfW(rep_text,rep_templ,(DWORD)ma.modem,(DWORD)ma.line,ret);
        module->db_report(REPORT_WARNING_TYPE,rep_text,0,0);
        }
        ++range.first;
    };
    modem_pu.erase(ma);
    wr_unlock();
   }
   if(module!=save_mod)
      DebugBreak();
   return ret;
}

  DWORD  __fastcall main_dblow::handle(otd_proto & op, modem_addr & from,LPDWORD lo,LPDWORD hi)
  {
    DWORD ret = 0;
    DWORD loN = 0;
    DWORD hiN = -1;
    if(wr_lock(INFINITE))
    {
     modem_addr ma = modem_pu.get_modem(*op.addr);
     if(ma.addr == (DWORD)-1)
        {
         MODEM_PROPS mp={0};
         mp.dw_size = sizeof(mp);
         module->get_modem_props(from,&mp,sizeof(mp));
         if(mp.flags&MPF_DATA_SOURCE)
         {
          ma.addr = from.addr;
          modem_pu.set(from,*op.addr);
         }
        }

   if((ma.sta == from.sta && ma.modem == from.modem))
     {
         ret = otd_dblow::handle(op,from,&loN,&hiN);
     }
     else
     {
      if( OTD_FA_ISCTRL(op.addr->fa))
       ret = handle_ctrl(op,from,&loN,&hiN);
      else
      {
      if(OTD_FA_ISSETUP(op.addr->fa))
       {
         ret = GKH_RET_SUCCESS;
       }
        else
       {
          //TODO
          //Конфликт в номерах ПУ. с разных модемов идут данный с одинаковыми номерами ПУ
          //Записать в ЛОГ
           if(!modem_pu.get_conflict(op.addr->pu))
           {
            report_conflict(op.addr->pu,ma.modem,from.modem);
            modem_pu.set_conflict(op.addr->pu,TRUE);
           }
       }
      }
     }
    }
    wr_unlock();
    if(lo) *lo = loN;
    if(hi) *hi = hiN;
    return ret;
  }

#pragma warn -8057

void   __fastcall main_dblow::handle_query (modem_addr & query_respond,otd_addr &  query_mask,DWORD parts ,DWORD lo ,DWORD hi )
{

}

#pragma warn .8057

DWORD  __fastcall main_dblow::get_index_range(otd_addr & addr,DWORD & lo,DWORD & hi)
{
 DWORD ret;
 rd_lock();
 proto_pointer  lo_ptr = otd_list.begin(),hi_ptr = otd_list.end();
 ret = get_range(addr,lo_ptr,hi_ptr);
 if(ret)
  {
    lo = distance(otd_list.begin(),lo_ptr);
    hi = distance(otd_list.begin(),hi_ptr);
  }
  else
  lo = hi = -1;
 rd_unlock();
 return ret;
}

void       __fastcall main_dblow::send_undo(DWORD addr,DWORD obj)
{
  BYTE buffer[MAX_PATH];
  LPMPROTO_HEADER mph = (LPMPROTO_HEADER)buffer;
  sotd_addr sa(addr);
  ZeroMemory(mph,sizeof(*mph));
  mph->addr_to.addr = modem_pu.get_modem(sa).addr;
  if(mph->addr_to.addr!=(DWORD)-1)
    {
     otd_proto op;
     ZeroMemory(&op,sizeof(op));
     op.dw_size = sizeof(op);
     mph->fa    = FA_OTD;
     mph->data_size = otd_proto_format_tutr((LPBYTE)mph->data,sizeof(buffer)-sizeof(*mph),&sa,obj,OTD_TUTROP_CANCEL,0,sizeof(DWORD),0,&op);
     sotd_proto sop((LPBYTE)mph->data,mph->data_size);
     sop.op.dw_size = sop.op.dw_size;
     module->send(mph);
    }

}

void       __fastcall main_dblow::send_tu(DWORD addr,DWORD obj,BOOL on)
{
  BYTE buffer[MAX_PATH];
  LPMPROTO_HEADER mph = (LPMPROTO_HEADER)buffer;
  sotd_addr sa(addr);
  ZeroMemory(mph,sizeof(*mph));
  mph->addr_to.addr = modem_pu.get_modem(sa).addr;
  if(mph->addr_to.addr!=(DWORD)-1)
    {
     otd_proto op;
     ZeroMemory(&op,sizeof(op));
     op.dw_size = sizeof(op);
     mph->fa    = FA_OTD;
     mph->data_size = otd_proto_format_tutr((LPBYTE)mph->data,sizeof(buffer)-sizeof(*mph),&sa,obj,on ? OTD_TUOP_ON:OTD_TUOP_OFF,0,sizeof(DWORD),0,&op);
     sotd_proto sop((LPBYTE)mph->data,mph->data_size);
     sop.op.dw_size = sop.op.dw_size;
     if(module)
        module->send(mph);
    }
}

DWORD      __fastcall main_dblow::remove_invalid_dynamic(DWORD addr)
{
 DWORD ret   = 0;
 DWORD error = 0;
 sotd_addr sa(addr);
 DBLOW_CHANGES_STRUCT dcs;
 dcs.parts = -1;
 dcs.loN = 0;
 dcs.hiN = -1;

 wr_lock();
 proto_pointer beg = otd_list.begin(),end = otd_list.end();
 if(get_range(sa,beg,end))
 {
  while(beg<end)
  {
   if(sa.is_include(*beg->op.addr,true) && (*(beg->op.diag )&OTD_DIAG_MASK) && is_dynamic(beg))
     {
      sotd_addr sa(get_owner_addr(*beg->op.addr));
      dcs.addr = beg->op.addr->addr;
      module->notify(TLM_DBLOW_NOTIFY_CHANGE,TLM_DBLOW_CODE_REMOVED,&dcs,sizeof(dcs));
      delete_sop(beg);
      otd_list.erase(beg);
      --end;
      update_diag(sa);
      ret++;
     }
     else
     ++beg;
  }
 }

 if(ret )
    {
      dcs.addr  = sa.addr;
      dcs.parts = OTD_PROTO_PART_DIAG;
      module->notify(TLM_DBLOW_NOTIFY_CHANGE,TLM_DBLOW_CODE_CHANGED,&dcs,sizeof(dcs));
    }
 wr_unlock();
 if(error)    SetLastError(error);
 return  ret;
}

 proto_pointer   __fastcall main_dblow::create      (otd_addr & addr,otd_proto_param & src_opp)
 {
   wr_lock();
   proto_pointer ret = otd_dblow::create(addr,src_opp);
   if(ret < otd_list.end())
     {
      DBLOW_CHANGES_STRUCT dcs;
      dcs.addr  = addr.addr;
      dcs.parts = -1;
      dcs.loN   = 0;
      dcs.hiN   = -1;
      module->notify(TLM_DBLOW_NOTIFY_CHANGE,TLM_DBLOW_CODE_ADDED,&dcs,sizeof(dcs));
     }
   wr_unlock();  
   return ret;
 }

 proto_pointer   __fastcall main_dblow::create_group(otd_addr & addr)
 {
   wr_lock();
   proto_pointer ret = otd_dblow::create_group(addr);
   if(ret < otd_list.end())
     {
      DBLOW_CHANGES_STRUCT dcs;
      dcs.addr  = addr.addr;
      dcs.parts = -1;
      dcs.loN   = 0;
      dcs.hiN   = -1;
      module->notify(TLM_DBLOW_NOTIFY_CHANGE,TLM_DBLOW_CODE_ADDED,&dcs,sizeof(dcs));
     }
   wr_unlock();
   return ret;
 }


DWORD  __fastcall main_dblow::handle_ctrl (otd_proto & op,modem_addr & from,LPDWORD lo_ch,LPDWORD hi_ch )
{
 /*
   Обработчик управления
   Команда выбора обрабатывается на месте
 */
 DWORD ret = 0;
 BYTE buffer    [8192];
 LPMPROTO_HEADER mph = (LPMPROTO_HEADER)buffer;
 ZeroMemory(mph,sizeof(*mph));
 DWORD _lo_ch = -1,_hi_ch = 0;
  if(otd_addr_type(op.addr) == OTD_ADDR_TYPE_SB && op.data)
  {
    modem_addr ma(modem_pu.get_modem(*op.addr));
    if(ma.addr!=DWORD(-1))
    {
     wchar_t tutr_log_text[MAX_PATH];
     //DWORD   tutr_log_len =
     module->get_modem_line_text(from,tutr_log_text,sizeof(tutr_log_text)/sizeof(wchar_t));
     proto_pointer ptr = find( *op.addr );
     if(!(ptr>=this->end()) && ptr->op.data)
     {
       mph->fa      = FA_OTD;
       DWORD tu_obj_beg = op.data->numbers.loN;
       DWORD tu_obj_end = op.data->numbers.hiN;
       while(tu_obj_beg<=tu_obj_end)
       {
       otd_tutr tutr ;
       if(otd_data_inrange(ptr->op.data,op.data->numbers.loN) && otd_data_inrange(ptr->op.data,op.data->numbers.hiN ))
       {
         DWORD tuobj_pd = 0;


         otd_get_value(op.data,tu_obj_beg,&tutr,sizeof(tutr));
         otd_get_value(ptr->op.personal_diag,tu_obj_beg,&tuobj_pd,sizeof(tuobj_pd));
         if(tutr.command == OTD_TUTR_CMDSELECT)
           {
            if(0==(tuobj_pd&(OTD_PSTATE_TUTR_MASK|OTD_PDIAG_TUTR_DESCRIPT)))
             {//Отправить команду разрешения НА ТУ/ТР
               mph->addr_to.addr      = from.addr;
               mph->addr_from.addr    = 0;
               mph->data_size = otd_proto_format_tutr((LPBYTE)mph->data,sizeof(buffer)-sizeof(*mph),
                                                      op.addr,tu_obj_beg,OTD_TUTR_CMDENABLE,
                                                      &tutr.param,sizeof(tutr.param),0,NULL);
               module->send(mph);
//               tuobj_pd|=OTD_PSTATE_TUTR_PREPARE;
//               otd_set_value(ptr->op.personal_diag,tu_obj_beg,&tuobj_pd,sizeof(tuobj_pd));
//               *ptr->op.diag|=OTD_PSTATE_TUTR_PREPARE;
//                ret|=OTD_PROTO_PART_DIAG|OTD_PROTO_PART_PERSONAL_DIAG;
//                _hi_ch = KeRTL::MAX(_hi_ch,tu_obj_beg);
//                _lo_ch = KeRTL::MIN(_lo_ch,tu_obj_beg);

             }
           }

           {
            //Ретранслировать команду соответсвубщему модему

            mph->addr_to.addr      = ma.addr;
            mph->addr_from.addr    = from.addr;
            mph->data_size = otd_proto_format_tutr((LPBYTE)mph->data,sizeof(buffer)-sizeof(*mph),
                                                   op.addr,tu_obj_beg,tutr.command,&tutr.param,
                                                   sizeof(tutr.param),0,NULL);
           }

       }
       if(mph->data_size)
         {
          module->send(mph);
          report_tutr_sended(*ptr->op.addr,tu_obj_beg,tutr,from);
        }
         mph->data_size = 0;
       ++tu_obj_beg;
      }
     }
    }
  }
  if(lo_ch && _lo_ch!=(DWORD)-1)
     *lo_ch = _lo_ch;
  if(hi_ch && _hi_ch)
     *hi_ch = _hi_ch;
  return  ret;
}

DWORD   __fastcall main_dblow::item_changed  (proto_pointer  ptr,DWORD ch_parts,DWORD lo_obj,DWORD hi_obj)
{
 otd_dblow::item_changed(ptr,ch_parts,lo_obj,hi_obj);
 if(module)
   {
     DBLOW_CHANGES_STRUCT dcs;
     dcs.parts = ch_parts;
     dcs.loN = lo_obj;dcs.hiN = hi_obj;

     if(ptr<end())
     dcs.addr  = ptr->op.addr->addr;
     else
     dcs.addr  = -1;

     module->send_otd_changes(*&sotd_addr(dcs.addr),ch_parts,lo_obj,hi_obj);
     module->notify(TLM_DBLOW_NOTIFY_CHANGE,TLM_DBLOW_CODE_CHANGED,&dcs,sizeof(dcs));
   }

 if((ch_parts&OTD_PROTO_PART_DATA) && ptr<end() && ptr->op.addr->sb!=OTD_ADDR_MAXVALUE &&   ((*ptr->op.diag)&OTD_PSTATE_TUTR_ACTIVE) && hi_obj!=(DWORD)-1 )
  {
    while(lo_obj<=hi_obj)
    {
      DWORD param = 0;
      DWORD pd    = 0;
      DWORD cm    = 0;
      otd_get_value(ptr->op.personal_diag,lo_obj,&pd,sizeof(pd));
      otd_get_value(ptr->op.personal_chmask,lo_obj,&cm,sizeof(pd));
      if((cm&1) && (pd&OTD_PSTATE_TUTR_ACTIVE))
      {
       otd_get_value(ptr->op.data,lo_obj,&param,sizeof(pd));
       report_tu_change(*ptr->op.addr,lo_obj,param);
      }
     ++lo_obj;
    }
  }
  return 0;
}



 DWORD      __fastcall main_dblow::get_name  (otd_addr & addr,wchar_t * text,DWORD bsz)
 {
   DWORD ret = 0;
   rd_lock();
   DWORD idx = find_byaddr(addr);
   if(idx!=(DWORD)-1)
    {
     proto_pointer ptr = otd_list.begin();
     advance(ptr,idx);
     if(ptr->op.name)
       ret= otd_text_getwstr(ptr->op.name,text,bsz);
    }
   rd_unlock();
   return  ret;
 }

 DWORD       __fastcall main_dblow::get_puname(DWORD pu,wchar_t * text,DWORD bsz)
 {
   sotd_addr sa(pu,-1,-1,-1);
   return    get_name(sa,text,bsz);
 }

 DWORD       __fastcall main_dblow::get_cpname(DWORD pu,DWORD cp,wchar_t * text,DWORD bsz)
 {
   sotd_addr sa(pu,cp,-1,-1);
   return get_name(sa,text,bsz);
 }

 void __fastcall main_dblow::report_tu_change(otd_addr & addr,DWORD obj,DWORD param)
 {
       wchar_t text[1024];
       wchar_t pu_name[MAX_PATH];
       wchar_t cp_name[MAX_PATH];
       pu_name[0] = cp_name[0] = 0;
       get_puname(addr.pu,pu_name,sizeof(pu_name)/sizeof(wchar_t));
       get_cpname(addr.pu,addr.cp,cp_name,sizeof(cp_name)/sizeof(wchar_t));
       swprintf(text,L"%03d.%03d.%03d.%03d %s %s в результате операции ТУ/ТР параметр %d принял значение  0x%X",
                           (DWORD)addr.pu,(DWORD)addr.cp,(DWORD)addr.fa,(DWORD)addr.sb,
                           pu_name,cp_name,
                           obj,param);
       module->report_dbtutr(REPORT_INFORMATION_TYPE,text);
 }

void __fastcall  main_dblow::report_conflict(DWORD pu, DWORD modem, DWORD conflict_modem)
{
        wchar_t rep_text[MAX_PATH];
        wchar_t pu_name [MAX_PATH];
        pu_name[0] = 0;
        get_puname(pu,pu_name,sizeof(pu_name)/sizeof(wchar_t));
        wsprintfW(rep_text,L"Конфликт: данные ПУ %s № %03d  приходят с модемов  %03d и %03d. С последнего адреса не обрабатываются ",
                  pu_name,pu,modem,conflict_modem);
        module->db_report(REPORT_WARNING_TYPE,rep_text);
}

void __fastcall main_dblow::report_tutr_sended(otd_addr & addr,DWORD obj , otd_tutr & tutr,modem_addr & src)
{
     wchar_t op_name[32];
     wchar_t PUname[MAX_PATH];
     wchar_t CPname[MAX_PATH];
     wchar_t modem_text[MAX_PATH];
     *PUname = *CPname = *op_name = *modem_text = 0;

     switch(tutr.command)
     {
      case OTD_TUOP_ON      : lstrcpyW(op_name,L"ВКЛ/+");break;
      case OTD_TUOP_OFF     : lstrcpyW(op_name,L"ОТКЛ/-");break;
      case OTD_TUTROP_CANCEL: lstrcpyW(op_name,L"ОТМЕНА");break;
     }
     if(*op_name)
     {
      wchar_t rep_text[2048];
      get_puname(addr.pu,PUname,MAX_PATH);
      get_cpname(addr.pu,addr.cp,CPname,MAX_PATH);
      DWORD mtl = module->get_modem_line_text(src,modem_text,sizeof(modem_text)/sizeof(wchar_t));
      swprintf(modem_text+mtl,L" %03d.%03d",(DWORD)src.modem,(DWORD)src.line);
      swprintf(rep_text,L"%03d.%03d.%03d.%03d %s %s объект № %03d команда %s отправлена. Источник %s",
               (DWORD)addr.pu,(DWORD)addr.cp,(DWORD)addr.fa,(DWORD)addr.sb,
               PUname,CPname,obj,op_name,modem_text);

      module->report_dbtutr(REPORT_SUCCESS_TYPE,rep_text);
     }
}

DWORD      __fastcall main_dblow::recreate  (otd_addr & addr,LPTLMDB_LOW_RECREATE rcs)
{
 DWORD ret = GKH_RET_ERROR;
 wr_lock();
 DWORD idx= this->find_byaddr(addr);
 if(idx!=(DWORD)-1)
 {
  proto_pointer ptr = this->begin();
  advance(ptr,idx);
  ret = otd_dblow::recreate(ptr,rcs->lo_obj,rcs->hi_obj,rcs->name);
  if(ret)
     module->config_change(CCFL_DB);
 }
 wr_unlock();
 return ret;
}

DWORD      __fastcall main_dblow::make_static(otd_addr & addr,BOOL st)
{
 DWORD ret = GKH_RET_ERROR;
 wr_lock();
 DWORD idx= this->find_byaddr(addr);
 if(idx!=(DWORD)-1)
 {
  if(st)
  {
   proto_pointer ptr = this->begin();
   advance(ptr,idx);
   ret = otd_dblow::make_static(ptr,TRUE);
  }
  else
  {
   proto_pointer beg_ptr = this->begin(),end_ptr = this->end();
   get_range(addr,beg_ptr,end_ptr);
   while(beg_ptr<end_ptr)
      ret|=otd_dblow::make_static(beg_ptr++,FALSE);


  }
  if(ret)
     module->config_change(CCFL_DB);
 }
 wr_unlock();
 return ret;
}








