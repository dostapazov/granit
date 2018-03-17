object RecordFrame: TRecordFrame
  Left = 0
  Top = 0
  Width = 515
  Height = 367
  TabOrder = 0
  object ScaleParam: TGroupBox
    Left = 0
    Top = 173
    Width = 356
    Height = 194
    Align = alClient
    Caption = #1055#1072#1088#1072#1084#1077#1090#1088#1099' '#1087#1077#1088#1077#1089#1095#1105#1090#1072' [ '#1085#1077' '#1079#1072#1076#1072#1085#1072' '#1096#1082#1072#1083#1072' ]'
    TabOrder = 1
    object Label7: TLabel
      Left = 8
      Top = 23
      Width = 49
      Height = 13
      Caption = #1064#1082#1072#1083#1072' '#1086#1090
    end
    object Label8: TLabel
      Left = 149
      Top = 26
      Width = 13
      Height = 13
      Caption = #1076#1086
    end
    object Label10: TLabel
      Left = 7
      Top = 72
      Width = 67
      Height = 13
      Caption = #1050#1074#1072#1085#1090#1086#1074#1072#1085#1080#1077
    end
    object ValueMin: TEdit
      Left = 78
      Top = 21
      Width = 63
      Height = 21
      BevelInner = bvNone
      BevelKind = bkFlat
      BorderStyle = bsNone
      TabOrder = 0
      Text = '0'
      OnChange = ValueMinChange
    end
    object ValueMax: TEdit
      Left = 182
      Top = 24
      Width = 63
      Height = 21
      BevelInner = bvNone
      BevelKind = bkFlat
      BorderStyle = bsNone
      TabOrder = 1
      Text = '250'
      OnChange = ValueMinChange
    end
    object Kvants: TEdit
      Left = 79
      Top = 69
      Width = 63
      Height = 21
      BevelInner = bvNone
      BevelKind = bkFlat
      BorderStyle = bsNone
      TabOrder = 3
      Text = '0'
      OnChange = ValueMinChange
    end
    object optInvScale: TCheckBox
      Left = 9
      Top = 46
      Width = 108
      Height = 17
      Caption = #1048#1085#1074#1077#1088#1089#1080#1103' '#1096#1082#1072#1083#1099
      TabOrder = 2
      OnClick = optInvScaleClick
    end
    object optSigned: TCheckBox
      Left = 148
      Top = 71
      Width = 119
      Height = 17
      Caption = #1050#1074#1072#1085#1090#1099' '#1089#1086' '#1079#1085#1072#1082#1086#1084
      TabOrder = 4
      OnClick = optSignedClick
    end
  end
  object CombinedRecordParam: TGroupBox
    Left = 0
    Top = 173
    Width = 356
    Height = 194
    Align = alClient
    TabOrder = 3
    object PageControl1: TPageControl
      Left = 2
      Top = 15
      Width = 352
      Height = 177
      ActivePage = TabSheet1
      Align = alClient
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
      TabOrder = 0
      object TabSheet1: TTabSheet
        Caption = #1057#1086#1089#1090#1072#1074' '#1074#1099#1095#1080#1089#1083#1103#1077#1084#1086#1075#1086
        object Panel1: TPanel
          Left = 0
          Top = 0
          Width = 344
          Height = 43
          Align = alTop
          BevelOuter = bvLowered
          Caption = 'Panel1'
          ShowCaption = False
          TabOrder = 0
          object CombObjAdd: TSpeedButton
            Left = 4
            Top = 4
            Width = 21
            Height = 16
            Hint = #1044#1086#1073#1072#1074#1080#1090#1100' '#1074' '#1089#1087#1080#1089#1086#1082' '#1074#1099#1095#1080#1089#1083#1103#1077#1084#1099#1093
            Enabled = False
            Flat = True
            Glyph.Data = {
              36050000424D3605000000000000360400002800000010000000100000000100
              0800000000000001000000000000000000000001000000010000000000000101
              0100020202000303030004040400050505000606060007070700080808000909
              09000A0A0A000B0B0B000C0C0C000D0D0D000E0E0E000F0F0F00101010001111
              1100121212001313130014141400151515001616160017171700181818001919
              19001A1A1A001B1B1B001C1C1C001D1D1D001E1E1E001F1F1F00202020002121
              2100222222002323230024242400252525002626260027272700282828002929
              29002A2A2A002B2B2B002C2C2C002D2D2D002E2E2E002F2F2F00303030003131
              3100323232003333330034343400353535003636360037373700383838003939
              39003A3A3A003B3B3B003C3C3C003D3D3D003E3E3E003F3F3F00404040004141
              410042424200434343004444440045454500464646004747470059454E006C43
              57008143590093435B00A1425600A1434B009C454100964738008C492F00834A
              2700794C2000704E1A006051140052530F0046560B003B580800325A05002A5B
              0400245D03001C5E020016600100106100000C62000008630000066400000564
              000003640000076B070009700C000B740F000F75120013761300157513001B70
              100028670C00365C0700494E0000594900006A440000754000007D3E0000893A
              0000913700009635000098350000993600009A3600009B3700009E3A0000A23E
              0000A9440000AF4A0000B5500000BC560000C05B0000C5600000C8630000CA65
              0100CB660100CB660100CB660100CB660100CB660100CB660100CB660100CA65
              0100CA650100C9650200C8640300C6640500C5650800C3660C00C1681200BE6B
              1A00BA702700B5783A00AE7E4D00A7856400A28B7300A08E7C009E9285009C96
              8F009A9A9A009B9B9B009C9C9C009D9D9D009E9E9E009F9F9F00A0A0A000A1A1
              A100A2A2A200A3A3A300A4A4A400A5A5A500A6A6A600A7A7A700A8A8A800A9A9
              A900AAAAAA00ABABAB00ACACAC00ADADAD00AEAEAE00AFAFAF00B0B0B000B1B1
              B100B2B2B200B3B3B300B4B4B400B5B5B500B6B6B600B7B7B700B8B8B800B9B9
              B900BABABA00BBBBBB00BCBCBC00BDBDBD00BEBEBE00BFBFBF00C0C0C000C1C1
              C100C2C2C200C6C5C400CAC9C700CFCDCA00D3D0CC00D6D3CE00DCD7D200E1DB
              D400EBE5DD00F3EDE500F8F2EA00FBF5ED00FDF7F000FEF8F100FEF9F200FEF9
              F300FEFAF400FEFAF500FEF9F200FEF8EF00FEF6EC00FEF5EA00FEF4E800FEF3
              E600FEF2E300FEF0E100FEEFDE00FEEEDC00FEEDDB00FEECD800FEEBD700FEEA
              D400FEEAD300FEE7D300FEE1D400FED1D700FEB8DB00FE93E100FE68E900FE4A
              EF00FE21F700FE0CFC00FE03FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00
              FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00
              FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00F6F6F6F6F6F6
              F6F6F6F6F6F6F6F6F6F686868686868686F6F6F6F6F6F6F6F6F6D3D0D8DDE2E3
              86F6F6F6F6F6F6F6F6F6D3D3D6DADFE386F6F6F6F6F6F6F6F6F6D3D3CFD8DDE2
              86F6F6F6F6F6F6F6F6F674747474747486F6F6F6F6F6F6F6F6F6F6F6F6F6F6F6
              F6F662F6867474747474F6F6F6F6F6F6F66262F686D0D8DDE2E3F6F6F6F6F6F6
              629662F686D3D6DADFE3F6F6F6F6F6F6F66262F686D3CFD8DDE2F6F6F6F6F6F6
              F6F662F686747474747474747474747486F6F6F6F6F6F6F6F6F6D3D0D8DDE2E3
              86F6F6F6F6F6F6F6F6F6D3D3D6DADFE386F6F6F6F6F6F6F6F6F6D3D3CFD8DDE2
              86F6F6F6F6F6F6F6F6F686868686868686F6F6F6F6F6F6F6F6F6}
            OnClick = CombObjAddClick
          end
          object CombObjRemove: TSpeedButton
            Left = 27
            Top = 4
            Width = 20
            Height = 16
            Hint = #1059#1073#1088#1072#1090#1100' '#1080#1079' '#1089#1087#1080#1089#1082#1072' '#1074#1099#1095#1080#1089#1083#1103#1077#1084#1099#1093
            Enabled = False
            Flat = True
            Glyph.Data = {
              36050000424D3605000000000000360400002800000010000000100000000100
              0800000000000001000000000000000000000001000000010000000000000101
              0100020202000303030004040400050505000606060007070700080808000909
              09000A0A0A000B0B0B000B0F0B000B140B000B180B000B1C0B000B200B000A26
              0A000A2B0A00092F09000833080007370700063A0600053E0500044004000343
              0300034403000246020002460200024702000247020002460200034603000445
              0400054405000643060008420800094109000B3F0B000E3D0E00113B1100143A
              1400173817001B361B001F341F002233220025332500293229002D312D003131
              3100323232003333330034343400353535003636360037373700383838003939
              39003A3A3A003B3B3B003C3C3C003D3D3D003E3E3E003F3F3F00404040004141
              4100424242004343430044444400454545004646460047474700484848004949
              49004A4A4A004B4B4B004C4C4C004D4D4D004E4E4E004F4F4F00505050005151
              5100525252005353530054545400555555005656560057575700585858005959
              59005A5A5A005B5B5B005C5C5C005D5D5D005E5E5E005F5F5F00606060006161
              6100626262006363630064646400656565006666660067676700686868006969
              69006A6A6A006B6B6B006C6C6C006D6D6D006E6E6E006F6F6F00707070007171
              7100727272007373730074747400757575007676760077777700787878007979
              79007A7A7A007B7B7B007C7C7C007D7D7D007E7E7E007F7F7F00808080008181
              8100828282008383830084848400858585008686860087878700888888008989
              89008A8A8A008B8B8B008C8C8C008D8D8D008E8E8E008F8F8F00909090009191
              9100929292009393930094949400959595009696960097979700989898009999
              99009A9A9A009B9B9B009C9C9C009D9D9D009E9E9E009F9F9F00A0A0A000A1A1
              A100A2A2A200A3A3A300A4A4A400A5A5A500A6A6A600A7A7A700A8A8A800A9A9
              A900AAAAAA00ABABAB00B1B0AF00BFBDBB00CECBC800E0DDD900EDEAE700F6F3
              EF00FBF7F300FDF9F600FEFBF800FEFBF800FEFBF800FEFAF600FEF9F400FEF8
              F200FEF7F000FEF6ED00FEF5EB00FEF4E900FEF3E700FEF2E500FEF1E300FEF0
              E000FEEFDE00FEEEDB00FEECD800FEEAD400FEE8D000FEE7CE00FEE7CC00FEE6
              CB00FEE6CA00FEE5C800FEE4C600FDE3C400FDE2C300FDE1C100FBDCBB00F8D6
              B300F4CDA900ECBA9100E3A77800D98F5900CF793D00C7692900C25C1800BF54
              0E00BC4F0700BB4C0200BB4B0100BB4A0000B8480000B4450000AF410000A83C
              00009E340000932C00008B2500008823000085210000831F0000831F0000841F
              0200881F08008E1D150099192E00AB155200BF107B00D50AA900E905D300F602
              ED00FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00
              FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00F8F8F8F8F8F8
              F8F8F8F8F8F8F8F8F8F8F8F8F8F8F8F8F8F8F8F8F8F8F8F8F8F8DBDBDBDBDBDB
              DBDBDBF8F8F8F8F8F8F8B5B8BCC0C4C7CDCFDBF8F8F8F8F8F8F8B5B6BABEC3C6
              CBCFDBF8F8F8F8F8F8F8B5B5B9BDC2C5C9CEDBF8F8F8F8F8F8F8E7E7E7E7E7E7
              E7E7E7E7E7DBF81DF8F8B5B6B9BCC0C3C5C8CCCFD0DBF81D1DF8B5B5B8BABEC2
              C4C6CBCFD0DBF81D821DB5B5B7BABEC1C4C6CBCECFDBF81D1DF8E7E7E7E7E7E7
              E7E7E7E7E7DBF81DF8F8B5B5B9BEC3C5CBCFDBF8F8F8F8F8F8F8B5B5B7BCC1C5
              C9CFDBF8F8F8F8F8F8F8B5B5B5BABEC3C6CCDBF8F8F8F8F8F8F8DBDBDBDBDBDB
              DBDBDBF8F8F8F8F8F8F8F8F8F8F8F8F8F8F8F8F8F8F8F8F8F8F8}
            OnClick = CombObjRemoveClick
          end
          object CombEntryName: TLabel
            Left = 8
            Top = 25
            Width = 17
            Height = 13
            Caption = '----'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clHotLight
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object CombObjId: TEdit
            Left = 55
            Top = 3
            Width = 67
            Height = 18
            BevelInner = bvNone
            BevelKind = bkFlat
            BevelOuter = bvRaised
            BorderStyle = bsNone
            TabOrder = 0
            OnChange = CombObjIdChange
          end
          object OptCombineCalcByLast: TCheckBox
            Left = 153
            Top = 1
            Width = 171
            Height = 18
            Caption = #1056#1072#1089#1095#1080#1090#1099#1074#1072#1090#1100' '#1087#1086' '#1087#1086#1089#1083#1077#1076#1085#1077#1084#1091
            TabOrder = 1
            OnClick = OptCombineCalcByLastClick
          end
        end
        object CombineList: TListView
          Left = 0
          Top = 43
          Width = 344
          Height = 106
          Align = alClient
          Columns = <
            item
              Caption = 'ID'
              Width = 40
            end
            item
              Caption = 'address'
              Width = 66
            end
            item
              Caption = #1048#1084#1103
              Width = 120
            end>
          ColumnClick = False
          GridLines = True
          HideSelection = False
          ReadOnly = True
          RowSelect = True
          SortType = stData
          TabOrder = 1
          ViewStyle = vsReport
          OnCompare = CombineListCompare
          OnDeletion = CombineListDeletion
          OnDragDrop = CombineListDragDrop
          OnDragOver = CombineListDragOver
          OnInsert = CombineListInsert
          OnResize = CombineListResize
          OnSelectItem = CombineListSelectItem
        end
      end
      object FormulaSheet: TTabSheet
        Caption = #1060#1086#1088#1084#1091#1083#1072
        ImageIndex = 1
        object Formula: TMemo
          Left = 0
          Top = 0
          Width = 344
          Height = 149
          Align = alClient
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -14
          Font.Name = 'Courier New'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 0
          OnChange = FormulaChange
        end
      end
      object TabSheet2: TTabSheet
        Caption = #1050#1072#1082' '#1087#1080#1089#1072#1090#1100' '#1089#1082#1088#1080#1087#1090#1099
        ImageIndex = 2
        object Memo1: TMemo
          Left = 0
          Top = 0
          Width = 344
          Height = 149
          Align = alClient
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -14
          Font.Name = 'Courier New'
          Font.Style = [fsBold]
          Lines.Strings = (
            #1057#1082#1088#1080#1087#1090' '#1085#1072' '#1103#1079#1099#1082#1077' Lua '#1076#1086#1083#1078#1077#1085' '#1085#1072#1095#1080#1085#1072#1090#1100#1089#1103' '#1089#1086' '
            #1089#1090#1088#1086#1082#1080' '
            '--Lua Script'
            #1055#1077#1088#1077#1076' '#1074#1099#1079#1086#1074#1086#1084' '#1091#1089#1090#1072#1085#1072#1074#1083#1080#1074#1072#1102#1090#1089#1103' '#1089#1083#1077#1076#1091#1102#1097#1080#1077' '
            #1075#1083#1086#1073#1072#1083#1100#1085#1099#1077' '#1087#1077#1088#1077#1084#1077#1085#1085#1099#1077
            'count - '#1050#1086#1083#1080#1095#1077#1089#1090#1074#1086' '#1087#1077#1088#1077#1084#1077#1085#1085#1099#1093
            
              'x0 .. xN  '#1075#1076#1077' N= count-1 '#1079#1085#1072#1095#1077#1085#1080#1103' '#1079#1072#1087#1080#1089#1077#1081' '#1074#1093#1086#1076#1103#1097#1080#1093' '#1074' '#1089#1086#1089#1090#1072#1074' '#1074#1099#1095#1080 +
              #1089#1083#1103#1077#1084#1086#1075#1086
            
              'id0...idN '#1075#1076#1077' N= count-1 ID '#1079#1072#1087#1080#1089#1077#1081' '#1074#1093#1086#1076#1103#1097#1080#1093' '#1074' '#1089#1086#1089#1090#1072#1074' '#1074#1099#1095#1080#1089#1083#1103#1077#1084#1086 +
              #1075#1086
            ''
            ''
            #1044#1086#1089#1090#1091#1087#1085#1099' '#1089#1083#1077#1076#1091#1102#1097#1080#1077' '#1074#1085#1077#1096#1085#1080#1077' '#1092#1091#1085#1082#1094#1080#1080
            'int bin_and(int v1,int v2  ) - '#1074#1086#1079#1074#1088#1072#1097#1072#1077#1090' '
            #1088#1077#1079#1091#1083#1100#1090#1072#1090' '#1076#1074#1086#1080#1095#1085#1086#1075#1086' '#1048
            'int bin_or (int v1,int v2  ) - '#1074#1086#1079#1074#1088#1072#1097#1072#1077#1090' '
            #1088#1077#1079#1091#1083#1100#1090#1072#1090' '#1076#1074#1086#1080#1095#1085#1086#1075#1086' '#1048#1051#1048
            'int bin_xor(int v1,int v2  ) - '#1074#1086#1079#1074#1088#1072#1097#1072#1077#1090' '
            #1088#1077#1079#1091#1083#1100#1090#1072#1090' '#1076#1074#1086#1080#1095#1085#1086#1075#1086' '#1080#1084#1082#1083#1102#1095#1072#1102#1097#1077#1075#1086' '#1048#1051#1048
            'int bin_not(int v          ) - '#1074#1086#1079#1074#1088#1072#1097#1072#1077#1090' '
            #1080#1085#1074#1077#1088#1089#1085#1086#1077' '#1079#1085#1072#1095#1077#1085#1080#1077' '#1072#1088#1075#1091#1084#1077#1085#1090#1072
            'int bin_shl(int v1,int n   ) - '#1074#1086#1079#1074#1088#1072#1097#1072#1077#1090' '
            #1088#1077#1079#1091#1083#1100#1090#1072#1090' '#1089#1076#1074#1080#1075#1072' '#1074#1083#1077#1074#1086'  '#1085#1072' n '#1073#1080#1090
            'int bin_shr(int v1,int n   ) - '#1074#1086#1079#1074#1088#1072#1097#1072#1077#1090' '
            #1088#1077#1079#1091#1083#1100#1090#1072#1090' '#1089#1076#1074#1080#1075#1072' '#1074#1087#1088#1072#1074#1086' '#1085#1072' n '#1073#1080#1090
            ''
            #1092#1091#1085#1082#1094#1080#1080' '#1087#1086#1083#1091#1095#1077#1085#1080#1103' min/max '#1072#1074#1072#1088#1080#1081#1085#1099#1093' '#1080' '#1087#1088#1077#1076#1091#1087#1088#1077#1076#1080#1090#1077#1083#1100#1085#1099#1093' '#1091#1089#1090#1072#1074#1086#1082
            #1074#1086#1079#1074#1088#1072#1097#1072#1102#1090' '#1087#1072#1088#1091' '#1079#1085#1072#1095#1077#1085#1080#1081' '
            '0,???    - '#1077#1089#1083#1080' '#1079#1072#1087#1080#1089#1100' '#1085#1077' '#1089#1091#1097#1077#1089#1090#1074#1091#1077#1090
            '1,'#1095#1080#1089#1083#1086'  - '#1077#1089#1083#1080' '#1079#1072#1087#1080#1089#1100' '#1089#1091#1097#1077#1089#1090#1074#1091#1077#1090' '#1080' '#1079#1085#1072#1095#1077#1085#1080#1077' '#1091#1089#1090#1072#1074#1082#1080
            ''
            
              'int, numeric get_max_alarm (rec_id) - '#1074#1086#1079#1074#1088#1072#1097#1072#1077#1090' '#1084#1072#1082#1089#1080#1084#1072#1083#1100#1085#1091#1102' '#1072#1074 +
              #1072#1088#1080#1081#1085#1091#1102' '#1091#1089#1090#1072#1074#1082#1091
            
              'int, numeric get_min_alarm (rec_id) - '#1074#1086#1079#1074#1088#1072#1097#1072#1077#1090' '#1084#1080#1085#1080#1084#1072#1083#1100#1085#1091#1102'  '#1072#1074 +
              #1072#1088#1080#1081#1085#1091#1102' '#1091#1089#1090#1072#1074#1082#1091
            
              'int, numeric get_max_danger(rec_id) - '#1074#1086#1079#1074#1088#1072#1097#1072#1077#1090' '#1084#1072#1082#1089#1080#1084#1072#1083#1100#1085#1091#1102' '#1087#1088 +
              #1077#1076#1091#1087#1088#1077#1076#1080#1090#1077#1083#1100#1085#1091#1102' '#1091#1089#1090#1072#1074#1082#1091
            
              'int, numeric get_min_danger(rec_id) - '#1074#1086#1079#1074#1088#1072#1097#1072#1077#1090' '#1084#1080#1085#1080#1084#1072#1083#1100#1085#1091#1102'  '#1087#1088 +
              #1077#1076#1091#1087#1088#1077#1076#1080#1090#1077#1083#1100#1085#1091#1102' '#1091#1089#1090#1072#1074#1082#1091
            ''
            #1055#1088#1080#1084#1077#1088' '#1089#1082#1088#1080#1087#1090#1072' '#1089#1080#1075#1085#1072#1083#1080#1079#1072#1094#1080#1080' '#1074#1099#1093#1086#1076#1072' '#1079#1072' '#1072#1074#1072#1088#1080#1081#1085#1091#1102' '#1091#1089#1090#1072#1074#1082#1091':'
            ''
            '--Lua'
            
              'success,alarm = get_max_alarm(id0); --'#1055#1086#1083#1091#1095#1072#1077#1084' '#1091#1089#1090#1072#1074#1082#1091' '#1076#1083#1103' '#1079#1072#1087#1080#1089 +
              #1080' '#1074' '#1089#1086#1089#1090#1072#1074#1077' '#1074#1099#1095#1080#1089#1083#1103#1077#1084#1086#1075#1086
            
              'if success == 1 and x0>alarm then -- '#1045#1089#1083#1080' '#1089#1091#1097#1077#1089#1090#1074#1091#1077#1090' '#1080' '#1079#1085#1072#1095#1077#1085#1080#1077' ' +
              #1087#1088#1077#1074#1099#1089#1080#1083#1086' '#1091#1089#1090#1072#1074#1082#1091' '#1090#1086' '
            ' return 1;  -- '#1074#1086#1079#1074#1088#1072#1097#1072#1077#1084'  '#1042#1050#1051';'
            'end'
            'return 0; '
            ''
            ''
            #1044#1072#1083#1077#1077' '#1088#1091#1082#1086#1074#1086#1076#1089#1090#1074#1086' '#1087#1086' '#1103#1079#1099#1082#1091' LUA'
            'https://www.lua.org/pil/contents.html')
          ParentFont = False
          ReadOnly = True
          ScrollBars = ssBoth
          TabOrder = 0
        end
      end
    end
  end
  object RecordParam: TGroupBox
    Left = 356
    Top = 173
    Width = 159
    Height = 194
    Align = alRight
    TabOrder = 2
    object StateDiscreteStatic: TCheckBox
      Left = 5
      Top = 16
      Width = 151
      Height = 17
      Caption = #1057#1090#1072#1090#1080#1095#1085#1086#1077' '#1086#1090#1086#1073#1088#1072#1078#1077#1085#1080#1077
      TabOrder = 0
      OnClick = StateDiscreteStaticClick
    end
    object StateDiscreteHidden: TCheckBox
      Left = 5
      Top = 34
      Width = 97
      Height = 17
      Caption = #1057#1082#1088#1099#1090#1072#1103
      TabOrder = 1
      OnClick = StateDiscreteStaticClick
    end
    object StateDiscreteFixed: TCheckBox
      Left = 5
      Top = 53
      Width = 106
      Height = 17
      Caption = #1053#1077#1082#1074#1080#1090#1080#1088#1091#1077#1084#1072#1103
      TabOrder = 2
      OnClick = StateDiscreteStaticClick
    end
    object StateDiscreteNormal: TCheckBox
      Left = 5
      Top = 70
      Width = 127
      Height = 21
      Caption = #1053#1086#1088#1084#1072#1083#1100#1085#1086#1077'  '#1042#1050#1051
      TabOrder = 3
      OnClick = StateDiscreteStaticClick
    end
    object StateDiscreteTU: TCheckBox
      Left = 6
      Top = 91
      Width = 97
      Height = 17
      Caption = 'TU - '#1054#1090#1083#1072#1076#1082#1072
      TabOrder = 4
      OnClick = StateDiscreteStaticClick
    end
    object StateDiscreteTUERR: TCheckBox
      Left = 7
      Top = 110
      Width = 140
      Height = 17
      Caption = 'TUERR - '#1054#1090#1083#1072#1076#1082#1072
      TabOrder = 5
      OnClick = StateDiscreteStaticClick
    end
  end
  object CommonRecordParam: TGroupBox
    Left = 0
    Top = 0
    Width = 515
    Height = 173
    Align = alTop
    Caption = #1054#1073#1097#1080#1077
    TabOrder = 0
    DesignSize = (
      515
      173)
    object Label1: TLabel
      Left = 8
      Top = 15
      Width = 77
      Height = 13
      Caption = #1040#1076#1088#1077#1089' '#1086#1073#1098#1077#1082#1090#1072
    end
    object Label2: TLabel
      Left = 8
      Top = 40
      Width = 70
      Height = 13
      Caption = #1050#1086#1088#1086#1090#1082#1086#1077' '#1080#1084#1103
    end
    object Label3: TLabel
      Left = 10
      Top = 60
      Width = 58
      Height = 13
      Caption = #1055#1086#1083#1085#1086#1077' '#1080#1084#1103
    end
    object Label4: TLabel
      Left = 224
      Top = 15
      Width = 13
      Height = 13
      Caption = 'Id '
    end
    object ID: TLabel
      Left = 248
      Top = 15
      Width = 11
      Height = 13
      Caption = 'ID'
    end
    object ScaleLabel: TLabel
      Left = 153
      Top = 84
      Width = 34
      Height = 13
      Caption = #1064#1082#1072#1083#1072
    end
    object Label9: TLabel
      Left = 11
      Top = 83
      Width = 44
      Height = 13
      Caption = #1055#1086#1088#1086#1075' %'
    end
    object AlarmBox: TGroupBox
      Left = 241
      Top = 134
      Width = 195
      Height = 36
      TabOrder = 11
      Visible = False
      object Label11: TLabel
        Left = 6
        Top = 16
        Width = 16
        Height = 13
        Caption = 'min'
      end
      object Label12: TLabel
        Left = 96
        Top = 16
        Width = 20
        Height = 13
        Caption = 'max'
      end
      object MaxAlarm: TEdit
        Left = 127
        Top = 15
        Width = 60
        Height = 15
        BevelInner = bvNone
        BevelKind = bkFlat
        BevelOuter = bvNone
        BorderStyle = bsNone
        TabOrder = 1
        Text = '000'
        OnChange = MaxAlarmChange
      end
      object MinAlarm: TEdit
        Left = 28
        Top = 15
        Width = 60
        Height = 16
        BevelInner = bvNone
        BevelKind = bkFlat
        BevelOuter = bvNone
        BorderStyle = bsNone
        TabOrder = 0
        Text = '000'
        OnChange = MinAlarmChange
      end
    end
    object DangerBox: TGroupBox
      Left = 4
      Top = 134
      Width = 196
      Height = 36
      TabOrder = 13
      Visible = False
      object Label5: TLabel
        Left = 8
        Top = 16
        Width = 16
        Height = 13
        Caption = 'min'
      end
      object Label6: TLabel
        Left = 99
        Top = 16
        Width = 20
        Height = 13
        Caption = 'max'
      end
      object MaxDanger: TEdit
        Left = 128
        Top = 15
        Width = 60
        Height = 16
        BevelInner = bvNone
        BevelKind = bkFlat
        BevelOuter = bvNone
        BorderStyle = bsNone
        TabOrder = 1
        Text = '000'
        OnChange = MaxDangerChange
      end
      object MinDanger: TEdit
        Left = 29
        Top = 15
        Width = 60
        Height = 16
        BevelInner = bvNone
        BevelKind = bkFlat
        BevelOuter = bvNone
        BorderStyle = bsNone
        TabOrder = 0
        Text = '000'
        OnChange = MinDangerChange
      end
    end
    object OtdAddr: TEdit
      Left = 94
      Top = 13
      Width = 121
      Height = 18
      BevelInner = bvNone
      BevelKind = bkFlat
      BevelOuter = bvRaised
      BorderStyle = bsNone
      TabOrder = 0
      OnChange = OtdAddrChange
    end
    object StateWorkOut: TCheckBox
      Left = 313
      Top = 13
      Width = 119
      Height = 17
      Caption = #1042#1099#1074#1077#1076#1077#1085' '#1080#1079' '#1088#1072#1073#1086#1090#1099
      TabOrder = 1
      OnClick = StateWorkOutClick
    end
    object OptAlarm: TCheckBox
      Left = 247
      Top = 129
      Width = 82
      Height = 14
      Caption = #1040#1072#1074#1088#1080#1081#1085#1099#1081
      TabOrder = 10
      OnClick = OptAlarmClick
    end
    object ScaleId: TComboBox
      Left = 196
      Top = 82
      Width = 289
      Height = 21
      BevelKind = bkFlat
      Anchors = [akLeft, akTop, akRight]
      TabOrder = 5
      OnChange = ScaleIdChange
      OnDropDown = ScaleIdDropDown
    end
    object Percent: TEdit
      Left = 94
      Top = 82
      Width = 53
      Height = 18
      BevelInner = bvNone
      BevelKind = bkFlat
      BevelOuter = bvRaised
      BorderStyle = bsNone
      TabOrder = 4
      OnChange = PercentChange
    end
    object OptDanger: TCheckBox
      Left = 9
      Top = 129
      Width = 131
      Height = 14
      Caption = #1055#1088#1077#1076#1091#1087#1088#1077#1076#1080#1090#1077#1083#1100#1085#1099#1081
      TabOrder = 12
      OnClick = OptAlarmClick
    end
    object StateHasTuTr: TCheckBox
      Left = 154
      Top = 109
      Width = 106
      Height = 13
      Caption = #1053#1072#1083#1080#1095#1080#1077' '#1058#1059'/'#1058#1056
      TabOrder = 7
      OnClick = StateWorkOutClick
    end
    object StateDiscreteValue: TCheckBox
      Left = 8
      Top = 106
      Width = 140
      Height = 17
      Caption = #1044#1080#1089#1082#1088#1077#1090#1085#1099#1081' '#1087#1072#1088#1072#1084#1077#1090#1088
      TabOrder = 6
      OnClick = StateDiscreteValueClick
    end
    object OptPseudo: TCheckBox
      Left = 285
      Top = 109
      Width = 98
      Height = 13
      Caption = #1055#1089#1077#1074#1076#1086#1086#1073#1098#1077#1082#1090
      TabOrder = 8
      Visible = False
      OnClick = OptPseudoClick
    end
    object OptCaution: TCheckBox
      Left = 400
      Top = 109
      Width = 90
      Height = 13
      Caption = #1055#1083#1072#1082#1072#1090
      TabOrder = 9
      Visible = False
      OnClick = OptCautionClick
    end
    object ShortName: TComboBox
      Left = 94
      Top = 37
      Width = 391
      Height = 21
      BevelKind = bkFlat
      Anchors = [akLeft, akTop, akRight]
      TabOrder = 2
      OnChange = ShortNameChange
      OnCloseUp = ShortNameCloseUp
      OnDropDown = ShortNameDropDown
      OnSelect = ShortNameSelect
    end
    object FullName: TComboBox
      Left = 94
      Top = 60
      Width = 391
      Height = 21
      BevelKind = bkFlat
      Anchors = [akLeft, akTop, akRight]
      TabOrder = 3
      OnChange = FullNameChange
      OnCloseUp = ShortNameCloseUp
      OnDropDown = ShortNameDropDown
      OnSelect = ShortNameSelect
    end
  end
  object adjust_timer: TTimer
    Enabled = False
    Interval = 333
    OnTimer = adjust_timerTimer
    Left = 472
    Top = 6
  end
end
