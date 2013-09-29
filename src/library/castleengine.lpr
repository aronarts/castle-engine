{$MODE ObjFPC}
Library castleengine;

{ Adapted from Jonas Maebe's example project :
  http://users.elis.ugent.be/~jmaebe/fpc/FPC_Objective-C_Cocoa.tbz
  http://julien.marcel.free.fr/ObjP_Part7.html

  To make c-compatible (and XCode-compatible) libraries, you must :
  1- use c-types arguments
  2- add a "cdecl" declaration after your functions declarations
  3- export your functions
}

uses
  ctypes, math, CastleFrame, Classes, CastleKeysMouse, CastleCameras, sysutils;

{ See http://fpc.freedoors.org/dos204full/source/rtl/unix/ctypes.inc
  For a full list of c-types
}

var
  aCastleFrame: TCastleFrame;

procedure CGE_Init(); cdecl;
begin
  try
    aCastleFrame := TCastleFrame.Create(nil);
    aCastleFrame.GLContextOpen;
  except
  end;
end;

procedure CGE_Close; cdecl;
begin
  try
    aCastleFrame.Destroy();
    aCastleFrame := nil;
  except
  end;
end;

procedure CGE_SetRenderParams(uiViewWidth, uiViewHeight: cUInt32); cdecl;
begin
  try
    aCastleFrame.SetRenderSize(uiViewWidth, uiViewHeight);
  except
  end;
end;

procedure CGE_Render; cdecl;
begin
  try
    aCastleFrame.Paint();
  except
    //on E: Exception do OutputDebugString(@E.Message[1]);
  end;
end;

procedure CGE_SetLibraryCallbackProc(aProc: TCgeLibraryCallbackProc); cdecl;
begin
  try
    aCastleFrame.SetLibraryCallbackProc(aProc);
  except
    //on E: Exception do OutputDebugString(@E.Message[1]);
  end;
end;

procedure CGE_OnIdle; cdecl;
begin
  try
    aCastleFrame.Update();
  except
    //on E: Exception do OutputDebugString(@E.Message[1]);
  end;
end;

function CGE_ShiftToFPCShift(uiShift: cUInt32): TShiftState;
var
  Shift: TShiftState;
begin
  Shift := [];
  if (uiShift and 1)=1 then Include(Shift, ssShift);
  if (uiShift and 2)=2 then Include(Shift, ssAlt);
  if (uiShift and 4)=4 then Include(Shift, ssCtrl);
  Result := Shift;
end;

procedure CGE_OnMouseDown(x, y: cInt32; bLeftBtn: cBool; uiShift: cUInt32); cdecl;
var
  MyButton: TMouseButton;
  Shift: TShiftState;
begin
  try
    if (bLeftBtn) then MyButton := mbLeft else MyButton := mbRight;
    Shift := CGE_ShiftToFPCShift(uiShift);
    aCastleFrame.OnMouseDown(MyButton, Shift, x, y);
  except
  end;
end;

procedure CGE_OnMouseMove(x, y: cInt32; uiShift: cUInt32); cdecl;
var
  Shift: TShiftState;
begin
  try
    Shift := CGE_ShiftToFPCShift(uiShift);
    aCastleFrame.OnMouseMove(Shift, x, y);
  except
  end;
end;

procedure CGE_OnMouseUp(x, y: cInt32; bLeftBtn: cBool; uiShift: cUInt32); cdecl;
var
  MyButton: TMouseButton;
  Shift: TShiftState;
begin
  try
    if (bLeftBtn) then MyButton := mbLeft else MyButton := mbRight;
    Shift := CGE_ShiftToFPCShift(uiShift);
    aCastleFrame.OnMouseUp(MyButton, Shift, x, y);
  except
  end;
end;

procedure CGE_OnMouseWheel(zDelta: cFloat; bVertical: cBool; uiShift: cUint32); cdecl;
begin
  try
    aCastleFrame.OnMouseWheel(zDelta/120, bVertical);
  except
  end;
end;

procedure CGE_LoadSceneFromFile(szFile: pcchar); cdecl;
begin
  try
    aCastleFrame.Load(StrPas(PChar(szFile)));
  except
    //on E: Exception do OutputDebugString(@E.Message[1]);
  end;
end;

function CGE_GetViewpointsCount(): cInt32; cdecl;
begin
  try
    Result := aCastleFrame.GetViewpointsCount();
  except
    Result := 0;
  end;
end;

procedure CGE_GetViewpointName(iViewpointIdx: cInt32; szName: pchar; nBufSize: cInt32); cdecl;
var
  sName: string;
begin
  try
    sName := aCastleFrame.GetViewpointName(iViewpointIdx);
    if (Length(sName) > nBufSize-1) then
      sName := Copy(sName, 1, nBufSize-1);
    StrPCopy(szName, sName);
  except
  end;
end;

procedure CGE_MoveToViewpoint(iViewpointIdx: cInt32; bAnimated: cBool); cdecl;
begin
  try
    aCastleFrame.MoveToViewpoint(iViewpointIdx, bAnimated);
  except
  end;
end;

function CGE_GetCurrentNavigationType(): cInt32; cdecl;
var
  aNavType: TCameraNavigationType;
begin
  Result := 0;
  try
    aNavType := aCastleFrame.GetCurrentNavigationType();
    if (aNavType = ntWalk) then Result := 0
    else if (aNavType = ntFly) then Result := 1
    else if (aNavType = ntExamine) then Result := 2
    else if (aNavType = ntArchitecture) then Result := 3;
  except
  end;
end;

procedure CGE_SetNavigationType(NewType: cInt32); cdecl;
var
  aNavType: TCameraNavigationType;
begin
  try
    case NewType of
    0: aNavType := ntWalk;
    1: aNavType := ntFly;
    2: aNavType := ntExamine;
    3: aNavType := ntArchitecture;
    else aNavType := ntExamine;
    end;
    aCastleFrame.SetNavigationType(aNavType);
  except
  end;
end;

procedure CGE_UpdateTouchInterface(eMode: cInt32); cdecl;
var
  aNewMode: TTouchCtlInterface;
begin
  try
    case eMode of
    0: aNewMode := etciNone;
    1: aNewMode := etciCtlWalkCtlRotate;
    2: aNewMode := etciCtlWalkDragRotate;
    else aNewMode := etciNone;
    end;
    aCastleFrame.UpdateTouchInterface(aNewMode);
  except
  end;
end;

exports
  CGE_Init, CGE_Close,
  CGE_Render, CGE_SetRenderParams, CGE_SetLibraryCallbackProc, CGE_OnIdle,
  CGE_OnMouseDown, CGE_OnMouseMove, CGE_OnMouseUp, CGE_OnMouseWheel,
  CGE_LoadSceneFromFile, CGE_GetCurrentNavigationType, CGE_SetNavigationType,
  CGE_GetViewpointsCount, CGE_GetViewpointName, CGE_MoveToViewpoint,
  CGE_UpdateTouchInterface;

begin
  {Do not remove the exception masking lines}
  SetExceptionMask([exInvalidOp, exDenormalized, exZeroDivide, exOverflow, exUnderflow, exPrecision]);
  Set8087CW(Get8087CW or $3f);
end.
