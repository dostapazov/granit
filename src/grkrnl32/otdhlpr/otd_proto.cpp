#ifdef __linux__
#include <gklib/otd_proto.h>
#else
#include <otd_proto.h>
#include <KeRTL.hpp>
#endif

DWORD WINAPI otd_proto_getver(LPBYTE buff,DWORD sz)//Получение версии
{
 if(buff && sz>=(sizeof(otd_addr)+sizeof(WORD)))
 {
  LPWORD ver_mask = (LPWORD)(buff+sizeof(otd_addr));
  return (DWORD)OTD_PROTO_GETVER(*ver_mask);
 }
 return DWORD(-1);
}

BOOL WINAPI otd_proto_get_verpart(LPBYTE buff,DWORD sz,LPDWORD ver,LPDWORD parts)//Получение маски parts
{
 if(buff && sz>=(sizeof(otd_addr)+sizeof(WORD)))
 {
  LPWORD ver_mask = (LPWORD)(buff+sizeof(otd_addr));
  if(ver  ) *ver = (DWORD)OTD_PROTO_GETVER(*ver_mask);
  if(parts) *parts = (DWORD)(*ver_mask)&OTD_PROTO_PARTSMASK;
  return TRUE;
 }
 return FALSE;
}

#ifdef __BORLANDC__
#pragma warn -8004
#endif

BOOL  WINAPI otd_proto_parser(LPBYTE buff,DWORD sz,lpotd_proto op,LPDWORD real_sz)
{
  /*Разбор протокола и заполнение структуры*/
  //TODO рассмотресть случай разбора кадра вызова
  BOOL  ret = FALSE;
  DWORD check_sz = sizeof(otd_addr)+sizeof(WORD);
  DWORD data_sz;
  if(buff && sz>=check_sz && op && op->dw_size == sizeof(*op))
  {
   ret = TRUE;
   //размер кадра не должен быть меньше чем адрес и версия_с_частями
   op->addr =(lpotd_addr) buff;
   buff+=sizeof(*op->addr);
   op->ver_parts =(LPWORD)buff;
   buff+=sizeof(*op->ver_parts);
   WORD parts = ((*op->ver_parts) & OTD_PROTO_PARTSMASK);
   op->diag = NULL;
   op->data = NULL;
   op->personal_diag   = NULL;
   op->personal_chmask = NULL;
   op->name = NULL;
   op->kpk  = NULL;
   if(op->addr->fa == OTD_ADDR_MAXVALUE || (op->addr->fa & OTD_FA_QUERY)==0)
   {
   /*Не вызов*/
   if(ret &&  (parts & OTD_PROTO_PART_DIAG) )
   {
    check_sz+=sizeof(*op->diag);
    if(sz >= check_sz)
      {
       op->diag  = (LPDWORD)buff,buff+=sizeof(*op->diag);

      }
      else
      ret = FALSE;
   }
   else
    op->diag = NULL;

   if(ret && (parts & OTD_PROTO_PART_DATA))
   {

    data_sz = OTD_DATA_GET_SIZE((lpotd_data)buff);
    check_sz+=data_sz;
    if(sz>=check_sz)
       op->data = (lpotd_data)buff,buff+=data_sz;
       else
       ret = FALSE,op->data = NULL;
   }
   else
    op->data = NULL;

   if(ret && (parts&OTD_PROTO_PART_PERSONAL_DIAG))
   {

    data_sz = OTD_DATA_GET_SIZE((lpotd_data)buff);
    check_sz+=data_sz;
    if(sz>=check_sz)
       op->personal_diag = (lpotd_data)buff,buff+=data_sz;
       else
       ret = FALSE,op->personal_diag = NULL;
   }
   else
    op->personal_diag = NULL;

   if(ret && (parts&OTD_PROTO_PART_PERSONAL_CHMASK))
   {

    data_sz = OTD_DATA_GET_SIZE((lpotd_data)buff);
    check_sz+=data_sz;
    if(sz>=check_sz)
       op->personal_chmask = (lpotd_data)buff,buff+=data_sz;
       else
       ret = FALSE,op->personal_chmask = NULL;
   }
   else
    op->personal_chmask = NULL;


   if(ret && (parts & OTD_PROTO_PART_NAME))
   {

    data_sz  = OTD_BLOCK_GET_SIZE((lpotd_block)buff);
    check_sz+=data_sz;
    if(sz>=check_sz)
      op->name = (lpotd_block)buff,buff+=data_sz;
      else
      ret = FALSE,op->name = NULL;
   }
   else
    op->name = NULL;

   if(ret && (parts&OTD_PROTO_PART_TIMESTAMP))
   {
    data_sz=sizeof(*op->time_stamp);
    check_sz+=data_sz;
    if(sz>=check_sz)
      op->time_stamp = (LPQWORD)buff,buff+=data_sz;
      else
      ret = FALSE,op->time_stamp = NULL;
   }
   else
    op->time_stamp = NULL;

   if(ret && (parts&OTD_PROTO_PART_ERROR))
   {
    data_sz  = OTD_BLOCK_GET_SIZE((lpotd_block)buff);
    check_sz+=data_sz;
    if(sz>=check_sz)
      op->error = (lpotd_block)buff,buff+=data_sz;
      else
      ret = FALSE,op->error = NULL;
   }
   else
    op->error = NULL;


   if(ret && (parts&OTD_PROTO_PART_PROTOEXTENSION))
   {
    data_sz  = OTD_BLOCK_GET_SIZE((lpotd_block)buff);
    check_sz+=data_sz;
    if(sz>=check_sz)
      op->extension = (lpotd_block)buff,buff+=data_sz;
      else
      ret = FALSE,op->extension = NULL;
   }
   else
    op->extension = NULL;

   if(ret && (parts&OTD_PROTO_PART_TIMESTAMP_END))
   {
    data_sz=sizeof(*op->time_stamp_end);
    check_sz+=data_sz;
    if(sz>=check_sz)
      op->time_stamp_end = (LPQWORD)buff,buff+=data_sz;
      else
      ret = FALSE,op->time_stamp_end = NULL;
   }
   else
    op->time_stamp_end = NULL;



   }

   if(ret && (parts & OTD_PROTO_PART_KPK))
   {
    check_sz+=sizeof(*op->kpk);
    if(sz>=check_sz)
      {
       op->kpk = (LPWORD)buff;
       buff+=sizeof(*op->kpk);
      }
      else
      ret = FALSE;
   }
   else
    op->kpk = NULL;

   if(!ret)
       check_sz = 0;

   op->proto_size = check_sz;
   if(real_sz)
      *real_sz = check_sz;
  }
  return ret;
}
#ifdef __BORLANDC__
#pragma warn .8004
#endif

DWORD WINAPI otd_proto_getparts_ex(LPVOID __restrict buff,DWORD bsz,LPVOID __restrict _opp,LPVOID __restrict _src ,DWORD ver)
{
 DWORD ret = 0;
 if(_src && _opp )
 {
  if(ver == 0)//Пока только версия протокола 0
  {
   otd_proto_param opp;
   memcpy(&opp,_opp,sizeof(opp));
   lpotd_proto       src = (lpotd_proto)_src;
   otd_proto         dest ;
#ifdef _WINDOWS_
   ZeroMemory(&dest,sizeof(dest));
#else
   bzero(&dest,sizeof(dest));
#endif
   dest.dw_size       = sizeof(dest);
   if(!opp.parts)
       opp.parts = ((DWORD)*src->ver_parts);
   if(!opp.dt_param.type && src->data  )
       opp.dt_param.type = src->data->otd_type;
   if(!opp.dt_param.count && src->data  )
       opp.dt_param.count = OTD_GET_COUNT(&src->data->numbers),opp.dt_param.first = src->data->numbers.loN;
   if(!opp.pd_param.type && src->personal_diag  )
       opp.pd_param.type = src->personal_diag->otd_type;
   if(!opp.pd_param.count && src->personal_diag  )
       opp.pd_param.count = OTD_GET_COUNT(&src->personal_diag->numbers),opp.pd_param.first = src->personal_diag->numbers.loN;
   if(!opp.cm_param.type && src->personal_chmask  )
       opp.cm_param.type = src->personal_chmask->otd_type;
   if(!opp.cm_param.count && src->personal_chmask  )
       opp.cm_param.count = OTD_GET_COUNT(&src->personal_chmask->numbers),opp.cm_param.first = src->personal_chmask->numbers.loN;
   if(!opp.name_sz  && src->name)
      opp.name_sz = OTD_TEXT_GET_LENGTH(src->name);
   if(!opp.error_size && src->error)
       opp.error_size = src->error->block_size;
   if(!opp.ext_size && src->extension)
       opp.ext_size = src->extension->block_size;

   ret = otd_proto_format_ex((LPBYTE)buff,bsz,&opp,0,&dest);
   if(ret)
   {
    dest.addr->addr = src->addr->addr;
    if(dest.diag && src->diag)
       *dest.diag = *src->diag;
    if(dest.data  && src->data)
       otd_copy(dest.data,src->data,src->data->numbers.loN,OTD_GET_COUNT(&src->data->numbers));
    if(dest.personal_diag  && src->personal_diag)
       otd_copy(dest.personal_diag,src->personal_diag,src->personal_diag->numbers.loN,OTD_GET_COUNT(&src->personal_diag->numbers));
    if(dest.personal_chmask  && src->personal_chmask)
       otd_copy(dest.personal_chmask,src->personal_chmask,src->personal_chmask->numbers.loN,OTD_GET_COUNT(&src->personal_chmask->numbers));
    if(dest.name && src->name)
       otd_text_copy(dest.name,src->name);
    if(dest.time_stamp && src->time_stamp)
       *dest.time_stamp = *src->time_stamp;
    if(dest.time_stamp_end && src->time_stamp_end)
       *dest.time_stamp_end = *src->time_stamp_end;
    if(dest.error && src->error)
       memcpy(dest.error->block_data,src->error->block_data,dest.error->block_size),dest.error->block_type = src->error->block_type;

    if(dest.extension && src->extension)
       {
           memcpy(dest.extension->block_data,src->extension->block_data,dest.extension->block_size);
           dest.extension->block_type = src->extension->block_type;
       }
   }
  }
 }
 return ret;
}


//Копирвание частей протокола по маске parts
//Даже если parts = 0 то копируется адрес и ver_parts (естесственно parts = 0)
DWORD WINAPI otd_proto_getparts(LPVOID __restrict buff,DWORD _bsz,DWORD parts,lpotd_proto __restrict src)
{
 otd_proto_param opp;
 ZeroMemory(&opp,sizeof(opp));
 opp.dw_size = sizeof(opp);
 opp.parts = parts;
 return otd_proto_getparts_ex(buff,_bsz,&opp,src,0);
}


//Вычисление размера буфера под кадр
#ifdef _MSC_VER
#pragma warning (disable:4101)
#endif
DWORD WINAPI otd_proto_calc_size(DWORD parts,DWORD count,DWORD type_data,DWORD type_pd,DWORD type_cm,DWORD name_sz)
{
  otd_proto_param opp;
  ZeroMemory(&opp,sizeof(opp));
  opp.dw_size    = sizeof(opp);
  opp.parts      = parts;
  opp.dt_param.count = opp.pd_param.count = opp.cm_param.count = count;
  opp.dt_param.type  = type_data;
  opp.pd_param.type  = type_pd;
  opp.cm_param.type  = type_cm;
  opp.name_sz    = name_sz;
  return otd_proto_calc_size_ex(&opp,0);
}

DWORD WINAPI otd_proto_calc_size_ex(LPVOID pp,DWORD ver)
{
 DWORD ret = 0;/*Пока для версии 0*/
 if(!ver && pp)
 {
  lpotd_proto_param opp = (lpotd_proto_param)pp;
  if(opp->dw_size == sizeof(*opp))
  {
  otd_proto op;
  ret = sizeof(otd_addr)+sizeof(*op.ver_parts);
  if(opp->parts&OTD_PROTO_PART_KPK)
     ret+=sizeof(*op.kpk);
  if(opp->parts&OTD_PROTO_PART_DIAG)
     ret+=sizeof(*op.diag);
  if((opp->parts&OTD_PROTO_PART_DATA) && opp->dt_param.type && opp->dt_param.count)
     ret+= OTD_DATA_CALC_SIZE(opp->dt_param.type,opp->dt_param.count)+sizeof(otd_data)-1;
  if((opp->parts&OTD_PROTO_PART_PERSONAL_DIAG) && opp->pd_param.type && opp->pd_param.count)
     ret+= OTD_DATA_CALC_SIZE(opp->pd_param.type,opp->pd_param.count)+sizeof(otd_data)-1;
  if((opp->parts&OTD_PROTO_PART_PERSONAL_CHMASK) &&opp->cm_param.type && opp->cm_param.count)
      ret+= OTD_DATA_CALC_SIZE(opp->cm_param.type,opp->cm_param.count)+sizeof(otd_data)-1;
  if((opp->parts&OTD_PROTO_PART_NAME) && opp->name_sz)
     ret+=OTD_BLOCK_GET_SIZE_FOR_BUFFER(sizeof(WORD)+opp->name_sz*sizeof(wchar_t));
  if(opp->parts & OTD_PROTO_PART_TIMESTAMP)
     ret+=sizeof(*op.time_stamp);
  if((opp->parts & OTD_PROTO_PART_ERROR) && opp->error_size)
     ret+=OTD_BLOCK_GET_SIZE_FOR_BUFFER(opp->error_size);
  if((opp->parts & OTD_PROTO_PART_PROTOEXTENSION) && opp->ext_size)
     ret+=OTD_BLOCK_GET_SIZE_FOR_BUFFER(opp->ext_size);
  if(opp->parts & OTD_PROTO_PART_TIMESTAMP_END)
     ret+=sizeof(*op.time_stamp_end);
  }
 }
 return ret;
}

DWORD WINAPI otd_proto_format_ex(LPBYTE buf,DWORD bsz,LPVOID pp,DWORD ver,LPVOID op)
{
 DWORD ret = 0;
 if(!ver && pp && ((lpotd_proto_param)pp)->dw_size == sizeof(otd_proto_param))
 {
  ret = otd_proto_calc_size_ex(pp,0);
  if(ret<=bsz)
  {
    lpotd_proto_param opp = (lpotd_proto_param)pp;
    otd_proto _op;
    ZeroMemory(&_op,sizeof(_op));
    ZeroMemory(buf,ret);
    _op.dw_size = sizeof(_op);
    _op.addr = (lpotd_addr)buf,buf+=sizeof(*_op.addr);
    _op.ver_parts = (LPWORD)buf,buf+=sizeof(*_op.ver_parts);
    DWORD parts = opp->parts&OTD_PROTO_FLAGS_MASK;

    if(opp->parts & OTD_PROTO_PART_DIAG)
       {
        _op.diag = (LPDWORD)buf,buf+=sizeof(*_op.diag),parts |= OTD_PROTO_PART_DIAG;
        *_op.diag = OTD_DIAG_NODATA;
       }

    if((opp->parts & OTD_PROTO_PART_DATA) && opp->dt_param.count && opp->dt_param.type)
     {
      parts |= OTD_PROTO_PART_DATA;
      _op.data = (lpotd_data)buf;
       buf+=OTD_DATA_CALC_SIZE(opp->dt_param.type,opp->dt_param.count)+sizeof(otd_data)-1;
       OTD_SET_NUMBERS(&_op.data->numbers,opp->dt_param.first,opp->dt_param.count);
       _op.data->otd_type =(WORD) opp->dt_param.type;
     }

    if((opp->parts & OTD_PROTO_PART_PERSONAL_DIAG) && opp->pd_param.count && opp->pd_param.type)
     {
      parts |= OTD_PROTO_PART_PERSONAL_DIAG;
      _op.personal_diag = (lpotd_data)buf;
       buf+=OTD_DATA_CALC_SIZE(opp->pd_param.type,opp->pd_param.count)+sizeof(otd_data)-1;
       OTD_SET_NUMBERS(&_op.personal_diag->numbers,opp->pd_param.first,opp->pd_param.count);
       _op.personal_diag->otd_type =(WORD) opp->pd_param.type;
       __int64 fill_val = OTD_PDIAG_NODATA;
       otd_fill(_op.personal_diag,&fill_val,sizeof(fill_val));
       if(_op.diag)
          *_op.diag |= OTD_PART_DIAG_PARAM;
     }

    if((opp->parts & OTD_PROTO_PART_PERSONAL_CHMASK) && opp->cm_param.count && opp->cm_param.type)
     {
       parts |= OTD_PROTO_PART_PERSONAL_CHMASK;
      _op.personal_chmask = (lpotd_data)buf;
       buf+=OTD_DATA_CALC_SIZE(opp->cm_param.type,opp->cm_param.count)+sizeof(otd_data)-1;
       OTD_SET_NUMBERS(&_op.personal_chmask->numbers,opp->cm_param.first,opp->cm_param.count);
       _op.personal_chmask->otd_type = (WORD)opp->cm_param.type;
     }

     if((opp->parts & OTD_PROTO_PART_NAME) && opp->name_sz)
     {
       parts |= OTD_PROTO_PART_NAME;
       _op.name = (lpotd_block)buf;
       DWORD bl_sz = OTD_BLOCK_GET_SIZE_FOR_BUFFER(sizeof(WORD)+opp->name_sz*sizeof(wchar_t));
       buf+= bl_sz;
       OTD_BLOCK_INIT(_op.name,OTD_BLOCKTYPE_TEXT,bl_sz);
     }

     if(opp->parts&OTD_PROTO_PART_TIMESTAMP)
       {
        parts|=OTD_PROTO_PART_TIMESTAMP;
        _op.time_stamp = (LPQWORD)buf;
        buf+=sizeof(*_op.time_stamp);
       }

     if((opp->parts&OTD_PROTO_PART_ERROR) && opp->error_size)
     {
       parts|=OTD_PROTO_PART_ERROR;
       _op.error = (lpotd_block)buf;
       DWORD bl_sz = OTD_BLOCK_GET_SIZE_FOR_BUFFER(opp->error_size);
       buf+= bl_sz;
       OTD_BLOCK_INIT(_op.error,OTD_BLOCKTYPE_ERROR,bl_sz);
     }

     if((opp->parts&OTD_PROTO_PART_PROTOEXTENSION) && opp->ext_size)
     {
       parts|=OTD_PROTO_PART_PROTOEXTENSION;
       _op.extension = (lpotd_block)buf;
       DWORD bl_sz = OTD_BLOCK_GET_SIZE_FOR_BUFFER(opp->ext_size);
       buf+= bl_sz;
       OTD_BLOCK_INIT(_op.extension,opp->ext_type,bl_sz);
     }

     if(opp->parts&OTD_PROTO_PART_TIMESTAMP_END)
       {
        parts|=OTD_PROTO_PART_TIMESTAMP_END;
        _op.time_stamp_end = (LPQWORD)buf;
        buf+=sizeof(*_op.time_stamp);
       }


     if(opp->parts & OTD_PROTO_PART_KPK)
       parts |= OTD_PROTO_PART_KPK, _op.kpk = (LPWORD)buf;
    *_op.ver_parts = (WORD) parts;// установили список составных частей
    _op.proto_size = ret;
    if(op)
       memcpy(op,&_op,sizeof(_op));

  }
  else ret = 0;
 }

 return ret;
}




DWORD WINAPI otd_proto_format(LPBYTE buf,DWORD bsz,DWORD parts,DWORD start,DWORD count,DWORD type_data,DWORD type_pd,DWORD type_cm,DWORD name_sz,lpotd_proto op)
{
 otd_proto_param opp;
 ZeroMemory(&opp,sizeof(opp));
 opp.dw_size = sizeof(opp);
 opp.parts   = parts;
 opp.dt_param.first = opp.pd_param.first = opp.cm_param.first = start;
 opp.dt_param.count = opp.pd_param.count = opp.cm_param.count = count;
 opp.dt_param.type  = type_data;
 opp.pd_param.type  = type_pd;
 opp.cm_param.type  = type_cm;
 opp.name_sz    = name_sz;
 return otd_proto_format_ex(buf,bsz,&opp,0,op);
}

DWORD WINAPI otd_proto_format_query(LPBYTE buf,DWORD bsz,lpotd_addr addr,DWORD ver,DWORD parts,LPVOID _op)
{
 /*Форматирование буфера под запрос*/
 DWORD ret = 0;
 if(ver == 0)
 {
  otd_proto op;
  ZeroMemory(&op,sizeof(op));
  op.dw_size = sizeof(op);
  ret = sizeof(*op.addr)+sizeof(*op.ver_parts) + ((parts&OTD_PROTO_PART_KPK) ? sizeof(*op.kpk):0);
  if(ret <=bsz)
  {
   ZeroMemory(buf,ret);
   op.addr = (lpotd_addr)buf;
   buf+=sizeof(otd_addr);
   op.ver_parts = (LPWORD)buf;
   buf+=sizeof(WORD);
   if(parts&OTD_PROTO_PART_KPK)
     op.kpk =(LPWORD) buf;
   op.addr->addr = addr->addr;
   if((op.addr->fa&OTD_FA_QUERY)==0)
   {
    op.addr->fa|=OTD_FA_QUERY;
   }
   *op.ver_parts = (WORD)(parts&OTD_PROTO_PARTSMASK);
   op.proto_size = ret;
   if(_op && (*(LPDWORD)_op) == op.dw_size)
     memcpy(_op,&op,sizeof(op));
  }
  else
  ret = 0;
 }
 return ret;
}


DWORD WINAPI otd_proto_getparam(lpotd_proto op,lpotd_proto_param opp)
{
 if(op && opp && op->dw_size == sizeof(*op) && opp->dw_size == sizeof(*opp))
 {
  memset(opp,0,sizeof(*opp));
  opp->dw_size = sizeof(*opp);
  if(op->diag) {opp->parts|= OTD_PROTO_PART_DIAG;}
  if(op->data)
   {
    opp->parts|= OTD_PROTO_PART_DATA;
    opp->dt_param.type = op->data->otd_type;
    opp->dt_param.first = op->data->numbers.loN;
    opp->dt_param.count = OTD_GET_COUNT(&op->data->numbers);
   }
  if(op->personal_diag)
   {
    opp->parts|= OTD_PROTO_PART_PERSONAL_DIAG;
    opp->pd_param.type  = op->personal_diag->otd_type;
    opp->pd_param.first = op->personal_diag->numbers.loN;
    opp->pd_param.count = OTD_GET_COUNT(&op->personal_diag->numbers);
   }
  if(op->personal_chmask)
   {
    opp->parts|= OTD_PROTO_PART_PERSONAL_CHMASK;
    opp->cm_param.type  = op->personal_chmask->otd_type;
    opp->cm_param.first = op->personal_chmask->numbers.loN;
    opp->cm_param.count = OTD_GET_COUNT(&op->personal_chmask->numbers);
   }

  if(op->name)
  {
   opp->parts|=OTD_PROTO_PART_NAME;
   opp->name_sz = OTD_TEXT_GET_CAPASITY(op->name);
  }
  if(op->time_stamp    ) opp->parts|=OTD_PROTO_PART_TIMESTAMP;
  if(op->time_stamp_end) opp->parts|=OTD_PROTO_PART_TIMESTAMP_END;

  if(op->error)
  {
   opp->parts|=OTD_PROTO_PART_ERROR;
   opp->error_size = op->error->block_size;
  }
  if(op->extension)
  {
   opp->parts|=OTD_PROTO_PART_PROTOEXTENSION;
   opp->ext_size = op->extension->block_size;
   opp->ext_type = op->extension->block_type;
  }
  if(op->kpk)
    opp->parts|=OTD_PROTO_PART_KPK;
  return TRUE;
 }
 return FALSE;
}

BOOL  WINAPI otd_proto_getvalue(lpotd_proto op,DWORD number,LPVOID val,DWORD val_sz,LPDWORD val_type,LPDWORD val_pd,LPDWORD val_chmask)
{
 /*Получение значений параметра, персонвльной диагностки и маски изменения*/
 BOOL ret = FALSE;
 if(op && op->dw_size == sizeof(*op) && op->proto_size)
 {
  if(val && val_sz)
   {
    ZeroMemory(val,val_sz);
    ret = otd_get_value(op->data,number,val,val_sz) ? ret:TRUE;
    if(ret && val_type )
      *val_type = (DWORD)OTD_GET_TYPE(op->data->otd_type);
   }
  if(val_pd)
  {
   *val_pd = 0;
   ret = otd_get_value(op->personal_diag,number,val_pd,sizeof(*val_pd)) ? ret:TRUE;
  }
  if(val_chmask)
  {
   *val_pd = 0;
   ret = otd_get_value(op->personal_chmask,number,val_chmask,sizeof(*val_chmask)) ? ret:TRUE;
  }
 }
 return ret;
}

int mem_cmp(void * ptr1,void * ptr2,int sz)
{
  int ret = 0;
  LPBYTE p1 = (LPBYTE) ptr1;
  LPBYTE p2 = (LPBYTE) ptr2;
  if(p1 && p2)
    {
      for(int i = 0;!ret && i<sz;i++,p1++,p2++)
          ret = int(*p1-*p2);
    }
    else
      ret = -1;
  return ret;
}

BOOL  WINAPI otd_proto_setvalue(lpotd_proto op,DWORD number,LPVOID val,DWORD val_sz,DWORD val_pd,BOOL inc_chmask)
{
  BOOL ret = FALSE;

  if(op && op->dw_size == sizeof(*op) && op->proto_size)
  {
   __int64 old_value  = 0;
   DWORD   old_pd     = 0;
   DWORD   chmask = 0;
   if(otd_proto_getvalue(op,number,&old_value,sizeof(old_value),NULL,&old_pd,NULL))
   {
     if((val && val_sz && memcmp(&old_value,val,val_sz)))
     {
       if(!otd_set_value(op->data,number,val,val_sz))
       {
       if(inc_chmask && op->personal_chmask->otd_type !=OTD_DISCRETE)
          otd_inc_value(op->personal_chmask,number,TRUE);
        else
        {
         chmask = 1;
         otd_set_value(op->personal_chmask,number,&chmask,sizeof(chmask));
        }
        ret = TRUE;
       }
     }

     if(val_pd !=old_pd)
     {
       if(!otd_set_value(op->personal_diag,number,&val_pd,sizeof(val_pd)))
       {
       if(inc_chmask)
          otd_inc_value(op->personal_chmask,number,TRUE);
        else
        {
         chmask += 2;
         otd_set_value(op->personal_chmask,number,&chmask,sizeof(chmask));
        }
        ret = TRUE;
       }
     }
   }
  }
  return ret;
}


unsigned short __fastcall WordRemind(unsigned char Val,unsigned short Del,unsigned short  Remind)
{
 unsigned long Temp=Remind;
 DWORD _Del = DWORD(Del);
 Temp<<=8;
 Temp+=(unsigned long)(Val);
 Temp= Temp%(_Del);
 Remind= (unsigned short)Temp;
 return Remind;
}



extern "C"
unsigned short WINAPI CalcRemind(void * Buffer,unsigned short len,unsigned short Del)
{
  // Расчет остатка
  DWORD   ret = 0;
  if(Buffer && len)
  {
  LPBYTE ptr  = (LPBYTE)Buffer;
  LPBYTE eptr = ptr+len;
  do{
     ret |=(DWORD)((*ptr)&0xFF);
     ret%= (DWORD)Del;
     if(++ptr<eptr)
       ret = 0xFFFFFF00&(ret<<8);
    }while  (ptr<eptr);
  }
  return (unsigned short)ret;
}

extern "C"
unsigned short WINAPI  CalcKPK(void * Buffer,unsigned short len,unsigned short Del)
{
 unsigned short remind = CalcRemind(Buffer,len,Del);
 remind = WordRemind(0,Del,remind);
 remind = WordRemind(0,Del,remind);

  //_AX = (unsigned short)(Del-remind);
  unsigned short diff =  remind ?  (unsigned short)(Del-remind) : 0;
  diff = ((diff>>8)&0x00FF) | ((diff<<8)&0xFF00);
  return diff;


}


/*Контрольные суммы*/
BOOL  WINAPI otd_proto_protect (lpotd_proto op,DWORD sz,DWORD p_del)//расчёт контрольной суммы
{
 if(op &&  op->dw_size ==sizeof(*op))
 {
   if(!sz) sz = op->proto_size;
   if(op->kpk)
      {
         if(!sz) sz = op->proto_size;
         WORD kpk = otd_calc_checksum(op->addr,sz-sizeof(WORD),p_del? p_del:OTD_DEF_POLINOM);
         if(!kpk)
             {
              kpk = p_del;
              kpk = ((kpk>>8)&0x00FF) | ((kpk<<8)&0xFF00);
             }
         *op->kpk = kpk;
         return TRUE;
      }
 }
 return FALSE;
}

BOOL  WINAPI otd_proto_check   (lpotd_proto op,DWORD sz,DWORD p_del)//проверка контрольной суммы
{
 if(op  && op->dw_size ==sizeof(*op))
 {
   if(!sz)  sz = op->proto_size;
   if(otd_calc_checksum(op->addr,sz,p_del ? p_del:OTD_DEF_POLINOM)==0)
     return TRUE;
 }
 return FALSE;
}

WORD WINAPI otd_calc_checksum (LPVOID buffer,DWORD bsz,DWORD p_del)
{
 //return (DWORD)CalcKPK(buffer,(WORD)bsz,(WORD)(p_del ? p_del:OTD_DEF_POLINOM));
 return KeRTL::calc_kpk(buffer,(WORD)bsz,(WORD)(p_del ? p_del:OTD_DEF_POLINOM));
}

/*Формирование кадра ТУ / ТР*/
DWORD WINAPI otd_proto_format_tutr (LPBYTE buf,DWORD bsz,lpotd_addr addr,
                                    DWORD obj,DWORD tu_command,
                                    LPVOID param,DWORD param_sz,
                                    DWORD ver,LPVOID _op )
{
 if(!ver)
 {

 otd_proto_param  opp;
 otd_proto        op;
 ZeroMemory(&opp,sizeof(opp));
 opp.dw_size       = sizeof(opp);
 ZeroMemory(&op,sizeof(op));
 op.dw_size         = sizeof(op);
 opp.parts          = OTD_PROTO_PART_DATA|OTD_PROTO_PART_KPK;
 opp.dt_param.type  = tu_command == OTD_TROP_SET ? OTD_TR_COMMAND_SET :OTD_TUTR_COMMAND;
 opp.dt_param.first = obj;
 opp.dt_param.count = 1;
 DWORD ret = otd_proto_format_ex(buf,bsz,&opp,0,&op);
 if(ret)
 {
  lpotd_tutr tutr = (lpotd_tutr) op.data->data;
  tutr->command   = tu_command;
  if(tu_command == OTD_TROP_SET)
     memcpy(&tutr->param,param,KeRTL::MIN(param_sz,(DWORD)sizeof(tutr->param)));
  op.addr->addr = addr->addr;
  op.addr->fa  |= OTD_FA_CTRL;
  otd_proto_protect(&op,ret,OTD_DEF_POLINOM);
  if(_op && ((lpotd_proto)_op)->dw_size == sizeof(otd_proto))
      memcpy(_op,&op,sizeof(op));
 }
 return ret;
 }
 return 0;
}

DWORD WINAPI otd_scan_personal_diag     (lpotd_proto op)
{
 //Сканирование персональной диапностики
 DWORD ret = OTD_DIAG_NODATA;

 if(sizeof(*op) == op->dw_size   )
 {
    ret = op->diag ? (*op->diag)&OTD_DIAG_MASK : 0;
    if(op->personal_diag)
    {
     DWORD  lo =          op->personal_diag->numbers.loN;
     DWORD  hi = 1+(DWORD)op->personal_diag->numbers.hiN;
     while(lo<hi)
     {
      QWORD  pd_val = 0;
      otd_get_value(op->personal_diag,lo,&pd_val,sizeof(pd_val));
      ret |=  (pd_val&OTD_PSTATE_TUTR_MASK)|((pd_val&OTD_PDIAG_MASK) ? OTD_PART_DIAG_PARAM:0);
      if(pd_val&OTD_PDIAG_TUTR_FAIL)
         ret|=OTD_DIAG_STATE_TUTR_ERROR;
      if(pd_val&OTD_PDIAG_TUTR_DESCRIPT)
         ret|=OTD_DIAG_STATE_TUTR_DESCRIPT;
      lo++;
     }
    }
 }
 return ret;
}

DWORD WINAPI otd_cmp_copy(otd_data *dest,otd_data * src,otd_data  * cm)
{
   int ch_count = 0;
   if(dest && src)
     {
      DWORD loN = KeRTL::MAX(dest->numbers.loN,src->numbers.loN);
      DWORD hiN = KeRTL::MIN(dest->numbers.hiN,src->numbers.hiN);
      while(loN<=hiN)
      {
       DWORD old_val = 0,new_val;
       otd_get_value(dest,loN,&old_val,sizeof(old_val));
       new_val  = old_val;
       otd_get_value(src,loN,&new_val,sizeof(new_val));
       if(new_val!=old_val)
        {
         ch_count++;
         otd_set_value(dest,loN,&new_val,sizeof(new_val));
         otd_inc_value(cm,loN,TRUE);
        }
       ++loN;
      }
     }
    return ch_count;
  }

BOOL  WINAPI otd_proto_getvalue_ex   (lpotd_proto op,DWORD param_num,lpotd_param_value pv)
{
  BOOL ret = FALSE;
  if(pv && pv->dw_size == sizeof(*pv))
  {
   pv->value.as_qword = 0;
   if(op->data &&  otd_data_inrange(op->data,param_num))
   {
    otd_proto_getvalue(op,param_num,&pv->value,sizeof(pv->value),&pv->type,&pv->pdiag,&pv->changes);
    if(op->diag)
      pv->diag = (*op->diag)&OTD_DIAG_MASK;
    ret = TRUE;
   }
   else
   {
    pv->type = 0;
    pv->diag    = OTD_DIAG_NODATA;
    pv->pdiag   = OTD_PDIAG_NODATA;
    pv->changes = 0;
   }
  }
  return ret;
}

DWORD WINAPI otd_proto_format_alarm(LPVOID buffer,DWORD bsz,LPDWORD need_size,lpotd_addr addr,DWORD count,lpotd_proto op)
{
  DWORD ret = 0;
  otd_proto_param opp;
  otd_proto       _op;
  if(!op)
      op = &_op;
  ZeroMemory(op,sizeof(*op));
  op->dw_size = sizeof(*op);
  lpotd_alarm_param  ap;
  ZeroMemory(&opp,sizeof(opp));
  opp.dw_size  = sizeof(opp);
  opp.parts    = OTD_PROTO_PART_PROTOEXTENSION|OTD_PROTO_PART_KPK;
  opp.ext_type = OTD_BLOCKTYPE_ALARM;
  opp.ext_size = OTD_BLOCK_GET_SIZE_FOR_BUFFER(count*sizeof(*ap));
  DWORD need_sz= otd_proto_calc_size_ex(&opp,0);
  if(need_size) *need_size = need_sz;
  if(need_sz<=bsz)
  {
   ret = otd_proto_format_ex((LPBYTE)buffer,bsz,&opp,0,op);
   if(ret && op->addr && addr)
   {
     op->addr->addr = addr->addr;
     op->addr->fa  &= OTD_FA_DATA_MASK;
     op->addr->fa  |= OTD_FA_SETUP;
   }
  }
  return ret;
}

