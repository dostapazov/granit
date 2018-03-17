#ifndef _GRANIT_DOS_INC_
#define _GRANIT_DOS_INC_

#include <ke_defs.h>



#define   tpTs        0
#define   tpTit       1
#define   tpTiiNull   2
#define   tpTr        3
#define   tpUr        4
#define   tpAll       5
#define   tpTii       6
#define   tpTime      7
#define   tpOf        0x0B
#define   tpConfig    0x0C
#define   tpReady     0x0E
#define   tpError     0x0F


#pragma pack(push,1)

  typedef struct _object_tu
  {
    WORD kp,sb,obj;  //����� ��, �������, �������
    WORD tlm_type;   //�� ��� ���
    BYTE tu_group,tu_object,kop;  //������ ��/��, ����� �������, ��� ��������
    WORD timeout,rivrio_timeout;
    WORD active;
    WORD timercounter,rivrio_timercounter;
    WORD select_timer;
    WORD reserved[2];
    BYTE v_mode;
    WORD remote;
  }object_tu,*lpobject_tu;

  typedef struct gratony_kadr
  {
    unsigned short kadr_len; //����� �����
    unsigned char  len;    // ����� �������
    union{struct{
    unsigned char  channel  :       4;    // ����� ������
    unsigned char  kadr_num  :      4;    // ����� �����
          };unsigned char bf1;};

    unsigned char  numCp;                // ����� ��

    union{struct{
    unsigned char  InfoType :      4;    // ��� ���� ����������
    unsigned char  P        :      1;    // ������� �������� ����������
    unsigned char  Mode     :      3;    // �����
    };unsigned char bf2;};

    union{struct{
    unsigned char  numGrp   :      7;    // ����� ������
    unsigned char  D        :      1;    // ������� ������������� == 0
    };unsigned char  bf3;};
    
    unsigned char    S;                    // S == 0 -> ����� ����������
    unsigned char data[ANYSIZE_ARRAY];
  }gratony_kadr,*lp_gratony_kadr;



  //������� ����������� �� �������
  #define MAX_CP_COUNT     255
  #define GRDIAG_CP        0x8000
  #define GRDIAG_TIT_MASK  0x7F00
  #define GRDIAG_TS_MASK   0x00FF

  typedef struct _grdiag_item
  {
   WORD   state;
   WORD   mask;
  }grdiag_item,*lp_grdiag_item;

  typedef struct _grdiag_map
  {
   grdiag_item items[MAX_CP_COUNT];
  }grdiag_map,*lp_grdiag_map;

 #ifdef __cplusplus
  extern "C" {
 #endif

 inline void __fastcall gd_init_item(lp_grdiag_item gdi)
 {  gdi->state = -1;gdi->mask = 0; }

 WORD  __fastcall gd_get_mask(DWORD fa,DWORD grp);
 DWORD __fastcall gd_set_diag(lp_grdiag_item gdi,WORD diag);
 DWORD __fastcall gd_mark_exist(lp_grdiag_item gdi,DWORD fa,DWORD grp);
 DWORD __fastcall gd_set_nowork(lp_grdiag_item gdi,DWORD fa,DWORD grp,BOOL no_work);
 BOOL  __fastcall gd_is_exist (lp_grdiag_item gdi,DWORD fa,DWORD grp);
 BOOL  __fastcall gd_is_nowork(lp_grdiag_item gdi,DWORD fa,DWORD grp);
 DWORD __fastcall gd_get_otd_diag(lp_grdiag_item gdi,DWORD fa,DWORD grp);
 DWORD __fastcall gd_set_otd_diag(lp_grdiag_item gdi,DWORD fa,DWORD grp,DWORD otd_diag);
 

 

 #ifdef __cplusplus
 } //extern "C" {
 #endif

 


#pragma pack(pop)

#endif
