//---------------------------------------------------------------------------


#include <stdlib.h>
#include "gkipclient.hpp"



TGKIPClient * gkclient = NULL;

void __fastcall debug_init();

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fwdreason, LPVOID lpvReserved)
{
        if(fwdreason ==  DLL_PROCESS_ATTACH)
        {
         gkclient = new TGKIPClient();
         #ifdef _DEBUG
         debug_init();
         #endif
         if(gkclient)
            gkclient->Start(0);
        }
        return 1;
}
//---------------------------------------------------------------------------

__declspec( dllexport )
     const TDataForOPCServer * PASCAL GetDataForOPCServer( void )
     {

	  TDataForOPCServer * ret =  gkclient ? gkclient->get_data() : NULL;
      return ret;
     }



//   2) ������� ��� ��������� �������� ����������� ����
__declspec( dllexport )
       unsigned char PASCAL SetAnTagValue(
       unsigned  number, // ���������� (HIWORD) �
                          // ����� ���� � ������� (LOWORD)
       float value            // ����� �������� ���������
       )
       {
        return 0;
       }

//   3) ������� ��� ��������� �������� �������� ����
__declspec( dllexport )
       unsigned char PASCAL SetBitTagValue(
       unsigned  number, // ���������� (HIWORD) �
                          // ����� ���� � ������� (LOWORD)
       unsigned char value    // ����� �������� ���������
       )
       {
        return 0;
       }

//   4) ������� ��� ��������� �������� ������ ����
__declspec( dllexport )
       unsigned char PASCAL SetIntTagValue(
       unsigned  number, // ���������� (HIWORD) �
                          // ����� ���� � ������� (LOWORD)
       int value    // ����� �������� ���������
       )
       {
        return 0;
       }

//   4) ������� ��� ��������� �������� ���������� ����
__declspec( dllexport )
       unsigned char PASCAL SetStringTagValue(
       unsigned  number, // ���������� (HIWORD) �
                          // ����� ���� � ������� (LOWORD)
       const char* value    // ����� �������� ���������
       )
       {
        return 0;
       }
__declspec( dllexport )
       void PASCAL OnShutDown()
       {
        if(gkclient)
           {
            gkclient->TerminateAndWait(20000);
            delete gkclient;
           }

        return;
       }

void __fastcall debug_init()
{
         #ifdef _DEBUG
         TDataForOPCServer * ptr = gkclient->get_data();
         while(ptr->structVersionNumber)
         {
         TAnTag   * an_ptr  = ptr->anTagsArray;
         TIntTag  * int_ptr = ptr->intTagsArray;
         TBitTag  * bit_ptr = ptr->bitTagsArray;
         ptr->dataIsValid = TRUE;

         __int64 _tm = GetTime(false);
         FILETIME tm;
         memcpy(&tm,&_tm,sizeof(tm));

         for(int i = 0;i<ptr->anTagsCount;i++)
         {
         exposeAnTag(an_ptr,float(rand())/100,QualityGood,tm);
		 ++an_ptr;
         }
         for(int i = 0;i<ptr->intTagsCount;i++)
         {
         exposeIntTag(int_ptr,rand(),QualityGood,tm);
		 ++int_ptr;
         }

         for(int i = 0;i<ptr->bitTagsCount;i++)
         {
         exposeBitTag(bit_ptr,(rand()&1) ? TRUE:FALSE,QualityGood,tm);
		 ++bit_ptr;
         }
         ptr++;
        }
      #endif  
   }


