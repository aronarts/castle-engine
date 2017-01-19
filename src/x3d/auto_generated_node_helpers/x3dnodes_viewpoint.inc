{ -*- buffer-read-only: t -*-

  Copyright 2015-2017 Michalis Kamburelis.

  This file is part of "Castle Game Engine".

  "Castle Game Engine" is free software; see the file COPYING.txt,
  included in this distribution, for details about the copyright.

  "Castle Game Engine" is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  ----------------------------------------------------------------------------
}

{ Automatically generated node properties.

  Do not edit this file manually!
  To add new properties, add them to text files in nodes_specification/components/ ,
  and regenerate include files by running
  nodes_specification/generate_x3d_nodes_to_pascal.lpr .

  The documentation for properties should go to x3dnodes_documentation.txt . }

{$ifdef read_interface}

  private
    function GetFieldOfView: Single;
    procedure SetFieldOfView(const Value: Single);
  public
    property FieldOfView: Single read GetFieldOfView write SetFieldOfView;

{$endif read_interface}

{$ifdef read_implementation}

{ TViewpointNode ----------------------------------------------- }

function TViewpointNode.GetFieldOfView: Single;
begin
  Result := FdFieldOfView.Value;
end;

procedure TViewpointNode.SetFieldOfView(const Value: Single);
begin
  FdFieldOfView.Send(Value);
end;

{$endif read_implementation}