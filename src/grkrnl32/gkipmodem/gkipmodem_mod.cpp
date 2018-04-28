#include <stdio.h>
#include <winerror.h>
#include <tregstry.hpp>

#pragma hdrstop

#include <KeStrm.hpp>
#include "gkipmodem_mod.hpp"
using namespace KeRTL;

 TGkIPModem::TGkIPModem()
 {
    ZeroMemory(&settings,sizeof(settings));
    ZeroMemory(used_lines,sizeof(used_lines));
    settings.dw_size = sizeof(settings);
    alloc_gkhandle();
    lock_param    = GKHB_AUTO_LOCK_OFF;
    ZeroMemory(server_sockets,sizeof(server_sockets));
    server_socket_event = NULL;
    reg_reports();
    TBaseSocket::InitWS(MAKEWORD(2,2));
 }

  void    __fastcall TGkIPModem::reg_reports()
  {
    report_id = this->report_reg_event_type("GKIP_MODEM","TCP/IP модем");
    //arch_rep_id = report_reg_event_type(L"ALARM_ARCH",L"OTD TCP/IP Аварийные Архивы " );
  }


 void        __fastcall TGkIPModem::delete_sockets()
 {
  lock();
  for(DWORD i = 0; i<=this->settings.bind_count;i++)
    {
     TIPSocket * ss = server_sockets[i];
     if(ss)
       {
        ss->DestroySocket();
        delete ss;
       } 
       server_sockets[i] = NULL;
    }
   unlock();
 }

 void        __fastcall TGkIPModem::release(LPVOID rp)
 {

  delete_sockets();
  TModemBase::release(rp);
  TBaseSocket::DeinitWS();
 }


 const char  * __fastcall TGkIPModem::get_gkthread_name(){return "GKIP server main thread";}

 DWORD   __fastcall TGkIPModem::get_window_module  (wchar_t * buf,DWORD bsz)
 {
   wchar_t    * module_name = L"gkipmodemwnd.dll";
   DWORD ret = lstrlenW(module_name);
   lstrcpynW(buf,module_name,bsz);
   if(ret>=bsz)
      ret = 0;
   return ret;
 }

 bool    __fastcall TGkIPModem::read_settings ()
 {
  bool ret = false ;
  if(reg_key)
  {
  TModemBase::read_settings();
  settings.modem_number = get_modem_number();
  TRegsReader rr(DupKey(reg_key));
  settings.flags     = rr.ReadDword(REGDW_IPM_FLAGS,0,true);
  settings.port      = rr.ReadDword(REGDW_IPM_PORT,45777,true);
  settings.bind_count= rr.ReadDword(REGDW_IPM_BINDCOUNT,0,true);
  rr.ReadBytes(REGBIN_IPM_BINDLIST,settings.bind_addrs,sizeof(settings.bind_addrs),true);
  rr.ReadString(REGSTR_IPM_SERVERADDR,settings.server_addr,sizeof(settings.server_addr),true);
  settings.client_options = rr.ReadDword(REGDW_IPM_CLIENT_OPTIONS,0,true);

  if(settings.flags&IPM_FLAG_ASCLIENT)
     read_client_lines();
  ret = true;
  cc_flags = 0;
  }
  return ret;
 }

 void        __fastcall TGkIPModem::read_client_lines ()
 {
   TMemoryStream ms;
   TRegsReader rd(DupKey(reg_key));
   DWORD type = 0;DWORD sz = 0;
   rd.GetValueType(REGBIN_IPM_LINES,&type,&sz,true);
   if(sz)
   {
     ms.SetSize(sz);
     rd.ReadBytes(REGBIN_IPM_LINES,ms.GetMemory(),sz,true);
     ms.Rewind();
     TLockHelper l(locker);
     memset(used_lines,0,sizeof(used_lines));
     while(lines.size())
      remove_line(lines.begin()->first);
     DWORD cnt = 0;

     ms>>cnt;
     for(DWORD i = 0;i<cnt;i++)
     {
       DWORD number;
       char server_addr[MAX_PATH];
       char *sa = server_addr;
       int port;
       ms>>number;
       ms>>port;
       ms.read_string(sa);
       int clopts = 0;
       ms>>clopts;

       ip_line * line = new ip_line(number,server_addr,port,clopts);
       used_lines[number] = TRUE;
       add_line(line);
     }
    }
 }

 void        __fastcall TGkIPModem::write_client_lines()
 {
   TMemoryStream ms;
   TLockHelper l(locker);
   DWORD lcnt = get_lines_count();
   ms<<lcnt;
   lines_map::iterator ptr = lines.begin();
   lines_map::iterator end = lines.end();
   lcnt = 0;
   while(ptr!=end)
   {

    ip_line * line = dynamic_cast<ip_line *>(ptr->second);
    if(line)
    {
     lcnt++;
     ms<<(DWORD)line->get_number();
     ms<<(int)line->get_server_port();
     const char * server_addr  = line->get_server_addr() ;
     ms.write_string(server_addr);
     ms<<(int)line->get_client_options();

    }
    ++ptr;
   }
   ms.Rewind();
   ms<<lcnt;
   TRegsWriter rw(DupKey(reg_key));
   rw.WriteBytes(REGBIN_IPM_LINES,ms.GetMemory(),ms.GetSize(),true);
   rw.Flush();
 }


 bool    __fastcall TGkIPModem::write_settings()
 {
  bool ret = false;
  if(reg_key)
  {
   ret = true;
   TModemBase::write_settings();
   TRegsWriter rgw(DupKey(reg_key));
   if(cc_flags&CCFL_IPM_FLAGS)
   {
    rgw.WriteDword(REGDW_IPM_FLAGS,settings.flags,true);
    cc_flags&=~CCFL_IPM_FLAGS;
   }

   if(cc_flags&CCFL_IPM_PORT)
   {
     rgw.WriteDword(REGDW_IPM_PORT,settings.port,true);
     cc_flags&=~CCFL_IPM_PORT;
   }

   if(cc_flags&CCFL_IPM_BINDPARAMS)
   {
    rgw.WriteDword(REGDW_IPM_BINDCOUNT,settings.bind_count,true);
    rgw.WriteBytes(REGBIN_IPM_BINDLIST,settings.bind_addrs,sizeof(settings.bind_addrs),true);
   }

   if(cc_flags&CCFL_IPM_SERVERADDR)
     rgw.WriteString(REGSTR_IPM_SERVERADDR,settings.server_addr,true);
   if(cc_flags&CCFL_IPM_CLIENT_OPTS)
      rgw.WriteDword(REGDW_IPM_CLIENT_OPTIONS,settings.client_options,true);

  if(cc_flags&CCFL_IPM_CLIENTS)
      {
       write_client_lines();
       cc_flags &=~CCFL_IPM_CLIENTS;
      }

  }
   return ret;
 }

#pragma warn -8057

 DWORD   __fastcall TGkIPModem::get_config_data(DWORD mask,LPVOID buf,DWORD bsz)
 {
  DWORD ret = GKH_RET_ERROR;
  LPGKIPM_SETTINGS  ipms = (LPGKIPM_SETTINGS) buf;
  if(check_data_size(ipms,sizeof(*ipms)))
  {
   TLockHelper l(locker);
   if(mask&CCFL_MODEM_NUMBER)
     ipms->modem_number = get_modem_number();
   if(mask&CCFL_IPM_FLAGS)
    ipms->flags = settings.flags;
   if(mask&CCFL_IPM_PORT)
     ipms->port = settings.port;
   if(mask&CCFL_IPM_BINDPARAMS)
    {
     ipms->bind_count = settings.bind_count;
     memcpy(ipms->bind_addrs,settings.bind_addrs,sizeof(ipms->bind_addrs));
    }
   if(mask & CCFL_IPM_SERVERADDR)
     {
       strcpy(ipms->server_addr,settings.server_addr);
     }
    if(mask & CCFL_IPM_CLIENT_OPTS)
       ipms->client_options = this->settings.client_options;
   ret = GKH_RET_SUCCESS;
  }
  return ret;
 }



 bool    __fastcall TGkIPModem::set_config_data(DWORD mask,LPVOID buf,DWORD bsz)
 {
  bool ret = false;
  LPGKIPM_SETTINGS  ipms = (LPGKIPM_SETTINGS) buf;
  DWORD error = 0;
  if(check_data_size(ipms,sizeof(*ipms)))
  {
   ret = true;
   if((mask&CCFL_MODEM_NUMBER))
      {
       if(set_modem_number(ipms->modem_number,NULL))
          {
           error = GetLastError();
           ret = false;
          }
          else
          settings.modem_number = get_modem_number();

      }
   if(mask&CCFL_IPM_FLAGS)
      settings.flags = ipms->flags ;

   if(mask&CCFL_IPM_PORT)
      settings.port = ipms->port;

   if(mask&CCFL_IPM_BINDPARAMS)
    {
      settings.bind_count = ipms->bind_count ;
      memcpy(settings.bind_addrs,ipms->bind_addrs,sizeof(settings.bind_addrs));
    }

   if(mask & CCFL_IPM_SERVERADDR)
     {
       strcpy(settings.server_addr,ipms->server_addr);
     }

   if(mask & CCFL_IPM_CLIENT_OPTS)
     {
       settings.client_options = ipms->client_options;
       ip_line * ipl = NULL;
       DWORD idx = 0;
       do{
          ipl = dynamic_cast<ip_line *>( get_line_byindex(idx++) );
          if(ipl)
             ipl->set_client_options(settings.client_options);
         }while(ipl);
     }

  }
  if(error) SetLastError(error);//Сохранить ошибку
  return ret;
 }

 bool    __fastcall TGkIPModem::compare_config_data(DWORD mask,LPVOID buf,DWORD bsz,LPDWORD ch_mask,BOOL * restart)
 {
  bool ret = false;
  DWORD  cm = 0;
  BOOL   _rst = FALSE;
  LPGKIPM_SETTINGS  ipms = (LPGKIPM_SETTINGS) buf;
  if(check_data_size(ipms,sizeof(*ipms)))
  {
   if((mask&CCFL_MODEM_NUMBER) && ipms->modem_number != get_modem_number())
      _rst = TRUE,cm|=CCFL_MODEM_NUMBER;

   if((mask&CCFL_IPM_FLAGS) && ipms->flags != settings.flags)
        {
         _rst = ((settings.flags^ipms->flags)&IPM_FLAG_ASCLIENT) ? TRUE:FALSE;
         cm|= CCFL_IPM_FLAGS;
        }

   if((mask&CCFL_IPM_PORT) && ipms->port != settings.port)
       _rst = TRUE,cm|=CCFL_IPM_PORT;

   if(mask&CCFL_IPM_BINDPARAMS)
    {
     if(ipms->bind_count != settings.bind_count ||  memcmp(ipms->bind_addrs,settings.bind_addrs,sizeof(ipms->bind_addrs)))
        _rst = TRUE,cm|=CCFL_IPM_BINDPARAMS;

    }

    if(mask&CCFL_IPM_SERVERADDR)
    {
     if(stricmp(settings.server_addr,ipms->server_addr))
        _rst = TRUE,cm|=CCFL_IPM_SERVERADDR;
    }
    if( (mask & CCFL_IPM_CLIENT_OPTS) && settings.client_options!= ipms->client_options )
       {
         _rst = TRUE,cm|=CCFL_IPM_CLIENT_OPTS;
       }
    if(ch_mask) *ch_mask = cm;
    if(restart) *restart = _rst;
    ret = cm ? true:false;
  }
  return ret;
 }

#pragma warn .8057

 void        __fastcall    TGkIPModem::free_line (modem_line * line)
 {
   if(line)
   {
    ip_line * ipl = dynamic_cast<ip_line*>(line);
    if(ipl)
       delete ipl;
       else
       delete line;
   }
 }

#pragma warn -8057

int         __fastcall    TGkIPModem::convert_rx_data(LPWORD fa,LPBYTE in,int in_len,LPBYTE out,DWORD out_sz)
{
 if(int(out_sz)>=in_len)
  {
   memcpy(out,in,in_len);
  }
  else
  in_len = -in_len;

 return in_len;
}

#pragma warn .8057

LRESULT     __fastcall    TGkIPModem::processing         (DWORD cmd,LPARAM p1,LPARAM p2)
{
 LRESULT ret ;
 switch(cmd)
 {
  case CMIPM_GETLINE_ADDRS :ret = get_line_addrs((LPGKIPM_LINE_ADDRS)p1);break;
  case CMIPM_SETLINE_ADDRS :ret = set_line_addrs((LPGKIPM_LINE_ADDRS)p1);break;
  case CMIPM_CREATE_LINE   :ret = create_line((LPGKIPM_LINE_ADDRS)p1);break;
  case CMIPM_DELETE_LINE   :ret = delete_line((DWORD)p1);break;
  case CMIPM_GET_LINE_OPTS :ret = get_line_opts((DWORD)p1,(LPDWORD)p2 );break;
  case CMIPM_SET_LINE_OPTS :ret = set_line_opts((DWORD)p1,(DWORD) p2 ); break;

  default: ret = TModemBase::processing(cmd,p1,p2);
 }
 return ret;
}


 LRESULT     __fastcall TGkIPModem::create_line(LPGKIPM_LINE_ADDRS addrs)
 {
   LRESULT ret = GKH_RET_ERROR;
   if(check_data_size(addrs,sizeof(*addrs)))
   {
     if(addrs->line_number == (DWORD)-1)
        addrs->line_number = this->find_free_line_number();
     if(!get_line(addrs->line_number))
     {
       if(addrs->line_number< this->get_lines_limit())
       {
        ip_line * ipl = new ip_line(addrs->line_number,addrs->host_name,addrs->host_port,settings.client_options);
        if(add_line(ipl))
        {
            used_lines[addrs->line_number] = true;
            ret = GKH_RET_SUCCESS;
            cc_flags|=CCFL_IPM_CLIENTS;
            if(this->is_running())
               ipl->start();
        }
       }
       else
       SetLastError(-1);//TODO вставить ошибку LINE_ALREADY_EXISTS
     }
     else
     SetLastError(ERROR_ALREADY_EXISTS);
   }
   return  ret;
 }

 LRESULT     __fastcall TGkIPModem::delete_line(DWORD line_number)
 {
   LRESULT ret = GKH_RET_ERROR;
   modem_line * line = get_line(line_number);
   if(line)
   {
    if(line->is_worked())
       line->stop() ;
    if( remove_line(line_number) )
      {
       used_lines[line_number] = 0;
       ret = GKH_RET_SUCCESS;
       cc_flags|=CCFL_IPM_CLIENTS;
      }

   }
   else
   SetLastError(-1);//TODO Вставить ошибку линия не существует
   return ret;
 }

 LRESULT     __fastcall TGkIPModem::get_line_opts(DWORD line_number,LPDWORD opts)
 {
   LRESULT ret = GKH_RET_ERROR;
   ip_line * line = dynamic_cast<ip_line*>( get_line(line_number) );
   if(line && opts)
   {
    *opts = line->  get_client_options();
    ret = GKH_RET_SUCCESS;
   }
   return ret ;
 }

 LRESULT     __fastcall TGkIPModem::set_line_opts(DWORD line_number,  DWORD opts)
 {
   LRESULT ret = GKH_RET_ERROR;
   ip_line * line = dynamic_cast<ip_line*>( get_line(line_number) );
   if(line )
   {
    line->  set_client_options(opts);
    ret = GKH_RET_SUCCESS;
   }
   return ret ;
 }



BOOL        __fastcall TGkIPModem::can_start(DWORD reason,LPARAM p2)
{
 if(TModemBase::can_start(reason,p2))
 {
    int proto[2] = {IPPROTO_TCP,0};
    WSAPROTOCOL_INFO pi[10] ={0};
    DWORD sz   = sizeof(pi);
    int cnt = WSAEnumProtocols(proto,pi,&sz);
    if(cnt <1)
       {

        SetLastError(WSAEPROTONOSUPPORT	);
        return FALSE;
       }

  return TRUE;
 }
 return FALSE;
}

BOOL        __fastcall TGkIPModem::can_stop (DWORD reason)
{
 if(TModemBase::can_stop(reason))
 {
   return TRUE;
 }
 return FALSE;
}


DWORD   __fastcall TGkIPModem::start(DWORD reason,LPARAM p2)
{
  DWORD ret = GKH_RET_ERROR;
  bool as_client = false;
  reg_reports();
  lock();
  if(settings.flags&IPM_FLAG_ASCLIENT)
    as_client = true;
    else
    ZeroMemory(used_lines,sizeof(used_lines));
  unlock();
  if(as_client)
  {
   //TODO удалить все линии
   /*if(lines.size()==0)
     add_line(new ip_line(0,settings.server_addr,settings.port));

   ip_line * ipl = dynamic_cast<ip_line*>(this->get_line(0));
   ipl->set_server(settings.server_addr,settings.port);
   */
   /*
   while(lines.size())
      remove_line(lines.begin()->first);
   add_line(new ip_line(0,settings.server_addr,settings.port));
   */
   ret = GKH_RET_SUCCESS;
  }
  else
  {
   while(lines.size())
      remove_line(lines.begin()->first);
  if(Start(16384))
   ret = GKH_RET_SUCCESS;
  }
  if(ret == GKH_RET_SUCCESS)
    ret = TModemBase::start(reason,p2);
 return ret;
}

DWORD   __fastcall TGkIPModem::stop (DWORD reason)
{
  DWORD ret;
  TerminateAndWait(3000);
  ret = TModemBase::stop(reason);
  if(!(settings.flags&IPM_FLAG_ASCLIENT))
      {
       clear_lines();
       ZeroMemory(used_lines,sizeof(used_lines));
      }

  return ret;   
}

void       __fastcall TGkIPModem::Terminate()
{
 terminate_event.SetEvent(true);
 TGKThread::Terminate();
}


bool        __fastcall TGkIPModem::BeforeExecute()
{
 ss.Clear();
 server_socket_event = WSACreateEvent();
 terminate_event.SetEvent(false);
 ss+=terminate_event();
 if(server_socket_event)
  {
    ss+=server_socket_event;
    TIPSocket * srv_socket;
    lock();
    //Привязка к адресам
    if(settings.bind_count)
      {
       for(DWORD i = 0;i<settings.bind_count;i++)
        {
           TIPAddress ipa(settings.bind_addrs[i],settings.port);
           srv_socket = new TIPSocket(ipa,true);
           srv_socket->EnableReuseAddress(true);
           srv_socket->Bind(ipa(),sizeof(ipa));
           srv_socket->Listen(SOMAXCONN);
           WSAEventSelect(srv_socket->GetSocket(),server_socket_event,FD_ACCEPT);
           server_sockets[i] = srv_socket;
        }
      }
      else
      {
      TIPAddress ipa(INADDR_ANY,settings.port);
      srv_socket = new TIPSocket(ipa,true);
      srv_socket->EnableReuseAddress(true);
      srv_socket->Bind(ipa(),sizeof(ipa));
      srv_socket->Listen(SOMAXCONN);
      server_sockets[0] = srv_socket;
      WSAEventSelect(srv_socket->GetSocket(),server_socket_event,FD_ACCEPT);
      }
   unlock();

 }
 return true;
}

void        __fastcall TGkIPModem::BeforeTerminate()
{
  delete_sockets();
  if(server_socket_event)
     WSACloseEvent(server_socket_event);
     server_socket_event = NULL;
}


int         __fastcall TGkIPModem::Execute()
{
 while(!CheckTerminateRequest()  )
 {

  DWORD wr = ss.Wait(INFINITE);
  switch(wr)
  {
   case 1: on_socket_event() ;break;
  }
 }
 return 0;
}

#pragma warn -8057

BOOL WINAPI TGkIPModem::accept_checker(  IN LPWSABUF lpCallerId,
                             IN LPWSABUF lpCallerData,
                             IN OUT LPQOS lpSQOS,
                             IN OUT LPQOS lpGQOS,
                             IN LPWSABUF lpCalleeId,
                             OUT LPWSABUF lpCalleeData,
                             OUT GROUP FAR * g,
                             IN DWORD dwCallbackData
                            )
 {

    
    TGkIPModem* ip_modem = reinterpret_cast<TGkIPModem*>(dwCallbackData);
    if(ip_modem->find_free_line_number()>=MODEM_ADDR_MAXVALUE)
    {
       char rep_text[MAX_PATH];
       sprintf(rep_text,"Отказ в подключении : нет свободных линий");
       ip_modem->ipm_report(false,REPORT_ERROR_TYPE,rep_text);
      
      return CF_REJECT;
    }
    return CF_ACCEPT;

 }

#pragma warn .8057


// #pragma warn -8004
 void        __fastcall TGkIPModem::on_socket_event()
 {
    WSANETWORKEVENTS ne;
    ZeroMemory(&ne,sizeof(ne));
    TIPSocket * server_socket = NULL;
    for(DWORD i = 0;i<=settings.bind_count;i++)
    {
    lock();
    server_socket = server_sockets[i];
    if(server_socket)
    {
    WSAEnumNetworkEvents(server_socket->GetSocket(),server_socket_event,&ne);
    if(ne.lNetworkEvents&FD_ACCEPT)
    {
     TIPAddress ipa;
     int ipa_sz = sizeof(ipa);

     SOCKET s = server_socket->WSAAccept(ipa(),&ipa_sz,accept_checker,(DWORD)this);
     if((SOCKET)SOCKET_ERROR != s)
     {
	  //Поиск и или создание новой линии
	  unlock();
	  ip_line * ipl = find_free_line();
	  if(ipl)
		 ipl->accept_connection(s,ipa);
		 else
		 closesocket(s);
      lock();
     }
     }
    }
    unlock();
   }
 }

 DWORD       __fastcall TGkIPModem::find_free_line_number()
 {
  /*Поиск свободного номера линии*/
  DWORD ret = -1;
  TLockHelper l(locker);
  for(DWORD i = 0;i<MODEM_ADDR_MAXVALUE;i++)
  {
   if(used_lines[i]==0)
      {ret = i;break;}
  }
  return ret;
 }

 ip_line *   __fastcall TGkIPModem::find_free_line()
 {
  ip_line * ret ;
  DWORD ln = this->find_free_line_number();
  if(lines.count(ln))
	 this->free_line(lines[ln]);

  {
   TLockHelper l(locker);
   used_lines[ln]=1;
  }

  ret = new ip_line(ln,settings.client_options);
  add_line( ret );
  return ret;
 }

LRESULT     __fastcall TGkIPModem::set_line_addrs(LPGKIPM_LINE_ADDRS addrs)
{
 LRESULT ret = GKH_RET_ERROR;
 if(check_data_size(addrs,sizeof(*addrs)))
 {
  ip_line * ipl= dynamic_cast<ip_line*>(get_line(addrs->line_number));
  if(ipl)
     {
      bool is_worked = ipl->is_worked() ;
      if(is_worked)  ipl->stop();
       ipl->set_line_addrs(addrs);
      ret = GKH_RET_SUCCESS;
      cc_flags|=CCFL_IPM_CLIENTS;
      if(is_worked)  ipl->start();
     }
 }
 return ret;

}

LRESULT     __fastcall TGkIPModem::get_line_addrs(LPGKIPM_LINE_ADDRS addrs)
{
 LRESULT ret = GKH_RET_ERROR;
 if(check_data_size(addrs,sizeof(*addrs)))
 {
  ip_line * ipl= dynamic_cast<ip_line*>(get_line(addrs->line_number));
  if(ipl)
     {
      ipl->get_line_addrs(addrs);
      ret = GKH_RET_SUCCESS;
     }
 }
 return ret;
}

void        __fastcall TGkIPModem::line_state_changed(DWORD number,DWORD line_state,DWORD changes)
{
 if(number<MODEM_ADDR_MAXVALUE && !(settings.flags&IPM_FLAG_ASCLIENT) && (changes&MODEM_LINE_STATE_EXIST) && !(line_state&MODEM_LINE_STATE_EXIST))
  {
   lock();
   lines.erase(number);
   used_lines[number] = 0;
   unlock();
   Sleep(0);
  }
 TModemBase::line_state_changed(number,line_state,changes);
}

DWORD       __fastcall TGkIPModem::get_modem_info(wchar_t * buff,DWORD buff_sz)
{
 if(settings.flags&IPM_FLAG_ASCLIENT)
  {
   return get_lang_string(IDS_TCP_CLIENT,buff,buff_sz);
  }
  else
  {
   return get_lang_string(IDS_TCP_SERVER,buff,buff_sz);
  }
}

#pragma warn -8057
void    __fastcall TGkIPModem::ipm_report(bool arch,DWORD type,char * descr,LPVOID data ,DWORD data_sz )
{
 DWORD rep_id = arch ? arch_rep_id : report_id;
 report(rep_id,type,descr);
}

void    __fastcall TGkIPModem::ipm_report(bool arch,DWORD type,wchar_t * descr,LPVOID data ,DWORD data_sz )
{
 DWORD rep_id = arch ? arch_rep_id : report_id;
 report(rep_id,type,descr);
}

#pragma warn .8057






