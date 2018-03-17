inherited SCBaseDataFrame: TSCBaseDataFrame
  Constraints.MinHeight = 293
  Constraints.MinWidth = 442
  DesignSize = (
    443
    293)
  object Label4: TLabel
    Left = 4
    Top = 71
    Width = 54
    Height = 13
    Caption = #1057#1086#1089#1090#1086#1103#1085#1080#1077
  end
  object GroupBox1: TGroupBox
    Left = 0
    Top = 0
    Width = 443
    Height = 93
    Align = alTop
    Caption = #1044#1080#1072#1075#1085#1086#1089#1090#1080#1082#1072
    TabOrder = 0
    DesignSize = (
      443
      93)
    object Label1: TLabel
      Left = 4
      Top = 16
      Width = 37
      Height = 13
      Caption = #1055#1086#1083#1085#1072#1103
    end
    object Label2: TLabel
      Left = 4
      Top = 34
      Width = 54
      Height = 13
      Caption = #1063#1072#1089#1090#1080#1095#1085#1072#1103
    end
    object Label3: TLabel
      Left = 4
      Top = 51
      Width = 54
      Height = 13
      Caption = #1057#1086#1089#1090#1086#1103#1085#1080#1077
    end
    object Label5: TLabel
      Left = 4
      Top = 71
      Width = 86
      Height = 13
      Caption = #1042#1088#1077#1084#1103' '#1080#1079#1084#1077#1085#1077#1085#1080#1103
    end
    object FullDiagText: TStaticText
      Left = 100
      Top = 9
      Width = 340
      Height = 17
      Alignment = taCenter
      Anchors = [akLeft, akTop, akRight]
      AutoSize = False
      BorderStyle = sbsSunken
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clGreen
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 0
      ExplicitWidth = 339
    end
    object PartDiagText: TStaticText
      Left = 100
      Top = 29
      Width = 340
      Height = 17
      Alignment = taCenter
      Anchors = [akLeft, akTop, akRight]
      AutoSize = False
      BorderStyle = sbsSunken
      Font.Charset = DEFAULT_CHARSET
      Font.Color = 3643311
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 1
      ExplicitWidth = 339
    end
    object StateText: TStaticText
      Left = 100
      Top = 49
      Width = 340
      Height = 17
      Alignment = taCenter
      Anchors = [akLeft, akTop, akRight]
      AutoSize = False
      BorderStyle = sbsSunken
      TabOrder = 2
      ExplicitWidth = 339
    end
  end
  object TimeStampText: TStaticText
    Left = 100
    Top = 69
    Width = 340
    Height = 17
    Alignment = taCenter
    Anchors = [akLeft, akTop, akRight]
    AutoSize = False
    BorderStyle = sbsSunken
    TabOrder = 1
    ExplicitWidth = 339
  end
  object DynTimer: TTimer
    Enabled = False
    OnTimer = DynTimerTimer
    Left = 408
    Top = 56
  end
end
