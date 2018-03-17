/*

  Вывод на щит "Гранит" протокол  HDLC

*/

#ifndef _GRAN_TABLET_HPP
#define _GRAN_TABLET_HPP

#include "grantablet.h"
#include <modem.hpp>
#include <gkio_base.h>
#include <mediumdb.h>
#include <otdm_proto.h>
#include <vector>
#include <algorithm>
#include <list>
#include <tregstry.hpp>
#include <kestrm.hpp>


using namespace KeRTL;
using namespace std;

typedef vector<mdb_kadr_entry>    kadr_entry_list;
typedef vector<mdb_short_record>  srecord_list;
typedef list<DWORD> changes_list;



struct output_kadr
{
 DWORD             output_mode;      //Режим вывода
 DWORD             output_position ; //Номер места для вывода 
 mdb_kadr          kadr;             //Описатель выводимого кадра
 kadr_entry_list   entry_list;
 wchar_t           kadr_name[MAX_PATH];
 output_kadr(){kadr.kadr_id = -1;kadr_name[0] = 0;}
 output_kadr(DWORD  kid,DWORD mode,DWORD o_pos){kadr.kadr_id = kid; output_mode = mode; output_position = o_pos;kadr_name[0] = 0; }
 output_kadr(const output_kadr & kadr){*this = kadr;}
 output_kadr & operator = (const  output_kadr & kdr)
 {kadr.kadr_id = kdr.kadr.kadr_id;output_mode = kdr.output_mode; output_position = kdr.output_position; safe_strcpy(kadr_name,kdr.kadr_name) ;return * this;}

};

inline bool operator < (const output_kadr & ok1,const output_kadr & ok2)
{
 if(ok1.kadr.kadr_id == ok2.kadr.kadr_id)
   return ok1.output_position < ok2.output_position ? true:false;
 return ok1.kadr.kadr_id < ok2.kadr.kadr_id ? true:false;
}

typedef std::vector<output_kadr>  output_kadr_vector;
typedef std::vector<mdb_short_record>   record_vector;


/* Этаж вывода */
class TOutputFloor: public modem_line,public TGKThread
{
 protected:
 TEvent              terminate_event;
 output_kadr_vector  kadrs;
 record_vector       records;
 DWORD               current_kadr;

 DWORD __fastcall get_line_text  (wchar_t * text,DWORD text_sz) ;
 DWORD __fastcall get_max_frame_size() ;
 DWORD __fastcall get_tx_queue_size()  ;
 bool  __fastcall do_start     ();
 bool  __fastcall do_stop      ();
 void  __fastcall on_connect   ();
 void  __fastcall on_disconnect();

 void __fastcall  clear_floor();
 void __fastcall  release();

 DWORD __fastcall refresh(DWORD sbl);

 bool  __fastcall BeforeExecute();
 void  __fastcall BeforeTerminate();
 int   __fastcall Execute();
 void  __fastcall on_timeout();
 void  __fastcall Terminate(){terminate_event.SetEvent(true);TGKThread::Terminate();}
 const char  * __fastcall get_gkthread_name(){return "OutputFloor thread ";}
 output_kadr_vector::iterator __fastcall lower_kadr(DWORD kid,DWORD position);
 output_kadr_vector::iterator __fastcall find_kadr(DWORD kid,DWORD pos = 0);
 output_kadr_vector::iterator __fastcall find_kadr_by_position(DWORD pos);
 record_vector::iterator      __fastcall lower_record(DWORD rec_id);
 record_vector::iterator      __fastcall find_record(DWORD rec_id);
 void          __fastcall handle_kadr(LPOTDM_PROTO_KADR kdr);
 void          __fastcall handle_kadr_entryes(LPOTDM_PROTO_KADR_ENTRYES entryes,bool beg_data,bool end_data);
 void          __fastcall handle_records(LPOTDM_PROTO_KADR_RECORDS k_recs);
 void          __fastcall handle_record_changes(LPOTDM_PROTO_RECORDS records);
 void          __fastcall do_output_kadr(DWORD rec_id,DWORD pos);
 void          __fastcall do_output_kadr(DWORD rec_id);
 void          __fastcall do_output_kadr(output_kadr_vector::iterator k_ptr);

 void          __fastcall  do_output_kadr_ready(output_kadr_vector::iterator k_ptr);
 void          __fastcall  do_output_kadr_discrete(output_kadr_vector::iterator k_ptr);
 void          __fastcall  do_output_kadr_analog(output_kadr_vector::iterator k_ptr);
 void          __fastcall  do_output_analog_value(output_kadr_vector::iterator k_ptr,kadr_entry_list::iterator entry);
 void          __fastcall  make_order(DWORD kid);
 bool          __fastcall  output_kadr_changes(output_kadr & o_kdr,changes_list  & ch_list);


 public:
 TOutputFloor (DWORD  number);
 DWORD         __fastcall get_line_flags();
 bool          __fastcall send        (LPMPROTO_HEADER mph,DWORD sz) ;
 DWORD         __fastcall floor_tx    (LPBYTE data,DWORD data_len);
 LRESULT       __fastcall enum_output_kadrs( LPOUTPUT_KADR_PARAM okp,DWORD index);
 LRESULT       __fastcall get_output_kadr_param(LPOUTPUT_KADR_PARAM  okp);
 bool          __fastcall create_output_kadr(LPOUTPUT_KADR_PARAM kp);
 bool          __fastcall delete_output_kadr(LPOUTPUT_KADR_PARAM kp);
 bool          __fastcall modify_output_kadr(LPOUTPUT_KADR_PARAM kp,DWORD new_position);
 DWORD         __fastcall stream_write(TStream & strm);
 DWORD         __fastcall stream_read (TStream & strm);


};

class TGranTablet:public TModemBase
{
 protected:
 GRTBL_SETTINGS          settings;
 TGKIoInterface          io_iface;
 LRESULT       __fastcall processing(DWORD cmd,LPARAM p1,LPARAM p2);
 DWORD         __fastcall start(DWORD reason,LPARAM p2);
 DWORD         __fastcall stop (DWORD reason);
 DWORD         __fastcall get_window_module  (wchar_t * buf,DWORD bsz);
 DWORD         __fastcall get_config_data(DWORD mask,LPVOID buf,DWORD bsz);
 bool          __fastcall set_config_data(DWORD mask,LPVOID buf,DWORD bsz);
 bool          __fastcall compare_config_data(DWORD mask,LPVOID buf,DWORD bsz,LPDWORD ch_mask,BOOL * restart);
 bool          __fastcall check_config_data  (DWORD mask,LPVOID buf,DWORD bsz);
 bool          __fastcall read_settings  ();
 bool          __fastcall write_settings ();
 void          __fastcall write_floors(TRegsWriter& rw);
 void          __fastcall read_floors (TRegsReader& rs);

 LRESULT       __fastcall enum_output_kadrs( LPOUTPUT_KADR_PARAM okp,DWORD index);
 LRESULT       __fastcall get_output_kadr_param(LPOUTPUT_KADR_PARAM  okp);
 LRESULT       __fastcall start_enum_dbkadrs(GKHANDLE dest);
 LRESULT       __fastcall create_floor(DWORD num);
 LRESULT       __fastcall delete_floor(DWORD num);
 LRESULT       __fastcall set_floor_number(DWORD old_num,DWORD new_num);
 LRESULT       __fastcall create_output_kadr(LPOUTPUT_KADR_PARAM kp);
 LRESULT       __fastcall delete_output_kadr(LPOUTPUT_KADR_PARAM kp);
 LRESULT       __fastcall modify_output_kadr(LPOUTPUT_KADR_PARAM kp,DWORD new_position);

 public:

  TGranTablet ();
  void        __fastcall    free_line     (modem_line *) ;
  int         __fastcall    convert_rx_data(LPWORD fa,LPBYTE in,int in_len,LPBYTE out,DWORD out_sz) ;
  DWORD       __fastcall    get_modem_flags(){return MLPF_DATASOURCE;}

  LRESULT     __fastcall    recv          (LPMPROTO_HEADER mph,DWORD sz);
  LRESULT     __fastcall    send (LPMPROTO_HEADER mph,DWORD sz);
  DWORD       __fastcall    get_lines_limit() {return 4;}
  DWORD       __fastcall    get_mem_used();
  DWORD       __fastcall    floor_tx    (LPBYTE data,DWORD data_len);
  DWORD       __fastcall    io_error    (DWORD err ,DWORD param);
  DWORD      __fastcall     floor_rx(LPMIO_REQUEST io_req,DWORD len);


};

#endif
