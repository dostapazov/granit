//---------------------------------------------------------------------------

#ifndef ReadLogThreadH
#define ReadLogThreadH
//---------------------------------------------------------------------------
#include <ComCtrls.hpp>
#include <vector>
#include <map>
#include <KeRTLio.hpp>
#include <kethreads.hpp>
#include <gkhandle.h>
//---------------------------------------------------------------------------
using namespace KeRTL;

struct TViews
{
  AnsiString name;
  TFile *log;
  DWORD read_pos;     //последнее прочитанное сообщение для чтения без индекса по времени
  int count;
  int index;            //текущая позиция в ListView
  LPVOID log_function;
  vector<DWORD> time_ind;
  bool time_ind_ready;
};

class TReadLogThread : public KeRTL::TThread
{
private:
        vector<TViews *> v_logs;
        void __fastcall ReadError(AnsiString desc);
        AnsiString __fastcall ToStringByte(LPVOID Data, int Code);
        DWORD __fastcall NextPosition(TViews *v, DWORD curpos, bool worward);
        DWORD __fastcall AddTimeInd(TViews *v, DWORD first_rec);
protected:
        int __fastcall Execute();
public:
        TReadLogThread();
        ~TReadLogThread();
        void __fastcall PreRead(int registred, bool last=false);
        DWORD __fastcall Read(TViews *v, DWORD pos, byte *buf, DWORD buf_size);
        DWORD __fastcall ReadByTime(TViews *v, int num, byte *buf, DWORD buf_size, bool growth);
        void __fastcall AddLog(TViews *v);
        void __fastcall InsertLog(UINT v_num, TViews *v);
        void __fastcall UpdateTimeInd(TViews *v, int end);
        DWORD __fastcall NewEvent(TViews *v, bool sound);
        void __fastcall ClearLog(UINT v_num);
        AnsiString __fastcall SetUpDesc(TViews *v);
        AnsiString __fastcall SetUpData(TViews *v, int Code);
        DWORD __fastcall GetSize(UINT v_num);
        int __fastcall FileRegistered(AnsiString name);
        void __fastcall SetEventFunc(DWORD source, DWORD dest);
};
//---------------------------------------------------------------------------

#endif
