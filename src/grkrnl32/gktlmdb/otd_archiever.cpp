/*Обработчик архивов*/
#pragma hdrstop
#include "gktlmdb_mod.hpp"


class TOtdArchiever : public TGKThread
{
  protected:
  struct Tarch_list:public std::vector<QWORD>
  {
  QWORD download_archive;
  Tarch_list(){download_archive = 0;}
  };

  typedef  std::vector<modem_addr>          Tshceduled;
  typedef  std::map<modem_addr,Tarch_list>  Tarch_db;

  Tarch_db arch_db;
  Tshceduled shceduled;
  TFASTMutex locker;
  TGKTlmDB * owner;
  TEvent     term_event;
  TEventSimpleDataQueue  dt_queue;
  rx_buffer              rxb;
  int                    timer_counter;
  TOtdArchiever(){};
  void __fastcall handle_data();
  void __fastcall handle_arch_command(modem_addr & ma,lpotd_addr addr,LPOTD_ARCH_HEADER ah,DWORD error);
  void __fastcall add_archieves(LPOTD_ARCH_LIST  al,Tarch_list & lst);
  void __fastcall start_download_archieves(modem_addr & ma);
  int  __fastcall Execute();
  bool __fastcall BeforeExecute();
  void __fastcall BeforeTerminate();
  void __fastcall shcedule_get_archive(const modem_addr & ma);
  void __fastcall on_timer();
  void __fastcall send_archieve_data(sotd_proto & sop);
  int  __fastcall print_arch_time(wchar_t * text,int bsz,QWORD timestamp);
  public:
  TOtdArchiever(TGKTlmDB * _owner);
  LRESULT __fastcall handle(LPMPROTO_HEADER mph);
  LRESULT __fastcall modem_line_connect (modem_addr & ma, bool connect,MODEM_PROPS & mp);
  void    __fastcall Terminate(){term_event.PulseEvent();TGKThread::Terminate();}


};


TOtdArchiever::TOtdArchiever(TGKTlmDB * _owner)
              :owner(_owner)
              ,TGKThread(_owner)
{
  rxb.set_size(1024);
}

bool __fastcall TOtdArchiever::BeforeExecute()
{
    term_event.SetEvent(false);
    arch_db.clear();
    timer_counter = 0;
    return TGKThread::BeforeExecute();
}

void __fastcall TOtdArchiever::BeforeTerminate()
{
  arch_db.clear();
  TGKThread::BeforeTerminate();
}


int __fastcall TOtdArchiever::Execute()
{
  TSynchroSet ss;
  ss+=term_event;
  ss+=dt_queue.GetEvent();
  while(!TerminateRequest)
  {
    switch(ss.Wait(1000,false))
    {
     case 0: TGKThread::Terminate();
     break;
     case 1: handle_data();
     break;
     default : on_timer();
     break;
    }
  }
  return 0;
}


  void __fastcall TOtdArchiever::on_timer()
  {
     if(shceduled.size())
        {
         start_download_archieves(shceduled.at(0));
         shceduled.erase(shceduled.begin());
         timer_counter = 0;
        }
        else
        {
         //Пробежаться по списку
         if(++timer_counter>5)
         {
             timer_counter = 0;
             Tarch_db::iterator beg = arch_db.begin();
             Tarch_db::iterator end = arch_db.end  ();
             while(beg!=end)
             {
              if(!beg->second.download_archive && beg->second.size())
                 {
                  shcedule_get_archive(beg->first);
                  break;
                 }
              ++beg;
             }
         }
        }
  }


LRESULT __fastcall TOtdArchiever::handle(LPMPROTO_HEADER mph)
{
   sotd_proto sop((LPBYTE)mph->data,mph->data_size);
   if(otd_proto_check(&sop.op,0,OTD_DEF_POLINOM))
   {
    dt_queue.PutIntoQueue(mph,mproto_size(mph),false);
    return GKH_RET_SUCCESS;
   }
  return false;

}

LRESULT __fastcall TOtdArchiever::modem_line_connect (modem_addr & ma, bool connect,MODEM_PROPS & mp)
{
  LRESULT ret = GKH_RET_SUCCESS;
  TLockHelper l(locker);
  if(mp.flags & MPF_DATA_SOURCE)
  {
      if(connect)
      {
       //Активизировась линия
       //Отправляем вызов списка архивов

       BYTE buf[1024];
       LPMPROTO_HEADER mph = (LPMPROTO_HEADER)buf;
       DWORD sz    = sizeof(buf)-sizeof(*mph)+sizeof(mph->data[0]);
       sotd_proto sop;
       if(otd_arch_proto_format((LPBYTE)mph->data,sz,-1,-1,OTD_ARCH_CMD_ALARM_ARCHIEVES_LIST,0,NULL,0,&sz,&sop.op))
       {
        mproto_init(mph,0,FA_OTD_ALARM_ARCHIVE,sop.op.proto_size,&ma);
        mproto_protect(mph);
        owner->send(mph);
       }
      }
      else
      {
        //связь потеряна, удаляем из списка
        arch_db.erase(ma);
      }
  }
  return  ret;
}

void __fastcall TOtdArchiever::handle_data()
{
  while(!TerminateRequest && this->dt_queue.QueueCount())
  {
    int need_sz = 0;
    if(!dt_queue.GetFromQueue(rxb.begin(),rxb.get_free_size(),&need_sz))
      {
        need_sz = dt_queue.TopDataSize();
        need_sz<<=1;
        rxb.grow(need_sz);
        dt_queue.GetFromQueue(rxb.begin(),rxb.get_free_size(),&need_sz);
      }


    LPMPROTO_HEADER mph = (LPMPROTO_HEADER) rxb.begin();
    modem_addr ma(mph->addr_from);

    sotd_proto  sop((LPBYTE)mph->data,mph->data_size);
    DWORD error =  otd_arch_proto_get_error(&sop.op);
      send_archieve_data(sop);
     if(sop.op.extension)
     {
      LPOTD_ARCH_HEADER ah = (LPOTD_ARCH_HEADER)sop.op.extension->block_data;
      handle_arch_command(ma,sop.op.addr,ah,error);
      send_archieve_data(sop);
     }

  }
}

int  __fastcall TOtdArchiever::print_arch_time(wchar_t * text,int bsz,QWORD timestamp)
{
  SYSTEMTIME st;
  FileTimeToSystemTime((LPFILETIME)&timestamp,&st);
  return snwprintf(text,bsz,L" %hd-%02hd-%02hd %02hd:%02hd:%02hd.%03hd",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond,st.wMilliseconds);
}

void __fastcall TOtdArchiever::handle_arch_command(modem_addr & ma,lpotd_addr addr,LPOTD_ARCH_HEADER ah,DWORD error)
{
  wchar_t buf[1024];
  //bool   is_respond     = OTD_ARCH_CMD_IS_RESPOND(ah->cmd);
  bool   is_end         = OTD_ARCH_CMD_IS_END(ah->cmd);

  LPOTD_ARCH_LIST  al = (LPOTD_ARCH_LIST)ah->data;
  TLockHelper l(locker);
  switch(OTD_ARCH_CMD(ah->cmd))
  {
   case OTD_ARCH_CMD_ALARM_ARCHIEVES_LIST:
   void alarm_add_archieves(LPOTD_ARCH_LIST al,bool is_end );
        alarm_add_archieves(al,is_end );
   break;
   case OTD_ARCH_CMD_ALARM_ARCHIEVES_CREATED:
        {
        add_archieves(al,lst );
        int len = snwprintf(buf,KERTL_ARRAY_COUNT(buf),L"Новый архив аварии %03d.%03d",(int)ma.modem,(int)ma.line);
        print_arch_time(buf+len,KERTL_ARRAY_COUNT(buf)-len,al->archive[0]);
        owner->archive_report(REPORT_WARNING_TYPE,buf);
        if(lst.size() == 1 )
           start_download_archieves(ma);
        }
   break;
   case OTD_ARCH_CMD_ALARM_ARCHIEVES_GET:
   case OTD_ARCH_CMD_ALARM_ARCHIEVES_GET_AND_ERASE:
        {

         if(is_end)
         {
          //Отправить уведомление о завершении приёма архива

          if(lst.size())
            {
             QWORD arch_id = *lst.begin();
             if(arch_id == al->archive[0])
             {
               if(error)
               {
                int len = snwprintf(buf,KERTL_ARRAY_COUNT(buf),L"Ошибка запроса архива %u %03d.%03d",error,(int)ma.modem,(int)ma.line);
                print_arch_time(buf+len,KERTL_ARRAY_COUNT(buf)-len,arch_id);
                owner->archive_report(REPORT_ERROR_TYPE,buf);
                //shcedule_get_archive(ma);
               }
               else
               {
               int len = snwprintf(buf,KERTL_ARRAY_COUNT(buf),L"Прием архива аварии завершен %03d.%03d",(int)ma.modem,(int)ma.line);
               print_arch_time(buf+len,KERTL_ARRAY_COUNT(buf)-len,arch_id);
               owner->archive_report(REPORT_INFORMATION_TYPE,buf);
                 {
                  lst.erase(lst.begin());
                  start_download_archieves(ma);
                 }
               }
             }
            }
          }
        }
    break;
  }
}

 void __fastcall TOtdArchiever::add_archieves(LPOTD_ARCH_LIST  al,Tarch_list & lst)
 {
    TLockHelper l(locker);
    Tarch_list::iterator beg  = al->archive;
    Tarch_list::iterator end = beg;
    std::advance(end,al->count);
    lst.insert(lst.end(),beg,end);
 }

 void __fastcall TOtdArchiever::start_download_archieves(modem_addr & ma)
 {
   wchar_t buf[1024];
   LPMPROTO_HEADER mph = (LPMPROTO_HEADER)buf;
   DWORD sz = sizeof(buf)-sizeof(*mph)+sizeof(mph->data[0]);
   QWORD arch_id = 0;
   TLockHelper l(locker);
   Tarch_list & lst = arch_db[ma];
   if(lst.size())
       arch_id = lst.at(0);
   lst.download_archive = arch_id;
   sotd_proto sop;
   if(arch_id && otd_arch_proto_format_list((LPBYTE)mph->data,sz,-1,-1,OTD_ARCH_CMD_ALARM_ARCHIEVES_GET_AND_ERASE,1,&arch_id,0,&sz,&sop.op))
   //if(arch_id && otd_arch_proto_format_list((LPBYTE)mph->data,sz,-1,-1,OTD_ARCH_CMD_ALARM_ARCHIEVES_GET,1,&arch_id,0,&sz,&sop.op))
   {

    l.unlock();
    mproto_init   (mph,0,FA_OTD_ALARM_ARCHIVE,sz,&ma);
    mproto_protect(mph);
    owner->send   (mph);
    int len = snwprintf(buf,KERTL_ARRAY_COUNT(buf),L"Запрос архива аварии отправлен %03d.%03d",(int)ma.modem,(int)ma.line);
    print_arch_time(buf+len,KERTL_ARRAY_COUNT(buf)-len,arch_id);
    owner->archive_report(REPORT_INFORMATION_TYPE,buf);
   }
 }

void __fastcall TOtdArchiever::shcedule_get_archive(const modem_addr & ma)
{
  shceduled.push_back(ma);
}

void __fastcall TOtdArchiever::send_archieve_data(sotd_proto & sop)
{
  BYTE __buffer[8192];
  DWORD sz = sizeof(__buffer);
  LPBYTE buff = __buffer;

  order_list olist;
  TOrders & orders = owner->get_archive_orders();
  {
      //TOrders::rd_locker rdl(orders);
      if(orders.get_order_addrs(*sop.op.addr,(*sop.op.ver_parts)&OTD_PROTO_PARTSMASK,olist))
      {
       LPMPROTO_HEADER mph;
       DWORD need_sz = sop.op.proto_size+sizeof(*mph)-sizeof(mph->data[0]);
       if(sz<need_sz)
        {
          sz = need_sz;
          buff = new BYTE [need_sz];
        }
        mph = (LPMPROTO_HEADER)buff;
        mproto_init(mph,0,FA_OTD_ALARM_ARCHIVE,sop.op.proto_size);
        memcpy(mph->data,sop.op.addr,sop.op.proto_size);
        order_list::iterator obeg = olist.begin() ,oend = olist.end();
       while(obeg<oend)
       {
            mph->addr_to.addr = obeg->addr;
            mproto_protect(mph);
            owner->send(mph);
            ++obeg;
       }
      }
  }
 if(buff && buff!=__buffer) delete [] buff;
}






