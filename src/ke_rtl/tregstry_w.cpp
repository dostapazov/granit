#include <tregstry.hpp>

#ifndef _NO_NAMESPACE
namespace KeRTL
{
#endif

     bool    __fastcall TRegsBase::OpenKey(const wchar_t * SubKey,bool)
     {
      if(RootKey && SubKey )
       {
         if(IsKeyOpen())
         {
          RegCloseKey(RootKey);
          RootKey  = CurrentKey;
         }
          if(ERROR_SUCCESS!=RegOpenKeyExW(RootKey,SubKey,0,Access,&CurrentKey))
             CurrentKey = 0;


       }

      return bool (CurrentKey!=0);
     }


  bool    __fastcall TRegsBase::GetClassName(wchar_t * Buff,LPDWORD cbSize,bool root)
  {
       bool ret(false);
       if(IsKeyOpen() || (root && RootKey))
         ret = bool (ERROR_SUCCESS == RegQueryInfoKeyW(root ? RootKey : CurrentKey ,Buff,cbSize,0,0,0,0,0,0,0,0,0));
       return ret;

  }

  bool  __fastcall TRegsBase::EnumKeys(int idx, wchar_t * NameBuff,LPDWORD nsz,bool root,wchar_t * ClassName ,LPDWORD csz )
  {
    bool ret(false);
    if(IsKeyOpen() || (root && RootKey))
       ret = bool (ERROR_SUCCESS == RegEnumKeyExW(root ? RootKey : CurrentKey ,idx,NameBuff,nsz,0,ClassName,csz,0));
    return ret;
  }

  bool  __fastcall TRegsBase::EnumValues(int idx,wchar_t * NameBuf,LPDWORD bsz,LPDWORD type,bool root ,LPVOID data,LPDWORD dtsz)
  {
    bool ret(false);
    if(IsKeyOpen() || (root && RootKey))
       ret = bool (ERROR_SUCCESS == RegEnumValueW(root ? RootKey : CurrentKey ,idx,NameBuf,bsz,0,type,(LPBYTE)data,dtsz));
    return ret;
  }



#ifndef _NO_NAMESPACE
}
#endif
