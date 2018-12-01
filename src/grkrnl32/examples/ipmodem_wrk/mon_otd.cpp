

#include <Text_viewer.hpp>
#include <otd.h>
#include <stdio.h>

int __fastcall get_fa_str(DWORD fa,DWORD pp,char * buf,DWORD buf_len)
{
 *buf = 0;
 if(fa&OTD_FA_CALL)
   {
    strcat(buf,"Вызов ");
   }
 if(fa&OTD_FA_ORDER)
 {
    strcat(buf,"Подписка ");

 }
  return strlen(buf);
}

int __fastcall get_tu_str(otd_tutr & tutr,TCHAR * out)
{
 int ret = 0;
 switch(tutr.command&OTD_TUTR_CMDMASK)
 {
  case OTD_TUTR_CMDSELECT  : ret = wsprintf(out,_T("Выбор ТУ-ТР"));break;
  case OTD_TUTR_CMDENABLE  : ret = wsprintf(out,_T("Разрешение ТУ-ТР"));break;
  case OTD_TUTR_CMDDESELECT: ret = wsprintf(out,_T("Отмена выбора ТУ-ТР"));break;
  default :
   {
       switch(tutr.command&OTD_TUTR_OPMASK)
       {
       case OTD_TUOP_ON       :  ret = wsprintf(out,_T("Команда ТУ-ТР ВКЛ /+"));break;
       case OTD_TUOP_OFF      :  ret = wsprintf(out,_T("Команда ТУ-ТР ОТКЛ/-"));break;
       case OTD_TUTROP_CANCEL :  ret = wsprintf(out,_T("Команда ТУ-ТР ОТМЕНА"));break;
       }
   }
   break;
 }

 return ret;
}

void __fastcall monitor_otd_old(KeWin::TTextViewer<TCHAR> * monitor, LPBYTE data, DWORD data_size)
{
 otd_proto op;
 op.dw_size = sizeof(op);
 DWORD len;
 if(otd_proto_parser(data,data_size,&op,&len))
 {
 TCHAR str[MAX_PATH<<2];
 TCHAR str_templ1[128];
 TCHAR str_templ2[128];
 lstrcpy(str_templ1,_T("ОТД адрес %03d.%03d.%03d.%03d "));
 len = wsprintf(str,str_templ1,(DWORD)op.addr->pu,(DWORD)op.addr->cp,(DWORD)(op.addr->fa),(DWORD)op.addr->sb);

 if(OTD_FA_ISQUERY(op.addr->fa))
 {
  char fa_str[64];
  get_fa_str(op.addr->fa,*op.ver_parts,fa_str,sizeof(fa_str));
  wsprintf(str +len ,_T("%s маска составляющих %08X"),fa_str,(DWORD)((*op.ver_parts)&OTD_PROTO_PARTSMASK));
  monitor->add_line(str);
 }
 else
 {

 if((*op.ver_parts)&OTD_PROTO_FLAG_QUERYRESPOND)
    {
    wsprintf(str+len,_T(" ** Ответ на вызов данных **"));
    }
    monitor->add_line(str);

 if(op.time_stamp)
  {
      TCHAR date_time[MAX_PATH];
      SYSTEMTIME st;
      FileTimeToSystemTime((LPFILETIME)op.time_stamp,&st);
      int dtl = GetDateFormat(LOCALE_USER_DEFAULT,DATE_SHORTDATE,&st,NULL,date_time,sizeof(date_time)/sizeof(*date_time));
      dtl--;
      *(date_time+dtl) = ' ';
      dtl++;
      dtl+=GetTimeFormat(LOCALE_USER_DEFAULT,0,&st,_T("HH':'mm':'ss"),date_time+dtl,sizeof(date_time)/sizeof(*date_time)-dtl);
      dtl--;
      wsprintf(date_time+dtl,_T(",%03d"),(DWORD)st.wMilliseconds);
      monitor->add_line(date_time);
  }

 if(op.diag)
    {
    wsprintf(str ,_T("Диагностика %08X "),*op.diag );
    monitor->add_line(str);
    }


 if(op.name)
   {
    wchar_t w_name[MAX_PATH];
    otd_text_getwstr(op.name,w_name,sizeof(w_name)/sizeof(wchar_t));
    char    name[MAX_PATH];
    KeRTL::Unicode2Ansi(name,w_name);
    wsprintf(str,_T("Имя - %s"), name);
    monitor->add_line(str);
   }

 if(op.dw_size == sizeof(op) &&  op.data)
 {
   /*Печать данных */
   //TODO Дописать вариант для ТУ /ТР
    DWORD begin = (DWORD)op.data->numbers.loN;
    DWORD end   = (DWORD)op.data->numbers.hiN;
   if(OTD_FA_ISCTRL(op.addr->fa))
   {
    //ТУ/ТР
    wsprintf(str,_T("%s с %03d по %03d"),(op.addr->fa&OTD_FA_ALL)? _T("ТР") : _T("ТУ"), begin,end);
    monitor->add_line(str);
    len = 0;
    for(;begin<=end;begin++)
    {
     otd_tutr value;
     otd_get_value(op.data,begin,&value,sizeof(value));
     len+=get_tu_str(value,str+len);
    }
   }
   else
   {
    wsprintf(str,_T("Данные с %03d по %03d"),begin,end);
    monitor->add_line(str);
    len = 0;
    for(;begin<=end;begin++)
    {
     DWORD value = 0;
     if(op.dw_size!=sizeof(op))
        DebugBreak();
     otd_get_value(op.data,begin,&value,sizeof(value));
     if(op.dw_size!=sizeof(op))
        DebugBreak();
     if(op.data->otd_type == OTD_DISCRETE)
      len += wsprintf(str+len,_T("%1d"),value ? TRUE:FALSE);
      else
      len += wsprintf(str+len,_T("%04X "),value );
    }
   }
    monitor->add_line(str);
 }
 if(op.personal_diag)
 {
    DWORD begin = (DWORD)op.personal_diag->numbers.loN;
    DWORD end   = (DWORD)op.personal_diag->numbers.hiN;
    wsprintf(str,_T("Персональная диагностика с %03d по %03d"),begin,end);
    monitor->add_line(str);
    len = 0;
    for(;begin<=end;begin++)
    {
     DWORD value = 0;
     otd_get_value(op.personal_diag,begin,&value,sizeof(value));
      len += wsprintf(str+len,_T("%04X "),value );
    }
    monitor->add_line(str);
 }
 if(op.personal_chmask)
 {
    DWORD begin = (DWORD)op.personal_chmask->numbers.loN;
    DWORD end   = (DWORD)op.personal_chmask->numbers.hiN;
    wsprintf(str,_T("Персональная маска изменений с %03d по %03d"),begin,end);
    monitor->add_line(str);
    len = 0;
    for(;begin<=end;begin++)
    {
     DWORD value = 0;
     otd_get_value(op.personal_chmask,begin,&value,sizeof(value));
      len += wsprintf(str+len,_T("%04X "),value );
    }
    monitor->add_line(str);
 }
  len = KeRTL::MIN((DWORD)sizeof(str),monitor->get_max_line_len());
  memset(str,'-',len);
  str[len] = 0;
  monitor->add_line(str);
 }
 }
}

void __fastcall monitor_otd(KeWin::TTextViewer<TCHAR> * monitor, LPBYTE data, DWORD data_size)
{
 otd_proto op;
 op.dw_size = sizeof(op);
 DWORD len;
 if(otd_proto_parser(data,data_size,&op,&len))
 {
 TCHAR str[MAX_PATH<<1];
 TCHAR str_templ1[128];
 TCHAR str_templ2[128];
 lstrcpy(str_templ1,_T("ОТД адрес %03d.%03d.%03d.%03d размер %d"));

 len = wsprintf(str,str_templ1,(DWORD)op.addr->pu,(DWORD)op.addr->cp,(DWORD)(op.addr->fa),(DWORD)op.addr->sb,(int)op.proto_size);
 if(op.kpk)
   len+= wsprintf(str+len,_T(" КПК %04X"),(DWORD)*op.kpk);
 if(OTD_FA_ISQUERY(op.addr->fa))
 {
  char fa_str[64];
  get_fa_str(op.addr->fa,*op.ver_parts,fa_str,sizeof(fa_str));
  wsprintf(str +len ,_T("%s маска составляющих %08X"),fa_str,(DWORD)((*op.ver_parts)&OTD_PROTO_PARTSMASK));
  monitor->add_line(str);
 }
 else
 {
 if((*op.ver_parts)&OTD_PROTO_FLAG_QUERYRESPOND)
    len+= wsprintf(str+len,_T(" ** Ответ на вызов данных **"));
    monitor->add_line(str);

 if(op.time_stamp)
  {
      TCHAR date_time[MAX_PATH];
      SYSTEMTIME st;
      FileTimeToSystemTime((LPFILETIME)op.time_stamp,&st);
      int dtl = GetDateFormat(LOCALE_USER_DEFAULT,DATE_SHORTDATE,&st,NULL,date_time,sizeof(date_time)/sizeof(*date_time));
      dtl--;
      *(date_time+dtl) = ' ';
      dtl++;
      dtl+=GetTimeFormat(LOCALE_USER_DEFAULT,0,&st,_T("HH':'mm':'ss"),date_time+dtl,sizeof(date_time)/sizeof(*date_time)-dtl);
      dtl--;
      wsprintf(date_time+dtl,_T(",%03d"),(DWORD)st.wMilliseconds);
      monitor->add_line(date_time);
  }

 if(op.diag)
    {
     wsprintf(str ,_T("Диагностика %08X "),*op.diag);
     monitor->add_line(str);
    }


 if(op.name)
   {
    wchar_t w_name[MAX_PATH];
    otd_text_getwstr(op.name,w_name,sizeof(w_name)/sizeof(wchar_t));
    TCHAR    name[MAX_PATH];
    #ifndef UNICODE
    KeRTL::Unicode2Ansi(name,w_name);
    #else
    lstrcpy(name,w_name);
    #endif
    wsprintf(str,_T("Имя - %s"), name);
    monitor->add_line(str);
   }

 if(op.data)
 {
   /*Печать данных */
   //TODO Дописать вариант для ТУ /ТР
    DWORD begin = (DWORD)op.data->numbers.loN;
    DWORD end   = (DWORD)op.data->numbers.hiN;
   if(OTD_FA_ISCTRL(op.addr->fa))
   {
    //ТУ/ТР
    wsprintf(str,_T("%s с %03d по %03d"),(op.addr->fa&OTD_FA_ALL)? _T("ТР") : _T("ТУ"), begin,end);
    monitor->add_line(str);
    len = 0;
    for(;begin<=end;begin++)
    {
     otd_tutr value;
     otd_get_value(op.data,begin,&value,sizeof(value));
     len+=get_tu_str(value,str+len);
    }
   }
   else
   {
    wsprintf(str,_T("Данные с %03d по %03d"),begin,end);
    monitor->add_line(str);
    len = 0;
    for(;begin<=end;begin++)
    {
     DWORD value = 0;
     otd_get_value(op.data,begin,&value,sizeof(value));
     if(op.data->otd_type == OTD_DISCRETE)
      len += wsprintf(str+len,_T("%1d"),value ? TRUE:FALSE);
      else
      {
       if(op.data->otd_type == OTD_FLOAT || op.data->otd_type == OTD_SIGNED_FLOAT)
       {
        float fv = *((float*)&value);
        len += wsprintf(str+len,_T("%.2f "),fv );
       }
       else
       len += wsprintf(str+len,_T("%04X "),value );


      }
    }
   }
    monitor->add_line(str);
 }
 if(op.personal_diag)
 {
    DWORD begin = (DWORD)op.personal_diag->numbers.loN;
    DWORD end   = (DWORD)op.personal_diag->numbers.hiN;
    wsprintf(str,_T("Персональная диагностика с %03d по %03d"),begin,end);
    monitor->add_line(str);
    len = 0;
    for(;begin<=end;begin++)
    {
     DWORD value = 0;
     otd_get_value(op.personal_diag,begin,&value,sizeof(value));
      len += wsprintf(str+len,_T("%04X "),value );
    }
    monitor->add_line(str);
 }
 if(op.personal_chmask)
 {
    DWORD begin = (DWORD)op.personal_chmask->numbers.loN;
    DWORD end   = (DWORD)op.personal_chmask->numbers.hiN;
    wsprintf(str,_T("Персональная маска изменений с %03d по %03d"),begin,end);
    monitor->add_line(str);
    len = 0;
    for(;begin<=end;begin++)
    {
     DWORD value = 0;
     otd_get_value(op.personal_chmask,begin,&value,sizeof(value));
      len += wsprintf(str+len,_T("%04X "),value );
    }
    monitor->add_line(str);
 }
  len = KeRTL::MIN((DWORD)sizeof(str)/sizeof(*str),monitor->get_max_line_len());
  std::fill(str,str+len,_T('-'));
  str[len] = 0;
  monitor->add_line(str);
 }
 }
}
