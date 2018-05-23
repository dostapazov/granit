inherited SecurityWnd: TSecurityWnd
  Caption = ''
  ClientHeight = 351
  ClientWidth = 518
  ExplicitWidth = 526
  ExplicitHeight = 378
  PixelsPerInch = 96
  TextHeight = 13
  object StatusBar1: TStatusBar
    Left = 0
    Top = 332
    Width = 518
    Height = 19
    Panels = <>
  end
  object ToolBar1: TToolBar
    Left = 0
    Top = 0
    Width = 518
    Height = 29
    Caption = 'ToolBar1'
    EdgeBorders = [ebBottom]
    TabOrder = 1
  end
  object PageControl1: TPageControl
    Left = 0
    Top = 29
    Width = 518
    Height = 303
    ActivePage = users_sheet
    Align = alClient
    TabOrder = 2
    object users_sheet: TTabSheet
      Caption = #1055#1086#1083#1100#1079#1086#1074#1072#1090#1077#1083#1080
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object Splitter1: TSplitter
        Left = 161
        Top = 0
        Width = 4
        Height = 275
      end
      object users_tree: TTreeView
        Left = 0
        Top = 0
        Width = 161
        Height = 275
        Align = alLeft
        Indent = 19
        TabOrder = 0
      end
    end
  end
end
