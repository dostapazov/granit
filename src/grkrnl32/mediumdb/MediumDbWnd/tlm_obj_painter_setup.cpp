//---------------------------------------------------------------------------
#pragma hdrstop
#include <vcl.h>
#include "tlm_obj_painter_setup.h"
#include "kadrs_editor.h"
#include "mediumdb_frm.h"
#include <tlm_painter.hpp>
#include <freeimage.hpp>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "gkmodule_form"
#pragma resource "*.dfm"
TTlmObjPainterSetup *TlmObjPainterSetup;
//---------------------------------------------------------------------------
__fastcall TTlmObjPainterSetup::TTlmObjPainterSetup(TComponent* Owner)
	: TGKModuleForm(Owner)
{


   paint_variant = (LONG)tlmpaint::painter_dll::get_instance()->paint_var_create(0) ;
   //TMediumDBFrm::painter_dll(PMCMD_CREATE_PAINT_VARIANT_EMPTY,0,0);
   ZeroMemory(&paint_param,sizeof(paint_param));
   paint_param.dw_size    = sizeof(paint_param);
   paint_param.short_name  =  L"Тест";
   paint_param.position.x = 10;
   paint_param.position.y = 10;
   paint_param.record.state |= MDBR_STATE_DISCRETE;
   ComboBox1->Text = UnicodeString();
   OffColor->Color         = clBlack;
   OnColor->Color          = clYellow;

}
//---------------------------------------------------------------------------

void __fastcall TTlmObjPainterSetup::bSelImageClick(TObject *Sender)
{
  TImage * image = dynamic_cast<TImage *>(Sender);

  if(image && Opd1->Execute(Handle))
  {

   if(image == Image1)
    {
     set_image(Opd1->FileName);
    }
    else
    set_image_mask(Opd1->FileName);
    TryBox->Repaint();
  }

}
//---------------------------------------------------------------------------

void     __fastcall TTlmObjPainterSetup::after_set_gkhandle()
{
  TGKModuleForm::after_set_gkhandle();
  read_variants();
}

BOOL     __fastcall TTlmObjPainterSetup::get_variant (DWORD idx,mdb_paint_variant & pv )
{
     return  mod_iface.call(MDBCM_PAINTVARS_ENUM,idx,(LPARAM)&pv) == GKH_RET_SUCCESS ? TRUE : FALSE;
}

void     __fastcall TTlmObjPainterSetup::read_variants()
{
  DWORD idx = 0;
  mdb_paint_variant pv;
  ComboBox1->Items->Clear();
  while(get_variant(idx,pv))
   {
     ComboBox1->Items->Add(TMediumDBFrm::get_text(mod_iface,pv.name_id));
     ++idx;
   }

}


void     __fastcall TTlmObjPainterSetup::on_module_state(GKHANDLE mod,DWORD state)
{
}

void     __fastcall TTlmObjPainterSetup::on_module_config_change(GKHANDLE mod,LPMODULE_CONFIG_DATA mcd)
{

}


void __fastcall TTlmObjPainterSetup::FormClose(TObject *Sender, TCloseAction &Action)
{
  Action = caFree;
  tlmpaint::painter_dll::get_instance()->call(PMCMD_DELETE_PAINT_VARIANT,paint_variant,0);
  paint_variant = 0;
}
//---------------------------------------------------------------------------

void     __fastcall TTlmObjPainterSetup::set_image     (UnicodeString fName)
{
     UnicodeString str;
     image_file_name = fName;
     int bmp_width = 0,cnt = 0;
     Image1->Picture->Bitmap->Dormant();
     Image1->Picture->Bitmap->ReleaseHandle();
     if(fName.Length())
     {
      HBITMAP bmp = freeimage::TFreeImage::load_image(fName.c_str());
      Image1->Picture->Bitmap->Handle = bmp;
      cnt = 1;
      bmp_width = Image1->Picture->Bitmap->Width;
      str.printf(L"Параметры изображения : cx = %d, cy = %d ",Image1->Picture->Bitmap->Width,Image1->Picture->Bitmap->Height);
     }
     Image1->Refresh() ;
     StatusBar1->Panels->Items[0]->Text = str;
     tlmpaint::painter_dll::get_instance()->call(PMCMD_PAINT_VAR_CTRL_SET_IMAGE,paint_variant,(LPARAM)image_file_name.c_str());
     this->eWidth->Text = bmp_width;
     this->eCount->Text = cnt;
}

void     __fastcall TTlmObjPainterSetup::set_image_mask(UnicodeString fName)
{
 mask_file_name = fName;
 Image2->Picture->Bitmap->Dormant();
 Image2->Picture->Bitmap->ReleaseHandle();
 if(fName.Length())
 {
      HBITMAP bmp = freeimage::TFreeImage::load_image(fName.c_str());
      Image2->Picture->Bitmap->Handle = bmp;
 }
 Image2->Refresh() ;
 tlmpaint::painter_dll::get_instance()->call(PMCMD_PAINT_VAR_CTRL_SET_MASK,paint_variant,(LPARAM)mask_file_name.c_str());
}



void __fastcall TTlmObjPainterSetup::eWidthChange(TObject *Sender)
{
 int tw = get_element_width();
 tlmpaint::painter_dll::get_instance()->call(PMCMD_PAINT_VAR_CTRL_SET_WIDTH,paint_variant,tw);
 eCount->Text = Image1->Picture->Bitmap->Width/tw;
 TryBox->Repaint();
}
//---------------------------------------------------------------------------


void __fastcall TTlmObjPainterSetup::OffColorClick(TObject *Sender)
{
  TPaintBox * pb = dynamic_cast<TPaintBox*>(Sender);
  if(pb)
  {
    ColorDlg->Color = pb->Color;
    if(ColorDlg->Execute(Handle))
    {
     pb->Color = ColorDlg->Color;
     pb->Refresh();
     TryBox->Invalidate();
    }
  }
}
//---------------------------------------------------------------------------

void __fastcall TTlmObjPainterSetup::OffColorPaint(TObject *Sender)
{
   TPaintBox * pb = dynamic_cast<TPaintBox*>(Sender);
   if(pb)
   {
    pb->Canvas->FillRect(pb->ClientRect);
   }
}
//---------------------------------------------------------------------------
int __fastcall TTlmObjPainterSetup::get_element_width()
{
 int ret = _wtoi(this->eWidth->Text.c_str());
 if(!ret) ret = 16;
 return ret;
}

int __fastcall TTlmObjPainterSetup::get_element_height()
{
   return Image1->Picture->Bitmap->Height;
}

void     __fastcall TTlmObjPainterSetup::paint_tlm_object()
{
  //TryBox->Repaint();
  TryBox->Canvas->FillRect(TryBox->ClientRect);
  paint_param.paint_dc   = TryBox->Canvas->Handle;
  paint_param.size.cx    = get_element_width();
  paint_param.size.cy    = get_element_height();
  paint_param.position.x = (TryBox->Width  - paint_param.size.cx - 16)/2;
  paint_param.position.y = (TryBox->Height - paint_param.size.cy)/2;
  paint_param.record.value = ObjValue->Position;
  paint_param.bkgnd_color  = OffColor->Color;
  paint_param.color        = OnColor->Color;

  if((DWORD)paint_param.record.value )
      paint_param.record.state |= MDBR_STATE_NORMAL;
     else
      paint_param.record.state &= ~MDBR_STATE_NORMAL;
      paint_param.normal_value = cbAbnormal->Checked ? !(DWORD)paint_param.record.value : (DWORD)paint_param.record.value;
   if(cbTuTr->Checked)
      paint_param.record.state |= MDBR_STATE_TUTR_ACTIVE;
      else
      paint_param.record.state &= ~MDBR_STATE_TUTR_ACTIVE;
   if(cbWorkOut->Checked)
      paint_param.record.state |= MDBR_STATE_WORKOUT;
      else
      paint_param.record.state &= ~MDBR_STATE_WORKOUT;
      paint_param.changed = RadioGroup1->ItemIndex;
      if(paint_param.changed > 1) paint_param.changed = 3;
      paint_param.record.diag  = cbBadDiag->Checked ? MDBR_DIAG_BAD : MDBR_DIAG_GOOD;
      tlmpaint::painter_dll::get_instance()->call(PMCMD_DO_PAINT_VARIANT,(LPARAM)&paint_param,paint_variant);
  //ReleaseDC(TryBox->Handle,paint_param.paint_dc);

}

void __fastcall TTlmObjPainterSetup::cbObjValueClick(TObject *Sender)
{
 switch(cbObjValue->State)
 {
  case  cbUnchecked: ObjValueText->Text = 0;break;
  case  cbChecked  : ObjValueText->Text = 1;break;
 }
 Application->ProcessMessages();
 TryBox->Invalidate();
}
//---------------------------------------------------------------------------

void __fastcall TTlmObjPainterSetup::cbSwitchedClick(TObject *Sender)
{
 //tlmpaint::painter_dll::get_instance()->call(PMCMD_PAINT_VAR_CTRL_SET_SWITCHED,(LPARAM)paint_variant,cbSwitched->Checked);
 //paint_tlm_object();
}
//---------------------------------------------------------------------------


void __fastcall TTlmObjPainterSetup::bAddVariantClick(TObject *Sender)
{
  //Создание варианта
  mdb_paint_variant mpv;
  mpv.name_id   = TMediumDBFrm::set_text(mod_iface,-1,ComboBox1->Text);
  mpv.image_id  = TMediumDBFrm::set_text(mod_iface,-1,image_file_name);
  mpv.mask_id   = TMediumDBFrm::set_text(mod_iface,-1,mask_file_name );
  mpv.switched  = 0;//cbSwitched->Checked;
  mpv.multi_position  = get_multipos();
  mpv.size.cy   = get_element_height();
  mpv.size.cx   = get_element_width ();
  if(GKH_RET_SUCCESS == mod_iface.call(MDBCM_PAINTVARS_ADD,(LPARAM)&mpv,0))
  {
    bAddVariant->Enabled = false;
    bUpdVariant->Enabled = true;
    ComboBox1->Items->Add(ComboBox1->Text);
  }

}
//---------------------------------------------------------------------------

void __fastcall TTlmObjPainterSetup::bUpdVariantClick(TObject *Sender)
{
  mdb_paint_variant mpv;
  if(get_variant(ComboBox1->ItemIndex,mpv))
  {
   mpv.name_id   = TMediumDBFrm::set_text(mod_iface,mpv.name_id,ComboBox1->Text);
   mpv.image_id  = TMediumDBFrm::set_text(mod_iface,mpv.image_id,image_file_name);
   mpv.mask_id   = TMediumDBFrm::set_text(mod_iface,mpv.mask_id,mask_file_name );
   mpv.switched  = 0;//cbSwitched->Checked;
   mpv.multi_position = get_multipos();
   mpv.size.cy        = get_element_height();
   mpv.size.cx        = get_element_width ();
   if(GKH_RET_SUCCESS == mod_iface.call(MDBCM_PAINTVARS_UPD,(LPARAM)&mpv,0))
   {

   }
  }
}
//---------------------------------------------------------------------------

void __fastcall TTlmObjPainterSetup::ComboBox1Change(TObject *Sender)
{
 int idx = ComboBox1->Items->IndexOf(ComboBox1->Text);
 bAddVariant->Enabled = idx<0 ? true  : false ;
 bUpdVariant->Enabled = idx<0 ? false : true  ;
}
//---------------------------------------------------------------------------

void __fastcall TTlmObjPainterSetup::ComboBox1Select(TObject *Sender)
{
 mdb_paint_variant mpv;
 if(get_variant(ComboBox1->ItemIndex,mpv))
 {
  UnicodeString str;
  str = TMediumDBFrm::get_text(mod_iface,mpv.image_id);
  set_image(str);
  str = TMediumDBFrm::get_text(mod_iface,mpv.mask_id);
  set_image_mask(str);
  //cbSwitched->Checked = mpv.switched;
  set_multipos(mpv.multi_position,mpv.switched);
  tlmpaint::painter_dll::get_instance()->call(PMCMD_PAINT_VAR_CTRL_SET_MULTIPOS,paint_variant,get_multipos());
  eWidth->Text = Image1->Picture->Bitmap->Handle ? mpv.size.cx : 0;
  eCount->Text = Image1->Picture->Bitmap->Handle ? Image1->Picture->Bitmap->Width/mpv.size.cx : 0;
  bAddVariant->Enabled = false;
  bUpdVariant->Enabled = true;
 }

}
//---------------------------------------------------------------------------

void __fastcall TTlmObjPainterSetup::eCountChange(TObject *Sender)
{

 int el_count = _wtoi(eCount->Text.c_str());
 if(el_count)
 {
  int wdt = Image1->Picture->Bitmap->Width/el_count;
  eWidth->Text = wdt;
 }
}
//---------------------------------------------------------------------------
int      __fastcall TTlmObjPainterSetup::get_multipos()
{
  return 1<<(multipos_size->ItemIndex) ;
}

void     __fastcall TTlmObjPainterSetup::set_multipos(int multi_pos,bool switched)
{
  if(switched)
     multi_pos = 2;
    else
     multi_pos = max(2,multi_pos);
     multipos_size->ItemIndex = (int)(log(fabs(float(multi_pos)))/log(2.0));
}


void __fastcall TTlmObjPainterSetup::TryBoxPaint(TObject *Sender)
{
  TryBox->Canvas->FillRect(TryBox->ClientRect);
  paint_tlm_object();
}
//---------------------------------------------------------------------------




void __fastcall TTlmObjPainterSetup::multipos_sizeChange(TObject *Sender)
{
  tlmpaint::painter_dll::get_instance()->call(PMCMD_PAINT_VAR_CTRL_SET_MULTIPOS,paint_variant,get_multipos());
  Application->ProcessMessages();
  TryBox->Refresh();
}
//---------------------------------------------------------------------------



void __fastcall TTlmObjPainterSetup::ObjValueClick(TObject *Sender, TUDBtnType Button)

{
  switch(ObjValue->Position)
  {
    case 0  : cbObjValue->State = cbUnchecked; break;
    case 1  : cbObjValue->State = cbChecked  ; break;
    default : cbObjValue->State = cbGrayed   ; break;
  }
  TryBox->Refresh();
}
//---------------------------------------------------------------------------

void __fastcall TTlmObjPainterSetup::cbAbnormalClick(TObject *Sender)
{
  Application->ProcessMessages();
  TryBox->Refresh();
}
//---------------------------------------------------------------------------

void __fastcall TTlmObjPainterSetup::ObjValueTextChange(TObject *Sender)
{
  TryBox->Refresh();
}
//---------------------------------------------------------------------------


