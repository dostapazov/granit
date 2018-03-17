#pragma hdrstop
#include <storage.hpp>
#include <algorithm>

#ifndef _NO_NAMESCPACE
namespace KeRTL{
#endif




  bool   __fastcall TStorage::close()
  {
    if(is_good())
    {
     save_unused_info();
     unused_list.clear();
     stream->Flush();
     stream->SetSize(stream->get_using_size());
     s_hdr.check_sum = calc_cs();
     flush();
     FillMemory(&s_hdr,sizeof(s_hdr),-1);
     return true;
    }
    return false;
  };

  void   __fastcall TStorage::flush()
  {
   if(is_good())
   {
    stream->Seek(0,TStream::smFromBegin);
    stream->Write(&s_hdr,sizeof(s_hdr));
    stream->Flush();
   }
  }

  bool   __fastcall TStorage::is_good()
  {
   return (stream && stream->IsGood() && s_hdr.rec_count!=(DWORD)-1) ? true:false;
  }

  bool   __fastcall TStorage::create         (DWORD ver,LPBYTE secret_words,DWORD sw_size,DWORD fixed_capacity)
  {
   bool ret = false;
   if(stream && stream->IsGood())
   {
    if(stream->SetSize(sizeof(s_hdr)))
    {
     ZeroMemory(&s_hdr,sizeof(s_hdr));
     s_hdr.version = ver;
	 s_hdr.version |=c_has_check_sum;
	 //#ifdef _WIN64
	 // strncpy(s_hdr.secret_word,secret_words,1+KeRTL::MIN(sw_size,(DWORD)sizeof(s_hdr.secret_word)))
	 //#else
	  memcpy((LPSTR)s_hdr.secret_word,(LPSTR)secret_words,KeRTL::MIN(sw_size,(DWORD)sizeof(s_hdr.secret_word)));
  //#endif
	 s_hdr.check_sum = 0;
     s_hdr.fixa_capacity =  fixed_capacity;
     ret = true;
     fixed_areas.clear();
     flush();
    }
   }
   return ret;
  }

  void   __fastcall TStorage::clear()
  {
   if(is_good())
   {
    s_hdr.rec_count = 0;
    stream->SetSize(0);
    stream->Reserve(sizeof(s_hdr));
    unused_list.clear();
    fixed_areas.clear();
    flush();
   }
  }

  bool   __fastcall TStorage::init()
  {
   //Инициализация - Создание списка смещений до фиксированных областей
   if(is_good())
   {
    fixed_areas.clear();
    if(stream->GetSize()>sizeof(s_hdr))
    {
     fixa_header fah;
     fah.prev = 0;
     fah.next = sizeof(s_hdr);
     do
     {
      fixed_areas.push_back(fah.next);
      stream->Seek(fah.next,TStream::smFromBegin);
     }while(fah.next && stream->Read(&fah,sizeof(fah)));
     restore_unused_info();
    }
    return true;
   }
   return false;
  }

  DWORD  __fastcall TStorage::get_record_offset(DWORD rec_no,DWORD chunk)
  {
   DWORD ret = -1;
   if(is_good() && rec_no < s_hdr.rec_count)
   {
    div_t dt = div(rec_no,s_hdr.fixa_capacity);
    if((DWORD)dt.quot<fixed_areas.size())
    {
     stream->Seek(fixed_areas[dt.quot]+dt.rem*sizeof(DWORD)+sizeof(fixa_header),TStream::smFromBegin);
     stream->Read(&ret,sizeof(ret));
     while(chunk && ret!=(DWORD)-1)
     {
      rec_chunk rc = {-1};
      stream->Seek(ret,TStream::smFromBegin);
      stream->Read(&rc,sizeof(rc));
      ret = rc.next_chunk;
      chunk--;
     }
    }
   }
   return ret ? ret:-1;
  }

  bool   __fastcall TStorage::open           (LPBYTE secret_words,DWORD sw_size,LPDWORD ver, bool IgnoreCheckSum)
  {
   bool ret = false;
   if(stream && stream->IsGood() && stream->GetSize()>=sizeof(s_hdr))
   {
    stream->Seek(0,TStream::smFromBegin);
    stream->Read(&s_hdr,sizeof(s_hdr));

    if(
       (check_cs() || IgnoreCheckSum )&&
       memcmp(s_hdr.secret_word,secret_words,
              KeRTL::MIN(sw_size,(DWORD)sizeof(s_hdr.secret_word)+(s_hdr.version & c_has_check_sum) ? DWORD(0):(DWORD)sizeof(DWORD)))==0)
    {
     ret =  true;
     if(ver)
       *ver = (s_hdr.version & ~c_has_check_sum) ;
     init();
    }
    else
     FillMemory(&s_hdr,sizeof(s_hdr),-1);
   }
   return ret;
  }

  DWORD  __fastcall TStorage::create_fixa()
  {
   //Создание фиксированной области
   if(is_good())
   {
   fixa_header fah = {0};
   if(this->fixed_areas.size())
   {
    fah.prev = fixed_areas[fixed_areas.size()-1];
   }
   DWORD fixa_offset = //stream->Seek(0,TStream::smFromEnd); // Получить смещение до начала фиксированной области
   //stream->Reserve(sizeof(fixa_header)+sizeof(DWORD)*s_hdr.fixa_capacity); // Зарезервировать под неё место
   this->reserve(sizeof(fixa_header)+sizeof(DWORD)*s_hdr.fixa_capacity);
   if(fah.prev)
   {
    stream->Seek(fah.prev+sizeof(DWORD),TStream::smFromBegin);//Создать ссылку на новую область у предыдущей
    stream->Write(&fixa_offset,sizeof(fixa_offset));
   }
   stream->Seek(fixa_offset,TStream::smFromBegin); //Проинициализировать новую область
   stream->Write(&fah,sizeof(fah));
   fixed_areas.push_back(fixa_offset);//Сохранить в списке смещений до фиксированных областей
   return fixa_offset;
   }
   return -1;
  }

  DWORD  __fastcall TStorage::do_add_chunk(DWORD prev_offset,DWORD size)
  {
    /*Добавляет кусочек к существующей записи*/
    DWORD chunk_offset = reserve(size);
 // Смещение до нового кусочка
    rec_chunk rc;
    rc.capacity = size;
    rc.size     = 0;
    rc.prev_chunk = prev_offset;
    rc.next_chunk = 0;
    stream->Seek(prev_offset+3*sizeof(DWORD),TStream::smFromBegin);
    stream->Write(&chunk_offset,sizeof(DWORD));
    stream->Seek (chunk_offset,TStream::smFromBegin);
    stream->Write(&rc,sizeof(rc));
    return chunk_offset;

  }

  DWORD  __fastcall TStorage::create_record  (DWORD  chunks_count,LPDWORD sizes,DWORD & rec_offset)
  {
   if(is_good() && chunks_count && sizes)
   {
    DWORD rec_index = this->s_hdr.rec_count;
    div_t dt = div(rec_index,this->s_hdr.fixa_capacity);
    DWORD    fixa_num    = dt.quot;
    DWORD    fixa_offset = dt.rem;
    while(fixed_areas.size()<=fixa_num)
        create_fixa();
         fixa_offset = fixed_areas[fixa_num]+fixa_offset*sizeof(DWORD)+sizeof(fixa_header);

    rec_offset = reserve(*sizes);
    stream->Seek(fixa_offset,TStream::smFromBegin);
    stream->Write(&rec_offset,sizeof(rec_offset));
    rec_chunk rc = {0};
    stream->Seek(rec_offset,TStream::smFromBegin);
    rc.capacity   = *sizes;
    rc.size       = 0;
    rc.prev_chunk = 0;
    rc.next_chunk = 0;
    stream->Write(&rc,sizeof(rc));
    ++s_hdr.rec_count;
    DWORD prev_chunk = rec_offset;
    while(--chunks_count)
     {
       ++sizes;
       prev_chunk =  do_add_chunk(prev_chunk,*sizes);
     }
    return rec_index;
   }
   return -1;
  }

  DWORD  __fastcall TStorage::add_record     (LPVOID data ,DWORD dw_size,DWORD dw_reserv)
  {
     DWORD rec_offset ;
     if(dw_reserv<dw_size || dw_reserv == (DWORD)-1)
        dw_reserv = dw_size;
     DWORD rec_no     = create_record(1,&dw_reserv,rec_offset);
     if(rec_no<s_hdr.rec_count)
       {
        rec_chunk rc;
        stream->Seek(rec_offset,TStream::smFromBegin);
        stream->Read(&rc,sizeof(rc));
        rc.size = dw_size;
        stream->Seek(rec_offset,TStream::smFromBegin);
        stream->Write(&rc,sizeof(rc));
        stream->Write(data,dw_size);
       }
     return rec_no;
  }


  DWORD  __fastcall TStorage::add_chunk       (DWORD rec_no,LPVOID data,DWORD dw_size,DWORD dw_reserv  )
  {
    DWORD ret = -1;
    if(is_good() && rec_no< s_hdr.rec_count)
    {
     if(dw_reserv<dw_size || dw_reserv == (DWORD)-1)
        dw_reserv = dw_size;
     DWORD  ch_count;
     get_record_info(rec_no,&ch_count);
     ret = do_add_chunk(get_record_offset(rec_no,ch_count-1),dw_reserv);
      if(ret!=(DWORD)-1)
      {
       if(data && dw_size)
       {
       //rec_chunk rc;
        stream->Seek(ret+sizeof(DWORD),TStream::smFromBegin);
        stream->Write(&dw_size,sizeof(dw_size));
        stream->Seek(2*sizeof(DWORD),TStream::smFromCurr);
        stream->Write(data,dw_size);
       }
       ret = ch_count;
      }
    }
    return ret;

  }

  DWORD  __fastcall TStorage::resize(DWORD count)
  {
   DWORD rec_count = this->get_record_count();
   if(count!=DWORD(-1))
   {
     while(rec_count<count)
        {
          add_record(NULL,0);
          ++rec_count;
        }
   }
    return rec_count;
  }

  DWORD  __fastcall TStorage::get_record     (DWORD  index,LPVOID buff,DWORD buff_sz,DWORD chunk )
  {
   if(buff && buff_sz)
   {
     DWORD rec_offset = this->get_record_offset(index,chunk);
     if(rec_offset<(DWORD)(-1))
     {
      stream->Seek(rec_offset,TStream::smFromBegin);
      rec_chunk rc;
      stream->Read(&rc,sizeof(rc));
      DWORD ret = KeRTL::MIN(rc.size,buff_sz);
      stream->Read(buff,ret);
      return ret;
     }
   }
   return 0;
  }

  DWORD  __fastcall TStorage::get_record_info (DWORD rec_no,LPDWORD chunks_count,DWORD sz_count , LPDWORD sizes,bool capacity)
  {
    DWORD total_sz   = 0;
    DWORD c_count    = 0;
    rec_chunk rc;
    rc.next_chunk = get_record_offset(rec_no);
    if(rc.next_chunk!=(DWORD)-1)
    {
     DWORD sz;
     do{
        stream->Seek(rc.next_chunk,TStream::smFromBegin);
        stream->Read(&rc,sizeof(rc));
        ++c_count;
        sz = capacity ? rc.capacity : rc.size;
        total_sz +=sz; 
        if(sizes && sz_count--)
           *sizes = sz,++sizes;
       }while(rc.next_chunk);
    }
    if(chunks_count)
       *chunks_count = c_count;
    return total_sz;
  }

  DWORD  __fastcall TStorage::modify_record  (DWORD rec_no,LPVOID data,DWORD size,DWORD chunk )
  {
   DWORD rec_offset = this->get_record_offset(rec_no,chunk);
   if(rec_offset!=(DWORD)-1)
   {
    rec_chunk rc;
    stream->Seek(rec_offset,TStream::smFromBegin);
    stream->Read(&rc,sizeof(rc));
    if(size<=rc.capacity)
    {
     stream->Write(data,size);
     stream->Seek(rec_offset+sizeof(DWORD),TStream::smFromBegin);
     stream->Write(&size,sizeof(size));
    }
    else
    {
     //Перенос кусочка в конец
     unused_mark(rec_offset,rc.capacity+sizeof(rc));
     DWORD new_offset = reserve(size);//stream->Seek(0,TStream::smFromEnd);
     stream->Seek(new_offset,TStream::smFromBegin);
     rc.size = rc.capacity = size;
     stream->Write(&rc,sizeof(rc));
     stream->Write(data,size);
     if(rc.prev_chunk)
       stream->Seek(rc.prev_chunk+3*sizeof(DWORD),TStream::smFromBegin);
      else
      {
        div_t dt = div(rec_no,s_hdr.fixa_capacity);
        stream->Seek(fixed_areas[dt.quot]+dt.rem*sizeof(DWORD)+sizeof(fixa_header),TStream::smFromBegin);
      }
       stream->Write(&new_offset,sizeof(new_offset));

    }
    return rec_no;
   }
   return -1;
  }


 #pragma warn -8004
  BOOL   __fastcall TStorage::insert_chunk    (DWORD rec_no,DWORD chunk,LPVOID data,DWORD sz,DWORD dw_reserv)
  {
   //Вставить кусочек  перед заданным
    BOOL ret = FALSE;
    DWORD ch_offset = get_record_offset(rec_no,chunk);
    if(ch_offset!=DWORD(-1))
    {
    if(dw_reserv<sz || dw_reserv==(DWORD)-1)
        dw_reserv = sz;
     rec_chunk rc;
     stream->Seek(ch_offset,TStream::smFromBegin);
     stream->Read(&rc,sizeof(rc));
     rc.next_chunk = ch_offset;
     stream->Seek(ch_offset+2*sizeof(DWORD),TStream::smFromBegin); // Обновить ссылку на предыдущий кусочек
     ch_offset = reserve(dw_reserv);
     stream->Write(&ch_offset,sizeof(ch_offset));
     stream->Seek(ch_offset,TStream::smFromEnd);
     rc.capacity   = dw_reserv;
     rc.size       = data && sz ? sz: 0;
     stream->Write(&rc,sizeof(rc));
     if(data && sz)
       stream->Write(data,sz);
     if(rc.prev_chunk)
        stream->Seek(rc.prev_chunk+3*sizeof(DWORD),TStream::smFromBegin);
        else
        {
        div_t dt = div(rec_no,s_hdr.fixa_capacity);
        stream->Seek(fixed_areas[dt.quot]+dt.rem*sizeof(DWORD)+sizeof(fixa_header),TStream::smFromBegin);
        }
      stream->Write(&ch_offset,sizeof(ch_offset));
      ret = TRUE;
    }
    else
     ret = add_chunk(rec_no,data,sz,dw_reserv)!=(DWORD)-1 ? TRUE:FALSE ;

   return ret;
  }
 #pragma warn .8004

  BOOL   __fastcall TStorage::delete_chunk    (DWORD rec_no,DWORD chunk)
  {
    DWORD ch_offset = get_record_offset(rec_no,chunk);
    if(ch_offset!=DWORD(-1))
    {

     rec_chunk rc;
     stream->Seek(ch_offset,TStream::smFromBegin);
     stream->Read(&rc,sizeof(rc));
     unused_mark(ch_offset,rc.capacity+sizeof(rc));
     if(rc.prev_chunk)
        stream->Seek(rc.prev_chunk+3*sizeof(DWORD),TStream::smFromBegin); //Обновить next_chunk - в предыдущей записи
       else
       {
        if(rc.next_chunk)
        {
         stream->Seek(rc.next_chunk+2*sizeof(DWORD),TStream::smFromBegin);
         DWORD zero = 0;
         stream->Write(&zero,sizeof(DWORD));
        }
        div_t dt = div(rec_no,s_hdr.fixa_capacity);
        stream->Seek(fixed_areas[dt.quot]+dt.rem*sizeof(DWORD)+sizeof(fixa_header),TStream::smFromBegin);
       }
       stream->Write(&rc.next_chunk,sizeof(DWORD));
     return TRUE;
    }
    return FALSE;
  }

  BOOL   __fastcall TStorage::delete_record   (DWORD rec_no)
  {
   //Удаление записи состоит из удаления всех кусочков записи
   while(delete_chunk(rec_no,0))
       ;
       return TRUE;   
  }

  void   __fastcall TStorage::unused_mark(DWORD offset,DWORD cap)
  {
    //Вставка свободной области со слиянием при необходимости
    unused_space sp(offset,cap);
    if(unused_list.size())
    {
     std::vector<unused_space>::iterator ptr = std::lower_bound(unused_list.begin(),unused_list.end(),sp);
     if(ptr<unused_list.end() && offset+cap == ptr->offset)
        {
         ptr->offset = offset;
         ptr->length+=cap;
        }
        else
        {
         if(std::distance(unused_list.begin(),ptr))
         {
           --ptr;
           if(ptr->offset+ptr->length == offset)
              { ptr->length+= cap; return ;}
           ++ptr;
         }
         unused_list.insert(ptr,sp);
        }
    }
    else
    unused_list.push_back(sp);
  }


  DWORD  __fastcall TStorage::unused_find(DWORD need_size)
  {
   DWORD ret = -1;
   std::vector<unused_space>::iterator b = unused_list.begin(), e = unused_list.end();
   while(b<e && ret == (DWORD)-1)
   {
    if(b->length>=need_size)
       {
        ret = b->offset;
        b->length-= need_size;
        b->offset+=need_size;
        if(!b->length)
           unused_list.erase(b);
       }
    ++b ;
   }
   return ret;
  }

  void   __fastcall TStorage::save_unused_info()
  {
   if(is_good() )
   {
    s_hdr.unused_info = 0;

    if(unused_list.size())
    {
     s_hdr.unused_info = stream->Seek(0,TStream::smFromEnd);
     std::vector<unused_space>::iterator beg = unused_list.begin(),end = unused_list.end();
     while(beg<end)
     {
      unused_space us(beg->offset,beg->length);
      stream->Write(&us,sizeof(us));

      ++beg;
     }
    }
   }

  }

  void   __fastcall TStorage::restore_unused_info()
  {
    if(is_good() && s_hdr.unused_info && s_hdr.unused_info < stream->GetSize())
    {
     DWORD  sp_cnt = (stream->GetSize()-s_hdr.unused_info)/sizeof(unused_space);
     stream->Seek(s_hdr.unused_info,TStream::smFromBegin);
     unused_list.reserve(sp_cnt);
     for(DWORD i = 0;i<sp_cnt;i++)
     {
      unused_space us;
      stream->Read(&us,sizeof(unused_space));
      unused_list.insert(unused_list.end(),us);

     }
     stream->SetSize(s_hdr.unused_info);
    }
  }


  DWORD  __fastcall TStorage::reserve(DWORD size)
  {
    DWORD ret = unused_find(size+sizeof(rec_chunk));
    if(ret==(DWORD)-1 && is_good())
    {
     ret = stream->GetSize();
     stream->Reserve(size+sizeof(rec_chunk));
    }
    return ret;
  }

 #define OTD_DEF_POLINOM     64709
  DWORD  __fastcall TStorage::calc_cs         ()
  {

    if(s_hdr.version & c_has_check_sum)
    {
     stream->Seek(sizeof(s_hdr),TStream::smFromBegin);
     char rd_buf[0x8000];
     int  rd_len = stream->Read(rd_buf,sizeof(rd_buf));
     s_hdr.check_sum = 0;
     while(rd_len)
     {
      WORD kpk = KeRTL::calc_kpk(rd_buf,rd_len,OTD_DEF_POLINOM);
      s_hdr.check_sum+=kpk;
      rd_len = stream->Read(rd_buf,sizeof(rd_buf));
     }
      return s_hdr.check_sum ;
    }
   return 0;
  }

  BOOL   __fastcall TStorage::check_cs        ()
  {
    //В старых версиях не проверяется
    if(s_hdr.version & c_has_check_sum)
    {
     DWORD old_cs = s_hdr.check_sum;
     if(calc_cs()!=old_cs)
       return FALSE;
    }

    return TRUE;
  }



#ifndef _NO_NAMESCPACE
}//namespace KeRTL{
#endif
