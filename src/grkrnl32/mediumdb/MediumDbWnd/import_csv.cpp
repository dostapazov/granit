//---------------------------------------------------------------------------

#pragma hdrstop
#include <vcl.h>

#include "import_csv.h"
#include <memory>
#include <Math.hpp>
#include "mediumdb_frm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TImportCSV *ImportCSV;
//---------------------------------------------------------------------------


__fastcall TImportCSV::TImportCSV(TComponent* Owner)
	: TForm(Owner)
{
  UnicodeString grid_caption = L"ID,ПУ,КП,Д/А,Группа,№-парам,sn_id,\"Cокр имя\",nm_id,\"Полное имя\",min,max,kvants,инв,\"% нечуств\",ту,Псевдо,стат,скрыт,неквит,норм,alarm,alarm_min,alarm_max,danger,danger_min,danger_max";
  ImpData->Rows[0]->CommaText = grid_caption;
  Ftree_node = NULL;
  int cw = 39;
  for(int i = 0;i<ImpData->ColCount;i++)
      ImpData->ColWidths[i]  = cw;
  cw = 150;
  ImpData->ColWidths[7] = cw;
  cw = 200;
  ImpData->ColWidths[9] = cw;
//  cw = 120;
//  ImpData->ColWidths[8] = cw;
  PageControl1->ActivePageIndex = 0;
}

void __fastcall TImportCSV::set_tree_node(TTreeNode * node)
{
  if(Ftree_node!=node)
  {
    Ftree_node=node;
    if(node && mod_iface.is_valid())
        {
          node->Owner->BeginUpdate();
          make_export_data(node);
          --ImpData->RowCount;
          node->Owner->EndUpdate();

        }
  }
}

void  __fastcall TImportCSV::set_gkhandle(GKHANDLE handle)
{
  if(!is_same_gkhandle(gkmodule,handle))
  {
   mod_iface.close();
   mod_iface.open(handle,true);
   if(Ftree_node)
      make_export_data(Ftree_node);
  }
}

void __fastcall  TImportCSV::rec2text(mdb_record & rec,TStrings * s)
{
   UnicodeString str;
  //L"ID,ПУ,КП,Д/А,Группа,№-парам,snm_id,Cокр имя,nm_id,Полное имя,min,max,kvants,инв,\"% нечуств\",ту,Псевдо,alarm,danger,стат,скрыт,неквит,норм";
  int kvants = rec.kvants;
  if(rec.is_signed_kvants()) kvants = -kvants;
  s->Clear();
  str.printf(L"%d",rec.rec_id);  s->Add(str);
  str.printf(L"%u",(unsigned)rec.addr.addr.pu);     s->Add(str);
  str.printf(L"%u",(unsigned)rec.addr.addr.cp);     s->Add(str);
  str.printf(L"%u",(unsigned)rec.addr.addr.fa);     s->Add(str);
  str.printf(L"%u",(unsigned)rec.addr.addr.sb);     s->Add(str);
  str.printf(L"%d",(int)rec.addr.param_number);     s->Add(str);
  if(rec.sname!=DWORD(-1))
  {
   str.printf(L"%u",rec.sname);     s->Add(str);
   s->Add(TMediumDBFrm::get_text(mod_iface,rec.sname));
  }
  else
  {
    s->Add(UnicodeString());
    s->Add(UnicodeString());
  }
  if(rec.sname!=DWORD(-1))
  {
   str.printf(L"%u",rec.name);     s->Add(str);
   s->Add(TMediumDBFrm::get_text(mod_iface,rec.name));
  }
  else
  {
    s->Add(UnicodeString());
    s->Add(UnicodeString());
  }
  str.printf(L"%02f",rec.min_value );s->Add(str);
  str.printf(L"%.2f",rec.max_value );s->Add(str);
  str.printf(L"%d"  ,kvants        );s->Add(str);
  str.printf(L"%d"  ,rec.options&MDBR_OPT_INVERSE_SCALE ? TRUE:FALSE);s->Add(str);
  str.printf(L"%.2f", float(rec.percent)/100.0);s->Add(str);
  str.printf(L"%d", (rec.state&MDBR_STATE_TUFLAG) ? TRUE:FALSE);s->Add(str);

  str.printf(L"%d", (rec.options&MDBR_OPT_PSEUDO) ? TRUE:FALSE);
  if((rec.options&MDBR_OPT_CAUTION)== MDBR_OPT_CAUTION)
      str.printf(L"%d",2);
  s->Add(str);

  str.printf(L"%d", (rec.state&MDBR_STATE_STATIC )    ? TRUE:FALSE);s->Add(str);
  str.printf(L"%d", (rec.state&MDBR_STATE_HIDDEN )    ? TRUE:FALSE);s->Add(str);
  str.printf(L"%d", (rec.state&MDBR_STATE_NORECEIPT)  ? TRUE:FALSE);s->Add(str);
  str.printf(L"%d", (rec.state&MDBR_STATE_NORMAL   )  ? TRUE:FALSE);s->Add(str);

  str.printf(L"%d", (rec.options&MDBR_OPT_DANGER )    ? TRUE:FALSE);s->Add(str);
  str.printf(L"%.2f", rec.min_danger);s->Add(str);
  str.printf(L"%.2f", rec.max_danger);s->Add(str);


  str.printf(L"%d", (rec.options&MDBR_OPT_ALARM  )    ? TRUE:FALSE);s->Add(str);
  str.printf(L"%.2f", rec.min_alarm);s->Add(str);
  str.printf(L"%.2f", rec.max_alarm);s->Add(str);

}

void __fastcall TImportCSV::make_export_data(TTreeNode * node)
{
  mdb_record rec;
  TMediumDBFrm::get_record(mod_iface,(DWORD)node->Data,rec);
  if(rec.rec_id != (DWORD)-1)
    {
     TStrings * s = ImpData->Rows[ImpData->RowCount-1];
     rec2text(rec,s);
     ImpData->RowCount++;
    }

   if(rec.addr.param_number == (DWORD)-1)
    {
     bool expanded = false;
     if(!node->Expanded)
     {
      node->Expand(false);
     }
     else
     expanded = true;

     for(int i = 0;i<node->Count;i++)
        make_export_data(node->Item[i]);
     if(!expanded) node->Collapse(true);
    }
}

//---------------------------------------------------------------------------
bool __fastcall TImportCSV::select_file(HWND wnd)
{
 bool ret = false;
   if(OpenDialog1->Execute(wnd))
   {
    std::auto_ptr<TStringList>  sl ( new TStringList);
    sl->LoadFromFile(OpenDialog1->FileName);
    if(sl->Count)
    {
      ImpData->RowCount = 1+sl->Count;
      for(int i = 0;i<sl->Count;)
      {
       UnicodeString str = sl->Strings[i];
       ++i;
       TStrings * s      = ImpData->Rows[i];
       s->CommaText      = str;
      }
     ret = true;
    }
   }
  return ret;
}


void __fastcall TImportCSV::bOkClick(TObject *Sender)
{
   if(bOk->ModalResult == mrNone)
    {
      if(this->Ftree_node)
      {
       if(SaveDialog1->Execute())
       {
         std::auto_ptr<TStringList>  sl ( new TStringList);
         for(int i = 1;i<ImpData->RowCount;i++)
            sl->Add(ImpData->Rows[i]->CommaText);

          sl->SaveToFile(SaveDialog1->FileName);
       }
       bOk->Caption     = "Закрыть";
       bOk->ModalResult = mrOk;
      }
      else
        do_import();
    }
}
//---------------------------------------------------------------------------

void __fastcall TImportCSV::do_import()
{
  addr.addr.addr    = -1;
  addr.param_number = -1;


  min_val = 0;
  max_val = 8191;
  kvants  = 8191;

  bCancel->Visible = false;
  Memo1->Lines->Clear();
  PageControl1->ActivePageIndex = 1;
  ProgressBar1->Min = 0;
  ProgressBar1->Max = ImpData->RowCount;
  ProgressBar1->Position = 0;
  ProgressBar1->Visible  = true;
  for(int i = 1;i<ImpData->RowCount;i++ )
  {
    ProgressBar1->StepIt();
    import_line(i);

  }

  bOk->Caption     = L"OK";
  bOk->ModalResult = mrOk;

}


enum class csv_indexes
{
 csv_id,csv_pu,csv_cp,csv_fa,csv_group,csv_param,csv_sname_id
,csv_sname,csv_name_id,csv_name,csv_minv,csv_maxv,csv_kvants,csv_inverse
,csv_percent,csv_tuflag,csv_pseudo,csv_static
,csv_hidden,csv_noreceipt,csv_normal
,csv_danger,csv_mindanger,csv_maxdanger
,csv_alarm,csv_minalarm,csv_maxalarm
};

bool __fastcall TImportCSV::get_otd_addr(otd_param_addr & addr,TStrings * s )
{
  bool ret = true;
  UnicodeString curr_str;
   curr_str = s->Strings[csv_indexes::csv_pu].Trim();
   if(!curr_str.IsEmpty())
      addr.addr.pu = curr_str.ToInt();
      else
      {
       if(addr.addr.pu == 0xFF){ Memo1->Lines->Add(L"Не задан номер ПУ"); ret = false;}
      }

   curr_str = s->Strings[csv_indexes::csv_cp].Trim();
   if(!curr_str.IsEmpty())
      addr.addr.cp = curr_str.ToInt();
      else
      {
       if(addr.addr.cp == 0xFF) {Memo1->Lines->Add(L"Не задан номер КП"); ret = false;}
      }

   curr_str = s->Strings[csv_indexes::csv_fa].Trim();
   if(!curr_str.IsEmpty())
      {
       addr.addr.fa = curr_str.ToInt();
      }

   if(addr.addr.fa>1 && addr.addr.fa != OTD_ADDR_MAXVALUE)
    {
      Memo1->Lines->Add(L"Не корректно задан тип группы"); ret = false;
    }

    curr_str = s->Strings[csv_indexes::csv_group].Trim();
    if(!curr_str.IsEmpty())
    {
     addr.addr.sb = curr_str.ToInt();
    }
    else
    {
      Memo1->Lines->Add(L"Не задан номер группы"); ret = false;
    }

    curr_str = s->Strings[csv_indexes::csv_param].Trim();
    if(!curr_str.IsEmpty())
       {
        addr.param_number = curr_str.ToInt();
       }
       else
       {
        Memo1->Lines->Add(L"Не задан номер параметра"); ret = false;
       }


  return ret;
}

void __fastcall TImportCSV::get_scale_values(BYTE fa,TStrings * s,float & min_value,float & max_value,DWORD &kvants,DWORD & options,DWORD & percent)
{
   UnicodeString curr_str;
   if(fa == OTD_FA_DISCRETE)
   {
      min_value = 0.0;
      max_value = 1.0;
      kvants    = 1;

   }
   else
   {
     curr_str = s->Strings[csv_indexes::csv_inverse].Trim();
     if(_wtoi(curr_str.c_str()))
      options|= MDBR_OPT_INVERSE_SCALE;
      else
      options&=~ MDBR_OPT_INVERSE_SCALE;
     curr_str  = s->Strings[csv_indexes::csv_minv].Trim();
     min_value = _wtof(curr_str.c_str());

     curr_str  = s->Strings[csv_indexes::csv_maxv].Trim();
     if(curr_str.IsEmpty())
     {
      if(IsZero(max_value))
         max_value = kvants;
     }
      else
      max_value = _wtof(curr_str.c_str());
      curr_str  = s->Strings[csv_indexes::csv_percent].Trim();
      percent   = 100.0*_wtof(curr_str.c_str());
   }

     curr_str   = s->Strings[csv_indexes::csv_kvants].Trim();
     int _kvants = _wtoi(curr_str.c_str());
     if(!_kvants) kvants = 8191;
     if(_kvants<0)
        {
          options|= MDBR_OPT_SIGNED_KVANTS;
          max_value = fabs(max_value);
          min_value = -max_value;


        }
        else
        options&= ~MDBR_OPT_SIGNED_KVANTS;

     kvants = (DWORD)abs(_kvants);

}

DWORD __fastcall TImportCSV::get_name_id(TStrings * s,bool long_name)
{
  UnicodeString str = s->Strings[long_name ? csv_indexes::csv_name_id : csv_indexes::csv_sname_id].Trim();
  if(str.IsEmpty()) return -1;
  return _wtoi(str.c_str());
}

bool __fastcall TImportCSV::text2rec        (mdb_record & rec,UnicodeString & long_name,UnicodeString & short_name,TStrings * s)
{
  bool ret = false;
  if(s)
  {
   rec.state   = 0;
   rec.options = 0;

   UnicodeString str = s->Strings[csv_indexes::csv_id].Trim();
   if(str.Length())
      rec.rec_id = str.ToInt();
      else
      rec.rec_id = -1;
    ret = get_otd_addr(rec.addr,s);
    if(ret)
    {
      if(rec.addr.addr.fa == OTD_FA_DISCRETE)
          rec.state     |=  MDBR_STATE_DISCRETE;
          else
          rec.state  &=    ~MDBR_STATE_DISCRETE;
    }

    if(_wtoi(s->Strings[csv_indexes::csv_noreceipt ].c_str())) rec.state |= MDBR_STATE_NORECEIPT;
    if(_wtoi(s->Strings[csv_indexes::csv_normal    ].c_str())) rec.state |= (MDBR_STATE_DYN_NORMAL| MDBR_STATE_NORMAL);
    if(_wtoi(s->Strings[csv_indexes::csv_tuflag    ].c_str())) rec.state |= MDBR_STATE_TUFLAG ;
    if(_wtoi(s->Strings[csv_indexes::csv_static    ].c_str())) rec.state |= MDBR_STATE_STATIC ;
    if(_wtoi(s->Strings[csv_indexes::csv_hidden    ].c_str())) rec.state |= MDBR_STATE_HIDDEN ;

    get_scale_values(rec.addr.addr.fa,s,rec.min_value,rec.max_value,rec.kvants,rec.options,rec.percent);

    switch(_wtoi(s->Strings[csv_indexes::csv_pseudo].c_str()))
    {
      case 1: rec.options|= MDBR_OPT_PSEUDO ;break;
      case 2: rec.options|= MDBR_OPT_CAUTION;break;
    }

    if(_wtoi(s->Strings[csv_indexes::csv_danger   ].c_str())) rec.options |= MDBR_OPT_DANGER;
    rec.min_danger = _wtof(s->Strings[csv_indexes::csv_mindanger].c_str());
    rec.max_danger = _wtof(s->Strings[csv_indexes::csv_maxdanger].c_str());

    if(_wtoi(s->Strings[csv_indexes::csv_alarm    ].c_str())) rec.options |= MDBR_OPT_ALARM ;
    rec.min_alarm = _wtof(s->Strings[csv_indexes::csv_minalarm].c_str());
    rec.max_alarm = _wtof(s->Strings[csv_indexes::csv_maxalarm].c_str());

    rec.sname  =  get_name_id (s,false);
    rec.name   =  get_name_id (s,true );
    long_name  =  s->Strings[csv_indexes::csv_name].Trim();
    short_name =  s->Strings[csv_indexes::csv_sname].Trim();
  }
  return ret;
}


void __fastcall TImportCSV::import_line(int lno)
{
   UnicodeString mon_text;
   mon_text.printf(L"Обработка строки №%d",lno);
   Memo1->Lines->Add(mon_text);
   ImpData->Row = lno;
   TStrings * s = ImpData->Rows[lno];
   Memo1->Lines->Add(s->CommaText);
   mdb_record rec;
   UnicodeString long_name,short_name;

 if(text2rec(rec,long_name,short_name,s))
  {
   if(rbAddRecords->Checked || mod_iface.call(MDBCM_RECORD_GET,rec.rec_id,(LPARAM) &rec ) == GKH_RET_ERROR)
    {
       //Такая запись не найдена
        rec.rec_id = -1;
        rec.sname  = rec.name = -1;
        rec.rec_id = mod_iface.call(MDBCM_RECORD_CREATE,(LPARAM)&rec,0);
        mon_text.printf(L"Добавлена запись ID %u",rec.rec_id);
        Memo1->Lines->Add(mon_text);
    }
    else
    {
     //Обновление записи
     if(GKH_RET_ERROR != mod_iface.call(MDBCM_RECORD_UPDATE,(LPARAM)&rec,MDBR_FIELD_MIN_VALUE|MDBR_FIELD_MIN_VALUE|MDBR_FIELD_KVANTS|MDBR_FIELD_OPTIONS|MDBR_FIELD_ADDR|MDBR_FIELD_PERCENT))
     {
       mon_text.printf(L"Обновлена запись ID %u",rec.rec_id);
       Memo1->Lines->Add(mon_text);
     }
    }

    if(rec.rec_id!=(DWORD)GKH_RET_ERROR)
    {

      BYTE buff[4096];
      LPMDB_RECORD_TEXT rt = (LPMDB_RECORD_TEXT)buff;
      rt->dw_size = sizeof(*rt);
      rt->text_len = rt->text_size = swprintf(rt->text,(sizeof(buff)-sizeof(*rt))/sizeof(wchar_t),L"%s",long_name.c_str());
      mod_iface.call(MDBCM_RECORD_SET_NAME,rec.rec_id,(LPARAM)rt);
      rt->text_len = rt->text_size = swprintf(rt->text,(sizeof(buff)-sizeof(*rt))/sizeof(wchar_t),L"%s",short_name.c_str());
      mod_iface.call(MDBCM_RECORD_SET_SNAME,rec.rec_id,(LPARAM)rt);
      mon_text.printf(L"Установлены имена записи ID %u",rec.rec_id);
      Memo1->Lines->Add(mon_text);

    }
   }
}


DWORD              __fastcall TImportCSV::set_text(DWORD str_id,WideString str)
{
  TMediumDBFrm::set_text(mod_iface,str_id,str);
  return str_id;
//  wchar_t buffer[2048];
//  LPMDB_RECORD_TEXT rt = (LPMDB_RECORD_TEXT)buffer;
//  rt->dw_size   = sizeof(buffer);
//  rt->text_size =  rt->text_len  =  str.Length()+1;
//  rt->text[0]   = 0;
//  safe_strcpy(rt->text,str.c_bstr());
//  str_id =  (DWORD)mod_iface.call(MDBCM_UPDATE_TEXT,str_id,(LPARAM)rt);
}




