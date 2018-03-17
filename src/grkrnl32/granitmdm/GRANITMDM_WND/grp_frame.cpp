//---------------------------------------------------------------------------

#pragma warn -8058
#include <vcl.h>
#pragma warn .8058
#pragma hdrstop

#include "grp_frame.h"
#include "grscfrm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "sc_base_frame"
#pragma resource "*.dfm"
TGroupFrame *GroupFrame;
//---------------------------------------------------------------------------
__fastcall TGroupFrame::TGroupFrame(TComponent* Owner)
        : TSCBaseFrame(Owner)
{
 props.dw_size = sizeof(props);
}
//---------------------------------------------------------------------------

bool  __fastcall TGroupFrame::apply_changes()
{
 bool ret             = false;
 props.mask           = changes_mask;
 props.addr.sb        = _wtoi(GrpNum->Text.c_str());
 props.addr.fa        = GrpFa->ItemIndex;
 props.mod_number     = _wtoi(ModNum->Text.c_str());
 props.respond_timer  = _wtoi(RespondTimer->Text.c_str());
 props.work_timer     = _wtoi(WorkTimer->Text.c_str());
 props.nowork_timer   = _wtoi(NoWorkTimer->Text.c_str());
 props.respond_smooth = _wtoi(RespondSmooth->Text.c_str());

 if(props.mod_number>=0 && props.addr.fa <OTD_FA_ALL)
 {
 if(create_mode)
    addr.addr = props.addr.addr;
 else
   props.mask |= ApplyTimersToAll->Checked ? GRSC_FPRP_TIMERS_TOALL:0;
   ApplyTimersToAll->Checked = false;

 ret = mod_iface.call(create_mode ? SCCM_CREATE_GROUP:SCCM_SET_PROPS,addr.addr,(LPARAM)&props) == GKH_RET_SUCCESS? true:false;
 if(ret)
    {
     addr.addr   = props.addr.addr;
     create_mode = false;
     if(changes_mask &(GRSC_FPRP_ADDR))
     {
        TSCGranFrm * grfrm = dynamic_cast<TSCGranFrm *>(Owner);
        if(grfrm && grfrm->SCTree->SelectionCount)
        {
         grfrm->SCTree->Selected->Data = (LPVOID) addr.addr;
         grfrm->sort_nodes(grfrm->SCTree->Selected->Parent);
         set_active(addr.addr);
        }
     }
     else
      setup_controls(props);
      changes_mask = 0;
    }

  }
  else
  SetLastError(GRSCERR_INVALID_ADDRESS);
  return ret;
}

bool  __fastcall TGroupFrame::undo_changes ()
{
  set_active(addr.addr);
  return true;
}

void  __fastcall TGroupFrame::set_active   (DWORD addr)
{
  AnsiString str;
  if(create_mode)
   {
    ModNum->Text = str;
    GrpNum->Text = str;
    ParamCount->Caption = str;
    GrpFa->ItemIndex = -1;
    RespondTimer->Text = 10;
    WorkTimer->Text    = 60;
    NoWorkTimer->Text  = 120;
    props.addr.addr    = addr;
   }
   TuModule->Text = str;
   TuHoldTime->Text = str;
   TuSelTime->Text = str;
   TuTimeout->Text = str;
   TuCmd->ItemIndex = -1;

   TSCBaseFrame::set_active(addr);
   GroupBox3->Visible = !create_mode;
   ModNum->SetFocus();


}

void    __fastcall TGroupFrame::update_changes(LPGRSC_ACR acr)
{
  if(!create_mode)
   {
    switch(acr->what )
    {
     case -1:     setup_tu_list();
     case  0:     mod_iface.call(SCCM_GET_PROPS,addr.addr,(LPARAM)&props);
                  setup_controls(props);
                  changes_mask &=~props.mask ;
                  break;
     case  GRSC_ACR_TUENTRY_ADDED   : on_tu_entry_added  (acr->loN);break;
     case  GRSC_ACR_TUENTRY_CHANGED : on_tu_entry_set    (acr->loN);break;
     case  GRSC_ACR_TUENTRY_REMOVED : on_tu_entry_removed(acr->loN);break;

    }
   }
}


void  __fastcall TGroupFrame::set_inactive()
{
   TSCBaseFrame::set_inactive();
}

void  __fastcall TGroupFrame::setup_controls(GR_GRP_PROPS & prp)
{

 //GrpFa->Enabled = create_mode;
 if(create_mode)
 {
  AnsiString str;
  GrpNum->Text        = str;
  GrpFa->ItemIndex    = -1;
  ParamCount->Caption = str;
  ModNum->Text        = str;
  RespondTimer->Text  = 10;
  RespondSmooth->Text = 1;
  WorkTimer->Text     = 60;
  NoWorkTimer->Text   = 120;
  TuTrList->Clear();
 }
 else
 {
  addr.addr           = prp.addr.addr;
  GrpNum->Text        = prp.addr.sb;
  GrpFa->ItemIndex    = prp.addr.fa;
  ParamCount->Caption = prp.param_count;
  ModNum->Text        = prp.mod_number;
  RespondTimer->Text  = prp.respond_timer;
  WorkTimer->Text     = prp.work_timer;
  NoWorkTimer->Text   = prp.nowork_timer;
  RespondSmooth->Text = prp.respond_smooth;
 }
}


void    __fastcall TGroupFrame::setup_tu_item(TListItem * item,GRSC_TU_ENTRY &tue)
{
 item->Caption = (DWORD) tue.object;
 item->Data    = (LPVOID)tue.object;
 item->SubItems->Clear();
 item->SubItems->Add(DWORD(tue.tu_group));
 item->SubItems->Add(DWORD(tue.tu_object));
 item->SubItems->Add(TuCmd->Items->Strings[(tue.cmd_on-1)/2]);
 item->SubItems->Add(tue.tu_timeout);
}

void    __fastcall TGroupFrame::setup_tu_list()
{
 TuTrList->Items->Clear();
 DWORD index = 0;
 GRSC_TU_ENTRY tue;
 tue.dw_size = sizeof(tue);
 tue.addr.addr = addr.addr;
 while(on_tu_entry_added(index))
  index++;
 if(TuTrList->Items->Count)
    TuTrList->Items->Item[0]->Selected = true;
    else
    {
     ObjNumber->Text = AnsiString();
     TuModule->ItemIndex  = -1;
     TuCmd->ItemIndex     = -1;
     TuGroup->ItemIndex   = -1;
     TuObj->ItemIndex     = -1;
     TuTimeout->ItemIndex = -1;
     TsInv->Checked = false;
     NoCheckTU->Checked = false;
     this->RAviaRC->Checked = false;
    }
}

bool           __fastcall TGroupFrame::on_tu_entry_added  (DWORD idx)
{
 GRSC_TU_ENTRY tue;
 tue.dw_size = sizeof(tue);
 tue.addr.addr = addr.addr;
 if(GKH_RET_SUCCESS == mod_iface.call(SCCM_ENUM_TUENTRY,(LPARAM)&tue,idx))
 {
  TListItem * item = TuTrList->Items->Insert(idx);
  setup_tu_item(item,tue);
  return true;
 }
 return false;
}

void           __fastcall TGroupFrame::on_tu_entry_set    (DWORD idx)
{

 GRSC_TU_ENTRY tue;
 tue.dw_size = sizeof(tue);
 tue.addr.addr = addr.addr;
 if(GKH_RET_SUCCESS == mod_iface.call(SCCM_ENUM_TUENTRY,(LPARAM)&tue,idx))
 {
  if(idx<(DWORD)TuTrList->Items->Count)
  {
  TListItem * item = TuTrList->Items->Item[idx];
   setup_tu_item(item,tue);
  } 
 }
}

void           __fastcall TGroupFrame::on_tu_entry_removed(DWORD idx)
{
 if(idx<(DWORD)TuTrList->Items->Count)
    TuTrList->Items->Delete(idx);
}


void __fastcall TGroupFrame::RespondTimerChange(TObject *Sender)
{
 changes_mask |=GRSC_FPRP_TIMERS;
}
//---------------------------------------------------------------------------

void __fastcall TGroupFrame::GrpFaChange(TObject *Sender)
{
 changes_mask |=GRSC_FPRP_ADDR;
}
//---------------------------------------------------------------------------


AnsiString __fastcall TGroupFrame::get_text()
{
 AnsiString str;
 str.printf("Группа %s № %d",GrpFa->Items->Strings[addr.fa].c_str(),(DWORD)addr.sb);
 return str;
}

void __fastcall TGroupFrame::flStaticClick(TObject *Sender)
{
 changes_mask |=GRSC_FPRP_FLAGS;
}
//---------------------------------------------------------------------------

void __fastcall TGroupFrame::ModNumChange(TObject *Sender)
{
 changes_mask |=GRSC_FPRP_MODULE;
}
//---------------------------------------------------------------------------
void __fastcall TGroupFrame::RespondSmoothChange(TObject *Sender)
{
 changes_mask|= GRSC_FPRP_RESPOND_SMOOTH;
}
//---------------------------------------------------------------------------



void    __fastcall TGroupFrame::setup_tu_controls(GRSC_TU_ENTRY & tue)
{
 AnsiString str;
 ObjNumber->Text    = tue.object;
 TuGroup->ItemIndex = tue.tu_group;
 TuObj->ItemIndex   = tue.tu_object;
 TuModule->Text     = tue.tu_module; 
 str.printf("%.2f",float(tue.tu_timeout)/1000.0);
 TuTimeout->Text    = str;
 str.printf("%.2f",float(tue.tu_hold_time)/1000.0);
 TuHoldTime->Text    = str;
 str.printf("%.2f",float(tue.tu_select_timeout)/1000.0);
 TuSelTime->Text    = str;
 TuCmd->ItemIndex   = (tue.cmd_on-1)/2;
 TsInv->Checked     = (tue.tu_flags&GRSC_TUFL_TSINVERSE) ? true:false;
 NoCheckTU->Checked = (tue.tu_flags&GRSC_TUFL_NOCHECK)   ? true:false;
 RAviaRC->Checked   = (tue.tu_flags&GRSC_TUFL_RAviaRC)   ? true:false;
}

bool    __fastcall TGroupFrame::make_tu_entry(GRSC_TU_ENTRY &tue,bool & new_entry)
{
 if(ObjNumber->Text.Length() &&  TuGroup->ItemIndex>=0 && TuObj->ItemIndex >=0)
 {
    ZeroMemory(&tue,sizeof(tue));
    tue.dw_size   = sizeof(tue);
    tue.addr.addr = addr.addr;
    tue.object    = _wtoi(ObjNumber->Text.c_str());
    tue.tu_module = _wtoi(TuModule->Text.c_str());
    if(!tue.tu_module)
        tue.tu_module = -1;
    if(TsInv->Checked)
       tue.tu_flags  |= GRSC_TUFL_TSINVERSE;
    if(NoCheckTU->Checked)
       tue.tu_flags  |= GRSC_TUFL_NOCHECK;
    if(RAviaRC->Checked)
     tue.tu_flags|=GRSC_TUFL_RAviaRC;
     tue.cmd_on     = TuCmd->ItemIndex*2+1;
     tue.cmd_off    = tue.cmd_on+1;
     tue.tu_group   = TuGroup->ItemIndex;
     tue.tu_object  = TuObj->ItemIndex;
     tue.tu_timeout = (DWORD)(_wtof(TuTimeout->Text.c_str())*1000.0);
     tue.tu_hold_time   = (DWORD)(_wtof(TuHoldTime->Text.c_str())*1000.0);
     tue.tu_select_timeout = (DWORD)(_wtof(TuSelTime->Text.c_str())*1000.0);
     if(!tue.tu_timeout  )  tue.tu_timeout = 10000;
     if(!tue.tu_hold_time)  tue.tu_hold_time = 2500;
     if(!tue.tu_select_timeout)tue.tu_select_timeout = 10000;
     new_entry = (TuTrList->FindCaption(0,AnsiString(int(tue.object)),false,true,false)) ? false:true;
     return true;
 }
 return false;
}

void __fastcall TGroupFrame::TuCmdChange(TObject *Sender)
{
 //Изменения в параметрах ТУ
 GRSC_TU_ENTRY tue;
 ZeroMemory(&tue,sizeof(tue));
 tue.dw_size = sizeof(tue);
 bool new_entry ;
 bool ok = make_tu_entry(tue,new_entry);
 if(ok)
 {
  AddTuEntry ->Enabled = new_entry;
  SetTuEntry ->Enabled = !new_entry;
 }
 else
 {
 AddTuEntry ->Enabled = false;
 SetTuEntry ->Enabled = false;
 }
}
//---------------------------------------------------------------------------


void __fastcall TGroupFrame::AddTuEntryClick(TObject *Sender)
{
  //Добавление записи ТУ
  GRSC_TU_ENTRY tue;
  bool new_entry;
  if(make_tu_entry(tue,new_entry))
  {
    begin_call_gkhandle("Добавление записи ТУ/ТР");
    DWORD idx = mod_iface.call(SCCM_ADD_TUENTRY,(LPARAM)&tue,0);
    if( idx !=(DWORD) GKH_RET_ERROR)
    {
     Application->ProcessMessages();
     if(idx<(DWORD)TuTrList->Items->Count)
        TuTrList->Items->Item[idx]->Selected = true;
    }
    done_call_gkhandle((idx == (DWORD)GKH_RET_ERROR) ? GKH_RET_ERROR:0);
    
    
  }
}
//---------------------------------------------------------------------------

void __fastcall TGroupFrame::TuTrListSelectItem(TObject *Sender,
      TListItem *Item, bool Selected)
{
     if(Selected)
     {
      GRSC_TU_ENTRY tue;
      tue.dw_size = sizeof(tue);
      tue.addr.addr = addr.addr;
      tue.object = (DWORD)Item->Data;
      mod_iface.call(SCCM_GET_TUENTRY,(LPARAM)&tue,0);
      setup_tu_controls(tue);
     }
     AddTuEntry->Enabled = false;
     SetTuEntry->Enabled = false;
     DelTuEntry->Enabled = Item && Selected;


}
//---------------------------------------------------------------------------

void __fastcall TGroupFrame::SetTuEntryClick(TObject *Sender)
{
  GRSC_TU_ENTRY tue;
  bool new_entry;
  if(make_tu_entry(tue,new_entry))
  {
    begin_call_gkhandle("Изменение параметров ТУ/ТР");
    if(mod_iface.call(SCCM_SET_TUENTRY,(LPARAM)&tue,0) == GKH_RET_ERROR)
    {
     //TODO display error
     done_call_gkhandle(GKH_RET_ERROR);
    }
    else
    {
     SetTuEntry->Enabled = false;
     done_call_gkhandle(GKH_RET_SUCCESS);
    }

  }

}
//---------------------------------------------------------------------------

void __fastcall TGroupFrame::DelTuEntryClick(TObject *Sender)
{
  GRSC_TU_ENTRY tue;
  bool new_entry;
  if(make_tu_entry(tue,new_entry))
  {
    begin_call_gkhandle("Удаление записи ТУ/ТР");
    if(mod_iface.call(SCCM_DEL_TUENTRY,(LPARAM)&tue,0) == GKH_RET_ERROR)
    {
      //TODO display error
     done_call_gkhandle(GKH_RET_ERROR);
    }
    else
      done_call_gkhandle(GKH_RET_SUCCESS);


  }
}
//---------------------------------------------------------------------------



void __fastcall TGroupFrame::ApplyTimersToAllClick(TObject *Sender)
{
 if(ApplyTimersToAll->Checked )
 {
  changes_mask|=GRSC_FPRP_TIMERS|GRSC_FPRP_RESPOND_SMOOTH;
 }
}
//---------------------------------------------------------------------------

