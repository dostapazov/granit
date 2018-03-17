object AppMdm: TAppMdm
  Left = 589
  Top = 260
  Width = 503
  Height = 406
  Caption = #1058#1077#1089#1090' '#1089#1074#1103#1079#1080' '#1089' Processmdm.gkm'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  OnResize = FormResize
  OnShow = FormShow
  DesignSize = (
    495
    379)
  PixelsPerInch = 96
  TextHeight = 13
  object GroupBox1: TGroupBox
    Left = 0
    Top = -2
    Width = 495
    Height = 139
    Anchors = [akLeft, akTop, akRight]
    TabOrder = 0
    object CheckBox1: TCheckBox
      Tag = 512
      Left = 3
      Top = 28
      Width = 170
      Height = 17
      Caption = #1044#1072#1085#1085#1099#1077' '
      TabOrder = 0
      OnClick = CheckBox1Click
    end
    object CheckBox2: TCheckBox
      Tag = 32
      Left = 3
      Top = 44
      Width = 170
      Height = 17
      Caption = #1052#1077#1090#1082#1072' '#1074#1088#1077#1084#1077#1085#1080
      TabOrder = 1
      OnClick = CheckBox1Click
    end
    object CheckBox3: TCheckBox
      Tag = 256
      Left = 3
      Top = 60
      Width = 170
      Height = 17
      Caption = #1055#1077#1088#1089#1086#1085#1072#1083#1100#1085#1072#1103' '#1076#1080#1072#1075#1085#1086#1089#1090#1080#1082#1072
      TabOrder = 2
      OnClick = CheckBox1Click
    end
    object CheckBox4: TCheckBox
      Tag = 128
      Left = 4
      Top = 76
      Width = 170
      Height = 17
      Caption = #1052#1072#1089#1082#1072' '#1080#1079#1084#1077#1085#1077#1085#1080#1081
      TabOrder = 3
      OnClick = CheckBox1Click
    end
    object CheckBox5: TCheckBox
      Tag = 64
      Left = 4
      Top = 93
      Width = 170
      Height = 17
      Caption = #1048#1084#1077#1085#1072
      TabOrder = 4
      OnClick = CheckBox1Click
    end
    object Button1: TButton
      Left = 176
      Top = 10
      Width = 75
      Height = 25
      Caption = #1042#1099#1079#1086#1074
      TabOrder = 5
      OnClick = Button1Click
    end
    object Button2: TButton
      Left = 177
      Top = 40
      Width = 75
      Height = 25
      Caption = #1055#1086#1076#1087#1080#1089#1082#1072
      TabOrder = 6
      OnClick = Button1Click
    end
    object AddrMask: TEdit
      Left = 264
      Top = 16
      Width = 121
      Height = 21
      TabOrder = 7
      Text = '255.255.255.255'
    end
    object CheckBox7: TCheckBox
      Left = 264
      Top = 48
      Width = 139
      Height = 17
      Caption = #1055#1077#1088#1080#1086#1076#1080#1095#1077#1089#1082#1080#1081' '#1074#1099#1079#1086#1074
      TabOrder = 8
      OnClick = CheckBox7Click
    end
    object CallPeriod: TEdit
      Left = 404
      Top = 46
      Width = 64
      Height = 21
      TabOrder = 9
      Text = '5'
      Visible = False
      OnChange = CallPeriodChange
    end
    object CheckBox6: TCheckBox
      Left = 177
      Top = 113
      Width = 76
      Height = 17
      Caption = #1055#1072#1091#1079#1072
      TabOrder = 10
    end
    object Button3: TButton
      Left = 236
      Top = 106
      Width = 121
      Height = 25
      Caption = #1054#1095#1080#1089#1090#1080#1090#1100' '#1084#1086#1085#1080#1090#1086#1088
      TabOrder = 11
      OnClick = Button3Click
    end
    object CheckBox8: TCheckBox
      Tag = 1024
      Left = 3
      Top = 11
      Width = 170
      Height = 17
      Caption = #1044#1080#1072#1075#1085#1086#1089#1090#1080#1082#1072' '
      TabOrder = 12
      OnClick = CheckBox1Click
    end
    object Button4: TButton
      Left = 178
      Top = 71
      Width = 120
      Height = 25
      Caption = #1054#1090#1082#1072#1079' '#1086#1090' '#1087#1086#1076#1087#1080#1089#1082#1080
      TabOrder = 13
      OnClick = Button1Click
    end
    object CheckBox9: TCheckBox
      Tag = 2
      Left = 5
      Top = 111
      Width = 132
      Height = 17
      Caption = #1063#1072#1089#1090#1080#1095#1085#1099#1077' '#1076#1072#1085#1085#1099#1077
      TabOrder = 14
      OnClick = CheckBox1Click
    end
  end
  object MonitorBox: TGroupBox
    Left = 0
    Top = 136
    Width = 493
    Height = 225
    Anchors = [akLeft, akTop, akRight, akBottom]
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    TabOrder = 1
  end
  object StatusBar1: TStatusBar
    Left = 0
    Top = 360
    Width = 495
    Height = 19
    Panels = <
      item
        Width = 250
      end
      item
        Width = 50
      end>
    SimplePanel = False
  end
  object Timer1: TTimer
    Enabled = False
    OnTimer = Timer1Timer
    Left = 408
    Top = 8
  end
end
