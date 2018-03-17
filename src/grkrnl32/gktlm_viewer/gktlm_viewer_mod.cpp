#pragma hdrstop

#include "gktlm_viewer_mod.hpp"
#include "tlm_viewer_main_form.h"
#include "history_data.h"
#include <tregstry.hpp>
#include "tlm_viewer_param.h"

#include <alloc.h>
#include <sql.h>


LONG TGKTlmViewer::total_alloc = 0;

using namespace stdlib_utils;


DWORD   __fastcall TGKTlmViewer::get_mem_used()
 {
  TMemoryManagerState mms;
  GetMemoryManagerState(mms);
  DWORD ret = 0;
  for(int i = 0;i<mms.SmallBlockTypeStates.Size();i++)
  {
   ret += mms.SmallBlockTypeStates[i].InternalBlockSize;
  }
  return  ret + mms.SmallBlockTypeStates.Size()+mms.TotalAllocatedLargeBlockSize+mms.TotalAllocatedMediumBlockSize;
  //return total_alloc;
 }


 TGKTlmViewer::TGKTlmViewer()
 {
   lock_param = GKHB_AUTO_LOCK_OFF;
   //alloc_gkhandle(TLMVIEWER_MOD_NAME);
   alloc_gkhandle();


   discrete_disparity_count = 0;
   discrete_back_count      = 0;
   params_wnd  = NULL;
   int_line        = 0;
   get_main_form   ();
 }




 TGKTlmViewer::~TGKTlmViewer()
 {
 }


 TTlmViewerMainForm * __fastcall TGKTlmViewer::get_main_form()
 {
   if(!main_form)
   {
       main_form                = new TTlmViewerMainForm(NULL,this);
       MODULE_INFO mi;
       mi.dw_size = sizeof(mi);
       mi.mi_mask = MI_FL_ICON ;
       get_module_info(&mi);
       main_form->Icon->Handle  = mi.icon;
       main_form->set_ready_font(&parameters.ready_font);
   }
   return main_form;
 }


 void __fastcall TGKTlmViewer::release_tables()
 {
   names.clear();
   clear_kadrs(kadrs_ordered );
   clear_kadrs(kadrs_avail   );
   records.clear();
   changed_records.clear();
 }

void __fastcall TGKTlmViewer::clear_kadrs(Tkadrs & kadrs)
{
 Tkadrs::iterator ptr  = kadrs.begin(),end = kadrs.end();
 while(ptr<end){ delete *ptr;++ptr;}
 kadrs.clear();
}

 void    __fastcall TGKTlmViewer::release(LPVOID p)
 {
   data_queue.DropData();
   if(main_form)
    delete main_form;
    main_form = NULL;
   release_tables();
 }



 DWORD   __fastcall TGKTlmViewer::get_window_module  (wchar_t * buf,DWORD bsz)
 {
   DWORD ret = 0;
   if(buf && bsz)*buf  = 0;
   show_parameters();
   return  ret;
 }

 DWORD   __fastcall TGKTlmViewer::get_config_data(DWORD mask,LPVOID buf,DWORD bsz)
 {
  DWORD  ret = GKH_RET_SUCCESS;
  return ret;
 }

 bool    __fastcall TGKTlmViewer::read_settings  ()
 {
   return read_config(-1,KeRTL::DupKey( reg_key ),NULL);
 }

 bool  __fastcall TGKTlmViewer::read_config      (DWORD mask,HKEY reg_key,wchar_t * config_folder)
 {
   if(mask&CCFL_PARAMS)
	  read_params(reg_key);
   if(mask&CCFL_NAMES)
	  read_names();
   if(mask&CCFL_RECORDS)
	  read_records();
   if(mask&CCFL_KADRS)
	  {
           read_kadrs();
          }
   return true;
 }


 bool    __fastcall TGKTlmViewer::write_settings ()
 {
    ::SendMessage((HWND)main_form->Handle,TLMMF_WRITE_CONFIG,cc_flags,(LPARAM)KeRTL::DupKey( reg_key ));
    //bool ret = main_form->write_config(cc_flags,KeRTL::DupKey( reg_key ),NULL);
    cc_flags = 0;
    return true;
 }


 bool    __fastcall TGKTlmViewer::set_config_data(DWORD mask,LPVOID buf,DWORD bsz)
 {
  bool ret = true;
  return ret;
 }

 bool    __fastcall TGKTlmViewer::compare_config_data(DWORD mask,LPVOID buf,DWORD bsz,LPDWORD ch_mask,BOOL * restart)
 {
  bool ret = true;
  return ret;
 }

 BOOL    __fastcall TGKTlmViewer::can_start(DWORD reason,LPARAM p2)
 {
     return TGKModule::can_start(reason,p2);
 }

 BOOL    __fastcall TGKTlmViewer::can_stop (DWORD reason)
 {
     return TGKModule::can_stop(reason);
 }

 DWORD   __fastcall TGKTlmViewer::start(DWORD reason,LPARAM p2)
 {
   DWORD  ret = GKH_RET_ERROR;
   modemmer.open(MODEMMER_HANDLE_NAME_W);
   if(!int_line)
      int_line = modemmer.call(MDMCM_REG_INTERNAL_MLINE,LPARAM(_handle),-1);
   if(int_line)
   {

    Start();
    pkt_num  = 0;
    data_queue.DropData();
    SetFlags(KERTL_THREAD_ENABLERESTART,true);
    ret = TGKModule::start(reason,p2);
    modemmer.call(MDMCM_REGISTER_HANDLER,LPARAM(this->_handle),FA_OTD_MEDIUM);
    modemmer.call(MDMCM_REGISTER_HANDLER,LPARAM(this->_handle),FA_OTD_MEDIUM_ALARM_ARCHIVE);
    modemmer.call(MDMCM_REGISTER_HANDLER,LPARAM(this->_handle),FA_SECURITY);
    modemmer.set_notify(_handle,MNF_MODEM,TRUE);
    discrete_disparity_count = 0;
    discrete_back_count      = 0;
    if(parameters.data_dir.length())
       ForceDirs((wchar_t*)parameters.data_dir.c_str());
    parameters.modem_source.addr = parameters.external_source ? -1 : 0;
    TTlmViewerMainForm * mf = get_main_form();
    if(mf)
      {
       mf->hot_key_register(HOTKEY_ID_CLEAR_ALL  ,parameters.hot_key_clear_all);
       mf->hot_key_register(HOTKEY_ID_RECEIPT_ALL,parameters.hot_key_receipt_all);
       mf->get_painter()->painter_dll(PMCMD_PAINT_SET_READY_COLORS,(LPARAM)&parameters.ready_colors,0);
       mf->set_ready_font(&parameters.ready_font);
       mf->start();
      }
   }
   return ret;
 }

 DWORD   __fastcall TGKTlmViewer::stop (DWORD reason)
 {
    TTlmViewerMainForm * mf = dynamic_cast<TTlmViewerMainForm*>(main_form);
    if(mf)
       mf->stop();
   TerminateAndWait(3000,true);
   modemmer.set_notify(_handle,MNF_MODEM,FALSE);
   modemmer.call(MDMCM_UNREGISTER_HANDLER,LPARAM(this->_handle),FA_OTD_MEDIUM);
   modemmer.call(MDMCM_UNREGISTER_HANDLER,LPARAM(this->_handle),FA_OTD_MEDIUM_ALARM_ARCHIVE);
   modemmer.call(MDMCM_UNREGISTER_HANDLER,LPARAM(this->_handle),FA_SECURITY);
   //MODULE_STOP_PROGRAM_SHUTDOWN
   modemmer.call(MDMCM_REG_INTERNAL_MLINE,LPARAM(_handle),int_line);
   int_line = 0;
   modemmer.close();
   if(cc_flags)
      write_settings();
   DWORD  ret =  TGKModule::stop(reason);
   return ret;
 }

 LRESULT __fastcall TGKTlmViewer::processing_input_proto(LPMPROTO_HEADER mph)
 {
        bool chk_ok =  mproto_check(mph);
        if(chk_ok )
        {
            if( mph->fa == FA_OTD_MEDIUM || mph->fa == FA_OTD_MEDIUM_ALARM_ARCHIVE || mph->fa == FA_SECURITY)
                data_queue.PutIntoQueue(mph,mproto_size(mph));
        }
        else
        {
          // поместить в отчеты
        }
    return GKH_RET_SUCCESS;
 }

 LRESULT __fastcall TGKTlmViewer::processing    (DWORD cmd,LPARAM p1,LPARAM p2)
 {
  LRESULT ret = GKH_RET_ERROR;
  switch(cmd)
  {
   case    MODEMCM_SEND    :ret = processing_input_proto((LPMPROTO_HEADER)p1); break;
   case    MDMCM_FA_HANDLE :
           if(MT_IS_MODEM(MCMD_TYPE(cmd)))
              ret = processing_input_proto((LPMPROTO_HEADER)p1);
           break;

   default : ret = TGKModule::processing(cmd,p1,p2);break;
  }
  return ret;
 }


DWORD  __fastcall TGKTlmViewer::send_data( LPMPROTO_HEADER mph)
{
  mph->addr_from.sta     = 0;
  mph->addr_from.modem   = 0;
  mph->addr_from.line    = int_line;
  mph->addr_from.reserv1 = -1;
  mph->internal          = MP_INTERNAL_DEFAULT;
  mph->pkt_num           = InterlockedIncrement(&pkt_num);
  mproto_protect(mph);
  DWORD ret = modemmer.call(MDMCM_SEND,(LPARAM)mph,MPROTO_SIZE(mph));
  return ret;
}

DWORD       __fastcall TGKTlmViewer::send_command(modem_addr ma,DWORD fa,bool begin,bool finish,DWORD cmd,DWORD err,LPVOID data,DWORD data_size )
{
 DWORD ret = 0;
 BYTE   buffer[16384];
 LPMPROTO_HEADER     mph;
 LPOTDM_PROTO_HEADER omph;
 DWORD  need_size = sizeof(MPROTO_HEADER)-sizeof(mph->data[0])+sizeof(*omph)-sizeof(omph->data[0])+data_size;
 if(need_size<sizeof(buffer))
   mph = (LPMPROTO_HEADER) buffer;
  else
   mph = (LPMPROTO_HEADER) new BYTE[need_size];
 if(mph)
 {
 ZeroMemory(mph,need_size);
 mph->addr_to.addr = ma.addr;
 mph->data_size = need_size+(sizeof(mph->data)-sizeof(*mph));
 mph->fa = fa;
 omph =(LPOTDM_PROTO_HEADER) mph->data;
 omph->command    = cmd&(OTDM_COMMAND_MASK|OTDM_ORDER);
 //omph->command   |= OTDM_RESPOND;
 if(begin)
   omph->command  |= OTDM_BEGINDATA;
 if(finish)
  omph->command  |= OTDM_ENDDATA;
  if(err)
  omph->command  |= OTDM_ERROR;
  omph->error     = err;
  omph->data_size = data_size;
  memcpy(omph->data,data,data_size);
  ret = send_data(mph);
  if( buffer!=(LPBYTE)mph)
      delete  [](LPBYTE)mph;
  }
  return ret;
}


 bool    __fastcall TGKTlmViewer::BeforeExecute  ()
 {
   bool ret = TGKThread::BeforeExecute();
   term_event.SetEvent(false);
   return ret;
 }

 int     __fastcall TGKTlmViewer::Execute        ()
 {
   TSynchroSet ss;
   ss+=term_event;
   ss+=data_queue.GetEvent();
   DWORD ptr_size = 16384;
   auto_ptr<BYTE> ptr(new BYTE [ptr_size] );
   do
   {
     if(1== ss.Wait(1000))
      {
        do{
           int need_sz = 0;
           if(!data_queue.GetFromQueue(ptr.get(),ptr_size,&need_sz))
              {
                ptr_size = data_queue.TopDataSize();
                ptr_size<<=1;
                delete [] ptr.release();
                ptr.reset(new BYTE[ptr_size]);
                data_queue.GetFromQueue(ptr.get(),ptr_size,&need_sz);
              }
              main_form->handle_recv((LPMPROTO_HEADER)ptr.get());
          }while(!CheckTerminateRequest() && data_queue.QueueCount());

      }
   }while(!CheckTerminateRequest());
  return 0;
 }

 void    __fastcall TGKTlmViewer::BeforeTerminate()
 {
     term_event.SetEvent(false);
     TGKThread::BeforeTerminate();
 }

 LRESULT  __fastcall TGKTlmViewer::on_notify_code(LPNOTIFY_CODE nc,DWORD mask)
 {
  if((mask & MNF_MODEM) )
  {
    if(nc->notify_code == MNCODE_MODEM_NUMBER)
    {
     LPMODEM_CHANGE_NUMBER mcn = (LPMODEM_CHANGE_NUMBER)nc->notify_data;
     modem_addr ma(0,mcn->old_number,-1,-1);

     if(ma.addr  == get_modem_source().addr)
       {
        TTlmViewerMainForm * mf = dynamic_cast<TTlmViewerMainForm*>(main_form);
        set_external_source (parameters.external_source);
        if(mf)  mf->reconnect();
       }
    }
    if(nc->notify_code == MNCODE_LINE_STATE)
    {
     on_modem_line_state((LPMODEM_LINE_STATE)nc->notify_data);
    }
  }

  return GKH_RET_SUCCESS;
 }

 void     __fastcall TGKTlmViewer::on_modem_line_state(LPMODEM_LINE_STATE mls)
 {
  if(mls->state_changes && main_form )
  {
    ::PostMessage(main_form->Handle,TLMKV_WM_MLINE_STATE,mls->addr.addr,mls->line_state);
  }
 }

bool  __fastcall TGKTlmViewer::__get_record        (DWORD id ,bool offset, mdb_record_ex & rec)
{
  bool ret = false;
  rec.rec_id = id;
  Trecords::iterator ptr;
  locker l( mut);
  if(offset)
  {
   if(id<records.size())
   {
    ptr = records.begin();
    std::advance(ptr,id);
    ret = true;
   }
  }
  else
   ret = binary_search(records,rec,&ptr);
  if(ret)
     rec = *ptr;
  return ret;
}

   void __fastcall TGKTlmViewer::set_text(DWORD str_id,UnicodeString s)
   {
     locker l(mut);
     Tnames::iterator nptr = names.find(str_id);
     if(nptr == names.end() || nptr->second.Compare(s))
       {
        if(nptr == names.end())
           names[str_id] = s;
           else
           nptr->second  = s;
           config_change(CCFL_NAMES);
       }
   }

   bool __fastcall TGKTlmViewer::__get_text(DWORD id,UnicodeString & name)
   {
     if(id!=DWORD(-1))
     {
      Tnames::iterator n_ptr;
      locker l(mut);
      n_ptr = names.find(id);
      if(n_ptr!=names.end())
        {
          name = n_ptr->second;
          return true;
        }
     }
     return false;

   }

  void __fastcall  TGKTlmViewer::get_record_text  (DWORD rec_id,UnicodeString & str)
  {
      mdb_record_ex  rec;
    locker l(mut);
    if(__get_record(rec_id,false,rec))
    {
       if(!__get_text(rec.name,str) && !__get_text(rec.sname,str))
         str.printf(L"record №%u",rec_id);
    }
    else
    str.printf(L"unknown record №%u",rec_id);
  }

 bool   __fastcall TGKTlmViewer::find_record       (DWORD rec_id,Trecords::iterator & ptr)
 {
    mdb_record_ex rec(rec_id);
    return binary_search(records,rec,&ptr);
 }

UnicodeString __fastcall TGKTlmViewer::get_data_dir()
{
   UnicodeString str = parameters.data_dir.c_str();
   int len = str.Length();
   if(!str.IsPathDelimiter(len)) str+=L"\\";
      return str;
}


void  __fastcall     TGKTlmViewer::read_params (HKEY reg)
{
 parameters.read(reg_key);
}

void  __fastcall     TGKTlmViewer::read_kadrs  ()
{
 UnicodeString fname = KADRS_FILE_NAME;
 UnicodeString kadrs_file = get_data_dir();
 kadrs_file    +=fname;
 KeRTL::TFileStream fs(kadrs_file.c_str(),true);
 KeRTL::TStorage    storage(&fs);
 AnsiString  sw(fname);
 DWORD ver = 0;
 if(storage.open(sw.c_str(),sw.Length(),&ver))
 {

  locker l(get_mutex());

  for(DWORD rno = 0;rno< storage.get_record_count();rno++)
  {
    MDB_KADR mk;
    bzero(&mk,sizeof(mk));
    if(sizeof(mk) >= storage.get_record(rno,&mk,sizeof(mk),0))
    {
      Tmdb_kadr_ex * kadr = new Tmdb_kadr_ex(mk);
      storage.get_record(rno,&kadr->get_kadr_entry(),sizeof(mdb_kadr_entry),1);
      kadr->kadr_entry.kadr_id = 0;
      kadr->kadr_diag = -1;
      DWORD ent_count = 0;
      storage.get_record(rno,&ent_count,sizeof(ent_count),2);
      if(ent_count)
         {
           Tentryes_list & ent = kadr->get_entryes();
           ent.resize(ent_count);
           storage.get_record(rno,&*ent.begin(),ent_count*sizeof(Tentryes_list::value_type),3);
         }
     kadrs_ordered.push_back(kadr);
     mdb_kadr_entry & ke = kadr->get_kadr_entry();
     if(main_form)
        main_form->get_painter()->add_entry(ke);
    }
  }
  reset_diag();
 }
}

void  __fastcall     TGKTlmViewer::read_names  ()
{
 UnicodeString fname = NAMES_FILE_NAME;
 UnicodeString file_name = get_data_dir();
 file_name    +=fname;
 KeRTL::TFileStream fs(file_name.c_str());
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
}

void  __fastcall     TGKTlmViewer::read_records()
{
 UnicodeString fname = RECORDS_FILE_NAME;
 UnicodeString file_name = get_data_dir();
 file_name    +=fname;
 KeRTL::TFileStream fs(file_name.c_str());
 KeRTL::TStorage    storage(&fs);
 AnsiString  sw(fname);
 DWORD ver = 0;
 if(storage.open(sw.c_str(),sw.Length(),&ver))
 {
  locker l(get_mutex());
  //Trecords & records = viewer_module->get_records() ;
  mdb_record_ex rec;
  for(DWORD rno = 0;rno<storage.get_record_count();rno++)
  {
    storage.get_record(rno,&rec,sizeof(rec));
    rec.clean();
    rec.reset_changes() ;
    records.push_back(rec);
  }
 }
}

void  __fastcall TGKTlmViewer::reset_diag()
{
  //Сброс диагностики
  locker l(get_mutex());
  //Trecords & records  = viewer_module->get_records() ;
  Trecords::iterator rptr = records.begin();
  Trecords::iterator rend = records.end  ();
  while(rptr<rend)
  {

   rptr->diag = MDBR_DIAG_BAD;
   rptr->state &= ~(MDBR_STATE_TUTR_ACTIVE|MDBR_STATE_TUTR_PREPARE|MDBR_STATE_TUTR_ERROR);
   ++rptr;
  }

  //Tkadrs & kadrs_ordered = viewer_module->get_kadrs_ordered();
  Tkadrs::iterator kptr = kadrs_ordered.begin();
  Tkadrs::iterator kend  = kadrs_ordered.end();
  while(kptr<kend)
  {
   set_kadr_diag(*kptr,MDBR_DIAG_BAD);
   TTlmViewerMainForm * mf = dynamic_cast<TTlmViewerMainForm*>(main_form);
   if(mf)
     mf->scan_ready   (*kptr);
   ++kptr;
  }

  main_form->Repaint();
}

void    __fastcall TGKTlmViewer::set_kadr_diag         (Tmdb_kadr_ex * kadr,DWORD diag)
{
 if(kadr)
 {
  if(kadr->kadr_diag != (WORD)diag)
  {
   kadr->kadr_diag   = diag;
   kadr->ready_timer = 0;
   kadr->ready_flags &=~ RDY_FLAG_STABLE;
  }
  if(kadr->tv)
  {
   ::PostMessage((HWND)kadr->tv->Handle,TLMKV_WM_UPDATE_KADR,MDB_KADR_FIELD_DIAG,kadr->kadr_id);
   ::PostMessage((HWND)Handle,TLMKV_WM_UPDATE_KADR,MDB_KADR_FIELD_DIAG,kadr->kadr_id);
  }
 }
}

void  __fastcall     TGKTlmViewer::write_params (HKEY reg)
{
 parameters.write(reg);
}

void  __fastcall     TGKTlmViewer::write_kadrs  ()
{
 UnicodeString fname = KADRS_FILE_NAME;
 UnicodeString kadrs_file = get_data_dir();
 kadrs_file    +=fname;
 KeRTL::TFileStream fs(kadrs_file.c_str());
 KeRTL::TStorage    storage(&fs);
 AnsiString  sw(fname);
 if(storage.create  (1,sw.c_str(),sw.Length(),256))
 {
     locker l(get_mutex());
//     Tkadrs & kadrs_ordered = viewer_module->get_kadrs_ordered() ;
     Tkadrs::iterator ptr = kadrs_ordered.begin();
     Tkadrs::iterator end = kadrs_ordered.end();
     while(ptr<end)
     {
       Tmdb_kadr_ex * kadr = *ptr;
       LPMDB_KADR     k = kadr;
       DWORD rno = storage.add_record(k,sizeof(*k));
       storage.add_chunk(rno,&kadr->get_kadr_entry(),sizeof(mdb_kadr_entry));
       Tentryes_list & ent = kadr->get_entryes();
       DWORD    ent_count = ent.size();
       storage.add_chunk(rno,&ent_count,sizeof(DWORD));
       storage.add_chunk(rno,&*ent.begin(),ent_count*sizeof(Tentryes_list::value_type));

       ++ptr;
     }
 }
}

void  __fastcall     TGKTlmViewer::write_names  ()
{

 UnicodeString fname = NAMES_FILE_NAME;
 UnicodeString file_name = get_data_dir();
 file_name    +=fname;
 KeRTL::TFileStream fs(file_name.c_str());
 KeRTL::TStorage    storage(&fs);
 AnsiString  sw(fname);
 if(storage.create  (1,sw.c_str(),sw.Length(),256))
 {
    locker l(get_mutex());
//    Tnames & names = viewer_module->get_names() ;
     Tnames::iterator ptr = names.begin();
     Tnames::iterator end = names.end();
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
}

void  __fastcall     TGKTlmViewer::write_records()
{
 UnicodeString fname = RECORDS_FILE_NAME;
 UnicodeString file_name = get_data_dir();
 file_name    +=fname;
 KeRTL::TFileStream fs(file_name.c_str());
 KeRTL::TStorage    storage(&fs);
 AnsiString  sw(fname);
 //DWORD ver = 0;
 if(storage.create(1,sw.c_str(),sw.Length(),512))
 {
    locker l(get_mutex());
    //Trecords & records = viewer_module->get_records() ;
   Trecords::iterator ptr = records.begin();
   Trecords::iterator end = records.end  ();
   while(ptr<end)
   {
    storage.add_record(&*ptr,sizeof(Trecords::value_type));
    ++ptr;
   }
 }

}

 bool __fastcall TGKTlmViewer::write_config     (DWORD mask,HKEY reg_key,wchar_t * config_folder)
 {
   if(mask&CCFL_PARAMS)
      write_params(reg_key);
   if(mask&CCFL_KADRS)
      write_kadrs();
   if(mask&CCFL_NAMES)
      write_names();
   if(mask&CCFL_RECORDS)
      write_records();

  return true;
 }

 void __fastcall TGKTlmViewer::set_main_form_monitor (int val)
 {
   if(parameters.main_form_monitor != val)
   {
     parameters.main_form_monitor = val;
     TTlmViewerMainForm * mf = dynamic_cast<TTlmViewerMainForm*>(main_form);
     if(mf)
        mf->move_to_monitor(val);

   }
 }

  void  __fastcall  TGKTlmViewer::show_parameters()
  {
     if(!params_wnd)
     {
       MODULE_INFO mi;
       mi.dw_size = sizeof(mi);
       mi.mi_mask = MI_FL_ICON ;
       get_module_info(&mi);

       params_wnd  = new TTlmViewerParams(NULL) ;
       params_wnd->Icon->Handle  = mi.icon;
       params_wnd->set_parameters(parameters);
       params_wnd->OnClose                 = params_close;
       params_wnd->on_request_kadr_list    = request_kadr_list;

     }
     params_wnd->Visible = true;
     params_wnd->SetFocus();
  }

  void  __fastcall     TGKTlmViewer::params_close   (TObject * Sender,TCloseAction &Action)
  {
   if(Action!=caNone)
   {
     Action = caFree;
     params_wnd = NULL;
   }
  }

void __fastcall TGKTlmViewer::request_kadr_list(modem_addr ma )
{
 clear_kadrs (get_kadrs_avail());
 send_command(ma,FA_OTD_MEDIUM,true,true,OTDMPROTO_CMD_ENUM_KADRS,0,NULL,0);
}










