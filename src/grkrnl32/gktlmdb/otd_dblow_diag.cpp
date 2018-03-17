#pragma hdrstop
#include "otd_dblow.hpp"

//Получение диагностики

DWORD   __fastcall otd_dblow::get_diag   (otd_addr & addr)
{
 DWORD   ret = OTD_DIAG_NODATA;
 proto_pointer l_ptr,h_ptr;
 if(get_range(addr,l_ptr,h_ptr))
   {
       ret = get_diag(l_ptr,false);
   }
 return  ret;
}

DWORD   __fastcall otd_dblow::get_diag   (proto_pointer  ptr,bool rescan)
{
     rescan = true;
     DWORD ret = OTD_DIAG_NODATA;
     if(otd_list.size() && ptr<otd_list.end())
     {
      ret = ptr->op.diag ? *ptr->op.diag:0;

      if(rescan)
        {
         ret&=~OTD_PART_DIAG_PARAM|OTD_DIAG_NODATA;
         if(ptr->op.addr->sb<OTD_ADDR_MAXVALUE)
            ret = otd_scan_personal_diag(&ptr->op);
          else
          {

           ret&=OTD_DIAG_MASK;
           if(ptr->op.data)
             *ptr->op.diag = ret;
           /*Пробежаться по всем подобъектам*/
           proto_pointer l_ptr = otd_list.begin(),h_ptr = ptr;
           if(get_range(*ptr->op.addr,l_ptr,h_ptr))
           {
            while(l_ptr<h_ptr)
            {
             if(l_ptr!=ptr)
             {
             DWORD diag = get_diag(l_ptr,true);
             if(l_ptr->op.diag &&(*l_ptr->op.diag^diag))
                 *l_ptr->op.diag = diag;
             ret|=(diag&(OTD_STATE_MASK));
             if(diag&OTD_DIAG_MASK)
               ret|= OTD_PART_DIAG_PARAM;
             if(diag&OTD_DIAG_STATE_TUTR_ERROR)
                ret|=OTD_DIAG_STATE_TUTR_ERROR;
             }
             ++l_ptr;
            }
           }
          }
        }
     }
     return ret;
}

inline bool    __fastcall is_subobject(otd_addr & a,otd_addr & checked)
{
  DWORD  a_type  = otd_addr_type(&a);
  DWORD  ch_type = otd_addr_type(&checked);
  return ( (a_type == OTD_ADDR_TYPE_BROADCAST && ch_type == OTD_ADDR_TYPE_PU)||
           (a_type == OTD_ADDR_TYPE_PU && ch_type == OTD_ADDR_TYPE_CP)       ||
           (a_type == OTD_ADDR_TYPE_CP && ch_type == OTD_ADDR_TYPE_SB) )  ? true:false;
}


DWORD   __fastcall otd_dblow::update_diag      (otd_addr & addr)
{
 //Обновление состояния диагностики
 //Возвращает маску изменений
 //TODO Переделать так чтобы обновлялись и все подообъекты
 DWORD ret = 0 ;
 DWORD idx = find_byaddr(addr);
 if(idx<otd_list.size())
 {
  proto_pointer ptr = otd_list.begin();
  advance(ptr,idx);
  DWORD n_diag = get_diag(ptr,true);
  if(ptr->op.diag )
  {
   ret = ret = (*ptr->op.diag) ^ n_diag;
   if(ret)
   {
   *ptr->op.diag = n_diag;
    sotd_addr sa(addr);
    sa = sa.owner_addr();
    update_diag(sa);
   } 
  } 
 }
 return ret;
}

DWORD   __fastcall otd_dblow::set_diag (otd_addr & addr, DWORD diag,bool set)
{
 DWORD  ret = OTD_DIAG_NODATA;
 DWORD  idx = this->find_byaddr(addr);
 if(idx<this->otd_list.size())
 {
  proto_pointer ptr = otd_list.begin();
  advance(ptr,idx);
  ret = set_diag(ptr,diag,set);
 }
 return ret;
}

DWORD   __fastcall otd_dblow::set_diag (proto_pointer ptr, DWORD diag,bool set)
{
  //Установка/сброс битов диагностики
  DWORD ret = 0;
  if(ptr->op.diag)
  {
   DWORD addr_type = otd_addr_type(ptr->op.addr);
   switch(addr_type)
   {
    case OTD_ADDR_TYPE_SB: ret = set_sb_diag(ptr,diag,set); break;
    case OTD_ADDR_TYPE_CP: ret = set_cp_diag(ptr,diag,set); break;
    case OTD_ADDR_TYPE_PU: ret = set_pu_diag(ptr,diag,set); break;
   }
  }
 return  ret;
}


DWORD __fastcall otd_dblow::set_sb_diag(proto_pointer ptr, DWORD diag,bool set)
{
  //Установка/сброс диагностики Модуля

  DWORD new_diag = set ? (*ptr->op.diag)|diag : (*ptr->op.diag)&~diag;
  DWORD ret = new_diag ^ (*ptr->op.diag);
  if(ret)
    {
      if(set)
      {
      if(ret & OTD_DIAG_MASK)
        {
         //Что - то сдохдло ПУ,КП или модуль
         new_diag|=OTD_PART_DIAG_PARAM;
         DWORD value = OTD_PDIAG_NODATA;
         otd_fill(ptr->op.personal_diag,&value,sizeof(value));
         *ptr->op.diag = new_diag;         
        }
      }
      else
      {
        //Стало работоспособным
        *ptr->op.diag = new_diag;
        if(ret & OTD_DIAG_CPMASK)
          {
           sotd_addr sa = (*ptr->op.addr);
           sa = sa.owner_addr();
           sa = sa.owner_addr();
           set_diag(sa,diag|OTD_DIAG_NODATA|OTD_PART_DIAG_PARAM,set);
          }
      }

    }
  return ret;
}

DWORD __fastcall otd_dblow::set_pu_diag(proto_pointer ptr, DWORD diag,bool set)
{
  //Установка/сброс диагностики ПУ

  DWORD new_diag = set ? (*ptr->op.diag)|diag : (*ptr->op.diag)&~diag;
  DWORD ret = new_diag ^ (*ptr->op.diag);
  if(ret&(OTD_DIAG_PUMASK|OTD_DIAG_NODATA))
    {
     *ptr->op.diag = new_diag;
     if(set)
     {
       if(ret & OTD_DIAG_PUMASK)
        {
          diag |= (~OTD_DIAG_PUMASK);
          diag&=(~OTD_DIAG_NODATA)&OTD_DIAG_FULL_MASK;
          *ptr->op.diag = new_diag;
          //Сдохло ПУ
          proto_pointer lptr = otd_list.begin() ,hptr = ptr;
          if(get_range(*ptr->op.addr,lptr,hptr))
          {
            while(lptr<hptr)
            {
             set_diag(lptr,diag,set);
             ++lptr;
            }
          }
        }
     }
     else
     {

      *ptr->op.diag = get_diag(ptr,true);
     } 
    }
    else
    if(ret)
     {
      //*ptr->op.diag = get_diag(ptr,true);
     }
  return ret;
}

DWORD __fastcall otd_dblow::set_cp_diag(proto_pointer ptr, DWORD diag,bool set)
{
 //Установка/сброс диагностики КП

  DWORD new_diag = set ? (*ptr->op.diag)|diag : (*ptr->op.diag)&~diag;
  DWORD ret = new_diag ^ (*ptr->op.diag);
  sotd_addr  pu_addr(*ptr->op.addr);
  pu_addr = pu_addr.owner_addr();
  if(ret)
    {
     if(set)
     {
       if(ret & (OTD_DIAG_CPMASK|OTD_DIAG_NODATA))
        {
          diag |= ~(OTD_DIAG_PUMASK|OTD_DIAG_NODATA);
          diag&=(~OTD_DIAG_NODATA)&OTD_DIAG_FULL_MASK;
          *ptr->op.diag = ((new_diag|(~OTD_DIAG_CPMASK)|OTD_PART_DIAG_PARAM)&~OTD_DIAG_NODATA)&OTD_DIAG_FULL_MASK;
          proto_pointer lptr = otd_list.begin() ,hptr = ptr;
          if(get_range(*ptr->op.addr,lptr,hptr))
          {
            while(lptr<hptr)
            {
             set_diag(lptr,diag,set);
             ++lptr;
            }
          }
          update_diag(pu_addr);
        }
     }
     else
     {
       *ptr->op.diag = new_diag;
       if(ret&OTD_DIAG_CPMASK)
       {
        sotd_addr sa = *ptr->op.addr;
        sa = sa.owner_addr();
        set_diag(sa,diag|OTD_DIAG_NODATA|OTD_PART_DIAG_PARAM,set);
       }
      *ptr->op.diag = get_diag(ptr,true);
     }

     
    }
    

  return ret;
}




 