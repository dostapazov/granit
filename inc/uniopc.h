#ifndef _UNIOPC_INC
#define _UNIOPC_INC

#include <opc_codes.h>
#include <kertl.hpp>
#include <kethreads.hpp>

using namespace KeRTL;
using namespace std;

#pragma pack(push,1)

enum{QualityUsed=0x80000000, QualityGood=0x800000c0, QualityBad=0x80000000};

struct TTagName
{
  char *name; // ���������� ������������
  TTagName(){name=NULL;};
 ~TTagName() {free_name();};
  void setName(  char *aName );
  void __fastcall free_name(){if(name) delete [] name;}
};

inline   void TTagName::setName( char *aName )
{
     free_name();
     name = newstr((char*)aName);
};

template <typename T>

struct  opc_tag:public TTagName
{
 T         value;
 int       quality;
 FILETIME  time;
 opc_tag(char * aName = NULL):value(T()),quality(QualityBad)
 {time.dwLowDateTime = time.dwHighDateTime = 0 ;setName(aName);}
 opc_tag & operator = (opc_tag & tag)
 { free_name(); setName(tag.name);value = tag.value;quality = tag.quality;time = tag.time; return * this;}
};

typedef  opc_tag<BYTE>  TBitTag;
typedef  opc_tag<int>   TIntTag;
typedef  opc_tag<float> TAnTag;


struct TDataForOPCServer
{

  static   CRITICAL_SECTION  *  _locker;
  unsigned char dataIsValid;   /* ������� ����, ��� ������ ���������
                                  (����� BAD ��� ���� ����� ����������)*/
  unsigned short anTagsCount;  // ���������� ���������� �����
  TAnTag * anTagsArray;   // ��������� �� ������ ���������� �����
  unsigned short bitTagsCount; // ���������� ������� �����
  TBitTag *bitTagsArray; // ��������� �� ������ ������� �����
  unsigned short intTagsCount; // ���������� ����� �����
  TIntTag *intTagsArray; // ��������� �� ������ ����� �����

  unsigned short structVersionNumber;/* ������ ���������.
    ��������!!!! ���� �� ����������� ������ ������������ ������,
    ������ ��������� � ����� �������� ������ ���� ������ 4 !!!
    */

  char DeviceName[64]; // ��� ����������

  struct
  {
    /*struct // � ��� ���� ����������:
    {
      unsigned short stringTagsQnty;  // ���������� ��������� �����
      const TStringTag *stringTagsArray; // ��������� �� ������ ��������� �����
    }  ension;*/
    WORD   reserv_count;
    LPVOID reserv_ptr;
    char Reserved[128-sizeof(WORD)-sizeof(LPVOID)]; // ��������������� ����� ��� ������� ����������.
  } Ext;
                      // (��� ������ ������ ���� ��������)
  TDataForOPCServer ();
  TDataForOPCServer (const char * dev_name,WORD anTagsCnt,WORD bitTagsCnt,WORD intTagsCnt,char * tag_name_prefix = 0);
  TDataForOPCServer (const TDataForOPCServer & src);
  ~TDataForOPCServer(){release_tags_arrays();};
  void __fastcall release_tags_arrays();
  TDataForOPCServer & operator = (const TDataForOPCServer & src);
  static void __fastcall lock();
  static void __fastcall unlock();


};


//=============== �������, ���������� OPC-��������: ==============
// ���������� ���� ������� ���������� �������� ������������ dll ==
// ������� ���������� � ������ dataservX.cpp
//===============================================================


extern "C"
{

/*  1) �������, ������������ ��������� 
       �� ��������� ���� TDataForOPCServer (��� �� ������ ���� ��������),
       ����������� ���������� � ����. ������������ ���� �������� ���������
       ������������ OPC-��������, ����� ������, ��� � dll ��� �����
       (�� ���� ��� �������� ����� � ��������� � ������ ����� ������)
       ������� ������ ���� ����������� ������������� dll. �������������
       ���������� ��������� (��� ������) TDataForOPCServer ����������,
       � � ���������� ������� ������ ������� �� ��� ����������.
*/

__declspec( dllexport )
     const TDataForOPCServer * PASCAL GetDataForOPCServer( void );



//   2) ������� ��� ��������� �������� ����������� ����
__declspec( dllexport )
       unsigned char PASCAL SetAnTagValue(
       unsigned  number, // ���������� (HIWORD) �
                          // ����� ���� � ������� (LOWORD)
       float value            // ����� �������� ���������
       );

//   3) ������� ��� ��������� �������� �������� ����
__declspec( dllexport )
       unsigned char PASCAL SetBitTagValue(
       unsigned  number, // ���������� (HIWORD) �
                          // ����� ���� � ������� (LOWORD)
       unsigned char value    // ����� �������� ���������
       );

//   4) ������� ��� ��������� �������� ������ ����
__declspec( dllexport )
       unsigned char PASCAL SetIntTagValue(
       unsigned  number, // ���������� (HIWORD) �
                          // ����� ���� � ������� (LOWORD)
       int value    // ����� �������� ���������
       );


//   4) ������� ��� ��������� �������� ���������� ����
__declspec( dllexport )
       unsigned char PASCAL SetStringTagValue(
       unsigned  number, // ���������� (HIWORD) �
                          // ����� ���� � ������� (LOWORD)
       const char* value    // ����� �������� ���������
       );


__declspec( dllexport )
       void PASCAL OnShutDown();



/* ������� ������� ������������ ������ OPC-��������,
   ��� ������������ dll �� ����� :
*/
__declspec( dllexport ) CRITICAL_SECTION* PASCAL  getTagLock();

__declspec( dllexport )
void exposeAnTag(TAnTag* tg, float val, unsigned qual, FILETIME tstamp);
__declspec( dllexport )
void exposeBitTag(TBitTag* tg, bool val, unsigned qual, FILETIME tstamp);
__declspec( dllexport )
void exposeIntTag(TIntTag* tg, int val, unsigned qual, FILETIME tstamp);




}; // end of extern "C"

#pragma pack(pop)

#endif
