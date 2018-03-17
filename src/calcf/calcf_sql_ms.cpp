/*Поддержка MS_SQL_Server 7.0*/

//#define AUTOLOAD_MSODS_DLL // должно подключать  библиотеку opends60.lib статически

#include <kertl.hpp>
#include <calcf.h>
#include <srv.h>
#include <stdio.h>

#ifdef _MSC_VER
#pragma warning (disable : 4800)
#endif

#ifndef SRVBIGVARCHAR
#define SRVBIGVARCHAR SRVVARCHAR
#endif

#ifndef ODS_VERSION			

#define SS_MAJOR_VERSION    7
#define SS_MINOR_VERSION    00
#define SS_LEVEL_VERSION    0000
#define SS_MINIMUM_VERSION  "7.00.00.0000"

#define ODS_VERSION			((SS_MAJOR_VERSION << 24)  | (SS_MINOR_VERSION << 16))
#endif


typedef int   ( *fp_srv_senddone      )(SRV_PROC* srvproc,USHORT status,USHORT curcmd,long int count);
typedef int   ( *fp_srv_sendrow       )(SRV_PROC* srvproc );
typedef int   ( *fp_srv_returnval     )( SRV_PROC *,char*,int,BYTE,long int,long int,long int,void* );


typedef int   ( *fp_srv_rpcparams     )(SRV_PROC*);
typedef int   ( *fp_srv_paraminfo     )(SRV_PROC*,int,BYTE*,ULONG*,ULONG*,BYTE*,BOOL*);
typedef int   ( *fp_srv_paramsetoutput)(SRV_PROC*,int,BYTE*,ULONG,BOOL);

typedef void* ( *fp_srv_paramdata     )(SRV_PROC*,int);
typedef int   ( *fp_srv_paramlen      )(SRV_PROC*,int);
typedef int   ( *fp_srv_parammaxlen   )(SRV_PROC*,int);
typedef int   ( *fp_srv_paramtype     )(SRV_PROC*,int);
typedef int   ( *fp_srv_paramset      )(SRV_PROC*,int,void*,int);

typedef char* ( *fp_srv_paramname     )(SRV_PROC*,int,int*);
typedef int   ( *fp_srv_paramnumber   )(SRV_PROC*,char*,int);

typedef BOOL  ( *fp_srv_willconvert   )(int srctype,int desttype );
typedef int   ( *fp_srv_convert       )(SRV_PROC*,int,void*,long int,int,void*,long int);

typedef char* ( *fp_srv_rpcname       )(SRV_PROC * srvproc,int* len );
typedef int   ( *fp_srv_paramstatus   )(SRV_PROC*,int);

typedef int   ( *fp_srv_describe      )(SRV_PROC*,int,char*,int,long int,long int,long int,long int,void*);
typedef int   ( *fp_srv_setutype      )(SRV_PROC* srvproc,int column,long int usertype);
typedef int   ( *fp_srv_setcoldata    )(SRV_PROC* srvproc,int column,void* data);
typedef int   ( *fp_srv_setcollen     )( SRV_PROC* srvproc,int column,int len);


class  TOpenDS_DLL
{
 static fp_srv_senddone       _srv_senddone;
 static fp_srv_sendrow        _srv_sendrow;
 static fp_srv_returnval      _srv_returnval;
 static fp_srv_rpcparams      _srv_rpcparams;
 static fp_srv_paraminfo      _srv_paraminfo;
 static fp_srv_paramsetoutput _srv_paramsetoutput;

 static fp_srv_paramdata      _srv_paramdata;
 static fp_srv_paramlen       _srv_paramlen;
 static fp_srv_parammaxlen    _srv_parammaxlen;
 static fp_srv_paramtype      _srv_paramtype;
 static fp_srv_paramset       _srv_paramset;

 static fp_srv_paramname      _srv_paramname;
 static fp_srv_paramnumber    _srv_paramnumber;
 static fp_srv_willconvert    _srv_willconvert;
 static fp_srv_convert        _srv_convert;

 static fp_srv_rpcname        _srv_rpcname;
 static fp_srv_paramstatus    _srv_paramstatus;

 static fp_srv_describe              _srv_describe ;
 static fp_srv_setutype              _srv_setutype ;
 static fp_srv_setcoldata            _srv_setcoldata;
 static fp_srv_setcollen             _srv_setcollen ;

 static HANDLE hLibInstance;
 public:
  TOpenDS_DLL();
 ~TOpenDS_DLL();
 static bool IsGood(){

 return
 #ifndef AUTOLOAD_MSODS_DLL
 (hLibInstance==0) ? false:
 #endif
 true;
 };

 static int srv_senddone(SRV_PROC* srvproc,USHORT status,USHORT curcmd,long int count);
 static int srv_sendrow  (SRV_PROC * srvproc);
 static int srv_returnval( SRV_PROC *,char*,int,BYTE,long int,long int,long int,void* );

 static int srv_rpcparams(SRV_PROC*);
 static int srv_paraminfo(SRV_PROC*,int,BYTE*,ULONG*,ULONG*,BYTE*,BOOL*);
 static int srv_paramsetoutput(SRV_PROC*,int,BYTE*,ULONG,BOOL);

 static void* srv_paramdata(SRV_PROC*,int);
 static int srv_paramlen(SRV_PROC*,int);
 static int srv_parammaxlen(SRV_PROC*,int);
 static int srv_paramtype(SRV_PROC*,int);
 static int srv_paramset(SRV_PROC*,int,void*,int);

 static char* srv_paramname(SRV_PROC*,int,int*);
 static int   srv_paramnumber(SRV_PROC*,char*,int);

 static BOOL  srv_willconvert   (int srctype,int desttype );
 static int   srv_convert       (SRV_PROC*,int,void*,long int,int,void*,long int);

 static char* srv_rpcname       (SRV_PROC * srvproc,int* len );
 static int   srv_paramstatus   (SRV_PROC*,int);

 static int   srv_describe      (SRV_PROC*,int,char*,int,long int,long int,long int,long int,void*);
 static int   srv_setutype      (SRV_PROC* srvproc,int column,long int usertype);
 static int   srv_setcoldata    (SRV_PROC* srvproc,int column,void* data);
 static int   srv_setcollen     ( SRV_PROC* srvproc,int column,int len);

};

  HANDLE TOpenDS_DLL::hLibInstance =0;

  TOpenDS_DLL::TOpenDS_DLL()
  {
   hLibInstance = LoadLibrary("opends60.dll");
   if(hLibInstance)
   {
    #ifndef AUTOLOAD_MSODS_DLL
     
    _srv_senddone       = (fp_srv_senddone        )GetProcAddress((HINSTANCE)hLibInstance,"srv_senddone");
    _srv_sendrow        = (fp_srv_sendrow         )GetProcAddress((HINSTANCE)hLibInstance,"srv_sendrow");
    _srv_rpcparams      = (fp_srv_rpcparams       )GetProcAddress((HINSTANCE)hLibInstance,"srv_rpcparams");
    _srv_paraminfo      = (fp_srv_paraminfo       )GetProcAddress((HINSTANCE)hLibInstance,"srv_paraminfo");
    _srv_paramsetoutput = (fp_srv_paramsetoutput  )GetProcAddress((HINSTANCE)hLibInstance,"srv_paramsetoutput");

    _srv_paramdata      = (fp_srv_paramdata       )GetProcAddress((HINSTANCE)hLibInstance,"srv_paramdata");
    _srv_paramlen       = (fp_srv_paramlen        )GetProcAddress((HINSTANCE)hLibInstance,"srv_paramlen");
    _srv_parammaxlen    = (fp_srv_parammaxlen     )GetProcAddress((HINSTANCE)hLibInstance,"srv_parammaxlen");
    _srv_paramtype      = (fp_srv_paramtype       )GetProcAddress((HINSTANCE)hLibInstance,"srv_paramtype");
    _srv_paramset       = (fp_srv_paramset        )GetProcAddress((HINSTANCE)hLibInstance,"srv_paramset");

    _srv_paramname      = (fp_srv_paramname       )GetProcAddress((HINSTANCE)hLibInstance,"srv_paramname");
    _srv_paramnumber    = (fp_srv_paramnumber     )GetProcAddress((HINSTANCE)hLibInstance,"srv_paramnumber");
    _srv_willconvert    = (fp_srv_willconvert     )GetProcAddress((HINSTANCE)hLibInstance,"srv_willconvert");
    _srv_convert        = (fp_srv_convert         )GetProcAddress((HINSTANCE)hLibInstance,"srv_convert");
    _srv_rpcname        = (fp_srv_rpcname         )GetProcAddress((HINSTANCE)hLibInstance,"srv_rocname");
    _srv_paramstatus    = (fp_srv_paramstatus     )GetProcAddress((HINSTANCE)hLibInstance,"srv_paramstatus");
    _srv_returnval      = (fp_srv_returnval       )GetProcAddress((HINSTANCE)hLibInstance,"srv_returnval");

    _srv_describe       = (fp_srv_describe        )GetProcAddress((HINSTANCE)hLibInstance,"srv_describe");
    _srv_setutype       = (fp_srv_setutype        )GetProcAddress((HINSTANCE)hLibInstance,"srv_setutype");
    _srv_setcoldata     = (fp_srv_setcoldata      )GetProcAddress((HINSTANCE)hLibInstance,"srv_setcoldata");
    _srv_setcollen      = (fp_srv_setcollen       )GetProcAddress((HINSTANCE)hLibInstance,"srv_setcollen");



    #else

    _srv_senddone       = srv_senddone;
    _srv_sendrow        = srv_sendrow;
    _srv_rpcparams      = srv_rpcparams;
    _srv_paraminfo      = srv_paraminfo;
    _srv_paramsetoutput = srv_paramsetoutput;

    _srv_paramdata      = srv_paramdata;
    _srv_paramlen       = srv_paramlen;
    _srv_parammaxlen    = srv_parammaxlen;
    _srv_paramtype      = srv_paramtype;
    _srv_paramset       = srv_paramset;

    _srv_paramname      = srv_paramname;
    _srv_paramnumber    = srv_paramnumber;
    _srv_willconvert    = srv_willconvert;
    _srv_convert        = srv_convert;
    _srv_rpcname        = srv_rocname;
    _srv_paramstatus    = srv_paramstatus;
    _srv_returnval      = srv_returnval;
    _srv_describe       = srv_describe;
    _srv_setutype       = srv_setutype;
    _srv_setcoldata     = srv_setcoldata;
    _srv_setcollen      = srv_setcollen;
    #endif


   }
  }

  TOpenDS_DLL::~TOpenDS_DLL()
  {
    FreeLibrary((HINSTANCE)hLibInstance);
  }

fp_srv_senddone TOpenDS_DLL::_srv_senddone=0;

int TOpenDS_DLL::srv_senddone(SRV_PROC* srvproc,USHORT status,USHORT curcmd,long int count)
{
 if(_srv_senddone)
    return _srv_senddone(srvproc,status,curcmd,count);
    return FAIL;
}

fp_srv_sendrow  TOpenDS_DLL::_srv_sendrow = 0;
int TOpenDS_DLL::srv_sendrow  (SRV_PROC * srvproc)
{
 if(_srv_sendrow)
    return _srv_sendrow(srvproc);
    return FAIL;
}

fp_srv_returnval      TOpenDS_DLL::_srv_returnval=0;
int   TOpenDS_DLL::srv_returnval( SRV_PROC * srvproc,char* name,int nlen,BYTE stat,long int type,long int msz,long int sz,void* data )
{
 if(_srv_returnval)
    return _srv_returnval(srvproc,name,nlen,stat,type,msz,sz,data);
    return FAIL;
}

fp_srv_rpcparams      TOpenDS_DLL::_srv_rpcparams= 0;
fp_srv_paraminfo      TOpenDS_DLL::_srv_paraminfo= 0;
fp_srv_paramsetoutput TOpenDS_DLL::_srv_paramsetoutput= 0;

fp_srv_paramdata      TOpenDS_DLL::_srv_paramdata= 0;
fp_srv_paramlen       TOpenDS_DLL::_srv_paramlen= 0;
fp_srv_parammaxlen    TOpenDS_DLL::_srv_parammaxlen= 0;
fp_srv_paramtype      TOpenDS_DLL::_srv_paramtype= 0;
fp_srv_paramset       TOpenDS_DLL::_srv_paramset= 0;

fp_srv_paramname      TOpenDS_DLL::_srv_paramname= 0;
fp_srv_paramnumber    TOpenDS_DLL::_srv_paramnumber= 0;
fp_srv_rpcname        TOpenDS_DLL::_srv_rpcname =0;
fp_srv_paramstatus    TOpenDS_DLL::_srv_paramstatus=0;


int TOpenDS_DLL::srv_rpcparams(SRV_PROC* srvproc)
{
 if(_srv_rpcparams)
    return _srv_rpcparams(srvproc);
    return FAIL;
}

int TOpenDS_DLL::srv_paraminfo(SRV_PROC* srvproc,int n,BYTE* pbType,ULONG* pcbMaxLen,ULONG* pcbActSize,BYTE* pbData,BOOL* pfNull)
{
 if(_srv_paraminfo)
    return _srv_paraminfo(srvproc,n,pbType,pcbMaxLen,pcbActSize,pbData,pfNull);
    return FAIL;
}

int TOpenDS_DLL::srv_paramsetoutput(SRV_PROC* srvproc,int n,BYTE* pbData,ULONG cbLen,BOOL fNull)
{
 if(_srv_paramsetoutput)
    return _srv_paramsetoutput(srvproc,n,pbData,cbLen,fNull);
    return FAIL;
}

void* TOpenDS_DLL::srv_paramdata(SRV_PROC* srvproc,int n)
{
 if(_srv_paramdata)
    return _srv_paramdata(srvproc,n);
    return FAIL;
}
int TOpenDS_DLL::srv_paramlen(SRV_PROC* srvproc,int n)
{
 if(_srv_paramlen)
    return _srv_paramlen(srvproc,n);
    return FAIL;
}
int TOpenDS_DLL::srv_parammaxlen(SRV_PROC* srvproc,int n)
{
 if(_srv_parammaxlen)
    return _srv_parammaxlen(srvproc,n);
    return FAIL;
}
int TOpenDS_DLL::srv_paramtype(SRV_PROC* srvproc,int n)
{
 if(_srv_paramtype)
    return _srv_paramtype(srvproc,n);
    return FAIL;
}

int TOpenDS_DLL::srv_paramset(SRV_PROC* srvproc,int n,void* data,int len)
{
 if(_srv_paramset)
    return _srv_paramset(srvproc,n,data,len);
    return FAIL;
}

char* TOpenDS_DLL::srv_paramname(SRV_PROC* srvproc,int n,int* len)
{
 if(_srv_paramname)
    return _srv_paramname(srvproc,n,len);
    return FAIL;
}

int TOpenDS_DLL::srv_paramnumber(SRV_PROC* srvproc,char* buf,int len)
{
 if(_srv_paramnumber)
    return _srv_paramnumber(srvproc,buf,len);
    return FAIL;
}

fp_srv_willconvert TOpenDS_DLL::_srv_willconvert = 0;
fp_srv_convert     TOpenDS_DLL::_srv_convert = 0;

BOOL  TOpenDS_DLL::srv_willconvert   (int srctype,int desttype )
{
 if(_srv_willconvert)
    return _srv_willconvert(srctype,desttype);
    return FAIL;
}

int   TOpenDS_DLL::srv_convert       (SRV_PROC* srvproc,int stype, void* src,long int slen,int dtype,void* dst,long int dlen)
{
 if(_srv_convert)
    return _srv_convert(srvproc,stype,src,slen,dtype,dst,dlen);
    return FAIL;
}

char* TOpenDS_DLL::srv_rpcname       (SRV_PROC * srvproc,int* len )
{
 if(_srv_rpcname)
    return _srv_rpcname(srvproc,len);
    return FAIL;
}
int   TOpenDS_DLL::srv_paramstatus(SRV_PROC* srvproc,int n)
{
 if(_srv_paramstatus)
    return _srv_paramstatus(srvproc,n);
    return FAIL;
}


 fp_srv_describe              TOpenDS_DLL::_srv_describe =0;
 fp_srv_setutype              TOpenDS_DLL::_srv_setutype =0;
 fp_srv_setcoldata            TOpenDS_DLL::_srv_setcoldata=0;
 fp_srv_setcollen             TOpenDS_DLL::_srv_setcollen =0;

  int TOpenDS_DLL::srv_describe      (SRV_PROC* srvproc,int cnum,char* cname,int clen,long int dt,long int dl,long int st,long int sl,void* src)
  {
   if(_srv_describe)
      return _srv_describe(srvproc,cnum,cname,clen,dt,dl,st,sl,src);
      return FAIL;
  }

  int TOpenDS_DLL::srv_setutype      (SRV_PROC* srvproc,int column,long int usertype)
  {
   if(_srv_setutype)
      return _srv_setutype(srvproc,column,usertype);
      return FAIL;
  }

  int TOpenDS_DLL::srv_setcoldata    (SRV_PROC* srvproc,int column,void* data)
  {
   if(_srv_setcoldata)
      return _srv_setcoldata(srvproc,column,data);
      return FAIL;
  }

  int TOpenDS_DLL::srv_setcollen     ( SRV_PROC* srvproc,int column,int len)
  {
   if(_srv_setcollen)
      return _srv_setcollen(srvproc,column,len);
      return FAIL;
  }





/* Интерфейс с сервером*/

struct TMSParamInfo
{
 int number;
 BYTE  bType;
 ULONG cbMaxLen;
 ULONG cbActualLen;
 BYTE  *pbData;
 BOOL  fNull;
 TMSParamInfo(){ZeroMemory(this,sizeof(*this));}
};


class TMSSrvProc
{
 SRV_PROC * srvproc;
 int     params_count;
 public:
  TMSSrvProc(SRV_PROC * srvproc);
  int    __fastcall GetParamsCount();
  bool   __fastcall GetParamInfo (TMSParamInfo & mpi);
  bool   __fastcall GetParamData (TMSParamInfo & mpi);
  LPCSTR __fastcall GetParamName (int n,int & name_len);
  bool   __fastcall ReturnValue  (int vType,int mazLen,int dataLen, void * data,int colnum,char * name = "RETURN",bool isNull = false);
  bool   __fastcall ReturnInteger(int val,int colnum =1,char * name = "INT",bool isNull = false);
  bool   __fastcall GetIntegerParam(int pnum,int * out , bool * isNull);
  bool   __fastcall ReturnString(char * str,int colnum =1,char * name = "STRING");
  bool   __fastcall GetStringParam(int pnum,char * out,int maxLen,bool *isNull);
};

  TMSSrvProc::TMSSrvProc(SRV_PROC * srvproc)
  {
   /*конструктор*/
   static TOpenDS_DLL ods_dll;
   this->srvproc = srvproc;
   params_count  = 0;
  }

  int   __fastcall TMSSrvProc::GetParamsCount()
  {
   if(!params_count)
       params_count = TOpenDS_DLL::srv_rpcparams(srvproc);
   return params_count;
  }

  bool  __fastcall TMSSrvProc::GetParamInfo(TMSParamInfo & mpi)
  {
   bool ret(false);
   if(TOpenDS_DLL::IsGood())
   {
    ret = (TOpenDS_DLL::srv_paraminfo(srvproc,mpi.number,&mpi.bType,&mpi.cbMaxLen,&mpi.cbActualLen,0,&mpi.fNull) == SUCCEED) ? true:false;
   }
   return ret;
  }

  bool  __fastcall TMSSrvProc::GetParamData(TMSParamInfo & mpi)
  {
   bool ret(false);
   if(TOpenDS_DLL::IsGood())
   {
    ret = (TOpenDS_DLL::srv_paraminfo(srvproc,mpi.number,&mpi.bType,&mpi.cbMaxLen,&mpi.cbActualLen,mpi.pbData,&mpi.fNull) == SUCCEED) ? true:false;
   }
   return ret;
  }

  LPCSTR __fastcall TMSSrvProc::GetParamName(int n,int & name_len)
  {
   if(TOpenDS_DLL::IsGood())
   {
    return TOpenDS_DLL::srv_rpcname(srvproc,&name_len);
   }
   return 0;
  }

  bool   __fastcall TMSSrvProc::ReturnValue (int vType,int maxLen,int dataLen, void * data,int colnum,char * name,bool isNull )
  {
   bool ret(false);
   if(TOpenDS_DLL::IsGood())
   {
   if(
   TOpenDS_DLL::srv_describe(srvproc,colnum,name,SRV_NULLTERM,vType,maxLen,vType,maxLen,data) &&
   TOpenDS_DLL::srv_setcollen(srvproc,colnum,dataLen) &&
   TOpenDS_DLL::srv_setcoldata(srvproc,colnum,data) &&
   TOpenDS_DLL::srv_sendrow(srvproc) &&
   TOpenDS_DLL::srv_paramsetoutput(srvproc,colnum,(BYTE*)data,dataLen,BOOL(isNull)) &&
   TOpenDS_DLL::srv_senddone( srvproc,(SRV_DONE_COUNT | SRV_DONE_MORE), 0, colnum)
   )
    ret = SUCCEED;
   }
   return ret;

  }

  bool   __fastcall TMSSrvProc::ReturnInteger(int val,int colnum,char * name,bool isNull)
  {
   return ReturnValue(SRVINT4,sizeof(val),sizeof(val),&val,colnum,name,isNull);
  }

  bool   __fastcall TMSSrvProc::GetIntegerParam(int pnum,int * out , bool * isNull)
  {
       bool ret (false);
       TMSParamInfo mpi;
       mpi.number = pnum;
       *out = 0;
       if(out && this->GetParamInfo(mpi))
       {
        if(mpi.bType == SRVINT4 /*|| mpi.bType == SRVINTN*/)
        {
         mpi.pbData = (BYTE*)out;
         GetParamInfo(mpi);
         ret = true;
        }
        else
        {
         if(TOpenDS_DLL::srv_willconvert(mpi.bType,SRVINT4))
         {
          // Попытка преобразовать
          mpi.pbData = new BYTE[mpi.cbActualLen];
          GetParamData(mpi);
          if(TOpenDS_DLL::srv_convert( srvproc,mpi.bType,mpi.pbData,mpi.cbActualLen,SRVINT4,out,sizeof(*out)))
             ret = true;

          delete [] mpi.pbData;
         }
        }
        if(ret && isNull)
         *isNull =(bool) mpi.fNull;
       }

       return ret;
  }

  bool   __fastcall TMSSrvProc::ReturnString(char * str,int colnum ,char * name )
  {
   int len = lstrlen(str)+1;

   return this->ReturnValue(SRVBIGVARCHAR,len,len,str,colnum,name,(str!=0) ? FALSE:TRUE);
  }

  bool   __fastcall TMSSrvProc::GetStringParam(int pnum,char * out,int maxLen,bool *isNull)
  {
   TMSParamInfo mpi;
   bool ret(false);
   mpi.number = pnum;
   if(this->GetParamInfo(mpi))
    {
     mpi.pbData = new BYTE[mpi.cbActualLen+1];
     GetParamData(mpi);
     mpi.pbData[mpi.cbActualLen]=0;

     if(TOpenDS_DLL::srv_willconvert(mpi.bType,SRVTEXT) )
      {
      if(TOpenDS_DLL::srv_convert( srvproc,mpi.bType,mpi.pbData,mpi.cbActualLen,SRVTEXT,out,maxLen))
      {
       ret = true;
      }
      }
      else
      {
       switch(mpi.bType)
       {
        case SRVBIGVARCHAR:
        default:
             lstrcpyn(out,(LPCSTR)mpi.pbData,maxLen);
             ret = true;
            break;

       }
      }
      delete [] mpi.pbData;
      if(ret && isNull)
         *isNull =(bool) mpi.fNull;
    }
    return ret;
  }


/* Собственно реализаци внешних процедур */

extern "C"  DWORD __GetXpVersion()
{
 return ODS_VERSION;
}


extern "C" SRVRETCODE msxp_AllocCalculator(SRV_PROC * srvproc)
{

 SRVRETCODE ret(FAIL);
 LPVOID calc = CreateCalculator();
 TMSSrvProc sp(srvproc);
 if(sp.ReturnInteger(int(calc)))
  {
   ret = SUCCEED;
  }

  return ret;
}

extern "C" SRVRETCODE msxp_FreeCalculator(SRV_PROC * srvproc)
{
 SRVRETCODE ret(FAIL);
 TMSSrvProc sp(srvproc);
 int param(0);
 if(sp.GetIntegerParam(1,&param,0))
 {
 
 __try
 {
 LPVOID calc = (LPVOID)param;
 if(calc)
    {
     //
	 DestroyCalculator(calc); ret = SUCCEED;
    }
 
 } 
  __except( EXCEPTION_EXECUTE_HANDLER)
 {
   ret = FAIL;
 }

 }

return ret;
}

extern "C" SRVRETCODE msxp_AddCalculatorVariable(SRV_PROC * srvproc)
{
 SRVRETCODE ret(FAIL);
 __try
 {
 TMSSrvProc sp(srvproc);
 int param(0);
 if(sp.GetIntegerParam(1,&param,0))
 {
 LPVOID calc = (LPVOID)param;
 if(calc)
    {
     char buffer [512]={0};
     if(sp.GetStringParam(2,buffer,sizeof(buffer),0))
       {
        AddVariableFromString(calc,buffer);
        ret = SUCCEED;
       }
    }
  }
 }
 __except( EXCEPTION_EXECUTE_HANDLER)
 {
	 ret = FAIL;
 }
 return ret;
}

extern "C" SRVRETCODE msxp_CalcFormula(SRV_PROC * srvproc)
{
	
 SRVRETCODE ret(FAIL);
 TMSSrvProc sp(srvproc);
 char buffer [1024]={0};

 __try{
 int param(0);
 if(sp.GetIntegerParam(2,&param,0))
 {
 LPVOID calc = (LPVOID)param;
 if(calc)
    {
     if(sp.GetStringParam(3,buffer,sizeof(buffer),0))
       {
        //char buffer1 [1024]={0};
        //wsprintf(buffer1,"Calc addr %X : Formula %s ",param,buffer);

        long double result = CalcFormula(calc,buffer);
        if(result || GetCalcError(calc)!=CALC_ERR_INVALID_OBJECT)
        {
         sprintf(buffer,"%.20LG",result);
         //wsprintf(buffer1+lstrlen(buffer1), " Result = %s ",buffer);
         //MessageBox(0,buffer1,"",MB_SERVICE_NOTIFICATION);
         if(sp.ReturnString(buffer,1))
         ret = SUCCEED;
         /*else
         MessageBox(0,"Return result Error","",MB_SERVICE_NOTIFICATION);*/
        }
       }
    }
  }
 }
 __except( EXCEPTION_EXECUTE_HANDLER)
 {
	 ret = FAIL;
 }
 return ret;
}

extern "C" SRVRETCODE msxp_CalcClearVariables(SRV_PROC * srvproc)
{
 SRVRETCODE ret(FAIL);
 TMSSrvProc sp(srvproc);
 int param(0);
 if(sp.GetIntegerParam(1,&param,0))
 {
 LPVOID calc = (LPVOID)param;
 if(calc)
  {
   ClearVariables(calc);
   ret = SUCCEED;
  }
 }
 return ret;
}

extern "C" SRVRETCODE msxp_CalcGetError(SRV_PROC * srvproc)
{
 SRVRETCODE ret(FAIL);
 TMSSrvProc sp(srvproc);
 int param(0);
 if(sp.GetIntegerParam(2,&param,0))
 {
 LPVOID calc = (LPVOID)param;
 if(calc )
   {
    sp.ReturnInteger(GetCalcError(calc));
    ret = SUCCEED;
   }

 }
 return ret;
}

extern "C" SRVRETCODE msxp_CalcSetVariable(SRV_PROC * srvproc)
{
 SRVRETCODE ret(FAIL);
 TMSSrvProc sp(srvproc);
 int param(0);
 if(sp.GetIntegerParam(1,&param,0))
 {
 LPVOID calc = (LPVOID)param;
 if(calc)
 { 
   int   idx;
   char   str[1024];
   sp.GetIntegerParam(2,&idx,0);
   sp.GetStringParam(3,str,sizeof(str),0);
   SetVariableFromString(calc,idx,str);
   ret = SUCCEED;
 }
 }
 return ret;
}

