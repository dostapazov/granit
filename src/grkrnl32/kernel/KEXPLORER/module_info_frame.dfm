object ModuleInfoFrame: TModuleInfoFrame
  Left = 0
  Top = 0
  Width = 443
  Height = 293
  Align = alClient
  Font.Charset = RUSSIAN_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  ParentFont = False
  TabOrder = 0
  OnResize = FrameResize
  object Panel1: TPanel
    Left = 0
    Top = -1
    Width = 443
    Height = 293
    BevelOuter = bvLowered
    TabOrder = 0
    DesignSize = (
      443
      293)
    object Label2: TLabel
      Left = 107
      Top = 9
      Width = 171
      Height = 13
      Caption = #1055#1088#1086#1094#1077#1089#1089#1086#1088#1085#1086#1077' '#1074#1088#1077#1084#1103' '#1087#1088#1086#1075#1088#1072#1084#1084#1099
    end
    object ModIcon: TImage
      Left = 8
      Top = 11
      Width = 37
      Height = 38
      AutoSize = True
      Center = True
    end
    object Label1: TLabel
      Left = 112
      Top = 40
      Width = 105
      Height = 13
      Caption = #1055#1088#1080#1086#1088#1080#1090#1077#1090' '#1087#1088#1086#1094#1077#1089#1089#1072
    end
    object ProcessPriorityClass: TLabel
      Left = 342
      Top = 40
      Width = 38
      Height = 13
      Caption = #1053#1080#1079#1082#1080#1081
    end
    object ListView1: TListView
      Left = 6
      Top = 63
      Width = 407
      Height = 226
      Anchors = [akLeft, akTop, akRight, akBottom]
      BevelInner = bvNone
      BevelKind = bkFlat
      BorderStyle = bsNone
      Columns = <
        item
          Width = 110
        end
        item
          Width = 289
        end>
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      GridLines = True
      ReadOnly = True
      RowSelect = True
      ParentFont = False
      ShowColumnHeaders = False
      TabOrder = 0
      ViewStyle = vsReport
    end
    object StaticText2: TStaticText
      Left = 283
      Top = 9
      Width = 89
      Height = 17
      Alignment = taCenter
      AutoSize = False
      BorderStyle = sbsSunken
      Caption = 'StaticText2'
      TabOrder = 1
    end
    object TrackBar1: TTrackBar
      Left = 264
      Top = 27
      Width = 72
      Height = 31
      Ctl3D = True
      Max = 3
      ParentCtl3D = False
      PageSize = 1
      TabOrder = 2
      TickMarks = tmTopLeft
      OnChange = TrackBar1Change
    end
  end
  object Timer1: TTimer
    OnTimer = Timer1Timer
    Left = 30
    Top = 227
  end
end
