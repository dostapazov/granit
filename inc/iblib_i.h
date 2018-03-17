#ifndef _IB_LIB_IFACE_
#define _IB_LIB_IFACE_

#include <kertl.hpp>
#include <dll_procs.hpp>
namespace fb_inc
{
extern "C" {
  #include <ibase.h>
 }
}

#ifndef _NO_NAMESPACE
namespace KeRTL
{
#endif

using namespace fb_inc;

class TIBCLLib:public TModule
{
 public :
 TIBCLLib():TModule(){};
 bool __fastcall load_library(const char * lib_path);
 void __fastcall AfterLoad   ();
 void __fastcall BeforeUnload();

 dll_proc6<ISC_STATUS*,short,const ISC_SCHAR*, isc_db_handle*,short,const ISC_SCHAR*,ISC_STATUS>
     isc_attach_database;

 dll_proc5<ISC_STATUS*,const ISC_ARRAY_DESC*,ISC_SHORT*,ISC_UCHAR*,ISC_SHORT*,ISC_STATUS>
 isc_array_gen_sdl  ;

 dll_proc7<ISC_STATUS*,isc_db_handle*,isc_tr_handle*,ISC_QUAD*,const ISC_ARRAY_DESC*,void*,ISC_LONG*,ISC_STATUS>
 isc_array_get_slice;

 dll_proc6<ISC_STATUS*,isc_db_handle*,isc_tr_handle*,const ISC_SCHAR*,const ISC_SCHAR*,ISC_ARRAY_DESC*,ISC_STATUS>
 isc_array_lookup_bounds;

 dll_proc6<ISC_STATUS*,isc_db_handle*,isc_tr_handle*,const ISC_SCHAR*,const ISC_SCHAR*,ISC_ARRAY_DESC*,ISC_STATUS >
 isc_array_lookup_desc;

 dll_proc7  <ISC_STATUS*,const ISC_SCHAR*,const ISC_SCHAR*,const short*,const short*,const short*,ISC_ARRAY_DESC*,ISC_STATUS>
 isc_array_set_desc;

 dll_proc7  <ISC_STATUS*, isc_db_handle*, isc_tr_handle*, ISC_QUAD*, const ISC_ARRAY_DESC*, void*, ISC_LONG *,ISC_STATUS >
 isc_array_put_slice;

 dll_proc3  <ISC_BLOB_DESC*,const ISC_UCHAR*,const ISC_UCHAR*,LPVOID>
 isc_blob_default_desc;

 dll_proc6  <ISC_STATUS*, const ISC_BLOB_DESC*, const ISC_BLOB_DESC*, unsigned short, ISC_UCHAR*, unsigned short*,ISC_STATUS  >
 isc_blob_gen_bpb;

 dll_proc6 <ISC_STATUS*,isc_blob_handle*,short,const ISC_SCHAR*,short,ISC_SCHAR*,ISC_STATUS  >
 isc_blob_info;

 dll_proc7  <ISC_STATUS*,isc_db_handle*,isc_tr_handle*,const ISC_UCHAR*,const ISC_UCHAR*,ISC_BLOB_DESC*,ISC_UCHAR*,ISC_STATUS>
 isc_blob_lookup_desc;

 dll_proc7  <ISC_STATUS*,const ISC_UCHAR*,const ISC_UCHAR*,short,short,short,ISC_BLOB_DESC*,ISC_STATUS  >
 isc_blob_set_desc;

 dll_proc2  <ISC_STATUS *,isc_blob_handle *,ISC_STATUS  >
 isc_cancel_blob;

 dll_proc3  <ISC_STATUS *,isc_db_handle *,ISC_LONG *,ISC_STATUS >
 isc_cancel_events;

 dll_proc2  <ISC_STATUS *,isc_blob_handle *,ISC_STATUS >
 isc_close_blob;

 dll_proc2  <ISC_STATUS *,isc_tr_handle *,ISC_STATUS >
 isc_commit_retaining;

 dll_proc2<ISC_STATUS *,isc_tr_handle *,ISC_STATUS >
 isc_commit_transaction;

 dll_proc5  <ISC_STATUS*,isc_db_handle*,isc_tr_handle*,isc_blob_handle*,ISC_QUAD*,ISC_STATUS >
 isc_create_blob;

 dll_proc7  <ISC_STATUS*,isc_db_handle*,isc_tr_handle*,isc_blob_handle*,ISC_QUAD*,short,const ISC_SCHAR*,ISC_STATUS>
 isc_create_blob2;

 dll_proc7  <ISC_STATUS*,short,const ISC_SCHAR*,isc_db_handle*,short,const ISC_SCHAR*,short,ISC_STATUS >
 isc_create_database;

 dll_proc6  <ISC_STATUS*,isc_db_handle*,short,const ISC_SCHAR*,short,ISC_SCHAR*,ISC_STATUS  >
 isc_database_info;

 dll_proc2  <const ISC_QUAD*,void*,LPVOID>
 isc_decode_date;

 dll_proc2  <const ISC_DATE*,void*,void *>
 isc_decode_sql_date;

 dll_proc2  <const ISC_TIME*, void*,void *>
 isc_decode_sql_time;

 dll_proc2  <const ISC_TIMESTAMP*,void*,void*>
 isc_decode_timestamp;

 dll_proc2  <ISC_STATUS *,isc_db_handle *,ISC_STATUS >
 isc_detach_database;

 dll_proc2  <ISC_STATUS *,isc_db_handle *,ISC_STATUS >
 isc_drop_database;

 dll_proc3  <ISC_STATUS *,isc_db_handle *,isc_stmt_handle *,ISC_STATUS >
 isc_dsql_allocate_statement;

 dll_proc3  <ISC_STATUS *,isc_db_handle *,isc_stmt_handle *,ISC_STATUS >
 isc_dsql_alloc_statement2;

 dll_proc4  <ISC_STATUS *,isc_stmt_handle *,unsigned short,XSQLDA *,ISC_STATUS >
 isc_dsql_describe;

 dll_proc4  <ISC_STATUS *,isc_stmt_handle *,unsigned short,XSQLDA *,ISC_STATUS >
isc_dsql_describe_bind;

 dll_proc8  <ISC_STATUS*,isc_db_handle*,isc_tr_handle*,unsigned short,const ISC_SCHAR*,unsigned short,const XSQLDA*,const XSQLDA*,ISC_STATUS >
 isc_dsql_exec_immed2;

 dll_proc5  <ISC_STATUS*,isc_tr_handle*,isc_stmt_handle*,unsigned short,const XSQLDA*,ISC_STATUS >
 isc_dsql_execute;

 dll_proc6  <ISC_STATUS*,isc_tr_handle*,isc_stmt_handle*,unsigned short,const XSQLDA*,const XSQLDA*,ISC_STATUS >
 isc_dsql_execute2;

 dll_proc7 <ISC_STATUS*,isc_db_handle*,isc_tr_handle*,unsigned short,const ISC_SCHAR*,unsigned short,const XSQLDA*,ISC_STATUS  >
 isc_dsql_execute_immediate;

 dll_proc4  <ISC_STATUS *,isc_stmt_handle *,unsigned short,const XSQLDA *,ISC_STATUS  >
 isc_dsql_fetch;

 dll_proc1  <isc_db_handle *,ISC_STATUS  >
 isc_dsql_finish;

 dll_proc3  <ISC_STATUS *,isc_stmt_handle *,unsigned short,ISC_STATUS  >
 isc_dsql_free_statement;

 dll_proc4  <ISC_STATUS*,isc_stmt_handle*,unsigned short,XSQLDA*,ISC_STATUS  >
 isc_dsql_insert;

 dll_proc7  <ISC_STATUS*,isc_tr_handle*,isc_stmt_handle*,unsigned short,const ISC_SCHAR*,unsigned short,XSQLDA*,ISC_STATUS  >
 isc_dsql_prepare;

 dll_proc4  <ISC_STATUS*,isc_stmt_handle*,const ISC_SCHAR*,unsigned short,ISC_STATUS  >
 isc_dsql_set_cursor_name;

 dll_proc6  <ISC_STATUS*,isc_stmt_handle*,short,const ISC_SCHAR*,short,ISC_SCHAR*,ISC_STATUS  >
 isc_dsql_sql_info;

 dll_proc2  <const void*,ISC_QUAD*,void *>
 isc_encode_date;

 dll_proc2  <const void*,ISC_DATE*,void *>
 isc_encode_sql_date;

 dll_proc2  <const void*,ISC_TIME*,void *>
 isc_encode_sql_time;

 dll_proc2  <const void*,ISC_TIMESTAMP*,void *>
 isc_encode_timestamp;


 dll_proc4  <ISC_SCHAR**,ISC_SCHAR**,ISC_USHORT,ISC_SCHAR**,ISC_USHORT >
 isc_event_block_a;

 dll_proc5  <ISC_SCHAR**,ISC_SCHAR**,ISC_USHORT,ISC_SCHAR**,ISC_USHORT*,void *>
 isc_event_block_s;

 dll_proc4  <ISC_ULONG*,short,ISC_UCHAR*,const ISC_UCHAR *,void  *>
 isc_event_counts;

 dll_proc5  <ISC_SCHAR**,short*,unsigned short,const ISC_SCHAR*,short,int  >
 isc_modify_dpb;


 dll_proc1  <ISC_SCHAR *,ISC_LONG  >
 isc_free;

 dll_proc5  <ISC_STATUS *,isc_blob_handle *,unsigned short *,unsigned short,ISC_SCHAR *,ISC_STATUS  >
 isc_get_segment;

 dll_proc11  <ISC_STATUS*,isc_db_handle*,isc_tr_handle*,ISC_QUAD*,short,const ISC_SCHAR*,short,const ISC_LONG*,ISC_LONG,void*,ISC_LONG*,ISC_STATUS >
 isc_get_slice;

 dll_proc2  <ISC_SCHAR*,ISC_STATUS**,ISC_LONG >
 isc_interprete;

/* This const params version used in the engine and other places. */
 dll_proc3  <ISC_SCHAR*,unsigned int,const ISC_STATUS**,ISC_LONG  >
 fb_interpret;

 dll_proc5  <ISC_STATUS*,isc_db_handle*,isc_tr_handle*,isc_blob_handle*,ISC_QUAD*,ISC_STATUS >
 isc_open_blob;

 dll_proc7  <ISC_STATUS*,isc_db_handle*,isc_tr_handle*,isc_blob_handle*,ISC_QUAD*,ISC_USHORT,const ISC_UCHAR*,ISC_STATUS >
 isc_open_blob2;

 dll_proc4  <ISC_STATUS*,isc_tr_handle*,ISC_USHORT,const ISC_UCHAR*,ISC_STATUS >
 isc_prepare_transaction2;

 dll_proc2  <ISC_SHORT,const ISC_STATUS*,void *>
 isc_print_sqlerror;

 dll_proc1  <const ISC_STATUS*,ISC_STATUS >
 isc_print_status;

 dll_proc4  <ISC_STATUS*,isc_blob_handle*,unsigned short,const ISC_SCHAR*,ISC_STATUS  >
 isc_put_segment;

 dll_proc10  <ISC_STATUS*,isc_db_handle*,isc_tr_handle*,ISC_QUAD*,short,const ISC_SCHAR*,short,const ISC_LONG*,ISC_LONG,void*,ISC_STATUS >
 isc_put_slice;

 dll_proc7  <ISC_STATUS*,isc_db_handle*,ISC_LONG*,short,const ISC_UCHAR*,ISC_EVENT_CALLBACK,void*,ISC_STATUS  >
 isc_que_events;

 dll_proc2 <ISC_STATUS*,isc_tr_handle*,ISC_STATUS >
 isc_rollback_retaining;

 dll_proc2  <ISC_STATUS*,isc_tr_handle*,ISC_STATUS  >
 isc_rollback_transaction;

 dll_proc4  <ISC_STATUS*,isc_tr_handle*,short,void *,ISC_STATUS  >
 isc_start_multiple;

 dll_proc2  <ISC_STATUS*, isc_tr_handle*,ISC_STATUS >
 fb_disconnect_transaction;

 dll_proc1  <const ISC_STATUS*,ISC_LONG  >
 isc_sqlcode;

 dll_proc2  <const ISC_STATUS*,ISC_ULONG*,void *>
 isc_sqlcode_s;

 dll_proc2  <char*,const ISC_STATUS*,void >
 fb_sqlstate;

 dll_proc3  <short,ISC_SCHAR*,short,void *>
 isc_sql_interprete;

 dll_proc6  <ISC_STATUS*,isc_tr_handle*,short,const ISC_SCHAR*,short,ISC_SCHAR*,ISC_STATUS  >
 isc_transaction_info;

 dll_proc9  <ISC_STATUS*,isc_db_handle*,isc_tr_handle*,unsigned short,ISC_SCHAR*,unsigned short,ISC_SCHAR*,unsigned short,ISC_SCHAR*,ISC_STATUS  >
 isc_transact_request;

 dll_proc2  <const ISC_SCHAR*,short,ISC_LONG  >
 isc_vax_integer;

 dll_proc2  <const ISC_UCHAR*,short,ISC_INT64  >
 isc_portable_integer;

 dll_proc2  <ISC_STATUS*, const USER_SEC_DATA*,ISC_STATUS >
 isc_add_user;

 dll_proc2  <ISC_STATUS*, const USER_SEC_DATA*,ISC_STATUS  >
 isc_delete_user;

 dll_proc2  <ISC_STATUS*, const USER_SEC_DATA*,ISC_STATUS >
 isc_modify_user;


 dll_proc5  <ISC_STATUS*,isc_db_handle*,isc_req_handle*,short,const ISC_SCHAR*,ISC_STATUS  >
 isc_compile_request;

 dll_proc5  <ISC_STATUS*,isc_db_handle*,isc_req_handle*,short,const ISC_SCHAR*,ISC_STATUS >
 isc_compile_request2;

 dll_proc5  <ISC_STATUS*,isc_db_handle*,isc_tr_handle*,short,const ISC_SCHAR*,ISC_STATUS >
 isc_ddl;

 dll_proc2  <ISC_STATUS*,isc_tr_handle*,ISC_STATUS  >
 isc_prepare_transaction;


 dll_proc6  <ISC_STATUS*,isc_req_handle*,short,short,void*,short,ISC_STATUS  >
 isc_receive;

 dll_proc5  <ISC_STATUS*,isc_db_handle*,isc_tr_handle*,short,const ISC_SCHAR*,ISC_STATUS>
 isc_reconnect_transaction;

 dll_proc2  <ISC_STATUS*,isc_req_handle*,ISC_STATUS >
 isc_release_request;

 dll_proc7  <ISC_STATUS*,isc_req_handle*,short,short,const ISC_SCHAR*,short,ISC_SCHAR*,ISC_STATUS  >
 isc_request_info;

 dll_proc5  <ISC_STATUS*,isc_blob_handle*,short,ISC_LONG,ISC_LONG*,ISC_STATUS  >
 isc_seek_blob;

 dll_proc6  <ISC_STATUS*,isc_req_handle*,short,short,const void*,short,ISC_STATUS >
 isc_send;

 dll_proc7  <ISC_STATUS*,isc_req_handle*,isc_tr_handle*,short,short,const void*,short,ISC_STATUS >
 isc_start_and_send;

 dll_proc4  <ISC_STATUS *,isc_req_handle *,isc_tr_handle *,short,ISC_STATUS  >
 isc_start_request;

 dll_proc3  <ISC_STATUS *,isc_tr_handle *,short,ISC_STATUS  >
 isc_unwind_request;

 dll_proc5  <ISC_STATUS*,isc_db_handle*,short,const ISC_UCHAR*,ISC_UCHAR*,ISC_STATUS >
 isc_wait_for_event;


 dll_proc2  <ISC_STATUS*,const ISC_SCHAR*,ISC_STATUS >
 isc_close;

 dll_proc3  <ISC_STATUS*,const ISC_SCHAR*,const ISC_SCHAR*,ISC_STATUS  >
 isc_declare;

 dll_proc3  <ISC_STATUS*,const ISC_SCHAR*,XSQLDA *,ISC_STATUS >
 isc_describe;

 dll_proc3  <ISC_STATUS*,const ISC_SCHAR*,XSQLDA*,ISC_STATUS  >
 isc_describe_bind;

 dll_proc4 <ISC_STATUS*,isc_tr_handle*,const ISC_SCHAR*,XSQLDA*,ISC_STATUS  >
  isc_execute;

 dll_proc5  <ISC_STATUS*,isc_db_handle*,isc_tr_handle*,short*,const ISC_SCHAR*,ISC_STATUS  >
 isc_execute_immediate;

 dll_proc3  <ISC_STATUS*,const ISC_SCHAR*,XSQLDA*,ISC_STATUS  >
 isc_fetch;

 dll_proc4  <ISC_STATUS*,isc_tr_handle*,const ISC_SCHAR*,XSQLDA*,ISC_STATUS  >
 isc_open;

 dll_proc7  <ISC_STATUS*,isc_db_handle*,isc_tr_handle*,const ISC_SCHAR*,const short*,const ISC_SCHAR*,XSQLDA*,ISC_STATUS >
 isc_prepare;



 dll_proc8  <ISC_STATUS*,isc_tr_handle*,isc_stmt_handle*,unsigned short,const ISC_SCHAR*,unsigned short,unsigned short,ISC_SCHAR*,ISC_STATUS  >
 isc_dsql_execute_m;

 dll_proc13  <ISC_STATUS*,isc_tr_handle*,isc_stmt_handle*,unsigned short,const ISC_SCHAR*,
 unsigned short,unsigned short,ISC_SCHAR*,unsigned short,ISC_SCHAR*,unsigned short,unsigned short,ISC_SCHAR*,ISC_STATUS  >
 isc_dsql_execute2_m;

 dll_proc11  <ISC_STATUS*,isc_db_handle*,isc_tr_handle*,unsigned short,const ISC_SCHAR*,
 unsigned short,unsigned short,ISC_SCHAR*,unsigned short,unsigned short,ISC_SCHAR*,ISC_STATUS  >
 isc_dsql_execute_immediate_m;

 dll_proc16  <ISC_STATUS*,isc_db_handle*,isc_tr_handle*,unsigned short,const ISC_SCHAR*,
 unsigned short,unsigned short,ISC_SCHAR*,unsigned short,unsigned short,const ISC_SCHAR*,unsigned short,
 ISC_SCHAR*,unsigned short,unsigned short,ISC_SCHAR*,ISC_STATUS >
 isc_dsql_exec_immed3_m;

 dll_proc7  <ISC_STATUS*,isc_stmt_handle*,unsigned short,ISC_SCHAR*,unsigned short,unsigned short,ISC_SCHAR*,ISC_STATUS  >
 isc_dsql_fetch_m;

 dll_proc7  <ISC_STATUS*,isc_stmt_handle*,unsigned short,const ISC_SCHAR*,unsigned short,unsigned short,const ISC_SCHAR*,ISC_STATUS  >
 isc_dsql_insert_m;

 dll_proc10  <ISC_STATUS*,isc_tr_handle*,isc_stmt_handle*,unsigned short,const ISC_SCHAR*,
 unsigned short,unsigned short,const ISC_SCHAR*,unsigned short,ISC_SCHAR*,ISC_STATUS  >
 isc_dsql_prepare_m;

 dll_proc2  <ISC_STATUS*,const ISC_SCHAR*,ISC_STATUS  >
 isc_dsql_release;

 dll_proc2  <ISC_STATUS*,const ISC_SCHAR*,ISC_STATUS  >
 isc_embed_dsql_close;

 dll_proc3  <ISC_STATUS*,const ISC_SCHAR*,const ISC_SCHAR*,ISC_STATUS  >
 isc_embed_dsql_declare;

 dll_proc4  <ISC_STATUS*,const ISC_SCHAR*,unsigned short,XSQLDA*,ISC_STATUS  >
 isc_embed_dsql_describe;

 dll_proc4  <ISC_STATUS*,const ISC_SCHAR*,unsigned short,XSQLDA*,ISC_STATUS  >
 isc_embed_dsql_describe_bind;

 dll_proc5  <ISC_STATUS*,isc_tr_handle*,const ISC_SCHAR*,unsigned short,XSQLDA*,ISC_STATUS  >
 isc_embed_dsql_execute;

 dll_proc6  <ISC_STATUS*,isc_tr_handle*,const ISC_SCHAR*,unsigned short,XSQLDA*,XSQLDA*,ISC_STATUS  >
 isc_embed_dsql_execute2;

 dll_proc7  <ISC_STATUS*,isc_db_handle*,isc_tr_handle*,unsigned short,const ISC_SCHAR*,unsigned short,XSQLDA*,ISC_STATUS  >
 isc_embed_dsql_execute_immed;

 dll_proc4  <ISC_STATUS*,const ISC_SCHAR*,unsigned short,XSQLDA*,ISC_STATUS  >
 isc_embed_dsql_fetch;

 dll_proc5  <ISC_STATUS*,int*,const ISC_SCHAR*,ISC_USHORT,XSQLDA*,ISC_STATUS >
 isc_embed_dsql_fetch_a;

 dll_proc2  <const ISC_UCHAR*,ISC_USHORT*,void * >
 isc_embed_dsql_length;

 dll_proc5  <ISC_STATUS*,isc_tr_handle*,const ISC_SCHAR*,unsigned short,XSQLDA*,ISC_STATUS  >
 isc_embed_dsql_open;

 dll_proc6  <ISC_STATUS*,isc_tr_handle*,const ISC_SCHAR*,unsigned short,XSQLDA*,XSQLDA*,ISC_STATUS  >
 isc_embed_dsql_open2;

 dll_proc4  <ISC_STATUS*,const ISC_SCHAR*,unsigned short,XSQLDA*,ISC_STATUS  >
 isc_embed_dsql_insert;

 dll_proc8  <ISC_STATUS*,isc_db_handle*,isc_tr_handle*,const ISC_SCHAR*,unsigned short,const ISC_SCHAR*,unsigned short,XSQLDA*,ISC_STATUS >
 isc_embed_dsql_prepare;

 dll_proc2  <ISC_STATUS*,const ISC_SCHAR*,ISC_STATUS  >
 isc_embed_dsql_release;



 dll_proc3  <isc_blob_handle,ISC_SCHAR*,int,BSTREAM*>
 BLOB_open;

 dll_proc2  <ISC_SCHAR,BSTREAM*,int  >
 BLOB_put;

 dll_proc1  <BSTREAM*,int >
 BLOB_close;

 dll_proc1  <BSTREAM*,int >
 BLOB_get;

 dll_proc4  <ISC_QUAD*,isc_db_handle,isc_tr_handle,const ISC_SCHAR*,int  >
 BLOB_display;

 dll_proc4  <ISC_QUAD*,isc_db_handle,isc_tr_handle,const ISC_SCHAR*,int  >
 BLOB_dump;

 dll_proc4  <ISC_QUAD*,isc_db_handle,isc_tr_handle,const ISC_SCHAR*,int  >
 BLOB_edit;

 dll_proc4  <ISC_QUAD*,isc_db_handle,isc_tr_handle,const ISC_SCHAR*,int  >
 BLOB_load;

 dll_proc4  <ISC_QUAD*,isc_db_handle,isc_tr_handle,const ISC_SCHAR*,int  >
 BLOB_text_dump;

 dll_proc4  <ISC_QUAD*,isc_db_handle,isc_tr_handle,const ISC_SCHAR*,int  >
 BLOB_text_load;

 dll_proc4  <ISC_QUAD*,isc_db_handle,isc_tr_handle,const ISC_SCHAR*,BSTREAM* >
 Bopen;

 dll_proc4  <const ISC_SCHAR*,const unsigned short,ISC_SCHAR*,const unsigned short,ISC_LONG  >
 isc_ftof;

 dll_proc4 <const ISC_SCHAR*,ISC_PRINT_CALLBACK,void*,short,ISC_STATUS  >
 isc_print_blr;

 dll_proc5  <const ISC_UCHAR*,ISC_ULONG,ISC_PRINT_CALLBACK,void*,short,int >
 fb_print_blr;

 dll_proc1  <int,void *  >
 isc_set_debug;

 dll_proc2  <const ISC_QUAD*,ISC_QUAD*,void *>
 isc_qtoq;

 dll_proc3  <const ISC_SCHAR*,ISC_SCHAR*,unsigned short,void *>
 isc_vtof;

 dll_proc3  <const ISC_SCHAR*,ISC_SCHAR*,short,void * >
 isc_vtov;

 dll_proc3  <isc_db_handle*,ISC_VERSION_CALLBACK,void*,int  >
 isc_version;

 dll_proc1  <ISC_USHORT,ISC_LONG   >
 isc_reset_fpe;

 dll_proc1  <ISC_SCHAR*,fb_inc::uintptr_t>
 isc_baddress;

 dll_proc2  <const ISC_SCHAR*,fb_inc::uintptr_t*,void*>
 isc_baddress_s;

 dll_proc6  <ISC_STATUS*,unsigned short,const ISC_SCHAR*,isc_svc_handle*,unsigned short,const ISC_SCHAR*,ISC_STATUS  >
 isc_service_attach;

 dll_proc2  <ISC_STATUS *,isc_svc_handle *,ISC_STATUS  >
 isc_service_detach;

 dll_proc9  <ISC_STATUS*,isc_svc_handle*,isc_resv_handle*,unsigned short,const ISC_SCHAR*,unsigned short,const ISC_SCHAR*,unsigned short,ISC_SCHAR*,ISC_STATUS  >
 isc_service_query;

 dll_proc5  <ISC_STATUS*,isc_svc_handle*,isc_resv_handle*,unsigned short,const ISC_SCHAR*,ISC_STATUS  >
 isc_service_start;


 dll_proc2  <unsigned int, const int,int  >
 fb_shutdown;

 dll_proc4  <ISC_STATUS*,FB_SHUTDOWN_CALLBACK,const int,void*,ISC_STATUS  >
 fb_shutdown_callback;

 dll_proc3  <ISC_STATUS*,isc_db_handle*,ISC_USHORT,ISC_STATUS >
 fb_cancel_operation;

 dll_proc1 < ISC_SCHAR* > isc_get_client_version       ;
 dll_proc  < int >        isc_get_client_major_version ;
 dll_proc  < int >        isc_get_client_minor_version ;



typedef ISC_LONG    (__cdecl * p_isc_event_block      )(ISC_UCHAR**,ISC_UCHAR**,ISC_USHORT, ...);
p_isc_event_block isc_event_block;

typedef void        (__cdecl * p_isc_expand_dpb       )(ISC_SCHAR**,short*, ...);
p_isc_expand_dpb isc_expand_dpb;

typedef ISC_STATUS  (__cdecl * p_isc_start_transaction)(ISC_STATUS*,isc_tr_handle*,short, ...);
p_isc_start_transaction isc_start_transaction;




};

extern TIBCLLib   cl_lib;

#ifndef _NO_NAMESPACE
//namespace KeRTL
}
#endif

#endif
