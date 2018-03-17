#ifndef KADRS_TABLE_HPP
#define KADRS_TABLE_HPP
#include <otd_proto_medium.h>
#include <otd_diag.h>
#include <kestrm.hpp>
#include <map>
#include <vector>
#include <algorithm>
#include <stdexcept>


using namespace std;

#pragma pack(push,1)


#pragma pack(pop)
  #define KADRS_TABLE_STREAM_VER_0 0
  #define KADRS_TABLE_STREAM_VER_1 1
  #define KADRS_TABLE_STREAM_VER   2

  typedef vector<mdb_kadr>        kadrs_list;
  typedef vector<mdb_kadr_entry>  kadr_entryes_table;
  typedef vector<DWORD>           entry_index;
  typedef vector<DWORD>           records_ids_t;

  class kadrs_table
  {
   protected:
   DWORD               gen_kadrid;
   kadrs_list          kadrs;
   kadr_entryes_table  entry_table;
   entry_index         index;

   DWORD __fastcall    build_index();
   public:
   kadrs_table():gen_kadrid(0){};
   kadrs_list::iterator  __fastcall create_kadr ();
   kadrs_list::iterator  __fastcall create_kadr  (MDB_KADR & kdr);
   kadrs_list::iterator  __fastcall find_kadr    (DWORD id);
                   bool  __fastcall find_kadr    (DWORD id,kadrs_list::iterator & kptr) {kptr = find_kadr(id); return is_valid_kadr_ptr(kptr);}

   kadrs_list::iterator  __fastcall begin_kadr   (){ return kadrs.begin(); }
   kadrs_list::iterator  __fastcall end_kadr     (){ return kadrs.end  (); }

   DWORD                 __fastcall enum_kadr    (DWORD idx);
   DWORD                 __fastcall enum_kadr    (DWORD idx,MDB_KADR & kdr);
   bool                  __fastcall remove_kadr  (DWORD id);
   bool                  __fastcall is_valid_kadr_ptr(kadrs_list::iterator kdr){return kdr< kadrs.end() ? true:false;}
   DWORD                 __fastcall kadr_clear(DWORD kadr_id);

   kadr_entryes_table::iterator __fastcall add_entry   (DWORD id,DWORD rec_id = -1);
   kadr_entryes_table::iterator __fastcall add_entry   (MDB_KADR_ENTRY & ke);
   kadr_entryes_table::iterator __fastcall find_entry  (DWORD kid,DWORD id,DWORD number);
   kadr_entryes_table::iterator __fastcall find_entry  (MDB_KADR_ENTRY & ke);
   kadr_entryes_table::iterator __fastcall entry_at    (DWORD offset);
   DWORD                 __fastcall enum_entry  (DWORD kid,DWORD idx);
   DWORD                 __fastcall enum_entry  (DWORD kid,DWORD idx,MDB_KADR_ENTRY & ke);
   bool                  __fastcall remove_entry(MDB_KADR * kdr,MDB_KADR_ENTRY & entry);
   bool                  __fastcall remove_entry(DWORD rec_id);
   DWORD                 __fastcall get_entry_range(DWORD kadr_id,kadr_entryes_table::iterator & low,kadr_entryes_table::iterator & hi);
   DWORD                 __fastcall get_entry_range(DWORD kadr_id,DWORD rec_id,kadr_entryes_table::iterator & low,kadr_entryes_table::iterator & hi);
   DWORD                 __fastcall get_kadr_range (DWORD rec_id,entry_index::iterator & low,entry_index::iterator & hi);
   DWORD                 __fastcall get_stream_ver(){return KADRS_TABLE_STREAM_VER;}
   DWORD                 __fastcall stream_write  (KeRTL::TStream & strm);
   DWORD                 __fastcall stream_read   (KeRTL::TStream & strm,DWORD ver,DWORD count);
   DWORD                 __fastcall size(){return kadrs.size();}
   DWORD                 __fastcall get_genid(){return gen_kadrid;}
   void                  __fastcall clear();
  };
  

  struct  kadr_entry_less_by_recid
  {
   kadr_entryes_table * entry_table;
   DWORD                table_size;
   mdb_kadr_entry       base;
   kadr_entry_less_by_recid(kadr_entryes_table * et,mdb_kadr_entry & ke);
   kadr_entry_less_by_recid(const kadr_entry_less_by_recid & src);
   bool  operator ()(const DWORD & idx1,const DWORD & idx2);
  };

   inline kadr_entry_less_by_recid::kadr_entry_less_by_recid(kadr_entryes_table * et,mdb_kadr_entry & ke)
   {
    entry_table = et;
    table_size = et? et->size():0;
    base = ke;
   }

   inline kadr_entry_less_by_recid::kadr_entry_less_by_recid(const kadr_entry_less_by_recid & src)
   {
    entry_table = src.entry_table;
    table_size  = src.table_size;
    base        = src.base;
   }

   inline bool  kadr_entry_less_by_recid::operator ()(const DWORD & idx1,const DWORD & idx2)
   {
    mdb_kadr_entry & e1 = idx1<table_size ? entry_table->at(idx1):base;
    mdb_kadr_entry & e2 = idx2<table_size ? entry_table->at(idx2):base;
    if(e1.rec_id == e2.rec_id)
      {
       if(e1.kadr_id == e2.kadr_id)
          return e1.number < e2.number ? true:false;
       return  e1.kadr_id < e2.kadr_id ? true:false;
      }
     return e1.rec_id < e2.rec_id ? true:false;
   }

  template <typename mdb_kadr_entry_comparer_t>
  class  kadr_entry_comparer_t
  {
   kadr_entryes_table * entry_table;
   mdb_kadr_entry       base;
   public:
   kadr_entry_comparer_t (kadr_entryes_table * et):entry_table(et),base(-1,0,0){};
   kadr_entry_comparer_t (kadr_entryes_table * et,mdb_kadr_entry & ke):entry_table(et),base(ke){};
   kadr_entry_comparer_t (const kadr_entry_comparer_t & src)
   {*this = src;}

   kadr_entry_comparer_t & operator = (const kadr_entry_comparer_t & src)
   {
     entry_table = src.entry_table;
     base        = src.base;
     return * this;
   }

   bool  operator ()(const DWORD & idx1,const DWORD & idx2);
   bool  operator ()(const mdb_kadr_entry & e1,const mdb_kadr_entry & e2){return mdb_kadr_entry_comparer_t()(e1,e2);};
  };

  template <typename entry_comparer_t>
  inline bool  kadr_entry_comparer_t<entry_comparer_t>::operator ()(const DWORD & idx1,const DWORD & idx2)
  {
     if(!entry_table) throw std::runtime_error( "kadr_entry_comparer: no entry_table" );
     DWORD table_size = entry_table->size();
     mdb_kadr_entry & e1 = idx1<table_size ? entry_table->at(idx1):base;
     mdb_kadr_entry & e2 = idx2<table_size ? entry_table->at(idx2):base;
     return (*this)(e1,e2);
  }

  struct mdb_kadr_entry_less
  {
    //Сравнение двух элементов кадров
    bool operator() (const mdb_kadr_entry & e1, const mdb_kadr_entry & e2)
    {
       if(e1.kadr_id == e2.kadr_id)
         {
          if(e1.rec_id == e2.rec_id)
             return e1.number < e2.number ? true : false;
           return e1.rec_id < e2.rec_id ? true : false;
         }
       return e1.kadr_id < e2.kadr_id ? true : false;
    }
  };


  struct mbd_kadr_entry_less_by_recid
  {
    //Сравнение двух элементов кадров по id записи
   bool operator() (const mdb_kadr_entry & e1, const mdb_kadr_entry & e2)
     {
      if(e1.rec_id == e2.rec_id)
       {
        if(e1.kadr_id == e2.kadr_id)
          return e1.number < e2.number ? true:false;
        return  e1.kadr_id < e2.kadr_id ? true:false;
       }
      return e1.rec_id < e2.rec_id ? true:false;
     }
  };


  struct point_comparer
  {
    bool operator ()(const POINT & pt1,const POINT & pt2)
    {
      if(pt1.x == pt2.x) return pt1.y < pt2.y ? true : false;
      return (pt1.x < pt2.x ) ? true : false;
    }
    bool operator ()(const MDB_GPOINT & pt1,const MDB_GPOINT & pt2)
    {
     POINT _pt1,_pt2;
     _pt1<<pt1;
     _pt2<<pt2;
     return (*this)(_pt1,_pt2);
    }
  };

  struct mdb_kadr_entry_position_less
  {
    //Сравнение двух элементов кадров по позиции
    bool operator() (const mdb_kadr_entry & e1, const mdb_kadr_entry & e2)
    {
       if(e1.kadr_id == e2.kadr_id)
         {
           POINT pt1,pt2;
           pt1.x = e1.position.x + e1.size.cx;
           pt2.x = e2.position.x + e2.size.cx;
           pt1.y = e1.position.y + e1.size.cy;
           pt2.y = e2.position.y + e2.size.cy;
           bool res = point_comparer()(pt1,pt2);
           return res;
         }
        return e1.kadr_id < e2.kadr_id ? true : false;
    }
  };

  inline bool __fastcall point_in_entry(const MDB_KADR_ENTRY & entry,const long x,const long y)
  {
   POINT pt;
   pt<<entry.position;
   if(pt.x<x && pt.y<y)
      {
       pt.x+=entry.size.cx; pt.y+=entry.size.cy;
       return (x<pt.x && y<pt.y) ? true : false;
      }
      return false;
  }

  struct mdb_kadr_entry_number_less
  {
    //Сравнение двух элементов кадров по позиции
    bool operator() (const mdb_kadr_entry & e1, const mdb_kadr_entry & e2)
    {
     return e1.number < e2.number ? true : false;
    }

  };



   // Структура для хранения информации по кадру

 class Tmouse_button_state
 {
  POINT switch_spot;
  POINT spot;
  bool  down;
  public:
  Tmouse_button_state(){set_state(false,0,0);}
  void    __fastcall set_state(bool _dn,int X,int Y){ spot.x = X; spot.y = Y; if(down!= _dn) switch_spot = spot; down = _dn;}
  POINT   __fastcall get_spot       () {return spot;}
  POINT   __fastcall get_switch_spot() {return switch_spot;}
  bool    __fastcall is_down () {return down;}
  bool    __fastcall is_up   () {return !down;}
  int     __fastcall get_dx  (int X) {return X-spot.x;}
  int     __fastcall get_dy  (int Y) {return Y-spot.y;}
  };

  struct Tmouse_state
  {
   Tmouse_button_state mb_left;
   Tmouse_button_state mb_right;
   Tmouse_button_state mb_middle;
  };


   enum  ready_state_flags_t
   {
    rds_rc_error            = 0x00000001
   ,rds_rc_prepare          = 0x00000002
   ,rds_rc_active           = 0x00000004
   ,rds_rc                  = 0x00000007

   ,rds_notnormal_off       = 0x00000010
   ,rds_notnormal_on        = 0x00000020
   ,rds_notnormal           = 0x00000030
   ,rds_notnormal_danger    = 0x00000040
   ,rds_notnormal_alarm     = 0x00000080
   ,rds_notnormal_flashing  = 0x00000100

   ,rds_backnormal_off      = 0x00001000
   ,rds_backnormal_on       = 0x00002000
   ,rds_backnormal          = 0x00003000
   ,rds_backnormal_danger   = 0x00004000
   ,rds_backnormal_alarm    = 0x00008000
   ,rds_backnormal_flashing = 0x00010000

   ,rds_many_off            = 0x00100000
   ,rds_many_on             = 0x00200000
   ,rds_many                = 0x00300000
   ,rds_many_danger         = 0x00400000
   ,rds_many_alarm          = 0x00800000
   ,rds_many_flashing       = 0x01000000

   ,rds_state_flashing      = 0x01010100
   ,rds_name_flashing       = 0x08000000

   ,rds_analog_alarm        = 0x10000000
   ,rds_analog_danger       = 0x20000000
   ,rds_alarm_history       = 0x40000000
   ,rds_stable_ready        = 0x80000000
   ,rds_any_danger          = rds_analog_danger|rds_many_danger|rds_notnormal_danger
   ,rds_any_alarm           = rds_analog_alarm |rds_many_alarm |rds_notnormal_alarm
   };

   struct single_kadr
   {
     mdb_kadr                     kadr;
     kadr_entryes_table           entryes;

     entry_index                  selected;
     entry_index                  position_idx;

     bool                         _position_index_enabled;
     LPVOID                       user_data;
     DWORD                        changes;
     DWORD                        ready_state;
     DWORD                        discrete_changes;
     DWORD                        discrete_not_normal;
     DWORD                        discrete_back_normal;

     typedef kadr_entryes_table::iterator entry_iterator;
     typedef entry_index::iterator  index_iterator;

     single_kadr():_position_index_enabled(false)
     {user_data = NULL;changes = ready_state = discrete_changes = discrete_not_normal = discrete_back_normal = 0;}
     virtual ~single_kadr(){clear_entryes();}
     void          __fastcall clear_entryes(){position_index_enable(false); entryes.clear();entryes.resize(0);clear_selections();}

     kadr_entryes_table::iterator
                  __fastcall  add(DWORD record_id,DWORD number,int x,int y,int cx,int cy,DWORD color = RGB(255,255,255),DWORD bk_color = 0);
     kadr_entryes_table::iterator
                  __fastcall  add(const mdb_kadr_entry & entry);

     bool         __fastcall remove_entry   (kadr_entryes_table::iterator ptr);
     bool         __fastcall remove_entry   (DWORD idx);
     bool         __fastcall remove_entry   (DWORD rec_id,DWORD num);
     int          __fastcall remove_selected();


     DWORD        __fastcall  get_offset(kadr_entryes_table::iterator ptr);
     kadr_entryes_table::iterator
                  __fastcall lower_entry(DWORD rec_id,DWORD num);
     kadr_entryes_table::iterator
                  __fastcall upper_entry(DWORD rec_id,DWORD num);
     DWORD   __fastcall range_entry(DWORD rec_id,kadr_entryes_table::iterator &lo,kadr_entryes_table::iterator & hi);

     bool         __fastcall find_entry(DWORD rec_id,DWORD num,kadr_entryes_table::iterator & ptr);
     DWORD        __fastcall remove_record(DWORD rec_id);

     DWORD        __fastcall sel_count(){return selected.size();}
     bool         __fastcall is_selected(DWORD offset);
     bool         __fastcall is_selected(kadr_entryes_table::iterator ptr);
     DWORD        __fastcall select(DWORD offset);
     DWORD        __fastcall select(kadr_entryes_table::iterator ptr);
     DWORD        __fastcall deselect(DWORD offset);
     DWORD        __fastcall deselect(kadr_entryes_table::iterator ptr);
     void         __fastcall toggle_select   (DWORD offset){ if(is_selected(offset)) deselect(offset); else select(offset);  }
     void         __fastcall clear_selections();
     int          __fastcall selections_restore(bool from_sel_list = false);

     DWORD        __fastcall select_next(bool forward,bool added);
     DWORD        __fastcall get_next   (DWORD offset,bool forward);
     void         __fastcall select_all ();
     void         __fastcall select_in_rect(const POINT & lt,const POINT & br,bool added);


     DWORD        __fastcall position_index_build();
     void         __fastcall position_index_enable(bool enable);
     void         __fastcall position_index_resort();
     void         __fastcall position_normalize   ();
     bool         __fastcall get_by_position      (int x,int y,entry_index::iterator &ptr);
     DWORD        __fastcall get_at_position      (int x,int y);
     DWORD        __fastcall get_range_position   (int x1,int y1,int x2,int y2,entry_index::iterator &lo,entry_index::iterator& hi);


     mdb_kadr_entry* __fastcall get_selected       (DWORD idx);
     DWORD           __fastcall get_selected_offset(DWORD idx){ return (idx<selected.size()) ? (DWORD)selected.at(idx) : (DWORD)-1; }

     mdb_kadr_entry* __fastcall get_first_selected       (){return get_selected(0);}
     mdb_kadr_entry* __fastcall get_last_selected        (){return get_selected(sel_count()-1);}
     DWORD           __fastcall get_first_selected_offset(){  return get_selected_offset(0);   }
     DWORD           __fastcall get_last_selected_offset (){  return get_selected_offset(sel_count()-1); }
     bool            __fastcall is_valid_iterator        (kadr_entryes_table::iterator ptr);
     DWORD           __fastcall make_selected_rec_ids    (records_ids_t &  rids_list);

     bool            __fastcall get_by_offset    (DWORD offset,entry_iterator & ptr);
     mdb_kadr_entry* __fastcall get_by_offset    (DWORD offset);
     DWORD           __fastcall get_entryes_count(){return entryes.size();}
     void            __fastcall resort();

     void            __fastcall get_pos_sizes(entry_index::iterator beg
                                             ,entry_index::iterator end
                                             ,LPMDB_GPOINT left_top,LPMDB_GPOINT right_bottom
                                             ,LPMDB_GSIZE min_sz,LPMDB_GSIZE max_sz);
     void            __fastcall get_pos_sizes(kadr_entryes_table::iterator beg
                                             ,kadr_entryes_table::iterator end
                                             ,LPMDB_GPOINT left_top,LPMDB_GPOINT right_bottom
                                             ,LPMDB_GSIZE min_sz,LPMDB_GSIZE max_sz);

     MDB_GPOINT      __fastcall layout_entryes   (entry_index::iterator  beg,entry_index::iterator  end,int from_left,int from_top ,int step,bool vert,bool equal_size);
     bool            __fastcall is_stable        (){return (ready_state & rds_stable_ready ) ? true : false;}
     bool            __fastcall is_ready_state_flashing (){return (ready_state & rds_state_flashing) ? true : false;}
     bool            __fastcall is_ready_name_flashing  (){return (ready_state & rds_name_flashing ) ? true : false;}
     bool            __fastcall is_ready_flashing       (){return (ready_state & (rds_name_flashing|rds_state_flashing) ) ? true : false;}

   };

inline mdb_kadr_entry *   __fastcall single_kadr::get_by_offset   (DWORD offset)
 {
   entry_iterator ptr;
   if(get_by_offset(offset,ptr)) return &*ptr;
   return NULL;
 }


inline bool          __fastcall single_kadr::is_valid_iterator(kadr_entryes_table::iterator ptr)
{
  return (ptr>= entryes.begin() && ptr < entryes.end() && ptr->kadr_id == kadr.kadr_id) ? true : false;
}

inline DWORD        __fastcall  single_kadr::get_offset(kadr_entryes_table::iterator ptr)
{
 return (is_valid_iterator(ptr)) ? distance(entryes.begin(),ptr) : -1;
}

inline bool operator < (const single_kadr & kdr1, const single_kadr & kdr2)
{
   return kdr1.kadr < kdr2.kadr;
}

inline bool operator == (const single_kadr & kdr1, const single_kadr & kdr2)
{
   return kdr1.kadr == kdr2.kadr;
}

inline bool operator != (const single_kadr & kdr1, const single_kadr & kdr2)
{
   return kdr1.kadr != kdr2.kadr;
}

struct single_kadr_less
{
  bool operator ()(const single_kadr & sk1,const single_kadr & sk2)
  {
   return  sk1<sk2;
  }

  bool operator ()(const single_kadr * sk1,const single_kadr * sk2)
  {
   return  (*this)(*sk1,*sk2);
  }
};

struct single_kadr_equal
{
  bool operator ()(const single_kadr & sk1,const single_kadr & sk2)
  {
   return  sk1 == sk2;
  }

  bool operator ()( const single_kadr * sk1, const single_kadr *  sk2)
  {
   return  (*this)(*sk1,*sk2);
  }
};



#endif

