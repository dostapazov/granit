//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <KeRTL.hpp>

//---------------------------------------------------------------------------
USEFORM("tlm_viewer_main.cpp", TlmViewer);
USEFORM("TIpClientForm.cpp", IPClient);
USEFORM("tlm_painter_frm.cpp", PainterForm);
USEFORM("LayoutEdit.cpp", LaoutEdit);
//---------------------------------------------------------------------------
struct s_int
{
 int i;
 s_int(int _v):i(_v){}
 s_int():i(0){}
 s_int(const s_int & si){*this = si;}
 s_int & operator = (const s_int & si){i = si(); return * this;}
 int     operator()(){return i;}
 int     operator()() const
 {return i;}

};

int operator  == (const s_int & si,const s_int & si1)
{
 return si() == si1();
}

s_int  operator  + (const s_int & si,const s_int & si1)
{
 return s_int(si()-si1());
}


//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
        try
        {
                 Application->Initialize();
                 Application->CreateForm(__classid(TTlmViewer), &TlmViewer);
                 Application->CreateForm(__classid(TIPClient), &IPClient);
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
       //SetMemoryManager(old_mm);

        return 0;
}
//---------------------------------------------------------------------------
