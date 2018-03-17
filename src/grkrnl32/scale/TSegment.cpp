//---------------------------------------------------------------------------
#pragma hdrstop

#include <math.h>
#include <values.h>
#include <Algorithm>
#include <System.hpp>
#include "scale.h"
#include <kertl.hpp>

//---------------------------------------------------------------------------

#pragma package(smart_init)

using namespace KeRTL;

__fastcall TSegment::TSegment()
{
  Clear();
}

__fastcall TSegment::TSegment(DWORD min_kv, DWORD max_kv, float beg, float end)
{
  Clear();
  min_kvant=min_kv;
  max_kvant=max_kv;
  SetLineK(beg,end);
}

float __fastcall TSegment::min_y()
{
  if (IsLine()) return std::min(calc(min_kvant),calc(max_kvant));
  float res=MAXFLOAT;
  DWORD i=min_kvant;
  do
  {
    res=std::min(res,calc(i++));
  } while (i<=max_kvant);
  return res;
}

float __fastcall TSegment::max_y()
{
  if (IsLine()) return std::max(calc(min_kvant),calc(max_kvant));
  float res=-MAXFLOAT;
  DWORD i=min_kvant;
  do
  {
    res=std::max(res,calc(i++));
  } while (i<=max_kvant);
  return res;
}

float __fastcall TSegment::calc(DWORD kvant)
{
  float res=k[0];
  float x;
  for (int i=1; i<6; i++)
  {
    x=k[i]*pow(kvant,(long double)i);
    res+=x;
  }
  return res;
}

bool __fastcall TSegment::reverse_calc(float y, DWORD &kvant)
{
  DWORD bx=min_kvant, ex=max_kvant, mx;
  float by=calc(bx)-y, my;
  do
  {
    mx=(bx+ex)/2;
    my=calc(mx)-y;
    if (by*my>0)
    {
      bx=mx;
      by=my;
    } else
      ex=mx;
  } while (ex-bx>1);
  kvant=bx;
  return true;
}

void __fastcall TSegment::SetLineK(float beg, float end)
{
  k[1]=(end-beg)/(max_kvant-min_kvant);
  k[0]=beg-k[1]*min_kvant;
  for (int i=2; i<6; i++) k[i]=0;
}

bool __fastcall TSegment::IsLine()
{
  return ((k[2]==0.0) && (k[3]==0.0) && (k[4]==0.0) && (k[5]==0.0));
}

bool __fastcall TSegment::operator<(const TSegment &T) const
{
  return min_kvant<T.min_kvant;
}

bool __fastcall TSegment::operator==(const TSegment &T) const
{
  bool ret=true;
  ret&=min_kvant==T.min_kvant;
  ret&=max_kvant==T.max_kvant;
  for (int i=0; i<6; i++) ret&=k[i]==T.k[i];
  ret&=percent==T.percent;
  return ret;
}

void __fastcall TSegment::Clear()
{
  ZeroMemory(this,sizeof(TSegment));
  max_kvant=250;
  percent=1.5;
}

__fastcall TScale::TScale()
{
  ZeroMemory(this,sizeof(*this));
}

__fastcall TScale::TScale(void *buf, DWORD size)
{
  ZeroMemory(this,sizeof(*this));
  SetAll(buf,size);
}

__fastcall TScale::~TScale()
{
  Clear();
}

void __fastcall TScale::Clear()
{
  if (s_name) delete [] s_name;
  segs.clear();
  id=0;
  s_name=0;
  warn_min=0;
  warn_max=0;
  alarm_min=0;
  alarm_max=0;
}

void __fastcall TScale::SetAll_v1(void *buf)
{
  Clear();
  if (!buf) return;
  DWORD pos=sizeof(DWORD), sz=*(DWORD *)buf, dw;
  byte *buff=(byte *)buf;
  wchar_t *str;
  TSegment seg;
  if (pos>=sz) return;
  id=(DWORD)buff[pos];
  pos+=sizeof(id);
  if (pos>=sz) return;
  str=(wchar_t * )(buff+pos);
  dw=wcslen(str)+1;
  s_name=new wchar_t[dw];
  safe_strcpy(s_name,str);
  pos+=dw*sizeof(wchar_t);
  limits=0;
  if (pos>=sz) return;
  warn_min=*(float *)(buff+pos);
  if (warn_min!=0.0) limits|=SCL_LIMIT_MINWARN;
  pos+=sizeof(warn_min);
  if (pos>=sz) return;
  warn_max=*(float *)(buff+pos);
  if (warn_max!=0.0) limits|=SCL_LIMIT_MAXWARN;
  pos+=sizeof(warn_max);
  if (pos>=sz) return;
  alarm_min=*(float *)(buff+pos);
  if (alarm_min!=0.0) limits|=SCL_LIMIT_MINALARM;
  pos+=sizeof(alarm_min);
  if (pos>=sz) return;
  alarm_max=*(float *)(buff+pos);
  if (alarm_max!=0.0) limits|=SCL_LIMIT_MAXALARM;
  pos+=sizeof(alarm_max);
  while (pos+sizeof(seg)<=sz)
  {
    memcpy(&seg,buff+pos,sizeof(seg));
    pos+=sizeof(seg);
    AddSegment(&seg);
  }
}

void __fastcall TScale::SetAll(void *buf, DWORD size)
{
  Clear();
  if (!buf) return;
  DWORD pos=sizeof(DWORD), sz=*(DWORD *)buf, dw;
  byte *buff=(byte *)buf;
  wchar_t *str;
  TSegment seg;
  if (size && size<sz) sz=size;
  if (pos>=sz) return;
  id=(DWORD)buff[pos];
  pos+=sizeof(id);
  if (pos>=sz) return;
  str=(wchar_t * )(buff+pos);
  dw=wcslen(str)+1;
  s_name=new wchar_t[dw];
  safe_strcpy(s_name,str);
  pos+=dw*sizeof(wchar_t);
  if (pos>=sz) return;
  warn_min=*(float *)(buff+pos);
  pos+=sizeof(warn_min);
  if (pos>=sz) return;
  warn_max=*(float *)(buff+pos);
  pos+=sizeof(warn_max);
  if (pos>=sz) return;
  alarm_min=*(float *)(buff+pos);
  pos+=sizeof(alarm_min);
  if (pos>=sz) return;
  alarm_max=*(float *)(buff+pos);
  pos+=sizeof(alarm_max);
  if (pos>=sz) return;
  limits=*(DWORD *)(buff+pos);
  pos+=sizeof(limits);
  while (pos+sizeof(seg)<=sz)
  {
    memcpy(&seg,buff+pos,sizeof(seg));
    pos+=sizeof(seg);
    AddSegment(&seg);
  }
}

DWORD __fastcall TScale::GetBuffer(void *buf, DWORD size)
{
  DWORD dw=sizeof(DWORD)*2+sizeof(wchar_t)+sizeof(float)*4+sizeof(DWORD)+sizeof(TSegment)*segs.size();
  if (s_name) dw+=wcslen(s_name)*sizeof(wchar_t);
  if (!buf || dw>size) return dw;
  TSegment seg;
  byte *buff=(byte *)buf;
  memcpy(buff,&dw,sizeof(dw));
  dw=sizeof(dw);
  memcpy(buff+dw,&id,sizeof(id));
  dw+=sizeof(id);
  ZeroMemory(buff+dw,sizeof(wchar_t));
  if (s_name)
  {
    memcpy(buff+dw,s_name,(wcslen(s_name)+1)*sizeof(wchar_t));
    dw+=wcslen(s_name)*sizeof(wchar_t);
  }
  dw+=sizeof(wchar_t);
  memcpy(buff+dw,&warn_min,sizeof(warn_min));
  dw+=sizeof(warn_min);
  memcpy(buff+dw,&warn_max,sizeof(warn_max));
  dw+=sizeof(warn_max);
  memcpy(buff+dw,&alarm_min,sizeof(alarm_min));
  dw+=sizeof(alarm_min);
  memcpy(buff+dw,&alarm_max,sizeof(alarm_max));
  dw+=sizeof(alarm_max);
  memcpy(buff+dw,&limits,sizeof(limits));
  dw+=sizeof(limits);
  for (int i=0; i<(int)segs.size(); i++)
  {
    seg=segs[i];
    memcpy(buff+dw,&seg,sizeof(seg));
    dw+=sizeof(seg);
  }
  return dw;
}

void __fastcall TScale::AddSegment(TSegment *seg)
{
  std::vector<TSegment>::iterator p;
  p=lower_bound(segs.begin(),segs.end(),*seg);
  segs.insert(p,*seg);
}

TSegment * __fastcall TScale::GetSegment(int num)
{
  if (num<(int)segs.size())
    return &segs[num];
  else
    return 0;
}

void __fastcall TScale::DeleteSegment(int num)
{
  if (num>=(int)segs.size()) return;
  std::vector<TSegment>::iterator p;
  p=segs.begin();
  p+=num;
  segs.erase(p);
}

DWORD __fastcall TScale::MinKvant()
{
  if (!segs.size()) return 0;
  std::vector<TSegment>::iterator p;
  p=segs.begin();
  return (*p).min_kvant;
}

DWORD __fastcall TScale::MaxKvant()
{
  if (!segs.size()) return 0;
  std::vector<TSegment>::iterator p;
  DWORD x=0;
  for (p=segs.begin(); p<segs.end(); p++)
    if ((*p).max_kvant>x)
      x=(*p).max_kvant;
  return x;
}

float __fastcall TScale::MinY()
{
  if (!segs.size()) return 0;
   std::vector<TSegment>::iterator p  =  segs.begin(),end = segs.end();
  float x=MAXFLOAT, y;
  while( p<end)
  {
    y=(*p).min_y();
    if (y<x)
      x=y;
    ++p;
  }

  return x;
}

float __fastcall TScale::MaxY()
{
  if (!segs.size()) return 0;
  std::vector<TSegment>::iterator p  =  segs.begin(),end = segs.end();
  float x=-MAXFLOAT, y;
  while( p<end)
  {
    y=(*p).max_y();
    if (y>x)
      x=y;
    ++p;  
  }
  return x;
}

void __fastcall TScale::SetName(wchar_t *name)
{
  if (s_name) delete [] s_name;
  s_name=0;
  if (name)
  {
    s_name=new wchar_t[wcslen(name)+1];
    safe_strcpy(s_name,name);
  }
}

void __fastcall TScale::SetName( char   *name)
{
  if (s_name) delete [] s_name;
  s_name=0;
  if (name)
  {
    s_name=new wchar_t[strlen(name)+1];
    Ansi2Unicode(s_name,name);
  }
}

int __fastcall TScale::GetName(wchar_t *name, int size)
{
  int len;
  len=wcslen(s_name)+1;
  if (!name) return len;
  if (size<len) return -len;
  safe_strcpy(name,s_name);
  return len;
}

int __fastcall TScale::GetName( char   *name, int size)
{
  int len;
  len=wcslen(s_name)+1;
  if (!name) return len;
  if (size<len) return -len;
  Unicode2Ansi(name,s_name);
  return len;
}

wchar_t * __fastcall TScale::w_str() const
{
  return s_name;
}

DWORD __fastcall TScale::Calc(DWORD kvant, float &y)
{
  DWORD ret=SCL_ERR_NOKVANT;
  if (kvant<MinKvant() || kvant>MaxKvant()) return ret;
  std::vector<TSegment>::iterator p;
  for (p=segs.begin(); p!=segs.end(); p++)
    if ((*p).InRange(kvant))
    {
      y=(*p).calc(kvant);
      ret=SCL_ERR_SUCCESS;
      break;
    }
  if (ret==SCL_ERR_SUCCESS)
  {
    if ((limits & SCL_LIMIT_MINALARM) && y<alarm_min)
      ret=SCL_ERR_MINALARM;
    else if ((limits & SCL_LIMIT_MAXALARM) && y>alarm_max)
      ret=SCL_ERR_MAXALARM;
    else if ((limits & SCL_LIMIT_MINWARN) && y<warn_min)
      ret=SCL_ERR_MINWARN;
    else if ((limits & SCL_LIMIT_MAXWARN) && y>warn_max)
      ret=SCL_ERR_MAXWARN;
  }
  return ret;
}

DWORD __fastcall TScale::ReverseCalc(float y, DWORD &kvant)
{
  if (y<MinY() || y>MaxY()) return SCL_ERR_NOVALUE;
  std::vector<TSegment>::iterator p;
  for (p=segs.begin(); p!=segs.end(); p++)
    if (y>=(*p).min_y() && y<=(*p).max_y())
      if ((*p).reverse_calc(y,kvant))
        return SCL_ERR_SUCCESS;
  return SCL_ERR_NOREVERSE;
}

