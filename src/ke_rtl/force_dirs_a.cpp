#include <kertl.hpp>
#include <stdlib.h>
#include <io.h>

#ifndef _NO_NAMESCPACE
namespace KeRTL
{
#endif
bool __fastcall __kertlForceDirs( char * target,LPSECURITY_ATTRIBUTES sa)
{
   char drive[MAX_PATH];
   char dir  [MAX_PATH];
   char name [MAX_PATH];
   char dst  [MAX_PATH];
  if(_access(target,0))
  {
   _splitpath(target,drive,dir,name,0);
   int l = lstrlenA(dir);
   if(l--)
    {
    *(dir+l)=0;
     _makepath(dst,drive,0,dir,0);
     if(_access(dst,0))
     {
        if(ForceDirs(dst,sa))
         {
          _makepath(dst,0,dst,name,0);
          return CreateDirectoryA(dst,sa) ? true:false;
         }
     }
     else
     {
      _makepath(dst,0,dst,name,0);
      return CreateDirectoryA(dst,sa) ? true:false;
      }
    }
    return CreateDirectoryA(name,sa) ? true:false;;

  }
  return true;
}

bool __fastcall ForceDirs( char * target,LPSECURITY_ATTRIBUTES sa)
{
 if(!__kertlForceDirs(target,sa))
   return GetLastError()==ERROR_ALREADY_EXISTS ? true:false;
   return true;
}


bool       __fastcall ClearDir      (char * dir_name,char * wild_card)
{
   if(dir_name && *dir_name)
   {
   WIN32_FIND_DATA fd;
   ZeroMemory(&fd,sizeof(fd));
   char folder[MAX_PATH<<2];
   lstrcpyA(folder,dir_name);
   int folder_len = lstrlen(folder);
   if(folder[folder_len-1]!='\\')
      {
       folder[folder_len++]='\\';
       folder[folder_len]=0;
      }

   lstrcat(folder,(wild_card && *wild_card) ? wild_card :"*.*");
   HANDLE find_handle = FindFirstFile(folder,&fd);
   if(find_handle && find_handle!=INVALID_HANDLE_VALUE)
   {
    //Óהאכÿול גסו פאיכû
    do{
       if(*fd.cFileName == '.')
          continue;
       lstrcpy(folder+folder_len,fd.cFileName);
       DeleteFile(folder);
      }while(FindNextFile(find_handle,&fd));
    FindClose(find_handle);
    }
    return true;
   }
   return false;
}

bool       __fastcall ForceDeleteDir(char * dir_name)
{

   if(dir_name && *dir_name)
   {
   char folder[MAX_PATH<<2];
   lstrcpyA(folder,dir_name);
   int folder_len = lstrlen(folder);
   if(folder[folder_len-1]!='\\')
      {
       folder[folder_len++]='\\';
       folder[folder_len]=0;
      }
   if(ClearDir(folder,(char*)"*.*"))
    return RemoveDirectory(dir_name) ? true:false;
   }
   return false;

}



#ifndef _NO_NAMESCPACE
}
#endif


 
