#ifndef GKREPORTER_HPP
#define GKREPORTER_HPP

#include <gkmodules.h>


//��� ��������� ��� ������������� ���. ������ ������ ���� � ���� ������
//Data - ������� DWORD (������ ������), ����� ���� ������
//str - ����� ��� ��������������� � ����� ������
//sz - ������ ������
//return - ����� ������+1 (���� ������ ������ ��� - �������� �������������
typedef int (WINAPI * ReportDataToStrA)(LPVOID Data, char *str, DWORD sz, int Code);
typedef int (WINAPI * ReportDataToStrW)(LPVOID Data, wchar_t *str, DWORD sz, int Code);

#pragma pack(push,1)

/* ��� ������ REPORTER'a */
#define REPORTER_NAME_W L"STD_GKREPORTER"
#define REPORTER_NAME_A  "STD_GKREPORTER"

/* ��� ������ �������� �������� */
#define REPORTEREX_NAME_W L"EXT_GKREPORTER"
#define REPORTEREX_NAME_A  "EXT_GKREPORTER"

//������� ���������
//p1 = (char*) str1
//p2 = (char*) str2
#define  RCM_MESSAGE_A     MCMD(MT_REPORTER,1)
//������� ��������� UNICODE
//p1 = (wchar_t*) str1
//p2 = (wchar_t*) str2
#define  RCM_MESSAGE_W      MCMD(MT_REPORTER,2)

//����������
//p1 LPEXCEPTION_POINTERS,
//p2 = char * text
#define  RCM_EXCEPTION      MCMD(MT_REPORTER,3)

//p1 = lpexcept_record,
//p2 = char * text
#define  RCM_GKH_EXCEPTION  MCMD(MT_REPORTER,4)

/* ����������� ���� ��������� */
// p1 = char * msg_type_name (������� �������� - ��� �����)
// p2 = char * ������ ��������
//ret - (UINT) ����� �������
#define RCM_REGISTER_MSG_TYPE_A MCMD(MT_REPORTER,5)

/* ����������� ���� ��������� */
// p1 = wchar_t * msg_type_name (������� �������� - ��� �����)
// p2 = wchar_t * ������ ��������
//ret - (UINT) ����� �������
#define RCM_REGISTER_MSG_TYPE_W MCMD(MT_REPORTER,6)

//���������� ����� ������� ��������� ���. ������ ������� ����
//p1 = UINT - ������������������ ��� ���������
//p2 = ����� �������
#define RCM_SET_REGISTER_FUNC   MCMD(MT_REPORTER,7)

//�������� ����� ������� ��������� ���. ������ ������� ���� �� ������ ��� �����
//p1 = UINT - ������������������ ��� ���������
//p2 = char * - ���� p1==-1, msg_type_name (������� �������� - ��� �����)
//ret = ����� �������
#define RCM_GET_REGISTER_FUNC   MCMD(MT_REPORTER,8)

//�������� ��� ��������� �� ������
//p1 = UIN - ����� ���������
//p2 = TEventNameA *
#define RCM_GET_MSG_NAME_A      MCMD(MT_REPORTER,9)

//�������� ��� ��������� �� ������
//p1 = UIN - ����� ���������
//p2 = TEventNameW *
#define RCM_GET_MSG_NAME_W      MCMD(MT_REPORTER,10)

//�������� ���� ����
//p1 = UINT - ������������������ ��� ���������
#define RCM_CLEAR_EVENT         MCMD(MT_REPORTER,11)

//��������� ����� ���������
//p1 = TReportMes *
//p2 = 0
#define  RCM_MESSAGE_EVENT      MCMD(MT_REPORTER,12)

//���� �������
#define REPORT_SUCCESS_TYPE     0       //�����
#define REPORT_INFORMATION_TYPE 1       //�����������
#define REPORT_WARNING_TYPE     2       //��������������
#define REPORT_ERROR_TYPE       4       //������

//�������� ���������
//p1 = bool       0- �����, 1- ����������
//p2 = bool       0- ���������, 1- ��������
#define  RCM_MESSAGE_SOUND      MCMD(MT_REPORTER,13)

//������ ������������ ������ ���-�����
//p1 = UINT   - ������������������ ��� ���������
//p2 = DWORD  - ������ �����, 0 - ������������
#define  RCM_MESSAGE_FILESIZE   MCMD(MT_REPORTER,14)

struct TReportMes
{
  UINT log;                     //��� ����� ����
  const wchar_t *desc;          //�������� �������
  DWORD type;                   //��� �������
  GKHANDLE source;              //�������� �������
  DWORD data_size;              //������ ���. ������
  DWORD data[ANYSIZE_ARRAY];    //���. ������
  #ifdef __cplusplus
  TReportMes(){ZeroMemory(this,sizeof(*this));}
  #endif
};

struct TEventNameA
{
  char *s_short, *s_full;
  int sz_short, sz_full;        //������ �����, ���� ��� ������������ ������������� �����
};

struct TEventNameW
{
  wchar_t *s_short, *s_full;
  int sz_short, sz_full;        //������ �����, ���� ��� ������������ ������������� �����
};

//����������� ���� �������
#define EVENT_SYSTEM          0 //"System" - ��������� ������� (������, ������� ������ � �.�.)
#define EVENT_EXCEPTIONS      1 //"Exceptions" - ��� ����������

#pragma pack(pop)

#endif


