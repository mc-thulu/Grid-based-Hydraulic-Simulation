# README

soon ...

## File layout

### Metadata (little-endian)

|Type|Description|
|-|-|
int_32|offset x
int_32|offset y
int_32|width
int_32|height
float_32|dt
float_32|padding
uint_64|sample resolution
float_32|height value for each cell (width x height many)

### Water level data (little-endian)

|Type|Description|
|-|-|
uint_32|number of following cells
{uint_32 + float_32}|for each cell: uint_64 index of that cell; float_32 for the water level

Exact layout and endian may vary on different platforms - WiP ...
