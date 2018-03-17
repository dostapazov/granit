/*-----------------------------------------------------------------------------
 12.02.2004 16:12:25 
 Модуль:   otd_addr
 Автор:    Остапенко Д.В.
 Назначение: Описание адресов для универсальной ОТД адресации

-----------------------------------------------------------------------------*/



#ifndef OTD_ADDR_H
#define OTD_ADDR_H 1
#ifdef __linux__
#include <lin_ke_defs.h>
#include <gklib/otd_types.h>
#else
#include <ke_defs.h>
#include <otd_types.h>
#endif



#pragma pack(push,1)

struct  otd_addr
{
  union{
  struct{ BYTE pu,cp,fa,sb;};
  DWORD  addr;
  };
};

#define lpotd_addr otd_addr*


#define OTD_SET_ADDR(a,p,c,f,m) ((a)->pu=(BYTE)p,(a)->cp=(BYTE)c,(a)->fa=(BYTE)f,(a)->sb = (BYTE)m)
#define OTD_ADDR_ISPU(a)  ((a)->pu!=0xFF && (a)->cp== 0xFF)
#define OTD_ADDR_ISCP(a)  ((a)->pu!=0xFF && (a)->cp!= 0xFF && ((a)->fa == 0xFF || (a)->fa ==OTD_FA_ALL) )
#define OTD_ADDR_ISFA(a)  ((a)->pu!=0xFF && (a)->cp!= 0xFF && (a)->fa != 0xFF && (a)->sb == 0xFF)
#define OTD_ADDR_ISSB(a)  ((a)->pu!=0xFF && (a)->cp!= 0xFF && (a)->fa != 0xFF && (a)->sb != 0xFF)


#define OTD_ADDR_ISQUERY(a) ((a)->fa!=OTD_ADDR_MAXVALUE && (a)->fa&OTD_FA_QUERY)
#define OTD_ADDR_ISSETUP(a) ((a)->fa!=OTD_ADDR_MAXVALUE && (a)->fa&OTD_FA_SETUP)


#define OTD_ADDR_TYPE_PU        0
#define OTD_ADDR_TYPE_CP        1
#define OTD_ADDR_TYPE_FA        2
#define OTD_ADDR_TYPE_SB        3
#define OTD_ADDR_TYPE_BROADCAST 4
#define OTD_ADDR_TYPE_UNDEF     (-1)


struct otd_param_addr
{
  otd_addr     addr;
  DWORD        param_number;
};

#define lpotd_param_addr otd_param_addr*

#ifdef __cplusplus
extern "C"
{
#endif


DWORD   winapi otd_addr_type    (const lpotd_addr addr);
int     winapi otd_addr_cmp_part(const lpotd_addr a1,const lpotd_addr a2,DWORD tp_lim);
BOOL    winapi otd_addr_include ( lpotd_addr checked,lpotd_addr mask);
#define        otd_addr_cmp(x,y) otd_addr_cmp_part((x),(y),OTD_ADDR_TYPE_SB)

#ifdef __cplusplus
} //extern "C"
#endif

#ifdef __cplusplus

inline bool operator < (const otd_addr & a1,const otd_addr & a2)
{return (otd_addr_cmp((lpotd_addr)&a1,(lpotd_addr)&a2)<0) ? true:false;}
inline bool operator ==(const otd_addr & a1,const otd_addr & a2)
{return (otd_addr_cmp((lpotd_addr)&a1,(lpotd_addr)&a2)==0)? true:false;}
inline bool operator !=(const otd_addr & a1,const otd_addr & a2)
{return (otd_addr_cmp((lpotd_addr)&a1,(lpotd_addr)&a2)!=0)? true:false;}
inline bool operator >(const otd_addr & a1,const otd_addr & a2)
{return (otd_addr_cmp((lpotd_addr)&a1,(lpotd_addr)&a2)>0)? true:false;}

struct otd_addr_less
{
 bool operator()(otd_addr & a1,otd_addr & a2)
 { return (otd_addr_cmp((lpotd_addr)&a1,(lpotd_addr)&a2)<0)? true:false;}
 bool operator()(const otd_addr & a1,const otd_addr & a2)
 { return (otd_addr_cmp((lpotd_addr)&a1,(lpotd_addr)&a2)<0)? true:false;}
};

struct otd_addr_equal
{
 bool  operator()(otd_addr & a1,otd_addr & a2)
 {return (otd_addr_cmp((lpotd_addr)&a1,(lpotd_addr)&a2)==0) ? true:false;}
};

struct sotd_addr:public otd_addr
{
 sotd_addr(){addr=(-1);}
 sotd_addr(DWORD a){addr = a;}
 sotd_addr(DWORD _pu,DWORD _cp,DWORD _fa,DWORD _sb){pu=(BYTE)_pu,cp=(BYTE)_cp,fa=(BYTE)_fa,sb=(BYTE)(_sb);}
 sotd_addr(const otd_addr & a){*this = a;}
 sotd_addr & operator = (const otd_addr & a){addr = a.addr;return *this;}
 bool operator < (const otd_addr & a){return (otd_addr_cmp((lpotd_addr)&addr,(lpotd_addr)&a)<0) ? true:false;}
 int  __fastcall addrtype(){return otd_addr_type(this);}
 sotd_addr  __fastcall owner_addr();
 DWORD      __fastcall number(){int type = addrtype();return type<OTD_ADDR_TYPE_BROADCAST ? (DWORD)( ((LPBYTE)&addr)[type]):0;}
 void       __fastcall set_number(DWORD num){int type =(int)addrtype();((LPBYTE)&addr)[type] =  num <OTD_ADDR_MAXVALUE ? (BYTE)num:0;}
 bool       __fastcall is_include(otd_addr & addr,bool as_mask = false){return (as_mask ? otd_addr_include(&addr,this):otd_addr_include(this,&addr)) ? true:false;}
};

inline sotd_addr  __fastcall sotd_addr::owner_addr()
{
 sotd_addr ret(*this);
 int tp = addrtype();
 if(tp< OTD_ADDR_TYPE_BROADCAST)
  ((LPBYTE)(&ret))[tp]=OTD_ADDR_MAXVALUE;
  return ret;
}


struct sotd_param_addr:public otd_param_addr
{
 sotd_param_addr(){addr.addr = -1;param_number = -1;}
 sotd_param_addr(DWORD _addr,DWORD paramN){addr.addr = _addr;param_number = paramN;}
 sotd_param_addr(const otd_addr & _addr,DWORD paramN){addr.addr = _addr.addr;param_number = paramN;};
 sotd_param_addr(const otd_param_addr & pa){*this = pa;};
 sotd_param_addr & operator = (const otd_param_addr & pa){addr.addr = pa.addr.addr;param_number = pa.param_number;return *this;}
 bool            __fastcall is_include(otd_param_addr & pa);
 sotd_param_addr __fastcall owner_addr();
 int             __fastcall addrtype();
};

inline bool operator == (const  otd_param_addr & pa1, const  otd_param_addr & pa2)
{
  return (pa1.addr == pa2.addr && pa1.param_number == pa2.param_number) ? true:false;
}

inline bool operator < (const  otd_param_addr & pa1, const  otd_param_addr & pa2)
{
  if(pa1.addr  == pa2.addr )
    return pa1.param_number<pa2.param_number ? true : false;
    return pa1.addr<pa2.addr ? true : false;
}

inline bool operator != (const  otd_param_addr & pa1, const  otd_param_addr & pa2)
{
  return !(pa1 == pa2);
}

inline bool __fastcall sotd_param_addr::is_include(otd_param_addr & pa)
{
 return ( sotd_addr(addr).is_include(pa.addr,true) &&
         (param_number == pa.param_number|| param_number == (DWORD)-1)) ?
          true:false;
}

inline sotd_param_addr __fastcall sotd_param_addr::owner_addr()
{
   sotd_param_addr addr(*this);
   if(addr.param_number == (DWORD)-1)
      addr.addr = sotd_addr(addr.addr).owner_addr();
      else
      addr.param_number = (DWORD)-1;
   return addr;
}

inline int  __fastcall sotd_param_addr::addrtype()
 {return otd_addr_type(&addr);}




#endif

#pragma pack(pop)
#endif
