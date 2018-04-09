inherited SCGroupDataFrame: TSCGroupDataFrame
  object Label10: TLabel [1]
    Left = 268
    Top = 128
    Width = 110
    Height = 13
    Caption = #1058#1072#1081#1084#1077#1088' '#1074#1099#1079#1086#1074#1072' ('#1089#1077#1082'):'
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Pitch = fpFixed
    Font.Style = []
    ParentFont = False
  end
  object Label15: TLabel [2]
    Left = 268
    Top = 149
    Width = 111
    Height = 13
    Caption = #1042#1088#1077#1084#1103' '#1086#1090#1074#1077#1090#1072'     ('#1084#1089') :'
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Pitch = fpFixed
    Font.Style = []
    ParentFont = False
  end
  object Label16: TLabel [3]
    Left = 268
    Top = 171
    Width = 113
    Height = 13
    Caption = #1048#1079#1084#1077#1085#1077#1085#1080#1081' '#1087#1086' '#1074#1099#1079#1086#1074#1091
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Pitch = fpFixed
    Font.Style = []
    ParentFont = False
  end
  object Label17: TLabel [4]
    Left = 268
    Top = 189
    Width = 120
    Height = 13
    Caption = #1044#1072#1085#1085#1099#1093' '#1073#1077#1079' '#1080#1079#1084#1077#1085#1077#1085#1080#1081
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Pitch = fpFixed
    Font.Style = []
    ParentFont = False
  end
  object Label11: TLabel [5]
    Left = 268
    Top = 210
    Width = 120
    Height = 13
    Caption = #1053#1077#1086#1090#1074#1077#1095#1077#1085#1085#1099#1093' '#1074#1099#1079#1086#1074#1086#1074
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Pitch = fpFixed
    Font.Style = []
    ParentFont = False
  end
  object ObjectList: TListView [6]
    Left = 1
    Top = 125
    Width = 258
    Height = 200
    Anchors = [akLeft, akTop, akBottom]
    Columns = <
      item
        Caption = #8470
        Width = 30
      end
      item
        Alignment = taCenter
        Caption = #1047#1085#1072#1095#1077#1085#1080#1077
        Width = 60
      end
      item
        Alignment = taCenter
        Caption = #1048#1079#1084#1077#1085#1077#1085#1080#1103
        Width = 70
      end
      item
        Alignment = taCenter
        Caption = #1044#1080#1072#1075#1085#1086#1089#1090#1080#1082#1072
        Width = 80
      end>
    GridLines = True
    HideSelection = False
    ReadOnly = True
    RowSelect = True
    TabOrder = 5
    ViewStyle = vsReport
    OnChanging = ObjectListChanging
    OnSelectItem = ObjectListSelectItem
  end
  inherited GroupBox1: TGroupBox
    inherited FullDiagText: TStaticText
      Width = 341
      ExplicitWidth = 341
    end
    inherited PartDiagText: TStaticText
      Width = 341
      ExplicitWidth = 341
    end
    inherited StateText: TStaticText
      Width = 341
      ExplicitWidth = 341
    end
  end
  inherited TimeStampText: TStaticText
    Width = 341
    ExplicitWidth = 341
  end
  object TuOnMore: TBitBtn [9]
    Left = 3
    Top = 96
    Width = 72
    Height = 23
    BiDiMode = bdLeftToRight
    Caption = #1041#1086#1083#1100#1096#1077
    Enabled = False
    Glyph.Data = {
      DE000000424DDE0000000000000076000000280000000D0000000D0000000100
      0400000000006800000000000000000000001000000000000000000000000000
      8000008000000080800080000000800080008080000080808000C0C0C0000000
      FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00FFFFFFFFFFFF
      F000FFFFFFFFFFFFF000FFFFF000FFFFF000FFFFF0F0FFFFF000FFFFF0F0FFFF
      F000FF0000F0000FF000FF0FFFFFFF0FF000FF0000F0000FF000FFFFF0F0FFFF
      F000FFFFF0F0FFFFF000FFFFF000FFFFF000FFFFFFFFFFFFF000FFFFFFFFFFFF
      F000}
    ParentBiDiMode = False
    TabOrder = 2
    OnClick = TuOnMoreClick
  end
  object TuOffLess: TBitBtn [10]
    Left = 78
    Top = 96
    Width = 72
    Height = 23
    Caption = #1052#1077#1085#1100#1096#1077
    Enabled = False
    Glyph.Data = {
      DE000000424DDE0000000000000076000000280000000D0000000D0000000100
      0400000000006800000000000000000000001000000000000000000000000000
      8000008000000080800080000000800080008080000080808000C0C0C0000000
      FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00FFFFFFFFFFFF
      F000FFFFFFFFFFFFF000FFFFFFFFFFFFF000FFFFFFFFFFFFF000FFFFFFFFFFFF
      F000FF000000000FF000FF0FFFFFFF0FF000FF000000000FF000FFFFFFFFFFFF
      F000FFFFFFFFFFFFF000FFFFFFFFFFFFF000FFFFFFFFFFFFF000FFFFFFFFFFFF
      F000}
    TabOrder = 3
    OnClick = TuOffLessClick
  end
  object TrUndo: TBitBtn [11]
    Left = 154
    Top = 96
    Width = 105
    Height = 23
    Caption = #1054#1090#1084#1077#1085#1072
    Enabled = False
    Glyph.Data = {
      F6000000424DF600000000000000760000002800000010000000100000000100
      0400000000008000000000000000000000001000000000000000000000000000
      8000008000000080800080000000800080008080000080808000C0C0C0000000
      FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00888888888888
      8888888888888888888888808888888888088800088888888888880000888888
      8088888000888888088888880008888008888888800088008888888888000008
      8888888888800088888888888800000888888888800088008888888000088880
      0888880000888888008888000888888888088888888888888888}
    TabOrder = 4
    OnClick = TrUndoClick
  end
  object CalcValueBox: TGroupBox [12]
    Left = 263
    Top = 226
    Width = 170
    Height = 122
    BiDiMode = bdLeftToRight
    Caption = #1050#1072#1083#1100#1082#1091#1083#1103#1090#1086#1088' '#1079#1085#1072#1095#1077#1085#1080#1103
    ParentBiDiMode = False
    TabOrder = 6
    object Label6: TLabel
      Left = 8
      Top = 15
      Width = 74
      Height = 13
      Caption = #1053#1072#1095#1072#1083#1086' '#1096#1082#1072#1083#1099
    end
    object Label7: TLabel
      Left = 9
      Top = 35
      Width = 68
      Height = 13
      Caption = #1050#1086#1085#1077#1094' '#1096#1082#1072#1083#1099
    end
    object Label8: TLabel
      Left = 11
      Top = 58
      Width = 39
      Height = 13
      Caption = #1050#1074#1072#1085#1090#1099
    end
    object Label9: TLabel
      Left = 12
      Top = 81
      Width = 53
      Height = 13
      Caption = #1056#1077#1079#1091#1083#1100#1090#1072#1090
    end
    object Result: TStaticText
      Left = 8
      Top = 96
      Width = 156
      Height = 16
      Alignment = taCenter
      AutoSize = False
      BorderStyle = sbsSunken
      TabOrder = 0
    end
    object ScaleBegin: TEdit
      Left = 90
      Top = 16
      Width = 76
      Height = 18
      BevelInner = bvLowered
      BevelKind = bkFlat
      BorderStyle = bsNone
      TabOrder = 1
      Text = '0'
      OnChange = KvantsChange
    end
    object ScaleEnd: TEdit
      Left = 90
      Top = 35
      Width = 76
      Height = 18
      BevelInner = bvLowered
      BevelKind = bkFlat
      BorderStyle = bsNone
      TabOrder = 2
      Text = '250'
      OnChange = KvantsChange
    end
    object Kvants: TEdit
      Left = 90
      Top = 56
      Width = 76
      Height = 18
      BevelInner = bvLowered
      BevelKind = bkFlat
      BorderStyle = bsNone
      TabOrder = 3
      Text = '0'
      OnChange = KvantsChange
    end
  end
  object PDiagText: TStaticText [13]
    Left = 2
    Top = 329
    Width = 258
    Height = 18
    Alignment = taCenter
    Anchors = [akLeft, akBottom]
    AutoSize = False
    BorderStyle = sbsSunken
    TabOrder = 7
  end
  object GrpTimerText: TStaticText [14]
    Left = 392
    Top = 127
    Width = 42
    Height = 17
    Alignment = taCenter
    AutoSize = False
    BorderStyle = sbsSunken
    Caption = '09'
    TabOrder = 8
  end
  object RespondTime: TStaticText [15]
    Left = 392
    Top = 148
    Width = 42
    Height = 17
    Alignment = taCenter
    AutoSize = False
    BorderStyle = sbsSunken
    Caption = '09'
    TabOrder = 9
  end
  object Chdbc_counter: TStaticText [16]
    Left = 392
    Top = 168
    Width = 42
    Height = 17
    Alignment = taCenter
    AutoSize = False
    BorderStyle = sbsSunken
    Caption = '09'
    TabOrder = 10
  end
  object Nchd_counter: TStaticText [17]
    Left = 392
    Top = 188
    Width = 42
    Height = 17
    Alignment = taCenter
    AutoSize = False
    BorderStyle = sbsSunken
    Caption = '09'
    TabOrder = 11
  end
  object NoRespondCounter: TStaticText [18]
    Left = 392
    Top = 209
    Width = 42
    Height = 17
    Alignment = taCenter
    AutoSize = False
    BorderStyle = sbsSunken
    Caption = '09'
    TabOrder = 12
  end
  object ResetTuError: TBitBtn [19]
    Left = 265
    Top = 96
    Width = 168
    Height = 23
    Caption = #1057#1085#1103#1090#1100' '#1085#1077#1080#1089#1087#1088#1072#1074#1085#1086#1089#1090#1100' '#1058#1059
    Glyph.Data = {
      36050000424D3605000000000000360400002800000010000000100000000100
      0800000000000001000000000000000000000001000000010000FF00FF000000
      82000566950003689A00086B9A000268A3000274A9000479AF001479AD00000E
      E6000011ED000016F700001AFB000020FF000325FF00072FFF000D36FF001641
      FF001D49FF002753FF002C59FF000787BB00108BBC001F91BF000386C100058B
      C7000791C5001CAAD3002CA7D30031B5D30030B0DA001FC6F40012CAFD0030C9
      E90020CEFE003FE3FE0079C0DC0048CFF20054DEFF0055F6FE005FFEFF0074E0
      F80068F6FF0073FAFE0078FBFE007FFEFF0083FAFB0089FBFE0092FFFF009FFB
      FE0098FFFF00A7E9F300A3F3FF00A0FFFF00B6F2FF00B7FFFF00C6FFFF00D8F6
      FC00DCFFFF00EDFAFC00F0FFFF00FBFFFF000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000002020202020200000000000000000006212022
      26291C0200000000000000062B23201F0101010101010101000000062B23201F
      0113110F0D0B0901000000062B23201F011412100D0C0A010000000631343639
      010101010101010100000006331B1A151617082402000000000000061D232022
      26291E0503000000000000062B23202226291E1802000000000000062B232022
      26291E1802000000000000062B23202226291E1802000000000000062C2A2728
      2C2F251902000000000000063D3C3835313130270200000000000000073B3A37
      312F2E0400000000000000000006060606060600000000000000}
    TabOrder = 13
    OnClick = ResetTuErrorClick
  end
end
