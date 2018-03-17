//---------------------------------------------------------------------------

#ifndef gktlm_viewer_formH
#define gktlm_viewer_formH
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "record_info.h"

//---------------------------------------------------------------------------
#include <storage.hpp>
//---------------------------------------------------------------------------

#include <map>
#include <vector>
#include <algorithm>
#include <tlmpainter.hpp>
#include <otdm_proto.h>
#include <stdlib_utils.hpp>

#define IMAGE_FOLDER L"images\\"

#define TLMKV_WM_RELOAD_IMAGE     (WM_USER+1000)
#define TLMKV_WM_SET_ENTRYES      (WM_USER+1001)
#define TLMKV_WM_UPDATE_KADR      (WM_USER+1002)
#define TLMKV_WM_RESCAN_READY     (WM_USER+1003)
#define TLMKV_WM_MLINE_STATE      (WM_USER+1004)
#define TLMKV_WM_ARCHIVE_PRESENT  (WM_USER+1005)
#define TLMKV_WM_RECORD_CHANGES   (WM_USER+1006)
#define TLMKV_SETUP_PAINTER_DLL   (WM_USER+1007)
#define TLMKV_HANDLE_KADR_ARCHIVE  (WM_USER+1008)


typedef std::map<DWORD,UnicodeString>  Tnames;
//typedef std::map<DWORD,Tentryes_list>  Tkadr_entryes  ;
typedef std::vector<mdb_record_ex>     Trecords;
typedef std::map<DWORD,DWORD>          Trecord_changes;
typedef std::vector<mdb_paint_variant> Tpaint_variants;

struct Thist_times:public OTDM_KADR_ARCHIVE
{
 Thist_times(bool _alarm,__int64  _hist_beg_time,__int64  _hist_end_time,bool _protect = false,char * aname = NULL)
 {
  alarms = (_alarm);
  beg_time = (_hist_beg_time);
  end_time = (_hist_end_time);
  protect  = (_protect);
  if(aname && *aname)
   strncpy(arch_name,aname,sizeof(arch_name));
   else
   memset(arch_name,0,sizeof(arch_name));
 }
 Thist_times(const OTDM_KADR_ARCHIVE& src){*this = src;}
 Thist_times & operator = (const Thist_times& src)
 {
  memcpy(this,&src,sizeof(*this));
  return * this;
 }
};


typedef std::vector<Thist_times> Thist_times_list;

#define KADR_FLAGS_ENTRYES_CHANGED    0x00000001


class TTlmViewerForm;

class  Tmdb_kadr_ex:public mdb_kadr_ex
{
 public:
  TTlmViewerForm * tv;         // Отображатель кадра
  mdb_kadr_entry   kadr_entry; //  Элемент отрисовки готовности
  Tentryes_list    entryes;    //  список  элементов кадра
  Thist_times_list hist_times;
  KeRTL::TFile     file;
  DWORD            timer;
  DWORD            ready_flags;
//  int              discrete_changes;
//  int              alarm_changes;
  DWORD            flags;
  DWORD            ready_timer;

 public:
 Tmdb_kadr_ex():mdb_kadr_ex(),tv(NULL),timer(0),ready_flags(0),flags(0)                           {}
 Tmdb_kadr_ex(DWORD _kadr_id):mdb_kadr_ex(_kadr_id),tv(NULL),timer(0),ready_flags(0),flags(0)     {}
 Tmdb_kadr_ex(const MDB_KADR     & src):mdb_kadr_ex(src),tv(NULL),timer(0),ready_flags(0),flags(0){}
 //Tmdb_kadr_ex(const Tmdb_kadr_ex & src){*this = src;}
 //Tmdb_kadr_ex   & operator = (const Tmdb_kadr_ex & src){memcpy(this,&src,sizeof(*this)); return * this;}
 ~Tmdb_kadr_ex();
 Tentryes_list  & get_entryes()    { return entryes;  }
 KeRTL::TFile   & get_file   ()    { return file;     }
 TTlmViewerForm * get_viewer_form(){ return tv;       }
 void set_viewer_form(TTlmViewerForm * _tv);
 mdb_kadr_entry & get_kadr_entry (){ return kadr_entry;}
 void  __fastcall push_hist_times(bool alarm,__int64 beg_tm,__int64 end_tm)
 {   hist_times.push_back(Thist_times(alarm,beg_tm,end_tm));}

};

inline bool operator == (const Tmdb_kadr_ex & k1, const Tmdb_kadr_ex & k2)
{
  return k1.kadr_id == k2.kadr_id ? true : false;
}

inline bool operator < (const Tmdb_kadr_ex & k1, const Tmdb_kadr_ex & k2)
{
  return k1.kadr_id < k2.kadr_id ? true : false;
}


struct Tmdb_kadr_eq
{
 bool operator ()(const Tmdb_kadr_ex * k1, const Tmdb_kadr_ex * k2)
 {
   if(k1 && k2)
   return k1->kadr_id == k2->kadr_id ? true : false;
   return false;
 }
};

 typedef std::vector<Tmdb_kadr_ex*>      Tkadrs;
 typedef std::vector<Tkadrs::size_type>  Tkadr_indexes;


class Tmdb_kadr_ex_less
{
  Tmdb_kadr_ex * base;
  Tkadrs       * kadrs;
  bool           by_number;
  public:
  Tmdb_kadr_ex_less():base(NULL),kadrs(NULL),by_number(false){}
  Tmdb_kadr_ex_less(Tmdb_kadr_ex * _base,Tkadrs       * _kadrs,bool _by_number):base(_base),kadrs(_kadrs),by_number(_by_number){}
  Tmdb_kadr_ex_less(const Tmdb_kadr_ex_less& src ){*this = src;}
  Tmdb_kadr_ex_less& operator = (const Tmdb_kadr_ex_less& src ){base = src.base; kadrs = src.kadrs;by_number = src.by_number;return * this;}

  bool operator () (const Tmdb_kadr_ex * k1, const Tmdb_kadr_ex * k2)
 {
   if(k1 && k2)
     {
      if(!by_number)
       return k1->kadr_id < k2->kadr_id ? true : false;
       else
       return k1->kadr_entry.number < k2->kadr_entry.number ? true : false;
     }
    return true;
 }

  bool operator ()(const Tkadrs::size_type idx1,const Tkadrs::size_type idx2)
  {
    if(kadrs)
    {
     Tmdb_kadr_ex * k1 = idx1<kadrs->size() ? kadrs->at(idx1) :  base;
     Tmdb_kadr_ex * k2 = idx2<kadrs->size() ? kadrs->at(idx2) :  base;
     if(k1 && k2) return (*this)(k1,k2);
    }
    return true;
  }

};


bool binary_search(Tkadrs & kadrs,Tmdb_kadr_ex * kdr,Tkadrs::iterator * _ptr = NULL)
{
 Tmdb_kadr_ex *& kdr_ptr = kdr;
 Tkadrs::iterator  ptr = std::lower_bound(kadrs.begin(),kadrs.end(),kdr_ptr,Tmdb_kadr_ex_less());
 if(_ptr) *_ptr = ptr;
     return ptr<kadrs.end() && (*ptr)->kadr_id == kdr->kadr_id ? true : false;
}



typedef bool  __fastcall ( __closure * Tgetter_record  )(TObject * sender,DWORD id,bool offset,mdb_record_ex & rec);
typedef bool  __fastcall ( __closure * Tgetter_text    )(TObject * sender,DWORD id,UnicodeString & str );
typedef void  __fastcall ( __closure * Tsend_data      )(TObject * sender,LPMPROTO_HEADER mph );
typedef void  __fastcall ( __closure * Tget_record_text)(TObject * sender,DWORD rec_id,UnicodeString & str);


class TGKTlmViewer;

class TTlmViewerForm : public TForm
{
__published:	// IDE-managed Components
	TPanel *PainterPanel;
	void __fastcall PainterPanelResize(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
private  :	// User declarations
protected:
 TGKTlmViewer            *viewer_module;
 TTlmPainter Fpainter;

 Tmdb_kadr_ex *    kadr;
 HFONT    painter_save_font;
 bool     Ffixed;

  virtual  bool __fastcall get_record       (TObject * sender, DWORD id, bool offset, mdb_record_ex & rec);
           bool __fastcall get_text         (TObject * sender, DWORD id, UnicodeString & str);
  virtual  bool __fastcall get_text         (DWORD id,UnicodeString & name);
 UnicodeString  __fastcall get_text         (DWORD id)  {UnicodeString str;get_text(id,str); return str;}
  virtual  void __fastcall get_record_text  (TObject * sender,DWORD rec_id,UnicodeString & str);
  virtual  DWORD __fastcall on_select_entry (TTlmPainter * p,DWORD entry  );
  virtual  DWORD __fastcall on_mouse_over   (TTlmPainter * p,DWORD entry  );
  virtual  BOOL  __fastcall on_mouse_buttons(TTlmPainter * p,Tmouse_data * md);
  virtual  BOOL  __fastcall on_mouse_move   (TTlmPainter * p,Tmouse_data * md);

           bool   __fastcall is_painter_created();
 virtual   bool   __fastcall create_painter    ();
 virtual   void   __fastcall destroy_painter   ();

           void   __fastcall add_record(mdb_record_ex & rec,DWORD & mask);
           void   __fastcall request_names(DWORD rec_id,bool short_name,bool long_name);


           void   __fastcall file_begin(LPOTDM_PROTO_IMAGE_BEGIN  ibeg);
           void   __fastcall file_begin(KeRTL::TFile & file, LPOTDM_PROTO_IMAGE_BEGIN  ibeg);
           void   __fastcall file_chunk(KeRTL::TFile & file, LPOTDM_PROTO_IMAGE_CHUNK ichunk,bool end);
           void   __fastcall file_chunk( LPOTDM_PROTO_IMAGE_CHUNK ichunk,bool end);
    UnicodeString __fastcall get_image_filename(DWORD kadr_id);
    UnicodeString __fastcall get_image_filename(Tmdb_kadr_ex * kadr);

 /*Обработчики протокла*/
 virtual   void   __fastcall handle_record_names   (LPOTDM_PROTO_HEADER omph,bool short_name);
 virtual   void   __fastcall handle_records_changed(LPOTDM_PROTO_HEADER omph,bool beg,bool end);
 virtual   void   __fastcall handle_records        (LPOTDM_PROTO_HEADER omph,bool beg,bool end);
 virtual   void   __fastcall handle_entryes        (LPOTDM_PROTO_HEADER omph,bool beg,bool end);
 virtual   void   __fastcall handle_image          (LPOTDM_PROTO_HEADER omph,bool beg,bool end);

 virtual   void   __fastcall handle_mproto         (LPMPROTO_HEADER mph);
 virtual   void   __fastcall handle_medproto_error (LPOTDM_PROTO_HEADER omph);
           void   __fastcall handle_mproto_archive (LPMPROTO_HEADER mph);
 virtual   DWORD  __fastcall handle_get_kadr       (Tkadrs & klist,Tnames * names ,LPOTDM_PROTO_HEADER omph,bool end);

           void   __fastcall check_kadr_image      (DWORD kadr_id);
           void   __fastcall request_kadr_image    (DWORD kadr_id);
 virtual   void   __fastcall update_kadr           (DWORD kadr_id,DWORD mask);
           void   __fastcall kadr_order(modem_addr ma,DWORD kadr_id,bool order );
           void   __fastcall request_paint_variants();



 static    DWORD   WINAPI    get_record_callback      (LPVOID ud,TTlmPainter * p,mdb_record_ex * rec);
 static    wstring WINAPI    get_text_callback        (LPVOID ud,TTlmPainter * p,DWORD str_id    );
 static    DWORD   WINAPI    on_select_entry_callback (LPVOID ud,TTlmPainter * p,DWORD entry     );
 static    DWORD   WINAPI    on_mouse_over_callback   (LPVOID ud,TTlmPainter * p,DWORD entry     );
 static    BOOL    WINAPI    on_mouse_buttons_callback(LPVOID ud,TTlmPainter * p,Tmouse_data    * md);
 static    BOOL    WINAPI    on_mouse_move_callback   (LPVOID ud,TTlmPainter * p,Tmouse_data    * md);
 static    BOOL    WINAPI    on_keydown_callback      (LPVOID ud,TTlmPainter * p,Tkeyboard_data * kd);


virtual  void __fastcall  WndProc            (Winapi::Messages::TMessage & msg);
virtual  void __fastcall  set_kadr_image     (DWORD kadr_id);
virtual  void __fastcall  set_kadr_entryes   (DWORD kadr_id);
         void __fastcall  set_kadr_entryes   (Tmdb_kadr_ex * kadr);
         void __fastcall  set_kadr_image     (Tmdb_kadr_ex * kadr);
         void __fastcall  set_kadr_size      (LONG cx, LONG cy);
virtual  void __fastcall  size_for_kadr_size (LONG cx, LONG cy, int & form_width,int & form_height);
         bool __fastcall  get_not_gray_bkgnd ();
         void __fastcall  set_not_gray_bkgnd (bool value);
         void __fastcall  set_fixed(bool val);


__fastcall TTlmViewerForm   (TComponent* Owner);

public:		// User declarations

      	__fastcall TTlmViewerForm   (TComponent* Owner,TGKTlmViewer *_viewer_module);

TGKTlmViewer*        get_viewer_module(){return    viewer_module;}
TTlmPainter * __fastcall get_painter(){return &Fpainter;}
              __property TTlmPainter * painter = {read = get_painter};
              __property bool painter_created  = {read = is_painter_created};
              __property bool not_gray_bkgnd   = {read = get_not_gray_bkgnd , write = set_not_gray_bkgnd};
              __property bool fixed            = {read = Ffixed, write = set_fixed};
virtual void  __fastcall handle_recv(LPMPROTO_HEADER mph);
UnicodeString __fastcall get_kadr_text(Tmdb_kadr_ex & kadr);
UnicodeString __fastcall get_kadr_name(Tmdb_kadr_ex & kadr,bool full_name);
      void    __fastcall open_kadr             (Tmdb_kadr_ex * _kadr);
      void    __fastcall set_kadr_ready_flags  (Tmdb_kadr_ex * _kadr,DWORD set_flags,DWORD reset_flags);
virtual void  __fastcall repaint_changed_records();

       void   __fastcall TTlmViewerForm::copy_record(mdb_record_ex * dest,mdb_record_ex * src,DWORD &  mask);
       void   __fastcall set_record_info(const mdb_record_ex & rec);


public:

static UnicodeString   __fastcall get_state_text     (DWORD state);
static UnicodeString   __fastcall get_state_tu_text  (DWORD state);
static UnicodeString   __fastcall get_state_view_text(DWORD state);
static UnicodeString   __fastcall get_time_text(__int64 tmval,char time_sep,bool msec);

};
//---------------------------------------------------------------------------
extern PACKAGE TTlmViewerForm *TlmViewerForm;
//---------------------------------------------------------------------------


template<typename T>
int __fastcall write_vector(std::vector<T>& v,UnicodeString file_name, char * secret_word )
{
 KeRTL::TFileStream fs(file_name);
 KeRTL::TStorage storage(fs);
 DWORD ver = 0;
 storage.create(ver,secret_word,strlen(secret_word),1000);
 std::vector<T>::iterator ptr = v.begin();
 std::vector<T>::iterator end = v.end  ();
 while(ptr<end)
 {
   T & wrval = *ptr;
   storage.add_record(&wrval,sizeof(T));
   ++ptr;
 }
}

template<typename T>
int __fastcall read_vector(std::vector<T>& v,UnicodeString file_name, char * secret_word )
{
 v.clear();
 KeRTL::TFileStream fs(file_name);
 KeRTL::TStorage storage(fs);
 DWORD ver = 0;
 if(storage.open(secret_word,strlen(secret_word),&ver))
 {
   for(DWORD i = 0;i<storage.get_record_count();i++)
   {
     T rdval;
     storage.get_record(i,&rdval,sizeof(T));
     v.push_back(rdval);
   }
 }
 return v.size();
}

 inline  Tmdb_kadr_ex::~Tmdb_kadr_ex()
 {if(tv) delete tv;}

 inline  void Tmdb_kadr_ex::set_viewer_form(TTlmViewerForm * _tv)
 {
   std::swap(tv,_tv);
   if(_tv)
    delete tv;
 }



#endif
