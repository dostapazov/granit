//---------------------------------------------------------------------------

#pragma warn -8058
#include <vcl.h>
#pragma warn .8058
#include <stdio.h>
#pragma hdrstop

#include "gkmodule_form.h"
#include <gkreporter.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

UnicodeString __fastcall TGKModuleForm::load_rcstring(HINSTANCE mod,DWORD str_id)
{
    wchar_t * text ;
    int len = LoadStringW(mod,str_id,(wchar_t*)&text,0);
    if(len)
       return UnicodeString(text,len);
 return UnicodeString();
}

//---------------------------------------------------------------------------
__fastcall TGKModuleForm::TGKModuleForm(TComponent* Owner,HWND owner_hwnd,GKHANDLE handle)
        : TForm(Owner),owner_window(owner_hwnd),mod_iface(handle,true)
{
 SetPropW(Handle,data_prop_name,this);
 form_gkhandle = 0;
}

const wchar_t * TGKModuleForm::data_prop_name = GKWND_PROP_DATA_W;

TGKModuleForm   * __fastcall TGKModuleForm::get_form(HWND wnd)
{
 TGKModuleForm * ret = NULL;
 if(IsWindow(wnd))
   ret = reinterpret_cast<TGKModuleForm* >(GetPropW(wnd,data_prop_name));
 return ret;
}

void __fastcall TGKModuleForm::setup_form_sizes()
{
   float w = Monitor->WorkareaRect.Width ();
   float h = Monitor->WorkareaRect.Height();
   if(w>800)
      Width  = w*0.7;
   if(h>600)
      Height = h*0.7;
}

void __fastcall TGKModuleForm::AfterConstruction(void)
{
setup_form_sizes();
TForm::AfterConstruction();
if(owner_window) ::SendMessage(owner_window,WGKM_WND_CREATED,WPARAM(mod_iface()),LPARAM(this->Handle));
if(mod_iface()) after_set_gkhandle();
}

void     __fastcall TGKModuleForm::clear_notify_list()
{
 TLockHelper l(locker);
 vector<LPBYTE>::reverse_iterator b = notify_list.rbegin(),e =notify_list.rend();
 while(b<e)
 {
  delete [](*b);
  b++;
 }
 notify_list.clear();
}

void __fastcall TGKModuleForm::BeforeDestruction(void)
{
clear_notify_list();
if(owner_window)
   SendMessage(owner_window,WGKM_WND_DESTROYED,WPARAM(mod_iface()),LPARAM(this->Handle));
destroy_form_gkhandle();
TForm::BeforeDestruction();
}

void __fastcall TGKModuleForm::DoClose(TCloseAction &Action)
{
 if(owner_window && Action == caHide)
   ::PostMessage(owner_window,WGKM_WND_HIDE,WPARAM(mod_iface()),LPARAM(this));
 TForm::DoClose(Action);
}
#pragma warn -8057
DWORD __fastcall TGKModuleForm::OnModuleWindowMessage(UINT msg,LPVOID wnd_handle,GKHANDLE h)
{
 return 0;
}

DWORD    __fastcall TGKModuleForm::get_notify_count()
{
  TLockHelper l(locker);
  return (DWORD) notify_list.size();
}

bool     __fastcall TGKModuleForm::get_notify_code(LPNOTIFY_CODE & nc,DWORD & mask)
{
 bool ret = true;

 DWORD notify_size = get_notify_count();
 if(notify_size)
   {
    TLockHelper l(locker);
    LPBYTE buffer = notify_list.front();
    notify_list.erase(notify_list.begin());
    mask = *((LPDWORD)buffer);
    nc   =  (LPNOTIFY_CODE)(buffer+sizeof(DWORD));
   }
   else
   ret = false;
 return ret;
}

void     __fastcall TGKModuleForm::release_notify_code(LPNOTIFY_CODE nc)
{
  LPBYTE buffer = (LPBYTE)nc;
  if((DWORD)buffer>sizeof(DWORD))
   {
    buffer-=sizeof(DWORD);
    delete [] buffer;
    //LocalFree(buffer);
   }
}

void  __fastcall TGKModuleForm::notify_handler()
{
  LPNOTIFY_CODE nc;
  DWORD         mask;
  int count = 0;
  while(get_notify_code(nc,mask))
  {
     try{
         on_gkhandle_notify(nc->from_module,nc,mask);
         release_notify_code(nc);
        }
        catch(...)
        {
         release_notify_code(nc);
         //clear_notify_list();
        }
     if((++count%8)==0 && GetQueueStatus(QS_ALLINPUT))
        Application->ProcessMessages();
  }
}

void  __fastcall TGKModuleForm::WndProc(Messages::TMessage &Message)
{

   switch(Message.Msg)
   {
   case WMSG_NOTIFY_CODE  :   notify_handler();break;
   case WGKM_WND_CREATED  :
   case WGKM_WND_SHOW     :
   case WGKM_WND_HIDE     :
   case WGKM_WND_CLOSE    :
   case WGKM_WND_DESTROYED:
        Message.Result = OnModuleWindowMessage(Message.Msg,(LPVOID)Message.LParam,(GKHANDLE)Message.WParam);
        break;
    case WGKM_SHOW_PROPWND: show_module_wnd((GKHANDLE)Message.LParam,MODULE_WND_TYPE_CONFIG,(LPVOID)Message.WParam); break;
    case WGKM_SHOW_VIEWWND: show_module_wnd((GKHANDLE)Message.LParam,MODULE_WND_TYPE_VIEWER,(LPVOID)Message.WParam); break;
    default : TForm::WndProc(Message);break;
   }
}

LRESULT __fastcall TGKModuleForm::on_gkhandle_notify (GKHANDLE from,LPNOTIFY_CODE nc,DWORD mask)
{
 if(mask == MNF_COMMON)
 {
  switch(nc->notify_code)
  {
   case MNCODE_MODULE_STATE  : on_module_state(nc->from_module,nc->notify_data[0]);break;
   case MNCODE_MODULE_CONFIG : on_module_config_change(nc->from_module,(LPMODULE_CONFIG_DATA)nc->notify_data);break;
   case MNCODE_ADDED_MODULE  : on_added_module(nc->from_module,(GKHANDLE)nc->notify_data[0],nc->notify_data[1]);break;
   case MNCODE_REMOVED_MODULE: on_removed_module(nc->from_module,(GKHANDLE)nc->notify_data[0],nc->notify_data[1]);break;
   case MNCODE_RENAMED_MODULE: on_renamed_module((GKHANDLE)nc->notify_data[0]);break;
  }
 }

 return GKH_RET_SUCCESS;
}


void   __fastcall TGKModuleForm::show_module_wnd(GKHANDLE handle,DWORD wnd_type,LPVOID param)
{
 if(owner_window)
    ::PostMessage(owner_window,wnd_type == MODULE_WND_TYPE_VIEWER ? WGKM_SHOW_VIEWWND:WGKM_SHOW_PROPWND,(WPARAM)param,LPARAM(handle));
}


bool __fastcall TGKModuleForm::init_form_gkhandle()
{
 if(form_gkhandle == NULL)
  {
    UnicodeString gkh_name = Name;
    gkh_name+=(int)this;
    //safe_strcpy(gkh_name,AnsiString(Name).c_str());
    //safe_strcpy(gkh_name,Name.c_str());
    int i = 0;
    do{
       //Создание уникального имени хендла
       if(!find_gkhandleW(gkh_name.c_str()))
         {
          form_gkhandle = create_gkhandleW(this,(pfn_entry_function)fh_func,0,gkh_name.c_str());
          old_except_filter = set_gkhandle_exception_filterP(form_exception_filter,form_gkhandle);
         }
       else
       {
        gkh_name+=i;
        //sprintf(gkh_name,"%s_%i",Name.c_str(),++i);
       }
      }while(form_gkhandle == NULL);
  }
  return form_gkhandle ? true:false;
}

void __fastcall TGKModuleForm::destroy_form_gkhandle()
{
 if(form_gkhandle)
    {
     reset_notify(mod_iface(),-1);
     close_gkhandle(form_gkhandle);
    }
  form_gkhandle = NULL;

}

//---------------------------------------------------------------------------

void     __fastcall TGKModuleForm::set_notify(GKHANDLE h,DWORD mask)
{

 if(init_form_gkhandle())
    call_gkhandle(h,MCMD_SET_NOTIFY_MASK,LPARAM(form_gkhandle),mask);

}

DWORD   __fastcall TGKModuleForm::on_handle_exception(lpexcept_record lp_er)
{
 LRESULT ret_code;
 call_gkhandle_bynameW(REPORTER_NAME_W,RCM_GKH_EXCEPTION,LPARAM(lp_er),0,&ret_code);
 return GKH_RET_EXCEPTION;
}

DWORD WINAPI          TGKModuleForm::form_exception_filter(lpexcept_record lp_er)
{
 LPVOID data = get_gkhandle_data(lp_er->handle);
 if(data)
 {
  TGKModuleForm * mfrm = reinterpret_cast<TGKModuleForm * >(data);
  if(mfrm)
     return mfrm->on_handle_exception(lp_er);
 }
 return GKH_RET_EXCEPTION;
}

LRESULT  WINAPI TGKModuleForm::fh_func(TGKModuleForm * form,DWORD cmd,LPARAM p1,LPARAM p2)
{

   LRESULT ret = GKH_RET_SUCCESS;
   if(cmd == HANDLE_CMD)
      {
        switch(p1)
        {
         case HANDLE_SUBCMD_RELEASE      :  form->form_gkhandle = NULL; form->locker.FullUnlock(); break;
         case HANDLE_SUBCMD_LOCK         :  ret =  p2 ? form->locker.Lock():form->locker.TryLock();break;
         case HANDLE_SUBCMD_UNLOCK       :  ret = form->locker.Unlock();break;
         case HANDLE_SUBCMD_FULLUNLOCK   :  ret = form->locker.FullUnlock();break;
         case HANDLE_SUBCMD_RESTORELOCK  :  ret = form->locker.RestoreLock((DWORD)p2);break;
        } 
      } 
   else
   ret = form ? form->gkhandle_processing(cmd,p1,p2) : GKH_RET_ERROR;
   return ret;
}

LRESULT __fastcall TGKModuleForm::gkhandle_processing(DWORD cmd,LPARAM p1,LPARAM p2)
{
   LRESULT ret = GKH_RET_ERROR;
   if(cmd == MCMD_NOTIFY_CODE)
     {
      LPNOTIFY_CODE nc    = (LPNOTIFY_CODE)p1;
      if(nc)
      {
         LPBYTE buffer = new BYTE[sizeof(DWORD)+nc->dw_size];
         //LPBYTE buffer =(LPBYTE) LocalAlloc(LMEM_FIXED,sizeof(DWORD)+nc->dw_size);
         if(buffer)
          {
           *((LPDWORD)buffer) = (DWORD)p2;
           memcpy(buffer+sizeof(DWORD),nc,nc->dw_size);
           TLockHelper l(locker);
           notify_list.push_back(buffer);
          }
      }
      if((notify_list.size()%16)== 1)
         PostMessage(Handle,WMSG_NOTIFY_CODE,0,0);
      ret = GKH_RET_SUCCESS;
     }
   return ret;
}


void     __fastcall TGKModuleForm::reset_notify(GKHANDLE h,DWORD mask)
{
 if(form_gkhandle)
    call_gkhandle(h,MCMD_RESET_NOTIFY_MASK,LPARAM(form_gkhandle),mask);
}

void __fastcall TGKModuleForm::after_set_gkhandle()
{
  if(!Caption.Length())
  {
   wchar_t     descr[MAX_PATH];
   descr[0]=0;
   mod_iface.get_module_name(descr,MAX_PATH);
   Caption = descr;
  }
  HICON icon  =  mod_iface.get_icon();
  if(!icon)
      icon     =  mod_iface.get_icon(true);
  Icon->Handle = icon;
}


/*---------------------------------------------------------------------------*/


void    __fastcall gkmod_wnd_iface::add_window_handle(LPVOID wh)
{
 mod_window * pmw = this->window_list;
 while(pmw)
 {
  if(pmw->window_handle == wh)
    return;
    pmw = pmw->next;
 }
 if(wh)
 window_list = new mod_window(wh,window_list);
}

void    __fastcall gkmod_wnd_iface::remove_window_handle(LPVOID wh)
{
 mod_window * pmw = this->window_list,*prev = NULL;
 while(pmw && wh)
 {
  if(pmw->window_handle == wh)
  {
   //удаляем
   if(pmw == window_list)
      window_list = pmw->next;
      else
      {
       prev->next = pmw->next;
      }
    delete pmw;
    return;
  }

  prev = pmw;
  pmw = pmw->next;
 }
}

bool    __fastcall gkmod_wnd_iface::load_window_module()
{

 if(!mwl.is_loaded())
 {
  wchar_t dll_name[MAX_PATH<<1];
  if(int(call(MCMD_GET_WINDOW_MODULE,LPARAM(dll_name),sizeof(dll_name)/sizeof(wchar_t)))>0)
     return mwl.load_module(dll_name);
 }
 return mwl.is_loaded();
}



LPVOID  __fastcall gkmod_wnd_iface::create_window(HWND parent,DWORD type,BOOL modal,LPRECT area,BOOL show)
{
 LPVOID wnd = mwl.create_window(parent,handle,type,modal,area,FALSE);
 if(show)
   show_window(wnd,modal);
 return wnd;
}

int     __fastcall gkmod_wnd_iface::show_window(LPVOID wh,BOOL modal)
{
 if(wh)
   {
    add_window_handle(wh);
    return mwl.show_window(wh,modal);
   }
   return IDCANCEL;
}


bool    __fastcall gkmod_wnd_iface::destroy_window(LPVOID wh,bool unload)
{
 remove_window_handle(wh);
 mwl.close_window(wh);
 mwl.destroy_window(wh);
 if(window_list == NULL)
    {
      if(unload)
        mwl.unload_module();
     return true;
    }
 return false;
}

gkmod_wnd_iface::~gkmod_wnd_iface()
{
  gkmod_wnd_iface::mod_window *pmw;
  while(window_list)
   {
    pmw = window_list->next;
    close_window(window_list->window_handle);
    mwl.destroy_window(window_list->window_handle);
    delete window_list;window_list = pmw;
   }
   mwl.unload_module();
}

void __fastcall TGKModuleForm::before_show(LPMODULE_WINDOW mw)
{
}

TModalResult  __fastcall TGKModuleForm::ShowForm(LPMODULE_WINDOW mw,bool modal)
{
  TModalResult mr = mrOk;
  before_show(mw);
  if(modal)
  {
         mr = ShowModal();
  }
  else
  {
    Visible = true;
    //SetForegroundWindow(Handle);
    //BringWindowToTop(Handle);
    SetFocus();
   }
  return mr;
}





