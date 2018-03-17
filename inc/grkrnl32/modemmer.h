#ifndef __MODEMMER_INC
#define __MODEMMER_INC

#include <gkmodules.h>
#include <modem_types.h>

#define IDS_MODEM_INFO 200

#define IDS_MODEMMER_REP_CHNUMBER     1000
#define IDS_MODEMMER_REP_REMOVEKADRS  1001
#define IDS_MODEMMER_REP_TXERR        1002

#ifndef RC_INVOKED  // Not a Resource Compiler
#include <modemmer_err.h>
#include <otd.h>
#include <modem_proto.h>

#define MODEMMER_HANDLE_NAME_A  "MODEMMER"
#define MODEMMER_HANDLE_NAME_W L"MODEMMER"

#define MODEMMER_FAHANDLERS_TERMINATE_NAME_W L"$fa_handlers.terminate$"
#define MODEMMER_FAHANDLERS_TERMINATE_NAME_A  "$fa_handlers.terminate$"

#define MDMCM_SETSPEED MCMD(MT_MODEMMER,1000)

#define MAX_MODEM_LINES_COUNT 255
#define MAX_MODEM_COUNT 255
#define MAX_FA_COUNT    255


#define REGSTR_REDIR_TABLE L"RedirectTable"
#define REGDW_REDIR_ENABLE L"RedirectEnable"




#define CCFL_MODEM_NUMBER         0x00000002 // ���� ��������� ������ ������
#define CCFL_REDIR_TABLE          0x00000004 // ���� ��������� ������� ���������������

#define REG_MODEM_NUMBER          L"ModemNumber"

#define MNF_MODEM                  0x40000000 // ����� ����������� ������
#define MNCODE_MODEM_NUMBER        1          // ��� �����������   � ����� ������ ������
                                              //

#define MNCODE_LINE_STATE          2          //��� �����������   � ���������/�������� �����
                                              //notify_data = LPMODEM_LINE_STATE

#define MNF_MODEMMER               0x20000000 // ����� ����������� � ��������  ����� ������
#define MNCODE_MODEMMER_RX         0
#define MNCODE_MODEMMER_TX         1
#define MNCODE_MODEMMER_REDIRECT   2
#define MNCODE_MODEMMER_ROUTE      3

#define MNCODE_FA_HANDLER          0x10

#define MNF_MODEMMER_REDIRECT      0x10000000 // ����� ����������� � ��������  ����� ������

#define MNCODE_REDIRECT_ADD        1        //��� ����������� �� ���������� ������ �����������
#define MNCODE_REDIRECT_REMOVE     2        //��� ����������� �� ��������   ������ �����������



#pragma pack(push,1)


/* ��������� �/� ������ */
#define MODEM_LINE_STATE_EXIST          0x0000001  // ����� ����������
#define MODEM_LINE_STATE_WORKED         0x0000002  // ����� �������
#define MODEM_LINE_STATE_CONNECT        0x0000004  // ���������� ����
#define MODEM_LINE_STATE_RXDATA         0x0000008  // ���� ������ ��� �����
#define MODEM_LINE_STATE_TXREADY        0x0000010  // ����������  � ��������
#define MODEM_LINE_STATE_CONNECTING     0x0000100  // � �������� ����������
#define MODEM_LINE_STATE_DISCONNECTING  0x0000200  // � �������� ������������


/*������� ������������ �������� �� ��������� ��������� ������ ����� ������*/
#define MODEMCM_LINE_STATE_CHANGE        MCMD(MT_MODEM,1)
// p1 = DWORD MODEM_ADDR, p2  = LineState;

typedef struct _MODEM_LINE_STATE
{
 DWORD      dw_size;
 MODEM_ADDR addr;
 DWORD      line_state;
 DWORD      state_changes;
}MODEM_LINE_STATE,*LPMODEM_LINE_STATE;

#define MPF_DATA_SOURCE     1 // ���� ������� ��������� ������ ��������������� ��������� ��� ����������� ���������
#define MPF_DATA_ACCEPTOR   2 // ����������� ������ ��������������� ��� �������� ������� ��������

typedef struct _MODEM_PROPS
{
 DWORD      dw_size;
 MODEM_ADDR addr;
 DWORD      lines_count      ;   // ���������� ����� �����
 DWORD      lines_limit      ;   // ������������ ���-�� �����
 DWORD      flags            ;   // �����
 DWORD      modem_info_len   ;   // �����
 DWORD      modem_info_size  ;
 wchar_t    modem_info[ANYSIZE_ARRAY];//������������� ������
}MODEM_PROPS,*LPMODEM_PROPS;

#define MODEMCM_GET_MODEM_PROPS     MCMD(MT_MODEM,2)
/*
 ��������� ������� ������
 p1 = LPMODEM_PROPS        p2 = DWORD buff_sz;
*/

typedef struct _MODEM_LINE_STATS
{
 DWORD dw_size;
 MODEM_ADDR      addr;
 DWORD           line_state;      //������� ��������� �����
 DWORD           connect_lost;    //���������� ������� �����
 DWORD           tx_bytes;        //�������� ����   c ������� ��������� �����
 DWORD           tx_kadrs;        //�������� ������ c ������� ��������� �����
 DWORD           tx_errors;       //������ ��� �������� c ������� ��������� �����
 DWORD           tx_total_bytes;  //�������� ���� �����
 DWORD           tx_total_kadrs;  //�������� ������ �����
 DWORD           tx_total_errors;  //������ ��� �������� �����
 DWORD           tx_quality;       //�������� ������  �� �������� * 100
 DWORD           tx_middle_speed;  //������� �������� ��������    * 100
 DWORD           tx_max_speed,tx_min_speed;
 DWORD           tx_queue_size;    //������ ���������� ������� �� ��������

 DWORD           rx_bytes;        //������� ����  c ������� ��������� �����
 DWORD           rx_kadrs;        //������� ������ c ������� ��������� �����
 DWORD           rx_errors;       //������ ��� ����� c ������� ��������� �����
 DWORD           rx_total_bytes;  //������� ����
 DWORD           rx_total_kadrs;  //������� ������
 DWORD           rx_total_errors; //������ ��� �����
 DWORD           rx_quality;      //�������� �����
 DWORD           rx_middle_speed; //������� �������� �����   * 100
 DWORD           rx_max_speed,rx_min_speed;
 LARGE_INTEGER   begin_connect_time;   //����� ����������� �����
 DWORD           modemmer_rx_kadrs;
 DWORD           modemmer_rx_bytes;
 DWORD           modemmer_tx_kadrs;
 DWORD           modemmer_tx_bytes;

}MODEM_LINE_STATS,*LPMODEM_LINE_STATS;

typedef struct _MODEM_LINE_PROPS
{
 DWORD      dw_size;
 MODEM_ADDR addr;
 DWORD      line_flags;
 DWORD      line_state;
 DWORD      tx_speed;
 DWORD      rx_speed;
 DWORD      max_frame_size;
 DWORD      line_text_sz;
 DWORD      line_text_len;
 wchar_t    line_text[ANYSIZE_ARRAY];
}MODEM_LINE_PROPS,*LPMODEM_LINE_PROPS;

#define   MLPF_RXABLE        1
#define   MLPF_TXABLE        2

#define   MLPF_DATASOURCE    4  // ���� ������� ��������� ������ ��������������� ��������� ��� ����������� ���������
#define   MLPF_DATAACCEPTOR  8  // ����������� ������ ��������������� ��� �������� ������� ��������


#define MODEMCM_GET_LINE_STATS MCMD(MT_MODEM,4)
/*
 ��������� ���������� �����
 DWORD ����� �����, p2 = LPMODEM_LINE_STATS
*/

#define MODEMCM_GET_LINE_PROPS MCMD(MT_MODEM,5)
/*
 ��������� ������� �����
 DWORD ����� �����, p2 = LPMODEM_LINE_PROPS
*/

#define MODEMCM_GET_LINE_STATE   MCMD(MT_MODEM,6)
/*
 ��������� ��������� �����
 DWORD ����� �����, p2 = LPMODEM_LINE_STATE

*/



#define MODEMCM_ENUM_LINES       MCMD(MT_MODEM,7)
/*������������ ����� ������
 p1 = DWORD idx
 p2 = LPMODEM_LINE_PROPS
*/

#define MODEMCM_GET_NUMBER       MCMD(MT_MODEM,10)
#define MODEMCM_SET_NUMBER       MCMD(MT_MODEM,11)
// ��������� ������ ������
// p1 = new_number, p2 = LPDWORD old_number;

#define MODEMCM_REFRESH_LINE     MCMD(MT_MODEM,20)
/* ���������� ������ �� ����� */
// p1 = ����� �����    ,p2 �������������� ��������


#define    MODEMCM_SEND       MCMD(MT_MODEM,100)
#define    MODEMCM_RECV       MCMD(MT_MODEM,101)
/*���� / �������� ������ ������
p1 = LPMPROTO_HEADER , p2 = DWORD sz
*/


//����� MPROTO_HEADER
#define  MPH_REDIRECTED   0x00000001


/****************  ������� ���������� ��������� *******************************/


#define MDMCM_GET_MODEM_HANDLE             MCMD(MT_MODEMMER,1)
/*�������� ����� ������*/
// p1 = DWORD modem_number

#define MDMCM_GET_MODEM_PROPS              MCMD(MT_MODEMMER,2)
/*���������  ������� ������ */
// p1 = LPMODEM_PROPS     , p2 = GKHANDLE

#define MDMCM_ENUM_MODEMS                  MCMD(MT_MODEMMER,3)
/* ������������ ������� */
// p1 = DWORD idx , p2 = LPMODEM_ADDR

#define MDMCM_ENUM_FREEMODEMS              MCMD(MT_MODEMMER,4)
// ������������ ��������� �������
// p1 = DWORD idx , p2 = LPGKHANDLE

#define MDMCM_ENUM_MODEM_LINES             MCMD(MT_MODEMMER,5)
/* ������������ ����� ������ */
// p1 = DWORD idx , p2 = LPMODEM_LINE_STATS  (MODEM_ADDR::modem - ����� ������)

#define MDMCM_GET_LINE_STATS               MCMD(MT_MODEMMER,6)
/*
 ��������� ��������� �����
 p1 = DWORD addr;p2 = LPMODEM_LINE_STATS
*/

#define MDMCM_GET_LINE_PROPS               MCMD(MT_MODEMMER,7)
/*
 ��������� ��������� �����
 p1 = DWORD addr;p2 = LPMODEM_LINE_PROPS
*/


#define MDMCM_CHANGE_MODEM_NUMBER          MCMD(MT_MODEMMER,10)
//p1 = LPMODEM_CHANGE_NUMBER, p=2 FALSE - do_call_modem, TRUE not_call_modem

typedef struct _MODEM_CHANGE_NUMBER
{
 DWORD dw_size;
 GKHANDLE modem;
 DWORD    new_number;
 DWORD    old_number;
}MODEM_CHANGE_NUMBER,*LPMODEM_CHANGE_NUMBER;

/*������� �������� ������ ��� ���������*/
#define MDMCM_SEND                 MCMD(MT_MODEMMER,50)
// p1 = LPPROTO_HEADER,  p2 = �������� ������� �������� 0,-1 �� �����
// ���������� id ����� ������������� �� ��������  ��������

#define MDMCM_RECV              MCMD(MT_MODEMMER,51)
/*
 ������� ������
 p1 =  LPMPROTO_HEADER, p2 = DWORD cbsz;
 ret = GHK_RET_SUCCES : GKH_RET_ERROR;
*/

/*���������������*/

typedef struct _REDIRECT_ENTRY
{
 DWORD         dw_size;
 MODEM_ADDR    from_addr;
 DWORD         fa;
 MODEM_ADDR    to_addr;
 DWORD         options;
}REDIRECT_ENTRY,*LPREDIRECT_ENTRY;

#define MDM_REDIR_DISABLED   0x80000000 // ��������� ���������������
#define MDM_REDIR_REDIRONLY  0x00000001 // �������� �� ���������� ����� �� ���������

#define MDMCM_ENABLE_REDIRECT MCMD(MT_MODEMMER,61)
//���������� ���������� ���������������
//p1 = BOOL enable

#define MDMCM_ISREDIRECT_ENABLED MCMD(MT_MODEMMER,62)
//��������  ����������/���������� ���������������
//p1 = LPBOOL enable_state


#define MDMCM_ENUM_REDIRENTRY  MCMD(MT_MODEMMER,63)
//������������ ��������� ���������������
// p1  = entry index  ,p2 = LPREDIRECT_ENTRY

#define MDMCM_REDIRENTRY_ADD     MCMD(MT_MODEMMER,64)
// ����������  ������ ���������������
// p1 = LPDWORD  entry_index ,p2 = LPREDIRECT_ENTRY  ,
#define MDMCM_REDIRENTRY_REMOVE  MCMD(MT_MODEMMER,65)
//�������� �������� ���������������
// p1  = entry index
#define MDMCM_REDIRENTRY_UPDATE  MCMD(MT_MODEMMER,66)
//��������� �������� ���������������
// p1  = entry index  ,p2 = LPREDIRECT_ENTRY

#define MDMCM_BEGIN_REDIRECT     MCMD(MT_MODEMMER,70)
/*������� ������������ ������ ��� ������ ���������������*/
//p1 == LPREDIRECT_ENTRY
#define MDMCM_END_REDIRECT       MCMD(MT_MODEMMER,71)
/*������� ������������ ������ ��� ��������� ���������������*/
//p1 == LPREDIRECT_ENTRY




/*��� ������� ����� ������������ ������ ��� ���������*/
#define MDMCM_FA_HANDLE             MCMD(MT_MODEMMER,100)

/* ������� ����������� ����������� */
#define MDMCM_REGISTER_HANDLER      MCMD(MT_MODEMMER,101)
// p1 = GKHANDLE handler, p2 = handle_fa;
// ret = MDMCM_FA_HANDLE  : GKH_RET_ERROR

/* ������� ������������� ����������� */
#define MDMCM_UNREGISTER_HANDLER    MCMD(MT_MODEMMER,102)
// p1 = GKHANDLE handler, p2 = handle_fa;


struct FA_HANDLER
{
  DWORD     dw_size;
  DWORD     fa;
  GKHANDLE  module;
  wchar_t   fa_text[64];
};

typedef FA_HANDLER*  LPFA_HANDLER;

#define MDMCM_ENUM_HANDLER    MCMD(MT_MODEMMER,105)
// p1 = index, p2 = LPFA_HANDLER;




/*������� ������� �������� �����*/
#define MDMCM_TXQUEUE_TIMER         MCMD(MT_MODEMMER,200)

//����������� / �������������  ���������� ����� ������ � 0
#define MDMCM_REG_INTERNAL_MLINE      MCMD(MT_MODEMMER,300)
// p1 = GKHANDLE p2 = -1 - ����������� � ���������  p2 >= 0 || <=250 ������ � �����������
// return - ����� ���������� ����� ���������� ������





#pragma pack(pop)




#endif // RC_INVOKED


#endif



