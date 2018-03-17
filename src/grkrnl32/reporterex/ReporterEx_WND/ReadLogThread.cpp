//---------------------------------------------------------------------------
#pragma warn -8058
#include <vcl.h>
#pragma warn .8058
#pragma hdrstop

#include "ReadLogThread.h"
#include <GKReporterEx.h>
#include <gkreporter.h>
#include <DateUtils.hpp>
#include "BeepThread.h"
#include <stdio.h>
#include <algorithm>

#pragma package(smart_init)
using namespace KeRTL;

TReadLogThread::TReadLogThread():KeRTL::TThread()
{
  SetThreadPriority(THREAD_PRIORITY_LOWEST);
}

TReadLogThread::~TReadLogThread()
{
  TViews *v;
  for (unsigned i=0; i<v_logs.size(); i++)
  {
    v=v_logs[i];
    if (v->log) delete v->log;
    delete v;
  }
}

//---------------------------------------------------------------------------
int __fastcall TReadLogThread::Execute()
{
//  while (!CheckTerminateRequest())
//  {
//    for (int i=0; i<v_logs.size(); i++)
//    {
//      if (CheckTerminateRequest()) return 0;
//      TViews *v=v_logs[i];
//      if (!v->time_ind_ready)
//      {
//        for (int j=v->time_ind.size(); j<v->count; j++)
//        {
//          if (CheckTerminateRequest()) return 0;
//          UpdateTimeInd(v,j);
//          Sleep(200);
//        }
//      }
//      Sleep(200);
//    }
//  }
  return 0;
}

void __fastcall TReadLogThread::PreRead(int registred, bool last)
{
  for (UINT i=(last?v_logs.size()-1:0); !CheckTerminateRequest() && i<v_logs.size(); i++)
  {
    TViews *v=v_logs[i];
    if (!v->log)
    {
      v->log=new TFile(v->name.c_str(),OPEN_EXISTING,FILE_SHARE_READ | FILE_SHARE_WRITE,GENERIC_READ);
    }
    TLogHeader header;
    DWORD sz;
    DWORD dwRead;
    v->log->SeekRd(0,FILE_BEGIN);
    v->log->Read(&header,sizeof(header),dwRead,0);
    v->read_pos=header.last_rec;
    v->count=header.num_rec;
    if (header.first_rec==header.last_rec && header.first_rec==header.size)
      v->time_ind_ready=true;
    else
      v->time_ind_ready=false;
    if (i>=UINT(registred) && (header.sig!=Signature || header.version!=LogVersion || header.size!=sizeof(header)))
    {
      v->count=0;
      v->time_ind_ready=true;
    }
  }
}

DWORD __fastcall TReadLogThread::Read(TViews *v, DWORD pos, byte *buf, DWORD buf_size)
{
  if (!v->log) return 0;
  DWORD sz;
  bool bRead;
  DWORD dwRead;
  v->log->SeekRd(pos,FILE_BEGIN);
  bRead=v->log->Read(buf,sizeof(DWORD),dwRead,0);
  sz=*(DWORD *)buf;
  if (!bRead || (dwRead<sizeof(DWORD)) || (sz>buf_size))
    goto BadRead;
  if (sz)
  {
    bRead=v->log->Read(buf+sizeof(sz),sz-sizeof(sz),dwRead,0);
    if (!bRead || (dwRead<sz-sizeof(sz)))
      goto BadRead;
  }
  return dwRead+sizeof(sz);
BadRead:
  vector<TViews *>::iterator p;
  p=find(v_logs.begin(),v_logs.end(),v);
  UINT num=p-v_logs.begin();
  ClearLog(num);
  ReadError(Format("Ошибка чтения файла событий '%s'",ARRAYOFCONST((v->name))));
  return 0;
}

//---------------------------------------------------------------------------
DWORD __fastcall TReadLogThread::NewEvent(TViews *v, bool sound)
{
  TLogHeader header;
  DWORD sz;
  bool bRead;
  DWORD dwRead, pos;
  v->log->Close();
  v->log->Open(v->name.c_str(),OPEN_EXISTING,FILE_SHARE_READ | FILE_SHARE_WRITE,GENERIC_READ);
  v->log->Read(&header,sizeof(header),dwRead,0);
  pos=AddTimeInd(v,header.first_rec);
  if (pos)
  {
    v->count=header.num_rec;
    v->index++;
    if (sound)
    {
      v->log->SeekRd(pos,FILE_BEGIN);
      TMess mes;
      if (v->log->Read(&mes,sizeof(mes),dwRead) && dwRead==sizeof(mes))
        if (mes.type==REPORT_ERROR_TYPE)
          new TSpeakerBeep(false);
    }
  }
  return pos;
}

void __fastcall TReadLogThread::AddLog(TViews * v)
{
  v_logs.push_back(v);
}

void __fastcall TReadLogThread::InsertLog(UINT v_num, TViews *v)
{
  vector<TViews *>::iterator p;
  int notreg=FileRegistered(v->name);
  if (notreg>=0)
  {
    p=v_logs.begin();
    p+=notreg;
    if ((*p)->log) delete (*p)->log;
    if (*p) delete *p;
    v_logs.erase(p);
  }
  if (v_num<v_logs.size())
  {
    p=v_logs.begin();
    p+=v_num;
    v_logs.insert(p,v);
  } else
    v_logs.push_back(v);
  if (!v->log)
  {
    v->log=new TFile(v->name.c_str(),OPEN_EXISTING,FILE_SHARE_READ | FILE_SHARE_WRITE,GENERIC_READ);
  }
  TLogHeader header;
  DWORD sz;
  DWORD dwRead;
  v->log->SeekRd(0,FILE_BEGIN);
  if (v->log->Read(&header,sizeof(header),dwRead,0))
  {
    v->read_pos=header.last_rec;
    v->count=header.num_rec;
    if (header.first_rec==header.last_rec && header.first_rec==header.size)
      v->time_ind_ready=true;
    else
      v->time_ind_ready=false;
  }
}

void __fastcall TReadLogThread::ClearLog(UINT v_num)
{
  if (v_num<v_logs.size())
  {
    TViews *v=v_logs[v_num];
    TLogHeader header;
    DWORD sz;
    bool bRead;
    DWORD dwRead;
    v->log->Close();
    v->log->Open(v->name.c_str(),OPEN_EXISTING,FILE_SHARE_READ | FILE_SHARE_WRITE,GENERIC_READ);
    v->count=0;
    v->index=-1;
    v->time_ind.clear();
    v->time_ind_ready=false;
    v->read_pos=0;
    if (v->log->Read(&header,sizeof(header),dwRead,0))
    {
      v->read_pos=header.last_rec;
      v->count=header.num_rec;
      if (header.first_rec==header.last_rec && header.first_rec==header.size)
        v->time_ind_ready=true;
      //todo: clear other indexes
    }
  }
}

void __fastcall TReadLogThread::ReadError(AnsiString desc)
{
  TReportMes mes;
  mes.log=EVENT_SYSTEM;
  mes.source=(GKHANDLE)(*this)();
  mes.type=REPORT_ERROR_TYPE;
  mes.desc=new wchar_t[desc.WideCharBufSize()];
  desc.WideChar(const_cast<wchar_t*>(mes.desc),desc.WideCharBufSize());
  call_gkhandle_bynameW(REPORTER_NAME_W,RCM_MESSAGE_EVENT,LPARAM(&mes),0,0);
//  MessageBox(0,desc.c_str(),0,MB_OK);
  delete [] mes.desc;
}

DWORD __fastcall TReadLogThread::NextPosition(TViews *v, DWORD curpos, bool worward)
{
  if (!v->log) return 0;
  TLogHeader header;
  DWORD dwRead;
  v->log->SeekRd(0,FILE_BEGIN);
  if (!v->log->Read(&header,sizeof(header),dwRead)) return 0;
  if (!curpos) return header.first_rec;
  DWORD ret, dw;
  if (worward)
  {
    if (curpos==header.last_rec && v->time_ind.size()>1) return 0;
    v->log->SeekRd(curpos,FILE_BEGIN);
    if (!v->log->Read(&dw,sizeof(dw),dwRead)) return 0;
    ret=curpos+dw;
    if (ret>=v->log->GetSize() && ret>header.last_rec && header.file_size) ret=header.size;
    if (ret==header.last_rec) return 0;
  } else
  {
    if (curpos==header.first_rec && v->time_ind.size()>0) return 0;
    if (curpos==header.size) curpos=v->log->GetSize();
    v->log->SeekRd(curpos-sizeof(DWORD),FILE_BEGIN);
    if (!v->log->Read(&dw,sizeof(dw),dwRead)) return 0;
    ret=curpos-dw;
  }
  return ret;
}

void __fastcall TReadLogThread::UpdateTimeInd(TViews *v, int end)
{
  if (v->time_ind_ready) return;
  end=v->time_ind.size()-end-1;
  if (end>=0) return;
  DWORD pos;
  DWORD dwRead;
  TMess mes;
  if (v->time_ind.size())
    pos=v->time_ind[0];
  else
    pos=v->read_pos;
  for (int i=end; i<0; i++)
  {
    pos=NextPosition(v,pos,false);
    if (pos)
      v->time_ind.insert(v->time_ind.begin(),pos);
    else
      break;
  }
  if (v->time_ind.size()==UINT(v->count)) v->time_ind_ready=true;
}

DWORD __fastcall TReadLogThread::AddTimeInd(TViews *v, DWORD first_rec)
{
  DWORD pos;
  if (v->time_ind.size())
    pos=v->time_ind[v->time_ind.size()-1];
  else
    pos=0;
  pos=NextPosition(v,pos,true);
  vector<DWORD>::iterator p, p1=v->time_ind.begin();
  for (p=p1; p!=v->time_ind.end(); p++)
    if (*p==first_rec)
    {
      if (!v->time_ind_ready) v->time_ind_ready=true;
      if (p!=p1)
        v->time_ind.erase(p1,p);
      break;
    }
  if (pos)
    v->time_ind.push_back(pos);
  if (v->time_ind.size()==UINT(v->count)) v->time_ind_ready=true;
  return pos;
}

DWORD __fastcall TReadLogThread::ReadByTime(TViews *v, int num, byte *buf, DWORD buf_size, bool growth)
{
  DWORD pos;
  if (num<0) return 0;
  if (UINT(num)>=v->time_ind.size()) UpdateTimeInd(v,num);
  if (!growth)
    num=v->time_ind.size()-num-1;
  if (num<0) return 0;
  pos=v->time_ind[num];
  return Read(v,pos,buf,buf_size);
}
//---------------------------------------------------------------------------

AnsiString __fastcall TReadLogThread::SetUpDesc(TViews *v)
{
  AnsiString s,ret="";
  DWORD dw;
  const buf_size=65536;
  byte buf[buf_size];
  if (ReadByTime(v,v->index,buf,buf_size,false))
  {
    TMess *mes=(TMess *)buf;
    AnsiString s;
    if (mes->num_strings)
    {
      dw=mes->strings_offset;
      s=(wchar_t *)(buf+dw);
      dw+=s.WideCharBufSize()*sizeof(wchar_t);
    }
    for (DWORD i=1; i<mes->num_strings; i++)
    {
      s=(wchar_t *)(buf+dw);
      dw+=s.WideCharBufSize()*sizeof(wchar_t);
      if ((dw-sizeof(DWORD))>mes->length) break;
      ret+=s;
      ret+="\r\n";
    }
  }
  return ret;
}

AnsiString __fastcall TReadLogThread::SetUpData(TViews *v, int Code)
{
  AnsiString ret="";
  DWORD dw;
  const buf_size=65536;
  byte buf[buf_size];
  if (ReadByTime(v,v->index,buf,buf_size,false))
  {
    TMess *mes=(TMess *)buf;
    AnsiString s;
    if (mes->data_length)
    {
      dw=mes->data_offset-sizeof(DWORD);
      *(DWORD *)(buf+dw)=mes->data_length;
      if (v->log_function && !IsBadCodePtr((FARPROC)(v->log_function)))
      {
        const DWORD sz=32656;
        wchar_t wstr[sz]=L"";
        FillMemory(wstr,sz*sizeof(wchar_t),1);
        int len=((ReportDataToStrW)(v->log_function))(buf+dw,wstr,sz,Code);
        if (len<0)
        {
          wchar_t *wstr1=new wchar_t[-len];
          ((ReportDataToStrW)(v->log_function))(buf+dw,wstr1,(DWORD)-len,Code);
          if (wstr1[len-1])
            ret=AnsiString((char *)wstr1);
          else
            ret=AnsiString(wstr1);
          delete [] wstr1;
        }
        else if (NULL==len)
          ret=ToStringByte(buf+dw,Code);
        else
        {
          if (wstr[len-1])
            ret=AnsiString((char *)wstr);
          else
            ret=AnsiString(wstr);
        }
      }
      else
        ret=ToStringByte(buf+dw,Code);
    }
  }
  return ret;
}

AnsiString __fastcall TReadLogThread::ToStringByte(LPVOID Data, int Code)
{
  AnsiString s="",s1,s2,f1,f2;
  int w;
  if (!Data) return s;
  DWORD sz=*reinterpret_cast<DWORD *>(Data);
  BYTE *PData=reinterpret_cast<BYTE *>((BYTE *)Data+sizeof(sz));
  BYTE b;
  DWORD i=0;
  switch (Code)
  {
    case 8: f1="%04o:"; f2=" %03o"; w=40; break;
    case 10: f1="%04u:"; f2=" %03u"; w=40; break;
    case 16: f1="%04X:"; f2=" %02X"; w=32; break;
    default: f1="%04X:"; f2=" %02X"; w=32;
  }
  while (i<sz)
  {
    s1.sprintf(f1.c_str(),i);
    s2="";
    for (DWORD j=0; j<8; j++)
    {
      if (i==sz) break;
      b=*(PData+i);
      s1.cat_sprintf(f2.c_str(),b);
      s2+=(b<32)?'.':(char)b;
      i++;
    }
    while (s1.Length()<w) s1+=" ";
    s+=s1+s2+"\r\n";
  }
  return s;
}

DWORD __fastcall TReadLogThread::GetSize(UINT v_num)
{
  DWORD ret=0;
  if (v_num<v_logs.size())
  {
    TLogHeader header;
    DWORD dwRead;
    v_logs[v_num]->log->SeekRd(0,FILE_BEGIN);
    if (v_logs[v_num]->log->Read(&header,sizeof(header),dwRead)) ret=header.file_size;
  }
  return ret;
}

int __fastcall TReadLogThread::FileRegistered(AnsiString name)
{
  for (UINT i=0; i<v_logs.size(); i++)
    if (v_logs[i]->name==name)
      return i;
  return -1;
}

void __fastcall TReadLogThread::SetEventFunc(DWORD source, DWORD dest)
{
  if (source>=v_logs.size() || dest>=v_logs.size()) return;
  v_logs[dest]->log_function=v_logs[source]->log_function;
}

