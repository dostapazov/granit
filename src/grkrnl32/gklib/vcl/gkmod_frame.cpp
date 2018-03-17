//---------------------------------------------------------------------------

#pragma warn -8058
#include <vcl.h>
#pragma warn .8058
#pragma hdrstop

#include "gkmod_frame.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TGKModuleFrame *GKModuleFrame;
//---------------------------------------------------------------------------
__fastcall TGKModuleFrame::TGKModuleFrame(TComponent* Owner)
        : TFrame(Owner)
{
 Fcurrent_object = NULL;
 Fin_setup       = 0;
 Fch_mask        = 0;
}
//---------------------------------------------------------------------------

void __fastcall TGKModuleFrame::Setmod_handle(GKHANDLE value)
{
    if(mod_iface()!=value)
    {
      mod_iface.close();
      if(value)
       mod_iface.open(value,true);
       after_set_gkhandle();
    }
}

GKHANDLE __fastcall TGKModuleFrame::Getmod_handle()
{
 return mod_iface();
}

void __fastcall TGKModuleFrame::enum_modules(GKHANDLE handle,LPVOID data)
{

 int i = 0;
 GKHANDLE h;
 do{
    h = (GKHANDLE)call_gkhandle(handle,MCMD_ENUM_MODULES,LPARAM(i++),0);
    if(h && LRESULT(h)!=GKH_RET_ERROR)
    {
     if(!on_enum_handle(handle,h,data))        break;
    }
    else
    break;
    }while(1);
}

void     __fastcall  TGKModuleFrame::set_ch_mask(DWORD value)
{
    if(!in_setup && Fch_mask!=value)
    {
        Fch_mask = value;
    }
   if(on_changed) on_changed(this);
}



