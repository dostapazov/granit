/*
 Определение адресов стуктур протокола  и пр.
*/

#ifndef _ADDRE_DEF_INC
#define _ADDRE_DEF_INC

#ifndef KE_DEFS_INC
#include <ke_defs.h>
#endif

/*
 Максимальные количество    канала и модема
 номер больше максимального указывает на то что модем не настроен
*/

#pragma pack (push,1)

typedef union _BES_FA
{
 BYTE bfa;
 struct{
        BYTE   fa :6;
        BYTE   ver:2;
       };
}BES_FA,*LPBES_FA;

typedef union _BES_NKDR //Номер кадра
{
 BYTE n_kdr;
 struct
 {
  BYTE number:6;// Номер кадра 1-63
  BYTE repeat:2;// Кол-во повторных передач
 };
}BES_NKDR,*LPBES_NKDR;



typedef struct _BES_HDR
{
 BES_FA      bfa;    // Функциональный адрес
 BYTE        bnch;   // номер канала
 BYTE        b_ctrl; // Байт управления
 BYTE        b_descr;// Байт описатель
 BYTE        b_fmc;  // Функционально модульный check
 BES_NKDR    b_nkdr; // Номер кадра
}BES_HDR,*LPBES_HDR;


//Байт управления
#define BCTRL_DONT_SAVE        0x80 //Не хранить
#define BCTRL_ROUTE            0x40 //Маршрутизировать
#define BCTRL_FIRST_CHANNEL    0x20 //Первый канал
#define BCTRL_IMPORTANT        0x10 //Важно
#define BCTRL_URGENT           0x08 //Срочно
#define BCTRL_ALARM            0x04 //Аварийно
#define BCTRL_CONFIDENTIAL     0x02 //конфиденциально
#define BCTRL_DONT_REPEAT      0x01 //Не повторять

//Байт описатель

#define BDEF_ADDR_TYPE_MASK    0xE0 // Маска варианта адресного блока
#define BDEF_INC               0x10 // Поле вкладышей
#define BDEF_PACKET            0x08 // Пакет данных
#define BDEF_IMMITATION        0x04 // Иммитация
#define BDEF_DIFA              0x02 // Наличие блока ДИФА
#define BDEF_INDEX             0x01 // Наличие индекса данных

//Байт функционально модульный check

#define BFMC_PREV_CHANNEL      0x80 //Предыдущий канал
#define BFMC_SELF_CHANNEL      0x40 //Собственный канал
#define BFMC_VALUE_MASK        0x3F //Код FMC


//Варианты адресного бока

//Одиночная аддресация
typedef struct _BES_ADDR_1
{
 BYTE n_st;   //Номер станции
 BYTE n_mod;  //Номер модуля
}BES_ADDR_1,*LPBES_ADDR_1;

typedef struct _BES_ADDR_2
{
 BES_ADDR_1 dst;//Приемник
 BES_ADDR_1 src;//Источник
}BES_ADDR_2,*LPBES_ADDR_2;

#pragma pack(pop)

#pragma pack(push,8)

//Структура разбора Данных протокола
typedef struct _BESPROTO_PARSED
{
 DWORD cb_sz;
 LPBES_HDR      bes_hdr;      //Адрес заголовка
 DWORD          bes_hdr_size; //его размер

 LPBES_ADDR_1   bes_addr_1;//Адресные блоки
 LPBES_ADDR_2   bes_addr_2;
 DWORD          bes_addr_size;//Размер

 LPVOID         bes_difa;
 DWORD          bes_difa_size;

 LPVOID         bes_index;
 DWORD          bes_index_size;
 LPWORD         hdr_kpk;

 LPBYTE         data;
 DWORD          data_size;
 LPWORD         kpk;
}BESPROTO_PARSED,*LPBESPROTO_PARSED;

#pragma pack(pop)


#ifdef __cplusplus
extern "C" {
#endif

    DWORD WINAPI besproto_parser(LPVOID buffer,DWORD b_sz,LPBESPROTO_PARSED bp ,DWORD cbsz);
    DWORD WINAPI besproto_maker (LPVOID buffer,DWORD b_sz,LPBESPROTO_PARSED  bp ,DWORD  cbsz,WORD pol_del);

#ifdef __cplusplus
} //extern "C" {
#endif

#endif

