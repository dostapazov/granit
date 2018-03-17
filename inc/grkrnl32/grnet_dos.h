#ifndef _GRNET_DEF_INC
#define _GRNET_DEF_INC

#include <ke_defs.h>
#include <granit_dos.h>

#define GRNET_NAMES_LEN      16
#define DEF_POLINOM_DEL     64709

#ifndef GRNET_DATA_LEN
 #define GRNET_DATA_LEN 256
#endif


const long NetSecret = 0x5850494EuL;    //NIPX


 const long
          CmConnect          =  1,
          CmDisconnect       =  2,
          CmData             =  3,
          CmRetranslation    =  5, //�������� ����������� .������ (TKadr)
          CmBeginTU          =  6, //������ ����������
          CmResetTU          =  7, //������ ����������
          CmQueryTU          =  8, //������ ������� �� ������ ����������
          CmTuFalse          =  9, //���������� ��
          CmQuery            =  10, //������ ������ �� ���� ����
          CmTuOn             =  11, // �� - ��������
          CmTuOff            =  12,
          CmTrNorm           =  13,
          CmTuCancel         =  14,
          CmEnableTU         =  15,
          CmDisableTU        =  16,
          CmErrorTU          =  17,
          CmActivateTR       =  18,
          CmQueryCp          =  20, //����� �� ��
	  CmQueryFa          =  21, //����� �� ��
	  CmQueryGroup       =  22, //����� �� ������

          CmIdle             = 0x100,

          CmTxFile           = 0x200,//������ �������� �����
          CmTxFileChunk      = 0x201,//��������� �����
          CmTxFileDone       = 0x202,//���������� �����
          CmRxFileDone       = 0x203,//���������� �����
          CmCantTxFile       = 0x204,//�� �������� �������� ����
          CmErrorTxFile      = 0x205,//������ ��� �������� �����
          CmResumeTxFile     = 0x206,//�������������� ��������

          CmGetAllRetro      = 0x400,//������ �� �������� �����
          CmGetAllRetroOK    = 0x401,//�������� ����� ��������
          CmGetAllRetroNone  = 0x402,//����� �� �������
          CmGetAllRetroBad   = 0x403,//���������� � ������ ������ �������� ��


          CmModemOff         = 45000, //����� ������� �����


          CmInformation      = 0xFFF0,
          CmError            = 0xFFFA,
          CmServerNotFound   = 0xFFFB,
          CmTestString       = 0xFFFC,

          CmCheckConnect     = 0xFFFD,
          CmOutputQueueEmpty = 0xFFFE //������� �� �������� �����
          ;


#define    CONNECT_NO_ERROR      0
#define    CONNECT_SPACE_ERROR   1  //��� ����� � ������
#define    CONNECT_LEVEL_ERROR   2  //����� ������� �������� �� ��������������
#define    CONNECT_DENY_ERROR    3  //������ ������� � ����������

#define    GRNET_FLAG_RECEIPT     0x00000001
#define    GRNET_FLAG_NEEDRECEIPT 0x00010000

#pragma pack (push,1)

typedef struct _GRNET_KADR
{
  DWORD  secret_dword;
  WORD   receipt;
  WORD   need_receipt;
  DWORD  command;
  WORD   id;
  WORD   kadr_num;
  WORD   retry_num;
  WORD   data_len;
  DWORD  check_sum;
  BYTE   data[ANYSIZE_ARRAY];
}GRNET_KADR,*LPGRNET_KADR;

typedef struct _GRNET_BEGIN_CONNECTION
{
 char server_name[GRNET_NAMES_LEN]; // ..
 char client_name[GRNET_NAMES_LEN]; // ..
 WORD client_level;                 // .. ��������������� ��������
 /*************************/
 WORD ID;                           // ..
 WORD error_connection;             // ..��������������� ��������
}GRNET_BEGIN_CONNECTION ,*LPGRNET_BEGIN_CONNECTION;


#pragma pack (pop)

void   __fastcall grnet_protect  (LPGRNET_KADR kadr);
BOOL   __fastcall grnet_check    (LPGRNET_KADR kadr);
BOOL   __fastcall grnet_init_kadr(LPGRNET_KADR kadr,DWORD cmd,WORD kadr_num,WORD id,BOOL need_receipt,LPVOID data,DWORD data_len);
inline
DWORD  __fastcall grnet_kadr_len(LPGRNET_KADR kadr)
{ return kadr->data_len+sizeof(GRNET_KADR)-sizeof(kadr->data);}


#endif

 