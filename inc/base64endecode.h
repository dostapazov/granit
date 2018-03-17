#ifndef __BASE_64_ENDECODE__INC
#define __BASE_64_ENDECODE__INC

#ifndef _WINDOWS_
 #ifndef  WIN32_LEAND_AND_MEAN
  #define WIN32_LEAND_AND_MEAN
  #define WIN32_LEAND_AND_MEAN_DEFINED
 #endif
#include <Windows.h>

 #ifdef WIN32_LEAND_AND_MEAN_DEFINED
  #undef WIN32_LEAND_AND_MEAN_DEFINED
  #undef WIN32_LEAND_AND_MEAN
 #endif

#endif


#ifdef __cplusplus
extern "C"{
#endif

// одирование раскодирование в формате BASE64
//возвращают количество байт помещЄнных в OUT_BUFF

DWORD WINAPI Base64Encode(LPVOID in_buff,DWORD in_sz,LPVOID out_buff,DWORD out_buff_sz);
DWORD WINAPI Base64Decode(LPVOID in_buff,DWORD in_sz,LPVOID out_buff,DWORD out_buff_sz);

#ifdef __cplusplus
}
#endif


#endif

 