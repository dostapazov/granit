#include <tregstry.hpp>

#ifndef _NO_NAMESPACE
namespace KeRTL
{
#endif



    bool __fastcall TRegsReader::GetValueType(const char * ValueName,LPDWORD Type,LPDWORD sz ,bool root )
    {
      bool ret(false);
      if(IsKeyOpen() || (root && RootKey))
         ret = bool(ERROR_SUCCESS == RegQueryValueExA(root ? RootKey : CurrentKey,ValueName,0,Type,0,sz));
       return ret;
    }

    bool __fastcall TRegsReader::ReadValue(const char * ValName,LPDWORD Type,LPVOID data,LPDWORD sz ,bool root )
    {
      bool ret(false);
      if(IsKeyOpen() || (root && RootKey))
         ret = bool(ERROR_SUCCESS == RegQueryValueExA(root ? RootKey : CurrentKey,ValName,0,Type,(LPBYTE)data,sz));
       return ret;
    }

  int   __fastcall TRegsReader::ReadString(const char * ValName,char * buf,int bsz,bool Root)
  {
    DWORD _sz = bsz;
    DWORD type;
    if(ReadValue(ValName,&type,buf,&_sz,Root) && type == REG_SZ)
       return int(_sz);
    return 0;
  }



  DWORD __fastcall TRegsReader::ReadDword (const char * ValName,DWORD Default,bool Root)
  {
   DWORD Val(Default);
   DWORD sz = sizeof(Val);
   DWORD type;

   if(!ReadValue(ValName,&type,&Val,&sz,Root) || type!=REG_DWORD)
      Val = Default;
   return Val;
  }

  int  __fastcall  TRegsReader::ReadBytes (const char * ValName,LPVOID buf,int bsz,bool Root)
  {
    DWORD _sz = bsz;
    DWORD type;
    if(ReadValue(ValName,&type,buf,&_sz,Root) && (type == REG_SZ || type == REG_BINARY))
       return int(_sz);
    return 0;
  }


#ifndef _NO_NAMESPACE
//namespace KeRTL
}
#endif
