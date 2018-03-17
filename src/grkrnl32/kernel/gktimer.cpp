#include "gkernel_mod.hpp"



 
 gktimer & gktimer::operator = (const GKTIMER & tm )
 {
  dw_size    = tm.dw_size;
  tm_target  = tm.tm_target;
  tm_p1      = tm.tm_p1;
  tm_cmd     = tm.tm_cmd ? tm.tm_cmd:CMGK_ONTIMER;
  period     = tm.period;
  alert_time = tm.alert_time;
  alert_counter = tm.alert_counter;
  return *this;
 }



 DWORD   __fastcall  TGKTimer::generate_tmid()
 {
    if(gen_id == DWORD(-1) || !gen_id)
       gen_id=1;
   while(timers.count(gen_id))
         gen_id++;
   return gen_id++;      
 }

 int     __fastcall  TGKTimer::Execute()
 {
    term_event.SetEvent(false);
    TSynchroSet ss;
    ss+=term_event();
    DWORD wres;
    ss+=wt();
    LARGE_INTEGER  due_time;
    __int64        work_time = 0;

    do{
       due_time.QuadPart = -(__int64)(resolution-work_time);
       wt.SetTimer(due_time,0,TRUE);
       wres = ss.Wait(INFINITE);

       while(wres)
       {
        work_time = do_timers();
        if(work_time<resolution)
           break;
       }
      }while(wres && !CheckTerminateRequest());
    return 0;
 }

 void    __fastcall  TGKTimer::Terminate()
 {
   term_event.SetEvent(true);
   TThread::Terminate();
 }

 __int64 __fastcall  TGKTimer::do_timers()
 {
  __int64 start,finish;
  GetSystemTimeAsFileTime((LPFILETIME)&start);
  lock();
  gktimer_list::iterator beg = timers.begin(),end = timers.end();
  for(;beg!=end;beg++)
  {

   DWORD id = beg->first;
   gktimer & tm = beg->second;
   tm.alert_counter+= resolution;
   if(tm.period)
   {
     if(tm.alert_counter>=0)
        {
         if(tm(id))
           {timers.erase(beg--);timers.end();}
           else
            tm.alert_counter = tm.alert_time;
        }
   }
   else
   {
    if(tm.alert_counter>=tm.alert_time)
      {
       tm(id);
       timers.erase(beg);
       beg--;
       end = timers.end();
      }
   }
  }
  unlock();
  GetSystemTimeAsFileTime((LPFILETIME)&finish);
  return (finish-start);
 }
 
 DWORD   __fastcall  TGKTimer::set_timer   (LPGKTIMER tm)
 {
  //Установка нового таймера
  if(!tm->period)
   {
    tm->alert_counter = GetTime();
    if(tm->alert_time<0)
       tm->alert_time = tm->alert_counter-tm->alert_time;
    if(tm->alert_time<=tm->alert_counter)
      return GKH_RET_ERROR;
   }
   else
   {
    if(tm->alert_time>=0)
     return GKH_RET_ERROR;
    tm->alert_counter = tm->alert_time; 
   }

  lock();
  DWORD ret = generate_tmid();
  if(timers.size()==0 && GetState()!=tsRunning)
     start();
  timers[ret] = *tm;
  unlock();
  return ret;
 }

 #pragma warn -8057

 BOOL    __fastcall  TGKTimer::reset_timer (DWORD  id,__int64 * new_alert_time)
 {
  BOOL ret = FALSE;
  lock();
  if(timers.count(id))
     {
      ret = TRUE;
      gktimer & tm = timers[id];
      if(tm.period)
         tm.alert_counter = tm.alert_time;
         else
         cancel_timer(id);
     }
  unlock();
  return ret;
 }

#pragma warn .8057

 BOOL    __fastcall  TGKTimer::cancel_timer(DWORD  id)
 {
  BOOL ret = FALSE;
  lock();
  if(timers.count(id))
     {
      timers.erase(id);
      ret = TRUE;
     }
  unlock();
  return ret;   
 }

 BOOL    __fastcall TGKTimer::get_timer   (DWORD  id,LPGKTIMER gktm)
 {
  BOOL ret = FALSE;
  lock();
  if(timers.count(id))
  {
   ret = TRUE;
   gktimer & tm = timers[id];
   gktm->tm_target     = tm.tm_target;
   gktm->tm_cmd        = tm.tm_cmd;
   gktm->tm_p1         = tm.tm_p1;
   gktm->alert_time    = tm.alert_time;
   gktm->alert_counter = tm.alert_counter;
   gktm->period        = tm.period;
  }
  unlock();
  return ret;
 }

 BOOL    __fastcall  TGKTimer::start(DWORD _resolution)
 {
  BOOL ret;
  lock();
  if(_resolution)
    {resolution = MSEC_NS100(_resolution);}
  ret = Start(0) ? TRUE:FALSE;
  unlock();
  return ret;
 }




 
