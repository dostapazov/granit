object GenLicenseForm: TGenLicenseForm
  Left = 0
  Top = 0
  Caption = #1043#1077#1085#1077#1088#1072#1090#1086#1088' '#1083#1080#1094#1077#1085#1079#1080#1081' '#1076#1083#1103' '#1084#1086#1076#1091#1083#1077#1081' '#1103#1076#1088#1072
  ClientHeight = 267
  ClientWidth = 590
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 16
    Top = 8
    Width = 60
    Height = 13
    Caption = #1044#1083#1103' '#1084#1086#1076#1091#1083#1103
  end
  object SpeedButton1: TSpeedButton
    Left = 416
    Top = 24
    Width = 23
    Height = 22
    Caption = '...'
    Flat = True
    OnClick = SpeedButton1Click
  end
  object bGenTrialLicense: TSpeedButton
    Left = 7
    Top = 51
    Width = 115
    Height = 22
    Caption = 'Generate Trial License'
    Enabled = False
    Flat = True
    OnClick = bGenTrialLicenseClick
  end
  object bOpenLicence: TSpeedButton
    Left = 134
    Top = 51
    Width = 70
    Height = 22
    Caption = 'Open license '
    Flat = True
    OnClick = bOpenLicenceClick
  end
  object bCheckLicense: TSpeedButton
    Left = 310
    Top = 51
    Width = 100
    Height = 22
    Caption = 'Check license valid'
    Flat = True
    OnClick = bCheckLicenseClick
  end
  object bIncTime: TSpeedButton
    Left = 419
    Top = 51
    Width = 73
    Height = 22
    Caption = 'inc time'
    Flat = True
    OnClick = bIncTimeClick
  end
  object bActivate: TSpeedButton
    Left = 511
    Top = 51
    Width = 69
    Height = 22
    Caption = 'Activate'
    Flat = True
    OnClick = bActivateClick
  end
  object SpeedButton2: TSpeedButton
    Left = 224
    Top = 51
    Width = 70
    Height = 22
    Caption = 'Close license '
    Flat = True
    OnClick = SpeedButton2Click
  end
  object ModuleName: TEdit
    Left = 16
    Top = 24
    Width = 401
    Height = 21
    TabOrder = 0
    OnChange = ModuleNameChange
  end
  object GroupBox1: TGroupBox
    Left = 2
    Top = 83
    Width = 580
    Height = 156
    Caption = 'Licence content'
    TabOrder = 1
    object Label2: TLabel
      Left = 8
      Top = 24
      Width = 76
      Height = 13
      Caption = 'Computer name'
    end
    object Label3: TLabel
      Left = 8
      Top = 49
      Width = 55
      Height = 13
      Caption = 'Volume info'
    end
    object Label4: TLabel
      Left = 8
      Top = 75
      Width = 61
      Height = 13
      Caption = 'Memory Info'
    end
    object Label5: TLabel
      Left = 7
      Top = 102
      Width = 18
      Height = 13
      Caption = 'UID'
    end
    object Label6: TLabel
      Left = 357
      Top = 24
      Width = 67
      Height = 13
      Caption = 'Worked Times'
    end
    object Label7: TLabel
      Left = 359
      Top = 53
      Width = 61
      Height = 13
      Caption = 'Expired Time'
    end
    object Label8: TLabel
      Left = 8
      Top = 129
      Width = 45
      Height = 13
      Caption = 'Comment'
    end
    object CompName: TEdit
      Left = 95
      Top = 21
      Width = 250
      Height = 21
      TabOrder = 0
    end
    object VolumeInfo: TEdit
      Left = 95
      Top = 46
      Width = 250
      Height = 21
      TabOrder = 1
    end
    object MemInfo: TEdit
      Left = 95
      Top = 72
      Width = 250
      Height = 21
      TabOrder = 2
    end
    object UID: TEdit
      Left = 94
      Top = 99
      Width = 250
      Height = 21
      TabOrder = 3
    end
    object IsActive: TCheckBox
      Left = 430
      Top = 94
      Width = 97
      Height = 17
      Alignment = taLeftJustify
      Caption = 'Active'
      TabOrder = 4
    end
    object WorkedTimes: TEdit
      Left = 430
      Top = 21
      Width = 140
      Height = 21
      TabOrder = 5
    end
    object ExpiredTime: TEdit
      Left = 429
      Top = 50
      Width = 140
      Height = 21
      TabOrder = 6
    end
    object Comment: TEdit
      Left = 94
      Top = 126
      Width = 475
      Height = 21
      TabOrder = 7
    end
  end
  object StatusBar1: TStatusBar
    Left = 0
    Top = 248
    Width = 590
    Height = 19
    Panels = <>
  end
  object ModOpenDlg: TOpenDialog
    DefaultExt = '*.gkm'
    Filter = 
      #1052#1086#1076#1091#1083#1080' '#1103#1076#1088#1072' "'#1062#1077#1085#1088'"|*.gkm|'#1044#1080#1085#1072#1084#1080#1095#1077#1089#1082#1080#1077' '#1073#1080#1073#1083#1080#1086#1090#1077#1082#1080'|*.dll|'#1055#1088#1086#1075#1088#1072#1084#1084#1099 +
      '|*.exe'
    Options = [ofHideReadOnly, ofNoChangeDir, ofPathMustExist, ofFileMustExist, ofEnableSizing, ofDontAddToRecent]
    Left = 168
  end
  object LicenseOpenDlg: TOpenDialog
    DefaultExt = '*.lic'
    Filter = #1060#1072#1081#1083#1099' '#1083#1080#1094#1077#1085#1079#1080#1081'|*.lic'
    Options = [ofHideReadOnly, ofNoChangeDir, ofPathMustExist, ofFileMustExist, ofEnableSizing, ofDontAddToRecent]
    Left = 216
  end
end
