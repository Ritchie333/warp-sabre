
#ifndef TILE_H
#define TILE_H

#include "Point.h"

class Tile
{
public:
	double latmin, latmax, lonmin, lonmax;
	int sx, sy;
	Tile();
	virtual ~Tile();
	void Project(double lat, double lon, double &ixOut, double &iyOut);
	const Point Project(double lat, double lon);
	const Point Project( const Point& pt );
	void UnProject(double x, double y, double &latOut, double &lonOut);
};

#endif /// TILE_H
