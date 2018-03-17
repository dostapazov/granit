#include <Windows.h>
#include <system.hpp>
#include <vector.h>

enum {TBL_OBJECTS,TBL_NAMES,TBL_COMBINE,TBL_SCALE};
enum {DB_SELECT, DB_INSERT, DB_UPDATE, DB_DELETE};
enum {DB_INC, DB_DEC, DB_LESS, DB_LESS_OR_EQUAL, DB_MORE, DB_MORE_OR_EQUAL};
typedef Set<int,DB_INC,DB_MORE_OR_EQUAL> TConditions;

#define DB_SUCCESS              0
#define DB_ERROR                1 //������������������� ������
#define DB_RECORD_NOT_FOUND     2 //��� ������� �� �������
#define DB_FAULT_PARAMS         3 //��������� �������������� ������������
#define DB_RECORD_EXISTS        4 //������ � ������ ������� ��� ����
#define DB_NOT_FOUND_KEY        5 //������ ��������
#define DB_FAULT_RECORD_TABLE   6 //������ ������ � ���� �������
#define DB_FAULT_RECORD_INDEX   7 //������ ������ � ���� �������

//��� ������� ������������ ��������� request
//rec - ������������ ��������
//rec_search - ������� ������ (�� ������� ����)
typedef struct _request
{
  DWORD sz;
  WORD table;           //�������
  WORD work_type;       //����������� �������� DB_SELECT ... DB_DELETE
  LPVOID rec;           //��������� �� ��������� ������
  LPVOID rec_search;    //��������� �� ��������� ������ - ������� ������
  TConditions condition;//���. ������� DB_INC, DB_DEC � �.�.
  LPVOID find;          //��������� ���� ��� �������� �������
} request, *lprequest;

//��������a ��� ������
typedef struct
{
  WORD table;                   //������� ������
  LPVOID rec_search;            //��������� ������ � ����������� ������
  TConditions cond;             //������� ������
  __int64 pos;                  //����� ��������� ������
  BOOL indexed;                 //����� �� �������
  WORD ind_type;                //�������� ����� �������
  DWORD ind;                    //������� ������ ��� ����� ������� � positions
  vector<__int64> positions;    //��� ���������������, �������� ��� ������ DBFindFirst
} TDBRecSearch;


