#include <kertlio.hpp>
namespace KeRTL
{

 DWORD       TMappedFile::sys_page_size  = TSysInfo().GetPageSize();
 TMappedFile::TMappedFile():inc_value(0),
                    mem_size (0),
                    beg_ptr  (0),
                    position (0)

 {
 }


 TMappedFile::TMappedFile(wchar_t * file_name,DWORD inc_val  ,bool ro )
 {
  open(file_name,inc_val,ro);
 }

 TMappedFile::TMappedFile(char    * file_name,DWORD inc_val ,bool ro )
 {
  open(file_name,inc_val,ro);
 }

 bool __fastcall TMappedFile::shrinkto(DWORD size)
 {
  if(is_good())
  {
   used_size = size;
   position  = KeRTL::MIN(position,used_size);
   return true;
  }
  return false;
 }

 bool   __fastcall TMappedFile::reserve(DWORD size)
 {
  if(is_good())
  {
   if(get_unused_size()<size )
   {
    if(growto(size-get_unused_size()))
    {used_size+=size; return true;}
   }
   else
   {    used_size+=size;  return true;   }   
  }
  return false;
 }

 bool __fastcall TMappedFile::growto(DWORD size)
 {
   /*Увеличение размера  на size*/
   
   DWORD inc_size = inc_value*sys_page_size;
   div_t dt = div(size,inc_size );
   DWORD new_size     = mem_size+inc_size*(dt.quot + (dt.rem ? 1:0));
   dt = div(new_size,sys_page_size);
   new_size+=dt.rem;
   if(file.SetSize(new_size))
   {
       mem_size = new_size;
       map_file.Unmap();
       map_file.Close();
       map_file.Create(file());
       beg_ptr =(LPBYTE) map_file.Map(beg_ptr,FILE_MAP_ALL_ACCESS ,0,mem_size);
       if(!beg_ptr)
          beg_ptr =(LPBYTE) map_file.Map(NULL,FILE_MAP_ALL_ACCESS ,0,mem_size);
       after_mapping();
       return beg_ptr ? true:false;
    }
    return false;
 }

 void   __fastcall TMappedFile::after_mapping(){};

 bool __fastcall   TMappedFile::init_mm(char * name,DWORD inc_val,bool ro)
 {
  position  = 0;
  beg_ptr   = NULL;
  used_size = 0;
  inc_value = inc_val ;
  if(file.IsValid())
  {
   used_size = mem_size  = (DWORD)file.GetSize();
   if(!mem_size )
      growto(inc_value*sys_page_size);
    map_file.Create(file());
  }
  else
    map_file.Open(name,PAGE_READWRITE);
    position = 0;
    beg_ptr  =(LPBYTE) map_file.Map(NULL,ro ? FILE_MAP_READ:FILE_MAP_WRITE);
    if(beg_ptr)
    {
     if(!mem_size)
       {
        MEMORY_BASIC_INFORMATION mbi;
        VirtualQuery(beg_ptr,&mbi,sizeof(mbi));
        mem_size = used_size = mbi.RegionSize;
       }
     after_mapping();
    } 
    return is_good();
  }

 bool __fastcall   TMappedFile::init_mm(wchar_t * name,DWORD inc_val,bool ro)
 {
  position  = 0;
  beg_ptr   = NULL;
  used_size = 0;
  inc_value = inc_val ;
  if(file.IsValid())
  {
   used_size = mem_size  = (DWORD)file.GetSize();
   if(!mem_size )
      growto(inc_value*sys_page_size);
  }
      map_file.Create(NULL,name);
      position = 0;
      beg_ptr  =(LPBYTE) map_file.Map(NULL,ro ? FILE_MAP_READ:FILE_MAP_WRITE);
      after_mapping();
    return is_good();
  }

 bool __fastcall   TMappedFile::open  (wchar_t * file_name,DWORD inc_val  ,bool ro )
 {
  DWORD cd    = ro ? OPEN_EXISTING : OPEN_ALWAYS;
  DWORD fs    = FILE_SHARE_READ;
  DWORD acc   = GENERIC_READ;
  if(!ro)
    acc|=GENERIC_WRITE;
  if(file.Open(file_name,cd,fs,acc))
     return init_mm(file_name,inc_val,ro);
     
  return false;
 }

 bool __fastcall   TMappedFile::open  ( char   * file_name,DWORD inc_val  ,bool ro )
 {
  DWORD cd    = ro ? OPEN_EXISTING : OPEN_ALWAYS;
  DWORD fs    = FILE_SHARE_READ;
  DWORD acc   = GENERIC_READ;
  if(!ro)
  acc|=GENERIC_WRITE;
  if(file.Open(file_name, cd,fs,acc))
     {
      map_file.Create(file(),(char*)NULL,0,PAGE_READWRITE);
     }
     else
     map_file.Open(file_name,PAGE_READWRITE);
     init_mm(file_name,inc_val,ro);
  return this->is_good();
 }

 bool __fastcall   TMappedFile::create(wchar_t * file_name,DWORD inc_val  )
 {
  DWORD cd    = CREATE_ALWAYS;
  DWORD fs    = FILE_SHARE_READ;
  DWORD acc   = GENERIC_READ|GENERIC_WRITE;
  if(file.Open(file_name, cd,fs,acc))
     return init_mm(file_name,inc_val,false);
  return false;
 }

 bool __fastcall   TMappedFile::create( char   * file_name,DWORD inc_val  )
 {

  DWORD cd    = CREATE_ALWAYS;
  DWORD fs    = FILE_SHARE_READ;
  DWORD acc   = GENERIC_READ|GENERIC_WRITE;
  if(file.Open(file_name, cd,fs,acc))
     return init_mm(file_name,inc_val,false);
  return false;
 }

 void __fastcall TMappedFile::close()
 {

  if(is_good())
  {
   map_file.Close();
   file.SetSize(used_size);
   file.Close();
  } 
  beg_ptr   = 0;
  position  = 0;
  mem_size  = 0;
  used_size = 0;
 }

 void __fastcall TMappedFile::flush()
 {
  map_file.Flush();
  file.Flush();
 }


 int    __fastcall TMappedFile::write(void * data,DWORD size )
 {
  if(is_good())
  {
  if(position+size>mem_size)
      growto(size);
  if(beg_ptr)
  {
   memcpy(beg_ptr+position,data,size);
   position+=size;
   used_size = KeRTL::MAX(position,used_size);
   return size;
  }
  }
  return 0;
 }

 int    __fastcall TMappedFile::read (void * buffer,DWORD sz)
 {
  if(is_good())
  {
  DWORD ret = KeRTL::MIN(used_size-position,sz);
  memcpy(buffer,beg_ptr+position,ret);
  position+=ret;
  return (int)ret;
  }
  return 0;
 }

 DWORD  __fastcall TMappedFile::seek(DWORD pos,DWORD sm)
 {
   switch(sm)
   {
    case FILE_BEGIN  : position = KeRTL::MIN(pos,mem_size);break;
    case FILE_CURRENT: position = KeRTL::MIN(position+pos,mem_size);break;
    case FILE_END    : position = used_size;break;
   }
   return position;
 }



}
 
