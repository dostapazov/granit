#ifndef _GKIO_BASE_INC_
#define _GKIO_BASE_INC_

#include <gkmodules.h>

#pragma pack(push,1)
typedef struct _MIO_REQUEST
{
 DWORD  dw_size;
 DWORD  flags;
 DWORD  channel;
 DWORD  data_size;
 DWORD  data_len;
 DWORD  data[ANYSIZE_ARRAY];
}MIO_REQUEST,*LPMIO_REQUEST;

#define  CMDIO_IO MCMD(MT_IO,1)
/*Команда  ввода вывода
  p1 = LPMIO_REQUEST
  p2 = BOOL   FALSE - read , TRUE write;
*/

#define MIO_REQUEST_LEN(mior)  ((mior)->data_len +sizeof(MIO_REQUEST)-sizeof(DWORD))
#define MIO_REQUEST_SIZE(mior) ((mior)->data_size+sizeof(MIO_REQUEST)-sizeof(DWORD))

typedef struct _MIO_DEV_TEXT
{
 DWORD     dw_size;
 DWORD     text_sz;
 wchar_t   dev_text[ANYSIZE_ARRAY];
}MIO_DEV_TEXT,*LPMIO_DEV_TEXT;

#define  CMDIO_GET_DEVICE_TEXT (MT_IO,10)
/*
  Получение текстовой информации об устройстве
  p1 = _MIO_DEV_TEXT

*/

typedef struct _MIO_ENUM_DEVICES
{
  DWORD   dw_size;
  DWORD   index;
  wchar_t dev_name[MAX_PATH];
}MIO_ENUM_DEVICES,*LPMIO_ENUM_DEVICES;


#define CMDIO_ENUM_DEVICES  MCMD(MT_IO,20)
/*Команда перечисления устройств*/
//p1 = LPMIO_ENUM_DEVICES

//Уведомление владельца  о наличии данных для чтения/Готовности к передаче
#define CMDIO_READ_READY   MCMD(MT_IO,100)
//p1 = LPMIO_DATA data, p2 =  DWORD len;
//В случае если возвращаемое значение != GKH_RET_SUCCESS,
//то данные помещаются во внутреннюю очередь устройства, и должны быть считаны
// Командой CMDIO_IO

//Уведомление владельца о готовности к передаче.
#define CMDIO_WRITE_READY  MCMD(MT_IO,200)
//p1 = LPMIO_DATA

/*Уведомление об ошибке*/
#define CMDIO_ERROR        MCMD(MT_IO,300)
// p1 = error _code

#define IO_ERROR_OPEN      1
#define IO_ERROR_CLOSE     2
#define IO_ERROR_CONNECT   0x80000000

#define CMDIO_ENABLE_WATCHDOG MCMD(MT_IO,300)
#define CMDIO_ENABLE_RESET    MCMD(MT_IO,301)
//p1 BOOL enable


#pragma pack(pop)

#ifdef  __cplusplus

#include <gkmodules.hpp>

 class TGKIoInterface:public TGKModuleInterface
 {
  public:
  TGKIoInterface(){};
  TGKIoInterface(GKHANDLE owner,wchar_t * dll_name,wchar_t* mod_name){load(owner,dll_name,mod_name);};
  TGKIoInterface(GKHANDLE owner,char    * dll_name,char   * mod_name){load(owner,dll_name,mod_name);};
  bool     __fastcall load(GKHANDLE owner,wchar_t * dll_name,wchar_t* mod_name);
  bool     __fastcall load(GKHANDLE owner,char    * dll_name,char   * mod_name);

  LRESULT  __fastcall read    (LPMIO_REQUEST io_req){return call(CMDIO_IO,(LPARAM)io_req,FALSE);}
  LRESULT  __fastcall write   (LPMIO_REQUEST io_req){return call(CMDIO_IO,(LPARAM)io_req,TRUE);}
  LRESULT  __fastcall enum_devices(LPMIO_ENUM_DEVICES ed){return call(CMDIO_ENUM_DEVICES,(LPARAM)ed,0); }
  bool     __fastcall get_device_text(wchar_t * text,DWORD bsz);


 };



/*Базовый класс реализации модулей ввода /вывода*/

 class TGKIOBase: public TGKModule
 {
   protected:
            BOOL       enable_watchdog;
            BOOL       enable_reset;
            WORD     __fastcall get_type  (){return MT_IO;}
   virtual  LRESULT  __fastcall read      (LPMIO_REQUEST)        = 0;
   virtual  LRESULT  __fastcall write     (LPMIO_REQUEST)        = 0;
   virtual  LRESULT  __fastcall enum_devices(LPMIO_ENUM_DEVICES) = 0;
   virtual  LRESULT  __fastcall get_device_text(wchar_t * text,DWORD text_sz){return GKH_RET_SUCCESS;}
            LRESULT  __fastcall processing(DWORD cmd,LPARAM p1,LPARAM p2);
            BOOL     __fastcall is_watchdog_enabled(){BOOL ret;lock();ret = enable_watchdog;unlock();return ret;}
   public:
   TGKIOBase(wchar_t * name);
   TGKIOBase(char    * name);
 };



 inline TGKIOBase::TGKIOBase(wchar_t * name)
 {enable_watchdog = FALSE; lock_param = GKHB_AUTO_LOCK_OFF; enable_reset = TRUE;alloc_gkhandle(name);}
 inline TGKIOBase::TGKIOBase(char    * name)
 {enable_watchdog = FALSE; lock_param = GKHB_AUTO_LOCK_OFF; enable_reset = TRUE;alloc_gkhandle(name);}

 inline LRESULT  __fastcall TGKIOBase::processing(DWORD cmd,LPARAM p1,LPARAM p2)
 {
  LRESULT ret = GKH_RET_ERROR;
  if(MCMD_TYPE(cmd) == MT_IO)
   {
     if(cmd == CMDIO_IO)
     {
     if(check_data_size((LPMIO_REQUEST)p1,sizeof(MIO_REQUEST)))
      {
        ret = p2? write((LPMIO_REQUEST)p1):read((LPMIO_REQUEST)p1);
      }
     }
     if(cmd == CMDIO_GET_DEVICE_TEXT)
     {
       if(check_data_size((LPVOID)p1,sizeof(MIO_DEV_TEXT)))
       ret = this->get_device_text(((LPMIO_DEV_TEXT)p1)->dev_text,((LPMIO_DEV_TEXT)p1)->text_sz);
     }

     if(cmd == CMDIO_ENUM_DEVICES )
      {
       if(check_data_size((LPMIO_ENUM_DEVICES)p1,sizeof(MIO_ENUM_DEVICES)))
        ret = enum_devices((LPMIO_ENUM_DEVICES)p1);
      }
     if(cmd == CMDIO_ENABLE_WATCHDOG)
        {
         lock();
         enable_watchdog = p1 ? TRUE:FALSE;
         unlock();
         ret = GKH_RET_SUCCESS;
        }

     if(cmd == CMDIO_ENABLE_RESET)
        {
         lock();
         enable_reset = p1 ? TRUE:FALSE;
         unlock();
         ret = GKH_RET_SUCCESS;
        }
   }
   else
  ret = TGKModule::processing(cmd,p1,p2);
  return ret;
 }


#endif

#endif

