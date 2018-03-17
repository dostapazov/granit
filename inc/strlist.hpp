/*
*/

#ifndef _KERTL_STRINGLIST_HPP
#define _KERTL_STRINGLIST_HPP

#include <vector>
#include <string>
#include <algorithm>

#ifndef _NO_NAMESPACE
namespace KeRTL
{
#endif



using namespace std;

template <typename charT,typename Allocator = allocator<charT> >
class _strings_list
{
 protected:
 typedef  basic_string<charT,char_traits<charT>,Allocator> str_type;
 str_type         line_separator;
 vector<str_type > lines;
 public:
 _strings_list(){};
 _strings_list(charT*   szText,charT * sep_line)   {line_separator = sep_line;set_text(szText);}
 _strings_list(str_type &szText,str_type& sep_line){line_separator = sep_line;set_text(szText);}
 _strings_list(const  _strings_list & sl);
 int  __fastcall set_line_separator(charT*    sep){return set_line_separator(*&str_type(sep));};
 int  __fastcall set_line_separator(str_type& sep);
 int  __fastcall count_lines() const {return lines.size();}
 int  __fastcall get_text(str_type & str,int to=-1,int from = 0);
 int  __fastcall set_text(const charT * szText){clear();return add_text(*&str_type(szText));}
 int  __fastcall set_text(const str_type & str){clear();return add_text(str);}
 int  __fastcall add_text(const charT * szText){return add_text(*&str_type(szText));}
 int  __fastcall add_text(const str_type & str);
 int  __fastcall add_line(const charT    * szText){return add_text(*&str_type(szText));};
 int  __fastcall add_line(const str_type & str);
 int  __fastcall insert_line(int line, const charT    * text){return insert_line(line,*&str_type(text));};
 int  __fastcall insert_line(int line, const str_type & text);
 int  __fastcall insert_lines(int line, _strings_list & text);
 int  __fastcall remove_line (int line);
 void __fastcall clear(){lines.erase(lines.begin(),lines.end());lines.resize(0);};
 int  __fastcall length();
 void __fastcall line_at(int index,str_type & str);
 void __fastcall set_line(int index,const str_type & str);
 void __fastcall set_line(int index,const charT    * str);
 _strings_list & operator  =  (const _strings_list & sl);
 _strings_list & operator +=  (const _strings_list & sl);
 int             operator +=  (const str_type & s);
 int             operator +=  (const charT  *   s);
 str_type&       operator [](int index){return lines[index];}
 void  __fastcall swap(int i1,int i2){swap(lines[i1],lines[i2]);}
 int   __fastcall find(const str_type & str,int from = 0,int to = -1);
 int   __fastcall find(const charT *    str,int from = 0,int to = -1){return find(*&str_type(str),from,to);}
 int   __fastcall find_substr(const str_type & str,int from = 0,int to = -1);
 int   __fastcall find_substr(const charT *    str,int from = 0,int to = -1){return find_substr(*&str_type(str),from,to);}

};

class strings_list:public _strings_list<char>
{
 public:
 strings_list(){line_separator = "\r\n";}
 strings_list(char* szText,char * sep_line =   "\r\n"):_strings_list<char>(szText,sep_line){}
 strings_list(string& text,string& sep_line =  *&string("\r\n")):_strings_list<char>(text,sep_line){}
 strings_list(const  strings_list & sl):_strings_list<char>(sl){}
};

class wstrings_list:public _strings_list<wchar_t>
{
 public:
 wstrings_list(){line_separator = L"\r\n";}
 wstrings_list(wchar_t* szText,wchar_t * sep_line =   L"\r\n"):_strings_list<wchar_t>(szText,sep_line){}
 wstrings_list(wstring& text,wstring& sep_line =  *&wstring(L"\r\n")):_strings_list<wchar_t>(text,sep_line){}
 wstrings_list(const  wstrings_list & sl):_strings_list<wchar_t>(sl){}
};



template <typename charT,typename Allocator = allocator<charT> >
int __fastcall _strings_list<charT,Allocator>::add_text( const str_type & str)
{
 int s_pos = 0,e_pos = 0,sep_len = line_separator.length();
 do{
     e_pos = str.find(line_separator,s_pos);
     lines.push_back(str.substr(s_pos, e_pos-s_pos));
     s_pos = e_pos+sep_len;
   }while(e_pos>0);
 return lines.size();//Возврат кол-ва строк
}

template <typename charT,typename Allocator = allocator<charT> >
int __fastcall _strings_list<charT,Allocator>::add_line(const str_type & str)
{
 return add_text(str);
}

template <typename charT,typename Allocator = allocator<charT> >
int __fastcall _strings_list<charT,Allocator>::set_line_separator(str_type & sep)
{
 str_type text;
 if(line_separator.length() && lines.size())
   get_text(text);
 line_separator = sep;
 if(text.length())
   set_text(text);
   return lines.size();
}

template <typename charT,typename Allocator = allocator<charT> >
int  __fastcall _strings_list<charT,Allocator>::get_text(str_type & str,int to,int from )
{
  str.clear();
  if(to<from)
     to = lines.size()-1;
  for(;from<to;from++)
  {
   str+=lines[from];
   str+=line_separator;
  }
  str+=lines[from];
  return str.length();
}

template <typename charT,typename Allocator = allocator<charT> >
int  __fastcall _strings_list<charT,Allocator>::length()
{
 int ret = 0;
 int ln = count_lines()-1;
 if(ln>=0)
 {
  int i = 0;
  for(;i<ln;i++)
  {
    ret+=(lines[i].length()+line_separator.length());
  }
  ret+=lines[i].length();
 }
 return ret;
}

template <typename charT,typename Allocator = allocator<charT> >
void  __fastcall _strings_list<charT,Allocator>::line_at(int index,str_type & str)
{
 str = index<lines.size() ? lines[index].c_str():(charT*) = 0;
}

template <typename charT,typename Allocator = allocator<charT> >
 void __fastcall _strings_list<charT,Allocator>::set_line(int index,const str_type & str)
 {
  if(index<int(lines.size()))
   {
    lines[index] = str;
   }
   else
   {
    lines.resize(index+1);
    set_line(index,str);
   }
 }

template <typename charT,typename Allocator = allocator<charT> >
inline  void __fastcall _strings_list<charT,Allocator>::set_line(int index,const charT    * str)
 {
  set_line(index,*&str_type(str));
 }

template <typename charT,typename Allocator = allocator<charT> >
inline _strings_list<charT,Allocator> & _strings_list<charT,Allocator>::operator  =  (const _strings_list & sl)
 {
  clear();
  line_separator = sl.line_separator;
  return (*this)+=sl;
 }

template <typename charT,typename Allocator = allocator<charT> >
 _strings_list<charT,Allocator> & _strings_list<charT,Allocator>::operator +=  (const _strings_list & sl)
 {
  for(int i = 0;i<sl.count_lines();i++)
      add_line(sl.lines[i]);
  return *this;
 }

template <typename charT,typename Allocator = allocator<charT> >
inline int             _strings_list<charT,Allocator>::operator +=  (const str_type & s)
{ return add_line(s);}

template <typename charT,typename Allocator = allocator<charT> >
inline int             _strings_list<charT,Allocator>::operator +=  (const charT  *   s)
{ return add_line(s);}

template <typename charT,typename Allocator = allocator<charT> >
 int   __fastcall _strings_list<charT,Allocator>::find(const str_type & str,int from ,int to  )
 {
  to = to<0 ? lines.size():to;
  for(;from<to;from++)
  {
    if(lines[from]==str)
      return from;
  }
  return -1;
 }

template <typename charT,typename Allocator = allocator<charT> >
 int   __fastcall _strings_list<charT,Allocator>::find_substr(const str_type & str,int from ,int to  )
 {
  to = to<0 ? lines.size():to;
  for(;from<to;from++)
  {
    if(int(lines[from].find(str,0))>=0)
      return from;
  }
  return -1;
 }

template <typename charT,typename Allocator = allocator<charT> >
 int  __fastcall _strings_list<charT,Allocator>::insert_line(int line, const str_type & text)
 {
  if(line<int(lines.size()))
   {
     _strings_list<charT,Allocator> sl((string&)*&text,line_separator);
     return insert_lines(line,sl);
   }
   return add_line(text);
 }

template <typename charT,typename Allocator = allocator<charT> >
 int  __fastcall _strings_list<charT,Allocator>::insert_lines(int line, _strings_list & text)
 {
   if(line<int(lines.size()))
   {
    lines.insert(lines.begin()+line,text.lines.begin(),text.lines.end());
    return line;
   }
   else
   {
    lines.resize(line+1);
    return insert_lines(line,text);
   }
 }

template <typename charT,typename Allocator = allocator<charT> >
int  __fastcall _strings_list<charT,Allocator>::remove_line (int line)
{
 if(line<int(lines.size()))
   lines.erase(lines.begin()+line);
  return line<int(lines.size());
}







#ifndef _NO_NAMESPACE
//namespace KeRTL
}
#endif


#endif
