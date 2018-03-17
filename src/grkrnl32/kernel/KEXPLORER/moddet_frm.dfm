object ModuleDefine: TModuleDefine
  Left = 383
  Top = 249
  Caption = #1054#1087#1088#1077#1076#1077#1083#1077#1085#1080#1077' '#1052#1086#1076#1091#1083#1103
  ClientHeight = 315
  ClientWidth = 372
  Color = clBtnFace
  Constraints.MaxWidth = 380
  Constraints.MinWidth = 380
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poOwnerFormCenter
  OnClose = FormClose
  OnDestroy = FormDestroy
  DesignSize = (
    372
    315)
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 6
    Top = 1
    Width = 31
    Height = 13
    Caption = #1040#1076#1088#1077#1089
  end
  object Edit1: TEdit
    Left = 3
    Top = 17
    Width = 283
    Height = 21
    TabOrder = 0
  end
  object Memo1: TMemo
    Left = 2
    Top = 47
    Width = 366
    Height = 237
    Anchors = [akLeft, akTop, akBottom]
    TabOrder = 1
    ExplicitHeight = 264
  end
  object Button1: TButton
    Left = 293
    Top = 15
    Width = 75
    Height = 25
    Caption = #1054#1087#1088#1077#1076#1077#1083#1080#1090#1100
    Default = True
    TabOrder = 2
    OnClick = Button1Click
  end
end
