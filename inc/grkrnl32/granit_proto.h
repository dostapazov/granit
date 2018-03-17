/*����������� ��������� ������*/

#ifndef _GRANIT_PROTO_INC_
#define _GRANIT_PROTO_INC_
#ifdef __linux__
#include <lin_ke_defs.h>
#else
#include <ke_defs.h>
#endif

#pragma pack(push,1)

#define GRSC_PROTO_VERSION    0x00010000
#define GRSC_MAX_FRAME_SIZE   0x15
#define GRSC_MIN_FRAME_SIZE   0x04

/*
                +----------------------+---------------------------------+------+
                �   ������       0101  � ����� �� ���������������        �      �
                �   ���������    0000  � ��������                        ��� ��̦
                �   �����        0001  �                       ��   1000 �      �
                �   �����        0011  �                      ���   1001 �      �
                �                      �                      ���   1010 �      �
                L----------------------+---------------------------------+------+
                �   ������       0100  � ����� ��������������� ��������� �      �
                �   ��� �����.   1001  � �����                 ���� ������ ��� �
                �   ������       1010  �                       ������ �  �      �
                �   ����������   1000  �                       ��������  �      �
                �   ���������    0000  �                                 �      �
                +----------------------+---------------------------------+------+
*/

//���� �������
#define GRKT_RECEIPT     0x00  // ���������
#define GRKT_CALL        0x01  // �����
#define GRKT_QUERY       0x03  // �����
#define GRKT_DATAIN      0x04  // ������ � ���
#define GRKT_DATAOUT     0x05  // ������ �� ���
#define GRKT_READY       0x08  // ����������  �� , ���
#define GRKT_NODATA      0x09  // ��� ����������
#define GRKT_ERROR       0x0A  // ������ ����������� ��,

//��� ������
#define GRINF_DISCONNECT 0x00   // ����� ����� � �� - ??????????
#define GRINF_TS         0x02   // ���������� ��
#define GRINF_TII        0x04   // ���������� ���
#define GRINF_TIT        0x06   // ���������� ���
#define GRINF_ERRFL      0x08   // ������� ������

#define GRINF_TUTR       0xF0   //�������������� ������� ����� ��/��
                                //(��������� ������ ��� ����������� �������������
                                // ��� ����������� ���� ������� ������ ��



#define GRKTAFB_RESERV1  0x00
#define GRKTAFB_TUON     0x01   //��������
#define GRKTAFB_TUOFF    0x02   //���������
#define GRKTAFB_TRMORE   0x03   //������ (�������������)
#define GRKTAFB_TRLESS   0x04   //������ (�������������)
#define GRKTAFB_CMD1     0x05   //������� ������� 1
#define GRKTAFB_CMD2     0x06   //������� ������� 2
#define GRKTAFB_RESERV2  0x07
#define GRKTAFB_CALLTS   0x08   //�����
#define GRKTAFB_CALLTIT  0x09
#define GRKTAFB_CALLTII  0x0A
#define GRKTAFB_TRCANCEL 0x0E   //������ (�������������)

#define GRPROTO_MKTYPE(type,afb) (BYTE)(((type<<4)&0xF0)|(afb&0x0F))
#define GRPROTO_MKINF(inf,grp)   (BYTE)(((inf<<4)&0xF0)|(grp&0x0F))

/*������������ ������ �� ��  ������ ����� � � ��*/
#define GRPROTO_MK_CPNUM(floor,lu)    ((BYTE)((floor<<4)&0xF0)|(lu&0x0F))
/*��������� ������ ����� �� ������ ��*/
#define GRPROTO_GET_FLOOR_NUM(cp)     ((cp>>4)&0x0F)
/*��������� ������ �� �� ������ ��*/
#define GRPROTO_GET_LU_NUM(cp)        (cp&0x0F)



typedef union  _gr_proto_cp
{
 struct
 {
   BYTE line :4; //����� ��
   BYTE floor:4; //����� �����
 };
 BYTE cp_number;//����� ��
}gr_proto_cp,*lpgr_proto_cp;

typedef union  _gr_proto_type
{
 struct{
   BYTE afb   :4; // ����� ��������������� �����
   BYTE ktype :4; // ��� �����  , ���� ��� ����� GRKT_CALL �� AFB - ��������� GRKTAFB_XXX
   };
 BYTE afb_type;
}gr_proto_type,*lpgr_proto_type;

typedef union _gr_proto_info
{
 struct{
   BYTE grp :4; // ����� ������
   BYTE inf :4; // ��� ������
       };
   BYTE grp_inf;
}gr_proto_info,*lpgr_proto_info;


/*��������� ����������� �� ����� �����*/
typedef struct   _gr_proto
{
 DWORD            dw_size;
 DWORD            kadr_len;
 lpgr_proto_cp    cp;
 lpgr_proto_type  type;
 lpgr_proto_info  info;
 DWORD            data_len;
 LPBYTE           data;
 LPWORD           kpk;
}gr_proto,*lpgr_proto;

typedef struct _gr_proto_data_discrete
{
 BYTE   values [8];
 BYTE   inverse[8];
}gr_proto_data_discrete,*lpgr_proto_data_discrete;

typedef struct _gr_proto_data_analog
{
 BYTE values[16];
}gr_proto_data_analog,*lpgr_proto_data_analog;

typedef struct _gr_proto_data_tu
{
 BYTE   pc_group    [2];
 BYTE   pc_object   [1];
 BYTE   pc_invafb   [2];
 BYTE   pc_invgroup [2];
 BYTE   pc_invobject[1];
}gr_proto_data_tu,*lpgr_proto_data_tu;

/*��������� ������ �� ��� ��*/
typedef struct _gr_proto_out_ts
{
 BYTE floor;
 BYTE type_afb;
 BYTE bits    [8];
 BYTE inv_bits[8];
 WORD kpk;
}gr_proto_out_ts,*lpgr_proto_out_ts;

typedef struct _gr_proto_out_ti
{
 BYTE floor;
 BYTE type_afb;
 BYTE reserv;
 BYTE data[3];
 WORD kpk;
}gr_proto_out_ti,*lpgr_proto_out_ti;

typedef struct _gr_proto_out_ready
{
 BYTE floor;
 BYTE type_afb;
 BYTE ready_state  [4];
 BYTE ready_flash  [4];
 BYTE reserv       [4];
 BYTE ready_slaking[4];
 WORD kpk;

}gr_proto_out_ready,*lpgr_proto_out_ready;


#pragma pack(pop)

/*������ ����� �� ��������� �����*/
#define GRPP_NOERROR             0
#define GRPP_ERR_STRUCT_SIZE    (-1)  //��������� ������ ���������
#define GRPP_ERR_INVALID_BUFFER (-2)  //������������ ������� ������
#define GRPP_ZERO_KADR          (-3)  //���� � �������� �������
#define GRPP_ERR_KPK            (-4)  //������ ���
#define GRPP_ERR_INVALID_TYPE   (-5)  //������������ ���
#define GRPP_ERR_INVALID_INF    (-6)  //����������� ��� ������
#define GRPP_ERR_INVPART        (-7)  //������������ ������ � ��������� �����
#define GRPP_MAX_ERROR           8

#ifdef __cplusplus
extern "C"{
#endif
DWORD  WINAPI    gr_proto_parser      (LPBYTE kadr,DWORD kadr_len,lpgr_proto gp);
DWORD  WINAPI    gr_proto_format      (BYTE   cp,BYTE type,BYTE info,LPBYTE buff,DWORD bsz,lpgr_proto gp);
DWORD  WINAPI    gr_proto_protect     (lpgr_proto gp);
DWORD  WINAPI    gr_proto_val2poscode (DWORD   value,LPBYTE pc,DWORD pc_size);
DWORD  WINAPI    gr_proto_poscode2val (LPBYTE pc,DWORD pc_size);
DWORD  WINAPI    gr_proto_fill_inverse(LPBYTE src,LPBYTE dst,DWORD len);
DWORD  WINAPI    gr_proto_tu          (BYTE   cp,BYTE tuop,BYTE tu_group,BYTE tu_obj,LPBYTE buff,DWORD bsz);
DWORD  WINAPI    gr_get_ts            (LPBYTE data,DWORD number);
void   WINAPI    gr_set_ts            (DWORD val,LPBYTE data,DWORD number);
#define gr_get_ti(data,number) (DWORD)(data[number])
#define gr_set_ti( val,data,number) data[number] = (BYTE)val

#define STD_GRANIT_POLINOM  0x1021
WORD   WINAPI   gr_calc_kpk(void  *buff  ,WORD len,WORD del);



inline DWORD WINAPI gr_proto_receipt(BYTE cp,BYTE afb,LPBYTE buf,DWORD bsz)
{
 DWORD ret = 0;
 gr_proto gp;gp.dw_size = sizeof(gp);
 if(gr_proto_format(cp,GRPROTO_MKTYPE(GRKT_RECEIPT,afb),0,buf,bsz,&gp) == GRPP_NOERROR)
 {
     gr_proto_protect(&gp);
     ret = gp.kadr_len;
 }
 return ret;
}

inline DWORD WINAPI gr_proto_callts(BYTE cp,LPBYTE buf,DWORD bsz)
{
 DWORD ret = 0;
 gr_proto gp;gp.dw_size = sizeof(gp);
 if(gr_proto_format(cp,GRPROTO_MKTYPE(GRKT_CALL,GRKTAFB_CALLTS),0,buf,bsz,&gp) == GRPP_NOERROR)
    {
     gr_proto_protect(&gp);
     ret = gp.kadr_len;
    }
 return ret;
}

inline DWORD WINAPI gr_proto_calltit(BYTE cp,LPBYTE buf,DWORD bsz)
{
 DWORD ret = 0;
 gr_proto gp;gp.dw_size = sizeof(gp);
 if(gr_proto_format(cp,GRPROTO_MKTYPE(GRKT_CALL,GRKTAFB_CALLTIT),0,buf,bsz,&gp) == GRPP_NOERROR)
    {
     gr_proto_protect(&gp);
     ret = gp.kadr_len;
    }
 return ret;
}

inline DWORD WINAPI gr_proto_calltii(BYTE cp,LPBYTE buf,DWORD bsz)
{
 DWORD ret = 0;
 gr_proto gp;gp.dw_size = sizeof(gp);
 if(gr_proto_format(cp,GRPROTO_MKTYPE(GRKT_CALL,GRKTAFB_CALLTII),0,buf,bsz,&gp) == GRPP_NOERROR)
    {
     gr_proto_protect(&gp);
     ret = gp.kadr_len;
    }
 return ret;
}





#ifdef __cplusplus
}//extern "C"{
#endif



#endif


