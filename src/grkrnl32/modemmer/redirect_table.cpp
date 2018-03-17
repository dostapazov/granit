#pragma hdrstop
#include "modemmer_main.hpp"
#include <tregstry.hpp>
#include <kestrm.hpp>
#include <algorithm>

using namespace std;

 int __fastcall redir_entry_cmp(const LPREDIRECT_ENTRY  re1,const LPREDIRECT_ENTRY  re2)
 {
  int ret = modem_addr_cmp(re1->from_addr,re2->from_addr);
  if(!ret)
  {
   ret =  int(re1->fa)- int(re2->fa);
   if(!ret)
     ret = modem_addr_cmp(re1->to_addr,re2->to_addr);
  }
  return ret;
 }

 struct redir_entry_less
 {
  bool operator()(const LPREDIRECT_ENTRY  re1,const LPREDIRECT_ENTRY  re2);
  bool operator()(const REDIRECT_ENTRY & re1,const REDIRECT_ENTRY & re2);
 };

 struct redir_entry_more
 {
  bool operator()(const LPREDIRECT_ENTRY  re1,const LPREDIRECT_ENTRY  re2);
  bool operator()(const REDIRECT_ENTRY & re1,const REDIRECT_ENTRY & re2);
 };

 struct redir_entry_equal
 {
  bool operator()(const LPREDIRECT_ENTRY  re1,const LPREDIRECT_ENTRY  re2);
  bool operator()(const REDIRECT_ENTRY & re1,const REDIRECT_ENTRY & re2);
 };

inline  bool redir_entry_less::operator()(const REDIRECT_ENTRY & re1,const REDIRECT_ENTRY & re2)
{   return (redir_entry_cmp((LPREDIRECT_ENTRY)&re1,(LPREDIRECT_ENTRY)&re2)< 0)  ? true:false;  }

inline   bool redir_entry_less::operator()(const LPREDIRECT_ENTRY re1,const LPREDIRECT_ENTRY  re2)
{   return (redir_entry_cmp(re1,re2)< 0)  ? true:false;  }

inline  bool redir_entry_more::operator()(const REDIRECT_ENTRY & re1,const REDIRECT_ENTRY & re2)
{   return (redir_entry_cmp((LPREDIRECT_ENTRY)&re1,(LPREDIRECT_ENTRY)&re2)> 0)  ? true:false;  }

inline   bool redir_entry_more::operator()(const LPREDIRECT_ENTRY  re1,const LPREDIRECT_ENTRY  re2)
{   return (redir_entry_cmp(re1,re2)> 0)  ? true:false;  }

inline  bool redir_entry_equal::operator()(const REDIRECT_ENTRY & re1,const REDIRECT_ENTRY & re2)
{   return (redir_entry_cmp((LPREDIRECT_ENTRY)&re1,(LPREDIRECT_ENTRY)&re2)== 0)  ? true:false;  }

inline   bool redir_entry_equal::operator()(const LPREDIRECT_ENTRY  re1,const LPREDIRECT_ENTRY  re2)
{   return (redir_entry_cmp(re1,re2)== 0)  ? true:false;  }



 bool   __fastcall  TRedirectTable::rlock(DWORD to )
 {
  return locker.LockForRead(to);
 }

 bool   __fastcall  TRedirectTable::runlock()
 {
  return locker.ReadUnlock();
 }

 bool   __fastcall  TRedirectTable::wlock(DWORD to )
 {
  return locker.LockForWrite(to);
 }

 bool   __fastcall  TRedirectTable::wunlock()
 {
  return locker.WriteUnlock();
 }

 void   __fastcall  TRedirectTable::release()
 {
  wlock();
  std::vector<LPREDIRECT_ENTRY>::reverse_iterator beg = table.rbegin(),end = table.rend();
  while(beg<end)
  {
   delete (*beg);
   beg++;
  }
  table.clear();
  wunlock();
 }

 DWORD  __fastcall  TRedirectTable::write (HKEY  key)
 {
  DWORD ret;
  TMemoryStream ms;
  rlock();
  ret = 0;
  std::vector<LPREDIRECT_ENTRY>::iterator beg = table.begin(),end = table.end();
  while(beg<end)
    {
     ret+=sizeof(REDIRECT_ENTRY);
     ms.Write(*beg,sizeof(REDIRECT_ENTRY));
     beg++;
    }
  runlock();
  TRegsWriter rw(DupKey(key));
  rw.WriteDword(REGDW_REDIR_ENABLE,(DWORD)redirect_enabled,true);
  rw.WriteBytes(REGSTR_REDIR_TABLE,ms.GetMemory(),ms.GetSize(),true);
  return ret;
 }

 DWORD  __fastcall  TRedirectTable::read  (HKEY key)
 {
  DWORD ret = 0;
  release();
  TRegsReader rr(DupKey(key));
  DWORD vt;
  wlock();
  this->redirect_enabled = (BOOL) rr.ReadDword(REGDW_REDIR_ENABLE,FALSE,TRUE);
  if(rr.GetValueType(REGSTR_REDIR_TABLE,&vt,&ret,true))
  {
  ret/=sizeof(REDIRECT_ENTRY);
  LPREDIRECT_ENTRY  lpre = new REDIRECT_ENTRY[ret];
  rr.ReadBytes(REGSTR_REDIR_TABLE,(LPVOID)lpre,ret*sizeof(REDIRECT_ENTRY),true);

  table.resize(ret);
  std::vector<LPREDIRECT_ENTRY>::iterator beg = table.begin();
  LPREDIRECT_ENTRY  re_ptr = lpre;
  for(DWORD i = 0;i<ret;i++)
   {
    LPREDIRECT_ENTRY re = new REDIRECT_ENTRY;
    memcpy(re,re_ptr++,sizeof(*re));
    *beg = re;
    beg++;
   }
  delete []lpre;
  }
  wunlock();
  return ret;
 }

 DWORD  __fastcall  TRedirectTable::add   (LPREDIRECT_ENTRY re)
 {
  /*Добавление новой записи*/
  DWORD ret = -1;
  wlock();
  std::vector<LPREDIRECT_ENTRY>::iterator insert_point = std::lower_bound(table.begin(),table.end(),re,redir_entry_less());
  if(!size() || insert_point == table.end() || redir_entry_cmp(*insert_point,re))
    {
     LPREDIRECT_ENTRY lre = new REDIRECT_ENTRY;
     memcpy(lre,re,sizeof(*lre));
     ret = distance(table.begin(),table.insert(insert_point,lre));
    }
  wunlock();
  return ret;
 }

 DWORD  __fastcall  TRedirectTable::remove(DWORD index)
 {
  DWORD ret = -1;
  wlock();
  if(index<table.size())
   {
     std::vector<LPREDIRECT_ENTRY>::iterator remove_point = table.begin();
     advance(remove_point,index);
     delete (*remove_point);
     table.erase(remove_point);
     ret = 0;
   }
  wunlock();
  return ret;
 }

 DWORD  __fastcall  TRedirectTable::update(DWORD index,LPREDIRECT_ENTRY re)
 {
  DWORD ret;
  wlock();
  remove(index);
  ret = add(re); 
  wunlock();
  return ret;
 }


 bool __fastcall check_redir_entry(LPREDIRECT_ENTRY re,LPREDIRECT_ENTRY se)
 {
  bool ret = false;
  if(se->from_addr.modem == re->from_addr.modem)
  {
    if(se->from_addr.line == re->from_addr.line || re->from_addr.line == MODEM_ADDR_MAXVALUE)
      {
       if(se->fa == re->fa || re->fa == FA_ALL)
         ret = true;
      }
  }
  return ret;
 }

 DWORD  __fastcall  TRedirectTable::make_redirect_list(LPMPROTO_HEADER mph,list<modem_addr> & lst,LPDWORD ropt)
 {
  DWORD ret = 0;
  rlock();
  if(this->redirect_enabled && table.size())
  {
    REDIRECT_ENTRY se;
    ZeroMemory(&se,sizeof(se));
    se.dw_size  =sizeof(se);
    se.from_addr.addr = mph->addr_from.addr;
    se.fa = mph->fa;
    se.to_addr.addr = 0;
    std::vector<LPREDIRECT_ENTRY>::iterator end   = table.end();
    std::vector<LPREDIRECT_ENTRY>::iterator point = std::lower_bound(table.begin(),end,&se,redir_entry_less());
    while(point<end && !redir_entry_more()(&se,*point))
    {
     LPREDIRECT_ENTRY re = *point;
     if(!(re->options&MDM_REDIR_DISABLED) && check_redir_entry(re,&se))
       {
        lst.push_back(modem_addr(re->to_addr));
        if(ropt)
           *ropt|=re->options;
       }
     point++;
    }

   lst.sort();
   lst.unique();
   ret = lst.size();

  }
  runlock();
  return ret;
 }

BOOL   __fastcall  TRedirectTable::enum_entry(DWORD index,LPREDIRECT_ENTRY re)
{
 BOOL ret = false;
 rlock();
 if(index<table.size())
 {
  memcpy(re,table.at(index),sizeof(*re));
  ret = TRUE;
 }
 runlock();
 return ret;
}

BOOL   __fastcall  TRedirectTable::redirect_enable(BOOL enable)
{
 BOOL ret = FALSE;
 wlock();
 if(redirect_enabled != enable)
    {
     redirect_enabled = enable;
     ret = TRUE;
    } 
 wunlock();
 return ret;
}

