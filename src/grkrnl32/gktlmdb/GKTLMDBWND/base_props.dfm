object BaseProps: TBaseProps
  Left = 0
  Top = 0
  Width = 443
  Height = 293
  Align = alClient
  TabOrder = 0
  DesignSize = (
    443
    293)
  object Label3: TLabel
    Left = 5
    Top = 10
    Width = 31
    Height = 13
    Caption = #1053#1086#1084#1077#1088
  end
  object Label4: TLabel
    Left = 99
    Top = 13
    Width = 19
    Height = 13
    Caption = #1048#1084#1103
  end
  object Label1: TLabel
    Left = 5
    Top = 41
    Width = 112
    Height = 13
    Caption = #1050#1086#1083#1080#1095#1077#1089#1090#1074#1086' '#1086#1073#1098#1077#1082#1090#1086#1074
  end
  object Label2: TLabel
    Left = 187
    Top = 43
    Width = 75
    Height = 13
    Caption = #1053#1086#1084#1077#1088' '#1087#1077#1088#1074#1086#1075#1086
  end
  object Number: TEdit
    Left = 45
    Top = 9
    Width = 50
    Height = 21
    TabOrder = 0
    Text = '0'
    OnChange = NumberChange
  end
  object Name: TEdit
    Left = 124
    Top = 9
    Width = 301
    Height = 21
    Anchors = [akLeft, akTop, akRight]
    TabOrder = 1
    OnChange = NumberChange
  end
  object Edit1: TEdit
    Left = 124
    Top = 40
    Width = 38
    Height = 21
    TabOrder = 2
    Text = '32'
    OnChange = NumberChange
  end
  object ObjCount: TUpDown
    Left = 162
    Top = 40
    Width = 15
    Height = 21
    Associate = Edit1
    Min = 1
    Max = 255
    Position = 32
    TabOrder = 3
  end
  object ObjFirstNumber: TEdit
    Left = 272
    Top = 40
    Width = 40
    Height = 21
    TabOrder = 4
    Text = '0'
    OnChange = ObjFirstNumberChange
  end
end
