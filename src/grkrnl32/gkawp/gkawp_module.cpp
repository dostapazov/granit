//   Important note about DLL memory management when your DLL uses the
//   static version of the RunTime Library:
//
//   If your DLL exports any functions that pass String objects (or structs/
//   classes containing nested Strings) as parameter or function results,
//   you will need to add the library MEMMGR.LIB to both the DLL project and
//   any other projects that use the DLL.  You will also need to use MEMMGR.LIB
//   if any other projects which use the DLL will be performing new or delete
//   operations on any non-TObject-derived classes which are exported from the
//   DLL. Adding MEMMGR.LIB to your project will change the DLL and its calling
//   EXE's to use the BORLNDMM.DLL as their memory manager.  In these cases,
//   the file BORLNDMM.DLL should be deployed along with your DLL.
//
//   To avoid using BORLNDMM.DLL, pass string information using "char *" or
//   ShortString parameters.
//
//   If your DLL uses the dynamic version of the RTL, you do not need to
//   explicitly add MEMMGR.LIB as this will be done implicitly for you

#pragma hdrstop
#include "gkawpmodule.hpp"
#include "awp_mod_param.h"
#include "awp_main_form.h"
#include <tregstry.hpp>
#include "tlm_kadr.h"

#pragma argsused

  KeRTL::THeap  TGkAwpModule::mod_heap;
  DWORD         TGkAwpModule::mem_used = 0;


static module_starter<TGkAwpModule> ms(MT_TLM_WORK_PLACE);
LRESULT WINAPI module_main(DWORD cmd,LPARAM p1,LPARAM p2)
{
  if(cmd==GKME_CMD_UNLOAD_MODULE)
     return TRUE;
   return ms(cmd,p1,p2);
}
//------------------------------------------------------------------------------

  void * __fastcall TGkAwpModule::alloc_mem(size_t sz )
  {
    if(!mod_heap.IsValid()) mod_heap.Create(8192);
    return mod_heap.Alloc(sz);
  }

  void   __fastcall TGkAwpModule::free_mem (void * ptr)
  {
    if(mod_heap.IsValid())  mod_heap.Free(ptr);
  }

 void * operator new (size_t sz)
 {
    return TGkAwpModule::alloc_mem(sz);
 }

 void operator delete (void * ptr)
 {
   TGkAwpModule::free_mem(ptr);
 }

 void * operator new [] (size_t sz)
 {
    return TGkAwpModule::alloc_mem(sz);
 }

 void operator delete[] (void * ptr)
 {
   TGkAwpModule::free_mem(ptr);
 }

extern "C" void tss_cleanup_implemented(void)
{
 //stub only for boost::tthread
}



 TGkAwpModule::TGkAwpModule():TGKModule(),mod_params_wnd(NULL),main_form(NULL),int_line(-1)
 {
    is_connected  = false;
    alloc_gkhandle();
    load_paint_dll();
    paint_context = tlmpaint::painter_dll::get_instance()->paint_context_create();
    thread_term_event = NULL;
    rx_queue          = NULL;
    local_ssid = remote_ssid = 0;
 }
//------------------------------------------------------------------------------
void    __fastcall TGkAwpModule::release      (LPVOID arg)
{
  if(is_running()) stop(MODULE_STOP_RELEASE);
  TGKModule::release(arg);
  if(thread_term_event) delete thread_term_event;
  if(rx_queue         ) delete rx_queue;
  clear_all_tables();
  tlmpaint::painter_dll::get_instance()->paint_context_delete(paint_context);
  tlmpaint::painter_dll::get_instance()->unload_library();


}

void    __fastcall TGkAwpModule::load_paint_dll()
{
    if(!tlmpaint::painter_dll::is_loaded())
    {
     std::wstring lib_name(L"tlmobj_painter.dll");
     tlmpaint::painter_dll::get_instance()->load_library(lib_name);
    }

}
HICON   __fastcall TGkAwpModule::get_module_icon    (bool small )
{
       MODULE_INFO mi;
       mi.dw_size = sizeof(mi);
       mi.mi_mask = small ? MI_FL_ICONSM : MI_FL_ICON ;
       get_module_info(&mi);
       return mi.icon;
}

void __fastcall TGkAwpModule::param_page_change(TObject *Sender)
{
 if(mod_params_wnd)
 {
  bool is_ready = mod_params_wnd->PageControl1->ActivePage == mod_params_wnd->ReadySheet ? true:false;
  mod_params_wnd->Timer1->Enabled = is_ready;
  if(main_form)
     main_form->editable = is_ready;
 }
}
//---------------------------------------------------------------------------


void    __fastcall TGkAwpModule::param_window_show  ()
{
  if(!mod_params_wnd)
      {
       mod_params_wnd = new TAwpModuleParams(NULL,this);
       mod_params_wnd->Icon->Handle = get_module_icon();
       mod_params_wnd->OnClose = param_window_close;
       mod_params_wnd->PageControl1->OnChange = param_page_change;
       mod_params_wnd->enum_layouts = this->enum_layouts;
       mod_params_wnd->open_layout  = do_open_layout;
       mod_params_wnd->save_layout  = do_save_layout;
       if(main_form)
           main_form->set_form_screen_position(mod_params_wnd);
       mod_params_wnd->Visible = true;
       Application->ProcessMessages();
       mod_params_wnd->set_parameters(params);
      }
   mod_params_wnd->SetFocus();
}
//------------------------------------------------------------------------------

void   __fastcall TGkAwpModule::kadrs_repaint()
{
  locker l(mut);
  kadrs_t::iterator ptr = kadrs.begin();
  kadrs_t::iterator end = kadrs.end  ();
  while(ptr<end)
  {
    kadr_t * kadr = *ptr;
    if(kadr->user_data) kadr->get_kadr_form()->paint_area->Invalidate();
    ++ptr;
  }
}

void    __fastcall TGkAwpModule::kadr_order(kadr_t * kadr,bool order)
{
  if(order)
  {
     kadr->recall_timer_set(get_kadr_recall_period());
  }
  send_order_command(kadr->kadr.kadr_id,order);
}
//------------------------------------------------------------------------------

void   __fastcall TGkAwpModule::kadrs_order (bool order)
{
  locker l(mut);
  kadrs_t::iterator ptr = kadrs.begin();
  kadrs_t::iterator end = kadrs.end  ();
  while(ptr<end)
  {
    kadr_order(*ptr,order);
    ++ptr;
  }
}

//------------------------------------------------------------------------------
void    __fastcall TGkAwpModule::remove_kadr_order(DWORD kadr_id)
{
        kadrs_t::iterator   kptr;
       locker l(mut);
        if(search_kadr_t(kadr_id,&kptr))
        {
                kadr_t * kadr = *kptr;
                if(kadr->user_data)
                   {
                    TTlmKadrForm * kf = kadr->get_kadr_form();
                    kf->fixed = false;
                    PostMessage(kf->Handle,WM_SYSCOMMAND,SC_CLOSE,0);
                    kadr->user_data = NULL;
                   }

                kadr_order (kadr,false);
                DWORD kadrs_size = kadrs.size();
                kadrs.erase(kptr);
                kadrs_size = kadrs.size();
                kadrs_size = kadrs.size();
                main_kadr.remove_record(kadr->kadr.kadr_id);
                DeleteFile(get_kadr_image_file(kadr,true));
                names.erase(kadr->kadr.name_id);
                names.erase(kadr->kadr.sname_id);
                names.erase(kadr->kadr.image_id);
                delete kadr;
        }
}

void    __fastcall TGkAwpModule::param_change_orders()
{
  if(mod_params_wnd)
     {
       locker l(mut);
       int order_changed_count = 0;
       bool need_order = false;
       int  idx = 0;
       LPOTDM_PROTO_KADR pkadr = mod_params_wnd->get_kadr(idx,need_order);
       while(pkadr)
       {

        kadrs_t::iterator   kptr;
        bool ordered = search_kadr_t(pkadr->kadr.kadr_id,&kptr);
        if(ordered!=need_order)
          {
            ++order_changed_count;
            if(need_order)
               {
                UnicodeString sname,lname;
                get_kadr_names(pkadr,sname,lname);
                set_string(pkadr->kadr.name_id ,lname);
                set_string(pkadr->kadr.sname_id,sname);
                // add order
                kptr = kadrs.insert(kptr,new kadr_t(pkadr->kadr));
                kadr_t * kadr = *kptr;
                kadr->ready_state = 0;
                mdb_kadr_entry entry;
                entry.kadr_id = 0;
                entry.rec_id  = kadr->kadr.kadr_id;
                entry.number  = main_kadr.entryes.size();
                main_kadr.add(entry);
                kadr_order  (kadr,true);
                kadr_changes[kadr->kadr.kadr_id]=-1;
               }
               else
               {
                // remove order
                remove_kadr_order(pkadr->kadr.kadr_id);
               }
          }
       pkadr = mod_params_wnd->get_kadr(++idx,need_order);
       }

       if(order_changed_count)
       {
         main_kadr.selections_restore(true);
         if(main_form)  main_form->layout_ready();
         set_config_changes(CCFL_KADRS);
       }
     }
}
//------------------------------------------------------------------------------
void    __fastcall TGkAwpModule::param_window_close (TObject * sender,TCloseAction & ca)
{
   if(ca!=caNone) ca = caFree;
   if(mod_params_wnd == sender && mod_params_wnd->ModalResult == mrOk)
      {
        //TODO set_parameters;
        TAWParameters new_params;
        bzero(&new_params,sizeof(new_params));
        mod_params_wnd->get_parameters(new_params);
        LPMODULE_CONFIG_DATA mcd;
        BYTE  cdata[ sizeof(*mcd)-sizeof(mcd->data[0])+sizeof(LPVOID)];
        mcd = (LPMODULE_CONFIG_DATA)cdata;
        mcd->dw_sz =  sizeof(cdata);
        mcd->mask  =  -1;
        TAWParameters * pptr = &new_params;
        memcpy(mcd->data,&pptr,sizeof(pptr));
        processing(MCMD_SET_MODULECONFIG_DATA,(LPARAM)mcd,mcd->dw_sz);
        if(mod_params_wnd->orders_changed)
              param_change_orders();

      }
   if(ca == caFree)
   {
    if(main_form) main_form->editable = false;
    mod_params_wnd = NULL;
    if(cc_flags) write_settings() ;
   }

}
//------------------------------------------------------------------------------

 DWORD   __fastcall TGkAwpModule::get_config_data(DWORD mask,LPVOID buf,DWORD bsz)
 {
   return 0;
 }
//------------------------------------------------------------------------------


 void    __fastcall TGkAwpModule::_clear_kadrs    ()
 {
   if(main_form) SendMessage(main_form->Handle,WM_DO_CLOSE_ALL_KADRS,1,1);
   locker l(mut);
   main_kadr.clear_entryes();
   main_kadr.get_file().Close();
   kadrs_t::iterator kptr = kadrs.begin();
   kadrs_t::iterator kend = kadrs.end  ();
   while(kptr<kend)
   {
    kadr_t * kadr =  *kptr;
    delete   kadr;
    ++kptr;
   }
   kadrs.clear() ;
 }

//------------------------------------------------------------------------------
 void    __fastcall TGkAwpModule::clear_all_tables()
 {
   _clear_kadrs();
   locker l(mut);
   records.clear();
   names.clear();
   paint_vars.clear();

 }
//------------------------------------------------------------------------------
void    __fastcall TGkAwpModule::create_folders  ()
{
    locker l(mut);
    if(!params.data_folder.IsEmpty())
       KeRTL::ForceDirs(params.data_folder.c_str());
    if(!params.comtrade_export_folder.IsEmpty())
       KeRTL::ForceDirs(params.comtrade_export_folder.c_str());
}
//------------------------------------------------------------------------------


 bool    __fastcall TGkAwpModule::set_config_data(DWORD mask,LPVOID buf,DWORD bsz)
 {
   bool ret = false;
   LPMODULE_CONFIG_DATA mcd;
   if(mask && buf && bsz > sizeof(*mcd))
   {
    mcd = (LPMODULE_CONFIG_DATA)buf;
    TAWParameters * new_params ;
    memcpy(&new_params,mcd->data,sizeof(new_params));
    if(new_params)
    {
     ret = true;
     locker l(mut);
     cc_flags|= CCFL_PARAMS;



     if(mask & CFGM_MODEM_SOURCE       ) {params.modem_source = new_params->modem_source; clear_all_tables();}
     if(mask & CFGM_DATA_DIR           ) {params.data_folder            = new_params->data_folder            ;}
     if(mask & CFGM_COMTRADE_DIR       ) {params.comtrade_export_folder = new_params->comtrade_export_folder ;}
     if(mask & CFGM_COMTRADE_VIEWER    ) {params.comtrade_viewer_app    = new_params->comtrade_viewer_app    ;}
     if(mask & CFGM_HOTKEY_RECEIPT_ALL )
       {
        params.hot_key_receipt_all    = new_params->hot_key_receipt_all    ;
        if(main_form) main_form->hot_key_register(HOTKEY_ID_RECEIPT_ALL,params.hot_key_receipt_all);
       }
     if(mask & CFGM_HOTKEY_CLEAR_ALL   )
        {
         params.hot_key_clear_all      = new_params->hot_key_clear_all      ;
         if(main_form) main_form->hot_key_register(HOTKEY_ID_CLEAR_ALL  ,params.hot_key_clear_all  );
        }
     if(mask & CFGM_PERIOD_RECALL      ) {params.recall_period          = new_params->recall_period          ;if(params.recall_period && is_connected) kadrs_order(true); }
     if(mask & CFGM_LAYOUT_NAME        ) {params.layout_name            = new_params->layout_name            ;}
     if(mask & CFGM_SOUND_ALARM        ) {params.sound_alarm            = new_params->sound_alarm            ;}
     if(mask & CFGM_SOUND_DANGER       ) {params.sound_danger           = new_params->sound_danger           ;}
     if(mask & CFGM_SOUND_DISPARITY    ) {params.sound_disparity        = new_params->sound_disparity        ;}
     if(mask & CFGM_SOUND_BACK         ) {params.sound_back             = new_params->sound_back             ;}

     if(mask & CFGM_READY_MONITOR      )
      {
       params.ready_monitor          = new_params->ready_monitor;
       if(main_form) {
                      main_form->set_window_monitor(params.ready_monitor,true);
                      main_form->set_form_screen_position(mod_params_wnd);
       }
      }

     if(mask & (CFGM_READY_SPACE|CFGM_READY_EQUAL_SIZE))
     {
       params.ready_space            = new_params->ready_space            ;
       params.ready_equal_size       = new_params->ready_equal_size       ;
       if(main_form) main_form->set_kadr_layout_params(params.ready_space,params.ready_equal_size,true);
     }

     if(mask & CFGM_READY_STABLE_TIMER ) {params.ready_stable_timer     = new_params->ready_stable_timer     ;}

     if(mask & CFGM_READY_COLORS       ) {memcpy(&params.ready_colors     , &new_params-> ready_colors    ,sizeof(params.ready_colors));
                                          if(main_form) main_form->set_ready_colors(params.ready_colors);}

     if(mask & CFGM_READY_FONT         ) {memcpy(&params.ready_font       , &new_params-> ready_font      ,sizeof(params.ready_font))  ;
                                          if(main_form)main_form->set_ready_font(params.ready_font,false,true);}

     if(mask & CFGM_READY_STATE_FONT   ) {memcpy(&params.ready_state_font , &new_params-> ready_state_font,sizeof(params.ready_state_font));
                                          if(main_form)main_form->set_ready_font(params.ready_state_font,true ,true);}

     if(main_form && (mask& (CFGM_READY_SPACE|CFGM_READY_EQUAL_SIZE|CFGM_READY_MONITOR|CFGM_READY_FONT|CFGM_READY_STATE_FONT)))
        main_form->layout_ready();

     if(mask & CFGM_FLASH_PERIOD)        params.flash_period         = new_params->flash_period;
     if(mask & CFGM_READY_SHOW_RC_ERROR)
        {
         params.ready_show_rc_error  = new_params->ready_show_rc_error;
         if(main_form) main_form->Repaint();
        }

    }
   }
   return ret;
 }
//------------------------------------------------------------------------------

 bool    __fastcall TGkAwpModule::compare_config_data(DWORD mask,LPVOID buf,DWORD bsz,LPDWORD _ch_mask,BOOL * _restart)
 {
   bool   ret = true;
   DWORD ch_mask = 0;
   BOOL  restart = FALSE;
   LPMODULE_CONFIG_DATA mcd;
   if(mask && buf && bsz > sizeof(*mcd))
   {
    mcd = (LPMODULE_CONFIG_DATA)buf;
    TAWParameters * new_params ;
    memcpy(&new_params,mcd->data,sizeof(new_params));
    locker l(mut);
    this->cc_flags |= CCFL_PARAMS;

    if(params.modem_source.addr != new_params->modem_source.addr)           {ch_mask |= CFGM_MODEM_SOURCE      ;restart = TRUE;}
    if(params.data_folder != new_params->data_folder)                       {ch_mask |= CFGM_DATA_DIR          ;}
    if(params.comtrade_export_folder != new_params->comtrade_export_folder) {ch_mask |= CFGM_COMTRADE_DIR      ;}
    if(params.comtrade_viewer_app    != new_params->comtrade_viewer_app   ) {ch_mask |= CFGM_COMTRADE_VIEWER   ;}
    if(params.hot_key_receipt_all    != new_params->hot_key_receipt_all   ) {ch_mask |= CFGM_HOTKEY_RECEIPT_ALL;}
    if(params.hot_key_clear_all      != new_params->hot_key_clear_all     ) {ch_mask |= CFGM_HOTKEY_CLEAR_ALL  ;}
    if(params.recall_period          != new_params->recall_period         ) {ch_mask |= CFGM_PERIOD_RECALL     ;}
    if(params.layout_name            != new_params->layout_name           ) {ch_mask |= CFGM_LAYOUT_NAME       ;}
    if(params.sound_alarm            != new_params->sound_alarm           ) {ch_mask |= CFGM_SOUND_ALARM       ;}
    if(params.sound_danger           != new_params->sound_danger          ) {ch_mask |= CFGM_SOUND_DANGER      ;}
    if(params.sound_disparity        != new_params->sound_disparity       ) {ch_mask |= CFGM_SOUND_DISPARITY   ;}
    if(params.sound_back             != new_params->sound_back            ) {ch_mask |= CFGM_SOUND_BACK        ;}

    if(params.ready_monitor          != new_params->ready_monitor         ) {ch_mask |= CFGM_READY_MONITOR     ;}
    if(params.ready_stable_timer     != new_params->ready_stable_timer    ) {ch_mask |= CFGM_READY_STABLE_TIMER;}
    if(params.ready_space            != new_params->ready_space           ) {ch_mask |= CFGM_READY_SPACE       ;}
    if(params.ready_equal_size       != new_params->ready_equal_size      ) {ch_mask |= CFGM_READY_EQUAL_SIZE  ;}

    if(memcmp(&params.ready_colors    ,&new_params->ready_colors    ,sizeof(new_params->ready_colors)))     {ch_mask |= CFGM_READY_COLORS ;}
    if(memcmp(&params.ready_font      ,&new_params->ready_font      ,sizeof(new_params->ready_font  )))     {ch_mask |= CFGM_READY_FONT   ;}
    if(memcmp(&params.ready_state_font,&new_params->ready_state_font,sizeof(new_params->ready_state_font))) {ch_mask |= CFGM_READY_STATE_FONT   ;}
    if(params.flash_period           != new_params->flash_period          ) {ch_mask|= CFGM_FLASH_PERIOD;}
    if(params.ready_show_rc_error    != new_params->ready_show_rc_error   ) {ch_mask|= CFGM_READY_SHOW_RC_ERROR;}
   }
   else
   ret = false;
   if(_ch_mask ) *_ch_mask = ch_mask;
   if(_restart ) *_restart = restart;
   return ret;
 }
//------------------------------------------------------------------------------

void    __fastcall TGkAwpModule::init_ready_colors()
{
     params.ready_colors.dw_size = sizeof(params.ready_colors);
     params.ready_colors.bkgnd           = 0x00C0C0C0;
     params.ready_colors.bkgnd_stable    = 0x00FFFFFF;
     params.ready_colors.tutr            = 0x00F21616;
     params.ready_colors.state_off       = 0x00000000;
     params.ready_colors.state_on        = 0x0015ADB0;
     params.ready_colors.state_danger    = 0x000080FF;
     params.ready_colors.state_alarm     = 0x00342AF5;
     params.ready_colors.name_bad        = 0x00808080;
     params.ready_colors.name_good       = 0x0000FF00;
     params.ready_colors.border          = 0;
     params.ready_colors.border_selected = 0x00FF0000;
}
//------------------------------------------------------------------------------

bool    __fastcall TGkAwpModule::_read_params   ()
{
 wchar_t text[8192];
 DWORD   text_sz = KERTL_ARRAY_COUNT(text);

 bzero(&params,sizeof(params));
 KeRTL::TRegsReader rd(DupKey(this->reg_key));
 if(rd.IsRootOnly())
 {
  params.modem_source.addr = rd.ReadDword(REGDW_MODEM_SOURCE,0,true);
  text[rd.ReadString(REGSTR_DATA_DIR,text,text_sz,true)] = 0;
  params.data_folder = text; if(params.data_folder.IsEmpty()) params.data_folder = L"wpdata";

  text[rd.ReadString(REGSTR_COMTRADE_DIR    ,text,text_sz,true)] = 0;
  params.comtrade_export_folder = text;
  text[rd.ReadString(REGSTR_COMTRADE_VIEWER ,text,text_sz,true)] = 0;
  params.comtrade_viewer_app = text;

  params.hot_key_receipt_all = rd.ReadDword(REGDW_HOTKEY_RECEIPT_ALL,0,true);
  params.hot_key_clear_all   = rd.ReadDword(REGDW_HOTKEY_CLEAR_ALL  ,0,true);

  params.recall_period       = rd.ReadDword(REGDW_PERIOD_RECALL     ,0,true);

  text[rd.ReadString(REGSTR_LAYOUT_NAME     ,text,text_sz,true)] = 0;
  params.layout_name = text;
  text[rd.ReadString(REGSTR_SOUND_ALARM     ,text,text_sz,true)] = 0;
  params.sound_alarm = text;
  text[rd.ReadString(REGSTR_SOUND_DANGER    ,text,text_sz,true)] = 0;
  params.sound_danger = text;
  text[rd.ReadString(REGSTR_SOUND_DISPARITY ,text,text_sz,true)] = 0;
  params.sound_disparity = text;
  text[rd.ReadString(REGSTR_SOUND_BACK      ,text,text_sz,true)] = 0;
  params.sound_back = text;

  params.ready_monitor      = rd.ReadDword(REGDW_READY_MONITOR     ,0,true);
  params.ready_space        = rd.ReadDword(REGDW_READY_SPACE       ,2,true);
  params.ready_equal_size   = rd.ReadDword(REGDW_READY_EQUAL_SIZE  ,0,true);

  params.ready_stable_timer = rd.ReadDword(REGDW_READY_STABLE_TIMER,0,true);
  if(!rd.ReadBytes(REGBIN_READY_COLORS,&params.ready_colors,sizeof(params.ready_colors),true))
      init_ready_colors();
  bzero(&params.ready_font      ,sizeof(params.ready_font));
  bzero(&params.ready_state_font,sizeof(params.ready_state_font));
  if(!rd.ReadBytes(REGBIN_READY_FONT      ,&params.ready_font      ,sizeof(params.ready_font      ),true))
      params.ready_font.lfHeight = -20;
   rd.ReadBytes(REGBIN_READY_STATE_FONT,&params.ready_state_font,sizeof(params.ready_state_font),true);
   params.flash_period        = rd.ReadDword(REGDW_FLASH_PERIOD,500,true);
   params.ready_show_rc_error = (bool)rd.ReadDword(REGDW_READY_SHOW_RC_ERROR,0,true);
   return true;
 }
  return false;
}
//------------------------------------------------------------------------------

void __fastcall    read_kadr_from_storage       (KeRTL::TStorage   & storage,DWORD rec_no,kadr_t * kadr)
{
     storage.get_record(rec_no,&kadr->kadr,sizeof(kadr->kadr));
     kadr->entryes.resize(kadr->kadr.entry_count);
     storage.get_record(rec_no,&*kadr->entryes.begin(),kadr->kadr.entry_count*sizeof(kadr_entryes_table::value_type),1);
}


bool    __fastcall TGkAwpModule::_write_paintvars()
{
 UnicodeString fname(PAINTVARS_FILE_NAME);
 UnicodeString file_name = make_module_file_name(fname,UnicodeString());
 KeRTL::TFileStream fs(file_name.c_str());
 KeRTL::TStorage    storage(&fs);
 AnsiString  sw(fname);
 DWORD ver;
  if(storage.create(1,sw.c_str(),sw.Length(),256))
  {
    locker l(get_mutex());
    paint_variants_t::iterator ptr = paint_vars.begin();
    paint_variants_t::iterator end = paint_vars.end  ();
    while(ptr<end)
    {
     paint_variants_t::value_type & var = *ptr;
     storage.add_record(&var,sizeof(var));
     ++ptr;
    }

  }
  return true;
}

bool    __fastcall TGkAwpModule::_read_paintvars()
{
 UnicodeString fname(PAINTVARS_FILE_NAME);
 UnicodeString file_name = make_module_file_name(fname,UnicodeString());
 KeRTL::TFileStream fs(file_name.c_str(),true);
 KeRTL::TStorage    storage(&fs);
 AnsiString  sw(fname);
 DWORD ver;
  if(storage.open(sw.c_str(),sw.Length(),&ver))
  {
    locker l(get_mutex());
    DWORD rec_no     = 0;
    DWORD rec_count  = storage.get_record_count();
    paint_vars.resize(rec_count);
    paint_variants_t::iterator ptr = paint_vars.begin();
    while(rec_no<rec_count)
    {
     storage.get_record(rec_no++,&*ptr,sizeof(paint_variants_t::value_type));
     ++ptr;
    }

    painter_dll_setup();
  }
  return true;
}

bool    __fastcall TGkAwpModule::_read_kadrs    ()
{
 UnicodeString fname(KADRS_FILE_NAME);
 UnicodeString file_name = make_module_file_name(fname,UnicodeString());
 KeRTL::TFileStream fs(file_name.c_str(),true);
 KeRTL::TStorage    storage(&fs);
 AnsiString  sw(fname);
 DWORD ver;
 if(storage.open(sw.c_str(),sw.Length(),&ver))
 {
    locker l(get_mutex());
    DWORD rec_no = 0;
    DWORD rec_count = storage.get_record_count() ;
    read_kadr_from_storage(storage,rec_no++,&main_kadr);
    kadrs.resize(rec_count-1);
    kadrs_t::iterator ptr = kadrs.begin();
    while(rec_no<rec_count)
    {
     kadr_t * kadr = new kadr_t;
     read_kadr_from_storage(storage,rec_no,kadr);
     *ptr = kadr;
     ++ptr;
     ++rec_no;
    }

 }
 return true;
}
//------------------------------------------------------------------------------

bool    __fastcall TGkAwpModule::_read_names    ()
{
 UnicodeString fname = NAMES_FILE_NAME;
 UnicodeString file_name = make_module_file_name(fname,UnicodeString());
 KeRTL::TFileStream fs(file_name.c_str(),true);
 KeRTL::TStorage    storage(&fs);
 AnsiString  sw(fname);
 DWORD ver = 0;
 if(storage.open(sw.c_str(),sw.Length(),&ver))
 {
  locker l(get_mutex());
  //Tnames & names = viewer_module->get_names();
  wchar_t buf[4096];
  DWORD  str_id;
  for(DWORD rno = 0;rno<storage.get_record_count();rno++)
  {
   storage.get_record(rno,&str_id,sizeof(str_id),0);
   storage.get_record(rno,&buf,sizeof(buf),1);
   names[str_id] = UnicodeString(buf);
  }
 }
 return true;
}
//------------------------------------------------------------------------------

bool    __fastcall TGkAwpModule::_read_records  ()
{
 UnicodeString fname(RECORDS_FILE_NAME);
 UnicodeString file_name = make_module_file_name(fname,UnicodeString());
 KeRTL::TFileStream fs(file_name.c_str(),true);
 KeRTL::TStorage    storage(&fs);
 AnsiString  sw(fname);
 DWORD ver;
 if(storage.open(sw.c_str(),sw.Length(),&ver))
 {
    locker l(mut);
    DWORD   rec_no    = 0;
    DWORD   rec_count = storage.get_record_count();
    records.resize(rec_count);
    records_t::iterator ptr = records.begin();
    while(rec_no < rec_count)
    {
      mdb_record & rec = *ptr;
      storage.get_record(rec_no,&rec,sizeof(rec));
      ++rec_no;
      ++ptr;
    }
 }
 return true;
}
//------------------------------------------------------------------------------

bool    __fastcall TGkAwpModule::read_settings  ()
{
  //Чтение параметров
    locker l(mut) ;
    _read_params   ();
    _read_names    ();
    _read_records  ();
    _read_kadrs    ();
    _read_paintvars();
  return true;
}
//------------------------------------------------------------------------------
bool    __fastcall TGkAwpModule::_write_params ()
{
  KeRTL::TRegsWriter wr(DupKey(reg_key));
  if(wr.IsRootOnly())
  {
    wr.WriteDword (REGDW_MODEM_SOURCE,params.modem_source.addr,true);
    wr.WriteString(REGSTR_DATA_DIR          ,params.data_folder.c_str()            ,true);
    wr.WriteString(REGSTR_COMTRADE_DIR      ,params.comtrade_export_folder.c_str() ,true);
    wr.WriteString(REGSTR_COMTRADE_VIEWER   ,params.comtrade_viewer_app.c_str()    ,true);
    wr.WriteDword (REGDW_HOTKEY_RECEIPT_ALL ,params.hot_key_receipt_all ,true);
    wr.WriteDword (REGDW_HOTKEY_CLEAR_ALL   ,params.hot_key_clear_all ,true);
    wr.WriteDword (REGDW_PERIOD_RECALL      ,params.recall_period ,true);
    wr.WriteString(REGSTR_LAYOUT_NAME       ,params.layout_name.c_str() ,true);
    wr.WriteString(REGSTR_SOUND_ALARM       ,params.sound_alarm.c_str() ,true);
    wr.WriteString(REGSTR_SOUND_DANGER      ,params.sound_danger.c_str() ,true);
    wr.WriteString(REGSTR_SOUND_DISPARITY   ,params.sound_disparity.c_str() ,true);
    wr.WriteString(REGSTR_SOUND_BACK        ,params.sound_back.c_str() ,true);

    wr.WriteDword (REGDW_READY_MONITOR      ,params.ready_monitor ,true);
    wr.WriteDword (REGDW_READY_STABLE_TIMER ,params.ready_stable_timer ,true);
    wr.WriteDword (REGDW_READY_SPACE        ,params.ready_space ,true);
    wr.WriteDword (REGDW_READY_EQUAL_SIZE   ,params.ready_equal_size ,true);
    wr.WriteBytes (REGBIN_READY_COLORS      ,&params.ready_colors,sizeof(params.ready_colors) ,true);
    wr.WriteBytes (REGBIN_READY_FONT        ,&params.ready_font,sizeof(params.ready_font) ,true);
    wr.WriteBytes (REGBIN_READY_STATE_FONT  ,&params.ready_state_font,sizeof(params.ready_state_font) ,true);
    wr.WriteDword (REGDW_FLASH_PERIOD       ,params.flash_period ,true);
    wr.WriteDword (REGDW_READY_SHOW_RC_ERROR,params.ready_show_rc_error ,true);
    return true;
  }
  return false;
}
//------------------------------------------------------------------------------
void    __fastcall write_kadr_to_storage         (KeRTL::TStorage  &  storage,kadr_t * kadr)
{
        kadr->kadr.entry_count = kadr->entryes.size();
 DWORD rec_no = storage.add_record(&kadr->kadr,sizeof(kadr->kadr));
                storage.add_chunk(rec_no,&*kadr->entryes.begin(),kadr->kadr.entry_count*sizeof(kadr_entryes_table::value_type));
}

bool    __fastcall TGkAwpModule::_write_kadrs    ()
{
 UnicodeString fname(KADRS_FILE_NAME);
 UnicodeString file_name = make_module_file_name(fname,UnicodeString());
 KeRTL::TFileStream fs(file_name.c_str());
 KeRTL::TStorage    storage(&fs);
 AnsiString  sw(fname);
 if(storage.create  (1,sw.c_str(),sw.Length(),256))
 {
    locker l(get_mutex());
    write_kadr_to_storage(storage,&main_kadr);
    kadrs_t::iterator ptr = kadrs.begin();
    kadrs_t::iterator end = kadrs.end  ();
    while(ptr<end)
    {
     kadr_t * kadr = *ptr;
       write_kadr_to_storage(storage,kadr);
     ++ptr;
    }
 }
 return true;
}
//------------------------------------------------------------------------------
bool    __fastcall TGkAwpModule::_write_names    ()
{
 UnicodeString fname(NAMES_FILE_NAME);
 UnicodeString file_name = make_module_file_name(fname,UnicodeString());
 KeRTL::TFileStream fs(file_name.c_str());
 KeRTL::TStorage    storage(&fs);
 AnsiString  sw(fname);
 if(storage.create  (1,sw.c_str(),sw.Length(),256))
 {
    locker l(get_mutex());
//    Tnames & names = viewer_module->get_names() ;
     names_t::iterator ptr = names.begin();
     names_t::iterator end = names.end();
     while(ptr!=end)
     {
       UnicodeString & s = ptr->second;
       if(s.Length())
       {
        DWORD key = ptr->first;
        DWORD rno = storage.add_record(&key,sizeof(key));
        storage.add_chunk(rno,s.c_str(),(s.Length()+1)*sizeof(wchar_t));
       }
      ++ptr;
     }
 }

 return true;
}
//------------------------------------------------------------------------------
bool    __fastcall TGkAwpModule::_write_records  ()
{
 UnicodeString fname(RECORDS_FILE_NAME);
 UnicodeString file_name = make_module_file_name(fname,UnicodeString());
 KeRTL::TFileStream fs(file_name.c_str());
 KeRTL::TStorage    storage(&fs);
 AnsiString  sw(fname);
 if(storage.create  (1,sw.c_str(),sw.Length(),256))
 {
    locker l(mut);
    records_t::iterator ptr = records.begin();
    records_t::iterator end = records.end  ();
    DWORD ch = 0;
    while(ptr<end)
    {
     mdb_record & rec = *ptr;
     std::swap(ch,rec.changes_count);
     storage.add_record(&rec,sizeof(rec));
     std::swap(ch,rec.changes_count);
     ++ptr;
    }
 }
 return true;
}
//------------------------------------------------------------------------------

bool    __fastcall TGkAwpModule::write_settings ()
{
 //Чтение параметров
  locker l(mut);
  if((cc_flags & CCFL_PARAMS ) && _write_params ())
       cc_flags &= ~CCFL_PARAMS;
  if((cc_flags & CCFL_KADRS  ) && _write_kadrs  ())
       cc_flags &= ~CCFL_KADRS;
  if((cc_flags & CCFL_NAMES  ) && _write_names  ())
       cc_flags &= ~CCFL_NAMES;
  if((cc_flags & CCFL_RECORDS) && _write_records())
       cc_flags &= ~CCFL_RECORDS;
  if((cc_flags & CCFL_PAINTVARS) && _write_paintvars())
       cc_flags &= ~CCFL_PAINTVARS;
  return true;
}

//------------------------------------------------------------------------------
LRESULT __fastcall TGkAwpModule::processing    (DWORD cmd,LPARAM p1,LPARAM p2)
{
  LRESULT ret;
  switch(cmd)
  {
   case    MDMCM_FA_HANDLE : ret = GKH_RET_SUCCESS;
                            if(MT_IS_MODEM(MCMD_TYPE(cmd)))
                             {
                              if(rx_queue)
                              rx_queue->PutIntoQueue((LPVOID)p1,MPROTO_SIZE((LPMPROTO_HEADER)p1));
                              else
                              handle_recv((LPMPROTO_HEADER)p1);  break;
                             }
                              break;

   case    MODEMCM_SEND    : ret = GKH_RET_SUCCESS;
                             if(rx_queue)
                             rx_queue->PutIntoQueue((LPVOID)p1,MPROTO_SIZE((LPMPROTO_HEADER)p1));
                             else
                             handle_recv((LPMPROTO_HEADER)p1);break;
                             break;
   default                 : ret = TGKModule::processing(cmd,p1,p2); break;
  }
  return ret;
}
//------------------------------------------------------------------------------
void    __fastcall TGkAwpModule::show_main_form()
{
    locker l(mut);
    if(!main_form)
    {
      main_form = new TAwpMainForm(NULL,this);
      main_form->Icon->Handle = this->get_module_icon(true);

      main_form->hot_key_register(HOTKEY_ID_CLEAR_ALL  ,params.hot_key_clear_all  );
      main_form->hot_key_register(HOTKEY_ID_RECEIPT_ALL,params.hot_key_receipt_all);

      main_form->on_get_text  = _get_string;
      main_form->set_ready_font(params.ready_font,false,true);
      main_form->set_ready_font(params.ready_state_font,true,true);
      main_form->set_kadr_layout_params(params.ready_space,params.ready_equal_size,true);
      main_form->set_ready_colors(params.ready_colors);
      main_form->set_kadr(&main_kadr);
//      if(!main_kadr.kadr.kadr_size.cy)
//         main_kadr.kadr.kadr_size<<main_form->paint_area->ClientRect.Size;
      main_form->set_window_monitor(params.ready_monitor,false);

    }
    if(IsDebuggerPresent()) main_form->FormStyle = fsNormal;
    main_form->Visible = true;
    if(!params.layout_name.IsEmpty()) this->do_open_layout(params.layout_name);
    param_page_change(mod_params_wnd);
}

bool    __fastcall TGkAwpModule::open_modemmer()
{
  if(!modemmer.is_valid()  && modemmer.open(MODEMMER_HANDLE_NAME_W))
  {
   modemmer.set_notify(_handle,MNF_MODEM,TRUE);
   modemmer.call(MDMCM_REGISTER_HANDLER,LPARAM(this->_handle),FA_OTD_MEDIUM);
   modemmer.call(MDMCM_REGISTER_HANDLER,LPARAM(this->_handle),FA_OTD_MEDIUM_ALARM_ARCHIVE);
   modemmer.call(MDMCM_REGISTER_HANDLER,LPARAM(this->_handle),FA_SECURITY);
   int_line = modemmer.call(MDMCM_REG_INTERNAL_MLINE,LPARAM(_handle),-1);
   return true;
  }
 return modemmer.is_valid();
}

bool   __fastcall TGkAwpModule::close_modemmer()
{
   if(modemmer.is_valid())
   {
    modemmer.set_notify(_handle,MNF_MODEM,FALSE);
    modemmer.call(MDMCM_UNREGISTER_HANDLER,LPARAM(this->_handle),FA_OTD_MEDIUM);
    modemmer.call(MDMCM_UNREGISTER_HANDLER,LPARAM(this->_handle),FA_OTD_MEDIUM_ALARM_ARCHIVE);
    modemmer.call(MDMCM_UNREGISTER_HANDLER,LPARAM(this->_handle),FA_SECURITY);
    if(int_line >=0 -1) modemmer.call(MDMCM_REG_INTERNAL_MLINE,LPARAM(_handle),int_line);
     int_line  = -1;
    modemmer.close();
   }
   return !modemmer.is_valid();
}

DWORD   __fastcall TGkAwpModule::thread_term_event_create()
{
  if(thread_term_event) delete thread_term_event;
  thread_term_event = new KeRTL::TEvent(false,false);
  return TRUE;
}

DWORD   __fastcall TGkAwpModule::start(DWORD reason,LPARAM as_service)
{
  DWORD ret = GKH_RET_SUCCESS;
  pkt_num   = 0;
  paint_vars_requested = FALSE;
  thread_term_request  = 0;
  disparity_count      = back_count = alarms_count = danger_count = 0;
  history_present      = FALSE;
  is_connected         = FALSE;

  if(as_service)
  {
     report(EVENT_SYSTEM,REPORT_INFORMATION_TYPE,L"Модуль просмотра нет смысла запускать в режиме службы");
  }
  else
  {
      if(tlmpaint::painter_dll::is_loaded() && open_modemmer())
      {
        if(rx_queue) delete rx_queue;
        rx_queue = new KeRTL::TEventSimpleDataQueue;
        thread_term_event_create();
        kadrs_set_bad_ready();
        ret = TGKModule::start(reason,as_service);
        modemmer.set_notify(_handle,MNF_MODEM,TRUE);
        show_main_form       ();
        Application->ProcessMessages();
        on_source_connect(check_source_connected());
        rx_thread    = boost::thread(boost::bind(&TGkAwpModule::rx_thread_proc   ,this));
        timer_thread = boost::thread(boost::bind(&TGkAwpModule::timer_thread_proc,this));
        sound_thread = boost::thread(boost::bind(&TGkAwpModule::sound_thread_proc,this));
      }
  }
  return ret;
}
//------------------------------------------------------------------------------

DWORD   __fastcall TGkAwpModule::stop (DWORD reason)
{
  DWORD ret;
  thread_term_event->SetEvent(true);
  thread_term_request = 1;
  timer_thread.join   ();
  sound_thread.join   ();
  rx_thread.join      ();
  locker l(mut);
  modemmer.set_notify(_handle,MNF_MODEM,FALSE);
  kadrs_order     (false);
  close_modemmer  ();
  close_all_kadrs (true);
  if(main_form)
  {
   delete main_form;
   main_form = NULL;
  }
  Application->ProcessMessages();
  delete rx_queue;
  rx_queue = NULL;
  ret = TGKModule::stop(reason);
  return ret;
}
//------------------------------------------------------------------------------

DWORD   __fastcall TGkAwpModule::get_window_module  (wchar_t * buf,DWORD bsz)
{
   if(buf && bsz) * buf = 0;
      param_window_show();
   return 0;
}
//------------------------------------------------------------------------------
UnicodeString   __fastcall TGkAwpModule::_get_string   (const DWORD str_id) const
{
   UnicodeString str;
   get_string(str_id,str);
   return str;
}

bool   __fastcall TGkAwpModule::get_string   (const DWORD str_id,UnicodeString & str) const
{
    TGkAwpModule* mod = const_cast<TGkAwpModule*>(this);
    if(mod)
    {
        locker l(mod->mut);
        bool ret = mod->names.count(str_id);
        if(ret)
            str = mod->names[DWORD(str_id)];
            else
            str = UnicodeString();
        return ret;
    }
    return false;
}

int   __fastcall TGkAwpModule::set_string   (const DWORD str_id,const UnicodeString   str)
{
  int ret = 0;
   if(str_id != DWORD(-1))
   {
     locker l(mut);
     UnicodeString & dest = names[str_id];
     ret = dest.Compare(str);
     if(ret)
     {
      dest = str;
      cc_flags|=CCFL_NAMES;
     }
   }
  return ret;
}



bool   __fastcall TGkAwpModule::get_record (const DWORD rec_id,      record_t  & rec) const
{
   TGkAwpModule* mod = const_cast<TGkAwpModule*>(this);
   locker l(mod->mut);
   records_t::iterator ptr ;
   bool ret = mod->search_record_t(rec_id,&ptr);
   if(ret) rec = *ptr;
   return ret;
}

bool    __fastcall TGkAwpModule::search_record_t     (const DWORD rec_id ,records_t::iterator * ptr)
{
   records_t::value_type rec;
   rec.rec_id = rec_id;
   locker l(mut);
   return stdlib_utils::binary_search(records,rec,mdb_record_id_less(),ptr);
}


single_kadr  * __fastcall TGkAwpModule::get_kadr   (DWORD kadr_id)
{
   kadrs_t::iterator  kptr;
   if(search_kadr_t(kadr_id,&kptr))
      return *kptr;
   return NULL;
}

bool    __fastcall TGkAwpModule::search_kadr_t        (const DWORD kadr_id,kadrs_t::iterator * kptr)
{
   //Поиск кадра по номеру (идентификатору)
   kadrs_t::iterator  _kptr = NULL;
   kadr_t kdr;
   kdr.kadr.kadr_id = kadr_id;
   locker l(mut);
   _kptr = std::lower_bound(kadrs.begin(),kadrs.end(),&kdr,single_kadr_less());
   if(kptr) *kptr = _kptr;
   return   (_kptr <kadrs.end() && (*_kptr)->kadr.kadr_id == kadr_id) ? true : false;

}

 bool   __fastcall TGkAwpModule::is_kadr_ordered(LPMODEM_ADDR ma,DWORD kadr_id)
 {
   locker l(mut);
   return (params.modem_source.addr == ma->addr &&  search_kadr_t(kadr_id,NULL)) ? true : false;
 }

  TTlmKadrForm *  __fastcall TGkAwpModule::open_kadr_form     (DWORD kadr_id,bool show)
  {
     if(!paint_vars_requested)
     {     request_paint_variants(); }

     kadr_t * sk = (kadr_t*)get_kadr(kadr_id);
     TTlmKadrForm * form = sk->get_kadr_form();
     if(!form)
     {
        form  = new TTlmKadrForm(main_form,this);
        form->Icon->Handle = get_module_icon(true);
        form->set_paint_context(paint_context);
        form->OnActivate       = kadr_activate;
        form->OnDeactivate     = kadr_deactivate;
        sk->selections_restore(true);
        sk->clear_selections();
        sk->user_data = form;
        form->set_kadr(sk);
     }
   if(show)
    {
      form->Visible = true;
      form->SetFocus() ;
    }
    Application->ProcessMessages();
    return form;
  }

void    __fastcall TGkAwpModule::kadr_reset_changes   (kadr_t * kadr)
{
    locker l(mut);
    kadr->changes     = 0;
    kadr->ready_state &=~rds_name_flashing;
    kadr_changes[kadr->kadr.kadr_id]|= MDB_KADR_CHANGE_READY;
}

void   __fastcall TGkAwpModule::kadrs_reset_changes  ()
{
   locker l(mut);
   kadrs_t::iterator ptr = kadrs.begin();
   kadrs_t::iterator end = kadrs.end  ();
   while(ptr<end)
   {
     kadr_reset_changes(*ptr);
     ++ptr;
   }

}


void    __fastcall TGkAwpModule::kadr_selected      (TObject * sender,bool active)
{
  TTlmKadrForm * form = dynamic_cast<TTlmKadrForm *>(sender);
  if(form)
  {
     locker l(mut);
     kadr_entryes_table::iterator lo_entry,hi_entry;
     if(main_kadr.range_entry(form->get_kadr()->kadr.kadr_id,lo_entry,hi_entry))
       {
        while(lo_entry<hi_entry)
         {
          if(active)
          {
            main_kadr.clear_selections();
            main_kadr.select(lo_entry);
            kadr_t * kadr = (kadr_t*)get_kadr(lo_entry->rec_id);
            if(kadr )
            {
             kadr_reset_changes(kadr);
            }
            lo_entry = hi_entry;
          }
          else
           main_kadr.deselect(lo_entry);
           ++lo_entry;
         }
       }
       main_form->refresh_kadr();
  }
}


struct kadr_closer
{
 bool           force;
 kadr_closer(bool _force):force(_force){}
 kadr_closer(const kadr_closer & src):force(src.force){}
 void operator ()(kadr_t * kdr)
 {
   if(kdr && kdr->user_data )
      {
       if(force)
       {
        delete kdr->get_kadr_form();
        kdr->user_data = NULL;
       }
       else
       PostMessage(kdr->get_kadr_form()->Handle,WM_SYSCOMMAND,SC_CLOSE,0);

      }
 }
};


void __fastcall  TGkAwpModule::close_all_kadrs   (bool force)
{
  kadrs_t::iterator ptr = kadrs.begin();
  kadrs_t::iterator end = kadrs.end  ();
  std::for_each(ptr,end,kadr_closer(force));
  Application->ProcessMessages();
}



bool   __fastcall TGkAwpModule::get_opened_kadrs(kadrs_t & _kadrs)
{
  locker l(mut);
  kadrs_t::iterator ptr = this->kadrs.begin();
  kadrs_t::iterator end = this->kadrs.end  ();
  while(ptr<end)
  {
    kadr_t * kdr = *ptr;
    if(kdr->user_data)
       _kadrs.push_back(kdr);
    ++ptr;
  }
 return kadrs.size() ;
}

 LRESULT  __fastcall TGkAwpModule::on_notify_code(LPNOTIFY_CODE nc,DWORD mask)
 {
  if((mask & MNF_MODEM) )
  {
    modem_addr msrc = get_modem_source();
    if(nc->notify_code == MNCODE_MODEM_NUMBER)
    {
     LPMODEM_CHANGE_NUMBER mcn = (LPMODEM_CHANGE_NUMBER)nc->notify_data;
     modem_addr old_addr(0,mcn->old_number,-1,-1);

     if(old_addr.addr  == msrc.addr)
        on_source_connect(false);
    }
    if(nc->notify_code == MNCODE_LINE_STATE)
    {
     LPMODEM_LINE_STATE mls = (LPMODEM_LINE_STATE)nc->notify_data;
     if((mls->state_changes&MODEM_LINE_STATE_CONNECT) && mls->addr.addr == msrc.addr )
         on_source_connect(mls->line_state&MODEM_LINE_STATE_CONNECT);
    }
  }
  TGKModule::on_notify_code(nc,mask);

  return GKH_RET_SUCCESS;
 }

void    __fastcall TGkAwpModule::on_source_connect   (bool connect)
{
  is_connected = connect;
  remote_ssid = local_ssid = 0;
  if(main_form) PostMessage(main_form->Handle,WM_ON_SOURCE_CONNECT,connect,0);
  if(connect)
    {
     if(rx_queue) rx_queue->DropData();
     history_check_present ();
     request_paint_variants();
     Sleep(100);
     kadrs_order   (true);
    }
    else
    {
     history_present      = FALSE;
     paint_vars_requested = FALSE;
     kadrs_set_bad_ready();
    }
}

bool    __fastcall TGkAwpModule::check_source_connected ()
{
  if(modemmer.is_valid())
  {
    MODEM_LINE_STATS mls;
    mls.dw_size = sizeof(mls);
    modem_addr src_addr ;
    {
      locker l(mut);
      src_addr = params.modem_source;
    }
        if(modemmer.call(MDMCM_GET_LINE_STATS,src_addr.addr,(LPARAM)&mls) == GKH_RET_SUCCESS )
          is_connected  = (mls.line_state & MODEM_LINE_STATE_CONNECT) ? true : false;
          //else
          //is_connected = src_addr.modem == 0 ? true: false;

  }
  return is_connected;
}


struct wnd_show_func
{
 bool show;
 wnd_show_func(bool _show):show(_show){}
 wnd_show_func(const wnd_show_func & src):show(src.show){}
 void operator()(kadr_t * kadr)
 {
    if(kadr->user_data)
      kadr->get_kadr_form()->Visible = show;
 }
};

void   __fastcall TGkAwpModule::show_all_windows(bool show)
{
  locker l(mut);
  for_each(kadrs.begin(),kadrs.end(),wnd_show_func(show));
  main_form->Visible = show;
}




