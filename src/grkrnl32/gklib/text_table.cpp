#pragma hdrstop
#include "text_table.hpp"

 text_table_t::text_table_t (wchar_t * file_name):stream(NULL)
 {
  open(file_name);
 }

 bool  __fastcall text_table_t::open(const wchar_t * _file_name,bool force)
 {
    file_name = _file_name;
    close();
    bool ret = false;
    stream  = new TFileStream(file_name.c_str());
    if(stream && stream->IsGood())
    {
    storage = new TStorage(stream);
    if(storage)
    {
    DWORD version = 1;
    BYTE sw[] = "text_table_t";
    ret =  storage->open(sw,sizeof(sw),&version);
    if(!ret )
      {
        if(force)
        {
         ret =  storage->open(sw,sizeof(sw),&version,true);
         if(!ret)
          ret = storage->create(version,sw,sizeof(sw),8192);
        }
      }
    }
    }

   return ret;
 }

 void  __fastcall text_table_t::close()
 {
  if(storage)
   {
     storage->flush();
     storage->close();
     delete storage;
     storage = 0;
   }
   if(stream)
   {
    stream->Flush();
    delete stream;
    stream = NULL; 
   }
 }


 DWORD __fastcall text_table_t::add(wchar_t * text,DWORD sz )
 {
  DWORD ret = -1;
  if(storage && text && *text)
   {
    DWORD len    = sz ? sz<<1:lstrlenW(text)<<1;
    DWORD reserv = len<<1;
    ret = storage->add_record(text,len,reserv);
   }
  return ret;
 }

 DWORD __fastcall text_table_t::get_length(DWORD id)
 {
  DWORD ret = 0;
  if(storage)
     {
      storage->get_record_info(id,NULL,1,&ret);
     } 
  return ret>>1;
 }

 DWORD __fastcall text_table_t::get_text  (DWORD id, wchar_t * buffer,DWORD cbsz)
 {
  DWORD ret = 0;
  if(storage && buffer && cbsz)
  {
   ret = storage->get_record(id,buffer,cbsz)>>1;
   buffer[ret] = 0;
  }
  return ret;
 }

 DWORD __fastcall text_table_t::update(DWORD id, wchar_t * text,DWORD sz)
 {
  DWORD ret = -1;
  if(storage)
   {
    storage->resize(id);
    sz<<=1;
    if(!sz)
       sz = lstrlenW(text)*sizeof(wchar_t);
    ret = storage->modify_record(id,text,sz);

   }
  return ret;
 }





 
