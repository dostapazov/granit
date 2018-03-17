//---------------------------------------------------------------------------
#pragma hdrstop
#include <vcl.h>
#include "about_common_frame.h"
#include "System.IOUtils.hpp"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TAboutCommonFrame *AboutCommonFrame;
//---------------------------------------------------------------------------
__fastcall TAboutCommonFrame::TAboutCommonFrame(TComponent* Owner)
	: TFrame(Owner)
{
  TStringList * sl = new TStringList;
  try{

      TPath p;
      UnicodeString file_name = p.GetDirectoryName(GetModuleName(NULL));
      file_name += L"\\product_customer.txt";

      sl->LoadFromFile(file_name);
      Memo2->Lines->AddStrings(sl);
     }
     catch(...)
     {
     }
  delete sl;
}
//---------------------------------------------------------------------------
