/*-----------------------------------------------------------------------------
 08.04.2004 14:29:17 
 Модуль:   gksecur
 Автор:    Остапенко Д.В.
 Назначение: Интерфейс механизма безопасности для GKHANDLE

-----------------------------------------------------------------------------*/



#ifndef __GKSECUR_INC_HPP
#define __GKSECUR_INC_HPP

#include <gkmodules.hpp>
#include <gksecurity.h>
#include <storage.hpp>
#include <string>
#include <map>
#include <modemmer.h>
#include <modem_proto.h>
#include <otd_base.h>
#include <gksecur_proto.h>




#pragma pack(push,1)

typedef struct  _SESSION_DESCRIPT
{
 DWORD      user_id;
 unsigned   __int64 session_cript_key;
}SESSION_DESCRIPT,*LPSESSION_DESCRIPT;


#pragma pack(pop)


typedef std::map<std::wstring,DWORD>     Tusers_map;
typedef std::pair<SESSION_ID,SESSION_ID> Tsessions;
typedef std::map<modem_addr, Tsessions>  Tmodem_sessions;
typedef std::multimap<DWORD,modem_addr>  Tuser_login;


#define USERS_RECID     0
#define USERS_CHK_COUNT 0


class TGKSecurityMod:public TGKModule
{
 protected:
 TStorage             *sdata;
 TFileStream          *Fstream;
 TFASTMutex            locker;
 Tusers_map            users_map;
 int                   int_line;
 TGKModuleInterface    modemmer;

 Tmodem_sessions modem_sessions;

 static unsigned __int64 secret_word;

 virtual DWORD __fastcall check_license(){return MOD_LICENSE_REGISTERED; }

 WORD    __fastcall get_type       () {return MT_SECURITY;}
 DWORD   __fastcall get_mem_used   ();
 void    __fastcall do_code        (LPVOID data,DWORD sz,LPVOID key,DWORD key_size);
 DWORD   __fastcall get_config_data(DWORD mask,LPVOID buf,DWORD bsz);
 bool    __fastcall set_config_data(DWORD mask,LPVOID buf,DWORD bsz);
 bool    __fastcall compare_config_data(DWORD mask,LPVOID buf,DWORD bsz,LPDWORD ch_mask,BOOL * restart);
 DWORD   __fastcall start(DWORD reason,LPARAM p2) ;
 DWORD   __fastcall stop (DWORD reason);
 DWORD   __fastcall get_window_module  (wchar_t * buf,DWORD bsz);


 bool    __fastcall open_users_dbase();
 void    __fastcall init_users_dbase();


 void       __fastcall read_users      ();
 DWORD      __fastcall add_user(wchar_t *user_name,wchar_t * pass);
 bool       __fastcall get_user_entry (DWORD user_id,LPUSER_ENTRY entry) ;
 SESSION_ID __fastcall login(wchar_t * user_name,wchar_t * password);
 LRESULT    __fastcall handle_proto(LPMPROTO_HEADER mph,DWORD sz);
 LRESULT    __fastcall handle_cmd_login (LPMPROTO_HEADER mph);
 LRESULT    __fastcall handle_cmd_logout(LPMPROTO_HEADER mph);
 LRESULT    __fastcall enum_user          (LPUSER_ENTRY entry,DWORD idx);
 LRESULT    __fastcall on_notify_code     (LPNOTIFY_CODE nc,DWORD mask);
 LRESULT    __fastcall on_modem_line_state(LPMODEM_LINE_STATE mls);
  bool      __fastcall handle_line_logout (MODEM_ADDR ma);
 LRESULT    __fastcall check_right        (DWORD _ma,LPVOID ctx);

 LRESULT    __fastcall send       (LPMPROTO_HEADER mph);
 DWORD      __fastcall mproto_send(MODEM_ADDR & to,DWORD cmd,DWORD err,LPVOID data,DWORD data_size,bool respond,bool begin,bool finish );


 public:
 TGKSecurityMod();
 virtual LRESULT __fastcall processing(DWORD cmd,LPARAM p1,LPARAM p2);
};


#endif
 