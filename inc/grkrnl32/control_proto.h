/*
Структуры  управляющего протокола
*/


#ifndef CONTROL_PROTO_H
#define CONTROL_PROTO_H


#ifdef __WIN32__
#include <ke_defs.h>
#include <otd_proto.h>
#endif

#ifdef __linux__
#include <lin_ke_defs.h>
#include <gklib/otd_proto.h>
#endif



#pragma pack (push,1)

#define  CTRL_PROTO_COMMAND_MASK 0x0FFFFFFF
#define  CTRL_PROTO_FLAG_RESPOND 0x80000000
#define  CTRL_PROTO_FLAG_END     0x10000000
#define  CTRL_PROTO_FLAG_ERROR   0x20000000
#define  CTRL_PROTO_FLAG_RESERV  0x40000000

#define  CTRL_PROTO_COMMAND(cmd) (cmd&CTRL_PROTO_COMMAND_MASK)
#define  CTRL_PROTO_IS_RESPOND(cmd) (cmd&CTRL_PROTO_FLAG_RESPOND)
#define  CTRL_PROTO_IS_ERROR(cmd) (cmd&CTRL_PROTO_FLAG_ERROR )
#define  CTRL_PROTO_IS_END(cmd) (cmd&CTRL_PROTO_FLAG_END)




#define CTRL_PROTO_CMD_TIMESYNC   1
struct ctrl_proto_header
{
  DWORD   command;
  DWORD   data_size;
  BYTE    data[ANYSIZE_ARRAY];
};
typedef ctrl_proto_header* lpctrl_proto_header;

struct ctrl_proto_times
{
 QWORD self_time;
 QWORD remote_time;
 BOOL utc;
};

typedef ctrl_proto_times* lpctrl_proto_times;


#pragma pack (pop)

#ifdef __cplusplus
inline DWORD winapi ctrl_proto_calc_size(DWORD sz)
{
  lpctrl_proto_header lph = NULL;
  return sz+sizeof(WORD)+sizeof(ctrl_proto_header)-sizeof(lph->data[0]);
}

inline LPWORD winapi ctrl_proto_get_lpkpk(lpctrl_proto_header lph)
{
  LPBYTE ptr = lph->data;
  ptr+=lph->data_size;
  return (LPWORD)ptr;
}

inline void winapi ctrl_proto_protect(lpctrl_proto_header lph)
{
   LPWORD lp_kpk = ctrl_proto_get_lpkpk(lph);
   DWORD  sz  = lph->data_size+sizeof(*lph)-sizeof(lph->data[0]);
   WORD kpk   = otd_calc_checksum(lph,sz,OTD_DEF_POLINOM);
   if(!kpk)
   {
            kpk = OTD_DEF_POLINOM;
            kpk = ((kpk>>8)&0x00FF) | ((kpk<<8)&0xFF00);
   }
   *lp_kpk = kpk;
}

inline bool winapi ctrl_proto_check(lpctrl_proto_header lph)
{
    return 0 == otd_calc_checksum(lph,ctrl_proto_calc_size(lph->data_size),0) ? true : false;
}

inline DWORD winapi ctrl_proto_init(DWORD cmd,void * ptr,DWORD sz,void * data,DWORD data_size,LPDWORD need_sz)
{
    DWORD ret = ctrl_proto_calc_size(data_size);
    if(ret<=sz && ptr)
    {
      lpctrl_proto_header lph = (lpctrl_proto_header)ptr;
      lph->command = cmd;
      lph->data_size = data_size;
      memcpy(lph->data,data,data_size);
      ctrl_proto_protect(lph);
    }
    else
    {
        if(need_sz)*need_sz = ret;
        ret = 0;
    }
    return ret;
}

#endif


#endif // CONTROL_PROTO_H



