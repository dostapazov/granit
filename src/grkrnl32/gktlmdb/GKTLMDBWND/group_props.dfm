object BaseProps: TBaseProps
  Left = 0
  Top = 0
  Width = 435
  Height = 254
  TabOrder = 0
  object Label3: TLabel
    Left = 8
    Top = 8
    Width = 34
    Height = 13
    Caption = #1053#1086#1084#1077#1088
  end
  object Label4: TLabel
    Left = 105
    Top = 11
    Width = 22
    Height = 13
    Caption = #1048#1084#1103
  end
  object GroupBox1: TGroupBox
    Left = 0
    Top = 34
    Width = 605
    Height = 43
    Caption = #1055#1072#1088#1072#1084#1077#1090#1088#1099
    TabOrder = 0
    object Label1: TLabel
      Left = 8
      Top = 16
      Width = 110
      Height = 13
      Caption = #1050#1086#1083#1080#1095#1077#1089#1090#1074#1086' '#1086#1073#1098#1077#1082#1090#1086#1074
    end
    object Label2: TLabel
      Left = 192
      Top = 16
      Width = 78
      Height = 13
      Caption = #1053#1086#1084#1077#1088' '#1087#1077#1088#1074#1086#1075#1086
    end
    object ObjectCount: TEdit
      Left = 125
      Top = 13
      Width = 38
      Height = 21
      TabOrder = 0
      Text = '32'
    end
    object UpDown1: TUpDown
      Left = 163
      Top = 11
      Width = 15
      Height = 23
      Associate = ObjectCount
      Min = 0
      Max = 254
      Position = 32
      TabOrder = 1
      Wrap = False
    end
    object ObjFirstNumber: TEdit
      Left = 275
      Top = 13
      Width = 46
      Height = 21
      TabOrder = 2
      Text = '0'
    end
  end
  object Number: TEdit
    Left = 48
    Top = 8
    Width = 50
    Height = 21
    TabOrder = 1
    Text = '0'
  end
  object Name: TEdit
    Left = 136
    Top = 8
    Width = 297
    Height = 21
    TabOrder = 2
  end
end
