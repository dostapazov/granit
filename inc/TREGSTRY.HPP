#ifndef _TREGISTRY_HPP_INC
#define _TREGISTRY_HPP_INC

#include <ke_defs.h>

#ifndef _Windows_
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef  WIN32_LEAN_AND_MEAN
#endif

#ifndef _WINREG_
 #include <winreg.h>
#endif

#ifndef _INC_REGSTR
#include <regstr.h>
#endif


#pragma pack (push,8)

#ifndef _NO_NAMESPACE
namespace KeRTL
{
#endif

  KERTL_CLASS TRegsBase
  {
    protected:
    HKEY  RootKey;
    HKEY  CurrentKey;
    DWORD Access;
    public:
    enum TRootKey{rtNone         = 0,
                  rtClasses      = int(HKEY_CLASSES_ROOT),
                  rtCurrUser     = int(HKEY_CURRENT_USER),
                  rtLocalMachine = int(HKEY_LOCAL_MACHINE),
                  rtUsers        = int(HKEY_USERS),
                  rtPerf         = int(HKEY_PERFORMANCE_DATA),
                  rtCurrConfig   = int(HKEY_CURRENT_CONFIG),
                  rtDynData      = int(HKEY_DYN_DATA)
                 };

                 TRegsBase(DWORD Acc = KEY_ALL_ACCESS);
                 TRegsBase(HKEY root,const char  * SubKey=0,DWORD Acc = KEY_ALL_ACCESS);
                 TRegsBase(HKEY root,const wchar_t * SubKey,DWORD Acc = KEY_ALL_ACCESS);
     virtual    ~TRegsBase();
     void    __fastcall SetRootKey(HKEY,bool CloseCurrKey = false);
     HKEY    __fastcall GetRootKey();
     bool    __fastcall OpenKey(const char * SubKey,bool CanCreate = false);
     bool    __fastcall OpenKey(const wchar_t* SubKey,bool CanCreate = false);
     bool    __fastcall CloseKey  (bool root = false);
     bool    __fastcall IsKeyOpen ();
     bool    __fastcall IsRootOnly();
     bool    __fastcall GetSubKeysCount(LPDWORD kc, LPDWORD kl,bool root = false);
     bool    __fastcall GetValuesCount (LPDWORD vc, LPDWORD vnl,LPDWORD vl,bool root = false);
     bool    __fastcall GetLastAccessTime(FILETIME & ft,bool root = false);
     bool    __fastcall GetClassName(char * Buff,LPDWORD cbSize,bool root = false);
     bool    __fastcall GetClassName(wchar_t * Buff,LPDWORD cbSize,bool root = false);
     bool    __fastcall Flush();
     bool    __fastcall EnumKeys(int idx,char * NameBuf,LPDWORD nsz,bool root = false,char * ClassName = 0,LPDWORD csz = 0);
     bool    __fastcall EnumKeys(int idx,wchar_t * NameBuf,LPDWORD nsz,bool root = false,wchar_t * ClassName = 0,LPDWORD csz = 0);
     bool    __fastcall EnumValues(int idx,char * NameBuf,LPDWORD bsz,LPDWORD type,bool root = false,LPVOID data=0,LPDWORD dtsz=0);
     bool    __fastcall EnumValues(int idx,wchar_t * NameBuf,LPDWORD bsz,LPDWORD type,bool root = false,LPVOID data=0,LPDWORD dtsz=0);
     HKEY    operator()(){return CurrentKey;}
  };

  KERTL_CLASS TRegsReader: virtual public TRegsBase
  {
   public:
        TRegsReader():TRegsBase(){};
        TRegsReader(HKEY root,char * SubKey = 0):TRegsBase(root,SubKey){};
        TRegsReader(HKEY root,wchar_t * SubKey ):TRegsBase(root,SubKey){};

        bool __fastcall GetValueType(const char * ValueName,LPDWORD Type,LPDWORD sz = 0,bool root = false);
        bool __fastcall ReadValue   (const char * ValName,LPDWORD Type,LPVOID data,LPDWORD dtSz = 0,bool root = false);
        bool __fastcall GetValueType(const wchar_t * ValueName,LPDWORD Type,LPDWORD sz = 0,bool root = false);
        bool __fastcall ReadValue   (const wchar_t * ValName,LPDWORD Type,LPVOID data,LPDWORD dtSz = 0,bool root = false);

        int   __fastcall ReadString (const char * ValName,char * buf,int bsz,bool root = false);
        DWORD __fastcall ReadDword  (const char * ValName,DWORD Default=0,bool root = false);
        int   __fastcall ReadBytes  (const char * ValName,LPVOID buf,int bsz,bool root = false);

        int   __fastcall ReadString (const wchar_t * ValName,wchar_t * buf,int bsz,bool root = false);
        DWORD __fastcall ReadDword  (const wchar_t * ValName,DWORD Default=0,bool root = false);
        int   __fastcall ReadBytes  (const wchar_t * ValName,LPVOID buf,int bsz,bool root = false);
  };

  KERTL_CLASS TRegsWriter: virtual public TRegsBase
  {
    public:
      TRegsWriter():TRegsBase(){};
      TRegsWriter(HKEY root,char * SubKey = 0):TRegsBase(root,SubKey){};
      TRegsWriter(HKEY root,wchar_t * SubKey ):TRegsBase(root,SubKey){};
      bool    __fastcall OpenKey(const char * SubKey,bool CanCreate = false);
      bool    __fastcall OpenKey(const wchar_t * SubKey,bool CanCreate = false);
      bool    __fastcall CreateKey   (const char * SubKey);
      bool    __fastcall WriteValue  (const char * ValName,DWORD Type,LPVOID data,DWORD sz,bool root = false);
      bool    __fastcall WriteString (const char * ValName,const char    * val,bool root = false);
      bool    __fastcall WriteDword  (const char * ValName,DWORD  val,bool root = false);
      bool    __fastcall WriteBytes  (const char * ValName,LPVOID val,DWORD sz,bool root = false);
      bool    __fastcall DeleteValue (const char * ValName,bool root = false);
      bool    __fastcall DeleteKey   (const char * ValName,bool root = false);

      bool    __fastcall CreateKey   (const wchar_t * SubKey);
      bool    __fastcall WriteValue  (const wchar_t * ValName,DWORD Type,LPVOID data,DWORD sz,bool root = false);
      bool    __fastcall WriteString (const wchar_t * ValName,const wchar_t    * val,bool root = false);
      bool    __fastcall WriteDword  (const wchar_t * ValName,DWORD  val,bool root = false);
      bool    __fastcall WriteBytes  (const wchar_t * ValName,LPVOID val,DWORD sz,bool root = false);
      bool    __fastcall DeleteValue (const wchar_t * ValName,bool root = false);
      bool    __fastcall DeleteKey   (const wchar_t * ValName,bool root = false);

  };

#ifndef __BORLANDC__
#pragma  warning  (disable  : 4250 ) 
#endif

  KERTL_CLASS TRegsRdWr:public TRegsReader,public TRegsWriter
  {
    public:
    TRegsRdWr():TRegsBase(){};
    TRegsRdWr(HKEY root,char    * SubKey=0):TRegsBase(root,SubKey){};
    TRegsRdWr(HKEY root,wchar_t * SubKey):TRegsBase(root,SubKey){};
  };

inline           TRegsBase::TRegsBase(DWORD Acc )
{RootKey = (HKEY)rtNone;CurrentKey = 0;Access = Acc;}

inline           TRegsBase::TRegsBase(HKEY root,const char * SubKey,DWORD Acc )
{
 RootKey = (HKEY)root;CurrentKey = 0;Access = Acc;
 OpenKey(SubKey);
}

inline           TRegsBase::TRegsBase(HKEY root,const wchar_t * SubKey,DWORD Acc )
{
 RootKey = (HKEY)root;CurrentKey = 0;Access = Acc;
 OpenKey(SubKey);
}


inline  TRegsBase::~TRegsBase()
{
  CloseKey(true);
}

inline           bool    __fastcall TRegsBase::IsKeyOpen()
{return bool(CurrentKey!=0);}

inline           bool    __fastcall TRegsBase::IsRootOnly()
{ return bool(CurrentKey==0 && RootKey!=0);}

inline           void    __fastcall TRegsBase::SetRootKey(HKEY rk,bool Close)
{
 if(Close)
   CloseKey(true);
   CurrentKey = 0;
  RootKey  = (HKEY)rk;
}

inline  HKEY    __fastcall TRegsBase::GetRootKey()
{
 return RootKey;
}

inline  bool    __fastcall TRegsBase::Flush()
{
  return bool (ERROR_SUCCESS == RegFlushKey(RootKey));
}

#ifndef _NO_NAMESPACE
}
#endif

#pragma pack (pop)
#endif
