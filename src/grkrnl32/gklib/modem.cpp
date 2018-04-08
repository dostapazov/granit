#pragma hdrstop
#include  <modem.hpp>
#include  <tregstry.hpp>


bool __fastcall check_buffer(LPBYTE & buf,DWORD & size,DWORD need_size)
{
 if(need_size>size || !buf)
   {
    size = need_size;
    if(buf)
      delete [] buf;
    buf    = new BYTE[size];
   }
 return buf ? true:false;
}

TModemBase::TModemBase(DWORD _number):modem_number(_number)
{
  temp_rd_buffer = 0;
  temp_rd_buffer_size = 0;
}


bool        __fastcall TModemBase::add_line(modem_line * line)
{
  /*
     Добавление линии в список
  */
  DWORD ln = line->get_number();
  bool ret = false;
  TLockHelper l(locker);
  // lock();
  if(lines.count(ln) == 0)
     {
      lines[ln] = line;
      line->set_owner(this);
      line->set_state(MODEM_LINE_STATE_EXIST,true);
      ret = true;
      }
      else
      SetLastError(ERROR_ALREADY_EXISTS);
  //unlock();
  return ret;
}

bool        __fastcall    TModemBase::renumber_line(DWORD old_num,DWORD new_num)
{
 
 bool ret = false;
 lock();
 if(lines.count(old_num))
 {
  modem_line* ln = lines[old_num];
  if(ln && (!lines.count(new_num)|| lines[new_num]==NULL))
  {
    ln->stop();
    lines.erase(old_num);
    ln->set_line_number(new_num);
    lines[new_num] = ln;

    if(is_running())
    {
       unlock();
       ln->start();
       lock();
    }
    ret = true;
  }
 }
 unlock();
 return  ret;
}

bool        __fastcall    TModemBase::remove_line(DWORD num)
{
 /*
  Удаление линии из списка
 */
 modem_line* ln = NULL;
 DWORD new_state;
 lock();
 if(lines.count(num))
    {
     ln = lines[num];
     lines[num] = NULL;
     lines.erase(num);
    }
 unlock();
 if(ln)
   {
    ln->stop();
    new_state = ln->get_state();
    free_line(ln);
   }

 if(ln)
    line_state_changed(num,0,new_state);
 return ln ? true:false;
}

void        __fastcall    TModemBase::clear_lines()
{
  /*Удаление всех существующих линий*/
  lock();
  lines_map::iterator i = lines.begin(),end = lines.end();
  while(lines.size())
        {
         remove_line(lines.begin()->first);
        }
 lines.clear();       
 unlock();
}

modem_line * __fastcall    TModemBase::get_line(DWORD number)
{
  modem_line* ret = NULL;
  lock();
 if(lines.count(number))
    ret = lines[number];
    else
    SetLastError(MDM_ERR_NO_MODEMLINE);
 unlock();
 return ret;
}

DWORD                  __fastcall    TModemBase::get_lines_count()
{
   DWORD ret;
   lock();
    ret = lines.size();
   unlock();
   return ret;

}

  DWORD __fastcall TModemBase::get_modem_number()
  {
   DWORD ret;
   //lock();
    ret = this->modem_number;
   //unlock();
   return ret;
  }


bool        __fastcall    TModemBase::read_settings()
{
 /*Чтение настроек*/
 TRegsReader rr(DupKey(reg_key));
 lock();
 modem_number = rr.ReadDword(REG_MODEM_NUMBER,-1,true);
 unlock();
 return TGKModule::read_settings() && modem_number != ((DWORD)-1) ? true:false;
}

bool        __fastcall    TModemBase::write_settings()
{
  lock();
  if(cc_flags&CCFL_MODEM_NUMBER)
  {
   TRegsWriter rw(DupKey(reg_key));
   rw.WriteDword(REG_MODEM_NUMBER,modem_number,true);
  } 
  cc_flags &=~CCFL_MODEM_NUMBER;
  unlock();
  return TGKModule::write_settings();
}

BOOL        __fastcall    TModemBase::can_start(DWORD reason,LPARAM start_param)
{
  if(get_modem_number()<MAX_MODEM_COUNT)
	{
	 return TGKModule::can_start(reason,start_param);
	}
	else
	SetLastError( MDM_ERR_NO_MODEM_NUMBER);
  return  FALSE;
}

DWORD       __fastcall    TModemBase::lines_start(bool start)
{
   DWORD error = 0;
   vector<modem_line *> vlines;
	  {
		  TLockHelper l(locker);
		  vlines.reserve(lines.size());
		  lines_map::iterator i = lines.begin(),end = lines.end();
		  while(i!=end)
			 {
			  vlines.push_back(i->second);
			  ++i;
			 }
	  }
     vector<modem_line *>::iterator lptr = vlines.begin();
     vector<modem_line *>::iterator lend = vlines.end();


  while(lptr<lend)
		{
		  modem_line * line = *lptr;
		 if(line)
			{
			 bool res =  start ? line->start() : line->stop() ;
			 if(!res)
                error = GetLastError();

			}
		 lptr++;
		}
  return error;
}

DWORD       __fastcall    TModemBase::start(DWORD reason,LPARAM start_param)
{
  DWORD ret  = GKH_RET_SUCCESS;

  if(get_modem_number()<MAX_MODEM_COUNT)
  {
	DWORD error = this->lines_start(true);
	if(ret == GKH_RET_SUCCESS)
		TGKModule::start(reason,start_param);
		SetLastError(error);
  }
  else
  {
   ret = GKH_RET_ERROR;
   SetLastError(MDM_ERR_NO_MODEM_NUMBER);
  }
 return  ret;

}

DWORD       __fastcall    TModemBase::stop (DWORD reason)
{
  modem_line * line;
  modem_line * stop_list[MODEM_ADDR_MAXVALUE];
  DWORD error = this->lines_start(false);
  SetLastError(error);
  //Отправить модеммеру кадр нет данных для всего ПУ
 return TGKModule::stop(reason);
}

void        __fastcall TModemBase::release(LPVOID arg)
{
  //Удалить линии
  TGKModule::release(arg);
  clear_lines();
  rx_queue.clear();
  if(temp_rd_buffer)
     delete [] temp_rd_buffer;
}

LRESULT  __fastcall TModemBase::set_modem_number(DWORD num,LPDWORD old_num)
{
  /*Установка номера модема*/
  LRESULT ret = GKH_RET_SUCCESS;
  if(num!=get_modem_number())
  {

   MODEM_CHANGE_NUMBER mcn;
   mcn.dw_size = sizeof(mcn);
   mcn.modem   = _handle;
   mcn.new_number = num;
   mcn.old_number = modem_number;
   bool running = check_running();
   if(running)
      stop(MODULE_STOP_RECONFIGURE);
      ret = call_owner(MDMCM_CHANGE_MODEM_NUMBER,(LPARAM)&mcn,TRUE);

   if(ret == GKH_RET_SUCCESS)
    {
     if(old_num)
        *old_num  = modem_number;
      lock();
      modem_number = num;
      cc_flags|= CCFL_MODEM_NUMBER;
      unlock();
      notify_change_number(mcn.new_number,mcn.old_number);
    if(running)
       start(MODULE_START_RECONFIGURE,start_param);
    }
  }
  return ret;
}

void  __fastcall    TModemBase::notify_change_number(DWORD new_num,DWORD old_num)
{
 if(need_notify(MNF_MODEM))
 {
  #define NSIZE (sizeof(NOTIFY_CODE)+sizeof(MODEM_CHANGE_NUMBER)-sizeof(DWORD))
  char buff[NSIZE];
  LPNOTIFY_CODE nc = (LPNOTIFY_CODE)buff;
  nc->dw_size      = NSIZE;
  nc->notify_code  = MNCODE_MODEM_NUMBER;
  LPMODEM_CHANGE_NUMBER  mcn = (LPMODEM_CHANGE_NUMBER)nc->notify_data;
  mcn->dw_size    = sizeof(*mcn);
  mcn->new_number = new_num;
  mcn->old_number = old_num;
  notify_handles(nc,MNF_MODEM);
 }
}



LRESULT  __fastcall TModemBase::processing(DWORD cmd,LPARAM p1,LPARAM p2)
{
 LRESULT ret = GKH_RET_ERROR;
 switch(cmd)
 {
   case MODEMCM_GET_NUMBER     : ret = get_modem_number();break;
   case MODEMCM_SET_NUMBER     : ret = this->set_modem_number((DWORD)p1,(LPDWORD)p2);break;
   case MODEMCM_GET_MODEM_PROPS: if(check_data_size((LPVOID)p1,sizeof(MODEM_PROPS)))
                                  ret = get_props((LPMODEM_PROPS)p1);
                                  break;
   case MODEMCM_GET_LINE_STATS : if(check_data_size((LPVOID)p2,sizeof(MODEM_LINE_STATS)))
                                  ret = get_line_stats(p1,(LPMODEM_LINE_STATS)p2);
                                  break;
   case MODEMCM_GET_LINE_PROPS : if(check_data_size((LPVOID)p2,sizeof(MODEM_LINE_PROPS)))
                                  ret = get_line_props(p1,(LPMODEM_LINE_PROPS)p2);
                                  break;
   case MODEMCM_GET_LINE_STATE :  ret = get_line_state((DWORD)p1,(LPMODEM_LINE_STATE) p2);break;                            
   case MODEMCM_ENUM_LINES     :  ret  = enum_lines(p1,(LPMODEM_LINE_PROPS)p2);break;
   case MODEMCM_REFRESH_LINE   :  ret  = refresh_line(p1,p2);break;

   case MODEMCM_RECV           :
                                  ret  = recv((LPMPROTO_HEADER)p1,(DWORD)p2);
                                  break;
                                  
   case MODEMCM_SEND           :
                                    ret  = send((LPMPROTO_HEADER)p1,(DWORD)p2)
                                   ;break;
   case MDMCM_BEGIN_REDIRECT   :  if(check_data_size((LPVOID)p1,sizeof(REDIRECT_ENTRY)))
                                  {ret  = begin_redirect((LPREDIRECT_ENTRY)p1);}
                                  break;
   case MDMCM_END_REDIRECT     :  if(check_data_size((LPVOID)p1,sizeof(REDIRECT_ENTRY)))
                                  {ret  = end_redirect((LPREDIRECT_ENTRY)p1);}
                                  break;

  default : ret =  TGKModule::processing(cmd,p1,p2);break;
 }
 return ret;
}

LRESULT     __fastcall    TModemBase::refresh_line(DWORD line_number,DWORD subline)
{
 modem_line * line;
 if(line_number<MAX_MODEM_LINES_COUNT)
 {
  line = get_line(line_number);
 if(line)
    {line->refresh(subline);return GKH_RET_SUCCESS;}
  return GKH_RET_ERROR;
 }
 else
 {
    DWORD i = 0;
    do{
       line = get_line_byindex(i);
       if(line)
          line->refresh(subline);
      }while(line && ++i<MAX_MODEM_LINES_COUNT); 
 }
 return GKH_RET_SUCCESS;
}


LRESULT     __fastcall    TModemBase::send(LPMPROTO_HEADER mph,DWORD sz)
{
 /*Отправка данных в линию*/
 LRESULT ret = GKH_RET_ERROR;
 sz = MPROTO_SIZE(mph);
 modem_line * line;
 if(mph->addr_to.line<MODEM_ADDR_MAXVALUE)
  {
  lock();
  line = get_line(mph->addr_to.line);
  if(line  )
    {
     if(line->check_state(MODEM_LINE_STATE_TXREADY,false))
     {
     if(line->send(mph,sz))
        ret = GKH_RET_SUCCESS;
     }
    }
    else
    SetLastError(MDM_ERR_NO_MODEMLINE);
    unlock();
   }
   else
   {
    lock();
    lines_map::iterator beg = lines.begin(),end = lines.end();
    while(beg!=end)
    {
     modem_line * line = beg->second;
     if(line->check_state(MODEM_LINE_STATE_TXREADY,false))
        line->send(mph,sz);
     beg++;
    }
    unlock();
    ret = GKH_RET_SUCCESS;
   }
 return ret;
}

void     __fastcall   TModemBase::queue_rxdata(DWORD addr_to,DWORD line_number,DWORD kadr_num,WORD otd_fa,LPVOID data,DWORD sz,bool first )
{
  modem_addr to(addr_to);
  modem_addr addr(get_modem_number(),line_number);
  if((rx_queue.put_data(&to,&addr,kadr_num,otd_fa,data,sz,first)%16)==1)
     line_state_changed(-1,MODEM_LINE_STATE_RXDATA,MODEM_LINE_STATE_RXDATA);

}

void         __fastcall   TModemBase::queue_rxdata(LPMPROTO_HEADER mph,bool first)
{
  if((rx_queue.put_data(mph,first)%16)==1)
     line_state_changed(-1,MODEM_LINE_STATE_RXDATA,MODEM_LINE_STATE_RXDATA);
}


/*
Функция чтения приянтых данных
В случае отсутствия линии        -  возвращается GKH_RET_ERROR и устанавливается ошибка MDM_ERR_NO_MODEMLINE
В случае нулевых входных данных  -  возвращается GKH_RET_ERROR и устанавливается ошибка MERR_INVALID_BUFFER_SIZE
Вызывает процедуру чтения  чтения из линии  и возвращает целочисленный результат
Положителльное значение всё ок
отрицательное значение говорит о необходимом размере буфера
*/

LRESULT     __fastcall    TModemBase::recv (LPMPROTO_HEADER mph,DWORD sz)
{
 LRESULT ret    = 0;
 DWORD   error  = 0;//MDM_ERR_NO_MODEMLINE;
 if(mph && sz>sizeof(*mph))
   {
    if(this->rx_queue.queue_count())
    {
      mph->addr_to.addr = 0;
      mph->flags        = 0;
      mph->internal     = 0;
      int rx_len = rx_queue.top_size();
      if(check_buffer(temp_rd_buffer,this->temp_rd_buffer_size,(DWORD)rx_len))
        {
         rx_queue.peek_data(&mph->addr_to,&mph->addr_from,&mph->pkt_num,&mph->fa,temp_rd_buffer,temp_rd_buffer_size,(int*)&ret);
         ret = convert_rx_data(&mph->fa,temp_rd_buffer,ret,(LPBYTE)mph->data,sz-sizeof(*mph)+sizeof(mph->data));
         if(ret>=0)
            {
             mph->internal  = MP_INTERNAL_DEFAULT;
             mph->data_size = ret;
             mproto_protect(mph);
             ret +=sizeof(*mph)-sizeof(mph->data);
             rx_queue.drop_first();
            }
         else
         {
          ret -=(sizeof(*mph)-sizeof(mph->data));
          error = (MERR_INVALID_BUFFER_SIZE);
         }
        }
    }
    else
     error = MERR_NOMORE_DATA;
   }else
     {
      ret = -(int) (sizeof(MPROTO_HEADER)-sizeof(DWORD)+ (rx_queue.queue_count() ? rx_queue.top_size():256));
      error = (MERR_INVALID_BUFFER_SIZE);
     }
   if(error)
     SetLastError(error);
   return ret;
}

 /*Перечисление линий модема*/
 LRESULT     __fastcall    TModemBase::enum_lines(DWORD idx,LPMODEM_LINE_PROPS lp)
 {
  LRESULT ret = GKH_RET_ERROR;
  if(check_data_size(lp,sizeof(*lp)))
  {
   TLockHelper l (locker);
   //lock();
   if(idx<lines.size())
   {
    lines_map::iterator i = lines.begin();
    advance(i,idx);
     modem_line *line =  i->second;
    if(line)
       line->get_props(lp);
    ret = GKH_RET_SUCCESS;
   }
   //unlock();
  }
  return ret;
 }


 modem_line * __fastcall    TModemBase::get_line_byindex(DWORD idx)
{
  modem_line * ret = NULL;
  lock();
  if(idx<lines.size())
  {
    lines_map::iterator i = lines.begin();
    advance(i,idx);
    ret  =  i->second;
  }
  unlock();
  return ret;
}

LRESULT     __fastcall    TModemBase::get_line_state(DWORD ln,LPMODEM_LINE_STATE mls)
{
  LRESULT ret = GKH_RET_ERROR;
  if(check_data_size(mls,sizeof(*mls)))
  {

    mls->addr.addr   = -1;
    mls->addr.sta    =  0;
    mls->addr.modem  = this->get_modem_number();
    if(ln >= MODEM_ADDR_MAXVALUE)
    {
       mls->line_state = mls->state_changes = MODEM_LINE_STATE_EXIST;
       if(rx_queue.queue_count())
          mls->line_state = mls->state_changes = MODEM_LINE_STATE_RXDATA;
      ret = GKH_RET_SUCCESS;
    }
    else
    {
	  //lock();
	  modem_line *line = get_line(ln);
	  if(line)
	  {
	   mls->line_state  = line->get_state();
	   mls->addr.line   = ln;
	   ret = GKH_RET_SUCCESS;
	  }
      //unlock();
    }
  }
  return  ret;
}

LRESULT     __fastcall    TModemBase::get_line_props(DWORD ln,LPMODEM_LINE_PROPS lp)
{
  LRESULT ret = GKH_RET_ERROR;
   modem_line *line = get_line(ln);
  if(line)
     {
       ret = GKH_RET_SUCCESS;
       line->get_props(lp);

      }
     else
     SetLastError(MDM_ERR_NO_MODEMLINE);
  return ret;
}

LRESULT __fastcall TModemBase::get_line_stats(DWORD ln,LPMODEM_LINE_STATS ls)
{
  LRESULT ret = GKH_RET_ERROR;
  lock();
  modem_line *line = get_line(ln);
  if(line)
     {
      line->get_stats(ls);
      ret = GKH_RET_SUCCESS;
     }
     else
     SetLastError(MDM_ERR_NO_MODEMLINE);
  unlock();
  return ret;
}

DWORD        __fastcall    TModemBase::get_modem_info(wchar_t * buff,DWORD buff_sz)
{
  DWORD ret = 0;
  lock_resource();
  if(buff && buff_sz)
  {
   ret   = LoadStringW(get_lang_instance(),IDS_MODEM_INFO,buff,buff_sz);
   if(!ret)
     ret = LoadStringW(get_lang_instance(),IDS_MODULE_NAME,buff,buff_sz);
  }

  unlock_resource();
  return ret;
}

LRESULT     __fastcall    TModemBase::get_props(LPMODEM_PROPS mp)
{
  mp->addr.addr      = -1;
  mp->addr.sta       =  0;
  mp->addr.modem     = get_modem_number();
  mp->lines_count    = get_lines_count();
  mp->lines_limit    = get_lines_limit();
  mp->flags          = this->get_modem_flags();
  mp->modem_info_len = 0;
  if(mp->modem_info_size)
  mp->modem_info_len = get_modem_info(mp->modem_info,mp->modem_info_size);
  
  return GKH_RET_SUCCESS;
}

  void  __fastcall  TModemBase::notify_line_state    (DWORD number,DWORD line_state,DWORD changes)
  {
         #define MLS_NOTIFY_SIZE (sizeof(NOTIFY_CODE)+sizeof(MODEM_LINE_STATE)-sizeof(DWORD))
         char buffer[MLS_NOTIFY_SIZE];
         LPNOTIFY_CODE nc = (LPNOTIFY_CODE)buffer;
         nc->dw_size      = MLS_NOTIFY_SIZE;
         nc->from_module  = _handle;
         nc->notify_code  = MNCODE_LINE_STATE;
         LPMODEM_LINE_STATE mls =(LPMODEM_LINE_STATE)nc->notify_data; ;
         mls->dw_size     = sizeof(*mls);
         mls->addr.addr   = -1;
         mls->addr.sta    =  0;
         mls->addr.line   = number;
         mls->addr.modem  = this->get_modem_number();
         mls->line_state  = line_state;
         mls->state_changes = changes;
         call_owner(MODEMCM_LINE_STATE_CHANGE,(LPARAM)nc->from_module,(LPARAM)mls);
         if(need_notify(MNF_MODEM) && (changes&(MODEM_LINE_STATE_EXIST|MODEM_LINE_STATE_WORKED|MODEM_LINE_STATE_CONNECT)))
         {
            notify_handles(nc,MNF_MODEM);
         }
  }

  void  __fastcall TModemBase::line_state_changed(DWORD number,DWORD line_state,DWORD changes)
  {
     notify_line_state(number,line_state,changes);
  }

  LRESULT  __fastcall TModemBase::begin_redirect(LPREDIRECT_ENTRY re)
  {

    DWORD line_begin = re->to_addr.line;
    DWORD line_end   = re->to_addr.line;
    if(line_begin==MODEM_ADDR_MAXVALUE)
        line_begin = 0;
        else
        line_end++;
   while(line_begin<line_end)
   {
     lock();
     modem_line * ml = get_line(line_begin++);
     if(ml)
       ml->begin_redirect(re);
     unlock();  
   }
   
   return GKH_RET_SUCCESS;
  }

  LRESULT  __fastcall TModemBase::end_redirect  (LPREDIRECT_ENTRY re) 
  {
   
    DWORD line_begin = re->to_addr.line;
    DWORD line_end   = re->to_addr.line;
    if(line_begin==MODEM_ADDR_MAXVALUE)
        line_begin = 0;
        else
        line_end++;
   while(line_begin<line_end)
   {
     lock();
     modem_line * ml = get_line(line_begin++);
     if(ml)
       ml->end_redirect(re);
      unlock(); 
   }
   
   return GKH_RET_SUCCESS;

  }





