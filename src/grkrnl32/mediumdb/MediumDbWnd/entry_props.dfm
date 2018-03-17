object EntryProps: TEntryProps
  Left = 0
  Top = 0
  BorderIcons = []
  BorderStyle = bsToolWindow
  Caption = #1057#1074#1086#1081#1089#1090#1074#1072' '#1086#1090#1086#1073#1088#1072#1078#1077#1085#1080#1103
  ClientHeight = 224
  ClientWidth = 333
  Color = clBtnFace
  Constraints.MaxWidth = 339
  Constraints.MinHeight = 187
  Constraints.MinWidth = 339
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  PopupMode = pmAuto
  Position = poDesigned
  OnHide = FormHide
  PixelsPerInch = 96
  TextHeight = 13
  object GridPanel1: TGridPanel
    Left = 0
    Top = 0
    Width = 333
    Height = 224
    Align = alClient
    Caption = #1055#1072#1088#1072#1084#1077#1090#1088#1099' '#1086#1090#1086#1073#1088#1072#1078#1077#1085#1080#1103
    ColumnCollection = <
      item
        Value = 39.759036144578320000
      end
      item
        Value = 60.240963855421680000
      end>
    ControlCollection = <
      item
        Column = 0
        Control = Label1
        Row = 0
      end
      item
        Column = 0
        Control = Label2
        Row = 1
      end
      item
        Column = 0
        Control = Label3
        Row = 2
      end
      item
        Column = 0
        Control = Label4
        Row = 3
      end
      item
        Column = 1
        Control = short_name
        Row = 3
      end
      item
        Column = 1
        Control = paint_as
        Row = 0
      end
      item
        Column = 1
        Control = bk_color
        Row = 1
      end
      item
        Column = 1
        Control = fg_color
        Row = 2
      end
      item
        Column = 0
        Control = Label5
        Row = 4
      end
      item
        Column = 1
        Control = Panel1
        Row = 4
      end
      item
        Column = 0
        Control = Label6
        Row = 5
      end
      item
        Column = 1
        Control = Panel2
        Row = 5
      end
      item
        Column = 0
        Control = Label7
        Row = 6
      end
      item
        Column = 1
        Control = Panel3
        Row = 6
      end>
    ParentShowHint = False
    RowCollection = <
      item
        SizeStyle = ssAuto
        Value = 100.000000000000000000
      end
      item
        SizeStyle = ssAuto
      end
      item
        SizeStyle = ssAuto
      end
      item
        SizeStyle = ssAuto
        Value = 100.000000000000000000
      end
      item
        SizeStyle = ssAuto
        Value = 30.000000000000000000
      end
      item
        SizeStyle = ssAuto
      end
      item
        Value = 100.000000000000000000
      end>
    ShowCaption = False
    ShowHint = True
    TabOrder = 0
    DesignSize = (
      333
      224)
    object Label1: TLabel
      Left = 10
      Top = 5
      Width = 113
      Height = 13
      Anchors = []
      Caption = #1042#1072#1088#1080#1072#1085#1090' '#1086#1090#1086#1073#1088#1072#1078#1077#1085#1080#1103
      ExplicitLeft = 31
    end
    object Label2: TLabel
      Left = 39
      Top = 28
      Width = 55
      Height = 13
      Anchors = []
      Caption = #1062#1074#1077#1090' '#1092#1086#1085#1072
      ExplicitLeft = 60
      ExplicitTop = 26
    end
    object Label3: TLabel
      Left = 19
      Top = 54
      Width = 94
      Height = 13
      Anchors = []
      Caption = #1094#1074#1077#1090' '#1080#1079#1086#1073#1088#1072#1078#1077#1085#1080#1103
      ExplicitLeft = 40
      ExplicitTop = 48
    end
    object Label4: TLabel
      Left = 31
      Top = 97
      Width = 70
      Height = 13
      Anchors = []
      Caption = #1050#1086#1088#1086#1090#1082#1086#1077' '#1080#1084#1103
      ExplicitLeft = 52
      ExplicitTop = 66
    end
    object short_name: TRadioGroup
      Left = 132
      Top = 74
      Width = 200
      Height = 60
      Align = alTop
      Columns = 2
      ItemIndex = 0
      Items.Strings = (
        #1053#1077#1090
        #1089#1087#1088#1072#1074#1072
        #1089#1083#1077#1074#1072
        #1089#1074#1077#1088#1093#1091
        #1089#1085#1080#1079#1091)
      TabOrder = 0
      OnClick = short_nameClick
    end
    object paint_as: TComboBox
      Left = 132
      Top = 1
      Width = 200
      Height = 21
      Align = alClient
      Style = csDropDownList
      TabOrder = 1
      OnChange = paint_asChange
    end
    object bk_color: TPanel
      Left = 132
      Top = 22
      Width = 200
      Height = 25
      Align = alTop
      Caption = 'bk_color'
      ShowCaption = False
      TabOrder = 2
      object SelBkColor: TSpeedButton
        Left = 140
        Top = 1
        Width = 59
        Height = 23
        Hint = #1042#1099#1073#1086#1088' '#1094#1074#1077#1090#1072' '#1092#1086#1085#1072
        Align = alRight
        Caption = #1042#1099#1073#1088#1072#1090#1100
        OnClick = SelBkColorClick
        ExplicitLeft = 120
        ExplicitTop = 2
        ExplicitHeight = 22
      end
    end
    object fg_color: TPanel
      Left = 132
      Top = 47
      Width = 200
      Height = 27
      Align = alTop
      Caption = 'fg_color'
      ShowCaption = False
      TabOrder = 3
      object SelFgColor: TSpeedButton
        Left = 140
        Top = 1
        Width = 59
        Height = 25
        Hint = #1042#1099#1073#1086#1088' '#1094#1074#1077#1090#1072
        Align = alRight
        Caption = #1042#1099#1073#1088#1072#1090#1100
        OnClick = SelBkColorClick
        ExplicitLeft = 141
        ExplicitTop = 2
      end
    end
    object Label5: TLabel
      Left = 51
      Top = 142
      Width = 31
      Height = 13
      Anchors = []
      Caption = #1051#1080#1085#1080#1103
      ExplicitLeft = 54
      ExplicitTop = 134
    end
    object Panel1: TPanel
      Left = 132
      Top = 134
      Width = 200
      Height = 30
      Align = alClient
      Caption = 'Panel1'
      ShowCaption = False
      TabOrder = 4
      object Edit1: TEdit
        Left = 5
        Top = 5
        Width = 44
        Height = 21
        TabOrder = 0
        Text = '0'
      end
      object line_width: TUpDown
        Left = 49
        Top = 5
        Width = 15
        Height = 21
        Associate = Edit1
        Max = 16
        TabOrder = 1
        OnChangingEx = line_widthChangingEx
      end
      object part_of_background: TCheckBox
        Left = 70
        Top = 6
        Width = 91
        Height = 17
        Caption = #1063#1072#1089#1090#1100' '#1092#1086#1085#1072
        TabOrder = 2
        OnClick = part_of_backgroundClick
      end
    end
    object Label6: TLabel
      Left = 43
      Top = 171
      Width = 47
      Height = 13
      Anchors = []
      Caption = #1058#1086#1095#1085#1086#1089#1090#1100
      ExplicitLeft = 46
      ExplicitTop = 164
    end
    object Panel2: TPanel
      Left = 132
      Top = 164
      Width = 200
      Height = 28
      Align = alClient
      Caption = 'Panel1'
      ShowCaption = False
      TabOrder = 5
      object Edit2: TEdit
        Left = 5
        Top = 3
        Width = 121
        Height = 21
        TabOrder = 0
        Text = '1'
      end
      object precision: TUpDown
        Left = 126
        Top = 3
        Width = 15
        Height = 21
        Associate = Edit2
        Max = 16
        Position = 1
        TabOrder = 1
        OnChangingEx = digitsChangingEx
      end
    end
    object Label7: TLabel
      Left = 30
      Top = 201
      Width = 73
      Height = 13
      Anchors = []
      Caption = #1050#1086#1083'-'#1074#1086' '#1079#1085#1072#1082#1086#1074
      ExplicitLeft = 31
      ExplicitTop = 220
    end
    object Panel3: TPanel
      Left = 132
      Top = 192
      Width = 200
      Height = 31
      Align = alClient
      Caption = 'Panel1'
      ShowCaption = False
      TabOrder = 6
      object Edit3: TEdit
        Left = 5
        Top = 5
        Width = 121
        Height = 21
        TabOrder = 0
        Text = '1'
      end
      object digits: TUpDown
        Left = 126
        Top = 5
        Width = 15
        Height = 21
        Associate = Edit3
        Min = 1
        Max = 16
        Position = 1
        TabOrder = 1
        OnChangingEx = digitsChangingEx
      end
    end
  end
  object ColorDialog1: TColorDialog
    Options = [cdFullOpen, cdAnyColor]
    Left = 299
    Top = 88
  end
end
