#ifdef __linux__
 #include <gklib/gksecur_proto.h>
 #include <wchar.h>
#else
 #pragma hdrstop
 #include <gksecur_proto.h>

#endif

 void    WINAPI gksecur_coder        (void * data,unsigned int sz,void * key,unsigned int key_size)
 {
    //Шифруется исключающим или по ключу
    //чет текущего значение
     if(data && sz && key && key_size)
     {
      LPBYTE d_ptr = (LPBYTE)data;
      LPBYTE d_end = d_ptr+sz;
      DWORD  curr = 0;
      LPBYTE key_ptr = (LPBYTE)key;
      LPBYTE key_end = key_ptr+key_size;
      bool   dir  = true;
      do
      {
       BYTE key_val = *key_ptr;
       if(!key_val || (curr&1))
           {
            key_val = ~key_val;
           }
       (*d_ptr)^=key_val;
       ++curr;
       key_ptr+= dir ? 1:-1;
       if((dir && key_ptr == key_end) || (!dir && key_ptr == key))
         {
          dir = !dir; if(!dir) --key_ptr;
         }
       ++d_ptr;
      }while(d_ptr<d_end);
     }
 }

 #pragma warn -8004

 DWORD WINAPI gksecur_login_extract      (LPOTDM_PROTO_HEADER omph,SESSION_ID * ssid,wchar_t * user_name,DWORD user_sz,wchar_t * passw,DWORD passw_sz)
{
 /* Дастает из принятого кадра
    SESSION_ID всегда
    user_name && psassword в случае запроса на авторизацию
    т.е если в поле OTF_MPROTO_HEADER::command не взведен бит ответа
 */
 DWORD ret = FALSE;
 if(omph && omph->data_size>sizeof(ssid) && (omph->command&OTDM_COMMAND_MASK) == GKSECUR_PROTO_CMD_LOGIN)
 {
  ret = TRUE;
  LPBYTE bptr = (LPBYTE)omph->data;
  SESSION_ID _ssid;
  _ssid = *((SESSION_ID*)bptr);
  bptr += sizeof(*ssid);
  if(ssid) *ssid = _ssid;
  if(!(omph->command&OTDM_RESPOND))
  {
    gksecur_coder(bptr,omph->data_size - sizeof(_ssid),&_ssid,sizeof(_ssid));
    lpotd_text ot;
    ot = (lpotd_text)bptr;
    if(user_name && user_sz)
    {
     memcpy(user_name,ot->text,KeRTL::MIN((DWORD)user_sz*sizeof(ot->text),(DWORD)ot->len*sizeof(ot->text)));
     if(user_sz>=(DWORD)(ot->len+1))
        user_name[ot->len] = 0;
        else
        ret = FALSE;
    }
    bptr+=sizeof(ot->len)+ot->len*sizeof(wchar_t);
    ot = (lpotd_text)bptr;
    if(passw && passw_sz)
    {
     memcpy(passw,ot->text,KeRTL::MIN((DWORD)passw_sz*sizeof(ot->text),(DWORD)ot->len*sizeof(ot->text)));
     if(passw_sz>=(DWORD)(ot->len+1))
        passw[ot->len] = 0;
        else
        ret = FALSE;
    }
    bptr = (LPBYTE)omph->data;
    bptr+=sizeof(_ssid);
    gksecur_coder(bptr,omph->data_size - sizeof(_ssid),&_ssid,sizeof(_ssid));
    bptr+=sizeof(ssid);
    bptr = bptr;
  }
 }
 return ret;
}

#pragma warn .8004

DWORD WINAPI gksecur_login_format       (void * data,unsigned int sz,SESSION_ID ss_id,wchar_t * user_name,wchar_t * user_pasw)
{
  //Данные команды авторизации
  lpotd_text ot;
  LPOTDM_PROTO_HEADER omph;
  DWORD need_sz = sizeof(*omph)-sizeof(omph->data) + sizeof(ss_id)+(sizeof(ot->len)<<1); // ИД сессии +  две длины текста
#ifdef _WINDOWS_
  if(user_name && *user_name) need_sz+= lstrlenW(user_name)*sizeof(*user_name);
  if(user_pasw && *user_pasw) need_sz+= lstrlenW(user_pasw)*sizeof(*user_pasw);
#else
  if(user_name && *user_name) need_sz+= wcslen(user_name)*sizeof(*user_name);
  if(user_pasw && *user_pasw) need_sz+= wcslen(user_pasw)*sizeof(*user_pasw);
#endif
  if(need_sz<=sz)
  {
      omph = (LPOTDM_PROTO_HEADER)data;
      memset(omph,0,sizeof(*omph));

      omph->command       = GKSECUR_PROTO_CMD_LOGIN;
      LPBYTE bptr          = (LPBYTE)omph->data;
      *((SESSION_ID*)bptr) = ss_id;
      DWORD dlen = sizeof(ss_id);
      omph->data_size+= dlen;

      bptr+=dlen;
      ot = (lpotd_text)bptr;
#ifdef _WINDOWS_
      ot->len       = lstrlenW(user_name);
#else
      ot->len       = wcslen(user_name);
#endif
      memcpy(ot->text,user_name,ot->len*sizeof(ot->text));
      dlen = sizeof(ot->len)+ot->len*sizeof(ot->text);
      omph->data_size+= dlen;

      bptr+=dlen;
      ot = (lpotd_text)bptr;
#ifdef _WINDOWS_
      ot->len       = lstrlenW(user_pasw);
#else
      ot->len       = wcslen(user_pasw);
#endif
      memcpy(ot->text,user_pasw,ot->len*sizeof(ot->text));
      dlen = sizeof(ot->len)+ot->len*sizeof(ot->text);
      omph->data_size+= dlen;

      gksecur_coder(((LPBYTE)omph->data)+sizeof(ss_id),omph->data_size-sizeof(ss_id),&ss_id,sizeof(ss_id));
      return omph->data_size + sizeof(*omph)-sizeof(omph->data);
  }

  return 0;
}



