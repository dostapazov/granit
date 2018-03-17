inherited MDBCurrentState: TMDBCurrentState
  Left = 287
  Top = 298
  Caption = #1058#1077#1082#1091#1097#1080#1077' '#1089#1086#1089#1090#1086#1103#1085#1080#1103
  ClientHeight = 469
  ClientWidth = 471
  OldCreateOrder = True
  OnClose = FormClose
  ExplicitWidth = 479
  ExplicitHeight = 496
  PixelsPerInch = 96
  TextHeight = 13
  object RecordsList: TListView
    Left = 0
    Top = 81
    Width = 466
    Height = 388
    Anchors = [akLeft, akTop, akRight, akBottom]
    Columns = <
      item
        Caption = 'ID'
      end
      item
        Alignment = taCenter
        Caption = #1047#1085#1072#1095#1077#1085#1080#1077
        Width = 60
      end
      item
        Alignment = taCenter
        Caption = #1044#1080#1072#1075#1085#1086#1089#1090#1080#1082#1072
        Width = 80
      end
      item
        Alignment = taCenter
        Caption = #1057#1086#1089#1090#1086#1103#1085#1080#1077
        Width = 70
      end
      item
        Alignment = taCenter
        Caption = #1042#1088#1077#1084#1103
        Width = 140
      end
      item
        Caption = #1048#1084#1103
        Width = 210
      end>
    GridLines = True
    HideSelection = False
    Items.ItemData = {}
    MultiSelect = True
    ReadOnly = True
    RowSelect = True
    TabOrder = 0
    ViewStyle = vsReport
    OnColumnClick = RecordsListColumnClick
    OnMouseDown = RecordsListMouseDown
    OnSelectItem = RecordsListSelectItem
  end
  object GroupBox1: TGroupBox
    Left = 2
    Top = 0
    Width = 467
    Height = 77
    Anchors = [akLeft, akTop, akRight]
    Caption = #1044#1086#1087#1086#1083#1085#1080#1090#1077#1083#1100#1085#1086
    TabOrder = 1
    object Label1: TLabel
      Left = 8
      Top = 16
      Width = 31
      Height = 13
      Caption = #1040#1076#1088#1077#1089
    end
    object OtdAddr: TLabel
      Left = 53
      Top = 17
      Width = 15
      Height = 13
      Caption = '-----'
    end
    object FullName: TLabel
      Left = 249
      Top = 16
      Width = 44
      Height = 13
      Caption = 'FullName'
    end
    object Label2: TLabel
      Left = 9
      Top = 51
      Width = 123
      Height = 13
      Caption = #1047#1085#1072#1095#1077#1085#1080#1077' '#1090#1077#1083#1077#1084#1077#1093#1072#1085#1080#1082#1080
    end
    object OtdValue: TLabel
      Left = 142
      Top = 52
      Width = 15
      Height = 13
      Caption = '-----'
    end
    object Label3: TLabel
      Left = 246
      Top = 54
      Width = 144
      Height = 13
      Caption = #1055#1077#1088#1089#1086#1085#1072#1083#1100#1085#1072#1103' '#1044#1080#1072#1075#1085#1086#1089#1090#1080#1082#1072
    end
    object OtdPd: TLabel
      Left = 398
      Top = 54
      Width = 15
      Height = 13
      Caption = '-----'
    end
    object Label4: TLabel
      Left = 248
      Top = 32
      Width = 90
      Height = 13
      Caption = #1056#1072#1079#1085#1080#1094#1072' '#1074#1088#1077#1084#1077#1085#1080
    end
    object TimeDiff: TLabel
      Left = 398
      Top = 32
      Width = 15
      Height = 13
      Caption = '-----'
    end
    object Label5: TLabel
      Left = 9
      Top = 32
      Width = 119
      Height = 13
      Caption = #1055#1088#1077#1076#1099#1076#1091#1097#1077#1077' '#1079#1085#1072#1095#1077#1085#1080#1077':'
    end
    object LastValue: TLabel
      Left = 142
      Top = 32
      Width = 15
      Height = 13
      Caption = '-----'
    end
    object Percent: TLabel
      Left = 456
      Top = 32
      Width = 37
      Height = 13
      Caption = 'Percent'
    end
    object MinMax: TLabel
      Left = 456
      Top = 56
      Width = 37
      Height = 13
      Caption = 'MinMax'
    end
  end
end
