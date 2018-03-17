#pragma hdrstop
#include <Text_viewer.hpp>
#include <otdm_proto.h>
#include <stdlib.h>
#include <stdio.h>
#include "modemmer_wnd.rh"

#ifdef UNICODE
#define mprintf swprintf
#else
#define mprintf sprintf
#endif

TCHAR * error_text[] =
{
 _T(""),
 _T("Недостаточно памяти для выполнения запроса"),
 _T("Отсутсвует кадр ID "),
 _T("У кадра нет изображения"),
 _T("Ошибка открытия файла"),
 _T("Нет такой записи")
 };

void __fastcall mon_otdm_error(KeWin::TTextViewer<char> * monitor,LPOTDM_PROTO_HEADER omph)
{
  TCHAR err_text[MAX_PATH];
  if(omph->error<6)
    {

     mprintf(err_text,_T("%s  %d"),error_text[omph->error],omph->data[0]);
    }
    else
     mprintf(err_text,_T("Ошибка %d - %d"),omph->error,omph->data[0]);
     monitor->add_line(err_text);

}

void __fastcall monitor_otd_sepline(KeWin::TTextViewer<char> *monitor)
{
     TCHAR text_buffer[2048];
     TCHAR *sep_text = _T("-");
     DWORD max_len = monitor->get_max_line_len();
     for(int i = 0;i<(int)max_len;i++)
         text_buffer[i]=*sep_text;
     text_buffer[max_len] = 0;
     monitor->add_line(text_buffer);

}

void __fastcall monitor_otd_medium(KeWin::TTextViewer<char> *monitor, LPBYTE data, DWORD data_size)
{
  if(data && data_size)
  {
   TCHAR text_buffer[8192];
   *text_buffer = 0;
   DWORD text_len = 0;
   LPOTDM_PROTO_HEADER omph = (LPOTDM_PROTO_HEADER)data;

  if(omph->command&OTDM_RESPOND)
    lstrcat(text_buffer,_T("Ответ "));
  if(omph->command&OTDM_BEGINDATA)
    lstrcat(text_buffer,_T(" Начало  данных"));


  if(omph->command&OTDM_ERROR)
   mon_otdm_error(monitor,omph);
   else
   {
     switch(omph->command&OTDM_COMMAND_MASK)
     {
      case OTDMPROTO_CMD_GETKADR:
      case OTDMPROTO_CMD_ENUM_KADRS:
     if(omph->command&OTDM_RESPOND)
     {
        monitor->add_line(text_buffer);
        LPOTDM_PROTO_KADR kadr = (LPOTDM_PROTO_KADR )omph->data;
        text_len = mprintf (text_buffer,
        _T("Кадр ID %d объектов %d диагностика %d сотояние %04X "),
        kadr->kadr.kadr_id,kadr->kadr.entry_count,kadr->kadr.kadr_diag,
        kadr->kadr.kadr_state);

        if(kadr->mask&MDB_KADR_FIELD_TIMES)
           {
            text_len += mprintf (text_buffer+text_len,_T("Изменение изображения"));
           }


        char skadr_name [MAX_PATH];
        char kadr_name [MAX_PATH];
        *skadr_name = *kadr_name = 0;
        wchar_t * kname = kadr->names;
        if(kadr->mask&MDB_KADR_FIELD_SNAME)
        {
        Unicode2Ansi(skadr_name,kname);
        kname+=lstrlenW(kname)+1;
        }
        if(kadr->mask&MDB_KADR_FIELD_NAME)
        {
        Unicode2Ansi(kadr_name,kname);
        }
        text_len+=mprintf(text_buffer+text_len,_T(" %s [ %s ]"),kadr_name,skadr_name);
     }
     else
     {
      if((omph->command&OTDM_COMMAND_MASK) == OTDMPROTO_CMD_ENUM_KADRS )
      text_len = mprintf (text_buffer,  _T("Запрос на перечисление кадров") );
      else
      {
        text_len = mprintf (text_buffer,  _T("%s %d"),omph->data[1] ? _T("Подписка на кадр") : _T("Отказ от подписки") , omph->data[0] );
      }

     }

     break;
      case OTDMPROTO_CMD_GETKADR_IMAGE:
                                       if(omph->command&OTDM_RESPOND)
                                       {
                                       if(omph->command&OTDM_BEGINDATA)
                                        {
                                         monitor->add_line(text_buffer);
                                         LPOTDM_PROTO_IMAGE_BEGIN img_beg = (LPOTDM_PROTO_IMAGE_BEGIN)omph->data;
                                         char img_name[MAX_PATH];
                                         *img_name = 0;
                                         Unicode2Ansi(img_name,img_beg->image_name);
                                         text_len = mprintf(text_buffer,_T("кадр %d изображение фона  %s  размер %d байт"),img_beg->kadr_id, img_name,img_beg->image_size);
                                        }
                                        else
                                        {
                                         if(omph->command&OTDM_ENDDATA)
                                         {
                                         monitor->add_line(text_buffer);
                                         LPOTDM_PROTO_IMAGE_CHUNK img_chunk = (LPOTDM_PROTO_IMAGE_CHUNK)omph->data;
                                         text_len = mprintf(text_buffer,_T("кадр %d фрагмент изображения %d байт"), img_chunk->kadr_id,img_chunk->chunk_size);
                                         }
                                         else
                                          *text_buffer = 0;

                                        }
                                       }
                                       else
                                       text_len = mprintf (text_buffer,_T("Запрос изображение кадра %d"),omph->data[0]);

                                        break;

   case OTDMPROTO_CMD_GETKADR_RECORDS :
   if(omph->command&OTDM_RESPOND)
      {
       LPOTDM_PROTO_KADR_RECORDS  krecs = (LPOTDM_PROTO_KADR_RECORDS)omph->data;
       mprintf (text_buffer,_T("Записи кадра %d"),krecs->kadr_id);
       monitor->add_line(text_buffer);
       for(DWORD i = 0;i<krecs->rec_count;i++)
       {
        mdb_short_record rec(krecs->records[i]);
        mprintf(text_buffer,_T("Id = %d значение %.2f диагностика %d сотояние %04X"),rec.rec_id,(double)rec.value,rec.diag,(DWORD)rec.state);
        monitor->add_line(text_buffer);
        *text_buffer = 0;
       }
      }
      else
       mprintf(text_buffer,_T("Запрос записей кадра %d"),omph->data[0]);

      break;
      case OTDMPROTO_CMD_GETKADR_ENTRYES :
     if(omph->command&OTDM_RESPOND)
      {
       LPOTDM_PROTO_KADR_ENTRYES entryes = (LPOTDM_PROTO_KADR_ENTRYES)omph->data;
       mprintf (text_buffer,_T("содержимое кадра %d  элементов %d"),entryes->kadr_id,entryes->entryes_count);
       monitor->add_line(text_buffer);
       text_len = 0;
       DWORD counter = 1;
       for(DWORD i = 0;i<entryes->entryes_count;i++)
       {
        text_len+=mprintf(text_buffer+text_len,_T("[ №-%03d  record_id %03d ] "),entryes->entryes[i].number,entryes->entryes[i].rec_id);
        if( (counter++%4)==0)
          {
           monitor->add_line(text_buffer);
           text_len = 0;
           counter = 1;
          }

       }
       monitor->add_line(text_buffer);
       *text_buffer = 0;
      }
      else
       mprintf(text_buffer,_T("Запрос содержимого кадра %d"),omph->data[0]);

      break;
     case OTDMPROTO_CMD_RECORDS:
     {
       monitor->add_line(_T("Изменившиеся значения:"));
       LPOTDM_PROTO_RECORDS records = (LPOTDM_PROTO_RECORDS)omph->data;
       LPBYTE src_ptr = (LPBYTE)records->records;
       mdb_record rec;
       __int64 rest_time = 0;
       for(DWORD i = 0;i<records->rec_count;i++)
       {
        LPOTDM_PROTO_RECORD proto_rec = (LPOTDM_PROTO_RECORD) src_ptr;
        src_ptr+=sizeof(proto_rec->mask);
        src_ptr+= otdm_unpack_record(src_ptr,omph->data_size,proto_rec->mask,&rec);
        if(proto_rec->mask&MDBR_FIELD_TIME)
           rest_time = rec.time;
           else
           {
            if(rest_time)
            {
             rec.time = rest_time;
             proto_rec->mask|= MDBR_FIELD_TIME;
            }
           }

        text_len = mprintf (text_buffer,_T("Id = %d изменения %06X "),rec.rec_id,proto_rec->mask);
        if(proto_rec->mask & MDBR_FIELD_VALUE)
           text_len+= mprintf (text_buffer+text_len,_T(" значение   %.3f  "),(float)rec.value);
        if(proto_rec->mask & MDBR_FIELD_DIAG)
           text_len+= mprintf (text_buffer+text_len,_T(" диагностика %03d "),rec.diag);
        if(proto_rec->mask & MDBR_FIELD_STATE)
           text_len+= mprintf (text_buffer+text_len,_T(" состояние   %04X "),(DWORD)rec.state);
        if(proto_rec->mask & MDBR_FIELD_OPTIONS)
           text_len+= mprintf (text_buffer+text_len,_T(" opts    %04X "),(DWORD)rec.options);
        if(proto_rec->mask & MDBR_FIELD_KVANTS)
           text_len+= mprintf (text_buffer+text_len,_T(" kvants  %d "),(__int32)rec.kvants);
        if(proto_rec->mask & MDBR_FIELD_OTDVALUE)
        {
           text_len+=mprintf (text_buffer+text_len,_T(" otd value "));
           if(proto_rec->mask & MDBR_FIELD_OPTIONS)
           {
               if(rec.options &MDBR_OPT_DOUBLE_TYPE)
                  text_len+= mprintf (text_buffer+text_len,_T("%4.2lf"),rec.otd_val_dbl);
                  else
                  {
                   if(rec.options &MDBR_OPT_FLOAT_TYPE)
                     text_len+= mprintf (text_buffer+text_len,_T("%4.2f"),rec.otd_val_flt);
                     else
                     text_len+= mprintf (text_buffer+text_len,_T("%4ld"),(LONGLONG)rec.otd_val);
                  }
           }
           else
           text_len+= mprintf (text_buffer+text_len,_T("%5ld"),(LONGLONG)rec.otd_val);
        }

        if(proto_rec->mask & MDBR_FIELD_OTDPD)
           text_len+= mprintf (text_buffer+text_len,_T(" otd pd    %4X "),(LONGLONG)rec.otd_pd);

        if(proto_rec->mask& MDBR_FIELD_TIME)
        {
         SYSTEMTIME st;
         FileTimeToSystemTime((LPFILETIME)&rec.time,&st);
         text_len+=mprintf(text_buffer+text_len,_T(" Время %02d:%02d:%02d.%03d"),(int)st.wHour,(int)st.wMinute,(int)st.wSecond,(int)st.wMilliseconds);
        }
           text_len = text_len;

        monitor->add_line(text_buffer);
        *text_buffer = 0;

       }
     }
     break;

     case OTDMPROTO_CMD_GET_RECORD_SNAME      :
     case OTDMPROTO_CMD_GET_RECORD_NAME       :
     case OTDMPROTO_CMD_GET_RECORD_FULL_SNAME :
     case OTDMPROTO_CMD_GET_RECORD_FULL_NAME  :
     if(omph->command&OTDM_RESPOND)
     {
       LPOTDM_PROTO_RECORD_NAME rec_name =(LPOTDM_PROTO_RECORD_NAME) omph->data;
       if(rec_name->len)
         {
          int len = mprintf (text_buffer,_T("Имя записи %d"), rec_name->rec_id);
          if(rec_name->name_id!=(DWORD)-1)
            mprintf (text_buffer+len,_T(" ID имени %04X"), rec_name->name_id);
          monitor->add_line(text_buffer);
          rec_name->text[rec_name->len] = 0;
          #ifdef UNICODE
           lstrcpy(text_buffer,rec_name->text);
          #else
           Unicode2Ansi(text_buffer,rec_name->text);
          #endif
          text_buffer[rec_name->len] = 0;
          monitor->add_line(text_buffer);
          *text_buffer = 0;
         }
     }
     else
     {
      mprintf (text_buffer,_T("Запрос %s имени записи %d"),(omph->command&OTDM_COMMAND_MASK) == OTDMPROTO_CMD_GET_RECORD_SNAME ? _T("короткого"):_T("") , omph->data[0]);
     }
     break;
   case OTDMPROTO_CMD_TUOPERATION:
   {
    LPOTDM_PROTO_TUOPERATION tuop =(LPOTDM_PROTO_TUOPERATION)omph->data;
    mprintf (text_buffer,_T("Операция ТУ/ТР ID записи %d код операции %d"),tuop->rec_id,tuop->op_code);
   }
   break;
   case OTDMPROTO_CMD_NOTIFY_DBCHANGE:
      mprintf (text_buffer,_T("******СМЕНА БАЗЫ ********"));
   break;
     default:
      mprintf (text_buffer,_T("Неизвестная OTD Команда %04X"),omph->command&OTDM_COMMAND_MASK);
     break;
   }
   if(*text_buffer)
      {
       monitor->add_line(text_buffer);

      }
  if(omph->command&OTDM_ENDDATA)
     monitor->add_line(_T("завершение данных"));
    monitor_otd_sepline(monitor);

   }
  }

}

TCHAR* arch_command_names[] =
{
  _T("Запрос данных истории")         /*OTDMPROTO_CMD_QUERY_ARCHIVE*/
 ,_T("Запрос списка  архивов по кадру")/*OTDMPROTO_CMD_QUERY_KADR_ARCHIVES*/
 ,_T("Запрос списка  архивов по кадру")/*OTDMPROTO_CMD_ARCHIVE_SET_PROPS*/
 ,_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T(""),_T("")
 ,_T("Проверка наличия модуля историй")  /*OTDMPROTO_CMD_ARCHIVE_PRESENT*/
 ,_T("")
};

void __fastcall monitor_otd_medium_alarm_archive  (KeWin::TTextViewer<char> *monitor, LPBYTE data, DWORD data_size)
{
   TCHAR mon_text[8192];
   LPOTDM_PROTO_HEADER         omph          = (LPOTDM_PROTO_HEADER)data;
   LPOTDM_ARCHIVE_HEADER       arch_hdr      = (LPOTDM_ARCHIVE_HEADER)  omph->data;
   LPOTDM_ARCHIVE_QUERY        aquery        = (LPOTDM_ARCHIVE_QUERY)   arch_hdr->data;
   LPOTDM_KADR_ARCHIVE_RESPOND arch_respond  = (LPOTDM_KADR_ARCHIVE_RESPOND) arch_hdr->data;

   DWORD command = (omph->command&OTDM_COMMAND_MASK);
   TCHAR * cmd_text = _T("");
   if(command>=OTDMPROTO_CMD_QUERY_ARCHIVE && command <= OTDMPROTO_CMD_ARCHIVE_PRESENT)
      cmd_text = arch_command_names[command-OTDMPROTO_CMD_QUERY_ARCHIVE];
   mprintf(mon_text,_T("Команда %X [%s %s%s] размер данных %u: Заголовок архива  id = %X")
                        , command
                        , cmd_text
                        ,(omph->command&OTDM_RESPOND) ? _T("Ответ "): _T("")
                        ,(omph->command&OTDM_ENDDATA) ? _T("Завершение "): _T("")
                        , omph->data_size
                        ,(DWORD)arch_hdr->hist_id
                       );

   monitor->add_line(mon_text);

   monitor_otd_sepline(monitor);

}





