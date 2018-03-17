#pragma hdrstop
#include "comtrade.h"
#include <functional>
#include <algorithm>

#ifdef __WIN32__
#include <windows.h>
#endif


char * Tcomtrade::crlf = "\n\r";

struct enumerator
{
  int number;
  enumerator():number(1){};
  void operator () (Tcomtrade_channel & c) {c.number = number++;}

};

void __fastcall Tcomtrade::renumber_channels()
{
  std::for_each(analogs.begin() ,analogs.end(),enumerator());
  std::for_each(discrets.begin(),discrets.end(),enumerator());
}


ostream & operator << (ostream & os, Tcomtrade_channel & ch)
{

 os<<ch.number<<','<<ch.id;
 if(ch.discrete)
    os<<','<<ch.normal_state;
   else
   {
    os<<','<<ch.phase
      <<','<<ch.component
      <<','<<  ch.unit
      <<','<<  ch.a
      <<','<<  ch.b
      <<','<<  ch.skew
      <<','<<  ch.vmin
      <<','<<  ch.vmax
      <<','<<1<<','<<1<<",S"
      ;
   }
 os<<Tcomtrade::crlf;
 return os;

}

struct osouter
{

 bool wr_value;
 ostream * os;
 osouter():os(NULL){ }
 osouter(ostream & _os,bool _wr_value = false):os(&_os),wr_value(_wr_value) {};
 osouter(const osouter & src):os(src.os),wr_value(src.wr_value){}
 osouter & operator = (const osouter & src){os = src.os;  wr_value = src.wr_value; return *this;}
 void operator () (Tcomtrade_channel & c)
 { if(wr_value)
      {(*os)<<',';c.write_value(*os);}
         else (*os)<<c;}
};

ostream & operator << (ostream & os, Tcomtrade_channels & channels)
{
 std::for_each(channels.begin(),channels.end(),osouter(os));
 return os;
}

ostream & operator << (ostream & os, Tcomtrade & cfg)
{
 os<<cfg.name<<','<<cfg.id<<Tcomtrade::crlf;
 os<< cfg.analogs.size()+cfg.discrets.size();
 if(cfg.analogs.size())
    os<<','<<cfg.analogs.size()<<'A';
 if(cfg.discrets.size())
    os<<','<<cfg.discrets.size()<<'D';
 os<<Tcomtrade::crlf;

 os<<cfg.analogs<<cfg.discrets;
 os<<cfg.freq<<Tcomtrade::crlf;
 os<<int(1)<<Tcomtrade::crlf<<cfg.freq_discrete<<','<<cfg.samples_count<<Tcomtrade::crlf;
 string begin_time,start_time;
 cfg.get_time_string(cfg.begin_time,begin_time);
 cfg.get_time_string(cfg.begin_time,start_time);

 os<<begin_time<<Tcomtrade::crlf;
 os<<start_time<<Tcomtrade::crlf;
 os<<"ASCII"<<Tcomtrade::crlf;

 return os;
}

#include <KeRTL.hpp>

void __fastcall  Tcomtrade::get_time_string(int64_t timeval,string & time_str)
{

 #ifdef __WIN32__
 SYSTEMTIME st;
 char text[MAX_PATH];
 //timeval *= 10000;// В 100 наносекудный формат
 FileTimeToSystemTime((LPFILETIME)&timeval,&st);
 int len = GetDateFormat(LOCALE_SYSTEM_DEFAULT,0,&st,"MM'/'dd'/'yyyy",text,sizeof(text));
 if(len)
 text[len-1]=',';
 else
 {
  KeRTL::GetErrorMessage(GetLastError(),text,sizeof(text),(HINSTANCE)NULL);
  KeRTL::GetErrorMessage(GetLastError(),text,sizeof(text),(HINSTANCE)NULL);
 }

 len+=GetTimeFormat(LOCALE_SYSTEM_DEFAULT,TIME_FORCE24HOURFORMAT,&st,"hh':'mm':'ss",text+len,sizeof(text)-len);
 --len;
 snprintf(text+len,sizeof(text)-len,".%hu",st.wMilliseconds);
 time_str = text;
 #endif

}

bool  __fastcall Tcomtrade::bin_search( Tcomtrade_channels & cnls,unsigned int rec_id, Tcomtrade_channels::iterator *ptr)
{
  Tcomtrade_channel   c(rec_id);
  Tcomtrade_channels::iterator cptr = lower_bound(cnls.begin(),cnls.end(),c,less<Tcomtrade_channel>());
  if(ptr) *ptr = cptr;
  return(cptr<cnls.end() && cptr->rec_id == rec_id) ? true : false;
}

bool __fastcall Tcomtrade::add_channel(Tcomtrade_channel & cc)
{
   Tcomtrade_channels & cnls = cc.discrete ? discrets : analogs;
   Tcomtrade_channels::iterator ptr ;
   if(!bin_search(cnls,cc.rec_id,&ptr))
       {
        cnls.insert(ptr,cc);
        return true;
       }
    return false;
}

Tcomtrade_channel * __fastcall Tcomtrade::find_channel(int rec_id)
{

  Tcomtrade_channels::iterator ptr;
  if( bin_search(analogs,rec_id,&ptr) || bin_search(discrets,rec_id,&ptr)  )
      return &*ptr;
  return NULL;
}


void __fastcall  Tcomtrade::write_samples(ostream & os,int64_t cur_time)
{
  if(cur_time>current_time)
  {
      ++samples_count;
      uint64_t tm_offset = NS100_MKSEC(cur_time-begin_time);

      os<<setw(10)<<setfill('0');
      os<<samples_count<<',';
      os<<setw(10)<<setfill('0')<<(tm_offset);
      for_each(analogs.begin(),analogs.end(),osouter(os,true));
      for_each(discrets.begin(),discrets.end(),osouter(os,true));
      current_time = cur_time;
      os<<crlf;

  }
}





