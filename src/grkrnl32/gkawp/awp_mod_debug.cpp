#pragma hdrstop
#include "gkawpmodule.hpp"

void   __fastcall TGkAwpModule::dbg_set_all_records_state(DWORD state,bool set)
{
   locker l(mut);
   records_t::iterator rptr = records.begin();
   records_t::iterator rend = records.end  ();
   while(rptr<rend)
   {
     if(set)
     rptr->state |=state;
     else
     rptr->state &=~state;
     record_changes[rptr->rec_id] |= MDBR_FIELD_STATE;
     ++rptr;
   }

}

