#include <gkmenu.hpp>

#ifndef _NO_NAMESPACE
namespace KrnlWin
{
#endif

                       TMenu::TMenu()
                       {hMenu = 0;ShouldDel = true;}

                       TMenu::TMenu(HMENU menu,bool ShouldDel )
                       {hMenu = menu; this->ShouldDel = ShouldDel ;}

                       TMenu::TMenu(LPCWSTR ResId,HINSTANCE from ,bool sd)
                       {
                        LoadFromResource(ResId,from);
                        ShouldDel = sd;
                       }
                       TMenu::TMenu(LPCSTR ResId,HINSTANCE from ,bool sd)
                       {
                        LoadFromResource(ResId,from);
                        ShouldDel = sd;
                       }

                       TMenu::TMenu(HWND Window,bool System,bool sd)
                       {
                         hMenu = System ? GetSystemMenu((::HWND)Window,false) : GetMenu((::HWND)Window);
                         ShouldDel = sd;
                       }

                       TMenu::~TMenu()
                       {
                        if(ShouldDel)
                           Destroy();
                       }

              bool     __fastcall TMenu::Destroy()
              {
				  return DestroyMenu((::HMENU)hMenu)? true:false;
              }

#ifdef __BORLANDC__
     #pragma warn -8057
#endif

              bool     __fastcall TMenu::CopyFrom (HMENU mnu)
              {
               //TODO - Реализовать
               return true;
              }
#ifdef __BORLANDC__
              #pragma warn .8057
#endif


              bool     __fastcall TMenu::MergeFrom(HMENU mnu)
              {
                MENUITEMINFO mi;
                TMenu menu(mnu,false);
                int Count = menu.GetItemCount() ,DstCount = GetItemCount();
                ZeroMemory(&mi,sizeof(mi));
                #if WINVER > 0x0500
                mi.cbSize = sizeof(mi) - sizeof(mi.cbSize);
                #else
                mi.cbSize = sizeof(mi) - sizeof(mi.cbSize);
                #endif
                TCHAR  ItemText [256] ;
                for ( int i = 0 ;i < Count;i++)
                {
                     mi.fMask = MIIM_TYPE|MIIM_DATA|MIIM_ID|MIIM_STATE|MIIM_SUBMENU|MIIM_CHECKMARKS;
                     mi.dwTypeData = ItemText;
                     mi.cch        = KERTL_ARRAY_COUNT(ItemText);
                    if(menu.GetItemInfo(i,true,&mi))
                    {
                     this->InsertItem(i+DstCount,true,&mi);
                    }
                    else
                    {
                     GetLastError();
                     return false;
                    }
                }
                return true;
              }

              bool     __fastcall TMenu::LoadFromResource(LPCSTR ResId,HINSTANCE from )
              {
               hMenu = (HMENU)LoadMenuA((::HINSTANCE)(from ? from :GetModuleHandle(0)),ResId);
               return bool(hMenu!=0);
              }

              bool     __fastcall TMenu::LoadFromResource(LPCWSTR ResId,HINSTANCE from )
              {
               hMenu = (HMENU)LoadMenuW((::HINSTANCE)(from ? from :GetModuleHandle(0)),ResId);
               return bool(hMenu!=0);
              }

              bool     __fastcall TMenu::GetItemInfo(int i,bool ByPos , LPMENUITEMINFO mi)
              {
		return GetMenuItemInfo((::HMENU)hMenu,i,ByPos,mi)?true:false;
              }

              bool     __fastcall TMenu::SetItemInfo(int i,bool ByPos , LPMENUITEMINFO mi)
              {
               return SetMenuItemInfo((::HMENU)hMenu,i,ByPos,mi)?true:false;
              }

              bool     __fastcall TMenu::InsertItem (int i,bool ByPos , LPMENUITEMINFO mi)
              {
               return InsertMenuItem((::HMENU)hMenu,i,ByPos,mi)?true:false;
              }

              int      __fastcall TMenu::GetItemCount()
              {return GetMenuItemCount((::HMENU)hMenu);}

              bool     __fastcall TMenu::EnableMenuItem (int i, bool byPos )
              {
               /*MENUITEMINFO mi;
               ZeroMemory(&mi,sizeof(mi));
               mi.cbSize = sizeof(mi);
               mi.fMask  = MIIM_STATE;
			   mi.wID = i;
               if( GetItemInfo(i,byPos,&mi) && (mi.fState&MFS_ENABLED) )
                 {
                   mi.fState|=MFS_DISABLED|MFS_GRAYED;
                   mi.fState&=~MFS_ENABLED;
                   return SetItemInfo(i,byPos,&mi);
                 }
                 */
		return ::EnableMenuItem((::HMENU)(*this)(),i,MF_ENABLED|(byPos ? MF_BYPOSITION : MF_BYCOMMAND)) ? true:false;
              }

              bool     __fastcall TMenu::DisableMenuItem(int i, bool byPos )
              {
			  /*
               MENUITEMINFO mi;
               ZeroMemory(&mi,sizeof(mi));
               mi.cbSize = sizeof(mi);
               mi.fMask  = MIIM_STATE;
			   mi.wID = i;
               if( GetItemInfo(i,byPos,&mi) && (mi.fState&MFS_DISABLED) )
                 {
                   mi.fState|=MFS_ENABLED;
                   mi.fState&=~(MFS_DISABLED|MFS_GRAYED);
                   return SetItemInfo(i,byPos,&mi);
                 }
                return false;
				*/
				  return ::EnableMenuItem((::HMENU)(*this)(),i,MF_DISABLED|MF_GRAYED|(byPos ? MF_BYPOSITION : MF_BYCOMMAND))? true:false;
              }



            TPopupMenu::TPopupMenu(HMENU mnu)
             {
              hMenu = CreatePopupMenu();
              this->MergeFrom(mnu);
             }

            TPopupMenu::TPopupMenu(LPCSTR  resId,HINSTANCE from)
             {
              TMenu menu(resId,from,false);
              hMenu = GetSubMenu((::HMENU)menu(),0);
             }

            TPopupMenu::TPopupMenu(LPCWSTR  resId,HINSTANCE from)
             {
              TMenu menu(resId,from,false);
              hMenu = GetSubMenu((::HMENU)menu(),0);
             }

             bool       __fastcall TPopupMenu::Track(POINT & Where,int flags,HWND Owner,LPRECT rect)
             {
              return TrackPopupMenu((::HMENU)hMenu,flags,Where.x,Where.y,0,(::HWND)Owner,rect)?true:false;
             }

             bool       __fastcall TPopupMenu::TrackEx(POINT &Where,int flags,HWND Owner,LPTPMPARAMS params)
             {
              return TrackPopupMenuEx((::HMENU)hMenu,flags,Where.x,Where.y,(::HWND)Owner,params)?true:false;
             }


#ifndef _NO_NAMESPACE
}
#endif

