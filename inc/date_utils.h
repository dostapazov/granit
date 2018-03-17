//---------------------------------------------------------------------------

#ifndef utilsH
#define utilsH
#ifndef  _WINDOWS_
#define  STRICT
#include <windows.h>
#endif
//---------------------------------------------------------------------------
//¬исокосный ли год
template <typename T>
T __fastcall du_min(T  v1,T  v2)
{ return v1<v2 ? v1:v2;}


inline
bool __fastcall is_leap_year(int Year)
{ return (Year %4 == 0) && ((Year % 100) || (Year % 400 == 0));}

//ќпределение количества дней в мес€це года
inline int  __fastcall get_month_days(int year,int mon_number)
{
 if (mon_number == 2)
    return 28+(is_leap_year(year) ? 1:0);

    return 30 + ((mon_number < 8 ) ?  int(mon_number&1) : int(!(mon_number&1)));
}


inline int __fastcall remain_days(int year,int mon,int day ,bool inc_curr = true)
{
  if(day<1) day = 1;
  return get_month_days(year,mon)-day+int(inc_curr);
}

inline  void __fastcall next_year(SYSTEMTIME & st)
{
 st.wYear++;
 st.wDay = du_min(st.wDay,WORD(get_month_days(st.wYear,st.wMonth)));
}

inline  void __fastcall prev_year(SYSTEMTIME & st)
{
 st.wYear--;
 st.wDay = du_min(st.wDay,WORD(get_month_days(st.wYear,st.wMonth)));
}

inline  void __fastcall next_month(SYSTEMTIME & st)
{
 if(++st.wMonth>12)
    {
     st.wMonth = 1;next_year(st);
    }
}

inline  void __fastcall prev_month(SYSTEMTIME & st)
{
 if(--st.wMonth<1)
    {
     st.wMonth = 12; prev_year(st);
    }
}


inline  void __fastcall next_day(SYSTEMTIME & st)
{
  int mon_days = get_month_days(st.wYear,st.wMonth);

  if(++st.wDay>mon_days)
      {
       st.wDay = 1;
       next_month(st);
      }

      if(++st.wDayOfWeek>7)st.wDayOfWeek = 1;
}

inline  void __fastcall prev_day(SYSTEMTIME & st)
{
  if(--st.wDay<1)
    {
      prev_month(st);
      st.wDay = get_month_days(st.wYear,st.wMonth);
    }
  if(int(--st.wDayOfWeek)<0)st.wDayOfWeek = 7;
}

inline void __fastcall next_hour(SYSTEMTIME & st)
{
 if(++st.wHour>23)
 {
  st.wHour = 0; next_day(st);
 }
}

inline void __fastcall prev_hour(SYSTEMTIME & st)
{
 if(int(--st.wHour)<0)
 {
  st.wHour = 23; prev_day(st);
 }
}

inline void __fastcall next_minute(SYSTEMTIME & st)
{
 if(++st.wMinute>59)
 {
  st.wMinute = 0; next_hour(st);
 }
}

inline void __fastcall prev_minute(SYSTEMTIME & st)
{
 if(int(--st.wMinute)<0)
 {
  st.wMinute = 59; prev_hour(st);
 }
}

inline void __fastcall next_second(SYSTEMTIME & st)
{
 if(++st.wSecond>59)
 {
  st.wSecond = 0; next_minute(st);
 }
}

inline void __fastcall prev_second(SYSTEMTIME & st)
{
 if(int(--st.wSecond)<0)
 {
  st.wSecond = 59; prev_minute(st);
 }
}

inline void __fastcall next_msec(SYSTEMTIME & st)
{
 if(++st.wMilliseconds>999)
 {
  st.wMilliseconds = 0; next_second(st);
 }
}

inline void __fastcall prev_msec(SYSTEMTIME & st)
{
 if(int(--st.wMilliseconds)<0)
 {
  st.wMilliseconds = 999; prev_second(st);
 }
}







#endif
