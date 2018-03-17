// Заголовок Окна Диалога
// GranitKernel32 V 1.0
// Остапенко Д.В. СДТУ 1993-2000

#ifndef GRKE_DIALOGS_INC
#define GRKE_DIALOGS_INC

#ifndef GRKE_WINDOW_INC
 #include <gkwind.hpp>
#endif 

#pragma pack (push,8)

#ifndef _NO_NAMESPACE
#ifndef KrnlWin
#define KrnlWin KeWin
#endif
namespace KeWin
{
#endif

 KEWIN_CLASS TDialog:public TWindow
 {
  protected:
  static  BOOL     CALLBACK DlgProc(HWND hWnd,UINT Msg,WPARAM wp,LPARAM lp);
  static  LRESULT  CALLBACK DefaultProc(HWND hWnd,UINT Msg,WPARAM wp,LPARAM lp);

  HWND    Parent;
  LPSTR   ResId;
  bool    ModalDialog;
  virtual bool __fastcall OnWMInitDialog();
  public:
    TDialog(HWND Parent,LPSTR ResId,HINSTANCE hinst = GetModuleHandle(0));
             bool   __fastcall OnWMCommand   (HWND From,int nCode,int id);
             bool   __fastcall DoCreate();
             int    __fastcall ShowModal();
             bool   __fastcall IsModal();
             void   __fastcall EndModal(int eCode);

 };

 inline TDialog::TDialog(HWND Parent,LPSTR ResId,HINSTANCE inst):TWindow(inst)
 {
  this->Parent = Parent;
  this->ResId = ResId;
  ModalDialog = false;
 };

inline  bool __fastcall TDialog::IsModal()
  { return ModalDialog; }

#ifndef _NO_NAMESPACE
}
#endif

#pragma pack (pop)

#endif

