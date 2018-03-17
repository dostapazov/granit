/*Определения протокола Гранит*/

#ifndef _GRANIT_PROTO_INC_
#define _GRANIT_PROTO_INC_
#ifdef __linux__
#include <lin_ke_defs.h>
#else
#include <ke_defs.h>
#endif

#pragma pack(push,1)

#define GRSC_PROTO_VERSION    0x00010000
#define GRSC_MAX_FRAME_SIZE   0x15
#define GRSC_MIN_FRAME_SIZE   0x04

/*
                +----------------------+---------------------------------+------+
                ¦   данные       0101  ¦ вызов по функциональному        ¦      ¦
                ¦   квитанция    0000  ¦ признаку                        ¦От ЭВМ¦
                ¦   вызов        0001  ¦                       ТС   1000 ¦      ¦
                ¦   опрос        0011  ¦                      ТИТ   1001 ¦      ¦
                ¦                      ¦                      ТИИ   1010 ¦      ¦
                L----------------------+---------------------------------+------+
                ¦   данные       0100  ¦ номер функционального ¦двоичный ¦      ¦
                ¦   нет инфор.   1001  ¦ блока                 ¦код места¦К ЭВМ ¦
                ¦   ошибка       1010  ¦                       ¦блока в  ¦      ¦
                ¦   готовность   1000  ¦                       ¦каркасе  ¦      ¦
                ¦   квитанция    0000  ¦                                 ¦      ¦
                +----------------------+---------------------------------+------+
*/

//Типы пакетов
#define GRKT_RECEIPT     0x00  // Квитанция
#define GRKT_CALL        0x01  // Вызов
#define GRKT_QUERY       0x03  // Опрос
#define GRKT_DATAIN      0x04  // Данные к ЭВМ
#define GRKT_DATAOUT     0x05  // Данные из ЭВМ
#define GRKT_READY       0x08  // Готовность  ЛУ , бТУ
#define GRKT_NODATA      0x09  // Нет информации
#define GRKT_ERROR       0x0A  // Ошибка контроллера КП,

//Вид данных
#define GRINF_DISCONNECT 0x00   // Обрыв связи с КП - ??????????
#define GRINF_TS         0x02   // Дискретные ТС
#define GRINF_TII        0x04   // Аналоговые ТИИ
#define GRINF_TIT        0x06   // Аналоговые ТИТ
#define GRINF_ERRFL      0x08   // Признак ошибки

#define GRINF_TUTR       0xF0   //Информационный признак блока ТУ/ТР
                                //(Фиктивный только для внутреннего использования
                                // для определения типа сублока внутри КП



#define GRKTAFB_RESERV1  0x00
#define GRKTAFB_TUON     0x01   //Включить
#define GRKTAFB_TUOFF    0x02   //Отключить
#define GRKTAFB_TRMORE   0x03   //Больше (регулирование)
#define GRKTAFB_TRLESS   0x04   //Меньше (регулирование)
#define GRKTAFB_CMD1     0x05   //Кодовая команда 1
#define GRKTAFB_CMD2     0x06   //Кодовая команда 2
#define GRKTAFB_RESERV2  0x07
#define GRKTAFB_CALLTS   0x08   //Вызов
#define GRKTAFB_CALLTIT  0x09
#define GRKTAFB_CALLTII  0x0A
#define GRKTAFB_TRCANCEL 0x0E   //Отмена (регулирование)

#define GRPROTO_MKTYPE(type,afb) (BYTE)(((type<<4)&0xF0)|(afb&0x0F))
#define GRPROTO_MKINF(inf,grp)   (BYTE)(((inf<<4)&0xF0)|(grp&0x0F))

/*Формирование Номера КП из  номера этажа и № ЛУ*/
#define GRPROTO_MK_CPNUM(floor,lu)    ((BYTE)((floor<<4)&0xF0)|(lu&0x0F))
/*Получение номера этажа из номера КП*/
#define GRPROTO_GET_FLOOR_NUM(cp)     ((cp>>4)&0x0F)
/*Получение номера ЛУ из номера КП*/
#define GRPROTO_GET_LU_NUM(cp)        (cp&0x0F)



typedef union  _gr_proto_cp
{
 struct
 {
   BYTE line :4; //Номер ЛУ
   BYTE floor:4; //Номер этажа
 };
 BYTE cp_number;//Номер КП
}gr_proto_cp,*lpgr_proto_cp;

typedef union  _gr_proto_type
{
 struct{
   BYTE afb   :4; // Адрес функционального блока
   BYTE ktype :4; // Тип кадра  , если ТИП кадра GRKT_CALL то AFB - принимает GRKTAFB_XXX
   };
 BYTE afb_type;
}gr_proto_type,*lpgr_proto_type;

typedef union _gr_proto_info
{
 struct{
   BYTE grp :4; // Номер гурппы
   BYTE inf :4; // Тип данных
       };
   BYTE grp_inf;
}gr_proto_info,*lpgr_proto_info;


/*Структура разобраного на части кадра*/
typedef struct   _gr_proto
{
 DWORD            dw_size;
 DWORD            kadr_len;
 lpgr_proto_cp    cp;
 lpgr_proto_type  type;
 lpgr_proto_info  info;
 DWORD            data_len;
 LPBYTE           data;
 LPWORD           kpk;
}gr_proto,*lpgr_proto;

typedef struct _gr_proto_data_discrete
{
 BYTE   values [8];
 BYTE   inverse[8];
}gr_proto_data_discrete,*lpgr_proto_data_discrete;

typedef struct _gr_proto_data_analog
{
 BYTE values[16];
}gr_proto_data_analog,*lpgr_proto_data_analog;

typedef struct _gr_proto_data_tu
{
 BYTE   pc_group    [2];
 BYTE   pc_object   [1];
 BYTE   pc_invafb   [2];
 BYTE   pc_invgroup [2];
 BYTE   pc_invobject[1];
}gr_proto_data_tu,*lpgr_proto_data_tu;

/*Структура вывода на щит ТС*/
typedef struct _gr_proto_out_ts
{
 BYTE floor;
 BYTE type_afb;
 BYTE bits    [8];
 BYTE inv_bits[8];
 WORD kpk;
}gr_proto_out_ts,*lpgr_proto_out_ts;

typedef struct _gr_proto_out_ti
{
 BYTE floor;
 BYTE type_afb;
 BYTE reserv;
 BYTE data[3];
 WORD kpk;
}gr_proto_out_ti,*lpgr_proto_out_ti;

typedef struct _gr_proto_out_ready
{
 BYTE floor;
 BYTE type_afb;
 BYTE ready_state  [4];
 BYTE ready_flash  [4];
 BYTE reserv       [4];
 BYTE ready_slaking[4];
 WORD kpk;

}gr_proto_out_ready,*lpgr_proto_out_ready;


#pragma pack(pop)

/*Разбор кадра на составные части*/
#define GRPP_NOERROR             0
#define GRPP_ERR_STRUCT_SIZE    (-1)  //Ошибочный размер структуры
#define GRPP_ERR_INVALID_BUFFER (-2)  //Неправильные входные данные
#define GRPP_ZERO_KADR          (-3)  //Кадр с нулевыми данными
#define GRPP_ERR_KPK            (-4)  //Ошибка КПК
#define GRPP_ERR_INVALID_TYPE   (-5)  //Неправильный тип
#define GRPP_ERR_INVALID_INF    (-6)  //Неправльный вид данных
#define GRPP_ERR_INVPART        (-7)  //Несовпадение прямой и инверсной части
#define GRPP_MAX_ERROR           8

#ifdef __cplusplus
extern "C"{
#endif
DWORD  WINAPI    gr_proto_parser      (LPBYTE kadr,DWORD kadr_len,lpgr_proto gp);
DWORD  WINAPI    gr_proto_format      (BYTE   cp,BYTE type,BYTE info,LPBYTE buff,DWORD bsz,lpgr_proto gp);
DWORD  WINAPI    gr_proto_protect     (lpgr_proto gp);
DWORD  WINAPI    gr_proto_val2poscode (DWORD   value,LPBYTE pc,DWORD pc_size);
DWORD  WINAPI    gr_proto_poscode2val (LPBYTE pc,DWORD pc_size);
DWORD  WINAPI    gr_proto_fill_inverse(LPBYTE src,LPBYTE dst,DWORD len);
DWORD  WINAPI    gr_proto_tu          (BYTE   cp,BYTE tuop,BYTE tu_group,BYTE tu_obj,LPBYTE buff,DWORD bsz);
DWORD  WINAPI    gr_get_ts            (LPBYTE data,DWORD number);
void   WINAPI    gr_set_ts            (DWORD val,LPBYTE data,DWORD number);
#define gr_get_ti(data,number) (DWORD)(data[number])
#define gr_set_ti( val,data,number) data[number] = (BYTE)val

#define STD_GRANIT_POLINOM  0x1021
WORD   WINAPI   gr_calc_kpk(void  *buff  ,WORD len,WORD del);



inline DWORD WINAPI gr_proto_receipt(BYTE cp,BYTE afb,LPBYTE buf,DWORD bsz)
{
 DWORD ret = 0;
 gr_proto gp;gp.dw_size = sizeof(gp);
 if(gr_proto_format(cp,GRPROTO_MKTYPE(GRKT_RECEIPT,afb),0,buf,bsz,&gp) == GRPP_NOERROR)
 {
     gr_proto_protect(&gp);
     ret = gp.kadr_len;
 }
 return ret;
}

inline DWORD WINAPI gr_proto_callts(BYTE cp,LPBYTE buf,DWORD bsz)
{
 DWORD ret = 0;
 gr_proto gp;gp.dw_size = sizeof(gp);
 if(gr_proto_format(cp,GRPROTO_MKTYPE(GRKT_CALL,GRKTAFB_CALLTS),0,buf,bsz,&gp) == GRPP_NOERROR)
    {
     gr_proto_protect(&gp);
     ret = gp.kadr_len;
    }
 return ret;
}

inline DWORD WINAPI gr_proto_calltit(BYTE cp,LPBYTE buf,DWORD bsz)
{
 DWORD ret = 0;
 gr_proto gp;gp.dw_size = sizeof(gp);
 if(gr_proto_format(cp,GRPROTO_MKTYPE(GRKT_CALL,GRKTAFB_CALLTIT),0,buf,bsz,&gp) == GRPP_NOERROR)
    {
     gr_proto_protect(&gp);
     ret = gp.kadr_len;
    }
 return ret;
}

inline DWORD WINAPI gr_proto_calltii(BYTE cp,LPBYTE buf,DWORD bsz)
{
 DWORD ret = 0;
 gr_proto gp;gp.dw_size = sizeof(gp);
 if(gr_proto_format(cp,GRPROTO_MKTYPE(GRKT_CALL,GRKTAFB_CALLTII),0,buf,bsz,&gp) == GRPP_NOERROR)
    {
     gr_proto_protect(&gp);
     ret = gp.kadr_len;
    }
 return ret;
}





#ifdef __cplusplus
}//extern "C"{
#endif



#endif


