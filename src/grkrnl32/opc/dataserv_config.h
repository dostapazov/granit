/*
 // ������ ����� ������������
 {
  server = 127.0.0.1;
  port   = 45777;
 }

 {
  //��� ���������� �� � ����� ������ ������ ���������� ����� � ������� - ;
  device      = Device1  //��� ����������
  tag_name_prefix = "unit1"
  analog_tag  = 64;      // ���������� ���������� �����
  analog_kadr = 512;     // ID - ����� � ����������� ������
  bit_tag     = 32;      // ���������� �������    �����
  bit_kadr    = 124;     // ID - ����� � ��������    ������
  int_tag     = 16;       // ���������� ������������� �����
  int_kadr    = 32;       // ID - ����� � �������������  ������
 }




*/

#ifndef _DATASERV_CONFIG_PARSER_
#define _DATASERV_CONFIG_PARSER_
#include <ke_defs.h>
#include <vector>
#include <string>
#include <map>

using namespace std;

struct device_config
{
 char  device_name[64];
 char  tag_name_prefix[64];
 DWORD an_tag_count;
 DWORD an_tag_kadrid;
 DWORD bit_tag_count;
 DWORD bit_tag_kadrid;
 DWORD int_tag_count;
 DWORD int_tag_kadrid;
};


typedef std::vector<device_config> dev_config_list;

class dev_config_parser
{
 protected:
 static std::map<std::string,int>  parse_map;
 static bool map_inited;
 bool __fastcall dev_config_parser::init_parse_map();
 dev_config_list dl;
 char server_name[MAX_PATH];
 int  server_port;
 // ���������
 DWORD      comment; //������� �����������
 void       __fastcall do_parse(std::string & str);
 public:
 dev_config_parser(const char * file_name);
 int dev_count(){return dl.size();}
 void __fastcall get_device(int i,device_config & dc){ if(i<(int)dl.size()) dc  = dl[i];}
 void __fastcall get_server_param(char *srv_addr,int srv_addr_sz ,int & srv_port);


};





#endif

 