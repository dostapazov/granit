#include <Windows.h>
#include <system.hpp>
#include <vector.h>

enum {TBL_OBJECTS,TBL_NAMES,TBL_COMBINE,TBL_SCALE};
enum {DB_SELECT, DB_INSERT, DB_UPDATE, DB_DELETE};
enum {DB_INC, DB_DEC, DB_LESS, DB_LESS_OR_EQUAL, DB_MORE, DB_MORE_OR_EQUAL};
typedef Set<int,DB_INC,DB_MORE_OR_EQUAL> TConditions;

#define DB_SUCCESS              0
#define DB_ERROR                1 //некласифицированная ошибка
#define DB_RECORD_NOT_FOUND     2 //нет записей по запросу
#define DB_FAULT_PARAMS         3 //параметры несоответсвуют ограничениям
#define DB_RECORD_EXISTS        4 //запись с такими ключами уже есть
#define DB_NOT_FOUND_KEY        5 //индекс ненайден
#define DB_FAULT_RECORD_TABLE   6 //ошибка записи в файл таблицы
#define DB_FAULT_RECORD_INDEX   7 //ошибка записи в файл индекса

//Для запроса используется структура request
//rec - возвращаемое значение
//rec_search - условия поиска (не нулевые поля)
typedef struct _request
{
  DWORD sz;
  WORD table;           //таблица
  WORD work_type;       //необходимое действие DB_SELECT ... DB_DELETE
  LPVOID rec;           //указатель на структуру данных
  LPVOID rec_search;    //указатель на структуру данных - условие выбора
  TConditions condition;//доп. условия DB_INC, DB_DEC и т.п.
  LPVOID find;          //служебная инфа для перебора записей
} request, *lprequest;

//структурa для поиска
typedef struct
{
  WORD table;                   //таблица поиска
  LPVOID rec_search;            //структура записи с параметрами поиска
  TConditions cond;             //условия поиска
  __int64 pos;                  //сдвиг найденной записи
  BOOL indexed;                 //поиск по индексу
  WORD ind_type;                //название файла индекса
  DWORD ind;                    //текущий индекс или номер позиции в positions
  vector<__int64> positions;    //для неиндексируемых, строится при вызове DBFindFirst
} TDBRecSearch;


