//---------------------------------------------------------------------------

#pragma hdrstop

#include <TIndexedFile.hpp>
#include <stdio.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)

TIndexedFile::TIndexedFile(bool uids)
{
  hdr_sz=IndFileHeaderSize;
  sec_hdr=0;
  hdr_pos=0;
  num_rec=0;
  unused_size=0;
  indexed=uids;
  uid=0;
  opened=false;
  f_name=0;
  read_only=0;
}

TIndexedFile::TIndexedFile(wchar_t * file_name, bool uids, bool ro)
{
  hdr_sz=IndFileHeaderSize;
  sec_hdr=0;
  hdr_pos=0;
  num_rec=0;
  unused_size=0;
  uid=0;
  opened=false;
  f_name=0;
  open(file_name,uids,ro);
}

TIndexedFile::TIndexedFile(char * file_name, bool uids, bool ro)
{
  hdr_sz=IndFileHeaderSize;
  sec_hdr=0;
  hdr_pos=0;
  num_rec=0;
  unused_size=0;
  uid=0;
  opened=false;
  f_name=0;
  open(file_name,uids,ro);
}

TIndexedFile::~TIndexedFile()
{
  close();
  if (f_name)
    delete [] f_name;
}

bool __fastcall TIndexedFile::open()
{
  if (!f_name) return false;
  DWORD cd    = read_only ? OPEN_EXISTING : OPEN_ALWAYS;
  DWORD fs    = FILE_SHARE_READ;
  DWORD acc   = GENERIC_READ;
  if(!read_only)
    acc|=GENERIC_WRITE;
  if (file.Open(f_name,cd,fs,acc))
  {
    opened=Init();
    return opened;
  }
  return false;
}

bool __fastcall TIndexedFile::open(wchar_t * file_name, bool uids, bool ro)
{
  indexed=uids;
  read_only=ro;
  if (file_name)
  {
    if (f_name) delete [] f_name;
    f_name=new wchar_t[wcslen(file_name)+1];
    safe_strcpy(f_name,file_name);
  }
  DWORD cd    = ro ? OPEN_EXISTING : OPEN_ALWAYS;
  DWORD fs    = FILE_SHARE_READ;
  DWORD acc   = GENERIC_READ;
  if(!ro)
    acc|=GENERIC_WRITE;
  if (file.Open(file_name,cd,fs,acc))
  {
    opened=Init();
    return opened;
  }
  return false;
}

bool __fastcall TIndexedFile::open  ( char   * file_name, bool uids, bool ro)
{
  indexed=uids;
  read_only=ro;
  if (file_name)
  {
    if (f_name) delete [] f_name;
    f_name=new wchar_t[strlen(file_name)+1];
    Ansi2Unicode(f_name,file_name);
  }
  DWORD cd    = ro ? OPEN_EXISTING : OPEN_ALWAYS;
  DWORD fs    = FILE_SHARE_READ;
  DWORD acc   = GENERIC_READ;
  if(!ro)
    acc|=GENERIC_WRITE;
  if (file.Open(file_name,cd,fs,acc))
  {
    opened=Init();
    return opened;
  }
  return false;
}

bool __fastcall TIndexedFile::create(wchar_t * file_name, bool uids)
{
  indexed=uids;
  read_only=false;
  if (file_name)
  {
    if (f_name) delete [] f_name;
    f_name=new wchar_t[wcslen(file_name)+1];
    safe_strcpy(f_name,file_name);
  }
  DWORD cd    = CREATE_ALWAYS;
  DWORD fs    = FILE_SHARE_READ;
  DWORD acc   = GENERIC_READ|GENERIC_WRITE;
  if (file.Open(file_name,cd,fs,acc))
  {
    opened=Init();
    return opened;
  }
  return false;
}

bool __fastcall TIndexedFile::create( char   * file_name, bool uids)
{
  indexed=uids;
  read_only=false;
  if (file_name)
  {
    if (f_name) delete [] f_name;
    f_name=new wchar_t[strlen(file_name)+1];
    Ansi2Unicode(f_name,file_name);
  }
  DWORD cd    = CREATE_ALWAYS;
  DWORD fs    = FILE_SHARE_READ;
  DWORD acc   = GENERIC_READ|GENERIC_WRITE;
  if (file.Open(file_name,cd,fs,acc))
  {
    opened=Init();
    return opened;
  }
  return false;
}

void __fastcall TIndexedFile::close()
{
  TIndFileHdr hdr;
  DWORD dw;
  file.SeekRd(0,FILE_BEGIN);
  if (!file.Read(&hdr,sizeof(hdr),dw))
  {
    ZeroMemory(&hdr,sizeof(hdr));
    hdr.size=sizeof(hdr);
    hdr.sig=IndFileSignature;
    hdr.int_ver=IndFileVersion;
    hdr.uid=uid;
  }
  hdr.num_rec=num_rec;
  hdr.unused_size=unused_size;
  hdr.index_save=file.GetSize();
  hdr.closed=true;
  file.SeekWr(0,FILE_BEGIN);
  file.Write(&hdr,hdr.size);
  file.SeekWr(0,FILE_END);
  DWORD d1,d2;
  if (indexed)
  {
//    file.Write(id_keys.begin(),sizeof(TInd_value)*id_keys.size());
    for (TInd_iter p=id_keys.begin(); p!=id_keys.end(); p++)
    {
      d1=(*p).first;
      d2=(*p).second;
      file.Write(&d1,sizeof(d1));
      file.Write(&d2,sizeof(d2));
    }
    id_keys.clear();
  }
  file.Flush();
  file.Close();
  if(sec_hdr) delete [] sec_hdr;
  sec_hdr=0;
  hdr_pos=0;
  num_rec=0;
  unused_size=0;
  uid=0;
  opened=false;
}

void __fastcall TIndexedFile::flush()
{
  if (opened) file.Flush();
}

//пишем в первое подходящее место,
//возвращает номер записи или -1
DWORD __fastcall TIndexedFile::write(void * data, DWORD size)
{
  if (!opened) return -1;
  return write(DWORD(-1),data,size,false);
}

//1. пишем по номеру записи, если хватает места - переписываем, иначе стираем запись
//2. ищем подходящую по размеру дырку и пишем в неё
//3. пишем в конец
//возвр. номер записи или -1 - ошибка
DWORD __fastcall TIndexedFile::write(DWORD num, void * data, DWORD size, bool by_index)
{
  DWORD ret=-1;
  if (!opened) return ret;
  DWORD ind;
  TInd_iter p;
  bool old=false;
  ind=num;
  if (indexed)
  {
	if (size<sizeof(DWORD)*2) return ret;
	if (by_index)
	{
	  p=id_keys.find(num);
	  if (p==id_keys.end()) return ret;
	  if (num!= *((DWORD *)data+1)) return ret;
	  old=true;
	} else
	{
	  if (num<id_keys.size())
	  {
		p=id_keys.begin();
		for (DWORD i=0; i<num; i++) p++;
		old=true;
	  }
	}
	if (old)
	  ind=(*p).second;
  }
  DWORD pos, dw, value, i, k;
  if (ind<DWORD(-1))                    //пробуем записать на старый индекс
  {
	pos=GetPos(ind);
	if (pos && pos<DWORD(-1))
	{
	  file.SeekRd(pos,FILE_BEGIN);
	  if (!file.Read(&value,sizeof(DWORD),dw) || dw!=sizeof(DWORD)) return ret;
	  if (size<=value)                    //пишем по старому индексу
	  {
		if(indexed && !old)
		  memcpy((DWORD *)data+1,&uid,sizeof(DWORD));
		file.SeekWr(pos,FILE_BEGIN);
		if (!file.Write(data,size)) return ret;
		unused_size+=value-size;
		if(indexed && !old)
		  IncUID(ind);
		return ind;
	  } else                              //удаляем старый индекс
		if (erase(num,by_index)==DWORD(-1)) return ret;
	}
  }
  if (unused_size>=size)                //ищем дырку
  {
	k=0;
	pos=GetPos(k);
	if (pos==DWORD(-1)) return ret;
	DWORD ini, fin;
	while (true)
	{
	  ini=0;
	  for (i=0; i<hdr_sz-1; i++)
		if (!sec_hdr[i])
		{
		  if (i)
		  {
			pos=sec_hdr[i-1];
			file.SeekRd(pos,FILE_BEGIN);
			if (!file.Read(&value,sizeof(DWORD),dw) || dw!=sizeof(DWORD)) return ret;
			ini=pos+value;
		  }else
			ini=hdr_pos+hdr_sz*sizeof(DWORD);
		  fin=i+1;
		  while (!sec_hdr[fin] && fin<hdr_sz-1)
			fin++;
		  if (!sec_hdr[fin] && fin==hdr_sz-1)
			fin=DWORD(-1);
		  else
			fin=sec_hdr[fin];
		  if (size<=(fin-ini))
		  {
			file.SeekWr(hdr_pos+i*sizeof(DWORD),FILE_BEGIN);
			if (file.Write(&ini,sizeof(DWORD)))
			{
			  if(indexed && !old)
				memcpy((DWORD *)data+1,&uid,sizeof(DWORD));
			  file.SeekWr(ini,FILE_BEGIN);
			  if (file.Write(data,size))
			  {
				num_rec++;
				unused_size-=size;
				if(indexed)
				{
				  if (old)
					id_keys.insert(TInd_value(*((DWORD *)data+1),k+i));
				  else
					IncUID(k+i);
				}
				return k+i;
			  }
			}
			return ret;
		  }
		}
	  if (sec_hdr[hdr_sz-1])
	  {
		k+=hdr_sz-1;
		pos=GetPos(k);
		if (pos==DWORD(-1)) return ret;
	  } else
		break;
	}
  }
  k=0;                                      //пишем в конец
  pos=GetPos(k);

  if (pos==DWORD(-1)) return ret;

  while ((pos=sec_hdr[hdr_sz-1])!=0)

  {
	k+=hdr_sz-1;
	pos=GetPos(k);
	if (pos==DWORD(-1)) return ret;
  }
  for (i=hdr_sz-2; i>=0; i--)
	if (sec_hdr[i]) break;
  i++;
  pos=hdr_pos+i*sizeof(DWORD);
  value=file.GetSize();
  file.SeekWr(pos,FILE_BEGIN);
  if (!file.Write(&value,sizeof(DWORD))) return ret;
  if(indexed && !old)
    memcpy((DWORD *)data+1,&uid,sizeof(DWORD));
  file.SeekWr(0,FILE_END);
  if (!file.Write(data,size)) return ret;
  num_rec++;
  if(indexed)
  {
    if (old)
      id_keys.insert(TInd_value(*((DWORD *)data+1),k+i));
    else
      IncUID(k+i);
  }
  ret=k+i;
  if (i==(hdr_sz-2))
  {
    pos=hdr_pos+(hdr_sz-1)*sizeof(DWORD);
    value=file.GetSize();
    file.SeekWr(pos,FILE_BEGIN);
    if (file.Write(&value,sizeof(DWORD)))
    {
      ZeroMemory(sec_hdr,hdr_sz*sizeof(DWORD));
      hdr_pos=value;
      file.SeekWr(hdr_pos,FILE_BEGIN);
      if (file.Write(sec_hdr,hdr_sz*sizeof(DWORD)))
        return ret;
    }
    return -1;
  }
  return ret;
}

//если buffer==0, только определяем необходимый размер
//возвр. размер прочитанных или необходимых данных
//0 - запись удалена, -1 - ошибка, -2 - не хватило места под запись
DWORD __fastcall TIndexedFile::read(DWORD num, void * buffer, DWORD sz, bool by_index)
{
  DWORD ret=-1;
  if (!opened) return ret;
  DWORD ind;
  TInd_iter p;
  if (indexed)
  {
    if (by_index)
    {
      p=id_keys.find(num);
      if (p==id_keys.end()) return ret;
    } else
    {
      if (num>=id_keys.size()) return ret;
      p=id_keys.begin();
      for (DWORD i=0; i<num; i++) p++;
    }
    ind=(*p).second;
  } else
    ind=num;
  DWORD pos,dw,need_sz;
  pos=GetPos(ind);
  if (pos==DWORD(-1))
    return ret;
  if (!pos)
    return 0;
  file.SeekRd(pos,FILE_BEGIN);
  if(!file.Read(&need_sz,sizeof(DWORD),dw) || dw!=sizeof(DWORD))
    return ret;
  if (buffer)           //читаем запись
  {
    if (sz<need_sz)
    {
      ret=-2;
      need_sz=sz;
    }
    safe_memcpy(buffer,&need_sz,sizeof(DWORD));
    if(!file.Read((byte*)buffer+sizeof(DWORD),need_sz-sizeof(DWORD),dw) || (dw+sizeof(DWORD))!=need_sz)
      return -1;
    if (ret!=DWORD(-2))
      ret=dw+sizeof(DWORD);
  }else                 //определяем размер под запись
    ret=need_sz;
  return ret;
}

//возвр. 0 - отлично, -1 - ошибка
DWORD __fastcall TIndexedFile::erase(DWORD num, bool by_index)
{
  DWORD ret=-1;
  if (!opened) return ret;
  DWORD ind;
  TInd_iter p;
  bool old=false;
  if (indexed)
  {
    if (by_index)
    {
      p=id_keys.find(num);
      if (p==id_keys.end()) return ret;
    } else
    {
      if (num>=id_keys.size()) return ret;
      p=id_keys.begin();
      for (DWORD i=0; i<num; i++) p++;
    }
    old=true;
    ind=(*p).second;
  } else
    ind=num;
  DWORD pos,dw,insec;
  pos=GetPos(ind);
  if (pos==DWORD(-1)) return ret;
  insec=ind%(hdr_sz-1);
  pos=sec_hdr[insec];
  if (pos)
  {
    DWORD value=0;
    file.SeekWr(hdr_pos+insec*sizeof(DWORD),FILE_BEGIN);
    if (!file.Write(&value,sizeof(DWORD)))
      return ret;
    file.SeekRd(pos,FILE_BEGIN);
    if (!file.Read(&value,sizeof(DWORD),dw) || dw!=sizeof(DWORD))
    {
      ReCount();
      return ret;
    }
    num_rec--;
    unused_size+=value;
    if (old)
      id_keys.erase(p);
    ret=ERROR_SUCCESS;
  }else
    return ERROR_SUCCESS;
  return ret;
}

//возвр. 0 - отлично, -1 - ошибка
DWORD __fastcall TIndexedFile::clear()
{
  DWORD ret=-1;
  if (!opened) return ret;
  ZeroMemory(sec_hdr,hdr_sz*sizeof(DWORD));
  file.SetSize(sizeof(TIndFileHdr)+hdr_sz*sizeof(DWORD));
  file.SeekWr(sizeof(TIndFileHdr),FILE_BEGIN);
  if (!file.Write(sec_hdr,hdr_sz*sizeof(DWORD))) return ret;
  if (indexed)
    id_keys.clear();
  num_rec=0;
  unused_size=file.GetSize()-sizeof(TIndFileHdr)-hdr_sz*sizeof(DWORD);
  return 0;
}

DWORD __fastcall TIndexedFile::get_count()
{
  if (!opened) return -1;
  return num_rec;
}

DWORD __fastcall TIndexedFile::get_size()
{
  if (!opened) return -1;
  return file.GetSize();
}

DWORD __fastcall TIndexedFile::get_unused_size()
{
  if (!opened) return -1;
  return unused_size;
}

//сжимаем пустоты
bool __fastcall TIndexedFile::shrink()
{
  if (!opened) return false;
  if (!unused_size) return true;
  TIndFileHdr hdr;
  DWORD old_uid, dw;
  file.SeekRd(0,FILE_BEGIN);
  file.Read(&hdr,sizeof(hdr),dw);
  old_uid=hdr.uid;
  char *tempname;
  if ((tempname=tempnam(0,0))==NULL) return false;
  TIndexedFile *tmp=new TIndexedFile(tempname,false);
  TFile *tmp1=0;
  if (!tmp) return false;
  byte *buf;
  DWORD buf_size=64*1024;
  buf=new byte[buf_size];
  DWORD i=0;
  while ((dw=read(i,0,0,false))<DWORD(-1))
  {
    if (dw>buf_size)
    {
      delete [] buf;
      buf_size=dw;
      buf=new byte[buf_size];
    }
    if (dw)
    {
      dw=read(i,buf,buf_size,false);
      if (dw==DWORD(-1)) goto bad;
      if (tmp->write(buf,dw)==DWORD(-1)) goto bad;
    }
    i++;
  }
  delete tmp;
  tmp=0;
  tmp1= new TFile(tempname,OPEN_EXISTING,FILE_SHARE_READ,GENERIC_READ);
  if (!tmp1) goto bad;
  file.SetSize(0);
  file.Flush();
  file.SeekWr(0,FILE_BEGIN);
  while (tmp1->Read(buf,buf_size,dw) && dw)
    file.Write(buf,dw);
  delete tmp1;
  delete [] buf;
  file.SeekRd(0,FILE_BEGIN);
  file.Read(&hdr,sizeof(hdr),dw);
  hdr.with_index=indexed;
  hdr.uid=old_uid;
  hdr.closed=false;
  file.SeekWr(0,FILE_BEGIN);
  file.Write(&hdr,sizeof(hdr));
  ReCount();
  remove(tempname);
  free(tempname);
  return true;
bad:
  delete [] buf;
  if (tmp) delete tmp;
  if (tmp1) delete tmp1;
  free(tempname);
  return false;
}

//пересчитать кол-во записей и неиспользуемое пространство
void __fastcall TIndexedFile::ReCount()
{
  DWORD pos, dw, k, i, value, prev, file_size=file.GetSize();
  pos=sizeof(TIndFileHdr);
  num_rec=0;
  unused_size=0;
  prev=pos;
  if (indexed)
    id_keys.clear();
  k=0;
  while (pos)
  {
    unused_size+=pos-prev;
    file.SeekRd(pos,FILE_BEGIN);
    if (!file.Read(sec_hdr,hdr_sz*sizeof(DWORD),dw) || dw!=hdr_sz*sizeof(DWORD)) return;
    prev=pos+dw;
    for (i=0; i<hdr_sz-1; i++)
    {
      if (sec_hdr[i]>file_size) return;
      file.SeekRd(sec_hdr[i],FILE_BEGIN);
      if (!file.Read(&value,sizeof(DWORD),dw) || dw!=sizeof(DWORD)) return;
      if (sec_hdr[i])
      {
        num_rec++;
        unused_size+=sec_hdr[i]-prev;
        prev=sec_hdr[i]+value;
        if (indexed)
        {
          if (!file.Read(&value,sizeof(DWORD),dw) || dw!=sizeof(DWORD)) return;
          id_keys.insert(TInd_value(value,k+i));
        }
      }
    }
    pos=sec_hdr[hdr_sz-1];
    k+=hdr_sz-1;
  }
  if (indexed)
    file.SetSize(prev);
  unused_size+=file.GetSize()-prev;
}

bool _fastcall TIndexedFile::Init()
{
  bool ret=false;
  sec_hdr=new DWORD[hdr_sz];
  ZeroMemory(sec_hdr,hdr_sz*sizeof(DWORD));
  TIndFileHdr hdr;
  DWORD dw;
  if(file.GetSize())
  {
    if(file.Read(&hdr,sizeof(hdr),dw) && dw==sizeof(hdr))
    {
      if (hdr.sig==IndFileSignature)
      {
        int v=CheckVersion(hdr.int_ver);
        if (v>=0)                                    //проверка прошла
        {
          if (v>0)                                   //перечитаем заголовок
          {
            file.SeekRd(0,FILE_BEGIN);
            file.Read(&hdr,sizeof(hdr),dw);
          }
          indexed=hdr.with_index;
          uid=hdr.uid;
          if(hdr.closed)
          {
            num_rec=hdr.num_rec;
            unused_size=hdr.unused_size;
            if (indexed)
            {
              if (hdr.index_save)
              {
                DWORD d1,d2;
                file.SeekRd(hdr.index_save,FILE_BEGIN);
                while (file.Read(&d1,sizeof(d1),dw) && dw==sizeof(d1))
                {
                  if (!file.Read(&d2,sizeof(d2),dw) || dw!=sizeof(d2)) return ret;
                  id_keys.insert(TInd_value(d1,d2));
                }
                file.SetSize(hdr.index_save);
                unused_size+=file.GetSize()-hdr.index_save;
                if (num_rec!=id_keys.size())
                ReCount();
              }else
                ReCount();
            }
          }else
            ReCount();
          dw=(byte *)&(hdr.closed)-(byte *)&hdr;
          file.SeekWr(dw,FILE_BEGIN);
          hdr.closed=false;
          file.Write((byte *)&(hdr.closed),sizeof(hdr.closed));
          ret=true;
        }
      }
    }
  }else
  {
    ZeroMemory(&hdr,sizeof(hdr));
    hdr.size=sizeof(hdr);
    hdr.sig=IndFileSignature;
    hdr.int_ver=IndFileVersion;
    hdr.with_index=indexed;
    ret=file.Write(&hdr,hdr.size);
    if(ret)
    {
      ZeroMemory(sec_hdr,hdr_sz*sizeof(DWORD));
      ret=file.Write(sec_hdr,hdr_sz*sizeof(DWORD));
    }
  }
  return ret;
}

// возврат 0 - всё хорошо
//         1 - перечитать заголовок после преобразования
//        -1 - преобразование невозможно
int _fastcall TIndexedFile::CheckVersion(DWORD ver)
{
  if(ver==IndFileVersion) return 0;
  if (ver==0)
  {
    TIndFileHdr hdr;
    hdr.int_ver=IndFileVersion;
    file.SeekWr((byte *)&(hdr.int_ver)-(byte *)&hdr,FILE_BEGIN);
    if (file.Write((byte *)&(hdr.int_ver),sizeof(hdr.int_ver)))
      return 0;
  }
  // можно попытаться преобразовать до текущей версии
  return -1;
}

//получаем позицию в файле по номеру записи
DWORD __fastcall TIndexedFile::GetPos(DWORD ind)
{
  DWORD ret=-1;
  DWORD sec,insec,pos,dw;
  sec=ind/(hdr_sz-1)+1;
  insec=ind%(hdr_sz-1);
  pos=sizeof(TIndFileHdr);
  for (int i=0; i<(int)sec; i++)
  {
    if(pos)
      hdr_pos=pos;
    else
      return ret;
    file.SeekRd(hdr_pos,FILE_BEGIN);
    if ((file.Read(sec_hdr,hdr_sz*sizeof(DWORD),dw) && hdr_sz*sizeof(DWORD)==dw))
      pos=sec_hdr[hdr_sz-1];
    else
      return ret;
  }
  ret=sec_hdr[insec];
  return ret;
}

void __fastcall TIndexedFile::IncUID(DWORD num)
{
  TIndFileHdr hdr;
  if (indexed)
  {
    DWORD old=uid;
    uid++;
    if (id_keys.count(uid))
    {
      //прошли по кругу все id
      //нужна умная обработка
    }
    DWORD dw=(byte *)&(hdr.uid)-(byte *)&hdr;
    file.SeekWr(dw,FILE_BEGIN);
    if (!file.Write(&uid,sizeof(uid)))
      uid=old;
    else
      id_keys.insert(TInd_value(old,num));
  }
}

int __fastcall TIndexedFile::get_file_name(wchar_t *file_name, int sz)
{
  if (!f_name) return 0;
  int len=wcslen(f_name)+1;
  if (!file_name) return len;
  if (len>sz) return -len;
  safe_strcpy(file_name,f_name);
  return len;
}

int __fastcall TIndexedFile::get_file_name(char *file_name, int sz)
{
  if (!f_name) return 0;
  int len=wcslen(f_name)+1;
  if (!file_name) return len;
  if (len>sz) return -len;
  char *str=new char[len];
  Unicode2Ansi(str,f_name);
  safe_strcpy(file_name,str);
  delete [] str;
  return len;
}

DWORD __fastcall TIndexedFile::get_ext_version()
{
  DWORD ret=0;
  if (opened)
  {
    TIndFileHdr hdr;
    file.SeekRd(0,FILE_BEGIN);
    DWORD dw;
    if (file.Read(&hdr,sizeof(hdr),dw) && dw==sizeof(hdr))
    {
      ret=hdr.ext_ver;
    }
  }
  return ret;
}

bool __fastcall TIndexedFile::set_ext_version(DWORD ver)
{
  bool ret=false;
  if (opened)
  {
    TIndFileHdr hdr;
    hdr.ext_ver=ver;
    file.SeekWr((byte *)&(hdr.ext_ver)-(byte *)&hdr,FILE_BEGIN);
    ret=file.Write((byte *)&(hdr.ext_ver),sizeof(hdr.ext_ver));
  }
  return ret;
}


