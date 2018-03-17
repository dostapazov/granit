//---------------------------------------------------------------------------

#ifndef GKReporterExH
#define GKReporterExH

#define MNF_REPORTEREX          0x40000000      //����� ����������� ���� ���������
#define MNCODE_REPORT_ADDED     0               //��������� ���������
#define MNCODE_REPORT_REFRESH   1               //���������� ���� ����
#define MNCODE_REPORT_NEWEVENT  2               //���������������� ����� �������


#pragma warn -8098
const DWORD Signature='FLKG';  //Granit Kernel Log File
#pragma warn .8098
const DWORD LogVersion=1;

//��������� ����� ����
typedef struct _TLogHeader
{
  DWORD size;           //������ ���������
  DWORD sig;            //������� �����
  DWORD version;        //������
  DWORD num_rec;        //���-�� �������
  DWORD file_size;      //������ ����� ��� ����������� �������, ���� 0 - ��� �����������
  DWORD first_rec;      //������ ������ ������
  DWORD last_rec;       //������ ��������� ������
} TLogHeader, *lplog_header;

//����� ���� �������� ������ ���������� ����� ����:
//struct _TMess
//{
//  DWORD length;
//  ...
//  DWORD length;
//}
//��� ���������������� ����������������� ������������

//���������� ������ ��������� ������ ����
typedef struct _TMess
{
  DWORD    length;         //������ ���������
  __int64  time;           //����� ������������� �������
  DWORD    type;           //��� �������
  DWORD    num_strings;    //���������� ����� � ���������� �����
  DWORD    strings_offset; //�������� �����
  DWORD    data_length;    //������ ���. ������
  DWORD    data_offset;    //�������� ���. ������
  //
  //����� �������:
  //
  //wchar_t strings[]   //��������, �������� ������� � ��.
  //LPVOID data[];      //���. ������
  //char pud[];         //���������� �� ������� DWORD
  //DWORD length;
} TMess;

//��� ������/������ ���� exception
typedef struct _exception_pointers
{
 EXCEPTION_RECORD except_rec;
 CONTEXT          except_ctx;
 DWORD            esp_dump[16];
 DWORD            ebp_dump[16];
}exception_pointers, *lpexception_pointers;


#endif
