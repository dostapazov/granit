//---------------------------------------------------------------------------

#pragma hdrstop
#include <vcl.h>

#include "kadrs_konstructor.h"
#include "pictdlgs.h"
#include <algorithm>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "gkmodule_form"
#pragma resource "*.dfm"
TMdbKadrConstuctor *MdbKadrConstuctor;
//---------------------------------------------------------------------------
__fastcall TMdbKadrConstuctor::TMdbKadrConstuctor(TComponent* Owner,GKHANDLE module)
        : TGKModuleForm(Owner,NULL,module)
{
 in_setup   = false;
 apply_mask = 0;
 root = KadrsTree->Items->Add(0,"Список кадров");
 root->Data = (LPVOID)-1;
 root->Expanded = true;
}
//---------------------------------------------------------------------------

void     __fastcall TMdbKadrConstuctor::on_module_config_change(GKHANDLE mod,LPMODULE_CONFIG_DATA mcd)
{

}

void     __fastcall TMdbKadrConstuctor::on_module_state(GKHANDLE mod,DWORD state)
{
}



void     __fastcall TMdbKadrConstuctor::after_set_gkhandle()
{
 TGKModuleForm::after_set_gkhandle();
 set_notify(mod_iface(),MDB_NOTIFY_MASK_RECORD);
 setup_kadrs_tree();
}

LRESULT  __fastcall TMdbKadrConstuctor::on_gkhandle_notify (GKHANDLE from,LPNOTIFY_CODE nc,DWORD mask)
{
 if(mask == MDB_NOTIFY_MASK_RECORD)
 {
  switch(nc->notify_code)
  {
   case
   MDB_NOTIFY_CODE_RECORD_REMOVED :
   on_record_removed(nc->notify_data[0]);
   break;
   case
   MDB_NOTIFY_CODE_RECORD_CREATED :
   break;
   case
   MDB_NOTIFY_CODE_RECORD_CHANGED :
      {
       LPMDB_RECORD_CHANGED rc =(LPMDB_RECORD_CHANGED)nc->notify_data;
       on_record_changed(rc->rec_id,rc->mask);
      }
   break;
  }
 }
 if(mask == MDB_NOTIFY_MASK_KADR)
 {
  switch(nc->notify_code)
  {
  case MDB_NOTIFY_CODE_KADR_CREATED : on_kadr_created((DWORD)nc->notify_data[0]);break;
  case MDB_NOTIFY_CODE_KADR_CONTENT : on_kadr_content((LPMDB_KADR_CHANGED)nc->notify_data);break;
  case MDB_NOTIFY_CODE_KADR_REMOVED : on_kadr_removed((DWORD)nc->notify_data[0]);break;
  }
 }
 return GKH_RET_SUCCESS;
}

void     __fastcall TMdbKadrConstuctor::on_kadr_created(DWORD kadr_id)
{
}

void     __fastcall TMdbKadrConstuctor::on_kadr_content(LPMDB_KADR_CHANGED kc)
{
}

void     __fastcall TMdbKadrConstuctor::on_kadr_removed(DWORD kadr_id)
{
}


bool              __fastcall TMdbKadrConstuctor::get_record     (DWORD id,mdb_record & rec)
{
 return id == (DWORD)mod_iface.call(MDBCM_RECORD_GET,id,(LPARAM)&rec)? true:false;
}

AnsiString __fastcall TMdbKadrConstuctor::get_text       (DWORD str_id)
{
  wchar_t buffer[2048];
  LPMDB_RECORD_TEXT rt = (LPMDB_RECORD_TEXT)buffer;
  rt->dw_size  = sizeof(buffer);
  rt->text_size = (sizeof(buffer)-sizeof(*rt))/sizeof(wchar_t);
  rt->text_len  = 0;
  rt->text[0]   = 0;
  mod_iface.call(MDBCM_GET_TEXT,str_id,(LPARAM)rt);
  return AnsiString(rt->text);
}

DWORD             __fastcall TMdbKadrConstuctor::set_text       (DWORD str_id,AnsiString str)
{
  
  wchar_t buffer[2048];
  LPMDB_RECORD_TEXT rt = (LPMDB_RECORD_TEXT)buffer;
  rt->dw_size   = sizeof(buffer);
  rt->text_size =  rt->text_len  =  str.Length()+1;
  rt->text[0]   = 0;
  Ansi2Unicode(rt->text,str.c_str());
  str_id =  (DWORD)mod_iface.call(MDBCM_UPDATE_TEXT,str_id,(LPARAM)rt);
  return str_id;
}

AnsiString __fastcall TMdbKadrConstuctor::get_record_name(DWORD id,bool short_name)
{
  wchar_t buffer[2048];
  LPMDB_RECORD_TEXT rt = (LPMDB_RECORD_TEXT)buffer;
  rt->dw_size  = sizeof(buffer);
  rt->text_size = (sizeof(buffer)-sizeof(*rt))/sizeof(wchar_t);
  rt->text_len  = 0;
  rt->text[0]   = 0;
  mod_iface.call(short_name? MDBCM_RECORD_GET_SNAME:MDBCM_RECORD_GET_NAME,id,(LPARAM)rt);
  return AnsiString(rt->text);
}

bool     __fastcall TMdbKadrConstuctor::get_kadr(DWORD kadr_id,MDB_KADR & kdr)
{
 bool ret =(DWORD) mod_iface.call(MDBCM_KADR_GET,kadr_id,(LPARAM)&kdr)< (DWORD)GKH_RET_EXCEPTION ? true:false;
 return ret;
}

void   __fastcall TMdbKadrConstuctor::setup_kadrs_tree()
{
 DWORD idx = 0;
 mdb_kadr  kadr;
 AnsiString name;
 while((DWORD)mod_iface.call(MDBCM_KADR_ENUM,idx++,(LPARAM)&kadr)<(DWORD)GKH_RET_EXCEPTION)
 {
   if(kadr.name_id!=(DWORD)-1)
    name = get_text(kadr.name_id);
    else
    name.printf("Кадр ID %d",kadr.kadr_id);
    TTreeNode * node = KadrsTree->Items->AddChild(root,name);
    node->Data = (LPVOID)kadr.kadr_id;
 }
 root->Expand(false);
 
}


void     __fastcall TMdbKadrConstuctor::add_list_item(mdb_kadr_entry &entry)
{
 TListItem    * item = KadrEntryList->Items->Add();
 item->Caption   = entry.number;
 item->SubItems->Add(entry.rec_id);
 AnsiString str  =  get_record_name(entry.rec_id,false);
 if(str.Length()==0)
   str  =  get_record_name(entry.rec_id,true);
 item->SubItems->Add(str);
 item->Data = (LPVOID)entry.rec_id;
 //KadrEntryList->AlphaSort();
}




void     __fastcall TMdbKadrConstuctor::on_record_changed(DWORD rec_id,DWORD mask)
{
  mdb_record rec;
  get_record(rec_id,rec);
  MDB_SHORT_RECORD sr;
  sr.rec_id = rec.rec_id;
  sr.value  = rec.value;
  sr.diag   = rec.diag;
  sr.state  = rec.state;
  sr.time   = rec.time;
  sr.min_value = rec.min_value;
  sr.max_value = rec.max_value;
  kadrs_map::iterator beg = kmap.begin(),end = kmap.end();
  while(beg!=end)
  {
   if(beg->second)
     beg->second->update_value(sr,mask);
   ++beg;
  }

}

void     __fastcall TMdbKadrConstuctor::on_record_removed(DWORD rec_id)
{
  kadr_entryes_table::iterator beg = entry_table.begin(),end = entry_table.end();
  while(beg<end )
  {
   if(beg->rec_id )
      {
       entry_table.erase(beg);
       --end;
      }
      else
       ++beg;
  }
}

mdb_kadr_entry *  __fastcall TMdbKadrConstuctor::find_entry(DWORD id,DWORD number  )
{
  mdb_kadr_entry * ret = 0;
  mdb_kadr_entry ev;
  ev.kadr_id = kadr.kadr_id;
  ev.rec_id  = id;
  ev.number  = number == (DWORD)-1 ? 0:number;  

  kadr_entryes_table::iterator beg = entry_table.begin(),end = entry_table.end();
  beg = lower_bound(beg,end,ev);
  if(beg<end && beg->rec_id == id)
  {
   if(beg->number == number || number == (DWORD)-1)
      ret = &*beg;
  }
 return ret;
}


mdb_kadr_entry *  __fastcall TMdbKadrConstuctor::find_entry(TListItem * item)
{
 DWORD number = atoi(item->Caption.c_str());
 DWORD id     = (DWORD)item->Data;
 mdb_kadr_entry * ret = 0;
 ret = find_entry(id,number);
 return ret;

}


void __fastcall TMdbKadrConstuctor::FormDestroy(TObject *Sender)
{
 MdbKadrConstuctor = NULL;
 kadrs_map::iterator beg = kmap.begin(),end = kmap.end();
 while(beg!=end)
 {
  TKadrForm * form = beg->second;
  if(form)
  {
   form->OnDestroy = NULL;
   delete form;
  }
  ++beg;
 }
 kmap.clear();
}
//---------------------------------------------------------------------------



void __fastcall TMdbKadrConstuctor::ClearImageClick(TObject *Sender)
{
 KadrImage->Text = AnsiString();
}
//---------------------------------------------------------------------------

void __fastcall TMdbKadrConstuctor::CreateKadrClick(TObject *Sender)
{

  PageControl1->ActivePageIndex = 0;
  TTreeNode * node = KadrsTree->Items->AddChild(root,"Создание кадра...");
  node->Data  = (LPVOID)-1;
  KadrsTree->Selected = node;
  create_mode = true;
  kadr = mdb_kadr();
  setup_kadr(-1,0);

}
//---------------------------------------------------------------------------


void __fastcall TMdbKadrConstuctor::Setapply_mask(DWORD value)
{
        //TODO: Add your source code here
    if(!in_setup)
    {
        if(Fapply_mask!=value)
        {
          Fapply_mask=value;
        }
          TKadrForm * kform = kmap.count(kadr.kadr_id) ? kmap[kadr.kadr_id]:NULL;
          if(kform && kform->changes^apply_mask)
            kform->changes|=apply_mask;
          ApplyBtn->Enabled = Fapply_mask ? true:false;
          UndoBtn->Enabled = Fapply_mask ? true:false;
    }      
}

void __fastcall TMdbKadrConstuctor::Setcreate_mode(bool value)
{
        //TODO: Add your source code here
        if(Fcreate_mode!=value)
        {
          Fcreate_mode=value;
        }
}
void __fastcall TMdbKadrConstuctor::KadrsTreeChanging(TObject *Sender,
      TTreeNode *Node, bool &AllowChange)
{
    AllowChange =      create_mode ? false:true;
}
//---------------------------------------------------------------------------

TTreeNode * __fastcall TMdbKadrConstuctor::find_kadr_node(DWORD kid)
{
  TTreeNode * node = this->root->getFirstChild();
  while(node && node->Data!=(LPVOID)kid)
      node = node->getNextSibling();
  return node;    
}

void   __fastcall TMdbKadrConstuctor::on_apply(TKadrForm * form,DWORD ch_mask,mdb_kadr_entry * entry)
{
  if(this->kadr.kadr_id != form->kadr->kadr_id)
  {
   TTreeNode * node = find_kadr_node(form->kadr->kadr_id);
   if(node)
   {
    node->Selected = true;
   }
  }
  ApplyBtnClick(this);
}

void   __fastcall TMdbKadrConstuctor::on_undo (TKadrForm * form,DWORD ch_mask,mdb_kadr_entry * entry)
{
  if(this->kadr.kadr_id != form->kadr->kadr_id)
  {
   TTreeNode * node = find_kadr_node(form->kadr->kadr_id);
   if(node)
   {
    node->Selected = true;
   }
  }
  UndoBtnClick(this);
}

void __fastcall TMdbKadrConstuctor::ApplyBtnClick(TObject *Sender)
{
 //Применение
 if(create_mode)
  {
   do_create_kadr();
   create_mode = false;
  }
  else
  {
   do_apply_kadr();
  }
  apply_mask = 0;
}
//---------------------------------------------------------------------------

void   __fastcall TMdbKadrConstuctor::do_create_kadr()
{
 AnsiString str = KadrName->Text;
 kadr.name_id  =  set_text(-1,str);
 str = SKadrName->Text;
 if(str.Length() == 0)
     str.printf("Кадр ID %d",kadr.kadr_id);
 kadr.sname_id =  set_text(-1,str);
 kadr.image_id =  set_text(-1,this->KadrImage->Text);
 mod_iface.call(MDBCM_KADR_CREATE,(LPARAM)&kadr,0);
 KadrsTree->Selected->Text = str;
 KadrsTree->Selected->Data = (LPVOID)kadr.kadr_id;
 set_entryes();
}

void   __fastcall TMdbKadrConstuctor::do_apply_kadr()
{

 
 if(apply_mask & MDB_KADR_FIELD_NAME)
     kadr.name_id = set_text(kadr.name_id,KadrName->Text);
 if(apply_mask & MDB_KADR_FIELD_SNAME)
     kadr.name_id = set_text(kadr.sname_id,SKadrName->Text);
 if(apply_mask & MDB_KADR_FIELD_IMAGE)
     kadr.image_id = set_text(kadr.image_id,KadrImage->Text);
 if(apply_mask& (MDB_KADR_FIELD_NAME|MDB_KADR_FIELD_IMAGE|MDB_KADR_FIELD_SIZE))
 {
   mod_iface.call(MDBCM_KADR_UPDATE,(LPARAM)&kadr,apply_mask);
 }
 if(apply_mask& MDB_KADR_FIELD_ENTRYES)
    set_entryes();
 TKadrForm * kform = kmap.count(kadr.kadr_id) ? kmap[kadr.kadr_id] : NULL;
 if(kform) {
    kform->changes = 0;
  }
  
}



void __fastcall TMdbKadrConstuctor::UndoBtnClick(TObject *Sender)
{
  //Отмена изменений
  if(kadr.kadr_id!=(DWORD)-1)
  {
    get_kadr(kadr.kadr_id,kadr);
    setup_kadr(kadr.kadr_id,0);
  }
  if(create_mode)
  {
   DeleteKadrClick(this);
  }
  apply_mask = 0;
}
//---------------------------------------------------------------------------
void __fastcall TMdbKadrConstuctor::DeleteKadrClick(TObject *Sender)
{
  TTreeNode * node = KadrsTree->Selected;
  if(node)
  {
  if(!create_mode)
   {
    mod_iface.call(MDBCM_KADR_REMOVE,(DWORD)node->Data,0);
    TKadrForm * form = NULL;
    if(kmap.count(kadr.kadr_id))
     {
      form = kmap[kadr.kadr_id];
      kmap.erase(kadr.kadr_id);
     }
    if(form)
      {
       form->Close();
       delete form;
      }

   }
   else
   create_mode = false;
   node->Delete();
  } 

}
//---------------------------------------------------------------------------

void __fastcall TMdbKadrConstuctor::KadrNameChange(TObject *Sender)
{
  apply_mask|= MDB_KADR_FIELD_NAME;
  if(kmap.count(kadr.kadr_id))
  {
   TKadrForm * kform = kmap[kadr.kadr_id];
   if(kform)
      kform->Caption = KadrName->Text;
  }      
}
//---------------------------------------------------------------------------

void __fastcall TMdbKadrConstuctor::KadrImageChange(TObject *Sender)
{
 apply_mask|=MDB_KADR_FIELD_IMAGE;
 TKadrForm * kform = NULL;
 if(kmap.count(kadr.kadr_id))
    kform = kmap[kadr.kadr_id];
   if(kform)
      kform->BkgndImage = KadrImage->Text;
}
//---------------------------------------------------------------------------


void __fastcall TMdbKadrConstuctor::KadrsTreeChange(TObject *Sender,
      TTreeNode *Node)
{
  in_setup = true;
  DWORD kadr_id = (DWORD)Node->Data;
  TKadrForm * kform = NULL;
  if(kmap.count(kadr_id))
    kform = kmap[kadr_id];
    setup_kadr(kadr_id,kform);
  in_setup = false;  
    apply_mask = kform ? kform->changes:0;

}
//---------------------------------------------------------------------------

void  __fastcall TMdbKadrConstuctor::set_entryes()
{
 DWORD entry_count = entry_table.size();
 if(entry_count)
 {
  DWORD need_size = sizeof(MDB_KADR_ENTRY_LIST)+(entry_count-1)*sizeof(MDB_KADR_ENTRY);
  LPMDB_KADR_ENTRY_LIST list = (LPMDB_KADR_ENTRY_LIST)new BYTE[need_size];
  if(list)
  {
   list->dw_size  = need_size;
   list->capacity = list->count = entry_count;
   kadr_entryes_table::iterator beg = entry_table.begin(),end = entry_table.end();
   LPMDB_KADR_ENTRY dest = list->entryes;
   while(beg<end)
   {
    memcpy(dest,&*beg,sizeof(*dest));
    ++dest;
    ++beg;
   }
   mod_iface.call(MDBCM_KADR_SET_ENTRY_LIST,kadr.kadr_id,(LPARAM)list);
   delete [](LPBYTE)list;
  }
 }
}

void  __fastcall TMdbKadrConstuctor::get_entryes(TKadrForm * kform)
{
 entry_table.clear();
 KadrEntryList->Clear();
  DWORD entry_count;
 if(kform)
 {
  //Установка элементов списка из кадра
  entry_count = kform->entry_count;
  for(DWORD i = 0;i<entry_count;i++)
  {
   mdb_kadr_entry entry = kform->entry_item[i];
   entry_table.push_back(entry);
   add_list_item(entry);
  }

 }
 else
 {
  entry_count = kadr.entry_count;
 if(entry_count)
 {
  DWORD need_size = sizeof(MDB_KADR_ENTRY_LIST)+(entry_count-1)*sizeof(MDB_KADR_ENTRY);
  LPMDB_KADR_ENTRY_LIST list = (LPMDB_KADR_ENTRY_LIST)new BYTE[need_size];
  if(list)
  {
   list->dw_size  = need_size;
   list->capacity = entry_count ;list->count =  0;
   mod_iface.call(MDBCM_KADR_GET_ENTRY_LIST,kadr.kadr_id,(LPARAM)list);

   LPMDB_KADR_ENTRY src = list->entryes;
   for(DWORD i = 0;i<list->count;++i)
      {
       mdb_kadr_entry entry(*src);
       entry_table.push_back(entry);
       add_list_item(entry);

       ++src;
      }
   delete [](LPBYTE)list;
  }
 }
 }
}

void __fastcall TMdbKadrConstuctor::setup_kadr(DWORD kid ,TKadrForm * form)
{
  if(form)
     kadr = *form->kadr;
     else
     get_kadr(kid,kadr);
  if(kid!=(DWORD)-1 || create_mode)
  {
   KadrEntryList->Clear();
   entry_table.clear();

   KadrName->Text        = get_text(kadr.name_id);
   SKadrName->Text       = get_text(kadr.sname_id);
   KadrImage->Text       = get_text(kadr.image_id);
   ImageCentr->Checked   = (kadr.kadr_flags&MDB_KADR_FLAG_IMAGE_CENTER) ? true:false;
   ImageStretch->Checked = (kadr.kadr_flags&MDB_KADR_FLAG_IMAGE_STRETCH) ? true:false;

  //TODO Получение содержимого кадра
   get_entryes(form);
   if(!form)
   {
   if(kmap.count(kadr.kadr_id))
      setup_kadr_form(kmap[kadr.kadr_id]);
   }

   Panel1->Visible = true;
  }
  else
  Panel1->Visible = false;

}



void __fastcall TMdbKadrConstuctor::KadrEntryListSelectItem(
      TObject *Sender, TListItem *Item, bool Selected)
{
  in_setup = true;
  if(Selected)
  {
  mdb_kadr_entry * entry = this->find_entry(Item);
  if(entry)
  {

  EntryNumber->Text = entry->number;
  EntryPrecision->Text = entry->options.prec;
  NormalValue->ItemIndex = entry->options.normal_value;
  EntryColor ->Font->Color = (TColor)entry->options.color;
  EntryBkgndColor->Font->Color = (TColor)entry->options.bkgnd_color;
  mdb_record rec;
  get_record(entry->rec_id,rec);
  if(rec.state&MDBR_STATE_DISCRETE)
   {
     DiscreteDrawAs->ItemIndex = entry->options.paint_as;
     AnalogGroup->Visible   = false;
    DiscreteGroup->Visible = true;
   }
   else
    {
    AnalogDrawAs->ItemIndex = entry->options.paint_as;
    DiscreteGroup->Visible   = false;
    AnalogGroup->Visible = true;
    }
    TKadrForm * form = kmap.count(kadr.kadr_id) ?  kmap[kadr.kadr_id]:NULL;
    if(form)
     {
      form->select_entry(entry);
     }

   }
   DrawParam->Visible = Selected;
   DeleteEntry->Enabled = Selected;
   UpEntry->Enabled     = Selected;
   DownEntry->Enabled   = Selected;
  }
  else
  {
   DrawParam->Visible = false;
   DeleteEntry->Enabled = false;
   UpEntry->Enabled     = false;
   DownEntry->Enabled   = false;
  }
  in_setup = false;
}
//---------------------------------------------------------------------------


void __fastcall TMdbKadrConstuctor::EntryBkgndColorClick(TObject *Sender)
{
  TListItem * item = KadrEntryList->Selected;
  if(item)
  {
    mdb_kadr_entry * entry = find_entry(item);
    if(entry)
    {
    ColorDialog1->Color =(TColor) entry->options.bkgnd_color;
    if(ColorDialog1->Execute())
    {
     entry->options.bkgnd_color = ColorDialog1->Color;
     EntryBkgndColor->Font->Color = ColorDialog1->Color;
     apply_mask |= MDB_KADR_FIELD_ENTRYES;
     TKadrForm * kform = kmap.count(kadr.kadr_id) ? kmap[kadr.kadr_id] : NULL;
     if(kform)
       kform->update_entry(entry);
    }
   }
  }
}
//---------------------------------------------------------------------------

void __fastcall TMdbKadrConstuctor::EntryColorClick(TObject *Sender)
{

  TListItem * item = KadrEntryList->Selected;
  if(item)
  {
    mdb_kadr_entry * entry = find_entry(item);
    if(entry)
    {
    ColorDialog1->Color = (TColor)entry->options.color;
    if(ColorDialog1->Execute())
    {
     entry->options.color = ColorDialog1->Color;
     EntryColor->Font->Color = ColorDialog1->Color;
     apply_mask |= MDB_KADR_FIELD_ENTRYES;
     TKadrForm * kform = kmap.count(kadr.kadr_id) ? kmap[kadr.kadr_id] : NULL;
     if(kform)
       kform->update_entry(entry);

    }
   }
  }
}
//---------------------------------------------------------------------------

void __fastcall TMdbKadrConstuctor::PageControl1Changing(TObject *Sender,
      bool &AllowChange)
{
   AllowChange = Panel1->Visible;
}
//---------------------------------------------------------------------------

void __fastcall TMdbKadrConstuctor::KadrEntryListCompare(TObject *Sender,
      TListItem *Item1, TListItem *Item2, int Data, int &Compare)
{
  Compare = atoi(Item1->Caption.c_str()) - atoi(Item2->Caption.c_str());
}
//---------------------------------------------------------------------------

void __fastcall TMdbKadrConstuctor::EntryDrawAsClick(TObject *Sender)
{
  TListItem * item = KadrEntryList->Selected;
  if(item)
  {
    mdb_kadr_entry * entry = find_entry(item);
    if(entry && entry->options.paint_as != (WORD)DiscreteDrawAs->ItemIndex)
    {
     entry->options.paint_as = DiscreteDrawAs->ItemIndex;
     apply_mask |=MDB_KADR_FIELD_ENTRYES;
     TKadrForm * kform = kmap.count(kadr.kadr_id) ? kmap[kadr.kadr_id] : NULL;
     if(kform)
       kform->update_entry(entry);
    }
   }

}
//---------------------------------------------------------------------------



void __fastcall TMdbKadrConstuctor::ToolButton2Click(TObject *Sender)
{
   mdb_kadr_entry *entry = this->find_entry(KadrEntryList->Selected);
   if(entry)
   {
      entry_table.erase(entry);
      KadrEntryList->Selected->Delete();
   }
}
//---------------------------------------------------------------------------


void __fastcall TMdbKadrConstuctor::Panel1Resize(TObject *Sender)
{
 KadrEntryList->Columns->Items[2]->Width =
  KadrEntryList->Width -
  KadrEntryList->Columns->Items[1]->Width -
  KadrEntryList->Columns->Items[0]->Width -
  (GetSystemMetrics(SM_CXEDGE)<<1) - GetSystemMetrics(SM_CXHTHUMB); 
}
//---------------------------------------------------------------------------


void __fastcall TMdbKadrConstuctor::DownEntryClick(TObject *Sender)
{
 TListItem * item = KadrEntryList->Selected;
 if(item && item->Index+1<KadrEntryList->Items->Count)
 {
  KadrEntryList->SortType = Comctrls::stNone;
  mdb_kadr_entry & entry = *find_entry(item);
  TListItem *new_item = KadrEntryList->Items->Insert(item->Index+2);
  new_item->Caption   = entry.number;
  new_item->SubItems->Add(entry.rec_id);
  AnsiString str  =  get_record_name(entry.rec_id,false);
  if(str.Length()==0)
   str  =  get_record_name(entry.rec_id,true);
  new_item->SubItems->Add(str);
  new_item->Data = (LPVOID)entry.rec_id;
  item->Delete();
  renumber_entryes();
  new_item->Selected = true;
  new_item->Focused  = true;
  new_item->MakeVisible(false);
  KadrEntryList->SortType = Comctrls::stData;
  apply_mask|=MDB_KADR_FIELD_ENTRYES;
 }
}
void __fastcall TMdbKadrConstuctor::UpEntryClick(TObject *Sender)
{
 TListItem * item = KadrEntryList->Selected;
 if(item && item->Index)
 {
  KadrEntryList->SortType = Comctrls::stNone;
  mdb_kadr_entry & entry = *find_entry(item);
  TListItem *new_item = KadrEntryList->Items->Insert(item->Index-1);
  new_item->Caption   = entry.number;
  new_item->SubItems->Add(entry.rec_id);
  AnsiString str  =  get_record_name(entry.rec_id,false);
  if(str.Length()==0)
   str  =  get_record_name(entry.rec_id,true);
  new_item->SubItems->Add(str);
  new_item->Data = (LPVOID)entry.rec_id;
  item->Delete();
  renumber_entryes();
  new_item->Selected = true;
  new_item->Focused  = true;
  new_item->MakeVisible(false);
  KadrEntryList->SortType = Comctrls::stData;
  apply_mask|=MDB_KADR_FIELD_ENTRYES;
 }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void   __fastcall TMdbKadrConstuctor::renumber_entryes()
{
 vector<mdb_kadr_entry*> renumber_list;
 for(int i = 0;i<KadrEntryList->Items->Count;i++)
 {
  TListItem * item = KadrEntryList->Items->Item[i];
  renumber_list.push_back(find_entry(item));
  item->Caption = item->Index;
 }
 for(DWORD i = 0;i<renumber_list.size();i++)
   {
    mdb_kadr_entry * ke =renumber_list[i];
    if(ke)
      ke->number = i;
   }
  TKadrForm * form = NULL;
  if(kmap.count(kadr.kadr_id))
     form = kmap[kadr.kadr_id];
  if(form)
  {
   form->clear_entryes();
   kadr_entryes_table::iterator beg  = entry_table.begin(),end =entry_table.end();
   while(beg<end)
   {
    mdb_record rec ;
    get_record(beg->rec_id,rec);
    MDB_SHORT_RECORD sr;
    sr.rec_id = rec.rec_id;
    sr.value  = rec.value;
    sr.diag   = rec.diag;
    sr.state  = rec.state;
    sr.time   = rec.time;
    sr.min_value = rec.min_value;
    sr.max_value = rec.max_value;

    form->add_entry(*beg,sr,get_record_name(sr.rec_id,true),get_record_name(sr.rec_id,false));
    ++beg;
   }
  }

}


void __fastcall TMdbKadrConstuctor::DeleteEntryClick(TObject *Sender)
{
 TListItem * item = KadrEntryList->Selected;
 mdb_kadr_entry * entry = this->find_entry(item);
 entry_table.erase(entry);
 item->Delete();
 apply_mask|=MDB_KADR_FIELD_ENTRYES;
  TKadrForm * form = NULL;
  if(kmap.count(kadr.kadr_id))
     form = kmap[kadr.kadr_id];
  if(form)
  {
   //TODO - Удалить из формы
  }

}
//---------------------------------------------------------------------------


void     __fastcall TMdbKadrConstuctor::setup_kadr_form(TKadrForm * form)
{
       form->clear();
       form->set_kadr(kadr);
       form->Caption    = this->get_text(kadr.name_id);
       form->BkgndImage = this->get_text(kadr.image_id);
       kadr_entryes_table::iterator beg = entry_table.begin(),end = entry_table.end();
       mdb_record rec;
       while(beg<end)
       {
        get_record(beg->rec_id,rec);
        MDB_SHORT_RECORD sr;
        sr.rec_id = rec.rec_id;
        sr.value  = rec.value;
        sr.diag   = rec.diag;
        sr.state  = rec.state;
        sr.time   = rec.time;
        sr.min_value = rec.min_value;
        sr.max_value = rec.max_value;

        form->add_entry(*beg,sr,get_text(rec.sname),get_text(rec.name));
        form->editable = TRUE;
        form->on_modify_kadr   = on_kadr_modify;
        form->on_select_entry  = on_select_entry;
        form->on_delete_entry  = on_delete_entry;
        ++beg;
       }
     form->Repaint();
}

void     __fastcall TMdbKadrConstuctor::open_kadr_form()
{
 TTreeNode * node = KadrsTree->Selected;
 if(node)
 {
  DWORD kadr_id = (DWORD)node->Data;
  TKadrForm * form = NULL;
  if(kmap.count(kadr_id))
     form = kmap[kadr_id];
   if(!form)
      {
       form = new TKadrForm(this);
       form->editable = TRUE;
       kmap[kadr_id] = form;
       setup_kadr_form(form);
       form->OnDragOver = KadrEntryList->OnDragOver;
       form->OnDragDrop = KadrEntryList->OnDragDrop;
       form->OnDestroy  = on_kadr_destroy;
       
       form->on_apply   = on_apply;
       form->on_undo    = on_undo;
       form->record_operations = record_operations;
      }
      form->Visible = true;
      form->SetFocus();

 }
}

void   __fastcall TMdbKadrConstuctor::on_kadr_destroy(TObject * Sender)
{
  TKadrForm *kform = dynamic_cast<TKadrForm*>(Sender);
  if(kform)
  {
   kmap.erase(kform->kadr->kadr_id);
  }
}




void   __fastcall TMdbKadrConstuctor::on_kadr_modify(TKadrForm * form,DWORD ch_mask,mdb_kadr_entry * entry)
{
  if(form->kadr->kadr_id == kadr.kadr_id)
  {
   apply_mask|= ch_mask;
   if(ch_mask&MDB_KADR_FIELD_SIZE)
     kadr.kadr_size = form->kadr->kadr_size;

   if((ch_mask & MDB_KADR_FIELD_ENTRYES) && entry)
    {
      kadr_entryes_table::iterator  beg =  entry_table.begin(),end = entry_table.end();
      while(beg<end)
      {
       if(beg->rec_id == entry->rec_id && beg->number == entry->number)
        {
          beg->position = entry->position;beg->size = entry->size;
          beg->options  = entry->options;
          beg = end;
        }
        ++beg;
      }
    }
  }
}

TListItem * __fastcall TMdbKadrConstuctor::find_list_item(mdb_kadr_entry * entry)
{
 for(int i = 0;i<this->KadrEntryList->Items->Count;i++)
 {
   TListItem * item = KadrEntryList->Items->Item[i];
   if((DWORD)item->Data == entry->rec_id && (DWORD)atoi(item->Caption.c_str()) == entry->number)
     return item;
 }
 return NULL;
}

void   __fastcall TMdbKadrConstuctor::on_delete_entry(TKadrForm * form,DWORD ch_mask,mdb_kadr_entry * entry)
{
  if(form->kadr->kadr_id == kadr.kadr_id)
  {
    TListItem * item = find_list_item(entry);
    if(item)
      item->Delete();
  }
}

void   __fastcall TMdbKadrConstuctor::on_select_entry(TKadrForm * form,DWORD ch_mask,mdb_kadr_entry * entry)
{
  if(form->kadr->kadr_id == kadr.kadr_id)
  {
    if(entry)
    {
     TListItem * item = find_list_item(entry);
     if(item)
       {
        item->Selected = true;
        item->Focused  = true;
        item->MakeVisible(false);
       }
    }
    else
    {
     TListItem * item = this->KadrEntryList->Selected;
     if(item)
        item->Selected = false;
    }
  }
}


void   __fastcall TMdbKadrConstuctor::on_kadr_close (TKadrForm * form)
{
  kmap.erase(form->kadr->kadr_id);
}



void __fastcall TMdbKadrConstuctor::KadrsTreeDblClick(TObject *Sender)
{
 open_kadr_form();
}
//---------------------------------------------------------------------------



void __fastcall TMdbKadrConstuctor::SpeedButton1Click(TObject *Sender)
{
  char file_name[MAX_PATH<<2];
  char save_curr_dir[MAX_PATH<<2];
  GetCurrentDirectory(sizeof(save_curr_dir),save_curr_dir);
  if(!OpenPicture(Handle,file_name,sizeof(file_name),"Выбор фонового рисунка",NULL,NULL,TRUE))
  {
   KadrImage->Text = file_name;
   if(kmap.count(kadr.kadr_id))
   {
     TKadrForm * form = kmap[kadr.kadr_id];
     if(form)
       {
        form->BkgndImage = file_name;
        form->Repaint();
       }
   }

  }
   SetCurrentDirectory(save_curr_dir);
}
//---------------------------------------------------------------------------


void __fastcall TMdbKadrConstuctor::EntryPrecisionChange(TObject *Sender)
{
  TListItem * item = KadrEntryList->Selected;
  mdb_kadr_entry * entry = find_entry(item);
  if(entry)
    {
     entry->options.prec = atoi(EntryPrecision->Text.c_str());
     apply_mask|=MDB_KADR_FIELD_ENTRYES;
     TKadrForm * kform = kmap.count(kadr.kadr_id) ? kmap[kadr.kadr_id] : NULL;
     if(kform)
       kform->update_entry(entry);
    }
}
//---------------------------------------------------------------------------

void __fastcall TMdbKadrConstuctor::KadrEntryListDragOver(TObject *Sender,
      TObject *Source, int X, int Y, TDragState State, bool &Accept)
{
 TTreeView * tv = dynamic_cast<TTreeView*>(Source);
 if(tv && tv->Name == "MdbTree")
    Accept = true;
}
//---------------------------------------------------------------------------

void __fastcall TMdbKadrConstuctor::KadrEntryListDragDrop(TObject *Sender,
      TObject *Source, int X, int Y)
{
 TMediumDBFrm * frm =dynamic_cast<TMediumDBFrm *>(Owner);
 if(frm)
 {
   mdb_kadr_entry entry(kadr.kadr_id,frm->DragRecordId,entry_table.size());
   entry.options.prec = 2;
   entry.position.x   = X,entry.position.y = Y;
   add_list_item(entry);
   entry_table.insert(std::lower_bound(entry_table.begin(),entry_table.end(),entry),entry);
   if(kmap.count(kadr.kadr_id))
   {
    TKadrForm * kform = kmap[kadr.kadr_id];
    if(kform)
    {
     mdb_record rec;
     get_record(entry.rec_id,rec);
     mdb_short_record sr(rec) ;
     kform->add_entry(entry,sr,get_text(rec.sname),get_text(rec.name));
    }
   }
   apply_mask|=MDB_KADR_FIELD_ENTRYES;
 }
}
//---------------------------------------------------------------------------

void __fastcall TMdbKadrConstuctor::SKadrNameChange(TObject *Sender)
{
 apply_mask|=MDB_KADR_FIELD_SNAME;
}
//---------------------------------------------------------------------------


void __fastcall TMdbKadrConstuctor::EntryNumberChange(TObject *Sender)
{
  TListItem * item = KadrEntryList->Selected;
  mdb_kadr_entry * entry = find_entry(item);
  if(entry)
    {
     KadrEntryList->AlphaSort();
     entry->number = atoi(EntryNumber->Text.c_str());
     item->Caption = EntryNumber->Text;
     apply_mask|=MDB_KADR_FIELD_ENTRYES;
     TKadrForm * kform = kmap.count(kadr.kadr_id) ? kmap[kadr.kadr_id] : NULL;
     if(kform)
       kform->update_entry(entry);
    }

}
//---------------------------------------------------------------------------

void __fastcall TMdbKadrConstuctor::FormClose(TObject *Sender,
      TCloseAction &Action)
{
   Action = caFree;
}
//---------------------------------------------------------------------------

void __fastcall TMdbKadrConstuctor::NormalValueClick(TObject *Sender)
{
  TListItem * item = KadrEntryList->Selected;
  if(item)
  {
    mdb_kadr_entry * entry = find_entry(item);
    if(entry && entry->options.normal_value != (WORD)NormalValue->ItemIndex)
    {
     entry->options.normal_value = NormalValue->ItemIndex;
     apply_mask |=MDB_KADR_FIELD_ENTRYES;
     TKadrForm * kform = kmap.count(kadr.kadr_id) ? kmap[kadr.kadr_id] : NULL;
     if(kform)
       kform->update_entry(entry);
    }
   }

}
//---------------------------------------------------------------------------

void __fastcall TMdbKadrConstuctor::ImageCentrClick(TObject *Sender)
{
  kadr.kadr_flags = 0;
  if(ImageCentr->Checked)
  kadr.kadr_flags |= MDB_KADR_FLAG_IMAGE_CENTER;
  if(ImageStretch->Checked)
  kadr.kadr_flags |= MDB_KADR_FLAG_IMAGE_STRETCH;
  TKadrForm * kform = kmap.count(kadr.kadr_id) ? kmap[kadr.kadr_id] : NULL;
  if(kform)
    kform->set_kadr(kadr);

  apply_mask|= MDB_KADR_FIELD_FLAGS;

}
//---------------------------------------------------------------------------


void __fastcall TMdbKadrConstuctor::AnalogDrawAsChange(TObject *Sender)
{
  TListItem * item = KadrEntryList->Selected;
  if(item)
  {
    mdb_kadr_entry * entry = find_entry(item);
    if(entry && entry->options.paint_as != (WORD)DiscreteDrawAs->ItemIndex)
    {
     entry->options.paint_as = AnalogDrawAs->ItemIndex;
     apply_mask |=MDB_KADR_FIELD_ENTRYES;
     TKadrForm * kform = kmap.count(kadr.kadr_id) ? kmap[kadr.kadr_id] : NULL;
     if(kform)
       kform->update_entry(entry);
    }
   }

}
//---------------------------------------------------------------------------

void __fastcall TMdbKadrConstuctor::record_operations(TKadrForm * Sender,DWORD record_id,DWORD op_code)
{
  mod_iface.call(MDBCM_RECORD_OPERATION,record_id,op_code);
}

