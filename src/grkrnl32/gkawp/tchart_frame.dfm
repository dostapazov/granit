object TChartFrame: TTChartFrame
  Left = 0
  Top = 0
  Width = 666
  Height = 360
  TabOrder = 0
  object Chart1: TChart
    Left = 0
    Top = 0
    Width = 666
    Height = 360
    Title.Text.Strings = (
      'TChart')
    Title.Visible = False
    SeriesGroups = <
      item
        Name = 'discrete_group'
      end
      item
        Name = 'analog_group'
      end>
    View3D = False
    Align = alClient
    TabOrder = 0
    DefaultCanvas = 'TGDIPlusCanvas'
    ColorPaletteIndex = 13
  end
end
