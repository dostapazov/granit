//---------------------------------------------------------------------------
#pragma warn -8058
#include <vcl.h>
#pragma warn .8058

#pragma hdrstop

#include <DateUtils.hpp>
#include <string>
#include "ReporterEx_frm.h"
#include <gkreporter.h>
#include <GKReporterEx.h>
#include "Property_frm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "GKMODULE_FORM"
#pragma link "gkmodule_form"
#pragma link "gkmodule_form"
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TReporterExFrm::TReporterExFrm(TComponent* Owner,HWND owner_hwnd,GKHANDLE handle)
        : TGKModuleForm(Owner,owner_hwnd,handle)
{
  Code=16;
}
//---------------------------------------------------------------------------

__fastcall TReporterExFrm::~TReporterExFrm()
{
  if (ReadLogThread)
    Close();
}

void  __fastcall TReporterExFrm::setup_form_sizes()
{
  TGKModuleForm::setup_form_sizes();
  TreeView->Width = Width/4;
}


void __fastcall TReporterExFrm::FormShow(TObject *Sender)
{
  TbSound->Down=mod_iface.call(RCM_MESSAGE_SOUND,0,0);
  ReadLogThread=new TReadLogThread();
  AnsiString dir= GetCurrentDir();
  //ExtractFileDir(Application->ExeName)+"\\log";
  dir+="\\log\\";
  AnsiString ext=".evt";
  if (!DirectoryExists(dir)) return;
  TEventNameA names;
  names.s_short=new char[MAX_PATH];
  names.s_full=new char[MAX_PATH];
  names.sz_short=MAX_PATH;
  names.sz_full=MAX_PATH;
  UINT i=0;
  DWORD ret;
  TViews *v;
  while (!mod_iface.call(RCM_GET_MSG_NAME_A,i++,(long)&names))
  {
    v=new TViews;
    v->name=dir+"\\"+names.s_short+ext;
    v->log=NULL;
    v->index=-1;
    v->count=0;
    v->log_function=(LPVOID)mod_iface.call(RCM_GET_REGISTER_FUNC,(LPARAM)-1,(LPARAM)names.s_short);
    ReadLogThread->AddLog(v);
    if (strlen(names.s_full))
      TreeView->Items->AddChildObject(TreeView->Items->Item[0],names.s_full,v);
    else
      TreeView->Items->AddChildObject(TreeView->Items->Item[0],names.s_short,v);
  }
  delete [] names.s_short;
  delete [] names.s_full;
//  подключать старые логи
  TSearchRec sr;
  TTreeNode *node=TreeView->Items->Item[TreeView->Items->Count-1];
  if (FindFirst(dir+"\\*.evt", faAnyFile, sr) == 0)
  {
    AnsiString s, s_full;
    do
    {
      s=ChangeFileExt(sr.Name,"");
      s_full=dir+"\\"+sr.Name;
      if (ReadLogThread->FileRegistered(s_full)<0)
      {
        v=new TViews;
        v->name=s_full;
        v->log=NULL;
        v->index=-1;
        v->count=0;
        v->log_function=0;
        ReadLogThread->AddLog(v);
        TreeView->Items->AddChildObject(node,s,v);
      }
    } while (FindNext(sr) == 0);
    FindClose(sr);
  }
  if (node->HasChildren) node->Expand(false);
  set_notify(mod_handle,MNF_REPORTEREX);
  ReadLogThread->Start(0,true);
  ReadLogThread->PreRead(TreeView->Items->Item[0]->Count);
  ReadLogThread->Resume();
  TreeView->Items->Item[0]->Item[0]->Selected=true;
}

void __fastcall TReporterExFrm::FormClose(TObject *Sender,
      TCloseAction &Action)
{
  if (ReadLogThread)
  {
    ReadLogThread->Terminate();
    while (ReadLogThread->IsRunning())
      Application->HandleMessage();
    delete ReadLogThread;
    ReadLogThread=NULL;
  }
  for (int i=0; i<TreeView->Items->Count; i++)
    TreeView->Items->Item[i]->DeleteChildren();
  reset_notify(mod_handle,MNF_REPORTEREX);
}
//---------------------------------------------------------------------------

void __fastcall TReporterExFrm::ListViewDataHint(TObject *Sender,
      int StartIndex, int EndIndex)
{
  if (TreeView->Selected->Data)
    ReadLogThread->UpdateTimeInd((TViews *)(TreeView->Selected->Data),EndIndex);
}
//---------------------------------------------------------------------------

AnsiString __fastcall format_date(__int64 value)
{
      char date_time[MAX_PATH];
      date_time[0] = 0;
      if(value)
      {
        SYSTEMTIME st;
        FileTimeToSystemTime((LPFILETIME)&value,&st);
        GetDateFormat(LOCALE_USER_DEFAULT,DATE_SHORTDATE,&st,NULL,date_time,sizeof(date_time));
       }
    return AnsiString(date_time);
}

AnsiString __fastcall format_time(__int64 value)
{
      char date_time[MAX_PATH];
      date_time[0] = 0;
      if(value)
      {
        SYSTEMTIME st;
        FileTimeToSystemTime((LPFILETIME)&value,&st);
        int dtl=GetTimeFormat(LOCALE_USER_DEFAULT,0,&st,"HH':'mm':'ss",date_time,sizeof(date_time));
        dtl--;
        wsprintf(date_time+dtl,",%03d",(DWORD)st.wMilliseconds);
       }
    return AnsiString(date_time);
}

void __fastcall TReporterExFrm::ListViewData(TObject *Sender,
      TListItem *Item)
{
  TViews *v=(TViews *)(TreeView->Selected->Data);
  if (!v) return;
  if (v->count<=Item->Index) return;
  if (ListView->Items->Count!=v->count) ListView->Items->Count=v->count;
  TMess *mes;
  DWORD dw;
  const buf_size=65536;
  byte buf[buf_size];
  if (ReadLogThread->ReadByTime(v,Item->Index,buf,buf_size,false))
    mes=(TMess *)buf;
  else
    return;
  AnsiString s;
  int t=-1;
  switch (mes->type)
  {
    case REPORT_SUCCESS_TYPE: s="Успешно"; t=0; break;
    case REPORT_INFORMATION_TYPE: s="Уведомление"; t=0; break;
    case REPORT_WARNING_TYPE: s="Предупреждение"; t=1; break;
    case REPORT_ERROR_TYPE: s="Ошибка"; t=2; break;
    default: s="Неизвестно";
  }
  Item->Caption=s;
  Item->ImageIndex=t;
  Item->SubItems->Add(format_date(mes->time));
  Item->SubItems->Add(format_time(mes->time));
  dw=mes->strings_offset;
  for (DWORD i=0; i<min((DWORD)2,mes->num_strings); i++)
  {
    s=(wchar_t *)(buf+dw);
    dw+=s.WideCharBufSize()*sizeof(wchar_t);
    if ((dw-sizeof(DWORD))>mes->length) return;
    int j;
    while ((j=s.Pos("\r\n"))>0)
    {
      Item->SubItems->Add(s.SubString(1,j-1));
      s.Delete(1,j+1);
    }
    if (s.Length()) Item->SubItems->Add(s);
  }
}
//---------------------------------------------------------------------------


void __fastcall TReporterExFrm::TreeViewChange(TObject *Sender,
      TTreeNode *Node)
{
  ListView->Clear();
  MemoDesc->Clear();
  MemoData->Clear();
  TViews *v=(TViews *)(Node->Data);
  if (v && v->count)
  {
    ListView->Items->Count=v->count;
    if (v->index<0)
      ListView->ItemIndex=0;
    else
    {
      int index=v->index;
      v->index=-1;
      ListView->ItemIndex=index;
      if (ListView->ItemIndex>=ListView->VisibleRowCount)
        ListView->Scroll(0,17*(ListView->ItemIndex-ListView->VisibleRowCount+1));
    }
  } else
  {
    ListView->Items->Count=0;
    StatusBar->Panels->Items[2]->Text="";
  }
  if (!TreeView->Selected->Level || TreeView->Selected->Parent->Index)
  {
    TbClear->Enabled=false;
    TbProperty->Enabled=false;
  } else
  {
    TbClear->Enabled=true;
    TbProperty->Enabled=true;
  }
}
//---------------------------------------------------------------------------

void __fastcall TReporterExFrm::ListViewChange(TObject *Sender,
      TListItem *Item, TItemChange Change)
{
  if (Item && Item->Selected && Change==ctState)
  {
    TViews *v=(TViews *)(TreeView->Selected->Data);
    if (v->index!=Item->Index)
    {
      v->index=Item->Index;
      MemoDesc->Lines->Text=ReadLogThread->SetUpDesc(v);
      MemoData->Lines->Text=ReadLogThread->SetUpData(v,Code);
      StatusBar->Panels->Items[2]->Text=IntToStr(v->index+1)+"/"+IntToStr(v->count);
    }
  }
}
//---------------------------------------------------------------------------

void __fastcall TReporterExFrm::TbCloseClick(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------

void __fastcall TReporterExFrm::TbClearClick(TObject *Sender)
{
  if (!TreeView->Selected->Parent->Index)
    if (MessageBox(0,"Стереть все сообщения из данного файла?","Очистка лог-файла",MB_YESNO | MB_ICONQUESTION | MB_TASKMODAL)==IDYES)
    {
      call_gkhandle_bynameW(REPORTER_NAME_W,RCM_CLEAR_EVENT,(LPARAM)TreeView->Selected->Index,0,0);
    }
}
//---------------------------------------------------------------------------

void __fastcall TReporterExFrm::TbSoundClick(TObject *Sender)
{
  mod_iface.call(RCM_MESSAGE_SOUND,(LPARAM)(TbSound->Down),1);
}
//---------------------------------------------------------------------------

void __fastcall TReporterExFrm::FormResize(TObject *Sender)
{
  StatusBar->Panels->Items[1]->Width=StatusBar->Width-250;
  ResizeView();
}
//---------------------------------------------------------------------------

LRESULT __fastcall TReporterExFrm::on_gkhandle_notify (GKHANDLE from,LPNOTIFY_CODE nc,DWORD mask)
{
  if (mask==MNF_REPORTEREX)
  {
    switch (nc->notify_code)
    {
      case MNCODE_REPORT_ADDED: on_report_added(nc);break;
      case MNCODE_REPORT_REFRESH: on_report_refresh(nc);break;
      case MNCODE_REPORT_NEWEVENT: on_report_newevent(nc);break;
      default: TGKModuleForm::on_gkhandle_notify(from,nc,mask);
    }
  }
  return GKH_RET_SUCCESS;
}
//---------------------------------------------------------------------------

void __fastcall TReporterExFrm::on_report_added(LPNOTIFY_CODE nc)
{
  TViews *v=(TViews *)(TreeView->TopItem->Item[nc->notify_data[0]]->Data);
  DWORD pos;
  if (!v) return;
  bool active=(TreeView->Selected->Data==v);
  bool sound=false;
  if (active && !Timer->Enabled) sound=TbSound->Down;
  if (sound) Timer->Enabled=true;
  pos=ReadLogThread->NewEvent(v,sound);
  if (pos)
  {
    if (active)
    {
      if (v->index<v->count)
      {
        int index=v->index;
        v->index=-1;
        ListView->ItemIndex=index;
      } else
      {
        ListView->ItemIndex=-1;
        ListView->ItemIndex=v->count-1;
      }
      if (ListView->TopItem->Index) ListView->Scroll(0,17);
      StatusBar->Panels->Items[2]->Text=IntToStr(v->index+1)+"/"+IntToStr(v->count);
      ListView->Refresh();
    }
    TMess *mes;
    DWORD dw;
    const buf_size=65536;
    byte buf[buf_size];
    if (ReadLogThread->Read(v,pos,buf,buf_size))
      mes=(TMess *)buf;
    else
      return;
    AnsiString s;
    int j;
    dw=mes->strings_offset;
    for (DWORD i=0;mes->length && i<min((DWORD)2,mes->num_strings); i++)
    {
      s=(wchar_t *)(buf+dw);
      dw+=s.WideCharBufSize()*sizeof(wchar_t);
      if ((dw-sizeof(DWORD))>mes->length) return;
      j=s.Pos("\r\n");
      if (j<1) j=s.Length()+1;
      StatusBar->Panels->Items[i]->Text=s.SubString(1,j-1);
    }
  }
}

void __fastcall TReporterExFrm::on_report_refresh(LPNOTIFY_CODE nc)
{
  ReadLogThread->ClearLog(nc->notify_data[0]);
  if (DWORD(TreeView->Selected->Index)==nc->notify_data[0] && !TreeView->Selected->Parent->Index)
  {
    ListView->Clear();
    MemoDesc->Clear();
    MemoData->Clear();
    StatusBar->Panels->Items[2]->Text="";
    TViews *v=(TViews *)TreeView->Selected->Data;
    ListView->Items->Count=v->count;
    if (v->count) ListView->ItemIndex=0;
  }
}

void __fastcall TReporterExFrm::on_report_newevent(LPNOTIFY_CODE nc)
{
  AnsiString dir=ExtractFileDir(Application->ExeName)+"\\log";
  AnsiString ext=".evt";
  TEventNameA names;
  names.s_short=new char[MAX_PATH];
  names.s_full=new char[MAX_PATH];
  names.sz_short=MAX_PATH;
  names.sz_full=MAX_PATH;
  UINT i=nc->notify_data[0];
  DWORD ret;
  TViews *v;
  if (!mod_iface.call(RCM_GET_MSG_NAME_A,i,(long)&names))
  {
    v=new TViews;
    v->name=dir+"\\"+names.s_short+ext;
    v->log=NULL;
    v->index=-1;
    v->count=0;
    v->read_pos=0;
    v->log_function=(LPVOID)mod_iface.call(RCM_GET_REGISTER_FUNC,(LPARAM)-1,(LPARAM)names.s_short);
    int notreg=ReadLogThread->FileRegistered(v->name);
    if (notreg>=0)
    {
      TTreeNode *node=TreeView->Items->Item[TreeView->Items->Count-1];
      if (node->Level) node=node->Parent;
      notreg-=TreeView->Items->Item[0]->Count;
      node->Item[notreg]->Delete();
    }
    ReadLogThread->InsertLog(i,v);
    if (strlen(names.s_full))
      TreeView->Items->AddChildObject(TreeView->Items->Item[0],names.s_full,v);
    else
      TreeView->Items->AddChildObject(TreeView->Items->Item[0],names.s_short,v);
  }
  delete [] names.s_short;
  delete [] names.s_full;
}

//---------------------------------------------------------------------------

void __fastcall TReporterExFrm::TimerTimer(TObject *Sender)
{
  Timer->Enabled=false;
}
//---------------------------------------------------------------------------

void __fastcall TReporterExFrm::TbPropertyClick(TObject *Sender)
{
  if (TreeView->Selected->Parent->Index) return;
  UINT num=TreeView->Selected->Index;
  DWORD old_sz=ReadLogThread->GetSize(num);
  FrmProp=new TFrmProp(this,TreeView->Selected->Text,old_sz,(TViews *)TreeView->Selected->Data);
  if (FrmProp->ShowModal()==mrOk)
  {
    DWORD sz=FrmProp->GetSize();
    if (old_sz!=sz) mod_iface.call(RCM_MESSAGE_FILESIZE,num,sz);
  }
  delete FrmProp;
  FrmProp=NULL;
}
//---------------------------------------------------------------------------

void __fastcall TReporterExFrm::ResizeView()
{
  int w=0;
  for (int i=0; i<ListView->Columns->Count-1; i++) w+=ListView->Columns->Items[i]->Width;
  if (ListView->VisibleRowCount<ListView->Items->Count) w+=16;
  w=ListView->Width-w-4;
  if (w<70) w=70;
  ListView->Columns->Items[ListView->Columns->Count-1]->Width=w;
}
//---------------------------------------------------------------------------

void __fastcall TReporterExFrm::Tb8Click(TObject *Sender)
{
  Code=8;
  if (MemoData->Lines->Text.Length())
  {
    TViews *v=(TViews *)(TreeView->Selected->Data);
    MemoData->Lines->Text=ReadLogThread->SetUpData(v,Code);
  }
}
//---------------------------------------------------------------------------

void __fastcall TReporterExFrm::Tb10Click(TObject *Sender)
{
  Code=10;
  if (MemoData->Lines->Text.Length())
  {
    TViews *v=(TViews *)(TreeView->Selected->Data);
    MemoData->Lines->Text=ReadLogThread->SetUpData(v,Code);
  }
}
//---------------------------------------------------------------------------

void __fastcall TReporterExFrm::Tb16Click(TObject *Sender)
{
  Code=16;
  if (MemoData->Lines->Text.Length())
  {
    TViews *v=(TViews *)(TreeView->Selected->Data);
    MemoData->Lines->Text=ReadLogThread->SetUpData(v,Code);
  }
}
//---------------------------------------------------------------------------

void __fastcall TReporterExFrm::Splitter2CanResize(TObject *Sender,
      int &NewSize, bool &Accept)
{
  if (NewSize<31) NewSize=31;
}
//---------------------------------------------------------------------------

void __fastcall TReporterExFrm::Splitter3CanResize(TObject *Sender,
      int &NewSize, bool &Accept)
{
  if (NewSize<31) NewSize=31;
}
//---------------------------------------------------------------------------

void __fastcall TReporterExFrm::AClearAllExecute(TObject *Sender)
{
  for (int i=0; i<TreeView->TopItem->Count; i++)
    call_gkhandle_bynameW(REPORTER_NAME_W,RCM_CLEAR_EVENT,(LPARAM)i,0,0);
}
//---------------------------------------------------------------------------

void __fastcall TReporterExFrm::ToolButton6Click(TObject *Sender)
{
  AnsiString dir=ExtractFileDir(Application->ExeName)+"\\log";
  AnsiString ext=".evt";
  if (!DirectoryExists(dir)) return;
  if (OpenDialog->InitialDir.IsEmpty())
    OpenDialog->InitialDir=dir;
  if (OpenDialog->Execute())
  {
    TViews *v;
    TTreeNode *node=TreeView->Items->Item[TreeView->Items->Count-1];
    while (node->Parent)
      node=node->Parent;
    v=new TViews;
    v->name=OpenDialog->FileName;
    v->log=NULL;
    v->index=-1;
    v->count=0;
    v->log_function=0;
    node=TreeView->Items->AddChildObject(node,OpenDialog->FileName,v);
    if (node->Parent->HasChildren) node->Parent->Expand(false);
    ReadLogThread->AddLog(v);
    ReadLogThread->PreRead(0,true);
    node->Selected=true;
  }
}
//---------------------------------------------------------------------------


void __fastcall TReporterExFrm::TreeViewDragDrop(TObject *Sender,
      TObject *Source, int X, int Y)
{
  TTreeNode *node1, *node2;
  node1=((TTreeView *)Source)->Selected;
  if (!node1 || !node1->Parent) return;
  if (node1->Parent!=TreeView->Items->Item[0]) return;
  node2=((TTreeView *)Source)->GetNodeAt(X,Y);
  ReadLogThread->SetEventFunc(node1->AbsoluteIndex-1,node2->AbsoluteIndex-2);
  node2->Selected=true;
}
//---------------------------------------------------------------------------



void __fastcall TReporterExFrm::TreeViewDragOver(TObject *Sender,
      TObject *Source, int X, int Y, TDragState State, bool &Accept)
{
  TTreeNode *node=((TTreeView *)Source)->GetNodeAt(X,Y);
  Accept=false;
  if (!node) return;
  if (node->Parent==TreeView->Items->Item[0]) return;
  if (!node->Parent) return;
  Accept=true;
}
//---------------------------------------------------------------------------

