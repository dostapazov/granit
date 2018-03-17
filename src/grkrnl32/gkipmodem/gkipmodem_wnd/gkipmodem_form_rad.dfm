inherited GKIpModemForm: TGKIpModemForm
  Caption = 'GKIpModemForm'
  ExplicitWidth = 419
  ExplicitHeight = 362
  PixelsPerInch = 96
  TextHeight = 13
  object PageControl1: TPageControl
    Left = 0
    Top = 0
    Width = 411
    Height = 335
    ActivePage = SettingsPage
    Align = alClient
    TabOrder = 0
    ExplicitLeft = -32
    ExplicitWidth = 443
    ExplicitHeight = 257
    object SettingsPage: TTabSheet
      Caption = #1053#1072#1089#1090#1088#1086#1081#1082#1080
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 435
      ExplicitHeight = 229
      object Label3: TLabel
        Left = 111
        Top = 36
        Width = 76
        Height = 13
        Caption = #1040#1076#1088#1077#1089' '#1089#1077#1088#1074#1077#1088#1072
        Visible = False
      end
      object Label1: TLabel
        Left = 7
        Top = 36
        Width = 66
        Height = 13
        Caption = #1053#1086#1084#1077#1088' '#1087#1086#1088#1090#1072
      end
      object Label2: TLabel
        Left = 8
        Top = 7
        Width = 77
        Height = 13
        Caption = #1053#1086#1084#1077#1088' '#1084#1086#1076#1077#1084#1072
      end
      object ServerAddr: TEdit
        Left = 108
        Top = 54
        Width = 277
        Height = 21
        BevelInner = bvLowered
        BevelKind = bkFlat
        BorderStyle = bsNone
        TabOrder = 4
        Visible = False
      end
      object PortNumber: TEdit
        Left = 4
        Top = 54
        Width = 89
        Height = 21
        BevelInner = bvLowered
        BevelKind = bkFlat
        BorderStyle = bsNone
        TabOrder = 3
      end
      object AddressBox: TGroupBox
        Left = 107
        Top = 85
        Width = 185
        Height = 125
        TabOrder = 10
        Visible = False
        object AddressList: TListView
          Left = 2
          Top = 18
          Width = 181
          Height = 103
          BevelInner = bvNone
          BevelOuter = bvNone
          BevelKind = bkFlat
          Checkboxes = True
          Columns = <
            item
              Width = 160
            end>
          ColumnClick = False
          GridLines = True
          ReadOnly = True
          RowSelect = True
          ShowColumnHeaders = False
          TabOrder = 0
          ViewStyle = vsReport
        end
      end
      object AllAddress: TCheckBox
        Left = 115
        Top = 80
        Width = 169
        Height = 17
        Caption = #1053#1072' '#1074#1089#1077#1093' '#1076#1086#1089#1090#1091#1087#1085#1099#1093' '#1072#1076#1088#1077#1089#1072#1093
        Checked = True
        State = cbChecked
        TabOrder = 9
      end
      object ApplyBtn: TBitBtn
        Left = 3
        Top = 88
        Width = 90
        Height = 25
        Caption = #1055#1088#1080#1084#1077#1085#1080#1090#1100
        Default = True
        Glyph.Data = {
          DE010000424DDE01000000000000760000002800000024000000120000000100
          0400000000006801000000000000000000001000000000000000000000000000
          80000080000000808000800000008000800080800000C0C0C000808080000000
          FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
          3333333333333333333333330000333333333333333333333333F33333333333
          00003333344333333333333333388F3333333333000033334224333333333333
          338338F3333333330000333422224333333333333833338F3333333300003342
          222224333333333383333338F3333333000034222A22224333333338F338F333
          8F33333300003222A3A2224333333338F3838F338F33333300003A2A333A2224
          33333338F83338F338F33333000033A33333A222433333338333338F338F3333
          0000333333333A222433333333333338F338F33300003333333333A222433333
          333333338F338F33000033333333333A222433333333333338F338F300003333
          33333333A222433333333333338F338F00003333333333333A22433333333333
          3338F38F000033333333333333A223333333333333338F830000333333333333
          333A333333333333333338330000333333333333333333333333333333333333
          0000}
        NumGlyphs = 2
        TabOrder = 5
      end
      object UndoBtn: TBitBtn
        Left = 3
        Top = 121
        Width = 90
        Height = 25
        Caption = #1054#1090#1084#1077#1085#1080#1090#1100
        Glyph.Data = {
          DE010000424DDE01000000000000760000002800000024000000120000000100
          0400000000006801000000000000000000001000000000000000000000000000
          80000080000000808000800000008000800080800000C0C0C000808080000000
          FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333444444
          33333333333F8888883F33330000324334222222443333388F3833333388F333
          000032244222222222433338F8833FFFFF338F3300003222222AAAAA22243338
          F333F88888F338F30000322222A33333A2224338F33F8333338F338F00003222
          223333333A224338F33833333338F38F00003222222333333A444338FFFF8F33
          3338888300003AAAAAAA33333333333888888833333333330000333333333333
          333333333333333333FFFFFF000033333333333344444433FFFF333333888888
          00003A444333333A22222438888F333338F3333800003A2243333333A2222438
          F38F333333833338000033A224333334422224338338FFFFF8833338000033A2
          22444442222224338F3388888333FF380000333A2222222222AA243338FF3333
          33FF88F800003333AA222222AA33A3333388FFFFFF8833830000333333AAAAAA
          3333333333338888883333330000333333333333333333333333333333333333
          0000}
        NumGlyphs = 2
        TabOrder = 6
      end
      object ModemNumber: TEdit
        Left = 92
        Top = 4
        Width = 72
        Height = 21
        BevelInner = bvLowered
        BevelKind = bkFlat
        BorderStyle = bsNone
        TabOrder = 0
      end
      object AsClient: TRadioButton
        Left = 255
        Top = 6
        Width = 65
        Height = 17
        Caption = #1050#1083#1080#1077#1085#1090
        TabOrder = 2
      end
      object AsServer: TRadioButton
        Left = 175
        Top = 7
        Width = 78
        Height = 17
        Caption = #1057#1077#1088#1074#1077#1088
        TabOrder = 1
      end
      object StartBtn: TBitBtn
        Left = 4
        Top = 153
        Width = 90
        Height = 25
        Caption = #1057#1090#1072#1088#1090
        Glyph.Data = {
          76010000424D7601000000000000760000002800000020000000100000000100
          04000000000000010000130B0000130B00001000000000000000000000000000
          800000800000008080008000000080008000808000007F7F7F00BFBFBF000000
          FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
          33333333333333333333EEEEEEEEEEEEEEE333FFFFFFFFFFFFF3E00000000000
          00E337777777777777F3E0F77777777770E337F33333333337F3E0F333333333
          70E337F3333F333337F3E0F33303333370E337F3337FF33337F3E0F333003333
          70E337F33377FF3337F3E0F33300033370E337F333777FF337F3E0F333000033
          70E337F33377773337F3E0F33300033370E337F33377733337F3E0F333003333
          70E337F33377333337F3E0F33303333370E337F33373333337F3E0F333333333
          70E337F33333333337F3E0FFFFFFFFFFF0E337FFFFFFFFFFF7F3E00000000000
          00E33777777777777733EEEEEEEEEEEEEEE33333333333333333}
        NumGlyphs = 2
        TabOrder = 7
      end
      object StopBtn: TBitBtn
        Left = 3
        Top = 185
        Width = 90
        Height = 25
        Caption = #1057#1090#1086#1087
        Glyph.Data = {
          76010000424D7601000000000000760000002800000020000000100000000100
          04000000000000010000130B0000130B00001000000000000000000000000000
          800000800000008080008000000080008000808000007F7F7F00BFBFBF000000
          FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
          33333333333333333333EEEEEEEEEEEEEEE333FFFFFFFFFFFFF3E00000000000
          00E337777777777777F3E0F77777777770E337F33333333337F3E0F333333333
          70E337F33333333337F3E0F33333333370E337F333FFFFF337F3E0F330000033
          70E337F3377777F337F3E0F33000003370E337F3377777F337F3E0F330000033
          70E337F3377777F337F3E0F33000003370E337F3377777F337F3E0F330000033
          70E337F33777773337F3E0F33333333370E337F33333333337F3E0F333333333
          70E337F33333333337F3E0FFFFFFFFFFF0E337FFFFFFFFFFF7F3E00000000000
          00E33777777777777733EEEEEEEEEEEEEEE33333333333333333}
        NumGlyphs = 2
        TabOrder = 8
      end
    end
    object TabSheet2: TTabSheet
      Caption = #1057#1087#1080#1089#1086#1082' '#1083#1080#1085#1080#1081
      ImageIndex = 1
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      DesignSize = (
        403
        307)
      object LinesList: TListView
        Left = 1
        Top = 3
        Width = 402
        Height = 302
        Anchors = [akLeft, akTop, akRight, akBottom]
        Columns = <
          item
            Caption = #1053#1086#1084#1077#1088
          end
          item
            Caption = #1051#1086#1082#1072#1083#1100#1085#1099#1081' '#1072#1076#1088#1077#1089
            Width = 150
          end
          item
            Caption = #1040#1076#1088#1077#1089' '#1093#1086#1089#1090#1072
            Width = 150
          end>
        GridLines = True
        HideSelection = False
        ReadOnly = True
        RowSelect = True
        TabOrder = 0
        ViewStyle = vsReport
      end
    end
    object TabSheet1: TTabSheet
      Caption = #1054' '#1084#1086#1076#1091#1083#1077
      ImageIndex = 2
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object Label4: TLabel
        Left = 8
        Top = 32
        Width = 177
        Height = 25
        AutoSize = False
        Caption = #1052#1086#1076#1077#1084' TCP/IP '#1082#1083#1080#1077#1085#1090' '#1089#1077#1088#1074#1077#1088'  '
        WordWrap = True
      end
      object Label30: TLabel
        Left = 8
        Top = 8
        Width = 232
        Height = 13
        Caption = #1054#1089#1090#1072#1087#1077#1085#1082#1086' '#1044'.'#1042'. '#1075'.'#1053#1086#1088#1080#1083#1100#1089#1082' - .'#1040#1079#1086#1074' 1993-2007'
      end
    end
  end
end
