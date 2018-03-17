//---------------------------------------------------------------------------

#pragma hdrstop

#include "scale.hpp"
#include <tregstry.hpp>
#include <stdio.h>


TGKScale::TGKScale():TGKModule()
{
  alloc_gkhandle(SCALE_NAME_W);
  lock_param = GKHB_AUTO_LOCK_OFF;
  file_name=NULL;
  file=NULL;
}

TGKScale::~TGKScale()
{
  if (file) delete file;
}

WORD __fastcall TGKScale::get_type()
{
  return MT_SCALE;
}
DWORD __fastcall TGKScale::get_config_data(DWORD mask,LPVOID buf,DWORD bsz)
{
  return 0;
}

bool __fastcall TGKScale::set_config_data(DWORD mask,LPVOID buf,DWORD bsz)
{
  return true;
}

bool __fastcall TGKScale::compare_config_data(DWORD mask,LPVOID buf,DWORD bsz,LPDWORD ch_mask,BOOL * restart)
{
  return true;
}

DWORD __fastcall TGKScale::get_window_module(wchar_t * buf,DWORD bsz)
{
  wchar_t wm_name[]=L"GKSCALE_WND.DLL";
  int len=lstrlenW(wm_name);
  if(buf && int(bsz)>len)
  {
    SetLastError(0);
    lstrcpyW(buf,wm_name);
  }
  else
  {
    SetLastError(MERR_INVALID_BUFFER_SIZE);
    len=-len;
  }
  return DWORD(len);
}

LRESULT __fastcall TGKScale::processing(DWORD cmd,LPARAM p1,LPARAM p2)
{
  LRESULT ret(GKH_RET_ERROR);
  switch(cmd)
  {
    case CM_SC_CALC: ret=cmd_calc((LPSC_CALC)p1,(bool)p2); break;
    case CM_SC_ENUM: ret=cmd_enum((DWORD)p1,(LPSCALE_ENUM)p2); break;
    case CM_SC_SELECT_A: ret=cmd_select((char *)p1,(DWORD *)p2); break;
    case CM_SC_SELECT_W: ret=cmd_select((wchar_t *)p1,(DWORD *)p2); break;
    case CM_SC_COUNT: ret=cmd_count((DWORD *)p1); break;
    case CM_SC_CREATE: ret=cmd_create((byte *)p1,(DWORD *)p2); break;
    case CM_SC_DELETE: ret=cmd_delete((DWORD)p1); break;
    case CM_SC_SCALE_SIZE: ret=cmd_size((DWORD)p1,(DWORD *)p2); break;
    case CM_SC_SET: ret=cmd_set((DWORD)p1,(byte *)p2); break;
    case CM_SC_GET: ret=cmd_get((DWORD)p1,(byte *)p2); break;
    case CM_SC_GET_FILENAME_A: ret=cmd_get_filename((char *)p1,(int *)p2); break;
    case CM_SC_GET_FILENAME_W: ret=cmd_get_filename((wchar_t *)p1,(int *)p2); break;
    case CM_SC_SET_FILENAME_A: ret=cmd_set_filename((char *)p1,(int)p2); break;
    case CM_SC_SET_FILENAME_W: ret=cmd_set_filename((wchar_t *)p1,(int)p2); break;
    case CM_SC_SHRINK: ret=cmd_shrink(); break;
    case CM_SC_INDEX_USED: ret=cmd_index_used((bool *)p1); break;
    case CM_SC_FILE_SIZE: ret=cmd_file_size((DWORD *)p1); break;
    case CM_SC_FILE_UNUSED: ret=cmd_file_unused((DWORD *)p1); break;
    case CM_SC_MINMAX: ret=cmd_minmax((LPSC_MINMAX)p1); break;
    default:
      ret = TGKModule::processing(cmd,p1,p2);
      break;
  }
  return ret;
}

DWORD __fastcall TGKScale::start(DWORD reason,LPARAM p2)
{
  DWORD ret(GKH_RET_ERROR);
  if(file_name)
  {
    if(!file)
    {
      file=new TIndexedFile(true);
      if (!file->open(file_name,true,false))
      {
        report(EVENT_SYSTEM,REPORT_ERROR_TYPE,"Не удаётся открыть файл шкал");
        delete file;
        file=NULL;
      }
    }
    if(file)
    {
      if (check_version())
      {
        notify(MNF_SCALE,MNCODE_SCALE_REFRESH,NULL,0);
        ret=TGKModule::start(reason,p2);
      } else
      {
        report(EVENT_SYSTEM,REPORT_ERROR_TYPE,"Неизвестный формат данных файла шкал");
        delete file;
        file=NULL;
      }
    }
  }
  return ret;
}

DWORD __fastcall TGKScale::stop (DWORD reason)
{
  DWORD ret=TGKModule::stop(reason);
  if(file)
  {
    file->close();
    delete file;
    file=0;
    notify(MNF_SCALE,MNCODE_SCALE_REFRESH,NULL,0);
  }
  return ret;
}

bool __fastcall TGKScale::read_settings()
{
  TRegsReader rr(DupKey(reg_key));
  wchar_t reg_name[MAX_PATH];
  int sz;
  sz=rr.ReadString(L"File name",reg_name,MAX_PATH,true);
  if (sz)
  {
    file_name=new wchar_t[sz];
    safe_strcpy(file_name,reg_name);
  }
  return TGKModule::read_settings();
}

bool __fastcall TGKScale::save_file_name()
{
  TRegsWriter rw(DupKey(reg_key));
  return rw.WriteString(L"File name",file_name,true);
}

DWORD __fastcall TGKScale::full_unlock()
{
  locker.FullUnlock();
  return TGKModule::full_unlock();
}

bool __fastcall TGKScale::check_version()
{
  bool ret=false;
  if (file)
  {
    DWORD dw=file->get_ext_version();
    if (dw==0)
    {
      ret=file->set_ext_version(SCALE_DATA_VERSION);
    } else if (dw==1)
    {
      char *tempname;
      if ((tempname=tempnam(0,0))!=NULL && cmd_set_filename(tempname,1)==GKH_RET_SUCCESS)
      {
        TIndexedFile *tmp=new TIndexedFile(tempname,true,false);
        TScale scl;
        if (tmp)
        {
          byte *buf;
          DWORD buf_size=4*1024;
          buf=new byte[buf_size];
          DWORD i=0;
          while ((dw=tmp->read(i,0,0,false))<DWORD(-1))
          {
            if (dw>buf_size)
            {
              delete [] buf;
              buf_size=dw;
              buf=new byte[buf_size];
            }
            if (dw)
            {
              dw=tmp->read(i,buf,buf_size,false);
              if (dw<DWORD(-2))
              {
                scl.SetAll_v1(buf);
                dw=scl.GetBuffer(0,0);
                if (dw>buf_size)
                {
                  delete [] buf;
                  buf_size=dw;
                  buf=new byte[buf_size];
                }
                scl.GetBuffer(buf,buf_size);
                file->write(scl.GetId(),buf,buf_size,true);
              }
            }
            i++;
          }
          delete tmp;
          std::free(tempname);
          delete [] buf;
          file->shrink();
          file->set_ext_version(SCALE_DATA_VERSION);
          report(EVENT_SYSTEM,REPORT_INFORMATION_TYPE,"Файл шкал успешно конвертирован в новую версию");
          ret=true;
        }
      }
    } else if (dw==2)
    {
      ret=true;
    }
  }
  return ret;
}

TScale * __fastcall TGKScale::get_scale(DWORD num, DWORD size, bool by_index)
{
  TScale *scl=0;
  if (!file) return 0;
  byte *buf=0;
  DWORD dw;
  locker.Lock();
  dw=file->read(num,0,0,by_index);
  if (dw && dw<DWORD(-1))
  {
    buf=new byte[dw];
    if (!size) size=dw;
    dw=file->read(num,buf,size,by_index);
  }
  locker.Unlock();
  if (dw<DWORD(-1)) scl=new TScale(buf,size);
  if (buf) delete [] buf;
  return scl;
}

DWORD __fastcall TGKScale::cmd_calc(LPSC_CALC c, bool direct)
{
  DWORD ret=GKH_RET_ERROR;
  TScale *scl;
  scl=get_scale(c->id,0,true);
  if (!scl) return ret;
  c->error=SCL_ERR_SUCCESS;
  if (direct)
    c->error=scl->Calc(c->kvant,c->result);
  else
    c->error=scl->ReverseCalc(c->result,c->kvant);
  delete scl;
  return GKH_RET_SUCCESS;
}

DWORD __fastcall TGKScale::cmd_enum(DWORD num, LPSCALE_ENUM enum_par)
{
  DWORD ret=GKH_RET_ERROR;
  if (!file) return ret;
  if(num>=file->get_count()) return ret;
  DWORD dw=enum_par->dw_size;
  byte *buf=new byte[dw];
  locker.Lock();
  dw=file->read(num,buf,dw,false);
  locker.Unlock();
  if (dw<DWORD(-1))
  {
    ZeroMemory(buf+sizeof(*enum_par)-sizeof(wchar_t),sizeof(wchar_t));
    enum_par->scale_id=*(DWORD *)(buf+sizeof(DWORD));
    safe_strcpy(enum_par->name,(wchar_t *)(buf+sizeof(DWORD)*2));
    ret=GKH_RET_SUCCESS;
  }
  delete [] buf;
  return ret;
}

DWORD __fastcall TGKScale::cmd_select(wchar_t * name, DWORD * id)
{
  DWORD ret=GKH_RET_ERROR;
  if (!file) return ret;
  const buf_sz=255;
  byte buf[buf_sz];
  DWORD dw, count;
  TScale scl;
  count=file->get_count();
  for (DWORD i=0; i<count; i++)
  {
    locker.Lock();
    dw=file->read(i,buf,buf_sz,false);
    locker.Unlock();
    if (dw==DWORD(-2))
    {
      buf[buf_sz-2]=0;
      buf[buf_sz-1]=0;
    }
    if (dw<DWORD(-1))
    {
      scl.SetAll(buf,buf_sz);
      if (!wcscmp(name,scl.w_str()))
      {
        *id=scl.GetId();
        return GKH_RET_SUCCESS;
      }
    }
  }
  return ret;
}

DWORD __fastcall TGKScale::cmd_select(char * name, DWORD * id)
{
  wchar_t *str;
  str=new wchar_t[strlen(name)+1];
  Ansi2Unicode(str,name);
  DWORD ret=cmd_select(str,id);
  delete [] str;
  return ret;
}

DWORD __fastcall TGKScale::cmd_count(DWORD * c)
{
  DWORD ret=GKH_RET_ERROR;
  if (!file) return ret;
  *c=file->get_count();
  if (*c<DWORD(-1)) ret=GKH_RET_SUCCESS;
  return ret;
}

DWORD __fastcall TGKScale::cmd_create(byte * buf, DWORD * id)
{
  DWORD ret=GKH_RET_ERROR;
  if (!file) return ret;
  DWORD dw;
  dw=*(DWORD *)buf;
  locker.Lock();
  dw=file->write(buf,dw);
  locker.Unlock();
  if (dw<DWORD(-1))
  {
    *id=*(DWORD *)(buf+sizeof(DWORD));
    SC_NOTIFY_DATA sc_data;
    sc_data.id=*id;
    sc_data.reason=MNCODE_SCALE_CREATE;
    notify(MNF_SCALE,MNCODE_SCALE_ID,&sc_data,sizeof(sc_data));
    ret=GKH_RET_SUCCESS;
  }
  return ret;
}

DWORD __fastcall TGKScale::cmd_delete(DWORD id)
{
  DWORD ret=GKH_RET_ERROR;
  if (!file) return ret;
  DWORD dw;
  locker.Lock();
  dw=file->erase(id,true);
  locker.Unlock();
  if (dw<DWORD(-1))
  {
    SC_NOTIFY_DATA sc_data;
    sc_data.id=id;
    sc_data.reason=MNCODE_SCALE_DELETE;
    notify(MNF_SCALE,MNCODE_SCALE_ID,&sc_data,sizeof(sc_data));
    ret=GKH_RET_SUCCESS;
  }
  return ret;
}

DWORD __fastcall TGKScale::cmd_size(DWORD id, DWORD * size)
{
  DWORD ret=GKH_RET_ERROR;
  if (!file) return ret;
  DWORD dw;
  locker.Lock();
  dw=file->read(id,0,0,true);
  locker.Unlock();
  if (dw<DWORD(-1))
  {
    *size=dw;
    ret=GKH_RET_SUCCESS;
  }
  return ret;
}

DWORD __fastcall TGKScale::cmd_set(DWORD id, byte * buf)
{
  DWORD ret=GKH_RET_ERROR;
  if (!file) return ret;
  TScale old_sc, new_sc;
  new_sc.SetAll(buf,0);
  DWORD dw;
  dw=*(DWORD *)buf;
  byte *buff=0;
  locker.Lock();
  dw=file->read(id,0,0,true);
  buff=new byte[dw];
  dw=file->read(id,buff,dw,true);
  dw=file->write(id,buf,dw,true);
  locker.Unlock();
  if (buff)
  {
    old_sc.SetAll(buff,0);
    delete [] buff;
  }
  if (dw<DWORD(-1))
  {
    SC_NOTIFY_DATA sc_data;
    sc_data.id=id;
    sc_data.reason=0;
    if (old_sc.w_str()!=new_sc.w_str()) sc_data.reason|=MNCODE_SCALE_NAME;
    if (old_sc.warn_min!=new_sc.warn_min) sc_data.reason|=MNCODE_SCALE_LIMITS;
    if (old_sc.warn_max!=new_sc.warn_max) sc_data.reason|=MNCODE_SCALE_LIMITS;
    if (old_sc.alarm_min!=new_sc.alarm_min) sc_data.reason|=MNCODE_SCALE_LIMITS;
    if (old_sc.alarm_max!=new_sc.alarm_max) sc_data.reason|=MNCODE_SCALE_LIMITS;
    if (old_sc.limits!=new_sc.limits) sc_data.reason|=MNCODE_SCALE_LIMITS;
    TSegment *seg1,*seg2;
    int i=0;
    do
    {
      seg1=old_sc.GetSegment(i);
      seg2=new_sc.GetSegment(i);
      if (seg1 && seg2 && !((*seg1)==(*seg2))) sc_data.reason|=MNCODE_SCALE_SEGMENT;
      i++;
    } while (seg1 && seg2);
    notify(MNF_SCALE,MNCODE_SCALE_ID,&sc_data,sizeof(sc_data));
    ret=GKH_RET_SUCCESS;
  }
  return ret;
}

DWORD __fastcall TGKScale::cmd_get(DWORD id, byte * buf)
{
  DWORD ret=GKH_RET_ERROR;
  if (!file) return ret;
  DWORD dw;
  dw=*(DWORD *)buf;
  locker.Lock();
  dw=file->read(id,buf,dw,true);
  locker.Unlock();
  if (dw<DWORD(-1)) ret=GKH_RET_SUCCESS;
  return ret;
}

DWORD __fastcall TGKScale::cmd_get_filename(wchar_t * name, int * size)
{
  DWORD ret=GKH_RET_ERROR;
  if (!name)
  {
    if (file_name)
      *size=wcslen(file_name)+1;
    else
      *size=0;
    return GKH_RET_SUCCESS;
  }
  if (!file_name) return ret;
  safe_strcpy(name,file_name);
  return GKH_RET_SUCCESS;
}

DWORD __fastcall TGKScale::cmd_get_filename(char * name, int * size)
{
  DWORD ret=GKH_RET_ERROR;
  if (!name)
  {
    if (file_name)
      *size=wcslen(file_name)+1;
    else
      *size=0;
    return GKH_RET_SUCCESS;
  }
  if (!file_name) return ret;
  Unicode2Ansi(name,file_name);
  return GKH_RET_SUCCESS;
}

DWORD __fastcall TGKScale::cmd_set_filename(wchar_t * name, int operation)
{
  DWORD ret=GKH_RET_ERROR;
  int er;
  DWORD dw;
  TFile f1,f2;
  int buf_sz=8*1024;
  byte *buf=new byte[buf_sz];
  switch (operation)
  {
    case 0:     //rename
      if (!file_name) return ret;
      if (!wcscmp(name,file_name)) return GKH_RET_SUCCESS;
      if (file)
      {
        locker.Lock();
        file->close();
      }
      er=_wrename(file_name,name);
      if (!er)
      {
        delete [] file_name;
        file_name=new wchar_t[wcslen(name)+1];
        safe_strcpy(file_name,name);
        save_file_name();
        ret=GKH_RET_SUCCESS;
      }
      if (file)
      {
        file->open(file_name,true,false);
        locker.Unlock();
      }
      break;
    case 1:     //copy to
      if (!file_name) return ret;
      if (!wcscmp(name,file_name)) return GKH_RET_SUCCESS;
      if (f2.Open(name,OPEN_EXISTING,FILE_SHARE_READ,GENERIC_READ))
      {
        f2.Close();
        return ret;
      }
      if (!f2.Open(name,CREATE_ALWAYS,FILE_SHARE_READ,GENERIC_READ|GENERIC_WRITE)) return ret;
      if (file)
      {
        locker.Lock();
        file->close();
      }
      if (f1.Open(file_name,OPEN_EXISTING,FILE_SHARE_READ,GENERIC_READ))
      {
        while (f1.Read(buf,buf_sz,dw) && dw)
          f2.Write(buf,dw);
        f1.Close();
        ret=GKH_RET_SUCCESS;
      }
      if (file)
      {
        file->open(file_name,true,false);
        locker.Unlock();
      }
      f2.Close();
      break;
    case 2:     //open other
      if (file)
      {
        locker.Lock();
        file->close();
      }
      if (file_name) delete [] file_name;
      file_name=new wchar_t[wcslen(name)+1];
      safe_strcpy(file_name,name);
      save_file_name();
      if (file)
      {
        file->open(file_name,true,false);
        locker.Unlock();
      }
      ret=GKH_RET_SUCCESS;
      break;
  }
  return ret;
}

DWORD __fastcall TGKScale::cmd_set_filename(char * name, int operation)
{
  wchar_t *str;
  str=new wchar_t[strlen(name)+1];
  Ansi2Unicode(str,name);
  DWORD ret=cmd_set_filename(str,operation);
  delete [] str;
  return ret;
}

DWORD __fastcall TGKScale::cmd_shrink()
{
  DWORD ret=GKH_RET_ERROR;
  if (!file) return ret;
  locker.Lock();
  if (file->shrink()) ret=GKH_RET_SUCCESS;
  locker.Unlock();
  return ret;
}

DWORD __fastcall TGKScale::cmd_index_used(bool * indexed)
{
  DWORD ret=GKH_RET_ERROR;
  if (!file) return ret;
  *indexed=file->index_used();
  ret=GKH_RET_SUCCESS;
  return ret;
}

DWORD __fastcall TGKScale::cmd_file_size(DWORD * sz)
{
  DWORD ret=GKH_RET_ERROR;
  if (!file) return ret;
  *sz=file->get_size();
  if (*sz<DWORD(-1)) ret=GKH_RET_SUCCESS;
  return ret;
}

DWORD __fastcall TGKScale::cmd_file_unused(DWORD * sz)
{
  DWORD ret=GKH_RET_ERROR;
  if (!file) return ret;
  *sz=file->get_unused_size();
  if (*sz<DWORD(-1)) ret=GKH_RET_SUCCESS;
  return ret;
}

DWORD __fastcall TGKScale::cmd_minmax(LPSC_MINMAX mm)
{
  DWORD ret=GKH_RET_ERROR;
  TScale *scl;
  scl=get_scale(mm->id,0,true);
  if (!scl) return ret;
  mm->min_kv=scl->MinKvant();
  mm->max_kv=scl->MaxKvant();
  mm->min_y=scl->MinY();
  mm->max_y=scl->MaxY();
  delete scl;
  return GKH_RET_SUCCESS;
}
