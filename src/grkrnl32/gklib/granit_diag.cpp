#pragma hdrstop
#include <granit_dos.h>
#include <otd.h>


 WORD __fastcall gd_get_mask(DWORD fa,DWORD grp)
 {
   if(fa == tpTs)
     return  grp<9 ? ((1<<grp)&GRDIAG_TS_MASK):GRDIAG_TS_MASK;
   if(fa == tpTit)
     return  grp<8 ? ((0x100<<grp)&GRDIAG_TIT_MASK):GRDIAG_TIT_MASK;
   return GRDIAG_TIT_MASK|GRDIAG_TS_MASK;
 }


 DWORD __fastcall gd_set_diag(lp_grdiag_item gdi,WORD diag)
 {
  /*��������� ��������� ����������� */
  /*������������ ����� ���������*/
   WORD chs = gdi->state^diag ,chm = 0;
   if(chs)
   {
   gdi->state  = diag;
   if(diag&GRDIAG_CP)
     {
      //�� ������
       
       diag = -1;//�� ������ �����
       if(gdi->mask)
         chm  = ~gdi->mask;
       gdi->mask = 0;
      }
     else
     {
      diag  = ~diag;
      chm  |= gdi->mask^diag;
      gdi->mask|=diag;
     } 
   }
   return MAKELONG(chs,chm);
 }

 DWORD __fastcall gd_mark_exist(lp_grdiag_item gdi,DWORD fa,DWORD grp)
 {
  /*�������� ��� ������������ ������*/
  /*������� ����� ���������*/
  WORD mask = gd_get_mask(fa,grp)|GRDIAG_CP;
  WORD chm  = gdi->mask|mask;
  chm = gdi->mask^chm;
  gdi->mask|=mask;
  return MAKELONG(0,chm);
 }

 DWORD __fastcall gd_set_nowork(lp_grdiag_item gdi,DWORD fa,DWORD grp,BOOL no_work)
 {
  /*���������� ����������������� ������*/
  /*������� ����� ���������*/
  DWORD ret ;
  DWORD mask = gd_get_mask(fa,grp);
  ret = gd_mark_exist(gdi,fa,grp);
  ret |= gd_set_diag(gdi,no_work ? (WORD)(gdi->state|mask):(WORD) (gdi->state&~(mask|GRDIAG_CP)));
  return ret;
 }

 BOOL  __fastcall gd_is_exist (lp_grdiag_item gdi,DWORD fa,DWORD grp)
 {
  /*�������� ������������� ������*/
  WORD mask = gd_get_mask(fa,grp);
  return ((gdi->mask&GRDIAG_CP) && (gdi->mask & mask)) ? TRUE:FALSE;
 }

 BOOL  __fastcall gd_is_nowork(lp_grdiag_item gdi,DWORD fa,DWORD grp)
 {
  /*�������� ������������������� ������*/
  WORD mask = gd_get_mask(fa,grp);
  if(gdi->mask & mask)
  {
   return (gdi->state&(mask|GRDIAG_CP)) ? TRUE:FALSE;
  }
  return FALSE;
 }

 DWORD __fastcall gd_get_otd_diag(lp_grdiag_item gdi,DWORD fa,DWORD grp)
 {
    DWORD ret = OTD_DIAG_NODATA;
   //�������������� ����������� ������� �  OTD
   WORD mask = gd_get_mask(fa,grp);
   if(gdi->mask&GRDIAG_CP)// �� ����������
      ret = (gdi->state&GRDIAG_CP) ? (OTD_DIAG_CPCONNECT|OTD_DIAG_CPRESPOND):0;
   if(gdi->mask & mask)
     {
      //������ ����������
      ret|= (gdi->state&mask) ? OTD_DIAG_MODRESPOND:0;
     }

   return ret;
 }

 DWORD __fastcall gd_set_otd_diag(lp_grdiag_item gdi,DWORD fa,DWORD grp,DWORD otd_diag)
 {
   //��������� ����������� �� ������ OTD �����������
   DWORD ret = 0;
     if(otd_diag&(OTD_DIAG_PUCONNECT|OTD_DIAG_PURESPOND|OTD_DIAG_CPCONNECT|OTD_DIAG_CPRESPOND))
        {
         ret = gd_set_diag(gdi,gdi->state|GRDIAG_CP);
        }
        ret |= gd_set_nowork(gdi,fa,grp,(otd_diag&OTD_DIAG_MASK) ? TRUE:FALSE);
   return ret;

 }






