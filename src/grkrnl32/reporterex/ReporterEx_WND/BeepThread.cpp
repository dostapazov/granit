//---------------------------------------------------------------------------
#pragma warn -8058
#include <vcl.h>
#pragma warn .8058
#pragma hdrstop


#include "BeepThread.h"


#pragma package(smart_init)
//---------------------------------------------------------------------------

__fastcall TSpeakerBeep::TSpeakerBeep(bool CreateSuspended)
        : Classes::TThread(CreateSuspended)
{
  FreeOnTerminate=true;
  Priority=tpLowest;
}
//---------------------------------------------------------------------------
void __fastcall TSpeakerBeep::Execute()
{
  MessageBeep(MB_OK);
  for (int i=0; i<5; i++)
  {
    Beep(2000,50);
    Beep(3000,40);
    Beep(4000,30);
    Beep(5000,20);
  }
}
//---------------------------------------------------------------------------
