#pragma hdrstop
#include "gkawpmodule.hpp"
#include "tlm_kadr.h"
#include <KeStrm.hpp>
#include <kfname.hpp>

   const char layout_sw[] = "LAYOUT";
   #pragma pack(push,1)

   struct     layout_entry
   {
     DWORD kadr_id;
     int   x_pos;
     int   y_pos;
     bool  not_gray;
     bool  fixed;
     layout_entry():kadr_id(-1),x_pos(0),y_pos(0),not_gray(false),fixed(false){}
   };

   #pragma pack(pop)

   void  __fastcall TGkAwpModule::do_save_layout(UnicodeString layout_name)
   {
//      UnicodeString  text;
//      text.sprintf(L"SaveLayout kadrs_ordered count = %d ",kadrs_ordered.size());
//      ShowMessage(text);

      std::vector<layout_entry> lev;
      {
       locker l(mut);

          kadrs_t::iterator kbeg = kadrs.begin();
          kadrs_t::iterator kend = kadrs.end  ();
          while(kbeg<kend)
          {
           kadr_t * kdr = (*kbeg);
           TTlmKadrForm * kadr_form = kdr->get_kadr_form() ;
           if( kadr_form  )
           {

               layout_entry le;
               le.kadr_id  = kdr->kadr.kadr_id;
               le.x_pos    = kadr_form->Left;
               le.y_pos    = kadr_form->Top;
               le.fixed    = kadr_form->fixed;
               le.not_gray = kadr_form->disable_bk_grayed;
               lev.push_back(le);
           }
           ++kbeg;
          }
      }

//      text.sprintf(L"layout entryes count = %d ",lev.size());
//      ShowMessage(text);


      layout_name = get_layout_file(layout_name);
      KeRTL::TFileStream  fs (layout_name.c_str());
      KeRTL::TStorage storage(&fs);
      if(storage.create(0,layout_sw,strlen(layout_sw),64))
      {
         storage.clear() ;
         for(unsigned i = 0;i<lev.size();i++)
         {
           layout_entry  le = lev.at(i);
           storage.add_record(&le,sizeof(le));
         }
      }

//      text.sprintf(L"end of save layout %s",layout_name.c_str());
//      ShowMessage(text);


   }

   UnicodeString __fastcall TGkAwpModule::get_layout_file(UnicodeString layout_name)
   {
    layout_name += L".layout";
    UnicodeString layout_file = this->make_module_file_name(layout_name,UnicodeString()) ;
    return layout_file;
   }

   void  __fastcall TGkAwpModule::do_open_layout(UnicodeString layout_name)
   {

      layout_name = get_layout_file(layout_name);
      KeRTL::TFileStream  fs (layout_name.c_str());
      KeRTL::TStorage storage(&fs);
      DWORD ver;
      if(storage.open(layout_sw,strlen(layout_sw),&ver))
      {
        locker l(mut);

        close_all_kadrs(true);
        DWORD count = storage.get_record_count();
        for(DWORD i = 0;i<count;i++)
        {
          layout_entry le;
          storage.get_record(i,&le,sizeof(le));
          kadr_t * kadr  = (kadr_t*)get_kadr(le.kadr_id);
          if(kadr)
          {
            TTlmKadrForm * frm     = open_kadr_form(le.kadr_id,false);
            frm->Left              = le.x_pos;
            frm->Top               = le.y_pos;
            frm->disable_bk_grayed = le.not_gray;
            frm->fixed             = le.fixed;
            frm->Visible           = true;
          }
        }
      }
      Application->ProcessMessages();
   }



       void __fastcall  TGkAwpModule::enum_layouts   (TStrings * strs)
       {
             strs->Clear();
             UnicodeString layouts_wildcard = get_layout_file(L"*");
             TSearchRec sr;
             if(!FindFirst(layouts_wildcard,faAnyFile,sr))
             {
               do{
                   wchar_t layout_name[MAX_PATH];
                   split_path(sr.Name.c_str(),NULL,NULL,layout_name,NULL);
                   strs->Add(layout_name);
                 }while(!FindNext(sr));
               FindClose(sr);
             }
       }

