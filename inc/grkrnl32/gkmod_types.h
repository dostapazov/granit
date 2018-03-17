#ifndef _GKMOD_TYPES_INC_
#define _GKMOD_TYPES_INC_

//Предопределённые типы модулей
#define MT_UNKNOWN      WORD(-1) //модуль неизвестного типа

#define MT_KERNEL             1        //Модуль ядра Гранит-Н
#define MT_REPORTER           0x0002   //Модуль для сообщений об ошибках


#define MT_TLMDB              0x0010   //Модуль телемеханической БД
#define MT_MEDIUMDB           0x0020   //База данных среднего уровня
#define MT_SECURITY           0x0040   //Модуль данных пользователей
#define MT_MEDIUMDB_HISTORY   0x0080   //Модуль хранения истории

#define MT_MODEMMER           0x0200   //Модуль модеммера
#define MT_IS_MODEM(type)     (MT_MODEMMER==(type&0xFF00))

#define MT_SCALE              0x0800   //Шкалы

#define MT_TLMVIEWER          0x1000   //Просмотрищик
#define MT_TLM_WORK_PLACE     0x2001   //Арм диспетчера

#define MT_IO                 0x1000   //Модуль ввода вывода
#define MT_WINDOW             0x4000   //Модуль окна
#define MT_USER               0x8000

#endif
