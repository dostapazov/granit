#pragma hdrstop
#ifndef _TLMOBJ_PAINTER_INTERNAL
#define _TLMOBJ_PAINTER_INTERNAL
#endif
#include "tlm_painter.h"




  enum discrete_paint_index_t
  {
   work_out_off = 0,                    // 0  - выведен из работы откл
   work_out_on,                         // 1  - выведен из работы вкл
   normal_off,                          // 2  - откл - норма
   normal_on,                           // 3  - вкл  - норма
   not_confomity_off,                   // 4  - откл - несоответствие 0
   not_confomity_on,                    // 5  - вкл  - несоответствие 0
   not_confomity_one_off,               // 6  - откл - несоответствие 1
   not_confomity_one_on,                // 7  - вкл  - несоответствие 1
   back_confomity_one_off,              // 8  - откл - возврат соответствие
   back_confomity_one_on,               // 9  - вкл  - возврат соответствие
   not_confomity_many_off,              // 10 - откл - несоответствие много
   not_confomity_many_on,               // 11 - вкл  - несоответствие много
   back_confomity_many_off,             // 12 - откл -   соответствие много
   back_confomity_many_on,              // 13 - вкл  -   соответствие много
   remote_control_prepare_off,          // 14  - откл ту подготовка
   remote_control_prepare_on,           // 15  - вкл  ту подготовка
   remote_control_active_off,           // 16  - откл ту активно
   remote_control_active_on,            // 17  - вкл  ту активно
   max_index
  };

  int   __fastcall discrete_painter_t::get_index(const LPMDB_RECORD  record   )
  {
    //Определение индекса по состоянию дисретного сигнала
    discrete_paint_index_t index = work_out_off;
    int curr_pos = (int)record->value;

    if(!is_record_workout(record))
    {
      if(is_record_rc_state(record))
      {
         if(is_record_rc_acitive(record))
            index = remote_control_active_off;
            else
            index = remote_control_prepare_off;
      }
      else
      {
          if(!is_record_normal(record))
          {
           //Несоответствие
           index = not_confomity_off;
          }
          else
          {
           //соответствие
             index = normal_off;
          }
      }
    }

    if(curr_pos) index = (discrete_paint_index_t)(int(index)|1);
    return index;
  }

  void  __fastcall discrete_painter_t::paint    (const LPTLMOBJ_PAINT_PARAM1  pparam,const Tpaint_variant * pvar)
  {
     if(!pparam->erase)
     {

     }
  }


