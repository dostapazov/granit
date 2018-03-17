#pragma hdrstop
#include "gkawpmodule.hpp"
#include "tlm_kadr.h"
#include "history_data.h"

void   __fastcall TGkAwpModule::history_check_present()
{
         if(is_source_connected ())
         {
           OTDM_ARCHIVE_HEADER ah;
           ah.hist_id =(QWORD)main_form;
           ah.data[0] = 0;
           send_command(FA_OTD_MEDIUM_ALARM_ARCHIVE,false,false,OTDMPROTO_CMD_ARCHIVE_PRESENT,0,&ah,sizeof(ah));
         }
         else
         this->history_set_present(false);
}

LRESULT __fastcall TGkAwpModule::handle_history        (MODEM_ADDR & from, LPOTDM_PROTO_HEADER omph, DWORD data_size)
{
    OTDM_PROTO_STATE ps(omph);
    if(ps.is_respond && !ps.is_error )
    {
     if(from.modem == params.modem_source.modem && (!from.modem || from.line == params.modem_source.line))
     {
     TForm * f = reinterpret_cast<TForm*>((LPVOID) ps.p_archive_hdr->hist_id);
     if(f && IsWindow(f->Handle))
      {
         switch(ps.command)
         {
          case OTDMPROTO_CMD_QUERY_ARCHIVE:
               SendMessage(f->Handle,HDCM_HANDLE_ARCHIVE,ps.end_data,(LPARAM)ps.p_archive_hdr->data);
          break;
          case OTDMPROTO_CMD_ARCHIVE_PRESENT:
               history_set_present ((LPVOID(f) == LPVOID(main_form)) ? true:false);
          break;
          case OTDMPROTO_CMD_QUERY_KADR_ARCHIVES:
               SendMessage(f->Handle,WM_HANDLE_KADR_ARCHIVE,ps.end_data,(LPARAM)ps.p_arch_hdr->data);
          break;
          default:
           f = f;
           break;
         }
      }
     }
    }
    return GKH_RET_SUCCESS;
}

void __fastcall TGkAwpModule::request_history(QWORD hist_id,QWORD tm_beg,QWORD tm_end,DWORD alarm,records_ids_t &  rec_ids)
{
    DWORD sel_count = rec_ids.size();
    if(sel_count)
    {
        LPMPROTO_HEADER        mph;
        LPOTDM_PROTO_HEADER    omph;
        LPOTDM_ARCHIVE_HEADER  arch_hdr;
        LPOTDM_ARCHIVE_QUERY   arch_query;
        DWORD query_size = sizeof(*arch_hdr)- sizeof(arch_hdr->data[0]) + sizeof(*arch_query)+sizeof(DWORD)*(sel_count-1);
        DWORD data_size  = sizeof(*omph)-sizeof(omph->data[0])+query_size;
        DWORD need_size  = data_size+sizeof(*mph)-sizeof(mph->data[0]);


        std::unique_ptr<BYTE, array_deleter_t<BYTE> > buf(new BYTE[need_size]);
        mph = (LPMPROTO_HEADER) buf.get();

        modem_addr ma = get_modem_source();
        mproto_init(mph,0,FA_OTD_MEDIUM_ALARM_ARCHIVE,data_size,&ma);
        omph = (LPOTDM_PROTO_HEADER)mph->data;
        omph->data_size = query_size;
        omph->command   = OTDMPROTO_CMD_QUERY_ARCHIVE;
        arch_hdr   = (LPOTDM_ARCHIVE_HEADER)omph->data;
        arch_query = (LPOTDM_ARCHIVE_QUERY )arch_hdr->data;

        arch_query->alarm           =   alarm;
        arch_query->period.beg_time =   tm_beg;
        arch_query->period.end_time =   tm_end;
        arch_query->period.end_time+=   MSEC_NS100(10000); //+ 5 секунд
        arch_query->rec_ids.count = sel_count;
        records_ids_t::iterator rptr = rec_ids.begin(),rend = rec_ids.end();
        LPDWORD ids = arch_query->rec_ids.rec_id;
        while(rptr<rend)
        {
          *ids = *rptr;
          ++ids;
          ++rptr;
        }
        arch_hdr->hist_id = hist_id;
        send_data(mph);
    }
}

struct history_present_setter
{
  bool present;
  history_present_setter(bool _present):present(_present){};
  history_present_setter(const history_present_setter & src):present(src.present){};
  void operator()(kadr_t * kadr)
  {
     if(kadr->user_data)
        PostMessage(kadr->get_kadr_form()->Handle,WM_HISTORY_PRESENT,present,0);
  }


};

void   __fastcall TGkAwpModule::history_set_present  (bool present)
{
   locker l(mut);
   if(history_present!=present)
   {
      history_present=present;
      for_each(kadrs.begin(),kadrs.end(),history_present_setter(present));
   }
}


