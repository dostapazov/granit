/*
  Функции преобразования протоколов
  ATONY     -> otd_proto
  otd_proto -> atony

*/

#ifndef _OTD_GRANIT_CONVERT_H
#define _OTD_GRANIT_CONVERT_H

#include <otd_proto.h>
#include <granit_dos.h>

#ifdef __cplusplus
extern "C"{
#endif

int WINAPI otd_convert_atony2otd(DWORD num_pu,lp_gratony_kadr src_kadr,DWORD src_diag,LPBYTE dest_buffer,DWORD dest_bsz,BOOL protect, lpotd_proto op);
int WINAPI otd_convert_otd2atony(lpotd_proto src,LPDWORD cmd,LPBYTE dest_buffer,DWORD dest_bsz);


#ifdef __cplusplus
//extern "C"{
}
#endif


#endif


 