//---------------------------------------------------------------------------

#pragma warn -8058
#include <vcl.h>
#pragma warn .8058
#pragma hdrstop

#include "sc_grp_data_frame.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "sc_base_data_frame"
#pragma resource "*.dfm"
TSCGroupDataFrame *SCGroupDataFrame;
//---------------------------------------------------------------------------
__fastcall TSCGroupDataFrame::TSCGroupDataFrame(TComponent* Owner)
        : TSCBaseDataFrame(Owner)
{
}
//---------------------------------------------------------------------------

void  __fastcall TSCGroupDataFrame::set_active     (DWORD _addr)
{
 ObjectList->Items->Clear();
 addr.addr = _addr;
 if(addr.fa == OTD_FA_DISCRETE)
 {
  TuOnMore->Caption = "Вкл";
  TuOffLess->Caption = "Откл";
  CalcValueBox->Visible = false;
 }
 else
 {
  TuOnMore->Caption  = "Больше";
  TuOffLess->Caption = "Меньше";
  CalcValueBox->Visible = true;
 }
 TuOnMore->Enabled  = false;
 TuOffLess->Enabled = false;
 TrUndo->Enabled    = false;
 TSCBaseDataFrame::set_active(_addr);
}

void  __fastcall TSCGroupDataFrame::setup_data     (otd_proto & op)
{
 TSCBaseDataFrame::setup_data(op);
 ObjectList->Items->BeginUpdate();
 if(!ObjectList->Items->Count)
    prepare_object_list(op);
   if(op.data)
      this->setup_param(*op.data);
   if(op.personal_diag)
      setup_pdiag(*op.personal_diag);
   if(op.personal_chmask)
      setup_pcm(*op.personal_chmask);
 ObjectList->Items->EndUpdate();


}

void    __fastcall TSCGroupDataFrame::prepare_object_list(otd_proto & op)
{
 //Подготовка списка
 DWORD      lo_num = -1;
 DWORD      hi_num =  0;
 AnsiString str;
 if(op.data)
   {
     lo_num = KeRTL::MIN(lo_num,(DWORD)op.data->numbers.loN);
     hi_num = KeRTL::MAX(hi_num,(DWORD)op.data->numbers.hiN);
   }

 if(op.personal_diag)
   {
     lo_num = KeRTL::MIN(lo_num,(DWORD)op.personal_diag->numbers.loN);
     hi_num = KeRTL::MAX(hi_num,(DWORD)op.personal_diag->numbers.hiN);
   }
 if(op.personal_chmask)
   {
     lo_num = KeRTL::MIN(lo_num,(DWORD)op.personal_chmask->numbers.loN);
     hi_num = KeRTL::MAX(hi_num,(DWORD)op.personal_chmask->numbers.hiN);
   }


 for(;lo_num<=hi_num;lo_num++)
 {
   TListItem     * item = ObjectList->Items->Add();
   item->Data    =(LPVOID) lo_num;
   item->Caption = (int)lo_num;
   item->SubItems->Add(str);
   item->SubItems->Add(str);
   item->SubItems->Add(str);
 }
}

void        __fastcall TSCGroupDataFrame::setup_param( otd_data & od )
{
 TListItem * item = ObjectList->FindData(0,(LPVOID)od.numbers.loN,true,false);
 AnsiString str;
 while(item)
 {
  DWORD param = 0;
  if(!otd_get_value(&od,(DWORD)item->Data,&param,sizeof(param)))
  {
    str.printf("%03u",param);
    item->SubItems->Strings[0] = str;
    if(item->Selected && addr.fa)
       Kvants->Text  = str;
  }
  else break;
  item = ObjectList->Items->Item[item->Index+1];
 }

}


void        __fastcall TSCGroupDataFrame::setup_pdiag( otd_data & od )
{
 TListItem * item = ObjectList->FindData(0,(LPVOID)od.numbers.loN,true,false);
 AnsiString str;
 while(item)
 {
  DWORD param = 0;
  if(!otd_get_value(&od,(DWORD)item->Data,&param,sizeof(param)))
  {
    str.printf("%02X",param);
    item->SubItems->Strings[2] = str;
    if(item->Selected)
      PDiagText->Caption = get_pdiag_text(param);
  }
  else  break;
  item = ObjectList->Items->Item[item->Index+1];
 }

}

void        __fastcall TSCGroupDataFrame::setup_pcm  ( otd_data & od )
{
 TListItem * item = ObjectList->Items->Item[0];// FindData(0,(LPVOID)od.numbers.loN,true,false);
 AnsiString str;
 DWORD param = 0;
 while(item)
 {
  if(otd_data_inrange(&od,(DWORD)item->Data) )
  {
    otd_get_value(&od,(DWORD)item->Data,&param,sizeof(param));
  }
  else
   param = 0;
    str.printf("%d",param);
   item->SubItems->Strings[1] = str;

  item = ObjectList->Items->Item[item->Index+1];
 }
}



void  __fastcall TSCGroupDataFrame::update_dyn_data()
{
  SC_DYNAMIC_GRP dyn_grp;
  dyn_grp.dw_size = sizeof(dyn_grp);
  if(mod_iface.call(SCCM_GET_DYNAMIC,addr.addr,(LPARAM)&dyn_grp) == GKH_RET_SUCCESS)
  {
    if(dyn_grp.call_sended & GRPR_STATE_CALL_SENDED )
       {
        Label10->Caption = "Таймер ответа (сек):";
        Label10->Font->Style =  TFontStyles()<< fsUnderline;
       }
       else
       {
        Label10->Caption = "Таймер вызова (сек):";
        Label10->Font->Style = TFontStyles();
       }

    GrpTimerText->Caption  = dyn_grp.tm_counter;
    RespondTime->Caption   = dyn_grp.respond_time;
    Chdbc_counter->Caption = dyn_grp.chdbc_counter;
    Nchd_counter->Caption  = dyn_grp.nchd_counter;
    NoRespondCounter->Caption = dyn_grp.norespond_counter;

  }
}



void __fastcall TSCGroupDataFrame::KvantsChange(TObject *Sender)
{
 float beg     = _wtof(ScaleBegin->Text.c_str()) ;
 float end     = _wtof(ScaleEnd  ->Text.c_str());
 float curr    = _wtof(Kvants    ->Text.c_str());
 float result  = (addr.fa == 1) ? beg+curr*((end-beg)/250.0):curr;
 WideString str;
 str.printf(L"%.4f",result);
 Result->Caption = str;
}
//---------------------------------------------------------------------------
void __fastcall TSCGroupDataFrame::ObjectListSelectItem(TObject *Sender,
      TListItem *Item, bool Selected)
{
   if(Item && Selected)
   {
    DWORD pdiag = hextoi(Item->SubItems->Strings[2].c_str());
    ResetTuError->Enabled = true;//(pdiag&(OTD_PDIAG_TUTR_ERROR|OTD_PDIAG_TUTR_DESCRIPT)) ? true:false;

    PDiagText->Caption = get_pdiag_text(pdiag);
    if(Item->SubItems->Count)
       Kvants->Text = Item->SubItems->Strings[0];
       GRSC_TU_ENTRY tue;
       tue.dw_size = sizeof(tue);
       tue.addr.addr = addr.addr;
       tue.object    = (DWORD)Item->Data;

       if(mod_iface.call(SCCM_GET_TUENTRY,(LPARAM)&tue,0) == GKH_RET_SUCCESS)
       {
        TuOnMore ->Enabled = true;
        TuOffLess->Enabled = true;
        TrUndo->Enabled    = false;

       }
   }
   else
   {
        ResetTuError->Enabled = false;
        TuOnMore ->Enabled = false;
        TuOffLess->Enabled = false;
        TrUndo->Enabled    = false;
   }

}
//---------------------------------------------------------------------------

void __fastcall TSCGroupDataFrame::ScaleBeginChange(TObject *Sender)
{
  KvantsChange(this);
}
//---------------------------------------------------------------------------

void __fastcall TSCGroupDataFrame::TuOnMoreClick(TObject *Sender)
{
 SC_TEST_TUTR  tt;
 tt.dw_size    = sizeof(tt);
 tt.addr.addr  = addr.addr;
 tt.object     = (DWORD)ObjectList->Selected->Data;
 tt.tu_command = OTD_TUOP_ON;
 this->begin_call_gkhandle(_T("Тестовое ТУ- \'ВКЛ /+\' "));
 DWORD ret = mod_iface.call(SCCM_TEST_TUTR,(LPARAM)&tt,NULL);
 done_call_gkhandle(ret);
 if(GKH_RET_SUCCESS == ret)
 {
   bool undo_enabled  = addr.fa ? true:false;
   TrUndo->Enabled    = undo_enabled;
   TuOnMore->Enabled  = !undo_enabled;
   TuOffLess->Enabled = !undo_enabled;
 }
}
//---------------------------------------------------------------------------

void __fastcall TSCGroupDataFrame::TuOffLessClick(TObject *Sender)
{
 SC_TEST_TUTR  tt;
 tt.dw_size    = sizeof(tt);
 tt.addr.addr  = addr.addr;
 tt.object     = (DWORD)ObjectList->Selected->Data;
 tt.tu_command =  OTD_TUOP_OFF;
 TrUndo->Enabled = addr.fa ? true:false;
 this->begin_call_gkhandle(_T("Тестовое ТУ- \'ОТКЛ/+\' "));
 DWORD ret = mod_iface.call(SCCM_TEST_TUTR,(LPARAM)&tt,NULL);
 done_call_gkhandle(ret);
 if(GKH_RET_SUCCESS == ret)
 {
   bool undo_enabled  = addr.fa ? true:false;
   TrUndo->Enabled    = undo_enabled;
   TuOnMore->Enabled  = !undo_enabled;
   TuOffLess->Enabled = !undo_enabled;
 }
}
//---------------------------------------------------------------------------

void __fastcall TSCGroupDataFrame::TrUndoClick(TObject *Sender)
{
 SC_TEST_TUTR  tt;
 tt.dw_size    = sizeof(tt);
 tt.addr.addr  = addr.addr;
 tt.object     = (DWORD)ObjectList->Selected->Data;
 tt.tu_command =  OTD_TUTROP_CANCEL;
 this->begin_call_gkhandle(_T("Тестовое ТУ- \'ОТМЕНА\' "));
 DWORD ret = mod_iface.call(SCCM_TEST_TUTR,(LPARAM)&tt,NULL);
 done_call_gkhandle(ret);

 TrUndo->Enabled = false;
 TuOnMore->Enabled  = true;
 TuOffLess->Enabled = true;

}
//---------------------------------------------------------------------------

void __fastcall TSCGroupDataFrame::ObjectListChanging(TObject *Sender,
      TListItem *Item, TItemChange Change, bool &AllowChange)
{
  if(Change == ctState)
    {
      AllowChange = !TrUndo->Enabled;
    }
}
//---------------------------------------------------------------------------


void __fastcall TSCGroupDataFrame::ResetTuErrorClick(TObject *Sender)
{
 //Сброс ошибки ту
 mod_iface.call(SCCM_RESET_TU_ERROR,addr.addr,(DWORD)ObjectList->Selected->Data);
}
//---------------------------------------------------------------------------

