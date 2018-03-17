inherited SCSettingsFrm: TSCSettingsFrm
  object SettingsBox: TGroupBox
    Left = 0
    Top = 0
    Width = 443
    Height = 293
    Align = alClient
    TabOrder = 0
    ExplicitHeight = 277
    object Label1: TLabel
      Left = 153
      Top = 16
      Width = 48
      Height = 13
      Caption = #1053#1086#1084#1077#1088' '#1055#1059
    end
    object Label2: TLabel
      Left = 8
      Top = 16
      Width = 71
      Height = 13
      Caption = #1053#1086#1084#1077#1088' '#1084#1086#1076#1077#1084#1072
    end
    object Label3: TLabel
      Left = 8
      Top = 32
      Width = 36
      Height = 13
      Caption = #1048#1084#1103' '#1055#1059
    end
    object Label4: TLabel
      Left = 8
      Top = 97
      Width = 116
      Height = 13
      Caption = #1052#1086#1076#1091#1083#1100' '#1074#1074#1086#1076#1072'/'#1074#1099#1074#1086#1076#1072
    end
    object Label5: TLabel
      Left = 10
      Top = 125
      Width = 83
      Height = 13
      Caption = #1048#1084#1103' '#1091#1089#1090#1088#1086#1081#1089#1090#1074#1072' '
    end
    object Label6: TLabel
      Left = 8
      Top = 194
      Width = 159
      Height = 13
      Caption = #1060#1072#1081#1083' '#1076#1083#1103' '#1093#1088#1072#1085#1077#1085#1080#1103' '#1089#1087#1080#1089#1082#1086#1074' '#1050#1055
    end
    object Label7: TLabel
      Left = 11
      Top = 148
      Width = 87
      Height = 13
      Caption = #1057#1082#1086#1088#1086#1089#1090#1100' '#1086#1073#1084#1077#1085#1072
    end
    object PuNum: TEdit
      Left = 212
      Top = 13
      Width = 84
      Height = 21
      BevelKind = bkFlat
      BorderStyle = bsNone
      TabOrder = 1
      Text = 'PuNum'
      OnChange = PuNumChange
    end
    object ModemNum: TEdit
      Left = 96
      Top = 13
      Width = 41
      Height = 21
      BevelKind = bkFlat
      BorderStyle = bsNone
      TabOrder = 0
      Text = 'PUNumber'
      OnChange = ModemNumChange
    end
    object PuName: TEdit
      Left = 8
      Top = 49
      Width = 424
      Height = 21
      BevelKind = bkFlat
      BorderStyle = bsNone
      TabOrder = 2
      Text = 'PUNumber'
      OnChange = PuNameChange
    end
    object IOModule: TEdit
      Left = 128
      Top = 95
      Width = 272
      Height = 21
      BevelKind = bkFlat
      BorderStyle = bsNone
      TabOrder = 3
      Text = 'PUNumber'
      OnChange = IOModuleChange
    end
    object Button1: TButton
      Left = 407
      Top = 96
      Width = 23
      Height = 19
      Caption = '...'
      TabOrder = 4
      OnClick = Button1Click
    end
    object IODevice: TComboBox
      Left = 128
      Top = 120
      Width = 161
      Height = 21
      BevelKind = bkFlat
      TabOrder = 5
      Text = 'IODevice'
      OnChange = IODeviceChange
      OnDropDown = IODeviceDropDown
    end
    object SaveFileName: TEdit
      Left = 8
      Top = 213
      Width = 389
      Height = 21
      BevelKind = bkFlat
      BorderStyle = bsNone
      TabOrder = 7
      OnChange = SaveFileNameChange
    end
    object SelFolderBtn: TButton
      Left = 406
      Top = 214
      Width = 23
      Height = 19
      Caption = '...'
      TabOrder = 8
      OnClick = SelFolderBtnClick
    end
    object Button2: TButton
      Left = 8
      Top = 243
      Width = 177
      Height = 25
      Caption = #1048#1084#1087#1086#1088#1090' '#1076#1072#1085#1085#1099#1093' '#1080#1079' '#1092#1072#1081#1083#1072
      TabOrder = 9
      OnClick = Button2Click
    end
    object EnableWatchdog: TCheckBox
      Left = 8
      Top = 174
      Width = 257
      Height = 17
      Caption = #1055#1077#1088#1077#1076#1072#1095#1072' '#1087#1091#1089#1090#1099#1093' '#1082#1072#1076#1088#1086#1074' '#1074#1086' '#1074#1088#1077#1084#1103' '#1087#1088#1086#1089#1090#1086#1103
      TabOrder = 6
      OnClick = EnableWatchdogClick
    end
    object IOSpeed: TEdit
      Left = 128
      Top = 145
      Width = 158
      Height = 21
      BevelKind = bkFlat
      BorderStyle = bsNone
      TabOrder = 10
      Text = 'PuNum'
      OnChange = IOSpeedChange
    end
  end
  object OpenDialog1: TOpenDialog
    Filter = 
      #1052#1086#1076#1091#1083#1080' '#1042#1086#1076#1072'/'#1074#1099#1074#1086#1076#1072'|*.mio|'#1044#1080#1085#1072#1084#1080#1095#1077#1089#1082#1080#1077' '#1073#1080#1073#1083#1080#1086#1090#1077#1082#1080'|*.dll|'#1042#1089#1077' '#1092#1072#1081#1083#1099 +
      '|*.*'
    Options = [ofHideReadOnly, ofNoChangeDir, ofPathMustExist, ofFileMustExist, ofNoNetworkButton, ofEnableSizing, ofDontAddToRecent]
    OptionsEx = [ofExNoPlacesBar]
    Left = 376
    Top = 16
  end
  object OpenDialog2: TOpenDialog
    DefaultExt = 'scd'
    Filter = #1060#1072#1081#1083#1099' '#1057#1062' '#1043#1088#1072#1085#1080#1090'|*.scd|'#1042#1089#1077' '#1092#1072#1081#1083#1099'|*.*'
    Options = [ofHideReadOnly, ofNoChangeDir, ofPathMustExist, ofNoTestFileCreate, ofNoNetworkButton, ofEnableSizing, ofDontAddToRecent]
    OptionsEx = [ofExNoPlacesBar]
    Left = 384
    Top = 240
  end
end
