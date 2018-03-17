#ifndef _SCALE_INC_
#define _SCALE_INC_

#include <gkmodules.h>
#include <vector>

/* ��� ������ ��������� ���� */
#define SCALE_NAME_W L"SCALE"
#define SCALE_NAME_A  "SCALE"

#define SCALE_DATA_VERSION      2  //������ ��������� ������ � ��������� �����

//������
#define SCL_ERR_SUCCESS         0  //�������
#define SCL_ERR_NOKVANT         1  //����� ������� �� �������
#define SCL_ERR_NOVALUE         2  //�������� ������� �� ��a����
#define SCL_ERR_NOREVERSE       3  //��� �������� ������� ��� �������
#define SCL_ERR_MINWARN         4  //����� �� ������� ���. �����������������
#define SCL_ERR_MAXWARN         8  //����� �� ������� ����. �����������������
#define SCL_ERR_MINALARM        16 //����� �� ������� ���. ���������
#define SCL_ERR_MAXALARM        32 //����� �� ������� ����. ���������

//����� ������� �������
#define SCL_LIMIT_MINWARN       0x00000001
#define SCL_LIMIT_MAXWARN       0x00000002
#define SCL_LIMIT_MINALARM      0x00000004
#define SCL_LIMIT_MAXALARM      0x00000008

//�����������
typedef struct _SC_NOTIFY_DATA
{
  DWORD id;                     //id �����
  DWORD reason;                 //������� ���������
} SC_NOTIFY_DATA, *LPSC_NOTIFY_DATA;

//���� �����������
#define MNF_SCALE               0x20000000      //����� ����������� ������ ����
#define MNCODE_SCALE_ID         0               //�������� ����� � id
#define MNCODE_SCALE_REFRESH    1               //�����-���� ����� ����

//������� ������ �����������
#define MNCODE_SCALE_CREATE     0x00000001      //������� �����
#define MNCODE_SCALE_DELETE     0x00000002      //������� �����
#define MNCODE_SCALE_NAME       0x00000004      //�������� ���
#define MNCODE_SCALE_LIMITS     0x00000008      //�������� �������
#define MNCODE_SCALE_SEGMENT    0x00000010      //�������� ��������

#pragma pack(push,1)

//��� ������ ���������� GKH_RET_SUCCESS ��� GKH_RET_ERROR

//��������� �� �����
//p1 = (LPSC_CALC) ��������� ��� ��������
//p2 = (bool)direct  true - ������ ������, falst - �������� ������
#define  CM_SC_CALC             MCMD(MT_SCALE,1)

typedef struct _SC_CALC
{
  DWORD id;                                     //id �����
  DWORD kvant;                                  //�����
  float result;                                 //��������� �� �����
  DWORD error;                                  //0-��� ������, >0 - ��� ������
} SC_CALC, *LPSC_CALC;

//������������� ���� (��������� ������ ������ ���� ����� - size=������ DWORD � ������)
//p1 = (int) ���������� ����� �����
//p2 = (LPSCALE_ENUM) �����
#define  CM_SC_ENUM             MCMD(MT_SCALE,2)

typedef struct _SCALE_ENUM
{
  DWORD dw_size;
  DWORD scale_id;
  wchar_t name[MAX_PATH];
} SCALE_ENUM, *LPSCALE_ENUM;

//������� ����� �� �����
//p1 = (char *)/(wchar_t *) ������� ��� �����
//p2 = (DWORD *) ������� id �����
#define  CM_SC_SELECT_A         MCMD(MT_SCALE,3)
#define  CM_SC_SELECT_W         MCMD(MT_SCALE,4)

//���������� ���������� ����
//p1 = (int *) ������� ���������� ����
#define  CM_SC_COUNT            MCMD(MT_SCALE,5)

//������� ����� �����
//p1 = (void *) �����
//p2 = (DWORD *) ������� id �����
#define  CM_SC_CREATE           MCMD(MT_SCALE,6)

//������� �����
//p1 = (DWORD) id �����
#define  CM_SC_DELETE           MCMD(MT_SCALE,7)

//�������� ������ ��������� ����� (����� �������)
//p1 = (DWORD) id �����
//p2 = (DWORD *) ������� ������������ ������� ��������� �����
#define  CM_SC_SCALE_SIZE       MCMD(MT_SCALE,8)

//��������/��������� ����� ��������� �� id (��� ������ - size=������ DWORD � ������)
//p1 = (DWORD) id �����
//p2 = (void *) �����
#define  CM_SC_SET              MCMD(MT_SCALE,9)
#define  CM_SC_GET              MCMD(MT_SCALE,10)

//��������� ��� ����� ����
//p1 = (char *)/(wchar_t *) ����� ��� ��� ����� (0 - ����� ������ ������ � p2)
//p2 = (int *) ������ ������
#define  CM_SC_GET_FILENAME_A   MCMD(MT_SCALE,11)
#define  CM_SC_GET_FILENAME_W   MCMD(MT_SCALE,12)

//���������� ��� ����� ����
//p1 = (char *)/(wchar_t *) ����� ��� �����
//p2 = (int) 0 - rename, 1 - copy to, 2 - open other
#define  CM_SC_SET_FILENAME_A   MCMD(MT_SCALE,13)
#define  CM_SC_SET_FILENAME_W   MCMD(MT_SCALE,14)

//����� ���� ����
#define  CM_SC_SHRINK           MCMD(MT_SCALE,15)

//������������ �� ������ �� id
//p1 - (bool *) �������
#define  CM_SC_INDEX_USED       MCMD(MT_SCALE,16)

//������ �����
//p1 - (DWORD) �������
#define  CM_SC_FILE_SIZE        MCMD(MT_SCALE,17)

//������ ��������������� �����
//p1 - (DWORD) �������
#define  CM_SC_FILE_UNUSED      MCMD(MT_SCALE,18)

//�������� �������� �������� �� �����
//p1 = (LPSC_MINMAX)
#define  CM_SC_MINMAX           MCMD(MT_SCALE,19)

typedef struct _SC_MINMAX
{
  DWORD id;
  DWORD min_kv, max_kv;
  float min_y, max_y;
} SC_MINMAX, *LPSC_MINMAX;

#pragma pack(pop)

#ifdef __cplusplus


class TSegment
{
public:
  DWORD min_kvant,max_kvant;
  float k[6];
  float percent;                                //������� ������������������
  __fastcall TSegment();
  __fastcall TSegment(DWORD min_kv, DWORD max_kv, float beg, float end);
  void __fastcall SetLineK(float beg, float end);
  bool __fastcall IsLine();
  bool __fastcall InRange(DWORD kvant) {return (min_kvant<=kvant && kvant<=max_kvant)?true:false;}
  float __fastcall calc(DWORD kvant);
  bool __fastcall reverse_calc(float y, DWORD &kvant);
  float __fastcall max_y();
  float __fastcall min_y();
  bool __fastcall operator<(const TSegment &T) const;
  bool __fastcall operator==(const TSegment &T) const;
  void __fastcall Clear();
};

class TScale
{
private:
  DWORD id;                                     //���������� �������������
  wchar_t *s_name;                              //��� �����
  std::vector<TSegment> segs;                        //������ ���������
  void __fastcall Clear();
public:
  float warn_min,warn_max,alarm_min,alarm_max;  //��������� �������
  DWORD limits;                                 //����� ������� �������
  __fastcall TScale();
  __fastcall TScale(void *buf, DWORD size=0);   //���� ����� ������, �� ��������� ����������
  __fastcall ~TScale();
  DWORD __fastcall GetId() {return id;};
  void __fastcall SetAll_v1(void *buf);             //������������� ������� ������ 1
  void __fastcall SetAll(void *buf, DWORD size=0);  //���� ����� ������, �� ��������� ����������
  DWORD __fastcall GetBuffer(void *buf, DWORD size);
  void __fastcall AddSegment(TSegment *seg);
  TSegment * __fastcall GetSegment(int num);
  void __fastcall DeleteSegment(int num);
  DWORD __fastcall MinKvant();
  DWORD __fastcall MaxKvant();
  float __fastcall MinY();
  float __fastcall MaxY();
  void __fastcall SetName(wchar_t *name);
  void __fastcall SetName( char   *name);
  int __fastcall GetName(wchar_t *name, int size);
  int __fastcall GetName( char   *name, int size);
  wchar_t * __fastcall w_str() const;
  DWORD __fastcall Calc(DWORD kvant, float &y);
  DWORD __fastcall ReverseCalc(float y, DWORD &kvant);
};

#endif


#endif


