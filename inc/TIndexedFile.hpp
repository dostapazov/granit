//---------------------------------------------------------------------------

#ifndef TIndexedFileH
#define TIndexedFileH

#include <KeRTLio.hpp>
#include <map>
//---------------------------------------------------------------------------
using namespace KeRTL;
using namespace std;

// ���� � ������������ �������� � ������� ������������ �����
// �������� uids ��� �������� ��������� �� ������������� ����������� ����� � ������� �� ����
// �������������� 2 ������� �������:
// 1 - ��������������� �� ����������� ������ ������
// 2 - ��������������� �� ����������� ������ ������ ��� �� id
// ��������� �����:
//   ����� ��������� - ������, ����� �������, �������������� �����, ������ ��������� ������
//   ��������� ������ - �������=����� �� ������ �� ������ �����,
//                      ��������� ������� ��������� �� ��������� ���������,
//                      0 ��������� �� ���������� ������ (��� �� ��������� ��� �������)
//     ������ ���������� � �������, ����� ���������� id (���� ������������)
// ��� ���������� ������� ������ ������������ � ����� �����

#pragma warn -8098
const DWORD IndFileSignature='FDNI';  //INDexed File
#pragma warn .8098
const DWORD IndFileVersion=1;
const DWORD IndFileHeaderSize=1024;

//������������ ������
#define INDF_ERR_SUCCESS        DWORD(0)        // ��� ������
#define INDF_ERR_ERROR          DWORD(-1)       // ����� ������
#define INDF_ERR_BUFSIZE        DWORD(-2)       // ����� �������������� �������

//��������� �����
typedef struct _TIndFileHdr
{
  DWORD size;           //������ ���������
  DWORD sig;            //������� �����
  union
  {
    DWORD version;      //������
    struct
    {
      WORD ext_ver;     //������ ������ � �����
      WORD int_ver;     //������ ������ �����
    };
  };
  DWORD num_rec;        //���-�� �������
  DWORD unused_size;    //��������� ������ ��������������� �����
  DWORD uid;            //��������� ���������������� ���������� id
  DWORD index_save;     //����� �� ����������� �������
  bool  with_index;     //������������ ������ �� ����������� �����
  bool  closed;         //���� false, ���� ����������� ������ � �������������� �����
} TIndFileHdr;

// ������ �� id
typedef map<DWORD,DWORD,less<DWORD> >                   TIndex;
typedef map<DWORD,DWORD,less<DWORD> >::value_type       TInd_value;
typedef map<DWORD,DWORD,less<DWORD> >::iterator         TInd_iter;

KERTL_CLASS TIndexedFile
{
private:
  TFile file;
  DWORD hdr_sz;         //������ ��������� ������ � DWORD'��
  DWORD *sec_hdr;       //���� ��� ���������� ��������� ������
  DWORD hdr_pos;        //������ �������� ���������
  DWORD num_rec;
  DWORD unused_size;
  bool indexed;
  bool read_only;
  DWORD uid;
  TIndex id_keys;
  bool opened;
  wchar_t *f_name;
  void __fastcall ReCount();
  bool __fastcall Init();
  int __fastcall CheckVersion(DWORD ver);
  DWORD __fastcall GetPos(DWORD ind);
  void __fastcall IncUID(DWORD num);
public:
  TIndexedFile(bool uids = false);
  TIndexedFile(wchar_t * file_name, bool uids = false, bool ro = false);
  TIndexedFile(char    * file_name, bool uids = false, bool ro = false);
  ~TIndexedFile();
  bool __fastcall open  ();             //������� ��������� �������� ����
  bool __fastcall open  (wchar_t * file_name, bool uids = false, bool ro = false);
  bool __fastcall open  ( char   * file_name, bool uids = false, bool ro = false);
  bool __fastcall create(wchar_t * file_name, bool uids = false);
  bool __fastcall create( char   * file_name, bool uids = false);
  void __fastcall close();
  void __fastcall flush();
  DWORD __fastcall write(void * data, DWORD size);                               //�����. ����� ������ (-1 - ������)
  DWORD __fastcall write(DWORD num, void * data, DWORD size, bool by_index=true);//������ �������, ����� ��������
  DWORD __fastcall read(DWORD num, void * buffer, DWORD sz, bool by_index=true); //�����. 0 - �������, -1 - ������, -2 - �� ������� �����, ��������� - ������ ������
  DWORD __fastcall erase(DWORD num, bool by_index=true);
  DWORD __fastcall clear();
  DWORD __fastcall get_count();
  DWORD __fastcall get_size();
  DWORD __fastcall get_unused_size();
  bool __fastcall shrink();              //�����
  bool __fastcall index_used() {return indexed;};
  bool __fastcall file_opened() {return opened;};
  int __fastcall get_file_name(wchar_t *file_name, int sz);
  int __fastcall get_file_name( char   *file_name, int sz);
  DWORD __fastcall get_ext_version();
  bool __fastcall set_ext_version(DWORD ver);
};

#endif