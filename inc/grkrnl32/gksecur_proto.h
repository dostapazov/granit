#ifndef __GKSECUR_PROTO_INC__
#define __GKSECUR_PROTO_INC__

#pragma once
#ifdef __linux__
#include <gklib/otd_base.h>
#include <gklib/otdm_proto.h>
#else
#include <otd_base.h>
#include <otdm_proto.h>
#endif


typedef QWORD SESSION_ID;

//Протокол безопасности начинается с заголова OTDM_PROTO_HEADER

//Команда
#define GKSECUR_PROTO_CMD_LOGIN   1
//OTDM_PROTO_HEADER::data
//1 SESSION_ID * session_id;
//2 lpotd_text  user_name,
//3 lpotd_text  password
//4 kpk
//  Ответ с флагом OTDM_RESPOND
//  SESSION_ID

#ifdef __cplusplus
extern "C"{
#endif

void  WINAPI gksecur_coder              (void * data,unsigned int sz,void    * key,unsigned int key_size);
DWORD WINAPI gksecur_login_format       (void * data,unsigned int sz,SESSION_ID ss,wchar_t * user_name,wchar_t * user_password);
DWORD WINAPI gksecur_login_extract      (LPOTDM_PROTO_HEADER omph,SESSION_ID * ssid,wchar_t * user_name,DWORD user_sz,wchar_t * passwe,DWORD passw_sz);

#ifdef __cplusplus
 } /*extern "C"{*/
#endif


#endif


