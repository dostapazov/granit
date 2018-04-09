//---------------------------------------------------------------------------

#pragma hdrstop
#include <vcl.h>

#include "kadrs_editor.h"
#include "mediumdb_frm.h"
#include "tlm_kadr_editor.h"
#include "paint_var_constructor.h"


//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "gkmodule_form"
#pragma link "tlm_painter_form"
#pragma link "tlm_kadr_editor"
#pragma link "tlm_kadr_editor"
#pragma resource "*.dfm"
TKadrsEditor *KadrsEditor;

//---------------------------------------------------------------------------
LONGLONG __fastcall get_file_create_time(wchar_t * fname)
{
   LONGLONG ret = -1;
   KeRTL::TFile file(fname,OPEN_EXISTING);
   if(file.IsValid())
   {
    GetFileTime(file(),NULL,NULL,(LPFILETIME)&ret);
    file.Close();
   }
  return ret;
}

//---------------------------------------------------------------------------
__fastcall TKadrsEditor::TKadrsEditor(TComponent* Owner,GKHANDLE module_handle)
        : TGKModuleForm(Owner,NULL,module_handle)
{
 create_mode   = false;
 Fin_setup     = false;
 pdll          = NULL;
 paint_context = -1;
 KadrsTree->Selected = kadrs_root = KadrsTree->Items->Add(0,"Список кадров");
 Fin_setup = 0;
 pvc       = NULL;
}
//---------------------------------------------------------------------------
UnicodeString         __fastcall TKadrsEditor::get_text(DWORD text_id) const
{
 char buffer[4096];
 LPMDB_RECORD_TEXT rt = (LPMDB_RECORD_TEXT) buffer;
 rt->dw_size  = sizeof(buffer);
 rt->text_len = 0;
 rt->text_size = sizeof(buffer)-sizeof(*rt)+sizeof(rt->text[0]);
 mod_iface.call(MDBCM_GET_TEXT,text_id,(LPARAM)rt);
 return UnicodeString(rt->text,lstrlenW(rt->text));
}
//---------------------------------------------------------------------------
DWORD              __fastcall TKadrsEditor::set_text(DWORD str_id,UnicodeString str)
{
  wchar_t buffer[4096];
  LPMDB_RECORD_TEXT rt = (LPMDB_RECORD_TEXT)buffer;
  rt->dw_size   = sizeof(buffer);
  rt->text_size =  rt->text_len  =  str.Length()+1;
  rt->text[0]   = 0;
  safe_strcpy(rt->text,str.c_str());
  str_id =  (DWORD)mod_iface.call(MDBCM_UPDATE_TEXT,str_id,(LPARAM)rt);
  return str_id;

}
//---------------------------------------------------------------------------
const wchar_t *        __fastcall TKadrsEditor::get_name(MDB_RECORD & rec,bool short_name ) const
{
   return get_text( short_name ? rec.sname : rec.name).c_str();
}

UnicodeString         __fastcall TKadrsEditor::get_name(DWORD rid,bool full ) const
{
 char buffer[4096];
 LPMDB_RECORD_TEXT rt = (LPMDB_RECORD_TEXT) buffer;
 rt->dw_size  = sizeof(buffer);
 rt->text_len = 0;
 rt->text_size = sizeof(buffer)/sizeof(wchar_t)-sizeof(*rt)+sizeof(rt->text[0]);
 mod_iface.call(full ? MDBCM_RECORD_GET_NAME:MDBCM_RECORD_GET_SNAME,rid,(LPARAM)rt);
 return UnicodeString(rt->text,rt->text_len);
}
//---------------------------------------------------------------------------
bool               __fastcall TKadrsEditor::get_record(DWORD rec_id,MDB_RECORD& sr,bool * is_visible ) const
{
 mdb_record rec;
 mod_iface.call(MDBCM_RECORD_GET,rec_id,(LPARAM)&rec);
 sr = MDB_RECORD(rec);
 if(is_visible) * is_visible = true;
 return rec.rec_id == rec_id ? true:false;
}
//---------------------------------------------------------------------------
void               __fastcall TKadrsEditor::reload()
{
  kadrs_root->DeleteChildren();
  setup_kadrs_tree  ();
  if(pdll)
     {
      setup_painter_dll() ;
     }
}
//---------------------------------------------------------------------------
void               __fastcall TKadrsEditor::after_set_gkhandle()
{
  reload();
  set_notify(mod_iface(),MDB_NOTIFY_MASK_RECORD|MDB_NOTIFY_MASK_KADR|MDB_NOTIFY_MASK_PAINTVAR);
}
//---------------------------------------------------------------------------
LRESULT            __fastcall TKadrsEditor::on_gkhandle_notify (GKHANDLE from,LPNOTIFY_CODE nc,DWORD mask)
{
 LRESULT ret = GKH_RET_SUCCESS;
 switch(mask)
 {
  case    MDB_NOTIFY_MASK_RECORD: on_record_notify(nc->notify_code,(LPMDB_RECORD_CHANGED)nc->notify_data);
  break;
  case    MDB_NOTIFY_MASK_KADR  : on_kadr_notify(nc->notify_code,(LPMDB_KADR_CHANGED)nc->notify_data);
  break;
  case MDB_NOTIFY_MASK_PAINTVAR :
       TMediumDBFrm::on_notify_paintvar(mod_iface,nc);
       update_paint_variants(nc);
  break;
  default : TGKModuleForm::on_gkhandle_notify(from,nc,mask);
 }
 return ret;
}
//---------------------------------------------------------------------------
void               __fastcall TKadrsEditor::on_record_notify(DWORD ncode,LPMDB_RECORD_CHANGED rc)
{
// TTreeNode * node = kadrs_root->getFirstChild();
// while(node)
//    {
//      Tkadr_data * kd = get_kadr(node);
//      if(kd && kd->user_data) kd->get_kadr_editor()->refresh_kadr() ;
//      node = node->getNextSibling();
//    }
}
//---------------------------------------------------------------------------
void               __fastcall TKadrsEditor::on_kadr_notify(DWORD ncode,LPMDB_KADR_CHANGED kc)
{
  TTreeNode * node = find_kadr(kc->kadr_id);
  switch(ncode)
  {
   case MDB_NOTIFY_CODE_KADR         :  on_kadr_change(node);
   break;
   case MDB_NOTIFY_CODE_KADR_CREATED :  if(!node) add_kadr_node(kc->kadr_id);
   break;
   case MDB_NOTIFY_CODE_KADR_CONTENT :  on_kadr_change_content(node);
   break;
   case MDB_NOTIFY_CODE_KADR_REMOVED : if(node)
                                        node->Delete();
   break;
  }
}

TTreeNode *        __fastcall TKadrsEditor::find_kadr    (DWORD kadr_id)
{
  if(kadr_map.count(kadr_id))
     return kadr_map[kadr_id];
   return  NULL;
}
//---------------------------------------------------------------------------
void               __fastcall TKadrsEditor::on_kadr_change        (TTreeNode * node)
{
  if(node)
    {
      Tkadr_data * kd = get_kadr(node);
      if(kd && kd->user_data)
               kd->get_kadr_editor() ->refresh_kadr() ;
    }
}

void               __fastcall TKadrsEditor::on_kadr_change_content(TTreeNode * node)
//---------------------------------------------------------------------------
{
}

void     __fastcall TKadrsEditor::on_module_config_change(GKHANDLE mod,LPMODULE_CONFIG_DATA mcd)
{
}
//---------------------------------------------------------------------------
void     __fastcall TKadrsEditor::on_module_state(GKHANDLE mod,DWORD state)
{
}
//---------------------------------------------------------------------------
void __fastcall TKadrsEditor::_set_kadr_changes   (Tkadr_data * kd,DWORD ch)
{
  if(kd )
   {
     if(kd->changes != ch)
     {
      kd->changes = ch;
     }
    TTlmKadrEditor * ke =  kd->get_kadr_editor();
    if(ke)
     {
      ke->kadr_changes = ch;
      if(ch&MDB_KADR_FIELD_ENTRYES) ke->refresh_kadr();
     }

    if(kd == current_kadr)
     {
      ApplyBtn->Enabled = ch ? true : false;
      UndoBtn->Enabled  = ch ? true : false;
    }
   }
}

//---------------------------------------------------------------------------
bool __fastcall TKadrsEditor::_apply_entryes(Tkadr_data * kd)
{
  bool ret = false;
  DWORD entry_count = kd->entryes.size();
  DWORD need_size = sizeof(MDB_KADR_ENTRY_LIST)+(entry_count-1)*sizeof(MDB_KADR_ENTRY);
  LPMDB_KADR_ENTRY_LIST list = (LPMDB_KADR_ENTRY_LIST)new BYTE[need_size];
  if(list)
  {
   list->dw_size  = std::max(need_size,(DWORD)sizeof(MDB_KADR_ENTRY_LIST));
   list->capacity = list->count = entry_count;
   kadr_entryes_table::iterator beg = kd->entryes.begin(),end = kd->entryes.end();
   LPMDB_KADR_ENTRY dest = list->entryes;
   //int number = 0;
   while(beg<end)
   {
    //beg->number = number++;
    memcpy(dest,&*beg,sizeof(*dest));
    ++dest;
    ++beg;
   }
   ret = mod_iface.call(MDBCM_KADR_SET_ENTRY_LIST,kd->kadr.kadr_id,(LPARAM)list) == GKH_RET_SUCCESS ? true:false;
   delete [](LPBYTE)list;
  }
 return ret;
}
//---------------------------------------------------------------------------

bool __fastcall TKadrsEditor::_apply_kadr_changes(Tkadr_data * kd)
{
  bool ret = true;
  if(kd)
  {
    //Применениe изменений
    bool was_new = kd->is_new();
    if(was_new || (kd->changes & MDB_KADR_FIELD_MASK) )
    {
      if(kd->changes & MDB_KADR_FIELD_NAME )
         kd->kadr.name_id = set_text(kd->kadr.name_id,kd->kadr_name.Trim());

      if(kd->changes&MDB_KADR_FIELD_SNAME )
         {
          kd->kadr.sname_id    = set_text(kd->kadr.sname_id,kd->kadr_sname.Trim());
         }

      if(kd->changes&MDB_KADR_FIELD_IMAGE)
        {
          kd->kadr.image_id   = set_text(kd->kadr.image_id,kd->image_name.Trim());
          kd->kadr.image_time = get_file_create_time(kd->image_name.Trim().c_str());
        }

      if(was_new)
      {
        kd->kadr.kadr_id = mod_iface.call(MDBCM_KADR_CREATE,(LPARAM)&kd->kadr,0);
        ret = (kd->kadr.kadr_id != (DWORD)GKH_RET_ERROR) ? true:false;
        if(ret)
           kadr_map[kd->kadr.kadr_id] = kd->owner_node;
      }
      else
      ret = GKH_SUCCESS(mod_iface.call(MDBCM_KADR_UPDATE,(LPARAM)&kd->kadr,kd->changes));
    }
      if(ret && (was_new || (kd->changes & MDB_KADR_FIELD_SNAME)))
      {
         UnicodeString str = kd->kadr_sname.Trim();
         if(str.IsEmpty())  str.printf(L"Кадр %d",kd->kadr.kadr_id);
         kd->owner_node->Text = str;
      }

   if( ret && (kd->changes & MDB_KADR_FIELD_ENTRYES))
       ret = _apply_entryes(kd);

    if( ret )
       _set_kadr_changes(kd,0);


  }
 return ret;
}
//---------------------------------------------------------------------------

bool __fastcall TKadrsEditor::_undo_kadr_changes(Tkadr_data * kd)
{
  bool ret = false;
  if(kd)
  {
     if(kd->changes & MDB_KADR_FIELD_MASK)
        {
         LRESULT res = mod_iface.call(MDBCM_KADR_GET,kd->kadr.kadr_id,(LPARAM)&kd->kadr);
         if(GKH_RET_SUCCESS == res)
           {
            kd->kadr_name  = this->get_text(kd->kadr.name_id );
            kd->kadr_sname = this->get_text(kd->kadr.sname_id);
            kd->image_name = this->get_text(kd->kadr.image_id);
            ret = true;
           }
        }
        else
        ret = true;

     if(ret && (kd->changes & MDB_KADR_FIELD_ENTRYES))
        {
          ret = get_kadr_entryes(kd);
          if(current_kadr == kd)
             this->setup_entry_list() ;
        }

     if( ret )
     {
       _set_kadr_changes(kd,0);
       if(kd->user_data) kd->get_kadr_editor()->set_kadr(kd);
     }
  }
 return ret;
}
//---------------------------------------------------------------------------

void __fastcall TKadrsEditor::WndProc(TMessage & msg)
{
  switch(msg.Msg)
  {
    case      WM_APPLY_CHANGES         : _apply_kadr_changes  (reinterpret_cast<Tkadr_data*>((void*)msg.LParam));break;
    case      WM_UNDO_CHANGES          : _undo_kadr_changes   (reinterpret_cast<Tkadr_data*>((void*)msg.LParam));break;
    case      WM_KADR_CHANGED          : _set_kadr_changes    (reinterpret_cast<Tkadr_data*>((void*)msg.LParam),msg.WParam);break;
    case      WM_ENTRY_SELECT_CHANGE   : _kadr_select_changed (reinterpret_cast<Tkadr_data*>((void*)msg.LParam),msg.WParam);break;
    case      WM_NOTIFY_ENTRYES_CHANGED: _kadr_entryes_changed(reinterpret_cast<Tkadr_data*>((void*)msg.LParam));break;
    default                   : TGKModuleForm::WndProc(msg);break;
  }
}

//---------------------------------------------------------------------------

TTreeNode *        __fastcall TKadrsEditor::add_kadr_node(DWORD kid)
{
 mdb_kadr kdr;
 mod_iface.call(MDBCM_KADR_GET,kid,(LPARAM)&kdr);
 return add_kadr_node(kdr);

}
//---------------------------------------------------------------------------

TTreeNode *        __fastcall TKadrsEditor::add_kadr_node(mdb_kadr & kadr)
{
  if(kadr.kadr_id!=(DWORD)-1)
  {
    Tkadr_data * kd = new Tkadr_data;
    kd->kadr = kadr;
    kd->kadr_sname = get_text(kadr.sname_id);
    kd->kadr_name  = get_text(kadr.name_id);
    kd->image_name = get_text(kadr.image_id);
    AnsiString str = kd->kadr_sname;
  if(!str.Length())
     str = kd->kadr_name;
  if(!str.Length())
   str.printf("Кадр ID - %d",kadr.kadr_id);
  TTreeNode * node = KadrsTree->Items->AddChild(kadrs_root,str);
  node->Data = (LPVOID)kd;
  kd->owner_node = node;
  kadr_map[kadr.kadr_id] = node;
  return node;
  }
  return 0; 
}
//---------------------------------------------------------------------------
void     __fastcall TKadrsEditor::setup_kadrs_tree()
{
 DWORD idx = 0;
 mdb_kadr  kadr;
 while((DWORD)mod_iface.call(MDBCM_KADR_ENUM,idx++,(LPARAM)&kadr)<(DWORD)GKH_RET_EXCEPTION)
 {
   if(kadr.kadr_id!=(DWORD)-1)
      add_kadr_node(kadr);
 }
 kadrs_root->Expand(false);
 kadrs_root->Selected = false;
 kadrs_root->Selected = true;
}
//---------------------------------------------------------------------------
void               __fastcall TKadrsEditor::show_kadr_state(Tkadr_data * kd)
{
 UnicodeString str;
 if(kd)
 {
   TCHAR *diag_str [] ={_T("Достоверны"),_T("Частично недостоверны"),_T("Недостоверны")};
   str.printf(L" Данные в кадре %s сотояние кадра %X изменений %d ТС %d аварийных %d ",
              diag_str[KeRTL::MIN(2,(int)kd->kadr.kadr_diag)],(DWORD)kd->kadr.kadr_state,kd->kadr.kadr_changes,
              kd->kadr.discrete_changes,kd->kadr.alarm_changes              );
 }
 StatusBar1->Panels->Items[0]->Text = str;
}

void __fastcall TKadrsEditor::EntryListCompare(TObject *Sender,
      TListItem *Item1, TListItem *Item2, int Data, int &Compare)
{
  Compare = (int)Item1->Data - (int) Item2->Data;
}
//---------------------------------------------------------------------------

void __fastcall TKadrsEditor::EntryListResize(TObject *Sender)
{

  DWORD width = GetSystemMetrics(SM_CXVSCROLL)+5*GetSystemMetrics(SM_CXBORDER);
  int i;
  for( i = 0;i<EntryList->Columns->Count-1;i++)
  {
     TListColumn * col =EntryList->Columns->Items[i];
     if(col->Width)
      width+=col->Width;
      else
      return;
  }
     EntryList->Columns->Items[i]->Width = EntryList->Width-width;

}
//---------------------------------------------------------------------------


void  __fastcall TKadrsEditor::get_recid_number(TListItem * item,DWORD &record_id,DWORD & num)
{
  #ifdef __CODEGEARC__
  record_id   = item ? atoi(item->SubItems->Strings[0].c_str()): -1;
  #else
  record_id   = item ? ::atoi(item->SubItems->Strings[0].c_str()): -1;
  #endif
  num  = (DWORD)item->Data;
}

void __fastcall TKadrsEditor::EntryListDragOver(TObject *Sender,
      TObject *Source, int X, int Y, TDragState State, bool &Accept)
{
 UnicodeString class_name = Sender->ClassName();
 TTreeView * tv = dynamic_cast<TTreeView*>(Source);
 TListView * lv = dynamic_cast<TListView*>(Source);
 if((tv && tv->Name == "MdbTree") || (lv && lv->Name == "RecordsList"))
    Accept = true;
    else
      Accept = false;
}
//---------------------------------------------------------------------------

void __fastcall TKadrsEditor::_add_entry  (Tkadr_data * kd ,mdb_kadr_entry & entry )
{
  kadr_entryes_table::iterator ptr = kd->add(entry);
  kd->select(ptr);
}

void __fastcall TKadrsEditor::_add_entry  (Tkadr_data * kd ,mdb_record & rec,int X,int Y )
{
  mdb_kadr_entry entry ;
  if(kd->entryes.size()) entry = *kd->entryes.rbegin();
     else
     entry.options.color = rec.is_discrete()? clYellow : clGreen;
  entry.kadr_id  = kd->kadr.kadr_id;
  entry.rec_id   = rec.rec_id;
  entry.number   = kd->entryes.size();
  entry.position.x = X;
  entry.position.y = Y;
  entry.options.paint_as     = rec.is_discrete() ;
  entry.options.normal_value = 0;
  tlmpaint::paint_action_t<TKadrsEditor> pa(paint_context,kd->user_data ? kd->get_kadr_editor()->paint_area->Canvas->Handle : this->Canvas->Handle,NULL,NULL,false,false,this);
  entry.size<<pa.calc_size(entry);
  this->_add_entry(kd,entry);
}

//---------------------------------------------------------------------------

void __fastcall TKadrsEditor::drop_finish(Tkadr_data * kd,int X,int Y)
{
 //Завершение опреации drag-drop
 TTlmKadrEditor * kadr_editor = kd->get_kadr_editor();
 kd->position_index_enable(false);
 kd->layout_entryes(kd->selected.begin(),kd->selected.end(),X,Y,kadr_editor ? kadr_editor->get_bk_grid() : 0,false,false);
 this->_set_kadr_changes(kd,MDB_KADR_FIELD_ENTRYES);
 kd->resort();
 if(kadr_editor) kadr_editor->refresh_kadr();
 if(current_kadr == kd) setup_entry_list();
}


#pragma warn -8004
void __fastcall TKadrsEditor::drop_from_tree_view(Tkadr_data * kd,TTreeView * src,int X,int Y)
{
  TTreeNode * node = src->Selected;
  mdb_record  rec;
  if(node )
  {
    kd->clear_selections();
    bool ctrl = (GetKeyState(VK_CONTROL)&0x8000) ? true : false;
    if(get_record(DWORD(node->Data),rec))
       {
        _add_entry(kd,rec,X,Y);
       }
    drop_finish(kd,X,Y);
  }
}
#pragma warn .8004

void __fastcall TKadrsEditor::drop_from_list_view(Tkadr_data * kd,TListView * src,int X,int Y)
{
  TListItem  * item = src->Selected;
  if(item)
  {
   TItemStates istats = TItemStates() << isSelected;
   mdb_record  rec;
   kd->clear_selections();
   while(item)
     {
       if(get_record((DWORD)item->Data,rec))
          {
            _add_entry(kd,rec,X,Y);
          }
      item = src->GetNextItem(item,sdAll,istats);
     }
    drop_finish(kd,X,Y);
  }
}

Tkadr_data * __fastcall TKadrsEditor::get_kadr_data_from_drop_sender(TObject * sender)
{
  TComponent * comp = dynamic_cast<TComponent*>(sender);
  while(comp)
    {
     TTlmKadrEditor  * kadr_editor = dynamic_cast<TTlmKadrEditor*>(comp);
     if(kadr_editor) return kadr_editor->kadr_data;
     TListView      * lv = dynamic_cast<TListView*>(comp);
     if(lv && lv == this->EntryList)
        return current_kadr;
     comp = comp->Owner;
    }
 return NULL;
}

void __fastcall TKadrsEditor::EntryListDragDrop(TObject *Sender, TObject *Source,
          int X, int Y)
{
  Tkadr_data * kd = get_kadr_data_from_drop_sender(Sender);
  if(kd)
  {
      TTreeView * tv = dynamic_cast<TTreeView*>(Source);
      if(tv)
       drop_from_tree_view(kd,tv,X,Y);
      else
      {
      TListView * lv = dynamic_cast<TListView*>(Source);
      if(lv)
        drop_from_list_view(kd,lv,X,Y);
      }
  }
}

//---------------------------------------------------------------------------
void   __fastcall TKadrsEditor::update_paint_variants(LPNOTIFY_CODE nc)
{
//  LPMDB_PAINT_VARIANT pv = (LPMDB_PAINT_VARIANT )nc->notify_data;
//  UnicodeString name = TMediumDBFrm::get_text(mod_iface,pv->name_id);
//  this->set_paint_variant(name,pv,true);
//  for(int i = 0;i<this->KadrsTree->Items->Count;i++)
//  {
//   TTreeNode * node =  this->KadrsTree->Items->Item[i];
//   Tkadr_data * kd = get_Tkadr_data(node);
//   if(kd && kd->kadr_painter_frame)
//      {
//       kd->kadr_painter_frame->add_paint_variant(true,pv->idx,name);
//      }
//  }
}

const wchar_t * __fastcall TKadrsEditor::get_painter_library_name()
{
  return L"tlmobj_painter.dll";
}

void __fastcall TKadrsEditor::FormDestroy(TObject *Sender)
{
  if(pdll)
     {
      pdll->paint_context_delete(paint_context);
      tlmpaint::painter_dll::release_instance();
      pdll = NULL;
     }
}
//---------------------------------------------------------------------------
#pragma warn -8004
void __fastcall TKadrsEditor::store_paint_variants(int pcontext)
{
   int     idx       = 0;
   LPVOID  pvar;
   wchar_t   text[4098];
   do
   {
    pvar = pdll->paint_var_get(pcontext,idx);
    if(pvar)
    {
     DWORD enum_result = pdll->enum_discretes_ex(pcontext,idx,text,KERTL_ARRAY_COUNT(text));
     mdb_paint_variant    mdb_pvar;
     DWORD is_new_var = mod_iface.call(MDBCM_PAINTVARS_ENUM,idx,(LPARAM)&mdb_pvar);
     mdb_pvar.name_id = set_text(mdb_pvar.name_id,text);

     pdll->paint_var_get_image_name(pvar,text);
     if(*text || mdb_pvar.image_id != DWORD(-1) )
         {
          mdb_pvar.image_id   = set_text(mdb_pvar.image_id,text);
          mdb_pvar.image_time = get_file_create_time(text);
         }

     pdll->paint_var_get_mask_name(pvar,text);
     if(*text || mdb_pvar.mask_id != DWORD(-1) )
        {
         mdb_pvar.mask_id   = set_text(mdb_pvar.mask_id,text);
         mdb_pvar.mask_time = get_file_create_time(text);
        }

      mdb_pvar.size.cx        = pdll->paint_var_get_width(pvar);
      mdb_pvar.size.cy        = pdll->paint_var_get_height(pvar);
      mdb_pvar.multi_position = pdll->paint_var_get_multipos(pvar);
      mod_iface.call(is_new_var ? MDBCM_PAINTVARS_ADD:MDBCM_PAINTVARS_UPD,(LPARAM)&mdb_pvar,0);
     ++idx;
    }
  }while(pvar);
   mod_iface.call(MDBCM_PAINTVARS_END_UPDATE,idx,0);
}
#pragma warn .8004

//---------------------------------------------------------------------------
void               __fastcall TKadrsEditor::set_paint_variant(UnicodeString name,LPMDB_PAINT_VARIANT pvar,bool update)
{
     UnicodeString text;
     LPVOID pv =  update ?  pdll->paint_var_get(paint_context,pvar->idx) :   pdll->paint_var_create(paint_context);
     if(!pv) {pv = pdll->paint_var_create(paint_context); update = false;}
     if(pv)
     {
         text = get_text(pvar->image_id);
         pdll->paint_var_set_image(pv,text.c_str());
         text = get_text(pvar->mask_id);
         pdll->paint_var_set_mask(pv,text.c_str());
         pdll->paint_var_set_width(pv,pvar->size.cx);
         pdll->paint_var_set_multipos(pv,pvar->multi_position);
         if(!update)
         {
          text = name;
          pdll->paint_var_add(pv,text.c_str());
         }
     }
}
//---------------------------------------------------------------------------
void __fastcall TKadrsEditor::setup_painter_dll()
{
       mdb_paint_variant pv;
       DWORD idx = 0;
       pdll->call(PMCMD_PAINT_VAR_CLEAR,paint_context,0);
       while(GKH_RET_SUCCESS == mod_iface.call(MDBCM_PAINTVARS_ENUM,idx,(LPARAM)&pv))
       {
         UnicodeString var_name = TMediumDBFrm::get_text(mod_iface,pv.name_id);
         if(var_name.Length())
            set_paint_variant(var_name,&pv,false);
        ++idx;
       }
}
//---------------------------------------------------------------------------
void __fastcall TKadrsEditor::load_painter_dll()
{
 if(!pdll) pdll = tlmpaint::painter_dll::get_instance();

  if(!pdll->is_loaded())
    {
       wstring  lib_name = get_painter_library_name();
       if(pdll->load_library(lib_name))
       {
        paint_context = pdll->paint_context_create();
        setup_painter_dll();
      }
    }
}
//---------------------------------------------------------------------------
bool    __fastcall TKadrsEditor::get_kadr_entryes(Tkadr_data * kd)
{

   DWORD bsz      =  MDB_KADR_ENTRY_LIST_CALC_CAPACITY(kd->kadr.entry_count);
   LPBYTE buffer  = new BYTE[bsz];
   LPMDB_KADR_ENTRY_LIST ent_list = (LPMDB_KADR_ENTRY_LIST)buffer;
   bzero(buffer,bsz);
   ent_list->dw_size  = bsz;
   ent_list->capacity = MDB_KADR_ENTRY_LIST_CAPACITY(ent_list);
   LRESULT ret = mod_iface.call(MDBCM_KADR_GET_ENTRY_LIST,kd->kadr.kadr_id,(LPARAM)ent_list);
   if(ret<0)
     {
      delete buffer;
      bsz = MDB_KADR_ENTRY_LIST_CALC_CAPACITY(-ret);
      buffer = new BYTE [bsz];
      ent_list = (LPMDB_KADR_ENTRY_LIST)buffer;
      bzero(buffer,bsz);
      ent_list->dw_size  = bsz;
      ent_list->capacity = MDB_KADR_ENTRY_LIST_CAPACITY(ent_list);
      ret = mod_iface.call(MDBCM_KADR_GET_ENTRY_LIST,kd->kadr.kadr_id,(LPARAM)ent_list);
     }

   if(ret == GKH_RET_SUCCESS)
   {

    LPMDB_KADR_ENTRY entry     = ent_list->entryes;
    LPMDB_KADR_ENTRY entry_end = entry+ent_list->count;
    kd->clear_entryes();
    while(entry<entry_end)
      {
       entry->state&=~MDB_KADR_ENTRY_STATE_SELECTED;
       kd->entryes.push_back(*entry);
       ++entry;
      }
   }
   delete [] buffer;
   return GKH_SUCCESS(ret);

}
//---------------------------------------------------------------------------
void __fastcall TKadrsEditor::setup_entry_list()
{
   Tkadr_data* kd = current_kadr;
   if(!kd) return;
   EntryList->Items->BeginUpdate();
   kadr_entryes_table::iterator eptr =  kd->entryes.begin();
   kadr_entryes_table::iterator eend =  kd->entryes.end  ();
   TListItem * item;
   int  current_num = 0;

   while(eptr<eend)
   {
    UnicodeString str;
    if(current_num<EntryList->Items->Count)
       {
        item = EntryList->Items->Item[current_num];
       }
      else
      {
       item = EntryList->Items->Add();
       item->SubItems->Add(str);
       item->SubItems->Add(str);
       item->SubItems->Add(str);
      }
    item->Data    = (LPVOID)eptr->number;
    item->Caption = eptr->number;
    item->SubItems->Strings[0] = eptr->rec_id;
    MDB_RECORD rec;
    get_record(eptr->rec_id,rec);

    if(rec.addr.addr.addr!=(DWORD)-1)
      str.printf(L"%02u.%02u.%02u.%02u-%03u"
                 ,(unsigned)rec.addr.addr.pu
                 ,(unsigned)rec.addr.addr.cp
                 ,(unsigned)rec.addr.addr.fa
                 ,(unsigned)rec.addr.addr.sb
                 ,(unsigned)rec.addr.param_number
                 );
      else
      str = L"---";
    item->SubItems->Strings[1] = str;
    item->SubItems->Strings[2] = get_text(rec.name != DWORD(-1) ? rec.name : rec.sname);
    item->Selected = eptr->is_selected();
    ++current_num;
    ++eptr;
   }
   while(current_num<EntryList->Items->Count  )
         EntryList->Items->Item[current_num]->Delete();
   EntryList->CustomSort(NULL,0);
   EntryList->Items->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TKadrsEditor::KadrsTreeChange(TObject *Sender, TTreeNode *Node)
{
  bool is_kadr_node    = Node && Node != kadrs_root ? true:false;
  Panel1->Visible      = is_kadr_node;
  DeleteBtn->Enabled   = is_kadr_node;
  OpenKadrBtn->Enabled = is_kadr_node;
  if(is_kadr_node && current_kadr)
  {
   ++Fin_setup;
   Tkadr_data* kd = current_kadr;
   KadrID->Caption = kd->kadr.kadr_id;
   KadrName->Text  = kd->kadr_name;
   KadrSName->Text = kd->kadr_sname;
  if(kd && !(kd->changes&MDB_KADR_FIELD_ENTRYES))
      {
       get_kadr_entryes(kd);
       TTlmKadrEditor * ke = kd->get_kadr_editor();
       if(ke)
        {
         ke->kadr_changes = kd->changes;
         ke->refresh_kadr();
        }
      }
    setup_entry_list();
    this->show_kadr_state(kd);
    ApplyBtn->Enabled = kd->changes ? true : false;
    UndoBtn->Enabled  = ApplyBtn->Enabled & !kd->is_new();
   --Fin_setup;
  }
}
//---------------------------------------------------------------------------
void __fastcall TKadrsEditor::KadrsTreeDeletion(TObject *Sender, TTreeNode *Node)
{
  Tkadr_data * kd = get_kadr(Node);
  if(kd )
  {
    if(kadr_map.size() && kadr_map.count(kd->kadr.kadr_id))
       kadr_map.erase(kd->kadr.kadr_id);
    delete kd;
  }
}
//---------------------------------------------------------------------------
  Tkadr_data* __fastcall TKadrsEditor::get_current_kadr()
  {
    return get_kadr(KadrsTree->Selected);
  }

  Tkadr_data* __fastcall TKadrsEditor::get_kadr(TTreeNode * Node)
  {
   if(Node && Node->Data)
      return (Tkadr_data*)Node->Data;
   return NULL;
  }
//---------------------------------------------------------------------------
void __fastcall TKadrsEditor::_kadr_entryes_changed(Tkadr_data * kd)
{
 if(kd == current_kadr)
     setup_entry_list();
}
void __fastcall TKadrsEditor::_kadr_select_changed(Tkadr_data * kd,DWORD offset)
{
 if(kd == current_kadr)
   {
     const mdb_kadr_entry  * entry = kd->get_by_offset(offset);
     if(entry)
     {
        TListItem * item = EntryList->FindData(0,(LPVOID)entry->number,true,false);
        if(item)
          {
            DWORD rec_id,number;
            get_recid_number(item,rec_id,number);
            if(entry->rec_id == rec_id && entry->number == number && entry->is_selected() != item->Selected)
               item->Selected = entry->is_selected();
          }
     }
   }
}
//---------------------------------------------------------------------------
void __fastcall TKadrsEditor::EntryListSelectItem(TObject *Sender, TListItem *Item,
          bool Selected)
{
   Tkadr_data * kd = current_kadr;
   if(Item && kd)
   {
    DWORD rec_id,num;
    get_recid_number(Item,rec_id,num);
    kadr_entryes_table::iterator ptr;
    if(kd->find_entry(rec_id,num,ptr))
    {
      if(Selected)
         kd->select(ptr);
         else
         kd->deselect(ptr);
       if(kd->user_data) kd->get_kadr_editor()->refresh_kadr() ;
    }
    bool enabled = EntryList->SelCount == 1;
    EntryMoveUp  ->Enabled = enabled;
    EntryMoveDown->Enabled = enabled;
   }
}
//---------------------------------------------------------------------------
void __fastcall TKadrsEditor::OpenKadrBtnClick(TObject *Sender)
{
  Tkadr_data * kd = current_kadr;
  if(kd)
  {
    if(!pdll) load_painter_dll();
    TTlmKadrEditor * kadr_editor =  kd->get_kadr_editor();
    if(!kadr_editor)
       {
         kadr_editor     = new TTlmKadrEditor(this);
         //kadr_editor->Icon->Assign(Icon);
         kadr_editor->paint_context = this->paint_context;
         kd->user_data   = kadr_editor;
         kadr_editor->on_get_record = get_record;
         kadr_editor->on_get_text   = get_text;
         kadr_editor->set_kadr(kd);
         kadr_editor->paint_area->OnDragOver = EntryList->OnDragOver;
         kadr_editor->paint_area->OnDragDrop = EntryList->OnDragDrop;
       }
     kadr_editor->Visible = true;
     kadr_editor->SetFocus() ;
  }
}
//---------------------------------------------------------------------------
void __fastcall TKadrsEditor::ApplyBtnClick(TObject *Sender)
{
   if(current_kadr)
     _apply_kadr_changes(current_kadr);
}
//---------------------------------------------------------------------------
void __fastcall TKadrsEditor::UndoBtnClick(TObject *Sender)
{
   if(current_kadr)
     {
      Tkadr_data * kd = current_kadr;
        _undo_kadr_changes(kd);
         KadrsTreeChange(this,KadrsTree->Selected);
     }
}
//---------------------------------------------------------------------------
mdb_kadr_entry * __fastcall TKadrsEditor::find_entry(TListItem * item)
{
  mdb_kadr_entry * entry = NULL;
  if(item)
  {
  kadr_entryes_table::iterator eptr;
  DWORD  rec_id,number;
  get_recid_number(item,rec_id,number);
  if(current_kadr->find_entry(rec_id,number,eptr))
    entry = &*eptr;
  }
  return entry;
}
//---------------------------------------------------------------------------
#pragma warn -8004
void __fastcall TKadrsEditor::move_entry_number(TListItem * item,bool down)
{
 DWORD        changes = 0;
 Tkadr_data * kd = current_kadr;
 if(item && kd)
   {
    mdb_kadr_entry * entry = find_entry(item);

    if(entry && (down || entry->number>0) )
    {
        DWORD new_number = entry->number +  (down ? 1:-1);
        TListItem * next_item =
        EntryList->GetNextItem(item,down ? sdBelow:sdAbove,TItemStates());
        mdb_kadr_entry * next_entry = find_entry(next_item);
        if(next_entry && abs(int(entry->number)-int(next_entry->number))== 1)
        {
         std::swap(entry->number,next_entry->number);
         changes=MDB_KADR_FIELD_ENTRYES;
         next_item->Data    = (LPVOID)next_entry->number;
         next_item->Caption =  next_entry->number;
         item->Data         = (LPVOID)entry->number;
         item->Caption      = entry->number;
        }
        else
        {
         entry->number = new_number;
         changes=MDB_KADR_FIELD_ENTRYES;
         item->Data = (LPVOID)entry->number;
         item->Caption   = entry->number;
        }
        if(changes)
        {
          kd->resort();
          EntryList->CustomSort(NULL,0);
          _set_kadr_changes(kd,kd->changes|changes);
        }

    }
   }
}
#pragma warn .8004
//---------------------------------------------------------------------------
void __fastcall TKadrsEditor::EntryMoveUpClick(TObject *Sender)
{
 TListItem * item = EntryList->Selected;
 if(item && current_kadr)
   {
     move_entry_number(item,Sender == EntryMoveDown ? true : false);
   }
}
//---------------------------------------------------------------------------
void __fastcall TKadrsEditor::KadrNameChange(TObject *Sender)
{

    Tkadr_data * kd = current_kadr;
    TEdit * edit = dynamic_cast<TEdit*>(Sender);
    if(!in_setup && kd && edit)
    {
     if(edit == KadrName)
          {
           kd->kadr_name = edit->Text;

          }
          else
          {
           kd->kadr_sname = edit->Text;
           kd->owner_node->Text = kd->kadr_sname;
          }
         _set_kadr_changes(kd,kd->changes|edit->Tag);
    }
}
//---------------------------------------------------------------------------
void __fastcall TKadrsEditor::EntryDeleteClick(TObject *Sender)
{
  if(current_kadr)
    {
      current_kadr->remove_selected();
      _set_kadr_changes(current_kadr,current_kadr->changes|MDB_KADR_FIELD_ENTRYES);
      setup_entry_list();
    }
}
//---------------------------------------------------------------------------
void __fastcall TKadrsEditor::CreateBtnClick(TObject *Sender)
{
   TTreeNode  * node = this->KadrsTree->Items->AddChild(kadrs_root,L"Новый кадр");
   Tkadr_data * kd = new Tkadr_data;
   kd->owner_node = node;

   node->Data = (LPVOID )kd;
   kd->kadr.kadr_size.cx = 300; kd->kadr.kadr_size.cy = 160;
   node->Selected = true;
}
//---------------------------------------------------------------------------
void __fastcall TKadrsEditor::DeleteBtnClick(TObject *Sender)
{
  //Удалить кадр
  Tkadr_data * kd = current_kadr;
  if(kd && MessageBoxW(Handle,(UnicodeString(L"Удалить кадр ")+kd->owner_node->Text + UnicodeString(L" ?")).c_str()
                              ,L"Удаление кадра",MB_YESNO|MB_ICONQUESTION ) == IDYES
    )
  {
    bool ret = true;
    if(!kd->is_new())
       ret = GKH_RET_SUCCESS == mod_iface.call(MDBCM_KADR_REMOVE,kd->kadr.kadr_id,0) ? true:false;
    if(ret)
    {
     delete kd->owner_node;
    }

  }
}
//---------------------------------------------------------------------------
void __fastcall do_repainter(kadr_map_t::value_type & v)
{
  Tkadr_data * kd = (Tkadr_data*)v.second->Data;

  if(kd && kd->user_data){
   TTlmKadrEditor* kedtr = kd->get_kadr_editor();
   kedtr->refresh_kadr();
   kedtr->update_paint_variants();
  }
}
//---------------------------------------------------------------------------
void __fastcall TKadrsEditor::repaint_all_kadrs()
{
 std::for_each(kadr_map.begin(),kadr_map.end(),do_repainter);
}
//---------------------------------------------------------------------------
void __fastcall TKadrsEditor::pvc_close(TObject * sender,TCloseAction & ca)
{
   ca  = caFree;
   if(pvc->ModalResult == mrOk)
     {
       store_paint_variants(pvc->paint_context);
       pdll->paint_context_copy_discrete(paint_context,pvc->paint_context);
       repaint_all_kadrs();
     }
   pvc = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TKadrsEditor::PaintVarEditClick(TObject *Sender)
{
   if(!pdll) this->load_painter_dll();
   if(!pvc)
    {
      pvc = new TPaintVariantConstruct(this,paint_context);
      pvc->Icon->Assign(Icon);
      pvc->OnClose = pvc_close;
    }
   pvc->Visible = true;
   pvc->SetFocus() ;
}
//---------------------------------------------------------------------------


void __fastcall TKadrsEditor::save_all_kadrs()
{
  kadr_map_t::iterator ptr = kadr_map.begin();
  kadr_map_t::iterator end = kadr_map.end  ();
  while(ptr!=end)
  {
     Tkadr_data * kd = this->get_kadr(ptr->second);
     if(kd && kd->changes)
       _apply_kadr_changes(kd);
     ++ptr;
  }

}

void __fastcall TKadrsEditor::FormCloseQuery(TObject *Sender, bool &CanClose)
{
  kadr_map_t::iterator ptr = kadr_map.begin();
  kadr_map_t::iterator end = kadr_map.end  ();
  while(ptr!=end)
  {
     Tkadr_data * kd = this->get_kadr(ptr->second);
     if(kd && kd->changes)
     {
       if(IDYES == MessageBoxW(Handle,L"Есть несохранненные изменения в кадрах. Сохранить все?",L"Запрос на сохранение", MB_YESNO|MB_ICONQUESTION))
          save_all_kadrs();
       return;
     }
     ++ptr;
  }
}
//---------------------------------------------------------------------------

void __fastcall TKadrsEditor::FormKeyPress(TObject *Sender, System::WideChar &Key)

{
  bool is_ctrl = (GetKeyState(VK_LCONTROL)&0x8000) ? true : false;
  switch(Key)
  {
   case VK_RETURN:
        if(is_ctrl)
        {
         Key = NULL;
         if(ApplyBtn->Enabled) ApplyBtnClick(ApplyBtn);
        }
   case VK_BACK:
       if(is_ctrl)
       {
           Key = NULL;
           if(UndoBtn->Enabled) UndoBtnClick(ApplyBtn);
       }
   break;
  }
}
//---------------------------------------------------------------------------

