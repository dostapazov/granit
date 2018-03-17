object AboutForm: TAboutForm
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = #1054' '#1084#1086#1076#1091#1083#1077' "'#1055#1088#1086#1089#1084#1086#1090#1088' '#1082#1072#1076#1088#1086#1074' MediumDB "'
  ClientHeight = 234
  ClientWidth = 438
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnDeactivate = FormDeactivate
  DesignSize = (
    438
    234)
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 8
    Top = 79
    Width = 4
    Height = 14
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object InfoText: TLabel
    Left = 20
    Top = 248
    Width = 9
    Height = 13
    Caption = '   '
  end
  object BitBtn1: TBitBtn
    Left = 185
    Top = 199
    Width = 105
    Height = 25
    Anchors = [akLeft, akBottom]
    Kind = bkOK
    NumGlyphs = 2
    TabOrder = 0
    OnClick = BitBtn1Click
  end
  object Memo1: TMemo
    Left = 0
    Top = 99
    Width = 437
    Height = 73
    Alignment = taCenter
    Anchors = [akLeft, akTop, akBottom]
    Color = clBtnFace
    Lines.Strings = (
      #1055#1088#1086#1089#1084#1086#1090#1088' '#1086#1087#1088#1077#1072#1090#1080#1074#1085#1099#1093' '#1080' '#1072#1088#1093#1080#1074#1085#1099#1093' '#1076#1072#1085#1085#1099#1093
      #1042#1077#1088#1089#1080#1103' 1.0 2006-2016'#39)
    TabOrder = 1
  end
  inline AboutCommonFrame1: TAboutCommonFrame
    Left = 0
    Top = 0
    Width = 438
    Height = 92
    Align = alTop
    TabOrder = 2
    ExplicitWidth = 438
    ExplicitHeight = 92
    inherited Memo2: TMemo
      Width = 438
      Height = 92
      ExplicitWidth = 438
      ExplicitHeight = 92
    end
  end
end
