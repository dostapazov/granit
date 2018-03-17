//---------------------------------------------------------------------------
#pragma warn -8058
#include <vcl.h>
#pragma warn .8058
#pragma hdrstop

#include "cp_frame.h"
#include "grscfrm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "sc_base_frame"
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TCPFrame::TCPFrame(TComponent* Owner)
        : TSCBaseFrame(Owner)
{
}
//---------------------------------------------------------------------------

bool  __fastcall TCPFrame::apply_changes()
{
  DWORD lu          = _wtoi(LuNum->Text.c_str());
  cpr.addr.cp       = GRPROTO_MK_CPNUM(GRPROTO_GET_FLOOR_NUM(addr.cp),lu);
  cpr.mask          = create_mode ? -1: changes_mask;
  cpr.flags         = 0;

  if(CpTestFlag->Checked )   cpr.flags |=  GRSC_FLAG_TEST ;
  if(CpLogErrors->Checked )  cpr.flags |=  GRSC_FLAG_CP_LOGHARDWARE_ERROR;
  if(CpNotReceipt->Checked ) cpr.flags |=  GRSC_FLAG_CP_NOTRECEIPT;
  if(NumberedTS->Checked)    cpr.flags |=  GRSC_FLAG_CP_NUMBERED_TS;

  safe_strcpyn(cpr.name,CpName->Text.c_str(),sizeof(cpr.name)/sizeof(wchar_t));
  cpr.send_wait_time  = _wtoi(swt->Text.c_str());
  if(create_mode)
     addr.cp = cpr.addr.cp;
 bool   ret = mod_iface.call(create_mode ? SCCM_CREATE_CP:SCCM_SET_PROPS,addr.addr,(LPARAM)&cpr) == GKH_RET_SUCCESS;
 if(ret)
    {


     if(!create_mode &&(changes_mask & GRSC_FPRP_ADDR))
     {
        addr.cp = cpr.addr.cp;
        TSCGranFrm * grfrm = dynamic_cast<TSCGranFrm *>(Owner);
        if(grfrm && grfrm->SCTree->SelectionCount)
        {
         grfrm->SCTree->Selected->Data = (LPVOID) addr.addr;
         grfrm->sort_nodes(grfrm->SCTree->Selected->Parent);
         set_active(addr.addr);
        }

     }
     else
     {
      changes_mask = 0;
      setup_controls(cpr);
     }
    }
 return ret;
}

bool  __fastcall TCPFrame::undo_changes ()
{
  set_active(this->addr.addr);
  return true;
}

void  __fastcall TCPFrame::set_active   (DWORD addr)
{

 ZeroMemory(&cpr,sizeof(cpr));
 cpr.dw_size = sizeof(cpr);
 cpr.addr.addr = addr;
 if(!create_mode)
   mod_iface.call(SCCM_GET_PROPS,addr,(LPARAM)&cpr);
 setup_controls(cpr);
 changes_mask = 0;
 TSCBaseFrame::set_active(addr);
 if(create_mode)
   LuNum->SetFocus();

}

void  __fastcall TCPFrame::set_inactive()
{
 TSCBaseFrame::set_inactive();
}

void   __fastcall  TCPFrame::setup_controls(GR_CP_PROPS & cpr)
{
  AnsiString str;
  if(create_mode)
  {
    LuNum->Text     = str;
    CpNum->Caption  = str;
    CpName->Text    = str;
    CpTestFlag->Checked = false;
    CpLogErrors->Checked = false;
  }
  else
  {
   CpNum->Caption  = cpr.addr.cp;
   LuNum->Text     = GRPROTO_GET_LU_NUM(cpr.addr.cp);
   CpName->Text = cpr.name;
   CpTestFlag->Checked   = (cpr.flags & GRSC_FLAG_TEST) ? true:false;
   CpLogErrors->Checked  = (cpr.flags & GRSC_FLAG_CP_LOGHARDWARE_ERROR) ? true:false;
   CpNotReceipt->Checked = (cpr.flags & GRSC_FLAG_CP_NOTRECEIPT) ? true:false;
   NumberedTS->Checked   = (cpr.flags & GRSC_FLAG_CP_NUMBERED_TS) ? true:false;
   this->swt->Text = cpr.send_wait_time;
  }
}

void   __fastcall TCPFrame::update_changes(LPGRSC_ACR acr)
{
 DWORD old_mask = cpr.mask;
 cpr.mask       = acr->parts;
 mod_iface.call(SCCM_GET_PROPS,addr.addr,(LPARAM)&cpr);
 setup_controls(cpr);
 cpr.mask|=old_mask;
 changes_mask&=~acr->parts;
}

void   __fastcall  TCPFrame::format_props  (GR_CP_PROPS & cpr)
{
}


void __fastcall TCPFrame::LuNumChange(TObject *Sender)
{
 this->changes_mask|= GRSC_FPRP_ADDR;
 CpNum->Caption     = GRPROTO_MK_CPNUM(GRPROTO_GET_FLOOR_NUM(addr.cp),_wtoi(LuNum->Text.c_str()));
}
//---------------------------------------------------------------------------

void __fastcall TCPFrame::CpNameChange(TObject *Sender)
{
 this->changes_mask|= GRSC_FPRP_NAME;
}
//---------------------------------------------------------------------------


AnsiString __fastcall TCPFrame::get_text()
{
  AnsiString str ;
  str.printf(" œ π %d ",(DWORD)addr.cp);
  str+= cpr.name;
  return str;

}

void __fastcall TCPFrame::CpTestFlagClick(TObject *Sender)
{
 changes_mask|= GRSC_FPRP_FLAGS;
}
//---------------------------------------------------------------------------

void __fastcall TCPFrame::swtChange(TObject *Sender)
{
 changes_mask|= GRSC_FPRP_SWT;
}
//---------------------------------------------------------------------------

