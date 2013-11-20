{ -*- compile-command: "sh compile.sh" -*- }
{
  Copyright 2004-2013 Michalis Kamburelis.

  This file is part of "interpolated_curves".

  "interpolated_curves" is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  "interpolated_curves" is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with "interpolated_curves"; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  ----------------------------------------------------------------------------
}

{ Simple 3D game. This unit implements the game logic,
  actually independent from Android. }
unit Game;

interface

uses CastleWindow;

var
  Window: TCastleWindow;

implementation

uses SysUtils, CastleControls, CastleUIControls, CastleRectangles,
  CastleGLUtils, CastleColors, X3DNodes;

var
  {$ifdef SOLID_BACKGROUND}
  Background: TCastleSimpleBackground;
  {$endif}
  Image: TCastleImageControl;
  ToggleShaderButton: TCastleButton;
  MyShaderEffect: TEffectNode;

type
  TDummy = class
    procedure ToggleShaderClick(Sender: TObject);
  end;

procedure TDummy.ToggleShaderClick(Sender: TObject);
begin
  if MyShaderEffect <> nil then
    MyShaderEffect.FdEnabled.Send(not MyShaderEffect.FdEnabled.Value);
end;

{ One-time initialization. }
procedure ApplicationInitialize;
begin
{$ifdef SOLID_BACKGROUND}
  { Show other controls under SceneManager, this way our Background
    is visible. Otherwise, Background defined in main 3D scene is used. }
  Window.SceneManager.Transparent := true;

  Background := TCastleSimpleBackground.Create(Window);
  Background.Color := Yellow;
  Window.Controls.InsertBack(Background);
{$endif}

  Image := TCastleImageControl.Create(Window);
  // TODO: png support for Android
  // TODO: read files using Anroid assets:
  // http://stackoverflow.com/questions/13317387/how-to-get-file-in-assets-from-android-ndk
//    Image.Image := TouchCtlOuter.MakeCopy;
  Image.URL := 'file:///sdcard/kambitest/sample_texture.ppm';
  Window.Controls.InsertFront(Image);

  Window.Load('file:///sdcard/kambitest/castle_with_lights_and_camera.wrl');

  ToggleShaderButton := TCastleButton.Create(Window);
  ToggleShaderButton.Caption := 'Toggle effect';
  ToggleShaderButton.OnClick := @TDummy(nil).ToggleShaderClick;
  Window.Controls.InsertFront(ToggleShaderButton);

  MyShaderEffect := Window.SceneManager.MainScene.RootNode.TryFindNodeByName(
    TEffectNode, 'MyShaderEffect', false) as TEffectNode;
end;

procedure WindowResize(Sender: TCastleWindowBase);
const
  Margin = 10;
begin
  Image.Left := Margin;
  Image.Bottom := Window.Height - Image.Height - Margin;

  ToggleShaderButton.Left := (Window.Width - ToggleShaderButton.Width) div 2;
  ToggleShaderButton.Bottom := Window.Height - ToggleShaderButton.Height - Margin;
end;

function MyGetApplicationName: string;
begin
  Result := 'cge_android_lib';
end;

initialization
  { This should be done as early as possible to mark our log lines correctly. }
  OnGetApplicationName := @MyGetApplicationName;

  { initialize Application callbacks }
  Application.OnInitialize := @ApplicationInitialize;

  { create Window and initialize Window callbacks }
  Window := TCastleWindow.Create(Application);
  Application.MainWindow := Window;
  Window.OnResize := @WindowResize;
end.
