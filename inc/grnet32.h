
//��������� ������������ ���������� GRNET32.DLL
#ifndef GRNET32_DLL_INC
#define GRNET32_DLL_INC

#include <winsock2.h>

#ifndef _Windows_
#ifndef STRICT
#define STRICT
#endif
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif
#include <wsipx.h>


#ifndef IPX_DATA_LEN
 #define IPX_DATA_LEN 256
#endif

#define RETRY_COUNT 4
#define FTP_FILE_NAME_LENGTH (IPX_DATA_LEN - 16)
#define GRNET_NAMES_LEN 16
#define DEF_POLINOM_DEL 64709

typedef HANDLE GRNET_OBJ;
enum tlmMode { psGroup = 1,psCtrlPoint,psSystem };
enum tlmType { tpTs,tpTit,tpTiiNull,tpTr,tpUr,tpAll,tpTii,tpTime,tpOf=11,tpConfig,tpReady=14,tpError = 15 };


#pragma pack (push,8)
 typedef   struct tagCLIENT_INFO
 {
  char   ClientName[GRNET_NAMES_LEN+1];
  char   ServerName[GRNET_NAMES_LEN+1];
  WORD   ClientId;
  SOCKADDR_IPX     Address;
  WORD   ConnectInfo;
  DWORD  WorkTime; //����� � ��������
 }CLIENT_INFO,* LPCLIENT_INFO;

 typedef struct tagCLIENT_STATS
 {
  DWORD WorkTime;
  DWORD KadrsSend;
  DWORD BytesSend;
  DWORD KadrsRecv;
  DWORD BytesRecv;
  DWORD ErrorSend;
  DWORD ErrorRecv;
  DWORD LostConnectCount;
 }CLIENT_STATS,*LPCLIENT_STATS;
#pragma pack (pop,8)


 #pragma pack(push,1)

  typedef struct _object_tu
  {
    WORD kp,sb,obj;
    WORD tlm_type;
    BYTE tu_group,tu_object,kop;
    WORD timeout,rivrio_timeout;
    WORD active;
    WORD timercounter,rivrio_timercounter;
    WORD select_timer;

    WORD reserved[2];
    BYTE v_mode;
    WORD remote;
  }object_tu,*lpobject_tu;

  typedef struct tagKadrHead
  {
    unsigned char len;    // ����� �������
    struct
    {
    unsigned char Channel  :      4;    // ����� ������
    unsigned char KadrNum  :      4;    // ����� �����
    }
    kadrInfo;

    unsigned char numCp;                // ����� ��

    struct
    {
    unsigned char InfoType :      4;    // ��� ���� ����������
    unsigned char P        :      1;    // ������� �������� ����������
    unsigned char Mode     :      3;    // �����
    }
    bF1;

    struct
    {
    unsigned char numGrp   :      7;    // ����� ������
    unsigned char D        :      1;    // ������� ������������� == 0
    }
    bF2;
    unsigned char S;                    // S == 0 -> ����� ����������
    unsigned char FirstByte;            // ������ ���� �������������� �����

  }TKadrHead,*lpTKadrHead;

  typedef struct tagTKadr
  {
    unsigned short len;
    union
    {
     TKadrHead header;
     unsigned char pos[128];
    };
  }TKadr,*lpTKadr;

 #pragma pack(pop)


 #ifdef __cplusplus
  extern "C"
  {
 #endif

 GRNET_OBJ WINAPI GRNET_CreateGranServer(LPCSTR name,WORD Socket,int adapter_num);
 GRNET_OBJ WINAPI GRNET_CreateGranClient(LPCSTR SName,WORD sSocket,LPCSTR CName,WORD cSocket,int adapter_num);
 BOOL      WINAPI GRNET_DestroyObj (GRNET_OBJ);
 BOOL      WINAPI GRNET_Send(GRNET_OBJ,DWORD ClientId,WORD Command,LPVOID data,WORD length,BOOL NeedReceipt,BOOL First);
 BOOL      WINAPI GRNET_Recv(GRNET_OBJ,LPDWORD From,LPDWORD Command,LPVOID data,DWORD cbSize,LPDWORD rdBytes,LPDWORD Remain);
 BOOL      WINAPI GRNET_Start(GRNET_OBJ,LPCSTR sName,WORD socket);
 BOOL      WINAPI GRNET_Stop(GRNET_OBJ);
 HANDLE    WINAPI GRNET_GetRxEvent(GRNET_OBJ);
 HANDLE    WINAPI GRNET_GetRestartEvent(GRNET_OBJ);
 BOOL      WINAPI GRNET_IsClient(GRNET_OBJ);
 BOOL      WINAPI GRNET_IsServer(GRNET_OBJ);
 BOOL      WINAPI GRNET_Restart (GRNET_OBJ);
 DWORD     WINAPI GRNET_GetWSAError(GRNET_OBJ);
 BOOL      WINAPI GRNET_GetName(GRNET_OBJ,LPSTR out,DWORD cbSize);
 WORD      WINAPI GRNET_GetIpxSocket(GRNET_OBJ);

 BOOL      WINAPI GRNET_GetServerName     (GRNET_OBJ,LPSTR out,DWORD cbSize);
 WORD      WINAPI GRNET_GetServerIpxSocket(GRNET_OBJ);

 typedef   BOOL   (CALLBACK * GRNET_EnumClientsProc)(LPVOID UserData,LPCLIENT_INFO);
 void      WINAPI GRNET_EnumClients(GRNET_OBJ,GRNET_EnumClientsProc,LPVOID UserData);

 DWORD    WINAPI GRNET_GetWaitReceiptTimeout(GRNET_OBJ, DWORD ClId);
 BOOL     WINAPI GRNET_SetWaitReceiptTimeout(GRNET_OBJ, DWORD ClId, DWORD to);
 DWORD    WINAPI GRNET_GetSendRetryCount    (GRNET_OBJ,  DWORD ClId);
 BOOL     WINAPI GRNET_SetSendRetryCount    (GRNET_OBJ,  DWORD ClId, DWORD Count);
 BOOL     WINAPI GRNET_EnableReconnect      (GRNET_OBJ obj,BOOL enable);
 BOOL     WINAPI GRNET_DisconnectAllClients (GRNET_OBJ obj); 

 #ifdef __cplusplus
  }
 #endif



         //�������������� ����
         const unsigned long  infoConnection    = 1
                    ;

         //�������� �������������
         const int  dcConnected      = 0,//�����������
                    dcSelfInitiate   = 1,//���������� �� ����������� ����������
                    dcLostConnection = 2,//������ �����
                    dcServerOff      = 3,//���������� ������
                    dcServerNotFound = 4,//������ �� ������
                    dcErrorConnection= 5,//����� ������� � �����������
                    dcNotConnected   = 0x7F
                    ;
 const long
          CmConnect          =  1,
          CmDisconnect       =  2,
          CmData             =  3,
          CmRetranslation    =  5, //�������� ����������� .������ (TKadr)
          CmBeginTU          =  6, //������ ����������
          CmResetTU          =  7, //������ ����������
          CmQueryTU          =  8, //������ ������� �� ������ ����������
          CmTuFalse          =  9, //���������� ��
          CmDataQuery        = 10, //������ ������ �� ���� ����
          CmTuOn             = 11, // �� - ��������
          CmTuOff            = 12,
          CmTrNorm           = 13,
          CmTuCancel         = 14,
          CmEnableTU         = 15,
          CmDisableTU        = 16,
          CmErrorTU          = 17,
          CmActivateTR       = 18,
          CmQueryCp          = 20, //����� �� ��
	  CmQueryFa          = 21, //����� �� ��
	  CmQueryGroup       = 22, //����� �� ������

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



#define FILE_PACKED_RETRO  0


#endif

