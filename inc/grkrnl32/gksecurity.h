#ifndef __GKSECURITY_INC__
#define __GKSECURITY_INC__

#pragma once

#pragma pack(push,1)

#define GKSECURITY_STR_SIZE  64
#define GKSECURITY_LSTR_SIZE 256

#define GKSECURITY_MOD_NAME   L"GKSECURITY"
#define GKSECURITY_WND_NAME   L"GKSECURITYWND"

#define DATA_FOLDER           L"GKSECUR_DATA\\"

#define CCFL_USERS            0x0001

typedef struct _user_entry
{
 DWORD    dwSize;
 DWORD    user_id;
 wchar_t  user_name    [GKSECURITY_STR_SIZE];
 wchar_t  user_password[GKSECURITY_STR_SIZE];
}USER_ENTRY,*LPUSER_ENTRY;


//Хранится в зашифрованном виде
//Шифруется по паролю пользователя
typedef struct _GKS_USER_INFO
{
 DWORD    dwSize;
 wchar_t  user_addr     [GKSECURITY_LSTR_SIZE];
 wchar_t  user_telno    [GKSECURITY_LSTR_SIZE];
 wchar_t  user_email    [GKSECURITY_LSTR_SIZE];
 wchar_t  user_descript [GKSECURITY_LSTR_SIZE];
} GKS_USER_INFO,*LPGKS_USER_INFO;


#define SCM_ENUM_USERS     MCMD(MT_SECURITY,1)
//p1 = LPUSER_ENTRY  p2 idx;

//Добавление пользователя
//p1 =  LPWSTR user_name,p2 = LPWSTR user_password
#define SCM_ADD_USER       MCMD(MT_SECURITY,2)

#define SCM_SET_USER_INFO  MCMD(MT_SECURITY,3)

#define SCM_GET_USER_INFO  MCMD(MT_SECURITY,4)

#define SCM_CHECK_RIGHTS   MCMD(MT_SECURITY,100)
// p1 = MODEM_ADDR p2 LPVOID


#pragma pack(pop);

#endif