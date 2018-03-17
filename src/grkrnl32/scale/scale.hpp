#include <gkmodules.hpp>
#include <scale.h>
#include <TIndexedFile.hpp>

class TGKScale: public TGKModule
{
  private:
        wchar_t *file_name;
        TIndexedFile *file;
        bool __fastcall check_version();
        TScale * __fastcall get_scale(DWORD num, DWORD size, bool by_index);
        DWORD __fastcall cmd_calc(LPSC_CALC c, bool direct);
        DWORD __fastcall cmd_enum(DWORD num, LPSCALE_ENUM enum_par);
        DWORD __fastcall cmd_select(wchar_t * name, DWORD * id);
        DWORD __fastcall cmd_select( char   * name, DWORD * id);
        DWORD __fastcall cmd_count(DWORD * c);
        DWORD __fastcall cmd_create(byte * buf, DWORD * id);
        DWORD __fastcall cmd_delete(DWORD id);
        DWORD __fastcall cmd_size(DWORD id, DWORD * size);
        DWORD __fastcall cmd_set(DWORD id, byte * buf);
        DWORD __fastcall cmd_get(DWORD id, byte * buf);
        DWORD __fastcall cmd_get_filename(wchar_t * name, int * size);
        DWORD __fastcall cmd_get_filename( char   * name, int * size);
        DWORD __fastcall cmd_set_filename(wchar_t * name, int operation);
        DWORD __fastcall cmd_set_filename( char   * name, int operation);
        DWORD __fastcall cmd_shrink();
        DWORD __fastcall cmd_index_used(bool * indexed);
        DWORD __fastcall cmd_file_size(DWORD * sz);
        DWORD __fastcall cmd_file_unused(DWORD * sz);
        DWORD __fastcall cmd_minmax(LPSC_MINMAX mm);
  protected:
        TFASTMutex locker;
        WORD    __fastcall get_type();
        DWORD   __fastcall get_config_data(DWORD mask,LPVOID buf,DWORD bsz);
        bool    __fastcall set_config_data(DWORD mask,LPVOID buf,DWORD bsz);
        bool    __fastcall compare_config_data(DWORD mask,LPVOID buf,DWORD bsz,LPDWORD ch_mask,BOOL * restart);
        bool    __fastcall read_settings ();
        bool    __fastcall save_file_name();
        LRESULT __fastcall processing(DWORD cmd,LPARAM p1,LPARAM p2);
        DWORD __fastcall get_window_module(wchar_t * buf,DWORD bsz);
        DWORD __fastcall start(DWORD reason,LPARAM p2);
        DWORD __fastcall stop (DWORD reason);
        virtual  DWORD      __fastcall check_license(){return MOD_LICENSE_REGISTERED; }
  public:
        TGKScale();
        ~TGKScale();
        DWORD __fastcall full_unlock();
};
