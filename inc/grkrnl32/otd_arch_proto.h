#ifndef OTD_ARCH_PROTO_H
#define OTD_ARCH_PROTO_H
#ifdef __linux__
#include <gklib/otd_proto.h>
#else
#include <otd_proto.h>
#endif

/*
  �������� ������ � OTD ��������
  � MODEM_PROTO  fa = FA_OTD_ARCHIVE
  �������� ������ �����-�� ��� � otd_proto;
  ������ ����������� ����� otd_proto::extension;
*/

#define OTD_ARCH_FLAG_RESPOND            0x80000000
#define OTD_ARCH_FLAG_ERROR              0x40000000
#define OTD_ARCH_FLAG_BEGIN              0x20000000
#define OTD_ARCH_FLAG_BEGIN_RESPOND      OTD_ARCH_FLAG_RESPOND|OTD_ARCH_FLAG_BEGIN
#define OTD_ARCH_FLAG_END                0x10000000
#define OTD_ARCH_FLAG_END_RESPOND        OTD_ARCH_FLAG_RESPOND|OTD_ARCH_FLAG_END
#define OTD_ARCH_CMD_MASK                0x00FFFFFF

#define OTD_ARCH_CMD(x)                  (x&OTD_ARCH_CMD_MASK)
#define OTD_ARCH_CMD_IS_RESPOND(x)       (x&OTD_ARCH_FLAG_RESPOND)
#define OTD_ARCH_CMD_IS_BEGIN(x)         (x&OTD_ARCH_FLAG_BEGIN)
#define OTD_ARCH_CMD_IS_END(x)           (x&OTD_ARCH_FLAG_END)
#define OTD_ARCH_CMD_IS_BEGINRESPOND(x)  (((x&(OTD_ARCH_FLAG_BEGIN_RESPOND))==(OTD_ARCH_FLAG_BEGIN_RESPOND)) ? true : false)
#define OTD_ARCH_CMD_IS_ENDRESPOND(x)    (((x&( OTD_ARCH_FLAG_END_RESPOND ))==( OTD_ARCH_FLAG_END_RESPOND )) ? true : false)
#define OTD_ARCH_CMD_IS_ERROR(x)         (x&OTD_ARCH_FLAG_ERROR)

#define OTD_ARCH_CMD_ALARM_ARCHIEVES_LIST                0x01 // ������ ������ ������� ������
#define OTD_ARCH_CMD_ALARM_ARCHIEVES_GET                 0x02 // �������� ������ ������
#define OTD_ARCH_CMD_ALARM_ARCHIEVES_GET_AND_ERASE       0x03 // �������� � ������� ������ ������
#define OTD_ARCH_CMD_ALARM_ARCHIEVES_ERASE               0x04 // ������� ������
#define OTD_ARCH_CMD_ALARM_ARCHIEVES_CREATED             0x05 // �������� ����� ����� ������


#define OTD_ARCH_ERROR_BUSY      0x0001        //�����
#define OTD_ARCH_NO_ARCHIVE      0x0002        //��� ������ ������

#pragma pack(push,1)

struct OTD_ARCH_HEADER
{
 DWORD cmd;
 DWORD data_size;
 DWORD data[ANYSIZE_ARRAY];
};

typedef OTD_ARCH_HEADER* LPOTD_ARCH_HEADER;


struct OTD_ARCH_LIST
{
  DWORD   count;
  QWORD   archive[ANYSIZE_ARRAY];
};

typedef OTD_ARCH_LIST * LPOTD_ARCH_LIST;

#pragma pack(pop)

#ifdef __cplusplus
extern "C"
{
#endif

int   WINAPI otd_arch_proto_format(LPBYTE buf, DWORD sz,DWORD pu,DWORD cp, DWORD cmd, DWORD data_size, LPVOID data, DWORD error, LPDWORD need_sz = NULL, lpotd_proto op = NULL);
DWORD WINAPI otd_arch_proto_get_error(lpotd_proto op);
int   WINAPI otd_arch_proto_format_list(LPBYTE buf,DWORD sz,DWORD pu,DWORD cp,DWORD cmd,DWORD count,LPQWORD list,DWORD error,LPDWORD need_sz = NULL,lpotd_proto op = NULL);

#ifdef __cplusplus
}
#endif











#endif // OTD_ARCH_PROTO_H
