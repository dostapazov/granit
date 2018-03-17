#include <kertl.hpp>

#ifndef _NO_NAMESPACE
namespace KeRTL
{
#endif

KERTL_FUNC void *     __fastcall safe_memcpy(void * dest, const void * src,size_t sz)
{

    return   (dest && src && sz) ? memcpy(dest,src,sz) : dest;
}


KERTL_FUNC wchar_t *  __fastcall safe_strcpyn(wchar_t * dest,const wchar_t * src,size_t lim)
{
 if(dest && lim)
 {
  *dest = 0;
  if(src && *src)
    safe_memcpy(dest,src,sizeof(wchar_t)*KeRTL::MIN((size_t)lstrlenW(src)+1,lim));

 }
 return dest;
}

KERTL_FUNC char    *  __fastcall safe_strcpyn(char    * dest,const  char   * src,size_t lim)
{
 if(dest && lim)
 {
  *dest = 0;
  if(src && *src)
    safe_memcpy(dest,src,KeRTL::MIN((size_t)lstrlenA(src)+1,lim));

 }
 return dest;
}


KERTL_FUNC wchar_t *  __fastcall safe_strcpy(wchar_t * dest,const wchar_t * src)
{
 if(dest)
 {
  *dest = 0;
  if(src && *src)
    safe_memcpy(dest,src,sizeof(wchar_t)*(lstrlenW(src)+1));
 }
 return  dest;
}
KERTL_FUNC char    *  __fastcall safe_strcpy(char    * dest,const  char   * src)
{
 if(dest)
 {
  *dest = 0;
  if(src && *src)
    safe_memcpy(dest,src,lstrlenA(src)+1);
 }
 return  dest;

}



#ifndef _NO_NAMESPACE
}//End of namespace
#endif
 
