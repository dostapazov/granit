inherited ModemmerFrm: TModemmerFrm
  Left = 142
  Top = 221
  Caption = ''
  ClientHeight = 456
  ClientWidth = 625
  Constraints.MinHeight = 472
  Constraints.MinWidth = 636
  Menu = MainMenu1
  OldCreateOrder = True
  OnClose = FormClose
  OnDestroy = FormDestroy
  OnHide = FormHide
  OnResize = FormResize
  OnShow = FormShow
  ExplicitWidth = 641
  ExplicitHeight = 514
  PixelsPerInch = 96
  TextHeight = 13
  object Splitter1: TSplitter
    Left = 184
    Top = 29
    Width = 4
    Height = 406
    OnMoved = Splitter1Moved
    ExplicitHeight = 376
  end
  object ModemsTree: TTreeView
    Left = 0
    Top = 29
    Width = 184
    Height = 406
    Align = alLeft
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    HideSelection = False
    Images = ImageList1
    Indent = 19
    ParentFont = False
    PopupMenu = PopupMenu1
    ReadOnly = True
    TabOrder = 0
    OnChange = ModemsTreeChange
    OnDblClick = ModemsTreeDblClick
    OnDeletion = ModemsTreeDeletion
    OnKeyPress = ModemsTreeKeyPress
    ExplicitHeight = 364
  end
  object PageControl1: TPageControl
    Left = 188
    Top = 29
    Width = 437
    Height = 406
    ActivePage = TabSheet1
    Align = alClient
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    TabOrder = 1
    OnChange = PageControl1Change
    ExplicitWidth = 452
    ExplicitHeight = 364
    object TabSheet1: TTabSheet
      Caption = #1054#1073#1097#1080#1081
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 444
      ExplicitHeight = 336
      DesignSize = (
        429
        378)
      object ModemBox: TGroupBox
        Left = 0
        Top = 0
        Width = 427
        Height = 36
        Anchors = [akLeft, akTop, akRight]
        Caption = #1048#1085#1092#1086#1088#1084#1072#1094#1080#1103' '#1086' '#1084#1086#1076#1077#1084#1077
        TabOrder = 0
        Visible = False
        ExplicitWidth = 442
        object Label2: TLabel
          Left = 132
          Top = 14
          Width = 39
          Height = 13
          Caption = #1087#1088#1077#1076#1077#1083':'
        end
        object ModemMaxLinesCount: TLabel
          Left = 177
          Top = 14
          Width = 22
          Height = 13
          Alignment = taCenter
          AutoSize = False
          Caption = '255'
          Layout = tlCenter
        end
        object Label3: TLabel
          Left = 5
          Top = 14
          Width = 95
          Height = 13
          Caption = #1050#1086#1083#1080#1095#1077#1089#1090#1074#1086' '#1083#1080#1085#1080#1081':'
        end
        object ModemLinesCount: TLabel
          Left = 102
          Top = 14
          Width = 22
          Height = 13
          Alignment = taCenter
          AutoSize = False
          Caption = '255'
          Layout = tlCenter
        end
        object Label33: TLabel
          Left = 214
          Top = 14
          Width = 44
          Height = 13
          Caption = #1044#1072#1085#1085#1099#1077':'
        end
        object ModemFlags: TLabel
          Left = 266
          Top = 14
          Width = 60
          Height = 13
          Caption = 'ModemFlags'
        end
      end
      object LineBox: TGroupBox
        Left = 0
        Top = 36
        Width = 428
        Height = 341
        Anchors = [akLeft, akTop, akRight, akBottom]
        Caption = #1057#1090#1072#1090#1080#1089#1090#1080#1082#1072' '#1083#1080#1085#1080#1080
        ParentShowHint = False
        ShowHint = False
        TabOrder = 1
        Visible = False
        ExplicitWidth = 443
        ExplicitHeight = 299
        object Label4: TLabel
          Left = 11
          Top = 14
          Width = 79
          Height = 13
          Caption = #1054#1073#1088#1099#1074#1086#1074' '#1089#1074#1103#1079#1080
        end
        object Label20: TLabel
          Left = 177
          Top = 15
          Width = 106
          Height = 13
          Caption = #1042#1088#1077#1084#1103' '#1088#1072#1073#1086#1090#1099' '#1083#1080#1085#1080#1080
        end
        object Label34: TLabel
          Left = 8
          Top = 307
          Width = 153
          Height = 13
          Caption = #1052#1072#1082#1089#1080#1084#1072#1083#1100#1085#1099#1081' '#1088#1072#1079#1084#1077#1088' '#1082#1072#1076#1088#1072
        end
        object LostConnection: TStaticText
          Left = 99
          Top = 14
          Width = 58
          Height = 17
          Alignment = taCenter
          AutoSize = False
          BevelInner = bvNone
          BevelOuter = bvNone
          BorderStyle = sbsSunken
          Caption = 'LostConnection'
          TabOrder = 0
        end
        object GroupBox1: TGroupBox
          Left = 3
          Top = 30
          Width = 410
          Height = 124
          Caption = #1055#1088#1080#1105#1084
          TabOrder = 1
          object Label15: TLabel
            Left = 2
            Top = 14
            Width = 50
            Height = 13
            Caption = #1050#1072#1095#1077#1089#1090#1074#1086' '
          end
          object Label8: TLabel
            Left = 170
            Top = 16
            Width = 85
            Height = 13
            Caption = #1057#1082#1086#1088#1086#1089#1090#1100' ('#1073#1080#1090'/c)'
          end
          object Label7: TLabel
            Left = 2
            Top = 35
            Width = 40
            Height = 13
            Caption = #1054#1096#1080#1073#1086#1082
          end
          object Label9: TLabel
            Left = 170
            Top = 34
            Width = 86
            Height = 13
            Caption = #1057#1088#1077#1076#1085#1103#1103' ('#1073#1072#1081#1090'/'#1089')'
          end
          object Label5: TLabel
            Left = 2
            Top = 60
            Width = 30
            Height = 13
            Caption = #1041#1072#1081#1090': '
          end
          object Label6: TLabel
            Left = 2
            Top = 78
            Width = 37
            Height = 13
            Caption = #1050#1072#1076#1088#1086#1074
          end
          object Label18: TLabel
            Left = 170
            Top = 55
            Width = 120
            Height = 13
            Caption = #1052#1072#1082#1089#1080#1084#1072#1083#1100#1085#1072#1103' ('#1073#1072#1081#1090'/'#1089')'
          end
          object Label19: TLabel
            Left = 170
            Top = 79
            Width = 114
            Height = 13
            Caption = #1052#1080#1085#1080#1084#1072#1083#1100#1085#1072#1103' ('#1073#1072#1081#1090'/'#1089')'
          end
          object Label17: TLabel
            Left = 2
            Top = 101
            Width = 161
            Height = 13
            Caption = #1052#1086#1076#1077#1084#1084#1077#1088' '#1087#1086#1083#1091#1095#1080#1083' '#1082#1072#1076#1088#1086#1074
            Font.Charset = RUSSIAN_CHARSET
            Font.Color = clGreen
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object Label26: TLabel
            Left = 272
            Top = 101
            Width = 28
            Height = 13
            Caption = #1073#1072#1081#1090
            Font.Charset = RUSSIAN_CHARSET
            Font.Color = clGreen
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object RxQuality: TStaticText
            Left = 58
            Top = 12
            Width = 100
            Height = 17
            Alignment = taCenter
            AutoSize = False
            BevelInner = bvNone
            BevelOuter = bvNone
            BorderStyle = sbsSunken
            Caption = 'RxQuality'
            TabOrder = 0
          end
          object RxSpeed: TStaticText
            Left = 300
            Top = 15
            Width = 100
            Height = 17
            Alignment = taCenter
            AutoSize = False
            BevelInner = bvNone
            BevelOuter = bvNone
            BorderStyle = sbsSunken
            Caption = 'RxSpeed'
            TabOrder = 1
          end
          object RxErrorsCount: TStaticText
            Left = 58
            Top = 33
            Width = 100
            Height = 18
            Alignment = taCenter
            AutoSize = False
            BevelInner = bvNone
            BevelOuter = bvNone
            BorderStyle = sbsSunken
            Caption = 'RxErrorsCount'
            ParentShowHint = False
            ShowHint = True
            TabOrder = 2
          end
          object RxSpeedMiddle: TStaticText
            Left = 300
            Top = 34
            Width = 100
            Height = 18
            Alignment = taCenter
            AutoSize = False
            BevelInner = bvNone
            BevelOuter = bvNone
            BorderStyle = sbsSunken
            Caption = 'RxSpeed'
            TabOrder = 3
          end
          object RxBytesCount: TStaticText
            Left = 58
            Top = 57
            Width = 100
            Height = 17
            Alignment = taCenter
            AutoSize = False
            BevelInner = bvNone
            BevelOuter = bvNone
            BorderStyle = sbsSunken
            Caption = 'RxBytesCount'
            ParentShowHint = False
            ShowHint = True
            TabOrder = 4
          end
          object RxKadrsCount: TStaticText
            Left = 58
            Top = 79
            Width = 100
            Height = 18
            Alignment = taCenter
            AutoSize = False
            BevelInner = bvNone
            BevelOuter = bvNone
            BorderStyle = sbsSunken
            Caption = 'RxKadrsCount'
            ParentShowHint = False
            ShowHint = True
            TabOrder = 5
          end
          object RxSpeedMax: TStaticText
            Left = 300
            Top = 55
            Width = 100
            Height = 17
            Alignment = taCenter
            AutoSize = False
            BevelInner = bvNone
            BevelOuter = bvNone
            BorderStyle = sbsSunken
            Caption = 'RxSpeedMax'
            TabOrder = 6
          end
          object RxSpeedMin: TStaticText
            Left = 300
            Top = 78
            Width = 100
            Height = 17
            Alignment = taCenter
            AutoSize = False
            BevelInner = bvNone
            BevelOuter = bvNone
            BorderStyle = sbsSunken
            Caption = 'RxSpeedMax'
            TabOrder = 7
          end
          object ModemmerRxKadrs: TStaticText
            Left = 170
            Top = 100
            Width = 100
            Height = 18
            Alignment = taCenter
            AutoSize = False
            BevelInner = bvNone
            BevelOuter = bvNone
            BorderStyle = sbsSunken
            Caption = 'RxKadrsCount'
            Font.Charset = RUSSIAN_CHARSET
            Font.Color = clGreen
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = [fsBold]
            ParentFont = False
            ParentShowHint = False
            ShowHint = True
            TabOrder = 8
          end
          object ModemmerRxBytes: TStaticText
            Left = 300
            Top = 100
            Width = 100
            Height = 18
            Alignment = taCenter
            AutoSize = False
            BevelInner = bvNone
            BevelOuter = bvNone
            BorderStyle = sbsSunken
            Caption = 'RxKadrsCount'
            Font.Charset = RUSSIAN_CHARSET
            Font.Color = clGreen
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = [fsBold]
            ParentFont = False
            ParentShowHint = False
            ShowHint = True
            TabOrder = 9
          end
        end
        object GroupBox2: TGroupBox
          Left = 4
          Top = 154
          Width = 409
          Height = 149
          Caption = #1055#1077#1088#1077#1076#1072#1095#1072
          TabOrder = 2
          object Label10: TLabel
            Left = 2
            Top = 14
            Width = 50
            Height = 13
            Caption = #1050#1072#1095#1077#1089#1090#1074#1086' '
          end
          object Label11: TLabel
            Left = 2
            Top = 35
            Width = 40
            Height = 13
            Caption = #1054#1096#1080#1073#1086#1082
          end
          object Label12: TLabel
            Left = 2
            Top = 60
            Width = 30
            Height = 13
            Caption = #1041#1072#1081#1090': '
          end
          object Label13: TLabel
            Left = 2
            Top = 78
            Width = 37
            Height = 13
            Caption = #1050#1072#1076#1088#1086#1074
          end
          object Label14: TLabel
            Left = 170
            Top = 16
            Width = 85
            Height = 13
            Caption = #1057#1082#1086#1088#1086#1089#1090#1100' ('#1073#1080#1090'/c)'
          end
          object Label16: TLabel
            Left = 170
            Top = 34
            Width = 86
            Height = 13
            Caption = #1057#1088#1077#1076#1085#1103#1103' ('#1073#1072#1081#1090'/'#1089')'
          end
          object Label21: TLabel
            Left = 170
            Top = 55
            Width = 120
            Height = 13
            Caption = #1052#1072#1082#1089#1080#1084#1072#1083#1100#1085#1072#1103' ('#1073#1072#1081#1090'/'#1089')'
          end
          object Label22: TLabel
            Left = 170
            Top = 79
            Width = 114
            Height = 13
            Caption = #1052#1080#1085#1080#1084#1072#1083#1100#1085#1072#1103' ('#1073#1072#1081#1090'/'#1089')'
          end
          object Label27: TLabel
            Left = 2
            Top = 124
            Width = 169
            Height = 13
            Caption = #1052#1086#1076#1077#1084#1084#1077#1088' '#1086#1090#1087#1088#1072#1074#1080#1083' '#1082#1072#1076#1088#1086#1074
            Font.Charset = RUSSIAN_CHARSET
            Font.Color = clGreen
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object Label28: TLabel
            Left = 272
            Top = 124
            Width = 28
            Height = 13
            Caption = #1073#1072#1081#1090
            Font.Charset = RUSSIAN_CHARSET
            Font.Color = clGreen
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object Label29: TLabel
            Left = 5
            Top = 100
            Width = 159
            Height = 13
            Caption = #1056#1072#1079#1084#1077#1088'  '#1086#1095#1077#1088#1077#1076#1080'   '#1087#1077#1088#1077#1076#1072#1090#1095#1080#1082#1072
          end
          object TxQuality: TStaticText
            Left = 58
            Top = 12
            Width = 100
            Height = 17
            Alignment = taCenter
            AutoSize = False
            BevelInner = bvNone
            BevelOuter = bvNone
            BorderStyle = sbsSunken
            Caption = 'TxQuality'
            TabOrder = 0
          end
          object TxErrorsCount: TStaticText
            Left = 58
            Top = 33
            Width = 100
            Height = 18
            Alignment = taCenter
            AutoSize = False
            BevelInner = bvNone
            BevelOuter = bvNone
            BorderStyle = sbsSunken
            Caption = 'TxErrorsCount'
            ParentShowHint = False
            ShowHint = True
            TabOrder = 1
          end
          object TxBytesCount: TStaticText
            Left = 58
            Top = 57
            Width = 100
            Height = 17
            Alignment = taCenter
            AutoSize = False
            BevelInner = bvNone
            BevelOuter = bvNone
            BorderStyle = sbsSunken
            Caption = 'TxBytesCount'
            ParentShowHint = False
            ShowHint = True
            TabOrder = 2
          end
          object TxKadrsCount: TStaticText
            Left = 58
            Top = 79
            Width = 100
            Height = 18
            Alignment = taCenter
            AutoSize = False
            BevelInner = bvNone
            BevelOuter = bvNone
            BorderStyle = sbsSunken
            Caption = 'TxKadrsCount'
            ParentShowHint = False
            ShowHint = True
            TabOrder = 3
          end
          object TxSpeed: TStaticText
            Left = 301
            Top = 15
            Width = 100
            Height = 17
            Alignment = taCenter
            AutoSize = False
            BevelInner = bvNone
            BevelOuter = bvNone
            BorderStyle = sbsSunken
            Caption = 'TxSpeed'
            TabOrder = 4
          end
          object TxSpeedMiddle: TStaticText
            Left = 301
            Top = 34
            Width = 100
            Height = 18
            Alignment = taCenter
            AutoSize = False
            BevelInner = bvNone
            BevelOuter = bvNone
            BorderStyle = sbsSunken
            Caption = 'TxSpeedMiddle'
            TabOrder = 5
          end
          object TxSpeedMax: TStaticText
            Left = 301
            Top = 55
            Width = 100
            Height = 17
            Alignment = taCenter
            AutoSize = False
            BevelInner = bvNone
            BevelOuter = bvNone
            BorderStyle = sbsSunken
            Caption = 'TxSpeedMax'
            TabOrder = 6
          end
          object TxSpeedMin: TStaticText
            Left = 302
            Top = 78
            Width = 100
            Height = 17
            Alignment = taCenter
            AutoSize = False
            BevelInner = bvNone
            BevelOuter = bvNone
            BorderStyle = sbsSunken
            Caption = 'TxSpeedMin'
            TabOrder = 7
          end
          object ModemmerTxKadrs: TStaticText
            Left = 172
            Top = 123
            Width = 98
            Height = 18
            Alignment = taCenter
            AutoSize = False
            BevelInner = bvNone
            BevelOuter = bvNone
            BorderStyle = sbsSunken
            Caption = 'RxKadrsCount'
            Font.Charset = RUSSIAN_CHARSET
            Font.Color = clGreen
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = [fsBold]
            ParentFont = False
            ParentShowHint = False
            ShowHint = True
            TabOrder = 8
          end
          object ModemmerTxBytes: TStaticText
            Left = 301
            Top = 123
            Width = 100
            Height = 18
            Alignment = taCenter
            AutoSize = False
            BevelInner = bvNone
            BevelOuter = bvNone
            BorderStyle = sbsSunken
            Caption = 'RxKadrsCount'
            Font.Charset = RUSSIAN_CHARSET
            Font.Color = clGreen
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = [fsBold]
            ParentFont = False
            ParentShowHint = False
            ShowHint = True
            TabOrder = 9
          end
          object TxQueueSize: TStaticText
            Left = 172
            Top = 99
            Width = 98
            Height = 18
            Alignment = taCenter
            AutoSize = False
            BevelInner = bvNone
            BevelOuter = bvNone
            BorderStyle = sbsSunken
            Caption = 'TxKadrsCount'
            ParentShowHint = False
            ShowHint = True
            TabOrder = 10
          end
        end
        object LineWorkTime: TStaticText
          Left = 305
          Top = 15
          Width = 100
          Height = 17
          Alignment = taCenter
          AutoSize = False
          BevelInner = bvNone
          BevelOuter = bvNone
          BorderStyle = sbsSunken
          Caption = 'RxSpeed'
          TabOrder = 3
        end
        object MaxFrameSize: TStaticText
          Left = 177
          Top = 306
          Width = 100
          Height = 18
          Alignment = taCenter
          AutoSize = False
          BevelInner = bvNone
          BevelOuter = bvNone
          BorderStyle = sbsSunken
          Caption = 'MaxFrameSize'
          ParentShowHint = False
          ShowHint = True
          TabOrder = 4
        end
      end
    end
    object TabSheet2: TTabSheet
      Caption = #1055#1088#1086#1089#1084#1086#1090#1088
      ImageIndex = 1
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      DesignSize = (
        429
        378)
      object MonitorBox: TGroupBox
        Left = 1
        Top = 84
        Width = 427
        Height = 293
        Anchors = [akLeft, akTop, akRight, akBottom]
        TabOrder = 1
        ExplicitWidth = 442
        ExplicitHeight = 251
      end
      object FilterBox: TGroupBox
        Left = 312
        Top = 1
        Width = 129
        Height = 51
        TabOrder = 2
        Visible = False
        object Label23: TLabel
          Left = 5
          Top = 7
          Width = 35
          Height = 13
          Caption = #1052#1086#1076#1077#1084
        end
        object Label24: TLabel
          Left = 48
          Top = 7
          Width = 32
          Height = 13
          Caption = #1051#1080#1085#1080#1103
        end
        object Label25: TLabel
          Left = 91
          Top = 7
          Width = 18
          Height = 13
          Caption = #1060#1040
        end
        object FilterModem: TEdit
          Left = 4
          Top = 20
          Width = 39
          Height = 17
          BevelInner = bvLowered
          BevelKind = bkFlat
          BorderStyle = bsNone
          TabOrder = 0
          Text = '255'
          OnChange = FilterModemChange
        end
        object FilterLine: TEdit
          Left = 46
          Top = 20
          Width = 39
          Height = 17
          BevelInner = bvLowered
          BevelKind = bkFlat
          BorderStyle = bsNone
          TabOrder = 1
          Text = '255'
          OnChange = FilterModemChange
        end
        object FilterFA: TEdit
          Left = 87
          Top = 20
          Width = 39
          Height = 17
          BevelInner = bvLowered
          BevelKind = bkFlat
          BorderStyle = bsNone
          TabOrder = 2
          Text = '255'
          OnChange = FilterModemChange
        end
      end
      object GroupBox3: TGroupBox
        Left = 1
        Top = -1
        Width = 121
        Height = 88
        TabOrder = 3
        object RxMonEnable: TCheckBox
          Left = 4
          Top = 7
          Width = 66
          Height = 16
          Caption = #1055#1088#1080#1105#1084
          Checked = True
          State = cbChecked
          TabOrder = 0
          OnClick = RxMonEnableClick
        end
        object TxMonEnable: TCheckBox
          Left = 4
          Top = 22
          Width = 76
          Height = 16
          Caption = #1055#1077#1088#1077#1076#1072#1095#1072
          Checked = True
          State = cbChecked
          TabOrder = 1
          OnClick = TxMonEnableClick
        end
        object RedirMonEnable: TCheckBox
          Left = 4
          Top = 36
          Width = 114
          Height = 17
          Caption = #1055#1077#1088#1077#1085#1072#1087#1088#1072#1074#1083#1077#1085#1080#1077
          TabOrder = 2
          OnClick = RedirMonEnableClick
        end
        object PausedMon: TCheckBox
          Left = 4
          Top = 53
          Width = 73
          Height = 16
          Caption = #1055#1072#1091#1079#1072
          TabOrder = 3
          OnClick = PausedMonClick
        end
      end
      object GroupBox4: TGroupBox
        Left = 122
        Top = -1
        Width = 190
        Height = 53
        TabOrder = 4
        object Label32: TLabel
          Left = 3
          Top = 11
          Width = 143
          Height = 13
          Caption = #1050#1086#1083#1080#1095#1077#1089#1090#1074#1086' '#1089#1090#1088#1086#1082' '#1084#1086#1085#1080#1090#1086#1088#1072
        end
        object MonitorData: TCheckBox
          Left = 34
          Top = 29
          Width = 152
          Height = 16
          Caption = #1054#1090#1086#1073#1088#1072#1078#1072#1090#1100' '#1089#1086#1076#1077#1088#1078#1080#1084#1086#1077' '
          TabOrder = 0
        end
        object MonLinesCount: TEdit
          Left = 151
          Top = 9
          Width = 35
          Height = 21
          AutoSize = False
          BevelInner = bvLowered
          BevelKind = bkFlat
          BorderStyle = bsNone
          TabOrder = 1
          Text = '8192'
          OnChange = MonLinesCountChange
        end
        object ClearMonitor: TBitBtn
          Left = 6
          Top = 25
          Width = 22
          Height = 21
          Glyph.Data = {
            36050000424D3605000000000000360400002800000010000000100000000100
            08000000000000010000EE0E0000EE0E000000010000000100006B6B6B008484
            8400C6C6C600DEDEDE00FF00FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
            FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
            FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
            FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
            FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
            FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
            FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
            FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
            FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
            FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
            FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
            FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
            FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
            FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
            FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
            FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
            FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
            FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
            FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
            FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
            FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
            FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
            FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
            FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
            FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
            FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
            FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
            FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
            FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
            FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
            FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
            FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00040404040404
            0404040404040404040404040404040404040404040404040404040404040404
            0404040404040404040404010000000000000104040404040404040002020202
            0202000004040404040404000202020202020001000404040404040100020202
            0202000101000404040404040003030303030300010100040404040404000303
            0303030300010100040404040404000303030303030001010004040404040400
            0303030303030001000404040404040400030303030303000004040404040404
            0400030303030303000404040404040404040100000000010104040404040404
            0404040404040404040404040404040404040404040404040404}
          TabOrder = 2
          OnClick = ClearMonitorClick
        end
      end
      object MonitorParam: TRadioGroup
        Left = 122
        Top = 51
        Width = 319
        Height = 36
        Caption = #1052#1086#1085#1080#1090#1086#1088#1080#1085#1075
        Columns = 3
        ItemIndex = 1
        Items.Strings = (
          #1042#1089#1105
          #1055#1086' '#1084#1086#1076#1077#1084#1091'/'#1083#1080#1085#1080#1080
          #1055#1086' '#1092#1080#1083#1100#1090#1088#1091)
        TabOrder = 0
        OnClick = MonitorParamClick
      end
    end
    object RedirectSheet: TTabSheet
      Caption = #1056#1077#1076#1080#1088#1077#1082#1090#1086#1088
      ImageIndex = 2
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      DesignSize = (
        429
        378)
      object RedirectList: TListView
        Left = 0
        Top = 72
        Width = 296
        Height = 244
        Anchors = [akLeft, akTop, akBottom]
        Columns = <
          item
            Caption = #1054#1090#1082#1091#1076#1072
            Width = 130
          end
          item
            Caption = #1060#1040
          end
          item
            Caption = #1055#1077#1088#1077#1085#1072#1087#1088#1072#1074#1083#1103#1090#1100' '#1085#1072' ...'
            Width = 110
          end>
        ColumnClick = False
        GridLines = True
        HideSelection = False
        ReadOnly = True
        RowSelect = True
        ParentShowHint = False
        ShowHint = False
        TabOrder = 5
        ViewStyle = vsReport
        OnSelectItem = RedirectListSelectItem
        ExplicitHeight = 202
      end
      object RedirNewBtn: TBitBtn
        Left = 2
        Top = 327
        Width = 100
        Height = 26
        Anchors = [akLeft, akBottom]
        Caption = #1044#1086#1073#1072#1074#1080#1090#1100'...'
        Enabled = False
        Glyph.Data = {
          36050000424D3605000000000000360400002800000010000000100000000100
          08000000000000010000E30E0000E30E0000000100000001000031319C003131
          A5003131AD003131B5003131BD003131C6003131CE003131D6003131DE003131
          E7003131EF003131F700FF00FF003131FF003139FF003939FF003942FF00424A
          FF004A4AFF005252FF006363FF006B6BFF006B73FF007B84FF00848CFF009C9C
          FF00C6CEFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000C1B1B1B1B1B
          1B1B1B1B1B1B1B1B1B0C1B16030404040505040403030201141B1B05080A0B0B
          0A0B0A0A0A090805001B1B070A0E0E0E0E0E0E0E0E0E0906021B1B090E0E0E0E
          0E1B180E0E0E0B08031B1B0A0E0E0E0E0E1B180E0E0E0E09041B1B0E0E0E0E0E
          0E1B180E0E0E0E0A051B1B0E0E181818181B181818180B0A061B1B0E0E1B1B1B
          1B1B1B1B1B1B0A0A061B1B0E10100E0E0E1B180E0E0B0A0A061B1B0E1313100E
          0E1B180E0E0B0A0A061B1B1015141110101B180E0E0E0B0B061B1B1318151312
          111B180E0E0E0E0B061B1B1419181514131211100E0E0E0B041B1B1A1412100E
          0E0E0E0E0E0E0B08171B0C1B1B1B1B1B1B1B1B1B1B1B1B1B1B0C}
        TabOrder = 6
        OnClick = RedirNewBtnClick
        ExplicitTop = 285
      end
      object RedirDeleteBtn: TBitBtn
        Left = 215
        Top = 327
        Width = 100
        Height = 26
        Anchors = [akLeft, akBottom]
        Caption = #1059#1076#1072#1083#1080#1090#1100
        Enabled = False
        Glyph.Data = {
          36050000424D3605000000000000360400002800000010000000100000000100
          08000000000000010000E30E0000E30E00000001000000010000104A7B00184A
          840018528C0018529400185A9C00185AA5001863AD001863B500186BBD00186B
          C6001873CE001873D600187BDE00187BE7001884E7001884EF001884F700188C
          F700FF00FF00188CFF001894FF002194FF00299CFF00319CFF0039A5FF004AAD
          FF0052ADFF0063B5FF006BBDFF0084C6FF00ADDEFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00121F1F1F1F1F
          1F1F1F1F1F1F1F1F1F121F1A030404040505040403030201191F1F05080A0A0A
          0A0A0A0A0A090805001F1F060A0C0C0D0E111111110C0906021F1F090C0F1F1D
          111111111D1F0A08031F1F0A0E11131F1D11111D1F0E0B09041F1F0C11131111
          1F1D1D1F110E0B0A051F1F0E13111111111F1D110F0C0A0A061F1F0F13131111
          1D1F1F1D0E0B0A0A061F1F131515131D1F100F1F1D0A0A0A061F1F1318181D1F
          13130F0E1F1D0A0A061F1F151A191F151514110F0E1F0A0A061F1F181C1A1817
          16161513100F0C0A061F1F191D1C1A191817161514110F0A041F1F1E19171513
          1311110F0E0C0A081B1F121F1F1F1F1F1F1F1F1F1F1F1F1F1F12}
        TabOrder = 7
        OnClick = RedirDeleteBtnClick
        ExplicitTop = 285
      end
      object RedirEditBtn: TBitBtn
        Left = 108
        Top = 327
        Width = 100
        Height = 26
        Anchors = [akLeft, akBottom]
        Caption = #1048#1079#1084#1077#1085#1080#1090#1100'...'
        Enabled = False
        Glyph.Data = {
          36050000424D3605000000000000360400002800000010000000100000000100
          08000000000000010000320B0000320B0000000100000001000000000000CE63
          00000010180039082100A56B2900DE8C3900E7396300526B7B0000089C007B9C
          A500D6CEAD00F7D6AD00FFD6AD00ADB5BD00B5C6C600BDC6C600C6C6C600FFE7
          C600ADC6CE00FFEFD6002163DE00FFEFDE00FFF7E700FFFFF700FF00FF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00181818181818
          1818181818181818181818010101010101010101010101010101180119191716
          1311070A0B0C0C0C0C011801190F0F0F0F0F0700120F0F0F0C01180119191919
          17160700000C0B0C0C011801190F0F0F0F0F071505000E0F0C01180119191919
          1919091505000C0C0C011801190F0F0F0F0F0F071505000D0C01180119191919
          191919091505000C0C011801190F0F0F0F0F0F0F071505000C01180119191919
          19191919091505020C0118011919191919191919190700140001180101010101
          0101010101041414030118180101010101010101010108080618181818181818
          1818181818181818181818181818181818181818181818181818}
        TabOrder = 8
        OnClick = RedirEditBtnClick
        ExplicitTop = 285
      end
      object RedirectEnable: TCheckBox
        Left = 8
        Top = 8
        Width = 177
        Height = 17
        Caption = #1056#1072#1079#1088#1077#1096#1080#1090#1100' '#1087#1077#1088#1077#1085#1072#1087#1088#1072#1074#1083#1077#1085#1080#1077
        TabOrder = 0
        OnClick = RedirectEnableClick
      end
      object RedirFrom: TEdit
        Left = 2
        Top = 40
        Width = 127
        Height = 21
        BevelInner = bvLowered
        BevelKind = bkFlat
        BorderStyle = bsNone
        TabOrder = 1
        OnChange = RedirFromChange
      end
      object RedirTo: TEdit
        Left = 183
        Top = 40
        Width = 108
        Height = 21
        BevelInner = bvLowered
        BevelKind = bkFlat
        BorderStyle = bsNone
        TabOrder = 3
        OnChange = RedirFromChange
      end
      object RedirOnly: TCheckBox
        Left = 305
        Top = 95
        Width = 109
        Height = 16
        Caption = #1053#1077' '#1086#1073#1088#1072#1073#1072#1090#1099#1074#1072#1090#1100
        TabOrder = 4
        OnClick = RedirOnlyClick
      end
      object RedirFA: TEdit
        Left = 133
        Top = 40
        Width = 46
        Height = 21
        BevelInner = bvLowered
        BevelKind = bkFlat
        BorderStyle = bsNone
        TabOrder = 2
        OnChange = RedirFromChange
      end
      object RedirDisabled: TCheckBox
        Left = 304
        Top = 75
        Width = 121
        Height = 17
        Caption = #1053#1077' '#1087#1077#1088#1077#1085#1072#1087#1088#1072#1074#1083#1103#1090#1100
        TabOrder = 9
        OnClick = RedirOnlyClick
      end
    end
    object TabSheet4: TTabSheet
      Caption = #1047#1072#1088#1077#1075#1077#1089#1090#1088#1080#1088#1086#1074#1072#1085#1085#1099#1077' '#1086#1073#1088#1072#1073#1086#1090#1095#1080#1082#1080
      ImageIndex = 4
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object DfaTree: TTreeView
        Left = 0
        Top = 0
        Width = 429
        Height = 378
        Align = alClient
        Indent = 19
        TabOrder = 0
        ExplicitWidth = 444
        ExplicitHeight = 336
      end
    end
    object pgAbout: TTabSheet
      Caption = #1054' '#1084#1086#1076#1091#1083#1077
      ImageIndex = 3
      object Label31: TLabel
        Left = 26
        Top = 104
        Width = 254
        Height = 39
        Caption = 
          #1052#1086#1076#1077#1084#1084#1077#1088' - '#1091#1087#1088#1072#1074#1083#1103#1077#1090' '#1084#1086#1076#1077#1084#1072#1084#1080', '#1086#1089#1091#1097#1077#1089#1090#1074#1083#1103#1077#1090' '#1087#1077#1088#1077#1076#1072#1095#1091',  '#1076#1080#1089#1087#1077#1090#1095#1077#1088 +
          #1080#1079#1072#1094#1080#1102' '#1080' '#1087#1077#1088#1077#1085#1072#1087#1088#1072#1074#1083#1077#1085#1080#1077' '#1087#1088#1080#1085#1103#1090#1099#1093' '#1076#1072#1085#1085#1099#1093
        WordWrap = True
      end
      inline AboutCommonFrame1: TAboutCommonFrame
        Left = 0
        Top = 0
        Width = 429
        Height = 94
        Align = alTop
        TabOrder = 0
        ExplicitWidth = 429
        ExplicitHeight = 94
        inherited Memo2: TMemo
          Width = 429
          Height = 94
          ExplicitWidth = 429
          ExplicitHeight = 94
        end
      end
    end
  end
  object StatusBar1: TStatusBar
    Left = 0
    Top = 435
    Width = 625
    Height = 21
    Panels = <
      item
        Text = #1052#1086#1076#1077#1084' '#1086#1089#1090#1072#1085#1086#1074#1083#1077#1085
        Width = 184
      end
      item
        Width = 50
      end>
    ExplicitTop = 393
    ExplicitWidth = 640
  end
  object ToolBar1: TToolBar
    Left = 0
    Top = 0
    Width = 625
    Height = 29
    ButtonHeight = 24
    ButtonWidth = 27
    Caption = 'ToolBar1'
    EdgeBorders = [ebTop, ebBottom]
    Images = ImageList1
    ParentShowHint = False
    ShowHint = True
    TabOrder = 3
    ExplicitWidth = 640
    object AddModemBtn: TToolButton
      Left = 0
      Top = 0
      Hint = #1055#1086#1076#1082#1083#1102#1095#1080#1090#1100' '#1084#1086#1076#1077#1084
      Caption = 'AddModemBtn'
      ImageIndex = 6
      ParentShowHint = False
      ShowHint = True
      OnClick = AddModemBtnClick
    end
    object RemoveModemBtn: TToolButton
      Left = 27
      Top = 0
      Hint = #1054#1090#1082#1083#1102#1095#1080#1090#1100' '#1084#1086#1076#1077#1084
      Caption = 'RemoveModemBtn'
      ImageIndex = 7
      OnClick = RemoveModemBtnClick
    end
    object ToolButton4: TToolButton
      Left = 54
      Top = 0
      Width = 11
      Caption = 'ToolButton4'
      ImageIndex = 5
      Style = tbsSeparator
    end
    object StartBtn: TToolButton
      Left = 65
      Top = 0
      Hint = #1047#1072#1087#1091#1089#1090#1080#1090#1100
      Caption = 'StartBtn'
      ImageIndex = 0
      OnClick = StartBtnClick
    end
    object StopBtn: TToolButton
      Left = 92
      Top = 0
      Hint = #1054#1089#1090#1072#1085#1086#1074#1080#1090#1100
      Caption = 'StopBtn'
      ImageIndex = 1
      OnClick = StopBtnClick
    end
    object RefreshBtn: TToolButton
      Left = 119
      Top = 0
      Hint = #1054#1073#1085#1086#1074#1080#1090#1100
      Caption = 'RefreshBtn'
      ImageIndex = 5
      OnClick = RefreshBtnClick
    end
    object ToolButton2: TToolButton
      Left = 146
      Top = 0
      Width = 21
      Caption = 'ToolButton2'
      ImageIndex = 5
      Style = tbsSeparator
    end
    object ToolButton1: TToolButton
      Left = 167
      Top = 0
      Hint = #1057#1086#1093#1088#1072#1085#1080#1090#1100' '#1080#1079#1084#1077#1085#1077#1085#1080#1103' '#1085#1072#1089#1090#1088#1086#1077#1082
      Caption = 'ToolButton1'
      ImageIndex = 13
      OnClick = ToolButton1Click
    end
    object ToolButton3: TToolButton
      Left = 194
      Top = 0
      Width = 13
      Caption = 'ToolButton3'
      ImageIndex = 2
      Style = tbsSeparator
    end
    object PropBtn: TToolButton
      Left = 207
      Top = 0
      Hint = #1048#1079#1084#1077#1085#1080#1090#1100' '#1085#1072#1089#1090#1088#1086#1081#1082#1080
      Caption = 'PropBtn'
      ImageIndex = 2
      OnClick = PropBtnClick
    end
    object ViewBtn: TToolButton
      Left = 234
      Top = 0
      Hint = #1055#1088#1086#1089#1084#1086#1090#1088
      Caption = 'ViewBtn'
      ImageIndex = 3
      OnClick = ViewBtnClick
    end
    object NewWindowBtn: TToolButton
      Left = 261
      Top = 0
      Hint = #1053#1086#1074#1086#1077'  '#1086#1082#1085#1086' '#1085#1072#1089#1090#1088#1086#1081#1082#1080
      Caption = 'NewWindowBtn'
      ImageIndex = 8
      OnClick = NewWindowBtnClick
    end
    object ToolButton6: TToolButton
      Left = 288
      Top = 0
      Width = 57
      Caption = 'ToolButton6'
      ImageIndex = 4
      Style = tbsSeparator
    end
    object SetMNBtn: TToolButton
      Left = 345
      Top = 0
      Hint = #1059#1089#1090#1072#1085#1086#1074#1080#1090#1100' '#1085#1086#1084#1077#1088' '#1084#1086#1076#1077#1084#1072
      Caption = 'SetMNBtn'
      Enabled = False
      ImageIndex = 4
      OnClick = SetModemNumberBtnClick
    end
    object MnPanel: TPanel
      Left = 372
      Top = 0
      Width = 157
      Height = 24
      BevelOuter = bvNone
      TabOrder = 0
      Visible = False
      object Label1: TLabel
        Left = 4
        Top = 4
        Width = 85
        Height = 16
        Alignment = taCenter
        AutoSize = False
        Caption = #1053#1086#1084#1077#1088' '#1084#1086#1076#1077#1084#1072'   '
      end
      object Edit1: TEdit
        Left = 89
        Top = 2
        Width = 40
        Height = 21
        TabOrder = 0
        Text = '1'
        OnChange = Edit1Change
      end
      object ModemNumber: TUpDown
        Left = 129
        Top = 2
        Width = 15
        Height = 21
        Associate = Edit1
        Min = 1
        Max = 255
        Position = 1
        TabOrder = 1
        OnChanging = ModemNumberChanging
      end
    end
  end
  object Timer1: TTimer
    Enabled = False
    Interval = 333
    OnTimer = Timer1Timer
    Left = 3
    Top = 367
  end
  object ImageList1: TImageList
    Left = 112
    Top = 367
    Bitmap = {
      494C010110001500040010001000FFFFFFFFFF10FFFFFFFFFFFFFFFF424D3600
      0000000000003600000028000000400000005000000001002000000000000050
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000639400004A6B006B6B6B000000
      0000000000000000000000000000000000000000000000000000AD5A5A00AD52
      5200A54A4A00AD949400C6CEC600CECEC600CECEC600C6CEC600C6CEC600B59C
      9C009C4242009C424200A5525200000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000355CC00000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000004DC400004DC400000000000000000000000000000000000000
      0000000000000000000000639400006394008CD6FF0000639400004A6B006B6B
      6B000000000000000000000000000000000000000000BD7B7300CE636300CE6B
      6B00B55A5A009C848400BDA5A500E7CECE00FFF7F700FFFFF700F7F7F700CEB5
      B500942929009C313100C65A5A00AD5A5A000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000151C6000C65D4000151C600004DC2000000000000000000000000000000
      00000000000000000000000000000000000000000000004DC400004DC400004D
      C4000151C6001676E500288FF800004DC4000000000000000000000000000000
      00000063940000639400007BB500007BB5008CD6FF000063940000639400004A
      6B006B6B6B0000000000000000000000000000000000BD7B7300CE636300CE63
      6300B55A5A009C7B7B009C424200B5737300E7DEDE00FFF7F700FFFFFF00D6B5
      B500943131009C313100BD5A5A00AD5A5A00000000000000000007E9E70000BD
      FF0000000000000000000000000000000000000000000000000000000000004D
      C2001B7DE700319AFF00004DC200000000000000000000000000000000000000
      0000000000000000000000000000000000000452CB000452CB001777E90042B2
      FF003EABFF001676E500004DC400000000000000000000000000006394000063
      9400007BB500007BB500007BB500007BB5008CD6FF0000639400006394000063
      9400004A6B006B6B6B00000000000000000000000000BD7B7300CE636300CE63
      6300B55A5A00AD8484009C3939009C393900CEBDBD00EFEFEF00FFFFFF00E7C6
      C6009429290094313100BD5A5A00AD5A5A0000000000000000000000000007E9
      E70000BDFF000000000000000000004DC200004DC200004DC2000151C6001676
      E500288FF800004DC20000000000000000000000000000000000000000000000
      000000000000000000000000000000000000004DC400268BFF00116ED7002489
      E9002D96FF002083FE00004DC400000000000000000000639400007BB500007B
      B500007BB500007BB500007BB500007BB5008CD6FF0000639400006394000063
      940000639400004A6B006B6B6B000000000000000000BD7B7300CE636300CE63
      6300B55A5A00B58C8C009C4A4A0094313100A59C9C00D6D6D600FFFFFF00E7C6
      C6009429290094313100BD5A5A00AD5A5A000000000000000000000000000000
      000000000000000000000355CC000558D0001676E90042B2FF003EABFF001676
      E500004DC2000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000004DC4003EABFF000B64
      D700116CE9001F81FF00004DC400000000000000000000639400007BB500007B
      B500007BB500007BB500007BB500007BB5008CD6FF0000639400006394000063
      94000063940000639400004A6B006B6B6B0000000000BD7B7300CE636300CE63
      6300BD5A5A00C6948C00C6949400B5848400AD8C8C00BDA5A500E7C6C600DEAD
      AD00A5393900A5393900C65A5A00AD5A5A000000000000000000000000000000
      00000000000000445D00075CE000268BFF00116ED7002489E9002D96FF002083
      FE00004DC2000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000004DC4002083
      FE00075CD4000D68E200004DC400000000000000000000639400007BB500007B
      B500007BB500007BB500007BB500007BB5008CD6FF0000639400006394000063
      94000063940000639400004A6B000000000000000000BD7B7300CE636300CE63
      6300CE636300CE636300CE636300CE636300CE636300C65A5A00C65A5A00CE63
      6300CE636300CE636300CE6B6B00AD525A000000000000000000000000000000
      0000056A08000D82190000485A001676E9003EABFF000C65D400116CE9001F81
      FF00004DC2000000000000000000000000000000000000000000000000000000
      000000000000000000006F707300393B3E00000000000000000000000000004D
      C4001F81FF00075CD400004DC400000000000000000000639400007BB500007B
      B500007BB500007BB500007BB5008CD6FF008CD6FF008CD6FF00006394000063
      94000063940000639400004A6B000000000000000000BD7B7300B5525200B55A
      5A00C6848400D6A5A500D6ADAD00D6ADA500D6ADAD00D6A5A500D6A5A500D6AD
      A500D6ADAD00D69C9C00CE636300AD5252000000000000000000000000000155
      01000770110032DC640020A7370000484400116CE5002083FE000558D0000D68
      E200004DC200000000000000000000000000000000000000000000000000056A
      0800005200006F707300393B3E00000000006F7073006F707300000000000000
      0000004DC400004DC40000000000000000000000000000639400007BB500007B
      B500007BB5008CD6FF008CD6FF00CECECE00C6C6C600007BB5008CD6FF000063
      94000063940000639400004A6B000000000000000000BD7B7300AD524A00E7CE
      CE00F7F7F700F7F7EF00F7F7F700F7F7F700F7F7F700F7F7F700F7F7F700F7F7
      F700F7F7F700DEBDBD00C65A5A00AD525A00000000000000000000000000035C
      05001AB64100138622002CD34A00077E0700004539000C64E3002083FE000558
      D000004DC2000000000000000000000000000000000000000000005200000770
      110032DC640000520000000000006F707300393B3E0000000000000000000000
      0000000000000000000000000000000000000000000000639400007BB5008CD6
      FF008CD6FF00BDBDBD007B7B7B00B5B5B5006B6B6B00007BB500007BB5008CD6
      FF000063940000639400004A6B000000000000000000BD7B7300B5524A00EFD6
      D600FFF7F700F7EFEF00F7EFEF00F7EFEF00F7EFEF00F7EFEF00F7EFEF00F7EF
      EF00F7F7F700DEBDBD00C65A5A00AD525A00000000000000000000000000035C
      05001AB641000D821900056A080001880200006A0000004C3D00085CDD00004D
      C200000000000000000000000000000000000000000000000000035C05001AB6
      4100138622002CD34A0000520000393B3E00000000006F707300393B3E000000
      00000000000000000000000000000000000000000000006394008CD6FF00007B
      B500007BB5008C8C8C00D6D6D600D6D6D600E7E7E700007BB500FF6B6B00DEC6
      CE008CD6FF0000639400004A6B000000000000000000BD7B7300B5524A00EFD6
      D600EFEFEF00D6D6D600D6D6D600D6D6D600D6D6D600D6D6D600D6D6D600D6D6
      D600EFEFEF00DEBDBD00C65A5A00AD525A00000000000000000000000000035C
      050015A533000B8C1A00006A0000035C0500077E070000610000005141000000
      0000000000000000000000000000000000000000000000000000035C05001AB6
      41000C871F00056A080001880200005200006F707300393B3E00000000000000
      0000000000000000000000000000000000000000000000000000004A6B00007B
      B500007BB50094ADB500B5B5B500007BB500007BB500007BB500FF000000FF4A
      4A00007BB5008CD6FF00004A6B000000000000000000BD7B7300B5524A00EFD6
      D600EFEFEF00DED6D600DEDEDE00DEDEDE00DEDEDE00DEDEDE00DEDEDE00DED6
      D600EFEFEF00DEBDBD00C65A5A00AD525A000000000000000000000000000770
      1100056A080003740600006A00000061000001550100015501000000000007E9
      E70000BDFF000000000000000000000000000000000000000000035C050015A5
      33000B8C1A0002660400035C0500007F00000052000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000004A
      6B00007BB500007BB500007BB500007BB500007BB500007BB500007BB500007B
      B500004A6B00004A6B00000000000000000000000000BD7B7300B5524A00EFD6
      D600F7F7EF00E7DEDE00E7DEDE00E7DEDE00E7DEDE00E7DEDE00E7DEDE00E7DE
      DE00EFEFEF00DEBDBD00C65A5A00AD525A0000000000000000000E7A1A001EB2
      3500035C0500004C0000004C0000004C0000004C000000000000000000000000
      000007E9E70000BDFF000000000000000000000000000000000007701100056A
      080003740600016A010002660400005200000052000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000004A6B00007BB500007BB500007BB500007BB500007BB500004A6B00004A
      6B000000000000000000000000000000000000000000BD7B7300B5524A00EFD6
      D600EFEFEF00D6D6D600D6D6D600D6D6D600D6D6D600D6D6D600D6D6D600D6D6
      D600EFEFEF00DEBDBD00C65A5A00AD525A000000000006660C0020C13A000374
      0600004C00000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000E7A1A001EB23500035C
      0500004900000049000000490000004900000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000004A6B00007BB500007BB500004A6B00004A6B00000000000000
      00000000000000000000000000000000000000000000BD7B7300B5524A00E7D6
      CE00FFF7F700F7EFEF00F7EFEF00F7EFEF00F7EFEF00F7EFEF00F7EFEF00F7EF
      EF00FFF7F700DEBDBD00C65A5A00AD525A000155010001550100035C0500004C
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000006660C0020C13A00037406000047
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000004A6B00004A6B000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000AD524A00CEB5
      B500D6D6D600CECECE00CECECE00CECECE00CECECE00CECECE00CECECE00CECE
      CE00D6D6D600CEADAD00A54A4A000000000007701100004C0000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000520000035C0500004900000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000004A9CC6008484840084A5AD000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000004A9CC6008484840084A5AD000000
      0000000000000000000000000000000000000000000029ADD60031B5DE0021AD
      D600000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000424242004242420042424200424242000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000006B8C940073CEE7005A737B000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000006B8C940073CEE7005A737B000000
      0000000000000000000000000000000000000000000029ADD6009CDEEF0084EF
      FF004AC6E70021ADD60018A5C60018A5C60018A5C60000000000000000000000
      000000000000000000000000000000000000000000008C636300424242004242
      42008C4A390094521800B55A0000424242008C6363008C6363008C6363008C63
      63008C6363008C6363000000000000000000000000004A9CC6008484840084A5
      AD0000000000000000000000000000000000738C94005AB5E700427B9C000000
      000000000000000000000000000000000000000000004A9CC6008484840084A5
      AD0000000000000000000000000000000000738C94005AB5E700427B9C000000
      0000000000000000000000000000000000000000000029ADD60052BDE7009CFF
      FF0094FFFF0073DEF70073DEF70073DEF70073DEF7004AC6E70021ADD60018A5
      C60000000000000000000000000000000000000000008C636300B55A0000B55A
      0000AD5A1000B55A0000C65A00004242420008A5180000840000008400000084
      000008A518008C6363000000000000000000000000006BADBD0073C6D6004A63
      6B0000000000000000000000000000000000636B6B00297B9C0029739C007B63
      63009C6B6B00000000000000000000000000000000006BADBD0073C6D6004A63
      6B0000000000000000000000000000000000636B6B00297B9C0029739C007B63
      63009C6B6B000000000000000000000000000000000029ADD60052BDE700ADFF
      FF008CF7FF008CEFFF008CEFFF008CEFFF0073DEF70073DEF70073DEF7004AC6
      EF0021ADD600000000000000000000000000000000008C636300C65A0000C65A
      0000C65A0000C65A0000CE6300004242420031C64A0010AD180010AD180010AD
      1800009C00008C636300000000000000000000000000528494005ABDF700426B
      8C0073636300000000009C737300AD737300AD6B6B0052848C0073CEE7008C73
      7B00D68484008C636300000000000000000000000000528494005ABDF7000031
      DE0073636300000000009C737300AD737300AD6B6B0052848C0073CEE7008C73
      7B00D68484000031DE0000000000000000000000000029ADD60029ADD600ADDE
      EF0094F7FF0094F7FF008CEFFF008CEFFF008CEFFF008CEFFF0073DEF70073DE
      F7004AC6EF00000000000000000000000000000000008C636300C65A0000CE63
      0000CE630000CE630000CE6300004242420031C64A0021BD310021BD310029C6
      4A0042D66B008C636300000000000000000000000000000000002184B5002973
      94006B4A4A009C6B6B00D6848400DE8C8C00C67B7B006B737B0084DEEF009484
      8C00DE8C8C00AD7373008C8C8C000000000000000000000000002184B5000031
      DE000031DE009C6B6B00D6848400DE8C8C00C67B7B006B737B0084DEEF009484
      8C000031DE00AD7373008C8C8C00000000000000000029ADD60073DEF70029AD
      D6009CFFFF008CF7FF008CF7FF008CF7FF008CEFFF008CEFFF008CEFFF0073DE
      F70073DEF70018A5C6000000000000000000000000008C636300CE630000CE6B
      0000CE6B0000CE6B0000D6730000424242000084000021AD310029BD390031C6
      4A0042D66B008C636300000000000000000000000000AD737300528CA50073D6
      FF00635A5A00A56B6B00D6848400D6848400D68484007B636300C6CECE00B58C
      8C00D6848400BD7B7B00947B7B000000000000000000AD737300528CA50073D6
      FF000031DE000031DE00D6848400D6848400D68484007B6363000031DE000031
      DE00D6848400BD7B7B00947B7B00000000000000000029ADD60094F7FF0029AD
      D600ADDEEF00A5EFF700A5EFF700A5F7FF008CEFFF008CEFFF008CEFFF0073DE
      F7000073080018A5C6000000000000000000000000008C636300CE6B0000CE6B
      0000DE841800FFF7DE00D673000042424200008400000084000000840000009C
      0000009C00008C636300000000000000000000000000BD7B7B008C84840084DE
      EF0073949C009C6B6B00C67B7B00C67B7B009C6B6B00A56B6B00CE8C8C00CE94
      9400CE949400C68C8C00948484000000000000000000BD7B7B008C84840084DE
      EF0073949C000031DE000031DE00C67B7B009C6B6B000031DE000031DE00CE94
      9400CE949400C68C8C0094848400000000000000000029ADD6009CFFFF0073DE
      F70029ADD60018A5C60018A5C60018A5C600ADDEEF008CF7FF0084EFFF000073
      08005AE78C000073080018A5C60000000000000000008C636300D6730000D673
      0000DE7B0800D6730000D673000042424200FFE7C600FFE7C600FFE7C600FFE7
      C600FFE7C6008C636300000000000000000000000000AD7373009C6B6B00949C
      9C00A5A5A5009C6B6B009C6B6B00CE8C8C00DEA5A500E7ADAD00DEA5A500DEA5
      A500DEA5A500C68C8C009C8484000000000000000000AD7373009C6B6B00949C
      9C00A5A5A5009C6B6B000031DE000031E7000031DE000031E700DEA5A500DEA5
      A500DEA5A500C68C8C009C848400000000000000000029ADD6009CFFFF0094F7
      FF0073DEF70073DEF70073DEF7006BDEF70029ADD600ADDEEF000073080052D6
      7B0042D66B0031C64A000073080000000000000000008C636300D6730000D673
      0000DE7B0000DE7B0000DE7B000042424200FFEFD600FFEFD600FFEFD600FFEF
      D600FFEFD6008C636300000000000000000000000000A56B6B00A56B6B00AD73
      7300CE9C9C00DEB5B500EFBDBD00EFB5B500DEA5A500E7A5A500EFADAD00EFAD
      AD00DEADAD00B58484008C8C8C000000000000000000A56B6B00A56B6B00AD73
      7300CE9C9C00DEB5B500EFBDBD000031E7000031E7000031DE000031DE00EFAD
      AD00DEADAD00B58484008C8C8C00000000000000000029ADD6009CFFFF0094F7
      FF0094F7FF0094F7FF0094F7FF0073DEF70073DEF70029ADD60018A5C600108C
      210031C64A00109C210018A5C60000000000000000008C636300DE7B0000E77B
      0000E77B0000E77B0000EF7B000042424200FFF7D600FFF7DE00FFF7DE00FFF7
      DE00FFF7DE008C636300000000000000000000000000AD7B7B00EFD6D600FFEF
      EF00FFE7E700EFCECE00E7C6C600EFC6C600F7C6C600EFBDBD00E7ADAD00EFAD
      AD00CE9C9C009C848400000000000000000000000000AD7B7B00EFD6D600FFEF
      EF00FFE7E700EFCECE000031DE000031EF00F7C6C600F7C6C6000031F7000031
      DE00CE9C9C009C84840000000000000000000000000029ADD600C6FFFF0094FF
      FF009CFFFF00D6FFFF00D6FFFF008CEFFF0094EFFF0073DEF70073DEF7000884
      100018AD2900088410000000000000000000000000008C636300FF840000F784
      0000EF7B0000EF7B0000EF7B000042424200FFF7D600FFF7D600FFF7D600FFF7
      D600FFF7D6008C636300000000000000000000000000B5848400DECECE00FFF7
      F700FFF7F700FFE7E700EFCECE00EFD6D600F7CECE00F7C6C600F7C6C600DEAD
      AD009C84840000000000000000000000000000000000B5848400DECECE00FFF7
      F700FFF7F7000031F7000031EF00EFD6D600F7CECE00F7C6C600F7C6C6000031
      F7009C8484000000000000000000000000000000000021ADD6009CDEEF00C6FF
      FF00C6FFFF009CDEEF0018ADD60018A5C60018A5C60018A5C60018A5C600088C
      100008A51800000000000000000000000000000000008C636300FF840000FF84
      0000F7840800F7840000FF84000042424200FFF7D600FFF7D600FFF7D600FFF7
      D600FFF7D6008C63630000000000000000000000000000000000B58C8C00CEAD
      AD00EFDEDE00FFEFEF00FFEFEF00FFE7E700EFC6C600EFB5B5009C8484009C84
      8400000000000000000000000000000000000000000000000000B58C8C00CEAD
      AD000031FF000031EF00FFEFEF00FFE7E700EFC6C600EFB5B5009C8484009C84
      84000031F700000000000000000000000000000000000000000031B5DE0029AD
      D60018A5C60018A5C60000000000000000000000000000000000088C100008A5
      18000884100000000000000000000000000000000000000000008C6363008C63
      6300DE732900E77B1800FF840000424242008484840084848400848484008484
      840084848400848484000000000000000000000000000000000000000000AD9C
      9C00C6ADAD00CEB5B500C6ADAD00BDA5A500BDA5A5009C8484008C8484007373
      7300000000000000000000000000000000000000000000000000000000000031
      F7000031F700CEB5B500C6ADAD00BDA5A500BDA5A5009C8484008C8484007373
      7300000000000031F70000000000000000000000000000000000000000000000
      000000000000000000000000000000730800087B0800088C1000088C1000087B
      0800000000000000000000000000000000000000000000000000000000000000
      00008C6363008C6363008C636300424242000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000A5A5A500FFFFFF008C8C8C007373
      7300000000000000000000000000000000000000000000000000000000000031
      F70000000000000000000000000000000000A5A5A500FFFFFF008C8C8C007373
      7300000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000A5A5A500FFFFFF008C8C8C007373
      7300000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000A5A5A500FFFFFF008C8C8C007373
      7300000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000A5636B00A563
      6B00A5636B00A5636B00A5636B00A5636B00A5636B00A5636B00A5636B00A563
      6B00A5636B00A5636B00A5636B00000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000A5636B00FFEF
      C600C6CE9400D6CE9400EFCE9C00E7CE9400EFC68400EFBD8400EFBD7B00EFBD
      8400EFBD8400EFC68400A5636B00000000000000000000000000000000000000
      000000000000000000006BAD840021842900218429006BAD8400000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000FF0000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000A5636B00FFEF
      CE009CBD7300299C21006BAD4A0021941000219410005AA53900CEB57300EFBD
      7B00EFBD7B00EFC68400A5636B00000000000000000000000000000000000000
      0000000000000000000063A5630094DE8C0039B5520021842900000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000FF000000FF000000FF00000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000A5635A00FFEF
      DE00BDCE9C00108C08000084000000840000008400000084000029941800DEBD
      7B00EFBD7B00EFC68400A5636B00000000000000000000000000000000000000
      0000000000000000000063A5630094DE8C0039B5520021842900000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000FF000000FF000000FF00000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000A5635A00FFF7
      E700BDCE9C00189410000084000018941000ADBD730073AD4A000084000073AD
      4A00EFBD8400EFC68400A5636B00000000000000000000000000000000000000
      0000000000000000000063A5630094DE8C0039B5520021842900000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      FF000000FF000000FF000000FF000000FF000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000A5736B00FFF7
      EF00BDD6A500088C0800008400000084000084B55A00EFCEA500A5B56B006BAD
      4A00EFC68C00EFC68400A5636B00000000000000000000000000000000000000
      0000000000000000000063A5630094DE8C0039B5520021842900000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000000000FF000000
      FF000000FF000000FF000000FF000000FF000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000A5736B00FFFF
      FF00E7E7D600A5CE94009CC6840094BD73009CBD7300EFD6AD00EFCEA5009CC6
      7B00EFC69400EFC68C00A5636B0000000000000000006BAD8400218429002184
      290021842900218429002184290094DE8C0039B5520021842900218429002184
      290021842900218429006BAD840000000000000000006B84C6000021A5000021
      A5000021A5000021A5000021A5000021A5000021A5000021A5000021A5000021
      A5000021A5000021A5006B84C60000000000000000007F7F7F000000FF000000
      FF0000000000000000000000FF000000FF000000FF0000000000000000000000
      0000000000000000000000000000000000000000000000000000BD846B00FFFF
      FF00A5DEA500FFEFE700F7EFD6009CC6840094BD730094BD73009CBD7300EFCE
      A500EFCE9C00F7CE9400A5636B00000000000000000063A5630039B5520039B5
      520039B5520039B5520039B5520039B5520039B5520039B5520039B5520039B5
      520039B5520039B552002184290000000000000000000021A5009494F7000029
      E7000029E7000029E7000029E7000029E7000029E7000029E7000029E7000029
      E7000029E7000029E7000021A500000000007F7F7F000000FF00000000000000
      00000000000000000000000000000000FF000000FF0000000000000000000000
      0000000000000000000000000000000000000000000000000000BD846B00FFFF
      FF0073C67300ADD6A500FFEFE70084C673000084000000840000088C0800EFD6
      AD00EFCEA500F7D6A500A5636B00000000000000000063A5630094DE8C0094DE
      8C0094DE8C0094DE8C0094DE8C0094DE8C0039B5520094DE8C0094DE8C0094DE
      8C0094DE8C0094DE8C002184290000000000000000000021A500B5C6FF009CBD
      FF009CBDFF009CB5FF009CB5FF009CB5FF00638CF700638CF700638CF700638C
      F700526BF700526BF7000021A500000000000000000000000000000000000000
      00000000000000000000000000000000FF000000FF000000FF00000000000000
      0000000000000000000000000000000000000000000000000000D6946B00FFFF
      FF0084CE8400008400007BC67300ADD6A5001894180000840000108C0800F7D6
      B500F7D6AD00EFCEA500A5636B0000000000000000006BAD840063A5630063A5
      630063A5630063A5630063A5630094DE8C0039B5520021842900218429002184
      290021842900218429006BAD840000000000000000006B84C6000021A5000021
      A5000021A5000021A5000021A5000021A5000021A5000021A5000021A5000021
      A5000021A5000021A5006B84C600000000000000000000000000000000000000
      0000000000000000000000000000000000000000FF000000FF00000000000000
      0000000000000000000000000000000000000000000000000000D6946B00FFFF
      FF00F7F7EF0029A5290000840000008400000084000000840000108C0800FFEF
      CE00DECEB500B5AD9400A5636B00000000000000000000000000000000000000
      0000000000000000000063A5630094DE8C0039B5520021842900000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000FF000000FF000000
      0000000000000000000000000000000000000000000000000000DE9C7300FFFF
      FF00FFFFFF00DEF7DE0063BD6300219C2100219C210073BD6B00299C2100946B
      5200A56B5A00A56B5A00A5636B00000000000000000000000000000000000000
      0000000000000000000063A5630094DE8C0039B5520021842900000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000007F7F7F000000
      FF00000000000000000000000000000000000000000000000000DE9C7300FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00DEF7DE00DEF7DE00FFFFF700ADB594008C6B
      5200E79C5200E78C3100B56B4A00000000000000000000000000000000000000
      0000000000000000000063A5630094DE8C0039B5520021842900000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000007F7F
      7F000000FF000000000000000000000000000000000000000000E7AD7B00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00DEC6C600A56B
      5A00FFB55A00BD7B5A0000000000000000000000000000000000000000000000
      0000000000000000000063A5630094DE8C0039B5520021842900000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000FF000000FF00000000000000000000000000E7AD7B00F7F7
      EF00F7F7EF00F7F7EF00F7F7EF00F7F7EF00F7F7EF00F7F7EF00DEC6C600A56B
      5A00BD846B000000000000000000000000000000000000000000000000000000
      000000000000000000006BAD840063A5630063A563006BAD8400000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000E7AD7B00D694
      6B00D6946B00D6946B00D6946B00D6946B00D6946B00D6946B00D6946B00A56B
      5A00000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000ADADA5008C8C8C006B6B
      6B00BDB5AD000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000ADA59C009C522900844A
      210073422100633921004A4A4A00BDB5AD000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000C6ADAD00B58C
      8C00B58C8C00B58C8C00B58C8C00B58C8C00B58C8C00B58C8C00B58C8C00B58C
      8C00A57B7B004A42420000000000000000000000000000000000000000000000
      000000000000004A000000630000004A00000000000000000000000000000000
      000000000000000000000000000000000000000000005A6BEF001029A5000010
      9C0000109C0000109C0000109C0000109C0000109C0000109C0000109C000821
      9C005A6BEF0000000000000000000000000000000000A59C9400AD4A0800DEAD
      7B00EFB58C00C67B42009C522900633921004A4A4A00BDB5AD00000000000000
      000000000000000000000000000000000000000000000000000000000000D6B5
      B500C69C9C00C69C9C00C69C9C00C69C9C00B58C8C00B58C8C00B58C8C00B58C
      8C00947373000000000000000000000000000000000000000000000000000000
      00000000000000630000089C210000630000004A000000000000000000000000
      000000000000000000000000000000000000000000001029C6000018C6000821
      C6001029C6001029C6000829CE001029CE001029CE000021CE000018CE000010
      AD0010219C00000000000000000000000000000000009C948C00B5521000E7BD
      9400FFEFD600FFDEC600FFDEB500F7BD8400C6844A009C522900523121000000
      0000000000000000000000000000000000000000000094737300947373004239
      3900392929003929290039292900392929002921210029212100292121002921
      2100210821009473730000000000000000000000000000000000000000000000
      00000000000000630000109C2100089C210000630000004A0000000000000000
      000000000000000000000000000000000000000000000018CE001031D6001831
      D6002139E7002942E7002142E7001842E7001039E7000831E7000029E7000018
      CE0000109C0000000000000000000000000000000000948C7B00B5521000E7C6
      AD00FFE7D600FFDEC600FFDEB500FFE7BD00FFD68400FFAD5A00733910009494
      940000000000000000000000000000000000CEA5A500CEA5A500CEA5A500CEA5
      A500CEA5A500BD949400B5848400AD848400AD848400AD848400AD848400AD84
      8400AD848400AD848400A5737300000000000000000000000000000000000000
      0000000000000063000010A5290010A52900089C210000630000004A00000000
      000000000000000000000000000000000000000000000021D6001831D6002942
      E700314AE700294AE700294AE7001842E7001042E7001039E7000831E7000021
      CE0000109C0000000000000000000000000000000000948C7B00BD631800E7CE
      B500FFEFD600CED6CE00009CCE00FFDEB500FFC65A00F7B57300633921008484
      840000000000000000000000000000000000CEA5A500AD8484009C7373009C73
      73009C7373009C7373009C737300947373009473730094737300947373009473
      730094737300B5A5A500A5737300000000000000000000000000000000000000
      0000000000000063000010B5390010A52900089C2100089C210000630000004A
      000000000000000000000000000000000000000000001031D6002142E7003952
      E7003152E700314AE700294AE7001842E7001839E7001039E7000831E7001031
      CE0000109C0000000000000000000000000000000000948C7B00BD631800FFDE
      C600DEE7DE00009CCE00009CCE00BDBDA50094947300D6946300844A18008484
      840094949400000000000000000000000000CEA5A500EFBD7300C6847B00FFFF
      FF00FFF7F700FFF7EF00FFF7EF00F7EFE700F7EFE700F7EFE700F7EFE700F7E7
      D600AD737300E7B56B00A5737300000000000000000000000000000000000000
      0000000000000063000018B54A0010AD390010AD390010AD390010A529000063
      000000520000000000000000000000000000000000002139E700314AE7003952
      E7003152E700314AE700294AE7001842E7001039E7001031E7000831E7001031
      CE0000109C00000000000000000000000000000000009C846B00C66B2900FFE7
      D60039ADD600BDD6DE00ADD6D600009CCE00A59C8400EFB58C00FFCE9400C684
      4A00946339006B6B6B009494940000000000CEA5A500EFBD7300C6847B00FFFF
      FF00FFFFFF00FFFFF700FFF7EF00FFF7EF00FFF7EF00F7EFE700F7EFE700F7EF
      E700AD737300E7B56B00AD7B7B00000000000000000000000000000000000000
      0000000000000063000052BD52005ABD52004ABD52004ABD52004ABD52000063
      00000052000000000000000000000000000000000000314AE700425AE7004252
      E7003152E700314AE7002942E7001839DE001031DE001031DE001031DE001031
      CE0000109C00000000000000000000000000000000009C846B00C66B2900F7EF
      E700F7F7EF00FFF7E700DEE7DE0039A5BD00FFE7C600E7AD7300C6844A00FFCE
      9400FFCE9400BD946B005A5A84006B6B6B00CEA5A500EFBD7B00C6847B00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFF7F700FFF7EF00FFF7EF00F7EFE700EFE7
      DE00AD737300E7B56B00AD7B7B00000000000000000000000000000000000000
      000000000000006300005AC65A006BC66B006BC663004ABD520000630000004A
      000000000000000000000000000000000000000000003952E7004A63E700425A
      E7003952E7003142E7002942DE001839DE001031D6001031DE001031DE001031
      CE0000109C00000000000000000000000000000000009C846B00C66B2900FFF7
      EF00FFF7F700FFF7E700FFF7E7004AADCE00F7EFDE00E7BD9400633918008C7B
      6300EFB58C00AD9C9C00315AD6004A52B500CEA5A500EFC68C00C6847B00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFF7EF00F7EFE700DED6C600CEBD
      B500AD737300E7B56B00AD7B7B00000000000000000000000000000000000000
      000000000000006300006BC66B007BD67B007BD67B0000630000004A00000000
      000000000000000000000000000000000000000000004252E700526BEF004A63
      E7004252DE00314AE7002942DE002139DE001839D6001831DE001031DE001031
      CE0000109C00000000000000000000000000000000009C846B00CE7B4200FFF7
      F700FFFFFF00FFF7F700FFF7EF00ADD6D600CEE7E700F7BD9400523121008484
      840000000000BDB5AD00426BD600424A9C00CEA5A500EFC68C00C6847B00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00F7EFEF00C6A59400C6947300CE8C
      5A00C68C6B00E7BD8400BD949400000000000000000000000000000000000000
      0000000000000063000063C663008CD68C0000630000004A0000000000000000
      000000000000000000000000000000000000000000004A63E7006B84EF005A73
      EF004A63E7004252E7003152E700314ADE002942DE002142DE002139D6001031
      CE0008189C0000000000000000000000000000000000AD8C6B00CE7B4200FFF7
      F700FFFFFF00FFFFFF00FFFFFF00D6EFEF00CEE7E700F7BD8400523121008484
      840000000000000000000000000000000000CEA5A500EFC68C00C6847B00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00F7EFEF00C69C8C00EFBD7B00F7AD
      4200AD737300F7D68400BD949400000000000000000000000000000000000000
      0000000000000063000039AD390000630000004A000000000000000000000000
      000000000000000000000000000000000000000000005A73EF008C94EF006B7B
      EF005273EF005263E7004A63E7004A5AE700425AE7003952E700294AE7001031
      CE001831A50000000000000000000000000000000000ADA59C00CE631000C673
      3900D6946300DEB59C00EFCEC600F7EFE700FFFFFF00E7BD94005A4229008484
      840000000000000000000000000000000000CEA5A500EFC68C00C6847B00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00F7EFEF00C69C8C00E7B56B00AD73
      7300FFDE8C00FFDE8C00BD949400000000000000000000000000000000000000
      000000000000004A000000630000004A00000000000000000000000000000000
      000000000000000000000000000000000000000000005A73EF005A73EF004A5A
      E7003952E700314AE700314AE7002942E7002939E7002139D6001839D6001831
      C6005A6BEF0000000000000000000000000000000000BDB5AD00E78C2900C65A
      0000C65A0000C65A0000BD520000C65A1000C67B4200C67339005A4A31009494
      940000000000000000000000000000000000CEA5A500EFC68C00C6847B00D6BD
      B500D6BDB500D6BDB500D6BDB500D6BDB500D6BDB500C6948400AD737300F7D6
      8C00FFEFA500FFDE8C00C69C9C00000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000BDB5
      AD00ADA59C009C948C00CE8C4200D6843100D6731000D66B0000949494000000
      000000000000000000000000000000000000CEA5A500E7BD9C00EFD6AD00EFD6
      AD00EFDEB500EFDEB500EFDEB500EFDEB500EFDEB500EFDEB500EFD6AD00EFD6
      AD00EFD6AD00EFD6AD00C69C9C00000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000CEA5A500CEA5A500CEA5
      A500CEA5A500CEA5A500CEA5A500CEA5A500CEA5A500CEA5A500CEA5A500CEA5
      A500CEA5A500CEA5A5000000000000000000424D3E000000000000003E000000
      2800000040000000500000000100010000000000800200000000000000000000
      000000000000000000000000FFFFFF0000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000FF1FC001FFFDFFF9FC0F8000FFF0FF80
      F0078000CFE1FF01C0038000E603FF0180018000FC07FF8180008000F807FFC1
      80018000F007FCE180018000E007E13380018000E007C27F80018000E00FC09F
      80018000E01FC03FC0018000E027C07FE0038000C073C07FF00F800087FF80FF
      F83F80000FFF0FFFFCFFC0013FFF1FFFFFFFFFFFFF1FFF1F8FFFF0FFFF1FFF1F
      807F80038F1F8F1F800F80038F078F07800780038403840380078003C001C001
      8003800380018001800380038001800180018003800180018001800380018001
      8001800380038003800380038007800780078003C00FC007C3C7C003E00FE00B
      FE0FF0FFFF0FEF0FFFFFFFFFFF0FFF0FFFFFC001FFFFFFFFFFFFC001FC3FFFFF
      F9FFC001FC3FFFFFF0FFC001FC3FFFFFF0FFC001FC3FFFFFE07FC001FC3FFFFF
      C07FC00180018001843FC001800180011E3FC00180018001FE1FC00180018001
      FF1FC001FC3FFFFFFF8FC001FC3FFFFFFFC7C001FC3FFFFFFFE3C003FC3FFFFF
      FFF8C007FC3FFFFFFFFFC00FFFFFFFFFFFFFFFFF87FFFFFFFFFFFFFF80FFC003
      F8FF8007803FE007F87F8007801F8003F83F8007800F0001F81F8007800F0001
      F80F800780070001F807800780010001F807800780000001F80F800780000001
      F81F800780080001F83F8007800F0001F87F8007800F0001F8FF8007800F0001
      FFFFFFFFE01F0001FFFFFFFFFFFF800300000000000000000000000000000000
      000000000000}
  end
  object OpenDialog1: TOpenDialog
    DefaultExt = 'dll'
    Filter = 
      #1055#1086#1076#1082#1083#1102#1095#1072#1077#1084#1099#1077' '#1084#1086#1076#1091#1083#1080'|*.gkm|'#1044#1080#1085#1072#1084#1080#1095#1077#1089#1082#1080#1077' '#1073#1080#1073#1083#1080#1086#1090#1077#1082#1080'|*.dll|'#1042#1089#1077' '#1092#1072#1081#1083 +
      #1099'|*.*'
    Options = [ofHideReadOnly, ofNoChangeDir, ofPathMustExist, ofFileMustExist, ofEnableSizing]
    Left = 144
    Top = 367
  end
  object MainMenu1: TMainMenu
    Images = ImageList1
    Left = 41
    Top = 366
    object N7: TMenuItem
      Caption = #1053#1072#1089#1090#1088#1086#1081#1082#1072
      object MiWndCopy: TMenuItem
        Caption = #1050#1086#1087#1080#1103' '#1086#1082#1085#1072
        SubMenuImages = ImageList1
        ImageIndex = 8
        OnClick = NewWindowBtnClick
      end
      object MiClose: TMenuItem
        Caption = #1047#1072#1074#1077#1088#1096#1080#1090#1100' '#1085#1072#1089#1090#1088#1086#1081#1082#1091' '#1084#1086#1076#1077#1084#1077#1088#1072
        ImageIndex = 9
        OnClick = MiCloseClick
      end
      object N10: TMenuItem
        Caption = '-'
      end
      object MiAbout: TMenuItem
        Caption = #1054' '#1084#1086#1076#1091#1083#1077
        OnClick = MiAboutClick
      end
    end
    object N1: TMenuItem
      Caption = #1052#1086#1076#1077#1084#1099
      OnClick = N1Click
    end
  end
  object PopupMenu1: TPopupMenu
    Images = ImageList1
    Left = 80
    Top = 366
    object MiAddModem: TMenuItem
      Caption = #1055#1086#1076#1082#1083#1102#1095#1080#1090#1100
      ImageIndex = 6
      OnClick = AddModemBtnClick
    end
    object MiRemoveModem: TMenuItem
      Caption = #1054#1090#1082#1083#1102#1095#1080#1090#1100
      ImageIndex = 7
      OnClick = RemoveModemBtnClick
    end
    object N5: TMenuItem
      Caption = '-'
    end
    object MiStartModem: TMenuItem
      Caption = #1047#1072#1087#1091#1089#1082
      ImageIndex = 0
      OnClick = StartBtnClick
    end
    object MiStopModem: TMenuItem
      Caption = #1054#1089#1090#1072#1085#1086#1074
      ImageIndex = 1
      OnClick = StopBtnClick
    end
    object N6: TMenuItem
      Caption = '-'
    end
    object MiSettings: TMenuItem
      Caption = #1053#1072#1089#1090#1088#1086#1081#1082#1072
      ImageIndex = 2
      OnClick = PropBtnClick
    end
    object MiView: TMenuItem
      Caption = #1055#1088#1086#1089#1084#1086#1090#1088
      ImageIndex = 3
      OnClick = ViewBtnClick
    end
    object N2: TMenuItem
      Caption = '-'
    end
    object MiRefresh: TMenuItem
      Caption = #1054#1073#1085#1086#1074#1080#1090#1100
      ImageIndex = 5
      OnClick = RefreshBtnClick
    end
  end
end
