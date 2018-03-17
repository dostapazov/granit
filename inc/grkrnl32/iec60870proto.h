#ifndef __IEC60870_PROTO_INC__
#define __IEC60870_PROTO_INC__
#include <ke_defs.h>
#include <otd_types.h>

 #define IEC60870_MAX_FRAME_LENGHT    253
 #define IEC60870_DEFAULT_TCP_PORT    2404
 #define IEC60870_PROTO_START_MARKER  0x68

 #pragma pack(push,1)

 enum acpi_type_t {acpi_type_I = 0,acpi_type_S = 1,acpi_type_U=3};

 struct   iec60870_seq_num_t
 {
    WORD   value;
 #ifdef __cplusplus
    WORD   get_number()           {return (value>>1) &0x7FFF; }
    void   set_number(WORD number){value = ((number<<1)&0xFFFE);}
    iec60870_seq_num_t &   operator ++()  {value+=2; return *this;}
    iec60870_seq_num_t &   operator +=(DWORD _v) {value+= (_v<<1); return *this;}
    WORD   operator ()() {return get_number();}
    void   reset () {value = 0;}
 #endif
 };

 struct  iec60870_acpiI_t
 {
   iec60870_seq_num_t tx_number;
   iec60870_seq_num_t rx_number;
 };

 struct  iec60870_acpiS_t
 {
    WORD format :2;
    WORD breserv:14;
    iec60870_seq_num_t rx_number;
 };

 enum act_type_t {at_no_act = 0,at_act = 1,at_con = 2};

 struct  iec60870_acpiU_t
 {
   BYTE format :2;
   BYTE startdt:2;
   BYTE stopdt :2;
   BYTE testfr :2;
   BYTE reserv[3];
 };


 struct  iec60870_proto_header_t
 {
  BYTE start_marker;
  BYTE lenght;
  union{
          DWORD dw_ctrl;
          iec60870_acpiI_t acpiI;
          iec60870_acpiS_t acpiS;
          iec60870_acpiU_t acpiU;
        };

   #ifdef __cplusplus
    iec60870_proto_header_t():start_marker(IEC60870_PROTO_START_MARKER),lenght(sizeof(dw_ctrl)){dw_ctrl = 0;}
    int __fastcall get_frame_lenght()  {return lenght+sizeof(start_marker)+sizeof(lenght);}
    acpi_type_t __fastcall get_format();
   #endif
 };

 typedef iec60870_proto_header_t* lpiec60870_proto_header_t;

 struct iec60870_cp24time2a
 {
   WORD   msec;
   BYTE   min:5;
   BYTE   res:1;
   BYTE   iv :1;
 };
 typedef iec60870_cp24time2a* lpiec60870_cp24time2a;

 struct iec60870_cp56time2a {
    WORD msec  :16;
    WORD min   :6;
    WORD res1  :1;
    WORD iv    :1;
    WORD hour  :5;
    WORD res2  :2;
    WORD su    :1;
    WORD mday  :5;
    WORD wday  :3;
    WORD month :4;
    WORD res3  :4;
    WORD year  :7;
    WORD res4  :1;
};

typedef iec60870_cp56time2a* lpiec60870_cp56time2a;

enum asdu_type_t
{
  M_SP_NA_1 = 1 ,//информация одноэлементная
  M_SP_TA_1 = 2 ,//информация одноэлементная     c меткой времени
  M_DP_NA_1 = 3 ,//информация двухэлементная
  M_DP_TA_1 = 4 ,//информация двухэлементная     c меткой времени
  M_ST_NA_1 = 5 ,//информация положение отпаек
  M_ST_TA_1 = 6 ,//информация положение отпаек  c меткой времени
  M_BO_NA_1 = 7 ,//Строка 32 бит
  M_BO_TA_1 = 8 ,//Строка 32 бит   с меткой времени
  M_ME_NA_1 = 9 ,//Значение измеряемой величины нормализованное
  M_ME_TA_1 = 10,//Значение измеряемой величины нормализованное    c меткой времени
  M_ME_NB_1 = 11,//Значение измеряемой величины масштабированное
  M_ME_TB_1 = 12,//Значение измеряемой величины масштабированное   c меткой времени
  M_ME_NC_1 = 13,//Значение измеряемой величины short float
  M_ME_TC_1 = 14,//Значение измеряемой величины short float   c меткой времени
  M_IT_NA_1 = 15,//Интегральные суммы
  M_IT_TA_1 = 16,//Интегральные суммы c меткой времени
  M_EP_TA_1 = 17,//Работа устройств защиты с меткой времени
  M_EP_TB_1 = 18,//Упакованная информация о срабатывании пусковых органов устройств защиты с меткой времени
  M_EP_TC_1 = 19,//Упакованная информация о срабатывании в выходных цепях устройства защиты с меткой времени
  M_PS_NA_1 = 20,//Упакованная одноэлементная информация с определением изменения состояния
  M_ME_ND_1 = 21,//Значение измеряемой величины, нормализованное значение без описателя качества

  ASDU_TYPE_22,
  ASDU_TYPE_23,
  ASDU_TYPE_24,
  ASDU_TYPE_25,
  ASDU_TYPE_26,
  ASDU_TYPE_27,
  ASDU_TYPE_28,
  ASDU_TYPE_29,

  M_SP_TB_1 =30 ,//Одноэлементная информация с меткой времени СР56Время2а
  M_DP_TB_1 =31 ,//Двухэлементная информация с меткой времени СР56Время2а
  M_ST_TB_1 =32 ,//Информация о положении отпаек с меткой времени СР56Время2а
  M_BO_TB_1 =33 ,//Строка из 32 битов с меткой времени СР56Время2а
  M_МЕ_ТD_1 =34 ,//Значение измеряемой величины, нормализованное значение с меткой времени СР56Время2а
  M_МЕ_ТЕ_1 =35 ,//Значение измеряемой величины масштабированное значение с меткой времени СР56Время2а
  M_ME_TF_1 =36 ,//Значение измеряемой величины, короткий формат с плавающей запятой с меткой времени СР56Время2а
  M_IT_TB_1 =37 ,//Интегральные суммы с меткой времени СР56Время2а
  M_EP_TD_1 =38 ,//Работа устройств защиты с меткой времени СР56Время2а
  M_EP_TE_1 =39 ,//Упакованное сообщение о срабатывании пусковых органов устройств защиты с меткой времени СР56Время2а
  M_EP_TF_1 =40 ,//Упакованная информация о срабатывании в выходных цепях устройства защиты с меткой времени СР56Время2а

  ASDU_TYPE_41,
  ASDU_TYPE_42,
  ASDU_TYPE_43,
  ASDU_TYPE_44,

  C_SC_NA_1 =45 ,// Однопозиционная команда
  C_DC_NA_1 =46 ,// Двухпозиционная команда
  C_RC_NA_1 =47 ,// Команда пошагового регулирования
  C_SE_NA_1 =48 ,// Команда уставки, нормализованное значение
  C_SE_NB_1 =49 ,// Команда уставки, масштабированное значение
  C_SE_NC_1 =50 ,// Команда уставки, короткий формат с плавающей запятой
  C_BO_NA_1 =51 ,// Строка из 32 битов

  ASDU_TYPE_52  ,
  ASDU_TYPE_53  ,
  ASDU_TYPE_54  ,
  ASDU_TYPE_55  ,
  ASDU_TYPE_56  ,
  ASDU_TYPE_57  ,

  C_SC_TA_1, //(* (058, 0x3A) Single command with time tag CP56Time2a *)
  C_DC_TA_1, //(* (059, 0x3B) Double command with time tag CP56Time2a *)
  C_RC_TA_1, //(* (060, 0x3C) Regulating step command with time tag CP56Time2a *)
  C_SE_TA_1, //(* (061, 0x3D) Measured value, normalised value command with time tag CP56Time2a *)
  C_SE_TB_1, //(* (062, 0x3E) Measured value, scaled value command with time tag CP56Time2a *)
  C_SE_TC_1, //(* (063, 0x3F) Measured value, short floating point number command with time tag CP56Time2a *)
  C_BO_TA_1, //(* (064, 0x40) Bitstring of 32 bit command with time tag CP56Time2a *)

  ASDU_TYPE_65,
  ASDU_TYPE_66,
  ASDU_TYPE_67,
  ASDU_TYPE_68,
  ASDU_TYPE_69,

  M_EI_NA_1 =70 ,//Окончание инициализации

  C_IC_NA_1 =100,// Команда опроса
  C_CI_NA_1 =101,// Команда опроса счетчиков
  C_RD_NA_1 =102,// Команда чтения
  C_CS_NA_1 =103,// Команда синхронизации часов
  C_TS_NA_1 =104,// Тестовая команда
  C_RP_NA_1 =105,// Команда установки процесса в исходное состояние Reset
  C_CD_NA_1 =106,// Команда задержки опроса
  C_TS_TA_1 =107,// Тестовая команда с меткой времени СР56Время2а

  ASDU_TYPE_108,
  ASDU_TYPE_109,

  P_ME_NA_1 =110,// Параметры измеряемых величин,нормализованное значение
  P_ME_NB_1 =111,// Параметры измеряемых величин,масштабированное значение
  P_ME_NC_1 =112,// Параметры измеряемых величин, короткий формат с плавающей запятой
  P_AC_NA_1 =113,// Активация параметра

  ASDU_TYPE_114,
  ASDU_TYPE_115,
  ASDU_TYPE_116,
  ASDU_TYPE_117,
  ASDU_TYPE_118,
  ASDU_TYPE_119,
  F_FR_NA_1    , //(* (120, 0x78) File ready *)
  F_SR_NA_1    , //(* (121, 0x79) Section ready *)
  F_SC_NA_1    , //(* (122, 0x7A) Call directory, select file, call file, call section *)
  F_LS_NA_1    , //(* (123, 0x7B) Last section, last segment *)
  F_FA_NA_1    , //(* (124, 0x7C) ACK file, ACK section *)
  F_SG_NA_1    , //(* (125, 0x7D) Segment *)
  F_DR_TA_1    , //(* (126, 0x7E) Directory *)
  ASDU_TYPE_127,
  ASDU_TYPE_RESERV_FIRST = 128,
  ASDU_TYPE_RESERV_LAST  = 255,
  ASDU_TYPE_MAX
};

/*Причина передачи*/
enum transmit_cause_t
{
  tc_default          = 0
 ,tc_periodic         = 1
 ,tc_background_scan  = 2
 ,tc_sporadic         = 3
 ,tc_init_msg         = 4
 ,tc_interrogation    = 5
 ,tc_activation       = 6
 ,tc_activation_cfrm  = 7
 ,tc_deactivation     = 8
 ,tc_deactivation_cfrm        = 9
 ,tc_deactivation_end         = 10
 ,tc_back_info_remote_command = 11
 ,tc_back_info_local_command  = 12
 ,tc_file_transmit            = 13
 ,tc_reserv_beg = 14
 ,tc_reserv_end = 19
 ,tc_interrogation_respond = 20
 ,tc_repond_group1         = 21
 ,tc_repond_group2         = 22
 ,tc_repond_group3         = 23
 ,tc_repond_group4 ,tc_repond_group5 ,tc_repond_group6 ,tc_repond_group7 ,tc_repond_group8,tc_repond_group9
 ,tc_repond_group10,tc_repond_group11,tc_repond_group12,tc_repond_group13,tc_repond_group14,tc_repond_group15,tc_repond_group16
 ,tc_interrogation_respond_counters
 ,tc_repond_counters1 ,tc_repond_counters2 ,tc_repond_counters3 ,tc_repond_counters4
 ,tc_reserv1_beg = 42,tc_reserv1_end = 43
 ,tc_unknown_type_id = 44
 ,tc_unknown_transmission_cause = 45
 ,tc_unknown_asdu_common_addr   = 46
 ,tc_unknown_object_addr        = 47
 ,tc_max_number        = 48
};


/*указательопроса*/
enum qoi_t
{
 qoi_reserv_beg = 0,qoi_reserv_end = 0x13,
 qoi_common = 0x14,
 qoi_group1 = 0x15,qoi_group2,qoi_group3,qoi_group4,qoi_group5,qoi_group6,qoi_group7,qoi_group8,qoi_group9,qoi_group10,qoi_group11,qoi_group12,qoi_group13,qoi_group14,qoi_group15,qoi_group16,
 qoi_reserv_std_beg,qoi_reserv_std_end = 63,
 qoi_reserv_private_beg,qoi_reserv_private_end = 255
};

struct iec60870_qoi
{
  BYTE     obj_addr[3];
  BYTE     qoi;
  BYTE     qoi_data[ANYSIZE_ARRAY];
};

struct iec60870_asdu_header
{
    BYTE asdu_type;
    union
    {
    BYTE bt0;
        struct{
        BYTE obj_count:7; // Кол-во объектов
        BYTE seq      :1; // Последовательная нумерация
        };
    };
    union
    {
    BYTE bt1;
        struct{
        BYTE cause    :6; // Причина передачи
        BYTE pn       :1; // Positive / Negative
        BYTE test     :1; // Тест
        };
    };
    BYTE origin_addr;
    WORD common_addr;
    iec60870_asdu_header()
    {
      asdu_type = bt0 = bt1 = 0;
      origin_addr =   0;
      common_addr =  -1;
    }
};

typedef iec60870_asdu_header* lpiec60870_asdu_header;



union iec60870_quality
{
    BYTE quality_byte;
    struct{
            unsigned char var :2;
            unsigned char res :2;
            unsigned char bl  :1; // blocked/not blocked
            unsigned char sb  :1; // substituted/not substituted
            unsigned char nt  :1; // not topical/topical
            unsigned char iv  :1; // valid/invalid
    };
};

typedef iec60870_quality* lpiec60870_quality;

union iec60870_quality_mes
{
    BYTE quality_byte;
 struct {
         unsigned char overflow :1;  //Overflow
         unsigned char res      :3;
         unsigned char bl       :1; // blocked/not blocked
         unsigned char sb       :1; // substituted/not substituted
         unsigned char nt       :1; // not topical/topical
         unsigned char iv       :1; // valid/invalid
       };
};

typedef iec60870_quality_mes* lpiec60870_quality_mes;

//M_SP_NA_1 = 1
struct iec60870_type_M_SP_NA_1
{
  iec60870_quality  quality;
 #ifdef __cplusplus
  DWORD                 __fastcall get_value  () const {return (DWORD)(quality.var&1);}
  const lpiec60870_quality    __fastcall get_quality() const {return (const lpiec60870_quality)&quality;}
  lpiec60870_cp24time2a __fastcall get_time   () const {return NULL;}
  DWORD                 __fastcall get_otd_fa () const {return OTD_FA_DISCRETE;}
 #endif
};

//M_SP_TA_1 = 2
struct iec60870_type_M_SP_TA_1
{
  iec60870_quality    quality;
  iec60870_cp24time2a time;
 #ifdef __cplusplus
  DWORD                 __fastcall get_value  () const {return (DWORD)(quality.var&1);}
  const lpiec60870_quality    __fastcall get_quality() const {return (const lpiec60870_quality)&quality;}
  const lpiec60870_cp24time2a __fastcall get_time   () const {return (const lpiec60870_cp24time2a)&time;}
  DWORD                 __fastcall get_otd_fa () const {return OTD_FA_DISCRETE;}
 #endif
};

enum iec60870_sp_state
{
  sp_off = 0,
  sp_on  = 1
};

enum iec60870_dp_state
{
  dp_indeterminate0 = 0,
  dp_off            = 1,
  dp_on             = 2,
  dp_indeterminate3 = 3
};

enum  iec60870_command_quality
{
 qu_unspecefied        = 0,
 qu_short_pulse        = 1,
 qu_long_pulse         = 2,
 qu_persistent         = 3,
 qu_min_std_reserv     = 4,
 qu_max_std_reserv     = 8,
 qu_min_other_reserv   = 9,
 qu_max_other_reserv   = 15,
 qu_min_private_reserv = 16,
 qu_max_private_reserv = 31,
};

enum iec60870_rc_step
{
  rcs_notallowed0    = 0,
  rcs_decrement      = 1,
  rcs_increment      = 2,
  rcs_notallowed3    = 3
};


enum iec60870_command_state { cs_execute = 0, cs_select = 1};

union iec60870_rctrl
{
   BYTE   control;
   struct
   {
     BYTE var  :2;
     BYTE qu   :5;
     BYTE se   :1;
   };
};

typedef iec60870_rctrl* lpiec60870_rctrl;


inline DWORD __fastcall get_dp_value( const  iec60870_quality  & quality)
{
  DWORD ret = (DWORD)(quality.var&2);
  if(ret == dp_off) ret = 0;
  else
  {
    if(ret == dp_on) ret = 1;
    else
    const_cast<iec60870_quality &>(quality).iv = 1;//Установить недостоверность
  }
  return ret ? 1:0;
}

//M_DP_NA_1 = 3
struct iec60870_type_M_DP_NA_1
{
  iec60870_quality quality;
  #ifdef __cplusplus
  DWORD                       __fastcall get_value  () const {return get_dp_value(quality);}
  const lpiec60870_quality    __fastcall get_quality() const {return (const lpiec60870_quality) &quality;}
  const lpiec60870_cp24time2a __fastcall get_time   () const {return NULL;}
  DWORD                       __fastcall get_otd_fa () const {return OTD_FA_DISCRETE;}
 #endif
};

//M_DP_TA_1 = 4
struct iec60870_type_M_DP_TA_1
{
  iec60870_quality    quality;
  iec60870_cp24time2a time;
  #ifdef __cplusplus
  DWORD                       __fastcall get_value  () const {return get_dp_value(quality);}
  const lpiec60870_quality    __fastcall get_quality() const {return (const lpiec60870_quality )&quality;}
  const lpiec60870_cp24time2a __fastcall get_time   () const {return (const lpiec60870_cp24time2a)&time;}
  DWORD                       __fastcall get_otd_fa () const {return OTD_FA_DISCRETE;}
 #endif
};

union iec60870_vti
{
  struct{
         BYTE   state : 7;
         BYTE   transient : 1;
        };
  BYTE value;
};


//M_ST_NA1 = 5
struct iec60870_type_M_ST_NA_1
{

  iec60870_vti        value;
  iec60870_quality    quality;
  #ifdef __cplusplus
  DWORD                       __fastcall get_value  () const {return value.value;}
  const lpiec60870_quality    __fastcall get_quality() const {return (const lpiec60870_quality )&quality;}
  const lpiec60870_cp24time2a __fastcall get_time   () const {return (const lpiec60870_cp24time2a)NULL;}
  DWORD                       __fastcall get_otd_fa () const {return OTD_FA_DISCRETE;}
 #endif
};


//M_ST_TA1 = 6
struct iec60870_type_M_ST_TA_1
{

  iec60870_vti        value;
  iec60870_quality    quality;
  iec60870_cp24time2a time;
  #ifdef __cplusplus
  DWORD                       __fastcall get_value  () const {return value.value;}
  const lpiec60870_quality    __fastcall get_quality() const {return (const lpiec60870_quality )&quality;}
  const lpiec60870_cp24time2a __fastcall get_time   () const {return (const lpiec60870_cp24time2a)&time;}
  DWORD                       __fastcall get_otd_fa () const {return OTD_FA_DISCRETE;}
 #endif
};


//M_ME_NA_1 = 9
struct iec60870_type_M_ME_NA_1
{
   short               mv;
   iec60870_quality    quality;
  #ifdef __cplusplus
  short                       __fastcall get_value  () const {return mv;}
  const lpiec60870_quality    __fastcall get_quality() const {return (const lpiec60870_quality )&quality;}
  const lpiec60870_cp24time2a __fastcall get_time   () const {return (const lpiec60870_cp24time2a)NULL;}
  DWORD                       __fastcall get_otd_fa () const {return OTD_FA_ANALOG;}
 #endif
};

//M_ME_TA_1 = 10
struct iec60870_type_M_ME_TA_1
{
   short               mv;
   iec60870_quality    quality;
   iec60870_cp24time2a time;
  #ifdef __cplusplus
  short                       __fastcall get_value  () const {return mv;}
  const lpiec60870_quality    __fastcall get_quality() const {return (const lpiec60870_quality )&quality;}
  const lpiec60870_cp24time2a __fastcall get_time   () const {return (const lpiec60870_cp24time2a)&time;}
  DWORD                       __fastcall get_otd_fa () const {return OTD_FA_ANALOG;}
 #endif
};


//M_ME_NB_1 = 11
struct iec60870_type_M_ME_NB_1
{
	short mv;
    iec60870_quality    quality;
  #ifdef __cplusplus
  short                       __fastcall get_value  () const {return mv;}
  const lpiec60870_quality    __fastcall get_quality() const {return (const lpiec60870_quality )&quality;}
  const lpiec60870_cp24time2a __fastcall get_time   () const {return (const lpiec60870_cp24time2a)NULL;}
  DWORD                       __fastcall get_otd_fa () const {return OTD_FA_ANALOG;}
 #endif
};

//M_ME_TB_1 = 12
struct iec60870_type_M_ME_TB_1
{
  short mv;
  iec60870_quality_mes    quality;
  iec60870_cp24time2a     time;
  #ifdef __cplusplus
  short                        __fastcall get_value  () const {return mv;}
  const lpiec60870_quality_mes __fastcall get_quality() const {return (const lpiec60870_quality_mes )&quality;}
  const lpiec60870_cp24time2a  __fastcall get_time   () const {return (const lpiec60870_cp24time2a)&time;}
  DWORD                        __fastcall get_otd_fa () const {return OTD_FA_ANALOG;}
 #endif
};


//M_ME_NC = 13
struct iec60870_type_M_ME_NC_1
{
	float mv;
	iec60870_quality_mes  quality;
 #ifdef __cplusplus
  float                         __fastcall get_value  () const {return mv;}
  const lpiec60870_quality_mes  __fastcall get_quality() const {return (const lpiec60870_quality_mes)&quality;}
  const lpiec60870_cp24time2a   __fastcall get_time   () const {return (const lpiec60870_cp24time2a)NULL;}
  DWORD                         __fastcall get_otd_fa () const {return OTD_FA_ANALOG;}
 #endif

};


typedef iec60870_type_M_ME_NC_1* lpiec60870_type_M_ME_NC_1;
//M_ME_TC_1 = 14
struct iec60870_type_M_ME_TC_1 {
    float                 mv;
    iec60870_quality_mes  quality;
    iec60870_cp24time2a   time;
 #ifdef __cplusplus
  float                   __fastcall get_value  () const {return mv;}
  const lpiec60870_quality_mes  __fastcall get_quality() const {return (const lpiec60870_quality_mes )&quality;}
  const lpiec60870_cp24time2a   __fastcall get_time   () const {return (const lpiec60870_cp24time2a  )&time;}
  DWORD                   __fastcall get_otd_fa () const {return OTD_FA_ANALOG;}
 #endif

};

typedef iec60870_type_M_ME_TC_1* lpiec60870_type_M_ME_TC_1;

// M_SP_TB_1 = 30 одноэлементная информация  с меткой времени cp56time2a
struct iec60870_type_M_SP_TB_1 {
    iec60870_quality    quality;
    iec60870_cp56time2a time;
 #ifdef __cplusplus
  DWORD                 __fastcall get_value  () const {return (DWORD)(quality.var&1);}
  const lpiec60870_quality    __fastcall get_quality() const {return (const lpiec60870_quality )&quality;}
  const lpiec60870_cp56time2a __fastcall get_time   () const {return (const lpiec60870_cp56time2a  )&time;}
  DWORD                 __fastcall get_otd_fa () const {return OTD_FA_DISCRETE;}
 #endif
};

typedef iec60870_type_M_SP_TB_1* lpiec60870_type_M_SP_TB_1;

// M_DP_TB_1 - 31 double point information with quality description and time tag
struct iec60870_type_M_DP_TB_1
{
    iec60870_quality    quality;
    iec60870_cp56time2a time;
 #ifdef __cplusplus
  DWORD                 __fastcall get_value        () const {return get_dp_value(quality);}
  const lpiec60870_quality    __fastcall get_quality() const {return (const lpiec60870_quality )&quality;}
  const lpiec60870_cp56time2a __fastcall get_time   () const {return (const lpiec60870_cp56time2a  )&time;}
  DWORD                 __fastcall get_otd_fa       () const {return OTD_FA_DISCRETE;}
 #endif
};

// M_МЕ_ТD_1  34 Значение измеряемой величины, нормализованное значение с меткой времени СР56Время2а
struct iec60870_type_M_ME_TD_1 {
    short                 mv;
    iec60870_quality_mes  quality;
    iec60870_cp56time2a   time;
 #ifdef __cplusplus
  short                        __fastcall get_value  () const { return mv;      }
  const lpiec60870_quality_mes __fastcall get_quality() const { return (const lpiec60870_quality_mes)&quality;}
  const lpiec60870_cp56time2a  __fastcall get_time   () const { return (const lpiec60870_cp56time2a )&time;}
  DWORD                        __fastcall get_otd_fa () const { return OTD_FA_ANALOG;}
 #endif
};
typedef iec60870_type_M_ME_TD_1* lpiec60870_type_M_ME_TD_1;


// M_МЕ_ТE_1  35 Значение измеряемой величины, масштабированное значение с меткой времени СР56Время2а
struct iec60870_type_M_ME_TE_1 {
    short                 mv;
    iec60870_quality_mes  quality;
    iec60870_cp56time2a   time;
 #ifdef __cplusplus
  short                        __fastcall get_value  () const { return mv;      }
  const lpiec60870_quality_mes __fastcall get_quality() const { return (const lpiec60870_quality_mes)&quality;}
  const lpiec60870_cp56time2a  __fastcall get_time   () const { return (const lpiec60870_cp56time2a )&time;}
  DWORD                        __fastcall get_otd_fa () const { return OTD_FA_ANALOG;}
 #endif
};
typedef iec60870_type_M_ME_TE_1* lpiec60870_type_M_ME_TE_1;

// M_ME_TF_1 = 36 short floating point measurement value and time tag
struct iec60870_type_M_ME_TF_1 {
    float                 mv;
    iec60870_quality_mes  quality;
    iec60870_cp56time2a   time;
 #ifdef __cplusplus
  float                        __fastcall get_value  () const { return mv;      }
  const lpiec60870_quality_mes __fastcall get_quality() const { return (const lpiec60870_quality_mes)&quality;}
  const lpiec60870_cp56time2a  __fastcall get_time   () const { return (const lpiec60870_cp56time2a )&time;}
  DWORD                        __fastcall get_otd_fa () const { return OTD_FA_ANALOG;}
 #endif
};
typedef iec60870_type_M_ME_TF_1* lpiec60870_type_M_ME_TF_1;



 #ifdef __cplusplus


 inline bool __fastcall iec60870_proto_is_valid        (lpiec60870_proto_header_t hdr)
 {
  return (hdr && hdr->start_marker == IEC60870_PROTO_START_MARKER) ? true : false;
 }
 inline unsigned int  __fastcall iec60870_proto_get_frame_length(lpiec60870_proto_header_t hdr)
 {
   return iec60870_proto_is_valid(hdr) ? (hdr->lenght + sizeof(hdr->start_marker)+sizeof(hdr->lenght)) : 0;
 }

 acpi_type_t __fastcall iec60870_proto_header_t::get_format()
 {
   return  (!(dw_ctrl&acpi_type_S)) ?  acpi_type_I  : (acpi_type_t)(dw_ctrl&acpi_type_U);
 }
 #else

 #define iec60870_proto_is_valid(hdr) ((hdr && hdr->start_marker == IEC60870_PROTO_START_MARKER) ? true : false)
 // etc ...
 #endif
 #pragma pack(pop)

 #ifdef __cplusplus

 extern "C" {
 #endif
 lpiec60870_proto_header_t WINAPI iec60870_proto_init_frame      (LPBYTE ptr,DWORD sz,LPBYTE data,DWORD len);
 int  WINAPI                      iec60870_proto_frame_add_data  (lpiec60870_proto_header_t phdr,LPBYTE data,DWORD len);
 lpiec60870_proto_header_t WINAPI iec60870_proto_startdt         (LPBYTE ptr,DWORD sz,bool confirm);
 lpiec60870_proto_header_t WINAPI iec60870_proto_stopdt          (LPBYTE ptr,DWORD sz,bool confirm);
 lpiec60870_proto_header_t WINAPI iec60870_proto_testfr          (LPBYTE ptr,DWORD sz,bool confirm);
 lpiec60870_asdu_header    WINAPI iec60870_proto_get_asdu_header (lpiec60870_proto_header_t phdr);
 DWORD WINAPI                     iec60870_proto_get_obj_addr    (LPBYTE ptr,DWORD addr_sz);
 void  WINAPI                     iec60870_proto_set_obj_addr    (DWORD addr,DWORD addr_sz,LPBYTE ptr_to);
 lpiec60870_proto_header_t WINAPI iec60870_proto_receipt         (LPBYTE buf,DWORD sz,WORD kadr_number);
 lpiec60870_proto_header_t WINAPI iec60870_proto_interrogate     (LPBYTE buf,DWORD sz,DWORD req_addr,DWORD common_addr,DWORD addr_size);
 lpiec60870_proto_header_t WINAPI iec60870_proto_control         (LPBYTE buf,DWORD sz,DWORD orig_addr,DWORD common_addr,DWORD command,DWORD ctrl_number,DWORD addr_sz,iec60870_rctrl rctrl);

 #ifdef __cplusplus
 }
 #endif
#endif
