#ifndef _COMTRADE_INC_H
#define _COMTRADE_INC_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <iomanip>

using namespace std;

#define CMTR_FLOAT_MULTIPLIER  100

struct Tcomtrade_channel
{
  bool      discrete;

  unsigned  int   rec_id   ;
  int             number   ;
  string          id       ;
  string          phase    ;
  string          component;
  string          unit     ;
  double          a,b      ;
  double          skew     ;
  double          vmin,vmax;
  int             normal_state;
  bool            valid;
  double          value;
  typedef         std::vector<double>  values_t;
  typedef         std::vector<__int64> times_t;
  values_t        values;
  times_t         times;

  Tcomtrade_channel(){};
  Tcomtrade_channel(unsigned int _rec_id):rec_id(_rec_id)  {};
  Tcomtrade_channel(const Tcomtrade_channel & src){*this = src;}

  Tcomtrade_channel(unsigned int _rec_id,string & _id,int _normal_state):discrete(true),rec_id(_rec_id),id(_id),normal_state(_normal_state),valid(true){};
  Tcomtrade_channel(unsigned int _rec_id,string & _id,string &_phase,string & _component,string & _unit
                   ,double _a,double _b,double _skew, double _vmin,double _vmax
                  );

  ;
  Tcomtrade_channel & operator = (const Tcomtrade_channel & src);
  void __fastcall set_value(double val ) { value = val  ;}
  int  __fastcall set_value(double val,__int64 tm_stamp ) { values.push_back(val),times.push_back(tm_stamp);return (int)values.size();}
  void __fastcall prepare(int sz){values.reserve(sz);times.reserve(sz);};
  void __fastcall set_valid(bool   _valid){ valid = valid;}
  void __fastcall get_value_time(int idx,double & val,__int64 &tm)
  {
   if(idx<(int)values.size())
      {
       val = values.at(idx);
       tm  = times.at(idx);
      }
  }
  void __fastcall write_value(ostream & os)
  {
   if(valid)
   {
   if(!discrete)
   {
    os<<std::setprecision(5)<<(value);
   }
   else
    os<<(value ? 1 : 0);
   }
   else   os<<"999999";
  }

  int __fastcall get_values_count(){return values.size();}


};

inline   Tcomtrade_channel::Tcomtrade_channel(unsigned int _rec_id,string & _id,string &_phase,string & _component,string & _unit
                   ,double _a,double _b,double _skew, double _vmin,double _vmax ):
     discrete    (  false       )
    ,rec_id      ( _rec_id      )
    ,id          ( _id          )
    ,phase       ( _phase       )
    ,component   ( _component   )
    ,unit        ( _unit        )
    ,a           ( _a           )
    ,b           ( _b           )
    ,skew        ( _skew        )
    ,vmin        ( _vmin        )
    ,vmax        ( _vmax        )
    ,normal_state( 0 )
    ,valid       (true)

{}


inline Tcomtrade_channel & Tcomtrade_channel::operator = (const Tcomtrade_channel & src)
{
    discrete     = src.discrete ;
    rec_id       = src.rec_id   ;
    number       = src.number   ;
    id           = src.id       ;
    phase        = src.phase    ;
    component    = src.component;
    unit         = src.unit     ;
    a            = src.a        ;
    b            = src.b        ;
    skew         = src.skew     ;
    vmin         = src.vmin     ;
    vmax         = src.vmax     ;
    normal_state = src.normal_state;
    valid        = src.valid;
    value        = src.value;
    return *this;
}


inline bool operator < (const Tcomtrade_channel & c1,const Tcomtrade_channel & c2)
{
 return c1.rec_id < c2.rec_id ? true : false;
}

inline bool operator == (const Tcomtrade_channel & c1,const Tcomtrade_channel & c2)
{
 return c1.rec_id == c2.rec_id ? true : false;
}

typedef std::vector<Tcomtrade_channel> Tcomtrade_channels;



struct Tcomtrade
{
  string  name;
  int     id;
  double  freq; //Частота сети
  double  freq_discrete; //Частота сети дискретизации

  int64_t begin_time;
  int64_t end_time;
  int64_t current_time;

  int64_t samples_count;

  Tcomtrade_channels analogs;
  Tcomtrade_channels discrets;


  bool  __fastcall write(string & name);
  Tcomtrade(string & _name,int id,double _freq,double _freq_discr,int64_t begin_time,int64_t end_time);
  bool __fastcall  add_channel(Tcomtrade_channel & cc);
  bool __fastcall  bin_search( Tcomtrade_channels & cnls,unsigned int rec_id, Tcomtrade_channels::iterator *ptr);
  void __fastcall  write_samples(ostream & os, int64_t cur_time);
  void __fastcall  renumber_channels();
  void __fastcall  get_time_string(int64_t timeval,string & time_str);
  Tcomtrade_channel * __fastcall find_channel(int rec_id);
  int                 __fastcall get_channels_count(bool discrete) {return discrete ? discrets.size() : analogs.size();}
  Tcomtrade_channel * __fastcall enum_channels(bool discrete,int idx)
  {
    Tcomtrade_channels * src = discrete ? &discrets : &analogs;
    return idx<(int)src->size() ? &src->at(idx): NULL;
  }

  static char * crlf;
};




inline Tcomtrade::Tcomtrade(string & _name,int _id,double _freq,double _freq_discr,int64_t _begin_time,int64_t _end_time)
       :name(_name)
       ,id  (_id  )
       ,freq(_freq)
       ,freq_discrete (_freq_discr)
       ,begin_time    (_begin_time)
       ,end_time      (_end_time  )
       ,current_time  (0)
       ,samples_count(0)
{}

ostream & operator << (ostream & os, Tcomtrade & cfg);
#endif
