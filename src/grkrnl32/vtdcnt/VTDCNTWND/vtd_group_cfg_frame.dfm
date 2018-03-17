inherited VtdGroupConfig: TVtdGroupConfig
  Width = 443
  Height = 293
  Align = alClient
  Visible = False
  ExplicitWidth = 443
  ExplicitHeight = 293
  DesignSize = (
    443
    293)
  object Label1: TLabel
    Left = 12
    Top = 39
    Width = 34
    Height = 13
    Caption = #1053#1086#1084#1077#1088' '
  end
  object Label3: TLabel
    Left = 10
    Top = 66
    Width = 139
    Height = 13
    Caption = #1055#1077#1088#1080#1086#1076' '#1086#1087#1088#1086#1089#1072' '#1087#1072#1088#1072#1084#1077#1090#1088#1086#1074
  end
  object Label11: TLabel
    Left = 210
    Top = 67
    Width = 173
    Height = 13
    Caption = #1089#1077#1082' ('#1084#1080#1085#1080#1084#1072#1083#1100#1085#1086#1077' '#1079#1085#1072#1095#1077#1085#1080#1077' 3 '#1089#1077#1082')'
  end
  object Label2: TLabel
    Left = 12
    Top = 16
    Width = 97
    Height = 13
    Caption = #1053#1086#1084#1077#1088' '#1075#1088#1091#1087#1087#1099' '#1074' '#1041#1044
  end
  object GroupNumber: TLabel
    Left = 155
    Top = 16
    Width = 13
    Height = 13
    Caption = '---'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object NumberEdit: TEdit
    Left = 54
    Top = 36
    Width = 55
    Height = 21
    BevelInner = bvNone
    BevelKind = bkFlat
    BorderStyle = bsNone
    TabOrder = 0
    Text = '1'
    OnChange = NumberEditChange
  end
  object ParamKind: TComboBox
    Left = 155
    Top = 36
    Width = 201
    Height = 21
    BevelInner = bvNone
    BevelKind = bkFlat
    ItemIndex = 0
    TabOrder = 1
    Text = #1058#1088#1091#1073#1072
    OnChange = ParamKindChange
    Items.Strings = (
      #1058#1088#1091#1073#1072
      #1058#1077#1082#1091#1097#1080#1077' '#1087#1072#1088#1072#1084#1077#1090#1088#1099'  '#1090#1088#1091#1073#1099
      #1055#1086#1090#1088#1077#1073#1080#1090#1077#1083#1100
      #1058#1077#1082#1091#1097#1080#1077' '#1087#1072#1088#1072#1084#1077#1088#1090#1088#1099' '#1087#1086#1090#1088#1077#1073#1080#1090#1077#1083#1103)
  end
  object GrpNumber: TUpDown
    Left = 109
    Top = 36
    Width = 15
    Height = 21
    Associate = NumberEdit
    Min = 1
    Max = 10
    Position = 1
    TabOrder = 2
  end
  object VTDPanel: TPanel
    Left = 6
    Top = 86
    Width = 373
    Height = 38
    Anchors = [akLeft, akTop, akRight, akBottom]
    BevelOuter = bvNone
    TabOrder = 3
    object Label4: TLabel
      Left = 4
      Top = 9
      Width = 135
      Height = 13
      Caption = #1053#1086#1084#1077#1088' '#1087#1077#1088#1074#1086#1075#1086' '#1087#1072#1088#1072#1084#1077#1090#1088#1072' '
    end
    object Label8: TLabel
      Left = 227
      Top = 9
      Width = 60
      Height = 13
      Caption = #1050#1086#1083#1080#1095#1077#1089#1090#1074#1086
    end
    object Edit3: TEdit
      Left = 150
      Top = 6
      Width = 49
      Height = 21
      BevelInner = bvNone
      BevelKind = bkFlat
      BorderStyle = bsNone
      TabOrder = 0
      Text = '1'
      OnChange = Edit3Change
    end
    object FirstParam: TUpDown
      Left = 199
      Top = 6
      Width = 15
      Height = 21
      Associate = Edit3
      Max = 62
      Position = 1
      TabOrder = 1
    end
    object Edit4: TEdit
      Left = 293
      Top = 6
      Width = 41
      Height = 21
      BevelInner = bvNone
      BevelKind = bkFlat
      BorderStyle = bsNone
      TabOrder = 2
      Text = '63'
      OnChange = Edit3Change
    end
    object ParamCount: TUpDown
      Left = 334
      Top = 6
      Width = 15
      Height = 21
      Associate = Edit4
      Min = 1
      Max = 63
      Position = 63
      TabOrder = 3
    end
  end
  object QueryPeriod: TEdit
    Left = 155
    Top = 61
    Width = 49
    Height = 21
    BevelInner = bvNone
    BevelKind = bkFlat
    BorderStyle = bsNone
    TabOrder = 4
    OnChange = QueryPeriodChange
  end
end
