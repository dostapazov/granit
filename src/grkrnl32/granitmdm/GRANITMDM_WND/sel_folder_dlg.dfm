object SelectFolderDlg: TSelectFolderDlg
  Left = 490
  Top = 268
  Anchors = [akLeft, akTop, akBottom]
  BorderIcons = []
  Caption = #1042#1099#1073#1086#1088' '#1087#1072#1087#1082#1080
  ClientHeight = 391
  ClientWidth = 352
  Color = clBtnFace
  Constraints.MaxWidth = 360
  Constraints.MinWidth = 360
  Font.Charset = RUSSIAN_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poOwnerFormCenter
  DesignSize = (
    352
    391)
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 8
    Top = 339
    Width = 54
    Height = 13
    Anchors = [akLeft, akBottom]
    Caption = 'D:\TEMP'
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object DriveComboBox1: TDriveComboBox
    Left = 7
    Top = 9
    Width = 105
    Height = 19
    DirList = DirectoryListBox1
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    TabOrder = 0
    TextCase = tcUpperCase
  end
  object DirectoryListBox1: TDirectoryListBox
    Left = 6
    Top = 38
    Width = 340
    Height = 291
    Anchors = [akLeft, akTop, akBottom]
    AutoComplete = False
    DirLabel = Label1
    TabOrder = 1
  end
  object BitBtn1: TBitBtn
    Left = 8
    Top = 361
    Width = 75
    Height = 23
    Anchors = [akLeft, akBottom]
    Caption = #1042#1099#1073#1088#1072#1090#1100
    Kind = bkOK
    NumGlyphs = 2
    TabOrder = 2
  end
  object BitBtn2: TBitBtn
    Left = 88
    Top = 361
    Width = 75
    Height = 23
    Anchors = [akLeft, akBottom]
    Caption = #1054#1090#1084#1077#1085#1072
    Kind = bkCancel
    NumGlyphs = 2
    TabOrder = 3
  end
  object BitBtn3: TBitBtn
    Left = 116
    Top = 8
    Width = 77
    Height = 23
    Caption = #1057#1086#1079#1076#1072#1090#1100
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
    TabOrder = 4
    OnClick = BitBtn3Click
  end
  object Edit1: TEdit
    Left = 200
    Top = 8
    Width = 145
    Height = 21
    TabOrder = 5
  end
end
