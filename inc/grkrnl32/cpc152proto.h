#ifndef __CPC152_PROTO_INC__
#define __CPC152_PROTO_INC__
#ifdef __linux__
#include <lin_ke_defs.h>
#else
#include <ke_defs.h>
#endif


#define CPC152_MAX_SLOTS   8    // Количество слотов в контроллере
#define CPC152_MAX_GROUPS  16   //Количество  групп в плате

#define CPC152_MAX_GROUP_DISCRETE 4
#define CPC152_MAX_PARAM_DISCRETE 24

#define CPC152_MAX_GROUP_ANALOG   16
#define CPC152_MAX_PARAM_ANALOG   32

#pragma pack (push,1)

typedef struct  _cpc152proto_hdr
{
 WORD fa;
 WORD count;
 BYTE data[ANYSIZE_ARRAY];
}cpc152proto_hdr,*lpcpc152proto_hdr;

typedef struct _cpc152alarm_file_content
{
  QWORD alarm_id;
  BYTE  data[ANYSIZE_ARRAY];
}cpc152alarm_file_content,*lpcpc152alarm_file_content;

#define CPC152PROTO_FA_FLAG_RESPOND      0x8000
#define CPC152PROTO_FA_FLAG_ERASE        0x4000
#define CPC152PROTO_FA_FLAG_END          0x2000

#define CPC152PROTO_FA_MASK              0x0FFF

#define CPC152PROTO_FA_SCAN_DATA 0


#define CPC152SCAN_DATA_DISCRETE    0x80
#define CPC152SCAN_DATA_NOTWORKED   0x40
#define CPC152SCAN_DATA_DEVNUM_MASK 0x3F

typedef  struct _cpc152scan_data
{
  WORD   seq_number;
  BYTE   dev_num;
  BYTE   group_num;
  QWORD  tm_beg;
  QWORD  tm_end;
  WORD   dlen;
  BYTE   data[ANYSIZE_ARRAY];
  #ifdef __cplusplus
  bool operator <  (const _cpc152scan_data & sd);
  bool operator == (const _cpc152scan_data & sd);
  _cpc152scan_data & operator =  (const _cpc152scan_data & sd);
  #endif
} cpc152scan_data, *lpcpc152scan_data;


#define CPC152PROTO_FA_ADD_CONDITION     0x0001

struct talarm_condition
{
    WORD             dev_num;
    WORD             grp_num;
    WORD             param_num;
    WORD             more;  //Больше
    int              alarm_value;
#ifdef __cplusplus
    talarm_condition() {
        more = false;
        alarm_value = 0;
    }
    talarm_condition(int _dev_num,int _grp_num,int _param_num,bool _more,int _value)
        :dev_num(_dev_num),grp_num(_grp_num),param_num(_param_num),more(_more),alarm_value(_value) {}
    talarm_condition(const talarm_condition& src) {
        *this = src;
    }
    talarm_condition & operator = (const talarm_condition& src);
    int  comparer(const talarm_condition& ac) const;
#endif

};

typedef talarm_condition* lptalarm_condition;

//time synchro command
#define CPC152PROTO_FA_SYNCTIME          0x0002

//Notify new alarm archive
#define CPC152PROTO_FA_NEW_ALARM         0x0003
//Erase flags

//Request alarms file list
#define CPC152PROTO_FA_GET_ALARMS_LIST   0x0010
//Request alarms file
#define CPC152PROTO_FA_GET_ALARM_FILE    0x0020
#define CPC152PROTO_FA_ERASE_ALARM_FILE  0x0040

//Request all discrete data
#define CPC152PROTO_FA_REQUEST_DISCRETE  0x0100

#pragma pack(pop)

#define CPC152_DATA_CALC_SIZE(sz) (sizeof(_cpc152scan_data)-sizeof(BYTE)+sz)

#ifdef __cplusplus
inline int cpc152scan_data_calc_size(int dlen)
{
  return CPC152_DATA_CALC_SIZE(dlen);
}

inline int cpc152scan_data_get_size(const cpc152scan_data & sd)
{
  return sizeof(sd)-sizeof(sd.data[0])+sd.dlen;
}

inline int cpc152scan_data_comparer(const cpc152scan_data & sd1,const cpc152scan_data & sd2)
{
 int v1 = (int)sd1.dev_num&(~CPC152SCAN_DATA_NOTWORKED);
 int v2 = (int)sd2.dev_num&(~CPC152SCAN_DATA_NOTWORKED);
 if(v1 == v2)
 {
   v1 = (int)sd1.group_num;
   v2 = (int)sd2.group_num;
   if(v1 == v2) return 0;
 }
 return v1 < v2 ? -1:1;
}

inline bool _cpc152scan_data::operator <  (const _cpc152scan_data & sd)
{
  return cpc152scan_data_comparer(*this,sd) < 0 ? true : false;
}

inline bool _cpc152scan_data::operator == (const _cpc152scan_data & sd)
{
   return cpc152scan_data_comparer(*this,sd) == 0 ? true : false;
}

inline _cpc152scan_data & _cpc152scan_data::operator =  (const _cpc152scan_data & sd)
{
  memcpy(this,&sd,cpc152scan_data_get_size(sd));

  return * this;
}

inline int cpc152proto_hdr_calc_size(int sz)
{
   lpcpc152proto_hdr ph;
   return (sizeof(*ph)-sizeof(ph->data[0])+sz);
}


inline talarm_condition & talarm_condition::operator = (const talarm_condition& src)
{

    dev_num   = src.dev_num;
    grp_num   = src.grp_num;
    param_num = src.param_num;
    more  = src.more;
    alarm_value = src.alarm_value;
    return *this;
}

inline int talarm_condition::comparer(const talarm_condition & ac) const
{
    int ret = 0;
    if(dev_num == ac.dev_num)
    {
        if(grp_num == ac.grp_num)
        {
          if(param_num==ac.param_num)
            ret = 0;
          else
           ret = param_num< ac.param_num ? -1 : 1;
        }
        else
         ret = grp_num< ac.grp_num ? -1 : 1;
    }
    else
      ret =   dev_num< ac.dev_num ? -1 : 1;
    return ret;
}

inline bool operator < (const  talarm_condition & ac1,const talarm_condition & ac2)
{
    return ac1.comparer(ac2)<0 ? true:false;
}

inline bool operator == (const  talarm_condition & ac1,const talarm_condition & ac2)
{
    return ac1.comparer(ac2)==0 ? true:false;
}

inline bool operator != (const  talarm_condition & ac1,const talarm_condition & ac2)
{
    return ac1.comparer(ac2)!=0 ? true:false;
}

#endif


#endif
