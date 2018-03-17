#pragma hdrstop
#include "modemmer_main.hpp"


  DWORD      __fastcall TInternalLines::reg_intline       (GKHANDLE h)
  {
    DWORD ret ;
    locker.Lock();
    ret = find_line(h);
    if(ret ==(DWORD)-1)
    {
      for(DWORD i = 1;i<MODEM_ADDR_MAXVALUE-5;++i)
        {
         if(lines[i].line_handle == NULL)
            {
             ret = i;
             int_line il(h,i);
             il.line_stats.tx_quality = 100;
             il.line_stats.rx_quality = 100;
             il.line_stats.line_state = MODEM_LINE_STATE_EXIST|MODEM_LINE_STATE_WORKED|MODEM_LINE_STATE_CONNECT|MODEM_LINE_STATE_TXREADY;
             LPQWORD tm = (LPQWORD) &il.line_stats.begin_connect_time;
             *tm = GetTime();
             lines[i] = il;
             break;
            }
        }
    }

    locker.Unlock();
    return ret;
  }

  DWORD      __fastcall TInternalLines::unreg_intline     (GKHANDLE h,DWORD line)
  {
    DWORD ret = NULL;
    if(line>0 && line<=MODEM_ADDR_MAXVALUE-5)
    {
     locker.Lock();
     if(is_same_gkhandle(h,lines[line].line_handle))
     {
      ret = (DWORD)h;
      lines[line] = int_line();
     }
     locker.Unlock();
    }
    return ret;
  }

  GKHANDLE   __fastcall TInternalLines::get_intline_handle(DWORD line)
  {
    GKHANDLE ret = NULL;
    if(line > 0 && line<MODEM_ADDR_MAXVALUE-5)
     {
      locker.Lock();
      ret = lines[line].line_handle;
      locker.Unlock();
     }
    return ret;
  }


  DWORD      __fastcall TInternalLines::get_next_kadr_num (DWORD line,bool tx)
  {
    if(line > 0 && line<MODEM_ADDR_MAXVALUE)
     {
      TLockHelper l (locker);
      if(tx)
      return ++lines[line].tx_kadr_number;
      else
      return ++lines[line].rx_kadr_number;
     }
   return (DWORD)-1;
  }

  DWORD      __fastcall TInternalLines::find_line         (GKHANDLE h)
  {
   DWORD ret = -1;
   locker.Lock();
   for(DWORD i = 1;ret==(DWORD)-1 && i<MODEM_ADDR_MAXVALUE-5;++i)
   {
    if(is_same_gkhandle(lines[i].line_handle ,h))
      ret = i;
   }
   locker.Unlock();
   return ret;
  }

 DWORD      __fastcall TInternalLines::get_lines_count()
 {
   DWORD ret = 0;
   locker.Lock();
   int_line * beg = this->lines;
   int_line * end = beg + MODEM_ADDR_MAXVALUE-5;
   while(++beg<end && ret)
    {
      if(beg->line_handle)
        ++ret;
    }
  locker.Unlock();
  return ret;
}


 DWORD      __fastcall TInternalLines::enum_lines        (DWORD idx,LPMODEM_LINE_PROPS mlp)
 {
   DWORD ret = -1;
   locker.Lock();
   DWORD current = 0;
   if(idx<MODEM_ADDR_MAXVALUE-5)
   {
    int_line * beg = this->lines;
    int_line * end = beg + MODEM_ADDR_MAXVALUE-5;
    while(++beg<end && ret)
    {
      if(beg->line_handle)
        {

         if(current == idx)
           {
             ret = GKH_RET_SUCCESS;
             DWORD line_num = beg->line_stats.addr.line;
             mlp->addr.line = line_num;
             get_line_props(line_num,mlp);

           }
         ++current;
        }
    }


   }

   locker.Unlock();
   return ret;
 }

 LRESULT    __fastcall TInternalLines::get_line_stats(DWORD line,LPMODEM_LINE_STATS mls)
 {
  LRESULT ret = GKH_RET_ERROR;
   if(line<MODEM_ADDR_MAXVALUE-5)
   {
    locker.Lock();
    int_line & ptr = lines[line];
    if(ptr.line_handle)
    {
     memcpy(mls,&ptr.line_stats,sizeof(*mls));
     ret = GKH_RET_SUCCESS;
    }
    locker.Unlock();
   }
  return ret;
 }

 LRESULT  __fastcall TInternalLines::get_line_props  (DWORD line,LPMODEM_LINE_PROPS mlp)
 {
  LRESULT ret = GKH_RET_ERROR;
   if(line<MODEM_ADDR_MAXVALUE-5)
   {
    locker.Lock();
    int_line * ptr = this->lines+line;
    if(ptr->line_handle)
    {
     mlp->addr.sta  = mlp->addr.modem = 0;
     mlp->addr.line = line;
     mlp->addr.reserv1   = -1;
     mlp->max_frame_size = -1;
     mlp->tx_speed       = -1;
     mlp->rx_speed       = -1;
     TGKModuleInterface iface(ptr->line_handle,true);
     wchar_t mod_name[256];
     MODULE_INFO mi;
     ZeroMemory(&mi,sizeof(mi));
     mi.dw_size = sizeof(mi);
     mi.module_name_cbsz = KERTL_ARRAY_COUNT(mod_name);
     mi.module_name      = mod_name;
     mi.mi_mask = MI_FL_MODULE_STATE|MI_FL_MODULE_NAME;
     iface.get_module_info(&mi);
     mlp->line_state = MODEM_LINE_STATE_EXIST;
     if((mi.module_state&MODULE_STATE_RUNNING))
         mlp->line_state |=   MODEM_LINE_STATE_WORKED|MODEM_LINE_STATE_TXREADY|MODEM_LINE_STATE_CONNECT;
         if(mlp->line_text_sz)
         {
          mlp->line_text_len = KeRTL::MIN(mlp->line_text_sz-1,(DWORD)wcslen(mod_name));
          wcsncpy(mlp->line_text,mod_name,mlp->line_text_len);
          mlp->line_text[mlp->line_text_len] = 0;
         }

     ret = GKH_RET_SUCCESS;
    }
    locker.Unlock();
   }
  return ret;

 }

void __fastcall TInternalLines::update_tx(DWORD line,DWORD kadrs,DWORD bytes)
{
   if(line<MODEM_ADDR_MAXVALUE-5)
   {

    locker.Lock();
    int_line * ptr = this->lines+line;
    ptr->line_stats.tx_bytes+=bytes;
    ptr->line_stats.tx_kadrs+=kadrs;
    ptr->line_stats.modemmer_tx_bytes +=bytes;
    ptr->line_stats.modemmer_tx_kadrs +=kadrs;
    __int64 curr_time=GetTime(false);
    if(ptr->tx_update_time)
    {
    __int64 delta_time = NS100_MSEC(curr_time - ptr->tx_update_time);
    if(delta_time)
    {
      ptr->line_stats.tx_middle_speed = (DWORD)(float(100000*(ptr->tx_zero_bytes+bytes))/(float)delta_time);
      ptr->line_stats.tx_max_speed    = KeRTL::MAX(ptr->line_stats.tx_middle_speed,ptr->line_stats.tx_max_speed);
      if(ptr->line_stats.tx_min_speed)
      ptr->line_stats.tx_min_speed    = KeRTL::MIN(ptr->line_stats.tx_middle_speed,ptr->line_stats.tx_min_speed);
      else
      ptr->line_stats.tx_min_speed = ptr->line_stats.tx_middle_speed;
      ptr->tx_zero_bytes = 0;
     }
     else
     ptr->tx_zero_bytes += bytes;
    }
    ptr->tx_update_time =curr_time;
    locker.Unlock();
   }

}

void __fastcall TInternalLines::update_rx(DWORD line,DWORD kadrs,DWORD bytes)
{
   if(line<MODEM_ADDR_MAXVALUE-5)
   {
    locker.Lock();
    int_line * ptr = this->lines+line;
    ptr->line_stats.rx_bytes+=bytes;
    ptr->line_stats.rx_kadrs+=kadrs;
    ptr->line_stats.modemmer_rx_bytes +=bytes;
    ptr->line_stats.modemmer_rx_kadrs +=kadrs;
    __int64 curr_time=GetTime(false);
    if(ptr->rx_update_time)
    {
    __int64 delta_time = NS100_MSEC(curr_time - ptr->rx_update_time);
    if(delta_time)
    {
     ptr->line_stats.rx_middle_speed = (DWORD)(float(100000*(ptr->rx_zero_bytes+bytes))/(float)delta_time);
     ptr->line_stats.rx_max_speed    = KeRTL::MAX(ptr->line_stats.rx_middle_speed,ptr->line_stats.rx_max_speed);
     if(ptr->line_stats.rx_min_speed)
        ptr->line_stats.rx_min_speed    = KeRTL::MIN(ptr->line_stats.rx_middle_speed,ptr->line_stats.rx_min_speed);
        else
        ptr->line_stats.rx_min_speed = ptr->line_stats.rx_middle_speed;
     ptr->rx_zero_bytes = 0;
    }else
     ptr->rx_zero_bytes += bytes;
    }
    ptr->rx_update_time =curr_time;

    locker.Unlock();
   }

}

