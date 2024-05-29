
#ifndef TRANSFORM_POLY_H
#define TRANSFORM_POLY_H

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

/*template <class outType, class InType, class InTypeIt> outType CastVector (InType in) {
	outType out;
	for(InTypeIt it = in.begin();it!=in.end();it++)
	{
		out.push_back(*it);
	}
	return out;
}*/

#endif // TRANSFORM_POLY_H
