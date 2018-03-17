
#ifndef GRKE_COMMCTRL_INC
#define GRKE_COMMCTRL_INC


#include <gkwind.hpp>
#include <commctrl.h>
#include <vc5core.h>
#include <shlwapi.h>
#include <keclosure.hpp>
//winuser.h

#ifndef _NO_NAMESPACE
namespace KrnlWin
{
#endif


KEWIN_CLASS TCommonControl:public TWindow
{
 protected:
 static  DWORD CtrlLoaded;
 static  DLLVERSIONINFO2 version;

 public:
 void          __fastcall CheckAndLoad(TCommonControl * ctrl);
 protected:
 virtual DWORD __fastcall GetControlFlag()=0;
         int   __fastcall OnWMNotify(int CtrlId,LPNMHDR mnhdr);
 LRESULT      __fastcall MessageHandler(MSG & msg);

 typedef int ( __stdcall * FN_ONCTRLNOTIFY)(LPVOID,TCommonControl*,LPNMHDR);
 TClosureMethod<LPVOID,TCommonControl*,LPNMHDR,int> _OnChar;
 TClosureMethod<LPVOID,TCommonControl*,LPNMHDR,int> _OnKey;
 TClosureMethod<LPVOID,TCommonControl*,LPNMHDR,int> _OnClick;
 TClosureMethod<LPVOID,TCommonControl*,LPNMHDR,int> _OnHover;
 TClosureMethod<LPVOID,TCommonControl*,LPNMHDR,int> _OnSetFocus;
 TClosureMethod<LPVOID,TCommonControl*,LPNMHDR,int> _OnNCHitTest;
 TClosureMethod<LPVOID,TCommonControl*,LPNMHDR,int> _OnOutOfMemory;
 TClosureMethod<LPVOID,TCommonControl*,LPNMHDR,int> _OnReleasedCapture;
 TClosureMethod<LPVOID,TCommonControl*,LPNMHDR,int> _OnReturn;
 TClosureMethod<LPVOID,TCommonControl*,LPNMHDR,int> _OnSetCursor;

 virtual int __fastcall OnChar(LPNMCHAR p );
 virtual int __fastcall OnKey(LPNMKEY p);
 virtual int __fastcall OnClick    (LPNMHDR p);
 virtual int __fastcall OnHover    (LPNMHDR p);
 virtual int __fastcall OnSetFocus (LPNMHDR p,bool);
 virtual int __fastcall OnNCHitTest(LPNMMOUSE p);
 virtual int __fastcall OnOutOfMemory(LPNMHDR p);
 virtual int __fastcall OnReleasedCapture(LPNMHDR p);
 virtual int __fastcall OnReturn(LPNMHDR p);
 virtual int __fastcall OnSetCursor(LPNMMOUSE p);

 public:
 TCommonControl(HINSTANCE hInstance = GetModuleHandle(0));
 void __fastcall SetNMCharCallback    (LPVOID data,FN_ONCTRLNOTIFY);
 void __fastcall SetNMKeyCallback     (LPVOID data,FN_ONCTRLNOTIFY);
 void __fastcall SetNMClickCallback   (LPVOID data,FN_ONCTRLNOTIFY);
 void __fastcall SetNMHoverCallback   (LPVOID data,FN_ONCTRLNOTIFY);
 void __fastcall SetNMSetFocusCallback(LPVOID data,FN_ONCTRLNOTIFY);
 void __fastcall SetNMNCHitTest       (LPVOID data,FN_ONCTRLNOTIFY);
 void __fastcall SetNMOutOfMemory     (LPVOID data,FN_ONCTRLNOTIFY);
 void __fastcall SetNMReleasedCapture (LPVOID data,FN_ONCTRLNOTIFY);
 void __fastcall SetNMReturn          (LPVOID data,FN_ONCTRLNOTIFY);
 void __fastcall SetNMSetCursor       (LPVOID data,FN_ONCTRLNOTIFY);
 static DWORD __fastcall GetDllVersion();

};

// Tooltips control

KEWIN_CLASS TToolInfoA:public TOOLINFOA
{
 public:
 TToolInfoA(){ZeroMemory(this,sizeof(*this)); cbSize = sizeof(TOOLINFOA);}
 TToolInfoA(HWND Ctrl,char  * Text = (char*)LPSTR_TEXTCALLBACK,DWORD Flags=0,HINSTANCE hInst =0 ,bool subClass=true);
 TToolInfoA(int Ctrl, TRect & r,char  * Text= (char*)LPSTR_TEXTCALLBACK,DWORD Flags=0,HINSTANCE hInst =0 ,bool subClass=true);

 void       __fastcall SetRect(TRect & r){*((TRect*)&rect) = r;}
 TRect      __fastcall GetRect(){return rect;}
 void       __fastcall SetWindow(HWND w){ hwnd = (::HWND)w;}
};

KEWIN_CLASS TToolInfoW:public TOOLINFOW
{

 public:
 TToolInfoW(){ZeroMemory(this,sizeof(*this)); cbSize = sizeof(TOOLINFOA);}
 TToolInfoW(HWND Ctrl,wchar_t  * Text =  (wchar_t*)LPSTR_TEXTCALLBACK,DWORD Flags=0,HINSTANCE hInst =0 ,bool subClass=true);
 TToolInfoW(int Ctrl, TRect & r,wchar_t  * Text=(wchar_t*) LPSTR_TEXTCALLBACK,DWORD Flags=0,HINSTANCE hInst =0 ,bool subClass=true);

 void       __fastcall SetRect(TRect & r){*((TRect*)&rect) = r;};
 TRect      __fastcall GetRect(){return rect;}
 void       __fastcall SetWindow(HWND w){ hwnd = (::HWND)w;}

};

KEWIN_CLASS TImageList:public KeRTL::TKeRTLBase
{
  HIMAGELIST hImageList;
  void       __fastcall Release(){if(hImageList && !IsFlagsSet(FLBASE_NODELETE))ImageList_Destroy(hImageList);hImageList = 0;}

  public:
  TImageList(HIMAGELIST list):hImageList(list){SetFlags(FLBASE_NODELETE,true);}
  TImageList(int cx,int cy,UINT flg = ILC_COLORDDB,int initial = 32, int cGrow = 32)
  :hImageList(ImageList_Create(cx,cy,flg,initial,cGrow)){}
  ~TImageList(){Release();}

  int          __fastcall  GetCount();
  int          __fastcall  Add(HBITMAP bmp,HBITMAP mask );
  int          __fastcall  Add(HBITMAP bmp,COLORREF mask_color=0);
  bool         __fastcall  Remove(int idx);
  TImageList    & operator +=(HBITMAP bmp){Add(bmp);return *this;}
  TImageList    & operator +=(HIMAGELIST list);//Merge
  TImageList    & operator = (HIMAGELIST list);//Copy
  HIMAGELIST      operator()(){return hImageList;}
};


inline   int          __fastcall  TImageList::Add(HBITMAP bmp,HBITMAP mask )
{ return ImageList_Add(hImageList,bmp,mask);}

inline   int          __fastcall  TImageList::Add(HBITMAP bmp,COLORREF mask_color)
{ return ImageList_AddMasked(hImageList,bmp,mask_color);}

inline   bool         __fastcall  TImageList::Remove(int idx)
{ return ImageList_Remove(hImageList,idx) ? true:false;}

inline   int          __fastcall  TImageList::GetCount()
{  return ImageList_GetImageCount(hImageList);}

inline   TImageList    & TImageList::operator +=(HIMAGELIST list)
{ImageList_Merge(hImageList,0,list,0,0,0);return *this;}

inline   TImageList    & TImageList::operator = (HIMAGELIST list)
{return *this;}


KEWIN_CLASS TToolTips:public TCommonControl
{
 protected:
 DWORD    __fastcall GetControlFlag(){return ICC_TREEVIEW_CLASSES;};
 LRESULT  __fastcall MessageHandler(MSG & msg);
 int      __fastcall OnWMNotify(int CtrlId,LPNMHDR mnhdr);

 TClosureMethod<LPVOID,TToolTips*, TToolInfoA*  , bool>   __FnTipTextA;
 TClosureMethod<LPVOID,TToolTips*, TToolInfoW*  , bool>   __FnTipTextW;
 TClosureMethod<LPVOID,TToolTips*,NMTTCUSTOMDRAW*, DWORD> __FnCustomDraw;

 virtual  bool  __fastcall GetTipText(TToolInfoA &);
 virtual  bool  __fastcall GetTipText(TToolInfoW &);
 virtual  DWORD __fastcall CustomDraw(NMTTCUSTOMDRAW *);

 public:
  TToolTips(HINSTANCE hInst= GetModuleHandle(0)):TCommonControl(hInst){};
  bool  __fastcall DoCreate(DWORD Style = TTS_ALWAYSTIP);
  void  __fastcall EnableTips(bool enable);

  bool     __fastcall AppendControl(HWND Ctrl,char  * Text= (char*)LPSTR_TEXTCALLBACK,DWORD Flags=0,HINSTANCE hInst =0 ,bool subClass=true);
  bool     __fastcall AppendControl(HWND parent,int  id  ,char  * Text= (char*)LPSTR_TEXTCALLBACK,DWORD Flags=0,HINSTANCE hInst =0 ,bool subClass=true);
  bool     __fastcall AppendControl(HWND Ctrl,wchar_t  * Text=(wchar_t*) LPSTR_TEXTCALLBACK,DWORD Flags=0,HINSTANCE hInst =0 ,bool subClass=true);
  bool     __fastcall AppendControl(HWND parent ,int  id  ,wchar_t  * Text= (wchar_t*)LPSTR_TEXTCALLBACK,DWORD Flags=0,HINSTANCE hInst =0 ,bool subClass=true);

  bool     __fastcall RemoveControl(HWND Ctrl);
  bool     __fastcall RemoveControl(HWND p,int id);
  bool     __fastcall GetToolInfo(HWND ctrl,TToolInfoA &);
  bool     __fastcall GetToolInfo(HWND parent,int id,TToolInfoA &);
  bool     __fastcall GetToolInfo(HWND ctrl,TToolInfoW &);
  bool     __fastcall GetToolInfo(HWND parent,int id,TToolInfoW &);

  COLORREF __fastcall GetTipBkColor();
  COLORREF __fastcall GetTipTextColor();

  void     __fastcall SetTipBkColor  (COLORREF);
  void     __fastcall SetTipTextColor(COLORREF);

  enum     TTitleIcon { ttNoIcon,ttInfoIcon,ttWarningIcon,ttErrorIcon};
  bool     __fastcall SetTitle(TTitleIcon icon,char * text);
  bool     __fastcall SetTitle(TTitleIcon icon,wchar_t * text);

  enum     TDelayTime {dtAutoPop = TTDT_AUTOPOP,dtInitial = TTDT_INITIAL,dtReshow = TTDT_RESHOW,dtAutomatic = TTDT_AUTOMATIC};
  void     __fastcall SetDelayTime(TDelayTime,int time);
  int      __fastcall GetDelayTime(TDelayTime);

  int      __fastcall GetMaxWidth();
  void     __fastcall SetMaxWidth(int w);

  void     __fastcall UpdateText(HWND Control,char * text,HINSTANCE hInst);
  void     __fastcall UpdateText(int  Control,char * text,HINSTANCE hInst);
  void     __fastcall UpdateText(HWND Control,wchar_t * text,HINSTANCE hInst);
  void     __fastcall UpdateText(int  Control,wchar_t * text,HINSTANCE hInst);

  void     __fastcall RelayEvent(MSG & msg);

 typedef  bool ( __stdcall * FN_TIPTEXTA  )(LPVOID,TToolTips*,TToolInfoA *);
 typedef  bool ( __stdcall * FN_TIPTEXTW  )(LPVOID,TToolTips*,TToolInfoW *);
 typedef  DWORD( __stdcall * FN_CUSTOMDRAW)(LPVOID,TToolTips*,NMTTCUSTOMDRAW*);

  void     __fastcall SetCustomDrawCallback(LPVOID,FN_CUSTOMDRAW);
  void     __fastcall SetTipTextCallback(LPVOID,FN_TIPTEXTA);
  void     __fastcall SetTipTextCallback(LPVOID,FN_TIPTEXTW);

};

KERTL_CLASS TListView:public TCommonControl
{
  public:
  typedef int ( __stdcall * FN_LVNOTIFY)  (LPVOID,TListView*,LPNM_LISTVIEW);
  typedef int ( __stdcall * FN_DISPNOTIFY)(LPVOID,TListView*,LV_DISPINFO*);

  TClosureMethod<LPVOID,TListView*,LPNM_LISTVIEW,int> _OnBeginDrag   ;
  TClosureMethod<LPVOID,TListView*,LPNM_LISTVIEW,int> _OnItemChanged ;
  TClosureMethod<LPVOID,TListView*,LPNM_LISTVIEW,int> _OnItemChanging;
  TClosureMethod<LPVOID,TListView*,LPNM_LISTVIEW,int> _OnDeleteItem  ;
  TClosureMethod<LPVOID,TListView*,LPNM_LISTVIEW,int> _OnColumnClick ;
  TClosureMethod<LPVOID,TListView*,LPNM_LISTVIEW,int> _OnInsertItem  ;

  TClosureMethod<LPVOID,TListView*,LV_DISPINFO*,int > _OnEditLabel  ;
  TClosureMethod<LPVOID,TListView*,LV_DISPINFO*,int > _OnGetDispInfo;
  TClosureMethod<LPVOID,TListView*,LV_DISPINFO*,int > _OnSetDispInfo;

  protected:
  DWORD    __fastcall GetControlFlag(){return ICC_LISTVIEW_CLASSES;};
  int      __fastcall OnWMNotify(int CtrlId,LPNMHDR mnhdr);
  virtual  void __fastcall OnBeginDrag   (LPNM_LISTVIEW hdr,bool end){};
  virtual  void __fastcall OnItemChanged (LPNM_LISTVIEW  hdr){}
  virtual  bool __fastcall OnItemChanging(LPNM_LISTVIEW hdr){return true;}
  virtual  void __fastcall OnDeleteItem  (LPNM_LISTVIEW hdr,bool all){}
  virtual  void __fastcall OnColumnClick (LPNM_LISTVIEW  hdr){}
  virtual  void __fastcall OnInsertItem  (LPNM_LISTVIEW  hdr){}

  virtual  bool __fastcall OnEditLabel   (LV_DISPINFO * hdr, bool end){return true;}
  virtual  void __fastcall OnGetDispInfo (LV_DISPINFO * hdr){};
  virtual  void __fastcall OnSetDispInfo (LV_DISPINFO * hdr){};

  public:

  enum   ViewType {lvIcon = LVS_ICON,lvSmallIcon = LVS_SMALLICON,lvList=LVS_LIST,lvReport=LVS_REPORT};
  TListView(HINSTANCE hInst = GetModuleHandle(0)):TCommonControl(hInst){};
  bool      __fastcall DoCreate(HWND parent,int id);//Создание по ресурсу
  bool      __fastcall DoCreate(HWND parent,TRect & r,DWORD Style,DWORD ExStyle,int id=-1);
  void      __fastcall SetViewType(ViewType v_type);
  ViewType  __fastcall GetViewType();
  COLORREF  __fastcall GetTextColor();
  COLORREF  __fastcall GetBkColor();
  bool      __fastcall SetTextColor(COLORREF);
  bool      __fastcall SetBkColor(COLORREF);
  DWORD     __fastcall SetStyle(DWORD style,bool set ,bool extended = false);
  int       __fastcall InsertColumn(int idx,char * name,int width = -1);
  int       __fastcall InsertColumn(int idx,wchar_t * name,int width=-1);
  int       __fastcall AddColumn(char * name,int width = -1);
  int       __fastcall AddColumn(wchar_t * name,int width=-1);
  bool      __fastcall GetColumn(int index,LVCOLUMNA *,DWORD mask = LVCF_FMT|LVCF_IMAGE|LVCF_ORDER|LVCF_SUBITEM|LVCF_WIDTH);
  bool      __fastcall SetColumn(int index,LVCOLUMNA *,DWORD mask = LVCF_FMT|LVCF_IMAGE|LVCF_ORDER|LVCF_SUBITEM|LVCF_WIDTH);
  bool      __fastcall GetColumn(int index,LVCOLUMNW *,DWORD mask = LVCF_FMT|LVCF_IMAGE|LVCF_ORDER|LVCF_SUBITEM|LVCF_WIDTH);
  bool      __fastcall SetColumn(int index,LVCOLUMNW *,DWORD mask = LVCF_FMT|LVCF_IMAGE|LVCF_ORDER|LVCF_SUBITEM|LVCF_WIDTH);

  int       __fastcall GetColumnText(int index,char * buffer,int bsz);
  int       __fastcall GetColumnText(int index,wchar_t * buffer,int bsz);
  int       __fastcall SetColumnText(int index,char * buffer);
  int       __fastcall SetColumnText(int index,wchar_t * buffer);
  int       __fastcall GetColumnWidth(int idx);
  bool      __fastcall SetColumnWidth(int idx,int width);
  DWORD     __fastcall GetColumnFormat(int idx);
  bool      __fastcall SetColumnFormat(int idx,DWORD fmt);
  bool      __fastcall DeleteColumn(int index);

  DWORD     __fastcall GetItemCount();
  DWORD     __fastcall AddItem   (char    * text,LPVOID param = 0,int image = 0);
  DWORD     __fastcall AddItem   (wchar_t * text,LPVOID param = 0,int image = 0);
  DWORD     __fastcall InsertItem(int idx,char    * text,LPVOID param = 0,int image = 0);
  DWORD     __fastcall InsertItem(int idx,wchar_t * text,LPVOID param = 0,int image = 0);
  void      __fastcall Clear(){ListView_DeleteAllItems((::HWND)hWnd);};
  void      __fastcall DeleteItem(int i){ListView_DeleteItem((::HWND)hWnd,i);}

  bool      __fastcall GetItem(int idx,LVITEMA *,DWORD Mask);
  bool      __fastcall GetItem(int idx,LVITEMW *,DWORD Mask);
  bool      __fastcall SetItem(int idx,LVITEMA *,DWORD Mask);
  bool      __fastcall SetItem(int idx,LVITEMW *,DWORD Mask);

  int       __fastcall GetItemText(int idx,char * buffer,int bsz);
  int       __fastcall GetItemText(int idx,wchar_t * buffer,int bsz);
  int       __fastcall SetItemText(int idx,char * buffer);
  int       __fastcall SetItemText(int idx,wchar_t * buffer);

  int       __fastcall GetSubItemText(int i,int si,char * buffer,int bsz);
  int       __fastcall GetSubItemText(int i,int si,wchar_t * buffer,int bsz);
  int       __fastcall SetSubItemText(int i,int si,char * buffer);
  int       __fastcall SetSubItemText(int i,int si,wchar_t * buffer);

  int       __fastcall FindItem(char * text,int start = -1);
  int       __fastcall FindItem(wchar_t * text,int start = -1);

};

KERTL_CLASS TProgressBar:public TCommonControl
{
  protected:
  WORD  minRange,maxRange,Position;
  int   Step;

  DWORD __fastcall GetControlFlag(){return ICC_PROGRESS_CLASS;}
  public:
  TProgressBar(WORD _minr = 0,WORD _maxr = 100,WORD pos = 0,int _step = 1,HINSTANCE  hInst  = GetModuleHandle(0)):
  TCommonControl(hInst){minRange =  _minr;Step = _step;maxRange = _maxr;Position=pos;};
  bool      __fastcall DoCreate(HWND parent,int id);//Создание по ресурсу
  bool      __fastcall DoCreate(HWND parent,TRect & r,DWORD Style,DWORD ExStyle,int id=-1);
  DWORD     __fastcall SetRange(WORD min_range,WORD max_range)
  {
   minRange = min_range;maxRange = max_range;
   return DWORD(SendMessage(PBM_SETRANGE,0,MAKELPARAM(min_range,max_range)));
  }
  DWORD     __fastcall SetPos  (WORD pos)   {Position = pos;return DWORD(SendMessage(PBM_SETPOS  ,WPARAM( Position ) ,0));}
  void      __fastcall SetStep (int step) {Step = step;}
  DWORD     __fastcall StepIt  (           ){return SetPos(Position+Step);}
  void      __fastcall MovePos(int delta){SetPos(Position +delta);}
  DWORD     __fastcall GetPos(){return Position;}
  int       __fastcall GetStep(){return Step;}
  DWORD     __fastcall GetMinRange(){return minRange;}
  DWORD     __fastcall GetMaxRange(){return maxRange;}

};

KEWIN_CLASS TToolBar:public TCommonControl
{
  protected:
          DWORD    __fastcall GetControlFlag(){return ICC_BAR_CLASSES;}
          int      __fastcall OnWMNotify(int CtrlId,LPNMHDR mnhdr);
  virtual bool     __fastcall OnGetButtonInfo(TBNOTIFY * tbn);
  virtual bool     __fastcall OnQueryInsert(TBNOTIFY * tbn);
  virtual bool     __fastcall OnQueryDelete(TBNOTIFY * tbn);
  public:
  TToolBar(HINSTANCE hInst = GetModuleHandle(0)):TCommonControl(hInst){};
  bool __fastcall DoCreate(HWND parent, DWORD ws, UINT wID,int nBitmaps,
                           UINT wBMID,LPCTBBUTTON lpButtons, int iNumButtons,
                           int dxButton,int dyButton,int dxBitmap,int dyBitmap);
  bool     __fastcall DoCreate(HWND parent,DWORD id);
  void     __fastcall AddBitmap(int b_cnt,DWORD id,HINSTANCE hInst = HINST_COMMCTRL);
  bool     __fastcall AddButton(TBBUTTON & tb);
  bool     __fastcall AddButton(int idCommand,int iBitmap, BYTE fsStyle =TBSTYLE_BUTTON,BYTE fsState=TBSTATE_ENABLED,int iString=0, DWORD dwData = 0);
  DWORD    __fastcall GetButtonState(int iButton);
  bool     __fastcall SetButtonState(int iButton,DWORD state);
};


KEWIN_CLASS TTreeView:public TCommonControl
{
 DWORD         __fastcall GetControlFlag(){return ICC_TREEVIEW_CLASSES;}
 int           __fastcall OnWMNotify(int CtrlId,LPNMHDR mnhdr);

 TClosureMethod<LPVOID,TTreeView*,LPNMTVDISPINFO,bool>   _OnBeginEditLabel;
 TClosureMethod<LPVOID,TTreeView*,LPNMTVDISPINFO,bool>   _OnEndLabelEdit;
 TClosureMethod<LPVOID,TTreeView*,LPNMTVDISPINFO,bool>   _OnGetDispInfo;
 TClosureMethod<LPVOID,TTreeView*,LPNMTVDISPINFO,bool>   _OnSetDispInfo;
 TClosureMethod<LPVOID,TTreeView*,LPNMTREEVIEW  ,bool>   _OnExpanding;
 TClosureMethod<LPVOID,TTreeView*,LPNMTREEVIEW  ,bool>   _OnExpanded;
 TClosureMethod<LPVOID,TTreeView*,LPNMTREEVIEW  ,bool>   _OnSelChanging;
 TClosureMethod<LPVOID,TTreeView*,LPNMTREEVIEW  ,bool>   _OnSelChanged;
 TClosureMethod<LPVOID,TTreeView*,LPNMTREEVIEW  ,bool>   _OnDeleteItem;
 TClosureMethod<LPVOID,TTreeView*,LPNMTREEVIEW  ,bool>   _OnBeginDrag;
 TClosureMethod<LPVOID,TTreeView*,LPNMTVKEYDOWN ,bool>   _OnKeyDown;
 TClosureMethod<LPVOID,TTreeView*,LPNMTVGETINFOTIP,bool> _OnGetTip;


 virtual bool  __fastcall OnBeginLabelEdit(LPNMTVDISPINFO){return true;};
 virtual bool  __fastcall OnEndLabelEdit  (LPNMTVDISPINFO){return true;};
 virtual void  __fastcall OnGetDispInfo   (LPNMTVDISPINFO){};
 virtual void  __fastcall OnSetDispInfo   (LPNMTVDISPINFO){};

 virtual bool  __fastcall OnExpanding     (LPNMTREEVIEW){return true;};
 virtual void  __fastcall OnExpanded      (LPNMTREEVIEW){};

 virtual bool  __fastcall OnSelChanging   (LPNMTREEVIEW){return true;};
 virtual void  __fastcall OnSelChanged    (LPNMTREEVIEW){};

 virtual void  __fastcall OnBeginDrag     (LPNMTREEVIEW){};
 virtual void  __fastcall OnDeleteItem    (LPNMTREEVIEW){};

 virtual bool  __fastcall OnKeyDown       (LPNMTVKEYDOWN){return true;};
 virtual void  __fastcall OnGetTip        (LPNMTVGETINFOTIP){};


 public:

 typedef       bool (__stdcall * dispinfo_notify)(LPVOID,TTreeView*,LPNMTVDISPINFO);
 typedef       bool (__stdcall * treeview_notify)(LPVOID,TTreeView*,LPNMTREEVIEW);
 typedef       bool (__stdcall * keydown_notify) (LPVOID,TTreeView*,LPNMTVKEYDOWN);
 typedef       bool (__stdcall * infotip_notify) (LPVOID,TTreeView*,LPNMTVGETINFOTIP);

 TTreeView(HINSTANCE hInst = GetModuleHandle(0)):TCommonControl(hInst) {}
 bool __fastcall DoCreate(HWND parent,DWORD id);
 bool __fastcall DoCreate(HWND parent,TRect & r,DWORD Style,DWORD ExStyle, DWORD id=-1);

 bool        __fastcall GetItem(TV_ITEMA * item,UINT mask = TVIF_CHILDREN|TVIF_HANDLE|TVIF_IMAGE|TVIF_PARAM|TVIF_SELECTEDIMAGE|TVIF_STATE);
 bool        __fastcall GetItem(TV_ITEMW * item,UINT mask = TVIF_CHILDREN|TVIF_HANDLE|TVIF_IMAGE|TVIF_PARAM|TVIF_SELECTEDIMAGE|TVIF_STATE);
 bool        __fastcall SetItem(TV_ITEMA * item,UINT mask );
 bool        __fastcall SetItem(TV_ITEMW * item,UINT mask );
 DWORD       __fastcall GetItemCount();
 TRect       __fastcall GetItemRect(HTREEITEM item,bool all = false);
 int         __fastcall GetIndent();
 void        __fastcall SetIndent(int indent);
  COLORREF   __fastcall GetBkColor();
  void       __fastcall SetBkColor(COLORREF c);
  COLORREF   __fastcall GetLineColor();
  void       __fastcall SetLineColor(COLORREF c);
  COLORREF   __fastcall GetTextColor();
  void       __fastcall SetTextColor(COLORREF c);

  bool       __fastcall SelectItem(HTREEITEM);
  bool       __fastcall ItemDelete(HTREEITEM item);
  HTREEITEM  __fastcall GetRootItem();
  HTREEITEM  __fastcall GetSelected();
  HTREEITEM  __fastcall GetNextItem(HTREEITEM item,DWORD flag);
  HTREEITEM  __fastcall GetParent(HTREEITEM item);
  HTREEITEM  __fastcall InsertItem(char    * text,HTREEITEM parent = 0,HTREEITEM after = TVI_LAST );
  HTREEITEM  __fastcall InsertItem(wchar_t * text,HTREEITEM parent = 0,HTREEITEM after = TVI_LAST );
  bool       __fastcall GetItemText(HTREEITEM item,char    * buffer,int bsz);
  bool       __fastcall GetItemText(HTREEITEM item,wchar_t * buffer,int bsz);
  bool       __fastcall SetItemText(HTREEITEM item,char    * buffer);
  bool       __fastcall SetItemText(HTREEITEM item,wchar_t * buffer);
  bool       __fastcall Expand     (HTREEITEM = 0,bool partial = false);
  bool       __fastcall Collapse   (HTREEITEM = 0);

};

/************************* inline implementations *****************************/

inline   int __fastcall TCommonControl::OnChar(LPNMCHAR p )
{return 0;}
inline   int __fastcall TCommonControl::OnKey(LPNMKEY p)
{return 0;}
inline   int __fastcall TCommonControl::OnClick(LPNMHDR p)
{return 0;}
inline   int __fastcall TCommonControl::OnHover(LPNMHDR p)
{return 0;}
inline   int __fastcall TCommonControl::OnSetFocus(LPNMHDR p,bool)
{return 0;}
inline   int __fastcall TCommonControl::OnNCHitTest(LPNMMOUSE p)
{return 0;}
inline   int __fastcall TCommonControl::OnOutOfMemory(LPNMHDR p)
{return 0;}
inline   int __fastcall TCommonControl::OnReleasedCapture(LPNMHDR p)
{return 0;}
inline   int __fastcall TCommonControl::OnReturn(LPNMHDR p)
{return 0;}
inline   int __fastcall TCommonControl::OnSetCursor(LPNMMOUSE p)
{return 0;}

 inline void __fastcall TCommonControl::SetNMCharCallback    (LPVOID data,TCommonControl::FN_ONCTRLNOTIFY fn)
 {
   _OnChar.SetClosureMethod(fn);
   _OnChar.SetUserData(data);
 }
 inline void __fastcall TCommonControl::SetNMKeyCallback     (LPVOID data,TCommonControl::FN_ONCTRLNOTIFY fn)
 {
   _OnKey.SetClosureMethod(fn);
   _OnKey.SetUserData(data);
 }
 inline void __fastcall TCommonControl::SetNMClickCallback   (LPVOID data,TCommonControl::FN_ONCTRLNOTIFY fn)
 {
   _OnClick.SetClosureMethod(fn);
   _OnClick.SetUserData(data);
 }
 inline void __fastcall TCommonControl::SetNMHoverCallback   (LPVOID data,TCommonControl::FN_ONCTRLNOTIFY fn)
 {
   _OnHover.SetClosureMethod(fn);
   _OnHover.SetUserData(data);
 }
 inline void __fastcall TCommonControl::SetNMSetFocusCallback(LPVOID data,TCommonControl::FN_ONCTRLNOTIFY fn)
 {
   _OnSetFocus.SetClosureMethod(fn);
   _OnSetFocus.SetUserData(data);
 }
 inline void __fastcall TCommonControl::SetNMNCHitTest       (LPVOID data,TCommonControl::FN_ONCTRLNOTIFY fn)
 {
   _OnNCHitTest.SetClosureMethod(fn);
   _OnNCHitTest.SetUserData(data);
 }
 inline void __fastcall TCommonControl::SetNMOutOfMemory     (LPVOID data,TCommonControl::FN_ONCTRLNOTIFY fn)
 {
   _OnOutOfMemory.SetClosureMethod(fn);
   _OnOutOfMemory.SetUserData(data);
 }
 inline void __fastcall TCommonControl::SetNMReleasedCapture (LPVOID data,TCommonControl::FN_ONCTRLNOTIFY fn)
 {
   _OnReleasedCapture.SetClosureMethod(fn);
   _OnReleasedCapture.SetUserData(data);
 }
 inline void __fastcall TCommonControl::SetNMReturn          (LPVOID data,TCommonControl::FN_ONCTRLNOTIFY fn)
 {
   _OnReturn.SetClosureMethod(fn);
   _OnReturn.SetUserData(data);
 }
 inline void __fastcall TCommonControl::SetNMSetCursor       (LPVOID data,TCommonControl::FN_ONCTRLNOTIFY fn)
 {
   _OnSetCursor.SetClosureMethod(fn);
   _OnSetCursor.SetUserData(data);
 }


/*ToolTips*/
inline   void  __fastcall TToolTips::EnableTips(bool enable)
{ SendMessage(TTM_ACTIVATE,BOOL(enable),0);};

inline COLORREF   __fastcall TToolTips::GetTipBkColor()
{  return SendMessage(TTM_GETTIPBKCOLOR,0,0);}

inline COLORREF   __fastcall TToolTips::GetTipTextColor()
{ return SendMessage(TTM_GETTIPTEXTCOLOR,0,0);}

inline   void     __fastcall TToolTips::SetTipBkColor  (COLORREF color)
{ SendMessage(TTM_SETTIPBKCOLOR,color,0);};

inline   void     __fastcall TToolTips::SetTipTextColor(COLORREF color)
{ SendMessage(TTM_SETTIPTEXTCOLOR,color,0);};

inline   bool     __fastcall TToolTips::SetTitle(TTitleIcon icon,char * text)
{ return SendMessage(TTM_SETTITLEA,WPARAM(icon),LPARAM(text)) ? true:false;};

inline   bool     __fastcall TToolTips::SetTitle(TTitleIcon icon,wchar_t * text)
{ return SendMessage(TTM_SETTITLEW,WPARAM(icon),LPARAM(text)) ? true:false;};

inline   void     __fastcall TToolTips::SetDelayTime(TDelayTime dt,int time)
  {SendMessage(TTM_SETDELAYTIME,WPARAM(dt),LPARAM(MAKELONG(time,0)));}

inline   int     __fastcall TToolTips::GetDelayTime(TDelayTime dt)
  { return int(SendMessage(TTM_GETDELAYTIME,WPARAM(dt),0));}

inline   int      __fastcall TToolTips::GetMaxWidth()
{   return int(SendMessage(TTM_GETMAXTIPWIDTH,0,0));  }

inline   void     __fastcall TToolTips::SetMaxWidth(int w)
{   SendMessage(TTM_SETMAXTIPWIDTH,0,w);  }

inline  void     __fastcall TToolTips::UpdateText(HWND Control,char * text,HINSTANCE hInst)
{
 TToolInfoA info(Control,text,0,hInst);
 info.SetWindow(hWnd);
 SendMessage(TTM_UPDATETIPTEXTA,0,LPARAM(&info));
}

inline  void     __fastcall TToolTips::UpdateText(int  Control,char * text,HINSTANCE hInst)
{
 #ifdef _WIN64
 TRect r;
 TToolInfoA info(Control,r,text,0,hInst);
 #else
 TToolInfoA info(Control,*&TRect(),text,0,hInst);
 #endif
 info.SetWindow(hWnd);
 SendMessage(TTM_UPDATETIPTEXTA,0,LPARAM(&info));
}


inline  void     __fastcall TToolTips::UpdateText(HWND Control,wchar_t * text,HINSTANCE hInst)
{
 TToolInfoW info(Control,text,0,hInst);
 info.SetWindow(hWnd);
 SendMessage(TTM_UPDATETIPTEXTW,0,LPARAM(&info));
}
inline  void     __fastcall TToolTips::UpdateText(int  Control,wchar_t * text,HINSTANCE hInst)
{
 TRect r;
 TToolInfoW info(Control,r,text,0,hInst);
 info.SetWindow(hWnd);
 SendMessage(TTM_UPDATETIPTEXTW,0,LPARAM(&info));
}

 inline bool     __fastcall TToolTips::RemoveControl(HWND p,int id)
 { return RemoveControl(::GetDlgItem((::HWND)p,id));  }

inline  void     __fastcall TToolTips::RelayEvent(MSG & msg)
{  SendMessage(TTM_RELAYEVENT,0,LPARAM(&msg));}

inline bool     __fastcall TToolTips::GetToolInfo(HWND parent,int id,TToolInfoA &i)
  {  return GetToolInfo(::GetDlgItem((::HWND)parent,id),i); }
inline  bool     __fastcall TToolTips::GetToolInfo(HWND parent,int id,TToolInfoW &i)
  {  return GetToolInfo(::GetDlgItem((::HWND)parent,id),i); }


inline void     __fastcall TToolTips::SetTipTextCallback(LPVOID ud,TToolTips::FN_TIPTEXTA fn)
{
  this->__FnTipTextA.SetClosureMethod(fn);
  __FnTipTextA.SetUserData(ud);
}

inline void     __fastcall TToolTips::SetTipTextCallback(LPVOID ud,TToolTips::FN_TIPTEXTW fn)
{
  this->__FnTipTextW.SetClosureMethod(fn);
  __FnTipTextW.SetUserData(ud);
}
inline  void     __fastcall TToolTips::SetCustomDrawCallback(LPVOID ud,TToolTips::FN_CUSTOMDRAW fn)
{
 __FnCustomDraw.SetUserData(ud);
 __FnCustomDraw.SetClosureMethod(fn);
}

/*TListView*/

inline    COLORREF  __fastcall TListView::GetTextColor()
{ return COLORREF(SendMessage(LVM_GETTEXTCOLOR,0,0));}

inline    COLORREF  __fastcall TListView::GetBkColor()
{ return COLORREF(SendMessage(LVM_GETBKCOLOR,0,0));}

inline    bool      __fastcall TListView::SetTextColor(COLORREF color)
{ return SendMessage(LVM_SETTEXTCOLOR,0,LPARAM(color)) ? true:false;}

inline    bool      __fastcall TListView::SetBkColor(COLORREF color)
{ return (SendMessage(LVM_SETBKCOLOR,0,LPARAM(color)) && SendMessage(LVM_SETTEXTBKCOLOR,0,LPARAM(color))) ?
  true:false;
}

inline DWORD     __fastcall TListView::SetStyle(DWORD style,bool set,bool extended)
{
  if(extended)
  {
   return SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,style, set ? style:0);
  }

  DWORD  _style = GetWindowLong(GWL_STYLE);
  _style  = set ? (style|_style) : (_style &(~style));
  return SetWindowLong( GWL_STYLE,_style);
}

inline   int       __fastcall TListView::AddColumn(char * name,int width )
{ return InsertColumn(0x7FFFFFFF,name,width);}

inline   int       __fastcall TListView::AddColumn(wchar_t * name,int width)
{ return InsertColumn(0x7FFFFFFF,name,width);}

inline  int       __fastcall TListView::GetColumnText(int index,char * buffer,int bsz)
{
 LVCOLUMNA col;
 col.pszText = buffer;
 col.cchTextMax = bsz;
 if(GetColumn(index,&col,LVCF_TEXT))
   return lstrlenA(buffer);
   return 0;
}

inline  int       __fastcall TListView::GetColumnText(int index,wchar_t * buffer,int bsz)
{
 LVCOLUMNW col;
 col.pszText = buffer;
 col.cchTextMax = bsz;
 if(GetColumn(index,&col,LVCF_TEXT))
   return lstrlenW(buffer);
   return 0;
}

inline  int       __fastcall TListView::SetColumnText(int index,char * buffer)
{
 LVCOLUMNA col;
 col.pszText = buffer;
 return SetColumn(index,&col,LVCF_TEXT);
}

inline  int       __fastcall TListView::SetColumnText(int index,wchar_t * buffer)
{
 LVCOLUMNW col;
 col.pszText = buffer;
 return SetColumn(index,&col,LVCF_TEXT);
}

inline  bool      __fastcall TListView::DeleteColumn(int index)
{ return SendMessage(LVM_DELETECOLUMN,WPARAM(index),0) ? true:false;}

inline    int       __fastcall TListView::GetColumnWidth(int idx)
{
 LVCOLUMN col;
 return GetColumn(idx,&col,LVCF_WIDTH) ? int(col.cx):0;
}

inline    bool      __fastcall TListView::SetColumnWidth(int idx,int width)
{
 LVCOLUMN col;
 col.cx = width;
 return SetColumn(idx,&col,LVCF_WIDTH) ;
}

inline    DWORD     __fastcall TListView::GetColumnFormat(int idx)
{
 LVCOLUMN col;
 return GetColumn(idx,&col,LVCF_FMT) ? DWORD(col.fmt):0;
}

inline    bool      __fastcall TListView::SetColumnFormat(int idx,DWORD fmt)
{
 LVCOLUMN col;
 col.fmt = fmt;
 return SetColumn(idx,&col,LVCF_FMT) ;
}

inline   void      __fastcall TListView::SetViewType(TListView::ViewType v_type)
{
 SetStyle(v_type,true);
}

inline   TListView::ViewType  __fastcall TListView::GetViewType()
{
 return TListView::ViewType(GetWindowLong(GWL_STYLE)&LVS_TYPEMASK);
}

inline     DWORD     __fastcall TListView::GetItemCount()
{
 return (DWORD)SendMessage(LVM_GETITEMCOUNT,0,0);
}

inline     DWORD     __fastcall TListView::AddItem   (char    * text,LPVOID param,int image )
{ return InsertItem(GetItemCount(),text,param);}

inline     DWORD     __fastcall TListView::AddItem   (wchar_t * text,LPVOID param,int image  )
{ return InsertItem(GetItemCount(),text,param);}

inline     DWORD     __fastcall TListView::InsertItem(int idx,char    * text,LPVOID param,int image  )
{
 LVITEMA item;
 ZeroMemory(&item,sizeof(item));
 item.mask = LVIF_TEXT|LVIF_PARAM;
 item.pszText  = text;
 item.cchTextMax = lstrlenA(text);
 item.lParam   = (LONG)param;
 item.iSubItem = 0;
 item.iItem    = idx;
 item.iImage    = image;
 return SendMessage(LVM_INSERTITEMA,idx,LPARAM(&item));
}

inline     DWORD     __fastcall TListView::InsertItem(int idx,wchar_t * text,LPVOID param,int image  )
{
 LVITEMW item;
 ZeroMemory(&item,sizeof(item));
 item.mask = LVIF_TEXT|LVIF_PARAM|LVIF_IMAGE;
 item.pszText  = text;
 item.cchTextMax = lstrlenW(text);
 item.lParam   = (LONG)param;
 item.iSubItem = 0;
 item.iItem    = idx;
 item.iImage    = image;
 return SendMessage(LVM_INSERTITEMW,0,LPARAM(&item));
}
inline      bool      __fastcall TListView::GetItem(int idx,LVITEMA * item,DWORD Mask)
{
  item->mask = Mask;
  item->iItem = idx;
  return SendMessage(LVM_GETITEMA,0,LPARAM(item)) ? true:false;
}

inline      bool      __fastcall TListView::GetItem(int idx,LVITEMW *item,DWORD Mask)
{
  item->mask = Mask;
  item->iItem = idx;
  return SendMessage(LVM_GETITEMW,0,LPARAM(item)) ? true:false;
}
inline      bool      __fastcall TListView::SetItem(int idx,LVITEMA *item,DWORD Mask)
{
  item->mask = Mask;
  item->iItem = idx;
  return SendMessage(LVM_SETITEMA,0,LPARAM(item)) ? true:false;
}

inline      bool      __fastcall TListView::SetItem(int idx,LVITEMW *item,DWORD Mask)
{
  item->mask = Mask;
  item->iItem = idx;
  return SendMessage(LVM_SETITEMW,0,LPARAM(item)) ? true:false;
}
inline   int       __fastcall TListView::GetItemText(int i,char * buffer,int bsz)
{
 LVITEMA item;item.iItem = i;item.pszText = buffer;item.cchTextMax = bsz;
 return int(SendMessage(LVM_GETITEMTEXTA,WPARAM(i),LPARAM(&item)));
}

inline   int       __fastcall TListView::GetItemText(int i,wchar_t * buffer,int bsz)
{
 LVITEMW item;item.iItem = i;item.pszText = buffer;item.cchTextMax = bsz;
 return int(SendMessage(LVM_GETITEMTEXTW,WPARAM(i),LPARAM(&item)));
}

inline   int       __fastcall TListView::SetItemText(int i,char * buffer)
{
 LVITEMA item;item.iItem = i;item.pszText = buffer;item.cchTextMax = lstrlenA(buffer);
 return int(SendMessage(LVM_GETITEMTEXTA,WPARAM(i),LPARAM(&item)));
}
inline   int       __fastcall TListView::SetItemText(int i,wchar_t * buffer)
{
 LVITEMW item;item.iItem = i;item.pszText = buffer;item.cchTextMax = lstrlenW(buffer);
 return int(SendMessage(LVM_GETITEMTEXTW,WPARAM(i),LPARAM(&item)));
}

inline   int       __fastcall TListView::GetSubItemText(int i,int si,char * buffer,int bsz)
{
 LVITEMA item;
 ZeroMemory(&item,sizeof(item));
 item.iSubItem= si;item.pszText = buffer;item.cchTextMax = bsz;
 return GetItem(i,&item,LVIF_TEXT);
}

inline   int       __fastcall TListView::GetSubItemText(int i,int si,wchar_t * buffer,int bsz)
{
 LVITEMW item;
 ZeroMemory(&item,sizeof(item));
 item.iSubItem= si;item.pszText = buffer;item.cchTextMax = bsz;
 return GetItem(i,&item,LVIF_TEXT);
}

inline   int       __fastcall TListView::SetSubItemText(int i,int si,char * buffer)
{
 LVITEMA item;
 ZeroMemory(&item,sizeof(item));
 item.iSubItem= si;item.pszText = buffer;item.cchTextMax = lstrlenA(buffer);
 return SetItem(i,&item,LVIF_TEXT);
}

inline   int       __fastcall TListView::SetSubItemText(int i,int si,wchar_t * buffer)
{
 LVITEMW item;
 ZeroMemory(&item,sizeof(item));
 item.iSubItem= si;item.pszText = buffer;item.cchTextMax = lstrlenW(buffer);
 return SetItem(i,&item,LVIF_TEXT);
}


  inline DWORD __fastcall  TToolBar::GetButtonState(int iButton)
  {
    return DWORD(SendMessage(TB_GETSTATE,iButton,0));
  }

  inline bool  __fastcall  TToolBar::SetButtonState(int iButton,DWORD state)
  {
    return DWORD(SendMessage(TB_SETSTATE,iButton,state)) == DWORD(-1) ? false:true;
  }


//TreeView
inline  bool __fastcall TTreeView::GetItem(TV_ITEMA * item,UINT mask )
 {
   item->mask = mask;
   return SendMessage(TVM_GETITEMA,0,LPARAM(item))? true:false;
 }

inline  bool __fastcall TTreeView::GetItem(TV_ITEMW * item,UINT mask )
 {
   item->mask = mask;
   return SendMessage(TVM_GETITEMW,0,LPARAM(item)) ? true:false;
 }

inline  bool __fastcall TTreeView::SetItem(TV_ITEMA * item,UINT mask )
 {
   item->mask = mask;
   return SendMessage(TVM_SETITEMA,0,LPARAM(item)) ? true:false;

 }

inline  bool __fastcall TTreeView::SetItem(TV_ITEMW * item,UINT mask )
 {
   item->mask = mask;
   return SendMessage(TVM_SETITEMW,0,LPARAM(item)) ? true:false;
 }

inline  DWORD     __fastcall TTreeView::GetItemCount()
{
 return TreeView_GetCount((::HWND)hWnd);
}

inline  HTREEITEM __fastcall TTreeView::GetRootItem()
{
 return TreeView_GetRoot((::HWND)hWnd);
}

inline  HTREEITEM __fastcall TTreeView::GetSelected()
{
 return TreeView_GetSelection((::HWND)hWnd);
}

inline  TRect     __fastcall TTreeView::GetItemRect(HTREEITEM item,bool all)
{
 TRect r;
 TreeView_GetItemRect((::HWND)hWnd,item,&r,BOOL(!all));
 return r;
}

inline  int       __fastcall TTreeView::GetIndent()
{
 return TreeView_GetIndent((::HWND)hWnd);
}

inline  void      __fastcall TTreeView::SetIndent(int indent)
{
 TreeView_SetIndent((::HWND)hWnd,indent);
}

inline   COLORREF __fastcall TTreeView::GetBkColor()
{ return TreeView_GetBkColor((::HWND)hWnd);}

inline   void     __fastcall TTreeView::SetBkColor(COLORREF c)
{ TreeView_SetBkColor((::HWND)hWnd, c);}

inline   COLORREF __fastcall TTreeView::GetLineColor()
{ return TreeView_GetLineColor((::HWND)hWnd);}

inline   void     __fastcall TTreeView::SetLineColor(COLORREF c)
{ TreeView_SetLineColor((::HWND)hWnd, c);}

inline   COLORREF __fastcall TTreeView::GetTextColor()
{ return TreeView_GetTextColor((::HWND)hWnd);}

inline   void     __fastcall TTreeView::SetTextColor(COLORREF c)
{ TreeView_SetTextColor((::HWND)hWnd, c);}

inline   bool       __fastcall TTreeView::SelectItem(HTREEITEM item)
{
  TV_ITEM tvi; tvi.hItem = item; tvi.state =TVIS_SELECTED; tvi.stateMask = TVIS_STATEIMAGEMASK;
  return SetItem(&tvi,TVIF_STATE);
}

inline   HTREEITEM  __fastcall TTreeView::GetNextItem(HTREEITEM item,DWORD flag)
{ return TreeView_GetNextItem((::HWND)hWnd,item,flag);}

inline   HTREEITEM  __fastcall TTreeView::GetParent(HTREEITEM item)
{
 return GetNextItem(item,TVGN_PARENT);
}

inline   HTREEITEM  __fastcall TTreeView::InsertItem(char    * text,HTREEITEM parent,HTREEITEM after  )
{
 TVINSERTSTRUCTA tvi;
 tvi.hParent    = parent;tvi.hInsertAfter = after;
 tvi.item.mask = TVIF_TEXT;
 tvi.item.pszText = text;tvi.item.cchTextMax = lstrlenA(text);
 return (HTREEITEM)SendMessage(TVM_INSERTITEMA,0,LPARAM(&tvi));
}

inline   HTREEITEM  __fastcall TTreeView::InsertItem(wchar_t * text,HTREEITEM parent,HTREEITEM after   )
{
 TVINSERTSTRUCTW tvi;
 tvi.hParent    = parent;tvi.hInsertAfter = after;
 tvi.item.mask = TVIF_TEXT;
 tvi.item.pszText = text;tvi.item.cchTextMax = lstrlenW(text);
 return (HTREEITEM)SendMessage(TVM_INSERTITEMW,0,LPARAM(&tvi));
}

inline   bool       __fastcall TTreeView::GetItemText(HTREEITEM item,char    * buffer,int bsz)
{
  TV_ITEMA  tvi;tvi.hItem = item;
  tvi.pszText = buffer; tvi.cchTextMax = bsz;
  return GetItem(&tvi,TVIF_TEXT);
}

inline   bool       __fastcall TTreeView::GetItemText(HTREEITEM item,wchar_t * buffer,int bsz)
{
  TV_ITEMW  tvi;tvi.hItem = item;
  tvi.pszText = buffer; tvi.cchTextMax = bsz;
  return GetItem(&tvi,TVIF_TEXT);
}

inline   bool       __fastcall TTreeView::SetItemText(HTREEITEM item,char    * buffer)
{
  TV_ITEMA  tvi;tvi.hItem = item;
  tvi.pszText = buffer; tvi.cchTextMax =lstrlenA(buffer);
  return SetItem(&tvi,TVIF_TEXT);
}

inline   bool       __fastcall TTreeView::SetItemText(HTREEITEM item,wchar_t * buffer)
{
  TV_ITEMW  tvi;tvi.hItem = item;
  tvi.pszText = buffer; tvi.cchTextMax =lstrlenW(buffer);
  return SetItem(&tvi,TVIF_TEXT);
}

inline    bool       __fastcall TTreeView::Expand     (HTREEITEM item,bool partial)
{
	return TreeView_Expand((::HWND)hWnd,item ? item:GetRootItem(),TVE_EXPAND|(partial ? TVE_EXPANDPARTIAL:0) )? true:false;
}

inline    bool       __fastcall TTreeView::Collapse   (HTREEITEM item)
{
	return TreeView_Expand((::HWND)hWnd,item ? item:GetRootItem(),TVE_COLLAPSE) ? true:false;
}


#ifndef _NO_NAMESPACE
//namespace KrnlWin
}
#endif
#endif

