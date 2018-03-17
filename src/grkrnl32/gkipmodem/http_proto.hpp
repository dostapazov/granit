#ifndef HTTP_PROTO_HPP
#define HTTP_PROTO_HPP

#ifndef _WINDOWS_
#define _WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include <string>
//#include <strlist.hpp>
using namespace std;

//протокол HTTP
//



class THttpProto
{
  protected:
  std::string header;
  BOOL        header_complete;
  DWORD       content_len_recv;
  DWORD       content_len;
  char    *   content;
  void __fastcall  setup_content_length();
  bool __fastcall  alloc_content(int len);
  void __fastcall  release_content(){if(content) delete[] content;content = 0;content_len=content_len_recv = 0;}
  public:
  THttpProto():header_complete(false),content_len(0),content_len_recv(0),content(0){}
  ~THttpProto()
  {
   release_content();
  }
  int   __fastcall add(char* bytes,int len);
  bool  __fastcall is_complete();
  bool  __fastcall is_connect();
  bool  __fastcall is_get();
  bool  __fastcall is_response();
  bool  __fastcall is_header_complete(){return header_complete ? true:false;}
  DWORD __fastcall get_header_len() {return header.length();}
  DWORD __fastcall get_content_len(){return content_len_recv;}
  DWORD __fastcall get_total_len()  {return header.length()+content_len_recv;}
  enum parts{pNothing,pHeader,pContent,pBoth};
  bool  __fastcall get_parts(char * buffer,int bsz,parts p,int * cpy_len);
  const char       *  get_header()  const{return header.c_str();}
  const char       *  get_content() const{return content;}
  bool  __fastcall    get_proxy_autorization(char * buffer,int bsz);
  bool  __fastcall    set_proxy_autorization(char *type,char * buffer);
  bool  __fastcall    set_pragma(char * ptext);
  int   __fastcall    get_response_code();
  bool  __fastcall    get_host(char * host,int sz);
  bool  __fastcall    get_url(char*url,int sz);
  void  __fastcall    setup_response(int ver,int error,char * err_text,char * server);
  void  __fastcall    setup_response_content(int cont_len,char * cont_type);
  void  __fastcall    add_header_line(char * c, char * arg);
  void  __fastcall    add_response_proxy_authenticate(char *);
  bool  __fastcall    get_proxy_connection(char * buffer,int bsz);
  bool  __fastcall    set_proxy_connection(char * buffer,int bsz);
  void  __fastcall    end_setup();


  static char  eoh                [];
  static char  response_begin     [];
  static char  content_type       [];
  static char  content_length     [];
  static char  accept_ranges      [];
  static char  host               [];

  static char  proxy_authorization[];
  static char  proxy_connection   [];
  static char  authorization      [];

  static char  pragma             [];


  static char  m_conn             [];
  static char  m_del              [];
  static char  m_get              [];
  static char  m_head             [];
  static char  m_opt              [];
  static char  m_post             [];
  static char  m_put              [];
  static char  m_trace            [];
  static char  eol                [];
  static char  server             [];

};

#endif


