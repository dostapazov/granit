inherited IEC60870LineParam: TIEC60870LineParam
  Width = 449
  Height = 232
  Visible = False
  ExplicitWidth = 449
  ExplicitHeight = 232
  object Label2: TLabel
    Left = 10
    Top = 11
    Width = 48
    Height = 13
    Caption = #1053#1086#1084#1077#1088' '#1050#1055
  end
  object Label3: TLabel
    Left = 8
    Top = 41
    Width = 36
    Height = 13
    Caption = #1048#1084#1103' '#1050#1055
  end
  object Label1: TLabel
    Left = 10
    Top = 68
    Width = 31
    Height = 13
    Caption = #1040#1076#1088#1077#1089
  end
  object Label4: TLabel
    Left = 314
    Top = 68
    Width = 25
    Height = 13
    Caption = #1055#1086#1088#1090
  end
  object Label5: TLabel
    Left = 208
    Top = 95
    Width = 155
    Height = 13
    Caption = #1056#1072#1079#1084#1077#1088' '#1072#1076#1088#1077#1089#1072' '#1086#1073#1098#1077#1082#1090#1072' ('#1073#1072#1081#1090')'
  end
  object Label6: TLabel
    Left = 10
    Top = 95
    Width = 127
    Height = 13
    Caption = #1047#1072#1076#1077#1088#1078#1082#1072' '#1087#1077#1088#1077#1076#1072#1095#1080' (ms)'
  end
  object Label7: TLabel
    Left = 11
    Top = 165
    Width = 125
    Height = 13
    Caption = #1055#1088#1080#1077#1084' '#1076#1086' '#1082#1074#1080#1090#1072#1085#1094#1080#1080' (W)'
  end
  object Label8: TLabel
    Left = 208
    Top = 168
    Width = 140
    Height = 13
    Caption = #1055#1077#1088#1077#1076#1072#1095#1072' '#1076#1086' '#1082#1074#1080#1090#1072#1085#1094#1080#1080' (K)'
  end
  object Label9: TLabel
    Left = 11
    Top = 127
    Width = 105
    Height = 13
    Caption = #1058#1072#1081#1084#1072#1091#1090' '#1087#1088#1080#1077#1084#1072' (ms)'
  end
  object Label10: TLabel
    Left = 122
    Top = 11
    Width = 91
    Height = 13
    Caption = #1072#1076#1088#1077#1089' '#1091#1089#1090#1088#1086#1081#1089#1090#1074#1072
  end
  object Label11: TLabel
    Left = 287
    Top = 11
    Width = 68
    Height = 13
    Caption = #1054#1073#1097#1080#1081' '#1072#1076#1088#1077#1089
  end
  object CpNumber: TEdit
    Tag = 1
    Left = 68
    Top = 8
    Width = 50
    Height = 21
    BevelInner = bvNone
    BevelKind = bkFlat
    BorderStyle = bsNone
    TabOrder = 0
    OnChange = CpNumberChange
  end
  object cp_name: TEdit
    Tag = 2
    Left = 68
    Top = 38
    Width = 350
    Height = 21
    BevelInner = bvNone
    BevelKind = bkFlat
    BorderStyle = bsNone
    TabOrder = 3
    OnChange = CpNumberChange
  end
  object host_addr: TEdit
    Tag = 4
    Left = 68
    Top = 65
    Width = 225
    Height = 21
    BevelInner = bvNone
    BevelKind = bkFlat
    BorderStyle = bsNone
    TabOrder = 4
    OnChange = CpNumberChange
  end
  object host_port: TEdit
    Tag = 8
    Left = 345
    Top = 65
    Width = 73
    Height = 21
    BevelInner = bvNone
    BevelKind = bkFlat
    BorderStyle = bsNone
    TabOrder = 5
    OnChange = CpNumberChange
  end
  object obj_addr_size: TComboBox
    Tag = 1
    Left = 384
    Top = 93
    Width = 43
    Height = 21
    BevelKind = bkFlat
    Style = csDropDownList
    ItemIndex = 2
    TabOrder = 10
    Text = '3'
    OnChange = CpNumberChange
    Items.Strings = (
      '1'
      '2'
      '3')
  end
  object transmit_delay: TEdit
    Tag = 1
    Left = 144
    Top = 92
    Width = 44
    Height = 21
    BevelInner = bvNone
    BevelKind = bkFlat
    BorderStyle = bsNone
    TabOrder = 6
    Text = '100'
    OnChange = CpNumberChange
  end
  object W_count: TEdit
    Tag = 1
    Left = 144
    Top = 163
    Width = 44
    Height = 21
    BevelInner = bvNone
    BevelKind = bkFlat
    BorderStyle = bsNone
    TabOrder = 8
    Text = '30'
    OnChange = CpNumberChange
  end
  object K_count: TEdit
    Tag = 1
    Left = 384
    Top = 162
    Width = 43
    Height = 21
    BevelInner = bvNone
    BevelKind = bkFlat
    BorderStyle = bsNone
    TabOrder = 9
    Text = '10'
    OnChange = CpNumberChange
  end
  object rx_timeout: TEdit
    Tag = 1
    Left = 144
    Top = 124
    Width = 44
    Height = 21
    BevelInner = bvNone
    BevelKind = bkFlat
    BorderStyle = bsNone
    TabOrder = 7
    Text = '100'
    OnChange = CpNumberChange
  end
  object dev_addr: TEdit
    Tag = 1
    Left = 220
    Top = 8
    Width = 50
    Height = 21
    BevelInner = bvNone
    BevelKind = bkFlat
    BorderStyle = bsNone
    TabOrder = 1
    OnChange = CpNumberChange
  end
  object common_addr: TEdit
    Tag = 1
    Left = 368
    Top = 8
    Width = 50
    Height = 21
    BevelInner = bvNone
    BevelKind = bkFlat
    BorderStyle = bsNone
    TabOrder = 2
    OnChange = CpNumberChange
  end
end
