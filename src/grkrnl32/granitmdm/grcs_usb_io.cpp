//---------------------------------------------------------------------------

#include "ft2usbio.hpp"
#include <gkio_base.h>
#include <kedatas.hpp>
#include <ftd2xx.h>

class  TUsbIo:public TGKIOBase,public TGKThread
{
  protected:
  wchar_t                      device_name[MAX_PATH];
  FT_HANDLE                    fthDevice;
  KeRTL::TSimpleDataQueue      rd_queue;
  KeRTL::TSimpleDataQueue      wr_queue;
  HANDLE                       stop_event;
  HANDLE                       rd_event;
  HANDLE                       wr_event;
  TSynchroSet                  synchro_set;
  OVERLAPPED                   rd_ovr;
  OVERLAPPED                   wr_ovr;

  LPBYTE  rd_buffer[1024];
  DWORD   rd_len;

  DWORD   __fastcall get_config_data(DWORD mask,LPVOID buf,DWORD bsz);
  bool    __fastcall set_config_data(DWORD mask,LPVOID buf,DWORD bsz);
  bool    __fastcall compare_config_data(DWORD mask,LPVOID buf,DWORD bsz,LPDWORD ch_mask,BOOL * restart);
  

  int      __fastcall Execute ();
  void     __fastcall on_timer();

  DWORD    __fastcall start(DWORD reason,LPARAM p);
  DWORD    __fastcall stop(DWORD reason);
  LRESULT  __fastcall read       (LPMIO_REQUEST) ;
  LRESULT  __fastcall write      (LPMIO_REQUEST) ;
  bool     __fastcall open_device();
  bool     __fastcall close_device();
  public:
  TUsbIo(wchar_t * name):TGKIOBase(name){*device_name = 0;fthDevice = NULL;}
 
};


  DWORD   __fastcall TUsbIo::get_config_data(DWORD mask,LPVOID buf,DWORD bsz)
  {
    lock();
    if(buf && bsz>=lstrlenW(device_name))
      lstrcpyW((wchar_t*)buf,device_name);
    unlock();  
    return GKH_RET_SUCCESS;  
  }
  
  bool    __fastcall TUsbIo::set_config_data(DWORD mask,LPVOID buf,DWORD bsz)
  {
   lock();
   lstrcpyW(device_name,(wchar_t*)buf);
   unlock();
   return true;
  }

  bool    __fastcall TUsbIo::compare_config_data(DWORD mask,LPVOID buf,DWORD bsz,LPDWORD ch_mask,BOOL * restart)
  {
    lock();
    if(lstrcmpiW(device_name,(wchar_t*)buf))
    {
     if(ch_mask)
        *ch_mask|=CCFL_MODULE_CONFIG;
     if(restart)
        *restart = TRUE;
    }
    unlock();
    return true;
  }

  int      __fastcall TUsbIo::Execute()
  {

   DWORD wr;
   do{
      wr = synchro_set.Wait(1000);
      switch(wr)
      {
       case 1: /*Завершилось чтение*/
             break;
       case 2: //Завершение записи
             break;
       case WAIT_TIMEOUT: on_timer();break;

      }

     }while(wr && !CheckTerminateRequest());
   return 0;
  }

  DWORD    __fastcall TUsbIo::start(DWORD reason,LPARAM p)
  {
   DWORD ret = GKH_RET_ERROR;
   if(*device_name)
   {
    /*Запуск модуля ввода вывода*/
    stop_event  = CreateEvent(NULL,FALSE,FALSE,NULL);
    rd_event    = CreateEvent(NULL,FALSE,FALSE,NULL);
    wr_event    = CreateEvent(NULL,FALSE,FALSE,NULL);
    fthDevice   = NULL;
    if(stop_event && rd_event && wr_event)
    {
      synchro_set+=stop_event;
      synchro_set+=  rd_event;
      synchro_set+=  wr_event;
      if(Start(32768))
         ret = GKH_RET_SUCCESS;
    }   
    
   }
   return ret;
  }

  DWORD    __fastcall TUsbIo::stop(DWORD reason)
  {
    DWORD ret = GKH_RET_ERROR;
    if(this->IsRunning())
    {
     SetEvent(stop_event);
     TerminateAndWait(INFINITE);
     CloseHandle(wr_event);
     CloseHandle(rd_event);
     CloseHandle(stop_event);
     synchro_set.Clear(); 
    }
  }

  LRESULT  __fastcall TUsbIo::read      (LPMIO_REQUEST io_req)
  {
    //Чтение
  }

  LRESULT  __fastcall TUsbIo::write     (LPMIO_REQUEST io_req)
  {
    //Запись
  }


bool     __fastcall TUsbIo::open_device()
{
  bool ret = false;
  char dev_name[MAX_PATH];
  lock();
  Unicode2Ansi(dev_name,device_name);
  DWORD crf       =  FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED |FT_OPEN_BY_SERIAL_NUMBER;
  fthDevice       =  FT_W32_CreateFile(dev_name,GENERIC_READ,0,NULL,OPEN_EXISTING,crf,NULL);
  if(IsValidHandle(fthDevice))
  {
   ResetEvent(rd_event);
   ResetEvent(wr_event);
   ZeroMemory(&rd_ovr,sizeof(rd_ovr));
   rd_ovr.hEvent = rd_event;
   ZeroMemory(&wr_ovr,sizeof(wr_ovr));
   wr_ovr.hEvent = wr_event;
   ret = true;
  }
  unlock();
  return ret;
}

bool     __fastcall TUsbIo::close_device()
{
 bool ret = false;
 lock();
 if(IsValidHandle(fthDevice))
 {
  FT_W32_CancelIo(fthDevice);
  FT_W32_CloseHandle(fthDevice);
  fthDevice = NULL;
 }
 unlock();
 return ret;
}


 #pragma pack(push,1)
 union W_B
 {
  WORD  w;
  struct {BYTE lo,hi;};
 };
 #pragma pack(pop)

 #define ERR_INPUT   (-1)
 #define ERR_OUTSIZE (-2)
 
 DWORD __fastcall usb_proto_convert(LPBYTE in,DWORD in_len,LPBYTE out,DWORD out_size,LPDWORD out_bytes)
 {
  DWORD ret = 0;
  if(in && in_len)
   {
    DWORD out_len = in_len<<1;
    if(out && out_len<=out_size)
    {
     if(out_bytes) *out_bytes = out_len;
     LPBYTE end = in+in_len;
     W_B *  out_ptr = (W_B*)out;
     while(in<end)
     {
      out_ptr->lo = ((*in)&0xF0)>>4;
      out_ptr->hi = (*in)&0x0F;
      out_ptr++;
      in++;
     }
    }
    else
    ret = ERR_OUTSIZE;
   }
   else
   ret = ERR_INPUT;
  return ret;
 }


 DWORD __fastcall usb_proto_deconvert(LPBYTE in,DWORD in_len,LPBYTE out,DWORD out_size,LPDWORD out_bytes)
 {
  DWORD ret = 0;
  if(in && in_len && !(in_len&1))
  {
   DWORD out_len = in_len>>1;
   if(out_len<=out_size)
   {
    if(out_bytes) *out_bytes = out_len;
    W_B * ptr = (W_B*)in,*end = ptr+out_len;
    while(ptr<end)
    {
     BYTE res  = 0;
     res  = ptr->lo&0x0F;
     res<<=4;
     res |= ptr->hi&0x0F;
     *out = res;
     out++;
     ptr++;
    }
   }
   else
    ret = ERR_OUTSIZE;
  }
  else
  ret = ERR_INPUT;//Ошибка входных данных
  return ret;
 }


LRESULT WINAPI module_main(DWORD cmd,LPARAM p1, LPARAM p2)
{
  LRESULT ret = 0;
       switch(cmd)
       {
        case GKME_CMD_GET_MODULETYPE:
			ret = MT_IO;break;
        case GKME_CMD_CREATE_HANDLE :
        {
            TUsbIo * mod = new TUsbIo((wchar_t*)p1);
            ret  = (LRESULT)(*(TGKHandleBased*)(mod))();
            if(!ret)
             delete mod;
        }
        break;
       }
       return ret;
}




