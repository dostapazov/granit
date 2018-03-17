#ifdef __linux__
#include <gklib/otdm_proto.h>
#else
#pragma hdrstop
#include <ke_defs.h>
#include <otd_addr.h>
#include <otdm_proto.h>
#endif

union multi_ptr
{
 LPVOID    v_ptr;
 LPBYTE    b_ptr;
 LPWORD    w_ptr;
 LPDWORD   dw_ptr;
 float   * float_ptr;
 __int64 * int64_ptr;
 lpotd_param_addr addr_ptr;
};

DWORD WINAPI otdm_pack_record(LPVOID buff,DWORD bsz,DWORD mask, LPMDB_RECORD  rec)
{
 DWORD ret = 0;
 if(rec && bsz>=sizeof(*rec))
 {
 multi_ptr ptr;
 ptr.v_ptr = buff;

 *ptr.dw_ptr = rec->rec_id;
 ++ptr.dw_ptr;
 ret+=sizeof(DWORD);

 if(!(mask & MDBR_RECORD_REMOVED))
 {
  if(mask & MDBR_FIELD_VALUE)
  {
   *ptr.float_ptr  = rec->value ;
   ++ptr.float_ptr ;
   ret+=sizeof(*ptr.float_ptr);
  }

  if(mask & MDBR_FIELD_DIAG)
  {
   *ptr.w_ptr  = rec->diag ;
   ++ptr.w_ptr ;
   ret+=sizeof(*ptr.w_ptr);
  }

  if(mask & MDBR_FIELD_STATE)
  {
   *ptr.w_ptr  = rec->state ;
   ++ptr.w_ptr ;
   ret+=sizeof(*ptr.w_ptr);
  }

  if(mask & MDBR_FIELD_TIME)
  {
   *ptr.int64_ptr  = rec->time ;
   ++ptr.int64_ptr;
   ret+=sizeof(*ptr.int64_ptr);
  }

  if(mask & MDBR_FIELD_KVANTS)
  {
   *ptr.dw_ptr  = rec->kvants ;
   ++ptr.dw_ptr ;
   ret+=sizeof(*ptr.dw_ptr);
  }


  if(mask & MDBR_FIELD_MIN_VALUE)
  {
   *ptr.float_ptr  = rec->min_value ;
   ++ptr.float_ptr ;
   ret+=sizeof(*ptr.float_ptr);
  }

  if(mask & MDBR_FIELD_MAX_VALUE)
  {
   *ptr.float_ptr  = rec->max_value ;
   ++ptr.float_ptr ;
   ret+=sizeof(*ptr.float_ptr);
  }

  if(mask & MDBR_FIELD_NAME)
  {
   *ptr.dw_ptr  = rec->name;
   ++ptr.dw_ptr ;
   ret+=sizeof(*ptr.dw_ptr);
  }

  if(mask & MDBR_FIELD_SNAME)
  {
   *ptr.dw_ptr  = rec->sname;
   ++ptr.dw_ptr ;
   ret+=sizeof(*ptr.dw_ptr);
  }

  if(mask & MDBR_FIELD_MIN_DANGER)
  {
   *ptr.float_ptr = rec->min_danger;
   ++ptr.float_ptr;
   ret+=sizeof(*ptr.float_ptr);
  }

  if(mask & MDBR_FIELD_MAX_DANGER)
  {
   *ptr.float_ptr = rec->max_danger;
   ++ptr.float_ptr;
   ret+=sizeof(*ptr.float_ptr);
  }

  if(mask & MDBR_FIELD_MIN_ALARM)
  {
   *ptr.float_ptr = rec->min_alarm;
   ++ptr.float_ptr;
   ret+=sizeof(*ptr.float_ptr);
  }

  if(mask & MDBR_FIELD_MAX_ALARM)
  {
   *ptr.float_ptr = rec->max_alarm;
   ++ptr.float_ptr;
   ret+=sizeof(*ptr.float_ptr);
  }


  if(mask & MDBR_FIELD_ADDR)
  {
   *ptr.addr_ptr  = rec->addr;
   ++ptr.addr_ptr ;
   ret+=sizeof(*ptr.addr_ptr);
  }

  if(mask & MDBR_FIELD_OTDVALUE)
  {
   *ptr.int64_ptr  = rec->otd_val;
   ++ptr.int64_ptr ;
   ret+=sizeof(*ptr.int64_ptr);
  }

  if(mask & MDBR_FIELD_OTDPD)
  {
   *ptr.dw_ptr  = rec->otd_pd;
   ++ptr.dw_ptr ;
   ret+=sizeof(*ptr.dw_ptr);
  }

  if(mask & MDBR_FIELD_OPTIONS)
  {
   *ptr.dw_ptr  = rec->options;
   ++ptr.dw_ptr ;
   ret+=sizeof(*ptr.dw_ptr);
  }

  if(mask & MDBR_FIELD_SCALEID)
  {
   *ptr.dw_ptr  = rec->scale_id;
   ++ptr.dw_ptr ;
   ret+=sizeof(*ptr.dw_ptr);
  }

  if(mask & MDBR_FIELD_PERCENT)
  {
   *ptr.dw_ptr  = rec->percent;
   ++ptr.dw_ptr ;
   ret+=sizeof(*ptr.dw_ptr);
  }
  if(mask & MDBR_FIELD_LASTVALUE)
  {
   *ptr.float_ptr  = rec->last_value;
   ++ptr.float_ptr ;
   ret+=sizeof(*ptr.float_ptr);
   *ptr.int64_ptr  = rec->last_time;
   ++ptr.int64_ptr ;
   ret+=sizeof(*ptr.int64_ptr);
  }

  if(mask & MDBR_FIELD_SECURITY)
  {
   *ptr.dw_ptr  = rec->security;
   ++ptr.dw_ptr ;
   ret+=sizeof(*ptr.dw_ptr);
  }


 }
 }
 return ret;

}

#pragma warn -8057
DWORD WINAPI otdm_unpack_record(LPVOID buff,DWORD bsz,DWORD mask, LPMDB_RECORD  rec)
#pragma warn .8057

#pragma warn -8057
{
 DWORD ret = 0;
 if(rec)
 {
 multi_ptr ptr;
 ptr.v_ptr = buff;

 rec->rec_id = *ptr.dw_ptr  ;
 ++ptr.dw_ptr;
 ret+=sizeof(DWORD);

 if(!(mask & MDBR_RECORD_REMOVED))
 {
  if(mask & MDBR_FIELD_VALUE)
  {
    rec->value = *ptr.float_ptr   ;
    ++ptr.float_ptr ;
    ret+=sizeof(*ptr.float_ptr);
  }

  if(mask & MDBR_FIELD_DIAG)
  {
   rec->diag = *ptr.w_ptr    ;
   ++ptr.w_ptr ;
   ret+=sizeof(*ptr.w_ptr);
  }

  if(mask & MDBR_FIELD_STATE)
  {
   rec->state = *ptr.w_ptr   ;
   ++ptr.w_ptr ;
   ret+=sizeof(*ptr.w_ptr);
  }

  if(mask & MDBR_FIELD_TIME)
  {
    rec->time  = *ptr.int64_ptr  ;
   ++ptr.int64_ptr;
   ret+=sizeof(*ptr.int64_ptr);
  }

  if(mask & MDBR_FIELD_KVANTS)
  {
   rec->kvants  = *ptr.dw_ptr   ;
   ++ptr.dw_ptr ;
   ret+=sizeof(*ptr.dw_ptr);
  }


  if(mask & MDBR_FIELD_MIN_VALUE)
  {
   rec->min_value =*ptr.float_ptr   ;
   ++ptr.float_ptr ;
   ret+=sizeof(*ptr.float_ptr);
  }

  if(mask & MDBR_FIELD_MAX_VALUE)
  {
   rec->max_value = *ptr.float_ptr   ;
   ++ptr.float_ptr ;
   ret+=sizeof(*ptr.float_ptr);
  }

  if(mask & MDBR_FIELD_NAME)
  {
   rec->name = *ptr.dw_ptr   ;
   ++ptr.dw_ptr ;
   ret+=sizeof(*ptr.dw_ptr);
  }

  if(mask & MDBR_FIELD_SNAME)
  {
   rec->sname = *ptr.dw_ptr  ;
   ++ptr.dw_ptr ;
   ret+=sizeof(*ptr.dw_ptr);
  }

  if(mask & MDBR_FIELD_MIN_DANGER)
  {
   rec->min_danger = *ptr.float_ptr  ;
   ++ptr.float_ptr;
   ret+=sizeof(*ptr.float_ptr);
  }

  if(mask & MDBR_FIELD_MAX_DANGER)
  {
   rec->max_danger = *ptr.float_ptr ;
   ++ptr.float_ptr;
   ret+=sizeof(*ptr.float_ptr);
  }

  if(mask & MDBR_FIELD_MIN_ALARM)
  {
   rec->min_alarm = *ptr.float_ptr ;
   ++ptr.float_ptr;
   ret+=sizeof(*ptr.float_ptr);
  }

  if(mask & MDBR_FIELD_MAX_ALARM)
  {
   rec->max_alarm = *ptr.float_ptr ;
   ++ptr.float_ptr;
   ret+=sizeof(*ptr.float_ptr);
  }


  if(mask & MDBR_FIELD_ADDR)
  {
   rec->addr = *ptr.addr_ptr   ;
   ++ptr.addr_ptr ;
   ret+=sizeof(*ptr.addr_ptr);
  }

  if(mask & MDBR_FIELD_OTDVALUE)
  {
    rec->otd_val = *ptr.int64_ptr  ;
   ++ptr.int64_ptr ;
   ret+=sizeof(*ptr.int64_ptr);
  }

  if(mask & MDBR_FIELD_OTDPD)
  {
   rec->otd_pd = *ptr.dw_ptr  ;
   ++ptr.dw_ptr ;
   ret+=sizeof(*ptr.dw_ptr);
  }

  if(mask & MDBR_FIELD_OPTIONS)
  {
   rec->options = *ptr.dw_ptr   ;
   ++ptr.dw_ptr ;
   ret+=sizeof(*ptr.dw_ptr);
  }

  if(mask & MDBR_FIELD_SCALEID)
  {
   rec->scale_id = *ptr.dw_ptr  ;
   ++ptr.dw_ptr ;
   ret+=sizeof(*ptr.dw_ptr);
  }

  if(mask & MDBR_FIELD_PERCENT)
  {
   rec->percent = *ptr.dw_ptr   ;
   ++ptr.dw_ptr ;
   ret+=sizeof(*ptr.dw_ptr);
  }
  if(mask & MDBR_FIELD_LASTVALUE)
  {
   rec->last_value = *ptr.float_ptr   ;
   ++ptr.float_ptr ;
   ret+=sizeof(*ptr.float_ptr);
   rec->last_time = *ptr.int64_ptr   ;
   ++ptr.int64_ptr ;
   ret+=sizeof(*ptr.int64_ptr);
  }

  if(mask & MDBR_FIELD_SECURITY)
  {
   rec->security = *ptr.dw_ptr  ;
   ++ptr.dw_ptr ;
   ret+=sizeof(*ptr.dw_ptr);
  }
 }
 }
 return ret;
}
#pragma warn .8057

#pragma warn -8057
DWORD WINAPI _otdm_pack_kadr(LPVOID data,DWORD data_size,DWORD mask,LPMDB_KADR kadr,bool unpack  )
{
  DWORD ret  = 0;
  multi_ptr ptr;
  ptr.v_ptr = data;

   if(unpack)   kadr->kadr_id = *ptr.dw_ptr;
      else      *ptr.dw_ptr = kadr->kadr_id;
   ++ptr.dw_ptr;
   ret+=sizeof(*ptr.dw_ptr);

if(mask&MDB_KADR_FIELD_DIAG               )
  {
    if(unpack)
      kadr->kadr_diag = *ptr.w_ptr;
      else
      *ptr.w_ptr = kadr->kadr_diag ;
    ++ptr.w_ptr;
    ret+=sizeof(*ptr.w_ptr);
  }

if(mask&MDB_KADR_FIELD_STATE              )
  {
   if(unpack)   kadr->kadr_state = *ptr.w_ptr;
        else   *ptr.w_ptr        = kadr->kadr_state;
   ++ptr.w_ptr;
    ret+=sizeof(*ptr.w_ptr);
  }
if(mask&MDB_KADR_FIELD_CHANGES            )
  {
   if(unpack) kadr->kadr_changes = *ptr.w_ptr;
      else *ptr.w_ptr = kadr->kadr_changes;
    ++ptr.w_ptr;
    ret+=sizeof(*ptr.w_ptr);
  }
if(mask&MDB_KADR_FIELD_DISCRETE_CHANGES   )
  {
   if(unpack) kadr->discrete_changes = *ptr.w_ptr;
      else *ptr.w_ptr = kadr->discrete_changes;
    ++ptr.w_ptr;
    ret+=sizeof(*ptr.w_ptr);
  }
if(mask&MDB_KADR_FIELD_ALARM_CHANGES      )
  {
   if(unpack) kadr->alarm_changes = *ptr.w_ptr;
      else *ptr.w_ptr = kadr->alarm_changes;
    ++ptr.w_ptr;
    ret+=sizeof(*ptr.w_ptr);
  }

if(mask&MDB_KADR_FIELD_ENTRY_COUNT        )
  {
   if(unpack) kadr->entry_count = *ptr.w_ptr;
      else *ptr.w_ptr = kadr->entry_count;
    ++ptr.w_ptr;
    ret+=sizeof(*ptr.w_ptr);
  }
if(mask&MDB_KADR_FIELD_NAME               )
  {
   if(unpack) kadr->name_id = *ptr.dw_ptr;
      else *ptr.dw_ptr = kadr->name_id;
    ++ptr.dw_ptr;
    ret+=sizeof(*ptr.dw_ptr);

  }
if(mask&MDB_KADR_FIELD_SNAME              )
  {
   if(unpack) kadr->name_id = *ptr.dw_ptr;
      else *ptr.dw_ptr = kadr->name_id;
    ++ptr.dw_ptr;
    ret+=sizeof(*ptr.dw_ptr);
  }

if(mask&MDB_KADR_FIELD_IMAGE              )
  {
   if(unpack) kadr->image_id = *ptr.dw_ptr;
      else *ptr.dw_ptr = kadr->image_id;
    ++ptr.dw_ptr;
    ret+=sizeof(*ptr.dw_ptr);
  }
  
if(mask&MDB_KADR_FIELD_FLAGS              )
  {
   if(unpack) kadr->kadr_flags = *ptr.dw_ptr;
      else *ptr.dw_ptr = kadr->kadr_flags;
    ++ptr.dw_ptr;
    ret+=sizeof(*ptr.dw_ptr);

  }
if(mask&MDB_KADR_FIELD_SIZE               )
  {
   if(unpack)
      {
       kadr->kadr_size.cx = *ptr.dw_ptr;
       ++ptr.dw_ptr;
       ret+=sizeof(*ptr.dw_ptr);
       kadr->kadr_size.cy = *ptr.dw_ptr;
      }
      else
      {
      *ptr.dw_ptr = kadr->kadr_size.cx;
       ++ptr.dw_ptr;
       ret+=sizeof(*ptr.dw_ptr);
      *ptr.dw_ptr = kadr->kadr_size.cy;
      }
    ++ptr.dw_ptr;
    ret+=sizeof(*ptr.dw_ptr);

  }
if(mask&MDB_KADR_FIELD_TIMES              )
  {
   if(unpack)
      {
       kadr->image_time = *ptr.int64_ptr;
       ++ptr.int64_ptr;
       ret +=sizeof(*ptr.int64_ptr);
       kadr->content_time = *ptr.int64_ptr;
      }
      else
      {
       *ptr.int64_ptr = kadr->image_time;
       ++ptr.int64_ptr;
       ret +=sizeof(*ptr.int64_ptr);
        *ptr.int64_ptr = kadr->content_time ;
      }
       ++ptr.int64_ptr;
       ret +=sizeof(*ptr.int64_ptr);
  }
 return ret;
}
#pragma warn .8057

DWORD WINAPI otdm_pack_kadr(LPVOID data,DWORD data_size,DWORD mask,LPMDB_KADR kadr )
{
 return _otdm_pack_kadr(data,data_size,mask,kadr,false);
}

DWORD WINAPI otdm_unpack_kadr(LPVOID data,DWORD data_size,DWORD mask,LPMDB_KADR kadr )
{
 return _otdm_pack_kadr(data,data_size,mask,kadr,true);
}

