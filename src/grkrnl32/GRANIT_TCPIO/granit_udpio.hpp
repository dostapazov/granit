#ifndef __GRANIT_UDPIO_HPP__
#define __GRANIT_UDPIO_HPP__

#pragma hdrstop

#include <gkio_base.h>
#include <kedatas.hpp>
#include <rx_buffer.hpp>
#include <wsockdef.h>
#include <gkmodules.hpp>
#include <ke_defs.h>
#include "granit_udpio.h"

#define GRUDPIO_RAW_FLAG_BEGIN     0x2A
#define GRUDPIO_RAW_FLAG_END       0x3B

/*���� ������*/

#define GRUDP_DT_GRANIT            0x30  // ������ - ������
#define GRUDP_DT_GRANIT_CONNECT    0x31  // ������ - ������ ����� �����
#define GRUDP_DT_TM512             0x40  // ������ ��512
#define GRUDP_DT_TM512_CONNECT     0x41  // ������ ��512  ����� �����
#define GRUDP_DT_VRTF3             0x50  // ������ ����-3
#define GRUDP_DT_VRTF3_CONNECT     0x51  // ������ ����-3 ����� �����

#define GRUDP_DT_FILLER            0x0E  //�����������

//����� 255
//IN
#define GRUDP_DT_RESTART            0x0F  //��������� � �����������
#define GRUDP_DT_BADCOMMAND         0xF1  //������ �������
#define GRUDP_DT_BADCHANNEL         0xF2  //������ ������ ������    data[0] = ����� ������������ ������
#define GRUDP_DT_INTERNAL_ERROR     0x0D  //������ ����������� ������
#define GRUDP_DT_SYNCERR            0xFE  //������ ������������� 2A - 3B data[0] - ����� ������
//OUT
#define GRUDP_DT_RESET              0xF0 //������� ����������� ����������



#define MT_GRSCIO                   (MT_IO+2)

/*
   0x8X                         ������      1  cmd, 2 err_chanel 3 - sync  15 internal
   0x4X                         ���         1  �����������
   0x1X                         ������      0 = ������, 1 - ��512 2- ����-3
   0�18X
   0x0X                         �j�����

*/


/*���� �� ����������*/
typedef struct _udp_dev_kadr
{
 BYTE       chanel;
 BYTE       data_len;
 BYTE       data_type;
 BYTE       data[24];
}udp_dev_kadr,*lpudp_dev_kadr;


/*����� �� ����������*/
typedef struct _udp_dev_rxpacket
{
 //BYTE         packet_type;
 BYTE           packet_len;
 BYTE           packet_num;
 udp_dev_kadr   kadrs[4];
}udp_dev_rxpacket,* lpudp_dev_rxpacket;

#define UDP_RX_RAWDATA_SIZE ((sizeof(udp_dev_rxpacket)<<1)+2*sizeof(BYTE))
#define UDP_TX_RAWDATA_SIZE ((sizeof(udp_dev_txpacket)<<1)+2*sizeof(BYTE))

typedef struct _udp_dev_txpacket
{
 //BYTE         packet_type;
 BYTE           packet_len;
 BYTE           packet_num;
 udp_dev_kadr   kadrs[2];
}udp_dev_txpacket,* lpudp_dev_txpacket;


class TGranitUdpIo:public TGKIOBase,public TGKThread
{
  typedef                      TGKIOBase  Inherit;
  wchar_t                      remote_addr[MAX_PATH];
  int                          remote_port;
  enum   net_mode_t {udp_mode = 0,tcp_mode = 1};
  net_mode_t                   net_mode;
  DWORD                        data_type;
  DWORD                        report_id;
  BOOL                         error_open_log_write;

  TIPSocket                  * socket;
  TIPAddress                   host_addr;
  BOOL                         is_connected;

  KeRTL::TSimpleDataQueue      rd_queue;
  KeRTL::TSimpleDataQueue      wr_queue[4];
  HANDLE                       stop_event;
  HANDLE                       wr_event;
  TWaitableTimer               tx_timer;

  HANDLE                       socket_event;

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
  DWORD   __fastcall queue_count();
  int     __fastcall Execute ();
  void    __fastcall on_timer();
  void    __fastcall handle_udp_read();
  void    __fastcall handle_kadr(lpudp_dev_kadr kadr);
  void    __fastcall handle_internal_kadr(lpudp_dev_kadr kadr);
  bool    __fastcall prepare_tx_packet(udp_dev_txpacket & packet);
  void    __fastcall do_tx_udpdata();

  DWORD   __fastcall device_send (lpudp_dev_txpacket packet);
  void    __fastcall device_reset();

  BOOL     __fastcall can_start(DWORD reason,LPARAM p2);
  DWORD    __fastcall start      (DWORD reason,LPARAM p);
  DWORD    __fastcall stop       (DWORD reason);
  LRESULT  __fastcall read       (LPMIO_REQUEST) ;
  LRESULT  __fastcall write      (LPMIO_REQUEST) ;
  bool     __fastcall is_ready   (bool tx);
  bool     __fastcall socket_create();
  bool     __fastcall socket_destroy();
  void     __fastcall handle_socket_events();
  void     __fastcall handle_connect      (DWORD error);
  void     __fastcall handle_disconnect   (DWORD error);
  void     __fastcall handle_recv         (DWORD error);
  void     __fastcall handle_recv         (rx_buffer & rxb);
  int      __fastcall socket_read         ();




  static
  DWORD __fastcall udp_proto_convert(LPBYTE in,DWORD in_len,LPBYTE out,DWORD out_size,LPDWORD out_bytes);
  static
  DWORD __fastcall udp_proto_deconvert(LPBYTE in,DWORD in_len,LPBYTE out,DWORD out_size,LPDWORD out_bytes);
  virtual  DWORD      __fastcall check_license(){return MOD_LICENSE_REGISTERED; }

  public:
  TGranitUdpIo(wchar_t * name);


};

#define WR_TERMINATE     0  //������� ����������
#define WR_WREVENT       1  //������� � ������� ������ �� ��������
#define WR_WRTIMER       2  //������ ���������� ��������
#define WR_SOCKET_EVENT  3  //������� �������  ������ ��� �����


#endif



