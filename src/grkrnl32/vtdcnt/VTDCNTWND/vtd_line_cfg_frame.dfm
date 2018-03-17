inherited VtdLineConfig: TVtdLineConfig
  Width = 443
  Height = 293
  Align = alClient
  Visible = False
  ExplicitWidth = 443
  ExplicitHeight = 277
  object Label1: TLabel
    Left = 12
    Top = 3
    Width = 81
    Height = 26
    Caption = #1053#1086#1084#1077#1088' '#1050#1055'('#1059#1089#1090#1088#1086#1081#1089#1090#1074#1072')'
    WordWrap = True
  end
  object Label2: TLabel
    Left = 17
    Top = 37
    Width = 36
    Height = 13
    Caption = #1048#1084#1103' '#1050#1055
  end
  object Label15: TLabel
    Left = 192
    Top = 8
    Width = 79
    Height = 13
    Caption = #1058#1080#1087' '#1091#1089#1090#1088#1086#1081#1089#1090#1074#1072
  end
  object Edit1: TEdit
    Left = 105
    Top = 5
    Width = 54
    Height = 21
    BevelInner = bvNone
    BevelKind = bkFlat
    BorderStyle = bsNone
    TabOrder = 0
    Text = '1'
    OnChange = Edit1Change
  end
  object DevName: TEdit
    Left = 104
    Top = 32
    Width = 294
    Height = 21
    BevelInner = bvNone
    BevelKind = bkFlat
    BorderStyle = bsNone
    TabOrder = 1
    OnChange = DevNameChange
  end
  object DevType: TComboBox
    Left = 280
    Top = 4
    Width = 117
    Height = 21
    BevelKind = bkFlat
    Style = csDropDownList
    DropDownCount = 3
    ItemIndex = 0
    TabOrder = 2
    Text = #1042#1058#1044
    OnChange = DevTypeChange
    Items.Strings = (
      #1042#1058#1044
      #1042#1058#1044'-'#1042'.'#1042#1058#1044'-'#1043
      #1042#1058#1044'-'#1059)
  end
  object DeviceNumber: TUpDown
    Left = 159
    Top = 5
    Width = 15
    Height = 21
    Associate = Edit1
    Min = 1
    Max = 255
    Position = 1
    TabOrder = 3
  end
end
