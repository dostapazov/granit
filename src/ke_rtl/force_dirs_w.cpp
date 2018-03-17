#include <kertl.hpp>
#include <stdlib.h>
#include <io.h>

#ifndef _NO_NAMESCPACE
namespace KeRTL
{
#endif

static bool __fastcall __kertlForceDirs( wchar_t * target,LPSECURITY_ATTRIBUTES sa)
{
   wchar_t drive[MAX_PATH];
   wchar_t dir  [MAX_PATH];
   wchar_t name [MAX_PATH];
   wchar_t dst  [MAX_PATH];
  if(_waccess(target,0))
  {
   _wsplitpath(target,drive,dir,name,0);
   int l = lstrlenW(dir);
   if(l--)
    {
    *(dir+l)=0;
     _wmakepath(dst,drive,0,dir,0);
     if(_waccess(dst,0))
     {
        if(ForceDirs(dst,sa))
         {
          _wmakepath(dst,0,dst,name,0);
          return CreateDirectoryW(dst,sa) ? true:false;
         }
     }
     else
     {
      _wmakepath(dst,0,dst,name,0);
      return CreateDirectoryW(dst,sa) ? true:false;
      }
    }
    return CreateDirectoryW(name,sa) ? true:false;;

  }
  return true;
}

bool __fastcall ForceDirs( wchar_t * target,LPSECURITY_ATTRIBUTES sa)
{
 if(!__kertlForceDirs(target,sa))
   return GetLastError()==ERROR_ALREADY_EXISTS ? true:false;
   return true;
}

bool       __fastcall ClearDir      (wchar_t * dir_name,wchar_t * wild_card)
{
   if(dir_name && *dir_name)
   {
   WIN32_FIND_DATAW fd;
   ZeroMemory(&fd,sizeof(fd));
   wchar_t folder[MAX_PATH<<2];
   lstrcpyW(folder,dir_name);
   int folder_len = lstrlenW(folder);
   if(folder[folder_len-1]!='\\')
      {
       folder[folder_len++]='\\';
       folder[folder_len]=0;
      }

   lstrcatW(folder,(wild_card && *wild_card) ? wild_card :L"*.*");
   HANDLE find_handle = FindFirstFileW(folder,&fd);
   if(find_handle && find_handle!=INVALID_HANDLE_VALUE)
   {
    //Óהאכÿול גסו פאיכû
    do{
       if(*fd.cFileName == '.')
          continue;
	   lstrcpyW(folder+folder_len,fd.cFileName);
	   DeleteFileW(folder);
	  }while(FindNextFileW(find_handle,&fd));
	FindClose(find_handle);
	}
	return true;
   }
   return false;
}

bool       __fastcall ForceDeleteDir(wchar_t * dir_name)
{

   if(dir_name && *dir_name)
   {
   wchar_t folder[MAX_PATH<<2];
   lstrcpyW(folder,dir_name);
   int folder_len = lstrlenW(folder);
   if(folder[folder_len-1]!='\\')
	  {
	   folder[folder_len++]='\\';
	   folder[folder_len]=0;
	  }
   if(ClearDir(folder,(wchar_t*)L"*.*"))
	return RemoveDirectoryW(dir_name) ? true:false;
   }
   return false;

}



#ifndef _NO_NAMESCPACE
}
#endif
