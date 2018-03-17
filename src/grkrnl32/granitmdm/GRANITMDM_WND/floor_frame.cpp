//---------------------------------------------------------------------------
#pragma warn -8058
#include <vcl.h>
#pragma warn .8058
#pragma hdrstop

#include "floor_frame.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "sc_base_frame"
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TFloorFrame::TFloorFrame(TComponent* Owner)
        : TSCBaseFrame(Owner)
{
}
//---------------------------------------------------------------------------

void  __fastcall TFloorFrame::set_active (DWORD  _addr)
{
 TSCBaseFrame::set_active(_addr);
 Visible   = true;
 if(create_mode)
   FloorNum->SetFocus();

}

void  __fastcall TFloorFrame::setup_controls(GR_FLOOR_PROPS & fpr)
{
  CpCount->Visible = !create_mode;
  if(create_mode)
   {
    FloorNum->Text =AnsiString();
   }
   else
   {
    FloorNum->Text       =  GRPROTO_GET_FLOOR_NUM(fpr.addr.cp);
    CpCount ->Caption    =  fpr.cp_count;
   }
}



bool  __fastcall TFloorFrame::apply_changes()
{
 props.mask = changes_mask;

 props.addr.addr = sotd_addr(0,GRPROTO_MK_CPNUM(_wtoi(FloorNum->Text.c_str()),0),-1,-1).addr;
 if(create_mode)
    {
    addr.addr = props.addr.addr;
    props.flags     = 0;
    props.mask = -1;
    }
 bool ret = mod_iface.call(create_mode ? SCCM_CREATE_FLOOR:SCCM_SET_PROPS,addr.addr,(LPARAM)&props) == GKH_RET_SUCCESS;
 if(ret)
    {
     changes_mask = 0;
     setup_controls(props);
    }


 return ret;
}


bool  __fastcall TFloorFrame::undo_changes ()
{
 set_active(addr.addr);
 return true;
}

AnsiString __fastcall TFloorFrame::get_text()
{
  AnsiString str = "Этаж №";
  str+=(DWORD)GRPROTO_GET_FLOOR_NUM(addr.cp);
  return str;

}


void    __fastcall TFloorFrame::update_changes(LPGRSC_ACR acr)
{
 props.dw_size = sizeof(props);
 props.mask    = -1;
 if(!create_mode)
     mod_iface.call(SCCM_GET_PROPS,addr.addr,(LPARAM)&props);
 setup_controls(props);
 changes_mask = 0;
}

void __fastcall TFloorFrame::FloorNumChange(TObject *Sender)
{
  changes_mask|=GRSC_FPRP_ADDR;
}
//---------------------------------------------------------------------------

void __fastcall TFloorFrame::flStaticClick(TObject *Sender)
{
 changes_mask|=GRSC_FPRP_FLAGS;
}
//---------------------------------------------------------------------------

