#pragma hdrstop
#include "modemmer_frm.h"
#include <otd.h>
#include <stdlib.h>
#include <stdio.h>
#include "modemmer_wnd.rh"



int __fastcall get_fa_str(DWORD fa,DWORD pp,TCHAR * buf,DWORD buf_len)
{
 *buf = 0;
 if(fa&OTD_FA_CALL)
   {
    if(fa&OTD_FA_ORDER)
      return LoadString(ModuleInstance,IDS_MON_FASTR2,buf,buf_len);
    else
      return LoadString(ModuleInstance,IDS_MON_FASTR0,buf,buf_len);

   }
    if(fa&OTD_FA_ORDER)
      {
       return LoadString(ModuleInstance,pp ? IDS_MON_FASTR1:IDS_MON_FASTR3,buf,buf_len);
      }
  return LoadString(ModuleInstance,IDS_MON_FASTR4,buf,buf_len);
}

int __fastcall get_tu_str(otd_tutr & tutr,TCHAR * out)
{
 int ret = 0;
 switch(tutr.command&OTD_TUTR_CMDMASK)
 {
  case OTD_TUTR_CMDSELECT  : ret = swprintf(out,_T("Выбор ТУ-ТР"))     ;break;
  case OTD_TUTR_CMDENABLE  : ret = swprintf(out,_T("Разрешение ТУ-ТР"));break;
  case OTD_TUTR_CMDDESELECT: ret = swprintf(out,_T("Отмена выбора ТУ-ТР"));break;
  default :
   {
       switch(tutr.command&OTD_TUTR_OPMASK)
       {
       case OTD_TUOP_ON       :  ret = swprintf(out,_T("Команда ТУ-ТР ВКЛ /+"));break;
       case OTD_TUOP_OFF      :  ret = swprintf(out,_T("Команда ТУ-ТР ОТКЛ/-"));break;
       case OTD_TUTROP_CANCEL :  ret = swprintf(out,_T("Команда ТУ-ТР ОТМЕНА"));break;
       }
   }
   break;
 }

 return ret;
}

int  __fastcall get_time_string(TCHAR * date_time,int sz,LPQWORD timestamp)
{
      SYSTEMTIME st;
      FileTimeToSystemTime((LPFILETIME)timestamp,&st);
      int dtl = GetDateFormat(LOCALE_USER_DEFAULT,DATE_SHORTDATE,&st,NULL,date_time,sz);
      dtl--;
      *(date_time+dtl) = ' ';
      dtl++;
      dtl+=GetTimeFormat(LOCALE_USER_DEFAULT,0,&st,_T("HH':'mm':'ss"),date_time+dtl,sz-dtl);
      dtl--;
      dtl+=snwprintf(date_time+dtl,sz-dtl,_T(",%03d"),(DWORD)st.wMilliseconds);
      return dtl;
}

void __fastcall mon_alarm_param(TModemmerMonitor * monitor,lpotd_alarm_param ap)
{
 char  str[MAX_PATH];
 sprintf(str,"параметр %02hu.%02hu.%02hu.%02hu-.%02u",(WORD)ap->param_addr.addr.pu,(WORD)ap->param_addr.addr.cp,(WORD)(ap->param_addr.addr.fa&OTD_FA_DATA_MASK),(WORD)ap->param_addr.addr.sb,ap->param_addr.param_number);
 monitor->add_line(str);
 int len = 0;
 if(ap->cmd&OTD_ALARM_CMD_SET)
    len += sprintf(str+len,"Установить ") ;
 if(ap->cmd&OTD_ALARM_CMD_DELETE)
    len += sprintf(str+len,"Удалить ")    ;
 if(ap->cmd&OTD_ALARM_CMD_CLEAR)
    len += sprintf(str+len,"Очистить ")   ;

 switch(ap->cmd&OTD_ALARM_CMD_MASK)
 {
  case OTD_ALARM_CMD_LESS     :
  case OTD_ALARM_CMD_MORE     :
  len = sprintf(str+len,"%s чем %u",(ap->cmd&OTD_ALARM_CMD_MASK) == OTD_ALARM_CMD_LESS ? "Меньше ":"Больше",ap->value);
  break;
  case OTD_ALARM_CMD_ENUM     : len = sprintf(str,"Перечислить") ;break;
 }
 if(len)
    monitor->add_line(str);

}

TCHAR * __fastcall get_otd_type_string(DWORD otd_type)
{
  switch(otd_type)
  {
    case OTD_DISCRETE       : return _T("distcrete");
    //case OTD_BYTE           : return "byte";
    case OTD_ANALOG_BYTE    : return _T("byte");
    case OTD_ANALOG_WORD    : return _T("analog word");
    case OTD_ANALOG_SHORT   : return _T("analog short");
    case OTD_ANALOG_DWORD   : return _T("analog dword");
    case OTD_ANALOG_LONG    : return _T("analog long");
    case OTD_ANALOG_QWORD   : return _T("analog qword");
    case OTD_ANALOG_LONGLONG: return _T("analog long long");
    //case OTD_POINTER        : return "pointer";
    case OTD_FLOAT          :
    case OTD_SIGNED_FLOAT   : return _T("float");
    case OTD_DOUBLE         :
    case OTD_SIGNED_DOUBLE  : return _T("double");
    //case OTD_TUTR_COMMAND   : return _T("command");
    case OTD_TR_COMMAND_SET : return _T("command_set");
  }
  return _T("unknown type");
}

void __fastcall monitor_otd(TModemmerMonitor * monitor, LPBYTE data, DWORD data_size)
{
 otd_proto op;
 op.dw_size = sizeof(op);
 DWORD len;
 if(otd_proto_parser(data,data_size,&op,&len))
 {
 TCHAR str[MAX_PATH<<4];
 TCHAR str_templ1[128];
 //char str_templ2[128];
 LoadString(ModuleInstance,IDS_MON_OTDADDR,str_templ1,KERTL_ARRAY_COUNT(str_templ1));
 len = swprintf(str,str_templ1,(DWORD)op.addr->pu,(DWORD)op.addr->cp,(DWORD)(op.addr->fa),(DWORD)op.addr->sb);
 if(OTD_FA_ISQUERY(op.addr->fa))
 {
  TCHAR fa_str[64];
  get_fa_str(op.addr->fa,*op.ver_parts,fa_str,KERTL_ARRAY_COUNT(fa_str));
  swprintf(str +len ,_T("%s маска составляющих %08X"),fa_str,(DWORD)((*op.ver_parts)&OTD_PROTO_PARTSMASK));
  monitor->add_line(str);
 }
 else
 {

 if((*op.ver_parts)&OTD_PROTO_FLAG_QUERYRESPOND)
    len+= swprintf(str+len,_T(" ** Ответ на вызов данных **"));
    monitor->add_line(str);

 if(op.time_stamp)
  {
      TCHAR date_time[MAX_PATH];
      get_time_string(date_time,KERTL_ARRAY_COUNT(date_time),op.time_stamp);
      monitor->add_line(date_time);
  }

 if(op.diag)
    {
     swprintf(str ,_T("Диагностика %08X "),*op.diag );
     monitor->add_line(str);
    }


 if(op.name)
   {
    wchar_t w_name[MAX_PATH];
    otd_text_getwstr(op.name,w_name,sizeof(w_name)/sizeof(wchar_t));
//    TCHAR    name[MAX_PATH];
//    KeRTL::Unicode2Ansi(name,w_name);
    swprintf(str,_T("Имя - %s"), w_name);
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
    swprintf(str,_T("%s с %03d по %03d"),(op.addr->fa&OTD_FA_ALL)? _T("ТР") : _T("ТУ"), begin,end);
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

    swprintf(str,_T("Данные с %03d по %03d - тип %s"),begin,end,get_otd_type_string(op.data->otd_type));
    monitor->add_line(str);
    len = 0;
    if(op.addr->sb == 108 )
       op.addr->sb = op.addr->sb;
    for(;begin<=end && op.data;begin++)
    {
     QWORD value = 0;
     otd_get_value(op.data,begin,&value,sizeof(value));
     switch(op.data->otd_type)
     {
      case OTD_DISCRETE :
           len += swprintf(str+len,_T("%1d"),value ? TRUE:FALSE);
      break;
      case OTD_FLOAT:
      case OTD_SIGNED_FLOAT:
       {
        float fv = *((float*)&value);
        len += swprintf(str+len,_T("%.2f "),fv );
       }
      break;
      case OTD_DOUBLE:
      case OTD_SIGNED_DOUBLE:
       {
        double dv = *((double*)&value);
        len += swprintf(str+len,_T("%.2f "),dv );
       }
      break;
      default:
      len += swprintf(str+len,_T("%04X "),(DWORD)value );
      break;
     }

    }
    }

    monitor->add_line(str);
 }
 if(op.personal_diag)
 {
    DWORD begin = (DWORD)op.personal_diag->numbers.loN;
    DWORD end   = (DWORD)op.personal_diag->numbers.hiN;
    swprintf(str,_T("Персональная диагностика с %03d по %03d"),begin,end);
    monitor->add_line(str);
    len = 0;
    for(;begin<=end;begin++)
    {
     DWORD value = 0;
     otd_get_value(op.personal_diag,begin,&value,sizeof(value));
      len += swprintf(str+len,_T("%04X "),value );
    }
    monitor->add_line(str);
 }
 if(op.personal_chmask)
 {
    DWORD begin = (DWORD)op.personal_chmask->numbers.loN;
    DWORD end   = (DWORD)op.personal_chmask->numbers.hiN;
    swprintf(str,_T("Персональная маска изменений с %03d по %03d"),begin,end);
    monitor->add_line(str);
    len = 0;
    for(;begin<=end;begin++)
    {
     DWORD value = 0;
     otd_get_value(op.personal_chmask,begin,&value,sizeof(value));
      len += swprintf(str+len,_T("%04X "),value );
    }
    monitor->add_line(str);
 }

  if(OTD_FA_ISSETUP(op.addr->fa))
  {
     swprintf(str,_T("Настройка  оборудование"));
     monitor->add_line(str);
     if(op.extension && op.extension->block_type == OTD_BLOCKTYPE_ALARM)
     {
       lpotd_alarm_param ap = (lpotd_alarm_param) op.extension->block_data;
       int param_count      OTD_BLOCK_GET_DATA_SIZE(op.extension);
           param_count/=sizeof(*ap);
       for(int i = 0;i<param_count;i++)
           mon_alarm_param(monitor,ap++);
     }
  }

  len = KeRTL::MIN((DWORD)KERTL_ARRAY_COUNT(str),monitor->get_max_line_len());
  std::fill(str,str+len,L'-');
  str[len] = 0;
  monitor->add_line(str);
 }
 }
}

void __fastcall monitor_arch_list   (TModemmerMonitor * monitor, LPOTD_ARCH_LIST al)
{
 TCHAR str[MAX_PATH];
 for(DWORD i = 0;i<al->count;i++)
 {
  QWORD   arch_id = al->archive[i];
  //SYSTEMTIME st;
  int dtl = get_time_string(str,KERTL_ARRAY_COUNT(str),&arch_id);
  snwprintf(str+dtl,sizeof(str)-dtl,_T(" - %Lu "),arch_id);
  monitor->add_line(str);
 }
}



void __fastcall monitor_otd_archieve(TModemmerMonitor * monitor, LPBYTE data, DWORD data_size)
{
 char *cmd_text[] =
 {
   "Неизвестная команда  "
  ,"Запрос списка аварийный архивов"
  ,"Получить аварийные архивы"
  ,"Получить аварийные архивы и удалить"
  ,"Удалить аварийные архивы"
  ,"Новый аварийный архив"
 };

 otd_proto op;
 op.dw_size = sizeof(op);
 DWORD len;
 if(otd_proto_parser(data,data_size,&op,&len))
 {
  if(op.data)
   monitor_otd(monitor,data,data_size);
   else
   {
    LPOTD_ARCH_HEADER ah = (LPOTD_ARCH_HEADER) op.extension->block_data;
    LPOTD_ARCH_LIST   al = (LPOTD_ARCH_LIST  ) ah->data;
    bool is_respond      = OTD_ARCH_CMD_IS_RESPOND(ah->cmd);
    bool is_end          = OTD_ARCH_CMD_IS_END(ah->cmd);
    DWORD cmd            = OTD_ARCH_CMD(ah->cmd);
    char str[MAX_PATH];
    snprintf(str,sizeof(str),"%s%s%s",cmd_text[cmd <=OTD_ARCH_CMD_ALARM_ARCHIEVES_CREATED ? cmd:0],is_end ? " Завершено":"", is_respond ? " Ответ":"");
    monitor->add_line(str);
    if(ah->data_size>=sizeof(*al))
      monitor_arch_list(monitor,al);

   }
 }
}


