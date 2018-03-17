#include "dataserv_config.h"
#include <KeStrm.hpp>

using namespace KeRTL;
using namespace std;


               bool   dev_config_parser::map_inited = false;
std::map<std::string,int>  dev_config_parser::parse_map;

bool __fastcall dev_config_parser::init_parse_map()
{
 parse_map["device"]      = 1;
 parse_map["analog_tag"]  = 2;
 parse_map["analog_kadr"] = 3;
 parse_map["bit_tag"]     = 4;
 parse_map["bit_kadr"]    = 5;
 parse_map["int_tag"]     = 6;
 parse_map["int_kadr"]    = 7;
 parse_map["tag_name_prefix"] = 8;
 parse_map["server"]      = 9;
 parse_map["port"]        = 10;
 return true;
 }

 void __fastcall dev_config_parser::do_parse(std::string & str)
 {
   device_config dc = {0};
   while(str.length())
   {
    string::size_type end = str.find(';',0);
    if(end<str.length())
    {
     string::size_type eq  = str.find('=',0);
     if( eq < end)
       {
         string name = str.substr(0,eq);
         string val  = str.substr(eq+1,end-eq-1);
         if(parse_map.count(name))
          {
           switch(parse_map[name])
           {
            case  1 : safe_strcpyn(dc.device_name,val.c_str(),sizeof(dc.device_name));break;
            case  2 : dc.an_tag_count   = atoi(val.c_str());break;
            case  3 : dc.an_tag_kadrid  = atoi(val.c_str());break;
            case  4 : dc.bit_tag_count   = atoi(val.c_str());break;
            case  5 : dc.bit_tag_kadrid  = atoi(val.c_str());break;
            case  6 : dc.int_tag_count   = atoi(val.c_str());break;
            case  7 : dc.int_tag_kadrid  = atoi(val.c_str());break;
            case  8 : safe_strcpyn(dc.tag_name_prefix,val.c_str(),sizeof(dc.tag_name_prefix));break;
            case  9 : safe_strcpyn(this->server_name,val.c_str(),sizeof(server_name));break;
            case  10: server_port  = atoi(val.c_str());break;

           }
          }
       }
    }
    else
     end = str.length();
     str.erase(0,++end);
   }
   if(*dc.device_name)
    this->dl.push_back(dc);
 }

dev_config_parser::dev_config_parser(const char * file_name)
{
  if(!map_inited)
     map_inited = init_parse_map();
  lstrcpy (server_name,"127.0.0.1");
  server_port = 45777;

  comment = 0;
  KeRTL::TFileStream fs(file_name,true);
  string str;
  str.reserve(4096);
  if(fs.IsGood())
  {
   char ch;
   while(!fs.Eos())
   {
    fs>>ch;
    switch(ch)
    {
     case '\n':
     case '\r': comment = 0;break;
     case '/' : ++comment;break;
     default:
     if(!comment && ch>' ') //Пропускаем комментарии и все непечаиные символы
     {
       if(ch == '{')
          str.erase(0,str.length());
          else
          {
           if(ch == '}')
            {
             do_parse(str);
             str.erase(0,str.length());
            }
            else
            str+=ch;

          }
     }break;
    }
   }
  }
}

 void __fastcall dev_config_parser::get_server_param(char *srv_addr,int srv_addr_sz ,int & srv_port)
 {
  safe_strcpyn(srv_addr,this->server_name,srv_addr_sz);
  srv_port = this->server_port;
 }

