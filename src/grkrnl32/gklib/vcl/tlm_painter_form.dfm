object TlmPainterForm: TTlmPainterForm
  Left = 0
  Top = 0
  Caption = 'TlmPainterForm'
  ClientHeight = 210
  ClientWidth = 415
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = [fsBold]
  KeyPreview = True
  OldCreateOrder = False
  OnKeyDown = FormKeyDown
  OnResize = FormResize
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object paint_area: TPaintBox
    Left = 0
    Top = 24
    Width = 415
    Height = 167
    Align = alClient
    PopupMenu = kadr_popup
    OnMouseDown = paint_areaMouseDown
    OnMouseMove = paint_areaMouseMove
    OnMouseUp = paint_areaMouseUp
    OnPaint = paint_areaPaint
    ExplicitTop = 35
    ExplicitWidth = 337
    ExplicitHeight = 278
  end
  object ToolBar1: TToolBar
    Left = 0
    Top = 0
    Width = 415
    Height = 24
    Caption = 'ToolBar1'
    Color = clBtnFace
    DrawingStyle = dsGradient
    EdgeBorders = [ebBottom]
    GradientStartColor = clBtnFace
    Images = TbImageList
    List = True
    GradientDirection = gdHorizontal
    ParentColor = False
    ParentShowHint = False
    AllowTextButtons = True
    ShowHint = True
    TabOrder = 0
  end
  object StatusBar1: TStatusBar
    Left = 0
    Top = 191
    Width = 415
    Height = 19
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clBtnText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    Panels = <
      item
        Width = 50
      end>
    UseSystemFont = False
    StyleElements = [seClient, seBorder]
  end
  object TbImageList: TImageList
    Left = 24
    Top = 40
  end
  object kadr_popup: TPopupMenu
    Images = TbImageList
    Left = 97
    Top = 40
    object miSelGroup: TMenuItem
      Caption = #1042#1099#1076#1077#1083#1077#1085#1080#1077
      object miSelectAll: TMenuItem
        Caption = #1042#1099#1076#1077#1083#1080#1090#1100' '#1074#1089#1077
        ShortCut = 16449
        OnClick = miSelectAllClick
      end
      object miInvertSelect: TMenuItem
        Tag = 1
        Caption = #1054#1073#1088#1072#1090#1080#1090#1100' '#1074#1099#1076#1077#1083#1077#1085#1080#1077
        ShortCut = 8257
        OnClick = miSelectAllClick
      end
      object miSelectDiscrete: TMenuItem
        Caption = #1042#1099#1076#1077#1083#1080#1090#1100' '#1076#1080#1089#1082#1088#1077#1090#1085#1099#1077
        ShortCut = 16452
        OnClick = miSelectDiscreteClick
      end
      object miSelectDiscreteInvert: TMenuItem
        Tag = 1
        Caption = #1054#1073#1088#1072#1090#1080#1090#1100' '#1074#1099#1076#1077#1083#1077#1085#1080#1077' '#1076#1080#1089#1082#1088#1077#1090#1085#1099#1093
        ShortCut = 8260
        OnClick = miSelectDiscreteClick
      end
      object miSelectAnalogs: TMenuItem
        Caption = #1042#1099#1076#1077#1083#1080#1090#1100' '#1072#1085#1072#1083#1086#1075#1086#1074#1099#1077
        ShortCut = 16457
        OnClick = miSelectAnalogsClick
      end
      object miSelectAnalogsInvert: TMenuItem
        Tag = 1
        Caption = #1054#1073#1088#1072#1090#1080#1090#1100' '#1074#1099#1076#1077#1083#1077#1085#1080#1077' '#1072#1085#1072#1083#1086#1075#1086#1074#1099#1093
        ShortCut = 8265
        OnClick = miSelectAnalogsClick
      end
    end
  end
end
