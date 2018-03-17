/*-----------------------------------------------------------------------------
 Unit Name: gkreporter
 Author:    Dimka
 Назначение: Запись всех сообщений в лог

-----------------------------------------------------------------------------*/


#pragma hdrstop
#include <gkreporter.hpp>
#include <string>
#include <stdio.h>

  TGKStdReporter::TGKStdReporter(bool external)
  {
   if (!external)
     alloc_gkhandle(REPORTER_NAME_W);
  };


  LRESULT __fastcall TGKStdReporter::processing(DWORD cmd,LPARAM p1,LPARAM p2)
  {
   LRESULT ret;
   switch(cmd)
   {
    case    RCM_MESSAGE_W:
    {
     char * s1 = 0,*s2 =0;
     if(p1){s1 = new char[lstrlenW((wchar_t*)p1)+1]; KeRTL::Unicode2Ansi(s1,(wchar_t*)p1);}
     if(p2){s2 = new char[lstrlenW((wchar_t*)p2)+1]; KeRTL::Unicode2Ansi(s2,(wchar_t*)p2);}
     write_strings(report_log(),REPFL_DATE|REPFL_TIME|REPFL_NEWLINES,s1,s2);
     if(s1) delete [] s1;
     if(s2) delete [] s2;
    }
    break;
    case    RCM_MESSAGE_A:
            write_strings(report_log(),REPFL_DATE|REPFL_TIME|REPFL_NEWLINES,(char*)p1,(char*)p2);
    break;
    case    RCM_MESSAGE_EVENT:
            write_struct(report_log(),0,(TReportMes *)p1, p2);
    break;
    case RCM_EXCEPTION:
    write_exception(REPFL_DATE|REPFL_TIME|REPFL_NEWLINES,LPEXCEPTION_POINTERS(p1),(char *)p2);
    break;
    case RCM_GKH_EXCEPTION:
    write_gkexception(lpexcept_record(p1),(char*)p2);
    break;
    case RCM_REGISTER_MSG_TYPE_A:
      ret=register_event((char *)p1,(char *)p2);
      break;
    case RCM_REGISTER_MSG_TYPE_W:
    {
      char *s1,*s2=0;
      if (p1) {s1=new char[wcslen((wchar_t *)p1)+1]; KeRTL::Unicode2Ansi(s1,(wchar_t *)p1);}
      if (p2) {s2=new char[wcslen((wchar_t *)p2)+1]; KeRTL::Unicode2Ansi(s2,(wchar_t *)p2);}
      ret=register_event(s1,s2);
      if (s1) delete [] s1;
      if (s2) delete [] s2;
      break;
    }
    case RCM_SET_REGISTER_FUNC:
    case RCM_GET_REGISTER_FUNC:
    case RCM_GET_MSG_NAME_A:
    case RCM_GET_MSG_NAME_W:
    case RCM_CLEAR_EVENT:
      ret=0;
      break;
    case MCMD_GETMODULE_INFO: ret = this->get_module_info(LPMODULE_INFO(p1));break;
    default : ret = TGKModule::processing(cmd,p1,p2);
    break;
   }

   return ret;
  }

  void    __fastcall TGKStdReporter::open_files(char * nt)
  {
   char fname[MAX_PATH];
   char fmt[] = "%s.%s";
   wsprintf(fname,fmt,nt,"log");
   report_log.Open(fname,OPEN_ALWAYS,FILE_SHARE_READ,GENERIC_READ | GENERIC_WRITE);
   report_log.SeekWr(0,FILE_END);
   wsprintf(fname,fmt,nt,"err");
   report_exception.Open(fname,OPEN_ALWAYS,FILE_SHARE_READ,GENERIC_READ | GENERIC_WRITE);
   report_exception.SeekWr(0,FILE_END);
  }

  void    __fastcall TGKStdReporter::release(LPVOID arg)
  {
    TGKModule::stop(0);
    report_log.Close();
    report_exception.Close();
    TGKModule::release(arg);
  }


  DWORD   __fastcall TGKStdReporter::start(DWORD reason,LPARAM p2)
  {
   DWORD ret = 0;
   if(!check_running())
   {
     char names_template[MAX_PATH<<2];
     if(IsBadReadPtr((LPVOID)p2,1))
        p2 = 0;
     if(p2 )
       safe_strcpy(names_template,(char*)p2);
     else
       {
        GetModuleFileName(0,names_template,sizeof(names_template));
        GetCurrentDirectory(sizeof(names_template),names_template);
       }
     char * point = strchr(names_template,'.');
     if(point)
       *point = 0;
     open_files(names_template);

     TGKModule::start(reason,p2);
   }
   return ret;
  }

  DWORD   __fastcall TGKStdReporter::stop (DWORD /*reason*/)
  {

   DWORD ret = 0;
   if(is_running())
   {
	ret = false;
   }
   return ret;
  }

#ifdef __BORLANDC__
 #pragma warn -8057
#endif

  void __fastcall TGKStdReporter::write_struct(HANDLE fout,DWORD flags,TReportMes *mes,LPARAM p2)
  {

	const int buf_size=32000;
	char buf[buf_size];
	string s="Модуль: ";
	if (!get_gkhandle_nameA(mes->source,buf,buf_size))
	  {
           #ifdef _WIN64
           sprintf(buf,"%llX",mes->source);
           #else
           sprintf(buf,"%X",mes->source);
           #endif
          }
	s=s+"'"+buf+"'";
	switch (mes->log)
	{
	  case 0:  sprintf(buf," пишет в 'System'"); break;
	  case 1:  sprintf(buf," пишет в 'Exceptions'"); break;
	  default: sprintf(buf," пишет в файл событий%d",mes->log);
	}
	s+=buf;
	KeRTL::Unicode2Ansi(buf,mes->desc);
	write_strings(report_log(),REPFL_DATE|REPFL_TIME|REPFL_NEWLINES,(char *)s.c_str(),buf);
  }

#ifdef __BORLANDC__
 #pragma warn .8057
#endif


  char line_sep[] = "\r\n\0";

  //Запись строк в указанный файл
  void    __fastcall TGKStdReporter::write_strings    (HANDLE fout,DWORD flags,char * str1,char * str2)
  {
   DWORD wr_len;
   SetFilePointer(fout,0,0,FILE_END);
   if(flags & (REPFL_DATE|REPFL_TIME))
   {
    char dt[MAX_PATH];
    *dt = 0;
    SYSTEMTIME st;
    GetLocalTime(&st);
    if(flags & REPFL_DATE)
      {
       GetDateFormat(LOCALE_SYSTEM_DEFAULT,DATE_SHORTDATE,&st,0,dt,sizeof(dt));
       if(flags&REPFL_TIME)
          lstrcat(dt,"  ");
      }
    if(flags&REPFL_TIME)
    {
     GetTimeFormat(LOCALE_SYSTEM_DEFAULT,0,&st,"HH':'mm':'ss",dt+lstrlenA(dt),sizeof(dt)-lstrlenA(dt));
    }
    lstrcat(dt,line_sep);
    WriteFile(fout,dt,lstrlenA(dt),&wr_len,0);
   }

   if(str1)
   {
    WriteFile(fout,str1,lstrlenA(str1),&wr_len,0);
    if(flags& REPFL_NEWLINES)
    WriteFile(fout,line_sep,2,&wr_len,0);

   }

   if(str2)
   {
    WriteFile(fout,str2,lstrlenA(str2),&wr_len,0);
    if(flags& REPFL_NEWLINES)
       WriteFile(fout,line_sep,2,&wr_len,0);
   }
  }

const char * __fastcall get_except_string(DWORD code)
{
 const char * str = "";
 switch(code)
 {
case EXCEPTION_ACCESS_VIOLATION      : str = "Нарушение прав доступа";break;

case EXCEPTION_ARRAY_BOUNDS_EXCEEDED : str = "Выход за границы массива";break;

//case EXCEPTION_BREAKPOINT
//A breakpoint was encountered.
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




  void    __fastcall TGKStdReporter::write_exception  (DWORD flags,LPEXCEPTION_POINTERS ep,char * text)
  {
     char str1[MAX_PATH<<1];
     char str2[MAX_PATH<<1];
     int len ;
    //Запись информации об исключении
    DWORD thread_id = GetCurrentThreadId();
    wsprintf(str1,"Нить 0x%X  %s\r\n исключение 0x%X  %s по адресу 0x%X\r\n",thread_id,text ? text:"",
                                                            ep->ExceptionRecord->ExceptionCode,
                                                            get_except_string(ep->ExceptionRecord->ExceptionCode),
                                                            ep->ExceptionRecord->ExceptionAddress);


    MEMORY_BASIC_INFORMATION mbi;
    #ifdef _DEBUG
    ZeroMemory(&mbi,sizeof(mbi));
    #endif
    VirtualQuery(ep->ExceptionRecord->ExceptionAddress,&mbi,sizeof(mbi));
    safe_strcpy(str2,"в модуле : ");
    len = lstrlen(str2);
    GetModuleFileName((HINSTANCE)mbi.AllocationBase,str2+len,sizeof(str2)-len);
    write_strings(report_exception(),flags|REPFL_NEWLINES,str1,str2);

    PCONTEXT ctx = ep->ContextRecord;

    char * fmt = const_cast<char*>("%5s=0x%8X\r\n");
    #ifdef __WIN32__
    len  = wsprintf(str2    ,fmt,"FLAGS",ctx->EFlags);
    len += wsprintf(str2+len,fmt,"EAX  ",ctx->Eax);
    len += wsprintf(str2+len,fmt,"EBX  ",ctx->Ebx);
    len += wsprintf(str2+len,fmt,"ECX  ",ctx->Ecx);
    len += wsprintf(str2+len,fmt,"EDX  ",ctx->Edx);
    len += wsprintf(str2+len,fmt,"EDI  ",ctx->Edi);
    len += wsprintf(str2+len,fmt,"ESI  ",ctx->Esi);

    len += wsprintf(str2+len,fmt,"ESP  ",ctx->Esp);

    len += wsprintf(str2+len,fmt,"SegSS",ctx->SegSs);
    len += wsprintf(str2+len,fmt,"SegCS",ctx->SegCs);
    len += wsprintf(str2+len,fmt,"SegDS",ctx->SegDs);
    len += wsprintf(str2+len,fmt,"SegES",ctx->SegEs);
    #endif

    #ifdef _WIN64
    #endif

    write_strings(report_exception(),REPFL_NEWLINES,str2,0);

  }

  void    __fastcall TGKStdReporter::write_gkexception(lpexcept_record er,char * text)
  {
   char str1[MAX_PATH<<1];
   char str2[MAX_PATH<<1];
   *str2 = 0;
   get_gkhandle_name(er->handle,str2,sizeof(str2));
   wsprintf(str1,"GKHANDLE 0x%X %s",DWORD(er->handle),str2);
   wsprintf(str2,"cmd = 0x%X p1 = 0x%X p2 = 0x%X",er->cmd,er->p1,er->p2);
   write_strings(report_exception(),REPFL_DATE|REPFL_TIME|REPFL_NEWLINES,str1,str2);
   EXCEPTION_POINTERS ep;
   ep.ExceptionRecord = &er->except_rec;
   ep.ContextRecord   = &er->except_ctx;
   write_exception(0,&ep,text);
  }

 DWORD    __fastcall TGKStdReporter::get_module_info(LPMODULE_INFO mi)
 {
  DWORD ret = TGKModule::get_module_info(mi);
  if(ret ==GKH_RET_SUCCESS)
  {
   if(mi->mi_mask&MI_FL_PROVIDER)
     *mi->provider = 0,
     #ifdef COMERCIAL_BUILD
     lstrcpynW(mi->provider,L"OOO \"Компания промышленной автоматизации\"",mi->provider_cbsz);
     #else
     lstrcpynW(mi->provider,L"Остапенко Д.В. г.Азов",mi->provider_cbsz);
     #endif

   if(mi->mi_mask&MI_FL_DESCRIPTION)
     *mi->description = 0,
     lstrcpynW(mi->description,L"Стандартный модуль отчета работы программы",mi->description_cbsz);


   if(mi->mi_mask&MI_FL_MODULE_NAME)
     *mi->module_name =0;

   if(mi->mi_mask&MI_FL_DLL_NAME)
     *mi->dll_name = 0;

   if(mi->mi_mask&MI_FL_LANGDLL_NAME)
     *mi->lang_dll_name = 0;
  }
  return ret;
 }




