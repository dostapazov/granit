#ifndef __GRANIT_MODEM_HPP
#define __GRANIT_MODEM_HPP

#include  "granit_modem.h"
#include  <modem.hpp>
#include  <Kedatas.hpp>
#include  <kestrm.hpp>
#include  <gkio_base.h>

class sc_floor;

using namespace KeRTL;

class TSCGranitModem:public TModemBase
{
 protected:
 grsc_settings           settings;
 DWORD                   grsc_event_rep;
 DWORD                   grsc_tutr_rep;
 TGKIoInterface          io_iface;

 void    __fastcall reg_reports();
 DWORD   __fastcall get_mem_used();
 void    __fastcall release(LPVOID arg);

 bool    __fastcall check_config_data  (DWORD mask,LPVOID buf,DWORD bsz);
 DWORD   __fastcall get_config_data(DWORD mask,LPVOID buf,DWORD bsz);
 bool    __fastcall set_config_data(DWORD mask,LPVOID buf,DWORD bsz);
 bool    __fastcall compare_config_data(DWORD mask,LPVOID buf,DWORD bsz,LPDWORD ch_mask,BOOL * restart);
 bool    __fastcall read_settings();
 bool    __fastcall write_settings();
 DWORD   __fastcall get_window_module (wchar_t * buf,DWORD bsz);
 DWORD   __fastcall start(DWORD reason,LPARAM start_param);
 DWORD   __fastcall stop (DWORD reason);

 LRESULT __fastcall processing(DWORD cmd,LPARAM p1,LPARAM p2);
 LRESULT __fastcall reset_tu_error(DWORD addr,DWORD obj);

 DWORD   __fastcall get_modem_flags(){return MPF_DATA_SOURCE;}
 void    __fastcall refresh(DWORD cp,DWORD fa,DWORD sb);
 void    __fastcall free_line (modem_line * line) {delete line;}
 int     __fastcall convert_rx_data(LPWORD fa,LPBYTE in,int in_len,LPBYTE out,DWORD out_sz);
 LRESULT __fastcall send        (LPMPROTO_HEADER mph,DWORD sz);

 DWORD   __fastcall get_lines_limit(){return 16;}
 DWORD   __fastcall get_pu_number();

 DWORD   __fastcall sc_rx(LPMIO_REQUEST data, DWORD len    );
 DWORD   __fastcall io_error(DWORD err ,DWORD param);
 void    __fastcall log_rx_error(DWORD channel,gr_proto & gp,DWORD proto_error);
 LRESULT __fastcall get_props(DWORD addr,LPVOID ptr);
 LRESULT __fastcall set_props(DWORD addr,LPVOID ptr);
 LRESULT __fastcall enum_cp (DWORD index,LPGR_CP_PROPS  cpp);
 LRESULT __fastcall enum_grp(DWORD index,LPGR_GRP_PROPS grp);
 LRESULT __fastcall sc_create_floor(DWORD addr ,LPGR_FLOOR_PROPS  fpr);
 LRESULT __fastcall sc_create_cp   (DWORD addr ,LPGR_CP_PROPS     cpp);
 LRESULT __fastcall sc_create_group(DWORD addr ,LPGR_GRP_PROPS    grp);
 LRESULT __fastcall sc_remove(DWORD addr);

 LRESULT __fastcall get_dynamic (DWORD addr,LPVOID dyn_data);
 LRESULT __fastcall get_otd_data(DWORD addr,LPSC_OTDDATA scod);
 LRESULT __fastcall test_tutr   (LPSC_TEST_TUTR tt);

 sc_floor * __fastcall get_floor(DWORD num);
 void       __fastcall write_floors();
 void       __fastcall read_floors();
 DWORD      __fastcall import_floors   (wchar_t * fname);
 DWORD      __fastcall do_import_floors(wchar_t * fname);
 LRESULT __fastcall get_tu_entry(LPGRSC_TU_ENTRY tue);
 LRESULT __fastcall add_tu_entry(LPGRSC_TU_ENTRY tue);
 LRESULT __fastcall set_tu_entry(LPGRSC_TU_ENTRY tue);
 LRESULT __fastcall del_tu_entry(LPGRSC_TU_ENTRY tue);
 LRESULT __fastcall enum_tu_entry(LPGRSC_TU_ENTRY tue,DWORD idx);
 virtual  DWORD      __fastcall check_license(){return MOD_LICENSE_REGISTERED; }
 public:
 TSCGranitModem();
 typedef TModemBase Inherit;
 DWORD   __fastcall sc_tx(LPBYTE data,DWORD data_len);
 void    __fastcall report_modem(DWORD rep_id,int rep_type,wchar_t * text,LPVOID data ,DWORD data_sz );
 void    __fastcall event_report(int rep_type,wchar_t * text,LPVOID data = 0,DWORD data_sz = 0);
 void    __fastcall tutr_report (int rep_type,wchar_t * text,LPVOID data = 0,DWORD data_sz = 0);
 void    __fastcall config_change(DWORD floor_num);
};

 inline void    __fastcall TSCGranitModem::event_report(int rep_type,wchar_t * text,LPVOID data ,DWORD data_sz )
 {
  report_modem(grsc_event_rep,rep_type,text,data,data_sz);
 }

 inline void    __fastcall TSCGranitModem::tutr_report (int rep_type,wchar_t * text,LPVOID data ,DWORD data_sz )
 {
  report_modem(grsc_tutr_rep,rep_type,text,data,data_sz);
 }





#endif

