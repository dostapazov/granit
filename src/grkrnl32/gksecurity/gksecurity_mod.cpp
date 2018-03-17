#pragma hdrstop
#include <gksecur.hpp>
#include <string.h>

TGKSecurityMod::TGKSecurityMod():
TGKModule()
{
 Fstream = NULL;
 sdata   = NULL;
 alloc_gkhandle(GKSECURITY_MOD_NAME);
}

 void    __fastcall TGKSecurityMod::do_code        (LPVOID _data,DWORD sz,LPVOID _key,DWORD key_size)
 {
    //Шифруется исключающим или по ключу
    //чет текущего значение
    LPBYTE data = (LPBYTE)_data;
    LPBYTE key  = (LPBYTE)_key;
     if(data && sz && key && key_size)
     {
      LPBYTE d_ptr = data;
      LPBYTE d_end = d_ptr+sz;
      DWORD  curr = 0;
      LPBYTE key_ptr = key;
      LPBYTE key_end = key_ptr+key_size;
      bool   dir  = true;
      do
      {
       BYTE key_val = *key_ptr;
       if(!key_val || (curr&1))
           {
            key_val = ~key_val;
           }
       (*d_ptr)^=key_val;
       ++curr;
       key_ptr+= dir ? 1:-1;
       if(dir && key_ptr == key_end || !dir && key_ptr == key)
         {
          dir = !dir; if(!dir) --key_ptr;
         }
       ++d_ptr;

      }while(d_ptr<d_end);
     }
 }

 DWORD   __fastcall TGKSecurityMod::get_config_data(DWORD mask,LPVOID buf,DWORD bsz)
 {
   DWORD  ret = GKH_RET_SUCCESS;
   return ret;
 }

 bool    __fastcall TGKSecurityMod::set_config_data(DWORD mask,LPVOID buf,DWORD bsz)
 {
   bool ret = true;

   return ret;
 }

 bool    __fastcall TGKSecurityMod::compare_config_data(DWORD mask,LPVOID buf,DWORD bsz,LPDWORD ch_mask,BOOL * restart)
 {
   bool ret = false;

   return ret;

 }

 DWORD   __fastcall TGKSecurityMod::get_window_module  (wchar_t * buf,DWORD bsz)
 {
 if(buf && bsz)
   {
    wcsncpy(buf,L"gksecurwnd.dll",bsz-1);
    return wcslen(buf);
   }
   return 0;
 }
 LRESULT __fastcall TGKSecurityMod::processing(DWORD cmd,LPARAM p1,LPARAM p2)
 {
   LRESULT ret = GKH_RET_ERROR;
   switch(cmd)
   {
    case MDMCM_FA_HANDLE:
    ret =  this->handle_proto((LPMPROTO_HEADER)p1,(DWORD)p2);
    break;
    default: ret = TGKModule::processing(cmd,p1,p2);
    break;
   }
  return ret;
 }

 DWORD   __fastcall TGKSecurityMod::start(DWORD reason,LPARAM p2)
 {
    LRESULT ret = GKH_RET_ERROR;
    int_line = -1;
    if(open_users_dbase())
    {
      modemmer.open(MODEMMER_HANDLE_NAME_W);
      int_line = modemmer.call(MDMCM_REG_INTERNAL_MLINE,LPARAM(_handle),-1);
      modemmer.call(MDMCM_REGISTER_HANDLER,LPARAM(_handle),FA_SECURITY);
      modemmer.set_notify(_handle,MNF_MODEM,TRUE);
      ret =TGKModule::start(reason,p2);
    }
   return ret;
 }

 DWORD   __fastcall TGKSecurityMod::stop (DWORD reason)
 {
  modemmer.set_notify(_handle,MNF_MODEM,FALSE);
  modemmer.call(MDMCM_REG_INTERNAL_MLINE,LPARAM(_handle),int_line);
  modemmer.call(MDMCM_UNREGISTER_HANDLER,LPARAM(_handle),FA_SECURITY);
  modemmer.close();
  this->sdata->flush();
  sdata->close();
  delete sdata;
  sdata = NULL;
  Fstream->Flush();
  delete Fstream;
  Fstream = NULL;
  {
    TLockHelper l(locker);
    modem_sessions.clear();
  }
  return TGKModule::stop(reason);
 }

 unsigned __int64 TGKSecurityMod::secret_word = *(unsigned __int64*)"GKUSRACC";

 DWORD   __fastcall TGKSecurityMod::add_user(wchar_t *user_name,wchar_t * pass)
 {
  DWORD ret = GKH_RET_ERROR;
  TLockHelper l(locker);
  USER_ENTRY entry = {0};
  entry.dwSize = sizeof(entry);
  _wcspcpy(entry.user_name,user_name);
  _wcspcpy(entry.user_password,pass);
  sdata->get_record_info(USERS_RECID,&entry.user_id);

  users_map[wstring(entry.user_name)] = entry.user_id;
  do_code(&entry,sizeof(entry),&secret_word,sizeof(secret_word));
  ret = sdata->add_chunk(USERS_RECID,&entry,sizeof(entry));
  return ret;
 }

 #pragma warn -8004

 void    __fastcall TGKSecurityMod::init_users_dbase()
 {
  //Инициализация бд пользователей
  //0 запись идентификатора
  TLockHelper l(locker);
  sdata->create(1,(char*)&secret_word,sizeof(secret_word),512);
  DWORD cnt = 0;
  DWORD rec_id = sdata->add_record(&cnt,sizeof(cnt));
  add_user(L"sysadm",L"");
 }
#pragma warn .8004


 bool    __fastcall TGKSecurityMod::open_users_dbase()
 {
  bool ret = false;
  wchar_t * data_folder =DATA_FOLDER;
  ForceDirs(data_folder);
  wchar_t file_name[MAX_PATH];
  swprintf(file_name,L"%s%s",data_folder,L"scurity.gkd");
  Fstream = new TFileStream(file_name);
  if(Fstream->IsGood())
    {
     sdata = new TStorage(Fstream);
     ret = true;
     DWORD ver;
     if(sdata->open((char*)&secret_word,sizeof(secret_word),&ver))
     {
         read_users();
     }
     else
       {
         init_users_dbase();
       }

    }

  return ret;
 }

 bool    __fastcall TGKSecurityMod::get_user_entry (DWORD user_id,LPUSER_ENTRY entry)
 {
  bool ret = false;
  if(check_data_size(entry,sizeof(*entry)))
  {
   TLockHelper l(locker);
   USER_ENTRY _entry;
   if(sdata->get_record(USERS_RECID,&_entry,sizeof(_entry),user_id))
   {
     do_code(&_entry,sizeof(_entry),&secret_word,sizeof(secret_word));
     if(_entry.dwSize == sizeof(_entry) && _entry.user_id == user_id)
       {
         ret = true;
         memcpy(entry,&_entry,sizeof(*entry));
       }
   }
  }
  return ret;
 }

 void    __fastcall TGKSecurityMod::read_users      ()
 {
  TLockHelper l(locker);
   users_map.clear();
   DWORD u_count = 0;
   sdata->get_record(USERS_RECID,&u_count,sizeof(u_count));
   DWORD i = 1;
   DWORD rdb;
   do
   {
     USER_ENTRY entry;
     rdb = sdata->get_record(USERS_RECID,&entry,sizeof(entry),i++);
     if(rdb)
     {
       do_code(&entry,sizeof(entry),&secret_word,(sizeof(secret_word)));
       if(entry.dwSize == sizeof(entry))
          users_map[entry.user_name] = entry.user_id;
     }
   }while(rdb);
 }

 LRESULT __fastcall TGKSecurityMod::send(LPMPROTO_HEADER mph)
 {
    if(mph && mph->data_size)
     return modemmer.call(MDMCM_SEND,(LPARAM)mph,MPROTO_SIZE(mph));
   return  GKH_RET_ERROR;
 }

DWORD       __fastcall TGKSecurityMod::mproto_send(MODEM_ADDR & to,DWORD cmd,DWORD err,LPVOID data,DWORD data_size,bool respond,bool begin,bool finish )
{
 DWORD ret = 0;
 BYTE   buffer[16384];
 LPMPROTO_HEADER     mph;
 LPOTDM_PROTO_HEADER omph;
 DWORD  need_size = sizeof(MPROTO_HEADER)-sizeof(mph->data[0])+sizeof(*omph)-sizeof(omph->data[0])+data_size;
 if(need_size<sizeof(buffer))
   mph = (LPMPROTO_HEADER) buffer;
  else
   mph = (LPMPROTO_HEADER) new BYTE[need_size];
 if(mph)
 {
 ZeroMemory(mph,sizeof(*mph));
 mph->addr_to.addr = to.addr;
 mph->data_size = need_size+(sizeof(mph->data)-sizeof(*mph));
 mph->fa = FA_SECURITY;
 if(to.line == MODEM_ADDR_MAXVALUE && (cmd&OTDM_COMMAND_MASK) == OTDMPROTO_CMD_ENUM_KADRS)
    mph->flags|=MPROTO_FLAG_SEND_BROADCAST;
 omph =(LPOTDM_PROTO_HEADER) mph->data;

 omph->command    = cmd&OTDM_COMMAND_MASK;

 if(respond)
   omph->command   |= OTDM_RESPOND;
 if(begin)
   omph->command  |= OTDM_BEGINDATA;
 if(finish)
  omph->command  |= OTDM_ENDDATA;
  if(err)
  omph->command  |= OTDM_ERROR;
  omph->error     = err;
  omph->data_size = data_size;
  memcpy(omph->data,data,data_size);
  ret = send(mph);
  if( buffer!=(LPBYTE)mph)
      delete  [](LPBYTE)mph;
  }
  return ret;

}

#pragma warn -8004

 SESSION_ID __fastcall TGKSecurityMod::login(wchar_t * user_name,wchar_t * password)
 {
  SESSION_ID ret = 0;
  TLockHelper l(locker);
  wstring str(user_name);
  Tusers_map::iterator ptr = users_map.find(str);
  if(ptr!= users_map.end())
     {
      DWORD user_id = ptr->second;
      USER_ENTRY entry;
      entry.dwSize    = sizeof(entry);
      if(get_user_entry (ptr->second,&entry))
      {
        if(0 == wcsncmp(entry.user_password,password,KERTL_ARRAY_COUNT(entry.user_password)) )
          {
           ret = (SESSION_ID)GetTime(true);
           Sleep(10);//Гарантия уникальности ID-Сессии
          }
      }
      else
      SetLastError(-1/*NO_USER*/);
     }
  return ret;
 }
#pragma warn .8004

 // создать otd_security.h



 LRESULT __fastcall TGKSecurityMod::handle_proto(LPMPROTO_HEADER mph,DWORD sz)
 {
     LRESULT ret = GKH_RET_ERROR;
     if(FA_SECURITY == mph->fa)
     {
      LPOTDM_PROTO_HEADER omph = (LPOTDM_PROTO_HEADER)mph->data;
      switch(omph->command&OTDM_COMMAND_MASK)
      {
       case GKSECUR_PROTO_CMD_LOGIN:   ret = handle_cmd_login(mph);
       break;
      }
     }
     return  ret;
 }

 LRESULT    __fastcall TGKSecurityMod::handle_cmd_login(LPMPROTO_HEADER mph)
 {

  LRESULT ret = GKH_RET_SUCCESS;
  wchar_t user_name[MAX_PATH] = {0};
  wchar_t password [MAX_PATH] = {0};
  SESSION_ID ssid = 0;
  if(gksecur_login_extract((LPOTDM_PROTO_HEADER)mph->data,&ssid,user_name,MAX_PATH,password,MAX_PATH))
    {
     SESSION_ID ssid2 =  login(user_name,password);
       //авторизация прошла успешно
      modem_addr ma(mph->addr_from);
      DWORD error  ;
      if(ssid2)
      {
       TLockHelper l(locker);
       Tsessions &ss = modem_sessions[ma];
       ss.first  = ssid;
       ss.second = ssid2;
       error = 0;
      }
      else
      error = -1;
      mproto_send(ma,((LPOTDM_PROTO_HEADER)mph->data)->command,error ,&ssid2,sizeof(ssid2),true,true,true);
     }

  return ret;
 }

 LRESULT  __fastcall TGKSecurityMod::on_notify_code(LPNOTIFY_CODE nc,DWORD mask)
 {
  if((mask & MNF_MODEM) )
  {
    if(nc->notify_code == MNCODE_MODEM_NUMBER)
    {
     //LPMODEM_CHANGE_NUMBER mcn = (LPMODEM_CHANGE_NUMBER)nc->notify_data;
    }

    if(nc->notify_code == MNCODE_LINE_STATE)
     return on_modem_line_state((LPMODEM_LINE_STATE)nc->notify_data);
  }

  return GKH_RET_SUCCESS;
 }

  LRESULT    __fastcall TGKSecurityMod::on_modem_line_state(LPMODEM_LINE_STATE mls)
  {
   if(mls->state_changes&  ( MODEM_LINE_STATE_EXIST
                            |MODEM_LINE_STATE_WORKED
                            |MODEM_LINE_STATE_CONNECT
                            )
      )
      handle_line_logout(mls->addr);
   return GKH_RET_SUCCESS;
  }

  bool __fastcall  TGKSecurityMod::handle_line_logout(MODEM_ADDR _ma)
  {
    modem_addr ma(_ma);
    TLockHelper l(locker);
    if(modem_sessions.count(ma))
       {
         modem_sessions.erase(ma);
         return true;
       }
     return false;
  }






