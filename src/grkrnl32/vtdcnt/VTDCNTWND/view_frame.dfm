inherited ViewFrame: TViewFrame
  Width = 407
  Height = 253
  ExplicitWidth = 407
  ExplicitHeight = 253
  object ViewDevPanel: TPanel
    Left = 0
    Top = 0
    Width = 407
    Height = 253
    Align = alClient
    BevelOuter = bvLowered
    TabOrder = 0
    Visible = False
    DesignSize = (
      407
      253)
    object Label1: TLabel
      Left = 7
      Top = 8
      Width = 148
      Height = 13
      Caption = #1047#1072#1074#1086#1076#1089#1082#1086#1081' '#1085#1086#1084#1077#1088' '#1091#1089#1090#1088#1086#1081#1089#1090#1074#1072
    end
    object PlantDevNumber: TLabel
      Left = 166
      Top = 8
      Width = 17
      Height = 13
      Caption = '----'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label3: TLabel
      Left = 7
      Top = 24
      Width = 144
      Height = 13
      Caption = #1044#1072#1090#1072' '#1080' '#1074#1088#1077#1084#1103' '#1085#1072' '#1091#1089#1090#1088#1086#1081#1089#1090#1074#1077
    end
    object DevDtTm: TLabel
      Left = 166
      Top = 24
      Width = 17
      Height = 13
      Caption = '----'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label4: TLabel
      Left = 7
      Top = 40
      Width = 140
      Height = 13
      Caption = #1044#1072#1090#1072'  '#1086#1090#1095#1105#1090#1086#1074' '#1055#1086#1089#1083#1077#1076#1085#1077#1075#1086' '
    end
    object LastRepDt: TLabel
      Left = 166
      Top = 40
      Width = 17
      Height = 13
      Caption = '----'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label5: TLabel
      Left = 233
      Top = 40
      Width = 73
      Height = 13
      Caption = #1055#1088#1077#1076#1099#1076#1091#1097#1077#1075#1086
    end
    object PrevRepDt: TLabel
      Left = 321
      Top = 40
      Width = 17
      Height = 13
      Caption = '----'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object GroupBox1: TGroupBox
      Left = 4
      Top = 57
      Width = 401
      Height = 192
      Anchors = [akLeft, akTop, akRight, akBottom]
      Caption = #1042#1088#1077#1084#1077#1085#1072' '#1087#1091#1089#1082#1086#1074' '#1080' '#1086#1089#1085#1086#1074#1085#1099#1077' '#1087#1072#1088#1072#1084#1077#1090#1088#1099' '#1082#1086#1085#1092#1080#1075#1091#1088#1072#1094#1080#1080
      TabOrder = 0
      DesignSize = (
        401
        192)
      object DevListView: TListView
        Left = 5
        Top = 16
        Width = 390
        Height = 170
        Anchors = [akLeft, akTop, akRight, akBottom]
        Columns = <
          item
            Caption = #8470' '#1090#1088
          end
          item
            Caption = #1042#1088#1077#1084#1103' '#1055#1091#1089#1082#1072
            Width = 100
          end
          item
            Caption = #1050#1086#1101#1092#1092'.'
          end
          item
            Caption = #1055#1072#1088#1072#1084#1077#1090#1088#1099
            Width = 80
          end
          item
            Caption = #1048#1085#1076#1080#1082#1072#1090#1086#1088
            Width = 100
          end>
        GridLines = True
        Items.ItemData = {
          051A0100000A00000000000000FFFFFFFFFFFFFFFF00000000FFFFFFFF000000
          0001310000000000FFFFFFFFFFFFFFFF00000000FFFFFFFF0000000001320000
          000000FFFFFFFFFFFFFFFF00000000FFFFFFFF0000000001330000000000FFFF
          FFFFFFFFFFFF00000000FFFFFFFF0000000001340000000000FFFFFFFFFFFFFF
          FF00000000FFFFFFFF0000000001350000000000FFFFFFFFFFFFFFFF00000000
          FFFFFFFF0000000001360000000000FFFFFFFFFFFFFFFF00000000FFFFFFFF00
          00000001370000000000FFFFFFFFFFFFFFFF00000000FFFFFFFF000000000138
          0000000000FFFFFFFFFFFFFFFF00000000FFFFFFFF0000000001390000000000
          FFFFFFFFFFFFFFFF00000000FFFFFFFF000000000231003000}
        ReadOnly = True
        TabOrder = 0
        ViewStyle = vsReport
      end
    end
  end
  object ViewGrpPanel: TPanel
    Left = 0
    Top = 0
    Width = 407
    Height = 253
    Align = alClient
    BevelOuter = bvLowered
    TabOrder = 1
    Visible = False
    DesignSize = (
      407
      253)
    object Label2: TLabel
      Left = 6
      Top = 6
      Width = 31
      Height = 13
      Caption = 'Label2'
      Visible = False
    end
    object ViewGrpList: TListView
      Left = 1
      Top = 24
      Width = 405
      Height = 221
      Anchors = [akLeft, akTop, akRight, akBottom]
      Columns = <>
      GridLines = True
      ReadOnly = True
      RowSelect = True
      TabOrder = 0
      ViewStyle = vsReport
    end
  end
end
