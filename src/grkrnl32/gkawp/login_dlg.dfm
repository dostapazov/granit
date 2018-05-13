object LoginDlg: TLoginDlg
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = #1040#1074#1090#1086#1088#1080#1079#1072#1094#1080#1103
  ClientHeight = 85
  ClientWidth = 385
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = False
  Position = poScreenCenter
  OnHide = FormHide
  OnKeyDown = FormKeyDown
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 8
    Top = 11
    Width = 72
    Height = 13
    Caption = #1055#1086#1083#1100#1079#1086#1074#1072#1090#1077#1083#1100
  end
  object Label2: TLabel
    Left = 9
    Top = 41
    Width = 37
    Height = 13
    Caption = #1055#1072#1088#1086#1083#1100
  end
  object user_name: TEdit
    Left = 85
    Top = 8
    Width = 200
    Height = 21
    BevelInner = bvNone
    BevelKind = bkFlat
    BorderStyle = bsNone
    TabOrder = 0
  end
  object user_passw: TEdit
    Left = 85
    Top = 38
    Width = 200
    Height = 21
    BevelInner = bvNone
    BevelKind = bkFlat
    BorderStyle = bsNone
    PasswordChar = '*'
    TabOrder = 3
  end
  object bLogin: TBitBtn
    Left = 299
    Top = 8
    Width = 75
    Height = 24
    Caption = 'Login'
    Kind = bkYes
    NumGlyphs = 2
    TabOrder = 2
  end
  object bCancel: TBitBtn
    Left = 299
    Top = 39
    Width = 75
    Height = 23
    Caption = #1054#1090#1084#1077#1085#1072
    Kind = bkCancel
    NumGlyphs = 2
    TabOrder = 1
    OnClick = bCancelClick
  end
  object StatusBar1: TStatusBar
    Left = 0
    Top = 66
    Width = 385
    Height = 19
    Panels = <
      item
        Width = 50
      end>
  end
end
