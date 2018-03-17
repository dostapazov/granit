object TlmPainterFrom: TTlmPainterFrom
  Left = 0
  Top = 0
  Caption = 'TlmPainterFrom'
  ClientHeight = 546
  ClientWidth = 794
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
  object PaintBox1: TPaintBox
    Left = 0
    Top = 29
    Width = 794
    Height = 498
    Align = alClient
    OnMouseDown = PaintBox1MouseDown
    OnMouseMove = PaintBox1MouseMove
    OnMouseUp = PaintBox1MouseUp
    OnPaint = PaintBox1Paint
    ExplicitTop = 35
    ExplicitWidth = 337
    ExplicitHeight = 278
  end
  object ToolBar1: TToolBar
    Left = 0
    Top = 0
    Width = 794
    Height = 29
    Caption = 'ToolBar1'
    Color = clBtnFace
    DrawingStyle = dsGradient
    EdgeBorders = [ebBottom]
    GradientStartColor = clBtnFace
    Images = TbImageList
    List = True
    GradientDirection = gdHorizontal
    ParentColor = False
    AllowTextButtons = True
    TabOrder = 0
  end
  object StatusBar1: TStatusBar
    Left = 0
    Top = 527
    Width = 794
    Height = 19
    Panels = <
      item
        Width = 50
      end>
  end
  object TbImageList: TImageList
    Left = 712
    Top = 88
  end
end
