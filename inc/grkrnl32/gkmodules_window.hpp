/*******************************************************************************
 *
 * Модуль    : gkmodules_window
 * Описание  :  Интерфейс для оконной подсистемы модулей
 * Автор     : Остапенко Д.В.
 * Версия    : 1
 * Создан    : 19.07.2002
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
 Базовый класс для реализации окон-отображателей настроек, просмотра и т.д.
 модуля
 Для всякого окна цикл выборки сообщений должен быть основан на
 MsgWaitForMultipleObjects. При переходе close_event в сигнальное состояние
 все открытые окна должны быть закрыты.
*/


class TGKWindowModule:public TGKModule
{
  protected:
  KeRTL::TFASTMutex  locker;

  KeRTL::TEvent      msg_event;       //Для механизма передачи сообщений
  KeRTL::TEvent      msg_event_done;  // между нитями
         MSG         message;
         bool        message_sending;                
};





#endif

