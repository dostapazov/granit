#include <kertl.hpp>
#include <tregstry.hpp>

#ifndef _NO_NAMESPACE
namespace KeRTL
{
#endif

  bool    __fastcall TRegsWriter::OpenKey(const wchar_t * SubKey,bool CanCreate )
   {
    if(!TRegsBase::OpenKey(SubKey) && CanCreate)
         CreateKey(SubKey);
     return bool(CurrentKey!=0);
   }

      bool    __fastcall TRegsWriter::CreateKey(const wchar_t * SubKey)
      {
        if(IsKeyOpen())
        {
         RegCloseKey(RootKey);
         RootKey = CurrentKey;
        }
        if(ERROR_SUCCESS!=RegCreateKeyW(RootKey,SubKey,&CurrentKey))
           CurrentKey = 0;
           return bool(CurrentKey!=0);

      }

  bool    __fastcall TRegsWriter::WriteValue (const wchar_t * ValName,DWORD Type,LPVOID data,DWORD sz,bool root)
  {
    bool ret(false);
      if(IsKeyOpen() || (root && RootKey))
        ret = bool(ERROR_SUCCESS == RegSetValueExW(root ? RootKey : CurrentKey,ValName,0,Type,(LPBYTE)data,sz));
      return ret;
  }

  bool    __fastcall TRegsWriter::WriteString(const wchar_t * ValName,const wchar_t * val,bool root )
  {
  if(val)
   return WriteValue(ValName,REG_SZ,(LPVOID)val,(lstrlenW(val)+1)*sizeof(wchar_t),root);
  return false;
  }


  bool    __fastcall TRegsWriter::WriteDword (const wchar_t * ValName,DWORD  val,bool root )
  {
   return WriteValue(ValName,REG_DWORD,&val,sizeof(val),root);
  }

  bool    __fastcall TRegsWriter::WriteBytes (const wchar_t * ValName,LPVOID val,DWORD sz,bool root)
  {
     return WriteValue(ValName,REG_BINARY,val,sz,root);
  }

  bool    __fastcall TRegsWriter::DeleteValue(const wchar_t * ValName,bool root )
  {
   if(IsKeyOpen() || (root && RootKey))
     return bool(ERROR_SUCCESS == RegDeleteValueW(root ? RootKey : CurrentKey,ValName));
    return false;
  }

  bool    __fastcall TRegsWriter::DeleteKey  (const wchar_t * KeyName,bool root )
  {
    HKEY save_key = CurrentKey;
    bool ret = false;
    if(CurrentKey == NULL)
     {
       CurrentKey = RootKey;
     }

    if(IsKeyOpen() || (root && RootKey))
    {
      TRegsWriter rgrw(DupKey(root ? RootKey : CurrentKey));
      if(root || rgrw.OpenKey(KeyName))
      {

         wchar_t key_name  [MAX_PATH<<1];
         DWORD nsz = sizeof(key_name)/sizeof(wchar_t);
         while(rgrw.EnumKeys(0,key_name,&nsz,root))
         {
           rgrw.DeleteKey(key_name,root);
           nsz = sizeof(key_name)/sizeof(wchar_t);
         }
      }
     ret = RegDeleteKeyW(root ? RootKey : CurrentKey ,KeyName)==ERROR_SUCCESS ? true:false;
     CurrentKey = save_key;
    }
    return ret;
  }

#ifndef _NO_NAMESPACE
}
#endif

