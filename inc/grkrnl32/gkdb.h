#include <otd.h>
#include <gkmodules.h>

#define MEDIUMDB_HANDLE_NAME_A  "MEDIUMDB"
#define MEDIUMDB_HANDLE_NAME_W L"MEDIUMDB"

#define MNF_MEDIUMDB            0x40000000      //����� ����������� ���� ��
#define MNCODE_MEDIUMDB_ADDED   0               //�������� ����� ������ (�������������)
#define MNCODE_MEDIUMDB_REFRESH 1               //��������� ������� ��������� �������

typedef struct _db_addr           //����� �������
{
  BYTE pu,cp,fa,sb;
  WORD obj;
  _db_addr(){memset(this,0xFF,sizeof(*this));};
  bool __fastcall operator<(const _db_addr& sec) const;
}db_addr, *lpdb_addr;

class TAddress: db_addr
{
public:
  __fastcall TAddress(db_addr src){SetAddress(src.pu,src.cp,src.fa,src.sb,src.obj);};
  __fastcall TAddress(BYTE pu, BYTE cp, BYTE fa, BYTE sb, WORD obj){SetAddress(pu,cp,fa,sb,obj);};
  void __fastcall SetAddress(BYTE pu, BYTE cp, BYTE fa, BYTE sb, WORD obj);
  db_addr __fastcall GetAddress() const {return *this;};
  bool __fastcall operator<(const TAddress& second) const;
  bool __fastcall operator!=(const TAddress& second);
  TAddress& __fastcall operator=(const TAddress& src);
  TAddress& __fastcall operator+(const TAddress& src);
  TAddress& __fastcall operator++();
  TAddress& __fastcall operator--();
  bool __fastcall IsPU();
  bool __fastcall IsCP();
  bool __fastcall IsFA();
  bool __fastcall IsSB();
  bool __fastcall IsObj();
  db_addr __fastcall GetParent();
  bool __fastcall IsValid(){return (pu<0xFF) || (cp<0xFF) || (fa<0xFF) || (sb<0xFF) || (obj<0xFFFF);};
};

class db_object         //������� BD
{
  TAddress *addr;
  DWORD data;
  otd_diag diag;
public:
  __fastcall db_object(db_addr a){memset(this,0,sizeof(*this)); addr=new TAddress(a);};
  __fastcall ~db_object(){delete addr;};
};

//��������� ������ � �����
typedef struct _header  //��������� �����
{
  DWORD sz;
  DWORD signature;              //������� ����� ���� ������
  WORD type;                    //��� ����� (�������, ������)
  WORD version;                 //������ �����
  __fastcall _header(){memset(this,0,sizeof(*this)); sz=sizeof(*this);};
} header;

//������ � �������
typedef struct _rec_objects
{
  DWORD sz;
  DWORD id;             //��� ������
  db_addr addr;         //����� (��, ��, ��, �������, ������)
  DWORD id_name;        //��� ���
  DWORD id_comb;        //��� ���������� �������
  DWORD id_scale;       //��� �����
  __fastcall _rec_objects(){memset(this,0,sizeof(*this)); sz=sizeof(*this); memset(&addr,0xFF,sizeof(addr));};
} rec_objects, *lprec_objects;

typedef struct _rec_names
{
  DWORD sz;
  DWORD id;                            //��� ������
  _TCHAR s_name[ANYSIZE_ARRAY];        //������� ��������
  _TCHAR f_name[ANYSIZE_ARRAY];        //������ ��������
  __fastcall _rec_names(){memset(this,0,sizeof(*this)); sz=sizeof(*this);};
} rec_names, *lprec_names;

typedef struct _rec_combine
{
  DWORD sz;
  DWORD id;                             //��� ������
  _TCHAR formula[ANYSIZE_ARRAY];        //�������
  __fastcall _rec_combine(){memset(this,0,sizeof(*this)); sz=sizeof(*this);};
} rec_combine, *lprec_combine;

typedef struct _rec_scale
{
  DWORD sz;
  DWORD id;             //��� ������
  FLOAT a[8];           //������������ �����
  BYTE lo_kv, hi_kv;    //������ � ������� ������
  FLOAT percent;        //% ������������������
  __fastcall _rec_scale(){memset(this,0,sizeof(*this)); sz=sizeof(*this);};
} rec_scale, *lprec_scale;

//���������� ������ �������
//p1 = (lprequest) req
//ret - DWORD
#define  DBCM_REQUEST           MCMD(MT_MEDIUMDB,1)

