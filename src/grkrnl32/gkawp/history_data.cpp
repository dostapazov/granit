//---------------------------------------------------------------------------
#pragma hdrstop
#include <vcl.h>
#include "history_data.h"
#include "sel_period.h"
#include "comtrade_param.h"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <VCLTee.Series.hpp>
#include <VCLTee.TeeSpline.hpp>
#include <Math.hpp>


//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "tchart_frame"
#pragma resource "*.dfm"
THistoryForm *HistoryForm;

//---------------------------------------------------------------------------
__fastcall THistoryForm::THistoryForm(TComponent* Owner)
	: TForm(Owner)
{
}
__fastcall THistoryForm::THistoryForm(TComponent* Owner,TGkAwpModule* mod,UnicodeString _kadr_text,UnicodeString _period_text)
	: TForm(Owner)
        ,module(mod)
{
 kadr_text     = _kadr_text;
 period_text   = _period_text;
 Caption       = kadr_text+UnicodeString(" ")+ period_text;
 Fon_get_text   = NULL;
 Fon_get_record = NULL;
 timer_count =0;
 Frequest_proc = NULL;
 start_time = stop_time = 0;
 temp_file_stream  = NULL;
 temp_data_storage = NULL;
 PageControl1->ActivePageIndex = 0;
 bzero(&rdy_colors,sizeof(rdy_colors));
 HistView->Color = (TColor)get_ready_colors().bkgnd_stable;
 chart_frame->module = module;
}
//---------------------------------------------------------------------------

READY_COLORS&  __fastcall THistoryForm::get_ready_colors()
{
  if(!rdy_colors.dw_size)
      module->get_ready_colors(rdy_colors);
  return rdy_colors;
}


 void __fastcall THistoryForm::WndProc(Winapi::Messages::TMessage &Message)
 {
   if(Message.Msg == HDCM_HANDLE_ARCHIVE)
   {
    LPVOID ptr = (LPVOID)Message.LParam;
    handle_archive((LPOTDM_ARCHIVE_RECORDS_RESPOND)ptr ,Message.WParam );
    Message.Result = 0;
   }
   else
   TForm::WndProc(Message);
 }

 void __fastcall THistoryForm::handle_archive(LPOTDM_ARCHIVE_RECORDS_RESPOND resp, BOOL end_of_data)
 {
    timer_count = 0;
    if(!storage_created()) storage_create();
    LPOTDM_ARCHIVE_REC ptr_data = resp->rec;
    LPOTDM_ARCHIVE_REC end_data = ptr_data + resp->count;
    Variant nullv;
    nullv.VType = varNull;
    while(ptr_data<end_data)
    {
     stop_time = max(stop_time,(__int64)ptr_data->ch_time);
     if(!start_time) start_time = stop_time;
     temp_data_storage->add_record(ptr_data,sizeof(*ptr_data));
     ++ptr_data;
    }
     if(end_of_data) end_recv_data();
 }

void __fastcall THistoryForm::end_recv_data()
{
       //Завершение приёма данных Показать себя
      HistView->Items->Count =  temp_data_storage->get_record_count();
      rec_index = -1;
      if(HistView->Items->Count)
      {
       UnicodeString str;
       str.printf(L"строк %d",HistView->Items->Count);
       StatusBar1->Panels->Items[0]->Text = str;
       tbTrends->Enabled = true;
       HistView->Cursor = crDefault;
      }
      else
      {
       HistView->Cursor = crSQLWait;
       tbTrends->Enabled = false;
      }
  actExportComtrade->Enabled = tbTrends->Enabled;
  tbRequestAlarmPeriod ->Enabled = tbTrends->Enabled;
  Timer1->Enabled = false;
  Visible = true;
}


void __fastcall THistoryForm::get_time_text (__int64 tmval,UnicodeString & text,bool is_null,char time_sep,bool msec)
{
  if(is_null)
    text = UnicodeString();
    else
    {
      if(!tmval)
        text = L"Инициализация";
       else
        text = TGkAwpModule::get_time_text(tmval,time_sep,msec);
    }
}



void __fastcall THistoryForm::TabSheet1Resize(TObject *Sender)
{

  int width     = HistView->Width - GetSystemMetrics(SM_CXVSCROLL);
  int cx_border = GetSystemMetrics(SM_CXBORDER);
  width-= cx_border*3;
  for(int i = 1;i<HistView->Columns->Count;i++)
    width-= (cx_border+HistView->Columns->Items[i]->Width);
    HistView->Columns->Items[0]->Width = std::max(width,100);
}
//---------------------------------------------------------------------------

void  __fastcall THistoryForm::get_value_text(DWORD rec_id,double val,UnicodeString & text, bool is_null)
{
  if(is_null)
     text = UnicodeString();
   else
   {
    if(on_get_record)
    {
     if(SameValue(val,double(.0)) || SameValue(val,double(1.0)))
     {
       if(record.rec_id != rec_id )
           on_get_record(rec_id,record,NULL);

       if(record.rec_id == rec_id && record.is_discrete())
       {
         text = DWORD(val) !=0 ? L"ВКЛ":L"ОТКЛ";
         return ;
       }
     }
    }
    text = FormatFloat(L"#,#####0.00000",val);
   }
}




void __fastcall THistoryForm::get_state_text ( DWORD state,UnicodeString & text,bool is_null)
{
      text = UnicodeString();
}

void __fastcall THistoryForm::get_state_view_text ( DWORD state,UnicodeString & text,bool is_null)
{
   if(is_null)
      text = UnicodeString();
    else
    {
     text = TGkAwpModule::get_state_view_text(state);
    }
}

void __fastcall THistoryForm::get_state_tu_text ( DWORD state,UnicodeString & text,bool is_null)
{
   if(is_null)
      text = UnicodeString();
    else
    {
     text = TGkAwpModule::get_state_tu_text(state);
    }
}

void __fastcall THistoryForm::FormClose(TObject *Sender, TCloseAction &Action)
{
  Action = caFree;
}
//---------------------------------------------------------------------------

void __fastcall THistoryForm::Timer1Timer(TObject *Sender)
{
  if(++timer_count > 2 && !Visible)
     Visible = true;
}
//---------------------------------------------------------------------------

void __fastcall THistoryForm::tbRequestAlarmPeriodClick(TObject *Sender)
{
  if(request_proc && HistView->Selected && storage_created())
  {
     TListItem * Item = HistView->Selected;
     OTDM_ARCHIVE_REC rec_data;
     temp_data_storage->get_record(Item->Index,&rec_data,sizeof(rec_data));

     TSelPeriod * sp = new TSelPeriod(this);
     LONGLONG  beg_tm = 1000;//rec_data.ch_time - MSEC_NS100(viewer_module->get_params().history_time_before  );
     LONGLONG  end_tm = 5000;//rec_data.ch_time + MSEC_NS100(viewer_module->get_params().history_time_after   );
     /*Определение завершения*/
     DWORD  rec_no = Item->Index;
     DWORD  rec_id = rec_data.rec_id;
     double value  = rec_data.rvalue;
     ++rec_no;
     while(rec_no<(DWORD)HistView->Items->Count)
     {
      temp_data_storage->get_record(rec_no,&rec_data,sizeof(rec_data));
      if(rec_data.ch_time<end_tm)
      {
        if(rec_data.rec_id == rec_id && !SameValue(value,(double)rec_data.rvalue) )
          {
           end_tm = rec_data.ch_time + 5000;//MSEC_NS100(viewer_module->get_params().history_time_after);
           value  = rec_data.rvalue;
          }
      }
      else
       break;
      ++rec_no;
     }


     sp->beg_time               = beg_tm;
     sp->end_time               = end_tm;
     sp->RadioGroup1->ItemIndex = alarm+1;
     UnicodeString s;
     TModalResult mr = sp->ShowModal();
     if(mr == mrOk)
       {
         s = sp->get_period_text();
         beg_tm = sp->beg_time;
         end_tm = sp->end_time;

        if(sp->RadioGroup1->ItemIndex)
           alarm = sp->RadioGroup1->ItemIndex-1;
           else
           alarm = -1;
       }
     delete sp;
     if(mr == mrOk)
       request_proc(s,beg_tm,end_tm,alarm);
  }
}
//---------------------------------------------------------------------------


string make_std_string(UnicodeString src)
{
  string res(AnsiString(src).c_str());
  return res;
}

 DWORD __fastcall THistoryForm::comtrade_prepare(Tcomtrade& cmt,int resample)
 {
    //Подготовка к записи
    Tcomtrade_channel ch;
    DWORD rec_no    = 0;
    DWORD storage_rec_count = this->temp_data_storage->get_record_count();
    OTDM_ARCHIVE_REC rec_data;
    temp_data_storage->get_record(rec_no,&rec_data,sizeof(rec_data));

    while((rec_no<storage_rec_count) && (rec_data.ch_time == 0/*(QWORD)start_time*/))
    {

      mdb_record rec;
      if(on_get_record(rec_data.rec_id,rec,NULL))
      {
        UnicodeString rec_name  =  on_get_text(rec.rec_id,false);
        string id = make_std_string(rec_name);

        if(rec.is_discrete())
            ch = Tcomtrade_channel(rec.rec_id,id,rec.get_normal_value());
            else
            {
             vector<string> slist;
             boost::split(slist,id,boost::is_any_of(","));
             string phase,unit,component;
             id = slist[0];
             if(slist.size()>1)
                unit = slist[1];
             if(slist.size()>2)
                phase = slist[2];
             if(slist.size()>3)
                component = slist[3];
             int skew = 0;//;15*rec.addr.param_number;
             //if(rec.addr.addr.sb) skew+=480;
             ch = Tcomtrade_channel(rec.rec_id, id, phase,component,unit, 1.0, .0, skew,rec.min_value,rec.max_value);
             if(resample)
                ch.prepare(storage_rec_count);
            }
          ch.set_valid(((rec_data.NULL_MASK&OTDM_ARCHIVE_DIAG_NULL) || 0 == rec_data.diag) ? true : false);
      if(!(rec_data.NULL_MASK&OTDM_ARCHIVE_RVALUE_NULL))
           ch.set_value(rec_data.rvalue);
      cmt.add_channel(ch);
      }
      ++rec_no;
      temp_data_storage->get_record(rec_no,&rec_data,sizeof(rec_data));
    }
   cmt.renumber_channels();
   return rec_no;
 }

 void resample_times( Tcomtrade_channel::times_t  & src_times, Tcomtrade_channel::times_t  &dst_times,int resample)
 {

   Tcomtrade_channel::times_t::iterator src_beg = src_times.begin(),src_end = src_times.end();
   Tcomtrade_channel::times_t::iterator dst_beg = dst_times.begin();
   __int64 first_time  = *src_beg;
   __int64 second_time ;
   __int64 dTM;
   ++src_beg;
   do{
      *dst_beg = first_time;
      ++dst_beg;
      dTM = (*src_beg-first_time)/resample;
      for(int i = 1;i<resample;i++)
      {
       first_time+=dTM;
       *dst_beg = first_time;
       ++dst_beg;
      }
      first_time = *src_beg;
      ++src_beg;

     }while(src_beg<src_end);

     *dst_beg = first_time;

 }

extern "C" int  __stdcall dspl_resample_lagrange(double *s, int n, int p, int q, double frd, double **y, int *ny);

 int  __fastcall THistoryForm::do_resample_channel      (Tcomtrade_channel * ch,int resample)
 {
   int size = ch->get_values_count();
   int rs   = resample*size;
   Tcomtrade_channel::values_t src_val;
   Tcomtrade_channel::times_t  src_times;
   src_val.resize  (size);
   src_times.resize(size);
   std::copy(ch->values.begin(),ch->values.end(),src_val.begin  ());
   std::copy(ch->times.begin (),ch->times.end (),src_times.begin());
   ch->values.resize(rs);
   ch->times.resize (rs);
   double * src = &*src_val.begin   ();
   double * dst = &*ch->values.begin();
   dspl_resample_lagrange(src,size,resample,1,0,&dst,&rs);
   resample_times(src_times,ch->times,resample);

   //dspl_resample_lagrange()
   return rs;
 }

 void __fastcall THistoryForm::comtrade_write_resample  (DWORD rec_no,Tcomtrade& cmt,string & file_name,int rs)
 {
  string  dat_name = file_name+ string(".dat");
  string  cfg_name = file_name+ string(".cfg");
  std::fstream fdat(dat_name.c_str(),ios_base::out|ios_base::trunc);
  DWORD storage_rec_count = temp_data_storage->get_record_count();
  if(fdat.is_open() )
  {
   Tcomtrade_channel * ch;
   OTDM_ARCHIVE_REC rec_data;
   int max_count = 0;
   while(rec_no<storage_rec_count)
   {
      temp_data_storage->get_record(rec_no++,&rec_data,sizeof(rec_data));
      if(rec_data.ch_time)
      {
       ch = cmt.find_channel(rec_data.rec_id);
        if(ch)
          max_count = std::max(max_count, ch->set_value(rec_data.rvalue,rec_data.ch_time));
      }
   }
    if(rs>1)
    {
      int idx = 0;
      do{
         ch = cmt.enum_channels(false,idx++);
         if(ch)
            max_count = std::max(max_count,do_resample_channel(ch,rs));
        }while(ch);
    }

    __int64 last_wr_time = 0;
    for(int i = 0;i<max_count;i++)
    {
      //Вывод в comtrade
      int idx = 0;
      do{
         ch = cmt.enum_channels(false,idx++);
         if(ch)
            {
             double val;__int64 tm;
             ch->get_value_time(i,val,tm);
             ch->set_value(val);
             if(!last_wr_time)  last_wr_time = tm;
            }

        }while(ch);
       cmt.write_samples(fdat,last_wr_time);
       last_wr_time = 0;
    }

    std::fstream fcfg(cfg_name.c_str(),ios_base::out|ios_base::trunc);
    if(fcfg.is_open())
    {
     fcfg<<cmt;
     fcfg.close();
    }
   fdat.close();
  }

 }

 void __fastcall THistoryForm::comtrade_write  (DWORD rec_no,Tcomtrade& cmt,string & file_name)
 {
  string  dat_name = file_name+ string(".dat");
  string  cfg_name = file_name+ string(".cfg");
  std::fstream fdat(dat_name.c_str(),ios_base::out|ios_base::trunc);
  DWORD storage_rec_count = temp_data_storage->get_record_count();
  if(fdat.is_open() )
  {
   Tcomtrade_channel * ch;
   OTDM_ARCHIVE_REC rec_data;
   temp_data_storage->get_record(rec_no,&rec_data,sizeof(rec_data));
   int64_t last_wr_time = rec_data.ch_time;

   while(rec_no<storage_rec_count)
   {
      if(rec_data.ch_time!=last_wr_time)
        {
          cmt.write_samples(fdat,last_wr_time);
          last_wr_time= rec_data.ch_time;
        }
      ch = cmt.find_channel(rec_data.rec_id);
      if(ch)
      {
        ch->set_valid(((rec_data.NULL_MASK&OTDM_ARCHIVE_DIAG_NULL) || 0 == rec_data.diag) ? true : false);
        if(!(rec_data.NULL_MASK&OTDM_ARCHIVE_RVALUE_NULL))
         ch->set_value(rec_data.rvalue);
      }

     ++rec_no;
     temp_data_storage->get_record(rec_no,&rec_data,sizeof(rec_data));
   }
    std::fstream fcfg(cfg_name.c_str(),ios_base::out|ios_base::trunc);
    if(fcfg.is_open())
    {
     fcfg<<cmt;
     fcfg.close();
    }
   fdat.close();
  }
 }

void __fastcall THistoryForm::actExportComtradeExecute(TObject *Sender)
{
  if(this->storage_created())
  {
  TComtradeParam * cp = new TComtradeParam(this);
  cp->ctName->Text    = Caption;
  UnicodeString time_text  ;
  get_time_text(start_time,time_text,false,'-',false);
  UnicodeString file_name ;
  file_name.cat_printf(L"%s-%s",kadr_text.c_str(),time_text.c_str());
  cp->ctFile->Text =  file_name;
  cp->ProgramName->Text = module->get_comtrade_export_viewer();
  if(cp->ShowModal()==mrOk)
  {
    std::string  ctName = make_std_string(cp->ctName->Text);
    Tcomtrade cmt(ctName,1,_wtof(cp->ctFreq->Text.c_str()),_wtof(cp->ctFreqSample->Text.c_str()),start_time,stop_time);
	//DWORD rec_no =
	comtrade_prepare(cmt);
    string cmt_fname = make_std_string(UnicodeString(module->get_comtrade_export_folder().c_str())+cp->ctFile->Text);
    if(cp->ResampleValue->Position>1 || cp->cbAdjustSampleTime->Checked)
       comtrade_write_resample(0,cmt,cmt_fname,cp->ResampleValue->Position);
        else
       comtrade_write  (0,cmt,cmt_fname);
    //if(ExportViewer.Compare(cp->ProgramName->Text))
       //ExportViewer  = cp->ProgramName->Text;

    if(cp->CheckBox1->Checked && !cp->ProgramName->Text.IsEmpty())
      {
        file_name = UnicodeString(L"\""+module->get_comtrade_export_folder()+cp->ctFile->Text+UnicodeString(L".cfg")+L"\"");
		HINSTANCE ret = ShellExecuteW(NULL,NULL,cp->ProgramName->Text.c_str(),file_name.c_str(),NULL,SW_SHOW);
		if(ret)
		   WaitForInputIdle(ret,1000);
		DWORD err = GetLastError();
		char err_text[MAX_PATH];
		GetErrorMessage(err,err_text,KERTL_ARRAY_COUNT(err_text),(HINSTANCE)NULL);
	  }
  }
  delete cp;
 }
 else
 {
  MessageBox(Handle,_T("Отсутствует STORAGE c данными"),_T("Ошибка экспорта"),MB_ICONSTOP|MB_OK);
 }
}
//---------------------------------------------------------------------------

void __fastcall THistoryForm::get_diag_text  ( DWORD diag,UnicodeString & text,bool is_null)
{
    if(is_null || !diag)
       text = UnicodeString();
       else
       {
        //text = diag >1 ? L"НЕДОСТОВЕРНО":L"PART";
        text = L"НЕДОСТОВЕРНО";
       }

}

UnicodeString  __fastcall THistoryForm::get_record_name(DWORD rec_id)
{
   UnicodeString str = on_get_text(rec_id,false);
   if(str.IsEmpty())
      str.printf(L"record id %d : %s",rec_id,on_get_text(rec_id,true).c_str());
   return str;
}

void __fastcall THistoryForm::HistViewData(TObject *Sender, TListItem *Item)
{

  if(temp_data_storage && Item->Index<(int)temp_data_storage->get_record_count())
  {
   OTDM_ARCHIVE_REC rec_data;
   temp_data_storage->get_record(Item->Index,&rec_data,sizeof(rec_data));
   UnicodeString text = get_record_name(rec_data.rec_id);
   Item->Caption = text;

   get_value_text(rec_data.rec_id,rec_data.rvalue,text,(rec_data.NULL_MASK&OTDM_ARCHIVE_RVALUE_NULL) ? true : false);
   Item->SubItems->Add(text);

   get_time_text(rec_data.ch_time,text,(rec_data.NULL_MASK&OTDM_ARCHIVE_CHTIME_NULL) ? true : false);
   Item->SubItems->Add(text);

   get_state_view_text((DWORD)rec_data.state,text,(rec_data.NULL_MASK&OTDM_ARCHIVE_STATE_NULL) ? true : false);
   Item->SubItems->Add(text);

   get_state_tu_text((DWORD)rec_data.state,text,(rec_data.NULL_MASK&OTDM_ARCHIVE_STATE_NULL) ? true : false);
   Item->SubItems->Add(text);

   get_diag_text(rec_data.diag,text,(rec_data.NULL_MASK&OTDM_ARCHIVE_DIAG_NULL) ? true : false);
   Item->SubItems->Add(text);
   Item->SubItems->Add(rec_data.alarm);
   text.printf(L"%.2f" , float(rec_data.kvants)/float(100));
   Item->SubItems->Add(text);
   get_time_text(rec_data.wr_time,text,(rec_data.wr_time) ?  false : true);
   Item->SubItems->Add(text);

  }
}
//---------------------------------------------------------------------------

 bool __fastcall THistoryForm::storage_create()
 {
  storage_delete();
  if(temp_file_name.IsEmpty())
  {
   wchar_t temp_dir[MAX_PATH<<1];
   GetTempPathW(KERTL_ARRAY_COUNT(temp_dir),temp_dir);
   temp_file_name = temp_dir;
   temp_file_name.cat_printf(L"%p.hist.tmp",this);
  }
  temp_file_stream  = new KeRTL::TFileStream(temp_file_name.c_str());
  temp_data_storage = new KeRTL::TStorage(temp_file_stream);
  temp_data_storage->create(1,"HISTDATA",8,8192);
  temp_data_storage->clear();
  return temp_data_storage->is_good();
 }

 void __fastcall THistoryForm::storage_delete()
 {
   if(temp_data_storage)
     {
      temp_data_storage->clear();
      delete temp_data_storage;
      temp_data_storage = NULL;
     }
   if(temp_file_stream)
   {
    delete temp_file_stream;
    temp_file_stream = NULL;
    DeleteFile(temp_file_name);
   }
 }


void __fastcall THistoryForm::FormDestroy(TObject *Sender)
{
  storage_delete();
}
//---------------------------------------------------------------------------

void __fastcall THistoryForm::HistViewSelectItem(TObject *Sender, TListItem *Item,
          bool Selected)
{
  UnicodeString str;
  if(Selected && Item && temp_data_storage && Item->Index<(int)temp_data_storage->get_record_count())
  {
   OTDM_ARCHIVE_REC rec_data;
   temp_data_storage->get_record(Item->Index,&rec_data,sizeof(rec_data));
   str.printf(L"rec_id = %d Состояние ",rec_data.rec_id);
   if(rec_data.state)
      str+=TGkAwpModule::get_state_text(rec_data.state);
  }
  StatusBar1->Panels->Items[1]->Text = str;
}
//---------------------------------------------------------------------------



void __fastcall THistoryForm::actAddSeriesExecute(TObject *Sender)
{
   //Добавить на график
   TListItem * item = HistView->Selected;

   if(item)
   {
    OTDM_ARCHIVE_REC rec_data;
    temp_data_storage->get_record(item->Index,&rec_data,sizeof(rec_data));
    DWORD rec_id = rec_data.rec_id;
    mdb_record  rec;

    if(module->get_record(rec_id,rec))
    {
      bool is_discrete =     (rec.is_discrete());
      TChartSeries  *    fl = chart_frame->get_series(rec_id);
      fl->Title             = item->Caption;
      fl->XValues->DateTime = true;

      for(int i = 0; i<HistView->Items->Count;i++)
      {
        temp_data_storage->get_record(i,&rec_data,sizeof(rec_data));
        if(rec_data.rec_id == rec_id && rec_data.ch_time)
        {

         SYSTEMTIME st;
         FileTimeToSystemTime((LPFILETIME)&rec_data.ch_time,&st);
         fl->AddXY(TDateTime(st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond,st.wMilliseconds).Val,rec_data.rvalue);
        }
      }

      fl->Visible = true;
      fl->Tag     = is_discrete;
      TFastLineSeries * fl_series = dynamic_cast<TFastLineSeries*>(fl);
      if(fl_series)
      {
       fl_series->DrawStyle      = flAll;
       fl_series->Stairs = is_discrete;
      }
      //Chart1->AddSeries(fl);
    }

   }
}
//---------------------------------------------------------------------------

void __fastcall THistoryForm::actRemoveSeriesExecute(TObject *Sender)
{
   TListItem * item = HistView->Selected;
   if(item)
   {
    OTDM_ARCHIVE_REC rec_data;
    temp_data_storage->get_record(item->Index,&rec_data,sizeof(rec_data));
    //DWORD rec_id = rec_data.rec_id;
    chart_frame->remove_series(rec_data.rec_id);
   }
}

/*Статистика*/
//---------------------------------------------------------------------------
 void __fastcall hist_stat_t::set_value(DWORD _rec_id,double value,QWORD tm)
 {
   if(rec_id == (DWORD)-1)   rec_id = _rec_id;
   if(rec_id == _rec_id)
   {

       if(count)
       {
        rec_min       = KeRTL::MIN(rec_min,curr_value);
        rec_max       = KeRTL::MAX(rec_max,curr_value);
        QWORD tm_delta = NS100_MSEC(tm-curr_time);
        time_lenght  += tm_delta;
        rec_integral += curr_value*(double)tm_delta;
        rec_avg     = time_lenght ? rec_integral/time_lenght : 0;
        if(fabs(curr_value-value)>.00001) ++changes;
       }

       curr_value = value;curr_time = tm;
       ++count;
   }
 }


 void __fastcall THistoryForm::make_staticstic()
 {
   if(temp_data_storage && temp_data_storage->is_good())
   {
    StatisticView->Cursor = crHourGlass;
    OTDM_ARCHIVE_REC rec_data;
    hist_stats_map_t hstat;

    for(DWORD i = 0;i<temp_data_storage->get_record_count();i++)
    {
      temp_data_storage->get_record(i,&rec_data,sizeof(rec_data));
      if(rec_data.ch_time && rec_data.diag == MDBR_DIAG_GOOD  )
      {
       hist_stat_t & hs =  hstat[rec_data.rec_id];
       double val = rec_data.rvalue;
       if((rec_data.NULL_MASK & OTDM_ARCHIVE_RVALUE_NULL))
           val = hs.curr_value;
       hs.set_value(rec_data.rec_id,val,rec_data.ch_time);
       if(!(i%100)) Application->ProcessMessages() ;
      }
    }

    hist_stats_map_t::iterator ptr = hstat.begin(), end = hstat.end();
    const wchar_t * ffmt = L"%.2f";
    while(ptr!=end)
    {
         hist_stat_t & hs = ptr->second;

         //hs.set_value(hs.rec_id,hs.curr_value,KeRTL::GetTime()) ;
         TListItem * item = StatisticView->Items->Add();
         UnicodeString text ;
         text = on_get_text(hs.rec_id,false);
         item->Caption = text;
         text.printf(ffmt,hs.rec_min);
         item->SubItems->Add(text);

         text.printf(ffmt,hs.rec_max);
         item->SubItems->Add(text);

         text.printf(ffmt,hs.rec_avg);
         item->SubItems->Add(text);

         text.printf(L"%.5f",hs.rec_integral/double(3600000));
         item->SubItems->Add(text);
         wchar_t hms_text[128];
         secunds2HMS(hs.time_lenght/1000,hms_text,KERTL_ARRAY_COUNT(hms_text));
         item->SubItems->Add(hms_text);
         item->SubItems->Add(hs.changes);

         ++ptr;
    }
        StatisticView->Cursor = crDefault;
   }
 }



void __fastcall THistoryForm::PageControl1Change(TObject *Sender)
{
  if(PageControl1->ActivePage == this->tabStatistic && !StatisticView->Items->Count)
  {
     make_staticstic();
  }
  bool graph_btn_visible = (PageControl1->ActivePage ==tabGraphics) ? true : false;
  tbHideLegend->Visible = graph_btn_visible;
  tbNoStairs  ->Visible = graph_btn_visible;
}
//---------------------------------------------------------------------------

AnsiString __fastcall THistoryForm::get_color_for_html (DWORD diag,DWORD state)
{
   AnsiString color;
   if(diag)
      color =  "\"black\"";
      else
      {
       if(state & MDBR_STATE_ALARM)
       color =  "\"red\"";
       else
       {
       if(state & MDBR_STATE_DANGER)
         color =  "\"brown\"";
          else
         color = "\"green\"" ;
       }
      }
  return color;
}

TColor __fastcall THistoryForm::get_color_for_list_view(DWORD diag,DWORD state)
{
   TColor color;
   if(diag)
      color =  (TColor)get_ready_colors().name_bad ;
      else
      {
       if(state & MDBR_STATE_ALARM)
       color =  (TColor)get_ready_colors().state_alarm;
       else
       {
       if(state & MDBR_STATE_DANGER)
         color =  (TColor)get_ready_colors().state_danger;
          else
         color =  (TColor)get_ready_colors().name_good ;
       }
      }
  return color;
}

void __fastcall THistoryForm::HistViewCustomDrawItem(TCustomListView *Sender, TListItem *Item,
          TCustomDrawState State, bool &DefaultDraw)
{
   if(rec_index != (DWORD)Item->Index)
   {
    rec_index = Item->Index ;
    temp_data_storage->get_record(Item->Index,&rec_data,sizeof(rec_data));
   }
   HistView->Canvas->Font->Color = get_color_for_list_view(rec_data.diag,rec_data.state);
   DefaultDraw = true;
}
//---------------------------------------------------------------------------

void __fastcall THistoryForm::HistViewCustomDrawSubItem(TCustomListView *Sender, TListItem *Item,
          int SubItem, TCustomDrawState State, bool &DefaultDraw)
{
   //TColor  color; = rec_data.diag ? clBlack : clGreen;
   //HistView->Canvas->Font->Color = color;
   DefaultDraw = true;
}
//---------------------------------------------------------------------------


const char  html_head [] =
"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\r\n"
"<html>\r\n"
  "<head>"
  "\n\r<meta http-equiv=\"Content-Type\" content=\"text/html; charset=windows-1251\">\n\r"
  "<title>%s %s </title>"
  "<style>"
  "body { font-family: serif;}"
  ".grid {font-family: monospace;font-weight:bold;font-size:78%;}"
  "</style>"
  "</head>\r\n"
;
const char  html_body [] =
  "<body>\r\n"
    "<h3 align=\"center\">%s</h3>\r\n"
    "<h4 align=\"center\">%s </h4>\r\n"
     "<p align=\"center\">"
     ;
const char  html_table_hdr_history [] =
          "<table class=\"grid\" cellspacing=1 cellpadding=1 bgcolor=\"oldlace\"  border=1>\r\n"
           "<tr>\r\n"
            "<th>№</th>"
            " <th>Имя</th>"
            " <th>Значение</th>"
            " <th>Время</th>"
            " <th>Состояние</th>"
            " <th>ТУ</th>"
            " <th>Диагностика</th>"
          "</tr>\r\n"
;

const char   html_table_row_history[] =
          "<tr>\r\n"
          " <td>%u</td>"
          " <td><font color=%s>%s</font></td>"
          " <td><font color=%s>%s</font></td>"
          " <td><font color=%s>%s</font></td>"
          " <td><font color=%s>%s</font></td>"
          " <td><font color=%s>%s</font></td>"
          " <td><font color=%s>%s</font></td>"
          "</tr>\r\n"
 ;


const char html_end[] =
"</table>\r\n"
"</body>\r\n"
"</html>\r\n"
;

void __fastcall THistoryForm::export_HTML_history(KeRTL::TFileStream & fs)
{
     UnicodeString text[6];
     AnsiString  str;
     char * title = "История телемеханики";
     str.printf(html_head,title,AnsiString(period_text).c_str());
     fs.Write(str.c_str(),str.Length());
     str.printf(html_body,title,AnsiString(period_text).c_str());
     fs.Write(str.c_str(),str.Length());

     str.printf(html_table_hdr_history);
     fs.Write(str.c_str(),str.Length());

     OTDM_ARCHIVE_REC rec_data;
     DWORD i = 0;
     DWORD rec_count = temp_data_storage->get_record_count();
     DWORD number = 1;
     while(i< rec_count )
     {
      temp_data_storage->get_record(i++,&rec_data,sizeof(rec_data));
      if(rec_data.ch_time)
      {
         text[0] = on_get_text(rec_data.rec_id,false);
         get_value_text(rec_data.rec_id,rec_data.rvalue,text[1],(rec_data.NULL_MASK&OTDM_ARCHIVE_RVALUE_NULL) ? true : false);
         get_time_text(rec_data.ch_time,text[2],(rec_data.NULL_MASK&OTDM_ARCHIVE_CHTIME_NULL) ? true : false);
         get_state_view_text((DWORD)rec_data.state,text[3],(rec_data.NULL_MASK&OTDM_ARCHIVE_STATE_NULL) ? true : false);
         get_state_tu_text((DWORD)rec_data.state,text[4],(rec_data.NULL_MASK&OTDM_ARCHIVE_STATE_NULL) ? true : false);
         get_diag_text(rec_data.diag,text[5],(rec_data.NULL_MASK&OTDM_ARCHIVE_DIAG_NULL) ? true : false);
         get_color_for_html(rec_data.diag,rec_data.state).c_str() ;
         AnsiString fcolor = get_color_for_html(rec_data.diag,rec_data.state);
         str.printf(html_table_row_history
                                    ,number++
                                    ,fcolor.c_str(),AnsiString(text[0]).c_str()
                                    ,fcolor.c_str(),AnsiString(text[1]).c_str()
                                    ,fcolor.c_str(),AnsiString(text[2]).c_str()
                                    ,fcolor.c_str(),AnsiString(text[3]).c_str()
                                    ,fcolor.c_str(),AnsiString(text[4]).c_str()
                                    ,fcolor.c_str(),AnsiString(text[5]).c_str()
                                    );
         fs.Write(str.c_str(),str.Length());
      }
     }
     fs.Write((void*)html_end,strlen(html_end));
}

const char  html_table_hdr_stats [] =
          "<table class=\"grid\" cellspacing=1 cellpadding=1 bgcolor=\"oldlace\"  border=1>\r\n"
           "<tr>\r\n"
            "<th>№</th>"
            " <th>Имя</th>"
            " <th>Min</th>"
            " <th>Max</th>"
            " <th>Среднее</th>"
            " <th>Интегральное(/час)</th>"
            " <th>Продолжительность</th>"
            " <th>Изменений</th>"
          "</tr>\r\n"
;

const char   html_table_row_stats[] =
          "<tr>\r\n"
          " <td>%u</td>"
          " <td>%s</td>"
          " <td>%s</td>"
          " <td>%s</td>"
          " <td>%s</td>"
          " <td>%s</td>"
          " <td>%s</td>"
          " <td>%s</td>"
          "</tr>\r\n"
 ;

void __fastcall THistoryForm::export_HTML_stats(KeRTL::TFileStream & fs)
{
     AnsiString  str;
     char * title = "Статистика телемеханики";
     str.printf(html_head,title,AnsiString(period_text).c_str());
     fs.Write(str.c_str(),str.Length());
     str.printf(html_body,title,AnsiString(period_text).c_str());
     fs.Write(str.c_str(),str.Length());

     str.printf(html_table_hdr_stats);
     fs.Write(str.c_str(),str.Length());

     StatisticView->Items->BeginUpdate();
      for(int i = 0; i< StatisticView->Items->Count;)
         {
           TListItem * item = StatisticView->Items->Item[i++];
           str.printf(html_table_row_stats
                     ,i
                     ,AnsiString(item->Caption).c_str()
                     ,AnsiString(item->SubItems->Strings[0]).c_str()
                     ,AnsiString(item->SubItems->Strings[1]).c_str()
                     ,AnsiString(item->SubItems->Strings[2]).c_str()
                     ,AnsiString(item->SubItems->Strings[3]).c_str()
                     ,AnsiString(item->SubItems->Strings[4]).c_str()
                     ,AnsiString(item->SubItems->Strings[5]).c_str()
                     );
          fs.Write(str.c_str(),str.Length());
         }

     StatisticView->Items->EndUpdate();

     fs.Write((void*)html_end,strlen(html_end));

}


void __fastcall THistoryForm::actExportHTMLExecute(TObject *Sender)
{
  //Сохранить в HTML
  if(SaveDialog1->Execute(Handle))
  {
     KeRTL::TFileStream fs(SaveDialog1->FileName.c_str());
     if(PageControl1->ActivePage == tabStatistic)
        export_HTML_stats  (fs);
        else
        export_HTML_history(fs);
     fs.SetSize(fs.GetPosition());
  }
}
//---------------------------------------------------------------------------


void __fastcall THistoryForm::Chart1DragOver(TObject *Sender, TObject *Source, int X,
          int Y, TDragState State, bool &Accept)
{
   Accept = true;
}
//---------------------------------------------------------------------------

void __fastcall THistoryForm::tbNoStairsClick(TObject *Sender)
{
  chart_frame->set_stairs(!tbNoStairs->Down);
}
//---------------------------------------------------------------------------

void __fastcall THistoryForm::tbHideLegendClick(TObject *Sender)
{
  chart_frame->legen_visible = !tbHideLegend->Down;
}
//---------------------------------------------------------------------------

