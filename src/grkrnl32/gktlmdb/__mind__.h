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
 dword       parts;   ������� ��������� ������
 [word        diag]      //�����������
 [otd_data    data]      //������ ������
 [otd_data    chmask]    //
 [otd_block]  std_param  //����������� ���������  ������
*/

#define OTD_PART_DIAG      0x0001
#define OTD_PART_DATA      0x0002
#define OTD_PART_CHMASK    0x0004
#define OTD_PART_STDPARAM  0x0008
#define OTD_PART_STATE     0x0010


#pragma pack(push,1)

typedef struct _otd_stdparam
{
 DWORD   hardware_type;     //��� ������������
 DWORD   options;           //
 DWORD   ctime_work;        //������ ������ ����������������
 DWORD   ctime_nowork;      //������ ������ ������������������
}otd_stdparam,*lpotd_stdparam;


typedef struct _otd_text /*����� ���������� RtlUnicodeString*/
{
 WORD size;
 WORD len;
 wchar_t text[ANYSIZE_ARRAY];
}otd_text,* lpotd_text;

#pragma pack(pop)

#endif
#endif

 