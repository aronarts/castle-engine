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
  To add new properties:
  - add them to the text files in nodes_specification/components/ ,
  - and regenerate include files by running
    nodes_specification/x3d-nodes-to-pascal/x3d-nodes-to-pascal.lpr .

  The documentation for properties should go to x3dnodes_documentation.txt . }

{$ifdef read_interface}

  private
    function GetMaxDepth: Integer;
    procedure SetMaxDepth(const Value: Integer);
    function GetLeafCapacity: Integer;
    procedure SetLeafCapacity(const Value: Integer);
  public
    property MaxDepth: Integer read GetMaxDepth write SetMaxDepth;
    property LeafCapacity: Integer read GetLeafCapacity write SetLeafCapacity;

{$endif read_interface}

{$ifdef read_implementation}

{ TKambiOctreePropertiesNode ----------------------------------------------- }

function TKambiOctreePropertiesNode.GetMaxDepth: Integer;
begin
  Result := FdMaxDepth.Value;
end;

procedure TKambiOctreePropertiesNode.SetMaxDepth(const Value: Integer);
begin
  FdMaxDepth.Send(Value);
end;

function TKambiOctreePropertiesNode.GetLeafCapacity: Integer;
begin
  Result := FdLeafCapacity.Value;
end;

procedure TKambiOctreePropertiesNode.SetLeafCapacity(const Value: Integer);
begin
  FdLeafCapacity.Send(Value);
end;

{$endif read_implementation}
