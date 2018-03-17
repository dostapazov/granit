object SettingsFrame: TSettingsFrame
  Left = 431
  Top = 371
  Align = alClient
  ClientHeight = 250
  ClientWidth = 435
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = True
  ExplicitWidth = 320
  ExplicitHeight = 240
  PixelsPerInch = 96
  TextHeight = 13
  object SettingsBox: TGroupBox
    Left = 0
    Top = 0
    Width = 435
    Height = 250
    Align = alClient
    TabOrder = 0
    object Label1: TLabel
      Left = 153
      Top = 16
      Width = 53
      Height = 13
      Caption = #1053#1086#1084#1077#1088' '#1055#1059
    end
    object Label2: TLabel
      Left = 8
      Top = 16
      Width = 77
      Height = 13
      Caption = #1053#1086#1084#1077#1088' '#1084#1086#1076#1077#1084#1072
    end
    object Label3: TLabel
      Left = 10
      Top = 32
      Width = 41
      Height = 13
      Caption = #1048#1084#1103' '#1055#1059
    end
    object Label4: TLabel
      Left = 9
      Top = 81
      Width = 114
      Height = 13
      Caption = #1052#1086#1076#1091#1083#1100' '#1074#1074#1086#1076#1072'/'#1074#1099#1074#1086#1076#1072
    end
    object Label5: TLabel
      Left = 10
      Top = 125
      Width = 85
      Height = 13
      Caption = #1048#1084#1103' '#1091#1089#1090#1088#1086#1081#1089#1090#1074#1072' '
    end
    object PuNum: TEdit
      Left = 208
      Top = 15
      Width = 84
      Height = 21
      BevelKind = bkFlat
      BorderStyle = bsNone
      TabOrder = 0
      Text = 'PuNum'
    end
    object ModemNum: TEdit
      Left = 96
      Top = 13
      Width = 41
      Height = 21
      BevelKind = bkFlat
      BorderStyle = bsNone
      TabOrder = 1
      Text = 'PUNumber'
    end
    object PuName: TEdit
      Left = 9
      Top = 49
      Width = 281
      Height = 21
      BevelKind = bkFlat
      BorderStyle = bsNone
      TabOrder = 2
      Text = 'PUNumber'
    end
    object IOModule: TEdit
      Left = 9
      Top = 95
      Width = 253
      Height = 21
      BevelKind = bkFlat
      BorderStyle = bsNone
      TabOrder = 3
      Text = 'PUNumber'
    end
    object Button1: TButton
      Left = 263
      Top = 96
      Width = 23
      Height = 19
      Caption = '...'
      TabOrder = 4
      OnClick = Button1Click
    end
    object IODevice: TComboBox
      Left = 12
      Top = 144
      Width = 273
      Height = 21
      BevelKind = bkFlat
      TabOrder = 5
      Text = 'IODevice'
      OnDropDown = IODeviceDropDown
    end
  end
  object OpenDialog1: TOpenDialog
    Filter = 
      #1052#1086#1076#1091#1083#1080' '#1042#1086#1076#1072'/'#1074#1099#1074#1086#1076#1072'|*.mio|'#1044#1080#1085#1072#1084#1080#1095#1077#1089#1082#1080#1077' '#1073#1080#1073#1083#1080#1086#1090#1077#1082#1080'|*.dll|'#1042#1089#1077' '#1092#1072#1081#1083#1099 +
      '|*.*'
    Options = [ofHideReadOnly, ofPathMustExist, ofFileMustExist, ofNoNetworkButton, ofEnableSizing, ofDontAddToRecent]
    OptionsEx = [ofExNoPlacesBar]
    Left = 376
    Top = 16
  end
end
