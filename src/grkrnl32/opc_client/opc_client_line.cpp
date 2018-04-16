#pragma hdrstop
#include "gkopc_client.hpp"
#include <math.h>
#include <otd_diag.h>


extern "C" void tss_cleanup_implemented(void)
{
 //stub only for boost thread
}


DWORD __fastcall opc_line::get_line_text  (wchar_t * text,DWORD text_sz)
{
 DWORD ret = GKH_RET_ERROR;
 //TLockHelper l(locker);
 snwprintf(text,text_sz,L"Линия №-%d %c %s",this->line_config.line_num,line_config.cp_name[0] ? ':':' ',line_config.cp_name);
 return ret;
}

void __fastcall opc_line::get_line_config(LPGKOPC_LINE_CONFIG _line_config)
{
  TLockHelper l(locker);
  memcpy(_line_config,&line_config,sizeof(*_line_config));
}

bool __fastcall opc_line::is_need_restart(const GKOPC_LINE_CONFIG & ln_conf)
{
   TLockHelper l(locker);
   bool need_restart = (is_worked() &&
                          (ln_conf.line_num != line_config.line_num ||
                          wcsncmp(ln_conf.server_host,line_config.server_host,KERTL_ARRAY_COUNT(ln_conf.server_host)) ||
                          wcsncmp(ln_conf.server_prog_id,line_config.server_prog_id,KERTL_ARRAY_COUNT(ln_conf.server_prog_id))
                          )
                       ) ? true:false;
    return need_restart ;
   }

int  __fastcall opc_line::load_items_from_file()
{
  //TLockHelper l(data_locker);
  TLockHelper l(locker);
  opc_items.clear();
  gkopc_items_io io;
  KeRTL::TFileStream fs(line_config.config_file,true);
  if(io.read(opc_items,&fs))
  {
    gkopc_items_t::iterator ptr = opc_items.begin();
    gkopc_items_t::iterator end = opc_items.end  ();
    vector<std::wstring> tu_tags;
    while(ptr<end)
    {
     if(!ptr->rc_tag.empty())
         tu_tags.push_back(ptr->rc_tag);
     ++ptr;
    }

    vector<std::wstring>::iterator tags_beg = tu_tags.begin();
    vector<std::wstring>::iterator tags_end = tu_tags.end();
    while(tags_beg<tags_end)
    {
      gkopc_item item(*tags_beg);
      if(!opc_items.find(item,ptr))
           opc_items.insert(item);
      ++tags_beg;
    }
  }
  return opc_items.size();
}

DWORD __fastcall  opc_line::__get_range_otd_type(gkopc_items_t::index_iterator  iptr,gkopc_items_t::index_iterator  iend)
{

   DWORD type = OTD_BIT_TYPE;
   DWORD sz   = 1;
   bool  is_signed = false;
   while(iptr<iend)
   {
     gkopc_item & item = opc_items.at(*iptr);
     if(!item.item_state.vDataValue.vt)
         item.item_state.vDataValue.vt = item.reg_result.vtCanonicalDataType;


     DWORD _sz   = 1;
     bool  _is_signed = false;
     if( item.item_state.is_value_integer(&_sz,&_is_signed))
       {
         if(type != OTD_FLOAT_TYPE )
            {
             if( !item.item_state.is_value_bool())
             {
              type      = OTD_BYTE_TYPE;
              sz        = std::max(sz,_sz);
              is_signed = std::max(is_signed,_is_signed);
             }
            }
       }
       else
       {

          item.item_state.is_value_float(&_sz);
          type      = OTD_FLOAT_TYPE;
          is_signed = true;
          sz        = std::max(sz,_sz);
        }

     ++iptr;
   }
   DWORD ret = (type|sz);
   if(is_signed) ret|=OTD_SIGNED_TYPE;
   return ret;

}


int  __fastcall opc_line::build_otd_storage   ()
{
   //TLockHelper l(data_locker);
   TLockHelper l(locker);
   this->storage.clear();
   gkopc_item  lo_item;
   gkopc_item  hi_item;
   opc_items.build_index();
   gkopc_items_t::index_iterator iptr =  opc_items.index_begin();
   gkopc_items_t::index_iterator iend =  opc_items.index_end();
   while(iptr<iend)
   {
     lo_item.group_param = opc_items.at(*iptr).group_param;
     hi_item.group_param = lo_item.group_param;
     hi_item.group_param.param = -1;
     if(opc_items.range(lo_item,hi_item,iptr,iend))
     {
      if(lo_item.group_param.group_data !=(DWORD)-1)
      {
       DWORD otd_type = __get_range_otd_type(iptr,iend);
       lo_item.group_param = opc_items.at(*iptr).group_param;
       --iend;
       hi_item.group_param = opc_items.at(*iend).group_param;
       ++iend;
       DWORD count = 1 + hi_item.group_param.param - lo_item.group_param.param;
       __create_otd_group(lo_item.group_param.fa,lo_item.group_param.sb,lo_item.group_param.param,count,otd_type);
      }
     }
     iptr = iend;
     iend = opc_items.index_end();
   }
   return storage.size();
}

DWORD __fastcall opc_line::__create_otd_group(DWORD otd_fa,DWORD otd_group,DWORD first_num,DWORD count,DWORD otd_type)
   {

     sotd_addr addr(0,0,otd_fa,otd_group);
     proto_pointer ptr;
//     TLockHelper l(data_locker);

     TLockHelper l(locker);
     if(storage.find(addr,ptr))
     {
       if(!ptr->op.data ||   count != (DWORD) OTD_GET_COUNT(&ptr->op.data->numbers))
         {
          LPBYTE buf = (LPBYTE)ptr->op.addr;
          storage.erase(ptr);
          delete [] buf;
          return __create_otd_group(otd_fa,otd_group,first_num,count,otd_type);
         }
     }
     else
     {
        otd_proto       op;
        op.dw_size   = sizeof(op);

        otd_proto_param opp;
        bzero(&opp,sizeof(opp));
        opp.dw_size = sizeof(opp);
        opp.parts   = OTD_PROTO_PART_KPK|OTD_PROTO_PART_DIAG|OTD_PROTO_PART_DATA|OTD_PROTO_PART_PERSONAL_DIAG|OTD_PROTO_PART_PERSONAL_CHMASK|OTD_PROTO_PART_TIMESTAMP;
        opp.dt_param.type   = otd_type;
        opp.dt_param.first  = first_num;
        opp.dt_param.count  = count;

        opp.pd_param = opp.dt_param;

        opp.pd_param.type = OTD_BYTE;
        opp.cm_param = opp.pd_param;

        DWORD  need_sz = otd_proto_calc_size_ex(&opp,0);
        LPBYTE buf = new BYTE[need_sz];
        if(!buf)
          {
            SetLastError(0/*NO_MEMORY*/);
            return GKH_RET_ERROR;
          }

         otd_proto_format_ex(buf,need_sz,&opp,0,&op);
         op.addr->addr = 0;
         op.addr->fa   = otd_fa;
         op.addr->sb   = otd_group;
         *op.diag      = OTD_DIAG_NODATA|OTD_PART_DIAG_PARAM;
         __int64 val = 0;
         otd_fill(op.data,&val,sizeof(val));
         otd_fill(op.personal_chmask,&val,sizeof(val));
         val = OTD_PDIAG_NODATA;
         otd_fill(op.personal_diag,&val,sizeof(val));
         __setup_group_values  (&op);
         ptr = storage.insert  (op);
         if(is_worked()) __queue_rxdata(&op);

     }
     return GKH_RET_SUCCESS;
   }

   void  __fastcall opc_line::__queue_rxdata(otd_proto * op)
   {
      op->addr->cp = line_config.line_num;
      if((line_config.options & GKOPC_LINE_OPT_REPLACE_TIME) && op->time_stamp) *op->time_stamp = GetTime(false);
      if(op->diag) *op->diag |= this->global_otd_diag;

      if(op->kpk)
         otd_proto_protect(op,op->proto_size,OTD_DEF_POLINOM);
      queue_rxdata(FA_OTD,(LPBYTE)op->addr,op->proto_size,false,0);
      op->addr->cp = 0;
      update_stats_rx(0,1,0);
   }


void __fastcall opc_line::__setup_group_values  (otd_proto * op)
{
   gkopc_item lo_item(op->addr->fa,op->addr->sb,0 );
   gkopc_item hi_item(op->addr->fa,op->addr->sb,-1);
   {
   TLockHelper l(locker);

       gkopc_items_t::index_iterator iptr = this->opc_items.index_begin();
       gkopc_items_t::index_iterator iend = this->opc_items.index_end();
       if(opc_items.range(lo_item,hi_item,iptr,iend))
       {
         while(iptr<iend)
         {
            gkopc_item & item = opc_items.at(*iptr);
            if(item.item_state.is_value_float() )
               {
                float value = (DWORD)item.item_state;
                otd_set_value(op->data,item.group_param.param,&value,sizeof(value));
               }
               else
               {
                DWORD value = item.item_state;
                otd_set_value(op->data,item.group_param.param,&value,sizeof(value));
               }
            if(op->personal_diag)
              {
                DWORD pd = item.get_personal_diag();
                otd_set_value(op->personal_diag,item.group_param.param,&pd,sizeof(pd));
              }
            ++iptr;
         }
       }
   }
   DWORD diag ;
   if(is_connected() )
   {
        diag = *op->diag;
        diag |= otd_scan_personal_diag(op);
   }
   else
   diag = OTD_DIAG_CPMASK;

   if(op->diag) *op->diag = diag;
}


bool __fastcall opc_line::set_line_config(LPGKOPC_LINE_CONFIG conf)
{
    bool need_restart = is_need_restart(*conf);
    if(get_number()!=conf->line_num)
     {
         if(owner )
         {
           if(!owner->renumber_line(get_number(),conf->line_num)) return false;
         }
         else
         set_line_number(conf->line_num);
     }
     memcpy(&line_config,conf,sizeof(*conf));

     if(need_restart)
        start();
      return true;
}

  void __fastcall opc_line::write(KeRTL::TStream & stream)
  {
     stream<<line_config.line_num
           <<line_config.server_type
           <<line_config.rate
           <<line_config.bandwidth

      ;
      stream.Write(&line_config.server_guid,sizeof(line_config.server_guid));
      stream.write_string(line_config.cp_name);
      stream.write_string(line_config.config_file);
      stream.write_string(line_config.server_host);
      stream.write_string(line_config.server_prog_id);
      stream<<line_config.options;
      stream.write_string(lua_scripts[0]);
      stream.write_string(lua_scripts[1]);
  }

  #pragma warn -8004
  void __fastcall opc_line::read (KeRTL::TStream & stream,DWORD stream_ver )
  {
     //if(stream_ver == OPC_MOD_STREAM_VER)
       {
         stream>>line_config.line_num
               >>line_config.server_type
               >>line_config.rate
               >>line_config.bandwidth
               ;
         stream.Read(&line_config.server_guid,sizeof(line_config.server_guid));
         wchar_t * str_ptr = line_config.cp_name;
         stream.read_string(str_ptr);
         str_ptr = line_config.config_file;
         stream.read_string(str_ptr);

         if(stream_ver > OPC_MOD_STREAM_VER_1)
         {
          str_ptr = line_config.server_host;
          stream.read_string(str_ptr);

          str_ptr = line_config.server_prog_id;
          stream.read_string(str_ptr);
          if(stream_ver > OPC_MOD_STREAM_VER_2)
             {
              stream>>line_config.options;
               if(stream_ver > OPC_MOD_STREAM_VER_3)
               {
                 stream.read_string(lua_scripts[0]);
                 stream.read_string(lua_scripts[1]);
               }
             }
         }
         this->set_line_number(line_config.line_num);
       }
  }

  #pragma warn -8004

  bool  __fastcall opc_line::send        (LPMPROTO_HEADER mph,DWORD sz)
  {
	if(mph->fa == FA_OTD)
	{
	      sotd_proto sop;
	      sop.assign((LPBYTE)mph->data,mph->data_size);
          if(OTD_ADDR_ISQUERY(sop.op.addr))
           __handle_otd_query(sop.op.addr);
          else
          {
           if( get_number() == DWORD(sop.op.addr->cp) && OTD_FA_ISCTRL(sop.op.addr->fa) )
               __handle_otd_control(&sop.op);
          }
        }
     return true;
  }

void __fastcall   opc_line::__handle_otd_query( lpotd_addr paddr)
{
      if(((DWORD)paddr->cp == get_number()) || ((DWORD)paddr->cp == OTD_ADDR_MAXVALUE))
      {
          __do_recv_cp_data(true);
          TLockHelper l(locker);
          otd_addr addr;
          addr.addr = paddr->addr;
          addr.cp = addr.pu = 0;
          proto_pointer pptr = storage.begin();
          proto_pointer pend = storage.end  ();
          if(storage.get_range(addr,pptr,pend))
          {
              while(pptr<pend)
              {
                __queue_rxdata(&pptr->op);
                ++pptr;
              }
          }
      }
}

wchar_t * get_rc_command_text(DWORD command)
{
 switch(command)
 {
   case  OTD_TUOP_ON :      return L"ВКЛ-Больше";
   case  OTD_TUOP_OFF:      return L"ОТКЛ-Меньше";
   case  OTD_TROP_SET:      return L"ТР-SET";
   case  OTD_TUTROP_CANCEL: return L"Отмена ТУ-ТР";
 }
 return L"???";
}

void __fastcall   opc_line::__handle_otd_control(lpotd_proto op)
{
  wchar_t report_text[MAX_PATH];
  report_text[0] = 0;
  DWORD rep_type = 0;
  if(get_number() == DWORD(op->addr->cp) && OTD_FA_ISCTRL(op->addr->fa))
  {

      DWORD otd_fa = (op->addr->fa&OTD_FA_DATA_MASK);
      DWORD object = op->data->numbers.loN;
      gkopc_item item(otd_fa,op->addr->sb,object);
      string    script;
      gkopc_items_t::index_iterator iptr;
      {
          TLockHelper l(locker);

          if(rc_queue.capacity()<32)  rc_queue.reserve(32);

          if(opc_items.find(item,iptr))
          {

           item = opc_items.at(*iptr);

           if(op->data->otd_type == OTD_TR_COMMAND_SET)
              otd_get_value(op->data,object,&item.rc_current,sizeof(item.rc_current));
              else
              otd_get_value(op->data,object,&item.rc_current.command,sizeof(item.rc_current.command));

           if(item.rc_current.command != OTD_TUTROP_CANCEL)
           {
             if(item.rc_object != DWORD(-1) || item.rc_tag.length() || item.rc_script.length() )
             {
               if(!item.rc_script.length())
                   item.rc_script = lua_scripts[otd_fa];
                rc_queue.push_back(item);
                icp.PostStatus(ICP_KEY_REMOTE_CONTROL,rc_queue.size(),NULL);
                snwprintf(report_text,KERTL_ARRAY_COUNT(report_text),L"%s ТУ/ТР команда %d [%s] - поставлена в очередь",item.id.c_str(),item.rc_current.command,get_rc_command_text(item.rc_current.command));
                rep_type = REPORT_INFORMATION_TYPE;
             }
             else
             {
               //Установить ошибку отсутсвтие описателя ТУ/ТР
               __int64 tm = GetTime();
               DWORD pdiag = OTD_PDIAG_TUTR_DESCRIPT;
               __set_opc_item_values(opc_items.at(*iptr),NULL,NULL,&tm,&pdiag);
               snwprintf(report_text,KERTL_ARRAY_COUNT(report_text),L"%s ТУ/ТР команда %d [%s] - отсутствует описатель",item.id.c_str(),item.rc_current.command,get_rc_command_text(item.rc_current.command));
               rep_type = REPORT_ERROR_TYPE;
             }
           }
           else
           {
             //Команда отмены
             gkopc_items_t::iterator rc_ptr = rc_queue.begin();
             gkopc_items_t::iterator rc_end = rc_queue.end  ();
             int   num = 0;
             while(rc_ptr<rc_end)
             {
              if(rc_ptr->group_param.group_data == item.group_param.group_data)
                {
                 if(num)
                   {
                    rc_queue.erase(rc_ptr);
                    rc_end  = rc_queue.end();
                    snwprintf(report_text,KERTL_ARRAY_COUNT(report_text),L"%s  Отмена ТУ/ТР. Удалено из очереди",item.id.c_str());
                    rep_type = REPORT_INFORMATION_TYPE;
                   }
                   else
                   {
                    snwprintf(report_text,KERTL_ARRAY_COUNT(report_text),L"%s  Отмена выполняемой операции ТУ/ТР. выставлен запрос на отмену",item.id.c_str());
                    rep_type = REPORT_INFORMATION_TYPE;
                    rc_undo_request = TRUE;
                    ++rc_ptr;
                   }
                }
                else
                ++rc_ptr;
                ++num;
             }

           }
          }
      }

  }
 if(report_text[0])
 {
   TGkOpcModem * modem = dynamic_cast<TGkOpcModem*>(owner);
   if(modem)
   {
    wchar_t full_report[MAX_PATH<<1];
    snwprintf       (full_report,KERTL_ARRAY_COUNT(full_report),L"[%d.%d] %s",modem->get_modem_number(),get_number(),report_text);
    modem->do_report(rep_type,full_report,NULL,0);
   }
  }
}

 void    __fastcall  opc_line::__do_recv_cp_data(bool inc_name )
 {

      TLockHelper l(locker);
      otd_proto_param opp;
      otd_proto       op;
      op.dw_size      = sizeof(op);
      ZeroMemory(&opp,sizeof(opp));
      opp.dw_size     = sizeof(opp);
      opp.parts       = OTD_PROTO_PART_DIAG;
      if(inc_name)
      {
       opp.parts       |= OTD_PROTO_PART_NAME;
       opp.name_sz     = lstrlenW(this->line_config.cp_name)+1;
      }
      BYTE   buffer [1024];
      DWORD need_size = otd_proto_calc_size_ex(&opp,0);
      LPBYTE  buf;
      buf = (need_size>sizeof(buffer)) ?    new BYTE[need_size] :   buffer;
      if(buf)
      {
      	 otd_proto_format_ex(buf,need_size,&opp,0,&op);
      	 op.addr->addr = -1;
      	 op.addr->pu   = 0;
         op.addr->cp   = 0;
         *op.diag      = get_line_diag(false)|global_otd_diag;
      	 otd_text_set(op.name,line_config.cp_name);
      	 __queue_rxdata(&op);
      	 if(buf!=buffer) delete [] buf;
       }
  }

 DWORD    __fastcall  opc_line::get_line_diag(bool rescan)
 {
    DWORD ret = 0;
    if(is_connected())
       {
        TLockHelper l(locker);

          proto_pointer pptr = storage.begin();
          proto_pointer pend = storage.end();
          while(pptr<pend)
          {
           ++pptr;
          }
       }
       else
       ret = OTD_DIAG_CPMASK;
    return ret;
 }

void  __fastcall  opc_line::otd_set_line_diag(DWORD diag)
{
 /*Установка диагностики по линии*/
 /*Которая будет добавляться по ИЛИ ко всем исходящим данным*/
   if(global_otd_diag != diag)
   {
    global_otd_diag = diag;
    sotd_addr addr(-1);
       __handle_otd_query(&addr);
    if(!diag)   refresh(-1);
   }
}





DWORD __fastcall opc_line::get_max_frame_size()
{
 DWORD ret = -1;
 return ret;
}

DWORD __fastcall opc_line::get_tx_queue_size ()
{
 DWORD ret = 0;
 return ret;
}

bool  __fastcall opc_line::do_start ()
{
 bool  ret = true;
 if(!IsRunning())
   {
    global_quality_mask = -1;
    global_otd_diag     =  0;
    ret = Start();
   }
 return ret;
}

bool  __fastcall opc_line::do_stop  ()
{
 bool  ret = TerminateAndWait(10000,true);
 return ret;
}


bool __fastcall opc_line::BeforeExecute()
{
 bool  ret = false;
 com_last_result = 0;
 bzero(this->WT,sizeof(WT));
 if(TGKThread::BeforeExecute())
   {
    HRESULT res = CoInitializeEx(NULL,COINIT_MULTITHREADED);
    if(res == S_OK)
    {
     CoInitializeSecurity(NULL,-1,NULL,NULL,RPC_C_AUTHN_LEVEL_CONNECT,RPC_C_IMP_LEVEL_IDENTIFY,NULL,EOAC_NONE,NULL);
     if(opc_items.size())
        opc_items.clear();
        load_items_from_file();
      if(opc_items.size())
        do_connect();
      ret = true;
    }
   }
 return ret;
}

void __fastcall opc_line::BeforeTerminate()
{
  TGKThread::BeforeTerminate();
  rc_thread.join();
  do_disconnect();
  CoUninitialize();
}

int  __fastcall opc_line::Execute()
{
 int  ret = 0;
 DWORD   key;
 DWORD   time_out;
 bool    wr;
 DWORD   bt;
 OVERLAPPED * lpovrlp = NULL;
 WTimer_start(WT_CHECK_CONNECT,WT_CONNECT_TIMEOUT);

 do{
    time_out = WT_RESOLUTION;
    key = 0;
    wr = icp.GetStatus(key,lpovrlp,time_out,&bt);
    if(wr){
      switch(key)
      {
        case ICP_KEY_HANDLE_CHANGES : handle_changes();break;
        case ICP_KEY_REMOTE_CONTROL : rc_start      ();break;
        case ICP_KEY_REFRESH        : do_refresh    ();break;
        default : break;
      }
    }
    else
    on_timeout();
   }while(!CheckTerminateRequest());
 do_disconnect();
 return ret;
}

void __fastcall opc_line::on_connect_timeout()
{
 if(is_connected())
 {
   if(!opc_server->update_status())
        connect(false);
 }
 else
     do_connect();
 WTimer_start(WT_CHECK_CONNECT,WT_CONNECT_TIMEOUT);
}

void __fastcall opc_line::on_timeout()
{
 if(WTimer_check(WT_CHECK_CONNECT))
    on_connect_timeout();

 if(changes_map.size()) handle_changes();
}

void  __fastcall  opc_line::handle_changes()
{
//  TLockHelper l(data_locker);
   TLockHelper l(locker);

  opc_line::otd_changes_map_t::iterator beg = changes_map.begin();
  opc_line::otd_changes_map_t::iterator end = changes_map.end  ();
  while(beg != end)
  {
    proto_pointer ptr;
    sotd_addr addr(beg->first);
    if(storage.find(addr,ptr))
      {
//        if(ptr->op.diag)
//           {
//            *ptr->op.diag  = OTD_DIAG_GOOD;
//            //*ptr->op.diag |= otd_scan_personal_diag(&ptr->op);
//           }

        __queue_rxdata(&ptr->op);
        if(ptr->op.personal_chmask)
        {
         DWORD zero = 0;
         otd_fill(ptr->op.personal_chmask,&zero,sizeof(zero));
        }
      }
    ++beg;
  }
  changes_map.clear();

}


void  __fastcall opc_line::start_async  ()
{

  if(opc_group)
  {
   opc_data_callback = new opc_data_callback_t;
   HRESULT res = opc_data_callback->start(opc_group,this);
   if(FAILED(res))
      {
       stop_async();
       report_com_error(res,_T("Start opc data callback"));
      }
  }
}

void  __fastcall opc_line::stop_async  ()
{
  if(opc_data_callback)
  {
   opc_data_callback->stop();
   while(opc_data_callback->Release())
        ;
   opc_data_callback = NULL;
  }
}


void  __fastcall opc_line::on_connect   ()
{
  Sleep(1000);
  __opc_create_group();
  build_otd_storage();
  start_async();
  modem_line::on_connect();
}

void  __fastcall opc_line::on_disconnect()
{
  modem_line::on_disconnect();
  __set_bad_diag();
  stop_async();
  if(opc_group)
     delete opc_group;
  opc_group = NULL;
 if(opc_server)
  {
    delete opc_server;
    opc_server = NULL;
  }
}

void __fastcall opc_line::do_connect()
{

  GUID server_guid = {0};
  wchar_t host[MAX_PATH] = {0};
  {
   TLockHelper l(locker);
   if(!opc_items.size())
   {
     this->do_report(REPORT_ERROR_TYPE,L"Нет записей в файле конфигурации. Незачем запускаться");
     return;
   }
   server_guid = line_config.server_guid;
   memcpy(host,line_config.server_host,sizeof(line_config.server_host));
   if(!opc_server) opc_server = new opc::TOpcServer;
  }

  if(!opc_server->IsBound())
  {
      set_state(MODEM_LINE_STATE_CONNECTING,true,true);
      HRESULT res = opc_server->create_instance(server_guid,host);
      if(res)
      {
        if(com_last_result != res)
        {
         com_last_result = res;
         report_com_error(res);
         set_state(MODEM_LINE_STATE_CONNECTING,false,true);
        }
      }
  }
  connect( opc_server->IsBound() );
}

void __fastcall opc_line::do_disconnect()
{
 connect(false);
}

void __fastcall opc_line::__set_bad_diag()
{
   TLockHelper l(locker);
   proto_pointer ptr = storage.begin();
   proto_pointer end = storage.end  ();
   while(ptr<end)
   {
     sotd_proto & sop = *ptr;
     if(sop.op.diag         ) *sop.op.diag = OTD_DIAG_CPMASK;

     if(sop.op.personal_diag) {
      DWORD diag = OTD_PDIAG_NODATA|OTD_PDIAG_ERROR;
      otd_fill(sop.op.personal_diag,&diag,sizeof(diag));
      ++ptr;
     }
     if(sop.op.kpk)  otd_proto_protect(&sop.op,sop.op.proto_size,OTD_DEF_POLINOM);
   }
}


void __fastcall opc_line::__opc_group_add_items()
{
     TLockHelper l(locker);
     gkopc_items_t::iterator ptr = opc_items.begin();
     gkopc_items_t::iterator end = opc_items.end();
     int try_count = 0;
     //OPCHANDLE hClient = 0;

     while(this->opc_group &&  ptr<end)
      {
       ptr->reg_result = opc::TOpcItemResult();
       ptr->item_state.wQuality = OPC_QUALITY_BAD;
       gkopc_item & item = *ptr;
       item.reg_result.hServer = -1;
       item.item_state.hClient = std::distance(opc_items.begin(),ptr);

       HRESULT res = opc_group->add_item(item.item_state.hClient,item.id.c_str(),item.access_path.c_str(),&item.reg_result);
       if(FAILED(res))
          {
           if(++try_count<10)
           {
           if(com_last_result != res )
              report_com_error(res,item.id.c_str());
           Sleep(200);
           continue;
           }
           else
           handle_com_error(res,item.id.c_str());
          }
          else
          {
           for(int i = 0;i<16 && res!= S_OK;i++)
              {
                Sleep(100);
                if(item.last_error != res)
                {
                  item.last_error = res;
                  TCHAR msg[2048];
                  snwprintf(msg,KERTL_ARRAY_COUNT(msg),_T("add item %s"),item.id.c_str());
                  report_opc_error(res,msg);
                }
                res = opc_group->add_item(item.item_state.hClient,item.id.c_str(),item.access_path.c_str(),&item.reg_result);
              }
           try_count = 0;
          }
        com_last_result = res;
       if(item.reg_result.hServer == (DWORD)-1)
          {
            //Неудачно зарегестрировано
            //Удалить ?
            //opc_items.erase(ptr);
            //--end;
          }
          else
          opc_group->set_items_active_state(1,&item.reg_result.hServer,true);

         ++ptr;
      }

}

void __fastcall opc_line::__opc_create_group()
{
   {
   std::wstring grp_name;
   DWORD rate;
   float bw;

   {
     TLockHelper l(locker);
     grp_name = line_config.cp_name;
     rate     = line_config.rate;
     bw       = line_config.bandwidth;
   }

   if(!opc_group) opc_group = new TOpcGroup(*opc_server);
    HRESULT res = opc_group->create_group(grp_name.c_str(),rate,bw,0);

    if(FAILED(res))
      {
        handle_com_error(res);
        return;
      }
    __opc_group_add_items();
   }
   refresh(-1);
}

DWORD __fastcall quality2otddiag(WORD quality)
{
  DWORD diag = 0;
  if(quality)
  {

   if( quality&OPC_QUALITY_NOT_CONNECTED )
         diag |= OTD_DIAG_CPCONNECT;
   if( quality&OPC_QUALITY_DEVICE_FAILURE )
         diag |= OTD_DIAG_MODRESPOND;
  }
  else
  diag = OTD_DIAG_NODATA;
  return diag;
}

#define OPCI_CH_VALUE   1
#define OPCI_CH_QUALITY 2
#define OPCI_CH_RCSTATE 4

void  __fastcall opc_line::__set_opc_item_values(gkopc_item & item,LPVARIANT v,LPWORD quality,__int64 * time ,LPDWORD rc_state )
{
   proto_pointer ptr;
   sotd_param_addr pa = item.group_param.get_param_addr();
   bool changes = false;

   if(quality )
    {
      WORD new_quality = (*quality)&global_quality_mask;
      if(item.item_state.wQuality != new_quality)
      {
       item.item_state.wQuality = new_quality;
       changes = true;
      }
    }

   if(rc_state && item.rc_state != *rc_state)
    {
           item.rc_state = *rc_state;
           changes = true;
    }
    if(time )
      item.item_state.ftTimeStamp = *((LPFILETIME)time);

   if(v)
    {
       VariantCopy(&item.item_state.vDataValue,v);
       __calc_item_value(item);
    }

  if(storage.find(pa.addr,ptr))
   {
       QWORD value = 0;
       otd_get_value(ptr->op.data,pa.param_number,&value,sizeof(value));
       DWORD personal_diag = item.get_personal_diag();
       if(quality)
       {
        DWORD new_diag = quality2otddiag(*quality);
        if( *ptr->op.diag != new_diag )
          {
            *ptr->op.diag =  new_diag;
            changes = true;
          }
       }

       if(item.item_state.is_value_integer())
       {
         if(value  !=  (QWORD)item.data_value || changes)
            {
             value  =  (QWORD)item.data_value;
             changes   = true;
             otd_proto_setvalue(&ptr->op,pa.param_number,&value,sizeof(value),personal_diag,true);
            }
       }
       else
       {
         if(ptr->op.data->otd_type == OTD_SIGNED_FLOAT)
           {
            float * fval = (float*)&value;
            if(fabs(*fval-(float)item.data_value)>.0001 || changes)
              {
               *fval = (float)item.data_value;
               otd_proto_setvalue(&ptr->op,pa.param_number,fval,sizeof(*fval),personal_diag,true);
               changes = true;
              }
           }

         if(ptr->op.data->otd_type == OTD_SIGNED_DOUBLE)
           {
            double * dval = (double*)&value;
            if(fabs(*dval - (double)item.data_value)>.0001 || changes)
              {
              *dval = (double)item.data_value;
               otd_proto_setvalue(&ptr->op,pa.param_number,dval,sizeof(*dval),personal_diag,true);
               changes = true;
              }
           }
        }

        if(ptr->op.time_stamp)
             *ptr->op.time_stamp = std::max((QWORD)*time,*ptr->op.time_stamp);
      if(changes)
         changes_map[pa.addr] += 1;

   }
   else
    changes = true;

      if(changes)
      {
//        if(time)
//        {
//        *((__int64*)&item.item_state.ftTimeStamp)    = *time;
//        }

        if(owner)
           owner->notify(MNF_LINE_ITEM_CHANGED,MAKELONG((WORD)line_number,(WORD)item.item_state.hClient),NULL,0 );
      }


}

  bool __fastcall try_lock(KeRTL::TFASTMutex & locker,DWORD to,DWORD repeat)
  {
    do{
       if(locker.TryLock()) return true;
          Sleep(to);
      }while(repeat--);
    return false;
  }


void  __fastcall opc_line::__read_sync()
{
   //if(!this->opc_data_callback)
    {
     OPCITEMSTATE * item_values = NULL;
     LONG         * errors = NULL;
     opc_handles_t  handles;
     DWORD        rx_bytes = 0;
     DWORD        rx_errors   = 0;
     gkopc_items_t::index_iterator ibeg;
     gkopc_items_t::index_iterator iend;
     DWORD count;
      {
       TLockHelper l(locker);
       opc_items.build_index();
       ibeg  = opc_items.index_begin();
       iend  = opc_items.index_end();
       count = __get_opc_handles(handles,ibeg,iend);
      }


     HRESULT res =  opc_group->read(true,handles.size(),&*handles.begin(),&item_values,&errors);
    if(FAILED(res))
       {
        if(com_last_result != res)
        {
          com_last_result = res;
          handle_com_error(res);
        }
        ++rx_errors;
       }
       else
       {
         OPCITEMSTATE * item_values_beg = item_values;
         LPLONG    perror = errors;

         while(count && ibeg < iend)
         {
          if(try_lock(locker,100,10))
          {
          gkopc_item & item = opc_items.at(*ibeg);
          if(*perror && item.last_error != *perror)
          {
            item.last_error = *perror;
            report_opc_error(item.last_error);
            ++rx_errors;
          }
            else
            {
              item.last_error = 0;
              __set_opc_item_values(item,&item_values_beg->vDataValue,&item_values_beg->wQuality,(__int64*)&item_values_beg->ftTimeStamp);
            }
           locker.unlock();
          }
          else
          TRACE(L"OPC-Line read_sync try lock timeout",0);

          VariantClear(&item_values_beg->vDataValue);
          ++item_values_beg;
          ++perror;
          ++ibeg;
         }
       }

    if(item_values)
    {
       CoTaskMemFree(item_values);
       rx_bytes += handles.size()*sizeof(*item_values);
    }

    if(errors)
    {
     CoTaskMemFree(errors);
     rx_bytes += handles.size()*sizeof(*errors);
    }
    update_stats_rx(rx_bytes,0,rx_errors);
   }
}


void __fastcall opc_line::do_refresh()
{
      DWORD cancel_id = 0;
      HRESULT res = opc_group->async2_refresh(OPC_DS_DEVICE,cancel_id);
      if(FAILED(res))
         {
          if(res!=HRESULT(-1)) //Интерфейс получен ошибка при вызове
            report_com_error(res,L"Refresh. do read manual");
          __read_sync();
         }
}

DWORD __fastcall opc_line::refresh(DWORD sbl)
{
  if(is_connected() /*&& !opc_data_callback*/)
     {
      icp.PostStatus(ICP_KEY_REFRESH,0,0);
     }
  return GKH_RET_SUCCESS;
}


/*Helper functions*/

DWORD __fastcall opc_line::__get_opc_handles (opc_handles_t & handles,gkopc_items_t::iterator beg,gkopc_items_t::iterator end)
{
  handles.resize(std::distance(beg,end));
  opc_handles_t::iterator hptr = handles.begin();
  while(beg<end)
  {
   *hptr = beg->reg_result.hServer;
   ++hptr;
   ++beg;
  }
  return handles.size();
}

DWORD __fastcall opc_line::__get_opc_handles (opc_handles_t & handles,gkopc_items_t::index_iterator ibeg,gkopc_items_t::index_iterator iend)
{
  DWORD count =  std::distance(ibeg,iend);
  handles.resize(count);
  opc_handles_t::iterator hptr = handles.begin();
  while(ibeg<iend)
  {
   *hptr = opc_items.at(*ibeg).reg_result.hServer;
   ++hptr;
   ++ibeg;
  }
  return handles.size();

}


void __fastcall opc_line::handle_com_error(HRESULT res,const TCHAR *msg )
{

  report_com_error(res,msg);
  if(FAILED(res))  connect(false);
}

void __fastcall opc_line::do_report(DWORD rep_type, wchar_t * text,LPVOID data,DWORD data_size)
{
  TGkOpcModem * modem = dynamic_cast<TGkOpcModem*>(owner);
  if(modem)
   modem->do_report(rep_type,text,data,data_size);
}

void __fastcall opc_line::report_com_error(HRESULT res,const TCHAR * msg)
{
  TGkOpcModem * modem = dynamic_cast<TGkOpcModem*>(owner);
  if(modem)
  {
     TCHAR text[4096];
     _com_error ce(res);
     snwprintf(text,KERTL_ARRAY_COUNT(text),_T("Модем %d линия %d %s\n\rОшибка COM %s"),owner->get_modem_number(),get_number(),msg ? msg:_T(""),ce.ErrorMessage());
     modem->do_report(REPORT_ERROR_TYPE,text,NULL,NULL);
  }
}

void __fastcall opc_line::report_opc_error(LONG err,const TCHAR * msg)
{
  TGkOpcModem * modem = dynamic_cast<TGkOpcModem*>(owner);
   if(modem && err)
   {
       wchar_t * err_str = NULL;
       (*opc_server)->GetErrorString(err,0,&err_str);
         TCHAR text[4096];
         snwprintf(text,KERTL_ARRAY_COUNT(text),_T("Модем %d линия %d %s\n\rОшибка OPC %d %s"),owner->get_modem_number(),this->get_number(),msg ? msg : _T(""),err,err_str ? err_str : _T(""));

         CoTaskMemFree(err_str);
         modem->do_report(REPORT_ERROR_TYPE,text,NULL,NULL);

   }
}


  void __fastcall opc_line::on_data_change  ( DWORD     conn_id  , opc::TOpcGroup * grp       , DWORD dwTransid
                                             ,OPCHANDLE hGroup   , HRESULT     hrMasterquality, HRESULT     hrMastererror
                                             ,DWORD     dwCount  , OPCHANDLE * phClientItems  , VARIANT   * pvValues
                                             ,WORD  * pwQualities, FILETIME  * pftTimeStamps  , HRESULT   * pErrors)
  {
      DWORD rd_bytes  = dwCount*(sizeof(*phClientItems) + sizeof(*pvValues) + sizeof(*pwQualities)+ sizeof(*pftTimeStamps) + sizeof(*pErrors)  );
      DWORD err_count = 0;
      //TRACE(_T("on_data_changes - count %d"),dwCount);
      if(try_lock(locker,100,10))
      {
           //TLockHelper l(locker);


          OPCHANDLE * phClientItemsEnd = phClientItems+dwCount;
          while(phClientItems<phClientItemsEnd)
          {


           gkopc_item & item = opc_items.at(*phClientItems);
           __set_opc_item_values(item,pvValues,pwQualities,(__int64*)pftTimeStamps);
           ++phClientItems;
           ++pvValues;
           ++pwQualities;
           ++pftTimeStamps;
           if(*pErrors) ++err_count;
           ++pErrors;
          }
           icp.PostStatus(ICP_KEY_HANDLE_CHANGES,rd_bytes,NULL);
           update_stats_rx(rd_bytes,err_count,0);
          locker.Unlock();
      }
      else
      {
        wchar_t  rep_text[1024];
        snwprintf(rep_text,KERTL_ARRAY_COUNT(rep_text),L"%s:%s on_data_change -> FastMutex lock failed", line_config.server_host,line_config.server_prog_id);
        do_report(REPORT_ERROR_TYPE,rep_text,NULL,0);
      }
  }

  void __fastcall opc_line::on_read_complete( DWORD     conn_id  , opc::TOpcGroup * grp       , DWORD dwTransid
                                             ,OPCHANDLE hGroup   , HRESULT     hrMasterquality, HRESULT     hrMastererror
                                             ,DWORD     dwCount  , OPCHANDLE * phClientItems  , VARIANT   * pvValues
                                             ,WORD  * pwQualities, FILETIME  * pftTimeStamps  , HRESULT   * pErrors)
  {
      //TLockHelper l(locker);
      TRACE(_T("on_read_complete - count %d"),dwCount);
  }


  void __fastcall opc_line::on_write_complete(DWORD     conn_id  , opc::TOpcGroup * grp       , DWORD dwTransid
                                             ,OPCHANDLE hGroup   , HRESULT     hrMasterquality, DWORD       dwCount
                                             ,OPCHANDLE * pClienthandles, HRESULT   * pErrors)
  {
      //TLockHelper l(locker);
      TRACE(_T("on_write_complete - count %d"),dwCount);
  }

 void __fastcall opc_line::on_cancel_complete(DWORD     conn_id  , opc::TOpcGroup * grp       , DWORD dwTransid,OPCHANDLE hGroup)
  {
      //TLockHelper l(locker);
      TRACE(_T("on_cancel_complete "),0);

  }


   void  __fastcall opc_line::WTimer_start(int idx,int tmo)
   {
     if(idx<MAX_WT_TIMERS)
     {
       TLockHelper l(locker);
       WT[idx] = tmo;
     }
   }

   void  __fastcall opc_line::WTimer_stop(int idx)
   {
     if(idx<MAX_WT_TIMERS)
     {
       TLockHelper l(locker);
       WT[idx] = 0;
     }
   }

   bool  __fastcall opc_line::WTimer_check(int idx)
   {
     if(idx<MAX_WT_TIMERS)
     {
       TLockHelper l(locker);
       int & wt = *(WT+idx);
       if(wt)
         {
           wt-=WT_RESOLUTION;
           if(wt<=0)
             {
              wt = 0;return true;
             }
         }
     }
    return false;
   }


 DWORD __fastcall opc_line::get_opc_item(DWORD idx,gkopc_item ** p_item)
 {
    TLockHelper l(locker);
    if(idx<opc_items.size() && p_item)
    {
      gkopc_item & src = opc_items.at(idx);
      *p_item = new gkopc_item(src);
      if(*p_item)
      {
       (*p_item)->reg_result = src.reg_result;
       (*p_item)->item_state = src.item_state;
        return GKH_RET_SUCCESS;
      }
    }
   return GKH_RET_ERROR;
 }

 DWORD __fastcall opc_line::release_opc_item(gkopc_item * item)
 {
     if(item)
       {
        delete item;
        return GKH_RET_SUCCESS;
       }
    return GKH_RET_ERROR;
 }

  bool __fastcall opc_line::opc_item_set_value(const wstring & item_id,double val)
  {
    gkopc_items_t::iterator ptr;
    TLockHelper l(locker);
    if(is_connected() && opc_items.find(*&gkopc_item (item_id),ptr) && ptr->reg_result.hServer != (DWORD) -1)
     {
       opc::TVariant v;
       gkopc_item &item = *ptr;
       v.vt = item.reg_result.vtCanonicalDataType;
       opc::TVariant iv(int(0));
       opc::TVariant src(val);
       //v = src;
       VariantChangeType(&v,&src,0,v.vt);
       LPLONG errors;
       HRESULT res = opc_group->write(1,&item.reg_result.hServer,&v,&errors);
       if(errors)
          CoTaskMemFree(errors);
       return SUCCEEDED(res) ? true : false;
     }
    return false;
  }

 int   __fastcall opc_line::opc_set_group_quality  (BYTE fa,BYTE grp,WORD quality)
 {

  BYTE fa0  = fa  == OTD_ADDR_MAXVALUE ? 0 : fa;
  BYTE grp0 = grp == OTD_ADDR_MAXVALUE ? 0 : grp;
  gkopc_item i0(fa0,grp0,0);
  gkopc_item i1(fa,grp  ,-1);

  TLockHelper l(locker);
  gkopc_items_t::index_iterator lo_idx = opc_items.index_begin()  ,hi_idx = opc_items.index_end();
  int count = opc_items.range(i0,i1,lo_idx,hi_idx);
  if( count  )
  {
   __int64 tm = GetTime();
   count = 0;
   while(lo_idx <hi_idx )
   {
     gkopc_item & item = opc_items.at(*lo_idx );
     if(item.item_state.wQuality != quality)
     {
      __set_opc_item_values(item,NULL,&quality,&tm,NULL);
      ++count;
     }

     ++lo_idx ;
   }
  }
  return count;
 }

 int   __fastcall opc_line::opc_set_global_quality_mask  (WORD quality,bool set)
 {
    if(set)
      {
       DWORD new_mask = global_quality_mask | quality;
       if(global_quality_mask != new_mask)
       {
          global_quality_mask |= new_mask;
          if(global_quality_mask & OPC_QUALITY_GOOD)
             refresh(-1);
       }
      }
      else
      {
       global_quality_mask &= ~quality;
      }
    return 1;
 }







