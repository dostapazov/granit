//---------------------------------------------------------------------------

#include <windows.h>
#pragma hdrstop
#include "GKReporterEx.hpp"

#pragma argsused
//---------------------------------------------------------------------------
module_starter<TGKReporterEx> ms(MT_REPORTER);

LRESULT WINAPI module_main(DWORD cmd,LPARAM p1,LPARAM p2)
{
  return ms(cmd,p1,p2);
//  //только один экземпл€р внешнего репортера загружаетс€
//  LRESULT ret = NULL;
//  switch(cmd)
//  {
//    case GKME_CMD_GET_MODULETYPE: ret = MT_REPORTER;break;
//    case GKME_CMD_CREATE_HANDLE :
//    {
//      wchar_t dll_name[MAX_PATH<<1];
//      MODULE_INFO mi;
//      mi.dw_size=sizeof(mi);
//      mi.mi_mask=MI_FL_DLL_NAME;
//      mi.dll_name=dll_name;
//      mi.dll_name_cbsz=sizeof(dll_name)/sizeof(wchar_t);
//      if (!call_gkhandle_bynameW(REPORTER_NAME_W,MCMD_GETMODULE_INFO,LPARAM(&mi),0,0) ||
//          ((wcslen(mi.dll_name)==0) && !find_gkhandleW(REPORTEREX_NAME_W)))
//      {
//        TGKReporterEx *reporter=new TGKReporterEx;
//        ret=(LRESULT)(*reporter)();
//        if (!ret)
//          delete reporter;
//      }
//    }
//    break;
//  }
//  return ret;
}

