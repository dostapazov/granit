#ifndef __MODEM_PROTO_INC
#define __MODEM_PROTO_INC

#ifdef _WIN32
#include <ke_defs.h>
#include <otd_proto.h>
#include <modem_proto_fa.h>
#else
#include <lin_ke_defs.h>
#include <gklib/otd_proto.h>
#include <gklib/modem_proto_fa.h>
#endif // _WIN32_

#pragma pack(push,1)

#define MODEM_ADDR_MAXVALUE 255
struct MODEM_ADDR
{
  union{
  struct {BYTE sta,modem,line,reserv1;};
  DWORD  addr;
       };
};

typedef MODEM_ADDR* LPMODEM_ADDR;


typedef struct _MPROTO_FRAME
{
 WORD   number;
 WORD   size;
 DWORD  data[ANYSIZE_ARRAY];
}MPROTO_FRAME, *LPMPROTO_FRAME;

typedef  struct MPROTO_FRAME0
{

}MPROTO_FRAME0,*LPMPROTO_FRAME0;


#define MP_INTERNAL_DEFAULT 0xAAAA5555

struct MPROTO_HEADER
{
 MODEM_ADDR   addr_to;    //Адрес кому
 MODEM_ADDR   addr_from;  //От кого
 WORD         pkt_num;    //Номер пакета
 WORD         fa;         //Функциональный адрес
 WORD         flags;      //Управляющие Флаги
 DWORD        internal ;  //Внутренне поле
 WORD         data_size;  //Размер данных
 WORD         header_cs;  //Контрольная сумма заголовка
 WORD         data[ANYSIZE_ARRAY];//Данные
};

typedef MPROTO_HEADER* LPMPROTO_HEADER;

#define MPROTO_SIZE(mph) ((mph)->data_size+sizeof(MPROTO_HEADER)-sizeof(WORD))



/*Флаг, отправки по вещательным адресам без разворачивания по линиям  */

#define MPROTO_FLAG_SEND_BROADCAST    0x00000001

#pragma pack(pop)

#ifdef __cplusplus

inline void mproto_init(LPMPROTO_HEADER mph,WORD pkt_num,WORD fa,WORD data_size,LPMODEM_ADDR  addr_to = NULL)
{
  ZeroMemory(mph,sizeof(*mph) - sizeof(mph->data));
  mph->fa        = fa;
  mph->data_size = data_size;
  mph->internal  = MP_INTERNAL_DEFAULT;
  mph->pkt_num   = pkt_num;
  if(addr_to)
    mph->addr_to.addr = addr_to->addr;
}

inline void mproto_protect(LPMPROTO_HEADER mph)
{
  mph->header_cs = otd_calc_checksum(mph,sizeof(*mph)-sizeof(mph->data)-sizeof(mph->header_cs),OTD_DEF_POLINOM);
  if(!mph->header_cs) mph->header_cs = ((OTD_DEF_POLINOM>>8)&0xFF)|((OTD_DEF_POLINOM<<8)&0xFF00);
}

inline bool mproto_check(LPMPROTO_HEADER mph)
{
 return  (0 == otd_calc_checksum(mph,sizeof(*mph)-sizeof(mph->data[0]),OTD_DEF_POLINOM)) ? true : false;
}

inline DWORD mproto_size(LPMPROTO_HEADER mph) {return mph->data_size + sizeof(*mph) - sizeof(mph->data);}


 struct modem_addr:public MODEM_ADDR
 {
    modem_addr(){addr = -1;}
    modem_addr(DWORD _modem,DWORD _line,DWORD r1 = -1){sta = 0;modem=(BYTE)_modem;line = (BYTE)_line;reserv1 = (BYTE)r1;}
    modem_addr(DWORD _sta ,DWORD _modem,DWORD _line,DWORD r1 = -1){sta = (BYTE)_sta;modem=(BYTE)_modem;line = (BYTE)_line;reserv1 = (BYTE)r1;}
    modem_addr(DWORD ma){addr = ma;}
    modem_addr(const MODEM_ADDR & ma){*this = ma;}
    modem_addr & operator = (const MODEM_ADDR & ma){addr = ma.addr; return * this;}
    bool operator < (const MODEM_ADDR & ma);
 };

 inline int  __fastcall modem_addr_cmp(const MODEM_ADDR & ma1,const MODEM_ADDR & ma2)
 {
   int ret = 0;
   if(ma1.addr!=ma2.addr)
   {
    ret = (int) ma1.sta   - (int)ma2.sta  ;
    if(!ret)
     {
      ret = (int) ma1.modem - (int)ma2.modem;
      if(!ret)
       {
        ret = (int) ma1.line     - (int)ma2.line   ;
        if(!ret)
          ret = (int) ma1.reserv1  - (int)ma2.reserv1;
       }
     }
   }
   return ret;
 }

inline bool modem_addr::operator < (const MODEM_ADDR & ma)
{
 return modem_addr_cmp(*this,ma)< 0 ? true:false;
}

inline bool operator < (const MODEM_ADDR & ma1,const MODEM_ADDR &ma2)
{
 return modem_addr_cmp(ma1,ma2)< 0 ? true:false;
}

inline bool operator > (const MODEM_ADDR & ma1,const MODEM_ADDR &ma2)
{
 return modem_addr_cmp(ma1,ma2)> 0 ? true:false;
}

inline bool operator  == (const MODEM_ADDR & ma1,const MODEM_ADDR &ma2)
{  return ma1.addr == ma2.addr  ? true:false; }

inline bool operator  != (const MODEM_ADDR & ma1,const MODEM_ADDR &ma2)
{  return ma1.addr != ma2.addr  ? true:false; }

struct modem_addr_less
{
 bool operator()(const modem_addr ma1,const modem_addr & ma2){return ma1<ma2;}
};

struct modem_addr_include
{
 bool operator()(const modem_addr ma1,const modem_addr & ma2);
};

inline bool modem_addr_include::operator()(const modem_addr ma1,const modem_addr & ma2)
{
  return (
  ( ma1.sta   == ma2.sta   || ma1.sta   == MODEM_ADDR_MAXVALUE || ma2.sta   == MODEM_ADDR_MAXVALUE) &&
  ( ma1.modem == ma2.modem || ma1.modem == MODEM_ADDR_MAXVALUE || ma2.modem == MODEM_ADDR_MAXVALUE) &&
  ( ma1.line  == ma2.line  || ma1.line  == MODEM_ADDR_MAXVALUE || ma2.line  == MODEM_ADDR_MAXVALUE) &&
  ( ma1.reserv1 == ma2.reserv1  || ma1.reserv1  == MODEM_ADDR_MAXVALUE || ma2.reserv1  == MODEM_ADDR_MAXVALUE)
  ) ? true:false;
}



#ifdef _WINDOWS_
#include <rx_buffer.hpp>
#else
#include <gklib/rx_buffer.hpp>
#endif

class modem_proto_buffer:public rx_buffer
{
  protected:
  public:
  modem_proto_buffer(int _sz = 0):rx_buffer(_sz){};
  int   __fastcall peek_mproto(LPBYTE buffer,DWORD sz,bool remove);
  DWORD __fastcall sync_internal(DWORD int_val = MP_INTERNAL_DEFAULT);
  int   __fastcall get_mproto_len();
  bool  __fastcall move_mproto();

};

#endif


#endif


