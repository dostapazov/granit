inherited SCCpDataFrame: TSCCpDataFrame
  Height = 344
  Constraints.MinHeight = 344
  ExplicitHeight = 344
  DesignSize = (
    451
    344)
  object Label8: TLabel [1]
    Left = 233
    Top = 100
    Width = 83
    Height = 13
    Caption = #1055#1077#1088#1077#1076#1072#1095#1072' '#1073#1072#1081#1090#1072
  end
  object Label9: TLabel [2]
    Left = 369
    Top = 99
    Width = 31
    Height = 13
    Caption = #1082#1072#1076#1088#1072
  end
  object Label10: TLabel [3]
    Left = 2
    Top = 100
    Width = 158
    Height = 13
    Caption = #1058#1072#1081#1084#1077#1088'  '#1088#1072#1073#1086#1090#1086#1089#1087#1086#1089#1086#1073#1085#1086#1089#1090#1080' '#1050#1055
  end
  object GroupBox2: TGroupBox [4]
    Left = 0
    Top = 122
    Width = 443
    Height = 598
    Anchors = [akLeft, akTop, akRight, akBottom]
    TabOrder = 2
    DesignSize = (
      443
      598)
    object ListView1: TListView
      Left = 2
      Top = 16
      Width = 149
      Height = 178
      Columns = <
        item
          Caption = #1040#1076#1088#1077#1089
          Width = 60
        end
        item
          Alignment = taCenter
          Caption = #1044#1080#1072#1075#1085#1086#1089#1090#1080#1082#1072
          Width = 80
        end>
      ColumnClick = False
      GridLines = True
      HideSelection = False
      ReadOnly = True
      RowSelect = True
      TabOrder = 0
      ViewStyle = vsReport
      OnSelectItem = ListView1SelectItem
    end
    object TuList: TListView
      Left = 152
      Top = 16
      Width = 288
      Height = 178
      Anchors = [akLeft, akTop, akRight]
      Columns = <
        item
          Caption = #1054#1073#1098#1077#1082#1090
          Width = 75
        end
        item
          Alignment = taCenter
          Caption = #1042#1083#1072#1076#1077#1083#1077#1094
          Width = 75
        end
        item
          AutoSize = True
          Caption = #1057#1086#1089#1090#1086#1103#1085#1080#1077
        end>
      ColumnClick = False
      GridLines = True
      HideSelection = False
      ReadOnly = True
      RowSelect = True
      TabOrder = 1
      ViewStyle = vsReport
    end
    object GrpPDDiag: TStaticText
      Left = 4
      Top = 200
      Width = 433
      Height = 17
      Anchors = [akLeft, akTop, akRight]
      AutoSize = False
      BorderStyle = sbsSunken
      TabOrder = 2
    end
  end
  inherited GroupBox1: TGroupBox
    inherited Label5: TLabel
      Top = 72
      ExplicitTop = 72
    end
  end
  inherited TimeStampText: TStaticText
    Top = 70
    Width = 339
    Caption = '12.12.2005 19:19:22:345'
    ExplicitTop = 70
    ExplicitWidth = 339
  end
  object TxByteTime: TStaticText [7]
    Left = 319
    Top = 98
    Width = 46
    Height = 17
    Alignment = taCenter
    AutoSize = False
    BorderStyle = sbsSunken
    Caption = '1222'
    TabOrder = 3
  end
  object TxDeliverTime: TStaticText [8]
    Left = 403
    Top = 98
    Width = 44
    Height = 17
    Alignment = taCenter
    AutoSize = False
    BorderStyle = sbsSunken
    Caption = '1222'
    TabOrder = 4
  end
  object RxTimer: TStaticText [9]
    Left = 172
    Top = 98
    Width = 54
    Height = 17
    Alignment = taCenter
    AutoSize = False
    BorderStyle = sbsSunken
    Caption = '1222'
    TabOrder = 5
  end
  object StaticText1: TStaticText [10]
    Left = 10
    Top = 120
    Width = 119
    Height = 17
    Caption = #1044#1080#1072#1075#1085#1086#1089#1090#1080#1082#1072' '#1075#1088#1091#1087#1087' '#1050#1055
    TabOrder = 6
  end
  object StaticText2: TStaticText [11]
    Left = 151
    Top = 120
    Width = 104
    Height = 17
    Caption = #1057#1087#1080#1089#1086#1082' '#1058#1059'/'#1058#1056' '#1085#1072' '#1050#1055
    TabOrder = 7
  end
  inherited DynTimer: TTimer
    Left = 368
    Top = 24
  end
end
