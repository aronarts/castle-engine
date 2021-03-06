{
  Copyright 2010-2017 Michalis Kamburelis.

  This file is part of "Castle Game Engine".

  "Castle Game Engine" is free software; see the file COPYING.txt,
  included in this distribution, for details about the copyright.

  "Castle Game Engine" is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  ----------------------------------------------------------------------------
}

{$ifdef read_interface}

  { An abstract slider user interface. Usually you want to use of its descendants,
    like @link(TCastleFloatSlider) or @link(TCastleIntegerSlider). }
  TCastleAbstractSlider = class(TUIControlFont)
  strict private
    FDisplayValue: boolean;
    FWidth: Cardinal;
    FHeight: Cardinal;
    FOnChange: TNotifyEvent;
    FCaption: string;
    procedure SetWidth(const Value: Cardinal);
    procedure SetHeight(const Value: Cardinal);
    function IndicatorWidth(const R: TRectangle): Integer;
    procedure SetCaption(const Value: string);
  private
    { Draw a slider at given Position. If Position is outside 0..1, it is clamped
      to 0..1 (this way we do not show slider at some wild position if it's
      outside the expected range; but DrawSliderText will still show the true,
      unclamped, value). }
    procedure DrawSliderPosition(const R: TRectangle; const Position: Single);

    { Returns a value of Position, always in 0..1 range,
      that would result in slider being drawn at XCoord screen position
      by DrawSliderPosition.
      Takes Rectangle as the rectangle currently occupied by the whole slider. }
    function XCoordToSliderPosition(const XCoord: Single;
      const R: TRectangle): Single;

    procedure DrawSliderText(const R: TRectangle; Text: string);
  strict protected
    { React to value change. The default implementation simply calls the OnChange
      event, if assigned. This is called when value is changed by user actions
      (key, mouse), not when it's explicitly assigned by code. }
    procedure DoChange; virtual;
  public
    const
      DefaultWidth = 200;
      DefaultHeight = 20;
    constructor Create(AOwner: TComponent); override;
    procedure Render; override;
    function Rect: TRectangle; override;
    procedure Update(const SecondsPassed: Single; var HandleInput: boolean); override;
  published
    property Width: Cardinal read FWidth write SetWidth default DefaultWidth;
    property Height: Cardinal read FHeight write SetHeight default DefaultHeight;

    property SmallFont default true;

    { Display the current value as text on the slider,
      right next to the @link(Caption).

      The exact method to display is defined by method
      @link(TCastleFloatSlider.ValueToStr) or
      @link(TCastleIntegerSlider.ValueToStr) (depending on descendant),
      so you can further customize it. }
    property DisplayValue: boolean
      read FDisplayValue write FDisplayValue default true;

    { Displayed on the slider. Right before value, if @link(DisplayValue). }
    property Caption: string read FCaption write SetCaption;

    property OnChange: TNotifyEvent read FOnChange write FOnChange;
  end;

  { Slider to change a float value within a given range. }
  TCastleFloatSlider = class(TCastleAbstractSlider)
  strict private
    FMin: Single;
    FMax: Single;
    FValue: Single;
    FMultipleOf: Single;
    procedure SetMin(const AMin: Single);
    procedure SetMax(const AMax: Single);
    procedure SetValue(const AValue: Single);
  public
    const
      DefaultMin = 0.0;
      DefaultMax = 1.0;
    constructor Create(AOwner: TComponent); override;
    procedure Render; override;
    function Press(const Event: TInputPressRelease): boolean; override;
    function Motion(const Event: TInputMotion): boolean; override;
    function ValueToStr(const AValue: Single): string; virtual;
    { Round to multiple of @link(MultipleOf), if non-zero, and clamp
      to @link(Min) and @link(Max) range. }
    function RoundAndClamp(const AValue: Single): Single;
  published
    property Min: Single read FMin write SetMin default DefaultMin;
    property Max: Single read FMax write SetMax default DefaultMax;

    { Current value. Usually within @link(Min) and @link(Max) range,
      although the general code should be ready for handle any value here
      (to work even during changes to @link(Min) and @link(Max) properties). }
    property Value: Single read FValue write SetValue default DefaultMin;

    { If non-zero, we force the value selected by user to be a multiple
      of this value (clamped to @link(Min), @link(Max) range).
      For example, if you set this to 0.25, and slider is between 0..1,
      then when user clicks around 0.3 --- we will pick 0.25. It user clicks
      around 0.4 --- we will pick 0.5.

      This only affects values selected by user interactions (clicking,
      dragging). This does not process the values you set by code to @link(Value)
      property, though you can use RoundAndClamp method on your values
      yourself. }
    property MultipleOf: Single read FMultipleOf write FMultipleOf;
  end;

  { Slider to change an integer value within a given range. }
  TCastleIntegerSlider = class(TCastleAbstractSlider)
  strict private
    FMin: Integer;
    FMax: Integer;
    FValue: Integer;
    function XCoordToValue(
      const XCoord: Single; const R: TRectangle): Integer;
    procedure SetMin(const AMin: Integer);
    procedure SetMax(const AMax: Integer);
    procedure SetValue(const AValue: Integer);
  public
    const
      DefaultMin = 0;
      DefaultMax = 10;
    constructor Create(AOwner: TComponent); override;
    procedure Render; override;
    function Press(const Event: TInputPressRelease): boolean; override;
    function Motion(const Event: TInputMotion): boolean; override;
    function ValueToStr(const AValue: Integer): string; virtual;
  published
    property Min: Integer read FMin write SetMin default DefaultMin;
    property Max: Integer read FMax write SetMax default DefaultMax;

    { Current value. Usually within @link(Min) and @link(Max) range,
      although the general code should be ready for handle any value here
      (to work even during changes to @link(Min) and @link(Max) properties). }
    property Value: Integer read FValue write SetValue default DefaultMin;
  end;

{$endif read_interface}

{$ifdef read_implementation}

{ TCastleAbstractSlider ------------------------------------------------------ }

constructor TCastleAbstractSlider.Create(AOwner: TComponent);
begin
  inherited;
  FDisplayValue := true;
  FWidth := DefaultWidth;
  FHeight := DefaultHeight;
  SmallFont := true;
end;

function TCastleAbstractSlider.Rect: TRectangle;
begin
  Result := Rectangle(Left, Bottom, Width, Height);
  // applying UIScale on this is easy...
  Result := Result.ScaleAround0(UIScale);
end;

procedure TCastleAbstractSlider.Render;
begin
  inherited;
  Theme.Draw(ScreenRect, tiSlider, UIScale);
end;

function TCastleAbstractSlider.IndicatorWidth(const R: TRectangle): Integer;
begin
  Result := R.Height div 2 { guess suitable tiSliderPosition width from height };
end;

procedure TCastleAbstractSlider.DrawSliderPosition(const R: TRectangle;
  const Position: Single);
var
  IndicatorW: Integer;
begin
  IndicatorW := IndicatorWidth(R);
  Theme.Draw(Rectangle(
    R.Left +
      Round(MapRangeClamped(Position, 0, 1, IndicatorW div 2, R.Width - IndicatorW div 2))
      - IndicatorW div 2,
    R.Bottom,
    IndicatorW,
    R.Height), tiSliderPosition, UIScale);
end;

function TCastleAbstractSlider.XCoordToSliderPosition(
  const XCoord: Single; const R: TRectangle): Single;
var
  IndicatorW: Integer;
begin
  IndicatorW := IndicatorWidth(R);
  Result := Clamped(MapRange(XCoord,
    R.Left + IndicatorW div 2,
    R.Left + R.Width - IndicatorW div 2, 0, 1), 0, 1);
end;

procedure TCastleAbstractSlider.DrawSliderText(
  const R: TRectangle; Text: string);
begin
  if (Caption <> '') and (Text <> '') then
    Text := Caption + ': ' + Text else
    Text := Caption + Text;
  Font.Print(
    R.Left + (R.Width - Font.TextWidth(Text)) div 2,
    R.Bottom + (R.Height - Font.RowHeight) div 2,
    Black, Text);
end;

procedure TCastleAbstractSlider.DoChange;
begin
  if Assigned(OnChange) then
    OnChange(Self);
end;

procedure TCastleAbstractSlider.SetWidth(const Value: Cardinal);
begin
  if FWidth <> Value then
  begin
    FWidth := Value;
    VisibleChange;
  end;
end;

procedure TCastleAbstractSlider.SetHeight(const Value: Cardinal);
begin
  if FHeight <> Value then
  begin
    FHeight := Value;
    VisibleChange;
  end;
end;

procedure TCastleAbstractSlider.SetCaption(const Value: string);
begin
  if FCaption <> Value then
  begin
    FCaption := Value;
    VisibleChange;
  end;
end;

procedure TCastleAbstractSlider.Update(const SecondsPassed: Single;
  var HandleInput: boolean);
begin
  inherited;
  { left / right arrow keys pressed are already handled by slider }
  if HandleInput and ExclusiveEvents and
     (Container.Pressed[K_Right] or Container.Pressed[K_Left]) then
    HandleInput := false;
end;

{ TCastleFloatSlider --------------------------------------------------------- }

constructor TCastleFloatSlider.Create(AOwner: TComponent);
begin
  inherited;
  FMin := DefaultMin;
  FMax := DefaultMax;
  FValue := FMin;
end;

procedure TCastleFloatSlider.Render;
var
  R: TRectangle;
begin
  inherited;
  R := ScreenRect;
  DrawSliderPosition(R, MapRange(Value, Min, Max, 0, 1));
  if DisplayValue then
    DrawSliderText(R, ValueToStr(Value));
end;

function TCastleFloatSlider.Press(const Event: TInputPressRelease): boolean;

  function ValueChange: Single;
  begin
    Result := (Max - Min) / 100;
  end;

begin
  Result := inherited;
  if Result then Exit;

  if Event.IsKey(K_Right) then
  begin
    if MultipleOf <> 0 then
      Value := CastleUtils.Min(Max, Value + MultipleOf) else
      Value := CastleUtils.Min(Max, Value + ValueChange);
    DoChange;
    Result := ExclusiveEvents;
  end else
  if Event.IsKey(K_Left) then
  begin
    if MultipleOf <> 0 then
      Value := CastleUtils.Max(Min, Value - MultipleOf) else
      Value := CastleUtils.Max(Min, Value - ValueChange);
    DoChange;
    Result := ExclusiveEvents;
  end else
  if Event.IsMouseButton(mbLeft) then
  begin
    Value := RoundAndClamp(MapRange(
      XCoordToSliderPosition(Event.Position[0], ScreenRect), 0, 1, Min, Max));
    DoChange;
    Result := ExclusiveEvents;
  end;
end;

function TCastleFloatSlider.Motion(const Event: TInputMotion): boolean;
begin
  Result := inherited;
  if Result then Exit;

  if mbLeft in Event.Pressed then
  begin
    Value := RoundAndClamp(MapRange(
      XCoordToSliderPosition(Event.Position[0], ScreenRect), 0, 1, Min, Max));
    DoChange;
    Result := ExclusiveEvents;
  end;
end;

function TCastleFloatSlider.ValueToStr(const AValue: Single): string;
begin
  Result := Format('%f', [AValue]);
end;

procedure TCastleFloatSlider.SetMin(const AMin: Single);
begin
  if FMin <> AMin then
  begin
    FMin := AMin;
    VisibleChange;
  end;
end;

procedure TCastleFloatSlider.SetMax(const AMax: Single);
begin
  if FMax <> AMax then
  begin
    FMax := AMax;
    VisibleChange;
  end;
end;

procedure TCastleFloatSlider.SetValue(const AValue: Single);
begin
  if FValue <> AValue then
  begin
    FValue := AValue;
    VisibleChange;
  end;
end;

function TCastleFloatSlider.RoundAndClamp(const AValue: Single): Single;
var
  DivResult: Int64;
  M, Remainder: Double;
begin
  if MultipleOf <> 0 then
  begin
    { we use FloatDivMod.
      We have to secure in case AValue or MultipleOf are < 0.
      For MultipleOf it's easy, just always use Abs(MultipleOf). }
    M := Abs(MultipleOf);
    if AValue >= 0 then
    begin
      FloatDivMod(AValue, M, DivResult, Remainder);
      if Remainder < M / 2 then
        Result := M * DivResult else
        Result := M * (DivResult + 1);
    end else
    begin
      FloatDivMod(-AValue, M, DivResult, Remainder);
      if Remainder < M / 2 then
        Result := M * DivResult else
        Result := M * (DivResult + 1);
      Result := -Result;
    end;
  end else
    Result := AValue;

  { Clamp at the end. If Min, Max are not a multiple of MultipleOf - so be it. }
  Result := Clamped(Result, Min, Max);
end;

{ TCastleIntegerSlider ------------------------------------------------------- }

constructor TCastleIntegerSlider.Create(AOwner: TComponent);
begin
  inherited;
  FMin := DefaultMin;
  FMax := DefaultMax;
  FValue := FMin;
end;

procedure TCastleIntegerSlider.Render;
var
  R: TRectangle;
begin
  inherited;
  R := ScreenRect;
  DrawSliderPosition(R, MapRange(Value, Min, Max, 0, 1));
  if DisplayValue then
    DrawSliderText(R, ValueToStr(Value));
end;

function TCastleIntegerSlider.Press(const Event: TInputPressRelease): boolean;
const
  ValueChange = 1;
begin
  Result := inherited;
  if Result then Exit;

  if Event.IsKey(K_Right) then
  begin
    Value := CastleUtils.Min(Max, Value + ValueChange);
    DoChange;
    Result := ExclusiveEvents;
  end else
  if Event.IsKey(K_Left) then
  begin
    Value := CastleUtils.Max(Min, Value - ValueChange);
    DoChange;
    Result := ExclusiveEvents;
  end else
  if Event.IsMouseButton(mbLeft) then
  begin
    Value := XCoordToValue(Event.Position[0], ScreenRect);
    DoChange;
    Result := ExclusiveEvents;
  end;
end;

function TCastleIntegerSlider.XCoordToValue(
  const XCoord: Single; const R: TRectangle): Integer;
begin
  { We do additional Clamped over Round result to avoid any
    chance of floating-point errors due to lack of precision. }
  Result := Clamped(Round(
    MapRange(XCoordToSliderPosition(XCoord, R), 0, 1,
      Min, Max)), Min, Max);
end;

function TCastleIntegerSlider.Motion(const Event: TInputMotion): boolean;
begin
  Result := inherited;
  if Result then Exit;

  if mbLeft in Event.Pressed then
  begin
    Value := XCoordToValue(Event.Position[0], ScreenRect);
    DoChange;
    Result := ExclusiveEvents;
  end;
end;

function TCastleIntegerSlider.ValueToStr(const AValue: Integer): string;
begin
  Result := IntToStr(AValue);
end;

procedure TCastleIntegerSlider.SetMin(const AMin: Integer);
begin
  if FMin <> AMin then
  begin
    FMin := AMin;
    VisibleChange;
  end;
end;

procedure TCastleIntegerSlider.SetMax(const AMax: Integer);
begin
  if FMax <> AMax then
  begin
    FMax := AMax;
    VisibleChange;
  end;
end;

procedure TCastleIntegerSlider.SetValue(const AValue: Integer);
begin
  if FValue <> AValue then
  begin
    FValue := AValue;
    VisibleChange;
  end;
end;

{$endif read_implementation}
