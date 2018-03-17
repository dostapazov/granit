#ifndef _HDLC_UNIT_INC
 #define _HDLC_UNIT_INC


#ifdef __FLAT__
 #ifndef _Windows_
 #define STRICT
 #include <Windows.h>
 #endif
#else
#define WINAPI
#endif

#ifdef __cplusplus
extern "C"{
#endif

//returns how many bytes need for insert bitstaffing
unsigned short  WINAPI NeedBytes(unsigned short len);
//Insert Bit Staffings and Two Flags
unsigned short  WINAPI InsertBitStaffings(char * dest,unsigned short destlen,char * src,
                                  unsigned short srclen,char BeginFlag,char EndFlag);
unsigned short  WINAPI RemoveBitStaffings(char * dest,char * src,unsigned short len);
unsigned short  WINAPI BitStaffingsCount (char * src,unsigned short len);
unsigned short  WINAPI MirrorBytes(char * buf,unsigned short len);

BOOL WINAPI Bytes2HDLC(LPBYTE dest,DWORD destSz,LPBYTE src,DWORD srcSz,
                       DWORD BeginFlag,DWORD EndFlag,LPDWORD ResCount,
                       DWORD lim );

BOOL WINAPI HDLC2Bytes(LPBYTE dest,DWORD destSz,LPBYTE src,DWORD srcSz,
                       LPDWORD ResCount,DWORD lim );

#ifdef __cplusplus
}
#endif

#endif


