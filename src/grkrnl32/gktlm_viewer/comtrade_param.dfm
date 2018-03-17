object ComtradeParam: TComtradeParam
  Left = 0
  Top = 0
  Caption = #1069#1082#1089#1087#1086#1088#1090' '#1074' COMTRADE'
  ClientHeight = 220
  ClientWidth = 455
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poOwnerFormCenter
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 216
    Top = 8
    Width = 68
    Height = 13
    Caption = #1063#1072#1089#1090#1086#1090#1072' '#1089#1077#1090#1080
  end
  object Label2: TLabel
    Left = 297
    Top = 8
    Width = 122
    Height = 13
    Caption = #1063#1072#1089#1090#1086#1090#1072' '#1076#1080#1089#1082#1088#1077#1090#1080#1079#1072#1094#1080#1080
  end
  object bSelProgram: TSpeedButton
    Left = 421
    Top = 120
    Width = 23
    Height = 22
    Caption = '...'
    Flat = True
    OnClick = bSelProgramClick
  end
  object Label3: TLabel
    Left = 216
    Top = 150
    Width = 105
    Height = 13
    Caption = #1055#1077#1088#1077#1076#1080#1089#1082#1088#1077#1090#1080#1079#1072#1094#1080#1103' '
  end
  object ctName: TLabeledEdit
    Left = 16
    Top = 24
    Width = 185
    Height = 21
    BevelInner = bvLowered
    BevelKind = bkFlat
    BevelOuter = bvRaised
    BorderStyle = bsNone
    EditLabel.Width = 19
    EditLabel.Height = 13
    EditLabel.Caption = #1048#1084#1103
    TabOrder = 0
    OnChange = ctNameChange
  end
  object ctFreq: TComboBox
    Left = 216
    Top = 24
    Width = 75
    Height = 21
    Style = csDropDownList
    ItemIndex = 0
    TabOrder = 1
    Text = '50'
    OnChange = ctNameChange
    Items.Strings = (
      '50'
      '60')
  end
  object BitBtn1: TBitBtn
    Left = 263
    Top = 187
    Width = 75
    Height = 25
    Kind = bkCancel
    NumGlyphs = 2
    TabOrder = 3
  end
  object bOk: TBitBtn
    Left = 344
    Top = 187
    Width = 75
    Height = 25
    Caption = 'OK'
    Default = True
    Glyph.Data = {
      DE010000424DDE01000000000000760000002800000024000000120000000100
      0400000000006801000000000000000000001000000000000000000000000000
      80000080000000808000800000008000800080800000C0C0C000808080000000
      FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
      3333333333333333333333330000333333333333333333333333F33333333333
      00003333344333333333333333388F3333333333000033334224333333333333
      338338F3333333330000333422224333333333333833338F3333333300003342
      222224333333333383333338F3333333000034222A22224333333338F338F333
      8F33333300003222A3A2224333333338F3838F338F33333300003A2A333A2224
      33333338F83338F338F33333000033A33333A222433333338333338F338F3333
      0000333333333A222433333333333338F338F33300003333333333A222433333
      333333338F338F33000033333333333A222433333333333338F338F300003333
      33333333A222433333333333338F338F00003333333333333A22433333333333
      3338F38F000033333333333333A223333333333333338F830000333333333333
      333A333333333333333338330000333333333333333333333333333333333333
      0000}
    ModalResult = 1
    NumGlyphs = 2
    TabOrder = 4
  end
  object ctFile: TLabeledEdit
    Left = 16
    Top = 69
    Width = 403
    Height = 21
    BevelKind = bkFlat
    BorderStyle = bsNone
    EditLabel.Width = 54
    EditLabel.Height = 13
    EditLabel.Caption = #1048#1084#1103' '#1092#1072#1081#1083#1072
    TabOrder = 2
    OnChange = ctNameChange
  end
  object CheckBox1: TCheckBox
    Left = 16
    Top = 96
    Width = 137
    Height = 17
    Caption = #1054#1090#1082#1088#1099#1090#1100' '#1074' '#1087#1088#1086#1075#1088#1072#1084#1084#1077
    Checked = True
    State = cbChecked
    TabOrder = 5
  end
  object ProgramName: TEdit
    Left = 16
    Top = 120
    Width = 403
    Height = 21
    BevelKind = bkFlat
    BorderStyle = bsNone
    TabOrder = 6
    Text = 'FastView.exe'
  end
  object ctFreqSample: TComboBox
    Left = 299
    Top = 24
    Width = 121
    Height = 21
    ItemIndex = 1
    TabOrder = 7
    Text = '333.333'
    Items.Strings = (
      '250'
      '333.333'
      '500'
      '1000')
  end
  object cbAdjustSampleTime: TCheckBox
    Left = 16
    Top = 147
    Width = 177
    Height = 17
    Caption = #1042#1099#1088#1086#1074#1085#1103#1090#1100' '#1074#1088#1077#1084#1077#1085#1072' '#1089#1088#1077#1079#1086#1074
    TabOrder = 8
  end
  object Edit1: TEdit
    Left = 327
    Top = 147
    Width = 49
    Height = 21
    TabOrder = 9
    Text = '1'
  end
  object ResampleValue: TUpDown
    Left = 376
    Top = 147
    Width = 16
    Height = 21
    Associate = Edit1
    Min = 1
    Max = 10
    Position = 1
    TabOrder = 10
  end
  object OpenDialog1: TOpenDialog
    DefaultExt = '.exe'
    Filter = #1055#1088#1086#1075#1088#1072#1084#1084#1099'|*.exe'
    Options = [ofHideReadOnly, ofNoChangeDir, ofPathMustExist, ofFileMustExist, ofEnableSizing, ofDontAddToRecent]
    Left = 216
    Top = 184
  end
end
