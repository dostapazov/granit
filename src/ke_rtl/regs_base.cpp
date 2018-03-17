#include <tregstry.hpp>

#ifndef _NO_NAMESPACE
namespace KeRTL
{
#endif

     bool    __fastcall TRegsBase::CloseKey (bool root)
     {
      Flush();
      if(IsKeyOpen())
      {
       if(ERROR_SUCCESS == RegCloseKey(CurrentKey))
         CurrentKey = 0;
      }

      if(root)
       {
        RegCloseKey(RootKey);
        RootKey = 0;
       }

      return bool (CurrentKey==0);
     }

     bool     __fastcall TRegsBase::GetSubKeysCount( LPDWORD kc, LPDWORD kl, bool root)
     {
       //Возврат количества ключей и максимальной длины ключа
       bool ret(false);
       if(IsKeyOpen() || (root && RootKey))
         ret = bool (ERROR_SUCCESS == RegQueryInfoKeyW(root ? RootKey : CurrentKey ,0,0,0,kc,kl,0,0,0,0,0,0));
       return ret;
     }

     bool    __fastcall TRegsBase::GetValuesCount (LPDWORD vc, LPDWORD vnl,LPDWORD vl,bool root)
     {
       //Возврат количества значений и максимальной длины имени и длины самого значения
       bool ret(false);
       if(IsKeyOpen() || (root && RootKey))
         ret = bool (ERROR_SUCCESS == RegQueryInfoKeyW(root ? RootKey : CurrentKey ,0,0,0,0,0,0,vc,vnl,vl,0,0));
       return ret;
     }

     bool    __fastcall TRegsBase::GetLastAccessTime(FILETIME & ft,bool root)
     {
       bool ret(false);
       if(IsKeyOpen() || (root && RootKey))
         ret = bool (ERROR_SUCCESS == RegQueryInfoKeyW(root ? RootKey : CurrentKey ,0,0,0,0,0,0,0,0,0,0,&ft));
       return ret;
     }

#ifndef _NO_NAMESPACE
}
#endif
 
