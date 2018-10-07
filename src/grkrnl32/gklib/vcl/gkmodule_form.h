//---------------------------------------------------------------------------

#ifndef gkmodule_formH
#define gkmodule_formH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <KeRTL.hpp>
#include <gkmodules.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.ToolWin.hpp>
#include <vector>
#include <map>

//---------------------------------------------------------------------------

using namespace std;
using namespace KeRTL;


template <typename VCL_Form>
class gkhandle_wnd_map
{
  protected:
  map<GKHANDLE,VCL_Form*,gkhandle_less> wnd_map;
  TFASTMutex locker;
  public:
  gkhandle_wnd_map(){};
  bool __fastcall lock  (){return locker.Lock();  }
  bool __fastcall unlock(){return locker.Unlock();}
  VCL_Form * operator[] (GKHANDLE handle);
  void __fastcall add_form   (VCL_Form * form);
  void __fastcall remove_form(VCL_Form * form);
  TFASTMutex & get_locker(){return locker};
};



#define WMSG_NOTIFY_CODE (WM_USER+256)


class TGKModuleForm : public TForm
{
__published:	// IDE-managed Components
protected:
HWND                 owner_window; //Окно владелец
GKHANDLE             form_gkhandle;//GK хендл связанный с формой
KeRTL::TFASTMutex    locker;
vector<LPBYTE>       notify_list;
gkhandle_exception_filter     old_except_filter;
static  DWORD WINAPI          form_exception_filter(lpexcept_record lp_er);
static  LRESULT  WINAPI fh_func(TGKModuleForm *,DWORD cmd,LPARAM,LPARAM);
        bool __fastcall init_form_gkhandle();
        void __fastcall destroy_form_gkhandle();
        TGKModuleInterface  mod_iface;//Интерфейс с модулем визуализацию которого осуществляет форма
        void     __fastcall Setmod_handle(GKHANDLE value);
        GKHANDLE __fastcall Getmod_handle();
        void     __fastcall AfterConstruction(void);
        void     __fastcall BeforeDestruction(void);
        void     __fastcall clear_notify_list();
        DWORD    __fastcall get_notify_count();
virtual void     __fastcall notify_handler();
        bool     __fastcall get_notify_code(LPNOTIFY_CODE & nc,DWORD & mask);
        void     __fastcall release_notify_code(LPNOTIFY_CODE);
        void     __fastcall WndProc(Messages::TMessage &Message);
DYNAMIC void     __fastcall DoClose(TCloseAction &Action);
virtual DWORD    __fastcall OnModuleWindowMessage(UINT msg,LPVOID wnd_handle,GKHANDLE h);
virtual LRESULT  __fastcall gkhandle_processing(DWORD cmd,LPARAM p1,LPARAM p2);
virtual LRESULT  __fastcall on_gkhandle_notify (GKHANDLE from,LPNOTIFY_CODE nc,DWORD mask);
virtual DWORD    __fastcall on_handle_exception(lpexcept_record lp_er);
virtual void     __fastcall on_module_state(GKHANDLE mod,DWORD state) = 0;
virtual void     __fastcall on_module_config_change(GKHANDLE mod,LPMODULE_CONFIG_DATA mcd)=0;
virtual void     __fastcall on_added_module  (GKHANDLE owner,GKHANDLE mod,DWORD mod_type){};
virtual void     __fastcall on_removed_module(GKHANDLE owner,GKHANDLE mod,DWORD mod_type){};
virtual void     __fastcall on_renamed_module(GKHANDLE mod){};
virtual void     __fastcall after_set_gkhandle();
virtual void     __fastcall show_module_wnd(GKHANDLE handle,DWORD wnd_type,LPVOID param);
virtual void     __fastcall setup_form_sizes();
        void     __fastcall set_notify(GKHANDLE h,DWORD mask);
        void     __fastcall reset_notify(GKHANDLE h,DWORD mask);
        GKHANDLE __fastcall get_form_notify_gkhandle(){return form_gkhandle;}
virtual __fastcall TGKModuleForm(TComponent* Owner):TForm(Owner){};
public:		// User declarations
        __fastcall TGKModuleForm(TComponent* Owner,HWND owner_hwnd,GKHANDLE handle = 0);
        __property GKHANDLE mod_handle  = { read=Getmod_handle, write=Setmod_handle };
        bool __fastcall lock(DWORD to = INFINITE){ return to ? locker.Lock():locker.TryLock();}
        bool __fastcall unlock(){return locker.Unlock();}
virtual void __fastcall before_show(LPMODULE_WINDOW mw);
 TModalResult __fastcall ShowForm(LPMODULE_WINDOW mw,bool modal);
static const wchar_t   * data_prop_name;
static TGKModuleForm   * __fastcall get_form(HWND wnd);
              static int __fastcall atoi(wchar_t * s){return _wtoi(s);}
              static int __fastcall atof(wchar_t * s){return _wtof(s);}
    static UnicodeString    __fastcall load_rcstring(HINSTANCE mod,DWORD str_id);
};




inline void __fastcall TGKModuleForm::Setmod_handle(GKHANDLE value)
{
  mod_iface.close();
  if(value)
    mod_iface.open(value);
  after_set_gkhandle();
}

inline GKHANDLE __fastcall TGKModuleForm::Getmod_handle()
{
  return mod_iface();
}


struct gkmod_wnd_iface
{
  GKHANDLE handle;
  TGKModuleWindowLoader mwl;
  gkmod_wnd_iface(GKHANDLE  h):handle(h),window_list(NULL){};
  ~gkmod_wnd_iface();
  struct mod_window
  {
   LPVOID window_handle;
   mod_window  * next;
   mod_window (LPVOID wh,mod_window *_next):window_handle(wh),next(_next){}
  };
  mod_window    * window_list;
  LRESULT __fastcall call(DWORD cmd,LPARAM p1,LPARAM p2);
  bool    __fastcall load_window_module();
  void    __fastcall add_window_handle(LPVOID wh);
  void    __fastcall remove_window_handle(LPVOID wh);
  int     __fastcall show_window(LPVOID wh,BOOL modal);
  LPVOID  __fastcall create_window(HWND parent,DWORD type,BOOL modal,LPRECT area,BOOL show);
  bool    __fastcall close_window  (LPVOID wh);
  bool    __fastcall destroy_window(LPVOID wh,bool unload = true);
};

inline   LRESULT __fastcall gkmod_wnd_iface::call(DWORD cmd,LPARAM p1,LPARAM p2)
{
 return call_gkhandle(handle,cmd,p1,p2);
}


inline bool    __fastcall gkmod_wnd_iface::close_window  (LPVOID wh)
{
  return mwl.close_window(wh) ? true:false ;
}

template <typename VCL_Form >
inline  VCL_Form * gkhandle_wnd_map<VCL_Form>::operator[] (GKHANDLE handle)
  {
    VCL_Form * ret = NULL;
    TLockHelper l(locker);

    if(wnd_map.count(handle))
       ret = this->wnd_map[handle];
    return ret;
  }

template <typename VCL_Form >
inline  void __fastcall gkhandle_wnd_map<VCL_Form>::add_form   (VCL_Form* form)
  {
    TLockHelper l(locker);
    wnd_map[form->mod_handle] = form;
  }

template <typename VCL_Form>
inline  void __fastcall gkhandle_wnd_map<VCL_Form>::remove_form(VCL_Form * form)
  {
    TLockHelper l(locker);
    if(wnd_map.count(form->mod_handle))
       wnd_map.erase(wnd_map.find(form->mod_handle));
  }

template <typename vcl_form>
class wnd_module_starter
{
 gkhandle_wnd_map<vcl_form>  wnd_map;
 DWORD module_type;
 MODULE_WINDOW mw;
 void       __fastcall  set_module_window(LPMODULE_WINDOW _mw){memcpy(&mw,_mw,sizeof(mw));}
 LRESULT    __fastcall  get_module_type(){return (LRESULT)module_type;}
 vcl_form * __fastcall  create_window   (LPMODULE_WINDOW mw)
 {
  set_module_window(mw);
  vcl_form * ret = wnd_map[mw->mod_handle];
  if(!ret)
      {
       ret = new vcl_form(Application,mw->parent_window,mw->mod_handle);
       wnd_map.add_form(ret);
      }
  return ret;
 }
 public:
 wnd_module_starter(DWORD _mt):module_type (_mt){}

 LRESULT operator()(DWORD cmd,LPARAM p1,LPARAM p2)
 {
  LRESULT ret = 0;
  vcl_form * mf = NULL;

  switch(cmd)
  {
   case GKME_CMD_GET_MODULETYPE : ret  = get_module_type();break;
   case MWCM_CREATE_WINDOW:
   {
    LPMODULE_WINDOW mw = (LPMODULE_WINDOW)p1;
    mf = create_window(mw);
    if(mf)
    {
     ret  =(LRESULT)mf->Handle;
     if((BOOL)p2)
        (*this)(MWCM_SHOW_WINDOW,ret,mw->modeless ? false:true);
    }
   }
   break;
   case MWCM_DESTROY_WINDOW:
    mf = dynamic_cast<vcl_form*>(TGKModuleForm::get_form((HWND)p1));
   //if(!mf) mf = reinterpret_cast<vcl_form*>((LPVOID)p1);
   if(mf /*&& wnd_map.count(mf)*/)
     {
      wnd_map.remove_form(mf);
      try{
         delete mf;
         }
         catch(...){}
     }
   break;
   case MWCM_SHOW_WINDOW:
   mf = dynamic_cast<vcl_form*>(TGKModuleForm::get_form((HWND)p1));
   if(mf)
     {
      mf->ShowForm(&mw,p2 ? true : false);
      if(IsIconic(mf->Handle))
          ShowWindow(mf->Handle,SW_RESTORE);

     }
   break;
   case MWCM_CLOSE_WINDOW:
   mf = dynamic_cast<vcl_form*>(TGKModuleForm::get_form((HWND)p1));
   if(mf)
   {
    mf->ModalResult = mrCancel;
    mf->Close();
   }
   break;
   case MWCM_SETFOCUS_WINDOW:
   mf = dynamic_cast<vcl_form*>(TGKModuleForm::get_form((HWND)p1));
   if(mf)
    mf->SetFocus();
    break;
  }
  return ret;
 }
};


#endif
