/*
 Интерфейс с классом клиента
*/

#include "tlm_net_client.hpp"
#include <algorithm>

 wstring __fastcall  tlm_client::get_wtext(DWORD id)
 {
  wstring ret;
  if(id!=DWORD(-1))
  {
  read_lock();
  if(this->names.count(id))
     ret = names[id];
  read_unlock();
  }
  return ret;
 }


 string __fastcall  tlm_client::get_text(DWORD id)
 {

  wstring  w_str = get_wtext(id);
  string   ret(w_str.length(),0);
  transform(w_str.begin(),w_str.end(),ret.begin(),mb2char());
  return ret;
 }



 void    __fastcall  tlm_client::set_text(DWORD id,wstring & str)
 {
  if(id!=(DWORD)-1)
  {
  if(write_lock())
  {
   names[id] = str;
   write_unlock();
  }
  }
 }

 rec_list::size_type
       __fastcall tlm_client::find_record(DWORD id)
 {
   rec_list::size_type ret = (rec_list::size_type)-1;
   read_lock();
   mdb_record rec;
   rec.rec_id = id;
   rec_list::iterator  ptr =  std::lower_bound(this->records.begin(),records.end(),rec,mdb_record_id_less());
   if(ptr < records.end() && ptr->rec_id == id)
      ret = distance(records.begin(),ptr);
   read_unlock();
   return ret;
 }

 bool  __fastcall tlm_client::get_record (DWORD id,mdb_record_ex & rec,bool by_offs)
 {
   bool ret = false;
   read_lock();
   if(!by_offs)
   {
    //ПРеобразуем к смещению
    id = find_record(id);
   }
    //По смещению
    if(id<records.size())
       rec = records.at(id),ret = true;
   read_unlock();
   return ret;
 }

void __fastcall copy_by_mask(mdb_record * dest,mdb_record * src,DWORD mask)
{
       if(mask&MDBR_FIELD_VALUE)
          {
           dest->value = src->value;
          } 
       if(mask&MDBR_FIELD_DIAG )
          dest->diag = src->diag ;
       if(mask&MDBR_FIELD_STATE)
          dest->state = src->state ;
       if(mask&MDBR_FIELD_TIME )
          dest->time = src->time ;
       if(mask&MDBR_FIELD_MIN_VALUE )
          dest->min_value = src->min_value ;
       if(mask&MDBR_FIELD_MAX_VALUE )
          dest->max_value = src->max_value ;
       if(mask&MDBR_FIELD_NAME     )
          dest->name = src->name ;
       if(mask&MDBR_FIELD_SNAME    )
          dest->sname = src->sname ;

       if(mask&MDBR_FIELD_ADDR     )
          dest->addr = src->addr ;
       if(mask&MDBR_FIELD_OTDVALUE )
          dest->otd_val = src->otd_val ;
       if(mask&MDBR_FIELD_OTDPD    )
          dest->otd_pd = src->otd_pd ;
       if(mask&MDBR_FIELD_OPTIONS  )
          dest->options = src->options ;
       /*if(mask&MDBR_FIELD_KVALUE   )
          dest->k_value = src->k_value ;
       if(mask&MDBR_FIELD_OFFVALUE )
          dest->off_value = src->off_value ;
       */   
       if(mask&MDBR_FIELD_SCALEID  )
          dest->scale_id = src->scale_id ;
       if(mask&MDBR_FIELD_PERCENT  )
          dest->percent = src->percent ;
       if(mask&MDBR_FIELD_LASTVALUE )
          dest->last_value = src->last_value ;
       if(mask&MDBR_FIELD_SECURITY  )
          dest->security = src->security ;
}

 bool   __fastcall tlm_client::add_record (mdb_record & rec,DWORD mask)
 {
   bool ret = false;
   write_lock();
   rec_list::iterator  ptr =  std::lower_bound(this->records.begin(),records.end(),rec,mdb_record_id_less());
   if(!(ptr < records.end() && ptr->rec_id == rec.rec_id))
      {
       //Нет такого
       ptr  = records.insert(ptr,rec);
       ret  = true;
      }
      else
      {
       //Копируем по маске
       copy_by_mask(&*ptr,&rec,mask);
       if(ptr->is_discrete())
        {
         if(mask&MDBR_FIELD_VALUE)
         {
          ptr->inc_changes();
         }
        }
      }

   write_unlock();
   return ret;
 }


 bool   __fastcall tlm_client::add_kadr(MDB_KADR & kadr,DWORD & ch_flags)
 {
  bool ret = false;
  ch_flags = 0;
  write_lock();
  kadr_list::iterator  ptr = kadrs.begin(),end = kadrs.end();
  mdb_kadr k_find(kadr);
  ptr = std::lower_bound(ptr,end,k_find);
  if(ptr<end && ptr->kadr_id == kadr.kadr_id)
     {
      //Такой кадр уже есть
      //Обновляем c проверкой того что надо синхронизировать
      if(ptr->kadr_diag^kadr.kadr_diag)
         ch_flags|=MDB_KADR_FIELD_DIAG;
      if(ptr->kadr_state^kadr.kadr_state)
         ch_flags|=MDB_KADR_FIELD_STATE;
      if(ptr->discrete_changes^kadr.discrete_changes)
         ch_flags|=MDB_KADR_FIELD_DISCRETE_CHANGES;
      if(ptr->kadr_changes^kadr.kadr_changes)
         ch_flags|=MDB_KADR_FIELD_CHANGES;
      if(ptr->alarm_changes^kadr.alarm_changes)
         ch_flags|=MDB_KADR_FIELD_ALARM_CHANGES;
      if(ptr->entry_count^kadr.entry_count)
         ch_flags|=MDB_KADR_FIELD_ENTRY_COUNT;
      if(ptr->image_time^kadr.image_time)
         ch_flags|=MDB_KADR_FIELD_IMAGE;
      if(ptr->kadr_flags^kadr.kadr_flags)
         ch_flags|= MDB_KADR_FIELD_FLAGS;
      if(ptr->kadr_size.cx!=kadr.kadr_size.cx || ptr->kadr_size.cy!=kadr.kadr_size.cy)
         ch_flags|= MDB_KADR_FIELD_SIZE;
      *ptr = k_find;
     }
     else
     {
      kadrs.insert(ptr,k_find);
      ch_flags=-1;
      ret = true;
     } 

  write_unlock();
  return ret;
 }

 kadr_list::size_type  __fastcall tlm_client::find_kadr(DWORD id)
 {
  kadr_list::size_type ret = -1;
  read_lock();
  kadr_list::iterator  ptr = kadrs.begin(),end = kadrs.end();
  mdb_kadr k_find(id);
  ptr = std::lower_bound(ptr,end,k_find);
  if(ptr<end && ptr->kadr_id == id)
     ret = std::distance(kadrs.begin(),ptr);
  read_unlock();
  return ret;
 }

 bool __fastcall  tlm_client::get_kadr(DWORD id,mdb_kadr & kadr,bool by_offs)
 {
  bool ret = false;
  read_lock();
  kadr_list::size_type  offset = by_offs ? (kadr_list::size_type)id:find_kadr(id);
  if(offset < kadrs.size())
    kadr = kadrs[offset],ret = true;
  read_unlock();
  return ret;
 }

 int   __fastcall tlm_client::order_kadr(DWORD id,HWND kadr_wnd)
 {
  /* Открытие кадра  */
  //Отправить запросы при необходимости на получение изображения
  //Подписаться на    кадр если не подписан

  // и по завершению отправить соотв команды
  int ret = 0;
  if(find_kadr(id)<this->kadrs.size())
  {
   opened_kadrs[id] = kadr_wnd;
   send_order_kadr(id);
   mdb_kadr kadr;
   if(get_kadr(id,kadr,false))
   {
    string str = get_text(kadr.name_id);
    if(!str.length())
       str = get_text(kadr.sname_id);
    if(!str.length())
    {
     str.resize(512);
     str.resize(wsprintf((char*)str.c_str(),"Кадр ID %d",kadr.kadr_id)+1);
    }

    SendMessage(kadr_wnd,TC_CMD_UPDATE_KADR,0,(LPARAM)&kadr);
    SendMessage(kadr_wnd,TC_CMD_SET_CAPTION,0,(LPARAM)str.c_str());
    /*Попытка открыть кадр*/

    /* Открытие изображения */
    str = this->get_kadr_image_file(kadr.kadr_id);
    bool need_request = true;
    if(str.length())
    {
     TFile file(str.c_str(),OPEN_EXISTING);
     if(file.IsValid())
     {
       FILETIME image_time;
       GetFileTime(file(),&image_time,NULL,NULL);
       if(__int64(*((__int64*)&image_time)) == kadr.image_time)
            need_request = false;
     }
     file.Close();

    }
    if(need_request)
     request_kadr_image(kadr.kadr_id);
     else
     SendMessage(kadr_wnd,TC_CMD_SET_BKGND,0,(LPARAM)str.c_str());
     update_opened_kadr_entryes(kadr.kadr_id);

   }

  }
  return ret;
 }

 int   __fastcall tlm_client::unorder_kadr(DWORD id,HWND kadr_wnd)
 {
    if(find_kadr(id)<this->kadrs.size())
    {
     opened_kadrs.erase(id);
     send_order_kadr(id,false);
    }
   return 0;
 }


 void   __fastcall tlm_client::update_opened_kadr_entryes(DWORD kadr_id,HWND kadr_wnd)
 {

     entryes_list  ent_list;
     read_lock();
     if(!kadr_wnd && opened_kadrs.count(kadr_id))
         kadr_wnd = opened_kadrs[kadr_id];
      if(kadr_wnd)
        ent_list = k_entryes[kadr_id];
     read_unlock();
     if(kadr_wnd)
     SendMessage(kadr_wnd,TC_CMD_SET_ENTRYES,ent_list.size(),(LPARAM)&ent_list.begin());
 }


 string  __fastcall  tlm_client::get_kadr_image_file (DWORD kid)
 {
  string ret;
  mdb_kadr kadr;
  if(get_kadr(kid,kadr,false))
  {
    ret = get_text(kadr.image_id);
  }
  return ret;
 }

 wstring __fastcall  tlm_client::get_kadr_image_fileW(DWORD kid)
 {
  wstring ret;
  mdb_kadr kadr;
  if(get_kadr(kid,kadr,false))
  {
    ret = get_wtext(kadr.image_id);
  }
  return ret;
 }

 string  __fastcall   tlm_client::get_data_dir()
 {
   return data_dir;
 }

 wstring __fastcall   tlm_client::get_data_dirW()
 {
  wstring ret(data_dir.length(),0);
  transform(data_dir.begin(),data_dir.end(),ret.begin(),char2mb());
  return    ret;
 }

 void __fastcall tlm_client::receipt_kadr(DWORD kadr_id)
 {
   read_lock();
   if(k_entryes.count(kadr_id))
   {
    entryes_list & el = k_entryes[kadr_id];
    entryes_list::iterator beg =   el.begin(),end = el.end();
    while(beg<end)
      {
       receipt(beg->rec_id,false);
       ++beg;
      } 

      receipt(-1,true);
   }
   read_unlock();
 }


 void __fastcall tlm_client::receipt     (DWORD rec_id,bool end)
 {
  read_lock();
  DWORD offs = find_record(rec_id);
  if(offs<records.size())
     records.at(offs).receipt();
  if(end)
  {
   this->refresh_opened_kadrs(true);
  }
  read_unlock();
 }



