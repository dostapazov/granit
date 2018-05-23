inherited OpcLineSetingsFrame: TOpcLineSetingsFrame
  Width = 531
  Height = 307
  Visible = False
  ExplicitWidth = 531
  ExplicitHeight = 307
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 531
    Height = 176
    Align = alTop
    ParentShowHint = False
    ShowHint = True
    TabOrder = 0
    DesignSize = (
      531
      176)
    object bSelConfigFile: TSpeedButton
      Left = 452
      Top = 86
      Width = 24
      Height = 24
      Hint = #1042#1099#1073#1086#1088' '#1092#1072#1081#1083#1072' '#1041#1044
      Anchors = [akTop, akRight]
      Caption = '...'
      Flat = True
      OnClick = bSelConfigFileClick
    end
    object Label5: TLabel
      Left = 8
      Top = 90
      Width = 102
      Height = 13
      Caption = #1060#1072#1081#1083' '#1082#1086#1085#1092#1080#1075#1091#1088#1072#1094#1080#1080
    end
    object Label4: TLabel
      Left = 13
      Top = 9
      Width = 48
      Height = 13
      Caption = #1053#1086#1084#1077#1088' '#1050#1055
    end
    object Label1: TLabel
      Left = 145
      Top = 7
      Width = 36
      Height = 13
      Caption = #1048#1084#1103' '#1050#1055
    end
    object Label6: TLabel
      Left = 7
      Top = 118
      Width = 76
      Height = 13
      Caption = #1055#1077#1088#1080#1086#1076' '#1086#1087#1088#1086#1089#1072
    end
    object Label7: TLabel
      Left = 183
      Top = 118
      Width = 104
      Height = 13
      Caption = #1055#1086#1083#1086#1089#1072' '#1087#1088#1086#1087#1091#1089#1082#1072#1085#1080#1103
    end
    object bRestartLine: TSpeedButton
      Left = 480
      Top = 86
      Width = 24
      Height = 24
      Hint = #1055#1077#1088#1077#1079#1072#1075#1088#1091#1079#1080#1090#1100' '#1041#1044' '#1074' '#1084#1086#1076#1091#1083#1077
      Anchors = [akTop, akRight]
      Flat = True
      Glyph.Data = {
        36030000424D3603000000000000360000002800000010000000100000000100
        1800000000000003000000000000000000000000000000000000FFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFD9E0D297AE8076955879975C9CB088DEE2DAFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF6F7F47EA1621DA9020ADF0002
        F70004F2000FD1002993088CA376F7F7F6FFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FBFBFA6D974D24C51A15E9150CF20C06F80602FC0200FE0000FF0015BD007B97
        60F8F8F8FAFAFAFFFFFFFFFFFFFFFFFFAABE9659B84B55D4553AB22C36941842
        9B2423B20F18E5180FEF0F08F6081FA702478D26478924BAC7AEFFFFFFFDFDFC
        719B51A4E2A1579337A3B98EF6F8F4FFFFFF97AF8032991720E1200BF30B01FD
        0100FF0009E30088A26EFFFFFFDEE5D692AF78688E44D8E0D0FFFFFFFFFFFFF0
        F3EC488F2848D24739D7392CDB2C1FE11F15E9151CB908B4C3A4FFFFFFB9C9A8
        3A780E538932B4C2A6F7F7F6FFFFFFFBFCFA709B51A0D999A7E9A794E49482E0
        8270DB70468522F4F5F2FFFFFF6D964C0DDE0300FC0011CA004A932CC2CCB8FC
        FCFCF6F8F491AC7798B481E8F3E5EBFAEBA2C5919CB286FFFFFFD2DCC82A9D0E
        12EC1203FB0300FF0000FC001CB104A8B898FFFFFFFFFFFFDEE6D68DA9717D9C
        5D3F7211ECEDEAFFFFFF82A16436C33023DC2312EC1203FB0300FF0002F80059
        953AFFFFFFFFFFFFFFFFFFC3D0B62C990C3E9920FAFAFAFFFFFF5B8D385ED65E
        38D03824DC241FE61F17E415327E09C9D3C0FFFFFFF4F5F297AE8021A40404F1
        007DA062FFFFFFFFFFFF799C594B8F2A40861C9DD89685E0854FD34F30BE2640
        8F225E92403396130ED40000FF001BAE02D4DCCCFFFFFFFFFFFFFFFFFFFCFCFC
        E4EBDE638C3FC5E8C19FE79F67D96739D13924DA2416E81609F50910CE0090AC
        78FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFDCE5D4688E439FC08CB0E6AE80
        DF804BD34B2BC7252A9F0E92AE7AFDFDFCFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFAFBF8B4C6A27A9B596D954B6B964A8FAA74DCE4D4FFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF}
      OnClick = bRestartLineClick
    end
    object cp_number: TEdit
      Tag = 1
      Left = 86
      Top = 5
      Width = 53
      Height = 21
      BevelKind = bkFlat
      BorderStyle = bsNone
      TabOrder = 0
      OnChange = cp_numberChange
    end
    object config_file: TEdit
      Tag = 8
      Left = 116
      Top = 87
      Width = 334
      Height = 21
      Anchors = [akLeft, akTop, akRight]
      BevelKind = bkFlat
      BorderStyle = bsNone
      TabOrder = 1
      OnChange = cp_numberChange
    end
    object GroupBox1: TGroupBox
      Left = 7
      Top = 26
      Width = 514
      Height = 55
      Anchors = [akLeft, akTop, akRight]
      Caption = 'OPC-'#1057#1077#1088#1074#1077#1088
      TabOrder = 2
      DesignSize = (
        514
        55)
      object Label2: TLabel
        Left = 5
        Top = 11
        Width = 18
        Height = 13
        Caption = #1058#1080#1087
      end
      object Label3: TLabel
        Left = 78
        Top = 9
        Width = 33
        Height = 13
        Caption = 'ProgID'
      end
      object Label8: TLabel
        Left = 358
        Top = 6
        Width = 37
        Height = 13
        Caption = #1057#1077#1088#1074#1077#1088
      end
      object server_type_list: TComboBox
        Left = 5
        Top = 26
        Width = 65
        Height = 21
        BevelKind = bkFlat
        Style = csDropDownList
        ItemIndex = 1
        TabOrder = 0
        Text = 'DA20'
        OnChange = server_type_listChange
        Items.Strings = (
          'DA10'
          'DA20')
      end
      object server_prog_id: TComboBox
        Tag = 4
        Left = 78
        Top = 26
        Width = 274
        Height = 21
        BevelKind = bkFlat
        Anchors = [akLeft, akTop, akRight]
        TabOrder = 1
        OnChange = server_prog_idChange
        OnDropDown = server_prog_idDropDown
      end
      object server_host: TEdit
        Tag = 4
        Left = 358
        Top = 26
        Width = 147
        Height = 21
        Anchors = [akTop, akRight]
        BevelKind = bkFlat
        BorderStyle = bsNone
        TabOrder = 2
        OnChange = cp_numberChange
      end
    end
    object cp_name: TEdit
      Tag = 2
      Left = 217
      Top = 5
      Width = 233
      Height = 21
      BevelKind = bkFlat
      BorderStyle = bsNone
      TabOrder = 3
      OnChange = cp_numberChange
    end
    object rate: TEdit
      Tag = 16
      Left = 116
      Top = 114
      Width = 53
      Height = 21
      BevelKind = bkFlat
      BorderStyle = bsNone
      TabOrder = 4
      OnChange = cp_numberChange
    end
    object bandwidth: TEdit
      Tag = 32
      Left = 327
      Top = 114
      Width = 53
      Height = 21
      BevelKind = bkFlat
      BorderStyle = bsNone
      TabOrder = 5
      OnChange = cp_numberChange
    end
    object cbLineOptReplaceTime: TCheckBox
      Tag = 64
      Left = 8
      Top = 152
      Width = 279
      Height = 17
      Caption = #1055#1086#1076#1084#1077#1085#1103#1090#1100' '#1074#1088#1077#1084#1103' '#1080#1079#1084#1077#1085#1077#1085#1080#1103' '#1085#1072' '#1090#1077#1082#1091#1097#1077#1077
      TabOrder = 6
      OnClick = cp_numberChange
    end
    object Button1: TButton
      Left = 330
      Top = 146
      Width = 193
      Height = 25
      Caption = #1056#1077#1076#1072#1082#1090#1080#1088#1086#1074#1072#1090#1100' '#1082#1086#1085#1092#1080#1075#1091#1088#1072#1094#1080#1102
      TabOrder = 7
      OnClick = Button1Click
    end
  end
  object PageControl1: TPageControl
    Left = 0
    Top = 176
    Width = 531
    Height = 131
    ActivePage = TabSheet1
    Align = alClient
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    TabOrder = 1
    object TabSheet1: TTabSheet
      Caption = #1057#1082#1088#1080#1087#1090' '#1058#1059
      object TuLuaScript: TMemo
        Tag = 8192
        Left = 0
        Top = 0
        Width = 523
        Height = 103
        Align = alClient
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -14
        Font.Name = 'Courier New'
        Font.Pitch = fpFixed
        Font.Style = []
        Lines.Strings = (
          'Hello World')
        ParentFont = False
        ScrollBars = ssBoth
        TabOrder = 0
        OnChange = cp_numberChange
      end
    end
    object TabSheet2: TTabSheet
      Caption = #1057#1082#1088#1080#1087#1090' '#1058#1056
      ImageIndex = 1
      object TrLuaScript: TMemo
        Tag = 16384
        Left = 0
        Top = 0
        Width = 523
        Height = 103
        Align = alClient
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -14
        Font.Name = 'Courier New'
        Font.Pitch = fpFixed
        Font.Style = []
        Lines.Strings = (
          'Hello TR World')
        ParentFont = False
        ScrollBars = ssBoth
        TabOrder = 0
        OnChange = cp_numberChange
      end
    end
    object TabSheet3: TTabSheet
      Caption = #1050#1072#1082' '#1087#1080#1089#1072#1090#1100
      ImageIndex = 2
      object Memo1: TMemo
        Left = 0
        Top = 0
        Width = 523
        Height = 103
        Align = alClient
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -14
        Font.Name = 'Courier New'
        Font.Style = []
        Lines.Strings = (
          #1071#1079#1099#1082' LUA. '#1089#1084'. '#1056#1091#1082#1086#1074#1086#1076#1089#1090#1074#1086' '#1087#1086' '#1103#1079#1099#1082#1091' www.lua.org (www.lua.ru/doc)'
          ''
          #1057#1082#1088#1080#1090#1087' '#1079#1072#1087#1091#1089#1082#1072#1077#1090#1089#1103' '#1089#1086' '#1089#1083#1077#1076#1091#1102#1097#1080#1084#1080' '#1075#1083#1086#1073#1072#1083#1100#1085#1099#1084#1080' '#1087#1077#1088#1077#1084#1077#1085#1085#1099#1084#1080
          #1050#1086#1085#1089#1090#1072#1085#1090#1099' '#1079#1085#1072#1095#1077#1085#1080#1081' '#1082#1086#1084#1072#1085#1076
          '1) cmd_on_more      '#1042#1050#1051'-'#1041#1054#1051#1068#1064#1045
          '2) cmd_off_less     '#1054#1058#1050#1051'-'#1052#1045#1053#1068#1064#1045
          '3) cmd_tr_set       '#1059#1089#1090#1072#1085#1086#1074#1082#1072' '#1079#1085#1072#1095#1077#1085#1080#1103'  '
          '4) cmd_undo         '#1054#1090#1084#1077#1085#1072' '#1086#1087#1088#1077#1072#1094#1080#1080
          ''
          ''
          #1055#1077#1088#1077#1084#1077#1085#1085#1099#1077' '
          
            ' 9)  item_id           - id  OPC '#1087#1077#1088#1077#1084#1077#1085#1085#1086#1081' '#1085#1072#1076' '#1082#1086#1090#1086#1088#1086#1081' '#1087#1088#1086#1074#1086#1076#1080#1090 +
            #1089#1103' '#1086#1087#1088#1077#1072#1094#1080#1103' '#1058#1059'/'#1058#1056
          '10)  item_inverse      -  '#1087#1088#1080#1079#1085#1072#1082' '#1080#1085#1074#1077#1088#1089#1080#1080
          '11)  rc_check_success  - '#1085#1077#1086#1073#1093#1086#1076#1080#1084#1086#1089#1090#1100' '#1087#1088#1086#1074#1077#1088#1082#1080' '#1091#1089#1087#1077#1093#1072' '#1058#1059'/'#1058#1056
          '12)  rc_object         - '#1072#1076#1088#1077#1089' '#1090#1091' '#1089#1086#1086#1090#1074#1077#1090#1089#1090#1074#1091#1102#1097#1081' '#1086#1073#1098#1077#1082#1090#1091
          '13)  rc_tag            -  '#1080#1084#1103' '#1090#1077#1075#1072'  '#1076#1083#1103' '#1082#1086#1084#1072#1085#1076' '#1058#1059
          '14)  rc_command        - '#1082#1086#1076' '#1086#1087#1077#1088#1072#1094#1080#1080' '#1089#1084'. cmd_XXX  '#1087#1087'.1-4 '
          
            '15)  rc_value          - '#1076#1083#1103' '#1058#1056' '#1047#1085#1072#1095#1077#1085#1080#1077' '#1076#1086' '#1082#1086#1090#1086#1088#1086#1075#1086' '#1086#1089#1091#1097#1077#1089#1090#1074#1083#1103#1090 +
            #1100' '#1088#1077#1075#1091#1083#1080#1088#1086#1074#1072#1085#1080#1077
          ''
          '  ** '#1044#1086#1089#1090#1091#1087#1085#1099' '#1089#1083#1077#1076#1091#1102#1097#1080#1077' '#1074#1085#1077#1096#1085#1080#1077' '#1092#1091#1085#1082#1094#1080#1080' **'
          '__trace (string text)  '
          #1092#1091#1085#1082#1094#1080#1103' '#1074#1099#1074#1086#1076#1080#1090' '#1090#1077#1082#1089#1090' '#1074' '#1084#1086#1085#1080#1090#1086#1088' '#1080#1089#1087#1086#1083#1085#1077#1085#1080#1103' '#1089#1082#1088#1080#1087#1090#1086#1074
          ''
          'sleep( integer N ) '#1055#1088#1080#1086#1089#1090#1072#1085#1086#1074#1082#1072' '#1074#1099#1087#1086#1083#1085#1077#1085#1080#1103' '#1085#1072'   N '#1084#1080#1083#1083#1080#1089#1077#1082#1091#1085#1076
          ''
          'check_terminate () '#1055#1088#1086#1074#1077#1088#1082#1072' '#1085#1077#1086#1073#1093#1086#1076#1080#1084#1086#1089#1090#1080' '#1079#1072#1074#1077#1088#1096#1077#1085#1080#1103'  '
          #1074#1086#1079#1074#1088#1072#1097#1072#1077#1090' '#1085#1077' 0 '#1087#1088#1080' '#1090#1088#1077#1073#1086#1074#1072#1085#1080#1080' '#1079#1072#1074#1077#1088#1096#1077#1085#1080#1103
          #1085#1077#1086#1073#1093#1086#1076#1080#1084#1086' '#1086#1090#1087#1088#1072#1074#1080#1090#1100' '#1086#1090#1084#1077#1085#1091' '#1086#1087#1088#1077#1088#1072#1094#1080#1080
          ''
          'int bin_and(int v1,int v2  ) - '#1074#1086#1079#1074#1088#1072#1097#1072#1077#1090' '#1088#1077#1079#1091#1083#1100#1090#1072#1090' '#1076#1074#1086#1080#1095#1085#1086#1075#1086' '#1048
          
            'int bin_or (int v1,int v2  ) - '#1074#1086#1079#1074#1088#1072#1097#1072#1077#1090' '#1088#1077#1079#1091#1083#1100#1090#1072#1090' '#1076#1074#1086#1080#1095#1085#1086#1075#1086' '#1048#1051 +
            #1048
          
            'int bin_xor(int v1,int v2  ) - '#1074#1086#1079#1074#1088#1072#1097#1072#1077#1090' '#1088#1077#1079#1091#1083#1100#1090#1072#1090' '#1076#1074#1086#1080#1095#1085#1086#1075#1086' '#1080#1084 +
            #1082#1083#1102#1095#1072#1102#1097#1077#1075#1086' '#1048#1051#1048
          
            'int bin_not(int v          ) - '#1074#1086#1079#1074#1088#1072#1097#1072#1077#1090' '#1080#1085#1074#1077#1088#1089#1085#1086#1077' '#1079#1085#1072#1095#1077#1085#1080#1077' '#1072#1088#1075 +
            #1091#1084#1077#1085#1090#1072
          
            'int bin_shl(int v1,int n   ) - '#1074#1086#1079#1074#1088#1072#1097#1072#1077#1090' '#1088#1077#1079#1091#1083#1100#1090#1072#1090' '#1089#1076#1074#1080#1075#1072' '#1074#1083#1077#1074#1086 +
            '  '#1085#1072' n '#1073#1080#1090
          
            'int bin_shr(int v1,int n   ) - '#1074#1086#1079#1074#1088#1072#1097#1072#1077#1090' '#1088#1077#1079#1091#1083#1100#1090#1072#1090' '#1089#1076#1074#1080#1075#1072' '#1074#1087#1088#1072#1074 +
            #1086' '#1085#1072' n '#1073#1080#1090
          ''
          ''
          ''
          
            #1044#1083#1103' '#1074#1089#1077#1093' '#1085#1080#1078#1077#1089#1083#1077#1076#1091#1102#1097#1080#1093' '#1092#1091#1085#1082#1094#1080#1081' '#1087#1072#1088#1072#1084#1077#1090#1088' arg_item_id - '#1085#1077#1086#1073#1103#1079#1072#1090#1077#1083 +
            #1100#1085#1099#1081'. '
          
            #1042' '#1101#1090#1086#1084' '#1089#1083#1091#1095#1072#1077' arg_item_id '#1073#1077#1088#1077#1090#1089#1103'  '#1080#1079' '#1075#1083#1086#1073#1072#1083#1100#1085#1086#1081' '#1087#1077#1088#1077#1084#1077#1085#1085#1086#1081' item' +
            '_id;'
          ''
          
            'set_rc_prepare(int set  ,string arg_item_id)   '#1089#1084'. '#1091#1089#1090#1072#1085#1086#1074#1082#1072' '#1089#1073#1088 +
            #1086#1089' '#1087#1086#1076#1075#1086#1090#1086#1074#1082#1080
          
            'set_rc_active (int set  ,string arg_item_id)   '#1089#1084'. '#1091#1089#1090#1072#1085#1086#1074#1082#1072' '#1089#1073#1088 +
            #1086#1089' '#1072#1082#1090#1080#1074#1085#1086#1089#1090#1080
          
            'set_rc_error  (int set  ,string arg_item_id)   '#1089#1084'. '#1091#1089#1090#1072#1085#1086#1074#1082#1072' '#1089#1073#1088 +
            #1086#1089' '#1086#1096#1080#1073#1082#1080
          ''
          'get_value(string arg_item_id) '
          ' '#1074#1086#1079#1074#1088#1072#1097#1072#1077#1090' 3 '#1079#1085#1072#1095#1077#1085#1080#1103' '
          ' 1) 0 - '#1085#1077#1090' '#1087#1077#1088#1077#1084#1077#1085#1085#1086#1081' 1 - ok '
          ' 2) '#1090#1077#1082#1091#1097#1077#1077' '#1079#1085#1072#1095#1077#1085#1080#1077' '#1087#1077#1088#1077#1084#1077#1085#1085#1086#1081
          ' 3) '#1082#1072#1095#1077#1089#1090#1074#1086' '#1087#1077#1088#1077#1084#1077#1085#1085#1086#1081
          ''
          'set_value(double value,string arg_item_id) '#1047#1072#1087#1080#1089#1100' '#1079#1085#1072#1095#1077#1085#1080#1103' opc')
        ParentFont = False
        ReadOnly = True
        ScrollBars = ssBoth
        TabOrder = 0
        WantReturns = False
      end
    end
  end
  object OpenDialog1: TOpenDialog
    DefaultExt = '*.opc2otd'
    Filter = #1060#1072#1081#1083#1099' OPC->'#1054#1058#1044'|*.opc2otd'
    Options = [ofHideReadOnly, ofNoChangeDir, ofPathMustExist, ofFileMustExist, ofNoNetworkButton, ofEnableSizing, ofDontAddToRecent]
    Left = 448
    Top = 176
  end
end
