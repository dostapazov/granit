object Form1: TForm1
  Left = 471
  Top = 256
  Caption = #1050#1072#1083#1100#1082#1091#1083#1103#1090#1086#1088' '#1084#1072#1090' '#1074#1099#1088#1072#1078#1077#1085#1080#1081
  ClientHeight = 361
  ClientWidth = 611
  Color = clBtnFace
  Font.Charset = RUSSIAN_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  Icon.Data = {
    0000010001002020100000000000E80200001600000028000000200000004000
    0000010004000000000080020000000000000000000000000000000000000000
    000000008000008000000080800080000000800080008080000080808000C0C0
    C0000000FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF000000
    0000000000000000000000000000000000000000000000000000000000000000
    000B3B3B3B33330000BB3B3000000000B3B3B3B3333333030BB3303300000003
    3330000000000030BB0B3B3330000003333B8B8383333033BBBB333330000000
    3333333333330B3B3BB33B3B3B0000000B3B3B3333330B3B33B3BBB3330000B3
    B3B3B3B333330B3B333BBB333B0003333000000000000B3B33BBBBB3B3000300
    000B3B3B33330B3B333BB0B33B000030B3B3B3B3B3330B3B333BBBB3B3000303
    3333333333330B33333BBB3B3B000003000B888383830BB33333333BB0000000
    33330000000000B33B3333BB300000033000B3B3B3B3B0BB33330BBB00000000
    0B3B3B3B3B3B3B0BB33B3BB00000000033333333333333300B33330000000000
    3000BBB838383830003000000000000003333380000000000000000000000000
    3338000B3B3B3B3B3B000000000000000330B3B3B3B3B3B3B3B3300000000000
    0003333FFFFFF33333333300000000000003088BBBB3B3B3B300030000000000
    000033333BBBBB3B3B33300000000000000333B3B3BBBBB3B3B3330000000000
    0000333B3BBBBBBB333330000000000000000003B3B3BFFFFB00000000000000
    0000000000000000000000000000000000000000000000000000000000000000
    000000000000000000000000000000000000000000000000000000000000FFFF
    FFFFFE003C1FF000000FE0000007C0000003C0000003C0000001C00000018000
    00010000000100000001000000010000000180000003C0000003C0000007E000
    000FE000001FE00000FFE00000FFE000007FF000003FF800001FFC00001FFC00
    001FFC00001FFE00003FFF00007FFFE003FFFFFFFFFFFFFFFFFFFFFFFFFF}
  OldCreateOrder = False
  Position = poScreenCenter
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object spl1: TSplitter
    Left = 444
    Top = 0
    Height = 320
    Align = alRight
  end
  object Panel1: TPanel
    Left = 0
    Top = 320
    Width = 611
    Height = 41
    Align = alBottom
    TabOrder = 0
    DesignSize = (
      611
      41)
    object Label3: TLabel
      Left = 462
      Top = 20
      Width = 47
      Height = 13
      Anchors = [akRight, akBottom]
      Caption = #1058#1086#1095#1085#1086#1089#1090#1100
    end
    object Button1: TButton
      Left = 11
      Top = 8
      Width = 75
      Height = 25
      Anchors = [akLeft, akBottom]
      Caption = 'Calc'
      Default = True
      TabOrder = 0
      OnClick = Button1Click
    end
    object Button2: TButton
      Left = 95
      Top = 8
      Width = 75
      Height = 25
      Anchors = [akLeft, akBottom]
      Caption = 'Bench'
      TabOrder = 1
      OnClick = Button2Click
    end
    object CheckBox1: TCheckBox
      Left = 189
      Top = 16
      Width = 115
      Height = 17
      Anchors = [akLeft, akBottom]
      Caption = #1054#1076#1085#1086' '#1074#1099#1088#1072#1078#1077#1085#1080#1077
      TabOrder = 2
    end
    object HexOut: TCheckBox
      Left = 306
      Top = 16
      Width = 85
      Height = 17
      Anchors = [akLeft, akBottom]
      Caption = 'HEX OUT'
      TabOrder = 3
    end
    object Edit1: TEdit
      Left = 534
      Top = 12
      Width = 65
      Height = 21
      Anchors = [akRight, akBottom]
      TabOrder = 4
      Text = '3'
    end
  end
  object Panel2: TPanel
    Left = 0
    Top = 0
    Width = 444
    Height = 320
    Align = alClient
    Caption = 'Panel2'
    TabOrder = 1
    DesignSize = (
      444
      320)
    object Label2: TLabel
      Left = 3
      Top = -2
      Width = 66
      Height = 19
      Caption = #1060#1086#1088#1084#1091#1083#1072
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'Times New Roman'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Expr: TMemo
      Left = -1
      Top = 17
      Width = 442
      Height = 303
      Anchors = [akLeft, akTop, akRight, akBottom]
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Courier New'
      Font.Style = []
      HideSelection = False
      ParentFont = False
      ScrollBars = ssHorizontal
      TabOrder = 0
      WordWrap = False
    end
  end
  object Panel3: TPanel
    Left = 447
    Top = 0
    Width = 164
    Height = 320
    Align = alRight
    Caption = 'Panel3'
    TabOrder = 2
    DesignSize = (
      164
      320)
    object Label1: TLabel
      Left = 26
      Top = 0
      Width = 76
      Height = 19
      Anchors = [akTop]
      Caption = #1056#1077#1079#1091#1083#1100#1090#1072#1090':'
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'Times New Roman'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Results: TMemo
      Left = 1
      Top = 16
      Width = 162
      Height = 303
      Anchors = [akLeft, akTop, akRight, akBottom]
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      TabOrder = 0
    end
  end
end
