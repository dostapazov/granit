//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------------
USEFORM("ipmodem_wrk.cpp", IpModemSampleFrm);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, TCHAR*, int)
{
        try
        {
                 Application->Initialize();
                 Application->CreateForm(__classid(TIpModemSampleFrm), &IpModemSampleFrm);
		Application->Run();
        }
        catch (Exception &exception)
        {
                 Application->ShowException(&exception);
        }
        catch (...)
        {
                 try
                 {
                         throw Exception("");
                 }
                 catch (Exception &exception)
                 {
                         Application->ShowException(&exception);
                 }
        }
        return 0;
}
//---------------------------------------------------------------------------
int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE hPrevInst, TCHAR * cmd_line ,int cmd_show)
{
  return WinMain(hInst,hPrevInst,cmd_line,cmd_show);
}

