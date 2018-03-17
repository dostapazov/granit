#ifndef _VTDCNT_INC_
#define _VTDCNT_INC_

#include <gkmodules.h>
#include <modemmer.h>
#include <otd.h>
#include "vtdproto.h"
#include "vtdcnt_err.h"

#pragma pack(push,1)

#define REGDW_VTDMOD_PUNUM       L"PuNumber"
#define REGSTR_VTDMOD_PUNAME     L"PuName"
#define REGSTR_VTDMOD_SERVERADDR L"ServerAddr"
#define REGDW_VTDMOD_SERVERPORT  L"ServerPort"
#define REGDB_VTDMOD_DEVICES     L"Devices" 

#define CCFL_PUNUMBER       0x80000000
#define CCFL_PUNAME         0x40000000
#define CCFL_SERVER_ADDR    0x20000000
#define CCFL_SERVER_PORT    0x10000000

#define CCFL_DEVICES        0x08000000

#define CCFL_DEV_NUM        0x04000000
#define CCFL_DEV_TYPE       0x02000000
#define CCFL_DEV_NAME       0x01000000

#define CCFL_GRP_NUM          0x00800000
#define CCFL_GRP_PARAMS       0x00400000
#define CCFL_GRP_QUERY_PERIOD 0x00200000


#define VTDNF_TXRX       0x00000100  // ���� ����������� � ������/��������

#define VTDNC_TX         0x00000001  //��� ����������� � ��������
#define VTDNC_RX         0x00000002  //��� ����������� � ������

#define VTDNC_RXKADR_STD 0x00000003  //��� ����������� � ������ �����
#define VTDNC_RXKADR_VG  0x00000004  //��� ����������� � ������ �����
#define VTDNC_RXKADR_Y   0x00000005  //��� ����������� � ������ �����

#define VTDNC_TXKADR_STD 0x00000006  //��� ����������� � �������� �����
#define VTDNC_TXKADR_VG  0x00000007  //��� ����������� � �������� �����
#define VTDNC_TXKADR_Y   0x00000008  //��� ����������� � �������� �����


#define VTDNF_DEV_CHANGED    0x00000200 //���� ����������� �� ��������� ������� �����
#define VTDNC_GROUP_CREATED  0x00000001
#define VTDNC_GROUP_DELETED  0x00000002
#define VTDNC_GROUP_UPDATED  0x00000003
#define VTDCNT_GROUP_DATA_CHANGED  0x00000004 //����������� �� ��������� ������ 




/*��������� ������*/
typedef struct _VTDCNTMOD_CONFIG
{
 DWORD   dw_size;
 DWORD   modem_number;
 DWORD   pu_number;
 wchar_t pu_name[128];     //��� ��
 wchar_t server_addr[128]; //����� �������
 int     server_port;      // ����� �����
}VTDCNTMOD_CONFIG,*LPVTDCNTMOD_CONFIG;






typedef struct _VTDDEV_PARAM
{
  DWORD     dw_size;
  DWORD     number; // ����� ����������, �� � ����� �����
  DWORD     type;
  wchar_t   name      [32];
}VTDDEV_PARAM,*LPVTDDEV_PARAM;


/*
 ������������ ���������
 p1 = LPVTDDEV_PARAM; p2 = idx
*/
#define VTDCMD_ENUM_DEVICE    MCMD(MT_MODEM_VTD,1000)


 // ��������� ���������
 // p1 = LPVTDDEV_PARAM; p2 = number;
#define VTDCMD_GET_DEVICE     MCMD(MT_MODEM_VTD,1001)

//�������� ����������
// p1 = LPVTDDEV_PARAM;

#define VTDCMD_CREATE_DEVICE  MCMD(MT_MODEM_VTD,1002)

//�������� ����������
// p1 = dev_number
#define VTDCMD_DELETE_DEVICE  MCMD(MT_MODEM_VTD,1003)

//��������� ����������  ����������
// p1 = LPVTDDEV_PARAM; p2 = mask;
#define VTDCMD_UPDATE_DEVICE  MCMD(MT_MODEM_VTD,1004)


//������������ ����� �� ����������
#define VTD_CURR_PARAM  0x10 // ������� ������� ����������
#define VTD_USER_PARAM  0x80 // ������� ���������� ������������
/*
  ������ ����� �������������� ���
  0x01 - 0x0A  - ��������� ����
  0x11 - 0x1A  - ������� ��������� ����
  0x81 - 0x8A  - ��������� ������������
  0x91 - 0x9A  - ������� ��������� ������������


*/

typedef struct _VTDDEV_GROUP
{
 DWORD dw_size;
 DWORD group_number; // ����� ������
 DWORD first;        // ��������� ��������
 DWORD count;        //���������� ���������� � ������
 DWORD query_period; //������ ������
}VTDDEV_GROUP,*LPVTDDEV_GROUP;

//������������ �����
// dev_num = LOWORD(p2),
// idx     = HIWORD(p2),
// LPVTDDEV_GROUP = p1
#define VTDCMD_ENUM_GROUPS   MCMD(MT_MODEM_VTD,1100)

//�������� ���������� � ������
// dev_num = LOWORD(p2),
// grp_num = HIWORD(p2),
// LPVTDDEV_GROUP = p1

#define VTDCMD_GET_GROUP     MCMD(MT_MODEM_VTD,1101)

//������� ������
//dev_num = (p2), LPVTDDEV_GROUP = p1
#define VTDCMD_CREATE_GROUP  MCMD(MT_MODEM_VTD,1102)

//������� ������  dev_num = p1, grp_num = p2
#define VTDCMD_DELETE_GROUP  MCMD(MT_MODEM_VTD,1103)

//�������� ������
//dev_num = LOWORD(p2)
//grp_num = HIWORD(p2)
//LPVTDDEV_GROUP = p1
#define VTDCMD_UPDATE_GROUP  MCMD(MT_MODEM_VTD,1104)

/*
 ��������� ���� ������� ���������� � �������� ���������� ������������
 p1 = dev_num, p2 ==  lpvtd_num_date_time ��� lpvtd_base_config
*/

#define VTDCMD_GET_NUMDATE_TIME  MCMD(MT_MODEM_VTD,2000)
#define VTDCMD_GET_BASE_CONFIG   MCMD(MT_MODEM_VTD,2001)

typedef struct _VTD_GET_GROUP_DATA
{
 DWORD dw_size;
 DWORD device;
 DWORD group;
 DWORD parts;
 DWORD data_len;
 BYTE  data[ANYSIZE_ARRAY];
}VTD_GET_GROUP_DATA,*LPVTD_GET_GROUP_DATA;
//��������� ������
//p1 = LPVTD_GET_GROUP_DATA;

#define VTDCMD_GET_GROUP_DATA         MCMD(MT_MODEM_VTD,2002)


//������� ������ �������
typedef struct _VTD_GET_HOUR_ARCHIVE
{
 DWORD dw_size;
 DWORD device;
 DWORD Npipe;
 DWORD NPar;
}VTD_GET_HOUR_ARCHIVE,*LPVTD_GET_HOUR_ARCHIVE;

#define VTDCMD_CALL_HOURS_ARCHIVE           MCMD(MT_MODEM_VTD,2100)
#define VTDCMD_CALL_DAYS_ARCHIVE           MCMD(MT_MODEM_VTD,2101)

#pragma pack(pop)


#endif

 