/******************************************************************************/
/*  Авторские права Остапенко Д.В. DOSTAP@NORCOM.RU                           */
/*  Реализация классов для работы с Interbase через IB-API (GDS32.DLL)        */
/*                                                                            */
/******************************************************************************/
#ifdef _MSC_VER
#define _M_M68K //для _strtold
#endif

#include <algorithm>
#include <math.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include <ibclass.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef _NO_NAMESPACE
namespace KeRTL
{
#endif

KeRTL::TFASTMutex TIBBase::mon_locker;
Tmonitor_callback TIBBase::monitor_callback;


bool   notify_param::operator ()(int phase,TIBBase * who)
{
 bool ret = false;
 if(notify_proc)
 {
   __try{
        try{
            ret = notify_proc(user_data,phase,type,who) ? true:false;
           }
           catch(...)
           {

           }
        }
        __except(EXCEPTION_EXECUTE_HANDLER)
        {
        }
 }
 return ret;

}

inline bool __fastcall ptr_comparer(const LPVOID p1,const LPVOID p2)
{
 /*Спец сравнение */
 if(p1 == p2 || p1 == NULL)
    return false;
  return p1<p2 ? true:false;
}

struct np_comparer
{
 bool operator()(const notify_param & np1,const notify_param& np2)
 {
  if(np1.type == np2.type)
  {
   if(np1.user_data == np2.user_data)
     {
      return ptr_comparer(np1.notify_proc,np2.notify_proc);
     }
     return ptr_comparer(np1.user_data ,np2.user_data);
  }
  return np1.type < np2.type ? true:false;
 }
};

inline bool operator == (const notify_param & np1,const notify_param& np2)
{
 return np1.type        == np2.type &&
        np1.user_data   == np2.user_data &&
        np1.notify_proc == np2.notify_proc
        ?
        true:false;
}

inline bool operator != (const notify_param & np1,const notify_param& np2)
{
 return !(np1==np2);
}

void __fastcall TIBBase::reg_notify  (int type,LPVOID _ud,pfn_ib_notify_proc _nproc)
{
  lock();
   /*Регистратор уведомления*/
   notify_param  np(type,_ud,_nproc);
   vector<notify_param>::iterator ptr =lower_bound(this->notify_list.begin(),notify_list.end(),np,np_comparer());
   if(!(ptr<notify_list.end() && *ptr==np))
      notify_list.insert(ptr,np);
  unlock();
}

void __fastcall TIBBase::unreg_notify(int type,LPVOID _ud,pfn_ib_notify_proc _nproc)
{
  lock();
   /*Дергистратор уведомления*/
   notify_param  np(type,_ud,_nproc);
   vector<notify_param>::iterator ptr =lower_bound(this->notify_list.begin(),notify_list.end(),np,np_comparer());
   if(ptr<notify_list.end() && *ptr==np)
      notify_list.erase(ptr);
  unlock();
}

bool __fastcall TIBBase::notifyer (int phase,int type)
{
 lock();
 notify_param np(type,NULL,NULL);
 vector<notify_param>::iterator l_ptr = lower_bound(notify_list.begin(),notify_list.end(),np,np_comparer());
 vector<notify_param>::iterator h_ptr = upper_bound(notify_list.begin(),notify_list.end(),np,np_comparer());
 while(l_ptr<h_ptr)
     (*l_ptr++)(phase,this);
 unlock();
 return true;
}

void   TIBBase::set_monitor_callback(LPVOID cb_data,Tmonitor_callback::closure_method fptr)
{
 mon_locker.Lock  ();
 monitor_callback.SetClosure(cb_data,fptr);
 mon_locker.Unlock();
}

void __fastcall TIBBase::monitor_call(DWORD code)
{
  //mon_locker.Lock  ();
  monitor_callback(this,code);
  //mon_locker.Unlock();
}


int  ibobj_vector::operator += (const TIBBase * b)
{
 iterator ptr = begin(),e_ptr = end();
 ptr = lower_bound(ptr,e_ptr,b,ibobj_less());
 if(!(ptr<e_ptr && *ptr == b))
   {
    ptr = insert(ptr,(TIBBase*)b);
    return distance(begin(),ptr);
   }
 return -1;  
}

int  ibobj_vector::operator -= (const TIBBase * b)
{
 iterator ptr = begin(),e_ptr = end();
 ptr = lower_bound(ptr,e_ptr,b,ibobj_less());
 if(ptr<e_ptr && *ptr == b)
 {
  erase(ptr);
  return TRUE;
 }
  return FALSE;
}

bool ibobj_vector::operator    ()(const TIBBase * b)
{
 return std::binary_search(begin(),end(),b,ibobj_less());
}



string __fastcall TIBStatus::get_error_text(int offs,int cnt )  const
{
 string ret;
 char   text[1024];
 if(status)
 {  int offset = 0;
    ISC_STATUS * ps = (ISC_STATUS*)status_vector+(offs<<1);
    while(cnt-- && isc_interprete(text+offset,&ps))
    {
      ret+=text;
      if(!offset)
         strcpy(text,"\n\r"),offset = 2;
    }
 }
 if(ret.length())
    ret+="\n\r";
 return ret;
}

bool       __fastcall TIBStatus::is_status_ok(bool no_except )
{
 bool ret = status == 0 ? true:false;
 if(!ret && enable_exceptions && !no_except)
   throw TIBException(this);
  return ret;
};

  TIBException::TIBException(const TIBStatus * _except_obj)
  {
   exception_object     = _except_obj->get_ib_class_name();
   this->exception_message = _except_obj->get_error_text();
  }




 const char  * __fastcall TIBDatabase::get_ib_class_name() const
 {
  return "TIBDatabase";
 }

 string       __fastcall TIBDatabase::get_conn_str()
 {
  return string(DbName);
 }



TIBDatabase & TIBDatabase::operator = (const TIBDatabase & src)
{
  if(is_connected())
      Disconnect();
  set_db_name(src.DbName);
  params = src.params;
  return * this;
}


int   __fastcall TIBDatabase::get_int_info(int info_code)
{
  int ret = 0;
  char req = info_code;
  char res[32];
  if(get_info(1,&req,sizeof(res),res))
    {
     char *p = res;
     if(req == *p++)
     {
      ret = isc_portable_integer(p,sizeof(short));
      p+=sizeof(short);
      ret = isc_portable_integer(p,ret);
     }
    }
  return ret;
}

int   __fastcall TIBDatabase::get_active_trans_count()
{
  return get_int_info(isc_info_active_tran_count);
}

int   __fastcall TIBDatabase::do_get_connection_id()
{
  return get_int_info(isc_info_attachment_id);
}

int   __fastcall TIBDatabase::get_reads  ()
{
  int sv = reads;
  reads = get_int_info(isc_info_reads);
  return reads - sv;
}

int   __fastcall TIBDatabase::get_writes ()
{
  int sv = writes;
  writes = get_int_info(isc_info_writes);
  return writes - sv;
}

int   __fastcall TIBDatabase::get_fetches()
{
 int sv = fetches;
 fetches = get_int_info(isc_info_fetches);
 return fetches - sv;
}

int   __fastcall TIBDatabase::get_marks  ()
{
 int sv = marks;
 marks = get_int_info(isc_info_marks);
 return marks - sv;
}



bool  __fastcall  TIBDatabase::Connect(LPCSTR _db_name,LPCSTR User,LPCSTR Password,LPCSTR sql_role,LPCSTR lc_type ,BYTE dialect )
{
   if(lc_type == NULL)
      lc_type = "WIN1251";
   params.Revert(true);
   params.add_param   (isc_dpb_version1);
   params.add_user    ((LPSTR)User);
   params.add_password((LPSTR)Password);
   if(sql_role && *sql_role)
      params.add_param(isc_dpb_sql_role_name,(LPBYTE)sql_role,strlen(sql_role));
   if(lc_type && *lc_type)
      {
      params.add_param(isc_dpb_lc_ctype,(LPBYTE)lc_type,strlen(lc_type));
      params.add_param(isc_dpb_lc_messages,(LPBYTE)lc_type,strlen(lc_type));
      }
   params.add_param(isc_dpb_sql_dialect,&dialect,1);
   set_db_name(_db_name);
   return Connect(); 
}

bool  __fastcall  TIBDatabase::Connect(LPCSTR _db_name,TIBParams & _params)
{
   params = _params;
   set_db_name(_db_name);
   return Connect();
}

bool  __fastcall  TIBDatabase::Connect()
{
  status = isc_attach_database(status_vector,0,DbName ?DbName:"unknown database",&HDB,params.get_params_size(),params.get_ib_params());
  bool ret = is_status_ok();
  if(ret)
    {
     reads = writes = fetches = marks = 0;
     connection_id = do_get_connection_id();
     notifyer(ibn_after,IBDB_NTYPE_CONNECT);
    }
    else
    connection_id = -1;
  reads = writes = fetches = marks = 0;   
  return ret;
}

void __fastcall   TIBDatabase::Disconnect()
{
 if(HDB!=0)
  {
    notifyer(ibn_before,IBDB_NTYPE_DISCONNECT);
    connection_id = -1;
__try{
     status = isc_detach_database(status_vector,&HDB);
     }
     __except(EXCEPTION_EXECUTE_HANDLER){status = 0;}
    
    if(status == 0)
       notifyer(ibn_after,IBDB_NTYPE_DISCONNECT);

  }
}




void __fastcall TIBDatabase::set_db_name(LPCSTR _db_name)
{

 if(DbName)
 {
   if(!_db_name || stricmp(DbName,_db_name))
      delete [] DbName,DbName = NULL;
 }

 if(_db_name && * _db_name)
 {
  DbName = new char[strlen(_db_name)+1];
  strcpy(DbName,_db_name);
 }
 
}


//////////////////////////////////////////////////////////////////////
// TIBParams Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



void __fastcall TIBParams::add_param(BYTE type, LPBYTE data, BYTE sz)
{
 params.push_back(BYTE(type));
 if(sz && data)
 {
  params.reserve(params.size()+sz+sizeof(BYTE));
  params.push_back((char)sz);
  copy((LPBYTE)data,((LPBYTE)data)+sz,back_inserter(params));
 }
}



void __fastcall   TIBParams::Revert(bool full )
{
  BYTE    fb = params.front();
  params.clear();
  if(!full)
     params.push_back(fb);

}


  const char  * __fastcall TIBTransaction::get_ib_class_name() const
  {
   return "TIBTransaction";
  }

 bool  __fastcall    TIBTransaction::set_database(TIBDatabase * _db)
 {
   if(_db)
   {
    if(db!=_db )
    {
      if(spin_count == 0)
       db = _db;
       else
       return false;
    }
   }
   return db!=NULL ? true:false;
 }

 bool  __fastcall    TIBTransaction::start(TIBDatabase * _db )
 {

   if(set_database(_db))
   {
   if(db && !spin_count)
     {
     profile_start();
     isc_db_handle HDB    =  (*db)();
     status = isc_start_transaction(status_vector,&HTR,1,&HDB,tran_params.get_params_size(),tran_params.get_ib_params());
     if(!status)
     {
       char         info_ch = isc_info_tra_id;
       char         res_buf[32] = {0};
       isc_transaction_info(status_vector,&HTR,sizeof(info_ch),&info_ch,sizeof(res_buf),res_buf);
       short len = isc_portable_integer((ISC_UCHAR*)res_buf+1,sizeof(short));
       trans_id  = isc_portable_integer((ISC_UCHAR*)res_buf+1+sizeof(short),len);
       db->get_trans_list()+=this;
      }
      profile_stop();
      monitor_call(IBC_MON_TRANSACTION_START|IBC_MONF_END);
    }

   if(is_status_ok() && HTR)
       ++spin_count;
   }
   return spin_count>0 ? true:false;
 }

 #pragma warn -8037
 bool  __fastcall    TIBTransaction::start(const TIBParams & params,TIBDatabase * db )
 {
  if(!is_active())
  {

   set_params(params);
   return start(db);
  }
  // Транзакция активна тогда повторное входжение возможно только при совпадении параметров
  if( tran_params.get_params_size() == params.get_params_size()
      && !memcmp((char*)tran_params.get_ib_params(),(char*)params.get_ib_params(),params.get_params_size())
      )
     return start(db);
  return false;
 }
 #pragma warn .8037



 bool  __fastcall    TIBTransaction::commit  (end_trans_action end_action )
 {
   if(spin_count)
   {
    if(end_action ==endtr_retain)
    {
     status = isc_commit_retaining(status_vector,&HTR);
     monitor_call(IBC_MON_TRANSACTION_COMMIT|IBC_MON_TRANSACTION_RETAIN|IBC_MONF_END);
    }
     else
     {
    if(--spin_count && end_action!=endtr_full)
       {
        if(end_action == endtr_retain)
         {
           status = isc_commit_retaining(status_vector,&HTR);
           monitor_call(IBC_MON_TRANSACTION_COMMIT|IBC_MON_TRANSACTION_RETAIN|IBC_MONF_END);
         }
       }
       else
       {
        status = isc_commit_transaction(status_vector,&HTR);
        monitor_call(IBC_MON_TRANSACTION_COMMIT|IBC_MONF_END);
        spin_count = 0;
        HTR        = 0;
        trans_id   = 0;
        db->get_trans_list()-=this;
       }
     }
   }
   if(spin_count == 0)
      profile_stop();
   return spin_count == 0 ? true:false;
 }

 bool  __fastcall    TIBTransaction::rollback(end_trans_action end_action )
 {
   if(spin_count)
   {
    if(--spin_count && end_action!=endtr_full)
       {
        if(end_action == endtr_retain)
        {
           status = isc_rollback_retaining(status_vector,&HTR);
           monitor_call(IBC_MON_TRANSACTION_ROLLBACK|IBC_MON_TRANSACTION_RETAIN|IBC_MONF_END);
        }

       }
       else
       {
       status = isc_rollback_transaction(status_vector,&HTR);
       monitor_call(IBC_MON_TRANSACTION_ROLLBACK|IBC_MONF_END);
       spin_count = 0;
       HTR = 0;
       trans_id   = 0;
       }

   }
   if(spin_count == 0)
   {
     profile_stop();
     return true;
   }
   return false;
 }


TIBXSQLDA::TIBXSQLDA(int count)
{
sqlda = NULL;
memset(&temp,0,sizeof(temp));
temp.version = SQLDA_VERSION1;temp.sqln = 1;temp.sqld = 1;
alloc_sqlda(count);
vars_data = NULL;
vars_data_size = 0;
rdf_counter = 0;
}

TIBXSQLDA::TIBXSQLDA(const TIBXSQLDA & src)
{
 sqlda = NULL;
 memset(&temp,0,sizeof(temp));
 temp.version = SQLDA_VERSION1;temp.sqln = 1;
 vars_data = NULL;
 vars_data_size = 0;
 rdf_counter = 0;
 *this = src;
}

#pragma warn -8037
TIBXSQLDA & TIBXSQLDA::operator = (const TIBXSQLDA & src)
{
 // Копия SQL данных
  alloc_sqlda(src()->sqln);
  memcpy((void*)(*this)(),src(),XSQLDA_LENGTH((*this)()->sqln));
  format_vars_data();
  src.get_raw_data(this->vars_data,vars_data_size);
  return *this;
}
#pragma warn .8037


int       __fastcall TIBXSQLDA::get_raw_data(LPBYTE buff, int bsz)
{
  int ret = 0;
  if(vars_data && vars_data_size)
  {
   ret  = std::min(bsz,vars_data_size);
   memcpy(buff,vars_data,ret);
  }
  return ret;
}

void      __fastcall TIBXSQLDA::free_vars()
{
  if(vars_data)
    delete [] vars_data;
    vars_data = 0;
    vars_data_size = 0;
}

void      __fastcall TIBXSQLDA::free_sqlda()
{
  if(sqlda)
    delete [] (LPBYTE) sqlda;
    sqlda = NULL;
}


bool __fastcall TIBXSQLDA::alloc_sqlda(int count)
{
 if(sqlda )
 {
   if(sqlda->sqln<count || !count)
      free_sqlda();
      else
      sqlda->sqld = count;
 }   

 if(count>1)
 {
  if(!sqlda)
  {
   int len = XSQLDA_LENGTH(count);
   sqlda   = (XSQLDA*) new BYTE[len];
   if(sqlda)
    {
     memset(sqlda,0,len);
     sqlda->version = SQLDA_VERSION1;
     sqlda->sqld = sqlda->sqln    = count;
     return true;
    }
  }
 }
 return (*this)->sqld ? true:false;
 //return sqlda ? true:false;
}


 int       __fastcall TIBXSQLDA::calc_vars_data_size(bool as_input)
 {
  int ret = 0;
  XSQLVAR * var;
  for(int i = 0;i<this->get_fields_count();++i)
     {
      var = (*this)[i];
      ret+=var->sqllen;
      if((var->sqltype&(~1)) == SQL_VARYING)
         ret+=sizeof(short);
      if(as_input || (var->sqltype&1))
        ret+=sizeof(*var->sqlind);
     }
  return ret;
 }

 void      __fastcall TIBXSQLDA::set_var_param(int num,char * name,int type,bool nullable,int size )
 {
  //Ручная разметка параметров
  if(num< get_fields_count())
  {
   XSQLVAR * var = (*this)[num];
   var->sqltype  = type|(nullable ? 1:0);

   switch(var->sqltype&(~1))
   {
        case SQL_TEXT     :
              break;
        case SQL_VARYING  :
             break;
        case SQL_TIMESTAMP:
        case SQL_QUAD     :
        case SQL_INT64    : size = sizeof(__int64);break;
        case SQL_TYPE_TIME:
        case SQL_TYPE_DATE:
        case SQL_LONG     :  size = sizeof(LONG)  ;break;
        case SQL_SHORT    :  size = sizeof(short) ;break;
        case SQL_FLOAT    :  size = sizeof(float) ;break;
        case SQL_D_FLOAT  :
        case SQL_DOUBLE   : size = sizeof(double) ;break;
   }
   var->sqllen = size;
   if(name && *name)
     {
      strncpy(var->aliasname,name,sizeof(var->aliasname));
      var->aliasname_length = std::min(sizeof(var->aliasname),strlen(name));
     } 
   else
      var->aliasname_length = sprintf(var->aliasname,"F%d",num);
   var->sqlname_length =var->aliasname_length;
   memcpy(var->sqlname,var->aliasname,std::min(var->aliasname_length,sizeof(var->aliasname)));
  }
 }

 void      __fastcall TIBXSQLDA::format_vars_data(bool as_input )
 {
   int new_vds = calc_vars_data_size(as_input);
   if(vars_data_size<new_vds)
   {
     free_vars();
     vars_data_size = new_vds;
     vars_data = new BYTE[vars_data_size];
   }

   if(vars_data)
   {
    memset(vars_data,0,vars_data_size);
    LPBYTE curr_ptr = vars_data;
    XSQLVAR * var;
    for(int i = 0;i<this->get_fields_count();++i)
     {
      var = (*this)[i];
      var->sqldata  =(char*) curr_ptr;
      curr_ptr     += var->sqllen;
      if((var->sqltype&(~1)) == SQL_VARYING)
         curr_ptr+=sizeof(short);
         
      var->sqlind = NULL;
      if(as_input || (var->sqltype&1))
      {
         var->sqlind  = (short*)curr_ptr;
         *var->sqlind = -1;
         curr_ptr+=sizeof(*var->sqlind);
      }
     }
   }
 }


const char  * __fastcall TIBSqlQuery::get_ib_class_name() const
{
 return "TIBSqlQuery";
}

TIBSqlQuery::TIBSqlQuery(TIBTransaction * trans,TIBDatabase * db)
{
 stmt = NULL;
 opened = false;
 rec_no = 0;
 eos = true;
 has_cursor  = false;
 database    =  db;
 transaction =  trans;
 prepared    = FALSE;
 if(trans)
 database    =  db ? db:transaction->get_database();
}

void  __fastcall    TIBSqlQuery::Release()
{
 close(true);
 out_data.free_vars();
 in_data.free_vars();
 TIBStatus::Release();
}



bool __fastcall   TIBSqlQuery::exec(const char * query_text)
{

 if(SQL.compare(query_text))
 {
  close(true);
  SQL = query_text;
 }
 if(!database && transaction)
     database = transaction->get_database();
 isc_db_handle HDB = (*database)();
 isc_tr_handle HTR = (*transaction).get_tr_handle();
 status = isc_dsql_execute_immediate(status_vector,&HDB,&HTR,strlen(query_text),query_text,3,NULL);
 return is_status_ok();
}

bool __fastcall TIBSqlQuery::set_unique_cursor()
{
 char c_name[128];
 ISC_STATUS status;
 static LONG cnt = 0;
 int    try_count = 256;
 if(!has_cursor)
 {
 do{
    sprintf(c_name,"res_set%d",InterlockedIncrement(&cnt));
    status = isc_dsql_set_cursor_name(status_vector,&stmt,c_name,0);
   }while(status && --try_count);
   has_cursor = is_status_ok();
 }
 return has_cursor;
}

#define IBSQL_NEED_CURSOR_MASK (isc_info_sql_stmt_select|isc_info_sql_stmt_insert|isc_info_sql_stmt_update|isc_info_sql_stmt_delete)

bool __fastcall   TIBSqlQuery::open(TIBXSQLDA * fields)
{
    bool ret = false;
    eos = true;

    if(is_prepared())
    {
 __try{
      if(opened) close(false);
       if(!fields)
           fields = &in_data;
       profile_start();
       isc_tr_handle HTR = transaction->get_tr_handle();
       monitor_call(IBC_MON_SQL_OPEN);
       if(sql_type & IBSQL_NEED_CURSOR_MASK)
       status = isc_dsql_execute2(status_vector,&HTR,&stmt,3,(*fields)->sqld ? (*fields)():NULL,NULL/*out_data()*/);
       else
       status = isc_dsql_execute2(status_vector,&HTR,&stmt,3,(*fields)->sqld ? (*fields)():NULL,out_data()->sqld ? out_data():NULL);
       monitor_call(IBC_MON_SQL_OPEN|IBC_MONF_END);
       ret = is_status_ok();
       if(ret  )
       {
        if(out_data()->sqld)
        {
         opened = true;
         if(sql_type & IBSQL_NEED_CURSOR_MASK)
          {
           ret    = set_unique_cursor();
           eos    = !(ret && fetch());
          }
        }
        else
        eos = true;
       }
       else
       eos = true;
       profile_stop();
      }
      __except(EXCEPTION_EXECUTE_HANDLER){}
    }
    return ret;

}

int __cdecl  TIBSqlQuery::assign_values(int count,...)
{
 int ret = 0;
 va_list args;
 va_start(args,count);
 TIBField f = get_input_field(0);
 count = std::min(count,get_input_fields_count());
 while(ret<count)
 {

  switch(f.sql_type())
  {
        case SQL_TEXT     :
        case SQL_VARYING  :
        case SQL_TIMESTAMP:
        case SQL_TYPE_TIME:
        case SQL_TYPE_DATE: f = (char*) va_arg(args,char*);
                            break;
        case SQL_SHORT    : f = (short) va_arg(args,short);
        break;
        case SQL_LONG     : f = (int) va_arg(args,int);
        break;
        case SQL_QUAD     :
        break;
        case SQL_INT64    : f = (__int64) va_arg(args,__int64);
        break;
        case SQL_FLOAT    : f = (float) va_arg(args,float);
        break;
        case SQL_DOUBLE   : f = (double) va_arg(args,double);
        break;
        case SQL_D_FLOAT  : f = (double) va_arg(args,long double);
        break;

  }
  ++ret;++f;
 }
 return ret;
}


bool          __fastcall    TIBSqlQuery::prepare_proc(const char * sql_text,TIBTransaction * trans )
{
 string cmd = "EXECUTE PROCEDURE ";
        cmd+= sql_text;
 return prepare(cmd.data(),trans);
}

bool          __fastcall    TIBSqlQuery::is_prepared(const char * query_text)
{
 bool ret = false ;
 if(stmt)
 {
  ret = !query_text || (!SQL.empty() && SQL.compare(query_text)==0) ? true:false;
  ret = ret && prepared ? true:false;
 }
 else
 {
  prepared = FALSE;
 }
 return ret;
}

bool __fastcall   TIBSqlQuery::prepare(const char * sql,TIBTransaction * trans)
{
  status = 0;
  if(is_prepared())
  {
   if(SQL.compare (sql) || (trans && ( trans!= transaction )))
      {
       close(true);
       prepare(sql,trans);
      }

  }
  else
  {
     profile_start();
     prepared = FALSE;
     SQL.reserve(strlen(sql)+1);
     SQL = sql;
     set_transaction(*trans);
     monitor_call(IBC_MON_SQL_PREPARE);
     if(transaction && transaction->is_active())
     {
     isc_db_handle HDB = (*database)();
     isc_tr_handle HTR = (*transaction).get_tr_handle();

      if(stmt)
        isc_dsql_free_statement(status_vector,&stmt,DSQL_drop);
    status   = isc_dsql_alloc_statement2(status_vector,&HDB,&stmt);
    this->prepared = false;
    if(!status)
    {

        status = isc_dsql_prepare(status_vector,&HTR,&stmt,SQL.length(),SQL.c_str(),3,out_data());
        if(!status)
        {
         if(out_data->sqln<out_data->sqld)
           out_data.alloc_sqlda(out_data->sqld);


          status = isc_dsql_describe(status_vector,&stmt,1,out_data());
          if(!status)
          {
           out_data.format_vars_data();
           out_data.rdf_counter = 0;
           status = isc_dsql_describe_bind(status_vector,&stmt,1,in_data());
           if(is_status_ok())
            {
             if(in_data->sqln<in_data->sqld)
                in_data.alloc_sqlda(in_data->sqld);

                status = isc_dsql_describe_bind(status_vector,&stmt,1,in_data());
                if(!status)
                {
                 in_data.rdf_counter = 0;
                 in_data.format_vars_data(true);
                 rec_no = 0;
                 eos    = true;
                 char type_info = isc_info_sql_stmt_type;
                 char buf[128];
                 if(!isc_dsql_sql_info(status_vector,&stmt,1,&type_info,sizeof(buf),buf))
                    sql_type = isc_portable_integer((ISC_UCHAR*)buf+3,2);
                 prepared = true;   
                }
            }
          }
        }
    }
   }
   profile_stop();
   monitor_call(IBC_MON_SQL_PREPARE|IBC_MONF_END);
   is_status_ok();
  }
  return is_prepared();
}


string     __fastcall TIBSqlQuery::get_plan()
{
  // Получение плана
  string ret;
  char buf[4096] = {0};
  char type_info = isc_info_sql_get_plan;
  if(!isc_dsql_sql_info(status_vector,&stmt,1,&type_info,sizeof(buf),buf))
      {
       char * ptr  = buf;
       while(*ptr != isc_info_end)
       {
        ++ptr;
        short len = isc_portable_integer((ISC_UCHAR*)ptr,sizeof(len));
        ptr+=sizeof(len);
        ret.append(ptr,len);
        ret.append("\n\r");
        ptr+=len;
       }
      }
return  ret;
}


bool  __fastcall    TIBSqlQuery::cancel()
{
  //status   = isc_dsql_free_statement(status_vector,&stmt,DSQL_cancel);
  close();
  rec_no = 0;
  return is_status_ok();
}

bool  __fastcall    TIBSqlQuery::close (bool full)
{
 if(stmt)
 {
   if(full)
    monitor_call(IBC_MON_SQL_CLOSE);
   if(opened || full)
     {
      status     = isc_dsql_free_statement(status_vector,&stmt,full ? DSQL_drop:DSQL_close);

     }

   if(status)
    {
     if(isc_dsql_cursor_close_err == get_status_code(3))
         status = 0;
    }
   opened     = false;
   rec_no     = 0;
   eos        = true;
   if(!stmt)
     {
      out_data()->sqld = in_data()->sqld = 0;
      has_cursor = false;
      prepared   = FALSE;
     }
     else
     {
      if(in_data.rdf_counter)
      {
       status = isc_dsql_describe_bind(status_vector,&stmt,1,in_data());
       in_data.format_vars_data();
       in_data.rdf_counter = 0;
      }
     }
   profile_stop();
   if(full)
     monitor_call(IBC_MON_SQL_CLOSE|IBC_MONF_END);

 }
 return opened;
}

void          __fastcall    TIBSqlQuery::clear_input()
{
 if(stmt && in_data.rdf_counter)
 {
       status = isc_dsql_describe_bind(status_vector,&stmt,1,in_data());
       in_data.format_vars_data();
       in_data.rdf_counter = 0;
 }
  TIBField f = this->get_input_field(0);
  while(f.field_number()>=0)
       f++.set_null(true);
}

void          __fastcall    TIBSqlQuery::clear_output()
{
  TIBField f = this->get_field(0);
  while(f.field_number()>=0)
      {
       __int64 d = 0;
       f.set_null(true);
       f.set_raw_data(&d,sizeof(d));
       ++f;
      } 
}



void          __fastcall TIBSqlQuery::set_database   (TIBDatabase    & db)
{
 if(database != &db)
  {
   close(true);
   database = &db;
  }
}

void          __fastcall TIBSqlQuery::set_transaction(TIBTransaction & trans)
{
  if(transaction!= &trans && &trans)
  {
   transaction = & trans;
   if(!database)
      database = trans.get_database();
  }
}


bool  __fastcall  TIBSqlQuery::unprepare()
{
 if(is_prepared())
 {
   close(true);
 }
 return !is_prepared();
}

bool  __fastcall  TIBSqlQuery::fetch()
{
  if(is_prepared() && out_data.get_fields_count())
  {
      status = isc_dsql_fetch(status_vector,&stmt,1,out_data());
      if(!status)
         {
          ++rec_no;
          opened = true;
          eos    = false;
         }
         else
         {
          eos = true;
          if(status == 100)
            status = 0;

         }
      profile_stop();
      monitor_call(IBC_MON_SQL_FETCH|IBC_MONF_END);
      is_status_ok();
      return bool(!eos);
  }
  return false;
}

int __fastcall   TIBSqlQuery::get_fields_count()
{
 return  is_prepared() ? out_data->sqld : 0;
}

int __fastcall   TIBSqlQuery::get_input_fields_count()
{
 return is_prepared() ? in_data->sqld:0;
}

int        __fastcall TIBXSQLDA::get_field_no(const char * field_name)
{
  if(field_name && * field_name)
  {
    int f_count = this->get_fields_count();
    for(int i = 0;i<f_count;i++)
    {
      XSQLVAR * var = (*this)[i];
      if(var->aliasname_length && 0 == strnicmp(var->aliasname,field_name,sizeof(var->aliasname)))
        return i;
      if(var->sqlname_length &&   0 == strnicmp(var->sqlname,field_name,sizeof(var->sqlname)))
        return i;
    }
  }
  return -1;
}




string __fastcall TIBField::name()
{
 string ret;
 if(sqlvar->sqlname_length)
    ret = string(sqlvar->sqlname,sqlvar->sqlname_length);
    else
    ret = aliase();
 if(!ret.length())
    {
     char num[32];
     sprintf(num,"№-%02d",this->idx);
     ret = num;
    }
  return ret;  
}

string __fastcall TIBField::aliase()
{

 return (sqlvar) ? string (sqlvar->aliasname,sqlvar->aliasname_length): string("");
}

  string     __fastcall TIBField::rel_name ()
  {
   return string (sqlvar->relname,sqlvar->relname_length);
  }

  string     __fastcall TIBField::owner ()
  {
   return string (sqlvar->ownname,sqlvar->ownname_length);
  }

    TIBField     & TIBField::operator ++()
    {

     if(sqlda )
     {
     if((idx+1)<sqlda->get_fields_count())
          sqlvar = (*sqlda)[++idx];
       else
       {
        sqlvar = NULL;
        idx = -1;
       }
     }
     return *this;
    }

    TIBField     & TIBField::operator --()
    {
     if(sqlda )
     {
      if(idx>0)
        sqlvar = (*sqlda)[--idx];
        else
        {
         sqlvar = NULL;
         idx = -1;
        }
     }
      return *this;
     }


  int          __fastcall TIBField::compare_raw_data(void * raw_data,int dsz)
  {
    int ret = -1;
    if(raw_data && sqlvar->sqldata && !is_null() && sqlvar->sqllen <= dsz)
    {
     ret = memcmp(raw_data,sqlvar->sqldata,dsz);
    }
    return ret;

  }

  int       __fastcall TIBField::get_raw_data(void * raw_buffer,int bsz)
  {
    int ret = 0;
    if(raw_buffer && sqlvar->sqldata && !is_null())
    {
     ret = std::min(sqlvar->sqllen,(short)bsz);
     memcpy(raw_buffer,sqlvar->sqldata,ret);
    }
    return ret;
  }

  int       __fastcall TIBField::set_raw_data(void * raw_buffer,int dsz)
  {
   int ret = 0;
   if(sqlvar->sqldata)
   {
    if(raw_buffer)
    {
    memset(sqlvar->sqldata,0,sqlvar->sqllen);
    ret = min(sqlvar->sqllen,(short)dsz);
    memcpy(sqlvar->sqldata,raw_buffer,ret);
    set_null(false);
    }
    else
    set_null(true);
   }
   return ret;
  }


  TIBField     & TIBField::operator = (const char * value)
  {
    if(sqlvar)
    {
    set_null(!value);
    if(value)
    {
    short type = sql_type();
    switch(type)
    {
     case SQL_TEXT:
     case SQL_VARYING:
     {
      if(value)
      {
            short value_length   = strlen(value);
            char  * dest= sqlvar->sqldata;
            short copy_len = KeRTL::min(value_length,sqlvar->sqllen - ((type == SQL_VARYING) ? sizeof(short):0 ));
            if(type == SQL_VARYING)
            {
              short * len = (short*)dest;
              dest     +=sizeof(*len);
              *len = copy_len;
            }
            else
            memset(dest,' ',sqlvar->sqllen);
            memcpy(dest,value,copy_len);

      }
     }
     break;
     case  SQL_BLOB:
      {
       TIBBlobField bf(*get_query(),this->field_number(),true);
       if(bf.open_blob())
       {
         bf.write_segment((LPVOID)value,strlen(value));
         bf.close();
       }
      }
      break;
       case SQL_FLOAT  :
       case SQL_DOUBLE :
       case SQL_D_FLOAT: *this = (double)atof(value); break;
       case SQL_SHORT  :
       case SQL_LONG   :
       case SQL_INT64  : *this = _atoi64(value); break;

      default:
      {
      ++sqlda->rdf_counter;
      sqlvar->sqltype = SQL_TEXT;
      sqlvar->sqllen = 0;
      if(sqlvar->sqlind)
        ++sqlvar->sqltype;
         set_null(false);
         sqlvar->sqllen  = strlen(value);//+(sqlvar->sqlind ? sizeof(*sqlvar->sqlind):0) ;
         sqlvar->sqldata =(char*) value;
      }
      break;

    }
   }
  }
    return *this;
 }

    TIBField     & TIBField::operator = (__int64      value)
    {

     if(sqlvar)
     {
    if(sqlvar->sqlscale)
       value = (__int64)(double(value)/pow(10.0,double(sqlvar->sqlscale)));
       switch(sql_type())
       {
         case SQL_TEXT   :
         case SQL_VARYING:
         {
          char text[128];
          sprintf(text,"%Ld",value);
          *this = text;
         }
         break;
         case SQL_FLOAT  : *this = (float )value; break;
         case SQL_DOUBLE : *this = (double)value; break;
         case SQL_D_FLOAT: *this = (double)value; break;
         case SQL_SHORT  : *this   =  (short)value ;break;
         case SQL_LONG   : *this   =  (int  )value ;break;
         case SQL_INT64  : set_raw_data(&value,sizeof(value)); break;
        default   : set_null(true);break;

       }
     }
     return *this;
    }

    TIBField     & TIBField::operator = (  int        value)
    {
     if(sqlvar)
     {
       switch(sql_type())
       {
         case SQL_TEXT   :
         case SQL_VARYING:
         {
          char text[128];
          sprintf(text,"%d",value);
          *this = text;
         }
         break;
         case SQL_FLOAT  : *this = (float   )value; break;
         case SQL_DOUBLE : *this = (double  )value; break;
         case SQL_D_FLOAT: *this = (double  )value; break;
         case SQL_SHORT  : *this = (short   )value ;break;
         case SQL_INT64  : *this = (__int64 )value ;break;
         case SQL_LONG   :  set_raw_data(&value,sizeof(value)); break;
         default         :  set_null(true);break;
       }
     }
     return *this;
    }

    TIBField     & TIBField::operator = (  long           value)
    {
     if(sqlvar)
     {
       switch(sql_type())
       {
         case SQL_TEXT   :
         case SQL_VARYING:
         {
          char text[128];
          sprintf(text,"%d",value);
          *this = text;
         }
         break;
         case SQL_FLOAT  : *this = (float   )value; break;
         case SQL_DOUBLE : *this = (double  )value; break;
         case SQL_D_FLOAT: *this = (double  )value; break;
         case SQL_SHORT  : *this = (short   )value ;break;
         case SQL_INT64  : *this = (__int64 )value ;break;
         case SQL_LONG   :  set_raw_data(&value,sizeof(value)); break;
         default         :  set_null(true);break;
       }
     }
     return *this;
    }


    TIBField     & TIBField::operator = (  short      value)
    {
     if(sqlvar)
     {
       switch(sql_type())
       {
         case SQL_TEXT   :
         case SQL_VARYING:
         {
          char text[128];
          sprintf(text,"%Ld",value);
          *this = text;
         }
         break;
         case SQL_FLOAT  : *this = (float  )value; break;
         case SQL_DOUBLE : *this = (double )value; break;
         case SQL_D_FLOAT: *this = (double )value; break;
         case SQL_INT64  : *this = (__int64)value ;break;
         case SQL_LONG   : *this = (int    )value ;break;
         case SQL_SHORT: set_raw_data(&value,sizeof(value)); break;
        default   : set_null(true);break;

       }

     }
     return *this;
    }

    TIBField     & TIBField::operator = (  float      value)
    {
     if(sqlvar)
     {
       switch(sql_type())
       {
         case SQL_TEXT   :
         case SQL_VARYING:
         {
          char text[256];
          sprintf(text,"%f",value);
          *this = text;
         }
         break;
         case SQL_SHORT  : *this = (short  )value; break;
         case SQL_DOUBLE : *this = (double )value; break;
         case SQL_D_FLOAT: *this = (double )value; break;
         case SQL_INT64  : *this = (__int64)value ;break;
         case SQL_LONG   : *this = (int    )value ;break;
         case SQL_FLOAT  : set_raw_data(&value,sizeof(value)); break;
        default          : set_null(true);break;

       }
     }
     return *this;
    }

    TIBField     & TIBField::operator = (  double     value)
    {
     if(sqlvar)
     {
       switch(sql_type())
       {
         case SQL_TEXT   :
         case SQL_VARYING:
         {
          char text[256];
          sprintf(text,"%lf",value);
          *this = text;
         }
         break;
         case SQL_SHORT  : *this = (short  )value; break;
         case SQL_FLOAT  : *this = (float  )value; break;
         case SQL_D_FLOAT: *this = (double )value; break;
         case SQL_INT64  : *this = (__int64)value ;break;
         case SQL_LONG   : *this = (int    )value ;break;
         case SQL_DOUBLE : set_raw_data(&value,sizeof(value)); break;
        default          : set_null(true);break;

       }
     }
     return *this;
    }






  string    __fastcall TIBField::get_string(bool r_trim ,bool l_trim )
  {
     string ret;
     char cvt[512];
     if(!is_null())
     {
     switch(sql_type())
     {
        case SQL_TEXT   :
                   ret.assign(sqlvar->sqldata,sqlvar->sqllen);
                   break;
        case SQL_VARYING:
                   {
                    PARAMVARY * vary = (PARAMVARY*)sqlvar->sqldata;
                    ret.assign((char*)vary->vary_string,vary->vary_length);
                   }
                   break;
        case SQL_SHORT    :
        case SQL_LONG     :
        case SQL_QUAD     :
        case SQL_INT64    :
                              _i64toa(get_int64(),cvt,10);
                              ret =cvt;
                              break;
        case SQL_FLOAT    :
        case SQL_DOUBLE   :
        case SQL_D_FLOAT  :
                               {
                               int len = sprintf(cvt,"%lf",get_double());
                               while(len)
                                {
                                 switch(cvt[--len])
                                 {
                                  case '0': cvt[len] = 0;break;
                                  case '.': cvt[len] = 0;
                                  default : len = 0;break;
                                 }
                                }
                               ret = cvt;
                               }
                               break;
        case SQL_TIMESTAMP:
        case SQL_TYPE_TIME:
        case SQL_TYPE_DATE:
                            {
                              SYSTEMTIME st = get_date_time();
                              int dtl = GetDateFormat(0,0,&st,"dd'.'MM'.'yyyy",cvt,sizeof(cvt));
                              dtl--;
                              *(cvt+dtl) = ' ';
                              dtl++;
                              dtl+=GetTimeFormat(LOCALE_USER_DEFAULT,0,&st,"HH':'mm':'ss",cvt+dtl,sizeof(cvt)-dtl);
                              wsprintf(cvt+dtl-1,".%03d",(DWORD)st.wMilliseconds);
                              ret = cvt;
                            }
                            break;

     }
    }
    if(l_trim)
    {
     string::iterator ptr = ret.begin(),end = ret.end();
     while(ptr< end && (*ptr == ' ' || *ptr == '\t') )
           ++ptr;
     ret.erase(ret.begin(),ptr);

    }

    if(r_trim)
    {
     string::iterator ptr = ret.end(),beg = ret.begin();
     while(--ptr>beg && (*ptr == ' ' || *ptr == '\t'))
          ;
     ret.erase(++ptr,ret.end());
    }

    return ret;
  }

  int          __fastcall TIBField::get_string(char * out ,int out_sz,bool r_trim ,bool l_trim )
  {
     int ret   = 0;
     char cvt[512];
     char *beg_ptr;

     if(!is_null())
     {
     switch(sql_type())
     {
        case SQL_TEXT   :
                   ret     = sqlvar->sqllen;
                   beg_ptr = sqlvar->sqldata;
                   break;
        case SQL_VARYING:
                   {
                    PARAMVARY * vary = (PARAMVARY*)sqlvar->sqldata;
                    ret              = vary->vary_length;
                    beg_ptr          = vary->vary_string;
                   }
                   break;
        case SQL_SHORT    :
        case SQL_LONG     :
        case SQL_QUAD     :
        case SQL_INT64    :
                              beg_ptr = cvt;
                              _i64toa(get_int64(),beg_ptr,10);
                              ret     = strlen(beg_ptr);
                              break;
        case SQL_FLOAT    :
        case SQL_DOUBLE   :
        case SQL_D_FLOAT  :
                               {
                               int len = ret = sprintf(cvt,"%lf",get_double());
                               while(len)
                                {
                                 switch(cvt[--len])
                                 {
                                  case '0': cvt[len] = 0;--ret;break;
                                  case '.': cvt[len] = 0;--ret;
                                  default : len = 0;break;
                                 }
                                }
                               beg_ptr = cvt;
                               }
                               break;
        case SQL_TIMESTAMP:
        case SQL_TYPE_TIME:
        case SQL_TYPE_DATE:
                            {
                              SYSTEMTIME st = get_date_time();
                              int dtl = GetDateFormat(0,0,&st,"dd'.'MM'.'yyyy",cvt,sizeof(cvt));
                              dtl--;
                              *(cvt+dtl) = ' ';
                              dtl++;
                              dtl+=GetTimeFormat(LOCALE_USER_DEFAULT,0,&st,"HH':'mm':'ss",cvt+dtl,sizeof(cvt)-dtl);
                              wsprintf(cvt+dtl-1,".%03d",(DWORD)st.wMilliseconds);
                              ret = strlen(cvt);
                              beg_ptr = cvt;
                            }
                            break;

     }
    }

    if(l_trim)
    {
     while(ret && (*beg_ptr == ' ' || *beg_ptr == '\t')  )
         --ret,++beg_ptr;
    }

    if(r_trim)
    {
     char *e_ptr = beg_ptr+ret-1;

     while(e_ptr>beg_ptr && (*e_ptr == ' ' || *e_ptr == '\t'))
        --ret,--e_ptr;
    }
    ret = std::min(ret,out_sz);
    strncpy(out,beg_ptr,ret);
    return ret;
  }

  int          __fastcall TIBField::get_string_len()
  {
   int ret = 0;
   if(!is_null())
   {
    switch(sql_type())
    {
        case SQL_TEXT   :
                   ret     = sqlvar->sqllen;
                   break;
        case SQL_VARYING:
                    ret              = ((PARAMVARY*)sqlvar->sqldata)->vary_length;
                   break;

        default:   {
                    char tmp[1024];
                    ret = get_string(tmp,sizeof(tmp)); 
                   }             
    }
   }
   return ret;
  }


  __int64   __fastcall TIBField::get_int64 (__int64 null_replace )
  {
   __int64 ret = null_replace;
     ib_variant ibv(sqlvar);

     if(!is_null() && ibv.data_len)
     {
     switch(sql_type())
     {
        case SQL_TEXT   :
        case SQL_VARYING:
                   {
                    string str = this->get_string();
                    ret = _atoi64(str.c_str());
                   }
                   break;
        case SQL_SHORT    :   ret = *ibv.s_ptr  ;break;
        case SQL_LONG     :   ret = *ibv.i_ptr  ;break;
        case SQL_TIMESTAMP:   ret = *(__int64*)ibv.timestamp;break;
        case SQL_INT64    :   ret = *ibv.i64_ptr;break;
        case SQL_FLOAT    :   ret = *ibv.f_ptr  ;break;
        case SQL_DOUBLE   :   ret = *ibv.d_ptr  ;break;
        case SQL_D_FLOAT  :   ret = *ibv.ld_ptr  ;break;
        case SQL_TYPE_TIME:
        case SQL_TYPE_DATE:
        case SQL_QUAD     :
        case SQL_BLOB     :  ret = *(__int64*)ibv.gdsq_ptr;break;
        case SQL_ARRAY    :
        break;
     }
    if(sqlvar->sqlscale)
       ret = (__int64)(double(ret)* pow(10.0,double(sqlvar->sqlscale)));
    }
    return ret;
  }

  int  __fastcall TIBField::get_int (int null_replace)
  {
     int ret = null_replace;
     ib_variant ibv(sqlvar);

     if(!is_null() && ibv.data_len)
     {
     switch(sql_type())
     {
        case SQL_TEXT   :
        case SQL_VARYING:
                   {
                    string str = this->get_string();
                    ret = atoi(str.c_str());
                   }
                   break;
        case SQL_SHORT  :   ret = *ibv.s_ptr  ;break;
        case SQL_LONG   :   ret = *ibv.i_ptr  ;break;
        case SQL_INT64  :   ret = *ibv.i64_ptr;break;
        case SQL_FLOAT  :   ret = *ibv.f_ptr  ;break;
        case SQL_DOUBLE :   ret = *ibv.d_ptr  ;break;
        case SQL_D_FLOAT:   ret = *ibv.ld_ptr  ;break;
     }
    }
    return ret;
  }


  short  __fastcall TIBField::get_short (short null_replace)
  {
     short ret = null_replace;
     ib_variant ibv(sqlvar);
     if(!is_null() && ibv.data_len)
     {
     switch(sql_type())
     {
        case SQL_TEXT   :
        case SQL_VARYING:
                   {
                    string str = this->get_string();
                    ret = atoi(str.c_str());
                   }
                   break;
        case SQL_SHORT  :   ret = *ibv.s_ptr  ;break;
        case SQL_LONG   :   ret = *ibv.i_ptr  ;break;
        case SQL_INT64  :   ret = *ibv.i64_ptr;break;
        case SQL_FLOAT  :   ret = *ibv.f_ptr  ;break;
        case SQL_DOUBLE :   ret = *ibv.d_ptr  ;break;
        case SQL_D_FLOAT:   ret = *ibv.ld_ptr  ;break;
     }
    }
    return ret;
  }

  float  __fastcall TIBField::get_float (float null_replace)
  {
     float ret = null_replace;
     ib_variant ibv(sqlvar);
     if(!is_null() && ibv.data_len)
     {
     switch(sql_type())
     {
        case SQL_TEXT   :
        case SQL_VARYING:
                   {
                    string str = this->get_string();
                    ret = atof(str.c_str());
                   }
                   break;
        case SQL_SHORT  :   ret = *ibv.s_ptr  ;break;
        case SQL_LONG   :   ret = *ibv.i_ptr  ;break;
        case SQL_INT64  :   ret = *ibv.i64_ptr;break;
        case SQL_FLOAT  :   ret = *ibv.f_ptr  ;break;
        case SQL_DOUBLE :   ret = *ibv.d_ptr  ;break;
        case SQL_D_FLOAT:   ret = *ibv.ld_ptr  ;break;
     }
    }
    return ret;
  }

  double  __fastcall TIBField::get_double (double null_replace)
  {
     double ret = null_replace;
     ib_variant ibv(sqlvar);
     if(!is_null() && ibv.data_len)
     {
     switch(sql_type())
     {
        case SQL_TEXT   :
        case SQL_VARYING:
                   {
                    string str = get_string();
                    char * e_ptr;
                    ret = strtod(str.c_str(),&e_ptr);
                   }
                   break;
        case SQL_SHORT    :   ret = *ibv.s_ptr  ;break;
        case SQL_LONG     :   ret = *ibv.i_ptr  ;break;
        case SQL_INT64    :   ret = *ibv.i64_ptr;break;
        case SQL_FLOAT    :   ret = *ibv.f_ptr  ;break;
        case SQL_DOUBLE   :   ret = *ibv.d_ptr  ;break;
        case SQL_D_FLOAT  :   ret = *ibv.ld_ptr  ;break;
        case SQL_TIMESTAMP:   ret = *(double*)ibv.timestamp;break;
     }
    if(sqlvar->sqlscale)
       ret  *= pow(10.0,double(sqlvar->sqlscale));
    }
    return ret;
  }


  long double  __fastcall TIBField::get_long_double (long double null_replace)
  {
     double ret = null_replace;
     ib_variant ibv(sqlvar);
     if(!is_null() && ibv.data_len)
     {
     switch(sql_type())
     {
        case SQL_TEXT   :
        case SQL_VARYING:
                   {
                    string str = get_string();
                    char * e_ptr;
                    ret = _strtold(str.c_str(),&e_ptr);
                   }
                   break;
        case SQL_SHORT  :   ret = *ibv.s_ptr  ;break;
        case SQL_LONG   :   ret = *ibv.i_ptr  ;break;
        case SQL_INT64  :   ret = *ibv.i64_ptr;break;
        case SQL_FLOAT  :   ret = *ibv.f_ptr  ;break;
        case SQL_DOUBLE :   ret = *ibv.d_ptr  ;break;
        case SQL_D_FLOAT:   ret = *ibv.ld_ptr ;break;
     }
    if(sqlvar->sqlscale)
       ret *= powl(10.0,(long double)sqlvar->sqlscale);
    }

    return ret;
  }

 struct tm  __fastcall TIBField::get_tm()
 {

     struct tm _tm  = {0};
     ib_variant ibv(sqlvar);
     if(!is_null() && ibv.data_len)
     {
      switch(sql_type())
      {
       case SQL_TIMESTAMP:
            isc_decode_timestamp(ibv.timestamp,&_tm);
            break;
       case SQL_TYPE_TIME:
            isc_decode_sql_time(ibv.time,&_tm);
            break;
       case SQL_TYPE_DATE:
            isc_decode_sql_date(ibv.date,&_tm);
            break;
      }
     }
    return _tm;
 }

 SYSTEMTIME  __fastcall TIBField::get_date_time()
 {
     struct tm _tm  = {0};
     SYSTEMTIME ret = {0};
     ib_variant ibv(sqlvar);
     if(!is_null() && ibv.data_len)
     {
      switch(sql_type())
      {
       case SQL_TIMESTAMP:
            isc_decode_timestamp(ibv.timestamp,&_tm);
            ret.wMilliseconds   = div(ibv.gdsq_ptr->gds_quad_low,ISC_TIME_SECONDS_PRECISION).rem/10;
            break;
       case SQL_TYPE_TIME:
            isc_decode_sql_time(ibv.time,&_tm);
            ret.wMilliseconds   = div(ibv.gdsq_ptr->gds_quad_low,ISC_TIME_SECONDS_PRECISION).rem/10;
            break;
       case SQL_TYPE_DATE:
            isc_decode_sql_date(ibv.date,&_tm);
            break;
      }
      ret.wYear         = _tm.tm_year+1900;
      ret.wMonth        = _tm.tm_mon+1 ;
      ret.wDay          = _tm.tm_mday;
      ret.wHour         = _tm.tm_hour;
      ret.wMinute       = _tm.tm_min;
      ret.wSecond       = _tm.tm_sec;
      ret.wDayOfWeek    = _tm.tm_wday;
     }
   return ret;
 }

  TIBField     & TIBField::operator = (  SYSTEMTIME &   st)
  {
   struct tm _tm = {0};
   _tm.tm_sec   = st.wSecond;
   _tm.tm_min   = st.wMinute;
   _tm.tm_hour  = st.wHour;
   _tm.tm_year  = st.wYear-1900;
   _tm.tm_mon   = st.wMonth-1;
   _tm.tm_mday  = st.wDay  ;
   ib_variant ibv(sqlvar);
   if(sqlvar)
   {
   switch(this->sql_type())
   {
     case SQL_TEXT:
     case SQL_VARYING:
     {
      char date_time [256];
      int dtl = 0;
      if(st.wYear && st.wMonth && st.wDay)
      {
      dtl = GetDateFormat(0,0,&st,"yyyy'-'MM'-'dd",date_time,sizeof(date_time));
      dtl--;
      *(date_time+dtl) = ' ';
      dtl++;
      }

      dtl+=GetTimeFormat(LOCALE_USER_DEFAULT,0,&st,"HH':'mm':'ss",date_time+dtl,sizeof(date_time)-dtl);
      dtl--;
      wsprintf(date_time+dtl,".%03d",(DWORD)st.wMilliseconds);
      *this = date_time;
     }
     break;
     case SQL_TIMESTAMP:
          isc_encode_timestamp(&_tm,ibv.timestamp);
          ibv.gdsq_ptr->gds_quad_low+= (st.wMilliseconds*10/*ISC_TIME_SECONDS_PRECISION*/);
          this->set_null(false);
          break;
     case SQL_TYPE_TIME:
          isc_encode_sql_time(&_tm,ibv.time);
          ibv.gdsq_ptr->gds_quad_low+=(st.wMilliseconds*10/*ISC_TIME_SECONDS_PRECISION*/);
          this->set_null(false);
          break;
     case SQL_TYPE_DATE:
          isc_encode_sql_date(&_tm,ibv.date);
          isc_decode_sql_date(ibv.date,&_tm);
          this->set_null(false);
          break;
   }
   }
   return *this;
  }







 const char  * __fastcall TIBBlobField::get_ib_class_name() const
 {return "TIBBlobField";}

 TIBBlobField::TIBBlobField(TIBSqlQuery & query,int index,bool _input):
 input(_input),HDB(query.get_db_handle()),HTR(query.get_tr_handle()),HBLOB(NULL)
 {
   sqlvar = input ? query.get_input_fields()[index]:query.get_output_fields()[index];
   if(sql_type() != SQL_BLOB)
    memset(&sqlvar,NULL,sizeof(sqlvar));
 };

 TIBBlobField::TIBBlobField(TIBField & f):
 HDB(f.get_query()->get_db_handle()),HTR(f.get_query()->get_tr_handle()),HBLOB(NULL)
 {
   input =  f.get_sqlda() == &f.get_query()->get_input_fields() ? true:false;
   sqlvar = (XSQLVAR*)f();
   idx = f.field_number();
   query = f.get_query ();
   sqlda = f.get_sqlda();
 }


 void  __fastcall TIBBlobField::get_blob_info()
 {
       char blob_items[] = {isc_info_blob_total_length,isc_info_blob_num_segments,isc_info_blob_type,isc_info_blob_max_segment};
       char values    [20] = {0};
       for(int i = 0;i<sizeof(blob_items);i++)
       {
        memset(values,0,sizeof(values));
        status = isc_blob_info(status_vector,&HBLOB,1,&blob_items[i],sizeof(values),(char*)values);
        if(is_status_ok())
         {
          char * p   = values+1;
          short  len = isc_portable_integer((ISC_UCHAR*)p,sizeof(short));
          p+=2;

          switch(blob_items[i])
          {
           case isc_info_blob_total_length :
                blob_length = isc_portable_integer((ISC_UCHAR*)p,len);
                break;
           case isc_info_blob_num_segments:
                blob_segments = isc_portable_integer((ISC_UCHAR*)p,len);
                break;
           case isc_info_blob_type :
                blob_type = isc_portable_integer((ISC_UCHAR*)p,len);
                break;
           case isc_info_blob_max_segment :
                blob_max_segment = isc_portable_integer((ISC_UCHAR*)p,len);
                break;
          }
         }

       }

 }

  bool  __fastcall TIBBlobField::open_blob()
  {

    blob_length =  blob_segments = blob_max_segment =  blob_type = 0;

    if(is_blob() && ( this->input || !is_null() ))
      {
       if(input)
         status = isc_create_blob2(status_vector,&HDB,&HTR,&HBLOB,(ISC_QUAD*)sqlvar->sqldata,0,NULL);
       else
       status = isc_open_blob2(status_vector,&HDB,&HTR,&HBLOB,(ISC_QUAD*)sqlvar->sqldata,0,NULL);
       if(is_status_ok()) get_blob_info();
      }
     return is_status_ok();
  }


 int   __fastcall TIBBlobField::read_segment(void * buffer,int len)
 {
   int ret = 0;
   if(!HBLOB)
   {
    //Открыть
    open_blob();
   }
   if(HBLOB && buffer && len)
      {
       status = isc_get_segment(status_vector,&HBLOB,(unsigned short*)&ret,len,(char*)buffer);
      }
   return ret;
 }

 int   __fastcall TIBBlobField::write_segment(void * buffer,int len)
 {
   int ret = 0;
   if(buffer && len)
   {
    set_null(false);
    status = isc_put_segment(status_vector,&HBLOB,WORD(len),(char*)buffer);
    if(!status)
      ret = len;
   }
   return ret;

 }

 void  __fastcall TIBBlobField::close()
 {
  if(HBLOB)
      isc_close_blob(status_vector,&HBLOB);
 }


  TIBEvents::TIBEvents(isc_db_handle _hdb )
  :active(FALSE),buffer_length(0),event_buf(NULL),
  events_result(NULL),events_id(0),HDB(_hdb),
  user_data(NULL),event_notify(NULL),first_time(FALSE)
 { };


 const char  * __fastcall TIBEvents::get_ib_class_name() const
 {return "TIBEvents";}

 bool       __fastcall   TIBEvents::wait(bool async)
 {
      if(async)
      {
       status = isc_que_events(status_vector,&HDB,&this->events_id,this->buffer_length,event_buf,(ISC_EVENT_CALLBACK)event_handler,this);
       active = is_status_ok() ? TRUE:FALSE;
      }
      else
      {
       events_id = 0;
       status = isc_wait_for_event(status_vector,&HDB,buffer_length,event_buf,events_result);
       if(is_status_ok())
          {
           event_handler(this,buffer_length,events_result);
          }
      }

   return active ? true:false;
 }

void       __fastcall   TIBEvents::enable(isc_db_handle _hdb)
 {
      locker.Lock();
      disable();
      if(_hdb)
        this->HDB = _hdb;
      if(!this->buffer_length)
      {
       ISC_UCHAR* array[MAX_EVENTS_COUNT+5] = {0};
       for(int i = 0;i<this->get_events_count();i++)
          array[i] = (ISC_UCHAR*)this->event_names[i].data();
       buffer_length = isc_event_block(&event_buf,&events_result,get_events_count(),
                                        array[0 ],array[1 ],array[2 ],array[3 ],array[4 ],
                                        array[5 ],array[6 ],array[7 ],array[8 ],array[9 ],
                                        array[10],array[11],array[12],array[13],array[14]);
      first_time = TRUE;
      wait(false);
      }
      first_time = active = FALSE;
      locker.Unlock();
 }

 void       __fastcall   TIBEvents::disable()
 {
  locker.Lock();
  if(buffer_length)
  {
   first_time = TRUE;
   active     = FALSE;
   status     = isc_cancel_events(status_vector,&HDB,&events_id);
   if(event_buf)
    isc_free((char*)event_buf),event_buf = NULL;
   if(events_result)
    isc_free((char*)events_result),events_result = NULL;
   buffer_length = 0;
  }
  locker.Unlock();
 }


 void TIBEvents::event_handler(TIBEvents * events,ISC_USHORT length, ISC_UCHAR* fired)
 {
   ISC_ULONG res_vector[20] = {0};
   events->locker.Lock();
__try{
   isc_event_counts(res_vector,length,events->event_buf,fired);
   int mask = 0,total_count = 0;
   if(!events->first_time  )
   {
   for(int i = MAX_EVENTS_COUNT;i>0;)
   {
    ISC_ULONG count = res_vector[--i];
    mask<<=1;
    if(count )
      {
       mask|= 1;
       total_count+=count;
       if(events->event_notify)
          events->event_notify(events->user_data,events,i,count);
      }
    }
    isc_event_counts(res_vector,length,events->event_buf,fired);
    if(mask && total_count && events->event_notify_mask )
       events->event_notify_mask(events->user_data_mask,events,mask,total_count);
   }

   events->first_time = false;
   if(events->active)
        events->wait(true);
   events->locker.Unlock();
  }
  __except(EXCEPTION_EXECUTE_HANDLER){events->locker.FullUnlock();};

 }



 TIBRecords & TIBRecords::operator = (TIBSqlQuery &q)
 {
  clear();
  rec_no = 0;
  return (*this)+=q;
 }

 TIBRecords & TIBRecords::operator += (TIBSqlQuery &q)
 {
  TIBXSQLDA xda;
  while(!q.is_eos())
  {
   push_back(q.get_output_fields());
   //*rbegin() = q.get_output_fields();
   q.fetch();
  }
  return *this;
 }

  void     __fastcall TIBRecords::erase(int _rec_no)
  {
   if(_rec_no<(int)size())
   {
    vector<TIBXSQLDA>::iterator ptr = begin();
    advance(ptr,_rec_no);
    vector<TIBXSQLDA>::erase(ptr);
   }
  }

 int      __fastcall TIBRecords::insert_record(int pos )
 {
  int ret = pos;
  if(ret < 0 || ret >(int)size())
     ret = size()-1;
  if(ret >=0)
    {
      TIBRecords::iterator ptr = begin();
      std::advance(ptr,ret);
      ptr = insert(ptr,at(0));
      TIBField f(ptr,0);
      while(f.field_number()>=0)
         f<<ib_null;

    }
  return ret;
 }


 TIBField   TIBRecords::operator()(int r,int field)
 {
  TIBField ret;
  if((size_type)r<this->size())
  {
   TIBXSQLDA & xda = at(r);
   ret = TIBField(&xda,field);
  }
  return ret;
 }

 TIBField   TIBRecords::operator()(int r,char * field)
 {
  TIBField ret;
  if((size_type)r<this->size())
  {
   TIBXSQLDA & xda = at(r);
   ret = TIBField(&xda,field);
  }
  return ret;
 }

 const TIBXSQLDA & __fastcall TIBRecords::get_fields(int _rec_no  )
 {
  if(_rec_no<0) _rec_no = this->rec_no;
  TIBXSQLDA & xda = at(_rec_no);
  return xda;
 }


#ifndef _NO_NAMESPACE
} // end of namespace
#endif





