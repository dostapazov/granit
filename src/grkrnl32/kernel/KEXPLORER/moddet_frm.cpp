//---------------------------------------------------------------------------
#pragma hdrstop
#include <vcl.h>
#include "moddet_frm.h"
#include <KeRTL.hpp>
#include "gkexplorer_form.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TModuleDefine *ModuleDefine;
//---------------------------------------------------------------------------
__fastcall TModuleDefine::TModuleDefine(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
DWORD __fastcall HexNumeric  (char * s)
{
    DWORD res = 0;

    while(*s)
    {
         char  c = toupper(*s);s++;
         res<<=4;
         res|= (c <'A') ? int(c-'0') : int(c-'A'+10);

    }
    return res;
}

#pragma warn -8057

void __fastcall TModuleDefine::Button1Click(TObject *Sender)
{

  LPVOID addr ;
  char module_name[4096];

  AnsiString str = Edit1->Text.UpperCase();
  if(str.SubString(1,2)=="0X")
     addr = (LPVOID)  HexNumeric(str.c_str()+2);
     else
     addr = (LPVOID)  atoi(str.c_str());

  str+= " - ";
  if(GetAsyncKeyState(VK_LCONTROL))
  {
   char text[MAX_PATH<<1];
   GetErrorMessage(DWORD(addr),text,sizeof(text),(char *)NULL);
   str+=text;
  }
  else
  {
   if(KeRTL::GetPtrModuleName(addr,module_name,sizeof(module_name)))
   {
    str+=module_name;
   }
   else
   str+= "NO MODULE";
  }
  Memo1->Lines->Add(str);
}


//---------------------------------------------------------------------------
void __fastcall TModuleDefine::FormDestroy(TObject *Sender)
{
  TGKExplorerFrm * frm = dynamic_cast<TGKExplorerFrm*>(Owner);
  if(frm)
     frm->mod_def = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TModuleDefine::FormClose(TObject *Sender,
      TCloseAction &Action)
{
 if(Action == caHide)
    Action = caFree;
}
#pragma warn .8057
//---------------------------------------------------------------------------

