inherited Cpc152SignalSpectr: TCpc152SignalSpectr
  Caption = 'Cpc152SignalSpectr'
  ClientHeight = 544
  ClientWidth = 903
  OnClose = FormClose
  ExplicitWidth = 919
  ExplicitHeight = 583
  PixelsPerInch = 96
  TextHeight = 13
  object Splitter1: TSplitter
    Left = 0
    Top = 232
    Width = 903
    Height = 3
    Cursor = crVSplit
    Align = alTop
    ExplicitTop = 209
    ExplicitWidth = 237
  end
  object StatusBar1: TStatusBar
    Left = 0
    Top = 525
    Width = 903
    Height = 19
    Panels = <>
  end
  object Panel3: TPanel
    Left = 0
    Top = 52
    Width = 903
    Height = 180
    Align = alTop
    Caption = 'Panel3'
    TabOrder = 1
    object SignalChart: TChart
      Left = 1
      Top = 1
      Width = 901
      Height = 178
      Title.Text.Strings = (
        'TChart')
      BottomAxis.LogarithmicBase = 2.718281828459050000
      Panning.MouseWheel = pmwNone
      View3D = False
      Zoom.MouseWheel = pmwNormal
      Align = alClient
      TabOrder = 0
      DefaultCanvas = 'TGDIPlusCanvas'
      PrintMargins = (
        15
        40
        15
        40)
      ColorPaletteIndex = 13
    end
  end
  object Panel1: TPanel
    Left = 0
    Top = 235
    Width = 903
    Height = 290
    Align = alClient
    TabOrder = 2
    object Splitter2: TSplitter
      Left = 663
      Top = 1
      Height = 288
      ExplicitLeft = 736
      ExplicitTop = 80
      ExplicitHeight = 100
    end
    object SpectrChart: TChart
      Left = 1
      Top = 1
      Width = 662
      Height = 288
      Legend.Visible = False
      Title.Text.Strings = (
        'TChart')
      View3D = False
      Align = alLeft
      TabOrder = 0
      DefaultCanvas = 'TGDIPlusCanvas'
      ColorPaletteIndex = 13
      object SpectrSeries: TBarSeries
        Marks.Visible = False
        Marks.Arrow.Visible = False
        Marks.Callout.Arrow.Visible = False
        XValues.Name = 'X'
        XValues.Order = loAscending
        YValues.Name = 'Bar'
        YValues.Order = loNone
      end
    end
    object PhaseChart: TChart
      Left = 666
      Top = 1
      Width = 236
      Height = 288
      Legend.Visible = False
      Title.Text.Strings = (
        'TChart')
      BottomAxis.Automatic = False
      BottomAxis.AutomaticMaximum = False
      BottomAxis.AutomaticMinimum = False
      BottomAxis.Grid.DrawAlways = True
      BottomAxis.GridCentered = True
      BottomAxis.Maximum = 1.000000000000000000
      BottomAxis.Minimum = -1.000000000000000000
      BottomAxis.MinorTickCount = 5
      BottomAxis.TickLength = 0
      LeftAxis.Automatic = False
      LeftAxis.AutomaticMaximum = False
      LeftAxis.AutomaticMinimum = False
      LeftAxis.Maximum = 1.000000000000000000
      LeftAxis.Minimum = -1.000000000000000000
      LeftAxis.Items = {
        0B0000000104546578740601310556616C7565050000000000000080FF3F0001
        04546578740603302C380556616C75650500D0CCCCCCCCCCCCFE3F0001045465
        78740603302C360556616C75650500A0999999999999FE3F0001045465787406
        03302C340556616C75650500D8CCCCCCCCCCCCFD3F000104546578740603302C
        320556616C75650500E0CCCCCCCCCCCCFC3F000104546578740601300556616C
        7565050000000000000080C93F0001045465787406042D302C320556616C7565
        0500C0CCCCCCCCCCCCFCBF0001045465787406042D302C340556616C75650500
        C8CCCCCCCCCCCCFDBF0001045465787406042D302C360556616C756505009899
        9999999999FEBF0001045465787406042D302C380556616C75650500D0CCCCCC
        CCCCCCFEBF0001045465787406022D310556616C7565050000000000000080FF
        BF00}
      View3D = False
      Align = alClient
      TabOrder = 1
      DefaultCanvas = 'TGDIPlusCanvas'
      ColorPaletteIndex = 13
      object PhaseSeries: TFastLineSeries
        LinePen.Color = 10708548
        XValues.Name = 'X'
        XValues.Order = loAscending
        YValues.Name = 'Y'
        YValues.Order = loNone
      end
    end
  end
  object Panel2: TPanel
    Left = 0
    Top = 0
    Width = 903
    Height = 52
    Align = alTop
    TabOrder = 3
    object Label1: TLabel
      Left = 180
      Top = 7
      Width = 49
      Height = 13
      Caption = #1087#1072#1088#1072#1084#1077#1090#1088
    end
    object SpeedButton1: TSpeedButton
      Left = 279
      Top = 3
      Width = 23
      Height = 22
      Caption = '+'
      OnClick = SpeedButton1Click
    end
    object SpeedButton2: TSpeedButton
      Left = 305
      Top = 3
      Width = 23
      Height = 22
      Caption = '-'
      OnClick = SpeedButton2Click
    end
    object Label2: TLabel
      Left = 8
      Top = 7
      Width = 60
      Height = 13
      Caption = #1059#1089#1090#1088#1086#1081#1089#1090#1074#1086
    end
    object Label3: TLabel
      Left = 104
      Top = 7
      Width = 35
      Height = 13
      Caption = #1043#1088#1091#1087#1087#1072
    end
    object Label4: TLabel
      Left = 334
      Top = 7
      Width = 47
      Height = 13
      Caption = #1054#1090#1089#1095#1077#1090#1086#1074
    end
    object Label5: TLabel
      Left = 692
      Top = 7
      Width = 63
      Height = 13
      Caption = #1056#1072#1079#1084#1077#1088' DFT'
    end
    object SpeedButton3: TSpeedButton
      Left = 8
      Top = 27
      Width = 61
      Height = 22
      Caption = #1054#1095#1080#1089#1090#1080#1090#1100
      OnClick = SpeedButton3Click
    end
    object SpeedButton4: TSpeedButton
      Left = 834
      Top = 4
      Width = 39
      Height = 22
      Caption = 'DFT!'
      OnClick = SpeedButton4Click
    end
    object Label6: TLabel
      Left = 779
      Top = 31
      Width = 61
      Height = 13
      Caption = #1055#1086#1089#1090#1086#1103#1085#1085#1072#1103
    end
    object TmLen: TLabel
      Left = 219
      Top = 32
      Width = 26
      Height = 13
      Caption = 'Time:'
    end
    object Label7: TLabel
      Left = 346
      Top = 33
      Width = 94
      Height = 13
      Caption = #1042#1099#1074#1086#1076#1080#1090#1100' '#1095#1072#1089#1090#1086#1090#1099
    end
    object RMS: TLabel
      Left = 536
      Top = 33
      Width = 26
      Height = 13
      Caption = 'Time:'
    end
    object DPhase: TLabel
      Left = 663
      Top = 33
      Width = 41
      Height = 13
      Caption = 'Resampl'
    end
    object DevNum: TEdit
      Left = 74
      Top = 5
      Width = 23
      Height = 21
      TabOrder = 0
      Text = '0'
    end
    object GroupNum: TEdit
      Left = 145
      Top = 4
      Width = 32
      Height = 21
      TabOrder = 1
      Text = '0'
    end
    object scan_count: TEdit
      Left = 387
      Top = 4
      Width = 49
      Height = 21
      TabOrder = 2
      Text = '50'
      OnChange = scan_countChange
    end
    object DftFrame: TEdit
      Left = 761
      Top = 4
      Width = 49
      Height = 21
      TabOrder = 3
      Text = '1024'
    end
    object Pause: TCheckBox
      Left = 76
      Top = 31
      Width = 56
      Height = 17
      Caption = #1055#1072#1091#1079#1072
      TabOrder = 4
    end
    object ParamNum: TComboBox
      Left = 233
      Top = 4
      Width = 41
      Height = 21
      Style = csDropDownList
      TabOrder = 5
    end
    object DftExp: TUpDown
      Left = 812
      Top = 3
      Width = 16
      Height = 24
      Min = 8
      Max = 16
      Position = 10
      TabOrder = 6
      OnChangingEx = DftExpChangingEx
    end
    object Const: TEdit
      Left = 848
      Top = 28
      Width = 49
      Height = 21
      TabOrder = 7
    end
    object DftWndType: TComboBox
      Left = 568
      Top = 0
      Width = 145
      Height = 21
      Style = csDropDownList
      ItemIndex = 0
      TabOrder = 8
      Text = #1055#1088#1103#1084#1086#1091#1075#1086#1083#1100#1085#1086#1077
      Items.Strings = (
        #1055#1088#1103#1084#1086#1091#1075#1086#1083#1100#1085#1086#1077
        'Gauss'
        'Hamming'
        'Hann'
        'BlackmanHarris')
    end
    object AutoDFT: TCheckBox
      Left = 130
      Top = 32
      Width = 83
      Height = 17
      Caption = #1047#1072#1087#1091#1089#1082' DFT'
      TabOrder = 9
    end
    object FrqBeg: TEdit
      Left = 446
      Top = 27
      Width = 31
      Height = 21
      TabOrder = 10
      Text = '40'
    end
    object FrqEnd: TEdit
      Left = 482
      Top = 27
      Width = 32
      Height = 21
      TabOrder = 11
      Text = '60'
    end
    object SampleFreq: TEdit
      Left = 456
      Top = 5
      Width = 57
      Height = 21
      TabOrder = 12
      Text = '500'
    end
    object Resampl: TEdit
      Left = 712
      Top = 29
      Width = 43
      Height = 21
      TabOrder = 13
      Text = '0'
    end
  end
  object TeeGDIPlus1: TTeeGDIPlus
    Active = True
    Left = 224
    Top = 64
  end
end
