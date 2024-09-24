
#ifndef TRANSFORM_POLY_H
#define TRANSFORM_POLY_H

#include "Tile.h"
#include "Point.h"

#include <vector>
using namespace std;

Point PolyProject(const Point& point, vector<double> pose, int order);
// vector<double> PolyUnProject(vector<double> point, vector<double>pose);
int CoeffSize(int order);
int CalcOrderFitForNumConstraints(int numConstr);

class PolyProjection
{
public:
	PolyProjection();
	virtual ~PolyProjection();

	// vector<double> Project(vector<double> point, vector<double>pose) {vector<double> empty; return empty;};
	// vector<double> UnProject(vector<double> point, vector<double>pose) {vector<double> empty; return empty;};

	void Clear();
	void AddPoint(const Point& original, const Point& transformed);
	void AddPoint(double ox, double oy, double tx, double ty);
	void AddPoint(double ox, double oy, const Point& transformed);
	vector<double> Estimate();

	vector<Point> originalPoints, transformedPoints;
	int order;
};

class PolyProjectArgs
{
public:
	typedef enum
	{
		OSGB = 0,
		Mercator,
		Cassini,
		BonneS,
		BonneI,
		BonneF,
		OSI,
		WO,
		WOI,
		OSGBY
	} ProjType;

	vector<double> imgToRefPoly;
	class Tile *ptile;
	ProjType projType;
	int mercatorOut;
	int order;

	PolyProjectArgs()
	{
		ptile = 0;
		projType = OSGB;
		mercatorOut = 0;
		order = 2;
	}
};

const Point ProjRefToOutImg(const Point& ref, PolyProjectArgs::ProjType projType, class Tile &tile, void *userPtr);
const Point PolyProjectWithPtr(const Point& in, void *userPtr);
void AddPointPoly(class Tile &tile, class PolyProjection &polyEst, double lat, double lon, double x, double y);
void SplitGbosRef(string in, string &zone, long &easting, long &northing);
void DrawCross(class ImgFrameBase &img, int x, int y, double r, double g, double b);

/*template <class outType, class InType, class InTypeIt> outType CastVector (InType in) {
	outType out;
	for(InTypeIt it = in.begin();it!=in.end();it++)
	{
		out.push_back(*it);
	}
	return out;
}*/

#endif // TRANSFORM_POLY_H
