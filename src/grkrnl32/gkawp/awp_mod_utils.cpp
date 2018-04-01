#pragma hdrstop
#include "gkawpmodule.hpp"
#include "awp_main_form.h"
#include "record_info.h"


UnicodeString   __fastcall TGkAwpModule::get_state_tu_text(DWORD state)
{
  UnicodeString ret;
   if(state & (MDBR_STATE_TUTR_ERROR|MDBR_STATE_TUTR_PREPARE|MDBR_STATE_TUTR_ON_MORE|MDBR_STATE_TUTR_OFF_LESS))
   {
    ret = L"ТУ:";
    if(state&MDBR_STATE_TUTR_PREPARE    ) ret+= L"Подготовка ";
    if(state&MDBR_STATE_TUTR_ON_MORE    ) ret+= L"Активно-ВКЛ ";
    if(state&MDBR_STATE_TUTR_OFF_LESS   ) ret+= L"Активно-ОТКЛ ";
    if(state&MDBR_STATE_TUTR_ERROR       ) ret+= L"Ошибка ";
   }
  return ret;
}



UnicodeString   __fastcall TGkAwpModule::get_state_text(DWORD state)
{
  UnicodeString ret;
   if(state&MDBR_STATE_DISCRETE      ) ret+= L" Дискретный ";
   if(state&MDBR_STATE_WORKOUT       ) ret+= L" Выведен из работы ";
   if(state&MDBR_STATE_TUFLAG        ) ret+= L" Наличие ТУ ";
   if(state&MDBR_STATE_HIDDEN        ) ret+= L" Скрытый ";
   if(state&MDBR_STATE_STATIC        ) ret+= L" Статичный ";
   if(state&MDBR_STATE_NORMAL        ) ret+= L"";
   if(state&MDBR_STATE_NORECEIPT     ) ret+= L" Неквитируемый ";
   if(state&MDBR_STATE_PREKVIT       ) ret+= L"";
   if(state&MDBR_STATE_FLASH         ) ret+= L" Мигание ";
   if(state&MDBR_STATE_DYN_NORMAL    ) ret+= L"";
   ret += get_state_view_text(state);
   ret += get_state_tu_text  (state);
  return ret;
}

UnicodeString   __fastcall TGkAwpModule::get_state_view_text(DWORD state)
{
  UnicodeString ret;
  if(state & MDBR_STATE_WORKOUT)
     ret +=L" Выведен из работы";
  else
  {
   if(state&MDBR_STATE_ALARM         ) ret+= L" Авария";
     else
     {
      if(state&MDBR_STATE_DANGER     ) ret+= L" Предупреждение ";
     }
  }
  return ret;
}

UnicodeString __fastcall TGkAwpModule::get_time_text(__int64 tmval,char time_sep,bool msec)
{
        UnicodeString text;
        SYSTEMTIME st;
        FileTimeToSystemTime((LPFILETIME)&tmval,&st);
        text.printf(L"%04hd-%02hd-%02hd %02hd%c%02hd%c%02hd",st.wYear,st.wMonth,st.wDay,st.wHour,(char)time_sep,st.wMinute,(char)time_sep,st.wSecond);
        //int _msc = tmval%10000;
        if(msec) text.cat_printf(L".%03hd",st.wMilliseconds);
        return text;
}



void      __fastcall TGkAwpModule::show_record_info  (const DWORD rec_id,bool _set)
{
   if(main_form && (_set || RecordInfo))
      PostMessage(main_form->Handle,WM_SHOW_REC_INFO,_set,rec_id);
}

bool __fastcall TGkAwpModule::do_play_alarm_sound()
{
 bool ret = false;
 if(!thread_term_request)
   {
     DWORD cnt     = 0;
     UnicodeString snd_name;
     {
      locker l(mut);
      swap(cnt,alarms_count);
      snd_name = params.sound_alarm;
     }

     if(!snd_name.IsEmpty()&& cnt)
        ret = PlaySoundW(snd_name.c_str(),NULL,SND_FILENAME);
   }
 return ret;
}

bool __fastcall TGkAwpModule::do_play_danger_sound()
{
 bool ret = false;
 if(!thread_term_request)
   {
     DWORD cnt     = 0;
     UnicodeString snd_name;
     {
      locker l(mut);
      swap(cnt,danger_count);
      snd_name = params.sound_danger;
     }

     if(!snd_name.IsEmpty() && cnt)
        ret = PlaySoundW(snd_name.c_str(),NULL,SND_FILENAME);
   }
 return ret;
}

bool __fastcall TGkAwpModule::do_play_disparty_sound()
{
 bool ret = false;
 if(!thread_term_request)
   {
     DWORD cnt     = 0;
     UnicodeString snd_name;
     {
      locker l(mut);
      swap(cnt,this->disparity_count);
      snd_name = params.sound_disparity;
     }
     if(!snd_name.IsEmpty() && cnt )
        ret = PlaySoundW(snd_name.c_str(),NULL,SND_FILENAME);
   }
 return ret;
}

bool __fastcall TGkAwpModule::do_play_back_sound()
{
 bool ret = false;
 if(!thread_term_request)
   {
     DWORD cnt     = 0;
     UnicodeString snd_name;
     {
      locker l(mut);
      swap(cnt,this->back_count);
      snd_name = params.sound_back;
     }

     if(!snd_name.IsEmpty()&& cnt)
        ret = PlaySoundW(snd_name.c_str(),NULL,SND_FILENAME);

   }
 return ret;
}



int       TGkAwpModule::sound_thread_proc()
{
  while(!thread_term_request)
  {
    Sleep(100);
    if(!do_play_alarm_sound())
      {
       if(!do_play_danger_sound())
          {
            if(!do_play_disparty_sound())
                do_play_back_sound();
          }
      }
  }
  PlaySound(NULL,NULL,0);
  return 0;
}

