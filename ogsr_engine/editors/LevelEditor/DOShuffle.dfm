object frmDOShuffle: TfrmDOShuffle
  Left = 676
  Top = 239
  Width = 325
  Height = 454
  BorderIcons = [biSystemMenu]
  Caption = 'Detail objects'
  Color = 10528425
  Constraints.MaxWidth = 325
  Constraints.MinHeight = 454
  Constraints.MinWidth = 325
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  KeyPreview = True
  OldCreateOrder = False
  OnClose = FormClose
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  OnKeyDown = FormKeyDown
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object paTools: TPanel
    Left = 158
    Top = 0
    Width = 159
    Height = 427
    Align = alRight
    BevelOuter = bvNone
    ParentColor = True
    TabOrder = 0
    object Panel3: TPanel
      Left = 0
      Top = 0
      Width = 159
      Height = 19
      Align = alTop
      BevelOuter = bvNone
      ParentColor = True
      TabOrder = 0
      object ebAppendIndex: TExtBtn
        Left = 28
        Top = 0
        Width = 130
        Height = 18
        Align = alNone
        BevelShow = False
        Caption = 'Append Color Index'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        FlatAlwaysEdge = True
        OnClick = ebAppendIndexClick
      end
      object ebMultiClear: TExtBtn
        Left = 1
        Top = 0
        Width = 26
        Height = 18
        Align = alNone
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        Glyph.Data = {
          C2010000424DC20100000000000036000000280000000C0000000B0000000100
          1800000000008C010000120B0000120B00000000000000000000FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFF000000000000FFFFFFFFFFFFFFFFFFFFFFFF000000000000FF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFF000000000000FFFFFFFFFFFF000000000000
          FFFFFFFFFFFFFFFFFF000000000000FFFFFFFFFFFF0000000000000000000000
          00FFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000000000FFFFFFFFFFFF00000000
          0000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000000000000000
          000000000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0000
          00000000FFFFFFFFFFFF000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00
          0000000000000000000000FFFFFFFFFFFF000000000000FFFFFFFFFFFFFFFFFF
          000000000000FFFFFFFFFFFF000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FF000000000000FFFFFFFFFFFFFFFFFFFFFFFF000000000000FFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFF}
        ParentFont = False
        Spacing = 0
        FlatAlwaysEdge = True
        OnClick = ebMultiClearClick
      end
    end
    object sbDO: TScrollBox
      Left = 0
      Top = 19
      Width = 159
      Height = 408
      VertScrollBar.ButtonSize = 11
      VertScrollBar.Increment = 10
      VertScrollBar.Size = 11
      VertScrollBar.Style = ssHotTrack
      VertScrollBar.Tracking = True
      Align = alClient
      BorderStyle = bsNone
      TabOrder = 1
    end
  end
  object Panel2: TPanel
    Left = 0
    Top = 0
    Width = 158
    Height = 427
    Align = alClient
    BevelOuter = bvNone
    ParentColor = True
    TabOrder = 1
    object Splitter1: TSplitter
      Left = 0
      Top = 241
      Width = 158
      Height = 3
      Cursor = crVSplit
      Align = alTop
      Beveled = True
      ResizeStyle = rsUpdate
    end
    object tvItems: TElTree
      Left = 0
      Top = 244
      Width = 158
      Height = 164
      Cursor = crDefault
      LeftPosition = 0
      DragCursor = crDrag
      Align = alClient
      AlwaysKeepSelection = False
      AutoCollapse = False
      AutoExpand = True
      DockOrientation = doNoOrient
      DefaultSectionWidth = 120
      BorderSides = [ebsLeft, ebsRight, ebsTop, ebsBottom]
      Color = clGray
      DragAllowed = True
      DrawFocusRect = False
      ExplorerEditMode = False
      Flat = True
      FocusedSelectColor = 10526880
      FocusedSelectTextColor = clBlack
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      FullRowSelect = False
      GradientSteps = 64
      HeaderHeight = 19
      HeaderHotTrack = False
      HeaderSections.Data = {
        F4FFFFFF0700000094EAD112002A0000FFFFFFFF000001010100000078000000
        000000001027000000010001401C7A0300000000000019010000000000000100
        00000000000000000001000000000000150A0D403E0A0D40620A0D40850A0D40
        A80A0D40CA0A0D40EC0A0D40120B0D40340B0D40620B0D408B0B0D40B00B0D40
        D50B0D40FA0B0D401F0C0D40440C0D40680C0D40010000000001000000000000
        00000000000094EAD112002A0000FFFFFFFF0000010101000000780000000000
        00001027000000010001F4410610010000000000190100000000000001000000
        0000000000000001000000000000150A0D403E0A0D40620A0D40850A0D40A80A
        0D40CA0A0D40EC0A0D40120B0D40340B0D40620B0D408B0B0D40B00B0D40D50B
        0D40FA0B0D401F0C0D40440C0D40680C0D400100000000010000000000000000
        0000000094EAD112002A0000FFFFFFFF00000101010000007800000000000000
        1027000000010001A86C620E0200000000001901000000000000010000000000
        000000000001000000000000150A0D403E0A0D40620A0D40850A0D40A80A0D40
        CA0A0D40EC0A0D40120B0D40340B0D40620B0D408B0B0D40B00B0D40D50B0D40
        FA0B0D401F0C0D40440C0D40680C0D4001000000000100000000000000000000
        000094EAD112002A0000FFFFFFFF000001010100000078000000000000001027
        0000000100010051320F03000000000019010000000000000100000000000000
        00000001000000000000150A0D403E0A0D40620A0D40850A0D40A80A0D40CA0A
        0D40EC0A0D40120B0D40340B0D40620B0D408B0B0D40B00B0D40D50B0D40FA0B
        0D401F0C0D40440C0D40680C0D40010000000001000000000000000000000000
        94EAD112002A0000FFFFFFFF0000010101000000780000000000000010270000
        000100017802F702040000000000190100000000000001000000000000000000
        0001000000000000150A0D403E0A0D40620A0D40850A0D40A80A0D40CA0A0D40
        EC0A0D40120B0D40340B0D40620B0D408B0B0D40B00B0D40D50B0D40FA0B0D40
        1F0C0D40440C0D40680C0D4001000000000100000000000000000000000094EA
        D112002A0000FFFFFFFF00000101010000007800000000000000102700000001
        0001B4E1DA040500000000001901000000000000010000000000000000000001
        000000000000150A0D403E0A0D40620A0D40850A0D40A80A0D40CA0A0D40EC0A
        0D40120B0D40340B0D40620B0D408B0B0D40B00B0D40D50B0D40FA0B0D401F0C
        0D40440C0D40680C0D4001000000000100000000000000000000000094EAD112
        002A0000FFFFFFFF000001010100000078000000000000001027000000010001
        288C870D06000000000019010000000000000100000000000000000000010000
        00000000150A0D403E0A0D40620A0D40850A0D40A80A0D40CA0A0D40EC0A0D40
        120B0D40340B0D40620B0D408B0B0D40B00B0D40D50B0D40FA0B0D401F0C0D40
        440C0D40680C0D40010000000001000000000000000000000000}
      HeaderFont.Charset = DEFAULT_CHARSET
      HeaderFont.Color = clWindowText
      HeaderFont.Height = -11
      HeaderFont.Name = 'MS Sans Serif'
      HeaderFont.Style = []
      HorizontalLines = True
      HorzDivLinesColor = 7368816
      HorzScrollBarStyles.ShowTrackHint = False
      HorzScrollBarStyles.Width = 16
      HorzScrollBarStyles.ButtonSize = 14
      HorzScrollBarStyles.UseSystemMetrics = False
      HorzScrollBarStyles.UseXPThemes = False
      IgnoreEnabled = False
      IncrementalSearch = False
      ItemIndent = 14
      KeepSelectionWithinLevel = False
      LineBorderActiveColor = clBlack
      LineBorderInactiveColor = clBlack
      LineHeight = 16
      LinesColor = clBlack
      MouseFrameSelect = True
      MultiSelect = False
      OwnerDrawMask = '~~@~~'
      ParentFont = False
      PlusMinusTransparent = True
      ScrollbarOpposite = False
      ScrollTracking = True
      ShowButtons = False
      ShowImages = False
      ShowLeafButton = False
      ShowLines = False
      ShowRootButtons = False
      SortMode = smAdd
      StoragePath = '\Tree'
      TabOrder = 0
      TabStop = True
      Tracking = False
      TrackColor = clBlack
      VertScrollBarStyles.ShowTrackHint = True
      VertScrollBarStyles.Width = 16
      VertScrollBarStyles.ButtonSize = 14
      VertScrollBarStyles.UseSystemMetrics = False
      VertScrollBarStyles.UseXPThemes = False
      VirtualityLevel = vlNone
      UseXPThemes = False
      TextColor = clBtnText
      BkColor = clGray
      OnItemFocused = tvItemsItemFocused
      OnDragDrop = tvItemsDragDrop
      OnDragOver = tvItemsDragOver
      OnStartDrag = tvItemsStartDrag
      OnKeyPress = tvItemsKeyPress
    end
    object paObject: TPanel
      Left = 0
      Top = 0
      Width = 158
      Height = 241
      Align = alTop
      BevelOuter = bvNone
      Constraints.MinHeight = 240
      ParentColor = True
      TabOrder = 1
      object paObjectProps: TPanel
        Left = 0
        Top = 158
        Width = 158
        Height = 83
        Align = alClient
        BevelOuter = bvNone
        ParentColor = True
        TabOrder = 0
      end
      object paImage: TMxPanel
        Left = 0
        Top = 0
        Width = 158
        Height = 158
        Align = alTop
        BevelOuter = bvLowered
        Caption = '<no image>'
        Color = 8750469
        TabOrder = 1
        OnPaint = paImagePaint
      end
    end
    object Panel1: TPanel
      Left = 0
      Top = 408
      Width = 158
      Height = 19
      Align = alBottom
      BevelOuter = bvNone
      ParentColor = True
      TabOrder = 2
      object ebAddObject: TExtBtn
        Left = 1
        Top = 1
        Width = 26
        Height = 17
        Align = alNone
        BevelShow = False
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        Glyph.Data = {
          AE000000424DAE00000000000000360000002800000006000000060000000100
          18000000000078000000120B0000120B00000000000000000000FFFFFFFFFFFF
          1C1C1C1C1C1CFFFFFFFFFFFF0000FFFFFFFFFFFF191919191919FFFFFFFFFFFF
          00001B1B1B1717170303030303031717171B1B1B00001B1B1B17171703030303
          03031717171B1B1B0000FFFFFFFFFFFF191919191919FFFFFFFFFFFF0000FFFF
          FFFFFFFF1C1C1C1C1C1CFFFFFFFFFFFF0000}
        ParentFont = False
        FlatAlwaysEdge = True
        OnClick = ebAddObjectClick
      end
      object ebDelObject: TExtBtn
        Left = 27
        Top = 1
        Width = 26
        Height = 17
        Align = alNone
        BevelShow = False
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        Glyph.Data = {
          AE000000424DAE00000000000000360000002800000006000000060000000100
          18000000000078000000120B0000120B00000000000000000000FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFF0000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          00000B0B0B0B0B0B0B0B0B0B0B0B0B0B0B0B0B0B00000B0B0B0B0B0B0B0B0B0B
          0B0B0B0B0B0B0B0B0000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0000FFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0000}
        ParentFont = False
        FlatAlwaysEdge = True
        OnClick = ebDelObjectClick
      end
      object ebLoadList: TExtBtn
        Left = 79
        Top = 1
        Width = 39
        Height = 17
        Align = alNone
        BevelShow = False
        Caption = 'Load...'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        FlatAlwaysEdge = True
        OnClick = ebLoadListClick
      end
      object ebSaveList: TExtBtn
        Left = 118
        Top = 1
        Width = 39
        Height = 17
        Align = alNone
        BevelShow = False
        Caption = 'Save...'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        FlatAlwaysEdge = True
        OnClick = ebSaveListClick
      end
      object ebClearList: TExtBtn
        Left = 53
        Top = 1
        Width = 26
        Height = 17
        Align = alNone
        BevelShow = False
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        Glyph.Data = {
          C2010000424DC20100000000000036000000280000000C0000000B0000000100
          1800000000008C010000120B0000120B00000000000000000000FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFF000000000000FFFFFFFFFFFFFFFFFFFFFFFF000000000000FF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFF000000000000FFFFFFFFFFFF000000000000
          FFFFFFFFFFFFFFFFFF000000000000FFFFFFFFFFFF0000000000000000000000
          00FFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000000000FFFFFFFFFFFF00000000
          0000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000000000000000
          000000000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0000
          00000000FFFFFFFFFFFF000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00
          0000000000000000000000FFFFFFFFFFFF000000000000FFFFFFFFFFFFFFFFFF
          000000000000FFFFFFFFFFFF000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FF000000000000FFFFFFFFFFFFFFFFFFFFFFFF000000000000FFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFF}
        ParentFont = False
        FlatAlwaysEdge = True
        OnClick = ebClearListClick
      end
    end
  end
  object fsStorage: TFormStorage
    OnSavePlacement = fsStorageSavePlacement
    OnRestorePlacement = fsStorageRestorePlacement
    StoredProps.Strings = (
      'paObject.Height')
    StoredValues = <
      item
        Name = 'EmitterDirX'
        Value = 0
      end
      item
        Name = 'EmitterDirY'
        Value = 0
      end
      item
        Name = 'EmitterDirZ'
        Value = 0
      end>
  end
end
