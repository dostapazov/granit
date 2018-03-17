#pragma hdrstop
#include "modemmer_main.hpp"
#include <grkernel.h>


modem_ref::modem_ref(TModemmer * _mdm,TModemInterface * _modem_iface)
:modemmer(_mdm),modem_iface(_modem_iface)
{
buffer_size = 1024;
buffer = new char[buffer_size];
scan_lines();
}

bool              __fastcall modem_ref::check_buffer(DWORD need_size)
{
 bool ret = (buffer && buffer_size>=need_size)? true:false;
 if(!ret)
 {
  if(buffer)
    delete [] buffer;
  buffer = NULL;
  buffer_size = 0;
  buffer  = new char[need_size];
  ret = buffer ? true:false;
  if(ret)
     buffer_size  = need_size;
 }
return ret;   
}

void              __fastcall modem_ref::release()
{

stop();
clear_lines();
if(buffer)
  delete [] buffer;
}

void __fastcall modem_ref::clear_tx_queue(modem_line_ref * mlr)
{
  mlr->clear();
}

void              __fastcall modem_ref::clear_lines()
{
 TLockHelper l(locker);
	lines_map::iterator begin = lines.begin(),end = lines.end();
	while(begin!=end)
	{
	 modem_line_ref * mlr = begin->second;
	 clear_tx_queue(mlr);
	 delete mlr;
	 begin->second = NULL;
	 begin++;
	}
 lines.clear();
}


modem_line_ref*   __fastcall modem_ref::find_line(DWORD num,bool create)
{
modem_line_ref *ret = NULL;

 {
  TLockHelper l(locker);
  if(lines.count(num))
	  ret = lines[num];
 }
if(!ret && create)
  {
   MODEM_LINE_PROPS mp;
   ZeroMemory(&mp,sizeof(mp));
   mp.dw_size = sizeof(mp);
   LRESULT res = modem_iface->get_line_props(num,&mp);
   TLockHelper l(locker);
   if(res == GKH_RET_SUCCESS )
	{
	 ret = new modem_line_ref(&mp);
	 lines[num] = ret;
	}
  }
return  ret;
}

void     __fastcall modem_ref::scan_lines()
{
   MODEM_LINE_PROPS mp;
   ZeroMemory(&mp,sizeof(mp));
   mp.dw_size = sizeof(mp);
   DWORD idx = 0;
   while(modem_iface->enum_lines(idx++,&mp)==GKH_RET_SUCCESS)
   {
     find_line((DWORD)mp.addr.line,true);
   }
}

bool    __fastcall modem_ref::start()
{
 bool ret = true;
 if(this->GetState()!=tsRunning)
     ret = Start(0x10000);
  return ret;   
}

bool    __fastcall modem_ref::stop()
{
 if(IsRunning())
  return TerminateAndWait(5000);
 return true; 
}

bool    __fastcall modem_ref::enum_lines(DWORD idx,MODEM_LINE_PROPS & lp)
{
 return modem_iface->enum_lines(idx,&lp) == GKH_RET_SUCCESS ? true:false;
}

bool    __fastcall modem_ref::get_line_addr(DWORD idx,LPMODEM_ADDR addr)
{
 bool ret = false;
 TLockHelper l(locker);

 if(idx<lines.size())
 {
  ret = true;
  lines_map::iterator ptr = lines.begin();
  advance(ptr,idx);
  if(ptr->second)
	 addr->addr = ptr->second->props.addr.addr;
	 else
	 addr->addr = -1;
 }
 return ret;
}

void    __fastcall modem_ref::update_modem_number(DWORD num)
{
 TLockHelper l(locker);
 lines_map::iterator begin = lines.begin(),end = lines.end();
 while(begin!=end)
 {
   modem_line_ref * mlr = begin->second;
   if(mlr) mlr->props.addr.modem = num;
	begin++;
 }
}

LRESULT __fastcall modem_ref::on_send_timeout(DWORD line,DWORD id)
{
 modem_line_ref * mlr = this->find_line(line,false);
 if(mlr)
   mlr->drop_data(id);
 return GKH_RET_ERROR;  
}

LRESULT __fastcall modem_ref::send(LPMPROTO_HEADER send_mph,DWORD wait)
{
 LRESULT ret = GKH_RET_ERROR;
 if(send_mph->addr_to.line<MODEM_ADDR_MAXVALUE)
 {
   modem_line_ref * mlr = this->find_line(send_mph->addr_to.line,false);
   if(mlr)
   {
   mproto_protect(send_mph);
   if(mlr->can_send())
   {
	//DWORD timer_id = -1;
	if(!wait || wait==(DWORD)-1 )
	 {
	   wait = 2000;//Таймаут по умолчанию 2 секунды
	 }
	if(!send_mph->header_cs)
		mproto_protect(send_mph);//send_mph->header_cs = otd_calc_checksum(send_mph,sizeof(*send_mph)-sizeof(send_mph->data[0])-sizeof(send_mph->header_cs),OTD_DEF_POLINOM);
	mlr->queue_data(send_mph,GetTime(false),MSEC_NS100(wait));
	if((mlr->queue_count()%32) == 1)
	   {
		post_txkey(send_mph->addr_to.addr);
	   }
	//unlock();

	ret = GKH_RET_SUCCESS;
	}
	else
	{
	 //unlock();
	 //modemmer->route(send_mph);
	 //Линия в отключке или не имеет право передавать данные - маршрутизировать
	}
   }
   else
   {
	//Нет такой линии - маршрутизировать
	//unlock();
	//modemmer->route(send_mph);
	SetLastError(MDM_ERR_NO_MODEMLINE);
   }

 }
 else
 {
  //Вещательный адрес
  //Отправить на все линии
  DWORD idx   = 0;
  MODEM_ADDR  addr;
  if(send_mph->flags&MPROTO_FLAG_SEND_BROADCAST)
   {
	/*Вещательный адрес с флаглом непосредственной отправки*/
	modemmer->txrx_notify(send_mph,MNCODE_MODEMMER_TX);
	modem_iface->send(send_mph,MPROTO_SIZE(send_mph));
   }
   else
   {
   while(get_line_addr(idx++,&addr))
	 {
	  send_mph->addr_to.line =  addr.line;
	  ret = send(send_mph,-1);
	 }
   send_mph->addr_to.line = -1; // Восстановить значение  вещания
  }
 }
 return ret;
}

bool              __fastcall modem_ref::BeforeExecute()
{
  EnableRestart(true);
  TGKThread::BeforeExecute();
  if(!icp.IsValid())
	 icp.AppendFileHandle((HANDLE)-1,0);
 return icp.IsValid();
}

void              __fastcall modem_ref::BeforeTerminate()
{
 do_recv();
 icp.Close();
 TGKThread::BeforeTerminate();
}

int               __fastcall modem_ref::Execute()
{
 while(!this->CheckTerminateRequest())
 {
  LPOVERLAPPED temp;
  DWORD key;
  MODEM_ADDR addr;
  DWORD state;
  if(icp.GetStatus(key,temp,200,&state))
  {
	switch(key)
	{
	 case MODEM_REF_TERMINATE_KEY: TThread::Terminate();break;
	 case MODEM_REF_TX_KEY       : addr.addr = (DWORD)temp;
								   state = modem_iface->get_line_state(addr.line);
	 case MODEM_REF_LINESTATE_KEY: addr.addr = (DWORD)temp;
		  handle_line_state(addr.line,state);break;

	 case MODEM_REF_TIMER_KEY: addr.addr = (DWORD)temp;
		  on_send_timeout(addr.line,state);
		  break;
	}
  }
  else
  {
   if(GetLastError()==WAIT_TIMEOUT)
   {
   /*Чтение */
   /*if(modem_iface->get_line_state(MODEM_ADDR_MAXVALUE)&MODEM_LINE_STATE_RXDATA)
      this->do_recv();
   */
    on_wait_timeout();
   }
  }
 }
 return 0;
}


void __fastcall modem_ref::on_wait_timeout()
{
 __int64 curr_time = GetTime(false);
 TLockHelper l(locker);
 lines_map::iterator beg = lines.begin(),end = lines.end();
 while(beg!=end)
 {
  modem_line_ref * mlr = beg->second;
  if(mlr)
   {

    DWORD cnt = mlr->erase_timeouts(curr_time);
    if(cnt)
     {
      wchar_t evt_templ[MAX_PATH];//L"Modem %d линия %d  по таймауту удалено %d кадров"
      TGKModule::get_lang_string(IDS_MODEMMER_REP_REMOVEKADRS,evt_templ,sizeof(evt_templ)/sizeof(wchar_t));
      wchar_t text[MAX_PATH];
      wsprintfW(text,evt_templ,(DWORD)mlr->props.addr.modem,(DWORD)mlr->props.addr.line, cnt);
      modemmer->modemmer_report(REPORT_WARNING_TYPE,text);
     }
   } 
  beg++;
 }
}

DWORD             __fastcall modem_ref::OnException(DWORD ExceptionCode,LPEXCEPTION_POINTERS exp)
{
 DWORD ret = TGKThread::OnException(ExceptionCode,exp);
 lines_map::iterator beg = lines.begin(),end = lines.end();
 while(beg!=end)
 {
  modem_line_ref * mlr = beg->second;
  mlr->get_locker().FullUnlock();
  beg++;
 }

 locker.FullUnlock();
 //Перезапуск
 //if(!TerminateRequest)
   //start();
 return ret;
}



void              __fastcall modem_ref::handle_line_state(DWORD line,DWORD state)
{

if(state & MODEM_LINE_STATE_RXDATA)
    //чтение данных
    do_recv();

  if(line<MODEM_ADDR_MAXVALUE)
  {
  modem_line_ref * mlr = this->find_line(line,(state&MODEM_LINE_STATE_EXIST) ? true:false);
  if(mlr)
  {
   DWORD ch_state = mlr->set_state(state);
   if((state&MODEM_LINE_STATE_TXREADY) )
      {
        int lock_count = locker.FullUnlock(); //unlock();
        do_send(mlr);
        locker.RestoreLock(lock_count); //lock();
      }
  if((ch_state&MODEM_LINE_STATE_CONNECT) && !(state&MODEM_LINE_STATE_CONNECT))
    {
       //Обрыв связи
       //TODO маршрутизировать  route_all(mlr);
       int lock_count = locker.FullUnlock(); //unlock();
       clear_tx_queue(mlr);
       locker.RestoreLock(lock_count); //lock();
    }

  if((ch_state&(MODEM_LINE_STATE_EXIST|MODEM_LINE_STATE_CONNECT|MODEM_LINE_STATE_WORKED))
        && modemmer->need_notify(MNF_MODEM)
        )
   {
       //Напоминание об изменении состоянии линии
       #define NFML_SIZE (sizeof(NOTIFY_CODE)+sizeof(MODEM_LINE_STATE)-sizeof(DWORD))
       BYTE buffer[NFML_SIZE];
       LPNOTIFY_CODE nc = (LPNOTIFY_CODE)buffer;
       nc->dw_size      = NFML_SIZE;
       nc->from_module  = (*modemmer)();
       nc->notify_code  = MNCODE_LINE_STATE;
       LPMODEM_LINE_STATE mls = (LPMODEM_LINE_STATE)nc->notify_data;
       mls->addr  = mlr->props.addr;
       mls->line_state    = mlr->props.line_state;
       mls->state_changes = ch_state;
       memcpy(nc->notify_data,mls,sizeof(*mls));
       //Sleep(100);
       int lock_count = locker.FullUnlock(); //unlock();
           modemmer->notify_handles(nc,MNF_MODEM);
       locker.RestoreLock(lock_count); //lock();

   }

  if((ch_state&MODEM_LINE_STATE_EXIST) && !(state&MODEM_LINE_STATE_EXIST))
    {
      //Удаление линии
     TLockHelper l(locker);
     lines.erase(line);
     delete mlr;
    }

  }
 }
}

void              __fastcall modem_ref::do_recv()
{
 //Чтение данных из модема
 bool do_continue = true;
 while(do_continue && !CheckTerminateRequest())
 {
  if(buffer)
	 ZeroMemory(buffer,KeRTL::MIN(buffer_size,(DWORD)sizeof(MPROTO_HEADER)));
  LPMPROTO_HEADER  mph = (LPMPROTO_HEADER)buffer;
  int ret = modem_iface->recv(mph,buffer_size);
  if (ret > 0)
	  {
	   if(!mph->header_cs || mproto_check(mph))
		 {
		  modemmer->input_handler((LPMPROTO_HEADER)buffer,ret);
		 }
	   #ifdef _DEBUG
	   else //Ошибка контрольной суммы заголовка
		DebugBreak();
	   #endif
		continue;
	  }
  if(ret == 0)
	 do_continue = false;
	 else
	 check_buffer((DWORD)KeRTL::ABS(ret));
 }
}

void              __fastcall modem_ref::do_send(modem_line_ref * mlr)
{
  bool do_continue =  true;
  DWORD q_size     =  mlr ?  mlr->queue_count() : 0;
  while(mlr && (mlr->get_state()&MODEM_LINE_STATE_TXREADY) && do_continue &&  q_size && !CheckTerminateRequest())
  {
	DWORD  need_size = (DWORD)mlr->top_size();
	if(check_buffer(need_size ))
	  {
	   LPMPROTO_HEADER  mph =(LPMPROTO_HEADER)buffer;
	   DWORD send_size = mlr->peek_data(mph,buffer_size);
	   //Передача данных линии модеммера
	   {
		 if(!mph->pkt_num)
		 {
		 //Если номер пакета == 0, то нумеруем автоматически
		 if(mlr->tx_kadr_number == 0)
			++mlr->tx_kadr_number;
			mph->pkt_num = mlr->tx_kadr_number++;
			mproto_protect(mph);
		 }
		 else
		   mlr->tx_kadr_number = mph->pkt_num;
			//TODO protect_mpheader(mph);
			//LRESULT ret   =
			modem_iface->send(mph,send_size);
			DWORD   error = GetLastError();
			if(error && error<(DWORD)-1)
			   {
				wchar_t evt_tmpl[MAX_PATH];//L"Модем %d линия %d Ошибка при передаче "
				TGKModule::get_lang_string(IDS_MODEMMER_REP_TXERR,evt_tmpl,sizeof(evt_tmpl)/sizeof(wchar_t));
				wchar_t err_rep[MAX_PATH];
				int len = wsprintfW(err_rep,evt_tmpl,(DWORD)mlr->props.addr.modem,(DWORD)mlr->props.addr.line);
                modemmer->get_error_text(error,err_rep+len,MAX_PATH-len,0);
                modemmer->modemmer_report(REPORT_ERROR_TYPE,err_rep);
                //modemmer->tx_error();
               }
               else
               modemmer->txrx_notify(mph,MNCODE_MODEMMER_TX);
            switch(error)
            {
             case MDM_ERR_NO_MODEMLINE:
									   //TODO route_all(mlr);
									  {
									   TLockHelper l(locker);
									   lines.erase(mlr->props.addr.line);
									  }
									   delete mlr;
	    							   do_continue = false;
									   break;
             case MERR_MODULE_NOTRUNNING   :
             case MDM_ERR_LINE_NOT_CONNECT :
                                      //TODO route_all(mlr);
                                      clear_tx_queue(mlr);
                                      mlr->props.line_state&=~(MODEM_LINE_STATE_CONNECT|MODEM_LINE_STATE_TXREADY);
                                      do_continue = false;
                                      break;
             case MDM_ERR_SEND_BUSY:   mlr->props.line_state&=~MODEM_LINE_STATE_TXREADY;
                                       do_continue = false;
                                       break;
             default               :
                                        mlr->drop_first();
                                        break;
            }
            q_size = mlr->queue_count();
           }
      }
      else
      {
       //Запись в лог /*Об нехватке памяти*/
       clear_tx_queue(mlr);
       mlr->clear();
      }
  }
}






