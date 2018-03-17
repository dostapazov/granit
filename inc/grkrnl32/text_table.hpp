#ifndef _TEXT_TABLE_HPP
#define _TEXT_TABLE_HPP
#include <storage.hpp>
#include <kestrm.hpp>
#include <string>

using namespace KeRTL;


class text_table_t
{
 protected:
 std::wstring      file_name;
 KeRTL::TStream  * stream;
 TStorage  * storage;
 public:
 ~text_table_t(){close();};
 text_table_t ():stream(NULL),storage(NULL){};
 text_table_t (wchar_t * file_name);
 bool  __fastcall open(const wchar_t * file_name,bool force = false);
 void  __fastcall close();
 DWORD __fastcall add (wchar_t * text,DWORD sz = 0);
 DWORD __fastcall get_length(DWORD id);
 DWORD __fastcall get_text  (DWORD id, wchar_t * buffer,DWORD cbsz);
 DWORD __fastcall update(DWORD id, wchar_t * text,DWORD sz = 0);
 DWORD __fastcall count(){return storage ? storage->get_record_count() : 0;}
 void  __fastcall reopen(){if(storage){close();open(file_name.c_str());} }

};





#endif

 