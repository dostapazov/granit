object tlm_entry_opts_frame: Ttlm_entry_opts_frame
  Left = 0
  Top = 0
  Width = 372
  Height = 189
  TabOrder = 0
  object GridPanel1: TGridPanel
    Left = 0
    Top = 0
    Width = 372
    Height = 189
    Align = alClient
    Caption = #1055#1072#1088#1072#1084#1077#1090#1088#1099' '#1086#1090#1086#1073#1088#1072#1078#1077#1085#1080#1103
    ColumnCollection = <
      item
        Value = 45.454545454545450000
      end
      item
        Value = 45.454545454545450000
      end
      item
        Value = 9.090909090909092000
      end>
    ControlCollection = <
      item
        Column = 0
        Control = Label1
        Row = 0
      end
      item
        Column = 0
        Control = Label2
        Row = 1
      end
      item
        Column = 0
        Control = Label3
        Row = 2
      end
      item
        Column = 0
        Control = Label4
        Row = 3
      end
      item
        Column = 1
        Control = RadioGroup1
        Row = 3
      end
      item
        Column = 0
        Control = Label5
        Row = 4
      end
      item
        Column = 1
        Control = Panel1
        Row = 4
      end
      item
        Column = 0
        Control = Label6
        Row = 5
      end
      item
        Column = 1
        Control = Panel2
        Row = 5
      end
      item
        Column = 1
        Control = ColorBox1
        Row = 2
      end
      item
        Column = 1
        Control = ComboBox1
        Row = 0
      end
      item
        Column = 1
        Control = ColorBox2
        Row = 1
      end
      item
        Column = 2
        Control = SpeedButton2
        Row = 1
      end
      item
        Column = 2
        Control = SpeedButton3
        Row = 2
      end
      item
        Column = 2
        Control = SpeedButton1
        Row = 0
      end>
    RowCollection = <
      item
        SizeStyle = ssAuto
        Value = 100.000000000000000000
      end
      item
        SizeStyle = ssAuto
      end
      item
        SizeStyle = ssAuto
      end
      item
        SizeStyle = ssAuto
      end
      item
        SizeStyle = ssAuto
      end
      item
        SizeStyle = ssAuto
      end
      item
        SizeStyle = ssAuto
      end>
    ShowCaption = False
    TabOrder = 0
    ExplicitHeight = 200
    DesignSize = (
      372
      189)
    object Label1: TLabel
      Left = 28
      Top = 5
      Width = 113
      Height = 13
      Anchors = []
      Caption = #1042#1072#1088#1080#1072#1085#1090' '#1086#1090#1086#1073#1088#1072#1078#1077#1085#1080#1103
      ExplicitLeft = 31
    end
    object Label2: TLabel
      Left = 57
      Top = 27
      Width = 55
      Height = 13
      Anchors = []
      Caption = #1062#1074#1077#1090' '#1092#1086#1085#1072
      ExplicitLeft = 60
      ExplicitTop = 26
    end
    object Label3: TLabel
      Left = 38
      Top = 49
      Width = 94
      Height = 13
      Anchors = []
      Caption = #1094#1074#1077#1090' '#1080#1079#1086#1073#1088#1072#1078#1077#1085#1080#1103
      ExplicitLeft = 40
      ExplicitTop = 48
    end
    object Label4: TLabel
      Left = 50
      Top = 94
      Width = 70
      Height = 13
      Anchors = []
      Caption = #1050#1086#1088#1086#1090#1082#1086#1077' '#1080#1084#1103
      ExplicitLeft = 52
      ExplicitTop = 66
    end
    object RadioGroup1: TRadioGroup
      Left = 169
      Top = 67
      Width = 168
      Height = 68
      Align = alClient
      Columns = 2
      ItemIndex = 0
      Items.Strings = (
        #1053#1077#1090
        #1089#1087#1088#1072#1074#1072
        #1089#1085#1080#1079#1091
        #1089#1083#1077#1074#1072
        #1089#1074#1077#1088#1093#1091)
      TabOrder = 0
    end
    object Label5: TLabel
      Left = 67
      Top = 141
      Width = 36
      Height = 13
      Anchors = []
      Caption = #1047#1085#1072#1082#1086#1074
      ExplicitLeft = 69
      ExplicitTop = 134
    end
    object Panel1: TPanel
      Left = 182
      Top = 135
      Width = 142
      Height = 25
      Anchors = []
      TabOrder = 1
      object Edit1: TEdit
        Left = 1
        Top = 1
        Width = 119
        Height = 23
        Align = alLeft
        NumbersOnly = True
        TabOrder = 0
        Text = '0'
      end
      object UpDown1: TUpDown
        Left = 120
        Top = 1
        Width = 17
        Height = 23
        Associate = Edit1
        TabOrder = 1
      end
    end
    object Label6: TLabel
      Left = 61
      Top = 165
      Width = 47
      Height = 13
      Anchors = []
      Caption = #1058#1086#1095#1085#1086#1089#1090#1100
      ExplicitLeft = 64
      ExplicitTop = 159
    end
    object Panel2: TPanel
      Left = 182
      Top = 160
      Width = 142
      Height = 24
      Anchors = []
      TabOrder = 2
      object Edit2: TEdit
        Left = 1
        Top = 1
        Width = 118
        Height = 22
        Align = alLeft
        TabOrder = 0
        Text = '0'
      end
      object UpDown2: TUpDown
        Left = 119
        Top = 1
        Width = 17
        Height = 22
        Associate = Edit2
        TabOrder = 1
      end
    end
    object ColorBox1: TColorBox
      Left = 169
      Top = 45
      Width = 168
      Height = 22
      Align = alClient
      TabOrder = 3
    end
    object ComboBox1: TComboBox
      Left = 169
      Top = 1
      Width = 168
      Height = 21
      Align = alClient
      TabOrder = 4
      Text = 'ComboBox1'
    end
    object ColorBox2: TColorBox
      Left = 169
      Top = 23
      Width = 168
      Height = 22
      Align = alClient
      TabOrder = 5
    end
    object SpeedButton2: TSpeedButton
      Left = 342
      Top = 23
      Width = 23
      Height = 22
      Anchors = []
      ExplicitLeft = 304
      ExplicitTop = 32
    end
    object SpeedButton3: TSpeedButton
      Left = 342
      Top = 45
      Width = 23
      Height = 22
      Anchors = []
      ExplicitLeft = 312
      ExplicitTop = 56
    end
    object SpeedButton1: TSpeedButton
      Left = 342
      Top = 1
      Width = 23
      Height = 22
      Anchors = []
      Visible = False
      ExplicitLeft = 344
      ExplicitTop = 0
    end
  end
end
