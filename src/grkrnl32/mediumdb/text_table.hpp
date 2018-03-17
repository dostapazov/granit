#ifndef _TEXT_TABLE_HPP
#define _TEXT_TABLE_HPP
#include <storage.hpp>
#include <kestrm.hpp>

using namespace KeRTL;


class text_table
{
 protected:

 KeRTL::TStream   * stream;
 TStorage  * storage;
 public:
 ~text_table(){close();};
 text_table():stream(NULL),storage(NULL){};
 text_table (wchar_t * file_name);
 bool  __fastcall open(wchar_t * file_name);
 void  __fastcall close();
 DWORD __fastcall add (wchar_t * text,DWORD sz = 0);
 DWORD __fastcall get_length(DWORD id);
 DWORD __fastcall get_text  (DWORD id, wchar_t * buffer,DWORD cbsz);
 DWORD __fastcall update(DWORD id, wchar_t * text,DWORD sz = 0);
 DWORD __fastcall count(){return storage ? storage->get_record_count() : 0;}

};





#endif

 