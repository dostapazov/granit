/******************************************************************************/
/*  Авторские права Остапенко Д.В. DOSTAP@NORCOM.RU                           */
/*  Интерфейс классов для работы с Interbase через IB-API (GDS32.DLL)         */
/*                                                                            */
/******************************************************************************/

#ifndef  __IBCLASS_INC
#define  __IBCLASS_INC


#ifdef __BORLANDC__
 #ifndef NO_VCL
  #pragma message ("ibclass.h include  VCL header" )
  #include <vcl.h>
  #else
  #pragma message ("ibclass.h **NOT** include  VCL header" )
 #endif
#endif

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>

#ifdef __cplusplus
 }
#endif

#include <vector>
#include <map>
#include <algorithm>
#include <iterator>
#include <string>

#ifndef  IB_NULL_REPLACE_INT
#define  IB_NULL_REPLACE_INT       (int(-1))
#endif

#ifndef  IB_NULL_REPLACE_SMALLINT
#define  IB_NULL_REPLACE_SMALLINT  (short(-1))
#endif

#ifndef  IB_NULL_REPLACE_BIGINT
#define  IB_NULL_REPLACE_BIGINT    (__int64(-1))
#endif

#ifndef  IB_NULL_REPLACE_FLOAT
#define  IB_NULL_REPLACE_FLOAT    (float(0))
#endif

#ifndef  IB_NULL_REPLACE_DOUBLE
#define  IB_NULL_REPLACE_DOUBLE    (double(0))
#endif

namespace fb_inc {

extern "C" {
  #include <ibase.h>
 }
}

#include <kertl.hpp>
#include <keclosure.hpp>
#include <dll_procs.hpp>

#ifndef _NO_NAMESPACE
namespace KeRTL
{
using namespace KeRTL;
#endif

using namespace std;
using namespace fb_inc;

class TIBParams
{
protected:
vector<char>  params;
public:
	void __fastcall Revert(bool full = false);
	void __fastcall add_password    (LPSTR Passw);
	int  __fastcall get_password    (LPSTR Passw,int sz);
	void __fastcall add_user        (LPSTR UserName);
	int  __fastcall get_user        (LPSTR UserName,int sz);

	void __fastcall add_param       (BYTE  type, BYTE * data = 0 ,BYTE sz = 0);
        TIBParams(BYTE ver ){params.reserve(128); if(ver)params.push_back(ver);} ;
        TIBParams(LPBYTE  src,int sz )  {copy(src,src+sz,std::back_inserter(params));} ;
	virtual  ~TIBParams();
        char *  __fastcall  get_ib_params() const;
	short   __fastcall  get_params_size() const;
        TIBParams & operator = (const TIBParams & p) {params.clear(); copy(p.params.begin(),p.params.end(),back_inserter(params));return * this;}
};

class TIBTransParams:public TIBParams
{
 public:
    TIBTransParams():TIBParams(isc_tpb_version3){};
};


  class TIBBase;
 /*Состояния уведомлений*/
 #define  ibn_before 0
 #define  ibn_after  1

 typedef BOOL (WINAPI * pfn_ib_notify_proc)(LPVOID user_data,int phase, int type,TIBBase * who);

 struct notify_param
 {

  notify_param():type(-1),user_data(NULL),notify_proc(NULL){}
  notify_param(int _type,LPVOID _ud,pfn_ib_notify_proc _np ):type(_type),user_data(_ud),notify_proc(_np){}
  notify_param(const notify_param & np){*this = np;}
  notify_param & operator = (const notify_param & np){type = np.type;user_data = np.user_data;notify_proc = np.notify_proc;return *this;}
  bool   operator ()(int phase,TIBBase * who);
  int type;
  LPVOID               user_data;
  pfn_ib_notify_proc   notify_proc;
 };

 typedef TClosureMethod<LPVOID,TIBBase *,DWORD,DWORD> Tmonitor_callback;


 #define IBC_MONF_END isc_info_end

 #define IBC_MON_DB_CONNECT            0x80000000
 #define IBC_MON_DB_DISCONNECT         0x40000000

 #define IBC_MON_TRANSACTION_START     0x08000000
 #define IBC_MON_TRANSACTION_COMMIT    0x04000000
 #define IBC_MON_TRANSACTION_ROLLBACK  0x02000000
 #define IBC_MON_TRANSACTION_RETAIN    0x01000000

  #define IBC_MON_SQL_PREPARE          0x00800000
  #define IBC_MON_SQL_OPEN             0x00400000
  #define IBC_MON_SQL_FETCH            0x00200000
  #define IBC_MON_SQL_CLOSE            0x00100000


 class TIBBase
 {
  protected:
  TFASTMutex locker;
  static TFASTMutex mon_locker;
  vector<notify_param>    notify_list;
  static   Tmonitor_callback monitor_callback;
  TIBBase():  _prof_start_time(0) , _prof_long_time (0){}
  virtual ~TIBBase(){Release();}
  virtual bool __fastcall notifyer (int phase,int type);
  DWORDLONG   _prof_start_time;
  DWORDLONG   _prof_long_time;
  void __fastcall profile_start(){_prof_start_time = GetTime(false);}
  DWORDLONG __fastcall profile_stop(){_prof_long_time = GetTime(false) - _prof_start_time; return _prof_long_time;}
  void __fastcall monitor_call(DWORD code);
  public:
  virtual void __fastcall Release  () {notify_list.clear();};
  void __fastcall reg_notify  (int type,LPVOID _ud,pfn_ib_notify_proc _np);
  void __fastcall unreg_notify(int type,LPVOID _ud,pfn_ib_notify_proc _np);
  bool __fastcall lock  (){return locker.Lock();}
  void __fastcall unlock(){locker.Unlock();}
  virtual const char  * __fastcall get_ib_class_name() const = 0;
  static void set_monitor_callback(LPVOID cb_data,Tmonitor_callback::closure_method fptr);
  DWORDLONG __fastcall get_profile_start(){return _prof_start_time;}
  DWORDLONG __fastcall get_profile_long (){return _prof_long_time;}
  static bool      __fastcall init_ib_lib(const char * lib_name);
  static bool      __fastcall is_clib_loaded();
  static void      __fastcall get_client_version(char * buf);
  static ISC_INT64 __fastcall get_portable_integer(const ISC_UCHAR*,  short);

 };

 typedef std::vector<TIBBase*> _ibobj_vector;

 struct  ibobj_less
 {
  ibobj_less(){};
  ibobj_less(const ibobj_less & iol){};

  bool operator ()(const TIBBase * v1,const TIBBase * v2)
  {   return int(v1)<int(v2); }
 };

 class ibobj_vector : public _ibobj_vector
 {
  public :
  ibobj_vector(){};
  int  operator += (const TIBBase * b);
  int  operator -= (const TIBBase * b);
  bool operator () (const TIBBase * b);
 };


 class TIBStatus;
 #ifdef INC_VCL

  class TIBException:public Sysutils::Exception
 #else
  
  class TIBException
 #endif
 {
  protected:
  ISC_STATUS    err_code;

  #ifdef INC_VCL
  AnsiString    exception_object  ;
  #else
  char          exception_object  [256 ];
  char          exception_message [2048];
  #endif
  public:
  TIBException  (const TIBStatus * _except_src);
  TIBException  (const char * message);
  TIBException  (const TIBException & src);
  TIBException &  operator =(const TIBException & src);
  const char *   operator()() const;
  const char *   __fastcall get_except_obj() const ;
  ISC_STATUS     __fastcall get_error_code() const {return err_code;} 
 };


 class TIBStatus:public TIBBase
 {
  protected:
  ISC_STATUS status;
  ISC_STATUS status_vector[20];
  BOOL       enable_exceptions;

  TIBStatus()
  {
  enable_exceptions =
  #ifdef _IBCLASS_EXCEPT_ENABLE
  TRUE
  #else
  FALSE
  #endif
  ;
  status = 0;ZeroMemory(status_vector,sizeof(status_vector));
  }
  TIBStatus(const TIBStatus & ibst){*this = ibst;}
  virtual ~TIBStatus(){};
  public:
  void  __fastcall  interprete(char * out)
  { if(out)
   {
     ISC_STATUS *  pvector = status_vector;
     isc_interprete(out,&pvector);
    }
  }

  int        __fastcall get_error_text  (char * text ,int bsz , int offs = 0, int cnt = 20) const ;
  string     __fastcall get_error_text  (int offs = 0,int cnt = 20) const;
  bool       __fastcall is_status_ok    (bool no_except = false   ) const throw (TIBException &);

  ISC_STATUS __fastcall get_status_code(int idx) const;
  TIBStatus & operator = (const TIBStatus & ibst){enable_exceptions = ibst.enable_exceptions; status = ibst.status;memcpy(status_vector,ibst.status_vector,sizeof(status_vector));return *this;}
  void __fastcall enable_exception(BOOL enable)  {enable_exceptions = enable;}
  const char  * __fastcall get_ib_class_name() const {return "TIBStatus";}

 };

struct  ib_user_def
{
 int         group_id;
 int         user_id;
 string      user_name;
 string      first_name;
 string      middle_name;
 string      last_name;
 ib_user_def()
 {  user_id     =   group_id    = -1; }
};




typedef std::vector<ib_user_def> ib_users_list;
//isc_spb_sts_db_log|
#define IB_SVC_ALL_OPT (isc_spb_sts_data_pages|isc_spb_sts_hdr_pages|isc_spb_sts_idx_pages|isc_spb_sts_sys_relations|isc_spb_sts_record_versions|isc_spb_sts_table)

class TIBService:protected TIBParams,public TIBStatus
{
 isc_svc_handle  svc_handle;
 bool   __fastcall fetch_users(ib_users_list & ul,char * buff);
 void __fastcall Release(){detach();TIBStatus::Release();}

 public:
 TIBService():TIBParams(0),svc_handle(0){}
 virtual ~TIBService(){};
 #ifdef __clang__
 using TIBParams::add_param;
 #else
 TIBParams::add_param;
 #endif

 void __fastcall add_spb_param_string(BYTE type,const char * str);
 void __fastcall add_spb_param_number(BYTE type,DWORD);
 bool __fastcall attach(const char * srv,const char * dba_user,const char * dba_pass);
 void __fastcall detach();
 bool __fastcall add_user(const char * user,const char * passw,const char * Fname,const char * Mname,const char * SName);
 bool __fastcall chg_user(const char * user,const char * passw,const char * Fname,const char * Mname,const char * SName);
 bool __fastcall rem_user(const char * user);
 bool __fastcall display_users(ib_users_list & ul);

 bool __fastcall backup  (const char * db_name,const char * bkp_name,ISC_ULONG opt,bool verbose = false);
 bool __fastcall restore (const char * db_name,const char * bkp_name,ISC_ULONG opt,ISC_ULONG page_size = 0,ISC_ULONG buffers = 0,bool ro=false,bool verbose = false);
 bool __fastcall is_running();
 bool __fastcall svc_query(int spb_len,char * spb,int req_len,char * req,int buf_len,char *buf);
 bool __fastcall get_svc_line(string & str);
 bool __fastcall get_svc_text(string & str);
 bool __fastcall get_statistics(char * db_name,unsigned long opt = IB_SVC_ALL_OPT);
 bool __fastcall sweep   (const char * db_name);
 bool __fastcall shutdown(const char * db_name,int timeout = 10,int type = 0);
 bool __fastcall online  (const char * db_name);

 const char  * __fastcall get_ib_class_name() const;

};






 #define IBDB_NTYPE_CONNECT        1
 #define IBDB_NTYPE_DISCONNECT     2
 #define IBDB_NTYPE_LOSTCONNECT    6


 class TIBDatabase:public TIBStatus
 {
 protected:
        isc_db_handle HDB;
        LPSTR DbName;
        TIBParams     params;
        ibobj_vector  trans_list;
        int connection_id;
        int reads,writes,fetches,marks;
        ISC_SHORT num_buff;
        ISC_SHORT buff_len;
        void __fastcall set_db_name(LPCSTR _db_name);
        void __fastcall Release();

 public:

 isc_db_handle __fastcall  GetDBHandle() const;
         void  __fastcall  SetDBHandle(isc_db_handle hDb); 
	 void  __fastcall  Disconnect();

	 bool  __fastcall  Connect(LPCSTR db_path,LPCSTR User,LPCSTR Password,
                                   LPCSTR sql_role = NULL,
                                   LPCSTR lc_type = NULL ,
                                   BYTE dialect = 3);
	 bool  __fastcall  Connect(LPCSTR db_path,TIBParams & params);
         bool  __fastcall  Connect();
                           TIBDatabase(int _num_buffs = 0,int _buff_len = 0);
                           TIBDatabase (const TIBDatabase & src);
                   virtual ~TIBDatabase(){if(is_connected()) Disconnect();};

     isc_db_handle   operator ()() {return HDB;}
         bool  __fastcall get_info(int req_len,char * req,int buf_len,char * buf);
         int   __fastcall get_active_trans_count();
         int   __fastcall get_connection_id(){return connection_id;};
         int   __fastcall do_get_connection_id();
         int   __fastcall get_reads  ();
         int   __fastcall get_writes ();
         int   __fastcall get_fetches();
         int   __fastcall get_marks  ();
         int   __fastcall get_int_info(int info_code);

  TIBParams &  __fastcall get_params(){return params;}
  string       __fastcall get_conn_str();
  bool         __fastcall is_connected(){return HDB!= NULL ? true:false;}
  TIBDatabase & operator = (const TIBDatabase & src);
  const char  * __fastcall get_ib_class_name() const;
  ibobj_vector & __fastcall get_trans_list(){return trans_list;}
  void           __fastcall set_num_buff(ISC_SHORT nb){num_buff = nb;}
  void           __fastcall set_buff_len(ISC_SHORT bl){buff_len = bl;}
  static bool    __fastcall make_db_path (const std::string & server_name,const std::string & db_name,std::string & db_path);
  static bool    __fastcall split_db_path(const std::string & db_path    ,std::string & server_name  ,std::string & db_name);
  };


enum  end_trans_action { endtr_default,// Уменьшает счетчик
                         endtr_full   ,// Безусловное завершение
                         endtr_retain  // вызов isc_XXX_retaining при spin_count!=0
                        };

class TIBTransaction:public TIBStatus
{
 protected:
 isc_tr_handle HTR;
 TIBDatabase   *db;
 TIBParams     tran_params;
 int           spin_count;
 int           trans_id;
 public   :
         TIBTransaction ();
         TIBTransaction (TIBDatabase & ibdb);
virtual ~TIBTransaction(){if(is_active()) commit(endtr_full);};
     void __fastcall Release(){commit(endtr_full);TIBStatus::Release();}
 bool  __fastcall    set_database(TIBDatabase * db);
 void  __fastcall    set_params(const TIBParams & params){tran_params = params;  }
 bool  __fastcall    start(TIBDatabase * db = NULL);
 bool  __fastcall    start(const TIBParams & params,TIBDatabase * db = NULL);

 bool  __fastcall    commit  (end_trans_action end_action = endtr_default );
 bool  __fastcall    rollback(end_trans_action end_action = endtr_default);
 isc_tr_handle __fastcall get_tr_handle(){return  HTR;}
 isc_db_handle __fastcall get_db_handle(){return  db ? db->GetDBHandle():NULL;};
 void          __fastcall clear_params(){tran_params.Revert();}
 int           __fastcall get_tr_id   (){return trans_id;}
 bool          __fastcall is_active   (){return HTR!=0 ? true:false;}
 int           __fastcall get_spin_count(){ return spin_count;}
 TIBDatabase * __fastcall get_database(){return db;}
 const char  * __fastcall get_ib_class_name() const;
       void    __fastcall get_params(TIBParams& p){ p = tran_params; };

};

class  TIBXSQLDA
{
 protected:
 XSQLDA    temp;
 XSQLDA  * sqlda;
 LPBYTE    vars_data;
 int       vars_data_size;
 public:
  TIBXSQLDA  (int count = 0);
  TIBXSQLDA  (const TIBXSQLDA & src);
 ~TIBXSQLDA(){free_sqlda();free_vars();};
 int       __fastcall get_fields_count(){return sqlda ? sqlda->sqld:temp.sqld;}
 XSQLDA  *  operator-> ()  {return sqlda ? sqlda:&temp;}
 XSQLDA  *  operator() ()  {return sqlda ? sqlda:&temp;}
 XSQLVAR *  operator[](int idx) {return sqlda && (idx>-1 && idx<sqlda->sqld) ? sqlda->sqlvar+idx :temp.sqlvar;}
 XSQLVAR *  operator[](const char * field_name){return (*this)[get_field_no(field_name)];}
 int        __fastcall get_field_no(const char * field_name);
 void      __fastcall free_sqlda();
 int       __fastcall calc_vars_data_size(bool as_input = false);
 void      __fastcall free_vars();
 bool      __fastcall alloc_sqlda(int cnt);
 void      __fastcall format_vars_data(bool as_input = false);
 int       __fastcall get_raw_data(LPBYTE buff, int bsz);
 TIBXSQLDA & operator = (const TIBXSQLDA & src);
 void      __fastcall set_var_param(int num,char * name,int type,bool nullable,int size = 32);
 int       rdf_counter;
};


struct ib_variant
{
short data_len;
short data_type;
bool  is_null;
union
{
 char            *  c_ptr;
 PARAMVARY       *  vc_ptr;
 __int64         *  i64_ptr;
   int           *  i_ptr;
   short         *  s_ptr;
   float         *  f_ptr;
   double        *  d_ptr;
   long double   *  ld_ptr;
   GDS_QUAD      *  gdsq_ptr;
   ISC_TIME      *  time;
   ISC_DATE      *  date;
   ISC_TIMESTAMP * timestamp;
 };
 ib_variant(XSQLVAR * v)
  {
    if(v)
    c_ptr =  v->sqldata, data_len =  v->sqllen, data_type =  v->sqltype&(~1),is_null = (v->sqlind && *v->sqlind == -1);
    else
    c_ptr =  0, data_len =  0, data_type =  0,is_null = true;
   };
};

struct _ib_null{bool null; _ib_null(bool _v ):null(_v){}; _ib_null():null(true){};};
#define ib_null _ib_null()

class TIBSqlQuery;

class  TIBField
{
 protected:
  TIBSqlQuery * query;
  int           idx;
  TIBXSQLDA   * sqlda;
  XSQLVAR     * sqlvar;
 public:
  TIBField():idx(0),sqlda(NULL), sqlvar (NULL),query(NULL){}
  TIBField(TIBXSQLDA * _sqlda,int _idx,TIBSqlQuery *_q = NULL):idx(_idx),sqlda(_sqlda),sqlvar((*_sqlda)[_idx]),query(_q){}
  TIBField(TIBXSQLDA * _sqlda,const char * f_name,TIBSqlQuery *_q = NULL):sqlda(_sqlda),sqlvar(0),idx(-1),query(_q)
  {idx = sqlda->get_field_no(f_name);sqlvar = (*sqlda)[idx];}
  //TIBField                  (TIBField  & src ) {*this = src;}
  TIBField     & operator = (TIBField   src  ) {idx=src.idx,sqlda=src.sqlda,sqlvar = src.sqlvar; query = src.query; return *this;}

  string       __fastcall name    ();
  string       __fastcall aliase  ();
  string       __fastcall rel_name();
  string       __fastcall owner   ();
  int          __fastcall field_number(){return idx;}
  TIBSqlQuery* __fastcall get_query(){return query;}
  
  ISC_SHORT    __fastcall sql_type()  {return (sqlvar ? sqlvar->sqltype&(~1) :0 );}
  bool         __fastcall is_null ()  {return (idx<0 || !sqlvar || (sqlvar->sqlind && *sqlvar->sqlind == ISC_SHORT(-1))) ? true:false; }
  void         __fastcall set_null(bool null = true){ if(sqlvar && sqlvar->sqlind) *sqlvar->sqlind = null ? -1:0; }
  bool         __fastcall is_blob()   {return ( sql_type() == SQL_BLOB) ? true:false;}
  int          __fastcall get_raw_data (void * raw_buffer,int bsz);
  int          __fastcall set_raw_data (void * raw_buffer,int dsz);
  int          __fastcall compare_raw_data(void * raw_data,int dsz);

  ib_variant   __fastcall get_variant(){return ib_variant(sqlvar);}
  string       __fastcall get_string(bool r_trim = true,bool l_trim = false);
  int          __fastcall get_string(char * out ,int out_sz,bool r_trim = true,bool l_trim = false);
  int          __fastcall get_string_len();
  __int64      __fastcall get_int64 (__int64 null_replace = IB_NULL_REPLACE_BIGINT);
    int        __fastcall get_int   (  int   null_replace = IB_NULL_REPLACE_INT);
    short      __fastcall get_short (  short null_replace = IB_NULL_REPLACE_SMALLINT);
    float      __fastcall get_float ( float  null_replace = IB_NULL_REPLACE_FLOAT);
    double     __fastcall get_double( double null_replace = IB_NULL_REPLACE_DOUBLE);
    long
    double     __fastcall get_long_double(long double null_replace = 0);
    struct tm  __fastcall get_tm();
    SYSTEMTIME __fastcall get_date_time();
    bool       __fastcall get_bool  (){return get_int64() ? true:false;}

    TIBField     & operator = (const char *     value);
    TIBField     & operator = (__int64          value);
    TIBField     & operator = (unsigned __int64 value){return *this = *(__int64*)&value;}
    TIBField     & operator = (  long           value);
    TIBField     & operator = (unsigned  long   value){return *this = *(  long * )&value;}
    TIBField     & operator = (  int            value);
    TIBField     & operator = (unsigned  int    value){return *this = *(  int * )&value;}
    TIBField     & operator = (  short          value);
    TIBField     & operator = (unsigned  short  value){return *this = *( short*)&value;}
    TIBField     & operator = (  float          value);
    TIBField     & operator = (  double         value);
    TIBField     & operator = (  SYSTEMTIME &   st   );
    TIBField     & operator = (  _ib_null       value){set_null(true);return *this ;}
    TIBField     & operator = (  bool  value         ){return *this = (  short)value;}
    /*
    __fastcall operator string          (){return get_string();}
    __fastcall operator short           (){return get_short();}
    __fastcall operator unsigned short  (){return (unsigned short)get_short();}
    */
    __fastcall operator string          (){return get_string();}
    __fastcall operator int             (){return (int)get_int();}
    __fastcall operator unsigned int    (){return (unsigned int)get_int();}
    __fastcall operator short           (){return (int)get_short();}
    __fastcall operator unsigned short  (){return (unsigned short)get_short();}
    __fastcall operator long            (){return (long)get_int();}
    __fastcall operator unsigned long   (){return (unsigned long)get_int();}
    __fastcall operator __int64         (){return get_int64();}
    __fastcall operator unsigned __int64(){return (unsigned __int64)get_int64();}
    __fastcall operator float           (){return get_float();}
    __fastcall operator double          (){return get_double();}
    __fastcall operator long double     (){return get_long_double();}

    __fastcall operator SYSTEMTIME      (){return get_date_time();}
    __fastcall operator struct tm       (){return get_tm();}
    //__fastcall operator bool            (){return get_bool();}


    TIBField       & operator ++();//{ if(sqlda && (*sqlda)->sqld<idx)sqlvar = (*sqlda)[++idx]; return *this;}
    TIBField         operator ++(int){ TIBField ret = *this;++(*this);  return ret;}
    TIBField       & operator --();//{ if(sqlda &&    idx>0          )sqlvar = (*sqlda)[--idx]; return *this;}
    TIBField         operator --(int){ TIBField ret = *this;--(*this);  return ret;}
    const XSQLVAR  * operator   ()() const { return sqlvar;}
    TIBXSQLDA      * __fastcall get_sqlda(){ return sqlda; };


};

inline    TIBField     & operator << (TIBField     & f,const char *     value)
{    f = value; return ++f;    }

inline    TIBField     & operator << (TIBField     & f,__int64          value)
{    f = value; return ++f;    }
inline    TIBField     & operator << (TIBField     & f,unsigned __int64 value)
{    f = value; return ++f;    }
inline    TIBField     & operator << (TIBField     & f,  int            value)
{    f = value; return ++f;    }
inline    TIBField     & operator << (TIBField     & f,unsigned  int    value)
{    f = value; return ++f;    }
inline    TIBField     & operator << (TIBField     & f,  long           value)
{    f = value; return ++f;    }
inline    TIBField     & operator << (TIBField     & f,unsigned  long   value)
{    f = value; return ++f;    }
inline    TIBField     & operator << (TIBField     & f,  short          value)
{    f = value; return ++f;    }
inline    TIBField     & operator << (TIBField     & f,unsigned  short  value)
{    f = value; return ++f;    }
inline    TIBField     & operator << (TIBField     & f,  float          value)
{    f = value; return ++f;    }
inline    TIBField     & operator << (TIBField     & f,  double         value)
{    f = value; return ++f;    }
inline    TIBField     & operator << (TIBField     & f,  SYSTEMTIME &   st   )
{    f = st; return ++f;    }

inline    TIBField     & operator << (TIBField     & f,  _ib_null  nv   )
{    f.set_null(nv.null) ; return ++f;    }

inline    TIBField     & operator << (TIBField     & f,  bool value    )
{    f = (int)value ; return ++f;    } 





inline    TIBField     & operator >> (TIBField     & f,__int64   &      value)
{     value = f.get_int64(); return ++f;    }

inline    TIBField     & operator >> (TIBField     & f,long   &      value)
{     value = f.get_int(); return ++f;    }
inline    TIBField     & operator >> (TIBField     & f,unsigned long   &      value)
{     value = f.get_int(); return ++f;    }



inline    TIBField     & operator >> (TIBField     & f,unsigned __int64 & value)
{    value = f.get_int64(); return ++f;    }
inline    TIBField     & operator >> (TIBField     & f,  int        &    value)
{    value = f.get_int(); return ++f;    }
inline    TIBField     & operator >> (TIBField     & f,unsigned  int &   value)
{    value = f.get_int(); return ++f;    }
inline    TIBField     & operator >> (TIBField     & f,  short       &   value)
{    value = f.get_short(); return ++f;    }
inline    TIBField     & operator >> (TIBField     & f,unsigned  short &  value)
{    value = f.get_short(); return ++f;    }
inline    TIBField     & operator >> (TIBField     & f,  float      &    value)
{    value = f.get_float(); return ++f;    }
inline    TIBField     & operator >> (TIBField     & f,  double     &    value)
{    value = f.get_double(); return ++f;    }

inline    TIBField     & operator >> (TIBField     & f,  SYSTEMTIME &   st   )
{    st = f.get_date_time(); return ++f;    }


inline    TIBField     & operator >> (TIBField     & f,  char * value  )
{    f.get_string(value,f.get_string_len()); return ++f;    }



inline    TIBField     & operator >> (TIBField     & f,  bool & value    )
{    value = f.get_short()? true:false ; return ++f;    }


/*
template <typename T>
inline    TIBField     & operator >> (TIBField     & f,  T value    )
{     value =(T) f ; return ++f;    }

template <typename T>
inline    TIBField & operator >> (TIBField     & f,  T & value    )
{    value =(T)f ; return ++f; }
*/

template <typename T>
inline    bool      operator == (TIBField     & f,  T value    )
{     return  (value ==(T)f) ? true : false ;     }

template <typename T>
inline    bool      operator == ( T value ,TIBField     & f )
{     return  (value ==(T)f) ? true : false ;     }

template <typename T>
inline    bool      operator != (TIBField     & f,  T value    )
{     return  (value !=(T)f) ? true : false ;     }

template <typename T>
inline    bool      operator != ( T value ,TIBField     & f )
{     return  (value !=(T)f) ? true : false ;     }

struct Tnamed_params
{
  typedef std::vector<int> param_numbers;
  typedef std::map<string,param_numbers > named_param;
  named_param  params;
  string qtxt;
  Tnamed_params()   {};
  void __fastcall parse (string & src_txt,string & pure_text);
  void __fastcall parse (const char   * src_txt,string & pure_text);
};

inline  void __fastcall Tnamed_params::parse (const char   * src_txt,string & pure_text)
  {
   string str = src_txt;
   parse(str,pure_text);
  }


 class TIBSqlQuery:public TIBStatus
 {
   protected:
   TIBDatabase      * database;
   TIBTransaction   * transaction;
   isc_stmt_handle   stmt;
   TIBXSQLDA  in_data;  // Входные параметры
   TIBXSQLDA  out_data; // Выходные параметры
   int  rec_no;
   int  sql_type;
   bool opened;
   bool has_cursor;
   bool eos;
   string            SQL;
   int  prepared;
   Tnamed_params      named_params;
   bool       __fastcall    set_unique_cursor();
   void       __fastcall    Release();
   public:
         TIBSqlQuery(TIBTransaction * trans = NULL,TIBDatabase * db = NULL);
virtual ~TIBSqlQuery(){Release();}
	  bool          __fastcall    fetch       ();
	  bool          __fastcall    unprepare   ();
	  bool          __fastcall    prepare     (const char * sql_text,TIBTransaction * trans = 0) ;
	  bool          __fastcall    prepare_proc(const char * sql_text,TIBTransaction * trans = 0) ;
          bool          __fastcall    cancel();
          void          __fastcall    clear_input();
          void          __fastcall    clear_output();
          bool          __fastcall    close (bool full = true);
          int           __fastcall    get_fields_count();
          int           __fastcall    get_input_fields_count();
          bool          __fastcall    exec(const char * query_text);
          bool          __fastcall    open(const char * query_text,TIBXSQLDA * fields = 0,TIBTransaction * trans = 0)
          {if(prepare(query_text,trans)) return open(fields); return false;}
          bool          __fastcall    open(TIBXSQLDA * fields = 0) ;
          bool          __fastcall    is_eos(){return eos;}
          bool          __fastcall    is_opened(){return opened;}
          bool          __fastcall    is_prepared(const char * query_text = 0);
          bool          operator  !() {return !eos; }

          TIBXSQLDA &   __fastcall get_input_fields () {return in_data; }
          TIBXSQLDA &   __fastcall get_output_fields() {return out_data;}
          TIBField     operator[ ](int idx)                 {return get_field(idx);           }
          TIBField     operator[ ](const char * field_name)       {return get_field(field_name);    }
          TIBField      __fastcall get_field(int idx)       {return TIBField(&out_data,idx,this);  }
          TIBField      __fastcall get_field(const char * fname)  {return TIBField(&out_data,fname,this);}
          TIBField      __fastcall get_input_field(int idx) {return TIBField(&in_data,idx,this);   }
          isc_db_handle __fastcall get_db_handle(){return database    ? (*database)():NULL;}
          isc_tr_handle __fastcall get_tr_handle(){return transaction ? transaction->get_tr_handle():NULL ;}
          TIBTransaction * __fastcall get_transaction(){return transaction;}
          TIBDatabase    * __fastcall get_database()   {return database;}
          int           __fastcall get_rec_no()   {return rec_no;}
          void          __fastcall set_transaction(TIBTransaction & trans);
          void          __fastcall set_database   (TIBDatabase    & db);
          string        __fastcall get_plan();
          string        __fastcall get_query_text(){return SQL;}
          int           __cdecl    assign_values(int count,...);
          template <typename V,typename FT>
          void          __fastcall set_named_param(char * pname,V value, FT field);
          template <typename V,typename FT>
          void          __fastcall set_named_param(int i ,V value, FT field);
  const char  * __fastcall get_ib_class_name() const;

 };


template <typename T>
struct to_lower: public  unary_function<T, T>
{
 T operator() (const T & c) {return tolower(c);} 
};

template <typename V,typename FT >
void          __fastcall TIBSqlQuery::set_named_param(char * pname,V value, FT field)
{
  if(pname && *pname)
  {
   std::string pName(pname);
   transform(pName.begin(),pName.end(),pName.begin(),to_lower<char>());
   if(named_params.params.count(pName))
   {
     Tnamed_params::param_numbers &  fidx = named_params.params[pName];
     Tnamed_params::param_numbers::iterator iptr = fidx.begin(), end = fidx.end();
     while(iptr<end)
     {
      field = (FT) this->get_input_field(*iptr);
      field = value;
      ++iptr;
     }
   }
  }
}

template <typename V,typename FT>
void          __fastcall TIBSqlQuery::set_named_param(int i ,V value, FT field)
{
   if(i<named_params.params.size())
   {
    #ifdef _WIN64
     throw "Not implemented!";
    #else
     Tnamed_params::param_numbers &  fidx = named_params.params.at(i);
     Tnamed_params::param_numbers::iterator iptr = fidx.begin(), end = fidx.end();
     while(iptr<end)
     {
      field = (FT) this->get_input_field(*iptr);
      field = value;
      ++iptr;
     }
    #endif
   }

}


class TIBBlobField:protected TIBField,public TIBStatus
{
 isc_db_handle    HDB;
 isc_tr_handle    HTR;
 isc_blob_handle  HBLOB;
 bool    input;
 __int64 blob_length;
 __int64 blob_segments;
 __int64 blob_max_segment;
 __int64 blob_type;

 void __fastcall get_blob_info();
 void          __fastcall  Release(){close();TIBStatus::Release();}

 public:

          TIBBlobField(TIBSqlQuery & query,int index,bool _input = false);
          TIBBlobField(TIBField & f);
 virtual ~TIBBlobField(){}
 bool  __fastcall open_blob();
 int   __fastcall read_segment(void * buffer,int len);
 int   __fastcall write_segment(void * buffer,int len);
 void  __fastcall close();
 __int64 __fastcall get_blob_length(){return blob_length;}
 __int64 __fastcall get_segment_size(){return blob_max_segment;}
 #if defined _WIN64
 using TIBField::is_null;
 using TIBField::set_null;
 #else
 TIBField::is_null;
 TIBField::set_null;
 #endif

 bool __fastcall is_input() {return  input;}
 bool __fastcall is_output(){return !input;}
 const char  * __fastcall get_ib_class_name() const;
};

/*template <typename T>
int __fastcall field_comparer<T>(const TIBField & f1,const TIBField & f2)
{
  int res = (int)(T(f1)-T(f2));
}*/

template < typename T,typename comp = less<T> >
struct recs_comparer
{
 int     fld_no;
 comp    comp_obj;
 recs_comparer(int        _fno = 0):fld_no(_fno){}
 recs_comparer(const recs_comparer & rc){*this = rc;}
 recs_comparer & operator = (const  recs_comparer & rc){fld_no = rc.fld_no;}
 bool operator ()(const TIBXSQLDA & r1,const TIBXSQLDA & r2)
 {
  TIBField f1( (TIBXSQLDA*)&r1,fld_no);
  TIBField f2( (TIBXSQLDA*)&r2,fld_no);
  return comp_obj(T(f1),T(f2));
 }
};



class TIBRecords:public vector<TIBXSQLDA>
{
 protected:
 int       rec_no;
 public:
 TIBRecords(){rec_no = 0;}
 TIBRecords & operator  = (TIBSqlQuery &q);
 TIBRecords & operator += (TIBSqlQuery &q);
 TIBRecords & operator  = (const TIBRecords  &rec);
 TIBField     operator  ()(int field)    {return (*this)(rec_no,field);}
 TIBField     operator  ()(char * f_name){return (*this)(rec_no,f_name);}
 TIBField     operator  ()(int r,int field);
 TIBField     operator  ()(int r,char * f_name);
 bool         operator !(){return (rec_no>=0 && rec_no<(int)size()) ? true:false;}

 int      __fastcall next (){if(rec_no<(int)size())++rec_no;  return rec_no;}
 int      __fastcall prior(){if(rec_no>0) --rec_no; return rec_no;}
 int      __fastcall first(){rec_no = 0; return rec_no;}
 int      __fastcall last (){if(size()) rec_no = int(size())-1; return rec_no;}

 
 template <typename T>
 int      __fastcall find(int field_no, T value,int start_from );
 template <typename T>
 int      __fastcall find(char * field_name, T value,int start_from );
 void     __fastcall set_rec_no(int _rec_no){if(rec_no< (int)size())rec_no = _rec_no;}
 int      __fastcall get_rec_no(){return rec_no;}
 const TIBXSQLDA & __fastcall get_fields(int rec_no = -1);
 void     __fastcall erase(int rec_no);
 int      __fastcall insert_record(int pos = 0);
 int      __fastcall add_record   () {return insert_record(-1);}
 void     __fastcall clear();

 template < typename T,typename comp >
 void     __fastcall sort_records(int by_field )
 {
  if(by_field>=0 && size()  )
  {
   recs_comparer<T,comp> rc(by_field);
   std::sort(begin(),end(),rc);
  }
 }

 template < typename T,typename comp >
 void     __fastcall sort_records(char * field_name )
 {
  if(size())
  {
   std::sort<T,comp>(at(0).get_field_no(field_name));
  }
 }
};



#define MAX_EVENTS_COUNT 15
class TIBEvents;
typedef void (__stdcall   * ib_event_notify)(void * user_data,TIBEvents * source,int event_index,ISC_ULONG ev_count);

class TIBEvents:public TIBStatus
{
 protected:
 BOOL              active;
 BOOL              first_time;
 ISC_LONG          buffer_length;
 ISC_UCHAR       * event_buf,* events_result;
 ISC_LONG          events_id;
 TIBDatabase       * db;
 vector<string>    event_names;
 static void event_handler(TIBEvents * events,ISC_USHORT, ISC_UCHAR*);
 void            * user_data;
 ib_event_notify   event_notify;
 void            * user_data_mask;
 ib_event_notify   event_notify_mask;
 void          __fastcall    Release()
 {disable();TIBStatus::Release();}
 public:
 TIBEvents();
 TIBEvents(TIBDatabase * _db );
 virtual ~TIBEvents(){};


 int        __fastcall  get_events_count(){return event_names.size();}
 string   & operator[]  (int idx)          {return event_names[min(idx,get_events_count())];  }
 TIBEvents& operator += (char * event_name){if(get_events_count()<MAX_EVENTS_COUNT) event_names.push_back(string(event_name)); return * this;}
 void       __fastcall   clear_events(){event_names.clear();}
 bool       __fastcall   wait(bool asinc = true);
 void       __fastcall   enable(TIBDatabase* _db);
 void       __fastcall   disable();
 void       __fastcall   set_notifyer     (ib_event_notify en,void * ud){user_data = ud;event_notify   = en;}
 void       __fastcall   set_mask_notifyer(ib_event_notify en,void * ud){user_data_mask = ud;event_notify_mask = en;}
 bool       __fastcall   is_active() {return active ? true:false;}
 const char  * __fastcall get_ib_class_name() const;

};



inline
TIBDatabase::TIBDatabase(int _num_buff ,int _buff_len ):
HDB(0),DbName(NULL),params(isc_dpb_version1),connection_id(-1),num_buff(_num_buff),buff_len(_buff_len)
{}

inline
TIBDatabase::TIBDatabase (const TIBDatabase & src):HDB(0),DbName(NULL),params(isc_dpb_version1),connection_id(-1)
{
 *this = src;
}






inline
TIBParams::~TIBParams()
{
 params.clear();
 params.resize(0);
}

inline
char * __fastcall TIBParams::get_ib_params() const
{
 char * ret;
 #ifdef __CODEGEARC__
  ret = (char*)&params.begin()[0];
 #else
 ret = (char*)params.begin();
 #endif

 return ret;
}

inline
void __fastcall TIBParams::add_user(LPSTR UserName)
{
  add_param(isc_dpb_user_name,(LPBYTE)UserName,strlen(UserName));
}

inline
void __fastcall TIBParams::add_password(LPSTR psw)
{
  add_param(isc_dpb_password,(LPBYTE)psw,strlen(psw));
}


inline
short __fastcall   TIBParams::get_params_size() const
{
  return (short)params.size();
}



inline
isc_db_handle __fastcall   TIBDatabase::GetDBHandle() const
{  return this->HDB;}


inline
TIBTransaction::TIBTransaction():tran_params(isc_tpb_version3),HTR(0),db(0),spin_count(0){}

inline
TIBTransaction::TIBTransaction(TIBDatabase & ibdb):tran_params(isc_tpb_version3),HTR(0),spin_count(0)
{db = &ibdb;}

 template <typename T>
 int      __fastcall TIBRecords::find(int field_no     , T value,int start_from )
 {
  int ret = -1;
  if((unsigned int)start_from<this->size())
  {
   vector<TIBXSQLDA>::iterator ptr = begin(), end_ptr = end();
   if(field_no<ptr->get_fields_count())
   {
    advance(ptr,start_from);
    while(ptr<end_ptr && ret<0)
    {

     #ifdef __clang__
     #else
     TIBField field(ptr,field_no);
     if(!field.is_null() && field==value)
      ret = start_from;
     #endif
     ++start_from;
     ++ptr;
    }
   }
  }
  return ret;
 }

 template <typename T>
 int      __fastcall TIBRecords::find(char * field_name, T value,int start_from )
 {
  int ret = -1;
  if((unsigned int)start_from<this->size())
  {

   int field_no = at(start_from).get_field_no(field_name);
   if(field_no>=0)
      ret = find(field_no,value,start_from);
  }
  return ret;
 }









#ifndef _NO_NAMESPACE
}// end of namespace
#endif

#endif

