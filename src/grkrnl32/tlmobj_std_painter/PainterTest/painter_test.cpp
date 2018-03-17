//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
//---------------------------------------------------------------------------
#include <Vcl.Styles.hpp>
#include <Vcl.Themes.hpp>
USEFORM("..\..\MediumDB\MediumDbWnd\paint_var_constructor.cpp", PaintVariantConstruct);
USEFORM("tlmentry_opts_frame.cpp", tlm_entry_opts_frame); /* TFrame: File Type */
USEFORM("test_form.cpp", TlmTestFrom);
USEFORM("..\..\GKLIB\VCL\tlm_painter_form.cpp", TlmPainterForm);
//---------------------------------------------------------------------------
int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
	try
	{
		Application->Initialize();
		Application->MainFormOnTaskBar = true;
		Application->CreateForm(__classid(TTlmTestFrom), &TlmTestFrom);
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
