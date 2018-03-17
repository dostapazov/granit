inherited ModbusDevFrame: TModbusDevFrame
  Width = 398
  Height = 313
  ExplicitWidth = 398
  ExplicitHeight = 313
  object pnl0: TPanel
    Left = 10
    Top = 103
    Width = 359
    Height = 149
    TabOrder = 3
    Visible = False
    object Label3: TLabel
      Left = 8
      Top = 16
      Width = 64
      Height = 13
      Caption = #1053#1086#1084#1077#1088' '#1087#1086#1088#1090#1072
    end
    object Label7: TLabel
      Left = 188
      Top = 14
      Width = 51
      Height = 13
      Caption = #1047#1072#1076#1077#1088#1078#1082#1072
    end
    object ParityControl: TRadioGroup
      Left = 152
      Top = 97
      Width = 192
      Height = 33
      Caption = #1050#1086#1085#1090#1088#1086#1083#1100' '#1095#1105#1090#1085#1086#1089#1090#1080
      Columns = 3
      ItemIndex = 0
      Items.Strings = (
        #1053#1077#1090
        #1053#1077#1095#1105#1090
        #1063#1105#1090)
      TabOrder = 0
    end
    object StopBits: TRadioGroup
      Left = 5
      Top = 97
      Width = 145
      Height = 33
      Caption = #1050#1086#1083#1080#1095#1077#1089#1090#1074#1086' '#1089#1090#1086#1087#1086#1074#1099#1093' '#1073#1080#1090
      Columns = 3
      ItemIndex = 1
      Items.Strings = (
        '1'
        '1,5'
        '2')
      TabOrder = 1
    end
    object ComPortSpeed: TRadioGroup
      Left = 4
      Top = 35
      Width = 340
      Height = 59
      Caption = #1057#1082#1086#1088#1086#1089#1090#1100' '#1086#1073#1084#1077#1085#1072' [ '#1073#1086#1076' ]'
      Columns = 4
      ItemIndex = 3
      Items.Strings = (
        '1200'
        '2400'
        '4800'
        '9600'
        '19200'
        '38400'
        '57600'
        '115200')
      TabOrder = 2
    end
    object ComPortNumber: TUpDown
      Left = 161
      Top = 13
      Width = 17
      Height = 20
      Min = 1
      Max = 255
      Position = 1
      TabOrder = 3
    end
    object Edit3: TEdit
      Left = 106
      Top = 13
      Width = 55
      Height = 20
      BevelInner = bvNone
      BevelKind = bkFlat
      BorderStyle = bsNone
      TabOrder = 4
      Text = '1'
    end
    object ComPortDelay: TEdit
      Left = 253
      Top = 12
      Width = 79
      Height = 20
      BevelInner = bvNone
      BevelKind = bkFlat
      BorderStyle = bsNone
      TabOrder = 5
      Text = '1'
      OnChange = host_addrChange
    end
  end
  object pnl1: TPanel
    Left = 10
    Top = 102
    Width = 359
    Height = 74
    TabOrder = 2
    Visible = False
    object Label4: TLabel
      Left = 8
      Top = 24
      Width = 37
      Height = 13
      Caption = #1057#1077#1088#1074#1077#1088
    end
    object Label6: TLabel
      Left = 10
      Top = 49
      Width = 25
      Height = 13
      Caption = #1055#1086#1088#1090
    end
    object Label5: TLabel
      Left = 152
      Top = 49
      Width = 51
      Height = 13
      Caption = #1047#1072#1076#1077#1088#1078#1082#1072
      Visible = False
    end
    object host_addr: TEdit
      Left = 57
      Top = 21
      Width = 284
      Height = 20
      BevelInner = bvNone
      BevelKind = bkFlat
      BorderStyle = bsNone
      TabOrder = 0
      OnChange = host_addrChange
    end
    object host_port: TEdit
      Left = 58
      Top = 47
      Width = 79
      Height = 20
      BevelInner = bvNone
      BevelKind = bkFlat
      BorderStyle = bsNone
      TabOrder = 1
      Text = '1'
      OnChange = host_addrChange
    end
    object ComPortDelayTCP: TEdit
      Left = 217
      Top = 47
      Width = 79
      Height = 20
      BevelInner = bvNone
      BevelKind = bkFlat
      BorderStyle = bsNone
      TabOrder = 2
      Text = '1'
      Visible = False
      OnChange = host_addrChange
    end
  end
  object media_type: TRadioGroup
    Left = 9
    Top = 64
    Width = 360
    Height = 33
    Caption = #1042#1080#1076' '#1089#1086#1077#1076#1080#1085#1077#1085#1080#1103
    Columns = 3
    ItemIndex = 1
    Items.Strings = (
      'COM-Port'
      'TCP/IP'
      'COM-Port/TCP')
    TabOrder = 0
    OnClick = media_typeClick
  end
  object GroupBox1: TGroupBox
    Left = 9
    Top = -1
    Width = 360
    Height = 66
    TabOrder = 1
    object Label2: TLabel
      Left = 12
      Top = 39
      Width = 80
      Height = 13
      Caption = #1048#1084#1103' '#1091#1089#1090#1088#1086#1081#1089#1090#1074#1072
    end
    object Label1: TLabel
      Left = 12
      Top = 11
      Width = 48
      Height = 13
      Caption = #1053#1086#1084#1077#1088' '#1050#1055
    end
    object DevName: TEdit
      Left = 97
      Top = 36
      Width = 260
      Height = 21
      BevelInner = bvNone
      BevelKind = bkFlat
      BevelOuter = bvRaised
      BorderStyle = bsNone
      TabOrder = 0
      OnChange = DevNumberChange
    end
    object DevNumber: TEdit
      Left = 97
      Top = 9
      Width = 54
      Height = 21
      BevelInner = bvNone
      BevelKind = bkFlat
      BevelOuter = bvRaised
      BorderStyle = bsNone
      TabOrder = 1
      OnChange = DevNumberChange
    end
  end
end
