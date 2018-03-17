//---------------------------------------------------------------------------
#pragma warn -8058
#include <system.hpp>
#pragma warn .8058
#include <io.h>
#include <stdio.h>
#include <dir.h>
#include <SysUtils.hpp>
#include <DateUtils.hpp>
#include <string>

#pragma hdrstop

#include "GKReporterEx.hpp"
#include <gkreporter.h>
#include <tregstry.hpp>
#include <grkernel.h>
#include <algorithm>
//#include <tregstry.hpp>

//---------------------------------------------------------------------------

TGKReporterEx::TGKReporterEx():TGKStdReporter(true)
{
  lock_param = GKHB_AUTO_LOCK_OFF; 
  alloc_gkhandle(REPORTEREX_NAME_W,&tag);
}

TGKReporterEx::~TGKReporterEx()
{
  Tlog_files::iterator p;
  for (p=log_files.begin(); p!=log_files.end(); p++)
    delete *p;
}


LRESULT __fastcall TGKReporterEx::processing(DWORD cmd,LPARAM p1,LPARAM p2)
{
  LRESULT ret;
  switch(cmd)
  {
    case RCM_SET_REGISTER_FUNC:
      ret=SetRegisterFunc((UINT)p1,(LPVOID)p2);
      break;
    case RCM_GET_REGISTER_FUNC:
      ret=(LRESULT)GetRegisterFunc((UINT)p1,(char *)p2);
      break;
    case RCM_GET_MSG_NAME_A:
      ret=GetMessageNameA((UINT)p1,(TEventNameA *)p2);
      break;
    case RCM_GET_MSG_NAME_W:
      ret=GetMessageNameW((UINT)p1,(TEventNameW *)p2);
      break;
    case RCM_CLEAR_EVENT:
      ret=ClearEvent((UINT)p1);
      break;
    case MCMD_GETMODULE_INFO:
      ret=get_module_info(LPMODULE_INFO(p1));
      break;
    case RCM_MESSAGE_SOUND:
      if ((BOOL)p2)
      {
        sound=(BOOL)p1;
        cc_flags|=CCFL_MODULE_CONFIG;
        ret=GKH_RET_SUCCESS;
      }
      else
        ret=sound;
      break;
    case RCM_MESSAGE_FILESIZE:
      ret=ChangeFileSize(UINT(p1),DWORD(p2));
      break;
    default:
      ret = TGKStdReporter::processing(cmd,p1,p2);
      break;
  }
  return ret;
}

DWORD __fastcall TGKReporterEx::start(DWORD reason,LPARAM p2)
{
  license_type = MOD_LICENSE_REGISTERED;
  DWORD ret ;
  wchar_t buf[MAX_PATH];
  if (get_gkhandle_nameW(_handle,buf,sizeof(buf)/sizeof(wchar_t)) && (wcscmp(buf,REPORTER_NAME_W)!=0))
  {
    if (find_gkhandleW(REPORTER_NAME_W))
    {
      ret=call_owner(CMGK_DELETE_SDTREPORTER,(LPARAM)_handle,0);
      if (ret!=GKH_RET_SUCCESS) return ret;
    }
    if(set_gkhandle_nameW(_handle,REPORTER_NAME_W,tag) && this->need_notify(MNF_COMMON))
    {
      NOTIFY_CODE nc;
      nc.dw_size=sizeof(nc);
      nc.from_module=_handle;
      nc.notify_code=MNCODE_RENAMED_MODULE;
      nc.notify_data[0]=(DWORD)_handle;
      notify_handles(&nc,MNF_COMMON);
      nc.notify_code=MNCODE_RESORT_MODULES;
      notify_handles(&nc,MNF_COMMON);
    }
  }
  ret = TGKStdReporter::start(reason,p2);
  if (!ret)
  {
    report(EVENT_SYSTEM,REPORT_INFORMATION_TYPE,L"Расширенный репортёр запущен");
  }
  return ret;
}

DWORD __fastcall TGKReporterEx::stop(DWORD reason)
{
  DWORD ret;
  
  if(reason==MODULE_STOP_REQUEST || reason==MODULE_STOP_PROGRAM_SHUTDOWN)
  {
    report(EVENT_SYSTEM,REPORT_INFORMATION_TYPE,L"Расширенный репортёр остановлен");
    ret=TGKModule::stop(reason);
    if (!ret)
    {
      NOTIFY_CODE nc;
      nc.dw_size=sizeof(nc);
      nc.from_module=_handle;
      nc.notify_data[0]=(DWORD)_handle;
      if (set_gkhandle_nameW(_handle,REPORTEREX_NAME_W,tag) && this->need_notify(MNF_COMMON))
      {
        nc.notify_code=MNCODE_RENAMED_MODULE;
        notify_handles(&nc,MNF_COMMON);
      }
      if(reason==MODULE_STOP_REQUEST)
      {
        ret=call_owner(CMGK_CREATE_STDREPORTER,(LPARAM)_handle,0);

        if (!ret)
        {
          nc.notify_code=MNCODE_RESORT_MODULES;
          notify_handles(&nc,MNF_COMMON);
        }
      }
    }
  }
  
  return ret;
}

void __fastcall TGKReporterEx::write_strings(HANDLE fout,DWORD flags,char * str1,char * str2)
{
  TReportMes mes;
  WideString ws;
  mes.source=INVALID_GKHANDLE_VALUE;
  mes.log=0;
  mes.type=0;
  ws=str1;
  if (str2)
    ws+=WideString(" - ")+WideString(str2);
  mes.desc=ws.c_bstr();
  write_struct(fout,flags,&mes,0);
}

void __fastcall TGKReporterEx::write_struct(HANDLE fout,DWORD flags,TReportMes *mes,LPARAM p2)
{
  TMess event;
  #define BUF_SIZE 65535
  byte buf[BUF_SIZE];
  wchar_t bufW[MAX_PATH];
  if (mes->source!=INVALID_GKHANDLE_VALUE)
  {
    TGKModuleInterface mi(mes->source,true);
    bufW[0]=0;
    mi.get_module_name(bufW,sizeof(bufW)/sizeof(wchar_t));
    if (!wcslen(bufW))
    {
      get_gkhandle_nameW(mes->source,bufW,wcslen(bufW));
      if (!wcslen(bufW))
        swprintf(bufW,L"GKHANDLE 0x%X",mes->source);
    }
  }
  else
    swprintf(bufW,L"?");
  event.time=GetTime(false);
  event.type=mes->type;
  event.num_strings=2;
  FillBuffer(buf,BUF_SIZE,&event,bufW,mes->desc,mes->data_size,mes->data);
  WriteToFile(mes->log,buf,event.length);
}

void __fastcall TGKReporterEx::write_exception  (DWORD flags,LPEXCEPTION_POINTERS ep,char * text)
{
  TMess event;
  #define BUF_SIZE 65535
  byte buf[BUF_SIZE];
  wchar_t bufW[MAX_PATH<<1];
  WideString s1,s2="",s3;
  event.time=GetTime(false);
  event.type=REPORT_ERROR_TYPE;
  event.num_strings=3;
  s1="Системное исключение";
  MEMORY_BASIC_INFORMATION mbi;
  #ifdef _DEBUG
  ZeroMemory(&mbi,sizeof(mbi));
  #endif;
  VirtualQuery(ep->ExceptionRecord->ExceptionAddress,&mbi,sizeof(mbi));
  GetModuleFileNameW((HINSTANCE)mbi.AllocationBase,bufW,sizeof(bufW)/sizeof(wchar_t));
  s2="в модуле : "+WideString(bufW);
  if (text) s3=WideString(text);
  exception_pointers myep;
  CopyMemory(&(myep.except_rec),ep->ExceptionRecord,sizeof(myep.except_rec));
  CopyMemory(&(myep.except_ctx),ep->ContextRecord,sizeof(myep.except_ctx));
  memcpy(myep.ebp_dump,(LPVOID)myep.except_ctx.Ebp,sizeof(myep.ebp_dump));
  memcpy(myep.esp_dump,(LPVOID)myep.except_ctx.Esp,sizeof(myep.esp_dump));

  FillBuffer(buf,BUF_SIZE,&event,s1.c_bstr(),s2.c_bstr(),s3.c_bstr(),sizeof(myep),&myep);
  WriteToFile(EVENT_EXCEPTIONS,buf,event.length);
}

char* __fastcall get_command_string(DWORD code)
{
//todo: вывод вербальной информации о номере команды
  char *str = "";
  switch(code)
  {
    case EXCEPTION_ACCESS_VIOLATION      : str = "Нарушение прав доступа";break;
    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED : str = "Выход за границы массива";break;
  }
  return str;
}

void __fastcall TGKReporterEx::write_gkexception(lpexcept_record er,char * text)
{
  TMess event;
  #define BUF_SIZE 65535
  byte buf[BUF_SIZE];
  wchar_t bufW[MAX_PATH<<1];
  WideString s1,s2="",s3,s4;
  event.time=GetTime(false);
  event.type=REPORT_ERROR_TYPE;
  event.num_strings=4;
  TGKModuleInterface mi(er->handle,true);
  mi.get_module_name(bufW,sizeof(bufW)/sizeof(wchar_t));
  if (!wcslen(bufW))
  {
    get_gkhandle_nameW(er->handle,bufW,wcslen(bufW));
    if (!wcslen(bufW))
      swprintf(bufW,L"GKHANDLE 0x%X",er->handle);
  }
  s1=WideString(bufW);
  swprintf(bufW,L"cmd = 0x%X p1 = 0x%X p2 = 0x%X",er->cmd,er->p1,er->p2);
  s2=WideString(bufW);
  MEMORY_BASIC_INFORMATION mbi;
  #ifdef _DEBUG
  ZeroMemory(&mbi,sizeof(mbi));
  #endif;
  VirtualQuery(er->except_rec.ExceptionAddress,&mbi,sizeof(mbi));
  GetModuleFileNameW((HINSTANCE)mbi.AllocationBase,bufW,sizeof(bufW)/sizeof(wchar_t));
  s3="в модуле : "+WideString(bufW);
  if (text) s4=WideString(text);
  exception_pointers myep;
  myep.except_rec=er->except_rec;
  myep.except_ctx=er->except_ctx;
  memcpy(myep.ebp_dump,er->ebp_dump,sizeof(myep.ebp_dump));
  memcpy(myep.esp_dump,er->esp_dump,sizeof(myep.esp_dump));
  FillBuffer(buf,BUF_SIZE,&event,s1.c_bstr(),s2.c_bstr(),s3.c_bstr(),s4.c_bstr(),sizeof(myep),&myep);
  WriteToFile(EVENT_EXCEPTIONS,buf,event.length);
}

void TGKReporterEx::FillBuffer(byte *buf, DWORD length, TMess *event, ...)
//остальные параметры:
//   wchar_t string[],
//   DWORD data_size,
//   LPVOID data[]
{
  //заполнить в структуре поля:
  //    length
  //    strings_offset
  //    data_length
  //    data_offset
  //и всё записать в буфер
  va_list ap;
  wchar_t *s;
  char *data;
  DWORD l=0,dw,len;
  l=sizeof(*event);
  event->strings_offset=l;
  va_start(ap, event);
  for (DWORD i=0; i<event->num_strings; i++)
  {
    s=va_arg(ap,wchar_t *);
    if (!s) s=L"\0";
    len=(wcslen(s)+1)*sizeof(wchar_t);
    if ((l+len+sizeof(event->length))<length)
    {
      data=(char *)s;
      CopyMemory(buf+l,data,len);
      l+=len;
    }
    else
      event->num_strings--;
  }
  dw=va_arg(ap,DWORD);
  event->data_length=dw;
  event->data_offset=l;
  if (dw)
  {
    if ((l+dw+sizeof(event->length))<length)
    {
      data=va_arg(ap,char *);
      CopyMemory(buf+l,data,dw);
      l+=dw;
    }
    else
      event->data_length=0;
  }
  va_end(ap);
  dw=sizeof(DWORD)-(l%sizeof(DWORD));
  if (dw<sizeof(DWORD))
  {
    len=0;
    data=(char *)&len;
    CopyMemory(buf+l,data,dw);
    l+=dw;
  }
  l+=sizeof(l);
  event->length=l;
  data=(char *)&l;
  CopyMemory(buf+l-sizeof(l),data,sizeof(l));
  data=(char *)event;
  CopyMemory(buf,data,sizeof(*event));
}

void __fastcall TGKReporterEx::WriteToFile(UINT log_name, byte * buf, DWORD sz)
{
  TLockHelper l(locker);
  TFile *log;
  TLogHeader header;
  if(!log_files.size())
     return;
  if (log_name>=log_files.size()) log_name=0;
  log=log_files[log_name];
  log->SeekRd(0,FILE_BEGIN);
  DWORD dw,rdw;
  log->Read(&header,sizeof(header),rdw);
  if (header.file_size)        //циклический файл
  {
    if (header.first_rec>=header.last_rec && header.first_rec>header.size) //конец раньше начала
    {
      if ((header.last_rec+sz)>header.first_rec) //новая запись перекрывает начало
      {
        do
        {
          log->SeekRd(header.first_rec,FILE_BEGIN);
          log->Read(&dw,sizeof(dw),rdw);
          header.first_rec+=dw;
          header.num_rec--;
        } while (header.num_rec && ((header.last_rec+sz)>header.first_rec) && (header.first_rec<log->GetSize()));
        if (header.first_rec>=log->GetSize())
          header.first_rec=sizeof(header);
        if ((header.last_rec+sz)>header.file_size) //новая запись выходит за границу файла
        {
          log->SetSize(header.last_rec);
          log->Flush();
          header.last_rec=sizeof(header);
          do
          {
            log->SeekRd(header.first_rec,FILE_BEGIN);
            log->Read(&dw,sizeof(dw),rdw);
            header.first_rec+=dw;
            header.num_rec--;
          } while (((header.last_rec+sz)>header.first_rec));
        } else
        {
          if (header.first_rec==header.size)
          {
            log->SetSize(header.last_rec);
            log->Flush();
          }
        }
      }
    } else                      //начало раньше конца
    {
      if ((header.last_rec+sz)>header.file_size) //превышен размер файла
      {
        log->SetSize(header.last_rec);
        log->Flush();
        header.last_rec=sizeof(header);
        do
        {
          log->SeekRd(header.first_rec,FILE_BEGIN);
          log->Read(&dw,sizeof(dw),rdw);
          header.first_rec+=dw;
          header.num_rec--;
        } while (((header.last_rec+sz)>header.first_rec));
      }
    }
  }
  log->SeekWr(header.last_rec,FILE_BEGIN);
  if (log->Write(buf,sz))
  {
    header.num_rec++;
    header.last_rec+=sz;
    log->SeekWr(0,FILE_BEGIN);
    if (log->Write(&header,sizeof(header)))
    {
      if(need_notify(MNF_REPORTEREX))
      {
        NOTIFY_CODE nc;
        nc.dw_size=sz;
        nc.from_module=_handle;
        nc.notify_code=MNCODE_REPORT_ADDED;
        nc.notify_data[0]=log_name;
        notify_handles(&nc,MNF_REPORTEREX);
      }
    }
  }

}

void __fastcall TGKReporterEx::open_files(char * nt)
{
  register_event("GKSystem","Система");
  SetRegisterFunc(register_event("Exceptions","Исключения"),&ExceptionToStr);
  char fname1[MAX_PATH],fname2[MAX_PATH];
  char fmt[] = "%s.%s";
  wsprintf(fname1,fmt,nt,"log");
  wsprintf(fname2,fmt,nt,"err");
  if (FileExists(fname1) || FileExists(fname2))
  {
    TLockHelper l(locker);

    TGKStdReporter::open_files(nt);
    WriteOldLog();
    WriteOldErr();
    DeleteFile(fname1);
    DeleteFile(fname2);
  }
}


DWORD __fastcall TGKReporterEx::get_module_info(LPMODULE_INFO mi)
{
  DWORD ret;
  TLockHelper l(locker);
  ret=TGKModule::get_module_info(mi);
  if (mi->mi_mask & (MI_FL_PROVIDER | MI_FL_DESCRIPTION | MI_FL_MODULE_NAME))
  {
    if(mi->mi_mask&MI_FL_PROVIDER)
    {
      *mi->provider = 0;
      ret=LoadStringW(lang_dll_instance,IDS_MODULE_PROVIDER,mi->provider,mi->provider_cbsz)?ret:GKH_RET_ERROR;
    }
    if(mi->mi_mask&MI_FL_DESCRIPTION)
    {
      *mi->description = 0;
      ret=LoadStringW(lang_dll_instance,IDS_MODULE_DESCRIPT,mi->description,mi->description_cbsz) ? ret:GKH_RET_ERROR;
    }
    if(mi->mi_mask&MI_FL_MODULE_NAME)
    {
      *mi->module_name =0;
      ret=LoadStringW(lang_dll_instance,IDS_MODULE_NAME,mi->module_name,mi->module_name_cbsz) ? ret:GKH_RET_ERROR;
    }
  }
  return ret;
}


DWORD __fastcall TGKReporterEx::get_window_module(wchar_t *buf, DWORD bsz)
{
  wchar_t wm_name[]=L"ReporterExWnd.DLL";
  int len=lstrlenW(wm_name);
  if(buf && int(bsz)>len)
  {
    SetLastError(0);
    safe_strcpy(buf,wm_name);
  }
  else
  {
    SetLastError(MERR_INVALID_BUFFER_SIZE);
    len=-len;
  }
  return DWORD(len);
}

char* __fastcall get_except_string(DWORD code)
{
  char *str = "";
  switch(code)
  {
    case EXCEPTION_ACCESS_VIOLATION      : str = "Нарушение прав доступа";break;
    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED : str = "Выход за границы массива";break;
    case EXCEPTION_DATATYPE_MISALIGNMENT :str = "Ошибка выравнивания";break;
    case EXCEPTION_FLT_DENORMAL_OPERAND  :str = "Ошибка FP - operation is denormal";break;
    case EXCEPTION_FLT_DIVIDE_BY_ZERO    :str = "Ошибка FP - деление на 0";break;
    case EXCEPTION_FLT_INEXACT_RESULT    :str = "Ошибка FP - floating-point operation cannot be represented exactly as a decimal fraction";break;
    case EXCEPTION_FLT_INVALID_OPERATION :str = "Ошибка FP - неправильная операция";break;
    case EXCEPTION_FLT_OVERFLOW          :str = "Ошибка FP - переполнение результата";break;
    case EXCEPTION_FLT_STACK_CHECK       :str = "Ошибка FP - переполнение стека";break;
    case EXCEPTION_FLT_UNDERFLOW         :str = "Ошибка FP - переполнение стека";break;
    case EXCEPTION_ILLEGAL_INSTRUCTION   :str = "Недопустимая операция";break;
    case EXCEPTION_IN_PAGE_ERROR         :str = "Ошибка доступа к странице";break;
    case EXCEPTION_INT_DIVIDE_BY_ZERO    :str = "Деление на 0";break;
    case EXCEPTION_INT_OVERFLOW          :str = "Переполнение целого ?";break;
    case EXCEPTION_INVALID_DISPOSITION   :str = "Ошибка при обработке исключения";break;
    case EXCEPTION_NONCONTINUABLE_EXCEPTION :str = "Невосстановимая ошибка ";break;
    case EXCEPTION_PRIV_INSTRUCTION         :str = "Попытка выполнить привелегированную инструкцию ";break;
    case EXCEPTION_STACK_OVERFLOW           :str = "Переполнение стека ";break;
  }
  return str;
}

int WINAPI ExceptionToStr(LPVOID Data, char *str, DWORD sz, int Code)
{
  AnsiString s="";
  lpexception_pointers ep;
  DWORD dw=*reinterpret_cast<DWORD *>(Data);
  if (((ep=reinterpret_cast<lpexception_pointers>((byte *)Data+sizeof(dw)))==0) || (sizeof(*ep)!=dw))
    return 0;
  //Запись информации об исключении
  s.cat_sprintf("Исключение 0x%X  %s по адресу 0x%X\r\n\r\n",ep->except_rec.ExceptionCode,
                get_except_string(ep->except_rec.ExceptionCode),ep->except_rec.ExceptionAddress);
  PCONTEXT ctx=&ep->except_ctx;
  char *fmt="%5s=0x%8X\r\n";
  s.cat_sprintf(fmt,"FLAGS",ctx->EFlags);
  s.cat_sprintf(fmt,"EAX",ctx->Eax);
  s.cat_sprintf(fmt,"EBX",ctx->Ebx);
  s.cat_sprintf(fmt,"ECX",ctx->Ecx);
  s.cat_sprintf(fmt,"EDX",ctx->Edx);
  s.cat_sprintf(fmt,"EDI",ctx->Edi);
  s.cat_sprintf(fmt,"ESI",ctx->Esi);
  s.cat_sprintf(fmt,"ESP",ctx->Esp);
  s.cat_sprintf(fmt,"EIP",ctx->Eip);
  s.cat_sprintf(fmt,"SegSS",ctx->SegSs);
  s.cat_sprintf(fmt,"SegCS",ctx->SegCs);
  s.cat_sprintf(fmt,"SegDS",ctx->SegDs);
  s.cat_sprintf(fmt,"SegES",ctx->SegEs);
//  if (sizeof(*ep)<dw)
  {
    s+="\r\nДамп памяти по EBP:\r\n";
    AnsiString s1;
    for (int i=0; i<4; i++)
    {
      s1="";
      for (int j=0; j<4; j++)
        s1.cat_sprintf("%08X ",ep->ebp_dump[i*4+j]);
      s+=s1+"\r\n";
    }
    s+="\r\nДамп памяти по ESP:\r\n";
    for (int i=0; i<4; i++)
    {
      s1="";
      for (int j=0; j<4; j++)
        s1.cat_sprintf("%08X ",ep->esp_dump[i*4+j]);
      s+=s1+"\r\n";
    }
  }
  int len=s.Length()+1;
  if ((DWORD)len>sz)
  {
    len=-len;
    strncpy(str,s.c_str(),sz-1);
    str[sz-1]='\0';
  }
  else
    safe_strcpy(str,s.c_str());
  return len;
}

UINT __fastcall TGKReporterEx::register_event(char *event, char *event_name)
{
  if (!(module_state&(MODULE_STATE_RUNNING|MODULE_STATE_START_PENDING))) return (UINT)-1;
  vector<string>::iterator p= std::find(reg_events.begin(),reg_events.end(),event);
  UINT i=p-reg_events.begin();
  if (p!=reg_events.end()) return i;
  char name[MAX_PATH<<2];
  /*
  char drive[MAXDRIVE];
  char dir[MAXDIR];
  char file[MAXFILE];
  char ext[MAXEXT];*/
  GetCurrentDirectory(sizeof(name),name);
  //fnsplit(name,drive,dir,file,ext);
  strcat(name,"\\log\\");
  if (access(name,0)!=0) mkdir(name);
  strcat(name,event);
  strcat(name,".evt");

  TFile *f;
  f=new TFile(name,OPEN_ALWAYS,FILE_SHARE_READ,GENERIC_READ | GENERIC_WRITE);
  bool badfile=!CheckLogFile(f,name);
  f->SeekWr(0,FILE_END);
  reg_events.push_back(event);
  if (event_name)
    event_names.push_back(event_name);
  else
    event_names.push_back("");
  log_files.push_back(f);
  log_funcs.push_back(0);
  if (badfile)
  {
    AnsiString s=s.sprintf("Файл событий '%s' повреждён. Создан новый файл событий.",name);
    report(EVENT_SYSTEM,REPORT_ERROR_TYPE,s.c_str());
  }
  if (need_notify(MNF_REPORTEREX))
  {
    NOTIFY_CODE nc;
    nc.dw_size=sizeof(nc);
    nc.from_module=_handle;
    nc.notify_code=MNCODE_REPORT_NEWEVENT;
    nc.notify_data[0]=i;
    notify_handles(&nc,MNF_REPORTEREX);
  }
  return i;
}

bool __fastcall TGKReporterEx::CheckLogFile(TFile *log, char *file_name)
{
//  Проверяет заголовок файла лога.
//  Если формат неверный, удаляем файл
  TLogHeader header;
  TMess *mes;
  #define BUF_SIZE 65535
  byte buf[BUF_SIZE];
  bool bRead;
  DWORD dwRead;
  bRead=log->Read(&header,sizeof(header),dwRead,0);
  if (!bRead || (dwRead<sizeof(header))) goto BadFile;
  if (header.size!=sizeof(header)) goto BadFile;
  if (header.sig!=Signature) goto BadFile;
  if (header.version!=LogVersion)
    if (!ConvertVersion(header.version)) goto BadFile;
  if (!header.file_size && (header.first_rec>header.size)) goto BadFile;
  if (header.file_size && (header.file_size<log->GetSize())) goto BadFile;
  if (header.first_rec>log->GetSize()) goto BadFile;
  if (header.last_rec>log->GetSize()) goto BadFile;
  return true;
BadFile:
  bool ret=true;
  if (log->GetSize())
  {
    delete log;
    char bak_name[MAX_PATH];
    char drive[MAXDRIVE];
    char dir[MAXDIR];
    char file[MAXFILE];
    char ext[MAXEXT];
    fnsplit(file_name,drive,dir,file,ext);
    safe_strcpy(ext,".bak");
    fnmerge(bak_name,drive,dir,file,ext);
    remove(bak_name);
    rename(file_name,bak_name);
    log=new TFile(file_name,CREATE_NEW,FILE_SHARE_READ,GENERIC_READ | GENERIC_WRITE);
    ret=false;
  }
  ZeroMemory(&header,sizeof(header));
  header.size=sizeof(header);
  header.sig=Signature;
  header.version=LogVersion;
  header.file_size=100*1024;
  header.first_rec=header.size;
  header.last_rec=header.size;
  log->Write(&header,header.size);
  return ret;
}

AnsiString __fastcall ReadString(TFile *f)
{
  AnsiString s;
  char line_sep[]="\r\n";
  char ch;
  //DWORD sz = 0;
  DWORD rd = 0;
  while (!f->EofRd())
  {
    if (!f->Read(&ch,sizeof(char),rd,0)) break;
    //sz+=rd;
    s+=ch;
    if (s.Pos(line_sep))
    {
      s.Delete(s.Pos(line_sep),2);
      break;
    }
  }
  return s;
}

void __fastcall TGKReporterEx::WriteOldLog()
{
  //читает файл .log и переписывает события из него в system.evt
//  WideString ws1,ws2,ws3;
//  TDateTime dt,dt_def=0;
//  TMess event;
//  #define BUF_SIZE 65535
//  byte buf[BUF_SIZE];
//  do
//    ws1=ReadString(&report_log);
//  while (ws1.IsEmpty() && !report_log.EofRd());
//  while (!report_log.EofRd())
//  {
//    dt=StrToDateTimeDef(ws1,dt_def);
//    if (dt!=dt_def)
//    {
//      ws2=ReadString(&report_log);
//      if (StrToDateTimeDef(ws2,dt_def)==dt_def)
//      {
//        ws3=ReadString(&report_log);
//        if (StrToDateTimeDef(ws3,dt_def)==dt_def)
//          ws1=ReadString(&report_log);
//        else
//        {
//          ws1=ws3;
//          ws3="";
//        }
//        ws2+=WideString(" - ")+ws3;
//        event.time=DateTimeToUnix(dt);
//        event.type=REPORT_INFORMATION_TYPE;
//        event.num_strings=2;
//        FillBuffer(buf,BUF_SIZE,&event,L"?",ws2.c_bstr(),0,0);
//        WriteToFile(EVENT_SYSTEM,buf,event.length);
//      }
//      else
//        ws1=ws2;
//    }
//    else
//      ws1=ReadString(&report_log);
//  }
  report_log.SetSize(0);
  report_log.Close();
}

DWORD StrToLong(AnsiString &s)
{
  DWORD dw=0;
  char chars[16]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
  int k;
  for (int i=1; i<=s.Length(); i++)
    if ((k=(find(chars,chars+16,s[i])-chars))<16)
      dw=dw*16+k;
    else
      if (dw)
      {
        dw=0;
        break;
      }
  return dw;
}

void __fastcall TGKReporterEx::WriteOldErr()
{
  //читает файл .err и переписывает события из него в exception.evt

//  WideString ws1,ws2,ws3,ws4;
//  AnsiString s1,s2;
//  TDateTime dt,dt_def=0;
//  TMess event;
//  exception_pointers myep;
//  ZeroMemory(&(myep.except_rec),sizeof(myep.except_rec));
//  ZeroMemory(&(myep.except_ctx),sizeof(myep.except_ctx));
//  #define BUF_SIZE 65535
//  byte buf[BUF_SIZE];
//  while (!report_exception.EofRd())
//  {
//    ws1=ReadString(&report_exception);
//    if (ws1.IsEmpty()) continue;
//    dt=StrToDateTimeDef(ws1,dt_def);
//    if (dt==dt_def) continue;
//    ws2=ReadString(&report_exception);
//    if (ws2.Pos("GKHANDLE")==1)
//    {
//      event.num_strings=4;
//      ws1=ws2;
//      ws2=ReadString(&report_exception);
//      ws4=ReadString(&report_exception);
//    }
//    else
//    {
//      event.num_strings=3;
//      ws1="Системное исключение";
//      ws4=ws2;
//    }
//    s1=ReadString(&report_exception);
//    do
//    {
//      ws3=ReadString(&report_exception);
//    } while (ws3.IsEmpty());
//    event.time=DateTimeToUnix(dt);
//    event.type=REPORT_ERROR_TYPE;
//    s2=s1.SubString(s1.Pos("исключение 0x")+13,s1.Pos("  ")-s1.Pos("исключение 0x")-13);
//    myep.except_rec.ExceptionCode=StrToLong(s2);
//    s2=s1.SubString(s1.Pos("по адресу 0x")+12,s1.Length()-s1.Pos("по адресу 0x")-11);
//    myep.except_rec.ExceptionAddress=(PVOID)StrToLong(s2);
//    s1=ReadString(&report_exception);
//    s2=s1.SubString(9,8);
//    myep.except_ctx.EFlags=StrToLong(s2);
//    s1=ReadString(&report_exception);
//    s2=s1.SubString(9,8);
//    myep.except_ctx.Eax=StrToLong(s2);
//    s1=ReadString(&report_exception);
//    s2=s1.SubString(9,8);
//    myep.except_ctx.Ebx=StrToLong(s2);
//    s1=ReadString(&report_exception);
//    s2=s1.SubString(9,8);
//    myep.except_ctx.Ecx=StrToLong(s2);
//    s1=ReadString(&report_exception);
//    s2=s1.SubString(9,8);
//    myep.except_ctx.Edx=StrToLong(s2);
//    s1=ReadString(&report_exception);
//    s2=s1.SubString(9,8);
//    myep.except_ctx.Edi=StrToLong(s2);
//    s1=ReadString(&report_exception);
//    s2=s1.SubString(9,8);
//    myep.except_ctx.Esi=StrToLong(s2);
//    s1=ReadString(&report_exception);
//    s2=s1.SubString(9,8);
//    myep.except_ctx.Esp=StrToLong(s2);
//    s1=ReadString(&report_exception);
//    s2=s1.SubString(9,8);
//    myep.except_ctx.SegSs=StrToLong(s2);
//    s1=ReadString(&report_exception);
//    s2=s1.SubString(9,8);
//    myep.except_ctx.SegCs=StrToLong(s2);
//    s1=ReadString(&report_exception);
//    s2=s1.SubString(9,8);
//    myep.except_ctx.SegDs=StrToLong(s2);
//    s1=ReadString(&report_exception);
//    s2=s1.SubString(9,8);
//    myep.except_ctx.SegEs=StrToLong(s2);
//    if (event.num_strings==4)
//      FillBuffer(buf,BUF_SIZE,&event,ws1.c_bstr(),ws2.c_bstr(),ws3.c_bstr(),ws4.c_bstr(),sizeof(myep),&myep);
//    else
//      FillBuffer(buf,BUF_SIZE,&event,ws1.c_bstr(),ws3.c_bstr(),ws4.c_bstr(),sizeof(myep),&myep);
//    WriteToFile(EVENT_EXCEPTIONS,buf,event.length);
//  }
  report_exception.SetSize(0);
  report_exception.Close();
}

DWORD __fastcall TGKReporterEx::ClearEvent(UINT i)
{
  DWORD ret=GKH_RET_ERROR;
  if (i<log_files.size())
  {
    TFile * file = log_files[i];
    if(file && !IsBadReadPtr(file,sizeof(*file)))
    {
      TLogHeader header;
      DWORD rdw;
      file->SeekRd(0,FILE_BEGIN);
      file->Read(&header,sizeof(header),rdw);
      file->SetSize(0);
      file->Flush();
      header.first_rec=header.size;
      header.last_rec=header.size;
      header.num_rec=0;
      file->SeekWr(0,TFile::FromBegin);
      file->Write(&header,sizeof(header));
      if (need_notify(MNF_REPORTEREX))
      {
        NOTIFY_CODE nc;
        nc.dw_size=sizeof(nc);
        nc.from_module=_handle;
        nc.notify_code=MNCODE_REPORT_REFRESH;
        nc.notify_data[0]=i;
        notify_handles(&nc,MNF_REPORTEREX);
      }
      ret=GKH_RET_SUCCESS;
    }
  }
  return ret;
}

DWORD __fastcall TGKReporterEx::GetMessageNameA(UINT event, TEventNameA * names)
{
  if (event>=reg_events.size()) return GKH_RET_ERROR;
  string s;
  s=reg_events[event];
  if (s.length()<(UINT)names->sz_short)
    safe_strcpy(names->s_short,s.c_str());
  else
  {
    strncpy(names->s_short,s.c_str(),names->sz_short-1);
    names->s_short[names->sz_short-1]='\0';
    names->sz_short=-(int)s.length();
  }
  s=event_names[event];
  if (!s.length())
    names->s_full[0]=0;
  else if (s.length()<(UINT)names->sz_full)
    safe_strcpy(names->s_full,s.c_str());
  else
  {
    strncpy(names->s_full,s.c_str(),names->sz_full-1);
    names->s_full[names->sz_full-1]=0;
    names->sz_full=-(int)s.length();
  }
  return GKH_RET_SUCCESS;
}

DWORD __fastcall TGKReporterEx::GetMessageNameW(UINT event, TEventNameW * names)
{
  if (event>=reg_events.size()) return GKH_RET_ERROR;
  WideString ws;
  ws=reg_events[event].c_str();
  if (ws.Length()<names->sz_short)
    wcscpy(names->s_short,ws.c_bstr());
  else
  {
    wcsncpy(names->s_short,ws.c_bstr(),names->sz_short-1);
    names->s_short[names->sz_short-1]='\0';
    names->sz_short=-ws.Length();
  }
  ws=event_names[event].c_str();
  if (!ws.Length())
    names->s_full[0]=0;
  else if (ws.Length()<names->sz_full)
    wcscpy(names->s_full,ws.c_bstr());
  else
  {
    wcsncpy(names->s_full,ws.c_bstr(),names->sz_full-1);
    names->s_full[names->sz_full-1]=0;
    names->sz_full=-ws.Length();
  }
  return GKH_RET_SUCCESS;
}


DWORD __fastcall TGKReporterEx::SetRegisterFunc(UINT i, LPVOID func)
{
  if (i<=log_funcs.size())
    log_funcs[i]=func;
  return GKH_RET_SUCCESS;
}

LPVOID __fastcall TGKReporterEx::GetRegisterFunc(UINT i, char * event_name)
{
  LPVOID ret=0;
  if ((int)i<0)
  {
    if (event_name && strlen(event_name))
      i=find(reg_events.begin(),reg_events.end(),event_name)-reg_events.begin();
  }
  if (i<log_funcs.size())
    ret=log_funcs[i];
  return ret;
}

bool __fastcall TGKReporterEx::read_settings()
{
  TRegsReader rr(DupKey(reg_key));
  DWORD dw;
  if (rr.OpenKey("")) dw=rr.ReadDword("Sound");
  if (dw) sound=true;
  return TGKStdReporter::read_settings();
}

bool __fastcall TGKReporterEx::write_settings()
{
  TRegsWriter rw(DupKey(reg_key));
  if (rw.OpenKey("")) rw.WriteDword("Sound",(DWORD)sound,true);
  return TGKStdReporter::write_settings();
}

void   __fastcall TGKReporterEx::report(UINT log,int type,char * desc)
{
  wchar_t *s=0;
  if(desc){s = new wchar_t[strlen(desc)+1]; /*KeRTL::*/Ansi2Unicode(s,desc);}
  report(log,type,s);
  if(s) delete [] s;
}

void   __fastcall TGKReporterEx::report(UINT log,int type,wchar_t * desc)
{
  TReportMes mes;
  mes.log=log;
  mes.source=_handle;
  mes.type=type;
  mes.desc=desc;
  processing(RCM_MESSAGE_EVENT,LPARAM(&mes),0);
}

bool __fastcall TGKReporterEx::ConvertVersion(DWORD version)
{
  if (version>LogVersion)
    return false;
  else
    return true;
}

DWORD __fastcall TGKReporterEx::ChangeFileSize(UINT num, DWORD size)
{
  DWORD ret=GKH_RET_ERROR;
  if (num<log_files.size())
  {
    TFile *f=log_files[num];
    if(f && !IsBadReadPtr(f,sizeof(*f)))
    {
      TLogHeader header;
      DWORD rdw;
      f->SeekRd(0,FILE_BEGIN);
      if (!f->Read(&header,sizeof(header),rdw)) return ret;
      bool rewr=true;
      if (header.file_size)
      {
        if (size)
        {
          if (size>=f->GetSize()) rewr=false;
        } else if (header.first_rec==header.size) rewr=false;
      } else if (size>=f->GetSize()) rewr=false;
      if (rewr)
      {
        DWORD dw, pos, offset;
        DWORD count = 0;
        if (size)
          count=f->GetSize()-size;
        pos=header.first_rec;
        while (count>0)
        {
          f->SeekRd(pos,FILE_BEGIN);
          if (!f->Read(&dw,sizeof(dw),rdw)) return ret;
          pos+=dw;
          count-=dw;
          if (pos>=f->GetSize() && pos>header.last_rec && header.file_size) pos=header.size;
        }
        byte *buf=new byte[size];
        count=0;
        offset=0;
        f->SeekRd(pos,FILE_BEGIN);
        while ((pos!=header.last_rec || !count) && count< header.num_rec)
        {
          if (!f->Read(&dw,sizeof(dw),rdw)) break;
          if (rdw!=sizeof(dw)) break;
          CopyMemory(buf+offset,&dw,rdw);
          offset+=rdw;
          if (!f->Read(buf+offset,dw-sizeof(dw),rdw)) break;
          if (rdw!=dw-sizeof(dw)) break;
          offset+=rdw;
          pos+=dw;
          count++;
          if (pos>=f->GetSize() && pos>header.last_rec && header.file_size)
          {
            pos=header.size;
            f->SeekRd(pos,FILE_BEGIN);
          }
        }
        f->SetSize(header.size);
        f->Flush();
        f->SeekWr(header.size,FILE_BEGIN);
        f->Write(buf,offset);
        delete [] buf;
        offset+=header.size;
        header.first_rec=header.size;
        header.last_rec=offset;
        header.num_rec=count;
      }
      header.file_size=size;
      f->SeekWr(0,FILE_BEGIN);
      f->Write(&header,sizeof(header));
      if (need_notify(MNF_REPORTEREX))
      {
        NOTIFY_CODE nc;
        nc.dw_size=sizeof(nc);
        nc.from_module=_handle;
        nc.notify_code=MNCODE_REPORT_REFRESH;
        nc.notify_data[0]=num;
        notify_handles(&nc,MNF_REPORTEREX);
      }
      ret=GKH_RET_SUCCESS;
    }
  }
  return ret;
}

