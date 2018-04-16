inherited GKOpcItemsViewFrame: TGKOpcItemsViewFrame
  Width = 662
  Height = 356
  ExplicitWidth = 662
  ExplicitHeight = 356
  object ListView1: TListView
    Left = 0
    Top = 0
    Width = 662
    Height = 356
    Align = alClient
    Columns = <
      item
        Caption = 'id'
        Width = 150
      end
      item
        Caption = 'Quality'
      end
      item
        Caption = 'Value'
        Width = 80
      end
      item
        Caption = 'Calc'
        Width = 80
      end
      item
        Caption = 'Time'
        Width = 150
      end
      item
        Caption = 'Server'
        Width = 60
      end
      item
        Caption = 'Client'
        Width = 60
      end
      item
        Caption = 'DataType'
        Width = 60
      end
      item
        Caption = 'otd addr'
        Width = 80
      end>
    GridLines = True
    HideSelection = False
    ReadOnly = True
    RowSelect = True
    TabOrder = 0
    ViewStyle = vsReport
    OnDeletion = ListView1Deletion
    ExplicitTop = -3
  end
end
