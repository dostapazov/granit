#pragma hdrstop
#include  <modem.hpp>

    modem_line::modem_line(DWORD ln):owner(NULL),line_number(ln)
    {
        line_state = MODEM_LINE_STATE_EXIST;
        rx_min_speed   = tx_min_speed = (DWORD)-1;
        tx_update_time = rx_update_time = 0;
        kadr_number    =  0;
        tx_speed       = -1;
        rx_speed       = -1;
        connect_lost   =  0;
        tx_zero_time_bytes = 0;
        tx_bytes       =  0;
        tx_kadrs       =  0;
        tx_errors      =  0;
        tx_quality     =  100;
        tx_middle_speed=  0;
        tx_max_speed   =  0;
        tx_min_speed   = -1;

        rx_zero_time_bytes = 0;
        rx_bytes       =  0;
        rx_kadrs       =  0;
        rx_errors      =  0;
        rx_quality     =  100;
        rx_middle_speed=  0;
        rx_max_speed   =  0;
        rx_min_speed   = -1;
        tx_total_kadrs = tx_total_errors = tx_total_bytes = rx_total_kadrs = rx_total_errors = rx_total_bytes = 0;
        modemmer_rx_kadrs =  modemmer_rx_bytes = modemmer_tx_kadrs =  modemmer_tx_bytes = 0;
    }

   void           __fastcall modem_line::set_owner(TModemBase * _owner)
   {

    TModemBase *old_owner = get_owner();
    DWORD        ln       = get_number(); 
    if(old_owner)
       owner->line_state_changed(ln,0,-1);
    lock();
    owner = _owner;
    DWORD st = get_state();
    unlock();
    if(_owner)
       _owner->line_state_changed(ln,st,st);
   }

   void  __fastcall modem_line::set_line_number(DWORD new_num)
   {
    if(get_number()!=new_num)
    {
     if(owner) owner->line_state_changed(line_number,0,-1);
     lock();
     line_number = new_num;
     DWORD st = get_state();
     unlock();
     if(owner) owner->line_state_changed(line_number,st,st);
    }
   }

   bool          __fastcall modem_line::start()
   {
    /*запуск линии*/
     bool ret = false;
     if(!check_state(MODEM_LINE_STATE_WORKED,false))
     {
       ret = true;
       kadr_number  = 0;
       if(do_start())
          set_state(MODEM_LINE_STATE_WORKED,true);
     }
     return    ret;
   }

   bool          __fastcall modem_line::stop()
   {
    //Останов линии
     bool ret = false;
     if(check_state(MODEM_LINE_STATE_WORKED,false))
     {
       ret = true;
       if(do_stop())
          {
           set_state(MODEM_LINE_STATE_WORKED,false);
           kadr_number  = 0;
          }
     }
     return    ret;
   }


   BOOL          __fastcall modem_line::check_state (DWORD ch_fl,bool exact)
   {
    BOOL ret = FALSE;
    lock();
    DWORD res = (line_state & ch_fl);
    unlock();
    if((exact && res == ch_fl) || (!exact && res))
        ret  = TRUE;
    return ret;
   }

   DWORD       __fastcall modem_line::set_state(DWORD state,bool set,bool notify )
   {
    DWORD ret ;
    DWORD new_state;
    if(set)
    {
     if(state)
       {
        state|= MODEM_LINE_STATE_EXIST;
        if(state & MODEM_LINE_STATE_CONNECT )
        {
           if( get_line_flags()&MLPF_TXABLE )
             state|=MODEM_LINE_STATE_TXREADY;
        }
       }

    }
    else
    {
     if(state&MODEM_LINE_STATE_EXIST)
       state|=MODEM_LINE_STATE_WORKED;

     if(state&MODEM_LINE_STATE_WORKED)
        state|=MODEM_LINE_STATE_CONNECT;

     if(state&MODEM_LINE_STATE_CONNECT)
        state|=(MODEM_LINE_STATE_TXREADY|MODEM_LINE_STATE_CONNECTING|MODEM_LINE_STATE_DISCONNECTING);
    }

    lock();

    new_state =  set ? (line_state|state):(line_state&~state);

    if(set)
    {
     if(state & MODEM_LINE_STATE_CONNECTING)
       new_state &=~(MODEM_LINE_STATE_CONNECT|MODEM_LINE_STATE_TXREADY|MODEM_LINE_STATE_RXDATA|MODEM_LINE_STATE_DISCONNECTING);

    if(new_state & MODEM_LINE_STATE_DISCONNECTING)
       new_state &=~(MODEM_LINE_STATE_TXREADY|MODEM_LINE_STATE_CONNECTING);

    if(new_state & MODEM_LINE_STATE_CONNECT)
       new_state &=~(MODEM_LINE_STATE_DISCONNECTING|MODEM_LINE_STATE_CONNECTING);
    }

    ret       = new_state^line_state;
    line_state = new_state;
    unlock();
    if(ret && notify && owner)
      {
       owner->line_state_changed(line_number,new_state,ret);
       Sleep(10);
      }
    return ret;
   }

   void  __fastcall modem_line::on_connect()
   {
    tx_update_time =GetTime(false);
    begin_connect_time = rx_update_time = tx_update_time;
    rx_zero_time_bytes = tx_zero_time_bytes = 0;;
    tx_kadrs = tx_errors = tx_bytes = rx_kadrs = rx_errors = rx_bytes = 0;
    update_stats_tx(0,0,0);update_stats_rx(0,0,0);
    refresh(-1);
   }

   __int64       __fastcall modem_line::get_connect_time()
   {
      return begin_connect_time ? GetTime(false)-begin_connect_time : 0;
   }

   void  __fastcall modem_line::on_disconnect()
   {
     lock();
     if(is_connected())
        connect_lost++;
     unlock();
   }

   void __fastcall modem_line::connect(bool connect)
   {
     if(set_state(MODEM_LINE_STATE_CONNECT,connect))
       {
        if(connect)
           on_connect();
           else
           on_disconnect();
       }
   }

   void  __fastcall modem_line::update_stats_tx(DWORD bytes,DWORD kadrs,DWORD errors)
   {
    __int64 curr_time,delta_time;
    if(bytes)
    {
     curr_time = GetTime(false);
     delta_time  = NS100_MSEC(curr_time - tx_update_time);

    }

    lock();
    if(bytes)
    {
     tx_total_bytes +=bytes;
     tx_bytes +=bytes;
     if(delta_time)
     {
      tx_middle_speed = (DWORD)(float(100000*(tx_zero_time_bytes+bytes))/(float)delta_time);
      tx_max_speed = KeRTL::MAX(tx_middle_speed,tx_max_speed);
      tx_min_speed = KeRTL::MIN(tx_middle_speed,tx_min_speed);
      tx_zero_time_bytes = 0;
      tx_update_time = curr_time;
     }
     else
       tx_zero_time_bytes+=bytes;
    }
    tx_kadrs+=kadrs;
    tx_total_kadrs+=kadrs;
    tx_errors+=errors;
    tx_total_errors = errors;
    calc_tx_quality();
    unlock();

   }

   void  __fastcall modem_line::update_stats_rx(DWORD bytes,DWORD kadrs,DWORD errors)
   {
    __int64 curr_time,delta_time;
    lock();
    if(bytes)
    {
     curr_time=GetTime(false);
     delta_time = NS100_MSEC(curr_time - rx_update_time);
     }
    rx_kadrs+=kadrs;
    rx_total_kadrs+=kadrs;
    rx_errors+=errors;
    rx_total_errors+=errors;
    calc_rx_quality();
    if(bytes)
    {

     rx_bytes+=bytes;
     rx_total_bytes+=bytes;
     if(delta_time)
     {
     DWORD _tmp =   (DWORD)(float(100000*(rx_zero_time_bytes+ bytes))/(float)delta_time);
     rx_middle_speed = (_tmp+rx_middle_speed)/2;

     rx_max_speed = KeRTL::MAX(rx_middle_speed,rx_max_speed);
     rx_min_speed = KeRTL::MIN(rx_middle_speed,rx_min_speed);
     rx_zero_time_bytes = 0;
     rx_update_time = curr_time;
     }
     else
       rx_zero_time_bytes+=bytes;

    }

    unlock();

   }

   void  __fastcall modem_line::calc_tx_quality()
   {
    float quality = 100.0;
    
    if(tx_kadrs && tx_errors)
      quality-= 100.0*float(tx_errors)/float(tx_kadrs);
    if(quality < 0)
       quality = 0;
    tx_quality = (DWORD)quality;

   };

   void  __fastcall modem_line::calc_rx_quality()
   {
    float quality = 100.0;
    
    if(rx_kadrs && rx_errors)
      quality-= 100.0*float(rx_errors)/float(rx_kadrs);
    if(quality < 0)
       quality = 0;
    rx_quality = (DWORD)quality;

   };



  bool          __fastcall modem_line::get_props(LPMODEM_LINE_PROPS lp)
  {
   bool ret = false;
   if(lp && lp->dw_size >= sizeof(*lp))
   {

    ret = true;
    lp->addr.addr      = -1;
    lp->addr.sta       =  0;
    TLockHelper l(locker);
    lp->addr.modem     = owner ? owner->get_modem_number() :-1;
    lp->addr.line      = line_number;
    lp->line_state     = line_state ;
    lp->tx_speed       = tx_speed;
    lp->rx_speed       = rx_speed;
    lp->line_flags     = get_line_flags();
    lp->max_frame_size = get_max_frame_size();
    if(lp->line_text_sz)
       lp->line_text_len = get_line_text(lp->line_text,lp->line_text_sz);
       else
       lp->line_text_len = 0;

   }
   return ret;
  }

  bool          __fastcall modem_line::get_stats(LPMODEM_LINE_STATS ls)
  {
   bool ret = false;
   if(ls && ls->dw_size >= sizeof(*ls))
   {
    ret = true;
    TLockHelper l(locker);
    ls->addr.addr    = -1;
    ls->addr.sta     =  0;
    ls->addr.modem   = owner ? owner->get_modem_number():-1;
    ls->addr.line    = line_number;
    ls->line_state     =  line_state     ;
    ls->connect_lost   =  connect_lost   ;
    ls->tx_bytes       =  tx_bytes       ;
    ls->tx_kadrs       =  tx_kadrs       ;
    ls->tx_errors      =  tx_errors      ;
    ls->tx_total_bytes =  tx_total_bytes ;
    ls->tx_total_kadrs =  tx_total_kadrs ;
    ls->tx_total_errors=  tx_total_errors;
    ls->tx_middle_speed=  tx_middle_speed;
    ls->tx_max_speed   =  tx_max_speed   ;
    ls->tx_min_speed   =  tx_min_speed   ;
    ls->tx_queue_size  =  get_tx_queue_size();

    ls->rx_bytes       =  rx_bytes       ;
    ls->rx_kadrs       =  rx_kadrs       ;
    ls->rx_errors      =  rx_errors      ;
    ls->rx_total_bytes =  rx_total_bytes ;
    ls->rx_total_kadrs =  rx_total_kadrs ;
    ls->rx_total_errors=  rx_total_errors;
    ls->rx_middle_speed=  rx_middle_speed;
    ls->rx_max_speed   =  rx_max_speed   ;
    ls->rx_min_speed   =  rx_min_speed   ;
    //rx_middle_speed    =  tx_middle_speed = 0;
    ls->begin_connect_time.QuadPart = begin_connect_time;
    

    if(line_state&MODEM_LINE_STATE_CONNECT)
    {
    ls->rx_quality     =  rx_quality     ;
    ls->tx_quality     =  tx_quality     ;
    }
    else
    ls->rx_quality     =  ls->tx_quality     =  0;
    ls->modemmer_rx_kadrs =modemmer_rx_kadrs;
    ls->modemmer_rx_bytes =modemmer_rx_bytes;
    ls->modemmer_tx_kadrs =modemmer_tx_kadrs;
    ls->modemmer_tx_bytes =modemmer_tx_bytes;

   }
   return ret;
  }

  void  __fastcall modem_line::update_modemmer_tx(DWORD bytes,DWORD kadrs)
  {
   lock();
   modemmer_tx_kadrs+=kadrs;
   modemmer_tx_bytes+=bytes;
   unlock();
  }

  void  __fastcall modem_line::update_modemmer_rx(DWORD bytes,DWORD kadrs)
  {
   lock();
   modemmer_rx_kadrs+=kadrs;
   modemmer_rx_bytes+=bytes;
   unlock();
  }


  void  __fastcall modem_line::queue_rxdata(WORD otd_fa,LPBYTE data,DWORD len,bool first,DWORD addr_to)
  {
   if(owner)
     {
      update_modemmer_rx(len,1);
      int lc = locker.GetLockCount();
      if(lc) locker.FullUnlock();
      owner->queue_rxdata(addr_to,line_number,kadr_number++,otd_fa,(LPVOID)data,len,first);
      if(lc) locker.RestoreLock(lc);
     }
  }




 
