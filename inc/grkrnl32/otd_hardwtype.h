/*-----------------------------------------------------------------------------
 11.02.2004 16:34:17 
 ������:   otd_hardwtype
 �����:    ��������� �.�.
 ����������: �������������� ���� ������������
 
-----------------------------------------------------------------------------*/



#ifndef OTD_HARDW_TYPE_H
#define OTD_HARDW_TYPE_H 1

#include <otd_base.h>

#pragma pack(push,1)

/*��������������� ���� ������������*/
#define OTD_HARDW_GRANIT   1  /*������*/
#define OTD_HARDW_TM512    2  /*��512*/
#define OTD_HARDW_VRTF3    3  /*����3*/



typedef LRESULT ( WINAPI * hardw_spec_callback)(LPVOID data,lpotd_block spec_block,LPARAM func);

typedef struct _otd_hardware_type
{
 WORD    hardware_type;//��� ������������
 wchar_t description[];//�������� ������������
 WORD    def_work_query_time;
 WORD    def_nowork_query_time;


}otd_hardware_type,*lpotd_hardware_type;

#pragma pack(pop)

#endif
 