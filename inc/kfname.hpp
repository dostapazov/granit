/*
 Шаблон класса для работы с именем файла(ключем реестра)
 TKFName<char>
 TKFName<wchar_t>
*/

#ifndef  _KFNAME_TMPL_INC
#define  _KFNAME_TMPL_INC

#include <ke_defs.h>
#include <stdlib.h>

#ifndef _NO_NAMESPACE
 namespace KeRTL {
#endif

#ifndef KFNAME_SIZE
#define KFNAME_SIZE MAX_PATH
#endif


inline void __fastcall  split_path(const char *path, char *drive, char *dir, char *name, char *ext)
{  _splitpath(path,drive,dir,name,ext);}

inline void __fastcall  split_path(const wchar_t *path, wchar_t *drive, wchar_t *dir, wchar_t *name, wchar_t *ext)
{  _wsplitpath(path,drive,dir,name,ext);}

inline void __fastcall make_path(char *path, const char *drive, const char *dir, const char *name, const char *ext)
{ _makepath(path,drive,dir,name,ext);}

inline void __fastcall make_path(wchar_t *path, const wchar_t *drive, const wchar_t *dir, const wchar_t *name, const wchar_t *ext)
{ _wmakepath(path,drive,dir,name,ext); }


inline void __fastcall str_cpy(char * dst,const char * src)
{ lstrcpyA(dst,src);}

inline void __fastcall str_cpy(wchar_t * dst,const wchar_t * src)
{ lstrcpyW(dst,src);}

inline void __fastcall str_cat (char * dst,const char * src)
{ lstrcatA(dst,src);}

inline void __fastcall str_cat (wchar_t * dst,const wchar_t * src)
{ lstrcatW(dst,src);}

inline int  __fastcall str_len (const char * s)
{ return lstrlenA(s);}

inline int  __fastcall str_len (const wchar_t * s)
{ return lstrlenW(s);}

template<typename C>
class TKFName
{
  protected:
  C full_name[(KFNAME_SIZE<<2)];
  C drive    [KFNAME_SIZE];
  C path     [(KFNAME_SIZE<<2)];
  C name     [(KFNAME_SIZE<<2)];
  C ext      [KFNAME_SIZE];
  public:
  TKFName(C * _name){set_fullname(_name);}
  TKFName(){*full_name = *drive = *path =* name = *ext = 0; }
  const   C *  operator()(){return full_name;}
  const   C * get_drive() {return drive;}
  const   C * get_path () {return path ;}
  const   C * get_name () {return name ;}
  const   C * get_ext  () {return ext  ;}

  void   __fastcall set_fullname(const C* _name){if(_name)str_cpy(full_name,_name);split_path(full_name,drive,path,name,ext);};

  void   __fastcall set_drive(const C * _drive){if(_drive)str_cpy(drive,_drive);make_path(full_name,drive,path,name,ext);};
  void   __fastcall set_path (const C * _path) {if(_path )str_cpy(path ,_path );make_path(full_name,drive,path,name,ext);};
  void   __fastcall set_name (const C * _name) {if(_name )str_cpy(name ,_name );make_path(full_name,drive,path,name,ext);};
  void   __fastcall set_ext  (const C * _ext ) {if(_ext  )str_cpy(ext  ,_ext  );make_path(full_name,drive,path,name,ext);};
  void   __fastcall add_path (const C * _path);
};

  template <typename C>
  void   __fastcall TKFName<C>::add_path (const C * _path)
  {
   if(_path)
    {
      str_cat(path,_path);
      C * text = path+str_len(path);
      text[0]=C('\\');
      text[1]=0;
      make_path(full_name,drive,path,name,ext);
    } 
  }

inline bool __fastcall is_file_exist( const char* fname)
{
   return GetFileAttributesA(fname) == DWORD(-1) ? false:true;
}

inline bool __fastcall is_file_exist( const wchar_t* fname)
{
   return GetFileAttributesW(fname) == DWORD(-1) ? false:true;
}

inline char * int2str(int v,const char * str)
{
 return itoa(v,(char*)str,10);
}

inline wchar_t * int2str(int v,const wchar_t * str)
{
 return ::_itow(v,(wchar_t*)str,10);
}

struct check_file_exist
{
 bool operator()(const char * name)   {return is_file_exist(name);}
 bool operator()(const wchar_t * name){return is_file_exist(name);}
};

//функция получения уникального имени файла по шаблону
//к имени файла добавляется номера от 1 - до varcount
//результат коприруется в output

template <typename C,typename Test>
bool __fastcall get_uni_filename(C * templ,C * output,int out_len, Test & tst,DWORD varcount = 256)
{
  TKFName<C> fname(templ);
  DWORD num(0);
  DWORD len = str_len(fname.get_name());
  while(num++ < varcount && tst(fname()))
  {

     int2str( int(num),(C*)(fname.get_name())+len);
     fname.set_name(0);
  }
  if(num<varcount && str_len(fname())<out_len && output)
  {
     str_cpy(output,fname());
     return true;
  }
  return false;
}

template <typename C>
void    __fastcall   make_folder_path(C * path)
{
 int len = str_len(path);
 if(len && path[len-1] != C('\\'))
    {path[len++] = C('\\');path[len]=0;}
}


#ifndef _NO_NAMESPACE
}// namespace KeRTL {
#endif

#endif