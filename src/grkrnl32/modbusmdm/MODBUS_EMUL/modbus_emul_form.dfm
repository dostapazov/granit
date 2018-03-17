object Form3: TForm3
  Left = 475
  Top = 258
  Caption = #1069#1084#1091#1083#1103#1090#1086#1088' MODBUS'
  ClientHeight = 335
  ClientWidth = 657
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  DesignSize = (
    657
    335)
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 18
    Top = 7
    Width = 25
    Height = 13
    Caption = #1055#1086#1088#1090
  end
  object Label2: TLabel
    Left = 16
    Top = 27
    Width = 97
    Height = 13
    Caption = #1053#1086#1084#1077#1088'  '#1091#1089#1090#1088#1086#1081#1089#1090#1074#1072
  end
  object Label3: TLabel
    Left = 336
    Top = 25
    Width = 34
    Height = 13
    Caption = #1050#1086#1083'-'#1074#1086
  end
  object TcpPort: TEdit
    Left = 48
    Top = 3
    Width = 60
    Height = 21
    TabOrder = 0
    Text = '502'
  end
  object DevNum: TEdit
    Left = 19
    Top = 46
    Width = 86
    Height = 21
    TabOrder = 1
    Text = '1'
  end
  object setup_data: TRadioGroup
    Left = 123
    Top = 32
    Width = 201
    Height = 32
    Caption = #1042#1080#1076
    Columns = 2
    ItemIndex = 0
    Items.Strings = (
      #1044#1080#1089#1082#1088#1077#1090#1085#1099#1081
      #1040#1085#1072#1083#1086#1075#1086#1074#1099#1081)
    TabOrder = 2
    OnClick = setup_dataClick
  end
  object ParamCount: TEdit
    Left = 336
    Top = 40
    Width = 86
    Height = 21
    TabOrder = 3
    Text = '8'
  end
  object StartBtn: TButton
    Left = 121
    Top = 2
    Width = 75
    Height = 25
    Caption = 'Start'
    TabOrder = 4
    OnClick = StartBtnClick
  end
  object StopBtn: TButton
    Left = 202
    Top = 2
    Width = 75
    Height = 25
    Caption = 'Stop'
    Enabled = False
    TabOrder = 5
    OnClick = StopBtnClick
  end
  object ParamList: TListView
    Left = 19
    Top = 74
    Width = 218
    Height = 226
    Anchors = [akLeft, akTop, akBottom]
    Columns = <
      item
        Caption = #1047#1085#1072#1095#1077#1085#1080#1077
        Width = 100
      end
      item
        Caption = #1053#1086#1084#1077#1088
        Width = 100
      end>
    TabOrder = 6
    ViewStyle = vsReport
  end
  object Memo1: TMemo
    Left = 256
    Top = 80
    Width = 383
    Height = 220
    Anchors = [akLeft, akTop, akRight, akBottom]
    TabOrder = 7
  end
  object StatusBar1: TStatusBar
    Left = 0
    Top = 316
    Width = 657
    Height = 19
    Panels = <>
  end
  object SrvSocket: TServerSocket
    Active = False
    Port = 0
    ServerType = stNonBlocking
    OnClientDisconnect = SrvSocketClientDisconnect
    OnClientRead = SrvSocketClientRead
    Left = 488
    Top = 24
  end
end
