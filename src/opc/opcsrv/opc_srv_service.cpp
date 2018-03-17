//---------------------------------------------------------------------------

#pragma hdrstop

#include "opc_srv_service.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

#include "unilog.h"
#define LOGID log,0
#include <ole2.h>
#include <olectl.h>
#include <oleauto.h>
#include <process.h>
#include <stdio.h>
#include <errno.h>
#include <locale.h>
#include <opcda.h>
#include <opcerror.h>
#include <iostream.h>
#include <TLogFile.hpp>
#include <kfname.hpp>
#include <comutil.h>

TGkOpcDriver * opc_drv = NULL;
DWORD          main_thread_id;

class TOpcCF : public IClassFactory
{
  protected:
  TFASTMutex locker;
  LONG server_count;
  LONG ref_count;
  TGkOpcDriver * opc_drv;


  public:
  TOpcCF (TGkOpcDriver * drv ):IClassFactory(){opc_drv = drv; ref_count = 0; server_count = 0;}
  bool __fastcall lock()  {return locker.Lock()  ;}
  bool __fastcall unlock(){return locker.Unlock();}

  void serverAdd(void);
  void serverRemove(void);

  STDMETHODIMP_(ULONG) AddRef (void);
  STDMETHODIMP_(ULONG) Release(void);
  STDMETHODIMP QueryInterface(REFIID iid, LPVOID *ppInterface);
  STDMETHODIMP LockServer(BOOL fLock);
  STDMETHODIMP CreateInstance(LPUNKNOWN pUnkOuter, REFIID riid,
                              LPVOID *ppvObject);
};


KeRTL::TLogFile Flog;

LONG WINAPI UNH_EF(LPEXCEPTION_POINTERS exps)
{

 char  hdr [2048];
 char  text[2048];
 char  mname[1024];
 GetPtrModuleName(exps->ExceptionRecord->ExceptionAddress,mname,KERTL_ARRAY_COUNT(mname));
 sprintf(hdr,"Unhandled Exception 0x%X in module %s",exps->ExceptionRecord->ExceptionCode,mname);
 sprintf(text,"at address 0x%X \n\r main_thread_id = 0x%X, curennt thread = 0x%X ,svc_thread = 0x%X ",exps->ExceptionRecord->ExceptionAddress,main_thread_id,GetCurrentThreadId(),opc_drv->GetID());
 Flog.WriteToLog(hdr,text);
 int len = 0;
 LPDWORD stack = (LPDWORD)exps->ContextRecord->Esp;
 for(int i = 0;i<32;++i,--stack)
 {
  len+=sprintf(text+len,"0x%X ",*stack);
  if(i%8 == 7)
    len+=sprintf(text+len,"\n\r");
 }

 Flog.WriteToLog("Stack dump",text);

 len = 0;
 stack = (LPDWORD)exps->ContextRecord->Ebp;
 for(int i = 0;i<32;++i,--stack)
 {
  len+=sprintf(text+len,"0x%X ",*stack);
  if(i%8 == 7)
    len+=sprintf(text+len,"\n\r");
 }

 Flog.WriteToLog("Ebp dump",text);

 return EXCEPTION_EXECUTE_HANDLER;
}

void __fastcall open_log()
{
 TKFName<char> log_name;
 char  exe_name[8192];
 GetModuleFileName(GetModuleHandle(NULL),exe_name,KERTL_ARRAY_COUNT(exe_name));
 log_name.set_fullname(exe_name);
 log_name.set_ext(".log");
 Flog.OpenLog(log_name());
 Flog.WriteToLog("opc_serv32.exe started",NULL);
}

int run_as_service(GUID & guid)
{

 main_thread_id = GetCurrentThreadId();
 open_log();
 SetUnhandledExceptionFilter(UNH_EF);
 HRESULT hr;
 setlocale(LC_ALL,"");
 hr = CoInitializeEx(NULL,COINIT_MULTITHREADED);
 if(SUCCEEDED(hr))
 {
  opc_drv = new TGkOpcDriver;
  opc_drv->readconfig();
  opc_drv->check_use_console();
  TOpcCF       * cf      = new TOpcCF(opc_drv);
  //if(!opc_drv->service_create(1000,loDF_NOCOMP,500)) //Создали службу
  {

    DWORD objid = 0;
    hr = CoRegisterClassObject(guid,cf,
               CLSCTX_LOCAL_SERVER | CLSCTX_REMOTE_SERVER | CLSCTX_INPROC_SERVER, REGCLS_MULTIPLEUSE,
               &objid);
   if(SUCCEEDED(hr))
   {
     cf->AddRef();
     //Запуск ожидания завершения
     int tmo = opc_drv->get_no_clients_work_time();
     while(tmo>0 && !opc_drv->IsRunning())
     {
         Sleep(1000);
         tmo-=1;
     }
     if(opc_drv->IsRunning())
        {
         while(!opc_drv->WaitFor(1000))
         {
           Sleep(0);
         }
        }

     opc_drv->service_destroy();
     Sleep(1000);
     CoRevokeClassObject(objid);

     delete opc_drv;
     delete cf;
   }
  }
  CoUninitialize();
 }
 Flog.WriteToLog("opc_serv32.exe finished",NULL);
 return 0;
}

  STDMETHODIMP_(ULONG) TOpcCF::AddRef (void)
  {
   ULONG ret = 0;
   if(InterlockedIncrement(&ref_count)>0)
      ret = 1;
   return ret;
  }

  STDMETHODIMP_(ULONG) TOpcCF::Release(void)
  {
   ULONG ret = 0;
   if(InterlockedDecrement(&ref_count)>0)
      ret = 1;
   return ret;
  }

  void TOpcCF::serverAdd(void)
  {
     lock();
     ++server_count;
     CoAddRefServerProcess();
     unlock();
  }

  void TOpcCF::serverRemove(void)
  {
   lock();
   CoReleaseServerProcess();
   --server_count;
   if(0>server_count)  server_count = 0;
   unlock();
  }

  STDMETHODIMP TOpcCF::LockServer(BOOL fLock)
    {

      if (fLock)
      serverAdd();
       else
      serverRemove();
      return S_OK;
    }

  STDMETHODIMP TOpcCF::QueryInterface(REFIID iid, LPVOID *ppInterface)
  {
    __try{
            *ppInterface = NULL;

          if (ppInterface == NULL)
            return E_INVALIDARG;
          if (iid == IID_IUnknown || iid == IID_IClassFactory)
            {
              *ppInterface = this;
              AddRef();
              return S_OK;
            }
         }
         __except(EXCEPTION_EXECUTE_HANDLER)
         {
            if(opc_drv) opc_drv->write_console("Exception in QueryInterface",true);
         }

      return E_NOINTERFACE;
  }

  STDMETHODIMP TOpcCF::CreateInstance(LPUNKNOWN pUnkOuter, REFIID riid,LPVOID *ppvObject)
  {
      IUnknown *server = 0;
      HRESULT hr = S_OK;
      char buff[1024];
     /* OPTIONAL: security stuff */
   __try
   {
      OLECHAR *userid = 0;
      CoQueryClientBlanket(0, 0, 0, 0, 0, (RPC_AUTHZ_HANDLE*)&userid, 0);
      opc_drv->write_console("Attach new client",true);
      if(userid)
      {
       KeRTL::Unicode2Ansi(buff,userid);
       opc_drv->write_console(buff);
      }


      if (pUnkOuter)
        {
         if (riid != IID_IUnknown)
          return CLASS_E_NOAGGREGATION;
        }
      serverAdd();

 IUnknown *inner = 0;
 if(!opc_drv->is_service_valid())
 {
    if(opc_drv->service_create(1000,loDF_NOCOMP,500))
     hr = E_ABORT;
     else
      opc_drv->write_console("Create service",true);
 }

 if (S_OK == hr && opc_drv->create_client_agg((loClient**)&server,
                       pUnkOuter, &inner,0))
    {
      serverRemove();
      hr = E_OUTOFMEMORY;

    }
  else
  if (pUnkOuter)
  *ppvObject = (void*)inner; /*aggregation requested*/
  else /* no aggregation */
    {
/* loClientCreate(my_service, (loClient**)&server,
                  0, &vendor, a_server_finished, cuserid) - with no aggregation */
      /* Initally the created SERVER has RefCount=1 */
      hr = server->QueryInterface(riid, ppvObject); /* Then 2 (if success) */
      server->Release(); /* Then 1 (on behalf of client) or 0 (if QI failed) */
      //if (FAILED(hr))
      //  UL_MESSAGE((LOGID, "myClassFactory::loClient QueryInterface() failed"));
      /* So we shouldn't carry about SERVER destruction at this point */
    }

  if (SUCCEEDED(hr))
    {

      if( !opc_drv->IsRunning())
           {
           if(opc_drv->GetState() == TThread::tsTerminating)
              opc_drv->WaitFor(5000);

            opc_drv->write_console("start working thread",true);
            if(!opc_drv->Start(0x20000,FALSE))
              {
                DWORD err = GetLastError();
                opc_drv->write_console("error start thread",true);
                KeRTL::GetErrorMessage(err,buff,sizeof(buff),(HINSTANCE)NULL);
                opc_drv->write_console(buff);
                hr = E_OUTOFMEMORY;
              }

           }

      opc_drv->set_state((loClient*)server,
              loOP_OPERATE, (int)OPC_STATUS_RUNNING, /* other states are possible */
              "Finished by client");

    }

    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
      opc_drv->write_console("Exception in CreateInstance",true);
      serverRemove();
      hr = E_UNEXPECTED;
    }

      return hr;
}

void __fastcall TGkOpcDriver::write_console(const char *text,bool time)
{
  if(get_use_console() && text )
  {
     if(time)
     {
      char date_time[1024];
      SYSTEMTIME st;
      GetSystemTime(&st);
      int dtl = GetDateFormat(LOCALE_USER_DEFAULT,DATE_SHORTDATE,&st,NULL,date_time,sizeof(date_time));
      dtl--;
      *(date_time+dtl) = ' ';
      dtl++;
      dtl+=GetTimeFormat(LOCALE_USER_DEFAULT,0,&st,"HH':'mm':'ss",date_time+dtl,sizeof(date_time)-dtl);
      dtl--;
      sprintf(date_time+dtl,",%03d : ",(DWORD)st.wMilliseconds);
      std::cout<<date_time;
     }
     std::cout<<text<<std::endl;
  }
}

void __fastcall TGkOpcDriver::check_use_console()
{
  HWND cw = GetConsoleWindow();
  if(get_use_console())
  {
   if(!cw)
     {
       AllocConsole();
       cw = GetConsoleWindow();
     }
     ShowWindow(cw,SW_SHOW);
  }
  else
  {
     if(cw)
     ShowWindow(cw,SW_HIDE);
  }
}

TOpcSrvConfig::tag_name_mode __fastcall TGkOpcDriver::get_tag_name_mode()
{

 TLockHelper l(config_locker);
 return config.tn_mode;
}

int __fastcall TGkOpcDriver::get_no_clients_work_time()
{
  TLockHelper l(config_locker);
  return config.no_clients_terminate_delay;
}

int __fastcall TGkOpcDriver::get_use_console()
{
  TLockHelper l(config_locker);
  return config.use_console;
}


 void __fastcall TGkOpcDriver::subscribe(const loCaller *ca, int count, loTagPair *til)
 {
   if(count>0)
    {
     write_console("subscribe",true);
     this->send_otd_subscribes(true);
    }
    else
    write_console("unsubscribe",true);

 }

void TGkOpcDriver::convert_tags(const loCaller *ca,
                 unsigned count, const loTagPair taglist[],
                 VARIANT *values, WORD *qualities, HRESULT *errs,
                 HRESULT *master_err, HRESULT *master_qual,
                 const VARIANT src[], const VARTYPE vtypes[], LCID lcid)
{
  TGkOpcDriver * drv = dynamic_cast<TGkOpcDriver*>( TGkOpcDriver::get_driver(ca) );
  if(drv)
     drv->do_convert_tags(ca,count,taglist,values,qualities,errs,master_err,master_qual,src,vtypes,lcid);

}

void   TGkOpcDriver::do_convert_tags(const loCaller *ca,
                 unsigned count, const loTagPair taglist[],
                 VARIANT *values, WORD *qualities, HRESULT *errs,
                 HRESULT *master_err, HRESULT *master_qual,
                 const VARIANT src[], const VARTYPE vtypes[], LCID lcid)
{
 for(unsigned i = 0;i<count;i++)
 {
  if(values->vt == *vtypes)
     VariantCopy(values,src);
  ++taglist;
  ++values;
  ++qualities;
  ++errs;
  ++src;
 }
}

        int TGkOpcDriver::write_tags(const loCaller * ca,
                       unsigned count, loTagPair taglist[], VARIANT values[],
                       HRESULT error[], HRESULT *master_err, LCID lcid)
{
  TGkOpcDriver * drv = dynamic_cast<TGkOpcDriver*>( TGkOpcDriver::get_driver(ca) );
  if(drv)
     return drv->do_write_tags(ca,count,taglist,values,error,master_err,lcid);
  return -1;

}

        int TGkOpcDriver::do_write_tags(const loCaller *,
                       unsigned count, loTagPair taglist[], VARIANT values[],
                       HRESULT error[], HRESULT *master_err, LCID lcid)
{
   char text[256];
   for(unsigned i = 0;i<count;i++)
   {
//      VARIANT result;
//      VariantInit(&result);
//      VariantChangeType(&result,values,0,VT_R4);
      snprintf(text,KERTL_ARRAY_COUNT(text),"write tag %d value %d %.2f %d ",taglist->tpTi,(int)values->boolVal,values->fltVal,values->intVal);
      write_console(text,true);
      ++taglist;
      ++values;
      ++error;
      ++master_err;

   }
   return loDW_ALLDONE;
}



  int __fastcall TGkOpcDriver::service_create   (int tag_count,unsigned Flags,int QueueMax,loMilliSec RefreshRate ,loMilliSec RefreshRateMin , char BranchSep )
  {
    //TODO Установить callbacks

    config_lock();
    if(config.read())
       {
         tag_count       = config.TagsCount;
         QueueMax        = config.MaxQueueCount;
         RefreshRateMin  = config.RefreshRateMin;
         RefreshRate     = config.RefreshRate;
         BranchSep       = config.BranchSep[0];
       }
    config_unlock();
    lo_driver.ldSubscribe   = int_subscribe;
    lo_driver.ldConvertTags = convert_tags;
    lo_driver.ldReadTags    = read_tags;
    lo_driver.ldWriteTags   = write_tags;
    data_lock();
    if(mtags.size())
       mtags.clear();
    data_unlock();
    return TOpcDriver::service_create(tag_count,Flags,QueueMax,RefreshRate,RefreshRateMin,BranchSep);
  }

 int  __fastcall TGkOpcDriver::service_destroy  ()
 {
   return TOpcDriver::service_destroy();
 }

 void __fastcall  TGkOpcDriver::on_change_config()
 {
  this->change_settings->SetEvent(false);

  TOpcSrvConfig new_config;
  new_config.read();
  bool need_reconnect = false;
  bool change_orders = false;
  bool need_recreate_service = false;
  config_lock();
  need_reconnect = stricmp(new_config.otd_server_addr,config.otd_server_addr) || new_config.otd_server_port != config.otd_server_port;
  change_orders  = new_config.order_kadrs.size() != config.order_kadrs.size();
   for(unsigned i = 0;!change_orders && i<new_config.order_kadrs.size();i++)
     change_orders = new_config.order_kadrs[i] != config.order_kadrs[i];
   need_recreate_service = change_orders || new_config.TagsCount > config.TagsCount;
   need_reconnect|=need_recreate_service;

  if(change_orders && !need_reconnect)
     {
     // Изменение пописки
     send_otd_subscribes(false);
     }

   if(need_reconnect)
     set_all_quality(OPC_QUALITY_BAD);

  if(need_recreate_service)
     {

      clients_shutdown(false,"Reconfigure");
      wait_clients_shutdown(2000);
      service_destroy();
     }
   config.read();
   config_unlock();

   if(need_reconnect)
   {
    otd_disconnect();
   }
 }

 void __fastcall  TGkOpcDriver::otd_connect   ()
 {
   mp_buff.clear();
   socket_event = WSACreateEvent();
   ss+=socket_event;
   socket = new TIPSocket(true);
   socket->EnableReuseAddress(true);
   socket->SetSendBuffSize(SEND_BUFFSZ);
   socket->SetRecvBuffSize(SEND_BUFFSZ);

   config_lock  ();
   TIPAddress addr(config.otd_server_addr,config.otd_server_port);
   config_unlock();
   WSAEventSelect(socket->GetSocket(),socket_event,FD_READ|FD_CLOSE|FD_WRITE|FD_CONNECT);
   socket->Connect(addr(),sizeof(addr));
   is_connected = FALSE;
   no_data_timer = 0;
 }

 void __fastcall  TGkOpcDriver::otd_disconnect()
 {
   send_queue->DropData();
   if(socket)
   {
      if(IsValidHandle(socket_event))
      {
         ss-= socket_event;
         WSACloseEvent(socket_event);
         socket_event = INVALID_HANDLE_VALUE;
      }

     socket->Disconnect   ();
     socket->DestroySocket();
     delete socket;
     socket = NULL;
     set_all_quality(OPC_QUALITY_NOT_CONNECTED);
     is_connected = FALSE;
     //set_all_quality(OPC_QUALITY_BAD);
     write_console("Disconnect from otd server");
   }
 }

 DWORD __fastcall TGkOpcDriver::OnException(DWORD ExceptionCode,LPEXCEPTION_POINTERS ptr)
 {
  ptr = ptr;
  char ex_text[512];
  sprintf(ex_text,"Exception 0x%X at %0x%X",ExceptionCode,ptr->ExceptionRecord->ExceptionAddress );
  write_console(ex_text,true);
  Flog.WriteToLog("Exteption in service thread",ex_text);
  return EXCEPTION_EXECUTE_HANDLER;
  //return EXCEPTION_CONTINUE_EXECUTION;
 }

 bool __fastcall  TGkOpcDriver::BeforeExecute  ()
 {

   if(TThread::BeforeExecute())
   {
    if(TBaseSocket::InitWS(MAKEWORD(2,2)))
    {
     term_request    = new TEvent(true,false);
     change_settings = new TEvent(true,false,EVENT_CHANGE_SETTINGS);
     send_queue      = new TEventSimpleDataQueue();
     ss.Clear();
     ss+=*term_request;
     ss+=*change_settings;
     ss+= send_queue->GetEvent();
     otd_connect    ();
     cache_lock();
     return true;
    }
   }
   return false;
 }

 void __fastcall  TGkOpcDriver::BeforeTerminate()
 {

  otd_disconnect();
  cache_unlock(true);
  if(cache)
     delete [] cache;
  cache = NULL;
  cache_size  = 0;
  cache_count = 0;
  if(term_request)
  {
   delete term_request;
   term_request = NULL;
  }

  if(change_settings)
  {
   delete change_settings;
   change_settings = NULL;
  }

  if(send_queue)
  {
   send_queue->DropData();
   delete send_queue;
   send_queue = NULL;
  }

  ss.Clear();
  TBaseSocket::DeinitWS();
 }

    const int SS_TERM_EVENT       = 0;
    const int SS_CH_CONFIG_EVENT  = 1;
    const int SS_QUEUE_EVENT      = 2;
    const int SS_NETWORK_EVENT    = 3;

 int  __fastcall  TGkOpcDriver::Execute()
 {
  int wait_result = 0;
  do{
     wait_result = ss.Wait(1000,false);
     switch(wait_result)
     {
      case SS_TERM_EVENT:  //Запрос на завершение
       break;
      case SS_CH_CONFIG_EVENT: //Изменения в настройках
           on_change_config();
      break;
      case SS_QUEUE_EVENT: //Очередь на передачу не пуста
           do_send();
      break;
      case SS_NETWORK_EVENT: //сеть
      handle_socket_events();
      break;
      default:
         on_timer();
         break;
     }
    }while(!TerminateRequest && wait_result);
  return 0;
 }

 void __fastcall  TGkOpcDriver::on_timer()
 {
  if(has_clients())
   {
    no_clients_timer = 0;

      if(is_connected)
      {
       send_keep_alive();
       if(++no_data_timer > 60 )
         {
           write_console("No receive data more than 60 sec\n\rsend orders",true);
           send_otd_subscribes(true);
           no_data_timer = 0;
         }
      }
      else
      {
       if(++no_data_timer> 5)
         {
          otd_disconnect();
          otd_connect();
         }
      }

   }
   else
   {
    if(++no_clients_timer > config.no_clients_terminate_delay)
      Terminate();//Нет клиентов завершаемся
   }


 }

void __fastcall TGkOpcDriver::client_release    (loClient * cli)
{
    write_console("client release",true);
    write_console(loClientName(cli));
    TOpcDriver::client_release(cli);
    if(!has_clients())
       {
        otd_disconnect();
       }

}


 void __fastcall TGkOpcDriver::handle_socket_events()
 {
   //Обработка событий сети
   WSANETWORKEVENTS ne={0};
    while(socket &&  !WSAEnumNetworkEvents(socket->GetSocket(),socket_event,&ne) && ne.lNetworkEvents)
     {
      if(ne.lNetworkEvents&FD_WRITE)
      {
       ne.lNetworkEvents&=~FD_WRITE;
       if(!ne.iErrorCode[FD_WRITE_BIT])
         kpa_timer = 0;
         if(send_queue->QueueCount()) SetEvent(ss[SS_QUEUE_EVENT]);

      }

      if(ne.lNetworkEvents&FD_READ)
        {
         handle_recv(ne.iErrorCode[FD_READ_BIT]);
         ne.lNetworkEvents&=~FD_READ;
        }


      if(ne.lNetworkEvents&FD_CONNECT)
         {
          handle_connect(ne.iErrorCode[FD_CONNECT_BIT]);
          ne.lNetworkEvents&=~FD_CONNECT;
         }


      if(ne.lNetworkEvents&FD_CLOSE)
      {
        handle_disconnect(ne.iErrorCode[FD_CLOSE_BIT]);
        ne.lNetworkEvents&=~FD_CLOSE;
      }

      if(ne.lNetworkEvents)
      {
       ne.lNetworkEvents = 0;
      }

     }
 }

 void __fastcall TGkOpcDriver::handle_recv      (DWORD err)
 {
   kpa_timer = 0;
   DWORD rd_len;
   do
   {
    socket->IoctlSocket(FIONREAD,&rd_len);
    if(rd_len)
    {
     if(mp_buff.get_free_size()<rd_len) mp_buff.grow(rd_len);
     char * b = mp_buff.end();
     rd_len = socket->Recv(b,rd_len,0);
     mp_buff.add(b,rd_len);
    }
   }while(rd_len);

   /*Вычитали из гнезда, начинаем разбор*/

   do{
      DWORD sync_offs = mp_buff.sync_internal(MP_INTERNAL_DEFAULT);
      if(sync_offs)
       {
         //Рассинхронизация
       }
      rd_len = mp_buff.get_mproto_len();
      if(rd_len)
      {
       DWORD err = 0;
       LPMPROTO_HEADER mph = (LPMPROTO_HEADER)mp_buff.begin();
       if(mproto_check(mph))
       {
       if(recv_pkt_num )
       {
         if(abs(int(mph->pkt_num-recv_pkt_num))>1 )
         {
          //Потеря пакета
          //
          ++err;
         }
       }
       recv_pkt_num = mph->pkt_num;

       if(mph->fa) // Исключить тестовую посылку кадра
         {
          handle_recv_mproto(mph);
         }
       }
       else
       {
        printf("Recv Error check sum \n\r");
       }
       mp_buff.move_mproto();
      }
     }while(rd_len);
 }

  void __fastcall TGkOpcDriver::handle_recv_mproto(LPMPROTO_HEADER mph)
  {
      if(mph->fa == FA_OTD_MEDIUM)
      {
       no_data_timer = 0;
       LPOTDM_PROTO_HEADER omph = (LPOTDM_PROTO_HEADER)mph->data;
       bool is_dbg = IsDebuggerPresent();
       if(!OTDM_PROTO_IS_ERROR(omph))
       {
       if(omph->command&OTDM_RESPOND)
        {
           switch(omph->command&OTDM_COMMAND_MASK)
           {
             case OTDMPROTO_CMD_GET_RECORD_NAME :
             case OTDMPROTO_CMD_GET_RECORD_SNAME:
                  this->handle_record_name((LPOTDM_PROTO_RECORD_NAME)omph->data);
                  break;
             case OTDMPROTO_CMD_ENUM_KADRS:
                  handle_get_kadr((LPOTDM_PROTO_KADR)omph->data,omph->data_size);

             break;
             case OTDMPROTO_CMD_GETKADR  :
                  handle_get_kadr((LPOTDM_PROTO_KADR)omph->data,omph->data_size);

             break;
             case OTDMPROTO_CMD_GETKADR_ENTRYES:
                  handle_get_kadr_entryes((LPOTDM_PROTO_KADR_ENTRYES)omph->data);

             break;

             case OTDMPROTO_CMD_GETKADR_RECORDS:
                  handle_get_kadr_records((LPOTDM_PROTO_KADR_RECORDS)omph->data);
             break;
             case OTDMPROTO_CMD_RECORDS:
                  handle_changed_record((LPOTDM_PROTO_RECORDS)omph->data,omph->data_size);
             break;
             default :
               if(is_dbg)
               {
                char text[1024];
                sprintf(text,"not handled OTD command %d ", int(omph->command&OTDM_COMMAND_MASK));
                write_console(text,true);
               }

              break;
           }
        }
       }
       else
       {
         //Ошибка
       }
     }
  }


void __fastcall TGkOpcDriver::handle_get_kadr_entryes( LPOTDM_PROTO_KADR_ENTRYES kes)
{
   bool is_dbg = IsDebuggerPresent();
  if(is_dbg)
 {
  char text[1024];
  sprintf(text,"handle get kadr entryes kadr_id %d, entry count %d",kes->kadr_id,kes->entryes_count);
  write_console(text,true);
 }

 LPMDB_KADR_ENTRY ke_beg = kes->entryes;
 LPMDB_KADR_ENTRY ke_end = ke_beg+kes->entryes_count;
 data_lock();
 while(ke_beg<ke_end)
   {

    set_rec_quality(ke_beg->rec_id,OPC_QUALITY_NOT_CONNECTED);
    ++ke_beg;
   }
  data_unlock();
}


void __fastcall TGkOpcDriver::handle_get_kadr        ( LPOTDM_PROTO_KADR pk,DWORD data_size)
{
 bool is_dbg = IsDebuggerPresent();
 if(is_dbg)
 {
  char text[1024];
  sprintf(text,"handle get_kadr id %d , entry count %d, alarm %d, discrete %d, changes %d ",pk->kadr.kadr_id,pk->kadr.entry_count,pk->kadr.alarm_changes,pk->kadr.discrete_changes,pk->kadr.kadr_changes);
  write_console(text,true);
 }
 pk = pk;
}

Tmdb_records::Trecords::iterator  __fastcall Tmdb_records::find_record(DWORD rec_id )
  {
   using namespace std;
   Trecords::iterator beg = records.begin();
   Trecords::iterator end = records.end();

   beg = lower_bound( beg,end,Trecords::value_type(rec_id),mdb_record_id_less());
   if(beg<end && beg->rec_id == rec_id )
      {
       return beg;
      }
   return end;
 }

Tmdb_records::Trecords::iterator   __fastcall Tmdb_records::add_record (mdb_short_record & rec)
  {
   using namespace std;
   Trecords::iterator beg = records.begin();
   Trecords::iterator end = records.end();
      beg = lower_bound( beg,end,rec,mdb_record_id_less());
   if(beg<end )
      {
       if(beg->rec_id!= rec.rec_id);
          beg = records.insert(beg,rec);
      }
      else
      beg = records.insert(beg,rec);

   return beg;
 }

void __fastcall get_tag_value(LPMDB_RECORD  sr,loTagValue & tv,DWORD mask)
{
         VARIANT & v = tv.tvValue;

         if(mask&MDBR_FIELD_TIME)
           tv.tvState.tsTime = *((LPFILETIME)&sr->time);

         if(mask & MDBR_FIELD_VALUE)
         {
             if(sr->addr.addr.fa == OTD_FA_DISCRETE)
             {
              v.vt = VT_BOOL;
              v.boolVal = (short)sr->value;
             }
             else
             {
              v.vt     = VT_R4;
              v.fltVal = sr->value;
             }
         }

        tv.tvState.tsError   = S_OK;


        if( !(mask & MDBR_FIELD_DIAG) || !sr->diag)
            tv.tvState.tsQuality = OPC_QUALITY_GOOD;
           else
            tv.tvState.tsQuality = (sr->diag&MDBR_DIAG_BAD) ? OPC_QUALITY_BAD : OPC_QUALITY_BAD  ;
}


void __fastcall TGkOpcDriver::handle_record(LPMDB_RECORD ks_rec,DWORD mask)
  {
    Tmdb_records::Trecords::iterator rec = records.find_record(ks_rec->rec_id);
    if(rec<records.end())
    {
      //Находится в списке ожидающих передачи имени
      if(mask&MDBR_FIELD_TIME)
         rec->time = ks_rec->time;
      if(mask & MDBR_FIELD_DIAG)
         rec->diag = ks_rec->diag;
      if(mask & MDBR_FIELD_VALUE)
         rec->value = ks_rec->value;
         ks_rec->addr = rec->addr;

    data_lock();
    int tvTi   = this->mtags[rec->rec_id];
    data_unlock();
    cache_lock();
        if(tvTi )
        {
         __try{
                   loTagValue * tv = get_cache_pointer(tvTi);
                   tv->tvTi = tvTi;
                   get_tag_value(&*rec,*tv,-1);

              }
              __except(EXCEPTION_EXECUTE_HANDLER)
              {
               cache_unlock(false);
               write_console("Exception catched",true);
              }
        }
    }
  }

  void __fastcall TGkOpcDriver::handle_record_name     (LPOTDM_PROTO_RECORD_NAME rn)
  {
     Tmdb_records::Trecords::iterator rec = records.find_record(rn->rec_id);
     if(rec<records.end())
     {
        mdb_short_record sr(*rec);
        //records.remove     (rec );

        char  tmp_buff[1024];
        char * name = tmp_buff;
        if(rn->len)
        {
        wstring ws (rn->text,rn->len);
        if(rn->len+1>sizeof(tmp_buff))
           name = new char[rn->len+1];
        KeRTL::Unicode2Ansi(name,ws.c_str());

        }
        else
        sprintf(name,"rec_id = %u",sr.rec_id);

        loTagId tid = add_tag(&sr,name);
        if(!tid)
            {
             sprintf(name,"[%u]",sr.rec_id);
             tid = add_tag(&sr,name);
            }
        if(name && name!= tmp_buff)
           delete [] name;
        handle_record(&sr,-1);
        cache_unlock(true);
     }
  }

  loTagId __fastcall TGkOpcDriver::add_tag(LPMDB_RECORD sr,char * tg_name)
  {
    loTagId tid = 0;
    VARIANT v = {0};
             if(sr->addr.addr.fa == OTD_FA_DISCRETE)
             {
              v.vt = VT_BOOL;
             loAddRealTag(service,&tid,(loRealTag)sr->rec_id,tg_name,0
                          ,OPC_READABLE | ((sr->state & MDBR_STATE_TUFLAG) ? OPC_WRITEABLE : 0)
                          ,&v ,0,0);
             }
             else
              {

              v.vt = VT_R4;
              loAddRealTag_a(service,&tid,(loRealTag)sr->rec_id,tg_name,0
                          ,OPC_READABLE | ((sr->state & MDBR_STATE_TUFLAG) ? OPC_WRITEABLE : 0)
                          ,&v,sr->min_value,sr->max_value);
              }

   if(tid)
   {
    data_lock();
     mtags[sr->rec_id] = tid;
    data_unlock();
    loTagValue * tv = get_cache_pointer(tid);
    get_tag_value(sr,*tv,-1);
    char text[2048];

    sprintf(text,"ADD TAG  rec_id %d [%d]  tg_name %s ",sr->rec_id,tv->tvTi,tg_name ? tg_name : "");
    write_console(text);
   }
   else
    {
     char text[256];
     sprintf(text,"Error adding tag %d",sr->rec_id);
     this->write_console(text);
     //std::cout<<"Error adding tag "<<sr->rec_id<<std::endl;
    }
   return tid;
  }

  void __fastcall TGkOpcDriver::handle_records(LPMDB_RECORD ks_rec,LPMDB_RECORD ks_rec_end )
  {

   VARIANT v;

   FILETIME ft;
   GetSystemTimeAsFileTime(&ft);
   data_lock();
   while(ks_rec<ks_rec_end)
   {
     std::map<DWORD,loTagId>::iterator p = mtags.find(ks_rec->rec_id);
     if(mtags.end() == p)
       {
         //не найдено
         TOpcSrvConfig::tag_name_mode tnm = this->get_tag_name_mode();
         records.add_record(*&mdb_short_record(*ks_rec));

         if(tnm == TOpcSrvConfig::tag_long_name || tnm == TOpcSrvConfig::tag_short_name)
         {
           send_record_query_name(ks_rec->rec_id,(tnm == TOpcSrvConfig::tag_short_name) ? false:true);
         }
         else
         {
             char  tg_name[MAX_PATH];
             if(tnm == TOpcSrvConfig::tag_name_otd_addr)
             {
             int tlen = sprintf(tg_name,"%03d.%03d.%03d.%03d",int(ks_rec->addr.addr.pu),int(ks_rec->addr.addr.cp),int(ks_rec->addr.addr.fa),int(ks_rec->addr.addr.sb));
             if(DWORD( -1) != ks_rec->addr.param_number )
                sprintf(tg_name+tlen,"-%03u",unsigned(ks_rec->addr.param_number));
                else
                sprintf(tg_name+tlen,"-ALL",unsigned(ks_rec->addr.param_number));
             }
             else
             sprintf(tg_name,"%u",ks_rec->rec_id);

             add_tag(ks_rec,tg_name);
        }
       }
    handle_record(ks_rec,-1);
    ++ks_rec;
   }
   data_unlock();
   cache_unlock(true);
  }

  void __fastcall TGkOpcDriver::handle_get_kadr_records( LPOTDM_PROTO_KADR_RECORDS kadr_records)
  {
    bool is_dbg = IsDebuggerPresent();
    if(is_dbg)
     {
      char text[1024];
      sprintf(text,"handle get kadr records kadr_id %d, count %d", kadr_records->kadr_id,kadr_records->rec_count);
      write_console(text,true);
     }

   LPMDB_RECORD ks_rec     =  kadr_records->records;
   LPMDB_RECORD ks_rec_end =  ks_rec+kadr_records->rec_count;
   handle_records(ks_rec,ks_rec_end);
  }

  void __fastcall TGkOpcDriver::handle_changed_record( LPOTDM_PROTO_RECORDS recs,int data_size)
  {

    bool is_dbg = IsDebuggerPresent();
    if(is_dbg)
     {
      char text[1024];
      sprintf(text,"handle changed records count %d", recs->rec_count);
      write_console(text,true);
     }

    if(recs->rec_count)
     {
      mdb_record rec;
      DWORD  unpacked_len;
      data_size -= sizeof(recs->rec_count);
      LPBYTE src_ptr = (LPBYTE)recs->records;

      for(DWORD i = 0;i<recs->rec_count;i++)
      {
       LPOTDM_PROTO_RECORD proto_rec = (LPOTDM_PROTO_RECORD) src_ptr;
       mdb_short_record  sr;


       DWORD mask = proto_rec->mask;
       data_size -= sizeof(proto_rec->mask);
       src_ptr  = (LPBYTE)proto_rec->rec_data;
       rec.rec_id   = proto_rec->rec_data[0];

       unpacked_len = otdm_unpack_record(src_ptr,data_size,mask,&rec);
       src_ptr+=unpacked_len;
       data_size-=unpacked_len;
       sr = rec;
       handle_record(&sr,mask);
       #ifdef _DEBUG
         //std::cout<<"Change record id "<<sr.rec_id<<" value ( "<<sr.value<<" ) diag [ "<<sr.diag<<" ]"<<std::endl;
       #endif

       //add_record(rec,mask);
       //TODO обработать изменения имен
      }
     }
     cache_unlock(true);
}
#pragma warn .8004

 void __fastcall TGkOpcDriver::set_rec_quality(DWORD rec_id,int quality)
 {


   Tmdb_records::Trecords::iterator rec = records.find_record(rec_id);
   if(rec<records.end())
   {
   maptags::iterator itr = mtags.find(rec_id);
   if(itr!=mtags.end())
   {
     data_lock();
     if(!cache) cache_lock();
     loTagId tid = (*itr).second;
     loTagValue * tv = get_cache_pointer(tid);
     get_tag_value(&*rec,*tv,-1);
     tv->tvTi = tid;
     tv->tvState.tsQuality = quality;
     data_unlock();
   }

   }
 }

 void __fastcall TGkOpcDriver::set_all_quality(int quality)
 {
   char text[128];
   sprintf(text,"set all tags qaulity %X",quality);
   write_console(text,true);
   cache_lock(true);
   data_lock();
   Tmdb_records::Trecords::iterator brec = records.begin(),erec = records.end();
   while(brec<erec)
   {
     loTagId tid = mtags[brec->rec_id];
     if(tid)
     {
      loTagValue * tv = get_cache_pointer(tid);
      tv->tvTi = tid;
      get_tag_value(&*brec,*tv,-1);
      tv->tvState.tsQuality = quality;
     }
     ++brec;
   }
   data_unlock();
   cache_unlock(true);
 }

 void __fastcall TGkOpcDriver::handle_disconnect(DWORD err)
 {
   if(is_connected)
   {
    char text [1024];
    config_lock();
    sprintf(text,"Connection with otd server IP [%s] closed",config.otd_server_addr);
    config_unlock();
    write_console(text,true);
   }

   send_queue->DropData();
   otd_disconnect();

 }

 void __fastcall TGkOpcDriver::handle_connect   (DWORD err)
 {
   if(err)
      handle_disconnect(err);
      else
       {
        send_pkt_num = 0;
        kpa_timer    = 0;
        recv_pkt_num = 0;
        no_data_timer = 0;
        send_queue->DropData();
        mp_buff.clear();
        char text [1024];
        config_lock();
        sprintf(text,"Connection with otd server IP [%s] established",config.otd_server_addr);
        config_unlock();
        write_console(text,true);
        is_connected = TRUE;
        send_otd_subscribes(true);

       }
 }



 void __fastcall TGkOpcDriver::do_send()
 {
  unsigned  char   buff [SEND_BUFFSZ];
  unsigned  char * ptr = buff;
  int data_len  = 0;
  int send_len  = 0;
  int last_error = 0;
  send_queue->Lock();
  if(socket  )
  {
      while(send_queue->QueueCount() && !last_error)
      {
       if(!send_queue->PeekFromQueue(ptr,sizeof(buff),&data_len))
          {
           if(ptr && ptr!=buff)  delete [] ptr;
           ptr = new unsigned char[data_len];
           send_queue->PeekFromQueue(ptr,data_len,0);
          }
        send_queue->DropFirst();
        send_len = socket->Send(ptr,data_len,0);
        if(send_len != data_len)
           last_error = socket->GetLastError();
      }
  }
  else
  otd_disconnect();
  send_queue->Unlock();
  if(ptr!=buff)
     delete [] ptr;
  if(last_error &&  WSAEWOULDBLOCK != last_error)
       otd_disconnect();

 }

 void __fastcall TGkOpcDriver::send_keep_alive()
 {
  if(socket )
  {

   if((++kpa_timer)%5==0 )
   {
    if(! this->send_queue->QueueCount())
    {
     MPROTO_HEADER mph = {0};
     mph.addr_from.line  = 0;
     mph.addr_from.modem = 0;
     mph.addr_from.reserv1 = -1;
     mph.data_size = sizeof(mph.data);
     send(&mph,sizeof(mph));
    }
    else
     otd_disconnect();
   }

  }
 }


 bool  __fastcall TGkOpcDriver::send        (LPMPROTO_HEADER mph,DWORD sz)
 {
   mph->internal = MP_INTERNAL_DEFAULT;//0xAAAA5555;
   mph->pkt_num  =  ++send_pkt_num;
   mproto_protect(mph);
   DWORD  mp_size = MPROTO_SIZE(mph);
   LPBYTE ptr     = (LPBYTE)mph;
   send_queue->Lock();
   while(mp_size)
   {
    DWORD put_sz = KeRTL::MIN((DWORD)SEND_BUFFSZ,mp_size);
    send_queue->PutIntoQueue(ptr,put_sz);
    ptr+=put_sz;
    mp_size-=put_sz;
   }
   send_queue->Unlock();
   return true;
 }


void __fastcall  TGkOpcDriver::send_otd_subscribe(int kadr_id, bool subscr)
{
  BYTE buffer[MAX_PATH];
  LPMPROTO_HEADER mph =(LPMPROTO_HEADER)buffer;
  LPOTDM_PROTO_HEADER omph;
  omph = (LPOTDM_PROTO_HEADER)mph->data;
  ZeroMemory(mph,sizeof(*mph)+sizeof(*omph));
  mph->fa = FA_OTD_MEDIUM;
  mph->internal   = MP_INTERNAL_DEFAULT;
  omph->command   = OTDMPROTO_CMD_GETKADR|OTDM_SUBSCRIBE;
  mph->data_size  = sizeof(*omph)+sizeof(DWORD);
  omph->data[0]   = kadr_id;
  omph->data[1]   = subscr ? -1 : 0;
  omph->data_size = sizeof(DWORD)<<1;
  send(mph,MPROTO_SIZE(mph));
}

void __fastcall TGkOpcDriver::send_otd_subscribes(bool subscr)
{
 write_console(subscr ? "send kadrs subscribe" : "send kadrs unsubcribe",true);
 config_lock();
 TOpcSrvConfig::Torder_kadrs::iterator beg = config.order_kadrs.begin();
 TOpcSrvConfig::Torder_kadrs::iterator end = config.order_kadrs.end();
 while(beg<end)
 {
  send_otd_subscribe(*beg,subscr);
  ++beg;
 }
 config_unlock();
}

void __fastcall TGkOpcDriver::send_record_query_name(DWORD rec_id,bool long_name)
{


  BYTE buffer[MAX_PATH];
  LPMPROTO_HEADER mph =(LPMPROTO_HEADER)buffer;
  LPOTDM_PROTO_HEADER omph;
  omph = (LPOTDM_PROTO_HEADER)mph->data;
  ZeroMemory(mph,sizeof(*mph)+sizeof(*omph));
  mph->fa = FA_OTD_MEDIUM;
  mph->internal   = MP_INTERNAL_DEFAULT;
  omph->command   = long_name ? OTDMPROTO_CMD_GET_RECORD_NAME : OTDMPROTO_CMD_GET_RECORD_SNAME;
  mph->data_size  = sizeof(*omph);//+sizeof(DWORD);
  omph->data[0]   = rec_id;
  //omph->data[1]   = order ? -1 : 0;
  omph->data_size = sizeof(DWORD);
  send(mph,MPROTO_SIZE(mph));

}

   loTrid       __fastcall TGkOpcDriver::cache_unlock(bool wait)
   {
     loTrid ret = 0;
     __try{
     if(cache && cache_count)
      {
       loTrid  trid = cache_update(cache_count,cache);
       cache_count = 0;
       if(wait)
         trans_wait(trid);
      }
      return ret;
     }
     __except(EXCEPTION_EXECUTE_HANDLER)
     {
      write_console("exception in cache_unlock ");
     }
     return ret;
   }

   bool         __fastcall TGkOpcDriver::cache_lock(bool flush )
   {
     bool ret = false;
     if(cache && flush)
        cache_unlock(true);
     if(!cache)
     {
     __try{
          cache_size = CACHE_SIZE;
          cache = new loTagValue[cache_size];
          memset(cache,0,sizeof(*cache)*cache_size);
          cache_count = 0;
          ret = true;
          return ret;
       }
     __except(EXCEPTION_EXECUTE_HANDLER)
     {
      cache_size  = 0;
      cache_count = 0;
      write_console("exception in cache_lock ");
     }
     }
     return ret;

   }

   int          __fastcall TGkOpcDriver::trans_wait(loTrid id)
   {
     int ret = 0;
     __try{
     ret = TOpcDriver::trans_wait(id);
      return ret;
     }
     __except(EXCEPTION_EXECUTE_HANDLER)
     {
      write_console("exception in trans_wait");
     }
     return ret;

   }

   loTrid       __fastcall TGkOpcDriver::cache_update(int count,loTagValue * tv)
   {

     loTrid ret = 0;
     if(count && tv)
     {
         __try{

          ret = TOpcDriver::cache_update(count,tv);
          cache_count = 0;
         }
     __except(EXCEPTION_EXECUTE_HANDLER)
     {
      write_console("exception in cache_update ");
     }
     }
     return ret;
   }

   loTagValue * __fastcall TGkOpcDriver::get_cache_pointer(loTagId tag_id)
   {
     if(!cache)
        cache_lock();
        if(this->cache_count<cache_size)
        {
         loTagValue *ret = cache+cache_count;
         cache_count++;
         ret->tvTi  = tag_id;
         return ret;

        }
        else
        {
          trans_wait(cache_update(cache_count,cache));
          cache_count = 0;
          return get_cache_pointer(tag_id);
        }
         /*
         loTagValue *ret = cache+tag_id;
         ret->tvTi  = tag_id;
         return ret;
        */
   }





