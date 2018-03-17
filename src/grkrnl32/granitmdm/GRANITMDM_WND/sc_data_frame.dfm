inherited SCDataFrame: TSCDataFrame
  AutoScroll = False
  Visible = True
  object GroupBox1: TGroupBox
    Left = 0
    Top = 0
    Width = 443
    Height = 277
    Align = alClient
    TabOrder = 0
    DesignSize = (
      443
      277)
    object Label1: TLabel
      Left = 7
      Top = 15
      Width = 67
      Height = 13
      Caption = #1044#1080#1072#1075#1085#1086#1089#1090#1080#1082#1072
    end
    object Label6: TLabel
      Left = 8
      Top = 40
      Width = 194
      Height = 13
      Caption = #1042#1088#1077#1084#1103' '#1087#1086#1089#1083#1077#1076#1085#1077#1075#1086' '#1080#1079#1084#1077#1085#1077#1085#1080#1103' '#1076#1072#1085#1085#1099#1093
      Visible = False
    end
    object Label7: TLabel
      Left = 8
      Top = 64
      Width = 83
      Height = 13
      Caption = #1058#1072#1081#1084#1077#1088' '#1074#1099#1079#1086#1074#1072' '
    end
    object ListView1: TListView
      Left = 5
      Top = 84
      Width = 295
      Height = 282
      Anchors = [akLeft, akTop, akBottom]
      BevelInner = bvNone
      BevelOuter = bvNone
      Columns = <
        item
          Caption = #1053#1086#1084#1077#1088
          Width = 46
        end
        item
          Caption = #1047#1085#1072#1095#1077#1085#1080#1077
          Width = 68
        end
        item
          Caption = #1044#1080#1072#1075#1085#1086#1089#1090#1080#1082#1072
          Width = 80
        end
        item
          Caption = #1048#1079#1084#1077#1085#1077#1085#1080#1103
          Width = 80
        end>
      ColumnClick = False
      FlatScrollBars = True
      GridLines = True
      HideSelection = False
      ReadOnly = True
      RowSelect = True
      TabOrder = 0
      ViewStyle = vsReport
      OnChange = ListView1Change
    end
    object Diag: TStaticText
      Left = 91
      Top = 15
      Width = 342
      Height = 17
      Alignment = taCenter
      AutoSize = False
      BorderStyle = sbsSunken
      Caption = 'Diag'
      TabOrder = 1
    end
    object GroupBox2: TGroupBox
      Left = 305
      Top = 136
      Width = 136
      Height = 197
      BiDiMode = bdLeftToRight
      Caption = #1050#1072#1083#1100#1082#1091#1083#1103#1090#1086#1088' '#1079#1085#1072#1095#1077#1085#1080#1103
      ParentBiDiMode = False
      TabOrder = 2
      object Label2: TLabel
        Left = 8
        Top = 16
        Width = 74
        Height = 13
        Caption = #1053#1072#1095#1072#1083#1086' '#1096#1082#1072#1083#1099
      end
      object Label3: TLabel
        Left = 9
        Top = 59
        Width = 68
        Height = 13
        Caption = #1050#1086#1085#1077#1094' '#1096#1082#1072#1083#1099
      end
      object Label4: TLabel
        Left = 10
        Top = 99
        Width = 38
        Height = 13
        Caption = #1050#1074#1072#1085#1090#1099
      end
      object Label5: TLabel
        Left = 12
        Top = 144
        Width = 52
        Height = 13
        Caption = #1056#1077#1079#1091#1083#1100#1090#1072#1090
      end
      object ScaleBegin: TEdit
        Left = 8
        Top = 32
        Width = 113
        Height = 21
        TabOrder = 0
        Text = '0'
        OnChange = KvantsChange
      end
      object ScaleEnd: TEdit
        Left = 9
        Top = 75
        Width = 113
        Height = 21
        TabOrder = 1
        Text = '250'
        OnChange = KvantsChange
      end
      object Kvants: TEdit
        Left = 10
        Top = 115
        Width = 113
        Height = 21
        TabOrder = 2
        Text = '0'
        OnChange = KvantsChange
      end
      object Result: TStaticText
        Left = 12
        Top = 160
        Width = 114
        Height = 16
        Alignment = taCenter
        AutoSize = False
        BorderStyle = sbsSunken
        TabOrder = 3
      end
    end
    object TimeStamp: TStaticText
      Left = 216
      Top = 39
      Width = 217
      Height = 17
      Alignment = taCenter
      AutoSize = False
      BorderStyle = sbsSunken
      Caption = '20-06-2005 01:02:03:44'
      TabOrder = 3
      Visible = False
    end
    object TrUndo: TBitBtn
      Left = 307
      Top = 109
      Width = 136
      Height = 23
      Caption = #1054#1090#1084#1077#1085#1072
      Enabled = False
      TabOrder = 4
      OnClick = TrUndoClick
      Glyph.Data = {
        F6000000424DF600000000000000760000002800000010000000100000000100
        0400000000008000000000000000000000001000000000000000000000000000
        8000008000000080800080000000800080008080000080808000C0C0C0000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00888888888888
        8888888888888888888888808888888888088800088888888888880000888888
        8088888000888888088888880008888008888888800088008888888888000008
        8888888888800088888888888800000888888888800088008888888000088880
        0888880000888888008888000888888888088888888888888888}
    end
    object TuOnMore: TBitBtn
      Left = 307
      Top = 84
      Width = 66
      Height = 23
      BiDiMode = bdLeftToRight
      Caption = #1041#1086#1083#1100#1096#1077
      Enabled = False
      ParentBiDiMode = False
      TabOrder = 5
      OnClick = TuOnMoreClick
      Glyph.Data = {
        DE000000424DDE0000000000000076000000280000000D0000000D0000000100
        0400000000006800000000000000000000001000000000000000000000000000
        8000008000000080800080000000800080008080000080808000C0C0C0000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00FFFFFFFFFFFF
        F000FFFFFFFFFFFFF000FFFFF000FFFFF000FFFFF0F0FFFFF000FFFFF0F0FFFF
        F000FF0000F0000FF000FF0FFFFFFF0FF000FF0000F0000FF000FFFFF0F0FFFF
        F000FFFFF0F0FFFFF000FFFFF000FFFFF000FFFFFFFFFFFFF000FFFFFFFFFFFF
        F000}
    end
    object TuOffLess: TBitBtn
      Left = 376
      Top = 84
      Width = 66
      Height = 23
      Caption = #1052#1077#1085#1100#1096#1077
      Enabled = False
      TabOrder = 6
      OnClick = TuOffLessClick
      Glyph.Data = {
        DE000000424DDE0000000000000076000000280000000D0000000D0000000100
        0400000000006800000000000000000000001000000000000000000000000000
        8000008000000080800080000000800080008080000080808000C0C0C0000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00FFFFFFFFFFFF
        F000FFFFFFFFFFFFF000FFFFFFFFFFFFF000FFFFFFFFFFFFF000FFFFFFFFFFFF
        F000FF000000000FF000FF0FFFFFFF0FF000FF000000000FF000FFFFFFFFFFFF
        F000FFFFFFFFFFFFF000FFFFFFFFFFFFF000FFFFFFFFFFFFF000FFFFFFFFFFFF
        F000}
    end
    object dyn_timer: TStaticText
      Left = 217
      Top = 62
      Width = 217
      Height = 17
      Alignment = taCenter
      AutoSize = False
      BorderStyle = sbsSunken
      Caption = '0'
      TabOrder = 7
      Visible = False
    end
  end
  object Timer1: TTimer
    Enabled = False
    OnTimer = Timer1Timer
    Left = 16
    Top = 240
  end
end
