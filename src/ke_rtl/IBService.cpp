#pragma hdrstop
#include <ibclass.h>
#include <iblib_i.h>

#ifndef _NO_NAMESPACE
namespace KeRTL{// end of namespace
#endif
 extern TIBCLLib  cl_lib;
 
 const char  *  __fastcall TIBService::get_ib_class_name() const
 {
  return "TIBService"; 
 }

void __fastcall TIBService::detach()
 {
  cl_lib.isc_service_detach(status_vector,&svc_handle);
 }

bool __fastcall TIBService::svc_query(int spb_len,char * spb,int req_len,char * req,int buf_len,char *buf)
 {
  status = cl_lib.isc_service_query(status_vector,&svc_handle,NULL,spb_len,spb,req_len,req,buf_len,buf);
  return is_status_ok();
 }


 bool __fastcall TIBService::attach(const char * srv,const char * dba_user,const char * dba_pass)
 {
  this->Revert(true);
  add_param(isc_spb_version);
  add_param(isc_spb_current_version);
  add_param(isc_spb_user_name,(ISC_UCHAR*)dba_user,strlen(dba_user));
  add_param(isc_spb_password,(ISC_UCHAR*)dba_pass,strlen(dba_pass));
  char svc_name[MAX_PATH];
  *svc_name = 0;
  if(srv && *srv)
    strcpy(svc_name,srv),strcat(svc_name,":");

   strcat(svc_name,"service_mgr");
   status = cl_lib.isc_service_attach(status_vector,0,svc_name,&svc_handle,this->get_params_size(),this->get_ib_params());
   this->Revert(true);
   return is_status_ok();
 }


 void __fastcall TIBService::add_spb_param_string(BYTE type,const char * str)
 {
  if(str && * str)
  {
   WORD      len = strlen(str);
   LPBYTE    c_len = (LPBYTE)&len;
   params.reserve(sizeof(type)+sizeof(len)+len);
   params.push_back(type);
   params.push_back(*c_len++);
   params.push_back(*c_len);
   std::copy(str,str+len,std::back_inserter(params));
  }
 }

 void __fastcall TIBService::add_spb_param_number(BYTE type,DWORD value)
 {
   params.reserve(sizeof(type)+sizeof(value));
   params.push_back(type);
   params.push_back(value&0xFF);
   params.push_back((value>>8)&0xFF);
   params.push_back((value>>16)&0xFF);
   params.push_back((value>>24)&0xFF);
 }

 bool __fastcall TIBService::chg_user(const char * user,const char * passw,const char * Fname,const char * Mname,const char * Sname)
 {
  bool ret ;
  Revert(true);
  add_param(isc_action_svc_modify_user);
  add_spb_param_string(isc_spb_sec_username,user);
  add_spb_param_string(isc_spb_sec_password,passw);
  add_spb_param_string(isc_spb_sec_firstname,Fname);
  add_spb_param_string(isc_spb_sec_middlename,Mname);
  add_spb_param_string(isc_spb_sec_lastname,Sname);
  status = cl_lib.isc_service_start(status_vector,&svc_handle,NULL,this->get_params_size(),get_ib_params());
  ret = is_status_ok();
  if(!ret)
     this->get_error_text();
  return ret;
 }

 bool __fastcall TIBService::add_user(const char * user,const char * passw,const char * Fname,const char * Mname,const char * Sname)
 {
  bool ret;
  Revert(true);
  add_param(isc_action_svc_add_user);
  add_spb_param_string(isc_spb_sec_username,user);
  add_spb_param_string(isc_spb_sec_password,passw);
  add_spb_param_string(isc_spb_sec_firstname,Fname);
  add_spb_param_string(isc_spb_sec_middlename,Mname);
  add_spb_param_string(isc_spb_sec_lastname,Sname);
  status = cl_lib.isc_service_start(status_vector,&svc_handle,NULL,this->get_params_size(),get_ib_params());
  ret = is_status_ok();
  if(!ret)
     this->get_error_text();
  return ret;
 }

 bool __fastcall TIBService::rem_user(const char * user)
 {
  Revert(true);
  add_param(isc_action_svc_delete_user);
  add_spb_param_string(isc_spb_sec_username,user);

  status   = cl_lib.isc_service_start(status_vector,&svc_handle,NULL,this->get_params_size(),get_ib_params());
  bool ret = is_status_ok();
  if(!ret)
     this->get_error_text();
  return ret;
 }

 bool   __fastcall TIBService::fetch_users(ib_users_list & ul,char * buff)
 {
  short str_len;
  ib_user_def ud;
  bool ret = false;

  while(*buff!=isc_info_end)
  {
   switch(*buff)
   {
   case isc_info_truncated:
    ret = true;
    break;
   case isc_spb_sec_userid:
     ++buff;
     ud.user_id = cl_lib.isc_portable_integer((ISC_UCHAR*)buff,sizeof(int));
     buff+=sizeof(int);
   break;
   case isc_spb_sec_groupid:
     ++buff;
     ud.group_id = cl_lib.isc_portable_integer((ISC_UCHAR*)buff,sizeof(int));
     buff+=sizeof(int);
   break;
   case isc_spb_sec_username:
     if(ud.user_id!= -1)
       {
        ul.push_back(ud);
        ud = ib_user_def();
       }
     ++buff;
     str_len = cl_lib.isc_portable_integer((ISC_UCHAR*)buff,sizeof(str_len));
     buff+=sizeof(str_len);
     if(str_len)
        ud.user_name = string((char*) buff,str_len);
     buff+=str_len;
   break;
   case isc_spb_sec_groupname:
   break;
   case isc_spb_sec_firstname:
     ++buff;
     str_len = cl_lib.isc_portable_integer((ISC_UCHAR*)buff,sizeof(str_len));
     buff+=sizeof(str_len);
     if(str_len)
     {
      ud.first_name = string((char*)buff,str_len);
      buff+=str_len;
     }
   break;
   case isc_spb_sec_middlename:
     ++buff;
     str_len = cl_lib.isc_portable_integer((ISC_UCHAR*)buff,sizeof(str_len));
     buff+=sizeof(str_len);
     if(str_len)
     {
     ud.middle_name = string((char*)buff,str_len);
     buff+=str_len;
     }

   break;
   case isc_spb_sec_lastname :
     ++buff;
     str_len = cl_lib.isc_portable_integer((ISC_UCHAR*)buff,sizeof(str_len));
     buff+=sizeof(str_len);
     if(str_len)
     {
     ud.last_name = string((char*)buff,str_len);
     buff+=str_len;
     }

   break;
   default:
      ++buff;break;

   }
  }
  if(ud.user_id != -1)
     ul.push_back(ud);
  return ret;
 }

 bool __fastcall TIBService::display_users(ib_users_list & ul)
 {
   Revert(true);
   add_param(isc_action_svc_display_user);
   status   = cl_lib.isc_service_start(status_vector,&svc_handle,NULL,this->get_params_size(),get_ib_params());
   if(is_status_ok())
   {
    char req = isc_info_svc_get_users;
    char buff[512] = {0};
    do{
       svc_query(0,0,1,&req,sizeof(buff),buff);
      }while(fetch_users(ul,buff));
   }
  return false;
 }


 bool __fastcall TIBService::backup  (const char * db_name,const char * bkp_name,ISC_ULONG opt,bool verbose )
 {
  Revert(true);
  add_param           (isc_action_svc_backup);
  add_spb_param_string(isc_spb_dbname,db_name);
  add_spb_param_string(isc_spb_bkp_file,bkp_name);
  if(opt)
  add_spb_param_number(isc_spb_options,opt);
  if(verbose)
     add_param(isc_spb_verbose);

  status   = cl_lib.isc_service_start(status_vector,&svc_handle,NULL,this->get_params_size(),get_ib_params());
  bool ret = is_status_ok();
  return ret;
 }

 bool __fastcall TIBService::restore (const char * db_name,const char * bkp_name,ISC_ULONG opt,ISC_ULONG page_size,ISC_ULONG buffers,bool ro,bool verbose)
 {
  Revert(true);
  add_param           (isc_action_svc_restore);
  add_spb_param_string(isc_spb_dbname,db_name);
  add_spb_param_string(isc_spb_bkp_file,bkp_name);
  if(opt)
     add_spb_param_number(isc_spb_options,opt);

  if(page_size)
     add_spb_param_number(isc_spb_res_page_size,page_size);

  if(buffers)
     add_spb_param_number(isc_spb_res_buffers,buffers);


  add_param(isc_spb_res_access_mode);
  add_param(ro ? isc_spb_prp_am_readonly:isc_spb_prp_am_readwrite);

  if(verbose)
    add_param(isc_spb_verbose);

  status   = cl_lib.isc_service_start(status_vector,&svc_handle,NULL,this->get_params_size(),get_ib_params());
  bool ret = is_status_ok();
  return ret;
 }

 bool __fastcall TIBService::is_running()
 {
   bool ret = false;
   char res_buf[128]   = {0};
   char req = isc_info_svc_running;
   if(svc_query(0,NULL,1,&req,sizeof(res_buf),res_buf))
      {
       ret = cl_lib.isc_portable_integer((ISC_UCHAR*)res_buf+1,sizeof(int)) ? true:false;
      }
   return ret;

 }


 bool __fastcall TIBService::get_svc_line(std::string & str)
 {
  char buf[1024];
  char req = isc_info_svc_line;
  if(svc_query(0,0,1,&req,sizeof(buf),buf))
  {
    int len = cl_lib.isc_portable_integer((ISC_UCHAR*)buf+1,sizeof(short));
     if(len)
     {
      str.reserve(len-1);
      char * beg = buf+1+sizeof(short);
      char*  end = beg+len-1;
	  std::copy(beg,end,std::back_inserter(str));
      if(*end == isc_info_truncated)
         get_svc_line(str);
     }
    return true;
  }
  return false;
 }

 bool __fastcall TIBService::get_svc_text(std::string & str)
 {
  char buf[1024];
  char req = isc_info_svc_to_eof;
  if(svc_query(0,0,1,&req,sizeof(buf),buf))
  {
    int len = cl_lib.isc_portable_integer((ISC_UCHAR*)buf+1,sizeof(short));
     if(len)
     {
      str.reserve(str.length()+len-1);
      char * beg = buf+1+sizeof(short);
      char*  end = beg+len-1;
	  //str.push_back(' ');
	  std::copy(beg,end,std::back_inserter(str));
      end++;
      if(*end == isc_info_truncated)
         get_svc_text(str);
     }
    return true;
  }
  return false;
 }

 bool __fastcall TIBService::get_statistics(char * db_name,unsigned long opt)
 {
  Revert(true);
  add_param           (isc_action_svc_db_stats);
  add_spb_param_string(isc_spb_dbname,db_name);
  add_spb_param_number(isc_spb_options,opt);
  status   = cl_lib.isc_service_start(status_vector,&svc_handle,NULL,this->get_params_size(),get_ib_params());
  bool ret = is_status_ok();
  return ret;

 }

 bool __fastcall TIBService::sweep(const char * db_name)
 {
  Revert(true);
  add_param           (isc_action_svc_repair);
  add_spb_param_string(isc_spb_dbname,db_name);
  add_spb_param_number(isc_spb_options,isc_spb_rpr_sweep_db);
  status   = cl_lib.isc_service_start(status_vector,&svc_handle,NULL,this->get_params_size(),get_ib_params());
  bool ret = is_status_ok();
  return ret;
 }

 bool __fastcall TIBService::shutdown(const char * db_name,int timeout,int type)
 {
  // Перевод базы в shutdown
  // Тип указывает как
  // 0  = isc_spb_prp_shutdown_db 
  // 1  = isc_spb_prp_deny_new_attachments
  // 2  = isc_spb_prp_deny_new_transactions

  Revert(true);
  add_param           (isc_action_svc_properties);
  add_spb_param_string(isc_spb_dbname,db_name);
  add_spb_param_number(isc_spb_prp_shutdown_db + type ,timeout);
  status   = cl_lib.isc_service_start(status_vector,&svc_handle,NULL,this->get_params_size(),get_ib_params());
  bool ret = is_status_ok();
  return ret;
 }

 bool __fastcall TIBService::online  (const char * db_name)
 {
  Revert(true);
  // Gthtdjl , в онлайн
  add_param           (isc_action_svc_properties);
  add_spb_param_string(isc_spb_dbname,db_name);
  this->add_spb_param_number(isc_spb_options , isc_spb_prp_db_online);
  status   = cl_lib.isc_service_start(status_vector,&svc_handle,NULL,this->get_params_size(),get_ib_params());
  bool ret = is_status_ok();
  return ret;
 }








#ifndef _NO_NAMESPACE
}// end of namespace
#endif



