#include <otd.h>
#include <gkmodules.h>

#define MEDIUMDB_HANDLE_NAME_A  "MEDIUMDB"
#define MEDIUMDB_HANDLE_NAME_W L"MEDIUMDB"

#define MNF_MEDIUMDB            0x40000000      //маска напоминания окна БД
#define MNCODE_MEDIUMDB_ADDED   0               //добавлен новый объект (автоматически)
#define MNCODE_MEDIUMDB_REFRESH 1               //обновлено текущее состояние объекта

typedef struct _db_addr           //адрес объекта
{
  BYTE pu,cp,fa,sb;
  WORD obj;
  _db_addr(){memset(this,0xFF,sizeof(*this));};
  bool __fastcall operator<(const _db_addr& sec) const;
}db_addr, *lpdb_addr;

class TAddress: db_addr
{
public:
  __fastcall TAddress(db_addr src){SetAddress(src.pu,src.cp,src.fa,src.sb,src.obj);};
  __fastcall TAddress(BYTE pu, BYTE cp, BYTE fa, BYTE sb, WORD obj){SetAddress(pu,cp,fa,sb,obj);};
  void __fastcall SetAddress(BYTE pu, BYTE cp, BYTE fa, BYTE sb, WORD obj);
  db_addr __fastcall GetAddress() const {return *this;};
  bool __fastcall operator<(const TAddress& second) const;
  bool __fastcall operator!=(const TAddress& second);
  TAddress& __fastcall operator=(const TAddress& src);
  TAddress& __fastcall operator+(const TAddress& src);
  TAddress& __fastcall operator++();
  TAddress& __fastcall operator--();
  bool __fastcall IsPU();
  bool __fastcall IsCP();
  bool __fastcall IsFA();
  bool __fastcall IsSB();
  bool __fastcall IsObj();
  db_addr __fastcall GetParent();
  bool __fastcall IsValid(){return (pu<0xFF) || (cp<0xFF) || (fa<0xFF) || (sb<0xFF) || (obj<0xFFFF);};
};

class db_object         //элемент BD
{
  TAddress *addr;
  DWORD data;
  otd_diag diag;
public:
  __fastcall db_object(db_addr a){memset(this,0,sizeof(*this)); addr=new TAddress(a);};
  __fastcall ~db_object(){delete addr;};
};

//Структуры записи в файлы
typedef struct _header  //заголовок файла
{
  DWORD sz;
  DWORD signature;              //подпись файла базы данных
  WORD type;                    //тип файла (таблица, индекс)
  WORD version;                 //версия файла
  __fastcall _header(){memset(this,0,sizeof(*this)); sz=sizeof(*this);};
} header;

//запись в таблицы
typedef struct _rec_objects
{
  DWORD sz;
  DWORD id;             //код записи
  db_addr addr;         //адрес (ПУ, КП, ФА, СубБлок, Объект)
  DWORD id_name;        //код имён
  DWORD id_comb;        //код составного объекта
  DWORD id_scale;       //код шкалы
  __fastcall _rec_objects(){memset(this,0,sizeof(*this)); sz=sizeof(*this); memset(&addr,0xFF,sizeof(addr));};
} rec_objects, *lprec_objects;

typedef struct _rec_names
{
  DWORD sz;
  DWORD id;                            //код записи
  _TCHAR s_name[ANYSIZE_ARRAY];        //краткое название
  _TCHAR f_name[ANYSIZE_ARRAY];        //полное название
  __fastcall _rec_names(){memset(this,0,sizeof(*this)); sz=sizeof(*this);};
} rec_names, *lprec_names;

typedef struct _rec_combine
{
  DWORD sz;
  DWORD id;                             //код записи
  _TCHAR formula[ANYSIZE_ARRAY];        //формула
  __fastcall _rec_combine(){memset(this,0,sizeof(*this)); sz=sizeof(*this);};
} rec_combine, *lprec_combine;

typedef struct _rec_scale
{
  DWORD sz;
  DWORD id;             //код записи
  FLOAT a[8];           //кооэфициенты шкалы
  BYTE lo_kv, hi_kv;    //нижний и верхний кванты
  FLOAT percent;        //% нечувствительности
  __fastcall _rec_scale(){memset(this,0,sizeof(*this)); sz=sizeof(*this);};
} rec_scale, *lprec_scale;

//Обработать запись объекта
//p1 = (lprequest) req
//ret - DWORD
#define  DBCM_REQUEST           MCMD(MT_MEDIUMDB,1)

