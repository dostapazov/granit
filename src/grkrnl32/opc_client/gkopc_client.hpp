#ifndef   __GKOPC_CLIENT_HPP__
#define   __GKOPC_CLIENT_HPP__
#define  BOOST_THREAD_USE_LIB
#include  <boost/thread.hpp>
#include  <gkopc_client.h>
#include  <modem.hpp>
#include  <tregstry.hpp>
#include  <opc_helper.hpp>
#include  <otd_storage.hpp>
#include  <vector_index.hpp>
#include  <kertlio.hpp>
#include  <lua/lua_iface.hpp>

using namespace opc;
using namespace KeRTL;


typedef std::vector<OPCHANDLE> opc_handles_t;

#define ICP_KEY_TERMINATE       1
#define ICP_KEY_REMOTE_CONTROL  10
#define ICP_KEY_HANDLE_CHANGES  100
#define ICP_KEY_REFRESH         200

#define WT_RESOLUTION    100
#define MAX_WT_TIMERS    8

#define WT_CHECK_CONNECT   0
#define WT_CONNECT_TIMEOUT 2000

#define WT_READ_SYNC     1

#define LUA_CONST_COMMAND_ON_MORE     "cmd_on_more"
#define LUA_CONST_COMMAND_OFF_LESS    "cmd_off_less"
#define LUA_CONST_COMMAND_SET         "cmd_tr_set"
#define LUA_CONST_COMMAND_UNDO        "cmd_undo"

#define LUA_CONST_TUTR_STATE_PREPARE         "state_prepare"
#define LUA_CONST_TUTR_STATE_ONMORE_ACTIVE   "state_onmore_active"
#define LUA_CONST_TUTR_STATE_OFFLESS_ACTIVE  "state_offless_active"
#define LUA_CONST_TUTR_STATE_RC_ERROR        "state_rc_error"



#define LUA_CLASS_NAME                   "opc_line"
#define LUA_OPC_ITEMID                   "item_id"

#define LUA_OPC_ITEM_VALUE               "item_value"
#define LUA_OPC_ITEM_QUALITY             "item_quality"

#define LUA_RC_TAG                       "rc_tag"
#define LUA_RC_OBJECT                    "rc_object"
#define LUA_RC_COMMAND                   "rc_command"
#define LUA_RC_VALUE                     "rc_value"


#define LUA_FUNC_OPC_ITEM_GET_VALUE      "get_value"
#define LUA_FUNC_OPC_ITEM_SET_VALUE      "set_value"
#define LUA_FUNC_OPC_SET_ITEMS_QUALITY   "set_items_quality"
#define LUA_FUNC_OPC_SET_GROUP_QUALITY   "set_group_quality"
#define LUA_FUNC_OPC_SET_GLOBAL_QUALITY_MASK   "set_global_quality_mask"
#define LUA_FUNC_OPC_SET_GLOBAL_DIAG     "set_line_diag"

#define LUA_FUNC_OPC_ITEM_SET_RC_STATE   "set_rc_state"

#define LUA_FUNC_OPC_ITEM_SET_RC_PREPARE "set_rc_prepare"
#define LUA_FUNC_OPC_ITEM_SET_RC_ACTIVE  "set_rc_active"
#define LUA_FUNC_OPC_ITEM_SET_RC_ERROR   "set_rc_error"

#define LUA_FUNC_CHECK_TERMINATE         "check_terminate"
#define LUA_FUNC_SLEEP                   "sleep"
#define LUA_TRACE                        "__trace"


class opc_line:public modem_line,public TGKThread
{
  public:
  typedef std::map<sotd_addr,DWORD> otd_changes_map_t;
  private:
  GKOPC_LINE_CONFIG      line_config;
  KeRTL::TFASTMutex      data_locker;
  gkopc_items_t          opc_items;
  otd_storage            storage;
  otd_changes_map_t      changes_map;

  TOpcServer           * opc_server;
  TOpcGroup            * opc_group;
  HRESULT                com_last_result;
  DWORD                  global_quality_mask;
  DWORD                  global_otd_diag;

  typedef opc::TOpcDataCallback<opc_line>  opc_data_callback_t;
  friend opc_data_callback_t;
  opc_data_callback_t * opc_data_callback;

  string                             lua_scripts[16];
  boost::thread                      rc_thread;
  gkopc_items_t::container_t         rc_queue;
  BOOL                               rc_undo_request;

  static int lua_otd_set_line_diag    (lua_State *L);
  static int lua_opc_item_get_value     (lua_State *L);
  static int lua_opc_item_set_value     (lua_State *L);
  static int lua_opc_set_items_quality  (lua_State *L);
  static int lua_opc_set_group_quality  (lua_State *L);
  static int lua_opc_set_global_quality_mask  (lua_State *L);
  static int lua_opc_item_set_rc_state  (lua_State *L);
  static int lua_opc_item_set_rc_prepare(lua_State *L);
  static int lua_opc_item_set_rc_active (lua_State *L);
  static int lua_opc_item_set_rc_error  (lua_State *L);

  static int lua_check_terminate        (lua_State *L);
  static int lua_thread_sleep           (lua_State *L);
  static int lua_trace                  (lua_State *L);

  void __fastcall rc_prepare_execute(lua::LUAInstance & lua);
  void __fastcall rc_run_script     (lua::LUAInstance & lua,gkopc_item & item);
  int             rc_thread_proc();
  void __fastcall rc_start();

  lua::LUAInstance   calc_scripts;


  KeRTL::TIoCompletionPort icp;
  int WT[MAX_WT_TIMERS];
  void  __fastcall WTimer_start(int idx,int tmo);
  void  __fastcall WTimer_stop (int idx);
  bool  __fastcall WTimer_check(int idx);


  virtual DWORD __fastcall get_line_text  (wchar_t * text,DWORD text_sz) ;
  virtual DWORD __fastcall get_max_frame_size() ;
  virtual DWORD __fastcall get_tx_queue_size () ;
  virtual bool  __fastcall do_start ();
  virtual bool  __fastcall do_stop  ();
  virtual bool  __fastcall send     (LPMPROTO_HEADER mph,DWORD sz) ;
  virtual DWORD __fastcall get_line_flags    (){return MLPF_TXABLE|MLPF_RXABLE|MLPF_DATASOURCE;}

  virtual bool __fastcall BeforeExecute();
  virtual void __fastcall BeforeTerminate();
  virtual int  __fastcall Execute   ();
  virtual void __fastcall Terminate (){ TGKThread::Terminate(); icp.PostStatus(ICP_KEY_TERMINATE,sizeof(DWORD),NULL);}
          void __fastcall on_timeout();
          void __fastcall on_connect_timeout();

  virtual void  __fastcall on_connect   ();
  virtual void  __fastcall on_disconnect();

          void __fastcall do_connect    ();
          void __fastcall do_disconnect ();
          void __fastcall do_refresh    ();
         DWORD __fastcall refresh(DWORD sbl);/*Функция обновления данных по линии*/


         bool  __fastcall is_need_restart(const GKOPC_LINE_CONFIG & ln_conf);
         int   __fastcall load_items_from_file();
         int   __fastcall build_otd_storage   ();

         DWORD __fastcall  __get_range_otd_type(gkopc_items_t::index_iterator iptr,gkopc_items_t::index_iterator  iend);

         DWORD __fastcall __create_otd_group   (DWORD fa,DWORD sb,DWORD first_num,DWORD count,DWORD otd_type);
         DWORD __fastcall __get_opc_handles    (opc_handles_t & handles,gkopc_items_t::iterator beg,gkopc_items_t::iterator end);
         DWORD __fastcall __get_opc_handles    (opc_handles_t & handles,gkopc_items_t::index_iterator ibeg,gkopc_items_t::index_iterator iend);
         void  __fastcall __set_opc_item_values(gkopc_item & item,LPVARIANT v,LPWORD quality,__int64 * time,LPDWORD rc_state = 0 );
         void  __fastcall __calc_item_value(gkopc_item & item);
         void  __fastcall __prepare_calc_scripts();

         void  __fastcall __read_sync();

         void  __fastcall  handle_changes();
         DWORD __fastcall  get_line_diag(bool rescan);
         void  __fastcall  otd_set_line_diag(DWORD diag);

         void  __fastcall __setup_group_values  (otd_proto * op);
         void  __fastcall __queue_rxdata(otd_proto * op);

         void  __fastcall __do_recv_cp_data   (bool inc_name );
         void  __fastcall __handle_otd_query  (lpotd_addr  addr);
         void  __fastcall __handle_otd_control(lpotd_proto op);
         void __fastcall __set_bad_diag();


         void __fastcall __opc_create_group   ();
         void __fastcall __opc_group_add_items();



  void __fastcall do_report(DWORD rep_type, wchar_t * text,LPVOID data = NULL,DWORD data_size = 0);
  void __fastcall report_com_error(HRESULT res,const TCHAR * msg = NULL);
  void __fastcall handle_com_error(HRESULT res,const TCHAR * msg = NULL);
  void __fastcall report_opc_error(LONG err ,const TCHAR * msg = NULL);

  void __fastcall on_data_change  ( DWORD     conn_id  , opc::TOpcGroup * grp       , DWORD dwTransid
                                   ,OPCHANDLE hGroup   , HRESULT     hrMasterquality, HRESULT     hrMastererror
                                   ,DWORD     dwCount  , OPCHANDLE * phClientItems  , VARIANT   * pvValues
                                   ,WORD  * pwQualities, FILETIME  * pftTimeStamps  , HRESULT   * pErrors);

  void __fastcall on_read_complete( DWORD     conn_id  , opc::TOpcGroup * grp       , DWORD dwTransid
                                   ,OPCHANDLE hGroup   , HRESULT     hrMasterquality, HRESULT     hrMastererror
                                   ,DWORD     dwCount  , OPCHANDLE * phClientItems  , VARIANT   * pvValues
                                   ,WORD  * pwQualities, FILETIME  * pftTimeStamps  , HRESULT   * pErrors);

  void __fastcall on_write_complete(DWORD     conn_id  , opc::TOpcGroup * grp       , DWORD dwTransid
                                   ,OPCHANDLE hGroup   , HRESULT     hrMasterquality, DWORD       dwCount
                                   ,OPCHANDLE * pClienthandles, HRESULT   * pErrors);
  void __fastcall on_cancel_complete(DWORD     conn_id  , opc::TOpcGroup * grp       , DWORD dwTransid,OPCHANDLE hGroup);

  void  __fastcall start_async  ();
  void  __fastcall stop_async   ();

  bool __fastcall opc_item_set_value   (const wstring & item_id,double);
  int  __fastcall opc_set_group_quality(BYTE fa,BYTE grp,WORD quality);
  int  __fastcall opc_set_global_quality_mask  (WORD quality,bool set);


  public:
  opc_line():modem_line(-1),opc_server(NULL),opc_group(NULL),com_last_result(0),opc_data_callback(NULL),icp(128),calc_scripts(NULL){}
  opc_line(DWORD line_num):modem_line(line_num),opc_server(NULL),opc_group(NULL),com_last_result(0),opc_data_callback(NULL),icp(128),calc_scripts(NULL){}
  bool __fastcall set_line_config(LPGKOPC_LINE_CONFIG conf);
  void __fastcall get_line_config(LPGKOPC_LINE_CONFIG line_config);
  void __fastcall write(KeRTL::TStream & stream);
  void __fastcall read (KeRTL::TStream & stream,DWORD stream_ver );
 DWORD __fastcall get_opc_item(DWORD idx,gkopc_item ** item);
 DWORD __fastcall release_opc_item(gkopc_item * item);

 DWORD __fastcall get_script   (DWORD script_idx,LPSCRIPT_TEXT);
 DWORD __fastcall set_script   (DWORD script_idx,LPSCRIPT_TEXT);

};


class TGkOpcModem  : public  TModemBase
{

  protected:
  static THeap              mod_heap;
  static DWORD              total_mem_alloc  ;
  GKOPC_CLIENT_MOD_CONFIG   mod_config;
  DWORD                     pu_diag;

  DWORD       __fastcall    get_mem_used();

  WORD        __fastcall    get_type(){return MT_MODEM_OPC;}
  virtual
  void        __fastcall    free_line     (modem_line *) ;
  virtual
  DWORD       __fastcall    get_modem_flags();
  virtual int __fastcall    convert_rx_data(LPWORD fa,LPBYTE in,int in_len,LPBYTE out,DWORD out_sz) ;
  virtual
  DWORD        __fastcall   get_lines_limit() ;
  LRESULT     __fastcall    processing(DWORD cmd,LPARAM p1,LPARAM p2);
  void        __fastcall    release(LPVOID arg);
  BOOL        __fastcall    can_start(DWORD reason,LPARAM start_param);
  DWORD       __fastcall    start(DWORD reason,LPARAM start_param);
  DWORD       __fastcall    stop (DWORD reason);

  bool        __fastcall    read_settings();
  int         __fastcall    read_lines   (KeRTL::TRegsReader & rd);
  bool        __fastcall    write_settings();
  int         __fastcall    write_lines   (KeRTL::TRegsWriter & rw);

  virtual
  DWORD   __fastcall get_config_data(DWORD mask,LPVOID buf,DWORD bsz);
  virtual
  bool    __fastcall set_config_data(DWORD mask,LPVOID buf,DWORD bsz);
  virtual
  bool    __fastcall compare_config_data(DWORD mask,LPVOID buf,DWORD bsz,LPDWORD ch_mask,BOOL * restart);
  virtual
  DWORD   __fastcall get_window_module  (wchar_t * buf,DWORD bsz);
  virtual
  DWORD   __fastcall check_license(){return MOD_LICENSE_REGISTERED; }

  opc_line * __fastcall __get_opc_line(DWORD num)
  {
    return  dynamic_cast<opc_line*>(get_line(num));
  }
  LRESULT __fastcall _line_get_config(LPGKOPC_LINE_CONFIG line_config);
  LRESULT __fastcall _line_add       (LPGKOPC_LINE_CONFIG line_config);
  LRESULT __fastcall _line_upd_config(LPGKOPC_LINE_CONFIG line_config,DWORD old_num) ;
  LRESULT __fastcall _line_del       (DWORD line_num);
  LRESULT __fastcall _line_restart   (DWORD line_num);
  LRESULT __fastcall _line_get_tu_script(DWORD line_num,LPARAM p);
  LRESULT __fastcall _line_set_tu_script(DWORD line_num,LPARAM p);
  LRESULT __fastcall _line_get_tr_script(DWORD line_num,LPARAM p);
  LRESULT __fastcall _line_set_tr_script(DWORD line_num,LPARAM p);




  BYTE  __fastcall get_pu_number ();
  DWORD __fastcall get_pu_diag    (bool rescan);
  DWORD __fastcall scan_pu_diag   ();
  void  __fastcall do_recv_pu_data(bool inc_name );

  public:
  TGkOpcModem(DWORD number = -1);
  void       __fastcall do_report(DWORD evt_type,wchar_t * rep_text,LPVOID data,DWORD dt_sz);
  LRESULT    __fastcall _line_get_item        (LPARAM line,LPARAM p2);
  LRESULT    __fastcall _line_release_item    (LPARAM line,LPARAM p2);
 static void * __fastcall mem_alloc(size_t size);
 static void   __fastcall mem_free (void * ptr );

  protected:
  DWORD rep_id;

};

inline  DWORD       __fastcall    TGkOpcModem::get_modem_flags()
{
  return  MPF_DATA_SOURCE;
}

inline   DWORD        __fastcall   TGkOpcModem::get_lines_limit()
{
   return MODEM_ADDR_MAXVALUE;
}



#endif

