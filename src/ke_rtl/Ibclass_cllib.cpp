#include <iblib_i.h>

#ifndef _NO_NAMESPACE
namespace KeRTL
{
using namespace KeRTL;
#endif

 


 bool __fastcall TIBCLLib::load_library(const char * lib_path)
 {
  if(IsLoaded()) Unload();
  bool ret = Load(lib_path);
  if(!ret)
  {
    char err_text [2048];
    GetErrorMessage(GetLastError(),err_text,sizeof(err_text),(HINSTANCE)NULL);
    GetErrorMessage(GetLastError(),err_text,sizeof(err_text),(HINSTANCE)NULL);
  }
  return ret;
 }

 void __fastcall TIBCLLib::AfterLoad()
 {
  isc_event_block       = (p_isc_event_block      )GetProcAddress("isc_event_block");
  isc_expand_dpb        = (p_isc_expand_dpb       )GetProcAddress("isc_expand_dpb");
  isc_start_transaction = (p_isc_start_transaction)GetProcAddress("isc_start_transaction");


  isc_attach_database.          assign(hModule,"isc_attach_database");
  isc_array_gen_sdl.            assign(hModule,"isc_array_gen_sdl");
  isc_array_get_slice.          assign(hModule,"isc_array_get_slice");
  isc_array_lookup_bounds.      assign(hModule,"isc_array_lookup_bounds");
  isc_array_lookup_desc.        assign(hModule,"isc_array_lookup_desc");
  isc_array_set_desc.           assign(hModule,"isc_array_set_desc");
  isc_array_put_slice.          assign(hModule,"isc_array_put_slice");
  isc_blob_default_desc.        assign(hModule,"isc_blob_default_desc");
  isc_blob_gen_bpb.             assign(hModule,"isc_blob_gen_bpb");
  isc_blob_info.                assign(hModule,"isc_blob_info");
  isc_blob_lookup_desc.         assign(hModule,"isc_blob_lookup_desc");
  isc_blob_set_desc.            assign(hModule,"isc_blob_set_desc");
  isc_cancel_blob.              assign(hModule,"isc_cancel_blob");
  isc_cancel_events.            assign(hModule,"isc_cancel_events");
  isc_close_blob.               assign(hModule,"isc_close_blob");
  isc_commit_retaining.         assign(hModule,"isc_commit_retaining");
  isc_commit_transaction.       assign(hModule,"isc_commit_transaction");
  isc_create_blob.              assign(hModule,"isc_create_blob");
  isc_create_blob2.             assign(hModule,"isc_create_blob2");
  isc_create_database.          assign(hModule,"isc_create_database");
  isc_database_info.            assign(hModule,"isc_database_info");
  isc_decode_date.              assign(hModule,"isc_decode_date");
  isc_decode_sql_date.          assign(hModule,"isc_decode_sql_date");
  isc_decode_sql_time.          assign(hModule,"isc_decode_sql_time");
  isc_decode_timestamp.         assign(hModule,"isc_decode_timestamp");
  isc_detach_database.          assign(hModule,"isc_detach_database");
  isc_drop_database.            assign(hModule,"isc_drop_database");
  isc_dsql_allocate_statement.  assign(hModule,"isc_dsql_allocate_statement");
  isc_dsql_alloc_statement2.    assign(hModule,"isc_dsql_alloc_statement2");
  isc_dsql_describe.            assign(hModule,"isc_dsql_describe");
  isc_dsql_describe_bind.       assign(hModule,"isc_dsql_describe_bind");
  isc_dsql_exec_immed2.         assign(hModule,"isc_dsql_exec_immed2");
  isc_dsql_execute.             assign(hModule,"isc_dsql_execute");
  isc_dsql_execute2.            assign(hModule,"isc_dsql_execute2");
  isc_dsql_execute_immediate.   assign(hModule,"isc_dsql_execute_immediate");
  isc_dsql_fetch.               assign(hModule,"isc_dsql_fetch");
  isc_dsql_finish.              assign(hModule,"isc_dsql_finish");
  isc_dsql_free_statement.      assign(hModule,"isc_dsql_free_statement");
  isc_dsql_insert.              assign(hModule,"isc_dsql_insert");
  isc_dsql_prepare.             assign(hModule,"isc_dsql_prepare");
  isc_dsql_set_cursor_name.     assign(hModule,"isc_dsql_set_cursor_name");
  isc_dsql_sql_info.            assign(hModule,"isc_dsql_sql_info");
  isc_encode_date.              assign(hModule,"isc_encode_date");
  isc_encode_sql_date.          assign(hModule,"isc_encode_sql_date");
  isc_encode_sql_time.          assign(hModule,"isc_encode_sql_time");
  isc_encode_timestamp.         assign(hModule,"isc_encode_timestamp");
  isc_event_block_a.            assign(hModule,"isc_event_block_a");
  isc_event_block_s.            assign(hModule,"isc_event_block_s");
  isc_event_counts.             assign(hModule,"isc_event_counts");
  isc_modify_dpb.               assign(hModule,"isc_modify_dpb");
  isc_free.                     assign(hModule,"isc_free");
  isc_get_segment.              assign(hModule,"isc_get_segment");
  isc_get_slice.                assign(hModule,"isc_get_slice");
  isc_interprete.               assign(hModule,"isc_interprete");
  fb_interpret.                 assign(hModule,"fb_interpret");
  isc_open_blob.                assign(hModule,"isc_open_blob");
  isc_open_blob2.               assign(hModule,"isc_open_blob2");
  isc_prepare_transaction2.     assign(hModule,"isc_prepare_transaction2");
  isc_print_sqlerror.           assign(hModule,"isc_print_sqlerror");
  isc_print_status.             assign(hModule,"isc_print_status");
  isc_put_segment.              assign(hModule,"isc_put_segment");
  isc_put_slice.                assign(hModule,"isc_put_slice");
  isc_que_events.               assign(hModule,"isc_que_events");
  isc_rollback_retaining.       assign(hModule,"isc_rollback_retaining");
  isc_rollback_transaction.     assign(hModule,"isc_rollback_transaction");
  isc_start_multiple.           assign(hModule,"isc_start_multiple");
  fb_disconnect_transaction.    assign(hModule,"fb_disconnect_transaction");
  isc_sqlcode.                  assign(hModule,"isc_sqlcode");
  isc_sqlcode_s.                assign(hModule,"isc_sqlcode_s");
  fb_sqlstate.                  assign(hModule,"fb_sqlstate");
  isc_sql_interprete.           assign(hModule,"isc_sql_interprete");
  isc_transaction_info.         assign(hModule,"isc_transaction_info");
  isc_transact_request.         assign(hModule,"isc_transact_request");
  isc_vax_integer.              assign(hModule,"isc_vax_integer");
  isc_portable_integer.         assign(hModule,"isc_portable_integer");
  isc_add_user.                 assign(hModule,"isc_add_user");
  isc_delete_user.              assign(hModule,"isc_delete_user");
  isc_modify_user.              assign(hModule,"isc_modify_user");
  isc_compile_request.          assign(hModule,"isc_compile_request");
  isc_compile_request2.         assign(hModule,"isc_compile_request2");
  isc_ddl.                      assign(hModule,"isc_ddl");
  isc_prepare_transaction.      assign(hModule,"isc_prepare_transaction");
  isc_receive.                  assign(hModule,"isc_receive");
  isc_reconnect_transaction.    assign(hModule,"isc_reconnect_transaction");
  isc_release_request.          assign(hModule,"isc_release_request");
  isc_request_info.             assign(hModule,"isc_request_info");
  isc_seek_blob.                assign(hModule,"isc_seek_blob");
  isc_send.                     assign(hModule,"isc_send");
  isc_start_and_send.           assign(hModule,"isc_start_and_send");
  isc_start_request.            assign(hModule,"isc_start_request");
  isc_unwind_request.           assign(hModule,"isc_unwind_request");
  isc_wait_for_event.           assign(hModule,"isc_wait_for_event");
  isc_close.                    assign(hModule,"isc_close");
  isc_declare.                  assign(hModule,"isc_declare");
  isc_describe.                 assign(hModule,"isc_describe");
  isc_describe_bind.            assign(hModule,"isc_describe_bind");
  isc_execute.                  assign(hModule,"isc_execute");
  isc_execute_immediate.        assign(hModule,"isc_execute_immediate");
  isc_fetch.                    assign(hModule,"isc_fetch");
  isc_open.                     assign(hModule,"isc_open");
  isc_prepare.                  assign(hModule,"isc_prepare");
  isc_dsql_execute_m.           assign(hModule,"isc_dsql_execute_m");
  isc_dsql_execute2_m.          assign(hModule,"isc_dsql_execute2_m");
  isc_dsql_execute_immediate_m. assign(hModule,"isc_dsql_execute_immediate_");
  isc_dsql_exec_immed3_m.       assign(hModule,"isc_dsql_exec_immed3_m");
  isc_dsql_fetch_m.             assign(hModule,"isc_dsql_fetch_m");
  isc_dsql_insert_m.            assign(hModule,"isc_dsql_insert_m");
  isc_dsql_prepare_m.           assign(hModule,"isc_dsql_prepare_m");
  isc_dsql_release.             assign(hModule,"isc_dsql_release");
  isc_embed_dsql_close.         assign(hModule,"isc_embed_dsql_close");
  isc_embed_dsql_declare.       assign(hModule,"isc_embed_dsql_declare");
  isc_embed_dsql_describe.      assign(hModule,"isc_embed_dsql_describe");
  isc_embed_dsql_describe_bind. assign(hModule,"isc_embed_dsql_describe_bin");
  isc_embed_dsql_execute.       assign(hModule,"isc_embed_dsql_execute");
  isc_embed_dsql_execute2.      assign(hModule,"isc_embed_dsql_execute2");
  isc_embed_dsql_execute_immed. assign(hModule,"isc_embed_dsql_execute_imme");
  isc_embed_dsql_fetch.         assign(hModule,"isc_embed_dsql_fetch");
  isc_embed_dsql_fetch_a.       assign(hModule,"isc_embed_dsql_fetch_a");
  isc_embed_dsql_length.        assign(hModule,"isc_embed_dsql_length");
  isc_embed_dsql_open.          assign(hModule,"isc_embed_dsql_open");
  isc_embed_dsql_open2.         assign(hModule,"isc_embed_dsql_open2");
  isc_embed_dsql_insert.        assign(hModule,"isc_embed_dsql_insert");
  isc_embed_dsql_prepare.       assign(hModule,"isc_embed_dsql_prepare");
  isc_embed_dsql_release.       assign(hModule,"isc_embed_dsql_release");
  BLOB_open.                    assign(hModule,"BLOB_open");
  BLOB_put.                     assign(hModule,"BLOB_put");
  BLOB_close.                   assign(hModule,"BLOB_close");
  BLOB_get.                     assign(hModule,"BLOB_get");
  BLOB_display.                 assign(hModule,"BLOB_display");
  BLOB_dump.                    assign(hModule,"BLOB_dump");
  BLOB_edit.                    assign(hModule,"BLOB_edit");
  BLOB_load.                    assign(hModule,"BLOB_load");
  BLOB_text_dump.               assign(hModule,"BLOB_text_dump");
  BLOB_text_load.               assign(hModule,"BLOB_text_load");
  Bopen.                        assign(hModule,"Bopen");
  isc_ftof.                     assign(hModule,"isc_ftof");
  isc_print_blr.                assign(hModule,"isc_print_blr");
  fb_print_blr.                 assign(hModule,"fb_print_blr");
  isc_set_debug.                assign(hModule,"isc_set_debug");
  isc_qtoq.                     assign(hModule,"isc_qtoq");
  isc_vtof.                     assign(hModule,"isc_vtof");
  isc_vtov.                     assign(hModule,"isc_vtov");
  isc_version.                  assign(hModule,"isc_version");
  isc_reset_fpe.                assign(hModule,"isc_reset_fpe");
  isc_baddress.                 assign(hModule,"isc_baddress");
  isc_baddress_s.               assign(hModule,"isc_baddress_s");
  isc_service_attach.           assign(hModule,"isc_service_attach");
  isc_service_detach.           assign(hModule,"isc_service_detach");
  isc_service_query.            assign(hModule,"isc_service_query");
  isc_service_start.            assign(hModule,"isc_service_start");
  fb_shutdown.                  assign(hModule,"fb_shutdown");
  fb_shutdown_callback.         assign(hModule,"fb_shutdown_callback");
  fb_cancel_operation.          assign(hModule,"fb_cancel_operation");
  isc_get_client_version.       assign(hModule,"isc_get_client_version");
  isc_get_client_major_version. assign(hModule,"isc_get_client_major_version");
  isc_get_client_minor_version. assign(hModule,"isc_get_client_minor_version");
}

 void __fastcall TIBCLLib::BeforeUnload()
 {
  //Обнулить все импортируемые ф-ции
  isc_event_block       = NULL;
  isc_expand_dpb        = NULL;
  isc_start_transaction = NULL;

  isc_attach_database.          assign(hModule,NULL);
  isc_array_gen_sdl.            assign(hModule,NULL);
  isc_array_get_slice.          assign(hModule,NULL);
  isc_array_lookup_bounds.      assign(hModule,NULL);
  isc_array_lookup_desc.        assign(hModule,NULL);
  isc_array_set_desc.           assign(hModule,NULL);
  isc_array_put_slice.          assign(hModule,NULL);
  isc_blob_default_desc.        assign(hModule,NULL);
  isc_blob_gen_bpb.             assign(hModule,NULL);
  isc_blob_info.                assign(hModule,NULL);
  isc_blob_lookup_desc.         assign(hModule,NULL);
  isc_blob_set_desc.            assign(hModule,NULL);
  isc_cancel_blob.              assign(hModule,NULL);
  isc_cancel_events.            assign(hModule,NULL);
  isc_close_blob.               assign(hModule,NULL);
  isc_commit_retaining.         assign(hModule,NULL);
  isc_commit_transaction.       assign(hModule,NULL);
  isc_create_blob.              assign(hModule,NULL);
  isc_create_blob2.             assign(hModule,NULL);
  isc_create_database.          assign(hModule,NULL);
  isc_database_info.            assign(hModule,NULL);
  isc_decode_date.              assign(hModule,NULL);
  isc_decode_sql_date.          assign(hModule,NULL);
  isc_decode_sql_time.          assign(hModule,NULL);
  isc_decode_timestamp.         assign(hModule,NULL);
  isc_detach_database.          assign(hModule,NULL);
  isc_drop_database.            assign(hModule,NULL);
  isc_dsql_allocate_statement.  assign(hModule,NULL);
  isc_dsql_alloc_statement2.    assign(hModule,NULL);
  isc_dsql_describe.            assign(hModule,NULL);
  isc_dsql_describe_bind.       assign(hModule,NULL);
  isc_dsql_exec_immed2.         assign(hModule,NULL);
  isc_dsql_execute.             assign(hModule,NULL);
  isc_dsql_execute2.            assign(hModule,NULL);
  isc_dsql_execute_immediate.   assign(hModule,NULL);
  isc_dsql_fetch.               assign(hModule,NULL);
  isc_dsql_finish.              assign(hModule,NULL);
  isc_dsql_free_statement.      assign(hModule,NULL);
  isc_dsql_insert.              assign(hModule,NULL);
  isc_dsql_prepare.             assign(hModule,NULL);
  isc_dsql_set_cursor_name.     assign(hModule,NULL);
  isc_dsql_sql_info.            assign(hModule,NULL);
  isc_encode_date.              assign(hModule,NULL);
  isc_encode_sql_date.          assign(hModule,NULL);
  isc_encode_sql_time.          assign(hModule,NULL);
  isc_encode_timestamp.         assign(hModule,NULL);
  isc_event_block_a.            assign(hModule,NULL);
  isc_event_block_s.            assign(hModule,NULL);
  isc_event_counts.             assign(hModule,NULL);
  isc_modify_dpb.               assign(hModule,NULL);
  isc_free.                     assign(hModule,NULL);
  isc_get_segment.              assign(hModule,NULL);
  isc_get_slice.                assign(hModule,NULL);
  isc_interprete.               assign(hModule,NULL);
  fb_interpret.                 assign(hModule,NULL);
  isc_open_blob.                assign(hModule,NULL);
  isc_open_blob2.               assign(hModule,NULL);
  isc_prepare_transaction2.     assign(hModule,NULL);
  isc_print_sqlerror.           assign(hModule,NULL);
  isc_print_status.             assign(hModule,NULL);
  isc_put_segment.              assign(hModule,NULL);
  isc_put_slice.                assign(hModule,NULL);
  isc_que_events.               assign(hModule,NULL);
  isc_rollback_retaining.       assign(hModule,NULL);
  isc_rollback_transaction.     assign(hModule,NULL);
  isc_start_multiple.           assign(hModule,NULL);
  fb_disconnect_transaction.    assign(hModule,NULL);
  isc_sqlcode.                  assign(hModule,NULL);
  isc_sqlcode_s.                assign(hModule,NULL);
  fb_sqlstate.                  assign(hModule,NULL);
  isc_sql_interprete.           assign(hModule,NULL);
  isc_transaction_info.         assign(hModule,NULL);
  isc_transact_request.         assign(hModule,NULL);
  isc_vax_integer.              assign(hModule,NULL);
  isc_portable_integer.         assign(hModule,NULL);
  isc_add_user.                 assign(hModule,NULL);
  isc_delete_user.              assign(hModule,NULL);
  isc_modify_user.              assign(hModule,NULL);
  isc_compile_request.          assign(hModule,NULL);
  isc_compile_request2.         assign(hModule,NULL);
  isc_ddl.                      assign(hModule,NULL);
  isc_prepare_transaction.      assign(hModule,NULL);
  isc_receive.                  assign(hModule,NULL);
  isc_reconnect_transaction.    assign(hModule,NULL);
  isc_release_request.          assign(hModule,NULL);
  isc_request_info.             assign(hModule,NULL);
  isc_seek_blob.                assign(hModule,NULL);
  isc_send.                     assign(hModule,NULL);
  isc_start_and_send.           assign(hModule,NULL);
  isc_start_request.            assign(hModule,NULL);
  isc_unwind_request.           assign(hModule,NULL);
  isc_wait_for_event.           assign(hModule,NULL);
  isc_close.                    assign(hModule,NULL);
  isc_declare.                  assign(hModule,NULL);
  isc_describe.                 assign(hModule,NULL);
  isc_describe_bind.            assign(hModule,NULL);
  isc_execute.                  assign(hModule,NULL);
  isc_execute_immediate.        assign(hModule,NULL);
  isc_fetch.                    assign(hModule,NULL);
  isc_open.                     assign(hModule,NULL);
  isc_prepare.                  assign(hModule,NULL);
  isc_dsql_execute_m.           assign(hModule,NULL);
  isc_dsql_execute2_m.          assign(hModule,NULL);
  isc_dsql_execute_immediate_m. assign(hModule,NULL);
  isc_dsql_exec_immed3_m.       assign(hModule,NULL);
  isc_dsql_fetch_m.             assign(hModule,NULL);
  isc_dsql_insert_m.            assign(hModule,NULL);
  isc_dsql_prepare_m.           assign(hModule,NULL);
  isc_dsql_release.             assign(hModule,NULL);
  isc_embed_dsql_close.         assign(hModule,NULL);
  isc_embed_dsql_declare.       assign(hModule,NULL);
  isc_embed_dsql_describe.      assign(hModule,NULL);
  isc_embed_dsql_describe_bind. assign(hModule,NULL);
  isc_embed_dsql_execute.       assign(hModule,NULL);
  isc_embed_dsql_execute2.      assign(hModule,NULL);
  isc_embed_dsql_execute_immed. assign(hModule,NULL);
  isc_embed_dsql_fetch.         assign(hModule,NULL);
  isc_embed_dsql_fetch_a.       assign(hModule,NULL);
  isc_embed_dsql_length.        assign(hModule,NULL);
  isc_embed_dsql_open.          assign(hModule,NULL);
  isc_embed_dsql_open2.         assign(hModule,NULL);
  isc_embed_dsql_insert.        assign(hModule,NULL);
  isc_embed_dsql_prepare.       assign(hModule,NULL);
  isc_embed_dsql_release.       assign(hModule,NULL);
  BLOB_open.                    assign(hModule,NULL);
  BLOB_put.                     assign(hModule,NULL);
  BLOB_close.                   assign(hModule,NULL);
  BLOB_get.                     assign(hModule,NULL);
  BLOB_display.                 assign(hModule,NULL);
  BLOB_dump.                    assign(hModule,NULL);
  BLOB_edit.                    assign(hModule,NULL);
  BLOB_load.                    assign(hModule,NULL);
  BLOB_text_dump.               assign(hModule,NULL);
  BLOB_text_load.               assign(hModule,NULL);
  Bopen.                        assign(hModule,NULL);
  isc_ftof.                     assign(hModule,NULL);
  isc_print_blr.                assign(hModule,NULL);
  fb_print_blr.                 assign(hModule,NULL);
  isc_set_debug.                assign(hModule,NULL);
  isc_qtoq.                     assign(hModule,NULL);
  isc_vtof.                     assign(hModule,NULL);
  isc_vtov.                     assign(hModule,NULL);
  isc_version.                  assign(hModule,NULL);
  isc_reset_fpe.                assign(hModule,NULL);
  isc_baddress.                 assign(hModule,NULL);
  isc_baddress_s.               assign(hModule,NULL);
  isc_service_attach.           assign(hModule,NULL);
  isc_service_detach.           assign(hModule,NULL);
  isc_service_query.            assign(hModule,NULL);
  isc_service_start.            assign(hModule,NULL);
  fb_shutdown.                  assign(hModule,NULL);
  fb_shutdown_callback.         assign(hModule,NULL);
  fb_cancel_operation.          assign(hModule,NULL);
  isc_get_client_version.       assign(hModule,NULL);
  isc_get_client_major_version. assign(hModule,NULL);
  isc_get_client_minor_version. assign(hModule,NULL);


}


#ifndef _NO_NAMESPACE
}
#endif

