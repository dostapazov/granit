#pragma hdrstop
 #include "mediumdb.hpp"
 #include <modem_proto.h>
 #include <alloc.h>
 #include <tregstry.hpp>


 THeap TMediumDB::heap; // 512 Кб
 DWORD TMediumDB::total_mem_alloc = 0;

 void * __stdcall TMediumDB::mem_alloc(size_t sz )
 {
  static int i = 0;
  void * ptr = NULL;
  if(!heap.IsValid())
	  heap.Create((TSysInfo().GetPageSize()<<8));
      ptr = heap.Alloc(sz);
  if(!ptr)
      {
       DebugBreak();
       ptr = malloc(sz);
      }
      else
      total_mem_alloc += sz;

  return ptr;
 }

 void   __stdcall TMediumDB::mem_free (void * ptr )
 {
  if(ptr)
  {
   if(heap.IsValid() &&  heap.Validate(ptr))
	 {
          total_mem_alloc -= heap.MemSize(ptr);
          heap.Free(ptr);
         }
   else
   {
     __try{
     //::free(ptr);
     }
     __finally{}
   }
  }
 }

DWORD   __fastcall TMediumDB::get_mem_used()
 {
  //return heap.IsValid() ?  heap.GetMemoryUsed() : GKH_RET_ERROR;
  return total_mem_alloc;//GKH_RET_ERROR;
 }



 TMediumDB::TMediumDB()
           :int_line(-1)
           ,terminate_event(true,false)
           ,waitable_timer(false)
           ,archvie_thread(this)
 {

  lock_param = GKHB_AUTO_LOCK_OFF;
  alloc_gkhandle(MEDIUMDB_MODULE_NAME);
  mdb.set_module(this);
  report_id      = report_reg_event_type(L"БД-Среднего уровня");
  report_tu_id   = report_reg_event_type(L"БД-Среднего уровня ТУ-ТР");
  report_scripts = report_reg_event_type(L"БД-Среднего уровня - ошибки скриптов");
  ZeroMemory(&settings,sizeof(settings));
  mdb_record rec(1);
  rec.min_value = 0;
  rec.max_value = 5;
  rec.options   = MDBR_OPT_SIGNED_KVANTS;
  rec.kvants    = 520;
  short skv1 = 255,skv2 = -255;

  DWORD kv = 0;
  memcpy(&kv,&skv1,sizeof(short));
  rec.calc_value(kv);
  memcpy(&kv,&skv2,sizeof(short));
  rec.calc_value(kv);
  SetFlags(KERTL_THREAD_ENABLERESTART,true);
 }

 const char  * __fastcall TMediumDB::get_gkthread_name()
 {
  return "TMediumDB::handler thread";
 }

 DWORD __fastcall TMediumDB::OnException(DWORD ExceptionCode,LPEXCEPTION_POINTERS exp)
 {
  mdb.full_unlock();
  full_unlock();
  input_queue.full_unlock();
  input_queue.DropData();
  return TGKThread::OnException(ExceptionCode,exp);
 }


 void   __fastcall TMediumDB::set_ccflags(DWORD fl,bool set)
 {
   lock();
   cc_flags = set ? (cc_flags|fl) : (cc_flags&(~fl));
   unlock();
 }

#pragma warn -8057
 DWORD   __fastcall TMediumDB::get_config_data(DWORD mask,LPVOID buf,DWORD bsz)
 {
  DWORD ret = GKH_RET_ERROR;
  if(bsz>=sizeof(settings))
  {
   CopyMemory(buf,&settings,sizeof(settings));
   ret = GKH_RET_SUCCESS;
  }
  return ret;
 }

 bool    __fastcall TMediumDB::set_config_data(DWORD mask,LPVOID buf,DWORD bsz)
 {
  bool ret = true;
  if(bsz>=sizeof(settings))
  {
    mdb.store(settings.mdb_path, cc_flags&MDB_CCFL_MASK);
    cc_flags&=~MDB_CCFL_MASK;
    memcpy(&settings,buf,sizeof(settings));
    open_mdb();
    notify(MDB_NOTIFY_MASK_RECORD,MDB_NOTIFY_CODE_DBREOPEN,0,0);
  }
  else
  ret = false;
  return ret;
 }

 bool    __fastcall TMediumDB::compare_config_data(DWORD mask,LPVOID buf,DWORD bsz,
                                                   LPDWORD ch_mask,BOOL * restart)
 {
  bool ret = true;
  if(bsz>=sizeof(settings))
  {
   DWORD chm = 0;
   BOOL  rst = FALSE;

   LPMDB_SETTINGS s = (LPMDB_SETTINGS)buf;
   if(wcsncmpi(settings.mdb_path,s->mdb_path,KERTL_ARRAY_COUNT(settings.mdb_path)))
      {
       chm |= MDB_CCFL_SETTINGS_MDBPATH;
       if(is_running())
          mdb.send_database_reopen();
       rst  = TRUE;
      }
   if(ch_mask)
      *ch_mask = chm;
   if(restart)
      *restart = rst;

  }
  else
  ret = false;
  return ret;
 }

#pragma warn .8057

 #ifdef _DEBUG
 void   __fastcall create_db(med_db & mdb)
 {
    wchar_t name [MAX_PATH];
    mdb_record rec;
    DWORD id ;
    sotd_addr sa(10,54,1,1);
    sotd_param_addr pa(sa,0);

   for(int i = 0;i<8;i++)
    {
     pa.param_number = i;
     mdb_record rec(pa);
     id = mdb.create(&rec);
     wsprintfW(name,L"%Имя ID-%d",id);
     mdb.set_name(id,name,false);
     lstrcatW(name,L" -- Длинное");
     mdb.set_name(id,name,true);
    }

    sa = sotd_addr(10,49,1,0);
    pa = sotd_param_addr (sa,0);
   for(int i = 0;i<8;i++)
    {
     pa.param_number = i;
     mdb_record rec(pa);
     id = mdb.create(&rec);
     wsprintfW(name,L"%Имя ID-%d",id);
     mdb.set_name(id,name,false);
     lstrcatW(name,L" -- Длинное");
     mdb.set_name(id,name,true);
    }
    sa = sotd_addr(10,49,1,1);
    pa = sotd_param_addr (sa,0);
   for(int i = 0;i<8;i++)
    {
     pa.param_number = i;
     mdb_record rec(pa);
     id = mdb.create(&rec);
     wsprintfW(name,L"%Имя ID-%d",id);
     mdb.set_name(id,name,false);
     lstrcatW(name,L" -- Длинное");
     mdb.set_name(id,name,true);
    }
    rec.options = MDBR_OPT_COMBINED;
    rec.addr = sotd_param_addr();
    id = mdb.create(&rec,"x0*x1");
    id = mdb.create(&rec,"x0+x1");
 }
 #endif


 void    __fastcall TMediumDB::open_mdb()
 {
   int len = wcslen(settings.mdb_path);
    if(len)
    {
     if(settings.mdb_path[len-1]!='\\')
        settings.mdb_path[len++]='\\';
     ForceDirs(settings.mdb_path);
    }
    mdb.open (settings.mdb_path);
 }

 bool    __fastcall TMediumDB::read_settings  ()
 {
  bool ret = TGKModule::read_settings();
  TRegsReader rr(DupKey(this->reg_key));
  if(!rr.ReadString(REGSTR_MDBDATA,settings.mdb_path,KERTL_ARRAY_COUNT(settings.mdb_path),true))
      wcscpy(settings.mdb_path,MEDIUMDB_DATA);
  open_mdb();
  #ifdef _DEBUG
   //create_db(mdb);
  #endif

  return ret;
 }
 bool    __fastcall TMediumDB::write_settings ()
 {
  bool ret = TGKModule::write_settings();
   lock();
   DWORD fl = cc_flags&MDB_CCFL_MASK;
   cc_flags&=~MDB_CCFL_MASK;
  TRegsWriter wr(DupKey(this->reg_key));
  wr.WriteString(REGSTR_MDBDATA,settings.mdb_path,true);
  unlock();
  mdb.store(settings.mdb_path,fl);
  return ret;
 }

 DWORD   __fastcall TMediumDB::get_window_module  (wchar_t * buf,DWORD bsz)
 {
   wchar_t * wnd_name = MEDIUMDB_WINDOW_MODULE;
   DWORD ret = wcslen(wnd_name);
   safe_strcpyn(buf,wnd_name,bsz);
   return  ret;
 }

 LRESULT __fastcall TMediumDB::get_modem_props(LPMODEM_PROPS mp)
 {
  LRESULT ret = GKH_RET_ERROR;
  //Эмуляция поведения модема
  lock();
  if((DWORD)mp->addr.line == int_line)
   {
      ret = GKH_RET_SUCCESS;
      mp->lines_count = 1;
      mp->lines_limit = 1;
      mp->flags = MPF_DATA_ACCEPTOR;
      if(mp->modem_info_size>8)
         mp->modem_info_len = wsprintfW(mp->modem_info,L"MEDIUMDB");
   }
  unlock();
  return ret;
 }

 LRESULT __fastcall TMediumDB::processing(DWORD cmd,LPARAM p1,LPARAM p2)
 {
  LRESULT ret;
  switch(cmd)
  {
   case     MDMCM_FA_HANDLE           : ret = fa_handler((LPMPROTO_HEADER)p1);break;
   case     MODEMCM_GET_MODEM_PROPS   : ret = get_modem_props((LPMODEM_PROPS)p1);break;
   case     MODEMCM_SEND              : ret = recv_handler((LPMPROTO_HEADER)p1,(DWORD)p2);break;
   case     MDBCM_RECORD_ENUM_BYID    : ret = mdb.enum_records((DWORD)p1,FALSE,(LPMDB_RECORD)p2);break;
   case     MDBCM_RECORD_ENUM_BYADDR  : ret = mdb.enum_records((DWORD)p1,TRUE, (LPMDB_RECORD)p2);break;
   case     MDBCM_RECORD_GET          : ret = mdb.get_record  ((DWORD)p1,(LPMDB_RECORD)p2);break;
   case     MDBCM_RECORD_GET_SNAME    : ret = get_name((DWORD)p1,(LPMDB_RECORD_TEXT)p2,false);break;
   case     MDBCM_RECORD_GET_NAME     : ret = get_name((DWORD)p1,(LPMDB_RECORD_TEXT)p2,true);break;
   case     MDBCM_RECORD_SET_SNAME    : ret = set_name((DWORD)p1,(LPMDB_RECORD_TEXT)p2,false);break;
   case     MDBCM_RECORD_SET_NAME     : ret = set_name((DWORD)p1,(LPMDB_RECORD_TEXT)p2,true);break;
   case     MDBCM_GET_COUNT           : ret = mdb.get_count(*&sotd_addr(p1));break;
   case     MDBCM_ENUM_OBJECTS        : ret = mdb.enum_objects(*&sotd_addr(p1),(DWORD)p2);break;
   case     MDBCM_COMBINE_GET_COUNT   : ret = mdb.get_combine_count((DWORD)p1);break;
   case     MDBCM_COMBINE_ENUM        : ret = mdb.enum_combine((DWORD)p1,(LPMDB_RECORD)p2);break;
   case     MDBCM_COMBINE_GET_FORMULA : ret = mdb.get_combine_formula((DWORD)p1,(LPMDB_RECORD_TEXT)p2);break;
   case     MDBCM_COMBINE_SET_FORMULA : ret = mdb.set_combine_formula((DWORD)p1,(LPMDB_RECORD_TEXT)p2);break;
   case     MDBCM_COMBINE_GET_LIST    : ret = mdb.get_combine_list((DWORD)p1,(LPMDB_COMBINE_LIST)p2);break;
   case     MDBCM_COMBINE_SET_LIST    : ret = mdb.set_combine_list((DWORD)p1,(LPMDB_COMBINE_LIST)p2);break;
   case     MDBCM_UPDATE_TEXT         : ret = mdb.update_text((DWORD)p1,(LPMDB_RECORD_TEXT)p2); break;
   case     MDBCM_GET_TEXT            : ret = mdb.get_text   ((DWORD)p1,(LPMDB_RECORD_TEXT)p2);break;
   case     MDBCM_RECORD_CREATE       : ret = mdb.create((LPMDB_RECORD)p1,NULL);break;
   case     MDBCM_RECORD_REMOVE       : ret = mdb.remove((DWORD)p1);break;
   case     MDBCM_RECORD_UPDATE       : ret = mdb.update((LPMDB_RECORD)p1,(DWORD)p2);break;
   case     MDBCM_KADR_ENUM           : ret = mdb.kadr_enum((DWORD)p1,(LPMDB_KADR)p2);break;
   case     MDBCM_KADR_GET            : ret = mdb.kadr_get ((DWORD)p1,(LPMDB_KADR)p2);break;
   case     MDBCM_KADR_CREATE         : ret = mdb.kadr_create((LPMDB_KADR)p1);break;
   case     MDBCM_KADR_REMOVE         : ret = mdb.kadr_remove((DWORD)p1);break;
   case     MDBCM_KADR_UPDATE         : ret = mdb.kadr_update((LPMDB_KADR)p1,(DWORD)p2);break;
   case     MDBCM_KADR_GET_ENTRY_LIST : ret = mdb.kadr_get_entry_list((DWORD)p1,(LPMDB_KADR_ENTRY_LIST)p2);break;
   case     MDBCM_KADR_SET_ENTRY_LIST : ret = mdb.kadr_set_entry_list((DWORD)p1,(LPMDB_KADR_ENTRY_LIST)p2);break;
   case     MDBCM_KADR_FIND           : ret = mdb.kadr_find_byname((wchar_t*)p1,(LPDWORD)p2);break;
   case     MDBCM_KADR_FIND_RECORD    : ret = mdb.kadr_find_record((DWORD)p1,(DWORD)p2);break;
   case     MDBCM_KADR_ADD_RECORD     : ret = mdb.kadr_add_record((DWORD)p1,(DWORD)p2);break;
   case     MDBCM_GET_DBINFO          : ret = mdb.get_info((LPMDB_DBINFO)p1);break;
   case     MDBCM_ENUM_ORDERS         : ret = mdb.enum_orders(p1,(LPORDER_ITEM)p2);break;

   case     MDBCM_COMBINE_REBUILD_INDEX: ret = mdb.rebuild_combine_index()               ;break;
   //case     MDBCM_RECORD_OPERATION     : ret = mdb.record_operation((DWORD)p1,(DWORD)p2,.0) ;break;
   case     MDBCM_ADDRECORDS_BYMASK    : ret = add_record_bymask((DWORD)p1)              ;break;
   case     MDBCM_CHECK_REPAIR         : ret = mdb.check_db((LPMDB_CHECK_PARAM)p1)       ;break;
   case     MDBCM_PAINTVARS_ENUM       : ret = mdb.paint_vars_enum(p1,(LPMDB_PAINT_VARIANT)p2); break;
   case     MDBCM_PAINTVARS_ADD        : ret = mdb.paint_vars_add((LPMDB_PAINT_VARIANT)p1)    ;break;
   case     MDBCM_PAINTVARS_UPD        : ret = mdb.paint_vars_update((LPMDB_PAINT_VARIANT)p1) ;break;
   case     MDBCM_PAINTVARS_CLEAR      : ret = mdb.paint_vars_clear()     ;break;
   case     MDBCM_PAINTVARS_END_UPDATE : ret = mdb.paint_vars_end_update(p1);break;

   default: ret = TGKModule::processing(cmd,p1,p2);break;
  }
  return ret;
 }


 void __fastcall test()
 {
 }

 DWORD   __fastcall TMediumDB::start(DWORD reason,LPARAM sp)
 {
  DWORD ret ;
  ForceDirs(MEDIUMDB_DATA);
  #ifdef _DEBUG
  test();
  #endif
  modemmer.open(MODEMMER_HANDLE_NAME_W);
  int_line = modemmer.call(MDMCM_REG_INTERNAL_MLINE,(LPARAM)_handle,-1);
  modemmer.set_notify(_handle,MNF_MODEM,TRUE);
  modemmer.call(MDMCM_REGISTER_HANDLER,LPARAM(_handle),FA_OTD_MEDIUM);
  modemmer.call(MDMCM_REGISTER_HANDLER,LPARAM(_handle),FA_OTD_MEDIUM_ALARM_ARCHIVE);
  scale.open(SCALE_NAME_W);
  scale.set_notify(_handle,MNF_SCALE,true);
  terminate_event.SetEvent(false);
  TGKThread::Start(32684,false,NULL);
  archvie_thread.Start(32684);

  if(int_line!=(DWORD)-1)
    {
     make_order(sotd_addr(-1),-1);
    }
  ret = TGKModule::start(reason,sp);
  mdb.recalc_combines();
  return ret;
 }

 DWORD   __fastcall TMediumDB::stop (DWORD reason)
 {
    DWORD ret ;
    terminate_event.SetEvent(true);
    archvie_thread.TerminateAndWait(3000,true);
    terminate_event.SetEvent(true);
    TerminateAndWait(3000,true);
    terminate_event.SetEvent(false);
    this->input_queue.DropData();
    if(int_line!=DWORD(-1))
      make_order(sotd_addr(-1),0);//Отказ от подписки
    //Дерегистрация внутренней линии;
    if(int_line!=DWORD(-1))
    {
      modemmer.call(MDMCM_REG_INTERNAL_MLINE,(LPARAM)_handle,int_line);
      int_line = -1;
    }
    modemmer.set_notify(_handle,MNF_MODEM,FALSE);
    modemmer.call(MDMCM_UNREGISTER_HANDLER,LPARAM(_handle),FA_OTD_MEDIUM);
    modemmer.call(MDMCM_UNREGISTER_HANDLER,LPARAM(_handle),FA_OTD_MEDIUM_ALARM_ARCHIVE);


  if(reason!=MODULE_STOP_PROGRAM_SHUTDOWN)
  {
      BYTE buff[256];
      sotd_proto op;
      otd_proto_format(buff,sizeof(buff),OTD_PROTO_PART_DIAG,0,0,0,0,0,0,&op());
      op().addr->addr = -1;
      *op().diag = OTD_DIAG_MASK;
      //Установить диагностику в BAD
       mdb.handle(op());
       mdb.handle_record_changes();
  }

    scale.set_notify(_handle,MNF_SCALE,FALSE);
    scale.close();

    modemmer.close();
    this->cc_flags |=MDB_CCFL_RECORD_TABLE;
    ret = TGKModule::stop(reason);
  return ret;
 }

 LRESULT  __fastcall TMediumDB::on_notify_code(LPNOTIFY_CODE nc,DWORD mask)
 {
   if( is_same_gkhandle(nc->from_module,scale()))
    {
     if((mask &MNF_SCALE) && nc->notify_code == MNCODE_SCALE_ID )
        this->mdb.record_scale_changed(nc->notify_data[0]);
    }
   if(is_same_gkhandle(nc->from_module,modemmer()))
    {
     if((mask & MNF_MODEM) && (nc->notify_code == MNCODE_LINE_STATE))
        on_modem_line_state((LPMODEM_LINE_STATE)nc->notify_data);

    }
   return GKH_RET_SUCCESS;
 }

 void    __fastcall TMediumDB::on_modem_line_state(LPMODEM_LINE_STATE mls)
 {
   if( (mls->state_changes&MODEM_LINE_STATE_CONNECT) &&  !(mls->line_state & MODEM_LINE_STATE_CONNECT))
   {
    if(mdb.remove_medium_order(*&modem_addr(mls->addr),-1))
    {
     wchar_t rep_text[MAX_PATH];
     wsprintfW(rep_text,L"Отключение линии подписчика %03d.%03d. Снятие подписки",(DWORD)mls->addr.modem,(DWORD)mls->addr.line);
     mdb_report(REPORT_INFORMATION_TYPE,rep_text,0,0);
    }
   }
 }

 int   __fastcall TMediumDB::Execute()
 {
  TSynchroSet ss;
  ss+= terminate_event();
  ss+= input_queue.GetEvent();
  ss+= waitable_timer ();
  DWORD wr = -1;

  while(wr && !this->CheckTerminateRequest())
  {
    wr = ss.Wait(1000);
    switch(wr)
    {
     case 1:
     while(input_queue.QueueCount())
           do_handle_recv();
     break;
     case 2: mdb.do_timer(1000);break;
    }
  }
  return 0;
 }

 void  __fastcall TMediumDB::BeforeTerminate()
 {
  terminate_event.SetEvent(false);
  waitable_timer.StopTimer ();
  input_queue.DropData();
 }

 bool  __fastcall TMediumDB::BeforeExecute()
 {
  input_queue.DropData();
  waitable_timer.StartPeriodTimer(1000);
  return  TGKThread::BeforeExecute();

 }


 LRESULT __fastcall TMediumDB::send(LPMPROTO_HEADER mph)
 {
    if(mph && mph->data_size)
     {
      mph->addr_from.addr = modem_addr(0,int_line).addr;
      mproto_protect(mph);
      return modemmer.call(MDMCM_SEND,(LPARAM)mph,MPROTO_SIZE(mph));
     }
   return  GKH_RET_ERROR;
 }
 
 LRESULT __fastcall TMediumDB::make_order(otd_addr &addr,DWORD parts)
 {
   BYTE buffer[1024];
   LPMPROTO_HEADER mph =(LPMPROTO_HEADER)buffer;
   ZeroMemory(mph,sizeof(*mph));
   mph->addr_from.line    = int_line;
   mph->addr_from.reserv1 = -1;
   mph->fa = FA_OTD;
   addr.fa &=OTD_FA_ALL;
   addr.fa|=OTD_FA_ORDER;
   mph->data_size =  otd_proto_format_query((LPBYTE)mph->data,sizeof(buffer)-sizeof(*mph),&addr,0,parts,NULL);
   mproto_protect(mph);
   send(mph);
   mph->fa = FA_OTD_ALARM_ARCHIVE;
   mproto_protect(mph);
   return  send(mph);
 }

 LRESULT __fastcall TMediumDB::send_call (otd_addr & _addr,DWORD parts )
 {
   BYTE buffer[1024];
   LPMPROTO_HEADER mph =(LPMPROTO_HEADER)buffer;
   ZeroMemory(mph,sizeof(*mph));
   sotd_addr addr = _addr;
   mph->addr_from.line    = int_line;
   mph->addr_from.reserv1 = -1;
   mph->fa =FA_OTD;
   addr.fa &=OTD_FA_ALL;
   addr.fa|=OTD_FA_CALL;
   mph->data_size =  otd_proto_format_query((LPBYTE)mph->data,sizeof(buffer)-sizeof(*mph),&addr,0,parts,NULL);
   return  send(mph);

 }

#pragma warn -8057

 LRESULT __fastcall TMediumDB::recv_handler(LPMPROTO_HEADER mph,DWORD sz)
 {
  LRESULT ret = GKH_RET_SUCCESS;
  switch(mph->fa)
  {
   case FA_OTD:
        input_queue.PutIntoQueue(mph,MPROTO_SIZE(mph));
   break;
   case FA_OTD_MEDIUM:
        ret = mdb.handle_otdm_proto(mph);
        break;
   case FA_OTD_ALARM_ARCHIVE:
        archvie_thread.put_data(mph,MPROTO_SIZE(mph));
   break;
   case FA_OTD_MEDIUM_ALARM_ARCHIVE:
        mph->fa = FA_OTD_MEDIUM_ALARM_ARCHIVE;  //DebugBreak();
   break;
   default:
   SetLastError(-1);
   ret = GKH_RET_ERROR;
   break;
  }

  return  ret ;
 }
/*
 LRESULT __fastcall TMediumDB::recv_handler(LPMPROTO_HEADER mph,DWORD sz)
 {
  LRESULT ret = GKH_RET_SUCCESS;
  switch(mph->fa)
  {
   case FA_OTD:
   case FA_OTD_MEDIUM:
   case FA_OTD_ALARM_ARCHIVE:
   case FA_OTD_MEDIUM_ALARM_ARCHIVE:
        input_queue.PutIntoQueue(mph,MPROTO_SIZE(mph));
   break;
   default:
   SetLastError(-1); // Нет обработчика
   ret = GKH_RET_ERROR;
   break;
  }
  return  ret ;
 }
*/
#pragma warn .8057

 LRESULT __fastcall TMediumDB::fa_handler(LPMPROTO_HEADER mph)
 {
   LRESULT ret = GKH_RET_ERROR;
   switch(mph->fa)
   {
    case FA_OTD: ret = mdb.handle_otdm_proto(mph);
         break;
    case FA_OTD_MEDIUM: ret = mdb.handle_otdm_proto(mph);
    break;
    case FA_OTD_MEDIUM_ALARM_ARCHIVE:
         ret = GKH_RET_SUCCESS;
         archvie_thread.put_data(mph,mproto_size(mph));
         break;
   }
   return ret;
 }

 void __fastcall TMediumDB::do_handle_recv()
 {
   sotd_proto sop;
   DWORD      hres      = 0;
   DWORD      in_size   = 0;
   LPBYTE     data_ptr  = NULL;
   wchar_t  * handle_phase ;
 __try{
           handle_phase = L"";
           while(input_queue.QueueCount())
             {
             handle_phase = L"Получение данных из очереди";
             in_size = input_queue.TopDataSize();
             if(handle_buffer.get_free_size()<in_size)
                handle_buffer.set_size(in_size<<1);
             data_ptr = handle_buffer.begin();
             LPMPROTO_HEADER mph = (LPMPROTO_HEADER)data_ptr;

             //TRACE("MEDDB input Queue count %d",input_queue.QueueCount());
             input_queue.GetFromQueue(data_ptr,in_size,NULL);
             handle_phase = L"Разбор протокола otd_proto";
             if(mph->fa == FA_OTD &&  otd_proto_parser ((LPBYTE)mph->data,mph->data_size,&sop(),NULL))
                {
                  handle_phase = L"Обработка otd_proto";
                  hres |= mdb.handle(sop());
                }
             }

           if(hres || mdb.get_record_changes_size())
             {
              handle_phase = L"Обработка-рассылка изменений";
              mdb.handle_record_changes();
             }
       }


   __except(EXCEPTION_CONTINUE_EXECUTION)
   {
    GetExceptionCode();
    wchar_t rep_text[MAX_PATH];
    snwprintf(rep_text,KERTL_ARRAY_COUNT(rep_text),L"Исключение при обработке: %s ",handle_phase);
    mdb_report(REPORT_ERROR_TYPE,rep_text,data_ptr,in_size);
   }
 }




 LRESULT __fastcall TMediumDB::get_name(DWORD id,LPMDB_RECORD_TEXT rt,bool long_name)
 {
  LRESULT ret = GKH_RET_ERROR;
  if(check_data_size(rt,sizeof(*rt)))
  {
   ret = mdb.get_name(id,rt,long_name);
  }
  return  ret;
 }

 LRESULT __fastcall TMediumDB::set_name(DWORD id,LPMDB_RECORD_TEXT rt,bool long_name)
 {
  LRESULT ret = GKH_RET_ERROR;
  if(check_data_size(rt,sizeof(*rt)))
  {
   ret = mdb.set_name(id,rt,long_name);
  }
  return  ret;
 }

 DWORD   __fastcall TMediumDB::on_exception  (lpexcept_record er)
 {
  mdb.full_unlock();
  full_unlock();
  input_queue.full_unlock();

  return  TGKModule::on_exception(er);
 }

 DWORD __fastcall TMediumDB::scale_calc(DWORD scale_id,DWORD  & kvants,float & result ,bool direct)
 {
   SC_CALC   cp;
   cp.id  = scale_id;
   cp.kvant  = kvants;
   cp.result = result;
   cp.error  = -1;
   scale.call(CM_SC_CALC,(LPARAM)&cp,direct ? TRUE:FALSE);
   result = cp.result;
   kvants = cp.kvant;
   return cp.error;
 }

 void       __fastcall TMediumDB::mdb_report(DWORD rep_id,DWORD rep_type,wchar_t * rep_text,LPVOID data ,DWORD dtsz )
 {
   LPBYTE buff = new BYTE[sizeof(TReportMes)+dtsz];
   if(buff)
   {
    TReportMes &mes = *((TReportMes*)buff);
    mes.log    = rep_id;
    mes.desc   = rep_text;
    mes.source = _handle;
    mes.type   = rep_type;
    mes.data_size = dtsz;
    if(dtsz)
     memcpy(mes.data,data,dtsz);
    report(RCM_MESSAGE_EVENT,(LPARAM)buff,0);
    delete [] buff;
   }
 }

 LRESULT __fastcall TMediumDB::add_record_bymask(DWORD addr_mask)
 {
  if(is_running())
  {
   sotd_addr addr(addr_mask);
   mdb.set_add_addr_mask(addr);
   if(addr_mask)
     send_call(addr,-1);
   return GKH_RET_SUCCESS;
  }
  return GKH_RET_ERROR;
 }

DWORD       __fastcall TMediumDB::mproto_send(int fa,MODEM_ADDR & to,DWORD cmd,DWORD err,LPVOID data,DWORD data_size,bool begin,bool finish )
{
 DWORD ret = 0;
 BYTE   buffer[16384];
 LPMPROTO_HEADER     mph;
 LPOTDM_PROTO_HEADER omph;
 DWORD  need_size = sizeof(MPROTO_HEADER)-sizeof(mph->data[0])+sizeof(*omph)-sizeof(omph->data[0])+data_size;
 if(need_size<sizeof(buffer))
   mph = (LPMPROTO_HEADER) buffer;
  else
   mph = (LPMPROTO_HEADER) new BYTE[need_size];
 if(mph)
 {
 ZeroMemory(mph,sizeof(*mph));
 mph->addr_to.addr = to.addr;
 mph->data_size = need_size+(sizeof(mph->data)-sizeof(*mph));
 mph->fa = fa;
  if(to.line == MODEM_ADDR_MAXVALUE && (cmd&OTDM_COMMAND_MASK) == OTDMPROTO_CMD_ENUM_KADRS)
    mph->flags|=MPROTO_FLAG_SEND_BROADCAST;
 omph =(LPOTDM_PROTO_HEADER) mph->data;
 omph->command    = cmd&OTDM_COMMAND_MASK;
 omph->command   |= OTDM_RESPOND;
 if(begin)
   omph->command  |= OTDM_BEGINDATA;
 if(finish)
  omph->command  |= OTDM_ENDDATA;
  if(err)
  omph->command  |= OTDM_ERROR;
  omph->error     = err;
  omph->data_size = data_size;
  memcpy(omph->data,data,data_size);

  ret = send(mph);
  if( buffer!=(LPBYTE)mph)
      delete  [](LPBYTE)mph;
  }
  return ret;
}









