inherited cpc152dev_frame: Tcpc152dev_frame
  Width = 448
  Height = 437
  ExplicitWidth = 448
  ExplicitHeight = 437
  DesignSize = (
    448
    437)
  object Label1: TLabel
    Left = 8
    Top = 8
    Width = 50
    Height = 13
    Caption = 'IP - '#1072#1076#1088#1077#1089
  end
  object Label2: TLabel
    Left = 10
    Top = 34
    Width = 48
    Height = 13
    Caption = #1053#1086#1084#1077#1088' '#1050#1055
  end
  object Label3: TLabel
    Left = 133
    Top = 32
    Width = 73
    Height = 13
    Caption = #1053#1072#1080#1084#1077#1085#1086#1074#1072#1085#1080#1077
  end
  object Label4: TLabel
    Left = 184
    Top = 6
    Width = 25
    Height = 13
    Caption = #1055#1086#1088#1090
  end
  object ip_addr: TEdit
    Tag = 1
    Left = 77
    Top = 5
    Width = 101
    Height = 21
    BevelKind = bkFlat
    BorderStyle = bsNone
    TabOrder = 0
    OnChange = ip_addrChange
  end
  object cp_number: TEdit
    Tag = 4
    Left = 77
    Top = 32
    Width = 50
    Height = 21
    BevelKind = bkFlat
    BorderStyle = bsNone
    TabOrder = 2
    OnChange = ip_addrChange
  end
  object cp_name: TEdit
    Tag = 8
    Left = 214
    Top = 30
    Width = 189
    Height = 21
    BevelKind = bkFlat
    BorderStyle = bsNone
    TabOrder = 3
    OnChange = ip_addrChange
  end
  object GroupBox1: TGroupBox
    Left = 4
    Top = 54
    Width = 441
    Height = 197
    Anchors = [akLeft, akTop, akRight]
    Caption = #1056#1072#1089#1095#1077#1090' '#1072#1085#1072#1083#1086#1075#1086#1074#1099#1093' '#1076#1072#1085#1085#1099#1093
    TabOrder = 4
    object Label5: TLabel
      Left = 5
      Top = 16
      Width = 42
      Height = 13
      Caption = #1063#1072#1089#1090#1086#1090#1072
    end
    object Label6: TLabel
      Left = 125
      Top = 16
      Width = 78
      Height = 13
      Caption = #1044#1080#1089#1082#1088#1077#1090#1080#1079#1072#1094#1080#1103
    end
    object Label7: TLabel
      Left = 9
      Top = 40
      Width = 45
      Height = 13
      Caption = #1054#1090#1089#1095#1077#1090#1099
    end
    object Label8: TLabel
      Left = 125
      Top = 46
      Width = 57
      Height = 13
      Caption = #1056#1072#1079#1084#1077#1088' DFT'
    end
    object Label12: TLabel
      Left = 275
      Top = 16
      Width = 95
      Height = 13
      Caption = #1063#1072#1089#1090#1086#1090#1085#1099#1077' '#1074#1086#1088#1086#1090#1072
    end
    object Label13: TLabel
      Left = 290
      Top = 40
      Width = 88
      Height = 13
      Caption = '% '#1040#1084#1087#1083#1080#1090#1091#1076' '#1091#1089#1088'.'
    end
    object Freq: TEdit
      Tag = 16
      Left = 64
      Top = 13
      Width = 49
      Height = 21
      BevelKind = bkFlat
      BorderStyle = bsNone
      TabOrder = 0
      OnChange = ip_addrChange
    end
    object SampleFreq: TEdit
      Tag = 32
      Left = 209
      Top = 13
      Width = 59
      Height = 21
      BevelKind = bkFlat
      BorderStyle = bsNone
      TabOrder = 1
      OnChange = ip_addrChange
    end
    object SamplesCount: TEdit
      Tag = 64
      Left = 65
      Top = 40
      Width = 48
      Height = 21
      BevelKind = bkFlat
      BorderStyle = bsNone
      TabOrder = 2
      OnChange = ip_addrChange
    end
    object FrameSize: TEdit
      Tag = 128
      Left = 209
      Top = 40
      Width = 59
      Height = 21
      BevelKind = bkFlat
      BorderStyle = bsNone
      Enabled = False
      TabOrder = 3
      Text = '0'
      OnChange = ip_addrChange
    end
    object UpDown1: TUpDown
      Left = 267
      Top = 40
      Width = 17
      Height = 21
      Min = 9
      Max = 17
      Position = 12
      TabOrder = 4
      OnChangingEx = UpDown1ChangingEx
    end
    object FrqGate: TEdit
      Tag = 256
      Left = 381
      Top = 13
      Width = 44
      Height = 21
      BevelKind = bkFlat
      BorderStyle = bsNone
      TabOrder = 5
      OnChange = ip_addrChange
    end
    object AmplPercent: TEdit
      Tag = 512
      Left = 381
      Top = 37
      Width = 44
      Height = 21
      BevelKind = bkFlat
      BorderStyle = bsNone
      TabOrder = 6
      OnChange = ip_addrChange
    end
    object CalcVariant: TRadioGroup
      Left = 3
      Top = 62
      Width = 430
      Height = 36
      Caption = #1057#1087#1086#1089#1086#1073' '#1088#1072#1089#1095#1077#1090#1072
      Columns = 5
      ItemIndex = 0
      Items.Strings = (
        'RMS'
        #1044#1077#1081#1089#1090#1074#1091#1102#1097#1077#1077
        #1040#1084#1087#1083#1080#1090#1091#1076#1072
        'max'
        'min')
      TabOrder = 7
      OnClick = CalcVariantClick
    end
    object CorrEnable: TCheckBox
      Left = 5
      Top = 99
      Width = 158
      Height = 15
      Caption = #1056#1072#1079#1088#1077#1096#1080#1090#1100' '#1082#1086#1088#1088#1077#1082#1094#1080#1102
      Checked = True
      State = cbChecked
      TabOrder = 8
      OnClick = CorrEnableClick
    end
    object CorrectionBox: TGroupBox
      Left = 3
      Top = 113
      Width = 430
      Height = 81
      TabOrder = 9
      object Label15: TLabel
        Left = 6
        Top = 12
        Width = 234
        Height = 13
        Caption = #1050#1074#1072#1085#1090#1086#1074' '#1087#1088#1080' '#1084#1072#1082#1089#1080#1084#1072#1083#1100#1085#1086#1084' '#1074#1093#1086#1076#1085#1086#1084' '#1079#1085#1072#1095#1077#1085#1080#1080
      end
      object Label16: TLabel
        Left = 6
        Top = 35
        Width = 114
        Height = 13
        Caption = #1050#1074#1072#1085#1090#1086#1074' '#1076#1086#1083#1078#1085#1086' '#1073#1099#1090#1100
      end
      object Label17: TLabel
        Left = 6
        Top = 59
        Width = 82
        Height = 13
        Caption = #1050#1086#1088#1088#1077#1082#1094#1080#1103' '#1085#1086#1083#1103
      end
      object CorrMaxKvants: TEdit
        Tag = 1024
        Left = 246
        Top = 9
        Width = 96
        Height = 21
        BevelKind = bkFlat
        BorderStyle = bsNone
        TabOrder = 0
        OnChange = ip_addrChange
      end
      object CorrMustKvants: TEdit
        Tag = 2048
        Left = 246
        Top = 32
        Width = 96
        Height = 21
        BevelKind = bkFlat
        BorderStyle = bsNone
        TabOrder = 1
        OnChange = ip_addrChange
      end
      object CorrZero: TEdit
        Tag = 4096
        Left = 246
        Top = 56
        Width = 96
        Height = 21
        BevelKind = bkFlat
        BorderStyle = bsNone
        TabOrder = 2
        OnChange = ip_addrChange
      end
    end
  end
  object ip_port: TEdit
    Tag = 2
    Left = 215
    Top = 3
    Width = 64
    Height = 21
    BevelKind = bkFlat
    BorderStyle = bsNone
    TabOrder = 1
    OnChange = ip_addrChange
  end
  object GroupBox3: TGroupBox
    Left = 3
    Top = 250
    Width = 441
    Height = 41
    Anchors = [akLeft, akTop, akRight]
    TabOrder = 5
    object ms: TLabel
      Left = 349
      Top = 14
      Width = 13
      Height = 13
      Caption = 'ms'
    end
    object Label14: TLabel
      Left = 180
      Top = 13
      Width = 85
      Height = 13
      Caption = #1044#1083#1080#1090#1077#1083#1100#1085#1086#1089#1090#1100#1102' '
    end
    object OscLenght: TEdit
      Left = 272
      Top = 12
      Width = 64
      Height = 21
      BevelKind = bkFlat
      BorderStyle = bsNone
      TabOrder = 0
      Text = '1000'
    end
    object CreateOscilogramm: TButton
      Left = 13
      Top = 8
      Width = 161
      Height = 25
      Caption = #1047#1072#1087#1080#1089#1072#1090#1100' '#1086#1089#1094#1080#1083#1086#1075#1088#1072#1084#1084#1091
      TabOrder = 1
      OnClick = CreateOscilogrammClick
    end
  end
  object page_control: TPageControl
    Left = 4
    Top = 297
    Width = 441
    Height = 137
    ActivePage = dft_enabled_page
    Anchors = [akLeft, akTop, akRight, akBottom]
    TabOrder = 6
    OnChange = page_controlChange
    object TabSheet2: TTabSheet
      AlignWithMargins = True
      Caption = #1048#1085#1080#1094#1080#1072#1090#1080#1074#1085#1099#1077' '#1089#1080#1075#1085#1072#1083#1099
      ImageIndex = 1
      object GroupBox2: TGroupBox
        Left = 0
        Top = 0
        Width = 427
        Height = 103
        Align = alClient
        Caption = #1048#1085#1080#1094#1080#1072#1090#1080#1074#1085#1099#1077' '#1089#1080#1075#1085#1072#1083#1099
        TabOrder = 0
        DesignSize = (
          427
          103)
        object Label9: TLabel
          Left = 9
          Top = 18
          Width = 59
          Height = 13
          Caption = #1059#1089#1090#1088#1086#1081#1089#1090#1074#1086
        end
        object Label10: TLabel
          Left = 111
          Top = 18
          Width = 36
          Height = 13
          Caption = #1043#1088#1091#1087#1087#1072
        end
        object Label11: TLabel
          Left = 189
          Top = 18
          Width = 31
          Height = 13
          Caption = #1053#1086#1084#1077#1088
        end
        object bAdd: TSpeedButton
          Left = 395
          Top = 46
          Width = 23
          Height = 22
          Enabled = False
          Flat = True
          Glyph.Data = {
            F6060000424DF606000000000000360000002800000018000000180000000100
            180000000000C006000000000000000000000000000000000000FFFFFFFFFFFF
            FFFFFFFFFFFFFDFDFDFBFBFBF7F7F7F3F3F3EEEEEEE3E3E3DCDCDCDADADADADA
            DADEDEDEE5E5E5F2F2F2F8F8F8FCFCFCFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
            FFFFFFFFFFFFFFFFFFFFFFF7F7F7E7E7E7D3D3D3BCBCBCA1A1A1888888898989
            8C8C8C9090908E8E8E8A8A8A878787909090B4B4B4DDDDDDF8F8F8FFFFFFFFFF
            FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0F0F0D5D5D5AEAEAE74747489
            8989B5B5B5D0D0D0DBDBDBE1E1E1D9D9D9C8C8C8B5B5B59595957F7F7F9C9C9C
            F0F0F0FDFDFDFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF9F9F9D1D1
            D19E9E9ECDCECDDEE7DFC3E3C87ECA885ABD673BB24D3CB14F5AB76A7FBE8BA5
            BFAAADB3AE9A9A9A999999DCDCDCFAFAFAFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
            FFFFFFD6D6D6B2B2B2DADADAC2E1C76BC57826B03A13AF2C14AF3015AE3316AF
            3916B23C15B43E2DB14F62B275A7BCABB5B5B5A7A7A7DFDFDFFFFFFFFFFFFFFF
            FFFFFFFFFFFFFFFFEEEEEEA7A7A7DCDCDCBFDFC434B34816B03115B53516B73C
            17AE4016A54117A64618B44F1AC2551AC6551DBE4F3DB55CC5D3C7CBCBCBA8A8
            A8F8F8F8FFFFFFFFFFFFFFFFFFF6F6F6B3B3B3DDDEDDB7DDBE30B24814B43816
            BB4119C24C1BBD5348B87083BD9988C09E56BE7D1EC35B1FD45F1ED65C1CCF53
            43C161CBE3D0D6D6D6BDBDBDFAFAFAFFFFFFFCFCFCD8D8D8C7C7C7DBEADE57BF
            6C15B23A16BB461AC5531DCD5D1CC25A75C694DCDEDDE4E4E491D0A625C65A1F
            D9581FDC571FDD5321D24B71D184E1EBE3BEBEBEE3E3E3FFFFFFFAFAFACCCCCC
            DBDBDBABDBB51BB13F15B94319C3501CCA5A1ECF5E1BC05572C28DD5D6D5DBDB
            DB8DCB9E24C5501FD94E20DD4C20E04920E2442BCF46BFE7C6D4D4D4CCCCCCFA
            FAFAEDEDEDCBCBCBEFEFEF4EBC6518B74019BC4819BC4E1ABA4E1AB94C19AE44
            7AC48EDEE0DFD9D9D989C29521B43E1CC3381CC8361DD23520E4351FE2326DD3
            79E8E8E8CECECEF4F4F4E4E4E4CFCFCFEFF2EF1DB13C27C04C2AC05247BA6855
            B67054B26D53AE6A99C7A4E6E7E6E9E9E9A9CDAE5AB46750B45C51B95B4BC454
            1FDC2821EC2839D042F2F2F2CDCDCDE9E9E9E0E0E0D4D4D4DCECDE18B53739CA
            5842C960C1DFC8D4E0D7D1DDD3D0DBD2DAE0DBE5E6E5E7E7E7E0E4E0D5E0D6D1
            DED2CBD8CCBFD1C02AD82A22F21F20D923F0F2F0CCCCCCE4E4E4E0E0E0DCDCDC
            D2E8D520BA3853D6685DD970E5F5E7F6F6F6F0F0F0EAEAEAE7E7E7E5E5E5E5E5
            E5E5E5E5E6E6E6E8E8E8E9E9E9E9E9E931E32923F8181DE01CE4EEE5CECECEE1
            E1E1E3E3E3DCDCDCE4EEE524BC3765DD7273E77FCCF7CFE1F9E2DFF7DFD8F0D8
            DFECDFEBEBEBE7E7E7D6E2D5BFE0BDBEE4BCBFE6BDB5E9B22DF12323FB1823DB
            22ECEEECCACACAE4E4E4E9E9E9DDDDDDF0F1F038BE466DDF7589EF8E94F79697
            F79792F7928DED8ABDEDBCF6F7F6F1F1F1A8DFA646DE3F3FF0363FF2363AF531
            24FB1923F8193FD541E5E5E5CACACAECECECF3F3F3DCDCDCECECEC7BCB8362D8
            679FF29FA0FC9E9CFE9998FE9392F58DC3F3C0FDFEFDFDFDFDC3F0C05FEC5841
            FC3737FF2D32FF272FFE2527EE2182D685D2D2D2CDCDCDF6F6F6FAFAFADDDDDD
            E3E3E3BBDDBE56CB5A9FEE9EB1FBAEAFFEABACFFA7A6F8A2CEF7CCFDFEFDFFFF
            FFD5F7D398F4938CFD8781FF7B74FF6E69FB6356DF56BADABBCBCBCBD8D8D8FA
            FAFAFCFCFCE7E7E7DCDCDCDFE5DF7BCC7F72DC73C0F9BDBEFDBBBCFEB8B7FBB3
            D7FAD5FDFEFDFFFFFFDDFADBABF9A6A2FE9D9EFF999AFF957DEF7B8BD68ED4D9
            D4CBCBCBEBEBEBFFFFFFFFFFFFF8F8F8D9D9D9E0E0E0DBE4DC6FCB7588E38ABF
            F7BEC8FDC6C6FDC3CCFDC9D4FBD3D3FBD1C7FCC4BBFDB7B7FEB3AEFDAA8EF18C
            7CD680D5DED6D2D2D2DCDCDCFAFAFAFFFFFFFFFFFFFFFFFFF4F4F4D6D6D6E5E5
            E5E3ECE47FD48476DB79A8F2A8CAFDC8CAFEC8CAFEC7C9FEC6C8FEC5C6FEC4AB
            F9A87EE77F85DB88DAE3DBDBDBDBD4D4D4F6F6F6FFFFFFFFFFFFFFFFFFFFFFFF
            FFFFFFEBEBEBDDDDDDEEEEEEEBF4ECBAE6BD75D27B5DD86089EC89A6F5A3A3F6
            A18EF18C6BE46B74D778B5E4B8DFE8E0E0E0E0D6D6D6EFEFEFFFFFFFFFFFFFFF
            FFFFFFFFFFFFFFFFFFFFFFFCFCFCF0F0F0DFDFDFEEEEEEF8F8F8FCFCFCE5F4E6
            B4E0B897D79C97D89DB3E1B7E2F2E4F5F5F5EAEAEADFDFDFD9D9D9F4F4F4FFFF
            FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF6F6F6E5E5E5E5
            E5E5E9E9E9F4F4F4F9F9F9FAFAFAFAFAFAF9F9F9EFEFEFDFDFDFDEDEDEE4E4E4
            F8F8F8FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
            FFFFFFFFFCFCFCF5F5F5EBEBEBE4E4E4E0E0E0DEDEDEDEDEDEE0E0E0E3E3E3EA
            EAEAF5F5F5FCFCFCFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF}
          OnClick = bAddClick
        end
        object bRemove: TSpeedButton
          Left = 395
          Top = 74
          Width = 23
          Height = 22
          Enabled = False
          Flat = True
          Glyph.Data = {
            F6060000424DF606000000000000360000002800000018000000180000000100
            180000000000C006000000000000000000000000000000000000FFFFFFFFFFFF
            FFFFFFFFFFFFFDFDFDFBFBFBF7F7F7F3F3F3EEEEEEE3E3E3DCDCDCDADADADADA
            DADEDEDEE5E5E5F2F2F2F8F8F8FCFCFCFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
            FFFFFFFFFFFFFFFFFFFFFFF7F7F7E7E7E7D3D3D3BCBCBCA1A1A1888888898989
            8C8C8C9090908E8E8E8A8A8A878787909090B4B4B4DDDDDDF8F8F8FFFFFFFFFF
            FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0F0F0D5D5D5AEAEAE74747489
            8989B5B5B5D0D0D0DBDBDBE1E1E1D9D9D9C8C8C8B5B5B59595957F7F7F9C9C9C
            F0F0F0FDFDFDFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF9F9F9D1D1
            D19E9E9ECDCDCEDDDEE4C0C3DB767EB8505AA62F3D9B313E9A515BA37981B1A3
            A6B9ACADB19A9A9A999999DCDCDCFAFAFAFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
            FFFFFFD6D6D6B2B2B2DADADABFC2D8626CB218279B0215A20218A9031AAF041C
            B3041DB1041DAC2134A25B66A3A6A8B6B5B5B5A7A7A7DFDFDFFFFFFFFFFFFFFF
            FFFFFFFFFFFFFFFFEEEEEEA7A7A7DCDCDCBCBFD528369F0519A40319AF031DB9
            0421C00525C70628CD072AD0082CD0082BCC0D2CBA3146A5C3C5CFCBCBCBA8A8
            A8F8F8F8FFFFFFFFFFFFFFFFFFF6F6F6B3B3B3DDDDDEB4B8D42333A1041BAF04
            1FBA0624C4072BD10830DA0A34E00A37E50C39E80D3BEA0D3CEB0D3BE90C37DA
            3850B3C9CDDDD6D6D6BDBDBDFAFAFAFFFFFFFCFCFCD8D8D8C7C7C7DADBE64E5B
            AF061DAA0621BA0728C7092FD40A35DF0B38E50C3BE80D3EEB0E40ED0F42F010
            44F21044F31143F1133DD46A7BC1E0E2E8BEBEBEE3E3E3FFFFFFFAFAFACCCCCC
            DBDBDBA8ADD21025A30621B20827B9092EC40B34CE0C38D50D3AD70E3CDA0E3D
            DB0F3FDD0F40DE1041E01041E11142E41141E41F41C0BCC3DED4D4D4CCCCCCFA
            FAFAEDEDEDCBCBCBEFEFEF4E5CAC1932B41D37B1344EB13652B53452B53552B7
            3755B93957BC3958BD3958BD3958BD3A58BD3A58BD3B5AC4274FD6113CD46678
            BCE8E8E8CECECEF4F4F4E4E4E4CFCFCFEFEFF22B3FAB2E47C08492D1DADDE8D6
            D9E3CBCFD9CBCED9D0D3DECCCFD9CBCED9D0D3DDD3D6E1D4D7E1D3D7E1D4D7E2
            8B9DDD1340DD2E4AB4F2F2F2CDCDCDE9E9E9E0E0E0D4D4D4DBDDE7374DB84059
            CBB4BFE7FEFEFEFBFBFBF8F8F8F6F6F6EDEDEDE3E3E3E2E2E2E7E7E7DEDEDEDD
            DDDDE4E4E4E9E9E9AAB7E41543E51337BCF0F0F1CCCCCCE4E4E4E0E0E0DCDCDC
            D1D3E25265C5566ED7B5C1EDFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFCFC
            FCF3F3F3ECECECEBEBEBEAEAEADDDDDD91A2DE1646F11137C4E3E5EACECECEE1
            E1E1E3E3E3DCDCDCE3E4EB6172C7697FDF899DEBB5C4F9B3C3FAB1C1FAABBDF9
            98AEF98CA4F88BA3F88BA3F78BA3F789A1F6839BF07E96EC456BF11848F8163A
            BEECECEDCACACAE4E4E4E9E9E9DDDDDDF0F0F17280C57D8FE37A91ED7793F872
            91FC6E8FFC6889FC5A7EFB466EFB2655FB184AFB174AFB1749FB1849FB1849FC
            1849FD1746F43450B6E5E5E5CACACAECECECF3F3F3DCDCDCECECECA3A9CE93A0
            E291A2EE8DA3F88AA3FD85A0FF7F9CFF7B98FF7795FF708FFF6083FF4970FF32
            5EFF2957FF2553FF2251FE1D47E17D8DC2D2D2D2CDCDCDF6F6F6FAFAFADDDDDD
            E3E3E3D1D3E0A0A9DAA1AFEC9EAFF69BAFFC97AEFE92A9FE8EA6FF89A2FF849E
            FF7F9AFF7B97FF7592FF6989FF5E80FF5679F95D76D3C2C6D8CBCBCBD8D8D8FA
            FAFAFCFCFCE7E7E7DCDCDCE2E2E5BEC2DCAEB8E7ADBAF3AABAF9A7B9FDA3B6FE
            9FB3FE9BB0FE96ACFF91A9FF8DA5FF87A1FF839DFF7F99FE7A92E9ABB4D6D9D9
            DBCBCBCBEBEBEBFFFFFFFFFFFFF8F8F8D9D9D9E0E0E0DEDFE4C7CBE1BCC4EDBA
            C5F5B7C4FAB4C3FDB1C1FEADBEFEA9BBFFA5B8FFA1B5FF9DB1FE98ADFC93A7EF
            B1BADBDDDEE1D2D2D2DCDCDCFAFAFAFFFFFFFFFFFFFFFFFFF4F4F4D6D6D6E5E5
            E5E8E9ECD1D5E9C3CAECC1CAF4C0CBFABDCAFCBAC8FDB7C6FEB4C3FEAFC0FDAB
            BBF8ABB9ECBFC7E3E4E4E7DBDBDBD4D4D4F6F6F6FFFFFFFFFFFFFFFFFFFFFFFF
            FFFFFFEBEBEBDDDDDDEEEEEEF3F3F6E2E4EFCDD2E9C4CCEDC4CDF2C3CCF5C1CB
            F5BEC9F3BAC5EEC6CDE8DADDEAE8E9EBE0E0E0D6D6D6EFEFEFFFFFFFFFFFFFFF
            FFFFFFFFFFFFFFFFFFFFFFFCFCFCF0F0F0DFDFDFEEEEEEF8F8F8FCFCFCEDEFF4
            DADDEADBDEEBD8DAE8DDDFEBEDEEF4F5F5F5EAEAEADFDFDFD9D9D9F4F4F4FFFF
            FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF6F6F6E5E5E5E5
            E5E5E9E9E9F4F4F4F9F9F9FAFAFAFAFAFAF9F9F9EFEFEFDFDFDFDEDEDEE4E4E4
            F8F8F8FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
            FFFFFFFFFCFCFCF5F5F5EBEBEBE4E4E4E0E0E0DEDEDEDEDEDEE0E0E0E3E3E3EA
            EAEAF5F5F5FCFCFCFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF}
          OnClick = bRemoveClick
        end
        object sDev: TEdit
          Tag = 4
          Left = 73
          Top = 16
          Width = 29
          Height = 21
          BevelKind = bkFlat
          BorderStyle = bsNone
          TabOrder = 0
          OnChange = sDevChange
        end
        object sGrp: TEdit
          Tag = 4
          Left = 153
          Top = 16
          Width = 29
          Height = 21
          BevelKind = bkFlat
          BorderStyle = bsNone
          TabOrder = 1
          OnChange = sDevChange
        end
        object sNum: TEdit
          Tag = 4
          Left = 231
          Top = 16
          Width = 29
          Height = 21
          BevelKind = bkFlat
          BorderStyle = bsNone
          TabOrder = 2
          OnChange = sDevChange
        end
        object sState: TRadioGroup
          Left = 265
          Top = 7
          Width = 124
          Height = 33
          Caption = #1047#1072#1087#1091#1089#1082
          Columns = 2
          Items.Strings = (
            #1042#1082#1083
            #1054#1090#1082#1083)
          TabOrder = 3
          OnClick = sDevChange
        end
        object AlarmList: TListView
          Left = 9
          Top = 43
          Width = 380
          Height = 53
          Anchors = [akLeft, akTop, akBottom]
          Columns = <
            item
              Caption = #1059#1089#1090#1088#1086#1081#1089#1090#1074#1086
              Width = 80
            end
            item
              Caption = #1043#1088#1091#1087#1087#1072
              Width = 70
            end
            item
              Caption = #1053#1086#1084#1077#1088
              Width = 70
            end
            item
              Caption = #1055#1086#1083#1086#1078#1077#1085#1080#1077
              Width = 70
            end
            item
              Caption = #1042#1085#1077#1096#1085#1080#1081
              Width = 70
            end>
          FlatScrollBars = True
          GridLines = True
          HideSelection = False
          OwnerData = True
          RowSelect = True
          TabOrder = 4
          ViewStyle = vsReport
          OnChange = AlarmListChange
          OnData = AlarmListData
          ExplicitHeight = 166
        end
      end
    end
    object dft_enabled_page: TTabSheet
      Caption = #1056#1072#1079#1088#1077#1096#1077#1085#1080#1103' DFT'
      DesignSize = (
        433
        109)
      object Label18: TLabel
        Left = 8
        Top = 6
        Width = 59
        Height = 13
        Caption = #1059#1089#1090#1088#1086#1081#1089#1090#1074#1086
      end
      object Label19: TLabel
        Left = 11
        Top = 33
        Width = 37
        Height = 13
        Caption = #1057#1091#1073#1083#1086#1082
      end
      object SpeedButton1: TSpeedButton
        Left = 8
        Top = 56
        Width = 128
        Height = 22
        Caption = #1056#1072#1079#1088#1077#1096#1080#1090#1100' '#1074#1089#1077
        OnClick = SpeedButton1Click
      end
      object SpeedButton2: TSpeedButton
        Left = 9
        Top = 84
        Width = 128
        Height = 22
        Caption = #1047#1072#1087#1088#1077#1090#1080#1090#1100' '#1074#1089#1077
        OnClick = SpeedButton1Click
      end
      object dev_num: TComboBox
        Left = 80
        Top = 2
        Width = 57
        Height = 21
        BevelKind = bkFlat
        Style = csDropDownList
        ItemIndex = 0
        TabOrder = 0
        Text = '0'
        OnChange = grp_numChange
        Items.Strings = (
          '0'
          '1'
          '2'
          '3'
          '4'
          '5'
          '6'
          '7')
      end
      object grp_num: TComboBox
        Left = 79
        Top = 29
        Width = 57
        Height = 21
        BevelKind = bkFlat
        Style = csDropDownList
        TabOrder = 1
        OnChange = grp_numChange
      end
      object lv_dft_enabled: TListView
        Left = 149
        Top = 3
        Width = 257
        Height = 73
        Anchors = [akLeft, akTop, akBottom]
        Columns = <
          item
            Caption = #1055#1072#1088#1072#1084#1077#1090#1088
            Width = 100
          end
          item
            Caption = #1057#1086#1089#1090#1086#1103#1085#1080#1077
            Width = 150
          end>
        FlatScrollBars = True
        GridLines = True
        OwnerData = True
        ReadOnly = True
        RowSelect = True
        TabOrder = 2
        ViewStyle = vsReport
        OnData = lv_dft_enabledData
        OnDblClick = lv_dft_enabledDblClick
        OnKeyPress = lv_dft_enabledKeyPress
      end
    end
  end
end
