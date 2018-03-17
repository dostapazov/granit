#ifndef grkrmenuH
#define grkrmenuH

#include <ke_defs.h>
#include <gkwind.hpp>

#ifndef _Windows_
  
 #ifndef STRICT
  #define STRICT
 #endif

 #ifndef _WINDOWS_
  #define WIN32_LEAN_AND_MEAN
  #include <Windows.h>
  #undef WIN32_LEAN_AND_MEAN
 #endif

#endif

#ifndef _NO_NAMESPACE
#ifndef KrnlWin
#define KrnlWin KeWin
#endif
  namespace KeWin
  {
#endif
     //Базовый класс для ресурсов
     KEWIN_CLASS TResource:public KeRTL::TKeRTLBase
     {
       protected:
       HRSRC    rsrc;
       HGLOBAL  resource;
       public:
       TResource():rsrc(0),resource(0){}
       bool  LoadResource(HINSTANCE module,char * type,char * name, WORD wLang = LANG_NEUTRAL);
       bool  LoadResource(HINSTANCE module,wchar_t * type,wchar_t * name, WORD wLang = LANG_NEUTRAL);
       void   __fastcall Release(){}
       LPVOID operator()(){return LockResource(resource);}
     };

inline bool  TResource::LoadResource(HINSTANCE module,char * type,char * name, WORD wLang)
{
 rsrc = FindResourceExA((::HINSTANCE)module,type,name,wLang);
 resource = (rsrc) ? ::LoadResource((::HINSTANCE)module,rsrc):NULL;
 return resource ? true:false;
}

inline bool  TResource::LoadResource(HINSTANCE module,wchar_t * type,wchar_t * name, WORD wLang )
{
 rsrc = FindResourceExW((::HINSTANCE)module,type,name,wLang);
 resource = (rsrc) ? ::LoadResource((::HINSTANCE)module,rsrc):NULL;
 return resource ? true:false;
}


     KEWIN_CLASS TMenu
     {
      protected:
      HMENU hMenu;
      bool  ShouldDel;
      public:
                       TMenu();
                       TMenu(HMENU,bool ShouldDel = true);
                       TMenu(LPCSTR ResId,HINSTANCE from = 0,bool ShouldDel = true);
                       TMenu(HWND Window,bool System,bool ShouldDel=true);
      virtual          ~TMenu();
              bool     __fastcall Destroy();
              bool     __fastcall CopyFrom (HMENU mnu);
              bool     __fastcall MergeFrom(HMENU mnu);
              bool     __fastcall LoadFromResource(LPCSTR ResId,HINSTANCE from = 0);
              bool     __fastcall GetItemInfo(int i,bool ByPos ,LPMENUITEMINFO mi);
              bool     __fastcall SetItemInfo(int i,bool ByPos ,LPMENUITEMINFO mi);
              bool     __fastcall InsertItem(int i,bool ByPos  ,LPMENUITEMINFO);
              int      __fastcall GetItemCount();
              bool     __fastcall EnableMenuItem (int i, bool byPos = false);
              bool     __fastcall DisableMenuItem(int i, bool byPos = false);
      HMENU operator()();
     };

     KEWIN_CLASS TPopupMenu :public TMenu
     {
      public:
             TPopupMenu():TMenu(){}
             TPopupMenu(HMENU );
             TPopupMenu(LPCSTR  resId,HINSTANCE = 0);
             bool       __fastcall Track(POINT & Where,int flags,HWND Owner,LPRECT =0);
             bool       __fastcall TrackEx(POINT &Where,int flags,HWND Owner,LPTPMPARAMS=0);
     };


     inline  HMENU TMenu::operator()()
     {return hMenu;}

#ifndef _NO_NAMESPACE
  }
#endif

#endif

