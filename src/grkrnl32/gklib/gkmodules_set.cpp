/***************************************************************
 *
 * Имя файла : gklib
 * Автор     : Остапенко Д.В.
 * Версия    : 1.0
 * Создан    : 04.07.2002
 * Описание  : Реализация набора модулей
 *
 ****************************************************************/

 #pragma hdrstop
 #include <gkmodules.hpp>
 #include <tregstry.hpp>
 #include <string>

   TGKModuleInterface* __fastcall TGKModulesSet::get_interface(GKHANDLE _h,DWORD /*type*/)
   {
    TGKModuleInterface * ret = new TGKModuleInterface(_h);
    return ret;
   }

   void             __fastcall TGKModulesSet::free_interface(TGKModuleInterface* iface)
   {
    if(iface)
       {
         iface->close();
         delete iface;
       }
   }

   LRESULT          __fastcall TGKModulesSet::processing  (DWORD cmd,LPARAM p1,LPARAM p2)
   {
     LRESULT ret;
     switch(cmd)
     {

      case   MCMD_GETMODULES_COUNT:
       ret = get_modules_count();
       break;
      case   MCMD_ENUM_MODULES:
       ret = LRESULT(enum_modules(int(p1)));
       break;
      case   MCMD_APPEND_MODULE:
       ret =(LRESULT) append_module((wchar_t*)p1,p2 ? true:false) ;
       break;
      case MCMD_REMOVE_MODULE:
       ret = remove_module(GKHANDLE(p1),p2 ? false:true);
       break;
      case MCMD_REPLACE_MODULE:
       ret = replace_module((GKHANDLE) p1,(wchar_t *) p2);
       break;
      default: ret = TGKModule::processing(cmd,p1,p2);
     }
     return ret;
   }

   bool   __fastcall TGKModulesSet::init_loaded_module(TGKModuleInterface * iface,HKEY module_reg_key,wchar_t * rk_name,wchar_t * config_folder)
   {
    bool ret = false;
    if(iface)
    {
     iface->set_language(current_lang);
     //Подготовка данных для инициализации
     MODULE_INIT m_init;
     ZeroMemory (&m_init,sizeof(m_init));
     m_init.dw_size = sizeof(m_init);
     m_init.owner   = _handle;
     m_init.reg_key = module_reg_key;
     m_init.config_folder = NULL;
     if(config_folder )
     {
      DWORD cfs = lstrlenW(config_folder);
      m_init.config_folder = new wchar_t[cfs+1];
      safe_strcpy(m_init.config_folder,config_folder);
     } 

     if(rk_name )
     {
      m_init.reg_key_name_size = lstrlenW(rk_name);
      m_init.reg_key_name = new wchar_t[m_init.reg_key_name_size+1];
      safe_strcpy(m_init.reg_key_name,rk_name);
     } 
     
     //Инициализация модуля
     ret = iface->module_init(&m_init);
     if(m_init.config_folder)
        delete [] m_init.config_folder;
     if(m_init.reg_key_name_size)
        delete [] m_init.reg_key_name;
     if(!ret)
        SetLastError(MERR_MODULE_INITIALIZATION);
        
    }
    return ret;
   }


   //Функция загрузки модуля
   TGKModuleInterface * __fastcall TGKModulesSet::do_load_module(wchar_t * image_name)
   {
      TGKModuleInterface * ret = NULL;
      TGKModuleLoader    mldr;
      GKHANDLE           mod_handle;
      DWORD              type;
      if(mldr.load_module(image_name))
      {
       type = mldr.get_module_type();
         if(check_module_type(type))
         {
          mod_handle = mldr.create_module();
          if(mod_handle)
          ret = get_interface(mod_handle,type);
         else
           SetLastError(MERR_CREATE_GKHANDLE);
       }
       else
         SetLastError(MERR_UNSUPPORTED_MODULE_TYPE);
         mldr.unload_module();
      }
      return  ret;
   }

   bool             __fastcall TGKModulesSet::load_module(wchar_t * key_name)
   {
    return load_module(reg_key,key_name);
   }

   bool             __fastcall TGKModulesSet::load_module(HKEY rgk,wchar_t * key_name)
   {
     bool ret = false;
     wchar_t  module_image[MAX_PATH<<1];
     TRegsRdWr rr(DupKey(rgk));
     DWORD error = 0;
     if(rr.OpenKey(key_name))
     {
       if(rr.ReadString(MREG_VALUE_IMAGEPATH,module_image,sizeof(module_image)/sizeof(wchar_t)))
         {
           TGKModuleLoader  mldr;
           //Читаем месторасположение модуля
           mldr.load_module(module_image);//Загружаем
           if(mldr.is_module())
           {
             DWORD    mod_type = mldr.get_module_type();
             if(check_module_type(mod_type))
             {
             if(rr.OpenKey(MREG_KEY_SETTINGS,true))
             {
              GKHANDLE handle   = mldr.create_module();
              if(handle   )
              {

               TGKModuleInterface * iface = get_interface(handle,mod_type);
               if(iface )
               {
               if(!init_loaded_module(iface,rr(),key_name,config_folder) || !accept_loaded_module(iface,mod_type))
                     {
                      error = GetLastError();
                      wchar_t text[4096];
                      iface->get_error_text(error,text,sizeof(text)/sizeof(wchar_t));
                      report(L"error init_loaded_module",text);
                      free_interface(iface);//ошибка инициализации // ошибка добавления в список
                     }
                     else
                     ret = true;
             }
            }
             else
              error = MERR_CREATE_GKHANDLE;
           }
           else
             error = MERR_WRITE_IN_REGISTRY;
            }
            else error = MERR_UNSUPPORTED_MODULE_TYPE;

           }
            else
             error = MERR_DLLNOT_MODULE;
           mldr.unload_module();
         }
           else
            error = GetLastError();//MERR_REGISTRY_MODULE_IMAGE;
            //Или нет функции или не поддерживаемый тип модуля

     }
         else
          error = MERR_REGISTRY_MODULE_IMAGE;

     if(error)
        {
         wchar_t err_msg[1024];
         wchar_t err_msg1[1024];
         ERROR_TEXT er;
         er.dw_size  = sizeof(er);
         er.error_text = err_msg1;
         er.error_text_size = sizeof(err_msg1)/sizeof(wchar_t);
         get_error_text(error,&er);
         int len = get_common_string(IDS_COMMON_LOADFAIL,err_msg,sizeof(err_msg1)/sizeof(wchar_t));
         safe_strcpy(err_msg+len,module_image);
         safe_strcpy(err_msg+lstrlenW(err_msg),L"\r\n");
         safe_strcpy(err_msg+lstrlenW(err_msg),err_msg1);
         report(EVENT_SYSTEM,REPORT_INFORMATION_TYPE,err_msg);
        }
     SetLastError(error);
     return ret;
   }


   /*Функция загрузки модулей*/

   void             __fastcall TGKModulesSet::load_modules()
   {
     pre_load_modules();
     if(reg_key)
     {
       TRegsReader rr;
       rr.SetRootKey(DupKey(reg_key));
       wchar_t module_key_name[MAX_PATH<<1];
       bool do_countinue;
       int index = 0;
       do{
          DWORD nsz = sizeof(module_key_name)/sizeof(wchar_t);
          do_countinue = rr.EnumKeys(index++,module_key_name,&nsz,true);
          if(do_countinue)
             {
               int need_delete = -1; 
               if(rr.OpenKey(MREG_KEY_SETTINGS))
               {//Проверка того, что модуль не удалён
                need_delete = int(rr.ReadDword(MREG_VALUE_DELETEMODULE,-1));
                rr.CloseKey();
               }
               if(need_delete <0)
                 load_module(module_key_name);
                else
                 {
                  TRegsWriter rw(DupKey(rr.GetRootKey()));
                  rw.DeleteKey(module_key_name);
                  rw.Flush();
                 } 
             }
         }while(do_countinue);
     }
   }

   void __fastcall TGKModulesSet::pre_load_modules()
   {
     /*if(reg_key)
     {
       TRegsReader rr;
       rr.SetRootKey(DupKey(reg_key));
       wchar_t module_key_name[MAX_PATH<<1];
       bool do_countinue;
       int index = 0;
       do
       {
         DWORD nsz = sizeof(module_key_name)/sizeof(wchar_t);
         do_countinue = rr.EnumKeys(index++,module_key_name,&nsz,true);
         if(do_countinue)
         {
           //Открытие ключа  Settings && проверка того ,что этот модуль удален
           TRegsReader  _rr(DupKey(rr.GetRootKey()),module_key_name);
           if(_rr.OpenKey(MREG_KEY_SETTINGS)  )
           {
             int need_delete = int(_rr.ReadDword(MREG_VALUE_DELETEMODULE,-1));
             rr.CloseKey();
             if(need_delete>=0)
             {
               TRegsWriter rwr(DupKey(rr.GetRootKey()));
               rwr.DeleteKey(module_key_name);
               rwr.Flush();
             }
           }
         }
       }while(do_countinue);
     }*/
   }

   bool            __fastcall TGKModulesSet::init_module  (LPMODULE_INIT mi)
   {
    if(TGKModule::init_module(mi))
    {
     load_modules();
     return true;
    }
    return false;
   }


   void __fastcall TGKModulesSet::unload_modules()
   {
    TGKModuleInterface * iface ;
    int m_count  = (int) modules_list.get_modules_count();

	  while(m_count--)
	  {
	   iface = modules_list.remove(m_count);
	   wchar_t rep_text[1024];
	   rep_text[0] = 0;
	   if(iface)
	   {
		get_common_string(IDS_COMMON_UNLOAD,rep_text,sizeof(rep_text)/sizeof(wchar_t));
		int len=wcslen(rep_text);
		iface->get_module_name(rep_text+len,(sizeof(rep_text)/sizeof(wchar_t)-len));
		report(EVENT_SYSTEM,REPORT_INFORMATION_TYPE,rep_text);
	   }
          //__try{
	     free_interface(iface);
           // }
            //__finally
            //{
            // iface->close();
            //}
	  };
   }


   void            __fastcall TGKModulesSet::release( LPVOID c )
   {
	//Подготовка к удалению
      unload_modules();
      TGKModule::release(c);
   }


   LRESULT             __fastcall TGKModulesSet::replace_module(GKHANDLE module,wchar_t * new_image)
   {
    /*Замена модуля*/
    LRESULT ret = GKH_RET_ERROR;
    int i_idx = modules_list.get_interface_index(module);
    if(i_idx<modules_list.get_modules_count())
    {
     wchar_t old_image[MAX_PATH<<1];
     TGKModuleInterface miface(module,true);
     miface.get_dll_name(old_image,sizeof(old_image)/sizeof(wchar_t));
     //1)Проверить является ли новый модуль другим
     if(lstrcmpiW(old_image,new_image))
     {
      TRegsRdWr reg(DupKey(reg_key));
      if(miface.get_usage_counter()==1)
        {
         //Замена
        }
        else
        {
          reg.WriteString(MREG_VALUE_REPLACEFROM,new_image);
        }
     }
    }
    return ret;
   }

   GKHANDLE            __fastcall TGKModulesSet::append_module(wchar_t * image_path,bool permanent)
   {
    //Добавление модуля
    GKHANDLE ret = (GKHANDLE)GKH_RET_ERROR;
    DWORD    error = 0;
    bool   append_ok(false);
    TGKModuleLoader mldr;
    TGKModuleInterface * iface = NULL;
    SetLastError(0);
    if(mldr.load_module(image_path))
    {

      if(mldr.is_module())
      {
      DWORD    mod_type = mldr.get_module_type();
      if( check_module_type(mod_type))
      {

         wchar_t module_name[MAX_PATH];
         wchar_t module_image_path[MAX_PATH<<1];
         wchar_t drive[MAX_PATH];
         GetModuleFileNameW(mldr.get_dll_instance(),module_image_path,sizeof(module_image_path)/sizeof(wchar_t));
        _wsplitpath(module_image_path,drive,0,module_name,0);
         GetDriveTypeW(drive);

        GKHANDLE handle   = mldr.create_module();
       if(handle )
       {
       iface = get_interface(handle,mod_type);
       iface->set_language(current_lang);
       if(!permanent)
          {
           append_ok = accept_loaded_module(iface,iface->get_type());
          }
       else
       {
       TRegsWriter rw(DupKey(reg_key));
       int key_number   = 1;
       int key_name_len = lstrlenW(module_name);
       //Создание уникального имени
       while(rw.OpenKey(module_name))
       {
        rw.CloseKey();
        wsprintfW(module_name+key_name_len,L"_%d",key_number++);
       }

       if(rw.OpenKey(module_name,true))
       {
         rw.WriteString(MREG_VALUE_IMAGEPATH,module_image_path);
         if(rw.OpenKey (MREG_KEY_SETTINGS,true))
         {
          rw.WriteDword(MREG_VALUE_RUNNING,0);
          init_loaded_module(iface,rw(),module_name,config_folder);
          append_ok = accept_loaded_module(iface,iface->get_type());

         }
       }
      else
        error = MERR_WRITE_IN_REGISTRY;// невозможно зписать параметры модуля в реестр

      }
      }else
      error =  MERR_CREATE_GKHANDLE  ;
     }
     else
     error = MERR_UNSUPPORTED_MODULE_TYPE;
     mldr.unload_module();
     }
     else
      error = MERR_DLLNOT_MODULE;
    }

    if(append_ok)
       {
        ret = (*iface)();
        wchar_t rep_text[1024];
        int  len =  get_common_string(IDS_COMMON_PLUG_MODULE,rep_text,sizeof(rep_text)/sizeof(wchar_t));
        iface->get_module_name(rep_text+len,sizeof(rep_text)/sizeof(wchar_t)-len);
        report(EVENT_SYSTEM,REPORT_SUCCESS_TYPE,rep_text);
       }
      else
      if(iface)
        free_interface(iface);

    if(error)
      SetLastError(error);
    return ret;
   }

   void  __fastcall TGKModulesSet::notify_module_load(GKHANDLE h,DWORD mod_type,BOOL load)
   {
    if(this->need_notify(MNF_COMMON))
    {
     BYTE buff [sizeof(NOTIFY_CODE)+sizeof(DWORD)];
     LPNOTIFY_CODE nc = (LPNOTIFY_CODE)buff;
     nc->dw_size     = sizeof(buff);
     nc->from_module = _handle;
     nc->notify_code = load ? MNCODE_ADDED_MODULE:MNCODE_REMOVED_MODULE;
     LPDWORD notify_data = nc->notify_data;
     notify_data[0] = (DWORD)h;
     notify_data[1] = mod_type;
     notify_handles(nc,MNF_COMMON);
     }
      wchar_t  rep_text[1024];
      int len = get_common_string(IDS_COMMON_UNLOAD-load,rep_text,sizeof(rep_text)/sizeof(wchar_t));
      TGKModuleInterface(h,true).get_module_name(rep_text+len,(sizeof(rep_text)/sizeof(wchar_t)-len));
      report(EVENT_SYSTEM,REPORT_INFORMATION_TYPE,rep_text);
   }
   
   bool  __fastcall TGKModulesSet::accept_loaded_module(TGKModuleInterface * iface,DWORD type)
   {
    bool ret = modules_list.add(iface);
    notify_module_load((*iface)(),type,TRUE);
    return ret;
   }

   bool            __fastcall TGKModulesSet::remove_module(GKHANDLE h,bool erase)
   {
    //Удаление модуля
     bool   ret(false);
     TGKModuleInterface * iface = modules_list.remove(modules_list.get_interface_index(h));
     if(iface)
     {
       if(erase)
       {
        TRegsWriter wr;
        wchar_t rkname  [MAX_PATH];
        CONFIG_PARAMS cp;
        ZeroMemory(&cp,sizeof(cp));
        cp.dw_size = sizeof(cp);
        cp.reg_key_name_size = sizeof(rkname)/sizeof(wchar_t);
        cp.reg_key_name = rkname;
        iface->get_config_params(&cp);
        wr.SetRootKey(DupKey(reg_key));
        if(!wr.DeleteKey(rkname))
        {
         wr.CloseKey(true);
         wr.SetRootKey(DupKey(cp.reg_key));
         wr.WriteDword(MREG_VALUE_DELETEMODULE,1,true);
        }
         wchar_t rep_text[1024];
         int  len =  get_common_string(IDS_COMMON_UNPLUG_MODULE,rep_text,sizeof(rep_text)/sizeof(wchar_t));
         iface->get_module_name(rep_text+len,sizeof(rep_text)/sizeof(wchar_t)-len);
         report(EVENT_SYSTEM,REPORT_WARNING_TYPE,rep_text);
       }
        notify_module_load((*iface)(),iface->get_type(),FALSE);
        free_interface(iface);
        ret = true;
     }
    return ret;
   }


   DWORD           __fastcall TGKModulesSet::start(DWORD reason,LPARAM p2)
   {
     set_module_state(MODULE_STATE_START_PENDING,true);

     {
     modules_list.lock();
     TGKModuleInterface * mir;
     int i = 0;
     do{
        mir = modules_list[i++];
        CONFIG_PARAMS cp;
        ZeroMemory(&cp,sizeof(cp));
        cp.dw_size = sizeof(cp);
        if(mir)
        {
          mir->get_config_params(&cp);
          TRegsReader rr(DupKey(cp.reg_key));
          if(rr.ReadDword(MREG_VALUE_RUNNING,0,true))
             mir->start(reason == MODULE_START_REQUEST ? MODULE_START_OWNER_REQUEST:reason,p2);
          rr.CloseKey(true);
        }
       }while(mir);
      modules_list.unlock();
     }
     return  TGKModule::start(reason,p2);

   }


   DWORD           __fastcall TGKModulesSet::stop (DWORD reason)
   {
     this->set_module_state(MODULE_STATE_STOP_PENDING,true);
     modules_list.lock();
     TGKModuleInterface * mir;
     int i = 0;
     do{
        mir = modules_list[i++];
        if(mir && mir->is_running())
        {
          mir->stop(reason == MODULE_STOP_REQUEST ?MODULE_STOP_OWNER_REQUEST:reason);
        }
       }while(mir);
     modules_list.unlock();
     return TGKModule::stop(reason);
   }


   DWORD           __fastcall TGKModulesSet::pause(DWORD reason,BOOL p)
   {
     modules_list.lock();
     TGKModuleInterface * mir;
     int i = 0;
     do{
        mir = modules_list[i++];
        if(mir)
        {
         mir->pause(reason,p ? true :false);
        }
       }while(mir);
     modules_list.unlock();
     return TGKModule::pause(reason,p) ;
   }
   


