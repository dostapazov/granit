#ifndef _GKTLM_VIEWER_INC_
#define _GKTLM_VIEWER_INC_

#include <gkmodules.h>
#include <modemmer.h>

#ifndef RC_INVOKED

#endif //RC_INVOKED

#define TLMVIEWER_MOD_NAME L"GKTLM_VIEWER"
#define TLMVIEWER_WND_NAME L"GKTLM_VIEWER_WND"


#define REGDW_EXTERNAL_SOURCE   L"external_source"
#define REGSTR_DATA_FOLDER      L"data_folder"
#define REGSTR_EXPORT_FOLDER    L"export_folder"
#define REGSTR_EXPORT_VIEWER    L"export_viewer"
#define REGSTR_AUTO_OPEN_LAYOUT L"auto_open_layout"
#define REGSTR_SND_DISPARITY    L"snd_disparity"
#define REGSTR_SND_BACK         L"snd_back"
#define REGDW_DOUBLE_BUFFERED   L"double_buffered"
#define REGDW_TIME_BEFORE       L"time_before"
#define REGDW_TIME_AFTER        L"time_after"
#define REGDW_READY_COLOR       L"ready_color"
#define REGDW_BKGND_NOGRAY      L"bkgnd_nogray"
#define REGDW_HKEY_CLEARALL     L"hk_clear_all"
#define REGDW_HKEY_RECEIPTALL   L"hk_receipt_all"
#define REGDW_HKEY_ORDER_PERIOD L"order_period"
#define REGDW_HKEY_STABLE_READY_TIMER L"stable_ready_timer"
#define REGDW_HKEY_READY_COLORS L"ready_colors"
#define REGBIN_READY_FONT       L"ready_font"
#define REGDW_DISPLAY_NUMBER    L"display_number"
#define REGDW_READY_SPACER      L"ready_spacer"


#define KADRS_FILE_NAME       L"kadrs_ordered.data"
#define NAMES_FILE_NAME       L"names.data"
#define RECORDS_FILE_NAME     L"records.data"

#define CCFL_PARAMS   0x0001000
#define CCFL_KADRS    0x0002000
#define CCFL_NAMES    0x0004000
#define CCFL_RECORDS  0x0008000

#ifdef __cplusplus
#include <string>

struct Tviewer_parameters
{
  BOOL                    external_source;
  MODEM_ADDR              modem_source;

  std::wstring            data_dir;
  std::wstring            export_dir;
  std::wstring            auto_open_layout;
  std::wstring            export_folder;
  std::wstring            export_viewer;

  DWORD                   history_time_before;
  DWORD                   history_time_after;

  BOOL                    enable_double_buffered ;
  WORD                    hot_key_clear_all;
  WORD                    hot_key_receipt_all;

  DWORD                   ready_color;
  READY_COLORS            ready_colors;
  int                     ready_spacer;
  BOOL                    same_ready_size;
  LOGFONT                 ready_font;


  int                    order_period;
  int                    stable_ready_timer;

  //Файлы звуков
  std::wstring           snd_back;
  std::wstring           snd_disparity;
  int                    main_form_monitor;

  Tviewer_parameters   ()
                       : external_source(0)
                       , data_dir              (L"gktlm_viewer_data")
                       , enable_double_buffered(false)
                       , hot_key_clear_all  (0)
                       , hot_key_receipt_all(0)
                       , ready_color        (32768)
                       , ready_spacer       (2)
                       , same_ready_size    (FALSE)
                       , order_period       (0)
                       , stable_ready_timer (0)
                       , main_form_monitor  (0)
  {
   modem_source.addr = 0;
   bzero(&ready_font     ,sizeof(ready_font  ));
   bzero(&ready_colors    ,sizeof(ready_colors));
   ready_colors.dw_size = sizeof(ready_colors);
  }

  bool __fastcall write(HKEY reg_key);
  bool __fastcall read (HKEY reg_key);
};

#endif






#endif //#define _GKTLM_VIEWER_INC_
