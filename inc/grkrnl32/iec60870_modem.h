#ifndef __IEC60870_MODEM_INC__
#define __IEC60870_MODEM_INC__

#include <modemmer.h>

 #ifndef RC_INVOKED
 #include <iec60870proto.h>

 #pragma pack(push,1)

 #define CCFL_PUNUMBER        0x800000
 #define CCFL_PUNAME          0x400000
 #define CCFL_LINES           0x200000


  #define REGDW_PUNUMBER L"pu_number"
  #define REGSTR_PUNAME  L"pu_name"
  #define REGBIN_LINES   L"lines"
  #define MAX_NAME_LEN   96

  #define IEC60870_MOD_STREAM_VER_0 0
  #define IEC60870_MOD_STREAM_VER_1 1
  #define IEC60870_MOD_STREAM_VER_2 2
  #define IEC60870_MOD_STREAM_VER   3


  #define MNF_LINE_TX             0x000100
  #define MNF_LINE_RX             0x000200
  #define MNF_LINE_RECORD_CHANGED 0x001000

  //Разрешение таймера
  #define WT_RESOLUTION 100



 struct IEC60870_MOD_CONFIG
 {
   DWORD   dw_size;
   DWORD   modem_number;
   DWORD   pu_number;
   wchar_t pu_name[MAX_NAME_LEN];
   #ifdef __cplusplus
   IEC60870_MOD_CONFIG():dw_size(sizeof(*this)),modem_number(-1),pu_number(-1) {bzero(pu_name,sizeof(pu_name));}
   #endif
 };

 typedef IEC60870_MOD_CONFIG * LPIEC60870_MOD_CONFIG;
 #define IEC60870_MCD_SIZE ( sizeof(MODULE_CONFIG_DATA)+ sizeof(IEC60870_MOD_CONFIG)-sizeof(BYTE))


 #define MAX_WT_TIMERS 8
 #define WT_TX_TIMER    0
 #define WT_RX_TIMER    1
 #define WT_TESTFR_TIMER 2




 struct IEC60870_LINE_CONFIG
 {
  DWORD   dw_size;
  DWORD   cp_number;
  wchar_t cp_name  [MAX_NAME_LEN];
  wchar_t host_addr[MAX_NAME_LEN];
  int     host_port;
  DWORD   obj_addr_size;
  DWORD   W_count;//Количество неподтвержденных принятых кадров
  DWORD   K_count;//Количество неподтвержденных прeданных кадров
  DWORD   WT[MAX_WT_TIMERS];
  DWORD   dev_addr;
  DWORD   common_addr;
  DWORD   reserv[4];
  #ifdef __cplusplus
  IEC60870_LINE_CONFIG():dw_size(sizeof(*this))
                        ,cp_number(-1),host_port(IEC60870_DEFAULT_TCP_PORT)
                        ,obj_addr_size(3)
                        ,W_count(30)
                        ,K_count(10)
  {bzero(cp_name,sizeof(cp_name));bzero(host_addr,sizeof(host_addr));bzero(WT,sizeof(WT));}
  #endif
 };

 typedef IEC60870_LINE_CONFIG* LPIEC60870_LINE_CONFIG;


 #define CMD_IEC60870_GET_LINE MCMD(MT_MODEM_IEC60870,1000)
 #define CMD_IEC60870_ADD_LINE MCMD(MT_MODEM_IEC60870,1001)
 #define CMD_IEC60870_UPD_LINE MCMD(MT_MODEM_IEC60870,1002)
 //p1 = LPIEC60870_LINE_CONFIG

 #define CMD_IEC60870_DEL_LINE MCMD(MT_MODEM_IEC60870,1003)
 //p1 - line number

 struct rc_param_t
 {
   iec60870_command_quality qu;
   DWORD        success_timeout;
  #ifdef __cplusplus
  rc_param_t():qu(qu_persistent),success_timeout(1000){}
  #endif
 };

 #define IEC60870_REC_FL_NUMBER      0x0001
 #define IEC60870_REC_FL_OTD_FA      0x0002
 #define IEC60870_REC_FL_OTD_GROUP   0x0004
 #define IEC60870_REC_FL_OTD_NUMBER  0x0008
 #define IEC60870_REC_FL_RC_COMMAND  0x0010
 #define IEC60870_REC_FL_RC_NUMBER   0x0020
 #define IEC60870_REC_FL_RC_PARAM    0x0040
 #define IEC60870_REC_FL_RC_STATE    0x0080
 #define IEC60870_REC_FL_IS_FLOAT    0x0100
 #define IEC60870_REC_FL_VALUE       0x0200
 #define IEC60870_REC_FL_QUALITY     0x0400
 #define IEC60870_REC_FL_TIMESTAMP   0x0800
 #define IEC60870_REC_FL_ASDU_TYPE   0x1000
 #define IEC60870_REC_FL_OPTIONS     0x2000


 #define IEC60870_REC_DYNOPT_MASK    0x00FF
 #define IEC60870_REC_DYNOPT_INVERSE 0x0001


 //Записывается в поле res iec60870_quality

 struct iec60870_record
 {
  DWORD          dw_size;
  WORD           changes_mask;
  DWORD          number;
  BYTE           otd_fa;
  BYTE           otd_group;
  DWORD          otd_number;
  BYTE           rc_command;
  DWORD          rc_number;
  iec60870_rctrl rc_ctrl;
  LONG           rc_timer;
  DWORD          rc_state;
  BYTE           is_float;
  union{
        DWORD      dw_value;
	float      fl_value;
       };
  iec60870_quality quality;
  unsigned __int64 timestamp;
  DWORD            asdu_type;
  DWORD            options;
  #ifdef __cplusplus
  static  bool __fastcall is_value_float (int  )      {return false;}
  static  bool __fastcall is_value_float (long )      {return false;}
  static  bool __fastcall is_value_float (DWORD)      {return false;}
  static  bool __fastcall is_value_float (short)      {return false;}
  static  bool __fastcall is_value_float (char)       {return false;}
  static  bool __fastcall is_value_float (BYTE)       {return false;}
  static  bool __fastcall is_value_float (float)      {return true ;}
  static  bool __fastcall is_value_float (double)     {return true ;}
  static  bool __fastcall is_value_float (long double){return true ;}
  void __fastcall construct_empty(WORD _ch_mask = -1)
  {
     dw_size  = sizeof(*this);
	 changes_mask    = _ch_mask;
	 number          = -1;
	 otd_fa          = otd_group = -1;
	 otd_number      = -1;
	 rc_command      = -1;
	 rc_number       = -1;
	 rc_state        =  0;
	 is_float        = false;
	 dw_value        = 0;
	 quality.quality_byte  = -1;
	 timestamp       = 0;
	 rc_timer        = 0;
	 rc_ctrl.control = 0;
         asdu_type       = 0;
         options         = 0;
  }

   iec60870_record(DWORD _otd_fa,DWORD _number)
   {
    construct_empty();
    otd_fa = _otd_fa;
    number = _number;
   }

   iec60870_record(DWORD _otd_fa,DWORD  _otd_group,DWORD  _otd_number)
   {
    construct_empty();
    otd_fa     = _otd_fa;
    otd_group  = _otd_group;
    otd_number = _otd_number;
   }

   iec60870_record()
   {
    construct_empty();
   }

   iec60870_record(const iec60870_record  & src) {dw_size =sizeof(*this);*this = src;}

   template<typename obj_t>
   iec60870_record (const obj_t * obj,const DWORD _number,DWORD _asdu_type)
   {

     construct_empty(0);
     asdu_type  = _asdu_type;
     number     = _number;
     this->otd_fa = obj->get_otd_fa();
     is_float = is_value_float(obj->get_value ());
     if(is_float)
        fl_value = obj->get_value();
        else
        dw_value = obj->get_value();
      quality.quality_byte    = obj->get_quality()->quality_byte;
      timestamp = get_timestamp(obj->get_time());
      changes_mask = IEC60870_REC_FL_VALUE|IEC60870_REC_FL_QUALITY|IEC60870_REC_FL_TIMESTAMP;
   }

   iec60870_record & operator = (const iec60870_record  & src)
   {
    number       = src.number;
    changes_mask = src.changes_mask;
    otd_fa       = src.otd_fa;
    otd_group    = src.otd_group;
    otd_number   = src.otd_number;
    rc_command   = src.rc_command;
    rc_number    = src.rc_number;
    rc_ctrl      = src.rc_ctrl;
    rc_timer = src.rc_timer;
    rc_state     = src.rc_state;
    is_float     = src.is_float;
    if(is_float)
       fl_value  = src.fl_value;
       else
       dw_value  = src.dw_value;
     quality     = src.quality;
     timestamp   = src.timestamp;
     asdu_type   = src.asdu_type;
     options     = src.options;
     return *this;
   }

   DWORD operator ^ (const  iec60870_record & r)
   {
     DWORD  ret = 0;
     if(number      != r.number    ) ret |= IEC60870_REC_FL_NUMBER    ;
     if(otd_fa      != r.otd_fa    ) ret |= IEC60870_REC_FL_OTD_FA    ;
     if(otd_group   != r.otd_group ) ret |= IEC60870_REC_FL_OTD_GROUP ;
     if(otd_number  != r.otd_number) ret |= IEC60870_REC_FL_OTD_NUMBER;
     if(rc_command  != r.rc_command) ret |= IEC60870_REC_FL_RC_COMMAND;
     if( rc_number  != r.rc_number ) ret |= IEC60870_REC_FL_RC_NUMBER ;
     if( rc_state   != r.rc_state  ) ret |= IEC60870_REC_FL_RC_STATE  ;

     if((is_float  && fl_value != r.fl_value) || (dw_value != r.dw_value  )) ret |= IEC60870_REC_FL_VALUE;
     if(quality.quality_byte |= r.quality.quality_byte ) ret |= IEC60870_REC_FL_QUALITY;
     if(timestamp != r.timestamp ) ret |= IEC60870_REC_FL_TIMESTAMP;
     if(asdu_type != r.asdu_type ) ret |= IEC60870_REC_FL_ASDU_TYPE;
     return ret;
   }
inline   __int64  __fastcall get_timestamp(lpiec60870_cp24time2a tm)
   {
     __int64 timestamp;
     SYSTEMTIME st;
     GetLocalTime(&st);
     if(tm)
      {
        st.wSecond       = tm->msec /1000;
        st.wMilliseconds = tm->msec %1000;
      }
      SystemTimeToFileTime(&st,(LPFILETIME)&timestamp);
      return timestamp;
   }

inline   __int64  __fastcall get_timestamp(lpiec60870_cp56time2a tm)
   {
     __int64 timestamp;
     SYSTEMTIME st;
     if(tm)
      {
        st.wYear         = tm->year+2000;
        st.wMonth        = tm->month;
        st.wDay          = tm->mday;
        st.wHour         = tm->hour;
        st.wMinute       = tm->min;
        st.wSecond       = tm->msec /1000;
        st.wMilliseconds = tm->msec %1000;
      }
      else
      GetLocalTime(&st);
      SystemTimeToFileTime(&st,(LPFILETIME)&timestamp);
    return timestamp;
   }

   DWORD  __fastcall get_otd_type()
   {
     if(otd_fa == OTD_FA_DISCRETE)
        return OTD_BYTE;
     return is_float ? OTD_FLOAT : OTD_ANALOG_LONG;
   }

   DWORD __fastcall get_otd_diag()
   {
     DWORD diag = rc_state;
     if(quality.quality_byte == (BYTE)-1)
         diag |= OTD_PDIAG_NODATA;
     else
     {
       if(quality.iv)
          diag |= OTD_PDIAG_ERROR;
       if(is_float && (quality.var&1))
          diag |= OTD_PDIAG_OUTSCALE;
     }
     return diag;
   }

   bool __fastcall is_otd_assigned(){return ((otd_number !=(DWORD)-1) && (otd_group != (BYTE)-1)) ?  true : false; }

  #endif
 };

 typedef iec60870_record* lpiec60870_record;

 #ifdef __cplusplus
  struct iec60870_record_comparer
  {
    bool __fastcall is_equal(const iec60870_record & r1 , const iec60870_record & r2 )
    {
     return (r1.otd_fa == r2.otd_fa && r1.number == r2.number) ? true : false;
    }

    int  __fastcall compare (const iec60870_record & r1 , const iec60870_record & r2 )
    {
        if(r1.otd_fa == r2.otd_fa)
           {
            if(r1.number != r2.number)
               return (r1.number<r2.number) ? -1 : 1;
             return 0;
           }
        return (r1.otd_fa < r2.otd_fa) ? -1:1;
    }

    int __fastcall compare_otd_number(const iec60870_record & r1 , const iec60870_record & r2 )
    {
        if(r1.otd_fa == r2.otd_fa)
           {
            if(r1.otd_group == r2.otd_group)
               {
                if(r1.otd_number == r2.otd_number)
                   return 0;
                if(r1.otd_number < r2.otd_number)
                return -1;
                 else
                return 1;
               }
             if(r1.otd_group < r2.otd_group)
             return -1;
             else
             return 1;
           }
        if(r1.otd_fa < r2.otd_fa)
        return -1;
        return  1;

    }
  };

  struct iec60870_record_less
  {
    bool operator () (const iec60870_record & r1 , const iec60870_record & r2 )
    {
      return (compare(r1,r2)<0) ? true : false;
    }
    int compare(const iec60870_record & r1 , const iec60870_record & r2 )
    {
     int    ret = iec60870_record_comparer().compare(r1,r2);
     return ret;
    }
  };

  struct iec60870_record_otd_less
  {
    bool operator () (const iec60870_record & r1, const iec60870_record & r2 )
    {
      return (compare(r1,r2)<0) ? true : false;
    }

    int compare(const iec60870_record & r1 , const iec60870_record & r2 )
    {
     return iec60870_record_comparer().compare_otd_number(r1,r2);
    }
  };


 #endif


#define CMD_IEC60870_LINE_GET_ASDU_RECORDS_COUNT      MCMD(MT_MODEM_IEC60870,2000)

#define CMD_IEC60870_LINE_ENUM_ASDU_RECORDS      MCMD(MT_MODEM_IEC60870,2001)
// p1 = line_num p2 = lpiec60870_record ptr; ptr->type,ptr->number as index
// ret = SUCCESS/ERROR

#define CMD_IEC60870_LINE_GET_ASDU_RECORD        MCMD(MT_MODEM_IEC60870,2002)
// p1 = line_num p2 = lpiec60870_record ptr; ptr->type,ptr->number
// ret = SUCCESS/ERROR

#define CMD_IEC60870_LINE_ASDU_RECORD_BIND_OTD       MCMD(MT_MODEM_IEC60870,2003)
//Привязка группы и номера отд
// otd_group == -1 && otd_number == -1 --> отвязка группы
// p1 = line_num p2 = lpiec60870_record ptr
//ret SUCCES/ERROR


struct IEC60870_LINE_GROUP_PARAM
{
  DWORD dw_size;
  DWORD otd_fa;
  DWORD otd_group;
  #ifdef __cplusplus
  IEC60870_LINE_GROUP_PARAM(DWORD _otd_fa ,DWORD _group_num)
    :dw_size ( sizeof(*this))
    ,otd_fa  (_otd_fa)
    ,otd_group(_group_num)
  {}
  #endif
};

typedef IEC60870_LINE_GROUP_PARAM* LPIEC60870_LINE_GROUP_PARAM;

#define CMD_IEC60870_LINE_GROUP_CREATE                 MCMD(MT_MODEM_IEC60870,2004)
//Создать группу (если в кол-во участников группы == 0 группа удаляется)
//p1 = line_number p2 = LPIEC60870_LINE_GROUP_PARAM

#define CMD_IEC60870_LINE_GROUP_COUNT                  MCMD(MT_MODEM_IEC60870,2005)
//получение количества элементов в группе
//p1 = line_number p2 = LPIEC60870_LINE_GROUP_PARAM

#define CMD_IEC60870_LINE_GROUP_MAX_NUMBER             MCMD(MT_MODEM_IEC60870,2006)
//получение максимального номера группы
//p1 = line_number p2 = LPIEC60870_LINE_GROUP_PARAM


#define CMD_IEC60870_LINE_ASDU_RECORD_BIND_OTD_RC_CMD    MCMD(MT_MODEM_IEC60870,2007)
// p1 = line_num p2 = lpiec60870_record ptr
// Привязка команды ту/тр работате только в случае если привязана группа и номер отд
// ret SUCCES/ERROR

 #pragma pack(pop)
 #endif //end RC_INVOKED

#endif
