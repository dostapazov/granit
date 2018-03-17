
#include <gkcommctrl.hpp>

#ifndef _NO_NAMESPACE
namespace KrnlWin
{
#endif


  bool      __fastcall TListView::DoCreate(HWND parent,int id)//Создание по ресурсу
  {
   CheckAndLoad(this);
   this->CreateFromHWND(::GetDlgItem((::HWND)parent,id));
   /*if(hWnd)
      ClientDC = GetDC(hWnd);*/
   return hWnd ? true:false;
  }

  bool      __fastcall TListView::DoCreate(HWND parent,TRect & r,DWORD Style,DWORD ExStyle,int id)
  {
   CheckAndLoad(this);
   Create(parent,0,r,id,0,Style,ExStyle,WC_LISTVIEW);
   if(hWnd)
      ClientDC = GetDC((::HWND)hWnd);
   return hWnd ? true:false;
  }


  int      __fastcall TListView::InsertColumn(int idx,char * name,int width)
  {
   LVCOLUMNA col;
   col.cx = width;
   if(width <=0)
   {
     TSize sz;
     GetTextExtentPoint32A((::HDC)ClientDC,name,lstrlenA(name),&sz);
     col.cx = sz.cx+5;
   }
   col.pszText = name;
   col.mask = LVCF_TEXT| LVCF_WIDTH ;
   return int(SendMessage(LVM_INSERTCOLUMNA,idx,LPARAM(&col)));
  }


  int      __fastcall TListView::InsertColumn(int idx,wchar_t * name,int width)
  {
   LVCOLUMNW col;
   col.cx = width;
   col.pszText = name;
   col.mask = LVCF_TEXT|(width>0 ? LVCF_WIDTH : 0);
   if(width <=0)
   {
     TSize sz;
     GetTextExtentPoint32W((::HDC)ClientDC,name,lstrlenW(name),&sz);
     col.cx = sz.cx+5;
   }
   return int(SendMessage(LVM_INSERTCOLUMNW,idx,LPARAM(&col)));
  }

  bool      __fastcall TListView::GetColumn(int index,LVCOLUMNA * col,DWORD mask  )
  {
   if(col)
   {
    col->mask = mask;
    return SendMessage(LVM_GETCOLUMNA,WPARAM(index),LPARAM(col))? true:false;
   }
   return false;
  }

  bool      __fastcall TListView::SetColumn(int index,LVCOLUMNA * col,DWORD mask )
  {
   if(col)
   {
    col->mask = mask;
    return SendMessage(LVM_SETCOLUMNA,WPARAM(index),LPARAM(col))? true:false;
   }
   return false;
  }

  bool      __fastcall TListView::GetColumn(int index,LVCOLUMNW * col,DWORD mask  )
  {
   if(col)
   {
    col->mask = mask;
    return SendMessage(LVM_GETCOLUMNW,WPARAM(index),LPARAM(col))? true:false;
   }
   return false;
  }

  bool      __fastcall TListView::SetColumn(int index,LVCOLUMNW * col,DWORD mask )
  {
   if(col)
   {
    col->mask = mask;
    return SendMessage(LVM_SETCOLUMNW,WPARAM(index),LPARAM(col))? true:false;
   }
   return false;
  }

  int   __fastcall TListView::OnWMNotify(int CtrlId,LPNMHDR mnhdr)
  {
   int ret(0);
   switch(mnhdr->code)
   {
    case HDN_BEGINTRACK      :
    case HDN_BEGINTRACKW     :

    case HDN_DIVIDERDBLCLICK :
    case HDN_DIVIDERDBLCLICKW:
    case HDN_ENDTRACK        :
    case HDN_ENDTRACKW       :
    case HDN_ITEMCHANGED     :
    case HDN_ITEMCHANGEDW    :
    case HDN_ITEMCHANGING    :
    case HDN_ITEMCHANGINGW   :
    case HDN_ITEMCLICK       :
    case HDN_ITEMCLICKW      :
    case HDN_ITEMDBLCLICK    :
    case HDN_ITEMDBLCLICKW   :
    case HDN_TRACK           :
    case HDN_TRACKW          :
            ret = DefaultProcessing();break;
    case LVN_BEGINDRAG:
    case LVN_BEGINRDRAG:
         if(_OnBeginDrag.HaveMethod())
            _OnBeginDrag.CallMethod(this,LPNM_LISTVIEW(mnhdr));
          else
             OnBeginDrag(LPNM_LISTVIEW(mnhdr),mnhdr->code ==  LVN_BEGINRDRAG ? true:false);
         break;
    case LVN_BEGINLABELEDIT:
    case LVN_ENDLABELEDIT:
         if(_OnEditLabel.HaveMethod())
            ret = _OnEditLabel.CallMethod(this,(LV_DISPINFO *)(mnhdr));
          else
           ret = int (OnEditLabel((LV_DISPINFO *)(mnhdr),mnhdr->code ==  LVN_ENDLABELEDIT ? true:false));
         break;
    case LVN_ITEMCHANGED:
         if(_OnItemChanged.HaveMethod())
            _OnItemChanged.CallMethod(this,LPNM_LISTVIEW(mnhdr));
          else
         OnItemChanged(LPNM_LISTVIEW(mnhdr));
         break;
    case LVN_ITEMCHANGING:
         if(_OnItemChanging.HaveMethod())
	    ret = _OnItemChanging.CallMethod(this,LPNM_LISTVIEW(mnhdr)) ? FALSE:TRUE;
          else
         ret = int(OnItemChanging(LPNM_LISTVIEW(mnhdr))) ? FALSE:TRUE;
         break;
    case LVN_INSERTITEM:
         if(_OnInsertItem.HaveMethod())
            _OnInsertItem.CallMethod(this,LPNM_LISTVIEW(mnhdr));
            else
             OnInsertItem(LPNM_LISTVIEW(mnhdr));
         break;
    case LVN_COLUMNCLICK:
         if(_OnColumnClick.HaveMethod())
            _OnColumnClick.CallMethod(this,LPNM_LISTVIEW(mnhdr));
            else
            OnColumnClick(LPNM_LISTVIEW(mnhdr));
         break;
    case LVN_DELETEALLITEMS:
    case LVN_DELETEITEM:
         if(_OnDeleteItem.HaveMethod())
         _OnDeleteItem.CallMethod(this,LPNM_LISTVIEW(mnhdr));
         else
         OnDeleteItem(LPNM_LISTVIEW(mnhdr),mnhdr->code ==  LVN_DELETEALLITEMS ? true:false);
         break;
    case LVN_GETDISPINFO:
         if(_OnGetDispInfo.HaveMethod())
         _OnGetDispInfo.CallMethod(this,(LV_DISPINFO*)(mnhdr));
         else
         OnGetDispInfo((LV_DISPINFO*)(mnhdr));
         break;
    case LVN_SETDISPINFO:
         if(_OnSetDispInfo.HaveMethod())
         _OnSetDispInfo.CallMethod(this,(LV_DISPINFO*)(mnhdr));
         else
         OnSetDispInfo((LV_DISPINFO*)(mnhdr));
         break;
    case LVN_KEYDOWN:
         if(this->_OnKey.HaveMethod())
            _OnKey.CallMethod(this,mnhdr);
         else
          OnKey(LPNMKEY(mnhdr));
         break;
    default: ret = TCommonControl::OnWMNotify(CtrlId,mnhdr);
   }
   return ret;
  }

  int       __fastcall TListView::FindItem(char * text,int start)
  {
    LV_FINDINFOA fi = {0};
    fi.flags = LVFI_STRING;
    fi.psz   = text;
    return (int)SendMessage(LVM_FINDITEM,WPARAM(start),LPARAM(&fi));
  }

  int       __fastcall TListView::FindItem(wchar_t * text,int start)
  {
    LV_FINDINFOW fi = {0};
    fi.flags = LVFI_STRING;
    fi.psz   = text;
    return (int)SendMessage(LVM_FINDITEM,WPARAM(start),LPARAM(&fi));
  }



#ifndef _NO_NAMESPACE
}
#endif
