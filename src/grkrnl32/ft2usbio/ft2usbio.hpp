#ifndef _FT2USBIO_HPP_
#define _FT2USBIO_HPP_

#include <ft2usbio.h>
#include <kedatas.hpp>
#include <rx_buffer.hpp>



class  TUsbIo:public TGKIOBase,public TGKThread
{
  protected:
  typedef                      TGKIOBase  Inherit;
  wchar_t                      device_name[MAX_PATH];
  DWORD                        io_speed;

  DWORD                        data_type;
  DWORD                        report_id;
  BOOL                         error_open_log_write;

  FT_HANDLE                    fthDevice;
  KeRTL::TSimpleDataQueue      rd_queue;
  KeRTL::TSimpleDataQueue      wr_queue[4];
  HANDLE                       stop_event;
  HANDLE                       wr_event;
  TWaitableTimer               tx_timer;
  HANDLE                       rx_event;
  TSynchroSet                  synchro_set;

  rx_buffer                    rxb;
  DWORD                        tx_packet_counter;
  DWORD                        last_packet_num;
  DWORD                        total_recv_bytes;
  DWORD                        success_recv_bytes;
  DWORD                        error_recv_bytes;
  DWORD                        last_tx_time;
  DWORD                        last_rx_time;

  void    __fastcall  tx_watchdog();
  DWORD   __fastcall  report_error      (wchar_t * text,LPVOID data,DWORD data_sz,DWORD type=REPORT_ERROR_TYPE);
  void    __fastcall  check_packet_num(DWORD new_num);
  DWORD   __fastcall  OnException(DWORD ExceptionCode,LPEXCEPTION_POINTERS);


  LRESULT __fastcall enum_devices( LPMIO_ENUM_DEVICES ed);
  WORD    __fastcall get_type(){return MT_GRSCIO ;}
  DWORD   __fastcall get_config_data(DWORD mask,LPVOID buf,DWORD bsz);
  bool    __fastcall set_config_data(DWORD mask,LPVOID buf,DWORD bsz);
  bool    __fastcall compare_config_data(DWORD mask,LPVOID buf,DWORD bsz,LPDWORD ch_mask,BOOL * restart);


  DWORD    __fastcall queue_count();
  int      __fastcall Execute ();
  void     __fastcall on_timer();
  void     __fastcall handle_usb_read();
  void     __fastcall handle_rx(rx_buffer & rxb);
  void     __fastcall handle_kadr(lpusb_dev_kadr kadr);
  void     __fastcall handle_internal_kadr(lpusb_dev_kadr kadr);
  bool     __fastcall prepare_tx_packet(usb_dev_txpacket & packet);
  void     __fastcall do_tx_usbdata();

  DWORD     __fastcall device_send (lpusb_dev_txpacket packet);
  void      __fastcall device_reset();

  DWORD    __fastcall start(DWORD reason,LPARAM p);
  DWORD    __fastcall stop(DWORD reason);
  LRESULT  __fastcall read       (LPMIO_REQUEST) ;
  LRESULT  __fastcall write      (LPMIO_REQUEST) ;
  bool     __fastcall is_ready   (bool tx);
  bool     __fastcall open_device();
  bool     __fastcall close_device();
  static
  DWORD __fastcall usb_proto_convert(LPBYTE in,DWORD in_len,LPBYTE out,DWORD out_size,LPDWORD out_bytes);
  static
  DWORD __fastcall usb_proto_deconvert(LPBYTE in,DWORD in_len,LPBYTE out,DWORD out_size,LPDWORD out_bytes);
  virtual  DWORD      __fastcall check_license(){return MOD_LICENSE_REGISTERED; }

  public:
  TUsbIo(wchar_t * name);

};

#define WR_TERMINATE  0  //Событие завершения
#define WR_WREVENT    1  //Событие о наличии данных на передачу
#define WR_WRTIMER    2  //Таймер отложенной передачи
#define WR_USBRX      3  //Событие наличия  данных для приёма


#endif

