inherited GrIpxModuleWnd: TGrIpxModuleWnd
  Left = 435
  Top = 436
  Width = 506
  Height = 380
  Caption = 'GrIpxModuleWnd'
  OldCreateOrder = True
  Position = poScreenCenter
  OnHide = FormHide
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object PageControl1: TPageControl
    Left = 0
    Top = 0
    Width = 498
    Height = 353
    ActivePage = TabSheet1
    Align = alClient
    TabIndex = 0
    TabOrder = 0
    OnChange = PageControl1Change
    object TabSheet1: TTabSheet
      Caption = #1055#1072#1088#1072#1084#1077#1090#1088#1099
      object GroupBox1: TGroupBox
        Left = 0
        Top = 96
        Width = 490
        Height = 51
        Caption = #1057#1077#1088#1074#1077#1088
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 0
        object Label2: TLabel
          Left = 11
          Top = 21
          Width = 22
          Height = 13
          Caption = #1048#1084#1103
        end
        object Label3: TLabel
          Left = 248
          Top = 19
          Width = 56
          Height = 13
          Caption = 'IPX '#1043#1085#1077#1079#1076#1086
        end
        object ServerName: TEdit
          Left = 43
          Top = 18
          Width = 198
          Height = 21
          BevelInner = bvNone
          BevelKind = bkSoft
          BorderStyle = bsNone
          TabOrder = 0
          Text = 'ServerName'
          OnChange = RetryTimeoutEditChange
        end
        object ServerSocket: TEdit
          Left = 320
          Top = 17
          Width = 121
          Height = 21
          BevelInner = bvNone
          BevelKind = bkSoft
          BorderStyle = bsNone
          TabOrder = 1
          Text = 'ServerSocket'
          OnChange = RetryTimeoutEditChange
        end
      end
      object GroupBox2: TGroupBox
        Left = 0
        Top = 0
        Width = 489
        Height = 87
        Caption = #1054#1073#1097#1080#1077
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 1
        object Label1: TLabel
          Left = 129
          Top = 17
          Width = 53
          Height = 13
          Caption = #1053#1086#1084#1077#1088' '#1055#1059
        end
        object Label4: TLabel
          Left = 8
          Top = 40
          Width = 160
          Height = 13
          Caption = #1050#1086#1083#1080#1095#1077#1089#1090#1074#1086' '#1087#1086#1074#1090#1086#1088#1085#1099#1093' '#1087#1077#1088#1077#1076#1072#1095
        end
        object Label5: TLabel
          Left = 8
          Top = 66
          Width = 152
          Height = 13
          Caption = #1058#1072#1081#1084#1072#1091#1090' '#1086#1078#1080#1076#1072#1085#1080#1103' '#1082#1074#1080#1090#1072#1085#1094#1080#1080
        end
        object Label6: TLabel
          Left = 228
          Top = 65
          Width = 13
          Height = 13
          Caption = 'ms'
        end
        object PUNumberEdit: TEdit
          Left = 186
          Top = 15
          Width = 31
          Height = 19
          BevelInner = bvNone
          BevelKind = bkSoft
          BorderStyle = bsNone
          TabOrder = 0
          Text = '0'
        end
        object PUNumber: TUpDown
          Left = 217
          Top = 15
          Width = 15
          Height = 19
          Associate = PUNumberEdit
          Min = 0
          Max = 255
          Position = 0
          TabOrder = 1
          Wrap = False
        end
        object RetryCountEdit: TEdit
          Left = 186
          Top = 39
          Width = 33
          Height = 18
          BevelInner = bvNone
          BevelKind = bkSoft
          BorderStyle = bsNone
          TabOrder = 2
          Text = '0'
        end
        object RetryCount: TUpDown
          Left = 219
          Top = 39
          Width = 15
          Height = 18
          Associate = RetryCountEdit
          Min = 0
          Max = 10
          Position = 0
          TabOrder = 3
          Wrap = False
        end
        object RetryTimeoutEdit: TEdit
          Left = 186
          Top = 62
          Width = 34
          Height = 18
          BevelInner = bvNone
          BevelKind = bkSoft
          BorderStyle = bsNone
          TabOrder = 4
          Text = '0'
          OnChange = RetryTimeoutEditChange
        end
        object ApplyBtn: TBitBtn
          Left = 264
          Top = 16
          Width = 97
          Height = 25
          Caption = #1055#1088#1080#1084#1077#1085#1080#1090#1100
          Default = True
          ModalResult = 6
          TabOrder = 5
          OnClick = ApplyBtnClick
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
        end
        object UndoBtn: TBitBtn
          Left = 368
          Top = 16
          Width = 97
          Height = 25
          Caption = #1054#1090#1084#1077#1085#1080#1090#1100' '
          ModalResult = 4
          TabOrder = 6
          OnClick = UndoBtnClick
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
        end
        object RadioButton1: TRadioButton
          Left = 5
          Top = 16
          Width = 61
          Height = 17
          Caption = #1057#1077#1088#1074#1077#1088
          TabOrder = 7
          OnClick = RadioButton1Click
        end
        object RadioButton2: TRadioButton
          Left = 67
          Top = 16
          Width = 58
          Height = 17
          Caption = #1050#1083#1080#1077#1085#1090
          TabOrder = 8
          OnClick = RadioButton1Click
        end
        object RadioButton3: TRadioButton
          Left = 352
          Top = 56
          Width = 113
          Height = 17
          Caption = 'RadioButton3'
          TabOrder = 9
          Visible = False
        end
      end
      object GroupBox3: TGroupBox
        Left = 0
        Top = 153
        Width = 490
        Height = 51
        Caption = #1050#1083#1080#1077#1085#1090
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 2
        object Label7: TLabel
          Left = 11
          Top = 21
          Width = 22
          Height = 13
          Caption = #1048#1084#1103
        end
        object Label8: TLabel
          Left = 248
          Top = 19
          Width = 56
          Height = 13
          Caption = 'IPX '#1043#1085#1077#1079#1076#1086
        end
        object ClientName: TEdit
          Left = 43
          Top = 18
          Width = 198
          Height = 21
          BevelInner = bvNone
          BevelKind = bkSoft
          BorderStyle = bsNone
          TabOrder = 0
          Text = 'Edit1'
          OnChange = RetryTimeoutEditChange
        end
        object ClientSocket: TEdit
          Left = 320
          Top = 17
          Width = 121
          Height = 21
          BevelInner = bvNone
          BevelKind = bkSoft
          BorderStyle = bsNone
          TabOrder = 1
          Text = 'Edit2'
          OnChange = RetryTimeoutEditChange
        end
      end
    end
    object TabSheet2: TTabSheet
      Caption = #1055#1088#1086#1089#1084#1086#1090#1088
      ImageIndex = 1
      object Splitter1: TSplitter
        Left = 161
        Top = 0
        Width = 3
        Height = 325
        Cursor = crHSplit
      end
      object ClientsTree: TTreeView
        Left = 0
        Top = 0
        Width = 161
        Height = 325
        Align = alLeft
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        Indent = 19
        ParentFont = False
        TabOrder = 0
      end
      object Memo1: TMemo
        Left = 164
        Top = 0
        Width = 326
        Height = 325
        Align = alClient
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Pitch = fpFixed
        Font.Style = []
        Lines.Strings = (
          'Memo1')
        ParentFont = False
        TabOrder = 1
      end
    end
  end
end
