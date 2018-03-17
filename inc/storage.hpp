#ifndef _KERTL_STORAGE_HPP
#define _KERTL_STORAGE_HPP

#include <kestrm.hpp>
#include <vector>

#ifndef _NO_NAMESCPACE
namespace KeRTL{
#endif


#pragma pack(push,1)

struct storage_header
{
 DWORD version;
 BYTE  secret_word[32];
 DWORD fixa_capacity;
 DWORD rec_count;
 DWORD unused_info;
};

struct fixa_header
{
 DWORD prev;
 DWORD next;
};

struct rec_chunk
{
 DWORD capacity;
 DWORD size;
 DWORD prev_chunk;
 DWORD next_chunk;
};

struct storage_header_v2
{
 DWORD version;
 BYTE  secret_word[28];
 DWORD check_sum;
 DWORD fixa_capacity;
 DWORD rec_count;
 DWORD unused_info;
};


#pragma pack(pop)

  struct unused_space
  {
   DWORD offset;
   DWORD length;
   unused_space():offset(-1),length(0){}
   unused_space(DWORD o,DWORD l):offset(o),length(l){}
   unused_space(const unused_space & usp){*this = usp;}
   unused_space & operator = (const unused_space & usp){offset = usp.offset; length = usp.length; return * this;}
  };

 inline  bool operator < (const unused_space  & usp1,const unused_space  & usp2){return usp1.offset<usp2.offset ? true:false;}

 const DWORD c_has_check_sum = 0x80000000;


class TStorage
{
  protected:
  TStream        *   stream;
  storage_header_v2     s_hdr;
  std::vector<DWORD> fixed_areas;
  std::vector<unused_space> unused_list;

  void   __fastcall unused_mark(DWORD offset,DWORD cap);
  DWORD  __fastcall unused_find(DWORD need_size);
  void   __fastcall save_unused_info();
  void   __fastcall restore_unused_info();

  bool   __fastcall init();
  DWORD  __fastcall create_fixa();
  DWORD  __fastcall get_record_offset(DWORD rec_no,DWORD chunk = 0);
  DWORD  __fastcall create_record  (DWORD  chunks_count,LPDWORD sizes,DWORD & rec_offset);
  DWORD  __fastcall do_add_chunk(DWORD prev_chunk,DWORD size);
  DWORD  __fastcall reserve(DWORD size);//Резерврование места под запись
  public:
   TStorage(TStream * _stream):stream(_stream){ FillMemory(&s_hdr,sizeof(s_hdr),-1);}
  ~TStorage(){close();}
  void   __fastcall flush();
  bool   __fastcall is_good();
  bool   __fastcall create          (DWORD ver,LPBYTE secret_words,DWORD sw_size,DWORD fixed_capacity);
  bool   __fastcall open            (LPBYTE secret_words,DWORD sw_size,LPDWORD ver, bool IgnoreCheckSum = false);
  bool   __fastcall close();
  DWORD  __fastcall create_record    (DWORD  chunks_count,LPDWORD sizes);
  DWORD  __fastcall add_record       (LPVOID data ,DWORD dw_size,DWORD dw_reserv = -1);
  DWORD  __fastcall add_chunk        (DWORD rec_no,LPVOID data,DWORD dw_size,DWORD dw_reserv = -1);
  DWORD  __fastcall get_record       (DWORD rec_no,LPVOID buff,DWORD buff_sz,DWORD chunk = 0);
  DWORD  __fastcall get_record_info  (DWORD rec_no,LPDWORD chunks_count,DWORD sz_count = 0, LPDWORD sizes = NULL,bool cap = false);
  DWORD  __fastcall modify_record    (DWORD index,LPVOID data,DWORD size,DWORD chunk = 0);
  DWORD  __fastcall get_record_count ();
  DWORD  __fastcall resize           (DWORD count);
  void   __fastcall clear();
  BOOL   __fastcall delete_chunk    (DWORD rec_no,DWORD chunk);
  BOOL   __fastcall insert_chunk    (DWORD rec_no,DWORD chunk,LPVOID data,DWORD sz,DWORD dw_reserv);
  BOOL   __fastcall delete_record   (DWORD rec_no);
  BOOL   __fastcall is_record_exist (DWORD rec_no);
  DWORD  __fastcall calc_cs         ();
  BOOL   __fastcall check_cs        ();
};

inline  DWORD  __fastcall TStorage::create_record  (DWORD  chunks_count,LPDWORD sizes)
{
 DWORD ro;
 return create_record(chunks_count,sizes,ro);
}

inline DWORD  __fastcall TStorage::get_record_count()
{
  return is_good() ? s_hdr.rec_count : 0;
}

inline BOOL   __fastcall TStorage::is_record_exist  (DWORD rec_no)
{
 return (rec_no<get_record_count() && get_record_offset(rec_no)!=(DWORD)-1) ? TRUE:FALSE;
}



#ifndef _NO_NAMESCPACE
}//namespace KeRTL{
#endif


#endif
