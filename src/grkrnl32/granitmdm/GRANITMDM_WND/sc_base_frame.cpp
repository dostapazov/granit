//---------------------------------------------------------------------------

#pragma warn -8058
#include <vcl.h>
#pragma warn .8058
#pragma hdrstop

#include "sc_base_frame.h"
#include "grscfrm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TSCBaseFrame *SCBaseFrame;
//---------------------------------------------------------------------------
__fastcall TSCBaseFrame::TSCBaseFrame(TComponent* Owner)
        : TFrame(Owner)
{
 Fon_change    = 0;
 Fchanges_mask = 0;
 Fcreate_mode  = false;
}
//---------------------------------------------------------------------------

AnsiString __fastcall TSCBaseFrame::get_text()
{return AnsiString();}

void __fastcall TSCBaseFrame::Setmod_handle(GKHANDLE value)
{
   if(!is_same_gkhandle(value,mod_iface()))
   {
     mod_iface.open(value,true);
     after_set_gkhandle();
   }
}
GKHANDLE __fastcall TSCBaseFrame::Getmod_handle()
{
        //TODO: Add your source code here
        return mod_iface();
}

void __fastcall TSCBaseFrame::Setchanges_mask(DWORD value)
{
        //TODO: Add your source code here
        if(Fchanges_mask!=value)
        {
          Fchanges_mask=value;
          if(on_change)
             on_change(this);
        }
}

void  __fastcall TSCBaseFrame::set_active (DWORD _addr)
{
 GRSC_ACR acr;
 acr.addr  = addr.addr = _addr;
 acr.what  = acr.parts = acr.hiN   = -1;
 acr.loN   = 0;
 update_changes(&acr);
 update_data_changes(&acr);
 Visible = true;
 Fchanges_mask = 0;
 if(on_change)
    on_change(this);
}

void  __fastcall TSCBaseFrame::begin_call_gkhandle(char * cmd_text)
{
 TSCGranFrm * frm = dynamic_cast<TSCGranFrm*>(Owner);
 if(frm)
    frm->begin_call_gkhandle(cmd_text);
}

void  __fastcall TSCBaseFrame::done_call_gkhandle (DWORD ret,char * text )
{
 TSCGranFrm * frm = dynamic_cast<TSCGranFrm*>(Owner);
 if(frm)
    frm->done_call_gkhandle(ret,text);

}



