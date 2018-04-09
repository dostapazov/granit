//---------------------------------------------------------------------------

#pragma warn -8058
#include <vcl.h>
#pragma warn .8058
#pragma hdrstop

#include "sc_cp_data_frame.h"
#include "grscfrm_rh.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "sc_base_data_frame"
#pragma resource "*.dfm"
TSCCpDataFrame *SCCpDataFrame;
//---------------------------------------------------------------------------
__fastcall TSCCpDataFrame::TSCCpDataFrame(TComponent* Owner)
        : TSCBaseDataFrame(Owner)
{
}
//---------------------------------------------------------------------------

void  __fastcall TSCCpDataFrame::set_active     (DWORD _addr)
{
  ListView1->Clear();
  GrpPDDiag->Caption = ' ';
  TuList->Clear();
  TxDeliverTime->Caption = '\0';
  bool is_cp = (GRPROTO_GET_LU_NUM(sotd_addr(_addr).cp)!=0) ? true :false;
  StaticText1->Visible  = is_cp;
  StaticText2->Visible  = is_cp;
  GroupBox2->Visible  = is_cp;
  Label8->Visible     = is_cp;
  Label9->Visible     = is_cp;
  Label10->Visible    = is_cp;
  TxByteTime->Visible = is_cp;
  TxDeliverTime->Visible = is_cp;
  RxTimer->Visible = is_cp;
  TSCBaseDataFrame::set_active(_addr);
}

void  __fastcall TSCCpDataFrame::setup_data     (otd_proto & op)
{
 TSCBaseDataFrame::setup_data(op);
 if(op.personal_diag)
 {
 DWORD addr_pd[2];
 DWORD lo = op.personal_diag->numbers.loN;
 DWORD hi = op.personal_diag->numbers.hiN;
 while(lo<=hi )
 {
  UnicodeString s;
  otd_get_value(op.personal_diag,lo,addr_pd,sizeof(addr_pd));
  TListItem * item = ListView1->FindData(0,(LPVOID)addr_pd[0],true,false);
  if(!item)
  {
   item = ListView1->Items->Add();
   item->SubItems->Add(s);
   item->Data  = (LPVOID)addr_pd[0];
   sotd_addr sa(addr_pd[0]);
   s.printf(L"%s № %02d",load_rcstring(ModuleInstance,STR_FA_TS+(DWORD)sa.fa),sa.sb);
   item->Caption = s;
   if(item->Index == 0) item->Selected = true;

  }
  s.printf(L"%08X",addr_pd[1]);
  item->SubItems->Strings[0] =s ;
  if(item->Selected) ListView1SelectItem(this,item,true);
  lo++;
 }
 }
}

void  __fastcall TSCCpDataFrame::update_data_changes(LPGRSC_ACR acr)
{
 if(!acr->what || acr->what == (DWORD)-1 )
    {
     TSCBaseDataFrame::update_data_changes(acr);
    }


 if(acr->what == (DWORD)-1)
 {
  DWORD idx =0;
  while(add_tu_entry(idx))
       idx++;
 }
 else
 {
 if(acr->what & GRSC_ACR_TUENTRY_ADDED)
   {
    add_tu_entry(acr->loN);
   }

 if(acr->what & GRSC_ACR_TUENTRY_REMOVED)
   {
    remove_tu_entry(acr->loN);
   }

 if(acr->what & GRSC_ACR_TUENTRY_CHANGED)
   {
    change_tu_entry(acr->loN);
   }
 }
}

  AnsiString __fastcall get_tu_text(LPGRSC_TU_ENTRY entry)
  {
   AnsiString ret;
   switch(entry->tu_state&(GRSC_TUTR_STATE_CMDON|GRSC_TUTR_STATE_CMDOFF|GRSC_TUTR_STATE_CMDCANCEL))
   {
    case GRSC_TUTR_STATE_CMDON    : ret = load_rcstring(ModuleInstance,STR_TU_CMD1);break;/* "Команда ВКЛ /+";*/
    case GRSC_TUTR_STATE_CMDOFF   : ret = load_rcstring(ModuleInstance,STR_TU_CMD2);break;/* "Команда ОТКЛ/-";*/
    case GRSC_TUTR_STATE_CMDCANCEL: ret = load_rcstring(ModuleInstance,STR_TU_CMD3);break;/* "Команда ОТМЕНА";*/
   }

   if(entry->tu_state&GRSC_TUTR_STATE_ACTIVE_MASK)
   {
   if(entry->tu_state&GRSC_TUTR_STATE_ACTIVE)
      ret+=load_rcstring(ModuleInstance,STR_TU_STATE1);//" отправлена";
   if(entry->tu_state&GRSC_TUTR_STATE_HOLDING)
      ret+=load_rcstring(ModuleInstance,STR_TU_STATE2);//" удержание";
   if(entry->tu_state&GRSC_TUTR_STATE_WAITING)
      ret+=load_rcstring(ModuleInstance,STR_TU_STATE3);//" ожидание";
   }
   else
   {
   if(entry->tu_state&GRSC_TUTR_STATE_QUEUED)
      ret+=load_rcstring(ModuleInstance,STR_TU_STATE4);//" в очереди";
      else
   if(entry->tu_state&GRSC_TUTR_STATE_SELECTED)
      ret +=load_rcstring(ModuleInstance,STR_TU_STATE5);//" подготовка";
      else
      ret+=load_rcstring(ModuleInstance,STR_TU_STATE6);//" сброс";
   }

   return ret;
  }

  bool  __fastcall TSCCpDataFrame::add_tu_entry   (DWORD idx)
  {
   GRSC_TU_ENTRY tue;
   tue.dw_size   = sizeof(tue);
   tue.addr.addr = this->addr.addr;
   while(idx>(DWORD)TuList->Items->Count && add_tu_entry(TuList->Items->Count))
         ;

   if(mod_iface.call(SCCM_ENUM_TUENTRY,(LPARAM)&tue,idx)!= GKH_RET_ERROR)
   {
    AnsiString  str;
    TListItem * item = TuList->Items->Insert(idx);
    str.printf("ГР %02d %s-%02d",(DWORD)tue.addr.sb,load_rcstring(ModuleInstance,STR_FA_TS+(DWORD)tue.addr.fa) ,tue.object);
    item->Caption = str;
    MODEM_ADDR ma;
    ma.addr = tue.owner_addr;
    str.printf("%03d.%03d.%03d",(DWORD)ma.sta,(DWORD)ma.modem,(DWORD)ma.line);
    item->SubItems->Add(str);
    item->SubItems->Add(get_tu_text(&tue));
    return true;
   }
   return false;
  }

  void  __fastcall TSCCpDataFrame::remove_tu_entry(DWORD idx)
  {
   if(idx<(DWORD)TuList->Items->Count)
      TuList->Items->Delete(idx);
  }

  void  __fastcall TSCCpDataFrame::change_tu_entry(DWORD idx)
  {
   GRSC_TU_ENTRY tue;
   tue.dw_size   = sizeof(tue);
   tue.addr.addr = this->addr.addr;
   if(idx<(DWORD)TuList->Items->Count)
   {
   if(mod_iface.call(SCCM_ENUM_TUENTRY,(LPARAM)&tue,idx)!= GKH_RET_ERROR)
      TuList->Items->Item[idx]->SubItems->Strings[1] = get_tu_text(&tue);
   }
   else
    add_tu_entry(idx);

  }

void  __fastcall TSCCpDataFrame::update_dyn_data()
{
 SC_DYNAMIC_CP dyn_cp;
 dyn_cp.dw_size = sizeof(dyn_cp);
 if(mod_iface.call(SCCM_GET_DYNAMIC,this->addr.addr,(LPARAM)&dyn_cp) == GKH_RET_SUCCESS)
 {
  TxDeliverTime->Caption = dyn_cp.tx_deliver_time;
  TxByteTime->Caption    = dyn_cp.tx_byte_time;
  RxTimer->Caption       = dyn_cp.rx_timer/1000; 
 }
}



void __fastcall TSCCpDataFrame::ListView1SelectItem(TObject *Sender,
      TListItem *Item, bool Selected)
{
  //Смена

  AnsiString str;
  if(Item && Selected)
  {
  str = Item->SubItems->Strings[0];
  DWORD diag  = hextoi(str.c_str());
  str.Delete(1,str.Length());

   if(diag&OTD_DIAG_MASK)
     str += get_diag_text (diag,false);
   if(diag&OTD_DIAG_PART_MASK)
     str += get_part_diag_text(diag,false);
   if(diag&OTD_STATE_MASK)
    str += get_state_text(diag,false);
  }
  GrpPDDiag->Caption = str;
}
//---------------------------------------------------------------------------

