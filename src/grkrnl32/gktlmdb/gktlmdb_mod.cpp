#include <tregstry.hpp>
#include <stdio.h>
#include <kfname.hpp>
#pragma hdrstop
#include "gktlmdb_mod.hpp"
#include <kestrm.hpp>
#include <alloc.h>

  THeap TGKTlmDB::heap; // 512 Кб
  LONG  TGKTlmDB::total_alloc = 0;

   DWORD  __fastcall TGKTlmDB::get_mem_used()
   {
	return heap.IsValid() ? heap.GetMemoryUsed() : GKH_RET_ERROR;
        //total_alloc;
   }


   void * __stdcall TGKTlmDB::mem_alloc(size_t sz )
   {
	 if(!heap.IsValid()) heap.Create(TSysInfo().GetPageSize()<<8);
	 void * ptr = heap.Alloc(sz);
         if(!ptr)
         {
           ptr = malloc(sz);
         }
	return ptr;
    }

   void   __stdcall TGKTlmDB::mem_free (void * ptr)
   {
	  if(ptr )
	  {
              if( heap.IsValid() && heap.Validate(ptr))
		heap.Free(ptr);
                else
                {
                 if(ptr && _USEDENTRY == heapchecknode(ptr)) ::free(ptr);
                }
          }
   }
   




 TGKTlmDB::TGKTlmDB()
         :responds(TRespond::creator)

 {
     if( alloc_gkhandle(GKTLMDB_HANDLE_NAME_W) )
     {
      lock_param = GKHB_AUTO_LOCK_OFF;
      lowdb.set_module(this);
      rep_id      = report_reg_event_type(L"GKTLMDB"         ,L"БД нижнего уровня");
      turep_id    = report_reg_event_type(L"GKTLMDB_TUTR"    ,L"БД нижнего уровня - Операции ТУ" );
     }
  }

 TGKTlmDB::~TGKTlmDB()
 {
 
 }


 void __fastcall TGKTlmDB::release(LPVOID p)
 {
  TGKModule::release(p);
 }


void     __fastcall TGKTlmDB::archive_report(DWORD evt_type,wchar_t * rep_text)
{
 TReportMes mes;
 mes.log    = arch_rep_id;
 mes.desc   = rep_text;
 mes.source = _handle;
 mes.type   = evt_type;
 report(RCM_MESSAGE_EVENT,(LPARAM)&mes,0);

}

void     __fastcall TGKTlmDB::db_report(DWORD evt_type,wchar_t * rep_text,LPVOID data,DWORD dt_sz)
{
 LPBYTE buff = new BYTE[sizeof(TReportMes)+dt_sz];
 if(buff)
 {
  TReportMes &mes = *((TReportMes*)buff);
  mes.log    = rep_id;
  mes.desc   = rep_text;
  mes.source = _handle;
  mes.type   = evt_type;
  mes.data_size = dt_sz;
  if(dt_sz)
   memcpy(mes.data,data,dt_sz);
  report(RCM_MESSAGE_EVENT,(LPARAM)buff,0);
  delete [] buff;
 }

}

wchar_t wm_name[] = L"GKTLMDBWND.DLL";
DWORD  __fastcall TGKTlmDB::get_window_module (wchar_t * buf,DWORD bsz)
{
 int len = lstrlenW(wm_name);
 if(buf && int(bsz) > len)
  {
   SetLastError(0);
   safe_strcpy(buf,wm_name);
  }
  else
  {SetLastError(MERR_INVALID_BUFFER_SIZE);len=-len;}
 return DWORD(len);
}


 bool    __fastcall TGKTlmDB::read_settings  ()
 {
  bool ret = TGKModule::read_settings();
  if(ret)
  {
    TRegsReader rgrd(DupKey(reg_key));
    settings.options = rgrd.ReadDword(TLMDB_REGDW_OPTIONS,0,true);
    if(!rgrd.ReadString(TLMDB_REGSTR_DBPATH,settings.db_file_name,sizeof(settings.db_file_name),true))
    {
      check_file_exist cfe;
      get_uni_filename((wchar_t*)L"LOWDB.ldb",(wchar_t*)settings.db_file_name,sizeof(settings.db_file_name)/sizeof(wchar_t),cfe);
      cc_flags|=CCFL_REGPARAMS;
    }
    read_db();

  }
  return ret;
 }


 bool    __fastcall TGKTlmDB::write_settings ()
 {
   if(cc_flags&CCFL_DB)
   {
    //Сохранений БД
    write_db();
   }
  bool ret = TGKModule::write_settings();
  if(ret)
  {
   if(cc_flags&CCFL_REGPARAMS)
   {
   TRegsWriter rgwr(DupKey(reg_key));
   rgwr.WriteString(TLMDB_REGSTR_DBPATH,settings.db_file_name,true);
   rgwr.WriteDword(TLMDB_REGDW_OPTIONS,settings.options,true);
   rgwr.Flush();
   cc_flags&=~CCFL_REGPARAMS;
   }


  }
  return ret;
 }

 #pragma pack(push,1)
 struct db_hdr
 {
  BYTE  dbsig[8];
  DWORD ver;
 };
 #pragma pack(pop)

 const char * db_sig = "TLMDBLOW";

 void     __fastcall TGKTlmDB::read_db()
 {
     TFileStream strm(settings.db_file_name,true);
     if(strm.IsGood())
     {
      db_hdr dbh;
      strm.Read(&dbh,sizeof(dbh));
      if(memcmp(dbh.dbsig,db_sig,8)==0)
        lowdb.stream_read(strm,dbh.ver);
     }
 }

 void     __fastcall TGKTlmDB::write_db()
 {
     TFileStream strm(settings.db_file_name);
     if(strm.IsGood())
     {
      db_hdr dbh;
      memcpy(dbh.dbsig,db_sig,8);
      dbh.ver = 1;
      strm.Write(&dbh,sizeof(dbh));
      lowdb.stream_write(strm);
      cc_flags&=~CCFL_DB;
     }
 }

 #pragma warn -8057
 DWORD       __fastcall TGKTlmDB::get_config_data(DWORD mask,LPVOID buf,DWORD bsz)
 {
  LPTLMDB_SETTINGS dbs = (LPTLMDB_SETTINGS)buf;
  if(bsz>=sizeof(*dbs))
  {
    memcpy(dbs,&settings,sizeof(settings));
    return GKH_RET_SUCCESS;
  }
  return GKH_RET_ERROR;
 }

 bool        __fastcall TGKTlmDB::set_config_data(DWORD mask,LPVOID buf,DWORD bsz)
 {
   if(bsz>=sizeof(settings))
   {
    LPTLMDB_SETTINGS src_settings = (LPTLMDB_SETTINGS)buf;
    DWORD ch_mask = (settings.options^src_settings->options);

    memcpy(&settings,src_settings,sizeof(settings));
//    if(ch_mask&TLMDB_OPT_HANDLE_ALARM_ARCHIVES)
//       {
//        if(settings.options&TLMDB_OPT_HANDLE_ALARM_ARCHIVES)
//          start_archives() ;
//          else
//          stop_archives();
//       }

    return true;
   }
   return false;
 }

 bool        __fastcall TGKTlmDB::compare_config_data(DWORD mask,LPVOID buf,DWORD bsz,LPDWORD ch_mask,BOOL * restart)
 {
   if(bsz>=sizeof(settings))
   {
    DWORD _ch_mask = 0;
    if(lstrcmpiW(settings.db_file_name,((LPTLMDB_SETTINGS)buf)->db_file_name))
       {_ch_mask |=TLMDB_SMASK_DBF;cc_flags|=CCFL_DB;}
    if(settings.options != ((LPTLMDB_SETTINGS)buf)->options)
       {
        _ch_mask |= TLMDB_SMASK_OPT;
       }

    if(ch_mask) *ch_mask = _ch_mask;
    if(restart && (_ch_mask&1)) *restart = TRUE;
    return true;
   }
   return false;

 }

 #pragma warn .8057

 /*DWORD WINAPI tf(LPVOID param)
 {
  TMrSw * p_mr = (TMrSw*) param;
  if(p_mr)
  {
   p_mr->LockForRead();
   p_mr->ReadUnlock();
   p_mr->LockForWrite();
   Sleep(10000);
   p_mr->WriteUnlock();
  }
  return 0;

 }
 */

// void   __fastcall TGKTlmDB::start_archives()
// {
//  if(this->settings.options & TLMDB_OPT_HANDLE_ALARM_ARCHIVES)
//  {
//    modemmer.call(MDMCM_REGISTER_HANDLER,LPARAM((*this)()),FA_OTD_ALARM_ARCHIVE);
//    otd_archiever.Start();
//    check_source_lines ();
//  }
// }
//
// void   __fastcall TGKTlmDB::stop_archives()
// {
//  if(otd_archiever.IsRunning())
//  {
//   otd_archiever.TerminateAndWait(5000,true);
//  }
//  modemmer.call(MDMCM_UNREGISTER_HANDLER,LPARAM((*this)()),FA_OTD_ALARM_ARCHIVE);
// }



 DWORD   __fastcall TGKTlmDB::start(DWORD reason,LPARAM p2)
 {


  DWORD ret = TGKModule::start(reason,p2);
  //Подготовка БД - установка состояний диагностики в -1
  //Регистрация себя как обработчика FA -OTD у ДФА модеммера
  lowdb.set_module(this);
  lowdb.on_start();
  modemmer.open((wchar_t*)MODEMMER_HANDLE_NAME_W);
  modemmer.set_notify(_handle,MNF_MODEM,TRUE);
  modemmer.call(MDMCM_REGISTER_HANDLER,LPARAM((*this)()),FA_OTD);
  modemmer.call(MDMCM_REGISTER_HANDLER,LPARAM((*this)()),FA_OTD_ALARM_ARCHIVE);
  //start_archives();
  refresh_pu(-1,-1);
  return ret;
 }



 DWORD   __fastcall TGKTlmDB::stop (DWORD reason)
 {
  BYTE diag_send [MAX_PATH];
  otd_proto op;
  op.dw_size = sizeof(op);
  otd_proto_format(diag_send,sizeof(diag_send),OTD_PROTO_PART_DIAG,-1,0,0,0,0,0,&op);
  op.addr->addr = -1;
  *op.diag = -1;
  modem_addr ma;
  ma.addr = -1; 
  lowdb.handle(op,ma,NULL,NULL);
  modemmer.set_notify(_handle,MNF_MODEM,FALSE);
  responds.terminate(5000);
  DWORD ret = TGKModule::stop(reason);
  modemmer.call(MDMCM_UNREGISTER_HANDLER,LPARAM((*this)()),FA_OTD);
  modemmer.call(MDMCM_UNREGISTER_HANDLER,LPARAM((*this)()),FA_OTD_ALARM_ARCHIVE);
  //stop_archives();
  modemmer.close();
  lowdb.on_stop();
  return ret;
 }

DWORD    __fastcall TGKTlmDB::on_exception(lpexcept_record er)
{
  lowdb.rd_unlock();
  lowdb.wr_unlock();
  return TGKModule::on_exception(er);
}


 LRESULT __fastcall TGKTlmDB::processing    (DWORD cmd,LPARAM p1,LPARAM p2)
 {
  LRESULT ret = GKH_RET_ERROR;
  if(MCMD_TYPE(cmd) == get_type())
  {
   switch(cmd)
   {
    case     TLMDBCM_DBLOW_ENUM_ENTRY  : ret = this->lowdb_enum_entry((DWORD)p1,(LPTLMDB_LOW_GET_ENTRY)p2);
    break;
    case     TLMDBCM_DBLOW_GET_ENTRY   : ret = this->lowdb_get_entry((DWORD)p1,(LPTLMDB_LOW_GET_ENTRY)p2);
    break;
    case     TLMDBCM_DBLOW_RECREATE    : ret = lowdb.recreate(*&sotd_addr((DWORD)p1),(LPTLMDB_LOW_RECREATE)p2);break;
    case     TLMDBCM_DBLOW_MAKE_STATIC : ret = lowdb.make_static(*&sotd_addr((DWORD)p1),(BOOL)p2);break;
    case     TLMDBCM_DBLOW_GET_SRCMODEM: ret = lowdb.get_modemaddr(sotd_addr((DWORD)p1)).addr;break;
    case     TLMDBCM_DBLOW_REFRESH_ADDR: ret = GKH_RET_SUCCESS; refresh_addr(*&sotd_addr(p1),p2 ? TRUE:FALSE);break;
    case     TLMDCMD_LOWDB_REMOVE_INVALID_DINAMIC: lowdb.remove_invalid_dynamic((DWORD)p1);
                                                   ret = GKH_RET_SUCCESS;
                                                   break;
    case     TLMDBCM_DBLOW_TUOFF :
    case     TLMDBCM_DBLOW_TUON  : lowdb.send_tu  ((DWORD)p1,(DWORD)p2,cmd&1);ret = GKH_RET_SUCCESS;break;
    case     TLMDBCM_DBLOW_TUUNDO: lowdb.send_undo((DWORD)p1,(DWORD)p2);ret = GKH_RET_SUCCESS;break;
    case     TLMDBCM_ENUM_ORDERS :
             if(check_data_size((LPTLMDB_ORDER_ENTRY)p2,sizeof(TLMDB_ORDER_ENTRY)))
                 ret = orders.orders_enum(p1,(LPTLMDB_ORDER_ENTRY)p2);break;

    default : SetLastError(MWRN_UNKNOWN_MODULE_COMMAND);break;

   }
   return ret;
  }
  if(MCMD_TYPE(cmd) == MT_MODEMMER)
  {
   switch(cmd)
   {
    case MDMCM_FA_HANDLE: ret = handle_data((LPMPROTO_HEADER)p1,modemmer());break;
    default : SetLastError(MWRN_UNKNOWN_MODULE_COMMAND);break;
   }
   return  ret;
  }

   ret =  TGKModule::processing(cmd,p1,p2);
  return ret;
 }


 void  __fastcall TGKTlmDB::refresh_addr(sotd_addr & addr,BOOL order)
 {
  BYTE buffer[MAX_PATH];
  LPMPROTO_HEADER mph =(LPMPROTO_HEADER)buffer;
  ZeroMemory(mph,sizeof(*mph));
  mph->addr_to.addr = lowdb.get_modemaddr(addr).addr;
  mph->addr_to.line = -1;
  mph->fa  = FA_OTD;
  addr.fa&= (OTD_FA_ALL|OTD_FA_SETUP);
  addr.fa|=OTD_FA_CALL;
  if(order)
    addr.fa|=OTD_FA_ORDER;

  mph->data_size = otd_proto_format_query((LPBYTE)mph->data,
                                          (DWORD)(sizeof(buffer)-sizeof(*mph)),
                                          &addr,0,
                                          OTD_PROTO_PART_DATA|
                                          OTD_PROTO_PART_PERSONAL_DIAG|
                                          OTD_PROTO_PART_NAME|
                                          OTD_PROTO_PART_DIAG|
                                          OTD_PROTO_PART_TIMESTAMP
                                          //|OTD_PROTO_FLAG_PARTIAL_DATA
                                          ,
                                          (LPVOID)NULL);
  send(mph);

 }

 void  __fastcall TGKTlmDB::refresh_pu  (DWORD modem_addr,DWORD pu_num)
 {
  BYTE buffer[MAX_PATH];
  LPMPROTO_HEADER mph =(LPMPROTO_HEADER)buffer;
  ZeroMemory(mph,sizeof(*mph));
  mph->addr_to.addr = modem_addr;
  mph->fa  = FA_OTD;
  sotd_addr addr(pu_num,OTD_ADDR_MAXVALUE,OTD_FA_ALL|OTD_FA_SETUP|OTD_FA_ORDER|OTD_FA_CALL,OTD_ADDR_MAXVALUE);

  mph->data_size = otd_proto_format_query((LPBYTE)mph->data,
                                          (DWORD)(sizeof(buffer)-sizeof(*mph)),
                                          &addr,0,
                                          OTD_PROTO_PART_DATA|
                                          OTD_PROTO_PART_PERSONAL_DIAG|
                                          OTD_PROTO_PART_NAME|
                                          OTD_PROTO_PART_DIAG|
                                          OTD_PROTO_PART_TIMESTAMP|
                                          OTD_PROTO_PART_PERSONAL_CHMASK
                                          //|OTD_PROTO_FLAG_PARTIAL_DATA
                                          ,
                                          (LPVOID)NULL);
  send(mph);
 }

 bool __fastcall is_query(otd_proto & op)
 {
  bool ret = false;
  if((op.addr->fa < OTD_ADDR_MAXVALUE) )
  {
     if(op.addr->fa&OTD_FA_QUERY)
     ret = false;
  }
  else
   ret = (!op.diag && !op.data && !op.personal_diag && !op.personal_chmask && !op.name && ! op.time_stamp && !op.error && !op.extension) ? true:false;
  return ret;
 }

 LRESULT  __fastcall TGKTlmDB::handle_data(LPMPROTO_HEADER mph,GKHANDLE src)
 {
  /*Обработчик входящих данных*/
  DWORD ret = GKH_RET_ERROR;
  switch(mph->fa)
  {
   case FA_OTD        : ret = handle_otd        (mph,src);
   break;
   case FA_OTD_ALARM_ARCHIVE: ret = handle_otd_archive(mph,src);
   break;
  }
  return ret;
 }

LRESULT  __fastcall TGKTlmDB::send(LPMPROTO_HEADER mph)
{
  return modemmer.call(MDMCM_SEND,(LPARAM)mph,0);
}

#pragma warn -8057
LRESULT __fastcall TGKTlmDB::handle_otd_archive(LPMPROTO_HEADER mph,GKHANDLE src)
{
  LRESULT ret = GKH_RET_ERROR;
  otd_addr addr;
  addr.addr = *((LPDWORD)mph->data);
  sotd_proto sop((LPBYTE)mph->data,mph->data_size);
     if(OTD_ADDR_ISQUERY(&addr))
     {

       handle_orders(mph->addr_from,mph->fa,sop.op,src);
       ret = GKH_RET_SUCCESS;
     }
   else
   {
    send_archieve_data(sop);
    ret = GKH_RET_SUCCESS;
   }
  return  ret;
}

void __fastcall TGKTlmDB::send_archieve_data(sotd_proto & sop)
{
  //отправка архивных даннх подписчикам
  BYTE __buffer[8192];
  DWORD sz = sizeof(__buffer);
  LPBYTE buff = __buffer;

  order_list olist;
  TOrders & orders = get_archive_orders();
  {
      //TOrders::rd_locker rdl(orders);
      if(orders.get_order_addrs(*sop.op.addr,(*sop.op.ver_parts)&OTD_PROTO_PARTSMASK,olist))
      {
       LPMPROTO_HEADER mph;
       DWORD need_sz = sop.op.proto_size+sizeof(*mph)-sizeof(mph->data[0]);
       if(sz<need_sz)
        {
          sz = need_sz;
          buff = new BYTE [need_sz];
        }
        mph = (LPMPROTO_HEADER)buff;
        mproto_init(mph,0,FA_OTD_ALARM_ARCHIVE,sop.op.proto_size);
        memcpy(mph->data,sop.op.addr,sop.op.proto_size);
        order_list::iterator obeg = olist.begin() ,oend = olist.end();
        while(obeg<oend)
        {
            mph->addr_to.addr = obeg->addr;
            mproto_protect(mph);
            send(mph);
            ++obeg;
        }
      }
  }
   if(buff && buff!=__buffer) delete [] buff;
}


void    __fastcall TGKTlmDB::handle_orders(MODEM_ADDR & addr_from,DWORD fa,otd_proto & op,GKHANDLE src)
{
  TOrders * ord = NULL;
  if(fa == FA_OTD)
     ord = &orders;
  if(fa == FA_OTD_ALARM_ARCHIVE)
       ord = &archive_orders;
  if(ord)
  {
       MODEM_PROPS mp = {0};
       mp.dw_size = sizeof(mp);
       modem_addr ma(addr_from);
       get_modem_props(ma,&mp,sizeof(mp));
       if(mp.flags&MPF_DATA_ACCEPTOR)
       {
       DWORD parts = (*op.ver_parts)&OTD_PROTO_PARTSMASK;
       if(op.addr->fa&OTD_FA_ORDER)
         {
          if(parts)
             ord->add_order   (this,ma,*op.addr,parts);
            else
             ord->remove_order(this,ma,*op.addr);
         }
        }
  }
}

LRESULT __fastcall TGKTlmDB::handle_otd(LPMPROTO_HEADER mph,GKHANDLE src)
{
  LRESULT ret = GKH_RET_SUCCESS;
  otd_proto op;
  op.dw_size = sizeof(op);
  DWORD real_sz;
  if(otd_proto_parser((LPBYTE)mph->data,mph->data_size,&op,&real_sz))
  {

     if(OTD_ADDR_ISQUERY(op.addr))
     {
       //TODO Обработка запроса-вызова
       handle_orders(mph->addr_from,mph->fa,op,src);
       if((*op.ver_parts)&OTD_PROTO_PARTSMASK )
         responds.start(mph->addr_from.addr,&lowdb,&op);

     }
     else
       {

        if(OTD_FA_ISSETUP(op.addr->fa))
        {
         //Команда настройки оьорудования
         //Пересылается на устройство
         mph->addr_to.addr = lowdb.get_modemaddr(*op.addr).addr;
         if(mph->addr_to.addr != (DWORD)-1)
            {
             mproto_protect(mph);
             send(mph);
            }

        }
        else
        {
         DWORD lo_obj = 0;
         DWORD hi_obj = -1;
         DWORD ch =
         lowdb.handle(op,*&modem_addr(mph->addr_from.addr),&lo_obj,&hi_obj);
         ch = ch;
        }
       }
  }
  return ret;
}
 #pragma warn .8057

 LRESULT  __fastcall TGKTlmDB::on_notify_code(LPNOTIFY_CODE nc,DWORD mask)
 {
  if((mask & MNF_MODEM) )
  {
    if(nc->notify_code == MNCODE_MODEM_NUMBER)
    {
     LPMODEM_CHANGE_NUMBER mcn = (LPMODEM_CHANGE_NUMBER)nc->notify_data;
     modem_addr ma(0,mcn->old_number,-1,-1);
     MODEM_PROPS mp = {0};
     mp.dw_size = sizeof(mp);
     get_modem_props(ma,&mp,sizeof(mp));
     lowdb.modem_connect(ma,false,&mp);
     orders.remove_order(this,ma,*&sotd_addr(-1));
    }
    if(nc->notify_code == MNCODE_LINE_STATE)
    {
     on_modem_line_state((LPMODEM_LINE_STATE)nc->notify_data);
    }
  }

  return GKH_RET_SUCCESS;
 }

 void     __fastcall TGKTlmDB::on_modem_line_state(LPMODEM_LINE_STATE mls)
 {
  if((mls->state_changes & MODEM_LINE_STATE_CONNECT))
  {
        modem_addr ma(mls->addr.addr);
        bool connect = mls->line_state&MODEM_LINE_STATE_CONNECT ? true:false;
        MODEM_PROPS mp = {0};
        mp.dw_size = sizeof(mp);
        get_modem_props(ma,&mp,sizeof(mp));
        lowdb.modem_connect(ma,connect,&mp);
        //otd_archiever.modem_line_connect(ma,connect,mp);
        if(!connect)
           {
            if(mp.flags&MPF_DATA_ACCEPTOR)
            {
             if(orders.remove_order(this,ma,sotd_addr(-1)))
               {
                wchar_t  rep_text[MAX_PATH<<1];
                wchar_t  rep_templ[MAX_PATH];
                get_lang_string(IDS_LOG_MODEM_DISCONNECTDEST,rep_templ,sizeof(rep_templ)/sizeof(wchar_t));
                wsprintfW(rep_text,rep_templ,(DWORD)ma.modem,(DWORD)ma.line);
                db_report(REPORT_INFORMATION_TYPE,rep_text,0,0);
               }
            }
           }
  }
 }


 LRESULT  __fastcall TGKTlmDB::lowdb_get_entry (DWORD addr ,LPTLMDB_LOW_GET_ENTRY ge)
 {
  LRESULT ret = GKH_RET_ERROR;
  lowdb.rd_lock();
  DWORD  index = lowdb.find_byaddr(*&sotd_addr(addr));
  if(index!=(DWORD)-1)
     {
      ret = lowdb_enum_entry(index,ge);
     }
     else
     SetLastError(TLMDB_ERR_MODULE_NOTFOUND);
  lowdb.rd_unlock();

  return ret;
 }

 LRESULT  __fastcall TGKTlmDB::lowdb_enum_entry(DWORD index,LPTLMDB_LOW_GET_ENTRY ge)
 {
      LRESULT ret = GKH_RET_ERROR;
      if(check_data_size(ge,sizeof(*ge)))
      {
      if(lowdb.rd_lock())
      {
      ge->data_len = lowdb.get_item(index,ge->parts,ge->loN,ge->hiN,ge->data,ge->data_size);
      if(ge->data_len>0)
         ret = GKH_RET_SUCCESS;
         else
         SetLastError(ERROR_INSUFFICIENT_BUFFER);
      lowdb.rd_unlock();
      }
      else
      DebugBreak();
      }
     return ret;
 }


void     __fastcall TGKTlmDB::send_otd_changes(otd_addr  & addr,DWORD parts,DWORD lo_obj,DWORD hi_obj)
{
 DWORD mon_len = 0;
 char  mon_text[2048];
 DWORD   ticks = GetTickCount();
 BYTE src_buffer[8192];
 if(parts&OTD_PROTO_PART_DATA)
    parts|=OTD_PROTO_PART_DIAG;
 if(otd_addr_type(&addr)<OTD_ADDR_TYPE_SB)
    parts&=OTD_PROTO_PART_DIAG|OTD_PROTO_PART_NAME;
 parts|=OTD_PROTO_PART_KPK;
 DWORD sz  = lowdb.get_item(lowdb.find_byaddr(addr),parts,0,-1,src_buffer,sizeof(src_buffer));
 if(sz)
 {
  sotd_proto sop(src_buffer,sz);
  otd_proto_param opp;
  ZeroMemory(&opp,sizeof(opp));
  opp.dw_size  = sizeof(opp);
  otd_proto_getparam(&sop(),&opp);
  order_list send_list;
  order_list bad_send;
  order_list::iterator beg ,end ;
  {
  //TOrders::rd_locker rdl(orders);
  sz = orders.get_order_addrs(addr,parts,send_list);
  if(sz)
  {
   mon_len = sprintf(mon_text,"Изменения %03d.%03d.%03d.%03d - %X отправлены подписчикам:",(DWORD)addr.pu, (DWORD)addr.cp,(DWORD)addr.fa,(DWORD)addr.sb,parts);
   notify(TLMDB_DBLOW_MONITOR,TLMDB_DBLOW_MONITOR_CHANGES,mon_text,mon_len+1);
   mon_len = 0;
   BYTE buffer[16384+sizeof(MPROTO_HEADER)];
   LPMPROTO_HEADER mph =  (LPMPROTO_HEADER)buffer;
   ZeroMemory(mph,sizeof(*mph));
   mph->fa = FA_OTD;
   beg = send_list.begin();
   end = send_list.end();
   while(beg<end)
   {

    bool partial_data = (beg->order_parts&OTD_PROTO_FLAG_PARTIAL_DATA )? true:false;
    otd_proto_param _opp;
    memcpy(&_opp,&opp,sizeof(opp));
    mph->addr_to.addr = beg->addr;
    _opp.parts&=beg->order_parts;
    _opp.parts|=OTD_PROTO_PART_KPK;
    if(partial_data && hi_obj!=(DWORD)-1)
     {
        DWORD cnt = hi_obj-lo_obj+1;
       _opp.dt_param.first = KeRTL::MAX(lo_obj,(DWORD)_opp.dt_param.first);
       _opp.dt_param.count = KeRTL::MIN(cnt,DWORD(_opp.dt_param.count));
       _opp.pd_param.first = KeRTL::MAX(lo_obj,(DWORD)_opp.pd_param.first);
       _opp.pd_param.count = KeRTL::MIN(cnt,DWORD(_opp.pd_param.count));
       _opp.cm_param.first = KeRTL::MAX(lo_obj,(DWORD)_opp.cm_param.first);
       _opp.cm_param.count = KeRTL::MIN(cnt,DWORD(_opp.cm_param.count));
     }
    mph->data_size = otd_proto_getparts_ex((LPBYTE)mph->data,sizeof(buffer)-sizeof(*mph),&_opp,&sop(),0);
    sotd_proto send_op;
    send_op.assign((LPBYTE)mph->data,mph->data_size);
    otd_proto_protect(&send_op(),send_op().proto_size,OTD_DEF_POLINOM);
    mproto_protect(mph);
    LRESULT send_result = send(mph);
    if(send_result)
       bad_send.push_back(*beg);
       else
       {
        mon_len+=sprintf(mon_text+mon_len,"%03d.%03d",(DWORD)mph->addr_to.modem,(DWORD)mph->addr_to.line);
       }
    ++beg;
    if(beg<end)
       lstrcat(mon_text,", "),mon_len+=2;
   }
  }
  }

   if(bad_send.size() && orders.wr_lock(100))
   {
      //удаление подписчиков передача которым завершилась с ошибкой
      beg = bad_send.begin();
      end = bad_send.end();
      while(beg<end)
      {
       sotd_addr sa((DWORD)-1);
       orders.remove_order(this,*beg,sa);
       ++beg;
      }
    orders.wr_unlock() ;
   }
 }
 if(mon_len)
  notify(TLMDB_DBLOW_MONITOR,TLMDB_DBLOW_MONITOR_CHANGES,mon_text,mon_len+1);

 ticks = GetTickCount()-ticks;
 if(mon_len)
   {
    mon_len=sprintf(mon_text,"Время рассылки %d ms", ticks);
    notify(TLMDB_DBLOW_MONITOR,TLMDB_DBLOW_MONITOR_CHANGES,mon_text,mon_len+1);
   }
}


DWORD    __fastcall TGKTlmDB::get_modem_props(modem_addr & ma,LPMODEM_PROPS mp,DWORD sz)
{
 mp->addr.addr = ma.addr;
 return modemmer.call(MDMCM_GET_MODEM_PROPS,(LPARAM)mp,sz);
}

DWORD    __fastcall TGKTlmDB::get_modem_line_text(modem_addr & ma,wchar_t * text,DWORD sz)
{
 DWORD ret = 0;
 BYTE b1[2048];
 BYTE b2[2048];

 LPMODEM_PROPS mp       = (LPMODEM_PROPS)b1;
 ZeroMemory(mp,sizeof(*mp));
 mp->dw_size = sizeof(*mp);
 LPMODEM_LINE_PROPS mlp = (LPMODEM_LINE_PROPS)b2;
 ZeroMemory(mlp,sizeof(*mlp));
 mlp->dw_size = sizeof(*mlp);

 mp->addr.addr  = mlp->addr.addr = ma.addr;
 mp->modem_info_size = (sizeof(b1)-sizeof(*mp)+sizeof(mp->modem_info))/sizeof(wchar_t);
 mlp->line_text_sz   = (sizeof(b2)-sizeof(*mlp)+sizeof(mlp->line_text))/sizeof(wchar_t);
 modemmer.call(MDMCM_GET_MODEM_PROPS,(LPARAM)mp,sizeof(b1));
 modemmer.call(MDMCM_GET_LINE_PROPS,ma.addr,(LPARAM)mlp);
 if(sz>ret+mp->modem_info_len+8)
    ret+= swprintf(text+ret,L"%s № %03d ",mlp->line_text,(DWORD)ma.line);
 if(sz>ret+mp->modem_info_len+8)
    ret+=swprintf(text+ret,L"%s № %03d ",mp->modem_info,(DWORD)ma.modem);
 return ret;
}

 void     __fastcall TGKTlmDB::report_dbtutr(DWORD type,wchar_t *  text)
 {
   report(turep_id,type,text);
 }

 void     __fastcall TGKTlmDB::check_source_lines ()
 {
//   MODEM_LINE_PROPS mlp;
//   ZeroMemory(&mlp,sizeof(mlp));
//   mlp.dw_size = sizeof(mlp);
//   MODEM_LINE_STATE mls;
//   mls.dw_size       = sizeof(mls);
//
//   DWORD modem_index = 1;

//   while(GKH_RET_SUCCESS == modemmer.call(MDMCM_ENUM_MODEMS,modem_index,(LPARAM)&mlp.addr))
//   {
//     int line_idx = 0;
//     while(GKH_RET_SUCCESS == modemmer.call(MDMCM_ENUM_MODEM_LINES,line_idx,(LPARAM)&mlp))
//     {
//       if(otd_archiever.IsRunning())
//       {
//        mls.addr.addr     = mlp.addr.addr;
//        mls.line_state    = mlp.line_state;
//        mls.state_changes = mlp.line_state;
//        on_modem_line_state(&mls);
//       }
//       ++line_idx;
//     }
//     ++modem_index;
//   }
 }








