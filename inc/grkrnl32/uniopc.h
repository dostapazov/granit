#ifndef UNIOPC_INC
#define UNIOPC_INC

#pragma pack(push,1)

struct TAnTag;
struct TBitTag;
struct TIntTag;
struct TStringTag;


struct TDataForOPCServer
{
  unsigned char dataIsValid;   /* ������� ����, ��� ������ ���������
                                 (����� BAD ��� ���� ����� ����������)*/
  unsigned short anTagsQnty;   // ���������� ���������� �����
  const TAnTag *anTagsArray;   // ��������� �� ������ ���������� �����
  unsigned short bitTagsQnty;  // ���������� ������� �����
  const TBitTag *bitTagsArray; // ��������� �� ������ ������� �����
  unsigned short intTagsQnty;  // ���������� ����� �����
  const TIntTag *intTagsArray; // ��������� �� ������ ����� �����

  unsigned short structVersionNumber;/* ������ ���������.
    ��������!!!! ���� �� ����������� ������ ������������ ������,
    ������ ��������� � ����� �������� ������ ���� ������ 4 !!!
    */

  char DeviceName[64]; // ��� ����������

  union
  {
    struct // � ��� ���� ����������:
    {
      unsigned short stringTagsQnty;  // ���������� ��������� �����
      const TStringTag *stringTagsArray; // ��������� �� ������ ��������� �����
    }  ension;

    char Reserved[128]; // ��������������� ����� ��� ������� ����������.
  } Ext;
                      // (��� ������ ������ ���� ��������)
};



#pragma pack(pop)

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



}; // end of extern "C"

#endif


