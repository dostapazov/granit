#include "http_proto.hpp"

/*
 Примечание 1. Если отсутствует ContentLength то
 поле content заполняется до тех пор, пока recv не вернет 0;
*/

char  THttpProto::eoh                [] = "\r\n\r\n";
char  THttpProto::eol                [] = "\r\n";
char  THttpProto::server             [] = "Server: ";
char  THttpProto::response_begin     [] = "HTTP/";
char  THttpProto::content_type       [] = "Content-Type: ";
char  THttpProto::content_length     [] = "Content-Length: ";
char  THttpProto::accept_ranges      [] = "Accept-Ranges: ";
char  THttpProto::host               [] = "Host: ";

char  THttpProto::proxy_authorization[] = "Proxy-Authorization: ";
char  THttpProto::proxy_connection   [] = "Proxy-Connection: ";

char  THttpProto::authorization      [] = "WWW-Authorization: ";
char  THttpProto::pragma             [] = "Pragma: ";



char  THttpProto::m_conn             []= "CONNECT ";
char  THttpProto::m_del              []= "DELETE ";
char  THttpProto::m_get              []= "GET ";
char  THttpProto::m_head             []= "HEAD ";
char  THttpProto::m_opt              []= "OPTIONS ";
char  THttpProto::m_post             []= "POST ";
char  THttpProto::m_put              []= "PUT ";
char  THttpProto::m_trace            []= "TRACE ";


  int  __fastcall THttpProto::add(char* bytes,int len)
  {
   if(len)
   {
   if(!header_complete)
   {
    //Заголовок принят не полностью
    char * eoh_ptr = strstr(bytes,eoh);
    int hda_len = len;
    if(eoh_ptr)
       {
        header_complete = true;
        hda_len = int(eoh_ptr-bytes)+(sizeof(eoh)-1);
       }
    header.append(bytes,hda_len);
    if(header_complete )
       setup_content_length();
    return hda_len;
   }
   else
   {
    //Добавляем в содержимое
    if(content_len!=content_len_recv)
     {
        //alloc_content(len);
        //int clen = len;
        //memcpy(content+content_len_recv,bytes,len);
        content_len_recv+=len;
        return  len;
     }
   }
   }
   else
    {
     if(content_len == DWORD(-1))
        content_len = content_len_recv;
    }
   return len;
  }

  bool __fastcall THttpProto::is_complete()
  {
    return (header_complete && ((content_len_recv == content_len) || (content_len_recv-2 == content_len))) ? true:false;
  }

  bool __fastcall THttpProto::is_response()
  {
   return (strstr(header.begin(),response_begin)== header.begin()) ? true:false;
  }

 int   __fastcall    THttpProto::get_response_code()
 {
  if(is_response())
     {
      char * ptr =  strchr(header.begin()+lstrlen(response_begin),' ');
      if(ptr)
      return atoi(ptr+1);
     }
  return -1;   
 }

  void __fastcall  THttpProto::setup_content_length()
  {
   DWORD multiplier = 1;

   int offset = header.find(content_type,0);
   if(offset>=0)
   {
   offset = header.find(accept_ranges,0);
   if(offset>=0)
     {
      char * ranges = header.begin()+offset+(sizeof(accept_ranges)-1);
      if(memicmp(ranges,"bytes",5))
        {
         if(memicmp(ranges,"words",5))
         {
          if(memicmp(ranges,"dwords",6))
            multiplier = 4;
         }
         else
         multiplier = 2;
        }
     }
     offset = header.find(content_length,0);
     if(offset>=0)
     {
      char * clen = header.begin()+offset+(sizeof(content_length)-1);
      content_len = atoi(clen)*multiplier;
     }
     else
     this->content_len = offset;

   }
   else
   {
    if(is_connect())
    {
     //Проверяем соединение ли это
     this->content_len = -1;
    }
   }
  }

  bool  __fastcall    THttpProto::is_connect()
  {
   return header.find(m_conn,0) == 0 ? true:false;
  }
  bool  __fastcall    THttpProto::is_get()
  {
   return header.find(m_get,0) == 0 ? true:false;
  }


  /*Получение частей или полностью пакет в буффер*/
  bool  __fastcall THttpProto::get_parts(char * buffer,int bsz,THttpProto::parts p,int * cpy_len)
  {
   bool ret = false;
   int cpb =0;
   if(buffer && bsz>0 && p)
   {
    switch(p)
    {
     case pHeader:
     {
      if(DWORD(bsz)>header.length())
      {
       lstrcpy(buffer,header.c_str());
       ret=true;
      }
      cpb = header.length();
     }
     break;

     case pContent:
     {
      if(content && content_len_recv)
      {
      if(bsz>=int(content_len_recv))
          {
           memcpy(buffer,content,content_len_recv);
           ret = true;
          }
         else
         memcpy(buffer,content,bsz);
         cpb = content_len_recv;

      }else
        {
         cpb =  0;
         ret =  content_len == 0 ? true:false; 
        }
     }
     break;
     case pBoth:
     {
      int cpy_len1=0,cpy_len2=0;
      ret = (
              get_parts(buffer,bsz,pHeader,&cpy_len1)
              &&
              get_parts(buffer+cpy_len1,bsz-cpy_len1,pContent,&cpy_len2)
             );
       cpb = cpy_len1+cpy_len2;
     }
     break;
    }
   }
   if(cpy_len)
      *cpy_len = cpb;
   return ret;
  }

  bool __fastcall  THttpProto::alloc_content(int len)
  {

   if(this->content_len && content_len !=DWORD(-1))
     {
      if(content==NULL)
      content = new char[content_len];
     }
    else
    {
    char * _mem = new char[content_len_recv+len];
    if(content )
     {
      memcpy(_mem,content,content_len_recv);
      delete [] content;
     }
     content = _mem;
    }
    return content ? true:false;
  }

  bool  __fastcall    THttpProto::get_proxy_autorization(char * buffer,int bsz)
  {
    //Получение строки Proxy-авторизации
    bool ret(false);
    int offset = int(header.find(proxy_authorization,0));
    if(offset>=0 && header_complete)
    {
     offset+=strlen(proxy_authorization);
     int len = int(header.find(eoh,offset,2))- offset;
     if(len<bsz)
        {
         memcpy(buffer,header.begin()+offset,len);
         buffer[len] = 0;
         ret = true;
        }

    }
    return ret;
  }

  bool  __fastcall    THttpProto::set_proxy_autorization(char* type,char * buffer)
  {
    bool ret(false);
    if(header_complete)
    {
     char   text[MAX_PATH+sizeof(proxy_authorization)+2];
     int offset = int(header.find(proxy_authorization,0));
     if(offset>=0)
     {
      //Замена
      if(type && buffer)
      {
       offset+=strlen(proxy_authorization);
       int len = int(header.find(eoh,offset,2))- offset;
       header.erase(offset,len);
       wsprintf(text,"%s %s",type,buffer);
       ret = true;
      }
     }
     else
     {
      //Добавление
      if(type && buffer)
      {
       offset = header.length()-sizeof(eoh)+1;
       wsprintf(text,"%sKeep-Alive%s%s%s %s",proxy_connection,eol,proxy_authorization,type,buffer);
       ret = true;
      }
     }
     if(ret)
       header.insert(offset,text);
    }
   return ret;
  }

  bool  __fastcall    THttpProto::set_pragma(char * ptext)
  {
   bool ret(false);
    if(header_complete)
    {
     char   text[MAX_PATH+sizeof(pragma)+2];
     int offset = int(header.find(pragma,0));
     if(offset>=0)
     {
      offset+=strlen(pragma);
      int len = int(header.find(eoh,offset,2))- offset;
      header.erase(offset,len);
      lstrcpy(text,ptext);
     }
     else
     {
      offset = header.length()-sizeof(eoh)+1;
      wsprintf(text,"%s%s",pragma,ptext);
     }

     if(ret)
      header.insert(offset,text);
    }
   return ret;
  }


  bool  __fastcall    THttpProto::get_url(char*url,int sz)
  {
   bool ret(false);
   if(url && is_get())
   {
    int offset = sizeof(m_get)-1;
    int len    = header.find(response_begin,offset,sizeof(response_begin)-1)-offset-1;
    if(len>0 && len<sz)
     {
      memcpy(url,header.begin()+offset,len);
      *(url+len) = 0;
     }
     else
     *url = 0;
   }
   return ret;
  }

  bool  __fastcall    THttpProto::get_host(char * _host,int sz)
  {
   if(this->header_complete && _host)
   {
    int offset = header.find(host,0);
    if(offset>=0)
    {
      offset+=sizeof(host)-1;
      char * ptr = header.begin()+offset;
      int len = header.find(eoh,offset,2)-offset;
      if(len<sz)
        {
         memcpy(_host,ptr,len);
         *(_host+len)=0;
         ptr = strstr(_host,proxy_connection);
         if(ptr) *ptr = 0;
         return true;
        }
    }
   }
   return false;
  }

  void  __fastcall    THttpProto::setup_response(int ver,int error,char * err_text,char * server_name)
  {
   header.resize(0);
   char buffer[1024];
   wsprintf(buffer,"%s%d.%d %d %s%s",response_begin,LOWORD(ver),HIWORD(ver),error,err_text? err_text:"",eol);
   header+=buffer;
   if(server_name)
   {
    wsprintf(buffer,"%s%s%s",server,server_name,eol);
    header+=buffer;
   }
  }

  void  __fastcall    THttpProto::setup_response_content(int cont_len,char * cont_type)
  {
   if(cont_len)
   {
    char buffer[1024];
    wsprintf(buffer,"%s%s%s",content_type,cont_type ? cont_type: "text/html",eol);
    header+=buffer;
    wsprintf(buffer,"%s%d%s",content_length,cont_len,eol);
    header+=buffer;
    this->content_len_recv = this->content_len = cont_len;
   }
  }


  void  __fastcall    THttpProto::add_header_line(char * c, char * arg)
  {
   header+=c;header+=arg;header+=eol;
  }

  void  __fastcall    THttpProto::add_response_proxy_authenticate(char * area)
  {
   char buffer[MAX_PATH];
   wsprintf(buffer,"Basic realm=\"%s\"",area);
   add_header_line("Proxy-Authenticate: ",buffer);
  }

  bool  __fastcall    THttpProto::get_proxy_connection(char * buffer,int bsz)
  {
   bool ret = false;
   if(is_header_complete()  )
   {
    int len = lstrlen(proxy_connection);
    int pos = header.find(proxy_connection,0,len);
    if(pos>=0)
    {
       pos+=len;
       len = header.find(eol,pos)-pos+1;
     if(buffer && len<bsz)
     {
      ret = true;
      lstrcpyn(buffer,header.begin()+pos,len);
      buffer[len] = 0;
     }
    }
   }
   return ret;
  }
  bool  __fastcall    THttpProto::set_proxy_connection(char * buffer,int bsz)
  {
   return false;
  }


void  __fastcall    THttpProto::end_setup()
{
header+="Connection: close";
header+=eol;
header+=proxy_connection;
header+="close";
header+=eol;
header+=eol;
}





