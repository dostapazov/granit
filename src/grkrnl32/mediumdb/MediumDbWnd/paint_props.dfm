object PaintProps: TPaintProps
  Left = 0
  Top = 0
  Caption = 'PaintProps'
  ClientHeight = 325
  ClientWidth = 396
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnDeactivate = FormDeactivate
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 8
    Top = 8
    Width = 68
    Height = 13
    Caption = #1056#1080#1089#1086#1074#1072#1090#1100' '#1082#1072#1082
  end
  object ComboBox1: TComboBox
    Left = 82
    Top = 6
    Width = 215
    Height = 21
    Style = csDropDownList
    TabOrder = 0
  end
  object BcColor: TColorBox
    Left = 81
    Top = 33
    Width = 87
    Height = 22
    Style = [cbStandardColors, cbExtendedColors, cbSystemColors, cbCustomColor, cbCustomColors]
    TabOrder = 1
  end
  object StaticText1: TStaticText
    Left = 16
    Top = 40
    Width = 59
    Height = 17
    Caption = #1062#1074#1077#1090' '#1092#1086#1085#1072
    TabOrder = 2
  end
  object StaticText2: TStaticText
    Left = 184
    Top = 40
    Width = 59
    Height = 17
    Caption = #1062#1074#1077#1090' '#1092#1086#1085#1072
    TabOrder = 3
  end
  object ColorBox1: TColorBox
    Left = 249
    Top = 35
    Width = 87
    Height = 22
    Selected = clMedGray
    Style = [cbCustomColor, cbCustomColors]
    TabOrder = 4
  end
end
