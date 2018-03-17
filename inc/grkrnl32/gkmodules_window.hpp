/*******************************************************************************
 *
 * ������    : gkmodules_window
 * ��������  :  ��������� ��� ������� ���������� �������
 * �����     : ��������� �.�.
 * ������    : 1
 * ������    : 19.07.2002
 *
 ******************************************************************************/


#ifndef GKMODULES_WINDOW_INC_H
#define GKMODULES_WINDOW_INC_H

#ifndef _GKHANDLE_INC_HPP
#include <gkhandle.hpp>
#endif

#ifndef GKMODULES_INC_H
#include <GKMODULES.HPP>
#endif

#include <KeRTL.HPP>


/*
 ������� ����� ��� ���������� ����-������������� ��������, ��������� � �.�.
 ������
 ��� ������� ���� ���� ������� ��������� ������ ���� ������� ��
 MsgWaitForMultipleObjects. ��� �������� close_event � ���������� ���������
 ��� �������� ���� ������ ���� �������.
*/


class TGKWindowModule:public TGKModule
{
  protected:
  KeRTL::TFASTMutex  locker;

  KeRTL::TEvent      msg_event;       //��� ��������� �������� ���������
  KeRTL::TEvent      msg_event_done;  // ����� ������
         MSG         message;
         bool        message_sending;                
};





#endif

