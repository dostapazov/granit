object IPClient: TIPClient
  Left = 385
  Top = 273
  Width = 543
  Height = 192
  Caption = 'IPClient'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 8
    Top = 16
    Width = 37
    Height = 13
    Caption = #1057#1077#1088#1074#1077#1088
  end
  object Label2: TLabel
    Left = 280
    Top = 16
    Width = 25
    Height = 13
    Caption = #1055#1086#1088#1090
  end
  object Label3: TLabel
    Left = 7
    Top = 44
    Width = 54
    Height = 13
    Caption = #1057#1086#1089#1090#1086#1103#1085#1080#1077
  end
  object connect_state: TLabel
    Left = 80
    Top = 43
    Width = 68
    Height = 13
    Caption = 'connect_state'
  end
  object ServerPort: TEdit
    Left = 313
    Top = 14
    Width = 92
    Height = 21
    BevelKind = bkFlat
    BorderStyle = bsNone
    TabOrder = 0
    Text = '45777'
  end
  object Button1: TButton
    Left = 412
    Top = 12
    Width = 75
    Height = 25
    Caption = #1055#1088#1080#1084#1077#1085#1080#1090#1100
    TabOrder = 1
  end
  object Panel1: TPanel
    Left = 0
    Top = 69
    Width = 535
    Height = 96
    Align = alBottom
    BevelOuter = bvLowered
    TabOrder = 2
    object Label4: TLabel
      Left = 16
      Top = 16
      Width = 74
      Height = 13
      Caption = #1055#1086#1083#1091#1095#1077#1085#1086' '#1073#1072#1081#1090
    end
    object Label5: TLabel
      Left = 204
      Top = 16
      Width = 36
      Height = 13
      Caption = #1082#1072#1076#1088#1086#1074
    end
    object Label6: TLabel
      Left = 16
      Top = 56
      Width = 87
      Height = 13
      Caption = #1054#1090#1087#1088#1072#1074#1083#1077#1085#1086' '#1073#1072#1081#1090
    end
    object Label7: TLabel
      Left = 204
      Top = 56
      Width = 37
      Height = 13
      Caption = #1050#1072#1076#1088#1086#1074
    end
    object RxBytes: TLabel
      Left = 112
      Top = 16
      Width = 3
      Height = 13
      Caption = '-'
    end
    object RxKadrs: TLabel
      Left = 264
      Top = 16
      Width = 3
      Height = 13
      Caption = '-'
    end
    object TxBytes: TLabel
      Left = 112
      Top = 57
      Width = 3
      Height = 13
      Caption = '-'
    end
    object TxKadrs: TLabel
      Left = 264
      Top = 57
      Width = 3
      Height = 13
      Caption = '-'
    end
  end
  object ServerAddr: TComboBox
    Left = 56
    Top = 14
    Width = 209
    Height = 21
    BevelInner = bvNone
    BevelKind = bkFlat
    ItemHeight = 13
    TabOrder = 3
    Text = '127.0.0.1'
    OnDropDown = ServerAddrDropDown
  end
  object Timer1: TTimer
    Enabled = False
    Interval = 500
    OnTimer = Timer1Timer
    Left = 496
    Top = 7
  end
end
