#ifdef __linux__
#include <gklib/otd_arch_proto.h>
#else
#include <grkrnl32/otd_arch_proto.h>
#endif


int  WINAPI otd_arch_proto_format(LPBYTE buf,DWORD sz,DWORD pu,DWORD cp,DWORD cmd,DWORD data_size,LPVOID data,DWORD error,LPDWORD need_sz ,lpotd_proto op )
{
 LPOTD_ARCH_HEADER ah;
 otd_proto _op;

 if(!buf ) sz = 0;


 otd_proto_param opp;
 ZeroMemory(&opp,sizeof(opp));
 opp.dw_size  = sizeof(opp);
 opp.parts    = OTD_PROTO_PART_PROTOEXTENSION|OTD_PROTO_PART_KPK  ;
 opp.ext_size = data_size+sizeof(OTD_ARCH_HEADER)-sizeof(ah->data[0]);
 opp.ext_type = OTD_BYTE;
 if(error)
   {
     opp.parts|=OTD_PROTO_PART_ERROR;
     opp.error_size = sizeof(error);
     cmd|=OTD_ARCH_FLAG_ERROR;
   }


 DWORD _need_sz = otd_proto_calc_size_ex(&opp,0);
 if(need_sz) *need_sz = _need_sz;
 if(_need_sz<=sz)
  {
   if(!op) op = &_op;
   ZeroMemory(op,sizeof(*op));
   op->dw_size = sizeof(*op);
   otd_proto_format_ex(buf,sz,&opp,0,op);
   op->addr->addr = (DWORD)-1;
   op->addr->pu = pu;
   op->addr->cp = cp;
   ah = (LPOTD_ARCH_HEADER)op->extension->block_data;
   ah->cmd = cmd;
   ah->data_size = data_size;
   if(data)
     memcpy(ah->data,data,data_size);
     else
     memset(ah->data,0,data_size);
   if(op->error)
   {
      LPDWORD dw_ptr = (LPDWORD)op->error->block_data ;
       *dw_ptr = error;
   }
   otd_proto_protect(op,0,OTD_DEF_POLINOM);
   return op->proto_size;
  }
 return 0;
}


DWORD WINAPI otd_arch_proto_get_error(lpotd_proto op)
{
 if(op->error)
    {
     LPDWORD ret = (LPDWORD)op->error->block_data;
     return *ret;
    }
 return 0;
}


int WINAPI otd_arch_proto_format_list(LPBYTE buf,DWORD sz,DWORD pu,DWORD cp,DWORD cmd,DWORD count,LPQWORD list,DWORD error,LPDWORD need_sz ,lpotd_proto op )
{
  otd_proto _op;
  if(!op)
  {
   ZeroMemory(&_op,sizeof(_op));
   _op.dw_size = sizeof(_op);
   op = &_op;
  }
  LPOTD_ARCH_HEADER lah;
  LPOTD_ARCH_LIST   lst;
  DWORD lst_size    = 0;
  if(count  ) lst_size = sizeof(*lst) + (count-1)*sizeof(*list);
  int ret = otd_arch_proto_format(buf,sz,pu,cp,cmd,lst_size,NULL,error,need_sz,op);
  if(ret && count)
  {
    lah = (LPOTD_ARCH_HEADER)op->extension->block_data;
    lst = (LPOTD_ARCH_LIST  )lah->data;
    lst->count = count;
    memcpy(lst->archive,list,sizeof(*list)*count);
  }
  otd_proto_protect(op,0,OTD_DEF_POLINOM);
 return ret;
}



