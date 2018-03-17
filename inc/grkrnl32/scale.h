#ifndef _SCALE_INC_
#define _SCALE_INC_

#include <gkmodules.h>
#include <vector>

/* Имя хендла менеджера шкал */
#define SCALE_NAME_W L"SCALE"
#define SCALE_NAME_A  "SCALE"

#define SCALE_DATA_VERSION      2  //версия структуры данных в индексном файле

//Ошибки
#define SCL_ERR_SUCCESS         0  //успешно
#define SCL_ERR_NOKVANT         1  //квант выходит за границы
#define SCL_ERR_NOVALUE         2  //значение выходит за грaницы
#define SCL_ERR_NOREVERSE       3  //при обратном расчёте нет решения
#define SCL_ERR_MINWARN         4  //выход за уставку мин. предупредительная
#define SCL_ERR_MAXWARN         8  //выход за уставку макс. предупредительная
#define SCL_ERR_MINALARM        16 //выход за уставку мин. аварийная
#define SCL_ERR_MAXALARM        32 //выход за уставку макс. аварийная

//Флаги наличия уставок
#define SCL_LIMIT_MINWARN       0x00000001
#define SCL_LIMIT_MAXWARN       0x00000002
#define SCL_LIMIT_MINALARM      0x00000004
#define SCL_LIMIT_MAXALARM      0x00000008

//Уведомления
typedef struct _SC_NOTIFY_DATA
{
  DWORD id;                     //id шкалы
  DWORD reason;                 //причина изменения
} SC_NOTIFY_DATA, *LPSC_NOTIFY_DATA;

//Типы уведомления
#define MNF_SCALE               0x20000000      //маска напоминания модуля шкал
#define MNCODE_SCALE_ID         0               //изменена шкала с id
#define MNCODE_SCALE_REFRESH    1               //старт-стоп файла шкал

//Причины вызова уведомления
#define MNCODE_SCALE_CREATE     0x00000001      //создана шкала
#define MNCODE_SCALE_DELETE     0x00000002      //удалена шкала
#define MNCODE_SCALE_NAME       0x00000004      //изменено имя
#define MNCODE_SCALE_LIMITS     0x00000008      //изменены уставки
#define MNCODE_SCALE_SEGMENT    0x00000010      //изменены сегменты

#pragma pack(push,1)

//Все вызовы возвращают GKH_RET_SUCCESS или GKH_RET_ERROR

//Расчитать по шкале
//p1 = (LPSC_CALC) структура для рассчёта
//p2 = (bool)direct  true - прямой расчёт, falst - обратный расчёт
#define  CM_SC_CALC             MCMD(MT_SCALE,1)

typedef struct _SC_CALC
{
  DWORD id;                                     //id шкалы
  DWORD kvant;                                  //квант
  float result;                                 //результат по шкале
  DWORD error;                                  //0-без ошибок, >0 - код ошибки
} SC_CALC, *LPSC_CALC;

//Перечислитель шкал (частичное чтение первых байт шкалы - size=первый DWORD в буфере)
//p1 = (int) порядковый номер шкалы
//p2 = (LPSCALE_ENUM) шкала
#define  CM_SC_ENUM             MCMD(MT_SCALE,2)

typedef struct _SCALE_ENUM
{
  DWORD dw_size;
  DWORD scale_id;
  wchar_t name[MAX_PATH];
} SCALE_ENUM, *LPSCALE_ENUM;

//Выбрать шкалу по имени
//p1 = (char *)/(wchar_t *) краткое имя шкалы
//p2 = (DWORD *) возврат id шкалы
#define  CM_SC_SELECT_A         MCMD(MT_SCALE,3)
#define  CM_SC_SELECT_W         MCMD(MT_SCALE,4)

//Подсчитать количество шкал
//p1 = (int *) возврат количества шкал
#define  CM_SC_COUNT            MCMD(MT_SCALE,5)

//Создать новую шкалу
//p1 = (void *) шкала
//p2 = (DWORD *) возврат id шкалы
#define  CM_SC_CREATE           MCMD(MT_SCALE,6)

//Удалить шкалу
//p1 = (DWORD) id шкалы
#define  CM_SC_DELETE           MCMD(MT_SCALE,7)

//получить размер структуры шкалы (перед чтением)
//p1 = (DWORD) id шкалы
//p2 = (DWORD *) возврат необходимого размера структуры шкалы
#define  CM_SC_SCALE_SIZE       MCMD(MT_SCALE,8)

//записать/прочитать шкалу полностью по id (при чтении - size=первый DWORD в буфере)
//p1 = (DWORD) id шкалы
//p2 = (void *) шкала
#define  CM_SC_SET              MCMD(MT_SCALE,9)
#define  CM_SC_GET              MCMD(MT_SCALE,10)

//прочитать имя файла шкал
//p1 = (char *)/(wchar_t *) буфер под имя файла (0 - чтобы узнать размер в p2)
//p2 = (int *) размер буфера
#define  CM_SC_GET_FILENAME_A   MCMD(MT_SCALE,11)
#define  CM_SC_GET_FILENAME_W   MCMD(MT_SCALE,12)

//установить имя файла шкал
//p1 = (char *)/(wchar_t *) новое имя файла
//p2 = (int) 0 - rename, 1 - copy to, 2 - open other
#define  CM_SC_SET_FILENAME_A   MCMD(MT_SCALE,13)
#define  CM_SC_SET_FILENAME_W   MCMD(MT_SCALE,14)

//сжать файл шкал
#define  CM_SC_SHRINK           MCMD(MT_SCALE,15)

//используется ли индекс по id
//p1 - (bool *) возврат
#define  CM_SC_INDEX_USED       MCMD(MT_SCALE,16)

//размер файла
//p1 - (DWORD) возврат
#define  CM_SC_FILE_SIZE        MCMD(MT_SCALE,17)

//размер неиспользуемого места
//p1 - (DWORD) возврат
#define  CM_SC_FILE_UNUSED      MCMD(MT_SCALE,18)

//Получить гранчные значения по шкале
//p1 = (LPSC_MINMAX)
#define  CM_SC_MINMAX           MCMD(MT_SCALE,19)

typedef struct _SC_MINMAX
{
  DWORD id;
  DWORD min_kv, max_kv;
  float min_y, max_y;
} SC_MINMAX, *LPSC_MINMAX;

#pragma pack(pop)

#ifdef __cplusplus


class TSegment
{
public:
  DWORD min_kvant,max_kvant;
  float k[6];
  float percent;                                //процент нечувствительности
  __fastcall TSegment();
  __fastcall TSegment(DWORD min_kv, DWORD max_kv, float beg, float end);
  void __fastcall SetLineK(float beg, float end);
  bool __fastcall IsLine();
  bool __fastcall InRange(DWORD kvant) {return (min_kvant<=kvant && kvant<=max_kvant)?true:false;}
  float __fastcall calc(DWORD kvant);
  bool __fastcall reverse_calc(float y, DWORD &kvant);
  float __fastcall max_y();
  float __fastcall min_y();
  bool __fastcall operator<(const TSegment &T) const;
  bool __fastcall operator==(const TSegment &T) const;
  void __fastcall Clear();
};

class TScale
{
private:
  DWORD id;                                     //уникальный идентификатор
  wchar_t *s_name;                              //имя шкалы
  std::vector<TSegment> segs;                        //массив сегментов
  void __fastcall Clear();
public:
  float warn_min,warn_max,alarm_min,alarm_max;  //аварийные уставки
  DWORD limits;                                 //флаги наличия уставок
  __fastcall TScale();
  __fastcall TScale(void *buf, DWORD size=0);   //если задан размер, то частичное заполнение
  __fastcall ~TScale();
  DWORD __fastcall GetId() {return id;};
  void __fastcall SetAll_v1(void *buf);             //инициализация данными версии 1
  void __fastcall SetAll(void *buf, DWORD size=0);  //если задан размер, то частичное заполнение
  DWORD __fastcall GetBuffer(void *buf, DWORD size);
  void __fastcall AddSegment(TSegment *seg);
  TSegment * __fastcall GetSegment(int num);
  void __fastcall DeleteSegment(int num);
  DWORD __fastcall MinKvant();
  DWORD __fastcall MaxKvant();
  float __fastcall MinY();
  float __fastcall MaxY();
  void __fastcall SetName(wchar_t *name);
  void __fastcall SetName( char   *name);
  int __fastcall GetName(wchar_t *name, int size);
  int __fastcall GetName( char   *name, int size);
  wchar_t * __fastcall w_str() const;
  DWORD __fastcall Calc(DWORD kvant, float &y);
  DWORD __fastcall ReverseCalc(float y, DWORD &kvant);
};

#endif


#endif


