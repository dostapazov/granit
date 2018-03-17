inherited ModbusGroupFrm: TModbusGroupFrm
  Width = 486
  Height = 312
  ExplicitWidth = 486
  ExplicitHeight = 312
  object Label1: TLabel
    Left = 9
    Top = 24
    Width = 71
    Height = 13
    Caption = #1053#1086#1084#1077#1088' '#1075#1088#1091#1087#1087#1099
    FocusControl = GroupNum
  end
  object Label2: TLabel
    Left = 8
    Top = 53
    Width = 108
    Height = 13
    Caption = #1053#1072#1095#1072#1083#1100#1085#1099#1081' '#1087#1072#1088#1072#1084#1077#1090#1088
    FocusControl = FirstParam
  end
  object Label3: TLabel
    Left = 8
    Top = 79
    Width = 98
    Height = 13
    Caption = #1050#1086#1083'-'#1074#1086' '#1087#1072#1088#1072#1084#1077#1090#1088#1086#1074
    FocusControl = CountParam
  end
  object Label4: TLabel
    Left = 8
    Top = 106
    Width = 97
    Height = 13
    Caption = #1055#1077#1088#1080#1086#1076' '#1086#1087#1088#1086#1089#1072'(ms)'
    FocusControl = RefreshTime
  end
  object Label5: TLabel
    Left = 259
    Top = 24
    Width = 92
    Height = 13
    Caption = #1053#1086#1084#1077#1088' '#1091#1089#1090#1088#1086#1081#1089#1090#1074#1072
    FocusControl = DevNo
  end
  object Label6: TLabel
    Left = 9
    Top = 4
    Width = 56
    Height = 13
    Caption = #1040#1076#1088#1077#1089' '#1054#1058#1044
  end
  object Label13: TLabel
    Left = 8
    Top = 131
    Width = 144
    Height = 13
    Caption = #1042#1088#1077#1084#1103' '#1086#1078#1080#1076#1072#1085#1080#1103' '#1086#1090#1074#1077#1090#1072'(ms)'
    FocusControl = RespondTime
  end
  object Label14: TLabel
    Left = 8
    Top = 154
    Width = 96
    Height = 13
    Caption = #1042#1088#1077#1084#1103' '#1086#1090#1082#1083#1080#1082#1072'(ms)'
  end
  object Label15: TLabel
    Left = 247
    Top = 154
    Width = 16
    Height = 13
    Caption = 'min'
  end
  object Label16: TLabel
    Left = 364
    Top = 154
    Width = 20
    Height = 13
    Caption = 'max'
  end
  object Label9: TLabel
    Left = 9
    Top = 178
    Width = 96
    Height = 13
    Caption = #1055#1086#1089#1083#1077#1076#1085#1103#1103' '#1086#1096#1080#1073#1082#1072
    FocusControl = LastError
  end
  object Label10: TLabel
    Left = 353
    Top = 178
    Width = 34
    Height = 13
    AutoSize = False
    Caption = #1082#1086#1083'-'#1074#1086
    FocusControl = ErrCount
  end
  object GroupNum: TEdit
    Tag = 1
    Left = 156
    Top = 21
    Width = 85
    Height = 21
    BevelKind = bkFlat
    BorderStyle = bsNone
    TabOrder = 0
    OnChange = GroupNumChange
  end
  object FirstParam: TEdit
    Tag = 2
    Left = 155
    Top = 48
    Width = 85
    Height = 21
    BevelKind = bkFlat
    BorderStyle = bsNone
    TabOrder = 1
    OnChange = GroupNumChange
  end
  object CountParam: TEdit
    Tag = 4
    Left = 155
    Top = 74
    Width = 85
    Height = 21
    BevelKind = bkFlat
    BorderStyle = bsNone
    TabOrder = 2
    OnChange = GroupNumChange
  end
  object RefreshTime: TEdit
    Tag = 8
    Left = 155
    Top = 101
    Width = 85
    Height = 21
    BevelKind = bkFlat
    BorderStyle = bsNone
    TabOrder = 3
    OnChange = GroupNumChange
  end
  object rg1: TRadioGroup
    Left = 252
    Top = 48
    Width = 203
    Height = 101
    Caption = #1058#1080#1087' '#1075#1088#1091#1087#1087#1099
    Items.Strings = (
      #1044#1080#1089#1082#1088#1077#1090#1085#1099#1081' '#1074#1099#1093#1086#1076
      #1044#1080#1089#1082#1088#1077#1090#1085#1099#1081' '#1074#1093#1086#1076
      'Holding register'
      #1040#1085#1072#1083#1086#1075#1086#1074#1099#1081' '#1074#1093#1086#1076
      #1055#1086#1083#1100#1079#1086#1074#1072#1090#1077#1083#1100#1089#1082#1080#1081)
    TabOrder = 5
    OnClick = rg1Click
  end
  object DevNo: TEdit
    Tag = 32
    Left = 357
    Top = 21
    Width = 71
    Height = 21
    BevelKind = bkFlat
    BorderStyle = bsNone
    TabOrder = 4
    OnChange = GroupNumChange
  end
  object otd_addr_text: TStaticText
    Left = 155
    Top = 3
    Width = 85
    Height = 17
    Alignment = taCenter
    AutoSize = False
    BorderStyle = sbsSunken
    Caption = 'otd_addr_text'
    TabOrder = 6
  end
  object GroupBox1: TGroupBox
    Left = 9
    Top = 201
    Width = 393
    Height = 47
    Caption = 'Comm Event Ctl'
    TabOrder = 7
    object Label7: TLabel
      Left = 81
      Top = 20
      Width = 54
      Height = 13
      Caption = #1057#1086#1089#1090#1086#1103#1085#1080#1077
    end
    object Label8: TLabel
      Left = 216
      Top = 20
      Width = 89
      Height = 13
      Caption = #1057#1095#1105#1090#1095#1080#1082' '#1089#1086#1073#1099#1090#1080#1081
    end
    object bGetCommEvent: TButton
      Left = 3
      Top = 16
      Width = 71
      Height = 25
      Caption = #1055#1086#1083#1091#1095#1080#1090#1100
      TabOrder = 0
      OnClick = bGetCommEventClick
    end
    object CommState: TStaticText
      Left = 142
      Top = 19
      Width = 67
      Height = 17
      Alignment = taCenter
      AutoSize = False
      BorderStyle = sbsSunken
      TabOrder = 1
    end
    object CommEvent: TStaticText
      Left = 312
      Top = 20
      Width = 67
      Height = 17
      Alignment = taCenter
      AutoSize = False
      BorderStyle = sbsSunken
      TabOrder = 2
    end
  end
  object RespondTime: TEdit
    Tag = 64
    Left = 156
    Top = 128
    Width = 85
    Height = 21
    BevelKind = bkFlat
    BorderStyle = bsNone
    TabOrder = 8
    OnChange = GroupNumChange
  end
  object RealRespondTime: TStaticText
    Left = 156
    Top = 154
    Width = 84
    Height = 17
    Alignment = taCenter
    AutoSize = False
    BorderStyle = sbsSunken
    Caption = '?'
    TabOrder = 9
  end
  object ModbusCommand: TEdit
    Tag = 16
    Left = 381
    Top = 123
    Width = 68
    Height = 21
    BevelKind = bkFlat
    BorderStyle = bsNone
    TabOrder = 10
    OnChange = ModbusCommandChange
  end
  object RealRespondTimeMin: TStaticText
    Left = 279
    Top = 154
    Width = 66
    Height = 17
    Alignment = taCenter
    AutoSize = False
    BorderStyle = sbsSunken
    Caption = '?'
    TabOrder = 11
  end
  object RealRespondTimeMax: TStaticText
    Left = 390
    Top = 154
    Width = 66
    Height = 17
    Alignment = taCenter
    AutoSize = False
    BorderStyle = sbsSunken
    Caption = '?'
    TabOrder = 12
  end
  object LastError: TStaticText
    Left = 156
    Top = 178
    Width = 195
    Height = 17
    Alignment = taCenter
    AutoSize = False
    BorderStyle = sbsSunken
    Caption = '-'
    TabOrder = 13
  end
  object ErrCount: TStaticText
    Left = 389
    Top = 177
    Width = 66
    Height = 17
    Alignment = taCenter
    AutoSize = False
    BorderStyle = sbsSunken
    Caption = '?'
    TabOrder = 14
  end
end
