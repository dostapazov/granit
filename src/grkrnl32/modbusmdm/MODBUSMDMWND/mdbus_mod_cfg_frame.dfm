inherited ModbusModConfig: TModbusModConfig
  Width = 406
  Height = 81
  ExplicitWidth = 406
  ExplicitHeight = 81
  object Label1: TLabel
    Left = 8
    Top = 16
    Width = 71
    Height = 13
    Caption = #1053#1086#1084#1077#1088' '#1084#1086#1076#1077#1084#1072
  end
  object Label2: TLabel
    Left = 7
    Top = 41
    Width = 36
    Height = 13
    Caption = #1048#1084#1103' '#1055#1059
  end
  object Label5: TLabel
    Left = 192
    Top = 18
    Width = 48
    Height = 13
    Caption = #1053#1086#1084#1077#1088' '#1055#1059
  end
  object Edit1: TEdit
    Left = 106
    Top = 13
    Width = 55
    Height = 20
    BevelInner = bvNone
    BevelKind = bkFlat
    BorderStyle = bsNone
    TabOrder = 0
    Text = '1'
    OnChange = Edit1Change
  end
  object ModemNumber: TUpDown
    Left = 161
    Top = 13
    Width = 15
    Height = 20
    Associate = Edit1
    Min = 1
    Max = 255
    Position = 1
    TabOrder = 1
  end
  object PuName: TEdit
    Left = 106
    Top = 38
    Width = 285
    Height = 20
    BevelInner = bvNone
    BevelKind = bkFlat
    BorderStyle = bsNone
    TabOrder = 2
    OnChange = PuNameChange
  end
  object Edit2: TEdit
    Left = 290
    Top = 15
    Width = 55
    Height = 20
    BevelInner = bvNone
    BevelKind = bkFlat
    BorderStyle = bsNone
    TabOrder = 3
    Text = '1'
    OnChange = Edit2Change
  end
  object PuNumber: TUpDown
    Left = 345
    Top = 15
    Width = 15
    Height = 20
    Associate = Edit2
    Min = 1
    Max = 255
    Position = 1
    TabOrder = 4
  end
end
