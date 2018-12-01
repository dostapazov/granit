object IpModemSampleFrm: TIpModemSampleFrm
  Left = 155
  Top = 187
  Caption = #1055#1088#1080#1084#1077#1088' '#1088#1072#1073#1086#1090#1099' '#1089' '#1094#1077#1085#1090#1088#1086#1084' '#1087#1086' TCP/IP'
  ClientHeight = 464
  ClientWidth = 638
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Scaled = False
  OnResize = FormResize
  OnShow = FormShow
  DesignSize = (
    638
    464)
  PixelsPerInch = 96
  TextHeight = 13
  object StatusBar1: TStatusBar
    Left = 0
    Top = 443
    Width = 638
    Height = 21
    Panels = <
      item
        Width = 50
      end>
  end
  object MonitorBox: TGroupBox
    Left = 0
    Top = 147
    Width = 636
    Height = 297
    Anchors = [akLeft, akTop, akRight, akBottom]
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    TabOrder = 1
  end
  object GroupBox1: TGroupBox
    Left = 0
    Top = -3
    Width = 636
    Height = 151
    Anchors = [akLeft, akTop, akRight]
    TabOrder = 2
    object PageControl1: TPageControl
      Left = 221
      Top = 15
      Width = 413
      Height = 134
      ActivePage = TabSheet1
      Align = alRight
      TabOrder = 2
      OnChange = PageControl1Change
      object TabSheet1: TTabSheet
        Caption = 'OTD'
        ExplicitLeft = 0
        ExplicitTop = 0
        ExplicitWidth = 0
        ExplicitHeight = 0
        object CheckBox8: TCheckBox
          Tag = 1024
          Left = 0
          Top = -1
          Width = 166
          Height = 17
          Caption = #1044#1080#1072#1075#1085#1086#1089#1090#1080#1082#1072' '
          TabOrder = 0
          OnClick = CheckBox8Click
        end
        object CheckBox1: TCheckBox
          Tag = 512
          Left = 0
          Top = 14
          Width = 166
          Height = 17
          Caption = #1044#1072#1085#1085#1099#1077' '
          TabOrder = 1
          OnClick = CheckBox8Click
        end
        object CheckBox2: TCheckBox
          Tag = 32
          Left = 0
          Top = 29
          Width = 166
          Height = 17
          Caption = #1052#1077#1090#1082#1072' '#1074#1088#1077#1084#1077#1085#1080
          TabOrder = 2
          OnClick = CheckBox8Click
        end
        object CheckBox3: TCheckBox
          Tag = 256
          Left = 0
          Top = 45
          Width = 166
          Height = 15
          Caption = #1055#1077#1088#1089#1086#1085#1072#1083#1100#1085#1072#1103' '#1076#1080#1072#1075#1085#1086#1089#1090#1080#1082#1072
          TabOrder = 3
          OnClick = CheckBox8Click
        end
        object CheckBox4: TCheckBox
          Tag = 128
          Left = 0
          Top = 59
          Width = 166
          Height = 17
          Caption = #1052#1072#1089#1082#1072' '#1080#1079#1084#1077#1085#1077#1085#1080#1081
          TabOrder = 4
          OnClick = CheckBox8Click
        end
        object CheckBox5: TCheckBox
          Tag = 64
          Left = 0
          Top = 74
          Width = 166
          Height = 17
          Caption = #1048#1084#1077#1085#1072
          TabOrder = 5
          OnClick = CheckBox8Click
        end
        object CheckBox9: TCheckBox
          Tag = 2
          Left = 0
          Top = 91
          Width = 166
          Height = 17
          Caption = #1063#1072#1089#1090#1080#1095#1085#1099#1077' '#1076#1072#1085#1085#1099#1077
          TabOrder = 6
          OnClick = CheckBox8Click
        end
        object Button1: TButton
          Left = 168
          Top = 0
          Width = 100
          Height = 24
          Caption = #1042#1099#1079#1086#1074
          Enabled = False
          TabOrder = 7
          OnClick = Button1Click
        end
        object Button2: TButton
          Left = 168
          Top = 29
          Width = 100
          Height = 24
          Caption = #1055#1086#1076#1087#1080#1089#1082#1072
          Enabled = False
          TabOrder = 8
          OnClick = Button1Click
        end
        object Button4: TButton
          Left = 168
          Top = 59
          Width = 100
          Height = 24
          Caption = #1054#1090#1082#1072#1079' '#1086#1090' '#1087#1086#1076#1087#1080#1089#1082#1080
          Enabled = False
          TabOrder = 9
          OnClick = Button1Click
        end
        object AddrMask: TEdit
          Left = 272
          Top = 0
          Width = 115
          Height = 21
          TabOrder = 10
          Text = '255.255.255.255'
        end
        object CheckBox7: TCheckBox
          Left = 271
          Top = 25
          Width = 134
          Height = 17
          Caption = #1055#1077#1088#1080#1086#1076#1080#1095#1077#1089#1082#1080#1081' '#1074#1099#1079#1086#1074
          TabOrder = 11
          OnClick = CheckBox7Click
        end
        object CallPeriod: TEdit
          Left = 273
          Top = 46
          Width = 64
          Height = 21
          TabOrder = 12
          Text = '5'
          Visible = False
          OnChange = CallPeriodChange
        end
        object CheckBox10: TCheckBox
          Left = 275
          Top = 74
          Width = 102
          Height = 17
          Caption = #1040#1074#1090#1086#1087#1086#1076#1087#1080#1089#1082#1072' '
          TabOrder = 13
        end
      end
      object TabSheet2: TTabSheet
        Caption = 'OTD_MEDIUM'
        ImageIndex = 1
        ExplicitLeft = 0
        ExplicitTop = 0
        ExplicitWidth = 0
        ExplicitHeight = 0
        object Label3: TLabel
          Left = 0
          Top = 8
          Width = 44
          Height = 13
          Caption = 'ID-'#1082#1072#1076#1088#1072
        end
        object Button5: TButton
          Left = 120
          Top = 8
          Width = 129
          Height = 20
          Caption = #1057#1087#1080#1089#1086#1082' '#1082#1072#1076#1088#1086#1074
          TabOrder = 0
          OnClick = Button5Click
        end
        object Button6: TButton
          Left = 120
          Top = 32
          Width = 129
          Height = 20
          Caption = #1048#1079#1086#1073#1088#1072#1078#1077#1085#1080#1077' '#1082#1072#1076#1088#1072
          TabOrder = 1
          OnClick = Button6Click
        end
        object KadrID: TEdit
          Left = 2
          Top = 27
          Width = 97
          Height = 21
          TabOrder = 2
        end
        object Button7: TButton
          Left = 120
          Top = 58
          Width = 129
          Height = 20
          Caption = #1057#1086#1076#1077#1088#1078#1080#1084#1086#1077' '#1082#1072#1076#1088#1072
          TabOrder = 3
          OnClick = Button7Click
        end
        object Button8: TButton
          Left = 120
          Top = 82
          Width = 129
          Height = 19
          Caption = #1047#1072#1087#1080#1089#1080' '#1082#1072#1076#1088#1072
          TabOrder = 4
          OnClick = Button8Click
        end
        object Button9: TButton
          Left = 256
          Top = 8
          Width = 138
          Height = 20
          Caption = #1055#1086#1076#1087#1080#1089#1082#1072' '#1085#1072' '#1082#1072#1076#1088
          TabOrder = 5
          OnClick = Button9Click
        end
        object Button10: TButton
          Left = 256
          Top = 32
          Width = 138
          Height = 20
          Caption = #1054#1090#1087#1080#1089#1082#1072' '#1086#1090' '#1082#1072#1076#1088#1072
          TabOrder = 6
          OnClick = Button9Click
        end
      end
      object TabSheet3: TTabSheet
        Caption = 'OTD_ARCHIVE'
        ImageIndex = 2
        ExplicitLeft = 0
        ExplicitTop = 0
        ExplicitWidth = 0
        ExplicitHeight = 0
        object Button11: TButton
          Left = 2
          Top = 1
          Width = 113
          Height = 21
          Caption = #1057#1087#1080#1089#1086#1082
          TabOrder = 0
        end
        object Button12: TButton
          Left = 2
          Top = 26
          Width = 113
          Height = 21
          Caption = #1055#1086#1083#1091#1095#1080#1090#1100
          TabOrder = 1
        end
        object Button13: TButton
          Left = 2
          Top = 52
          Width = 113
          Height = 21
          Caption = #1055#1086#1083#1091#1095#1080#1090#1100' '#1080' '#1091#1076#1072#1083#1080#1090#1100
          TabOrder = 2
        end
        object Button14: TButton
          Left = 2
          Top = 76
          Width = 113
          Height = 21
          Caption = #1091#1076#1072#1083#1080#1090#1100
          TabOrder = 3
        end
        object ArchList: TListView
          Left = 121
          Top = 0
          Width = 250
          Height = 104
          Columns = <
            item
              Caption = #1040#1088#1093#1080#1074
              Width = 230
            end>
          TabOrder = 4
          ViewStyle = vsReport
        end
      end
    end
    object StartBtn: TBitBtn
      Left = 152
      Top = 16
      Width = 63
      Height = 21
      Caption = #1057#1090#1072#1088#1090
      Glyph.Data = {
        36030000424D3603000000000000360000002800000010000000100000000100
        18000000000000030000120B0000120B00000000000000000000FF00FFFF00FF
        FF00FFFF00FFFF00FF521E156B25087D2C057E2D056D2607551F13FF00FFFF00
        FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF552117552117BF5C15E19855EF
        BD8AF0BF8CE29D5BC26218541F16541F16FF00FFFF00FFFF00FFFF00FFFF00FF
        6C290EAF4704EBB179FFFEF8FCF8F3F2DAC4F2D9C2FCF6F0FFFFFCEEBA87B64E
        07602311FF00FFFF00FFFF00FF732D0DB04602F4D0ABFFFEFED78E50C05208B7
        4100BA4600C05307D48644FCF8F4F8DDBDB64D05541F16FF00FFFF00FF732D0D
        E6AB72FFFFFFCF762BB63C00FFFFFFD17F37B74100BB4700B74100C9691EFCF8
        F6EFBC88541F16FF00FF893406C05C11FFFCFAE1A36AC25001C25002FFFFFFFF
        FFFFD89255BA4300BB4800B74100D58946FFFFFFC16016551F139A3B02DD9554
        FFFFFFD37628CF6716CC600FFFFFFFFFFFFFFFFFFFE3B080BD4C03BA4400C053
        05FCF8F3E29D5A551F13A94403EBBB8AFBF0E7D87A2BD9792AD5701FFFFFFFFF
        FFFFFFFFFFFFFFFFEFD4B6C1550ABA4400F2DAC2EFC08E551F13B04A06EEBF90
        FCF4EBE28A3EE2893CDE8131FFFFFFFFFFFFFFFFFFFFFFFFE5B283BF4F05BA46
        00F3DEC7EFBD8A551F13B04905E9AA6EFFFFFFEDA562EB994EE99144FFFFFFFF
        FFFFFFFFFFE09D5FC04E00BA4600C15607FEFBF8E09752551F13AD4502DD893F
        FFFCF8FAD7B4F6A760F0A056FFFFFFFFFFFFE19652CC600EC55909B84100D995
        56FFFFFEBD5A10551F13FF00FFB24701F8CEA5FFFFFFFED1A5F4A55CFFFFFFE5
        9852D77421CF6816C25001D07931FFFFFFEAAF73501F18FF00FFFF00FFB24701
        E0873BFEE5CBFFFFFFFAD5B2EDA15DE1883BD97B2DD4782AE1A46CFFFFFFF3CC
        A4AC4402501F18FF00FFFF00FFFF00FFB64C04DE873BF8CEA3FFFAF3FFFFFFFE
        F6EEFCF3EAFFFFFFFEF7EEE6A56AAC44035A2214FF00FFFF00FFFF00FFFF00FF
        FF00FFAF4501AF4501DC8840E9A76CEEBB89EBB581DC914DBD590F5D23155D23
        15FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFA34103A54507A6
        46089E40068B3505752D0CFF00FFFF00FFFF00FFFF00FFFF00FF}
      TabOrder = 0
      OnClick = StartBtnClick
    end
    object StopBtn: TBitBtn
      Left = 152
      Top = 43
      Width = 63
      Height = 21
      Caption = #1057#1090#1086#1087
      Glyph.Data = {
        36030000424D3603000000000000360000002800000010000000100000000100
        18000000000000030000120B0000120B00000000000000000000FF00FFFF00FF
        FF00FFFF00FFFF00FF521E156B25087D2C057E2D056D2607551F13FF00FFFF00
        FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF552117552117BF5C15E19855EF
        BD8AF0BF8CE29D5BC26218541F16541F16FF00FFFF00FFFF00FFFF00FFFF00FF
        6C290EAF4704EBB179FFFEF8FCF8F3F2DAC4F2D9C2FCF6F0FFFFFCEEBA87B64E
        07602311FF00FFFF00FFFF00FF732D0DB04602F4D0ABFFFEFED78E50C05208B7
        4100BA4600C05307D48644FCF8F4F8DDBDB64D05541F16FF00FFFF00FF732D0D
        E6AB72FFFFFFCF762BB63C00FFFFFFD17F37B74100BB4700B74100C9691EFCF8
        F6EFBC88541F16FF00FF893406C05C11FFFCFAE1A36AC25001C25002FFFFFFFF
        FFFFD89255BA4300BB4800B74100D58946FFFFFFC16016551F139A3B02DD9554
        FFFFFFD37628CF6716CC600FFFFFFFFFFFFFFFFFFFE3B080BD4C03BA4400C053
        05FCF8F3E29D5A551F13A94403EBBB8AFBF0E7D87A2BD9792AD5701FFFFFFFFF
        FFFFFFFFFFFFFFFFEFD4B6C1550ABA4400F2DAC2EFC08E551F13B04A06EEBF90
        FCF4EBE28A3EE2893CDE8131FFFFFFFFFFFFFFFFFFFFFFFFE5B283BF4F05BA46
        00F3DEC7EFBD8A551F13B04905E9AA6EFFFFFFEDA562EB994EE99144FFFFFFFF
        FFFFFFFFFFE09D5FC04E00BA4600C15607FEFBF8E09752551F13AD4502DD893F
        FFFCF8FAD7B4F6A760F0A056FFFFFFFFFFFFE19652CC600EC55909B84100D995
        56FFFFFEBD5A10551F13FF00FFB24701F8CEA5FFFFFFFED1A5F4A55CFFFFFFE5
        9852D77421CF6816C25001D07931FFFFFFEAAF73501F18FF00FFFF00FFB24701
        E0873BFEE5CBFFFFFFFAD5B2EDA15DE1883BD97B2DD4782AE1A46CFFFFFFF3CC
        A4AC4402501F18FF00FFFF00FFFF00FFB64C04DE873BF8CEA3FFFAF3FFFFFFFE
        F6EEFCF3EAFFFFFFFEF7EEE6A56AAC44035A2214FF00FFFF00FFFF00FFFF00FF
        FF00FFAF4501AF4501DC8840E9A76CEEBB89EBB581DC914DBD590F5D23155D23
        15FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFA34103A54507A6
        46089E40068B3505752D0CFF00FFFF00FFFF00FFFF00FFFF00FF}
      TabOrder = 1
      OnClick = StopBtnClick
    end
    object CheckBox6: TCheckBox
      Left = 152
      Top = 95
      Width = 57
      Height = 17
      Caption = #1055#1072#1091#1079#1072
      TabOrder = 3
    end
    object Button3: TButton
      Left = 150
      Top = 70
      Width = 65
      Height = 19
      Caption = #1054#1095#1080#1089#1090#1080#1090#1100
      TabOrder = 4
      OnClick = Button3Click
    end
    object grp1: TGroupBox
      Left = 0
      Top = 11
      Width = 146
      Height = 102
      Caption = #1057#1077#1088#1074#1077#1088
      TabOrder = 5
      object Label2: TLabel
        Left = 6
        Top = 47
        Width = 25
        Height = 13
        Caption = #1055#1086#1088#1090
      end
      object ServerName: TEdit
        Left = 1
        Top = 20
        Width = 134
        Height = 21
        TabOrder = 0
        Text = '127.0.0.1'
      end
      object ServerPort: TEdit
        Left = 3
        Top = 64
        Width = 133
        Height = 21
        TabOrder = 1
        Text = '45777'
      end
    end
  end
  object Timer1: TTimer
    Enabled = False
    Interval = 100
    OnTimer = Timer1Timer
    Left = 40
    Top = 184
  end
  object ClientSocket1: TClientSocket
    Active = False
    ClientType = ctNonBlocking
    Port = 0
    OnConnect = ClientSocket1Connect
    OnDisconnect = ClientSocket1Disconnect
    OnRead = ClientSocket1Read
    OnError = ClientSocket1Error
    Left = 8
    Top = 184
  end
end
