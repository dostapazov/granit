inherited GKOpcClientFrm: TGKOpcClientFrm
  Caption = ''
  ClientHeight = 453
  ClientWidth = 728
  OnClose = FormClose
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  ExplicitWidth = 736
  ExplicitHeight = 480
  PixelsPerInch = 96
  TextHeight = 13
  object Splitter1: TSplitter
    Left = 177
    Top = 29
    Height = 405
    ExplicitLeft = 208
    ExplicitTop = 88
    ExplicitHeight = 100
  end
  object StatusBar1: TStatusBar
    Left = 0
    Top = 434
    Width = 728
    Height = 19
    Panels = <
      item
        Width = 250
      end
      item
        Width = 50
      end>
  end
  object ToolBar1: TToolBar
    Left = 0
    Top = 0
    Width = 728
    Height = 29
    Caption = 'ToolBar1'
    Images = ImageList1
    TabOrder = 1
    object bStart: TToolButton
      Left = 0
      Top = 0
      Enabled = False
      ImageIndex = 0
      OnClick = bStartClick
    end
    object bStop: TToolButton
      Left = 23
      Top = 0
      Caption = 'bStop'
      Enabled = False
      ImageIndex = 1
      OnClick = bStopClick
    end
    object bRefresh: TToolButton
      Left = 46
      Top = 0
      Hint = #1054#1073#1085#1086#1074#1080#1090#1100
      ImageIndex = 6
      OnClick = bRefreshClick
    end
    object ToolButton1: TToolButton
      Left = 69
      Top = 0
      Width = 8
      Caption = 'ToolButton1'
      ImageIndex = 2
      Style = tbsSeparator
    end
    object tbNew: TToolButton
      Left = 77
      Top = 0
      Caption = 'tbNew'
      Enabled = False
      ImageIndex = 2
      OnClick = tbNewClick
    end
    object tbDelete: TToolButton
      Left = 100
      Top = 0
      Caption = 'tbDelete'
      Enabled = False
      ImageIndex = 3
      OnClick = tbDeleteClick
    end
    object ToolButton2: TToolButton
      Left = 123
      Top = 0
      Width = 8
      Caption = 'ToolButton2'
      ImageIndex = 8
      Style = tbsSeparator
    end
    object tbApply: TToolButton
      Left = 131
      Top = 0
      Caption = 'tbApply'
      Enabled = False
      ImageIndex = 4
      OnClick = tbApplyClick
    end
    object tbUndo: TToolButton
      Left = 154
      Top = 0
      Caption = 'tbUndo'
      Enabled = False
      ImageIndex = 5
      OnClick = tbUndoClick
    end
  end
  object LinesTree: TTreeView
    Left = 0
    Top = 29
    Width = 177
    Height = 405
    Align = alLeft
    HideSelection = False
    Indent = 19
    ReadOnly = True
    TabOrder = 2
    OnChange = LinesTreeChange
    OnChanging = LinesTreeChanging
    OnDeletion = LinesTreeDeletion
  end
  object Panel1: TPanel
    Left = 180
    Top = 29
    Width = 548
    Height = 405
    Align = alClient
    TabOrder = 3
    object PageControl1: TPageControl
      Left = 1
      Top = 1
      Width = 546
      Height = 403
      ActivePage = ConfigSheet
      Align = alClient
      TabOrder = 0
      OnChange = PageControl1Change
      object ConfigSheet: TTabSheet
        Caption = #1053#1072#1089#1090#1088#1086#1081#1082#1080
        inline OpcModSettings1: TOpcModSettings
          Left = -3
          Top = 5
          Width = 467
          Height = 126
          TabOrder = 0
          Visible = False
          ExplicitLeft = -3
          ExplicitTop = 5
          inherited Label1: TLabel
            Width = 77
            ExplicitWidth = 77
          end
          inherited Label2: TLabel
            Width = 53
            ExplicitWidth = 53
          end
          inherited Label3: TLabel
            Width = 41
            ExplicitWidth = 41
          end
        end
        inline OpcLineSetingsFrame1: TOpcLineSetingsFrame
          Left = 22
          Top = 77
          Width = 531
          Height = 185
          TabOrder = 1
          Visible = False
          ExplicitLeft = 22
          ExplicitTop = 77
          ExplicitHeight = 185
          inherited Panel1: TPanel
            inherited Label5: TLabel
              Width = 104
              ExplicitWidth = 104
            end
            inherited Label4: TLabel
              Width = 52
              ExplicitWidth = 52
            end
            inherited Label1: TLabel
              Width = 40
              ExplicitWidth = 40
            end
            inherited Label6: TLabel
              Width = 77
              ExplicitWidth = 77
            end
            inherited Label7: TLabel
              Width = 106
              ExplicitWidth = 106
            end
            inherited GroupBox1: TGroupBox
              inherited Label2: TLabel
                Width = 19
                ExplicitWidth = 19
              end
            end
          end
          inherited PageControl1: TPageControl
            Height = 9
            ExplicitHeight = 9
            inherited TabSheet1: TTabSheet
              ExplicitLeft = 4
              ExplicitTop = 24
              ExplicitWidth = 523
              ExplicitHeight = 103
            end
            inherited TabSheet2: TTabSheet
              ExplicitLeft = 4
              ExplicitTop = 24
              ExplicitWidth = 523
              ExplicitHeight = 103
            end
            inherited TabSheet4: TTabSheet
              ExplicitLeft = 4
              ExplicitTop = 24
              ExplicitWidth = 523
              ExplicitHeight = 103
            end
          end
        end
      end
      object ViewOpcItemsSheet: TTabSheet
        Caption = #1055#1088#1086#1089#1084#1086#1090#1088
        ImageIndex = 1
        inline DataFrame: TGKOpcItemsViewFrame
          Left = -124
          Top = 19
          Width = 662
          Height = 356
          TabOrder = 0
          ExplicitLeft = -124
          ExplicitTop = 19
        end
      end
      object AboutSheet: TTabSheet
        Caption = #1054' '#1084#1086#1076#1091#1083#1077
        ImageIndex = 2
        inline AboutCommonFrame1: TAboutCommonFrame
          Left = 0
          Top = 0
          Width = 538
          Height = 96
          Align = alTop
          TabOrder = 0
          ExplicitWidth = 538
          ExplicitHeight = 96
          inherited Memo2: TMemo
            Width = 538
            Height = 96
            ExplicitWidth = 538
            ExplicitHeight = 96
          end
        end
        object Memo1: TMemo
          Left = 0
          Top = 96
          Width = 538
          Height = 279
          Align = alClient
          BevelEdges = []
          BevelInner = bvNone
          BevelOuter = bvNone
          BorderStyle = bsNone
          Color = clBtnFace
          Enabled = False
          Lines.Strings = (
            ''
            ''
            '                                      '#1050#1083#1080#1077#1085#1090' OPC V 1.0')
          ReadOnly = True
          TabOrder = 1
          WantReturns = False
        end
      end
    end
  end
  object ImageList1: TImageList
    Left = 664
    Top = 176
    Bitmap = {
      494C010107000900040010001000FFFFFFFFFF10FFFFFFFFFFFFFFFF424D3600
      0000000000003600000028000000400000002000000001002000000000000020
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000892602000000
      0000000000000000000000000000000000000000000000000000B1908E00B190
      8E00B1908E00B1908E00B1908E00B1908E00B1908E00B1908E00B1908E00B190
      8E00B1908E00B1908E0000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000008A2703009C3402008A27
      0300000000000000000000000000000000000000000000000000B1908E00FEFB
      FA00FEFBFA00FEFBFA00FEFBFA00FEFBFA00FEFBFA00FEFBFA00FEFBFA00FEFB
      FA00FEFBFA00B1908E0000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000FF0000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000008A270300AD410100AB3E
      01008A2703000000000000000000000000000000000000000000B1908E00FEFB
      FA00FEFBFA00FEFBFA00FEFBFA00CCE5CA000070000000700000CCE5CA00FEFB
      FA00FEFBFA00B1908E0000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000FF000000FF000000FF00000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000008A2703008E290300B849
      00009F3501008A27030000000000000000000000000000000000B1908E00FEFA
      F400FEFAF40000700000CCE3C50000700000CCE3C500CCE3C50000700000FEFA
      F400FEFAF400B1908E0000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000FF000000FF000000FF00000000000000000000000000000000000000
      0000000000000000000000000000000000008926020089260200892602008926
      02008926020089260200892602008A2703008A270300000000008A2703008E29
      0300B8490000983002008A270300000000000000000000000000B1908E00FEF7
      F000FEF7F0000070000000700000CCE1C200FEF7F000FEF7F000CCE1C200FEF7
      F000FEF7F000B1908E0000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      FF000000FF000000FF000000FF000000FF000000000000000000000000000000
      00000000000000000000000000000000000089260200D8721B00C65B0C00B649
      0200B4430000B5450000B6450000B64600009C34020089260200000000008A27
      0300932E0200B84900008E290300000000000000000000000000B1908E00FEF4
      EB00FEF4EB00007000000070000000700000FEF4EB00FEF4EB00FEF4EB00FEF4
      EB00FEF4EB00B1908E0000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000000000FF000000
      FF000000FF000000FF000000FF000000FF000000000000000000000000000000
      00000000000000000000000000000000000089260200FFA63700E27B1E00D86C
      1000892602008926020089260200892602008926020000000000000000000000
      00008A270300A63B0100A63B01008A2703000000000000000000B1908E00FEF2
      E600FEF2E600FEF2E600FEF2E600FEF2E600007000000070000000700000FEF2
      E600FEF2E600B1908E0000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000007F7F7F000000FF000000
      FF0000000000000000000000FF000000FF000000FF0000000000000000000000
      00000000000000000000000000000000000089260200FFAA3900E27B1E00DE79
      1E00C1590F008723020000000000000000000000000000000000000000000000
      000000000000902B0200B6450000892703000000000000000000B1908E00FEEF
      E200FEEFE200CCDAB700FEEFE200FEEFE200CCDAB7000070000000700000FEEF
      E200FEEFE200B1908E0000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000007F7F7F000000FF00000000000000
      00000000000000000000000000000000FF000000FF0000000000000000000000
      00000000000000000000000000000000000089260200FFAA3900A03B0A008A28
      0300F6932A00AC460C0087240200000000000000000000000000000000000000
      00000000000089260300C4510000892603000000000000000000B1908E00FFED
      DD00FFEDDD0000700000CED9B200CED9B20000700000CED9B20000700000FFED
      DD00FFEDDD00B1908E0000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000FF000000FF000000FF00000000000000
      00000000000000000000000000000000000089260200FFAB3A00892602008623
      01009D380900FA982E00C25B1200872402000000000000000000000000000000
      00008A270300932D0200BC4B0000892603000000000000000000B1908E00FFEB
      D900FFEBD900CED8B0000070000000700000CED8B000D3C1B100D3C1B100D3C1
      B100D3C1B100B1908E0000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000FF000000FF00000000000000
      00000000000000000000000000000000000089260200FFAB3A00892602000000
      0000882402009E380900EB892800D16C19008E28030089250200000000008A26
      030089260300B1430000A63B01008A2703000000000000000000B1908E00FFE9
      D400FFE9D400FFE9D400FFE9D400FFE9D400FFE9D400B5A19300B2A09200B19F
      9100B09E9000B1908E0000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000FF000000FF000000
      00000000000000000000000000000000000089260200FFAC3A00892602000000
      000000000000892502008F2A0300D5701F00EA862300AB4309009A340300972F
      0200B1430000B64500008E290300000000000000000000000000B1908E00FFE6
      CF00FFE6CF00FFE6CF00FFE6CF00FFE6CF00FFE6CF009C8B7E009C8B7E009C8B
      7E009C8B7E00B1908E0000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000007F7F7F000000
      FF000000000000000000000000000000000088240200FB9C3300892602000000
      000000000000000000008926020088240200A5400C00CB651600CB600F00B246
      03009A330200892603008A270300000000000000000000000000B1908E00FFE3
      CB00FFE3CB00FFE3CB00FFE3CB00FFE3CB00BAA69700FAF6F000FAF6F000DECF
      CE00B1908E000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000007F7F
      7F000000FF000000000000000000000000000000000089250200000000000000
      0000000000000000000000000000000000008926020089250200892602008926
      03008A2703000000000000000000000000000000000000000000B1908E00FFE1
      C600FFE1C600FFE1C600FFE1C600FFE1C600BAA69700FAF6F000D5C0B700B190
      8E00000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000FF000000FF00000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000B1908E00FFDE
      C200FFDEC200FFDEC200FFDEC200FFDEC200BAA69700D5C0B700B1908E000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000B1908E00B190
      8E00B1908E00B1908E00B1908E00B1908E00B1908E00B1908E00000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000521E15006B2508007D2C05007E2D05006D260700551F13000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000521E15006B2508007D2C05007E2D05006D260700551F13000000
      0000000000000000000000000000000000000000000000000000B1908E00B190
      8E00B1908E00B1908E00B1908E00B1908E00B1908E00B1908E00B1908E00B190
      8E00B1908E00B1908E0000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000005B7000005B7000000000000000000000000005521
      170055211700BF5C1500E1985500EFBD8A00F0BF8C00E29D5B00C2621800541F
      1600541F16000000000000000000000000000000000000000000000000005521
      170089320400BF5C1500E1985500EFBD8A00F0BF8C00E29D5B00C2621800551F
      1300541F16000000000000000000000000000000000000000000B1908E00FEFB
      FA00FEFBFA00FEFBFA00FEFBFA00FEFBFA00FEFBFA00FEFBFA00FEFBFA00FEFB
      FA00FEFBFA00B1908E000000000000000000000000000005B7000005B7000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000005B7000005B7000000000000000000000000006C290E00AF47
      0400EBB17900FFFEF800FCF8F300F2DAC400F2D9C200FCF6F000FFFFFC00EEBA
      8700B64E070060231100000000000000000000000000000000006C290E00AF47
      0400EBB17900FFFEF800FEF8F400F2DAC400F2D9C200FCF6F000FFFFFC00EEBA
      8700B64E07006023110000000000000000000000000000000000B1908E00FEFB
      FA00FEFBFA00FEFBFA00FEFBFA00FEFBFA00FEFBFA00FEFBFA00FEFBFA00FEFB
      FA00FEFBFA00B1908E000000000000000000000000000005B7000005B7000005
      B700000000000000000000000000000000000000000000000000000000000000
      00000005B7000005B700000000000000000000000000732D0D00B0460200F4D0
      AB00FFFEFE00D78E5000C0520800B7410000BA460000C0530700D4864400FCF8
      F400F8DDBD00B64D0500541F16000000000000000000732D0D00B0460200F4D0
      AB00FFFEFE00D78E4F00C0540900BA460000BA460000BF520700D4864400FCF8
      F400F8DDBD00B64D0500541F1600000000000000000000000000B1908E00FEFA
      F400FEFAF400FEFAF400FEFAF400FEFAF400FEFAF400FEFAF400FEFAF400FEFA
      F400FEFAF400B1908E000000000000000000000000000005B7000005B6000005
      B7000005B7000000000000000000000000000000000000000000000000000005
      B7000005B70000000000000000000000000000000000732D0D00E6AB7200FFFF
      FF00CF762B00B63C0000FFFFFF00D17F3700B7410000BB470000B7410000C969
      1E00FCF8F600EFBC8800541F16000000000000000000732D0D00E6AB7200FFFF
      FF00CF762C00B73E0000B8430000BA440000BA440000B8420000B53C0000C96A
      1E00FCFAF600EFBC8800551F1300000000000000000000000000B1908E00FEF7
      F000FEF7F000FEF7F000FEF7F000FEF7F000FEF7F000FEF7F000FEF7F000FEF7
      F000FEF7F000B1908E00000000000000000000000000000000000006D7000005
      BA000005B7000005B700000000000000000000000000000000000005B7000005
      B7000000000000000000000000000000000089340600C05C1100FFFCFA00E1A3
      6A00C2500100C2500200FFFFFF00FFFFFF00D8925500BA430000BB480000B741
      0000D5894600FFFFFF00C1601600551F130089340600C05C1100FFFCFA00E1A3
      6A00C04B0000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00B63C
      0000D5894600FFFFFF00C1601600551F13000000000000000000B1908E00FEF4
      EB00FEF4EB00FEF4EB00FEF4EB00FEF4EB00FEF4EB00FEF4EB00FEF4EB00FEF4
      EB00FEF4EB00B1908E0000000000000000000000000000000000000000000000
      00000005B7000005B7000005B600000000000005B6000005B7000005B7000000
      0000000000000000000000000000000000009A3B0200DD955400FFFFFF00D376
      2800CF671600CC600F00FFFFFF00FFFFFF00FFFFFF00E3B08000BD4C0300BA44
      0000C0530500FCF8F300E29D5A00551F13009A3B0200DD955400FFFFFF00D376
      2800CC600D00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00B841
      0000C0530500FCF8F300E29D5A006D2607000000000000000000B1908E00FEF2
      E600FEF2E600FEF2E600FEF2E600FEF2E600FEF2E600FEF2E600FEF2E600FEF2
      E600FEF2E600B1908E0000000000000000000000000000000000000000000000
      0000000000000005B6000006C7000006C7000006CE000005B400000000000000
      000000000000000000000000000000000000A9440300EBBB8A00FBF0E700D87A
      2B00D9792A00D5701F00FFFFFF00FFFFFF00FFFFFF00FFFFFF00EFD4B600C155
      0A00BA440000F2DAC200EFC08E00551F1300A9440300EBBB8A00FBF0E700D87A
      2B00D7742200FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00BA44
      0000BB460000F2DAC200EFC08E007D2C04000000000000000000B1908E00FEEF
      E200FEEFE200FEEFE200FEEFE200FEEFE200FEEFE200FEEFE200FEEFE200FEEF
      E200FEEFE200B1908E0000000000000000000000000000000000000000000000
      000000000000000000000006C1000005C1000006DA0000000000000000000000
      000000000000000000000000000000000000B04A0600EEBF9000FCF4EB00E28A
      3E00E2893C00DE813100FFFFFF00FFFFFF00FFFFFF00FFFFFF00E5B28300BF4F
      0500BA460000F3DEC700EFBD8A00551F1300B04A0600EEBF9000FCF4EB00E28A
      3E00E1843500FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00BA44
      0000BB470000F3DEC700EFBD8A007B2C04000000000000000000B1908E00FFED
      DD00FFEDDD00FFEDDD00FFEDDD00FFEDDD00FFEDDD00FFEDDD00FFEDDD00FFED
      DD00FFEDDD00B1908E0000000000000000000000000000000000000000000000
      0000000000000005B6000006D7000006CE000006DA000006E900000000000000
      000000000000000000000000000000000000B0490500E9AA6E00FFFFFF00EDA5
      6200EB994E00E9914400FFFFFF00FFFFFF00FFFFFF00E09D5F00C04E0000BA46
      0000C1560700FEFBF800E0975200551F1300B0490500E9AA6E00FFFFFF00EDA4
      6200EA934600FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00B842
      0000C1550600FEFBF800E09752006B2408000000000000000000B1908E00FFED
      DD00FFEBD900FFEDDD00FFEBD900FFEDDD00FFEBD900D3C1B100D3C1B100D3C1
      B100D3C1B100B1908E0000000000000000000000000000000000000000000000
      00000006E5000006DA000006D30000000000000000000006E5000006EF000000
      000000000000000000000000000000000000AD450200DD893F00FFFCF800FAD7
      B400F6A76000F0A05600FFFFFF00FFFFFF00E1965200CC600E00C5590900B841
      0000D9955600FFFFFE00BD5A1000551F1300AD450200DD893F00FFFCF800FAD7
      B400F4A55C00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00B73E
      0000D9955600FFFFFE00BD5A1000521F16000000000000000000B1908E00FFEB
      D900FFE6CF00FFEBD900FFE6CF00FFEBD900FFEBD900B5A19300B2A09200B09E
      9000B09E9000B1908E0000000000000000000000000000000000000000000006
      F8000006DA000006EF00000000000000000000000000000000000006F8000006
      F6000000000000000000000000000000000000000000B2470100F8CEA500FFFF
      FF00FED1A500F4A55C00FFFFFF00E5985200D7742100CF681600C2500100D079
      3100FFFFFF00EAAF7300501F18000000000000000000B2470100F8CEA500FFFF
      FF00FED1A600F4A55B00EA934600E1843500D7732200CC600E00C04B0000D079
      3100FFFFFF00EAAF7300521F1600000000000000000000000000B1908E00FFE6
      CF00FFE6CF00FFE6CF00FFE6CF00FFE6CF00FFE6CF009C8B7E009C8B7E009C8B
      7E009C8B7E00B1908E00000000000000000000000000000000000006F6000006
      F6000006F8000000000000000000000000000000000000000000000000000006
      F6000006F60000000000000000000000000000000000B2470100E0873B00FEE5
      CB00FFFFFF00FAD5B200EDA15D00E1883B00D97B2D00D4782A00E1A46C00FFFF
      FF00F3CCA400AC440200501F18000000000000000000B2470100E0873B00FEE5
      CB00FFFFFF00FAD5B200EDA56200E28A3F00D97B2C00D4782A00E1A46C00FFFF
      FF00F3CCA400AC440200501F1800000000000000000000000000B1908E00FFE6
      CF00FFE1C600FFE6CF00FFE1C600FFE6CF00B5A19300FEF4EB00FEF4EB00DECF
      CE00B1908E00000000000000000000000000000000000006F6000006F6000006
      F600000000000000000000000000000000000000000000000000000000000000
      0000000000000006F60000000000000000000000000000000000B64C0400DE87
      3B00F8CEA300FFFAF300FFFFFF00FEF6EE00FCF3EA00FFFFFF00FEF7EE00E6A5
      6A00AC4403005A22140000000000000000000000000000000000B64C0400DE87
      3B00F8CEA300FFFAF300FFFFFF00FEF6EE00FCF3EA00FFFFFF00FEF7EE00E6A5
      6A00AC4403005A22140000000000000000000000000000000000B1908E00FFE1
      C600FFE1C600FFE1C600FFE1C600FFE1C600BAA69700FEFAF400D3C1B100B190
      8E00000000000000000000000000000000000006F6000006F6000006F6000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000AF45
      0100AF450100DC884000E9A76C00EEBB8900EBB58100DC914D00BD590F005D23
      15005D231500000000000000000000000000000000000000000000000000AF45
      0100AF450100DC884000E9A76C00EEBB8900EBB58100DC914D00BD590F00752D
      0C005D2315000000000000000000000000000000000000000000B1908E00FFDE
      C200FFDEC200FFDEC200FFDEC200FFDEC200B5A19300D3C1B100B1908E000000
      0000000000000000000000000000000000000006F6000006F600000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000A3410300A5450700A64608009E4006008B350500752D0C000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000A3410300A5450700A64608009E4006008B350500752D0C000000
      0000000000000000000000000000000000000000000000000000B1908E00B190
      8E00B1908E00B1908E00B1908E00B1908E00B1908E00B1908E00000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000424D3E000000000000003E000000
      2800000040000000200000000100010000000000000100000000000000000000
      000000000000000000000000FFFFFF00FFFFFFDFC0030000FFFFFF8FC0030000
      F9FFFF87C0030000F0FFFF83C0030000F0FF0041C0030000E07F0021C0030000
      C07F0070C0030000843F03F8C00300001E3F01F8C0030000FE1F00F0C0030000
      FF1F1020C0030000FF8F1801C0030000FFC71C01C0070000FFE3BF07C00F0000
      FFF8FFFFC01F0000FFFFFFFFC03F0000F81FF81FC003FFFCE007E007C0039FF9
      C003C003C0038FF380018001C00387E780018001C003C3CF00000000C003F11F
      00000000C003F83F00000000C003FC7F00000000C003F83F00000000C003F19F
      00000000C003E3CF80018001C003C7E780018001C0078FFBC003C003C00F1FFF
      E007E007C01F3FFFF81FF81FC03FFFFF00000000000000000000000000000000
      000000000000}
  end
end