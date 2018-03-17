#pragma hdrstop
#ifndef _TLMOBJ_PAINTER_INTERNAL
#define _TLMOBJ_PAINTER_INTERNAL
#endif
#include "tlm_painter.h"




  enum discrete_paint_index_t
  {
   work_out_off = 0,                    // 0  - ������� �� ������ ����
   work_out_on,                         // 1  - ������� �� ������ ���
   normal_off,                          // 2  - ���� - �����
   normal_on,                           // 3  - ���  - �����
   not_confomity_off,                   // 4  - ���� - �������������� 0
   not_confomity_on,                    // 5  - ���  - �������������� 0
   not_confomity_one_off,               // 6  - ���� - �������������� 1
   not_confomity_one_on,                // 7  - ���  - �������������� 1
   back_confomity_one_off,              // 8  - ���� - ������� ������������
   back_confomity_one_on,               // 9  - ���  - ������� ������������
   not_confomity_many_off,              // 10 - ���� - �������������� �����
   not_confomity_many_on,               // 11 - ���  - �������������� �����
   back_confomity_many_off,             // 12 - ���� -   ������������ �����
   back_confomity_many_on,              // 13 - ���  -   ������������ �����
   remote_control_prepare_off,          // 14  - ���� �� ����������
   remote_control_prepare_on,           // 15  - ���  �� ����������
   remote_control_active_off,           // 16  - ���� �� �������
   remote_control_active_on,            // 17  - ���  �� �������
   max_index
  };

  int   __fastcall discrete_painter_t::get_index(const LPMDB_RECORD  record   )
  {
    //����������� ������� �� ��������� ���������� �������
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
           //��������������
           index = not_confomity_off;
          }
          else
          {
           //������������
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


