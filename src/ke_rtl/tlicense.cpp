#pragma hdrstop
#include <tlicense.hpp>
#include <kfname.hpp>
#include <rpc.h>
#include <algorithm>

 void __fastcall TLicense::close()
 {
  if(storage)
  {
     storage->close();
     delete storage;
     storage = NULL;
  }
  if(fs)
  {
   fs->Flush();
   delete fs;
   fs = NULL;
  }
 }

void   __fastcall TLicense::coder(wchar_t * str,int len)
{
 #pragma pack (push,1)
  union wrd
  {
    struct {BYTE a,b;} bt ;
    WORD   w;
  } ;
  #pragma pack(pop)

  wrd * ptr = (wrd*)str;
  wrd * ptr_end = ptr+len;
  int i = 0;
  while(ptr<ptr_end)
  {
    if(i&1)
       std::swap(ptr->bt.a,ptr->bt.b);
    ptr->w = ~ptr->w;
    ++ptr; ++i;
  }
}

int  __fastcall TLicense::coder( void  * data,int len)
{
  if(data && len)
  {
     if(len&1) ++len;
     coder((wchar_t*)data,len/sizeof(wchar_t));
     return len;
  }
 return 0;
}


int  __fastcall TLicense::gen_uuid_string (wchar_t * result,int res_size)
{
  UUID uid;
  if(result && res_size )
    {
      *result = 0;
      wchar_t * ptr = NULL;
      if(RPC_S_OK == UuidCreate(&uid) && RPC_S_OK == UuidToStringW(&uid,(LPWORD*)&ptr))
      {
       wcsncpy(result,ptr,res_size);
       RpcStringFreeW((LPWORD*)&ptr);
       return wcslen(result);
      }
    }
  return 0;
}

 /*
  Формат файла лицензии
  rec 0 строку Generate Trial License
        Содержит строку идентифицирующую компьютер
        chunk 0 = computer_name; chunk 1 volume id chunk2 - memory info
  rec 1 содержит chunk-0 string UUID и chunk 1  флаг активации
  rec 2 содержит chunk-0 счетчик отработанного времени в секундах который наращивается по таймеру в случае неактивированного ключа
               и chunk-1 пороговое значение  в секундах
*/

bool  __fastcall get_volume_info(wchar_t * vol_name,LPDWORD len)
{

  if(vol_name && len && *len)
    {
      BYTE serial_no[4] = {0};
      DWORD max_size   = sizeof(serial_no);
      wchar_t win_dir[MAX_PATH];
      GetWindowsDirectoryW(win_dir,KERTL_ARRAY_COUNT(win_dir));
      TKFName<wchar_t> path;
      path.set_fullname(win_dir);
      GetVolumeInformationW(path.get_drive(),vol_name,*len,(LPDWORD)serial_no,&max_size,NULL,NULL,0);
     int _len = snwprintf(vol_name,*len,L"%s:%02X-%02X-%02X-%02X",win_dir,(DWORD)serial_no[0],(DWORD)serial_no[1],(DWORD)serial_no[2],(DWORD)serial_no[3]);
     if(_len<=(int)len)
        {
         *len = _len;
         return true;
        }

    }
  //GetVolumeInformationW(NULL,vol_name,len,(LPDWORD)serial_no,&max_size,NULL,NULL,0);
  //swprintf(vol_name+wcslen(vol_name),L"%02X-%02X-%02X-%02X",(DWORD)serial_no[0],(DWORD)serial_no[1],(DWORD)serial_no[2],(DWORD)serial_no[3]);
  return false;
}

bool  __fastcall get_memory_info(wchar_t * text,LPDWORD len)
{
  if(text && len && *len)
  {
   MEMORYSTATUSEX ms;
   ms.dwLength = sizeof(ms);
   GlobalMemoryStatusEx(&ms);
   BYTE bt[4] = {0};
   memcpy(bt,&ms.ullTotalPhys,sizeof(bt));
   int _len = snwprintf(text,*len,L"MEMORY-%02X-%02X-%02X-%02X",(DWORD)bt[0],(DWORD)bt[1],(DWORD)bt[2],(DWORD)bt[3]);
   if(_len<=(int)*len)
     {
      *len = _len;
       return true;
     }
  }
  return false;
}

 bool __fastcall get_computer_strings(wchar_t * comp_name,LPDWORD sz1,wchar_t * vol_info,LPDWORD sz2,wchar_t * mem_info,LPDWORD sz3)
 {
   if(GetComputerNameW(comp_name,sz1)  &&
      get_volume_info (vol_info,sz2)   &&
      get_memory_info (mem_info,sz3)
      )
      return true;
   return false;
 }


 static const char * storage_sw = ".license";

 bool __fastcall TLicense::open_storage(const wchar_t * module_name)
 {
   close();
   TKFName<wchar_t> fname;
   fname.set_fullname(module_name);
   fname.set_ext(L".lic");
   fs = new KeRTL::TFileStream(fname());
   if(fs->IsGood())
   {
     storage = new TStorage(fs);
     if(storage->open((LPBYTE)storage_sw,strlen(storage_sw),&storage_version))
     {
      wchar_t stored_module_name[MAX_PATH<<1];
      DWORD len = storage->get_record(RECNO_MODNAME,stored_module_name,sizeof(stored_module_name));
      if(len && 0 == wcscmpi(fname.get_name(),stored_module_name))
         return true;
     }

   }
   else
   {
    DWORD err = GetLastError();
    char text[MAX_PATH];
    KeRTL::GetErrorMessage(err,text,sizeof(text),(HINSTANCE)0);
    KeRTL::GetErrorMessage(err,text,sizeof(text),(HINSTANCE)0);
   }
   close();
   return false;

 }


 bool __fastcall TLicense::create_storage(const wchar_t * module_name,DWORD ver,const wchar_t * ext)
 {
   close();
   TKFName<wchar_t> fname;
   fname.set_fullname(module_name);
   if(!ext || !*ext)
      ext =   L".lic";
   fname.set_ext(ext);
   fs = new KeRTL::TFileStream(fname());
   if(fs->IsGood())
   {
     storage = new TStorage(fs);
     if(storage->create(ver,(LPBYTE)storage_sw,strlen(storage_sw),8))
        {
         storage->add_record((LPVOID)fname.get_name(),(1+wcslen(fname.get_name()))*sizeof(wchar_t));
         return true;
        }
   }
   close();
   return false;
 }


 bool __fastcall TLicense::is_valid_license(wchar_t * module_name)
 {
    wchar_t comp_name[MAX_PATH];
    DWORD sz1 = MAX_PATH;
    wchar_t vol_info [MAX_PATH];
    DWORD   sz2 = MAX_PATH;
    wchar_t mem_info     [MAX_PATH];
    DWORD   sz3 = MAX_PATH;
    bool ret = get_computer_strings(comp_name,&sz1,vol_info,&sz2,mem_info,&sz3);
    if(ret && open_storage(module_name))
    {
     ret = false;
     coder(comp_name,sz1);
     coder(vol_info ,sz2);
     coder(mem_info ,sz2);

     wchar_t buff[MAX_PATH];
     DWORD   rd_size;
     rd_size = storage->get_record(RECNO_STRINGS,buff,sizeof(buff),0);
     if(rd_size == sz1*sizeof(wchar_t) && !memcmp(buff,comp_name,rd_size) )
      {
       rd_size = storage->get_record(RECNO_STRINGS,buff,sizeof(buff),1);
       if(rd_size == sz2*sizeof(wchar_t) && !memcmp(buff,vol_info,rd_size))
       {
        rd_size = storage->get_record(RECNO_STRINGS,buff,sizeof(buff),2);
        if(rd_size == sz3*sizeof(wchar_t) && !memcmp(buff,mem_info,rd_size))
        {
          ret = true;
        }
       }
      }
    }
    else
    ret = false;
   return ret;
 }



 bool __fastcall TLicense::generate_timed_license(wchar_t * module_name,DWORD days,wchar_t * comment)
 {
   //Создает лицензию c ограничением по времени
    wchar_t uuid[MAX_PATH];
    int uid_len = gen_uuid_string(uuid,MAX_PATH);
    wchar_t comp_name[MAX_PATH];
    DWORD sz1 = MAX_PATH;
    wchar_t vol_info [MAX_PATH];
    DWORD   sz2 = MAX_PATH;
    wchar_t mem_info     [MAX_PATH];
    DWORD   sz3 = MAX_PATH;
    bool ret = get_computer_strings(comp_name,&sz1,vol_info,&sz2,mem_info,&sz3);


   if(uid_len && ret && create_storage(module_name,1))
   {
    coder(comp_name,sz1);
    coder(vol_info ,sz2);
    coder(mem_info ,sz2);
    coder(uuid     ,uid_len);

    DWORD rec_no;

    rec_no = storage->add_record(comp_name,sz1*sizeof(wchar_t));
             storage->add_chunk (rec_no,vol_info,sz2*sizeof(wchar_t));
             storage->add_chunk (rec_no,mem_info,sz3*sizeof(wchar_t));
    rec_no = storage->add_record(uuid,uid_len*sizeof(wchar_t));
    DWORD active = 0;
             storage->add_chunk(rec_no,&active,sizeof(DWORD));
    DWORD timer = 0;
    rec_no = storage->add_record(&timer,sizeof(DWORD));
    timer = days*3600*24;
    storage->add_chunk(rec_no,&timer,sizeof(DWORD));
    if(!comment || !(*comment)) comment = const_cast<wchar_t*>(L";-)");
       storage->add_record(comment ,sizeof(wchar_t)*(wcslen(comment)+1));
   close();
   return true;

   }
  return false;
 }

 bool __fastcall TLicense::open_license          (wchar_t * file_name   )
 {
   close();
   return open_storage(file_name);
 }

 DWORD __fastcall TLicense::get_lic_string        (DWORD rec_no,DWORD chunk,wchar_t * buf,DWORD sz)
 {
    if(storage && storage->is_good())
    {
      DWORD ch_sizes[3];
      DWORD chunks_count = 0;
      storage->get_record_info(rec_no,&chunks_count,KERTL_ARRAY_COUNT(ch_sizes),ch_sizes);
      if(chunk<chunks_count && sz*sizeof(wchar_t)>=ch_sizes[chunk])
      {
        storage->get_record(rec_no,buf,ch_sizes[chunk],chunk);
        sz = ch_sizes[chunk]/sizeof(wchar_t);
        coder(buf,sz);
        buf[sz] = 0;
        return sz;
      }
    }
   return 0;
 }

 DWORD __fastcall TLicense::get_lic_computer_name (wchar_t * buf,DWORD sz)
 {
  return get_lic_string(RECNO_STRINGS,0,buf,sz);
 }

 DWORD __fastcall TLicense::get_lic_volume_name   (wchar_t * buf,DWORD sz)
 {
  return get_lic_string(RECNO_STRINGS,1,buf,sz);
 }

 DWORD __fastcall TLicense::get_lic_mem_info      (wchar_t * buf,DWORD sz)
 {
  return get_lic_string(RECNO_STRINGS,2,buf,sz);
 }

 DWORD __fastcall TLicense::get_lic_uid_str       (wchar_t * buf,DWORD sz)
 {
  return get_lic_string(RECNO_UID,0,buf,sz);

 }

 bool __fastcall TLicense::get_times             (LPDWORD t1,LPDWORD t2 )
 {
   if(t1) * t1 = 0;
   if(t2) * t2 = 0;
   if(storage && storage->is_good() )
   {
      if(t1)
         storage->get_record(RECNO_TIMES,t1,sizeof(*t1),0);
      if(t2)
         storage->get_record(RECNO_TIMES,t2,sizeof(*t2),1);
      return true;
   }
  return false;
 }

 bool __fastcall TLicense::is_license_active     ()
 {
   if(storage && storage->is_good() )
   {
     DWORD   active = 0;
     if(storage->get_record(RECNO_UID,&active,sizeof(active),1)==sizeof(active))
        return active ? true : false;

   }
  return false;
 }

DWORD __fastcall TLicense::get_module_name(wchar_t * buf,DWORD sz)
{
  if(storage && storage->is_good() && buf && sz)
  {
     sz = storage->get_record(RECNO_MODNAME,buf,sz,0);
     sz/=sizeof(wchar_t);
     return sz;
  }
 return 0;
}

 DWORD __fastcall TLicense::time_increase         (DWORD sec)
 {
  //Увеличивает  время работы под лицензией
  //и возвращает сколько времени осталось
  if(storage && storage->is_good())
    {
     if(is_license_active())
        return -1;
      DWORD times[2];
      storage->get_record(RECNO_TIMES,times+0,sizeof(times[0]),0);
      storage->get_record(RECNO_TIMES,times+1,sizeof(times[1]),1);
      if(sec)
      {
       times[0]+=sec;
       storage->modify_record(RECNO_TIMES,times+0,sizeof(times[0]),0);
       storage->flush();
      }
      if(times[1]>times[0])
         return times[1]-times[0];
    }
   return 0;
 }

 DWORD __fastcall TLicense::time_remind           ()
 {
   return time_increase(0);
 }

 bool  __fastcall TLicense::activate_license      (wchar_t * key,DWORD key_len)
 {
  if(key && key_len)
  {
    wchar_t buf1[MAX_PATH];
    wchar_t buf2[MAX_PATH];
    key_len = sizeof(wchar_t)*std::min(key_len,(DWORD)KERTL_ARRAY_COUNT(buf1));
    memcpy(buf1,key,key_len);
    coder(buf1,key_len);
    DWORD rd_len = storage->get_record(RECNO_UID,buf2,sizeof(buf2),0);
    if(rd_len == key_len && 0 == memcmp(buf1,buf2,key_len))
     {
       DWORD active = 1;
       storage->modify_record(RECNO_UID,&active,sizeof(active),1);
       storage->flush();
       return true;
     }
  }
  return false;
 }

 DWORD __fastcall TLicense::get_comment           (wchar_t * comment,DWORD bsz)
 {
     DWORD ret = 0;
     if(this->is_opened())
     {
       if(RECNO_COMMENT< storage->get_record_count())
       {
         DWORD chunks;
         DWORD sz;
         storage->get_record_info(RECNO_COMMENT,&chunks,1,&sz);
         ret = storage->get_record(RECNO_COMMENT,comment,bsz*sizeof(wchar_t));
       }
     }
   return ret;
 }





