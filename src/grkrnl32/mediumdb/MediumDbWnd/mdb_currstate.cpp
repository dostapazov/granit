//---------------------------------------------------------------------------
#pragma hdrstop
#include <vcl.h>

#include "mdb_currstate.h"
#include <stdio.h>
#include <math.h>
#include "mediumdb_frm.h"


//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "gkmodule_form"
#pragma resource "*.dfm"
TMDBCurrentState *MDBCurrentState;
//---------------------------------------------------------------------------
__fastcall TMDBCurrentState::TMDBCurrentState(TComponent* Owner,GKHANDLE handle)
        : TGKModuleForm(Owner,NULL,handle)
{
  only_combined = false;
  this->Ffilter_addr.addr = 0; 
}
//---------------------------------------------------------------------------
void __fastcall TMDBCurrentState::FormClose(TObject *Sender,
      TCloseAction &Action)
{
   Action = caFree;
   MDBCurrentState = NULL;
}
//---------------------------------------------------------------------------

void    __fastcall TMDBCurrentState::set_filter_addr(otd_addr  addr)
{
  if(Ffilter_addr.addr!=addr.addr)
  {
    Ffilter_addr.addr = addr.addr;
    setup_list_view(true);
  }
}


void     __fastcall TMDBCurrentState::on_module_config_change(GKHANDLE mod,LPMODULE_CONFIG_DATA mcd)
{

}

void     __fastcall TMDBCurrentState::on_module_state(GKHANDLE mod,DWORD state)
{
}

void     __fastcall TMDBCurrentState::after_set_gkhandle()
{
 TGKModuleForm::after_set_gkhandle();
 setup_list_view(false);
 set_notify(mod_iface(),MDB_NOTIFY_MASK_RECORD);
 //reset_notify(mod_iface(),MDB_NOTIFY_MASK_RECORD);
}

LRESULT  __fastcall TMDBCurrentState::on_gkhandle_notify (GKHANDLE from,LPNOTIFY_CODE nc,DWORD mask)
{
 if(mask == MDB_NOTIFY_MASK_RECORD)
 {
  switch(nc->notify_code)
  {
   case
   MDB_NOTIFY_CODE_RECORD_REMOVED :
   on_record_removed(nc->notify_data[0]);
   break;
   case
   MDB_NOTIFY_CODE_RECORD_CREATED :
   on_record_created(nc->notify_data[0]);
   break;
   case
   MDB_NOTIFY_CODE_RECORD_CHANGED :
      {
       LPMDB_RECORD_CHANGED rc =(LPMDB_RECORD_CHANGED)nc->notify_data;
       on_record_changed(rc->rec_id,rc->mask);
      }
   break;

  }
 }
 return GKH_RET_SUCCESS;
}


void        __fastcall setup_time(UnicodeString & result,DWORD sz,__int64 & time)
{

      TCHAR date_time[MAX_PATH];
      SYSTEMTIME st;
      FileTimeToSystemTime((LPFILETIME)&time,&st);
      int dtl = GetDateFormat(LOCALE_USER_DEFAULT,DATE_SHORTDATE,&st,NULL,date_time,KERTL_ARRAY_COUNT(date_time));
      dtl--;
      *(date_time+dtl) = ' ';
      dtl++;
      dtl+=GetTimeFormat(LOCALE_USER_DEFAULT,0,&st,_T("HH':'mm':'ss"),date_time+dtl,KERTL_ARRAY_COUNT(date_time)-dtl);
      dtl--;
      wsprintf(date_time+dtl,_T(",%03d"),(DWORD)st.wMilliseconds);
      result = date_time;
}


void     __fastcall TMDBCurrentState::setup_list_item(TListItem * item,mdb_record & rec,DWORD mask)
{
  UnicodeString  str;
  wchar_t buffer[2048];
  LPMDB_RECORD_TEXT rt = (LPMDB_RECORD_TEXT)buffer;
  rt->dw_size  = sizeof(buffer);
  rt->text_size = (sizeof(buffer)-sizeof(*rt))/sizeof(wchar_t);
  rt->text_len  = 0;
  rt->text[0]   = 0;

  if(mask&MDBR_FIELD_ID)
   item->Caption = rec.rec_id;
  if(mask&MDBR_FIELD_VALUE)
   {
    if(rec.state& MDBR_STATE_DISCRETE)
    str.printf(_T("%d"),(DWORD)rec.value);
    else
    str.printf(_T("%.2f"),rec.value);
    item->SubItems->Strings[0] = str;
   }

 if(mask & (MDBR_FIELD_DIAG|MDBR_FIELD_OTDPD))
  {
   str.printf(_T("%02X [pd-%02X]"),(DWORD)rec.diag,rec.otd_pd) ;
   item->SubItems->Strings[1] = str;
  }

  if(mask & MDBR_FIELD_STATE)
  {
   str.printf(_T("%04X"),rec.state) ;
   item->SubItems->Strings[2] = str;
  }

  if((mask & MDBR_FIELD_TIME) && rec.time)
  {
   setup_time(str,sizeof(str),rec.time);
   rt->text[rt->text_len]=0;
   item->SubItems->Strings[3]  = str;
  }

  if(mask & (MDBR_FIELD_SNAME|MDBR_FIELD_NAME))
  {
   mod_iface.call(rec.name == (DWORD)-1 ? MDBCM_RECORD_GET_SNAME:MDBCM_RECORD_GET_NAME,rec.rec_id,(LPARAM)rt);
   rt->text[rt->text_len]=0;
   item->SubItems->Strings[4]  = rt->text;
  }

}

void     __fastcall TMDBCurrentState::setup_list_view(bool by_addr)
{
 RecordsList->Items->BeginUpdate();
 RecordsList->Clear();
 mdb_record rec;
 DWORD idx = 0;
 AnsiString str;
 DWORD cmd = this->only_combined ? MDBCM_COMBINE_ENUM : (by_addr? MDBCM_RECORD_ENUM_BYADDR:MDBCM_RECORD_ENUM_BYID);
 while(mod_iface.call(cmd,idx,(LPARAM)&rec)!=GKH_RET_ERROR)
 {
  if(only_combined || otd_addr_include (&rec.addr.addr,&Ffilter_addr))
  {
   TListItem * item = RecordsList->Items->Add();
   item->Data = (LPVOID)rec.rec_id;
   item->SubItems->Add(str);
   item->SubItems->Add(str);
   item->SubItems->Add(str);
   item->SubItems->Add(str);
   item->SubItems->Add(str);
   setup_list_item(item,rec,-1);
  }
  ++idx;
 }
 if(RecordsList->Items->Count)
    RecordsList->Items->Item[0]->Selected = true;
 RecordsList->Items->EndUpdate();

}

void     __fastcall TMDBCurrentState::on_record_changed  (DWORD id,DWORD mask)
{
  mdb_record rec;
  if(mod_iface.call(MDBCM_RECORD_GET,id,(LPARAM)&rec)==GKH_RET_SUCCESS)
  {
  TListItem * item = RecordsList->FindData(0,(LPVOID)id,true,false);
  if(item)
  {
    setup_list_item(item,rec,mask);
    if(item->Selected)
      {
       setup_additional(rec,mask);
      }

  }
 }
}

void     __fastcall TMDBCurrentState::on_record_created  (DWORD id)
{
  mdb_record rec;
  if(mod_iface.call(MDBCM_RECORD_GET,id,(LPARAM)&rec)==GKH_RET_SUCCESS)
  {
  TListItem * item = RecordsList->FindData(0,(LPVOID)id,true,false);
  if(!item)
      {
       item = RecordsList->Items->Add();
       item->Data = (LPVOID)id;
       AnsiString str;
       item->SubItems->Add(str);
       item->SubItems->Add(str);
       item->SubItems->Add(str);
       item->SubItems->Add(str);
       item->SubItems->Add(str);
      }
  if(item)
    setup_list_item(item,rec,-1);

  }
}

void     __fastcall TMDBCurrentState::on_record_removed  (DWORD id)
{
  TListItem * item = RecordsList->FindData(0,(LPVOID)id,true,false);
  if(item)
    item->Delete();
}



void __fastcall TMDBCurrentState::RecordsListColumnClick(TObject *Sender,
      TListColumn *Column)
{
  switch(Column->Index)
  {
   case 0 :  setup_list_view(false);break;
   case 1 :  setup_list_view(true);break;
  }
}
//---------------------------------------------------------------------------

 float    __fastcall  calc_change_percent(mdb_record & rec)
 {
  float ret = 0;
  float dif = fabs(rec.value - rec.last_value);
  if(dif> 0.0001)
  {
  if(rec.last_value > 0.0001)
   {
     ret = fabs(dif/rec.last_value)*100.0;
   }
   else
    ret = 100.0;
  }
  return ret ;
 }

void     __fastcall TMDBCurrentState::setup_additional(mdb_record & rec,DWORD mask)
{
  UnicodeString str;
  if(mask&MDBR_FIELD_VALUE)
    {
     str.printf(L"%.2f",rec.last_value);
     LastValue->Caption = str;
     lldiv_t dt = div(NS100_MSEC(rec.time - rec.last_time),__int64(1000));
     str.printf(L"%Ld c %003Ld ms",dt.quot,dt.rem);
     TimeDiff->Caption = str;
     float percent =  calc_change_percent(rec);
     str.printf(L"%.3f %%",percent);
     Percent->Caption = str;
    }
  if(mask&MDBR_FIELD_ADDR)
  {
  str.printf(L"%03d.%03d.%03d.%03d ",(DWORD)rec.addr.addr.pu,(DWORD)rec.addr.addr.cp,(DWORD)rec.addr.addr.fa,(DWORD)rec.addr.addr.sb);
  if(rec.addr.param_number!= (DWORD)-1)
    str.cat_printf(L"¹ %03d",rec.addr.param_number);
  OtdAddr->Caption = str;
  }

  if(mask & MDBR_FIELD_OTDVALUE)
  {
   if(rec.options & MDBR_OPT_DOUBLE_TYPE)
     str.printf(L"%.2lf (double)",rec.otd_val_dbl) ;
     else
     {
      if(rec.options & MDBR_OPT_FLOAT_TYPE)
       str.printf(L"%.2f (float)",rec.otd_val_flt) ;
       else
       str.printf(L"%u",rec.otd_val) ;
     }


   OtdValue->Caption = str;
  }

  if(mask & MDBR_FIELD_OTDPD)
  {
   str.printf(L"%X",rec.otd_pd) ;
   OtdPd->Caption = str;
  }

  if(mask & MDBR_FIELD_NAME)
  {
   wchar_t buffer[2048];
   LPMDB_RECORD_TEXT rt = (LPMDB_RECORD_TEXT)buffer;
   rt->dw_size  = sizeof(buffer);
   rt->text_len  = 0;
   rt->text_size = (sizeof(buffer)-sizeof(*rt))/sizeof(wchar_t);
   rt->text[0]   = 0;
   mod_iface.call(MDBCM_RECORD_GET_NAME,rec.rec_id,(LPARAM)rt);
   rt->text[rt->text_len]=0;
   FullName->Caption = rt->text;
  }
  if(mask &(MDBR_FIELD_MIN_VALUE|MDBR_FIELD_MAX_VALUE))
  {
   str.printf(L"%.2f .. %.2f",rec.min_value,rec.max_value);
   MinMax->Caption = str;
  }

}

void __fastcall TMDBCurrentState::on_select_record(TListItem *Item, bool Selected)
{
  mdb_record rec;
  if(Selected)
  {
   mod_iface.call(MDBCM_RECORD_GET,(LPARAM)Item->Data,(LPARAM)&rec);
//   TMediumDBFrm * frm = dynamic_cast<TMediumDBFrm *>(Owner);
//   if(frm  && this->RecordsList->Focused())
//      frm->select_node_by_id((DWORD)Item->Data);
  }
  setup_additional(rec,-1);
}

void __fastcall TMDBCurrentState::RecordsListSelectItem(TObject *Sender,
      TListItem *Item, bool Selected)
{
  on_select_record(Item,Selected);
}
//---------------------------------------------------------------------------

bool    __fastcall TMDBCurrentState::select_item_byid(DWORD rec_id)
{

 TListItem * item = RecordsList->Selected;
 if(!item || rec_id != (DWORD)item->Data  )
 {
     item = RecordsList->FindData(0,(LPVOID)rec_id,true,true);
     if(item)
     {
      item->MakeVisible(false);
      item->Selected = true;
      return true;
     }
 }
 return false;
}



void __fastcall TMDBCurrentState::RecordsListMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
   if(Button == mbLeft)
     {
      TMediumDBFrm * frm = dynamic_cast<TMediumDBFrm *>(Owner);
      if(frm )
         {
          if(RecordsList->Selected)
          {
          frm->DragRecordId =    (DWORD)RecordsList->Selected->Data;
          RecordsList->BeginDrag(false,16);
          }
          else
          frm->DragRecordId = DWORD (-1);
         }
     }
}
//---------------------------------------------------------------------------


