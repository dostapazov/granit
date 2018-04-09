//---------------------------------------------------------------------------

#pragma hdrstop
#include <vcl.h>

#include "rechistory_form.h"
#include <otd_proto_medium.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "gkmodule_form"
#pragma link "about_common_frame"
#pragma resource "*.dfm"
TRecHistroyForm *RecHistroyForm;
//---------------------------------------------------------------------------
__fastcall TRecHistroyForm::TRecHistroyForm(TComponent* Owner)
	: TGKModuleForm(Owner)
{
 monitor  = NULL;
}

__fastcall TRecHistroyForm::TRecHistroyForm(TComponent* Owner,HWND wnd,GKHANDLE handle)
          :TGKModuleForm(Owner,wnd,handle)
{
 monitor  = NULL;
}

void     __fastcall TRecHistroyForm::begin_call_gkhandle(wchar_t * cmd_name)
{

 StatusBar1->Panels->Items[0]->Text =  cmd_name;
 StatusBar1->Panels->Items[1]->Text =  AnsiString();
}

void     __fastcall TRecHistroyForm::done_call_gkhandle(LRESULT ret,wchar_t * str)
{
 DWORD last_error = ret ? GetLastError():0;
 if(str)
 {
  StatusBar1->Panels->Items[0]->Text = str;
 }
 switch(ret)
 {

  case GKH_RET_SUCCESS:
  case GKH_RET_ERROR  :
                      {
                      wchar_t  err_text[MAX_PATH];
                      mod_iface.get_error_text(last_error,err_text,sizeof(err_text)/sizeof(wchar_t));
                      StatusBar1->Panels->Items[1]->Text = AnsiString(err_text);
                      }
                      break;


 }
}


void     __fastcall TRecHistroyForm::after_set_gkhandle()
{
   in_setup = 0;
   TGKModuleForm::after_set_gkhandle();
   get_module_settings();
   setup_controls(&settings);
   on_module_state(mod_iface(),mod_iface.get_module_state());
   set_notify(mod_iface(),MNF_COMMON|RECHISTORY_NOTIFY_STATS);
   PageControl2->ActivePageIndex = 0;
}

//---------------------------------------------------------------------------

void     __fastcall TRecHistroyForm::on_module_state(GKHANDLE mod,DWORD state)
{
  bool running = ((state & MODULE_STATE_RUNNING_MASK )== MODULE_STATE_RUNNING) ? true:false;
  bool stopped  = (state & MODULE_STATE_RUNNING_MASK ) ? false:true;
  tbStop->Enabled      = running;
  tbStart->Enabled     = stopped;
  tbDbRestore->Enabled = stopped;
  if(running)
     StatusBar1->Panels->Items[0]->Text = "Запись ведётся";
  if(stopped)
     StatusBar1->Panels->Items[0]->Text = "Запись остановлена";

}



void     __fastcall TRecHistroyForm::on_module_config_change(GKHANDLE mod,LPMODULE_CONFIG_DATA mcd)
{

}

void     __fastcall TRecHistroyForm::setup_controls(LPREC_HIST_CONFIG rhc)
{
  begin_setup();
  cl_lib->Text  = rhc->client_lib;
  user->Text    = rhc->user_name;
  passw->Text   = rhc->password;
  db_path->Text = rhc->dbpath;
  WrChanges->Checked = rhc->write_changes;
  WrAlarms->Checked  = rhc->write_alarm_archives;
  WrSqlQuery->Text   = rhc->sql_wr_history;
  RdSqlQuery->Text   = rhc->sql_rd_history;

  SqlClearHist->Text = rhc->sql_clear_hist;
  SqlUpdIndexes->Text= rhc->sql_idx_update;

  CommitTimeOut->Text = rhc->commit_time_out;
  CommitAfter->Text   = rhc->commit_after;
  this->ArchiveDaysLong->Text = rhc->timestamp_limit;
  ch_mask       = 0;
  end_setup();
}




bool __fastcall  TRecHistroyForm::get_module_settings()
{
 BYTE buf[sizeof(MODULE_CONFIG_DATA)+sizeof(REC_HIST_CONFIG)] ;
 memset(buf,0,sizeof(buf));
 LPMODULE_CONFIG_DATA mcd  = (LPMODULE_CONFIG_DATA)buf;
 mcd->dw_sz = MCD_SIZE;
 LPREC_HIST_CONFIG     hcfg = (LPREC_HIST_CONFIG)mcd->data;
 hcfg->dw_size = sizeof(*hcfg);
 mcd->mask = -1;
 if(mod_iface.get_module_config_data(mcd,MCD_SIZE))
 {
   memcpy(&settings,hcfg,sizeof(settings));
   return true;
 }
 return false;
}




void __fastcall TRecHistroyForm::tbSelLibClick(TObject *Sender)
{
  OpenDialog1->FileName = this->cl_lib->Text;
  if(OpenDialog1->Execute(Handle))
  {
    cl_lib->Text = OpenDialog1->FileName;
  }
}
//---------------------------------------------------------------------------

void __fastcall  TRecHistroyForm::set_ch_mask(DWORD chm)
{
 if(Fch_mask^chm)
 {
  Fch_mask = chm;
 }
 bApply->Enabled = Fch_mask ? true:false;
 bUndo-> Enabled = Fch_mask ? true:false;
}


void __fastcall TRecHistroyForm::cl_libChange(TObject *Sender)
{
  if(!in_setup)
  {
      TEdit * edit = dynamic_cast<TEdit*>(Sender);
      if(edit)
      {
       const wchar_t * str = edit->Text.c_str();
       UnicodeString  val;
       switch(edit->Tag)
       {
         case RCH_CONF_CLIENTLIB:
               str = settings.client_lib;
         break;
         case RCH_CONF_DBPATH:
              str = settings.dbpath;
         break;
         case RCH_CONF_USERNAME:
              str = settings.user_name;
         break;
         case RCH_CONF_USERPASSW:
              str = settings.password;
         break;
         case RCH_CONF_COMMIT_TIMEOUT:
              val = settings.commit_time_out;
              str = val.c_str();
         break;
         case RCH_CONF_COMMIT_AFTER:
              val = settings.commit_after;
              str = val.c_str();
         break;
         case RCH_CONF_TIMESTAMP_LIMIT:
              val = settings.timestamp_limit;
              str = val.c_str();
         break;
       }
       if(wcscmp(str,edit->Text.c_str()))
          ch_mask|= edit->Tag;
          else
          ch_mask&=~edit->Tag;
      }
  }
}
//---------------------------------------------------------------------------

void __fastcall TRecHistroyForm::bUndoClick(TObject *Sender)
{
  get_module_settings();
  setup_controls(&settings);
}
//---------------------------------------------------------------------------

void __fastcall TRecHistroyForm::bApplyClick(TObject *Sender)
{
  BYTE buf[sizeof(MODULE_CONFIG_DATA)+sizeof(REC_HIST_CONFIG)] ;
  memset(buf,0,sizeof(buf));
  LPMODULE_CONFIG_DATA mcd  = (LPMODULE_CONFIG_DATA)buf;
  mcd->dw_sz = MCD_SIZE;
  REC_HIST_CONFIG  &  hcfg = *(LPREC_HIST_CONFIG)mcd->data;
  hcfg.dw_size = sizeof(REC_HIST_CONFIG);
  mcd->mask = -1;
  wcsncpy(hcfg.client_lib,cl_lib->Text.c_str(),KERTL_ARRAY_COUNT (hcfg.client_lib));
  wcsncpy(hcfg.dbpath    ,db_path->Text.c_str(),KERTL_ARRAY_COUNT(hcfg.dbpath));
  wcsncpy(hcfg.user_name ,user->Text.c_str   (),KERTL_ARRAY_COUNT(hcfg.user_name));
  wcsncpy(hcfg.password  ,passw->Text.c_str  (),KERTL_ARRAY_COUNT(hcfg.password));
  hcfg.write_changes        = WrChanges->Checked;
  hcfg.write_alarm_archives = WrAlarms->Checked;
  AnsiString wrq_str = WrSqlQuery->Text;
  AnsiString rdq_str = RdSqlQuery->Text;
  AnsiString sql_clr_hist = SqlClearHist->Text;
  AnsiString sql_upd_idx  = SqlUpdIndexes->Text;

  strncpy(hcfg.sql_rd_history,rdq_str.c_str(),sizeof(hcfg.sql_rd_history));
  strncpy(hcfg.sql_wr_history,wrq_str.c_str(),sizeof(hcfg.sql_wr_history));
  strncpy(hcfg.sql_clear_hist,sql_clr_hist.c_str(),sizeof(hcfg.sql_clear_hist));
  strncpy(hcfg.sql_idx_update,sql_upd_idx.c_str(),sizeof(hcfg.sql_idx_update));

  hcfg.commit_time_out = std::max(100,_wtoi(CommitTimeOut->Text.c_str()));
  hcfg.commit_after    = std::max(100,_wtoi(CommitAfter->Text.c_str()));
  hcfg.timestamp_limit = _wtoi(this->ArchiveDaysLong->Text.c_str());

  hcfg.dw_size = hcfg.dw_size;
  begin_call_gkhandle(L"Применение настроек");
  LRESULT res = GKH_RET_ERROR;
  if(mod_iface.set_module_config_data(mcd,MCD_SIZE))
  {
    ch_mask = 0;
    memcpy(&settings,&hcfg,sizeof(settings));
    res = GKH_RET_SUCCESS;
  }

  done_call_gkhandle(res,NULL);
}
//---------------------------------------------------------------------------

void __fastcall TRecHistroyForm::tbStartClick(TObject *Sender)
{
  done_call_gkhandle(mod_iface.start(MODULE_START_REQUEST,0),L"Запуск записи");
}
//---------------------------------------------------------------------------

void __fastcall TRecHistroyForm::tbStopClick(TObject *Sender)
{
 done_call_gkhandle(mod_iface.stop(MODULE_STOP_REQUEST),L"Останов записи");
}
//---------------------------------------------------------------------------



void __fastcall TRecHistroyForm::WrChangesClick(TObject *Sender)
{
  TCheckBox * cb = dynamic_cast<TCheckBox*>(Sender);
  if(cb && cb->Tag)
  {
    bool v = cb->Checked;
    switch(cb->Tag)
    {
      case RCH_CONF_WR_CHANGES        : v = settings.write_changes;
      break;
      case RCH_CONF_WR_ALARM_ARCHIVES : v = settings.write_alarm_archives;
      break;
    }
   if(v!=cb->Checked)
      ch_mask|= cb->Tag;
      else
      ch_mask&= ~cb->Tag;
  }
}
//---------------------------------------------------------------------------

void __fastcall TRecHistroyForm::WrSqlQueryChange(TObject *Sender)
{
  TMemo * memo = dynamic_cast<TMemo*>(Sender);
  if(memo && memo->Tag)
  {
    UnicodeString str = memo->Text;
    switch(memo->Tag)
    {
      case RCH_CONF_SQL_WRHISTROY  : str = settings.sql_wr_history;
      break;
      case RCH_CONF_SQL_RDHISTROY  : str = settings.sql_rd_history;
      break;
      case RCH_CONF_SQL_CLEAR_HIST : str = settings.sql_clear_hist;
      break;
      case RCH_CONF_SQL_UPD_IDX    : str = settings.sql_idx_update;
      break;
    }
    if(str.CompareIC(memo->Text))
      ch_mask|= memo->Tag;
      else
      ch_mask&= ~memo->Tag;

  }
}
//---------------------------------------------------------------------------

void __fastcall TRecHistroyForm::PageControl2Change(TObject *Sender)
{
   if(PageControl2->ActivePage == MonSheet)
   {
     create_monitor();
     set_notify(mod_iface(),get_notify_mask()|MNF_COMMON);
   }
   else
   {
    reset_notify(mod_iface(), get_notify_mask());
    set_notify(mod_iface()  , MNF_COMMON       );
   }

}
//---------------------------------------------------------------------------


DWORD __fastcall TRecHistroyForm::get_notify_mask()
{
 DWORD ret = MNF_COMMON;
   if(cbRead->Checked)
     ret |= (RECHISTORY_NOTIFY_READ|RECHISTORY_NOTIFY_SEND);
   if(cbStore->Checked)
     ret |= RECHISTORY_NOTIFY_STORE;
   if(cbCommit->Checked)
     ret |= RECHISTORY_NOTIFY_COMMIT;
   if(cbUpdidx->Checked)
     ret |= RECHISTORY_NOTIFY_UPDIDX;
   if(ret) ret|= RECHISTORY_NOTIFY_OPENDB;
  return ret;
}

void __fastcall TRecHistroyForm::cbStoreClick(TObject *Sender)
{
  reset_notify(mod_iface(),-1);
  set_notify(mod_iface(),get_notify_mask());
}
//---------------------------------------------------------------------------

void __fastcall TRecHistroyForm::create_monitor()
{
 if(!monitor)
 {
   monitor = new TRHMonitor;
   KeWin::TRect r(0,0,MonBox->Width,MonBox->Height);
   r.Grow(-4,-10);
   r.Move(4,8);
   monitor->DoCreate(MonBox->Handle,r,-1,256,L"RHMonitor");
   HFONT font = CreateFontW(-12,0,0,0,FW_NORMAL,0,0,0,
               RUSSIAN_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,
               L"Courier New");
   font = monitor->SetFont(font,false);
   monitor->SetBkColor(GetSysColor(COLOR_INFOBK));
   monitor->SetFlags(FLWND_DOUBLE_BUFFERED,false);
   DeleteObject(font);
   monitor->set_lines_limit(4096);
 }
}

void __fastcall TRecHistroyForm::FormDestroy(TObject *Sender)
{
  if(monitor)
     {
      monitor->Destroy();
      delete monitor;
     }
}
//---------------------------------------------------------------------------

LRESULT __fastcall TRecHistroyForm::on_gkhandle_notify (GKHANDLE from,LPNOTIFY_CODE nc,DWORD mask)
{
   LRESULT ret = GKH_RET_SUCCESS;

   switch(mask)
   {
     case RECHISTORY_NOTIFY_STORE:
          on_store(nc);
     break;
     case RECHISTORY_NOTIFY_READ:
          on_read(nc);
     break;
     case RECHISTORY_NOTIFY_SEND:
          on_send(nc);
     break;
     case RECHISTORY_NOTIFY_COMMIT:
          on_commit(nc);
     break;
     case RECHISTORY_NOTIFY_UPDIDX:
          on_updidx(nc);
     break;
     case RECHISTORY_NOTIFY_OPENDB:
     on_opendb(nc);
     break;
     case RECHISTORY_NOTIFY_STATS :
     on_statistic(nc);
     break;
     default:
       ret =  TGKModuleForm::on_gkhandle_notify(from,nc,mask);
     break;
   }
  return ret;
}

void __fastcall  TRecHistroyForm::on_store (LPNOTIFY_CODE nc)
{
     AnsiString   text;
     SYSTEMTIME   st;
     MDB_RECORD * rec = (MDB_RECORD *)nc->notify_data;
   if(RecFilter->Text.IsEmpty() || DWORD(_wtoi(RecFilter->Text.c_str())) == rec->rec_id)
   {
     text.printf("Сохранение изменений rec_id %d маска = %04X",rec->rec_id, nc->notify_code);
     if(nc->notify_code& MDBR_FIELD_VALUE)
         text.cat_printf(" знач=%.3f", rec->value);
     if(nc->notify_code& MDBR_FIELD_DIAG)
         text.cat_printf(" diag=%hX",rec->diag);
     if(nc->notify_code& MDBR_FIELD_STATE)
        text.cat_printf(" state=%hX",rec->state);
      FileTimeToSystemTime((LPFILETIME)&rec->time,&st);
      text.cat_printf(" tm=%04d-%02d-%02d %02d:%02d:%02d.%03d",
                     (int)st.wYear,(int)st.wMonth,(int)st.wDay,
                     (int)st.wHour,(int)st.wMinute,(int)st.wSecond,(int)st.wMilliseconds
                     );
     monitor_add_line(text.c_str());
   }

}

void __fastcall  TRecHistroyForm::on_read  (LPNOTIFY_CODE nc)
{
  char text[512];
  snprintf(text,sizeof(text),"Чтение истории - %s",nc->notify_code ? "получение данных":"подготовка запроса");
  monitor_add_line(text);
}

void    __fastcall TRecHistroyForm::on_send  (LPNOTIFY_CODE nc)
{
  char text[512];
  snprintf(text,sizeof(text),"Передача результатов чтения  %d байт",nc->notify_code );
  monitor_add_line(text);

}

void __fastcall  TRecHistroyForm::on_commit(LPNOTIFY_CODE nc)
{
  char text[512];
  snprintf(text,sizeof(text),"%s транзакции id %d",nc->notify_code ? "Commit":"Rollback",*(LPDWORD)nc->notify_data);
  monitor_add_line(text);

}

void __fastcall  TRecHistroyForm::on_updidx(LPNOTIFY_CODE nc)
{
  char text[512];
  snprintf(text,sizeof(text),"Обновление статистики индексов");
  monitor_add_line(text);
}

void    __fastcall TRecHistroyForm::on_opendb(LPNOTIFY_CODE nc)
{
  char text[512];
  snprintf(text,sizeof(text),"%s базы данныx",nc->notify_code ? "ОТкрытие":"Закрытие");
  monitor_add_line(text);
}

void    __fastcall TRecHistroyForm::on_statistic(LPNOTIFY_CODE nc)
{
  UnicodeString str((char*)nc->notify_data,NOTIFY_DATA_SIZE(nc));
  StatsMemo->Lines->Add(str);
}


void __fastcall TRecHistroyForm::PageControl2Resize(TObject *Sender)
{
  if(monitor)
     monitor->SetSize(MonBox->Width-8,MonBox->Height-12);
}
//---------------------------------------------------------------------------

void __fastcall TRecHistroyForm::bMonClearClick(TObject *Sender)
{
  if(monitor)
     monitor->clear();
}
//---------------------------------------------------------------------------

void    __fastcall TRecHistroyForm::monitor_add_line(char * text)
{
 if(monitor && !bPause->Down)
 {
  SYSTEMTIME st;
  GetLocalTime(&st);
  AnsiString str;
  str.printf("[%02hu:%02hu:%02hu.%03hu] - %s",st.wHour,st.wMinute,st.wSecond,st.wMilliseconds,text);
  monitor->add_line(str.c_str());
 }
}

void __fastcall TRecHistroyForm::tbDbRestoreClick(TObject *Sender)
{
 //Восстановление БД
 RECHIST_RESTORE rhr;
 ZeroMemory(&rhr,sizeof(rhr));
 rhr.dw_size = sizeof(rhr);
 rhr.page_size = 4096;
 rhr.buffers   = 25000;
 begin_call_gkhandle(L"Восстановление БД");
 if(!db_path->Text.IsEmpty() && !user->Text.IsEmpty() && !passw->Text.IsEmpty())
 {
   AnsiString str;
   str = db_path->Text;
   strncpy(rhr.target_name,str.c_str(),sizeof(rhr.target_name));
   str = user->Text;
   strncpy(rhr.user_name,str.c_str(),sizeof(rhr.user_name));
   str = passw->Text;
   strncpy(rhr.password,str.c_str(),sizeof(rhr.password));
   str = cl_lib->Text;
   rhr.page_size = this->PageSize->Text.ToInt();
   rhr.buffers   = this->Buffers->Text.ToInt();
   done_call_gkhandle(mod_iface.call(RHCM_RESTORE_DATABASE,(LPARAM)&rhr,(LPARAM)str.c_str()),NULL);
 }
 else
 {
  StatusBar1->Panels->Items[1]->Text = L"Укажите путь к БД, пользователя и пароль";
 }
}
//---------------------------------------------------------------------------

void __fastcall TRecHistroyForm::bStatisticClick(TObject *Sender)
{
   tabStats->TabVisible = true;
   PageControl1->ActivePage = tabStats;
   set_notify(mod_iface(),RECHISTORY_NOTIFY_STATS);
   mod_iface.call(RHCM_GET_STATISTICS,0,0);

}
//---------------------------------------------------------------------------

void __fastcall TRecHistroyForm::bMaintanceClick(TObject *Sender)
{
   mod_iface.call(RHCM_START_MAINTANCE,0,0);
}
//---------------------------------------------------------------------------

