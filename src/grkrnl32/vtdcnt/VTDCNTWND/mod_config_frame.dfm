inherited VtdModConfig: TVtdModConfig
  Width = 443
  Height = 293
  Align = alClient
  Visible = False
  ExplicitWidth = 443
  ExplicitHeight = 293
  object Label1: TLabel
    Left = 8
    Top = 16
    Width = 71
    Height = 13
    Caption = #1053#1086#1084#1077#1088' '#1084#1086#1076#1077#1084#1072
  end
  object Label2: TLabel
    Left = 8
    Top = 69
    Width = 36
    Height = 13
    Caption = #1048#1084#1103' '#1055#1059
  end
  object Label3: TLabel
    Left = 8
    Top = 101
    Width = 75
    Height = 13
    Caption = #1040#1076#1088#1077#1089' '#1089#1077#1088#1074#1077#1088#1072
  end
  object Label4: TLabel
    Left = 8
    Top = 130
    Width = 69
    Height = 13
    Caption = #1055#1086#1088#1090' '#1089#1077#1088#1074#1077#1088#1072
  end
  object Label5: TLabel
    Left = 8
    Top = 42
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
    Width = 17
    Height = 20
    Associate = Edit1
    Min = 1
    Max = 255
    Position = 1
    TabOrder = 1
  end
  object PuName: TEdit
    Left = 106
    Top = 66
    Width = 285
    Height = 20
    BevelInner = bvNone
    BevelKind = bkFlat
    BorderStyle = bsNone
    TabOrder = 2
    OnChange = PuNameChange
  end
  object ServerAddr: TEdit
    Left = 106
    Top = 95
    Width = 285
    Height = 20
    BevelInner = bvNone
    BevelKind = bkFlat
    BorderStyle = bsNone
    TabOrder = 3
    OnChange = ServerAddrChange
  end
  object ServerPort: TEdit
    Left = 106
    Top = 127
    Width = 103
    Height = 20
    BevelInner = bvNone
    BevelKind = bkFlat
    BorderStyle = bsNone
    TabOrder = 4
    OnChange = ServerPortChange
  end
  object Edit2: TEdit
    Left = 106
    Top = 39
    Width = 55
    Height = 20
    BevelInner = bvNone
    BevelKind = bkFlat
    BorderStyle = bsNone
    TabOrder = 5
    Text = '1'
    OnChange = Edit2Change
  end
  object PuNumber: TUpDown
    Left = 161
    Top = 39
    Width = 16
    Height = 20
    Associate = Edit2
    Min = 1
    Max = 255
    Position = 1
    TabOrder = 6
  end
end
