#pragma hdrstop
#include "tlmdb.hpp"


 DWORD __fastcall tlm_db::create (DWORD _addr,int beg,int count,DWORD type )
 {
  /*Создание объекта*/
  DWORD ret = -1;
  sotd_addr addr(_addr);
  if(addr.addrtype() == OTD_ADDR_TYPE_SB)
    {
      addr.fa&=OTD_FA_ANALOG;
      module_list.write_lock(true);
      if(module_list.find(addr)==NULL)
      {
      tlm_module * mod = new tlm_module;
      if(addr.fa&OTD_FA_ANALOG)
          mod->create_analog(_addr,beg,count,type);
         else
          mod->create_discrete(_addr,beg,count);
       ret = module_list.insert(mod);
      }
      else
      SetLastError(-1/*ERR_MODULE_EXISTS*/);
      module_list.write_lock(false);
    }
    else
    {
      spr_list.write_lock(true);
      if(spr_list.find(addr)==NULL)
      {
       tlm_group * grp = new tlm_group;
       grp->create(_addr);
       spr_list.insert(grp);
      }
      else
      SetLastError(-1/*ERR_MODULE_EXISTS*/);
      spr_list.write_lock(false);
    }

    return ret;
 }

 tlm_base * __fastcall tlm_db::find(const otd_addr & addr)
 {
  if(addr.sb<OTD_ADDR_MAXVALUE)
     return module_list.find(addr);
   return spr_list.find(addr);
 }




 #pragma pack(push,1)
 const char  tlmdb_magic[]   = "TLMDB-low.SDTU Lab:";
 const WORD  tlmdb_entry_beg = 0x3C5B;
 const WORD  tlmdb_entry_end = 0x5D3E;

 struct tlmdb_head_rec
 {
  char  magic[sizeof(tlmdb_magic)];
  DWORD version;
 };

 #pragma pack(pop)

 DWORD __fastcall tlm_db::strm_write(TStream & out)
 {
    DWORD ret = 0;
    tlmdb_head_rec hr;
    lstrcpy(hr.magic,tlmdb_magic);
    hr.version = 0x00010001;
    out.Write(&hr,sizeof(hr));
    out<<tlmdb_entry_beg;
    write_list(out,module_list);
    write_list(out,spr_list);
    out<<tlmdb_entry_end;
    return ret;
 }

 DWORD     __fastcall tlm_db::write_list(TStream & out,tlm_list & list)
 {
  DWORD ret;
  list.write_lock(true);
  ret = list.count();
  out<<ret;
  for(DWORD i = 0;i<ret;i++)
  {
   tlm_base * mod = list[i];
   DWORD ver = mod->strm_version();
   out<<tlmdb_entry_beg<<ver;
   mod->strm_write(out);
   out<<tlmdb_entry_end;
  }
  list.write_lock(false);
  return ret;
 }

 bool __fastcall sync(TStream & in,WORD value)
 {
  WORD  r_val = 0;
  while(!in.Eos())
  {
   in.Read(((LPBYTE)&r_val)+1,1);
   if(r_val == value)
      return true;
   else
    r_val>>=8;  
  }
  return false;
 }

 DWORD     __fastcall tlm_db::read_mod_list (TStream & in)
 {
  DWORD ret = 0,ver;
  tlm_module * mod;
  in>>ret;
  module_list.write_lock(true);
  for(DWORD i=0;i<ret;i++)
  {
   if(sync(in,tlmdb_entry_beg))
   {
    in>>ver;
    mod = new tlm_module;
    mod->strm_read(in,ver);
    module_list.insert(mod);
    sync(in,tlmdb_entry_end);
   }
  }
  module_list.write_lock(false);
  return ret;
 }
 
 DWORD     __fastcall tlm_db::read_spr_list (TStream & in)
 {
  DWORD ret = 0,ver;
  tlm_group * mod;
  in>>ret;
  spr_list.write_lock(true);
  for(DWORD i=0;i<ret;i++)
  {
   if(sync(in,tlmdb_entry_beg))
   {
    in>>ver;
    mod = new tlm_group;
    mod->strm_read(in,ver);
    spr_list.insert(mod);
    sync(in,tlmdb_entry_end);
   }
  }
  spr_list.write_lock(false);
  return ret;
 }

 DWORD __fastcall tlm_db::strm_read (TStream & in)
 {
   DWORD ret = 0;
   tlmdb_head_rec hr;
   in.Read(&hr,sizeof(hr));
   if(lstrcmp(hr.magic,tlmdb_magic)==0)
   {
     if(sync(in,tlmdb_entry_beg))
     {
     ret+=this->read_mod_list(in);
     ret+=this->read_spr_list(in);
     } 
   }
   return ret;
 }

 inline bool __fastcall is_group_query(lpotd_addr addr)
 {
  return ( addr->sb == OTD_ADDR_MAXVALUE ||
           addr->fa == OTD_ADDR_MAXVALUE ||
           addr->cp == OTD_ADDR_MAXVALUE ||
           addr->pu == OTD_ADDR_MAXVALUE  ) ? true:false;
 }

 DWORD __fastcall tlm_db::handle_query(lpotd_proto op,TSimpleDataQueue * queue/*,GKHANDLE who*/)
 {
   //Обработка вызова
   DWORD ret = 0;// возврат  количество кадров помещеных в очередь
   //ПУЛ нитей обработки запросов
   //query_pool.start(0,op,queue);
   return ret;
 }

 DWORD __fastcall tlm_db::handle_data (lpotd_proto op)
 {
  //Обработка данных
  return 0;
 }
 


