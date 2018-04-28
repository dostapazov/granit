/*-----------------------------------------------------------------------------
 11.02.2004 9:49:11
 Модуль:   otd_dblow
 Автор:    Остапенко Д.В.
 Назначение: Определения для ОТД БД - нижнего уровня

-----------------------------------------------------------------------------*/




#ifndef OTD_PROTO_H
#define OTD_PROTO_H 1

#ifdef __linux__
#include <lin_ke_defs.h>
#include <gklib/otd_base.h>
#include <gklib/otd_diag.h>
#include <gklib/otd_addr.h>
#else
#include <ke_defs.h>
#include <otd_base.h>
#include <otd_diag.h>
#include <otd_addr.h>

#endif // _WINDOWS_


#if OTD_PROTO_H == 1
#pragma pack(push,4)
#define  OTD_DEF_POLINOM     64709

struct otd_proto
{
 DWORD       dw_size;
 DWORD       proto_size;
 lpotd_addr  addr;
 LPWORD      ver_parts;
 LPDWORD     diag;
 lpotd_data  data;
 lpotd_data  personal_diag;
 lpotd_data  personal_chmask;
 lpotd_block name;
 LPQWORD     time_stamp;
 lpotd_block error;
 lpotd_block extension;
 LPQWORD     time_stamp_end;
 LPWORD      kpk;
};

#define lpotd_proto  otd_proto*

#define OTD_PROTO_VERMASK   0xF000
//#define OTD_PROTO_PARTMASK  (~OTD_PROTO_VERMASK)
#define OTD_PROTO_GETVER(x) ((x&OTD_PROTO_VERMASK)>>12)

#define OTD_PROTO_PARTSMASK            0x0FFF
#define OTD_PROTO_PART_ALL             OTD_PROTO_PARTSMASK
#define OTD_PROTO_PART_KPK             0x0800
#define OTD_PROTO_PART_DIAG            0x0400
#define OTD_PROTO_PART_DATA            0x0200
#define OTD_PROTO_PART_PERSONAL_DIAG   0x0100
#define OTD_PROTO_PART_PERSONAL_CHMASK 0x0080
#define OTD_PROTO_PART_NAME            0x0040

#define OTD_PROTO_PART_TIMESTAMP       0x0020
#define OTD_PROTO_PART_ERROR           0x0010
#define OTD_PROTO_PART_PROTOEXTENSION  0x0008
#define OTD_PROTO_PART_TIMESTAMP_END   0x0004

/*Флаг поддержки частичной передачи/обработки данных*/
#define OTD_PROTO_FLAG_PARTIAL_DATA    0x0002
//Флаг признак ответа на вызов
#define OTD_PROTO_FLAG_QUERYRESPOND    0x0001

#define OTD_PROTO_FLAGS_MASK           0x0003


struct otd_data_param
{
 DWORD  type ;//Тип
 DWORD  first;//Номер первого
 DWORD  count;//Количество
};

typedef otd_data_param*  lpotd_data_param;

struct otd_proto_param
{
 DWORD dw_size;         //Общий размер данных под параметры
 DWORD parts;           //Составные части
 otd_data_param dt_param;//Параметры данных
 otd_data_param pd_param;//Параметры персональной  диагностики
 otd_data_param cm_param;//Параметры масок изменений
 DWORD name_sz;          //Максимальный размер имени
 DWORD error_size;       //Размер ошибки
 DWORD ext_size;         //Размер расширения
 DWORD ext_type;         //Тип расширения
};

typedef  otd_proto_param*  lpotd_proto_param;

/*Структура управления аварийными уставками*/
struct   otd_alarm_param
{
  otd_param_addr  param_addr;
  WORD   cmd;
  WORD   type;
  DWORD  value;
};

typedef otd_alarm_param* lpotd_alarm_param;

/*
Управление аварийными уставками
1) Перечислить
2) Установить
3) Удалить
4) Удалить всё
*/

#define OTD_ALARM_CMD_LESS    0x0001
#define OTD_ALARM_CMD_MORE    0x0002
#define OTD_ALARM_CMD_ENUM    0x0004

#define OTD_ALARM_CMD_MASK    0x000F

#define OTD_ALARM_CMD_SET     0x1000
#define OTD_ALARM_CMD_DELETE  0x2000
#define OTD_ALARM_CMD_CLEAR   0x4000


#pragma pack(pop)

#pragma pack (push,1)
 //Персональная диагностика для групповых объектов
 struct otd_group_pd_entry
 {
  DWORD      diag;
  otd_addr   addr;
 };
 typedef  otd_group_pd_entry*  lpotd_group_pd_entry;

 #define OTD_GROUP_PD       (OTD_BYTE_TYPE|sizeof(otd_group_pd_entry))

 struct otd_param_value
 {
   DWORD dw_size;
   DWORD type;
   union{
         BYTE   as_byte;
         WORD   as_word;
         DWORD  as_dword;
         QWORD  as_qword;
         float  as_float;
         double as_double;
        }value;
  DWORD diag;
  DWORD pdiag;
  DWORD changes;
 };

 #define lpotd_param_value  otd_param_value*

typedef otd_alarm_param* lpotd_alarm_param;

struct   otd_oscillgramm_param
{
  otd_param_addr  param_addr;
  DWORD           osc_lenght;
};

typedef   otd_oscillgramm_param* lpotd_oscillgramm_param;



#pragma pack(pop)





#ifdef __cplusplus
extern "C"
{
#endif

DWORD WINAPI otd_proto_getver       (LPBYTE buff,DWORD sz);//Получение версии
BOOL  WINAPI otd_proto_get_verpart(LPBYTE buff,DWORD sz,LPDWORD ver,LPDWORD parts);//Получение маски parts

BOOL  WINAPI otd_proto_parser(LPBYTE buff,DWORD sz,lpotd_proto op,LPDWORD real_sz);//Разбор кадра OTD на составные части
DWORD WINAPI otd_proto_calc_size(DWORD parts,DWORD count,DWORD type_data,DWORD type_pd,DWORD type_cm,DWORD name_sz);
DWORD WINAPI otd_proto_calc_size_ex(LPVOID pp,DWORD ver);
DWORD WINAPI otd_proto_format(LPBYTE buf,DWORD bsz,DWORD parts,DWORD start,DWORD count,DWORD type_data,DWORD type_pd,DWORD type_cm,DWORD name_sz,lpotd_proto op);
DWORD WINAPI otd_proto_format_ex   (LPBYTE buf,DWORD bsz,LPVOID pp,DWORD ver,LPVOID op);
DWORD WINAPI otd_proto_format_query(LPBYTE buf,DWORD bsz,lpotd_addr addr,DWORD ver,DWORD parts,LPVOID op);
DWORD WINAPI otd_proto_format_tutr (LPBYTE buf,DWORD bsz,lpotd_addr addr,DWORD obj,WORD tu_command,WORD tu_command_attr,LPVOID param,DWORD param_sz,DWORD ver,LPVOID op );

//Копирвание частей протокола по маске parts
//Даже если parts = 0 то копируется адрес и ver_parts (естесственно parts = 0)
DWORD WINAPI otd_proto_getparts(LPVOID buff,DWORD bsz,DWORD parts,lpotd_proto src);
DWORD WINAPI otd_proto_getparts_ex(LPVOID buff,DWORD bsz,LPVOID op,LPVOID src ,DWORD ver);
BOOL  WINAPI otd_proto_getvalue(lpotd_proto op,DWORD number,LPVOID val,DWORD val_sz,LPDWORD val_type,LPDWORD val_pd,LPDWORD val_chmask);
BOOL  WINAPI otd_proto_setvalue(lpotd_proto op,DWORD number,LPVOID val,DWORD val_sz,DWORD val_pd,BOOL inc_chmask);
BOOL  WINAPI otd_proto_getvalue_ex(lpotd_proto op,DWORD param_num,lpotd_param_value pv);

DWORD WINAPI otd_proto_getparam(lpotd_proto op,lpotd_proto_param opp);
BOOL  WINAPI otd_proto_protect (lpotd_proto op,DWORD sz,DWORD p_del);//расчёт контрольной суммы
BOOL  WINAPI otd_proto_check   (lpotd_proto op,DWORD sz,DWORD p_del);//проверка контрольной суммы
WORD  WINAPI otd_calc_checksum (LPVOID buffer,DWORD bsz,DWORD p_del);

DWORD WINAPI otd_scan_personal_diag(lpotd_proto op); // Сканирование персональной диагностики
DWORD WINAPI otd_cmp_copy          (otd_data *dest,otd_data * src,otd_data  * cm);
DWORD WINAPI otd_proto_format_alarm(LPVOID buffer,DWORD bsz,LPDWORD need_size,lpotd_addr addr,DWORD count,lpotd_proto op);

#ifdef __cplusplus
}


 struct sotd_proto
 {
  otd_proto op;
  sotd_proto (){ZeroMemory(&op,sizeof(op));op.dw_size = sizeof(op);}
  sotd_proto (LPBYTE proto_buf,DWORD len);
  sotd_proto (const otd_proto  & op);
  sotd_proto (const sotd_proto & sop);
  sotd_proto  & operator = (const otd_proto & op);
  otd_proto   & operator()(){return op;}
  LPBYTE        operator()(LPDWORD len);
  bool          operator <(otd_proto & sop);
  bool          operator <(otd_addr  &addr);
  bool          assign(LPBYTE proto_buf,DWORD len);
 };


 inline bool operator == (sotd_proto & sop,otd_addr & addr)
 {
  return (*sop.op.addr) == addr;
 }

 struct sotd_proto_less
 {
   sotd_addr   addr;
   sotd_proto_less(){}
   sotd_proto_less(const otd_addr & _addr):addr(_addr){};
   sotd_proto_less(const sotd_proto_less &sopl){addr.addr = sopl.addr.addr;}
   bool operator ()(const sotd_proto & sop1,const sotd_proto & sop2);
 };

inline   bool sotd_proto_less::operator ()(const sotd_proto & sop1,const sotd_proto & sop2)
  {

    lpotd_addr a1;
    lpotd_addr a2;
    a1 = sop1.op.addr ? sop1.op.addr :&addr;
    a2 = sop2.op.addr ? sop2.op.addr :&addr;
    return (*a1)<(*a2);
  }

 struct sotd_proto_more
 {
   sotd_addr   addr;
   sotd_proto_more(){}
   sotd_proto_more(otd_addr & _addr):addr(_addr){};
   sotd_proto_more(sotd_proto_more &sopl){addr.addr = sopl.addr.addr;}
   bool operator ()(const sotd_proto & sop1,const sotd_proto & sop2);
 };

inline   bool sotd_proto_more::operator ()(const sotd_proto & sop1,const sotd_proto & sop2)
  {

    lpotd_addr a1;
    lpotd_addr a2;
    a1 = sop1.op.addr ? sop1.op.addr:&addr;a2 = sop2.op.addr ? sop2.op.addr:&addr;
    return (*a2)<(*a1);
  }





inline   sotd_proto::sotd_proto(LPBYTE proto_buf,DWORD len)
{
 op.dw_size = sizeof(op);
 otd_proto_parser(proto_buf,len,&op,NULL);
}

inline bool     sotd_proto::assign(LPBYTE proto_buf,DWORD len)
 {
   op.dw_size = sizeof(op);
   return otd_proto_parser(proto_buf,len,&op,NULL) ? true:false;
 }




inline   sotd_proto::sotd_proto (const otd_proto & op)
{   *this = op;  }

inline   sotd_proto::sotd_proto (const sotd_proto & sop)
{   *this = sop.op;  }


inline  LPBYTE        sotd_proto::operator()(LPDWORD len)
{
  if(len)
     *len = op.proto_size;
   return (LPBYTE)op.addr;
}

inline   sotd_proto  & sotd_proto::operator = (const otd_proto & op)
{
 if(op.dw_size == sizeof(op))
   {
    memcpy(&this->op,&op,sizeof(op));
   }
  return *this;
}

inline bool          sotd_proto::operator <(otd_proto & op)
{
   return (*op.addr)<(*op.addr);
}


#endif // __cplusplus

#endif // OTD_VER 1

#endif





