inherited CPFrame: TCPFrame
  object GroupBox1: TGroupBox
    Left = 0
    Top = 0
    Width = 443
    Height = 277
    Align = alClient
    TabOrder = 0
    object Label1: TLabel
      Left = 9
      Top = 24
      Width = 14
      Height = 13
      Caption = #1051#1059
    end
    object Label2: TLabel
      Left = 6
      Top = 54
      Width = 36
      Height = 13
      Caption = #1048#1084#1103' '#1050#1055
    end
    object Label5: TLabel
      Left = 97
      Top = 24
      Width = 14
      Height = 13
      Caption = #1050#1055
    end
    object Label3: TLabel
      Left = 108
      Top = 110
      Width = 121
      Height = 35
      AutoSize = False
      Caption = #1056#1072#1079#1088#1077#1096#1072#1102#1090#1089#1103' '#1086#1087#1077#1088#1072#1094#1080#1080' '#1058#1059'/'#1058#1056' '#1076#1083#1103' '#1086#1090#1083#1072#1076#1082#1080
      WordWrap = True
    end
    object Label4: TLabel
      Left = 6
      Top = 84
      Width = 228
      Height = 13
      Caption = #1042#1088#1077#1084#1103' '#1086#1078#1080#1076#1072#1085#1080#1103' '#1075#1086#1090#1086#1074#1085#1086#1089#1090#1080' '#1085#1072' '#1087#1077#1088#1077#1076#1072#1095#1091' ms'
    end
    object Label6: TLabel
      Left = 316
      Top = 81
      Width = 121
      Height = 67
      AutoSize = False
      Caption = 
        #1054#1090#1088#1080#1094#1072#1090#1077#1083#1100#1085#1086#1077' '#1079#1085#1072#1095#1077#1085#1080#1077' '#1073#1091#1076#1077#1090' '#1103#1074#1083#1103#1090#1100#1089#1103' '#1084#1085#1086#1078#1080#1090#1077#1083#1077#1084' '#1074#1088#1077#1084#1077#1085#1080' '#1076#1086#1089#1090#1072#1074#1082 +
        #1080
      ShowAccelChar = False
      Layout = tlCenter
      WordWrap = True
    end
    object LuNum: TEdit
      Left = 49
      Top = 20
      Width = 41
      Height = 20
      BevelKind = bkFlat
      BorderStyle = bsNone
      TabOrder = 0
      Text = 'LuNum'
      OnChange = LuNumChange
    end
    object CpName: TEdit
      Left = 49
      Top = 50
      Width = 313
      Height = 20
      BevelKind = bkFlat
      BorderStyle = bsNone
      TabOrder = 1
      Text = 'CpNum'
      OnChange = CpNameChange
    end
    object CpNum: TStaticText
      Left = 125
      Top = 22
      Width = 56
      Height = 16
      Alignment = taCenter
      AutoSize = False
      BorderStyle = sbsSunken
      Caption = 'GrpCount'
      TabOrder = 2
    end
    object CpTestFlag: TCheckBox
      Left = 12
      Top = 116
      Width = 97
      Height = 17
      Caption = #1058#1077#1089#1090#1086#1074#1086#1077' '#1050#1055
      TabOrder = 4
      OnClick = CpTestFlagClick
    end
    object CpLogErrors: TCheckBox
      Left = 12
      Top = 170
      Width = 253
      Height = 17
      Caption = #1047#1072#1087#1080#1089#1099#1074#1072#1090#1100' '#1074' '#1083#1086#1075' '#1086#1096#1080#1073#1082#1080' '#1082#1086#1085#1090#1088#1086#1083#1083#1077#1088#1072' '#1050#1055
      TabOrder = 6
      OnClick = CpTestFlagClick
    end
    object CpNotReceipt: TCheckBox
      Left = 12
      Top = 144
      Width = 205
      Height = 17
      Caption = #1047#1072#1087#1088#1077#1090' '#1082#1074#1080#1090#1080#1088#1086#1074#1072#1085#1080#1103
      TabOrder = 5
      OnClick = CpTestFlagClick
    end
    object swt: TEdit
      Left = 246
      Top = 82
      Width = 56
      Height = 20
      BevelKind = bkFlat
      BorderStyle = bsNone
      TabOrder = 3
      Text = '1000'
      OnChange = swtChange
    end
    object NumberedTS: TCheckBox
      Left = 13
      Top = 192
      Width = 404
      Height = 17
      Caption = 
        #1053#1091#1084#1077#1088#1086#1074#1072#1085#1085#1099#1077' '#1082#1072#1076#1088#1099'  '#1075#1088#1091#1087#1087' '#1058#1057' ( '#1085#1086#1084#1077#1088#1072' 0-3 ) '#1055#1077#1088#1077#1076#1077#1083#1082#1072' '#1057#1044#1058#1059' '#1053#1086#1088#1080#1083 +
        #1100#1089#1082
      TabOrder = 7
    end
  end
end
