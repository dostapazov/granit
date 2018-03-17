inherited FloorFrame: TFloorFrame
  object GroupBox1: TGroupBox
    Left = 0
    Top = 0
    Width = 443
    Height = 277
    Align = alClient
    TabOrder = 0
    object Label1: TLabel
      Left = 8
      Top = 16
      Width = 65
      Height = 13
      Caption = #1053#1086#1084#1077#1088' '#1101#1090#1072#1078#1072
    end
    object Label2: TLabel
      Left = 196
      Top = 15
      Width = 77
      Height = 13
      Caption = #1050#1086#1083#1080#1095#1077#1089#1090#1074#1086' '#1050#1055
    end
    object FloorNum: TEdit
      Left = 101
      Top = 13
      Width = 75
      Height = 21
      BevelKind = bkFlat
      BorderStyle = bsNone
      TabOrder = 0
      Text = 'FloorNum'
      OnChange = FloorNumChange
    end
    object CpCount: TStaticText
      Left = 286
      Top = 13
      Width = 72
      Height = 17
      AutoSize = False
      BorderStyle = sbsSunken
      Caption = 'CpCount'
      TabOrder = 1
    end
  end
end
