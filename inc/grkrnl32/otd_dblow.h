/*-----------------------------------------------------------------------------
 11.02.2004 9:49:11 
 ������:   otd_dblow
 �����:    ��������� �.�.
 ����������: ����������� ��� ��� �� - ������� ������
 
-----------------------------------------------------------------------------*/




#ifndef OTD_DB_H
#define OTD_DB_H 1

#include <ke_defs.h>
#include <otd_base.h>
#include <otd_diag.h>

#if OTD_DB_H == 1

/*
 ������(c�����) ������� �� ��������� ������:

  dword       address //� �/c , � ������//
  word        parts;   ������� ��������� ������
 [word        version]
 [word        diag]      //�����������
 [FILETIME    timestamp]
 [otd_data    data]      //������ ������
 [otd_data    chmask]    //
 [otd_block   stdparam  //����������� ���������  ������
 [otd_block   state]
 [otd_text    name ]
 word kpk           //����������� �����
*/

#define OTD_PART_VERSION     0x0001
#define OTD_PART_EXTCMD      0x0002
#define OTD_PART_DIAG        0x0004 //����������
#define OTD_PART_TIMESTAMP   0x0008
#define OTD_PART_DATA        0x0010 //������ otd_data
#define OTD_PART_CHMASK      0x0020 //����� ���������
#define OTD_PART_STDPARAM    0x0040 //����������� ���������
#define OTD_PART_STATE       0x0080 //���������
#define OTD_PART_NAME        0x0100 //��� otd_text;
#define OTD_PART_EXTCMD_DATA 0x0200 //������ ���������� �������


#pragma pack(push,1)

typedef struct _otd_stdparam
{
 DWORD   hardware_type;     //��� ������������
 DWORD   options;           //
 DWORD   ctime_work;        //������ ������ ����������������
 DWORD   ctime_nowork;      //������ ������ ������������������
}otd_stdparam,*lpotd_stdparam;

typedef struct _otd_state
{
 DWORD state_mask;
 WORD  query_count;//���������� �������
 WORD  nc_count;   //������ ������ ��� ���������
}otd_state,*lpotd_state;

//��������� ������������ ���������
typedef struct _otd_proto_parsed
{
 lpotd_addr    addr;
 LPWORD        parts;
 LPWORD        version;
 LPWORD        ext_cmd;
 LPWORD        diag;
 LPFILETIME    time_stamp;
 lpotd_data    data;
 lpotd_data    chmask;
 lpotd_block   param;
 lpotd_block   state;
 lpotd_text    name;
 LPWORD        kpk;
};


#pragma pack(pop)

#endif
#endif

 