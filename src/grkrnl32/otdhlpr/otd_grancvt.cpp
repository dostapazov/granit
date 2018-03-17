#include <otd_grancvt.h>
#include <grnet_dos.h>



inline DWORD   __fastcall get_gran_ts(DWORD i ,LPWORD ts_array)
{
 div_t dt = div(i,16);
 ts_array+=dt.quot;
 return ((*ts_array)&(0x8000>>dt.rem)) ? TRUE:FALSE;

}

inline DWORD   __fastcall get_gran_tit(DWORD i ,LPBYTE tit_array)
{
  return ((DWORD)(tit_array[i]))&0x000000FF;
}

inline void __fastcall set_gran_ts(DWORD i ,BYTE val,LPWORD ts_array)
{
 if(i<64)
 {
  div_t dt = div(i,16);
  ts_array+=dt.quot;
  WORD mask = (0x8000>>dt.rem);
  if(val)
     (*ts_array)|=mask;
     else
     (*ts_array)&=~mask;
 }
}

inline void __fastcall set_gran_tit(DWORD i ,BYTE val,LPBYTE tit_array)
{
 if(i<16)
  tit_array[i] = val;
}

#pragma warn -8057
/*
 Возвращает количество байт в выходном буфере;
 если значение отрицательное - то размер буфера недостаточен и
 модуль возврата - необходимый размер буфера
*/
int WINAPI otd_convert_atony2otd(DWORD num_pu,
                                 lp_gratony_kadr src_kadr,
                                 DWORD src_diag,
                                 LPBYTE dest_buffer,DWORD dest_bsz,
                                 BOOL protect, lpotd_proto op)
{
 int ret_len;
 if(src_kadr && src_kadr->len )
   {
     otd_proto_param opp;
     DWORD otd_fa   = OTD_FA_DISCRETE;
     LPBYTE array;
     ZeroMemory (&opp,sizeof(opp));
     opp.dw_size = sizeof(opp);
     opp.parts   = OTD_PROTO_PART_DATA|OTD_PROTO_PART_DIAG|(protect ? OTD_PROTO_PART_KPK:0);
     if(src_kadr->P)
        opp.parts|=OTD_PROTO_FLAG_QUERYRESPOND;
     opp.dt_param.type  = OTD_DISCRETE;
     opp.dt_param.count = 64;
     if(src_kadr->InfoType == tpTit)
      {
        otd_fa  = OTD_FA_ANALOG;
        opp.dt_param.type  = OTD_ANALOG_BYTE;
        opp.dt_param.count = 16;
        // Предварительный просмотр всех значение тит
        array = src_kadr->data;
        for(DWORD i = 0;i<opp.dt_param.count;i++)
         {
          if((*array)>250)
              opp.parts|=OTD_PROTO_PART_PERSONAL_DIAG;
          array++;
         }
      }
      if(opp.parts&OTD_PROTO_PART_PERSONAL_DIAG)
        {
         opp.pd_param.type  = OTD_ANALOG_BYTE;
         opp.pd_param.count = opp.dt_param.count;
        }

     ret_len = (int)otd_proto_calc_size_ex(&opp,0);
     if((DWORD)ret_len<= dest_bsz)
     {
      otd_proto new_op;
      new_op.dw_size = sizeof(new_op);
      otd_proto_format_ex(dest_buffer,dest_bsz,&opp,0,&new_op);
      *new_op.diag = 0;
      OTD_SET_ADDR(new_op.addr,num_pu,src_kadr->numCp,otd_fa,src_kadr->numGrp);
      array = src_kadr->data;
        /*Копирование элементов*/
        for(DWORD i = 0;i<opp.dt_param.count;i++)
         {
          DWORD value,pd;
          value = otd_fa ? get_gran_tit(i,(LPBYTE)array):get_gran_ts(i,(LPWORD)array);
          if(value>250)
             {
              pd = OTD_PDIAG_OUTSCALE;
              (*new_op.diag)|=OTD_PART_DIAG_PARAM;
             } 
             else
             pd = 0;
           otd_proto_setvalue(&new_op,i,&value,sizeof(value),pd,FALSE);
         }

      if(protect)
         otd_proto_protect(&new_op,ret_len,OTD_DEF_POLINOM);
      if(op && op->dw_size == sizeof(*op))
        memcpy(op,&new_op,sizeof(new_op));
     }
     else
     ret_len =-ret_len;

   }
   else
   ret_len = 0;

 return ret_len;
}
#pragma warn .8057

#ifdef __BORLANDC__
#pragma warn -8004
#endif

int __fastcall otd_convert_data2atony(lpotd_proto src,LPBYTE dest_buffer,DWORD dest_bsz)
{
  int need_size = 0;
  int param_count = 0;
  if(src && src->proto_size && src->addr)
  {
    lp_gratony_kadr  kadr = (lp_gratony_kadr)dest_buffer;
    /*Вычисление необходимого размера*/
    int need_size = 0;
    if(src->addr->fa == OTD_FA_DISCRETE && src->data)
      {
       need_size+=sizeof(*kadr)+7;//8 байт тс 1 уже есть в atony_kadr::data;
       param_count = KeRTL::MIN(64,OTD_GET_COUNT(&src->data->numbers));

      }
    if(src->addr->fa == OTD_FA_ANALOG)
       {
        need_size+=sizeof(*kadr)+15;//16 байт тит 1 уже есть в atony_kadr::data;
        param_count = KeRTL::MIN(16,OTD_GET_COUNT(&src->data->numbers));
       }
    if(dest_buffer && need_size<(int)dest_bsz)
    {
     ZeroMemory(kadr,sizeof(*kadr));
     memset(kadr->data,-1,need_size+sizeof(kadr->data)-sizeof(*kadr));
     kadr->kadr_len = need_size+sizeof(kadr->kadr_len);
     kadr->len      = need_size;
     kadr->numCp    = src->addr->cp;
     kadr->numGrp   = src->addr->sb;
     kadr->InfoType = src->addr->fa;

     for(int i = 0;i<param_count;i++)
     {
      BYTE   value = -1;
      otd_get_value(src->data,i,&value,sizeof(value));
      if(src->addr->fa == OTD_FA_DISCRETE)
        set_gran_ts(i,value,(LPWORD)kadr->data);
        else
        set_gran_tit(i,value,kadr->data);

     }

    }
    else
    need_size = -need_size;
  }
  return need_size;

}
#ifdef __BORLANDC__
#pragma warn .8004
#endif

int WINAPI otd_convert_otd2atony(lpotd_proto src,LPDWORD cmd,LPBYTE dest_buffer,DWORD dest_bsz)
{
 int ret = 0;
 if(cmd)
    *cmd = -1;
 if(src && src->addr)
 {
  if(OTD_FA_ISQUERY(src->addr->fa))
  {
   //Вызовы данных
   DWORD net_command = CmQuery;
   lp_gratony_kadr kadr  = (lp_gratony_kadr)dest_buffer;
   ret = sizeof(gratony_kadr);
   if(ret<=(int)dest_bsz)
   {
   ZeroMemory(dest_buffer,ret);
   kadr->kadr_len = ret;
   kadr->len      = ret;
   if(src->addr->cp!= OTD_ADDR_MAXVALUE)
    {
     net_command    = CmQueryCp;
     kadr->numCp    = src->addr->cp;
     kadr->InfoType = tpAll;
    }
   }
   else
   ret = -ret;

      /*Вызов данных*/
      if(cmd)
         *cmd = net_command;
      return ret;

  }
  
  if(src->addr->cp != OTD_ADDR_MAXVALUE &&
     src->addr->fa<OTD_FA_ALL && src->addr->sb != OTD_ADDR_MAXVALUE)
     {
      /*Это данные*/
      ret = otd_convert_data2atony(src,dest_buffer,dest_bsz);
      if(cmd)
         *cmd = CmRetranslation;
      return ret;   
     }
 }
 return ret;
}


