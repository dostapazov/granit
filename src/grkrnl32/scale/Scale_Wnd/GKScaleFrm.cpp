/*-----------------------------------------------------------------------------
 Unit Name: GKScaleFrm
 Author:    vitus
 Purpose:
 History:
-----------------------------------------------------------------------------*/


//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "GKScaleFrm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "gkmodule_form"
#pragma resource "*.dfm"
TGKScaleFrm *GKScaleFrm;
//---------------------------------------------------------------------------
__fastcall TGKScaleFrm::TGKScaleFrm(TComponent* Owner,HWND owner_hwnd,GKHANDLE handle,DWORD win_type)
        : TGKModuleForm(Owner,owner_hwnd,handle)
{
  Grid->Rows[0]->Add("Мин. квант");
  Grid->Rows[0]->Add("Макс. квант");
  Grid->Rows[0]->Add("A0");
  Grid->Rows[0]->Add("A1");
  Grid->Rows[0]->Add("A2");
  Grid->Rows[0]->Add("A3");
  Grid->Rows[0]->Add("A4");
  Grid->Rows[0]->Add("A5");
  Grid->Rows[0]->Add("%");
  Grid->ColWidths[0]=70;
  Grid->ColWidths[1]=70;
  Grid->ColWidths[8]=38;

  incalc=LINE_UPDATE;
  if (win_type==MODULE_WND_TYPE_CONFIG)
    Pages->ActivePageIndex=2;
  else
    Pages->ActivePageIndex=0;
  const f_sz=MAX_PATH;
  wchar_t filename[f_sz];
  bool b=(mod_iface.call(CM_SC_GET_FILENAME_W,(LPARAM)filename,(LPARAM)&f_sz)==GKH_RET_SUCCESS);
  if (b)
  {
    STScaleFile->Caption=filename;
    EdFileName->Text=filename;
    FillSizes();
    FillTree();
  }
  TBNew->Enabled=b && wcslen(filename);
  BtShrink->Enabled=b && wcslen(filename);
  set_notify(mod_handle,MNF_SCALE);
}

__fastcall TGKScaleFrm::~TGKScaleFrm()
{
  if (extfile) delete extfile;
  extfile=NULL;
  reset_notify(mod_handle,MNF_SCALE);
}

//---------------------------------------------------------------------------
void __fastcall TGKScaleFrm::Splitter2Moved(TObject *Sender)
{
  Grid->Height=Splitter2->Top-84;
}
//---------------------------------------------------------------------------

void __fastcall TGKScaleFrm::Splitter2CanResize(TObject *Sender,
      int &NewSize, bool &Accept)
{
  if (NewSize>TabGraph->ClientHeight-153) NewSize=TabGraph->ClientHeight-153;
}
//---------------------------------------------------------------------------

void __fastcall TGKScaleFrm::TabGraphResize(TObject *Sender)
{
  Grid->Height=Splitter2->Top-84;
  if (Splitter2->Top<150) Panel1->Height=TabGraph->ClientHeight-153;
}
//---------------------------------------------------------------------------

void __fastcall TGKScaleFrm::SBFileNameClick(TObject *Sender)
{
  if (!EdFileName->Text.IsEmpty())
    OpenDialog->FileName=EdFileName->Text;
  if (OpenDialog->Execute())
  {
    EdFileName->Text=OpenDialog->FileName;
    EdFileNameExit(this);
  }
}
//---------------------------------------------------------------------------

void __fastcall TGKScaleFrm::BtLoadClick(TObject *Sender)
{
  if (extfile) delete extfile;
  extfile=NULL;
  Tree->Color=clWindow;
  if (EdFileName->Text.IsEmpty()) return;
  if (STScaleFile->Caption!=EdFileName->Text || !mod_iface.is_running())
  {
    extfile=new TIndexedFile(EdFileName->Text.c_str(),true);
    if (!extfile || !extfile->file_opened())
    {
      Caption="Диспетчер шкал";
      if (extfile) delete extfile;
      extfile=NULL;
      TBNew->Enabled=false;
      return;
    }
    Tree->Color=clMoneyGreen;
    BtSet->Enabled=true;
  } else
  {
    if (extfile) delete extfile;
    extfile=NULL;
    Tree->Color=clWindow;
  }
  TBNew->Enabled=true;
  BtLoad->Enabled=false;
  BtSaveAs->Enabled=false;
  BtShrink->Enabled=true;
  FillSizes();
  FillTree();
  Pages->ActivePageIndex=0;
}
//---------------------------------------------------------------------------

void __fastcall TGKScaleFrm::FillTree()
{
  Tree->Items->Clear();
  ClearFields();
  SCALE_ENUM enum_par;
  enum_par.dw_size=sizeof(SCALE_ENUM);
  DWORD dw, count;
  AnsiString str;
  if (extfile)
    count=extfile->get_count();
  else
    if (mod_iface.call(CM_SC_COUNT,(LPARAM)&count,0)!=GKH_RET_SUCCESS) return;
  if (count==INDF_ERR_ERROR) return;
  for (DWORD i=0; i<count; i++)
  {
    if (extfile)
    {
      enum_par.dw_size=sizeof(SCALE_ENUM);
      dw=extfile->read(i,&enum_par,enum_par.dw_size,false);
      if (dw==DWORD(-2))
        ZeroMemory((byte *)&enum_par+sizeof(SCALE_ENUM)-sizeof(wchar_t),sizeof(wchar_t));
      dw=GKH_RET_SUCCESS;
    } else
      dw=mod_iface.call(CM_SC_ENUM,i,(LPARAM)&enum_par);
    if (dw==GKH_RET_SUCCESS)
    {
      str=ConstructName(enum_par.name,enum_par.scale_id);
      Tree->Items->AddObject(0,str,(void *)enum_par.scale_id);
    }
  }
  Tree->AlphaSort();
}

void __fastcall TGKScaleFrm::EdFileNameExit(TObject *Sender)
{
  if (ExtractFileExt(EdFileName->Text).IsEmpty())
    EdFileName->Text=ChangeFileExt(EdFileName->Text,"."+OpenDialog->DefaultExt);
  EdFileName->Text=ExpandFileName(EdFileName->Text);
}
//---------------------------------------------------------------------------

void __fastcall TGKScaleFrm::FillSizes()
{
  DWORD dw;
  CBIndex->Checked=false;
  STCount->Caption="";
  STSize->Caption="";
  STUnused->Caption="";
  if (extfile)
  {
    Caption="Диспетчер шкал - "+ExtractFileName(EdFileName->Text);
    CBIndex->Checked=extfile->index_used();
    STCount->Caption=extfile->get_count();
    dw=extfile->get_size();
    STSize->Caption=FloatToStrF(float(dw)/1024,ffFixed,3,1)+" кБ";
    if (dw)
      STUnused->Caption=FloatToStrF(float(extfile->get_unused_size())*100/dw,ffFixed,3,1)+"%";
  } else
  {
    if (STScaleFile->Caption.IsEmpty()) return;
    Caption="Диспетчер шкал - "+ExtractFileName(STScaleFile->Caption);
    bool b=false;
    if (mod_iface.call(CM_SC_INDEX_USED,(LPARAM)&b,0)==GKH_RET_SUCCESS)
      CBIndex->Checked=b;
    if (mod_iface.call(CM_SC_COUNT,(LPARAM)&dw,0)==GKH_RET_SUCCESS)
      STCount->Caption=dw;
    if (mod_iface.call(CM_SC_FILE_SIZE,(LPARAM)&dw,0)==GKH_RET_SUCCESS)
      STSize->Caption=FloatToStrF(float(dw)/1024,ffFixed,3,1)+" кБ";
    if (dw)
    {
      DWORD dw1;
      if (mod_iface.call(CM_SC_FILE_UNUSED,(LPARAM)&dw1,0)==GKH_RET_SUCCESS)
        STUnused->Caption=FloatToStrF(float(dw1)*100/dw,ffFixed,3,1)+"%";
    }
  }
}

void __fastcall TGKScaleFrm::BtSaveAsClick(TObject *Sender)
{
  if (EdFileName->Text.IsEmpty() || STScaleFile->Caption==EdFileName->Text) return;
  if (extfile)
  {
    char str[MAX_PATH];
    extfile->get_file_name(str,MAX_PATH);
    if (EdFileName->Text!=str)
    {
      extfile->close();
      if (!CopyFile(str,AnsiString(EdFileName->Text).c_str(),true))
        ShowMessage("Файл с таким именем уже существует");
      extfile->open();
    }
  } else
    mod_iface.call(CM_SC_SET_FILENAME_A,(LPARAM)(EdFileName->Text.c_str()),1);
  FillSizes();
  FillTree();
}
//---------------------------------------------------------------------------

void __fastcall TGKScaleFrm::BtSetClick(TObject *Sender)
{
  if (EdFileName->Text.IsEmpty() || STScaleFile->Caption==EdFileName->Text) return;
  if (extfile) delete extfile;
  extfile=NULL;
  Tree->Color=clMoneyGreen;
  bool b=(mod_iface.call(CM_SC_SET_FILENAME_A,(LPARAM)(EdFileName->Text.c_str()),2)==GKH_RET_SUCCESS);
  if (b)
  {
    STScaleFile->Caption=EdFileName->Text;
    Tree->Color=clWindow;
  }
  TBNew->Enabled=b;
  BtLoad->Enabled=!b;
  BtSaveAs->Enabled=!b;
  BtSet->Enabled=!b;
  BtShrink->Enabled=b;
  FillSizes();
  FillTree();
}
//---------------------------------------------------------------------------

void __fastcall TGKScaleFrm::BtShrinkClick(TObject *Sender)
{
  if (extfile)
    extfile->shrink();
  else
    mod_iface.call(CM_SC_SHRINK,0,0);
  FillSizes();
  FillTree();
}
//---------------------------------------------------------------------------

void __fastcall TGKScaleFrm::TBNewClick(TObject *Sender)
{
  TTreeNode *node;
  if (Pages->ActivePageIndex)
  {
    Pages->ActivePageIndex=0;
    PagesChange(this);
  }
  if (Tree->Selected)
    node=Tree->Items->Insert(Tree->Selected,NEW_SCALE_NAME);
  else
    node=Tree->Items->Add(0,NEW_SCALE_NAME);
  Tree->Selected=node;
  s_name->SetFocus();
  TBNew->Enabled    = false;
  TBDelete->Enabled = false;
  TBApplay->Enabled = true;
  TBCancel->Enabled = true;
}
//---------------------------------------------------------------------------

AnsiString __fastcall TGKScaleFrm::ConstructName(AnsiString ed, DWORD id)
{
  AnsiString ret;
  if (ed.IsEmpty())
    ret="id: "+IntToStr((int)id);
  else
    ret=ed;
  return ret;
}

bool __fastcall TGKScaleFrm::NameExists(AnsiString name)
{
  if (name==NEW_SCALE_NAME) return true;
  for (int i=0; i<Tree->Items->Count; i++)
    if (name==Tree->Items->Item[i]->Text)
      return true;
  return false;
}

void __fastcall TGKScaleFrm::TBDeleteClick(TObject *Sender)
{
  if (MessageDlg("Шкала '"+ConstructName(s_name->Text,STid->Caption.ToInt())+"' будет удалена",
                 mtWarning,TMsgDlgButtons() << mbYes << mbNo,0)==mrNo) return;
  DWORD dw=(DWORD)(Tree->Selected->Data);
  if (extfile)
    extfile->erase(dw,true);
  else
    mod_iface.call(CM_SC_DELETE,dw,0);
  Tree->Items->Delete(Tree->Selected);
  if (!Tree->Items->Count) ClearFields();
}
//---------------------------------------------------------------------------

void __fastcall TGKScaleFrm::TBApplayClick(TObject *Sender)
{
  if (Tree->Selected->Text!=s_name->Text &&
      !s_name->Text.IsEmpty() &&
      NameExists(s_name->Text))
  {
    s_name->SetFocus();
    return;
  }
  if (Pages->ActivePageIndex==1 && TBSave->Enabled)
  {
    TBSaveClick(Sender);
    if (TBSave->Enabled) return;
  }
  Tree->SetFocus();
  if (!mask)
  {
    TBCancelClick(Sender);
    return;
  }
  if (mask & S_NAME_CHANGE) scl.SetName(s_name->Text.c_str());
  if (mask & MIN_WARN_CHANGE)
  {
    if (MinWarn->Text.IsEmpty())
    {
      scl.warn_min=0;
      scl.limits&=~SCL_LIMIT_MINWARN;
    } else
    {
      scl.warn_min=MinWarn->Text.ToDouble();
      scl.limits|=SCL_LIMIT_MINWARN;
    }
  }
  if (mask & MAX_WARN_CHANGE)
  {
    if (MaxWarn->Text.IsEmpty())
    {
      scl.warn_max=0;
      scl.limits&=~SCL_LIMIT_MAXWARN;
    } else
    {
      scl.warn_max=MaxWarn->Text.ToDouble();
      scl.limits|=SCL_LIMIT_MAXWARN;
    }
  }
  if (mask & MIN_ALARM_CHANGE)
  {
    if (MinAlarm->Text.IsEmpty())
    {
      scl.alarm_min=0;
      scl.limits&=~SCL_LIMIT_MINALARM;
    } else
    {
      scl.alarm_min=MinAlarm->Text.ToDouble();
      scl.limits|=SCL_LIMIT_MINALARM;
    }
  }
  if (mask & MAX_ALARM_CHANGE)
  {
    if (MaxAlarm->Text.IsEmpty())
    {
      scl.alarm_max=0;
      scl.limits&=~SCL_LIMIT_MAXALARM;
    } else
    {
      scl.alarm_max=MaxAlarm->Text.ToDouble();
      scl.limits|=SCL_LIMIT_MAXALARM;
    }
  }
  bool old=(Tree->Selected->Text!=NEW_SCALE_NAME);
  DWORD dw=scl.GetBuffer(0,0);
  byte *buf=new byte[dw];
  scl.GetBuffer(buf,dw);
  if (old)
  {
    if (extfile)
      dw=extfile->write(scl.GetId(),buf,dw,true);
    else
      dw=mod_iface.call(CM_SC_SET,scl.GetId(),(LPARAM)buf);
  } else
  {
    DWORD id;
    if (extfile)
      dw=extfile->write(buf,dw);
    else
      dw=mod_iface.call(CM_SC_CREATE,(LPARAM)buf,(LPARAM)&id);
    scl.SetAll(buf,0);
  }
  delete [] buf;
  if (dw==(DWORD)GKH_RET_ERROR)
    FillFields((DWORD)Tree->Selected->Data,old);
  else
  {
    Tree->Selected->Text=ConstructName(scl.w_str(),scl.GetId());
    if (!old)
    {
      Tree->Selected->Data=(void *)scl.GetId();
      STid->Caption=scl.GetId();
    }
  }
  SetMask(DWORD(-1),false);
  TBNew->Enabled=true;
  TBDelete->Enabled=true;
  TBApplay->Enabled=false;
  TBCancel->Enabled=false;
}
//---------------------------------------------------------------------------

void __fastcall TGKScaleFrm::TBCancelClick(TObject *Sender)
{
  if (Pages->ActivePageIndex==1 && TBSave->Enabled)
  {
    TBUndoClick(Sender);
    if (TBSave->Enabled) return;
  }
  if (Tree->Selected->Text==NEW_SCALE_NAME)
  {
    ClearFields();
    Tree->Items->Delete(Tree->Selected);
  }
  else
    FillFields(scl.GetId(),true);
  SetMask(DWORD(-1),false);
  TBNew->Enabled=true;
  TBDelete->Enabled=true;
  TBApplay->Enabled=false;
  TBCancel->Enabled=false;
}
//---------------------------------------------------------------------------


void __fastcall TGKScaleFrm::TreeChange(TObject *Sender, TTreeNode *Node)
{
  bool old=false;
  if (Tree->Selected)
  {
    old=(Tree->Selected->Text!=NEW_SCALE_NAME);
    FillFields((DWORD)Tree->Selected->Data,old);
    if (Pages->ActivePageIndex==1) PagesChange(this);
  }
  TBDelete->Enabled=old;
}
//---------------------------------------------------------------------------


void __fastcall TGKScaleFrm::ClearFields()
{
  STid->Caption="";
  s_name->Text="";
  min_kvant->Caption="";
  max_kvant->Caption="";
  min_scale->Caption="";
  max_scale->Caption="";
  ListView->Clear();
  MinWarn->Text="";
  MaxWarn->Text="";
  MinAlarm->Text="";
  MaxAlarm->Text="";
  EdMinValue->Text="";
  EdMaxValue->Text="";
}

void __fastcall TGKScaleFrm::FillFields(DWORD id, bool old)
{
  if (!old)
  {
    scl.SetAll(0,0);
    ClearFields();
    return;
  }
  byte *buf = NULL;
  DWORD dw;
  if (extfile)
  {
    dw=extfile->read(id,0,0,true);
    if (dw==DWORD(-1)) return;
    buf=new byte[dw];
    dw=extfile->read(id,buf,dw,true);
    if (dw<DWORD(-1))
      scl.SetAll(buf,dw);
  }else
  {
    if (mod_iface.call(CM_SC_SCALE_SIZE,id,(LPARAM)&dw)!=GKH_RET_SUCCESS) return;
    buf=new byte[dw];
    *(DWORD *)buf=dw;
    if (mod_iface.call(CM_SC_GET,id,(LPARAM)buf)==GKH_RET_SUCCESS)
      scl.SetAll(buf,dw);
  }
  if (buf) delete [] buf;
  STid->Caption=scl.GetId();
  s_name->Text=scl.w_str();
  ReFillSegments();
  if (scl.limits & SCL_LIMIT_MINWARN)
    MinWarn->Text=FloatToStrF(scl.warn_min,ffFixed,5,3);
  else
    MinWarn->Clear();
  if (scl.limits & SCL_LIMIT_MAXWARN)
    MaxWarn->Text=FloatToStrF(scl.warn_max,ffFixed,5,3);
  else
    MaxWarn->Clear();
  if (scl.limits & SCL_LIMIT_MINALARM)
    MinAlarm->Text=FloatToStrF(scl.alarm_min,ffFixed,5,3);
  else
    MinAlarm->Clear();
  if (scl.limits & SCL_LIMIT_MAXALARM)
    MaxAlarm->Text=FloatToStrF(scl.alarm_max,ffFixed,5,3);
  else
    MaxAlarm->Clear();
}

void __fastcall TGKScaleFrm::SetMask(DWORD m, bool set)
{
  if (!Tree->Selected) return;
  if (set)
    mask|=m;
  else
    mask&=~m;
  if (Tree->Selected->Text!=NEW_SCALE_NAME)
  {
    bool b=mask;
    TBNew->Enabled=!b;
    TBDelete->Enabled=!b;
    TBApplay->Enabled=b;
    TBCancel->Enabled=b;
  }
}

void __fastcall TGKScaleFrm::MinWarnExit(TObject *Sender)
{
  if (!MinWarn->Text.IsEmpty())
  {
    try
    {
      MinWarn->Text.ToDouble();
    } catch(...)
    {
      MinWarn->SetFocus();
    }
  }
}
//---------------------------------------------------------------------------

void __fastcall TGKScaleFrm::MaxWarnExit(TObject *Sender)
{
  if (!MaxWarn->Text.IsEmpty())
  {
    try
    {
      MaxWarn->Text.ToDouble();
    } catch(...)
    {
      MaxWarn->SetFocus();
    }
  }
}
//---------------------------------------------------------------------------

void __fastcall TGKScaleFrm::MinAlarmExit(TObject *Sender)
{
  if (!MinAlarm->Text.IsEmpty())
  {
    try
    {
      MinAlarm->Text.ToDouble();
    } catch(...)
    {
      MinAlarm->SetFocus();
    }
  }
}
//---------------------------------------------------------------------------

void __fastcall TGKScaleFrm::MaxAlarmExit(TObject *Sender)
{
  if (!MaxAlarm->Text.IsEmpty())
  {
    try
    {
      MaxAlarm->Text.ToDouble();
    } catch(...)
    {
      MaxAlarm->SetFocus();
    }
  }
}
//---------------------------------------------------------------------------

void __fastcall TGKScaleFrm::s_nameChange(TObject *Sender)
{
  if (s_name->Text==scl.w_str())
    SetMask(S_NAME_CHANGE,false);
  else
    SetMask(S_NAME_CHANGE,true);
}
//---------------------------------------------------------------------------

void __fastcall TGKScaleFrm::PagesChange(TObject *Sender)
{
  if (Pages->ActivePageIndex==1)
  {
    FillGraph();
    Grid->Col=1;
    Grid->Col=0;
  }
  if (Pages->ActivePageIndex==2) FillSizes();
  bool b=(Pages->ActivePageIndex==1);
  TBAdd->Visible=b;
  TBDel->Visible=b;
  TBSave->Visible=b;
  TBUndo->Visible=b;
  TBExp->Visible=b;
  TBAdd->Enabled=(Tree->Selected!=NULL);
}
//---------------------------------------------------------------------------

void __fastcall TGKScaleFrm::ListViewDblClick(TObject *Sender)
{
  Pages->ActivePageIndex=1;
  PagesChange(this);
}
//---------------------------------------------------------------------------

void __fastcall TGKScaleFrm::FillGraph()
{
  int i;
  for (i=0; i<ListView->Items->Count; i++)
  {
    Grid->Rows[i+1]->Clear();
    Grid->Rows[i+1]->Add(ListView->Items->Item[i]->Caption);
    Grid->Rows[i+1]->AddStrings(ListView->Items->Item[i]->SubItems);
  }
  if (ListView->Items->Count)
  {
    Grid->RowCount=ListView->Items->Count+1;
    Grid->Options << goEditing;
    EdMinValue->Enabled=true;
    EdMaxValue->Enabled=true;
    if (RGPercent->ItemIndex) RGPercentClick(this);
  } else
  {
    Grid->RowCount=2;
    Grid->Rows[1]->Clear();
    Grid->Options >> goEditing;
    EdMinValue->Enabled=false;
    EdMaxValue->Enabled=false;
  }
  TBDel->Enabled=ListView->Items->Count;
  PaintBox->Refresh();
}
//---------------------------------------------------------------------------

void __fastcall TGKScaleFrm::RGInputClick(TObject *Sender)
{
  EdMinValue->Visible=RGInput->ItemIndex;
  EdMaxValue->Visible=RGInput->ItemIndex;
  if (RGInput->ItemIndex)
  {
    if ((Grid->Col>1) && (Grid->Col<8)) Grid->Col=0;
    if (RGInput->ItemIndex) CalcLineSegs(LINE_MIN_X);
  }
}

void __fastcall TGKScaleFrm::RGPercentClick(TObject *Sender)
{
  float x,delta=scl.MaxY()-scl.MinY();
  for (int i=1;i<Grid->RowCount; i++)
  {
    try
    {
      if (RGPercent->ItemIndex)
        x=delta*Grid->Cells[8][i].ToDouble()/100;
      else
        if (delta)
          x=Grid->Cells[8][i].ToDouble()/delta*100;
        else
          x=1.5;
    }
    catch(...)
    {
      if (RGPercent->ItemIndex)
        x=delta*1.5/100;
      else
        x=1.5;
    }
    Grid->Cells[8][i]=FloatToStrF(x,ffFixed,3,1);
  }
}
//---------------------------------------------------------------------------

void __fastcall TGKScaleFrm::GridSelectCell(TObject *Sender, int ACol,
      int ARow, bool &CanSelect)
{
  if (!Tree->Selected) CanSelect=false;
  if (TBSave->Enabled && ARow!=Grid->Row) CanSelect=false;
  if (RGInput->ItemIndex && (ACol>1) && (ACol<8)) CanSelect=false;
  TBDel->Enabled=(ListView->Items->Count && !TBSave->Enabled);
  if (CanSelect && !TBSave->Enabled)
  {
    TSegment *seg=scl.GetSegment(ARow-1);
    if (seg)
    {
      tseg=*seg;
    }
  }
}
//---------------------------------------------------------------------------

void __fastcall TGKScaleFrm::Splitter1Moved(TObject *Sender)
{
  TBSep->Width=Splitter1->Left-92;
}
//---------------------------------------------------------------------------


void __fastcall TGKScaleFrm::TBAddClick(TObject *Sender)
{
  SetMask(SEGMENTS_CHANGE,true);
  Grid->RowCount=ListView->Items->Count+2;
  Grid->Row=Grid->RowCount-1;
  SetSegmentMask(true);
  Grid->Options << goEditing;
  EdMinValue->Enabled=true;
  EdMaxValue->Enabled=true;
  tseg.Clear();
  Grid->Rows[Grid->Row]->Strings[0]=tseg.min_kvant;
  Grid->Rows[Grid->Row]->Strings[1]=tseg.max_kvant;
  Grid->Rows[Grid->Row]->Strings[2]=FloatToStrF(tseg.k[0],ffFixed,8,6);
  Grid->Rows[Grid->Row]->Strings[3]=FloatToStrF(tseg.k[1],ffFixed,8,6);
  Grid->Rows[Grid->Row]->Strings[4]=FloatToStrF(tseg.k[2],ffFixed,8,6);
  Grid->Rows[Grid->Row]->Strings[5]=FloatToStrF(tseg.k[3],ffFixed,8,6);
  Grid->Rows[Grid->Row]->Strings[6]=FloatToStrF(tseg.k[4],ffFixed,8,6);
  Grid->Rows[Grid->Row]->Strings[7]=FloatToStrF(tseg.k[5],ffFixed,8,6);
  Grid->Rows[Grid->Row]->Strings[8]=FloatToStrF(tseg.percent,ffFixed,3,1);
  if (RGInput->ItemIndex) CalcLineSegs(LINE_MIN_X);
}
//---------------------------------------------------------------------------

void __fastcall TGKScaleFrm::TBDelClick(TObject *Sender)
{
  SetMask(SEGMENTS_CHANGE,true);
  scl.DeleteSegment(Grid->Row-1);
  Grid->Rows[Grid->RowCount-1]->Clear();
  ReFillSegments();
  if (!ListView->Items->Count)
    TBDel->Enabled=false;
  TSegment *seg=scl.GetSegment(Grid->Row-1);
  if (seg)
    tseg=*seg;
  else
    tseg.Clear();
  if (RGInput->ItemIndex) CalcLineSegs(LINE_MIN_X);
}
//---------------------------------------------------------------------------

void __fastcall TGKScaleFrm::TBSaveClick(TObject *Sender)
{
  Grid->EditorMode=false;
  SetMask(SEGMENTS_CHANGE,true);
  AnsiString s=Grid->Cells[0][Grid->Row];
  if (Grid->RowCount-1==ListView->Items->Count)
    scl.DeleteSegment(Grid->Row-1);
  scl.AddSegment(&tseg);
  ReFillSegments();
  SetSegmentMask(false);
  SelectGridRow(s);
  PaintBox->Refresh();
}
//---------------------------------------------------------------------------

void __fastcall TGKScaleFrm::TBUndoClick(TObject *Sender)
{
  Grid->EditorMode=false;
  FillGraph();
  SetSegmentMask(false);
  if (!ListView->Items->Count)
    TBDel->Enabled=false;
  TSegment *seg=scl.GetSegment(Grid->Row-1);
  if (seg)
    tseg=*seg;
  else
    tseg.Clear();
  if (RGInput->ItemIndex) CalcLineSegs(LINE_MIN_X);
}
//---------------------------------------------------------------------------

void __fastcall TGKScaleFrm::TBExpClick(TObject *Sender)
{
//
}
//---------------------------------------------------------------------------

void __fastcall TGKScaleFrm::ReFillSegments()
{
  min_kvant->Caption=scl.MinKvant();
  max_kvant->Caption=scl.MaxKvant();
  min_scale->Caption=scl.MinY();
  max_scale->Caption=scl.MaxY();
  ListView->Clear();
  int i=0;
  TSegment *seg;
  TListItem *item;
  #pragma warn -8060
  while (seg=scl.GetSegment(i++))
  #pragma warn .8060
  {
    item=ListView->Items->Add();
    item->Caption=seg->min_kvant;
    item->SubItems->Add(seg->max_kvant);
    item->SubItems->Add(FloatToStrF(seg->k[0],ffFixed,8,6));
    item->SubItems->Add(FloatToStrF(seg->k[1],ffFixed,8,6));
    item->SubItems->Add(FloatToStrF(seg->k[2],ffFixed,8,6));
    item->SubItems->Add(FloatToStrF(seg->k[3],ffFixed,8,6));
    item->SubItems->Add(FloatToStrF(seg->k[4],ffFixed,8,6));
    item->SubItems->Add(FloatToStrF(seg->k[5],ffFixed,8,6));
    item->SubItems->Add(FloatToStrF(seg->percent,ffFixed,3,1));
  }
  FillGraph();
}

void __fastcall TGKScaleFrm::SelectGridRow(AnsiString str)
{
  for (int i=1; i<Grid->RowCount; i++)
    if (Grid->Cells[0][i]==str)
    {
      Grid->Row=i;
      return;
    }
}

void __fastcall TGKScaleFrm::PagesChanging(TObject *Sender,
      bool &AllowChange)
{
  if (Pages->ActivePageIndex==1 && TBSave->Enabled) AllowChange=false;
}
//---------------------------------------------------------------------------

void __fastcall TGKScaleFrm::PaintBoxPaint(TObject *Sender)
{
  int bx=scl.MinKvant(), ex=scl.MaxKvant();
  float by=scl.MinY(), ey=scl.MaxY();
  PaintBox->Canvas->Pen->Color=clTeal;
  PaintBox->Canvas->Pen->Width=1;
  int d=Screen->PixelsPerInch/6;
  for (int i=d; i<PaintBox->ClientWidth; i+=d)
  {
    PaintBox->Canvas->MoveTo(i,0);
    PaintBox->Canvas->LineTo(i,PaintBox->ClientHeight);
  }
  for (int i=PaintBox->ClientHeight-d; i>0; i-=d)
  {
    PaintBox->Canvas->MoveTo(0,i);
    PaintBox->Canvas->LineTo(PaintBox->ClientWidth,i);
  }
  float dx=ex-bx;
  float dy=ey-by;
  float KX=(dx!=0.0)?PaintBox->ClientWidth/dx:0.0;
  float KY=(dy!=0.0)?PaintBox->ClientHeight/dy:0.0;
  PaintBox->Canvas->Pen->Width =2;
  if (by<0 != ey<0)
  {
    //График пересекает ось OX
    float zero_y=(dy-((ey>0.0)?ey:-ey))*KY;
    PaintBox->Canvas->Pen->Color=TColor(RGB(255,255,0));
    PaintBox->Canvas->MoveTo(0,PaintBox->ClientHeight-zero_y);
    PaintBox->Canvas->LineTo(PaintBox->ClientWidth,PaintBox->ClientHeight-zero_y);
  }
  int i=0;
  int px,py;
  TSegment *seg;
  #pragma warn -8060
  while (seg=scl.GetSegment(i))
  #pragma warn .8060
  {
    if (Grid->Row==(i+1))
      PaintBox->Canvas->Pen->Color=clRed;
    else
      PaintBox->Canvas->Pen->Color=clLime;
    if (seg->IsLine())
    {
      px=(seg->min_kvant-bx)*KX;;
      py=(ey-seg->calc(seg->min_kvant))*KY;
      PaintBox->Canvas->MoveTo(px,py);
      px=(seg->max_kvant-bx)*KX;
      py=(ey-seg->calc(seg->max_kvant))*KY;
      PaintBox->Canvas->LineTo(px,py);
    } else
    {
      int b=seg->min_kvant, e=seg->max_kvant, d=(e-b)/100+1;
      float v;
      px=(b-bx)*KX;
      py=(ey-seg->calc(seg->min_kvant))*KY;
      PaintBox->Canvas->MoveTo(px,py);
      for(;b<=e;b+=d)
      {
        v=seg->calc(b);
        PaintBox->Canvas->LineTo((b-bx)*KX,(ey-v)*KY);
      }
    }
    i++;
  }
}
//---------------------------------------------------------------------------
/*
void __fastcall TGKScaleFrm::GridSetEditText(TObject *Sender, int ACol,
      int ARow, const AnsiString Value)
{
  if (Value.IsEmpty()) return;
  if (ACol<2)
  {
    try
    {
      int i=Value.ToInt();
      if (ACol)
        tseg.max_kvant=i;
      else
        tseg.min_kvant=i;
    } catch (...) {}
    if (RGInput->ItemIndex) CalcLineSegs(ACol?LINE_MAX_X:LINE_MIN_X);
  } else
  {
    try
    {
      float f=Value.ToDouble();
      if (ACol==8)
      {
        if (RGPercent->ItemIndex)
        {
          float d=tseg.max_y()-tseg.min_y();
          tseg.percent=(d==0.0)?1.5:f/d*100;
        } else
          tseg.percent=f;
      } else
        tseg.k[ACol-2]=f;
    } catch (...) {}
  }
  SetMask(SEGMENTS_CHANGE,true);
  SetSegmentMask(true);
}
*/
//---------------------------------------------------------------------------

void __fastcall TGKScaleFrm::EdFileNameChange(TObject *Sender)
{
  AnsiString s=EdFileName->Text;
  if (ExtractFileExt(s).IsEmpty())
    s=ChangeFileExt(s,"."+OpenDialog->DefaultExt);
  s=ExpandFileName(s);
  bool b;
  char str[MAX_PATH];
  if (extfile)
    extfile->get_file_name(str,MAX_PATH);
  else
    safe_strcpy(str,AnsiString(STScaleFile->Caption).c_str());
  b=(s==str && mod_iface.is_running());
  BtLoad->Enabled=!b;
  BtSaveAs->Enabled=!b;
  BtSet->Enabled=false;
  BtShrink->Enabled=b;
}
//---------------------------------------------------------------------------

void __fastcall TGKScaleFrm::TreeChanging(TObject *Sender, TTreeNode *Node,
      bool &AllowChange)
{
  AllowChange=!TBApplay->Enabled;        
}
//---------------------------------------------------------------------------

void __fastcall TGKScaleFrm::GridClick(TObject *Sender)
{
  PaintBox->Refresh();
  if (RGInput->ItemIndex) CalcLineSegs(LINE_MIN_X);
}
//---------------------------------------------------------------------------

void __fastcall TGKScaleFrm::FormCloseQuery(TObject *Sender,
      bool &CanClose)
{
  CanClose=!TBApplay->Enabled;
}
//---------------------------------------------------------------------------


void __fastcall TGKScaleFrm::EdMinValueChange(TObject *Sender)
{
  if (CalcLineSegs(LINE_MIN_Y))
  {
    SetMask(SEGMENTS_CHANGE,true);
    SetSegmentMask(true);
  }
}
//---------------------------------------------------------------------------

void __fastcall TGKScaleFrm::EdMaxValueChange(TObject *Sender)
{
  if (CalcLineSegs(LINE_MAX_Y))
  {
    SetMask(SEGMENTS_CHANGE,true);
    SetSegmentMask(true);
  }
}
//---------------------------------------------------------------------------

void __fastcall TGKScaleFrm::SetSegmentMask(bool set)
{
  TBAdd->Enabled=!set;
  TBDel->Enabled=!set;
  TBSave->Enabled=set;
  TBUndo->Enabled=set;
}

void __fastcall TGKScaleFrm::PaintBoxMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
  float f=scl.MaxKvant()-scl.MinKvant();
  f=f*X/PaintBox->ClientWidth+scl.MinKvant();
  DWORD kv=f+0.5;
  AnsiString s;
  f=0.0;
  scl.Calc(kv,f);
  s.printf("Кванты = %d Параметр = %.2f ",kv,f);
  TPoint p(X,Y);
  p=PaintBox->ClientToScreen(p);
  PaintBox->Hint=s;
  Application->ActivateHint(p);
}
//---------------------------------------------------------------------------

void __fastcall TGKScaleFrm::PaintBoxMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  DWORD kv=(scl.MaxKvant()-scl.MinKvant())*X/PaintBox->ClientWidth+scl.MinKvant();
  int i=0;
  TSegment *seg;
  #pragma warn -8060
  while (seg=scl.GetSegment(i++))
  #pragma warn .8060
  {
    if (seg->InRange(kv))
    {
      Grid->Row=i;
      break;
    }
  }
}
//---------------------------------------------------------------------------

LRESULT __fastcall TGKScaleFrm::on_gkhandle_notify (GKHANDLE from,LPNOTIFY_CODE nc,DWORD mask)
{
  if (mask==MNF_SCALE)
  {
    switch (nc->notify_code)
    {
      case MNCODE_SCALE_ID: on_scale_id(nc->notify_data[0]);break;
      case MNCODE_SCALE_REFRESH: on_scale_refresh();break;
      default: TGKModuleForm::on_gkhandle_notify(from,nc,mask);
    }
  }
  return GKH_RET_SUCCESS;
}
//---------------------------------------------------------------------------

void __fastcall TGKScaleFrm::on_scale_id(DWORD id)
{
  if (!extfile && Tree->Selected)
    if (scl.GetId()==id)
      if (mask) TBCancelClick(this);
}

void __fastcall TGKScaleFrm::on_scale_refresh()
{
  const f_sz=MAX_PATH;
  wchar_t filename[f_sz];
  if (mod_iface.call(CM_SC_GET_FILENAME_W,(LPARAM)filename,(LPARAM)&f_sz)==GKH_RET_SUCCESS)
    STScaleFile->Caption=filename;
  if (!extfile)
  {
    if (mask) TBCancelClick(this);
    EdFileName->Text=filename;
    scl.SetAll(0,0);
    FillSizes();
    FillTree();
    PagesChange(this);
  }
}

bool __fastcall TGKScaleFrm::CalcLineSegs(DWORD from)
{
  bool ret=false;
  if (incalc!=LINE_UPDATE) return ret;
  DWORD bx, ex;
  float by, ey;
  incalc=from;
  switch (incalc)
  {
    case LINE_UPDATE: break;
    case LINE_MIN_X:
    case LINE_MAX_X:
      try
      {
        bx=tseg.min_kvant;
        ex=tseg.max_kvant;
        EdMinValue->Text=FloatToStrF(tseg.calc(bx),ffFixed,4,2);
        EdMaxValue->Text=FloatToStrF(tseg.calc(ex),ffFixed,4,2);
      }
      catch(...){}
      break;
    case LINE_MIN_Y:
    case LINE_MAX_Y:
      try
      {
        by=EdMinValue->Text.ToDouble();
        ey=EdMaxValue->Text.ToDouble();
        if (tseg.max_kvant-tseg.min_kvant!=0)
        {
          tseg.SetLineK(by,ey);
          ret=true;
          for (int i=2; i<Grid->ColCount-1; i++)
            Grid->Cells[i][Grid->Row]=FloatToStrF(tseg.k[i-2],ffFixed,8,6);
        }
      }
      catch(...){}
      break;
  }
  incalc=LINE_UPDATE;
  return ret;
}

void __fastcall TGKScaleFrm::MinWarnChange(TObject *Sender)
{
  if (MinWarn->Text.IsEmpty())
  {
    if (scl.limits & SCL_LIMIT_MINWARN)
      SetMask(MIN_WARN_CHANGE,true);
    else
      SetMask(MIN_WARN_CHANGE,false);
  } else
  {
    float f;
    try
    {
      f=MinWarn->Text.ToDouble();
    } catch(...)
    {
      return;
    }
    if ((scl.limits & SCL_LIMIT_MINWARN) && (scl.warn_min==f))
      SetMask(MIN_WARN_CHANGE,false);
    else
      SetMask(MIN_WARN_CHANGE,true);
  }
}
//---------------------------------------------------------------------------

void __fastcall TGKScaleFrm::MaxWarnChange(TObject *Sender)
{
  if (MaxWarn->Text.IsEmpty())
  {
    if (scl.limits & SCL_LIMIT_MAXWARN)
      SetMask(MAX_WARN_CHANGE,true);
    else
      SetMask(MAX_WARN_CHANGE,false);
  } else
  {
    float f;
    try
    {
      f=MaxWarn->Text.ToDouble();
    } catch(...)
    {
      return;
    }
    if ((scl.limits & SCL_LIMIT_MAXWARN) && (scl.warn_max==f))
      SetMask(MAX_WARN_CHANGE,false);
    else
      SetMask(MAX_WARN_CHANGE,true);
  }
}
//---------------------------------------------------------------------------

void __fastcall TGKScaleFrm::MinAlarmChange(TObject *Sender)
{
  if (MinAlarm->Text.IsEmpty())
  {
    if (scl.limits & SCL_LIMIT_MINALARM)
      SetMask(MIN_ALARM_CHANGE,true);
    else
      SetMask(MIN_ALARM_CHANGE,false);
  } else
  {
    float f;
    try
    {
      f=MinAlarm->Text.ToDouble();
    } catch(...)
    {
      return;
    }
    if ((scl.limits & SCL_LIMIT_MINALARM) && (scl.alarm_min==f))
      SetMask(MIN_ALARM_CHANGE,false);
    else
      SetMask(MIN_ALARM_CHANGE,true);
  }
}
//---------------------------------------------------------------------------

void __fastcall TGKScaleFrm::MaxAlarmChange(TObject *Sender)
{
  if (MaxAlarm->Text.IsEmpty())
  {
    if (scl.limits & SCL_LIMIT_MAXALARM)
      SetMask(MAX_ALARM_CHANGE,true);
    else
      SetMask(MAX_ALARM_CHANGE,false);
  } else
  {
    float f;
    try
    {
      f=MaxAlarm->Text.ToDouble();
    } catch(...)
    {
      return;
    }
    if ((scl.limits & SCL_LIMIT_MAXALARM) && (scl.alarm_max==f))
      SetMask(MAX_ALARM_CHANGE,false);
    else
      SetMask(MAX_ALARM_CHANGE,true);
  }
}
//---------------------------------------------------------------------------

void __fastcall TGKScaleFrm::GridSetEditText(TObject *Sender, int ACol, int ARow,
          const UnicodeString Value)
{
  if (Value.IsEmpty()) return;
  if (ACol<2)
  {
    try
    {
      int i=Value.ToInt();
      if (ACol)
        tseg.max_kvant=i;
      else
        tseg.min_kvant=i;
    } catch (...) {}
    if (RGInput->ItemIndex) CalcLineSegs(ACol?LINE_MAX_X:LINE_MIN_X);
  } else
  {
    try
    {
      float f=Value.ToDouble();
      if (ACol==8)
      {
        if (RGPercent->ItemIndex)
        {
          float d=tseg.max_y()-tseg.min_y();
          tseg.percent=(d==0.0)?1.5:f/d*100;
        } else
          tseg.percent=f;
      } else
        tseg.k[ACol-2]=f;
    } catch (...) {}
  }
  SetMask(SEGMENTS_CHANGE,true);
  SetSegmentMask(true);

}
//---------------------------------------------------------------------------

