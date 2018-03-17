
#include <tregstry.hpp>

#ifndef _NO_NAMESPACE
namespace KeRTL
{
#endif

     bool    __fastcall TRegsBase::OpenKey(const char * SubKey,bool)
     {
      if(RootKey && SubKey )
       {
         if(IsKeyOpen())
         {
          RegCloseKey(RootKey);
          RootKey  = CurrentKey;
         }
          if(ERROR_SUCCESS!=RegOpenKeyExA(RootKey,SubKey,0,Access,&CurrentKey))
             CurrentKey = 0;


       }

      return bool (CurrentKey!=0);
     }


  bool    __fastcall TRegsBase::GetClassName(char * Buff,LPDWORD cbSize,bool root)
  {
       bool ret(false);
       if(IsKeyOpen() || (root && RootKey))
         ret = bool (ERROR_SUCCESS == RegQueryInfoKeyA(root ? RootKey : CurrentKey ,Buff,cbSize,0,0,0,0,0,0,0,0,0));
       return ret;

  }

  bool  __fastcall TRegsBase::EnumKeys(int idx,char * NameBuff,LPDWORD nsz,bool root,char * ClassName ,LPDWORD csz )
  {
    bool ret(false);
    if(IsKeyOpen() || (root && RootKey))
       ret = bool (ERROR_SUCCESS == RegEnumKeyExA(root ? RootKey : CurrentKey ,idx,NameBuff,nsz,0,ClassName,csz,0));
    return ret;
  }

  bool  __fastcall TRegsBase::EnumValues(int idx,char * NameBuf,LPDWORD bsz,LPDWORD type,bool root ,LPVOID data,LPDWORD dtsz)
  {
    bool ret(false);
    if(IsKeyOpen() || (root && RootKey))
       ret = bool (ERROR_SUCCESS == RegEnumValueA(root ? RootKey : CurrentKey ,idx,NameBuf,bsz,0,type,(LPBYTE)data,dtsz));
    return ret;
  }



#ifndef _NO_NAMESPACE
}
#endif
