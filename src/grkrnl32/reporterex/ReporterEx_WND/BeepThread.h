//---------------------------------------------------------------------------

#ifndef BeepThreadH
#define BeepThreadH
//---------------------------------------------------------------------------
#include <Classes.hpp>
//---------------------------------------------------------------------------
class TSpeakerBeep : public Classes::TThread
{            
private:
protected:
        void __fastcall Execute();
public:
        __fastcall TSpeakerBeep(bool CreateSuspended);
};
//---------------------------------------------------------------------------
#endif
