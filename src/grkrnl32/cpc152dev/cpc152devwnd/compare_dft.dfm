object CompareDFT: TCompareDFT
  Left = 0
  Top = 0
  Caption = 'CompareDFT'
  ClientHeight = 311
  ClientWidth = 643
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object ListView1: TListView
    Left = 0
    Top = 0
    Width = 643
    Height = 292
    Align = alClient
    Columns = <
      item
        Caption = 'F'
        Width = 70
      end
      item
        Caption = 'Re1'
        Width = 70
      end
      item
        Caption = 'Im1'
        Width = 70
      end
      item
        Caption = 'Arg1'
        Width = 70
      end
      item
        Caption = 'Re2'
        Width = 70
      end
      item
        Caption = 'Im2'
        Width = 70
      end
      item
        Caption = 'Arg2'
        Width = 70
      end>
    Ctl3D = True
    FlatScrollBars = True
    GridLines = True
    ReadOnly = True
    RowSelect = True
    TabOrder = 0
    ViewStyle = vsReport
    ExplicitLeft = 40
    ExplicitTop = 32
    ExplicitWidth = 250
    ExplicitHeight = 150
  end
  object StatusBar1: TStatusBar
    Left = 0
    Top = 292
    Width = 643
    Height = 19
    Panels = <>
  end
end
