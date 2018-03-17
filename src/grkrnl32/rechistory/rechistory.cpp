#ifndef BOOST_THREAD_USE_LIB
#define BOOST_THREAD_USE_LIB
#endif

#pragma hdrstop
#pragma argsused
#include "rechistory.hpp"
#include <TREGSTRY.hpp>
#include <functional>
#include <algorithm>
#include <otd_arch_proto.h>
#include <alloc.h>
#include <boost/bind.hpp>
#include <kfname.hpp>
#include <modemmer.h>
#include <boost/scoped_ptr.hpp>


static THeap heap(TSysInfo().GetPageSize()<<8); // 512 Кб
static DWORD total_mem_alloc = 0;

void * operator new(size_t sz)
{
 void * ptr = NULL;
 if(heap.IsValid())
 {
  ptr =  heap.Alloc(sz);
 }
 else
 ptr = malloc(sz);
 return ptr;
}

void * operator new [](size_t sz)
{
 void * ptr = NULL;

 if(heap.IsValid())
 {
  ptr = heap.Alloc(sz);
 }
 else
 ptr = malloc(sz);
 return ptr;
}

void operator delete( void * ptr)
{
  if(ptr && heap.Validate(ptr))
    {
     heap.Free(ptr);
    }
    else
    {
    if(ptr && _USEDENTRY == heapchecknode(ptr)) free(ptr);
    }
}

void operator delete[]( void * ptr)
{

  if(ptr && heap.Validate(ptr))
    {
     heap.Free(ptr);
    }
   else
   {
    if(ptr && _USEDENTRY == heapchecknode(ptr)) free(ptr);
   }
}

DWORD   __fastcall TRecHistory::get_mem_used()
 {
  return heap.IsValid() ? heap.GetMemoryUsed() : GKH_RET_ERROR;
  //total_mem_alloc;

  /*heapinfo hi;
  heapwalk(&hi);
  return hi.in_use;*/
 }

//extern "C" int _libmain(unsigned long reason)
//{
//	return 1;
//}

extern "C" void tss_cleanup_implemented(void)
{
 //stub only for boost::tthread
}


#pragma warn -8057

LRESULT WINAPI module_main(DWORD cmd,LPARAM p1,LPARAM p2)
{
	   static module_starter<TRecHistory> ms(MT_MEDIUMDB_HISTORY);
	   if(cmd==GKME_CMD_UNLOAD_MODULE) return 1;
       return ms(cmd,p1,p2);
}
#pragma warn .8057

DWORD   __fastcall TRecHistory::get_window_module  (wchar_t * buf,DWORD bsz)
{
   wchar_t * wnd_name = RECHISTORY_WNDMOD_NAME;
   DWORD ret = wcslen(wnd_name);
   safe_strcpyn(buf,wnd_name,bsz);
   return  ret;
}

char * __fastcall TRecHistory::new_str(const char * src)
{
 int len = src  ? strlen(src)+1 : 0;
 char * res = NULL;
 if(len)
 {
   res = new char[len];
   strcpy(res,src);
 }
 return res;
}

void __fastcall TRecHistory::replace_str(char ** dst,const char * src)
{
 if(*dst) delete [] *dst;
   *dst = new_str(src);
}


 TRecHistory::TRecHistory():term_event(true,false)
 {
   lock_param = GKHB_AUTO_LOCK_OFF;
   memset(&config,0,sizeof(config));
   config.dw_size = sizeof(config);
   alloc_gkhandle(RECHISTORY_MOD_NAME);
   rep_id = report_reg_event_type(L"Запись истории");
//   wr_trans       = NULL;
//   rd_trans       = NULL;
//   wr_query_recs  = NULL;
//   wr_ins_records = NULL;
//   wr_ins_string  = NULL;
//   wr_query_arch  = NULL;
//   rd_query       = NULL;
 }

 TRecHistory::~TRecHistory()
 {
 }

 DWORD   __fastcall TRecHistory::get_config_data(DWORD mask,LPVOID buf,DWORD bsz)
 {
   DWORD ret = GKH_RET_ERROR;
   LPREC_HIST_CONFIG    rc  = (LPREC_HIST_CONFIG)buf;
    if(check_data_size(rc,sizeof(*rc)))
      {
       guarder l (locker);
       if(mask&RCH_CONF_CLIENTLIB)
          wcsncpy(rc->client_lib,config.client_lib,sizeof(rc->client_lib)/sizeof(*rc->client_lib));

       if(mask&RCH_CONF_DBPATH   )
          wcsncpy(rc->dbpath,config.dbpath,sizeof(rc->dbpath)/sizeof(*rc->dbpath));
       if(mask&RCH_CONF_USERNAME )
          wcsncpy(rc->user_name,config.user_name,sizeof(rc->user_name)/sizeof(*rc->user_name));
       if(mask&RCH_CONF_USERPASSW)
          wcsncpy(rc->password,config.password,sizeof(rc->password)/sizeof(*rc->password));
       if(mask&RCH_CONF_WR_CHANGES)
         rc->write_changes = config.write_changes;

       if(mask&RCH_CONF_WR_ALARM_ARCHIVES)
         rc->write_alarm_archives = config.write_alarm_archives;

       if(mask&RCH_CONF_SQL_WRHISTROY)
          strcpy(rc->sql_wr_history,config.sql_wr_history);
       if(mask&RCH_CONF_SQL_RDHISTROY)
          strcpy(rc->sql_rd_history,config.sql_rd_history);

       if(mask&RCH_CONF_SQL_CLEAR_HIST)
          strcpy(rc->sql_clear_hist,config.sql_clear_hist);
       if(mask&RCH_CONF_SQL_UPD_IDX)
          strcpy(rc->sql_idx_update,config.sql_idx_update);

       if(mask&RCH_CONF_COMMIT_TIMEOUT)
          rc->commit_time_out = config.commit_time_out;
       if(mask&RCH_CONF_COMMIT_AFTER)
          rc->commit_after = config.commit_after;
       if(mask&RCH_CONF_TIMESTAMP_LIMIT)
          rc->timestamp_limit = config.timestamp_limit;
       ret = GKH_RET_SUCCESS;
      }

    return ret;
 }

 bool    __fastcall TRecHistory::set_config_data(DWORD mask,LPVOID buf,DWORD bsz)
 {
   bool ret = false;
   LPREC_HIST_CONFIG cd = (LPREC_HIST_CONFIG)buf;
   if(cd && cd->dw_size == sizeof(config))
   {
       ret = true;
       guarder l(locker);

       if(mask& RCH_CONF_CLIENTLIB )
         wcscpy(config.client_lib,cd->client_lib);
       if(mask& RCH_CONF_DBPATH    )
         wcscpy(config.dbpath,cd->dbpath);
       if(mask& RCH_CONF_USERNAME  )
         wcscpy(config.user_name,cd->user_name);
       if(mask& RCH_CONF_USERPASSW )
         wcscpy(config.password,cd->password);
       if(mask&RCH_CONF_WR_CHANGES)
          config.write_changes = cd->write_changes;
       if(mask&RCH_CONF_WR_ALARM_ARCHIVES)
          config.write_alarm_archives = cd->write_alarm_archives;

       if(mask&RCH_CONF_COMMIT_TIMEOUT)
           config.commit_time_out = cd->commit_time_out;
       if(mask&RCH_CONF_COMMIT_AFTER)
          config.commit_after = cd->commit_after ;

       if(ret &&  (mask&RCH_CONF_SQL_WRHISTROY))
       {
          ret = (GKH_RET_SUCCESS == check_query(cd->sql_wr_history));
          if(ret)
            {

             strcpy(config.sql_wr_history,cd->sql_wr_history);
             if(wr_query_recs.is_prepared())
                wr_query_recs.close(true);
            }
       }

       if(ret &&  (mask&RCH_CONF_SQL_RDHISTROY))
       {
          ret = (GKH_RET_SUCCESS == check_query(cd->sql_rd_history));
          if(ret)
          {
           strcpy(config.sql_rd_history,cd->sql_rd_history);
           if(rd_query.is_prepared())
              rd_query.close(true);
          }
       }
       if(ret &&  (mask&RCH_CONF_SQL_CLEAR_HIST))
          {
           ret = (GKH_RET_SUCCESS == check_query(cd->sql_clear_hist));
           if(ret)
             strcpy(config.sql_clear_hist,cd->sql_clear_hist);
          }

       if(ret &&  (mask&RCH_CONF_SQL_UPD_IDX))
       {
          ret = (GKH_RET_SUCCESS == check_query(cd->sql_idx_update));
          if(ret)
            strcpy(config.sql_idx_update,cd->sql_idx_update);
       }

     if(mask&(RCH_CONF_WR_CHANGES|RCH_CONF_WR_ALARM_ARCHIVES))
          send_order(true);
     if(mask&RCH_CONF_TIMESTAMP_LIMIT)
          config.timestamp_limit = cd->timestamp_limit;

   }

   return ret;

 }

 bool    __fastcall TRecHistory::compare_config_data(DWORD mask,LPVOID buf,DWORD bsz,LPDWORD ch_mask,BOOL * restart)
 {
   bool ret = false;
   LPREC_HIST_CONFIG cd = (LPREC_HIST_CONFIG)buf;
   if(cd && cd->dw_size == sizeof(config))
   {
       ret = true;
       guarder l(locker);
       if((mask& RCH_CONF_CLIENTLIB) && _wcsicmp(config.client_lib,cd->client_lib))
         *restart = TRUE;
         else
         mask&=~RCH_CONF_CLIENTLIB;
       if((mask& RCH_CONF_DBPATH) && _wcsicmp(config.dbpath,cd->dbpath) )
          *restart = TRUE;
          else
          mask&=~RCH_CONF_DBPATH;
       if((mask& RCH_CONF_USERNAME)  && _wcsicmp(config.user_name,cd->user_name) )
         *restart = TRUE;
         else
         mask&= ~RCH_CONF_USERNAME;

       if((mask& RCH_CONF_USERPASSW) && _wcsicmp(config.password,cd->password))
         *restart = TRUE;
         else
         mask&= ~RCH_CONF_USERPASSW;

       if( (mask&RCH_CONF_SQL_WRHISTROY) && !stricmp(config.sql_wr_history,cd->sql_wr_history))
            mask &= ~RCH_CONF_SQL_WRHISTROY;
       if( (mask&RCH_CONF_SQL_RDHISTROY) && !stricmp(config.sql_rd_history,cd->sql_rd_history))
           mask &= ~RCH_CONF_SQL_RDHISTROY;

       if((mask&RCH_CONF_COMMIT_TIMEOUT) && config.commit_time_out == cd->commit_time_out)
           mask&= ~RCH_CONF_COMMIT_TIMEOUT;
       if( (mask&RCH_CONF_COMMIT_AFTER) && config.commit_after == cd->commit_after)
            mask&= ~RCH_CONF_COMMIT_AFTER;
       if((mask & RCH_CONF_TIMESTAMP_LIMIT) && config.timestamp_limit == cd->timestamp_limit)
           mask &= ~RCH_CONF_TIMESTAMP_LIMIT;

      if(ch_mask) *ch_mask = mask;
      if(*restart && this->is_running())
         ret = check_data_base(cd);

   }


   return ret;
 }

bool    __fastcall TRecHistory::check_data_base       (LPREC_HIST_CONFIG cfg)
{
  TIBDatabase db;
  TIBTransaction rd_tr;
  TIBSqlQuery    rdq;
  bool   ret = open_data_base(db,*cfg,NULL,&rd_tr,NULL,&rdq);
  if(ret)
  {
   ret = false;
   rd_tr.enable_exception(FALSE);
   rdq.enable_exception(FALSE);
   if(rd_tr.start())
   {
    if(rdq.prepare("select count (*) from rdb$relations r where r.rdb$relation_name = upper(:tbl_name)"))
    {
      rdq.set_named_param("tbl_name","rechistory",TIBField());
      if(rdq.open())
      {
       if(!rdq[0].get_int(0))
         SetLastError(REC_HIST_ERROR_NO_TABLES);
         else
         ret = true;

      }
     rd_tr.commit(endtr_full);
    }
    else
    SetLastError(REC_HIST_ERROR_SQL_QUERY);
   }
   else
   SetLastError(REC_HIST_ERROR_START_TRANSACTION);
   db.Disconnect();
  }
  return ret;

}

void __fastcall TRecHistory::init_trans_query(TIBDatabase * ibdb,TIBTransaction * trans,TIBSqlQuery * query,bool write)
{
  if(trans)
  {
    trans->enable_exception(FALSE);
    if(write)
    {
     BYTE tr_wr_wait [] = {isc_tpb_version3,isc_tpb_read_committed,isc_tpb_rec_version,isc_tpb_write,isc_tpb_wait};
     TIBParams wr_tp(tr_wr_wait,sizeof(tr_wr_wait));
     trans->set_params      (wr_tp);
    }
    else
    {
     BYTE tr_rdp     [] = {isc_tpb_version3,isc_tpb_read ,isc_tpb_read_committed,/*isc_tpb_no_rec_version*/isc_tpb_rec_version,isc_tpb_nowait};
     TIBParams rd_tp(tr_rdp    ,sizeof(tr_rdp));
     trans->set_params  (rd_tp);
    }
   trans->set_database(ibdb);
   if(query)
      {
       query->enable_exception(FALSE);
       query->set_transaction(*trans);
      }
  }
}

bool __fastcall TRecHistory::end_transaction(TIBTransaction * tr,bool commit)
{
 bool ret = false;
  if(tr && tr->is_active())
    {
     DWORD tr_id = tr->get_tr_id();
     if(commit)
       {
        ret = tr->commit(endtr_full);
        if(ret)
        notify(RECHISTORY_NOTIFY_COMMIT,true,&tr_id,sizeof(tr_id));
       }
       else
       {
        ret = tr->rollback(endtr_full);
        if(ret)
           notify(RECHISTORY_NOTIFY_COMMIT,false,&tr_id,sizeof(tr_id));
       }


     if(!ret)
        report_sql_error(tr);
    }
  return ret;
}
bool __fastcall TRecHistory::open_data_base(TIBDatabase & ibdb,REC_HIST_CONFIG & config
                                            ,TIBTransaction * wr_trans,TIBTransaction * rd_trans
                                            ,TIBSqlQuery * wr_query,TIBSqlQuery * rd_query)
{
      char client_lib[MAX_PATH];
      char dbpath    [MAX_PATH<<1];
      char user_name [MAX_PATH];
      char user_passw[MAX_PATH];
     {

      Unicode2Ansi(client_lib,config.client_lib);
      Unicode2Ansi(dbpath,config.dbpath);
      Unicode2Ansi(user_name,config.user_name);
      Unicode2Ansi(user_passw,config.password);
     }
      if(TIBBase::is_clib_loaded() || TIBBase::init_ib_lib(client_lib))
       {
        ibdb.enable_exception(FALSE);
        if(ibdb.Connect(dbpath,user_name,user_passw,NULL,"WIN1251",3))
        {
            init_trans_query(&ibdb,wr_trans,wr_query,true);
            init_trans_query(&ibdb,rd_trans,rd_query,true);
        }
        else
        SetLastError(REC_HIST_ERROR_CONNECT_DATABASE);

       }
       else
       SetLastError(REC_HIST_ERROR_LOAD_CLIENTLIB);
   if(ibdb.is_connected())
      notify(RECHISTORY_NOTIFY_OPENDB,TRUE,0,0);
      else
      notify(RECHISTORY_NOTIFY_OPENDB,FALSE,0,0);

   return ibdb.is_connected();
}

 BOOL    __fastcall TRecHistory::can_start(DWORD reason,LPARAM p2)
 {
   BOOL ret = TGKModule::can_start(reason,p2) ;
   if(ret && reason == MODULE_START_REQUEST)
      ret = check_data_base(&config);
   return ret;
 }

 DWORD   __fastcall TRecHistory::start(DWORD reason,LPARAM p2)
 {
    DWORD ret = GKH_RET_ERROR;
    modemmer.open(MODEMMER_HANDLE_NAME_W);
    int_line = modemmer.call(MDMCM_REG_INTERNAL_MLINE,LPARAM(_handle),-1);
    pkt_num = 0;

    REC_HIST_CONFIG cfg;
    {
     guarder l(locker);
     memcpy(&cfg,&config,sizeof(cfg));
    }

    if(int_line>=0)
    {
        modemmer.call(MDMCM_REGISTER_HANDLER,(LPARAM)(this->_handle),FA_OTD_MEDIUM_ALARM_ARCHIVE);
        term_event.SetEvent(false);
        main_thread = boost::thread(&TRecHistory::write_sql_thread ,this);
        maintance_thread  = boost::thread(&TRecHistory::maintance_thread_proc,this);
        maintance_thread.detach();
        ret = TGKModule::start(reason,p2);
    }
   return ret;
 }

 DWORD   __fastcall TRecHistory::stop (DWORD reason)
 {
   DWORD ret;
   send_order(false);
   term_event.SetEvent(true);
   maintance_icp.PostStatus(MAINTANCE_TERM_KEY,1,NULL);
   main_thread.join();
   maintance_thread.join();
   TIBBase::init_ib_lib(NULL);
   modemmer.call(MDMCM_REG_INTERNAL_MLINE,LPARAM(_handle),int_line);
   modemmer.call(MDMCM_UNREGISTER_HANDLER,(LPARAM)(this->_handle),FA_OTD_MEDIUM_ALARM_ARCHIVE);
   int_line = -1;
   modemmer.close();
   Sleep(1000);
   close_cached_db();
   ret = TGKModule::stop(reason);
   return ret;
 }

 void __fastcall TRecHistory::close_cached_db()
 {
   guarder l(dbc_locker);
   while(this->db_cached.size())
   {
     TIBDatabase * db = db_cached.top();
     db_cached.pop();
     db->Disconnect();
     delete db;
   }
 }

 LRESULT __fastcall TRecHistory::processing(DWORD cmd,LPARAM p1,LPARAM p2)
 {
   LRESULT ret = GKH_RET_ERROR;
   if(MT_IS_MODEM(MCMD_TYPE(cmd)))
     {
      switch(cmd&MCMD(MT_MODEMMER,0xFFFF))
      {
       case MDMCM_SEND     :
            ret =  GKH_RET_SUCCESS;
       case MDMCM_FA_HANDLE: ret = GKH_RET_SUCCESS;
             {
              LPMPROTO_HEADER mph = (LPMPROTO_HEADER)p1;
              if(mproto_check(mph))
                 {
                    if(handle_query_archive(mph))// Не запрос архива
                       {
                        data_queue.PutIntoQueue(mph,mproto_size(mph));
                        if(data_queue.QueueCount()>1000)
                           data_queue.GetEvent().SetEvent(true);
                       }
                 }
             }
             break;
       default:
             break;
      }
     }
     else
     {
       switch(cmd)
       {
         case  RHCM_RESTORE_DATABASE: ret = restore_database((LPRECHIST_RESTORE)p1,(char *)p2) ;
         break;
         case RHCM_START_MAINTANCE:
              maintance_icp.PostStatus(MAINTANCE_START_KEY,1,NULL);
              ret = GKH_RET_SUCCESS;
         break;
         case RHCM_GET_STATISTICS:
              statistic_thread = boost::thread(&TRecHistory::statistic_thread_proc ,this);
              ret = GKH_RET_SUCCESS;
         break;
         default : ret = TGKModule::processing(cmd,p1,p2);
       }
   }
  return ret;
 }

void   __fastcall coder(wchar_t * str,int len)
{
 #pragma pack (push,1)
  union wrd
  {
    struct {BYTE a,b;} bt ;
    WORD   w;
  } ;
  #pragma pack(pop);

  wrd * ptr = (wrd*)str;
  wrd * ptr_end = ptr+len;
  while(ptr<ptr_end)
  {
    std::swap(ptr->bt.a,ptr->bt.b);
    ptr->w = ~ptr->w;
    ++ptr;
  }
}

bool    __fastcall TRecHistory::write_settings()
{
   bool ret = false;
   ret = TGKModule::write_settings();
   if(ret)
   {
     TRegsWriter rw(DupKey(reg_key));
     ret = rw.IsRootOnly();
     if(ret)
     {
       rw.WriteDword(REGDW_MOD_VERSION,RECHIST_MODULE_VERSION,true);
       guarder l(locker);
       rw.WriteString(REGSTR_CLIENT_LIB,config.client_lib,true);
       rw.WriteString(REGSTR_DBPATH,config.dbpath,true);

       wchar_t buf[KERTL_ARRAY_COUNT(config.user_name)];
       wcsncpy(buf,config.user_name,KERTL_ARRAY_COUNT(config.user_name));
       int len = wcslen(buf);
       coder(buf,len);
       rw.WriteBytes(REGSTR_USER,buf,len*sizeof(wchar_t),true);
       wcsncpy(buf,config.password,KERTL_ARRAY_COUNT(config.user_name));
       len = wcslen(buf);
       coder(buf,len);
       rw.WriteBytes(REGSTR_PASSW,buf,len*sizeof(wchar_t),true);

       rw.WriteDword(REGDW_WRITE_ALARM_ARCHIVES,config.write_alarm_archives,true);
       rw.WriteDword(REGDW_WRITE_CHANGES       ,config.write_changes,true);
       rw.WriteBytes(REGSTR_WRITE_QUERY,config.sql_wr_history,strlen(config.sql_wr_history)+1,true);
       rw.WriteBytes(REGSTR_READ_QUERY ,config.sql_rd_history,strlen(config.sql_rd_history)+1,true);
       rw.WriteBytes(REGSTR_CLEAR_HIST ,config.sql_clear_hist,strlen(config.sql_clear_hist)+1,true);
       rw.WriteBytes(REGSTR_IDX_UPDATE ,config.sql_idx_update,strlen(config.sql_idx_update)+1,true);

       rw.WriteDword(REGDW_COMMIT_TIMEOUT,config.commit_time_out,true);
       rw.WriteDword(REGDW_COMMIT_AFTER,config.commit_after,true);
       rw.WriteBytes(REGBIN_TIMESTAMP_LIMIT,&config.timestamp_limit,sizeof(config.timestamp_limit),true);
     }
   }
  return ret;
}

bool   __fastcall TRecHistory::read_settings()
{
   bool ret = false;
   ret = TGKModule::read_settings();
   if(ret)
   {
     TRegsReader rd(DupKey(reg_key));
     ret = rd.IsRootOnly();
     if(ret)
     {
       guarder l(locker);
       DWORD rd_len;
       rd_len = rd.ReadString(REGSTR_CLIENT_LIB,config.client_lib,KERTL_ARRAY_COUNT(config.client_lib),true);
       config.client_lib[rd_len] = 0;
       rd_len = rd.ReadString(REGSTR_DBPATH,config.dbpath   ,KERTL_ARRAY_COUNT(config.dbpath    ),true);
       config.dbpath    [rd_len]=0;
       rd_len = 0;
       if(rd.GetValueType(REGSTR_USER,NULL,&rd_len,true))
       {
       ZeroMemory(config.user_name,sizeof(config.user_name ));
       rd_len = rd.ReadBytes(REGSTR_USER ,config.user_name,sizeof(config.user_name),true);
       coder(config.user_name,rd_len/sizeof(wchar_t));
       }

       if(rd.GetValueType(REGSTR_PASSW,NULL,&rd_len,true))
       {
       ZeroMemory(config.password,sizeof(config.password ));
       rd_len = rd.ReadBytes(REGSTR_PASSW ,config.password,sizeof(config.password) ,true);
       coder(config.password,rd_len/sizeof(wchar_t));
       }

       config.write_changes        = rd.ReadDword(REGDW_WRITE_CHANGES,1,true);
       config.write_alarm_archives = rd.ReadDword(REGDW_WRITE_ALARM_ARCHIVES,1,true);

       rd.ReadBytes(REGSTR_WRITE_QUERY,config.sql_wr_history,sizeof(config.sql_wr_history),true);
       rd.ReadBytes(REGSTR_READ_QUERY ,config.sql_rd_history,sizeof(config.sql_wr_history),true);
       rd.ReadBytes(REGSTR_CLEAR_HIST ,config.sql_clear_hist,sizeof(config.sql_clear_hist),true);
       rd.ReadBytes(REGSTR_IDX_UPDATE ,config.sql_idx_update,sizeof(config.sql_idx_update),true);

       config.commit_time_out = rd.ReadDword(REGDW_COMMIT_TIMEOUT,200 ,true);
       config.commit_after    = rd.ReadDword(REGDW_COMMIT_AFTER  ,30000,true);
       config.timestamp_limit = 0;
       rd.ReadBytes(REGBIN_TIMESTAMP_LIMIT,&config.timestamp_limit,sizeof(config.timestamp_limit),true);

       DWORD version = rd.ReadDword(REGDW_MOD_VERSION,0,true);

       if(version < RECHIST_MODULE_VERSION)
          config.sql_wr_history[0]  = config.sql_rd_history[0] = config.sql_clear_hist[0] = config.sql_idx_update[0]  = 0;


       if(!config.sql_wr_history[0] )
       {
        strcpy
        (
        config.sql_wr_history,
        "update or \n\r"
        "insert into RECHISTORY \n\r"
        "(REC_ID, DIAG, STATE, RVALUE, CH_TIME,alarm,KVANTS,OTDPD) \n\r"
        "values (:REC_ID, :DIAG, :STATE, :RVALUE, :CH_TIME,:alarm,:KVANTS,:OTDPD) \n\r"
        "matching (rec_id,ch_time,alarm,diag,state) \n\r"
         );
       }

       if(!config.sql_rd_history[0])
       {
        strcpy
        (
        config.sql_rd_history,
        "--alarm is null - выбор всего \n\r"
        "--alarm =  0 обычные \n\r"
        "--alarm =  1  аварийный архив \n\r"
        "execute block (lst blob = :lst,beg_tm bigint = :beg_tm, end_tm bigint = :end_tm,alarm integer = :alarm) \n\r"
        "returns \n\r"
        "(REC_ID INTEGER,DIAG SMALLINT,STATE SMALLINT,RVALUE BIGINT,CH_TIME BIGINT,is_alarm integer,KVANTS INTEGER,OTDPD INTEGER,wr_time timestamp) \n\r"
        "as begin \n\r"
        " diag  = 0; state = NULL; rvalue  = 0; ch_time = 0; \n\r"
        " for select l.id from usp_list_to_rows(:lst) l into :rec_id \n\r"
        "  do suspend; \n\r"
        "for \n\r"
        " select   r.rec_id ,r.diag ,r.state ,r.rvalue ,r.ch_time ,r.alarm ,r.kvants,r.otdpd,r.wr_time \n\r"
        "   from rechistory r \n\r"
        "   join usp_list_to_rows(:lst) l on l.id = r.rec_id and  r.ch_time between :beg_tm and :end_tm \n\r"
        "   and r.alarm = coalesce(:alarm,r.alarm) \n\r"
        " order by r.ch_time \n\r"
        " into :rec_id,:diag,:state,:rvalue,:ch_time,:is_alarm ,:kvants,:otdpd,:wr_time \n\r"
        " do \n\r"
        "  begin \n\r"
        "   suspend; \n\r"
        "  end \n\r"
        "end \n\r"
        );
       }
       if(!config.sql_clear_hist[0])
       {
        strcpy( config.sql_clear_hist,
            "execute block ( TM bigint = :TM) \n\r"
            "as \n\r"
            "begin \n\r"
            "  delete from rechistory rh where rh.ch_time< :tm and coalesce(rh.alarm,0) = 0; \n\r"
            "  delete from alarmshist ah where ah.kadr_id>0 and ah.alarm_start < :tm and coalesce(ah.protect,0) = 0; \n\r"
            "  for \n\r"
            "  select max(ah.alarm_end) from alarmshist ah where ah.kadr_id>0 and  ah.alarm_start < :tm into :tm \n\r"
            "  do \n\r"
            "  delete from rechistory rh where rh.ch_time < :tm and coalesce(rh.alarm,0) <> 0; \n\r"
            "end \n\r"
              );
       }

       if(!config.sql_idx_update[0])
       {
        strcpy( config.sql_idx_update,
                 " execute block \n\r"
                 "  as \n\r"
                 "  declare variable index_name varchar(128); \n\r"
                 "  begin \n\r"
                 "   for select ri.rdb$index_name from rdb$indices ri where ri.rdb$system_flag = 0 \n\r"
                 "   into :index_name \n\r"
                 "   do \n\r"
                 "   begin \n\r"
                 "    execute statement 'SET STATISTICS INDEX '||:index_name; \n\r"
                 "   end \n\r"
                 "  end \n\r"

              );
       }

       //REGBIN_ORDERS

     }
   }
  return ret;
}


DWORD       __fastcall TRecHistory::send_command(DWORD fa,bool begin,bool finish,DWORD cmd,DWORD err,LPVOID data,DWORD data_size )
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
 ZeroMemory(mph,need_size);

 mph->data_size = need_size+(sizeof(mph->data)-sizeof(*mph));
 mph->fa = fa;
 omph =(LPOTDM_PROTO_HEADER) mph->data;
 omph->command    = cmd&(OTDM_COMMAND_MASK|OTDM_ORDER);
 //omph->command   |= OTDM_RESPOND;
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

 void    __fastcall TRecHistory::send_order(bool order)
 {
  BYTE buffer[MAX_PATH];
  LPMPROTO_HEADER mph =(LPMPROTO_HEADER)buffer;
  LPOTDM_PROTO_HEADER omph;
  omph = (LPOTDM_PROTO_HEADER)mph->data;
  ZeroMemory(mph,sizeof(*mph)+sizeof(*omph));
  omph->command   = OTDMPROTO_CMD_GETKADR|OTDM_ORDER;
  mph->data_size  = sizeof(*omph)+sizeof(DWORD);
  omph->data[0]   = (DWORD)-1;
  omph->data_size = sizeof(DWORD)<<1;

  mph->fa = FA_OTD_MEDIUM;
  omph->data[1] = (order && config.write_changes ) ? -1 : 0;
  send(mph);


   mph->fa = FA_OTD_MEDIUM_ALARM_ARCHIVE;
   omph->data[1] = (order && config.write_alarm_archives) ? -1:0;
   send(mph);

 }

 LRESULT __fastcall TRecHistory::send(LPMPROTO_HEADER mph)
 {
    if(mph && mph->data_size)
    {
     mph->internal       = MP_INTERNAL_DEFAULT;
     mph->addr_from.addr = 0;
     mph->addr_from.reserv1 = -1;
     mph->addr_from.line = int_line;
     mph->pkt_num        = InterlockedIncrement(&pkt_num);
     mproto_protect(mph);
     return modemmer.call(MDMCM_SEND,(LPARAM)mph,MPROTO_SIZE(mph));
    }
   return  GKH_RET_ERROR;
 }

 LRESULT __fastcall TRecHistory::handle_otdm_proto(LPMPROTO_HEADER mph)
 {
   LRESULT ret =  GKH_RET_SUCCESS;
   if(mph->fa == FA_OTD_MEDIUM || mph->fa == FA_OTD_MEDIUM_ALARM_ARCHIVE)
   {

    LPOTDM_PROTO_HEADER omph = (LPOTDM_PROTO_HEADER)mph->data;

    if(!(omph->command&OTDM_ERROR))
    {
     bool is_end = OTMD_PROTO_ISEND(omph);
     switch(omph->command&OTDM_COMMAND_MASK)
     {
      case OTDMPROTO_CMD_GETKADR:
           if(!handle_get_kadr(omph,mph->fa == FA_OTD_MEDIUM_ALARM_ARCHIVE ? true : false,is_end))
            ret = GKH_RET_ERROR;
           break;

      case OTDMPROTO_CMD_RECORDS:
           if(!handle_records_changes(omph,mph->fa == FA_OTD_MEDIUM_ALARM_ARCHIVE ? true : false))
            ret = GKH_RET_ERROR;
           break;

      case OTDMPROTO_CMD_GETKADR_RECORDS :
           if(!handle_records(omph))
              ret = GKH_RET_ERROR;
           break;
    case OTDMPROTO_CMD_GET_RECORD_SNAME:
    case OTDMPROTO_CMD_GET_RECORD_FULL_SNAME:
         handle_record_names(omph,true);
    break;
    case OTDMPROTO_CMD_GET_RECORD_NAME :
    case OTDMPROTO_CMD_GET_RECORD_FULL_NAME:
         handle_record_names(omph,false);
    break;

     }
    }
   }
  return ret;
 }

 char q_write_names[] =
"execute block ( REC_ID integer = :REC_ID ,str_id integer = :str_id ,str varchar(4096) = :str, short_name integer = :short_name) \n"
"as \n"
"begin \n"
" short_name = coalesce(short_name,0); \n"
" update or insert into STRINGS (ID,TEXT) VALUES(:str_id,:str) matching (ID); \n"
" update records r \n"
" set \n"
"   r.short_name = IIF(:short_name <> 0,:str_id,r.short_name) \n"
"  ,r.long_name  = IIF(:short_name =  0,:str_id,r.long_name) \n"
"  where r.rec_id = :rec_id; \n"
"end \n"
 ;

 void   __fastcall TRecHistory::handle_record_names(LPOTDM_PROTO_HEADER omph,bool short_name)
 {
  LPOTDM_PROTO_RECORD_NAME rn = (LPOTDM_PROTO_RECORD_NAME) omph->data;
  wchar_t src_text[8192];
  char text[8192];
  memcpy(src_text,rn->text,sizeof(wchar_t)*rn->len);
  src_text[rn->len] = 0;
  Unicode2Ansi(text,sizeof(text),src_text);
  if(!wr_trans.is_active())
  {
   if(!wr_trans.start())
      report_sql_error(&wr_trans);
  }

  if(wr_trans.is_active())
  {
    if(wr_ins_string.prepare(q_write_names))
    {
      TIBField p;
      wr_ins_string.set_named_param("rec_id",rn->rec_id,p);
      wr_ins_string.set_named_param("str_id",rn->name_id,p);
      wr_ins_string.set_named_param("str"   ,text,p);
      wr_ins_string.set_named_param("short_name",short_name ? 1:0,p);
      wr_ins_string.open();
      wr_ins_string.close(false);

    }
    else
    report_sql_error(&wr_ins_string);
  }
  /*
  set_text(rn->name_id,name);
  Trecords::iterator rptr ;
  if(binary_search(records,*&mdb_record_ex(rn->rec_id),&rptr))
  {
   DWORD & name_id = short_name ? rptr->sname : rptr->name;
   if(name_id != rn->name_id)
      {
       name_id = rn->name_id;
       viewer_module->config_change(CCFL_RECORDS);
      }
  }
  */
 }


 void __fastcall TRecHistory::request_record_name  (DWORD rec_id, bool short_name)
 {
   if(short_name)
     send_command (FA_OTD_MEDIUM,true,true,OTDMPROTO_CMD_GET_RECORD_SNAME,0,&rec_id,sizeof(rec_id));
   else
     send_command (FA_OTD_MEDIUM,true,true,OTDMPROTO_CMD_GET_RECORD_FULL_NAME,0,&rec_id,sizeof(rec_id));
 }


 bool __fastcall TRecHistory::handle_records        (LPOTDM_PROTO_HEADER omph)
 {
     bool ret = true;
     LPOTDM_PROTO_KADR_RECORDS  krecs = (LPOTDM_PROTO_KADR_RECORDS)omph->data;
     guarder g(locker);
     for(DWORD i = 0;ret && i<krecs->rec_count;i++)
     {
       mdb_record rec(krecs->records[i]);
       DWORD mask = MDBR_VALUE_FIELDS;
      if(rec.name != DWORD(-1))     mask|= MDBR_FIELD_NAME;
      if(rec.sname!= DWORD(-1))     mask|= MDBR_FIELD_SNAME;
       ret = store_record(rec,-1,false,true);
     }
   return ret;
 }

 bool    __fastcall TRecHistory::handle_get_kadr       (LPOTDM_PROTO_HEADER omph,bool archive,bool end)
 {

   bool ret = true;
   LPOTDM_PROTO_KADR p_kadr;
   if(omph->data_size>=sizeof(*p_kadr))
   {
    p_kadr = (LPOTDM_PROTO_KADR )omph->data;
    if((p_kadr->mask & MDB_KADR_FIELDS_HIST_TIMES) && p_kadr->kadr.hist_beg_time)
    {
      bool alarm = true;
      ret = store_archive_times(p_kadr->kadr.kadr_id,alarm,p_kadr->kadr.hist_beg_time,p_kadr->kadr.hist_end_time);
    }
   }
  return ret;
 }


 bool __fastcall TRecHistory::handle_records_changes(LPOTDM_PROTO_HEADER omph,bool archive)
 {
    bool ret = true;
    LPOTDM_PROTO_RECORDS records = (LPOTDM_PROTO_RECORDS)omph->data;
    LPBYTE src_ptr = (LPBYTE)records->records;

       __int64 rest_time;
       for(DWORD i = 0;ret && i<records->rec_count;i++)
       {
        mdb_record rec;
        LPOTDM_PROTO_RECORD proto_rec = (LPOTDM_PROTO_RECORD) src_ptr;
        DWORD mask = proto_rec->mask;
        src_ptr+=sizeof(proto_rec->mask);
        src_ptr+= otdm_unpack_record(src_ptr,omph->data_size,mask,&rec);
        if(mask&MDBR_FIELD_TIME)
            rest_time = rec.time;
           else
           {
            if(rest_time)
            {
             mask|=MDBR_FIELD_TIME;
             rec.time = rest_time;
            }
            //else
            //DebugBreak();
           }
        ret = store_record(rec,mask,archive);
      }
     return ret;
 }


 char  q_ins_record [] =
"execute block ( REC_ID integer = :REC_ID ,short_name integer = :short_name, long_name integer = :long_name) \n"
"as \n"
"begin \n"
"  if(short_name  is not null and not exists (select * from strings s where s.id = :short_name)) then \n"
"      insert into strings(id) values(:short_name); \n"
"  if(long_name  is not null and not exists (select * from strings s where s.id = :long_name)) then \n"
"      insert into strings(id) values(:long_name); \n"
"   update or insert into records (REC_ID,SHORT_NAME,LONG_NAME) values(:rec_id,:short_name,:long_name) matching(REC_ID); \n"
"end \n"
;

 bool    __fastcall TRecHistory::store_record          (mdb_record & rec,DWORD mask,bool alarm,bool upd_rec_table)
 {
  bool ret = false;

  if(mask& MDBR_FIELD_NAME)
     request_record_name(rec.rec_id,false);
  if(mask& MDBR_FIELD_SNAME)
     request_record_name(rec.rec_id,true);

  //Сохраняем только изменения значения или состояния, или диагностики
  ret = wr_trans.is_active();
  if(!ret)
      {
       ret = wr_trans.start();
       if(ret)
       {
        guarder l(locker);
        if(!wr_query_recs.is_prepared(config.sql_wr_history) && !wr_query_recs.prepare(config.sql_wr_history))
            {
             report_sql_error(&wr_query_recs);
             ret = false;
            }

       }
       else
       report_sql_error(&wr_trans);
      }
  if(ret)
  {
  if(upd_rec_table)
  {
    if(wr_ins_records.prepare(q_ins_record))
    {
      TIBField p;
      wr_ins_records.clear_input();
      wr_ins_records.set_named_param("rec_id",rec.rec_id,p);
      if(rec.sname!=(DWORD)-1)
         wr_ins_records.set_named_param("short_name",rec.sname,p);
      if(rec.name!=(DWORD)-1)
         wr_ins_records.set_named_param("long_name",rec.name,p);
      wr_ins_records.open();
      wr_ins_records.close(false);
    }
  }

  if(mask & (MDBR_FIELD_VALUE|MDBR_FIELD_DIAG|MDBR_FIELD_STATE|MDBR_FIELD_KVANTS) )
  {
   if(wr_query_recs.is_prepared())
   {
     wr_query_recs.clear_input();
     TIBField p;
     wr_query_recs.set_named_param("REC_ID",rec.rec_id,p);

     if(mask & MDBR_FIELD_VALUE)
        {
         wr_query_recs.set_named_param("RVALUE",__int64(rec.value*FLOAT_MULTYPLIER),p);

        }
      if(mask & MDBR_FIELD_OTDVALUE)
      {
       LONGLONG val ;
       if(rec.options & MDBR_OPT_DOUBLE_TYPE)
          val = 100.0*rec.otd_val_dbl;
       else
       {
       if(rec.options & MDBR_OPT_FLOAT_TYPE)
          val = 100.0*rec.otd_val_flt;
          else
          val = 100*rec.otd_val;
       }
       wr_query_recs.set_named_param("KVANTS",val,p);
      }

     if(mask & MDBR_FIELD_OTDPD)
      {
       wr_query_recs.set_named_param("OTDPD",__int32(rec.otd_pd),p);
      }

     if(mask & MDBR_FIELD_DIAG)
         wr_query_recs.set_named_param("DIAG",rec.diag,p);

     if(mask & MDBR_FIELD_STATE)
       wr_query_recs.set_named_param("STATE",rec.state,p);
     wr_query_recs.set_named_param("CH_TIME",(mask& MDBR_FIELD_TIME) ? rec.time : KeRTL::GetTime(),p);
     wr_query_recs.set_named_param("alarm"  ,alarm,p);
     if(wr_query_recs.open())
     {
      notify(RECHISTORY_NOTIFY_STORE,mask,&rec,sizeof(rec));
      wr_query_recs.close(false);
      ret = true;
     }
     else
     {
      report_sql_error(&wr_query_recs);
     }
   }
  }
  }
  return ret;
 }

 bool    __fastcall TRecHistory::store_archive_times(DWORD kadr_id,bool alarm,__int64 beg_tm,__int64 end_tm)
 {
   char * sql_arch_text =
   "update or insert into ALARMSHIST (KADR_ID, ALARM_START, ALARM_END) \n\r"
   "values (:KADR_ID, :ALARM_START, :ALARM_END ) \n\r"
   "matching(kadr_id,alarm_start) \n\r";
   ;
   bool ret = true;
   if(!wr_trans.is_active())
      ret = wr_trans.start();
   if(ret)
   {
     ret = wr_query_arch.prepare(sql_arch_text,&wr_trans);
     if(ret)
     {
       TIBField p;
       wr_query_arch.set_named_param("kadr_id"    ,kadr_id,p);
       wr_query_arch.set_named_param("alarm"      ,alarm  ,p);
       wr_query_arch.set_named_param("ALARM_START",beg_tm ,p);
       wr_query_arch.set_named_param("ALARM_END"  ,end_tm ,p);
       ret = wr_query_arch.open();
       if(ret)
          wr_query_arch.close(true);
     }
     if(!ret)
         report_sql_error(&wr_query_arch);

   }
   else
   report_sql_error(&wr_trans);
   return ret;
 }

 void __fastcall TRecHistory::report_sql_error(TIBStatus * ibs)
 {
      string error_text = "Ошибка SQL ";
      error_text  +=  ibs->get_ib_class_name();
      error_text  += " : ";
      error_text += ibs->get_error_text();
      report(this->rep_id,REPORT_ERROR_TYPE,(char*)error_text.c_str());
}

 LRESULT __fastcall TRecHistory::check_query(char * query)
 {
  LRESULT ret = GKH_RET_ERROR;
  TIBDatabase db;
  TIBTransaction tr;
  if(open_data_base(db,config,NULL,&tr,NULL,NULL))
    {
     ret = check_query(&tr,query);
    }
  return ret;
 }

 LRESULT __fastcall TRecHistory::check_query(TIBTransaction * tr,char * query)
 {
  LRESULT ret = GKH_RET_ERROR;
  if(tr && tr->start())
  {
    TIBSqlQuery q(tr);
    q.enable_exception(false);
    if(q.prepare(query))
      ret = GKH_RET_SUCCESS;
      else
      SetLastError(REC_HIST_ERROR_PREPARE_QUERY);
      tr->commit();

  }
  else
  SetLastError(REC_HIST_ERROR_START_TRANSACTION);
  return ret;
 }

int            TRecHistory::maintance_thread_proc    ()
{
    /*
    Процедура обслуживания производится 3 часа ночи
    */

       bool updated = false;
       SYSTEMTIME st;
       LPOVERLAPPED ovrlp = NULL;
       DWORD icp_key = -1,io_bytes = 0;
    do{
       if(icp_key)
       {
        GetLocalTime(&st);
        if(icp_key == MAINTANCE_START_KEY || st.wHour == 3)
        {
           if(!updated)
           {
              updated = true;
              do_maintance_db();
           }
        }
        else
        updated = false;

        if(!maintance_icp.GetStatus(icp_key,ovrlp,1800000,&io_bytes))
           icp_key = -1;
       }
     }while(icp_key);


 return 0;
}

void __fastcall TRecHistory::do_maintance_db       ()
{
;
  int ret = 0;
  REC_HIST_CONFIG cfg;
  {
   guarder l(locker);
   memcpy(&cfg,&config,sizeof(cfg));
  }
  char * erase_old_data_text = cfg.sql_clear_hist;
  char * upd_query_text      = cfg.sql_idx_update;

  TIBDatabase db;
  TIBTransaction trans;
  TIBSqlQuery    query;
  report(rep_id,REPORT_INFORMATION_TYPE,L"Обслуживание - запуск");
  if(open_data_base(db,cfg,&trans,NULL,&query,NULL))
  {
   if(trans.start())
   {
    if(cfg.timestamp_limit>0 && * erase_old_data_text)
    {
     __int64 tm      = KeRTL::GetTime();
     __int64  tm_dec = MSEC_NS100(1000);
     tm_dec*= (3600*24)*cfg.timestamp_limit;
     tm -= tm_dec;

//      SYSTEMTIME st;
//      FileTimeToSystemTime((LPFILETIME)&tm,&st);
//      FileTimeToSystemTime((LPFILETIME)&tm,&st);

     report(rep_id,REPORT_INFORMATION_TYPE,L"Удаение старых записей- запуск");
     if(query.prepare(erase_old_data_text))
     {
        query.get_input_field(0) =  tm;
        query.open();
        trans.commit(endtr_retain);
     }
     report(rep_id,REPORT_INFORMATION_TYPE,L"Удаение старых записей- завершено");
    }

    if(*upd_query_text)
    {
     report(rep_id,REPORT_INFORMATION_TYPE,L"Обновление статистики индексов - запуск.");
     if(query.prepare(upd_query_text))
       query.open();
     query.close(true);
     end_transaction(&trans,true);
     notify(RECHISTORY_NOTIFY_UPDIDX,0,0,0);
     report(rep_id,REPORT_INFORMATION_TYPE,L"Обновление статистики индексов - завершено.");
    }

   }
   db.Disconnect();
  }
}

 int            TRecHistory::statistic_thread_proc ()
 {
 static char * stat_query_text =
    "execute block returns(  table_name varchar(64) ,rec_count bigint) \n\r"
    "as \n\r"
    "begin \n\r"
    " for  select r.rdb$relation_name from rdb$relations r where r.rdb$system_flag = 0 and r.rdb$relation_type = 0 \n\r"
    " into table_name \n\r"
    " do \n\r"
    " begin \n\r"
    "   execute statement    'select count(*) from '||:table_name \n\r"
    "   into :rec_count; \n\r"
    "   suspend; \n\r"
    " end \n\r"
    "end \n\r"
;
  REC_HIST_CONFIG cfg;
  {
   guarder l(locker);
   memcpy(&cfg,&config,sizeof(cfg));
  }

  TIBDatabase db;
  TIBTransaction trans;
  TIBSqlQuery    query;
  if(open_data_base(db,cfg,NULL,&trans,&query,NULL))
  {
   if(trans.start())
   {
    if(query.prepare(stat_query_text,&trans) && query.open() && !query.is_eos())
    {
     TIBField table_name = query[0];
     TIBField rec_count  = query[1];
     char text[128];
     int  len;
     len = snprintf(text,KERTL_ARRAY_COUNT(text),"*** %32s ***","Statistic begin");
     notify(RECHISTORY_NOTIFY_STATS,0,text,len);
     do
     {
      len = snprintf(text,KERTL_ARRAY_COUNT(text),"%-32s - %8ld",table_name.get_string().c_str(),rec_count.get_int64(0));
      notify(RECHISTORY_NOTIFY_STATS,0,text,len);
     }while(query.fetch());

    }
    end_transaction(&trans,true);
   }
  }

  return 0;
 }




int            TRecHistory::write_sql_thread        ()
{
  //Открыть базу
      int ret = 0;
      REC_HIST_CONFIG cfg;
      ZeroMemory(&cfg,sizeof(cfg));
      cfg.dw_size = sizeof(cfg);
      get_config_data(-1,&cfg,sizeof(cfg));

      SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_HIGHEST);

      KeRTL::TEvent _term_event(term_event.Dup(false));
      data_queue.DropData();
      DWORD buf_sz = 4096;
      std::unique_ptr< BYTE,array_deleter_t<BYTE> > bptr(new BYTE [buf_sz]);

      TSynchroSet ss;
      ss+= _term_event;
      ss+= data_queue.GetEvent();
      int time_count     = 2000;
      int cycle_count    = 0;
      int need_terminate = 0;

      while(!need_terminate)
      {
          switch(ss.Wait(100))
          {
            case 0: need_terminate = 1;break;
            case 1:
              do{
                int need_sz = 0;
                if(!data_queue.GetFromQueue(bptr.get(),buf_sz,&need_sz))
                  {
                    //delete [] bptr.release();
                    buf_sz = data_queue.TopDataSize();
                    buf_sz<<=1;
                    bptr.reset(new BYTE[buf_sz]);
                    data_queue.GetFromQueue(bptr.get(),buf_sz,&need_sz);
                  }

                  if(ibdb.is_connected())
                  {
                   if(handle_otdm_proto((LPMPROTO_HEADER)bptr.get()))
                   {
                     //Ошибка при записи в БД
                     data_queue.DropData();
                     ibdb.Disconnect();
                     report(rep_id,REPORT_ERROR_TYPE,"Ошибка работы с базой.Отключение.Очистка очереди");
                   }
                   else
                   {
                   if(++cycle_count>cfg.commit_after)
                      {
                       end_transaction(&wr_trans,true);
                       cycle_count = 0;
                       time_count  = 0;
                       get_config_data(RCH_CONF_COMMIT_TIMEOUT|RCH_CONF_COMMIT_AFTER,&cfg,sizeof(cfg));
                      }
                   }
                  }
                  else
                  data_queue.DropData();
                }while(data_queue.QueueCount() && WAIT_TIMEOUT == data_queue.GetEvent().Wait(0));
              break;
            case WAIT_TIMEOUT:
            time_count+=100;
            if(++time_count>cfg.commit_time_out)
            {
              get_config_data(RCH_CONF_COMMIT_TIMEOUT|RCH_CONF_COMMIT_AFTER,&cfg,sizeof(cfg));
              if(ibdb.is_connected())
               {
                end_transaction(&wr_trans,true);
                time_count  = 0;
                cycle_count = 0;
               }
               else
               {
                  /*Попытка подключения к базе*/
                   if(time_count>5000)
                   {
                       wchar_t rep_text[1024];
                       swprintf(rep_text,L"Подключение к базе данных %s",cfg.dbpath);
                       report(this->rep_id,REPORT_INFORMATION_TYPE,rep_text);
                       if(open_data_base(ibdb,cfg,&wr_trans,NULL,&wr_query_recs,NULL))
                          {
                           send_order(true);
                           report(this->rep_id,REPORT_INFORMATION_TYPE,L"ОК - Запрос кадров отправлен");
                           wr_ins_records.set_transaction(wr_trans);
                           wr_ins_string.set_transaction(wr_trans);
                          }
                          else
                          report(this->rep_id,REPORT_INFORMATION_TYPE,L"Ошибка прдключения к БД");
                          time_count  = 0;
                   }
               }
               cycle_count = 0;

            }
            break;
          }
      }
    SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_NORMAL);
    send_order(false);

    __try{
             wr_query_recs.close(true);
             if(wr_trans.is_active())
               wr_trans.commit(endtr_full);
             if(ibdb.is_connected())
             {
              ibdb.Disconnect() ;
             }
      }
      __finally{};
    return ret;
}


LRESULT __fastcall TRecHistory::handle_query_archive (LPMPROTO_HEADER mph)
{
 //Обработка запроса на получение истории
 LRESULT ret = GKH_RET_ERROR;

  if( mph->fa == FA_OTD_MEDIUM_ALARM_ARCHIVE)
    {
     LPOTDM_PROTO_HEADER omph = (LPOTDM_PROTO_HEADER)mph->data;
     if(!(omph->command&OTDM_RESPOND) )
         {
           switch(omph->command)
           {
            case OTDMPROTO_CMD_GETKADR:
            ret = GKH_RET_ERROR;
            break;
            case OTDMPROTO_CMD_QUERY_ARCHIVE :
            case OTDMPROTO_CMD_QUERY_KADR_ARCHIVES:

            {
           //TODO запустить нитку обработки запроса
            DWORD sz = mproto_size(mph);
            LPBYTE  ptr = new BYTE [sz];
            memcpy(ptr,mph,sz);
            boost::thread(boost::bind(&TRecHistory::read_archive_thread,this,omph->command,ptr)).detach();
            ret = GKH_RET_SUCCESS;
            }
            break;
           case OTDMPROTO_CMD_ARCHIVE_SET_PROPS  :
           {
           //TODO запустить нитку обработки запроса
            DWORD sz = mproto_size(mph);
            LPBYTE  ptr = new BYTE [sz];
            memcpy(ptr,mph,sz);
            boost::thread(boost::bind(&TRecHistory::update_archive_thread,this,omph->command,ptr)).detach();
            ret = GKH_RET_SUCCESS;
            }
            break;





            case OTDMPROTO_CMD_ARCHIVE_PRESENT:
            {
               ret = GKH_RET_SUCCESS;
               LPMPROTO_HEADER smp;
               char buffer[sizeof(*smp)+sizeof(*omph)+sizeof(OTDM_ARCHIVE_HEADER)];
               ZeroMemory(buffer,sizeof(buffer));
               smp = (LPMPROTO_HEADER)buffer;
               DWORD sz = sizeof(*omph)-sizeof(omph->data[0])+sizeof(OTDM_ARCHIVE_HEADER);
               mproto_init(smp,0,FA_OTD_MEDIUM_ALARM_ARCHIVE,sz,&mph->addr_from);
               LPOTDM_PROTO_HEADER somph = (LPOTDM_PROTO_HEADER )smp->data;
               memcpy(somph,omph,sizeof(*omph)-sizeof(omph->data[0]));
               somph->command|=OTDM_RESPOND;
               ((LPOTDM_ARCHIVE_HEADER)somph->data)->hist_id = ((LPOTDM_ARCHIVE_HEADER)omph->data)->hist_id;
               send(smp);
              }
            break;
           }
         }
    }
  return ret;
}


bool           TRecHistory::query_records_archive(TIBSqlQuery & query,LPOTDM_ARCHIVE_QUERY arch_query,char * sql_rd_history)
{
   bool ret =  query.prepare(sql_rd_history);
   if(ret)
   {
       string param_str;
       param_str.reserve(arch_query->rec_ids.count*10);
       for(DWORD i = 0;i<arch_query->rec_ids.count;i++)
         {
          char id[64];
          sprintf(id,"%s%u",i ? ",":"",arch_query->rec_ids.rec_id[i]);
          param_str+=id;
         }
       TIBField p;
       query.set_named_param("lst",param_str.c_str(),p);
       query.set_named_param("beg_tm",arch_query->period.beg_time,p);
       query.set_named_param("end_tm",arch_query->period.end_time,p);
       if(arch_query->alarm == (DWORD) -1)
          query.set_named_param("alarm",ib_null,p);
          else
          query.set_named_param("alarm",arch_query->alarm,p);
       ret = query.open();
   }
   return ret;

}

bool           TRecHistory::query_kadr_alarms_archives  (TIBSqlQuery & query,LPOTDM_ARCHIVE_QUERY arch_query)
{
   char * query_text =
   "select distinct KADR_ID, ALARM_START, ALARM_END,PROTECT ,ALARM_NAME from ALARMSHIST \n"
    "where KADR_ID = :KADR_ID "
    "and  ALARM_START >= :beg_time "
    "and  ALARM_END   <= :end_time "
    "order by alarm_start desc";
   bool ret =  query.prepare(query_text);
   if(ret)
   {
     TIBField p;
     query.set_named_param("kadr_id" ,arch_query->rec_ids.rec_id[0],p);
     query.set_named_param("beg_time",arch_query->period.beg_time  ,p);
     query.set_named_param("end_time",arch_query->period.end_time  ,p);

     ret = query.open();
   }
   return ret;

}

int            TRecHistory::query_archive_set_props(TIBSqlQuery & query,LPOTDM_ARCHIVE_HEADER arch_hdr)
{
 int ret = 0;
 //Выполнить установку имени и защиту архива
 char qtext [] =
 "update alarmshist ah \n"
 "set  ah.alarm_name = :alarm_name,ah.protect = :protect \n"
 "where ah.kadr_id = :kadr_id  and ah.alarm_start = :alarm_start \n"
 ;
 LPOTDM_KADR_ARCHIVE kar = (LPOTDM_KADR_ARCHIVE)arch_hdr->data;
 if(query.prepare(qtext))
 {
  TIBField p;
  query.set_named_param("alarm_name" ,kar->arch_name,p);
  query.set_named_param("protect"    ,kar->protect  ,p);
  query.set_named_param("kadr_id"    ,kar->kadr_id  ,p);
  query.set_named_param("alarm_start",kar->beg_time ,p);
  ret = query.open();
  query.close(true);

 }

 return ret;

}

int            TRecHistory::update_archive_thread   (DWORD cmd ,LPBYTE request)
{
 REC_HIST_CONFIG cfg;
  {
   guarder l(locker);
   memcpy(&cfg,&config,sizeof(cfg));
  }

 TIBDatabase    *ibdb  = NULL;
 TIBTransaction  wrtr;
 TIBSqlQuery     query;

 dbc_locker.lock();
 if(db_cached.size())
      {
       ibdb = db_cached.top();
       db_cached.pop       ();
      }
 dbc_locker.unlock();
 if(!ibdb)
   {
     ibdb = new TIBDatabase;
     open_data_base(*ibdb,cfg,NULL,NULL,NULL,NULL);
   }

 if(ibdb->is_connected())
 {
  init_trans_query(ibdb,&wrtr,&query,true);
  if(wrtr.start() )
  {
       LPMPROTO_HEADER     mph           = (LPMPROTO_HEADER)request;
       LPOTDM_PROTO_HEADER omph         = (LPOTDM_PROTO_HEADER)mph->data;
       LPOTDM_ARCHIVE_HEADER arch_hdr   = (LPOTDM_ARCHIVE_HEADER)omph->data;

   bool is_opened = true;
   switch(cmd)
   {
    case OTDMPROTO_CMD_ARCHIVE_SET_PROPS :   is_opened = query_archive_set_props(query,arch_hdr);break;
   }

   if(is_opened)
   {
        notify(RECHISTORY_NOTIFY_UPDATE,1,0,0);
        modem_addr ma(mph->addr_from);
        QWORD hist_id = arch_hdr->hist_id;
        //request.reset();
        SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_IDLE);
        /*
        switch(cmd)
        {
         case OTDMPROTO_CMD_QUERY_ARCHIVE :
         send_archive_query_records_results(cmd,query,mph->addr_from,arch_hdr->hist_id);
         break;
         case OTDM_CMD_QUERY_KADR_ARCHIVES: send_archive_query_alarms_results(cmd,query,mph->addr_from,arch_hdr->hist_id);
         break;
        }
        */
        SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_NORMAL);
     }
   else
   report_sql_error(&query);
  }
  else
  report_sql_error(&wrtr);
  query.close(true);
  end_transaction(&wrtr,true);
  //ibdb->Disconnect();
  //delete ibdb;
  dbc_locker.lock();
  db_cached.push(ibdb);
  dbc_locker.unlock();
 }
 delete [] request;
 return 0;
}

int            TRecHistory::read_archive_thread   (DWORD cmd ,LPBYTE request)
{
 REC_HIST_CONFIG cfg;
  {
   guarder l(locker);
   memcpy(&cfg,&config,sizeof(cfg));
  }

 TIBDatabase    *ibdb  = NULL;
 TIBTransaction  rdtr;
 TIBSqlQuery     query;

 dbc_locker.lock();
 if(db_cached.size())
      {
       ibdb = db_cached.top();
       db_cached.pop       ();
      }
 dbc_locker.unlock();
 if(!ibdb)
   {
     ibdb = new TIBDatabase;
     open_data_base(*ibdb,cfg,NULL,NULL,NULL,NULL);
   }

 if(ibdb->is_connected())
 {
  init_trans_query(ibdb,&rdtr,&query,false);
  if(rdtr.start() )
  {
       LPMPROTO_HEADER     mph          = (LPMPROTO_HEADER)request;
       LPOTDM_PROTO_HEADER omph         = (LPOTDM_PROTO_HEADER)mph->data;
       LPOTDM_ARCHIVE_HEADER arch_hdr   = (LPOTDM_ARCHIVE_HEADER)omph->data;
       LPOTDM_ARCHIVE_QUERY  arch_query = (LPOTDM_ARCHIVE_QUERY) arch_hdr->data;
   bool is_opened = true;
   switch(cmd)
   {
    case OTDMPROTO_CMD_QUERY_ARCHIVE      :   is_opened = query_records_archive(query,arch_query,cfg.sql_rd_history);break;
    case OTDMPROTO_CMD_QUERY_KADR_ARCHIVES:   is_opened = query_kadr_alarms_archives(query,arch_query);break;
   }

   if(is_opened)
   {
        notify(RECHISTORY_NOTIFY_READ,1,0,0);
        modem_addr ma(mph->addr_from);
        QWORD hist_id = arch_hdr->hist_id;
        //request.reset();
        SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_IDLE);
        switch(cmd)
        {
         case OTDMPROTO_CMD_QUERY_ARCHIVE :
         send_archive_query_records_results(cmd,query,mph->addr_from,arch_hdr->hist_id);
         break;
         case OTDMPROTO_CMD_QUERY_KADR_ARCHIVES: send_archive_query_alarms_results(cmd,query,mph->addr_from,arch_hdr->hist_id);
         break;
        }
        SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_NORMAL);
     }
   else
   report_sql_error(&query);
  }
  else
  report_sql_error(&rdtr);
  query.close(true);
  end_transaction(&rdtr,true);
  //ibdb->Disconnect();
  //delete ibdb;
  dbc_locker.lock();
  db_cached.push(ibdb);
  dbc_locker.unlock();
 }
 delete [] request;
 return 0;
}

inline bool check_need_terminate(KeRTL::TEvent & event)
{
  return (event.Wait(0) != WAIT_TIMEOUT) ? true : false;
}

void __fastcall TRecHistory::send_archive_query_records_results(DWORD cmd,TIBSqlQuery & query,MODEM_ADDR& _to_addr,QWORD hist_id)
{
  LPMPROTO_HEADER        mph;
  LPOTDM_PROTO_HEADER    omph;
  LPOTDM_ARCHIVE_HEADER  arch_hdr;
  LPOTDM_ARCHIVE_RECORDS_RESPOND arch_respond;
  modem_addr to_addr(_to_addr);

  DWORD frame_limit   = get_modem_line_frame_size(to_addr);
  frame_limit-= ( sizeof(*omph)+sizeof(*mph)+sizeof(*arch_respond)+sizeof(*arch_hdr) - sizeof(mph->data[0]) - sizeof(omph->data[0]));

  int respond_chunk_size = frame_limit/sizeof(arch_respond->rec[0]);

  DWORD resp_size  = sizeof(*arch_respond)+sizeof(arch_respond->rec[0])*(respond_chunk_size);
  DWORD omph_size  = sizeof(*arch_hdr) - sizeof(arch_hdr->data[0]);
  DWORD mph_size   = sizeof(*omph    ) - sizeof(omph->data[0]);
  DWORD total_size = sizeof(*mph     ) - sizeof(mph->data[0]);
        total_size+= mph_size+omph_size+resp_size;
  auto_ptr<BYTE> ptr(new BYTE[total_size]);
  mph           = (LPMPROTO_HEADER)        ptr.get();
  omph          = (LPOTDM_PROTO_HEADER)    mph->data;
  arch_hdr      = (LPOTDM_ARCHIVE_HEADER)  omph->data;
  arch_respond  = (LPOTDM_ARCHIVE_RECORDS_RESPOND) arch_hdr->data;

  memset(mph,0,total_size);
  arch_hdr->hist_id = hist_id;
  omph->command    = cmd|OTDM_RESPOND|OTDM_BEGINDATA;
  KeRTL::TEvent _term_event(term_event.Dup(false));

  TIBField rec_id      =  query["rec_id"];
  TIBField rec_diag    =  query["diag"];
  TIBField rec_state   =  query["state"];
  TIBField rec_value   =  query["rvalue"];
  TIBField kvants      =  query["kvants"];
  TIBField rec_ch_time =  query["ch_time"];
  TIBField is_alarm    =  query["is_alarm"];
  TIBField otdpd       =  query["otdpd"];
  TIBField rec_wr_time =  query["wr_time"];


  LPOTDM_ARCHIVE_REC  arch_rec;

  arch_respond->count = 0;
             arch_rec = arch_respond->rec;
  omph->data_size = sizeof(*arch_hdr)-sizeof(arch_hdr->data[0])+sizeof(arch_respond->count);

  while(!query.is_eos())
  {
    if(!query.is_status_ok(true))
       return;
    if(arch_respond->count < (DWORD)respond_chunk_size)
    {
     arch_rec->NULL_MASK = 0;
     arch_rec->rec_id  = (DWORD)rec_id;
     arch_rec->ch_time = (QWORD)rec_ch_time;
     if(rec_value.is_null())
     arch_rec->NULL_MASK|= OTDM_ARCHIVE_RVALUE_NULL;
     else
     arch_rec->rvalue  = (float)rec_value/(float)FLOAT_MULTYPLIER;
     if(kvants.is_null())
     arch_rec->NULL_MASK|= OTDM_ARCHIVE_KVANTS_NULL;
     else
     arch_rec->kvants    = (int)kvants;

     if(rec_state.is_null())
        arch_rec->NULL_MASK|= OTDM_ARCHIVE_STATE_NULL;
        else
        arch_rec->state    = (WORD)rec_state;
     if(rec_diag.is_null())
        arch_rec->NULL_MASK|= OTDM_ARCHIVE_DIAG_NULL;
        else
        arch_rec->diag    = (WORD)rec_diag;
        arch_rec->alarm   = is_alarm.get_int(0);
        arch_rec->otdpd   = otdpd.get_int(0);
        if(rec_wr_time.is_null())
           arch_rec->wr_time = 0;
           else
           {
            SYSTEMTIME stm = (SYSTEMTIME)rec_wr_time;
            SystemTimeToFileTime(&stm,(LPFILETIME)&arch_rec->wr_time);
           }
     omph->data_size+=sizeof(*arch_rec);
     ++arch_rec;
     ++arch_respond->count;
     query.fetch();
    }
    else
    {
      //Отправить кадр
      mproto_init(mph,0,FA_OTD_MEDIUM_ALARM_ARCHIVE,omph->data_size + sizeof(*omph)-sizeof(omph->data[0]),&to_addr);
      send (mph);
      Sleep(0);
      notify(RECHISTORY_NOTIFY_SEND,mproto_size(mph),0,0);
      omph->command &= ~OTDM_BEGINDATA;
      if(query.is_eos()) omph->command |= OTDM_ENDDATA;
      arch_respond->count = 0;
      arch_rec = arch_respond->rec;
      omph->data_size = sizeof(*arch_hdr)-sizeof(arch_hdr->data[0])+sizeof(arch_respond->count);

    }
   if(check_need_terminate(_term_event))
      return;

  }

  //if(!check_need_terminate(_term_event))
     {
      omph->command |= OTDM_ENDDATA;
      mproto_init(mph,0,FA_OTD_MEDIUM_ALARM_ARCHIVE,omph->data_size + sizeof(*omph)-sizeof(omph->data[0]),&to_addr);
      send (mph);
      notify(RECHISTORY_NOTIFY_SEND,mproto_size(mph),0,0);
     }
}

DWORD __fastcall TRecHistory::get_modem_line_frame_size(modem_addr & to_addr)
{
  MODEM_LINE_PROPS mlp;
  bzero(&mlp,sizeof(mlp));
  mlp.dw_size  = sizeof(mlp);
  LRESULT ret = GKH_RET_ERROR;
  if(!call_gkhandle_bynameW(MODEMMER_HANDLE_NAME_W,MDMCM_GET_LINE_PROPS,to_addr.addr,(LPARAM)&mlp,&ret) || ret)
     mlp.max_frame_size = 2048;
  if(mlp.max_frame_size == DWORD (-1))
       mlp.max_frame_size = 16284;
  return mlp.max_frame_size;
}

void __fastcall TRecHistory::send_archive_query_alarms_results(DWORD cmd,TIBSqlQuery & query,MODEM_ADDR& _to_addr,QWORD hist_id)
{

  LPMPROTO_HEADER             mph;
  LPOTDM_PROTO_HEADER         omph;
  LPOTDM_ARCHIVE_HEADER       arch_hdr;
  LPOTDM_KADR_ARCHIVE_RESPOND arch_respond;
  modem_addr to_addr(_to_addr);
  DWORD frame_limit   = get_modem_line_frame_size(to_addr);
  frame_limit-= ( sizeof(*omph)+sizeof(*mph) + sizeof(*arch_respond)+sizeof(*arch_hdr) - sizeof(mph->data[0]) - sizeof(omph->data[0]));
  int respond_chunk_size = frame_limit/sizeof(arch_respond->krec[0]);

  DWORD resp_size  = sizeof(*arch_respond)+sizeof(arch_respond->krec[0])*(respond_chunk_size);
  DWORD omph_size  = sizeof(*arch_hdr) - sizeof(arch_hdr->data[0]);
  DWORD mph_size   = sizeof(*omph    ) - sizeof(omph->data[0]);
  DWORD total_size = sizeof(*mph     ) - sizeof(mph->data[0]);
        total_size+= mph_size+omph_size+resp_size;
  auto_ptr<BYTE> ptr(new BYTE[total_size]);
  mph           = (LPMPROTO_HEADER)        ptr.get();
  omph          = (LPOTDM_PROTO_HEADER)    mph->data;
  arch_hdr      = (LPOTDM_ARCHIVE_HEADER)  omph->data;
  arch_respond  = (LPOTDM_KADR_ARCHIVE_RESPOND) arch_hdr->data;

  memset(mph,0,total_size);
  arch_hdr->hist_id = hist_id;
  omph->command    = cmd|OTDM_RESPOND|OTDM_BEGINDATA;
  KeRTL::TEvent _term_event(term_event.Dup(false));

  TIBField kadr_id     =  query["kadr_id"];
  TIBField beg_tm      =  query["alarm_start"];
  TIBField end_tm      =  query["alarm_end"];
  TIBField is_alarm    =  query["is_alarm"];
  TIBField alarm_name  =  query["alarm_name"];
  TIBField alarm_prot  =  query["protect"]  ;


  LPOTDM_KADR_ARCHIVE  krec;

  arch_respond->count = 0;
             krec = arch_respond->krec;
  omph->data_size = sizeof(*arch_hdr)-sizeof(arch_hdr->data[0])+sizeof(arch_respond->count);

  while(!query.is_eos())
  {
    if(!query.is_status_ok(true))
       return;
    if(arch_respond->count < (DWORD)respond_chunk_size)
    {

     krec->kadr_id   = (DWORD)kadr_id;
     krec->beg_time  = (QWORD)beg_tm;
     krec->end_time  = (QWORD)end_tm;
     krec->protect   = alarm_prot.get_int(0);
     *krec->arch_name = 0;
     strncpy(krec->arch_name,alarm_name.get_string().c_str(),sizeof(krec->arch_name));
     omph->data_size+=sizeof(*krec);
     ++krec;
     ++arch_respond->count;
     query.fetch();
    }
    else
    {
      //Отправить кадр
      mproto_init(mph,0,FA_OTD_MEDIUM_ALARM_ARCHIVE,omph->data_size + sizeof(*omph)-sizeof(omph->data[0]),&to_addr);
      send (mph);
      Sleep(0);
      notify(RECHISTORY_NOTIFY_SEND,mproto_size(mph),0,0);
      omph->command &= ~OTDM_BEGINDATA;
      if(query.is_eos()) omph->command |= OTDM_ENDDATA;
      arch_respond->count = 0;
      krec = arch_respond->krec;
      omph->data_size = sizeof(*arch_hdr)-sizeof(arch_hdr->data[0])+sizeof(arch_respond->count);

    }
   if(check_need_terminate(_term_event))
      return;

  }

  //if(!check_need_terminate(_term_event))
     {
      omph->command |= OTDM_ENDDATA;
      mproto_init(mph,0,FA_OTD_MEDIUM_ALARM_ARCHIVE,omph->data_size + sizeof(*omph)-sizeof(omph->data[0]),&to_addr);
      send (mph);
      notify(RECHISTORY_NOTIFY_SEND,mproto_size(mph),0,0);
     }
}






LRESULT __fastcall TRecHistory::restore_database (LPRECHIST_RESTORE rhr,char * cl_lib)
{
  LRESULT ret = GKH_RET_ERROR;
  if(!is_running() && check_data_size(rhr,sizeof(*rhr)))
  {
    //Загрузка библиотеки
    bool lib_loaded = TIBBase::is_clib_loaded();
    if(!lib_loaded)
      {
       if(!cl_lib || *cl_lib == 0)
           cl_lib = "gds32.dll";
       TIBBase::init_ib_lib(cl_lib);
      }
    TIBService svc;
    svc.enable_exception(0);
    std::string srv_name,db_name;
    std::string db_path(rhr->target_name);
    if(TIBDatabase::split_db_path(db_path,srv_name,db_name))
    {
     if(!svc.attach(srv_name.c_str(),rhr->user_name,rhr->password))
     {
        if(svc.attach(srv_name.c_str(),"SYSDBA","masterke"))
           {
            ib_users_list ul;
            svc.add_user(rhr->user_name,rhr->password,"Admin","Data","Base");
            svc.detach();
            if(!svc.attach(srv_name.c_str(),rhr->user_name,rhr->password))
                return GKH_RET_ERROR;
           }
     }
     char mod_dll[MAX_PATH<<1];
     GetModuleFileName(this->get_module_instance(),mod_dll,sizeof(mod_dll));
     TKFName<char> bkp;
     bkp.set_fullname(mod_dll);
     bkp.set_name("rechistory");
     bkp.set_ext(".fbk");
     ret = svc.restore(db_name.c_str(),bkp(),isc_spb_res_create|isc_spb_res_replace,rhr->page_size,rhr->buffers,false,false) ? GKH_RET_SUCCESS : GKH_RET_ERROR;
     if(!ret)
        {
         if(!svc.get_svc_text(db_path))
             {
              string err_str = " Ошибка восстановления  БД\n\r";
              err_str += db_path;
              err_str += "\n\r";
              err_str += svc.get_error_text();
              report(this->rep_id,REPORT_ERROR_TYPE,(char*)err_str.c_str());
              ret = GKH_RET_ERROR;
             }
        }
     svc.detach();
    }
   if(!lib_loaded)
      TIBBase::init_ib_lib(NULL);
  }

  return  ret;
}


