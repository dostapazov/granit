/*
 ����������� ������� ������� ���������  � ��.
*/

#ifndef _ADDRE_DEF_INC
#define _ADDRE_DEF_INC

#ifndef KE_DEFS_INC
#include <ke_defs.h>
#endif

/*
 ������������ ����������    ������ � ������
 ����� ������ ������������� ��������� �� �� ��� ����� �� ��������
*/

#pragma pack (push,1)

typedef union _BES_FA
{
 BYTE bfa;
 struct{
        BYTE   fa :6;
        BYTE   ver:2;
       };
}BES_FA,*LPBES_FA;

typedef union _BES_NKDR //����� �����
{
 BYTE n_kdr;
 struct
 {
  BYTE number:6;// ����� ����� 1-63
  BYTE repeat:2;// ���-�� ��������� �������
 };
}BES_NKDR,*LPBES_NKDR;



typedef struct _BES_HDR
{
 BES_FA      bfa;    // �������������� �����
 BYTE        bnch;   // ����� ������
 BYTE        b_ctrl; // ���� ����������
 BYTE        b_descr;// ���� ���������
 BYTE        b_fmc;  // ������������� ��������� check
 BES_NKDR    b_nkdr; // ����� �����
}BES_HDR,*LPBES_HDR;


//���� ����������
#define BCTRL_DONT_SAVE        0x80 //�� �������
#define BCTRL_ROUTE            0x40 //����������������
#define BCTRL_FIRST_CHANNEL    0x20 //������ �����
#define BCTRL_IMPORTANT        0x10 //�����
#define BCTRL_URGENT           0x08 //������
#define BCTRL_ALARM            0x04 //��������
#define BCTRL_CONFIDENTIAL     0x02 //���������������
#define BCTRL_DONT_REPEAT      0x01 //�� ���������

//���� ���������

#define BDEF_ADDR_TYPE_MASK    0xE0 // ����� �������� ��������� �����
#define BDEF_INC               0x10 // ���� ���������
#define BDEF_PACKET            0x08 // ����� ������
#define BDEF_IMMITATION        0x04 // ���������
#define BDEF_DIFA              0x02 // ������� ����� ����
#define BDEF_INDEX             0x01 // ������� ������� ������

//���� ������������� ��������� check

#define BFMC_PREV_CHANNEL      0x80 //���������� �����
#define BFMC_SELF_CHANNEL      0x40 //����������� �����
#define BFMC_VALUE_MASK        0x3F //��� FMC


//�������� ��������� ����

//��������� ����������
typedef struct _BES_ADDR_1
{
 BYTE n_st;   //����� �������
 BYTE n_mod;  //����� ������
}BES_ADDR_1,*LPBES_ADDR_1;

typedef struct _BES_ADDR_2
{
 BES_ADDR_1 dst;//��������
 BES_ADDR_1 src;//��������
}BES_ADDR_2,*LPBES_ADDR_2;

#pragma pack(pop)

#pragma pack(push,8)

//��������� ������� ������ ���������
typedef struct _BESPROTO_PARSED
{
 DWORD cb_sz;
 LPBES_HDR      bes_hdr;      //����� ���������
 DWORD          bes_hdr_size; //��� ������

 LPBES_ADDR_1   bes_addr_1;//�������� �����
 LPBES_ADDR_2   bes_addr_2;
 DWORD          bes_addr_size;//������

 LPVOID         bes_difa;
 DWORD          bes_difa_size;

 LPVOID         bes_index;
 DWORD          bes_index_size;
 LPWORD         hdr_kpk;

 LPBYTE         data;
 DWORD          data_size;
 LPWORD         kpk;
}BESPROTO_PARSED,*LPBESPROTO_PARSED;

#pragma pack(pop)


#ifdef __cplusplus
extern "C" {
#endif

    DWORD WINAPI besproto_parser(LPVOID buffer,DWORD b_sz,LPBESPROTO_PARSED bp ,DWORD cbsz);
    DWORD WINAPI besproto_maker (LPVOID buffer,DWORD b_sz,LPBESPROTO_PARSED  bp ,DWORD  cbsz,WORD pol_del);

#ifdef __cplusplus
} //extern "C" {
#endif

#endif

