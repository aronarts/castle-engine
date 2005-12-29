{
  Copyright 2005 Michalis Kamburelis.

  This file is part of "Kambi's OpenGL Pascal units".

  "Kambi's OpenGL Pascal units" is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  "Kambi's OpenGL Pascal units" is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with "Kambi's OpenGL Pascal units"; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
}

{ Simple demo that you can change TGLWindow.MainMenu value at runtime,
  this is a simple way to temporary change whole menu structure
  to something else.
}

program menu_test_alternative;

uses GLWindow, GLW_Win;

var FirstMainMenu, SecondMainMenu: TMenu;

procedure MenuCommand(glwin: TGLWindow; Item: TMenuItem);
begin
 case Item.IntData of
  1: glwin.MainMenu := FirstMainMenu;
  2: glwin.MainMenu := SecondMainMenu;
  else Writeln('You clicked menu item "', Item.Caption, '"');
 end;
end;

var M: TMenu;
begin
 try
  { init FirstMainMenu }
  FirstMainMenu := TMenu.Create('Main menu');
  M := TMenu.Create('First menu');
    M.Append(TMenuItem.Create('Change to second menu', 2));
    M.Append(TMenuSeparator.Create);
    M.Append(TMenuItem.Create('Foo', 10, 'f'));
    M.Append(TMenuItem.Create('Bar', 11));
    FirstMainMenu.Append(M);

  { init SecondMainMenu }
  SecondMainMenu := TMenu.Create('Main menu');
  M := TMenu.Create('Second menu');
    M.Append(TMenuItem.Create('Change back to first menu', 1));
    M.Append(TMenuSeparator.Create);
    M.Append(TMenuItem.Create('Xyz' , 20, 'x'));
    M.Append(TMenuItem.Create('Blah', 21));
    SecondMainMenu.Append(M);

  { init glw properties related to menu }
  glw.OwnsMainMenu := false;
  glw.MainMenu := FirstMainMenu;
  glw.OnMenuCommand := MenuCommand;

  { init other glw properties }
  glw.OnResize := Resize2D;
  glw.Width := 300;
  glw.Height := 300;
  glw.ParsePars;
  glw.DepthBufferBits := 0;
  glw.Caption := 'Test changing MainMenu';

  { start }
  glw.InitLoop;
 finally
  FirstMainMenu.Free;
  SecondMainMenu.Free;
 end;
end.