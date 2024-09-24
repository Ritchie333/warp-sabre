
#ifndef SLIPPY_MAP_TILE_H
#define SLIPPY_MAP_TILE_H

#include <math.h>

// Source: http://wiki.openstreetmap.org/wiki/Slippy_map_tilenames
// Tiles are 256 × 256 pixel PNG files

int long2tile(double lon, int z);
int lat2tile(double lat, int z);
double tile2long(int x, int z);
double tile2lat(int y, int z);

#endif // SLIPPY_MAP_TILE_H
