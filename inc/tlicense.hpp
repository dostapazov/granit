#ifndef _TLICENSE_INC_
#define _TLICENSE_INC_

#include <kertlio.hpp>
#include <storage.hpp>

using namespace KeRTL;

    #define RECNO_MODNAME 0
    #define RECNO_STRINGS 1
    #define RECNO_UID     2
    #define RECNO_TIMES   3
    #define RECNO_COMMENT 4


class TLicense
{
 protected:
   KeRTL::TFileStream * fs;
   KeRTL::TStorage    * storage;
   DWORD                storage_version;
   bool __fastcall open_storage  (const wchar_t * module_name);
   bool __fastcall create_storage(const wchar_t * module_name,DWORD ver = 1,const wchar_t * ext = 0);
  DWORD __fastcall get_lic_string        (DWORD rec_no,DWORD chunk,wchar_t * buf,DWORD sz);

 public:
          TLicense():fs(NULL),storage(NULL) {}
 virtual ~TLicense()          {close();}
 void  __fastcall close();
 bool  __fastcall is_valid_license      (wchar_t * module_name);
 bool  __fastcall generate_timed_license(wchar_t * module_name,DWORD days,wchar_t * comment);
 bool  __fastcall open_license          (wchar_t * file_name  );
 DWORD __fastcall get_lic_computer_name (wchar_t * buf,DWORD sz);
 DWORD __fastcall get_lic_volume_name   (wchar_t * buf,DWORD sz);
 DWORD __fastcall get_lic_mem_info      (wchar_t * buf,DWORD sz);
 DWORD __fastcall get_lic_uid_str       (wchar_t * buf,DWORD sz);
 bool  __fastcall get_times             (LPDWORD t1,LPDWORD t2 );
 bool  __fastcall is_license_active     ();
 DWORD __fastcall get_module_name       (wchar_t * buf,DWORD sz);
 DWORD __fastcall time_increase         (DWORD sec);
 DWORD __fastcall time_remind           ();
 DWORD __fastcall get_comment           (wchar_t * comment,DWORD bsz);
 bool  __fastcall is_opened             (){return storage && storage->is_good() ? true : false;}
 bool  __fastcall activate_license      (wchar_t * key,DWORD key_len);



 static int   __fastcall gen_uuid_string (wchar_t * result,int res_size);
 static void __fastcall coder(wchar_t * str,int len);
 static int  __fastcall coder( void  * data,int len);
};

#endif //_TLICENSE_INC_
