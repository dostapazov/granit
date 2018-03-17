#include "processmdm.hpp"
#include <tregstry.hpp>
#include <io.h>

static THeap heap(TSysInfo().GetPageSize()<<8); // 512  б
static DWORD total_mem_alloc = 0;

void * operator new(size_t sz)
{

 void * ptr = heap.Alloc(sz);
 total_mem_alloc+= heap.MemSize(ptr,0);
 return ptr;
}

void * operator new [](size_t sz)
{
 void * ptr = heap.Alloc(sz);
 total_mem_alloc+= heap.MemSize(ptr,0);
 return ptr;
}

void operator delete( void * ptr)
{
  if(ptr && heap.Validate(ptr))
    {
     total_mem_alloc-= heap.MemSize(ptr,0);
     heap.Free(ptr);
    }
}

void operator delete[]( void * ptr)
{
  if(ptr && heap.Validate(ptr))
    {
     total_mem_alloc-= heap.MemSize(ptr,0);
     heap.Free(ptr);
    }
}


 TProcessModem::TProcessModem()
 {
  alloc_gkhandle((wchar_t*)NULL);
  lock_param = GKHB_AUTO_LOCK_OFF;
  
 }

wchar_t wm_name[] = L"PROCMDMWND.DLL";
DWORD  __fastcall TProcessModem::get_window_module (wchar_t * buf,DWORD bsz)
{
 int len = lstrlenW(wm_name);
 if(buf && int(bsz) > len)
  {
   SetLastError(0);
   safe_strcpy(buf,wm_name);
  }
  else
  {SetLastError(MERR_INVALID_BUFFER_SIZE);len=-len;}
 return DWORD(len);
}

 bool    __fastcall TProcessModem::read_settings  ()
 {
  bool ret ;
  TRegsReader rr(DupKey(reg_key));
  ret = TModemBase::read_settings();
  lock();
  settings.proc_name[rr.ReadString(PROCMDM_REGSTR_PROCNAME,settings.proc_name,sizeof(settings.proc_name)/sizeof(wchar_t),true)] = 0;
  settings.cmd_line [rr.ReadString(PROCMDM_REGSTR_CMDLINE ,settings.cmd_line ,sizeof(settings.cmd_line )/sizeof(wchar_t),true)] = 0;
  settings.work_dir [rr.ReadString(PROCMDM_REGSTR_WORKDIR ,settings.work_dir ,sizeof(settings.work_dir )/sizeof(wchar_t),true)] = 0;
  settings.cmd_show     = rr.ReadDword(PROCMDM_REGDW_CMDSHOW,SW_HIDE);
  settings.auto_restart = rr.ReadDword(PROCMDM_REGDW_AUTORESTART,TRUE,true);
  
  unlock();
  return ret;
 }

 bool    __fastcall TProcessModem::write_settings ()
 {

  bool ret ;
  TRegsWriter rw(DupKey(reg_key));
  ret = TModemBase::write_settings();
  lock();
  if(cc_flags&CCFL_PROCNAME)
  {
   rw.WriteString(PROCMDM_REGSTR_PROCNAME,settings.proc_name,true);
   cc_flags&=~CCFL_PROCNAME;
  }
  if(cc_flags&CCFL_CMDLINE)
  {
   rw.WriteString(PROCMDM_REGSTR_CMDLINE ,settings.cmd_line ,true);
   cc_flags&=~CCFL_CMDLINE;
  }
  if(cc_flags&CCFL_WORKDIR)
  {
   rw.WriteString(PROCMDM_REGSTR_WORKDIR ,settings.work_dir ,true);
   cc_flags&=~CCFL_WORKDIR;
  }
  if(cc_flags&CCFL_CMDSHOW)
  {
   rw.WriteDword  (PROCMDM_REGDW_CMDSHOW,settings.cmd_show);
   cc_flags&=~CCFL_CMDSHOW;
  }
  if(cc_flags&CCFL_AUTORESTART)
  {
   rw.WriteDword  (PROCMDM_REGDW_AUTORESTART,settings.auto_restart);
   cc_flags&=~CCFL_AUTORESTART;
  }
  unlock();
  return ret;
 }

 DWORD   __fastcall TProcessModem::get_config_data(DWORD mask,LPVOID buf,DWORD bsz)
 {
   LPPMSETTINGS pms =(LPPMSETTINGS)buf;
   if(check_data_size(pms,sizeof(*pms)))
   {
   if((mask&CCFL_MODEM_NUMBER))
    pms->modem_number = get_modem_number();   
   lock();
   if(mask&CCFL_PROCNAME)
      safe_strcpy(pms->proc_name,settings.proc_name);
   if(mask&CCFL_WORKDIR )
     safe_strcpy(pms->work_dir,settings.work_dir);
   if(mask&CCFL_CMDLINE )
     safe_strcpy(pms->cmd_line,settings.cmd_line);
   if(mask&CCFL_CMDSHOW )
     pms->cmd_show = settings.cmd_show;
   if(mask&CCFL_AUTORESTART )
     pms->auto_restart = settings.auto_restart;
   unlock();
     return GKH_RET_SUCCESS;
   }
  return GKH_RET_ERROR;
 }

 bool    __fastcall TProcessModem::set_config_data(DWORD mask,LPVOID buf,DWORD bsz)
 {
  bool ret = false;
  LPPMSETTINGS pms =(LPPMSETTINGS)buf;
  DWORD error = 0;
  if(check_data_size(pms,sizeof(*pms)))
  {
   ret = true;
   if((mask&CCFL_MODEM_NUMBER))
      {
       if(set_modem_number(pms->modem_number,NULL))
          {
           error = GetLastError();
           ret = false;
          }
          else
          settings.modem_number = get_modem_number();
      }
   lock();
   if(mask&CCFL_PROCNAME)
      safe_strcpy(settings.proc_name,pms->proc_name);
   if(mask&CCFL_WORKDIR )
     safe_strcpy(settings.work_dir,pms->work_dir);
   if(mask&CCFL_CMDLINE )
     safe_strcpy(settings.cmd_line,pms->cmd_line);
   if(mask&CCFL_CMDSHOW )
     settings.cmd_show = pms->cmd_show;
   unlock();

  }
  if(error) SetLastError(error);//—охранить ошибку
  return ret;
 }

 bool    __fastcall TProcessModem::compare_config_data(DWORD mask,LPVOID buf,DWORD bsz,LPDWORD ch_mask,BOOL * restart)
 {
  bool ret = false;
  LPPMSETTINGS pms =(LPPMSETTINGS)buf;
  DWORD cm   = 0;
  BOOL  rst  = FALSE;
  if(check_data_size(pms,sizeof(*pms)))
  {
   ret = true;
   if((mask&CCFL_MODEM_NUMBER) && pms->modem_number!=get_modem_number())
       cm|=CCFL_MODEM_NUMBER,rst = TRUE;
   lock();
   if((mask&CCFL_PROCNAME) && lstrcmpiW(settings.proc_name,pms->proc_name))
      cm|= CCFL_PROCNAME,rst = TRUE;

   if((mask&CCFL_WORKDIR ) &&  lstrcmpiW(settings.work_dir,pms->work_dir) )
      cm|= CCFL_WORKDIR ,rst = TRUE;

   if((mask&CCFL_CMDLINE ) && lstrcmpiW(settings.cmd_line,pms->cmd_line))
      cm|= CCFL_CMDLINE,rst = TRUE;
   if((mask&CCFL_CMDSHOW ) &&  settings.cmd_show != pms->cmd_show)
     cm|=CCFL_CMDSHOW;
   if((mask&CCFL_AUTORESTART ) &&  settings.auto_restart != pms->auto_restart)
     cm|=CCFL_AUTORESTART;
   unlock();
  }
  if(ch_mask)*ch_mask = cm; if(restart) *restart = rst;
  return ret;
 }

 bool    __fastcall TProcessModem::check_config_data  (DWORD mask,LPVOID buf,DWORD bsz)
 {
   return TModemBase::check_config_data(mask,buf,bsz);
 }

 int     __fastcall TProcessModem::convert_rx_data(LPWORD fa,LPBYTE in,int in_len,LPBYTE out,DWORD out_sz)
 {
  if(out && out_sz)
  {
  if(int(out_sz)>=in_len)
   {
    memcpy(out,in,in_len);
   }
   else
    {
     memcpy(out,in,out_sz);
     in_len = -in_len;
    }
   }
   else
   in_len = -in_len;
   return in_len;
 }

 void    __fastcall TProcessModem::free_line     (modem_line * line)
 {
  if(line)
  {
  process_line * pl = dynamic_cast<process_line*>(line);
  if(pl)
     delete pl;
  else
     delete line;
  }
 }

BOOL        __fastcall TProcessModem::can_start(DWORD reason,LPARAM p2)
{
 if(TModemBase::can_start(reason,p2))
 {
  if(GetFileAttributesW(settings.proc_name)!=(DWORD)-1)
     return TRUE;
 }
 return FALSE;
}

DWORD   __fastcall TProcessModem::start(DWORD reason,LPARAM start_param)
{
 if(!get_lines_count())
     add_line(new process_line(0));
 lock();    
 process_line * pl = dynamic_cast<process_line*>(get_line(0));
 if(pl)
    pl->set_start_param(&settings);
 unlock();       
 return TModemBase::start(reason,start_param);
}




