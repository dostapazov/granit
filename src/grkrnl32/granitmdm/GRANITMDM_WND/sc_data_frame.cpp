//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "sc_data_frame.h"
#include "grscfrm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "sc_base_frame"
#pragma resource "*.dfm"
TSCDataFrame *SCDataFrame;
//---------------------------------------------------------------------------
__fastcall TSCDataFrame::TSCDataFrame(TComponent* Owner)
        : TSCBaseFrame(Owner)
{
}
//---------------------------------------------------------------------------

void  __fastcall TSCDataFrame::set_active   (DWORD _addr)
{
  this->addr.addr = _addr;
  bool upd = update_data(0,-1);
  if(upd)
    TSCBaseFrame::set_active(_addr);
    changes_mask = 0;
  Timer1->Enabled = true;
}

void  __fastcall TSCDataFrame::set_inactive()
{
    Timer1->Enabled = false;
    dyn_timer->Visible = false;
    TSCBaseFrame::set_inactive();

}


bool  __fastcall TSCDataFrame::update_data  (WORD lo,WORD hi)
{

 BYTE buf[2048];

 LPSC_OTDDATA scd = (LPSC_OTDDATA)buf;
 scd->dw_size = sizeof(buf);
 scd->parts   = -1;
 scd->lo_num  = KeRTL::MIN(lo,hi);
 scd->hi_num  = KeRTL::MAX(lo,hi);
 scd->data_size  = sizeof(buf)-sizeof(*scd)+sizeof(BYTE);
 scd->data_len   = 0;

 if(mod_iface.call(SCCM_GET_OTDDATA,addr.addr,(LPARAM)scd)==GKH_RET_SUCCESS)
 {
   return setup_data(*scd, (lo == 0 && hi == (WORD)-1) ? false:true);
 }
 return false;
}

//Установка данных


void    __fastcall TSCDataFrame::prepare_list_view(otd_proto & op)
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

 ListView1->Items->Clear();
 for(;lo_num<=hi_num;lo_num++)
 {
   TListItem     * item = ListView1->Items->Add();
   item->Data    =(LPVOID) lo_num;
   item->Caption = (int)lo_num;
   item->SubItems->Add(str);
   item->SubItems->Add(str);
   item->SubItems->Add(str);
 }
}

void        __fastcall TSCDataFrame::setup_param( otd_data & od )
{
 TListItem * item = ListView1->FindData(0,(LPVOID)od.numbers.loN,true,false);
 AnsiString str;
 while(item)
 {
  DWORD param = 0;
  if(!otd_get_value(&od,(DWORD)item->Data,&param,sizeof(param)))
  {
    str.printf("%04u",param);
    item->SubItems->Strings[0] = str;
  }
  else break;
  item = ListView1->Items->Item[item->Index+1];
 }
  ListView1->Visible = true;

}


void        __fastcall TSCDataFrame::setup_pdiag( otd_data & od )
{
 TListItem * item = ListView1->FindData(0,(LPVOID)od.numbers.loN,true,false);
 AnsiString str;
 while(item)
 {
  DWORD param = 0;
  if(!otd_get_value(&od,(DWORD)item->Data,&param,sizeof(param)))
  {
    str.printf("%04X",param);
    item->SubItems->Strings[1] = str;
  }
  else  break;
  item = ListView1->Items->Item[item->Index+1];
 }
   ListView1->Visible = true;

}

void        __fastcall TSCDataFrame::setup_pcm  ( otd_data & od )
{
 TListItem * item = ListView1->Items->Item[0];// FindData(0,(LPVOID)od.numbers.loN,true,false);
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
   item->SubItems->Strings[2] = str;

  item = ListView1->Items->Item[item->Index+1];
 }
 ListView1->Visible = true;
}


bool    __fastcall TSCDataFrame::setup_data(SC_OTDDATA & scod,bool upd)
{
 otd_proto op;
 op.dw_size = sizeof(op);
 DWORD real_sz;
 AnsiString  str;
 op.dw_size = sizeof(op);
 if(otd_proto_parser(scod.data,scod.data_len,&op,&real_sz))
 {
   DWORD dw_diag = op.diag ? *op.diag : 0;
   Diag->Caption = get_diag_text(dw_diag);

   __int64 time_stamp = op.time_stamp ? (*(__int64*)op.time_stamp):0;
   if(time_stamp)
   {
    SYSTEMTIME st;
    FileTimeToSystemTime((LPFILETIME)&time_stamp,&st);
    char date_time[MAX_PATH];
    int dtl =
    GetDateFormat(LOCALE_USER_DEFAULT,DATE_SHORTDATE,&st,NULL,date_time,sizeof(date_time));
    dtl = lstrlen(date_time);
    *(date_time+dtl++) = ' ';
    dtl+=GetTimeFormat(LOCALE_USER_DEFAULT,0,&st,"HH':'mm':'ss",date_time+dtl,sizeof(date_time)-dtl);
    dtl = lstrlen(date_time);
    wsprintf(date_time+dtl,":%02d",(DWORD)st.wMilliseconds);
    TimeStamp->Caption = date_time;
    TimeStamp->Visible = true;
    Label6->Visible    = true;
   }
   else
   {
    TimeStamp->Visible = false;
    Label6->Visible    = false;
   }

   ListView1->Items->BeginUpdate();
   if(!upd)
   {
      TuOnMore ->Enabled  = false;
      TuOffLess->Enabled  = false;
      TrUndo   ->Enabled  = false;

      ListView1->Visible  = false;
      GroupBox2->Visible  = false;
      TuOnMore ->Visible  = false;
      TuOffLess->Visible  = false;
      TrUndo   ->Visible  = false;


   if(op.addr->fa == OTD_FA_DISCRETE)
     {

      TuOnMore->Caption  = "Вкл.";
      TuOffLess->Caption = "Откл.";
      TuOnMore ->Visible = true;
      TuOffLess->Visible = true;
     }

   if(op.addr->fa == OTD_FA_ANALOG)
     {
      GroupBox2->Visible = true;
      TuOnMore->Caption  = "Больше";
      TuOffLess->Caption = "Меньше";
      TuOnMore->Visible  = true;
      TuOffLess->Visible = true;
      TrUndo->Visible    = true;
     }

       prepare_list_view(op);
   }


   if(op.data)
      this->setup_param(*op.data);
   if(op.personal_diag)
      setup_pdiag(*op.personal_diag);
   if(op.personal_chmask)
      setup_pcm(*op.personal_chmask);
   ListView1->Items->EndUpdate();


   return true;
  }
 return false;
}

AnsiString  __fastcall TSCDataFrame::get_diag_text(DWORD dw_diag)
{
 AnsiString ret_str;
 ret_str.printf("%08X ",dw_diag);
 DWORD mask = 0x80000000;
 for(int i = 0;i<32;i++)
 {
  if(dw_diag&mask)
  {
   ret_str+=' ';
   ret_str+=ret_str.LoadStr(ModuleInstance,STR_DIAG_BEGIN+i);
  }
  mask>>=1;
 }
 return ret_str;
}

void __fastcall TSCDataFrame::KvantsChange(TObject *Sender)
{
 float beg     = atof(ScaleBegin->Text.c_str()) ;
 float end     = atof(ScaleEnd  ->Text.c_str());
 float curr    = atof(Kvants    ->Text.c_str());
 float result  = (addr.fa == 1) ? beg+curr*((end-beg)/250.0):curr;
 AnsiString str;
 str.printf("%.4f",result);
 Result->Caption = str;

}
//---------------------------------------------------------------------------

void __fastcall TSCDataFrame::ListView1Change(TObject *Sender,
      TListItem *Item, TItemChange Change)
{
   if(ListView1->Selected == Item)
   {
    if(Item->SubItems->Count)
       Kvants->Text = Item->SubItems->Strings[0];
       TuOnMore ->Enabled = true;
       TuOffLess->Enabled = true;
   }
}
//---------------------------------------------------------------------------

void __fastcall TSCDataFrame::TuOnMoreClick(TObject *Sender)
{
 SC_TEST_TUTR  tt;
 tt.dw_size    = sizeof(tt);
 tt.addr.addr  = addr.addr;
 tt.object     = (DWORD)ListView1->Selected->Data;
 tt.tu_command = addr.fa ?  OTD_TRCMD_MORE:OTD_TUCMD_ON;
 TrUndo->Enabled = true;
 mod_iface.call(SCCM_TEST_TUTR,(LPARAM)&tt,NULL);
}
//---------------------------------------------------------------------------

void __fastcall TSCDataFrame::TuOffLessClick(TObject *Sender)
{
 SC_TEST_TUTR  tt;
 tt.dw_size    = sizeof(tt);
 tt.addr.addr  = addr.addr;
 tt.object     = (DWORD)ListView1->Selected->Data;
 tt.tu_command =  addr.fa ?  OTD_TRCMD_LESS:OTD_TUCMD_OFF;
 TrUndo->Enabled = true;
 mod_iface.call(SCCM_TEST_TUTR,(LPARAM)&tt,NULL);
}
//---------------------------------------------------------------------------

void __fastcall TSCDataFrame::TrUndoClick(TObject *Sender)
{
 SC_TEST_TUTR  tt;
 tt.dw_size    = sizeof(tt);
 tt.addr.addr  = addr.addr;
 tt.object     = (DWORD)ListView1->Selected->Data;
 tt.tu_command =  OTD_TRCMD_CANCEL;
 mod_iface.call(SCCM_TEST_TUTR,(LPARAM)&tt,NULL);

}
//---------------------------------------------------------------------------

void __fastcall TSCDataFrame::Timer1Timer(TObject *Sender)
{
  SC_DYNAMIC_GRP dyn_grp;
  dyn_grp.dw_size = sizeof(dyn_grp);
  if(mod_iface.call(SCCM_GET_DYNAMIC,addr.addr,(LPARAM)&dyn_grp) == GKH_RET_SUCCESS)
  {
    if(dyn_grp.state & GRPR_STATE_CALL_SENDED )
       Label7->Caption = "Таймер ответа";
       else
       Label7->Caption = "Таймер вызова";
    dyn_timer->Caption = dyn_grp.tm_counter;
    dyn_timer->Visible = true;
  }
}
//---------------------------------------------------------------------------

