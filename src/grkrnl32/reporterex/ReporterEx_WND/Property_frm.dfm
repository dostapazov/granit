object FrmProp: TFrmProp
  Left = 548
  Top = 423
  BorderStyle = bsDialog
  Caption = #1057#1074#1086#1081#1089#1090#1074#1072' '#1092#1072#1081#1083#1072' '#1089#1086#1086#1073#1097#1077#1085#1080#1081
  ClientHeight = 237
  ClientWidth = 336
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poOwnerFormCenter
  OnCloseQuery = FormCloseQuery
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 40
    Top = 104
    Width = 276
    Height = 13
    Caption = #1084#1072#1082#1089#1080#1084#1072#1083#1100#1085#1099#1081' '#1088#1072#1079#1084#1077#1088' '#1092#1072#1081#1083#1072' '#1074' '#1082#1041', 0 - '#1073#1077#1079#1088#1072#1079#1084#1077#1088#1085#1099#1081
  end
  object LName: TLabeledEdit
    Left = 120
    Top = 16
    Width = 200
    Height = 21
    Color = clBtnFace
    EditLabel.Width = 64
    EditLabel.Height = 13
    EditLabel.Caption = #1057#1086#1086#1073#1097#1077#1085#1080#1103': '
    LabelPosition = lpLeft
    ReadOnly = True
    TabOrder = 0
  end
  object LFile: TLabeledEdit
    Left = 120
    Top = 48
    Width = 200
    Height = 21
    Color = clBtnFace
    EditLabel.Width = 63
    EditLabel.Height = 13
    EditLabel.Caption = #1048#1084#1103' '#1092#1072#1081#1083#1072': '
    LabelPosition = lpLeft
    ReadOnly = True
    TabOrder = 1
  end
  object LMaxSize: TLabeledEdit
    Left = 120
    Top = 80
    Width = 200
    Height = 21
    EditLabel.Width = 80
    EditLabel.Height = 13
    EditLabel.Caption = #1056#1072#1079#1084#1077#1088' '#1092#1072#1081#1083#1072': '
    LabelPosition = lpLeft
    TabOrder = 2
  end
  object LRecs: TLabeledEdit
    Left = 120
    Top = 128
    Width = 200
    Height = 21
    Color = clBtnFace
    EditLabel.Width = 83
    EditLabel.Height = 13
    EditLabel.Caption = #1063#1080#1089#1083#1086' '#1079#1072#1087#1080#1089#1077#1081': '
    LabelPosition = lpLeft
    ReadOnly = True
    TabOrder = 3
  end
  object LSize: TLabeledEdit
    Left = 120
    Top = 160
    Width = 200
    Height = 21
    Color = clBtnFace
    EditLabel.Width = 105
    EditLabel.Height = 13
    EditLabel.Caption = #1047#1072#1085#1080#1084#1072#1077#1084#1086#1077' '#1084#1077#1089#1090#1086': '
    LabelPosition = lpLeft
    ReadOnly = True
    TabOrder = 4
  end
  object BitBtn1: TBitBtn
    Left = 71
    Top = 200
    Width = 75
    Height = 25
    Kind = bkOK
    NumGlyphs = 2
    TabOrder = 5
  end
  object BitBtn2: TBitBtn
    Left = 191
    Top = 200
    Width = 75
    Height = 25
    Kind = bkCancel
    NumGlyphs = 2
    TabOrder = 6
  end
end
