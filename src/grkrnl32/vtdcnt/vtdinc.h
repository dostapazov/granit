#ifndef _STP_VTD_INC
#define _STP_VTD_INC

#include <ke_defs.h>

#pragma pack(push,1)

typedef struct _vtd_kadr
{
 BYTE dev_number;   //Номер устройства
 BYTE command;      // команда в документации обозначена как КИ - код информации
 BYTE data[ANYSIZE_ARRAY];
} vtd_kadr,*lpvtd_kadr;

typedef struct _vtd_resp_kadr
{
 BYTE dev_number;   //Номер устройства
 BYTE command;      // команда в документации обозначена как КИ - код информации
 BYTE data_len;
 BYTE data[ANYSIZE_ARRAY];
} vtd_resp_kadr,*lpvtd_resp_kadr;


#define VTD_GET_RESPOND_DATA_SIZE(vk) ((DWORD)vk->data[0]);

typedef struct _vtd_query_param // Параметры команды
{
 BYTE qp[4];
 WORD crc;
}vtd_query_param,*lpvtd_query_param;


#define VTD_QUERY_PARAM		0xB0 //Команда запроса параметров

#define VTD_QUERY_DATE_TIME 	0xB1 // Запрос номера,даты и времени

typedef struct _vtd_date
{
  BYTE day,mon,year,reserv;

}vtd_date,*lpvtd_date;

typedef struct _vtd_time
{
  BYTE sec,min,hour,reserv;
}vtd_time,*lpvtd_time;

typedef struct _vtd_date_time
{
 vtd_date date;
 vtd_time time;
}vtd_date_time,*lpvtd_date_time;


 union vtd_digits{
       BYTE            bt  [4];
       struct{
              BYTE    dig1:4;
              BYTE    dig2:4;
              BYTE    dig3:4;
              BYTE    dig4:4;
              BYTE    dig5:4;
              BYTE    dig6:4;
              BYTE    dig7:4;
              BYTE    dig8:4;
             };
      };


typedef struct _vtd_num_date_time
{
 vtd_digits          nc_code;
 vtd_date        curr_date;
 vtd_time        curr_time;
 vtd_date        prevrep_date;
 vtd_date        lastrep_date;
 vtd_date_time   start_dt[10];
 WORD            crc;
}vtd_num_date_time,*lpvtd_num_date_time;

// Вызов основных параметров конфигурации труб и потребителей
#define VTD_QUERY_BASE_CONFIG 0xB2

typedef struct _vtd_Pj01
{
 DWORD reserv1;
 float Pj01;
}vtd_Pj01,*lpvtd_Pj01;

typedef struct _vtd_Pj00
{
 BYTE Pj00[12];
}vtd_Pj00,* lpvtd_Pj00;

typedef struct _vtd_base_config
{
 vtd_digits   ki[10];
 vtd_Pj01     Pj01[10];
 vtd_Pj00     Pj00[10];
 WORD crc;
}vtd_base_config,*lpvtd_base_config;

#define VTD_QUERY_CURRENT_PARAM 0xB3

typedef struct _vtd_pipe_param
{
 vtd_time      time;
 float         pressure [10];
 float         temp     [10];//Температура прямая
 float         temp_back[10];//Температура обратная
 float         Gc       [10];//Массовый расход
 float         Vc       [10];//Тотадная масса
 float         Nk       [10];//Тепловая мощьность
}vtd_pipe_param,*lpvtd_pipe_param;

typedef struct _vtd_user_param
{
 float          W [10];//тотальная тепловая (электрическая) энергия потребителей
 float          Qc[10];//массовый расход утечек (приведенный объемный расход утечек) потребителей
 float          Vc[10];//тотальная масса утечек (тотальный приведенный объем утечек) потребителей
 float          Wl[10];//тотальная тепловая (электрическая) энергия потребителей по льготе
}vtd_user_param,*lpvtd_user_param;

#define VTD_QUERY_ERRORS_ARCHIVE  0xA0


#define VTD_QUERY_PARAM_DAYS_ARCHIVE  0xA1



#define VTD_QUERY_PARAM_HOURS_ARCHIVE  0xA2


/*Описатели протокола ВТД-У*/

typedef struct _vtdy_query_param // Параметры команды
{
 BYTE qp[6];
 WORD crc;
}vtdy_query_param,*lpvtdy_query_param;

#define VTDY_QUERY_USER_DEFINED_PARAM 0x50


#define VTDY_QUERY_CURRENT_PARAM        0x51

#define VTDY_QUERY_MEASURE_PARAM        0x52

#define VTDY_QUERY_VALUES_FOR_CALC      0x53

#define VTDY_QUERY_PARAM_HOURS_ARCHIVE  0x54

#define VTDY_QUERY_PARAM_DAYS_ARCHIVE   0x55

#define VTDY_QUERY_PARAM_MONS_ARCHIVE   0x56

#define VTDY_QUERY_ERRORS_ARCHIVE       0x57

#define VTDY_QUERY_ERRORS_ARCHIVE       0x57

#define VTDY_QUERY_TOTAL                0x58

#define VTDY_SET_CONFIG_VALUE           0x60



#pragma pack(pop)
#endif