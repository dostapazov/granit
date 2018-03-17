object Form2: TForm2
  Left = 0
  Top = 0
  Caption = #1056#1077#1076#1072#1082#1090#1086#1088' '#1087#1086#1083#1100#1079#1086#1074#1072#1090#1077#1083#1077#1081' GRKERNEL'
  ClientHeight = 328
  ClientWidth = 616
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  ScreenSnap = True
  PixelsPerInch = 96
  TextHeight = 13
  object pnl1: TPanel
    Left = 0
    Top = 0
    Width = 616
    Height = 76
    Align = alTop
    TabOrder = 0
    object grp1: TGroupBox
      Left = 4
      Top = 3
      Width = 313
      Height = 69
      Caption = #1057#1086#1077#1076#1080#1085#1077#1085#1080#1077
      TabOrder = 0
      object lbl1: TLabel
        Left = 4
        Top = 15
        Width = 37
        Height = 13
        Caption = #1057#1077#1088#1074#1077#1088
      end
      object Label1: TLabel
        Left = 4
        Top = 42
        Width = 25
        Height = 13
        Caption = #1055#1086#1088#1090
      end
      object conn_btn: TSpeedButton
        Left = 133
        Top = 41
        Width = 124
        Height = 22
        Caption = #1055#1086#1076#1082#1083#1102#1095#1080#1090#1100#1089#1103
        Flat = True
        OnClick = conn_btnClick
      end
      object gkServer: TEdit
        Left = 52
        Top = 15
        Width = 254
        Height = 21
        TabOrder = 0
        Text = '127.0.0.1'
      end
      object gkSrvPort: TEdit
        Left = 52
        Top = 42
        Width = 75
        Height = 21
        TabOrder = 1
        Text = '45777'
      end
    end
    object grp2: TGroupBox
      Left = 320
      Top = 3
      Width = 291
      Height = 69
      Caption = #1042#1093#1086#1076' '#1072#1076#1084#1080#1085#1080#1089#1090#1088#1072#1090#1086#1088#1072
      TabOrder = 1
      Visible = False
      object Label2: TLabel
        Left = 5
        Top = 18
        Width = 72
        Height = 13
        Caption = #1055#1086#1083#1100#1079#1086#1074#1072#1090#1077#1083#1100
      end
      object Label3: TLabel
        Left = 5
        Top = 43
        Width = 37
        Height = 13
        Caption = #1055#1072#1088#1086#1083#1100
      end
      object login_btn: TSpeedButton
        Left = 243
        Top = 15
        Width = 39
        Height = 22
        AllowAllUp = True
        Caption = 'Login'
        Flat = True
        OnClick = login_btnClick
      end
      object user_name: TEdit
        Left = 80
        Top = 16
        Width = 157
        Height = 21
        TabOrder = 0
        Text = 'sysadm'
      end
      object user_password: TEdit
        Left = 79
        Top = 42
        Width = 158
        Height = 21
        PasswordChar = '*'
        TabOrder = 1
      end
    end
  end
  object txt1: TStaticText
    Left = 64
    Top = 88
    Width = 24
    Height = 17
    Caption = 'txt1'
    TabOrder = 1
  end
  object csocket: TClientSocket
    Active = False
    ClientType = ctNonBlocking
    Port = 0
    OnConnect = csocketConnect
    OnDisconnect = csocketDisconnect
    OnRead = csocketRead
    OnError = csocketError
    Left = 8
    Top = 80
  end
end
