object TlmViewer: TTlmViewer
  Left = 33
  Top = 125
  BorderIcons = [biSystemMenu]
  BorderStyle = bsToolWindow
  Caption = 'TlmViewer'
  ClientHeight = 36
  ClientWidth = 713
  Color = clBtnFace
  Font.Charset = RUSSIAN_CHARSET
  Font.Color = clWindowText
  Font.Height = -13
  Font.Name = 'Times New Roman'
  Font.Style = [fsBold]
  Menu = MainMenu1
  OldCreateOrder = False
  OnActivate = FormActivate
  OnClose = FormClose
  OnDestroy = FormDestroy
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 15
  object MainMenu1: TMainMenu
    Left = 624
    object N1: TMenuItem
      Caption = #1055#1088#1086#1075#1088#1072#1084#1084#1072
      object N5: TMenuItem
        Caption = #1054' '#1087#1088#1086#1075#1088#1072#1084#1084#1077
        OnClick = N5Click
      end
      object N4: TMenuItem
        Caption = '-'
      end
      object N2: TMenuItem
        Caption = #1047#1072#1074#1077#1088#1096#1080#1090#1100
        OnClick = N2Click
      end
    end
    object N3: TMenuItem
      Caption = #1057#1086#1077#1076#1080#1085#1077#1085#1080#1077
      object ConnProps: TMenuItem
        Caption = #1055#1072#1088#1072#1084#1077#1090#1088#1099
        OnClick = ConnPropsClick
      end
      object DoConnect: TMenuItem
        Caption = #1055#1086#1076#1082#1083#1102#1095#1080#1090#1100#1089#1103
        OnClick = DoConnectClick
      end
      object DoDisconnect: TMenuItem
        Caption = #1054#1090#1082#1083#1102#1095#1080#1090#1100#1089#1103
        OnClick = DoDisconnectClick
      end
    end
    object N7: TMenuItem
      Caption = #1050#1072#1076#1088#1099
      OnClick = N7Click
    end
  end
  object KadrsPopupMenu: TPopupMenu
    Left = 664
  end
  object FlashTimer: TTimer
    Interval = 333
    OnTimer = FlashTimerTimer
    Left = 568
  end
  object PopupMenu1: TPopupMenu
    Left = 528
    object ReadyMoveLeft: TMenuItem
      Caption = #1055#1077#1088#1077#1084#1077#1089#1090#1080#1090#1100' '#1074#1083#1077#1074#1086
    end
    object ReadyMoveRight: TMenuItem
      Caption = #1055#1077#1088#1077#1084#1077#1089#1090#1080#1090#1100' '#1074' '#1087#1088#1072#1074#1086
    end
    object N6: TMenuItem
      Caption = '-'
    end
    object SaveLauout: TMenuItem
      Caption = #1057#1086#1093#1088#1072#1085#1080#1090#1100' '#1088#1072#1089#1087#1086#1083#1086#1078#1077#1085#1080#1077
    end
  end
end
