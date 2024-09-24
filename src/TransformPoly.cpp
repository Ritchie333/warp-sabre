
#include "TransformPoly.h"
#include "newmat-10/newmat.h"
#include "ErrorHandling.h"
#include "gbos1936/Gbos1936.h"
#include "ImgFrameBase.h"
#include <iostream>
#include <math.h>
#ifdef use_namespace
using namespace NEWMAT;
#endif
// #define USE_OLD_POLYNOMIAL_EQ
#define HIGHEST_ALLOWED_POLY 100

vector<double> GetCoeff(int order, double x, double y)
{
	vector<double> out;

#ifdef USE_OLD_POLYNOMIAL_EQ
	// Old equation (<= version 0.4)
	for (int i = 0; i < order; i++)
	{
		out.push_back(pow(x, i + 1));
		out.push_back(pow(y, i + 1));
	}
	out.push_back(1.0);
#else
	// Correct equation
	for (int i = 0; i <= order; i++)
		for (int j = 0; j <= order; j++)
			out.push_back(pow(x, i) * pow(y, j));
#endif

	return out;
}

int CoeffSize(int order)
{
	vector<double> dummy = GetCoeff(order, 0.0, 0.0);
	return dummy.size();
}

int CalcOrderFitForNumConstraints(int numConstr)
{
	numConstr = numConstr / 2;
	for (unsigned int i = 1; i < HIGHEST_ALLOWED_POLY; i++)
	{
		if (CoeffSize(i) > numConstr)
			return i - 1;
	}
	return HIGHEST_ALLOWED_POLY;
}

Point PolyProject(const Point& point, vector<double> pose, int order)
{
	if (pose.size() == 0 || pose.size() % 2)
	{
		cout << "Polynomial coeff matrix size " << pose.size() << endl;
		ThrowError<logic_error>("Affine transform vector unexpected size", __LINE__, __FILE__);
	}
	
	vector<double> coeff = GetCoeff(order, point.x, point.y);

	double totx = 0.0;
	for (unsigned int i = 0; i < coeff.size(); i++)
	{
		totx += coeff[i] * pose[i];
	}

	double toty = 0.0;
	int indexOff = (pose.size() / 2);
	for (unsigned int i = 0; i < coeff.size(); i++)
	{
		toty += coeff[i] * pose[i + indexOff];
	}

	return Point(totx, toty);
}

/*vector<double> PolyUnProject(vector<double> point, vector<double>pose)
{

}*/

//**********************************************************************

PolyProjection::PolyProjection()
{
	order = 2;
}

PolyProjection::~PolyProjection()
{
}

void PolyProjection::Clear()
{
	originalPoints.clear();
	transformedPoints.clear();
}

void PolyProjection::AddPoint(const Point& original, const Point& transformed)
{
	originalPoints.push_back(original);
	transformedPoints.push_back(transformed);
}

void PolyProjection::AddPoint(double ox, double oy, double tx, double ty)
{
	Point temp( ox, oy );
	Point temp2( tx, ty );
	AddPoint(temp, temp2);
}

void PolyProjection::AddPoint(double ox, double oy, const Point& transformed)
{
	Point temp( ox, oy );
	AddPoint(temp, transformed);
}

void PrintMatrixP(Matrix &m)
{
	for (int i = 0; i < m.Nrows(); i++)
	{
		for (int j = 0; j < m.Ncols(); j++)
		{
			cout << m(i + 1, j + 1) << ",";
		}
		cout << endl;
	}
}

vector<double> PolyProjection::Estimate()
{
	if (originalPoints.size() != transformedPoints.size())
		ThrowError<logic_error>("Inconsistent number of points in constraints", __LINE__, __FILE__);
	if (originalPoints.size() == 0)
		ThrowError<logic_error>("Cannot estimate transform with no points", __LINE__, __FILE__);
	try
	{

		Matrix po((CoeffSize(order)), originalPoints.size());
		for (unsigned int i = 0; i < originalPoints.size(); i++)
		{

			vector<double> coeff = GetCoeff(order, originalPoints[i].x, originalPoints[i].y);
			for (unsigned int j = 0; j < coeff.size(); j++)
			{
				po(j + 1, i + 1) = coeff[j];
			}
		}
		// PrintMatrixP(po);

		Matrix pt(2, transformedPoints.size());
		for (unsigned int i = 0; i < transformedPoints.size(); i++)
		{
			pt(1, i + 1) = transformedPoints[i].x;
			pt(2, i + 1) = transformedPoints[i].y;
		}
		// PrintMatrixP(pt);

		Matrix invPo = po.t() * (po * po.t()).i(); // Pseudo inverse
		Matrix ptInvPo = pt * invPo;
		// PrintMatrixP(ptInvPo);

		// Convert 2D matrix into 1D for output
		vector<double> out;
		for (int i = 0; i < ptInvPo.Nrows(); i++)
		{
			for (int j = 0; j < ptInvPo.Ncols(); j++)
			{
				out.push_back(ptInvPo(i + 1, j + 1));
			}
		}

		// Project input to check for accuracy
		cout << "Projection accuracy check" << endl;
		double totalError = 0.0, totalCount = 0.0;
		for (unsigned int i = 0; i < originalPoints.size(); i++)
		{
			Point proj = PolyProject(originalPoints[i], out, order);
			cout << originalPoints[i].x << "," << originalPoints[i].y << "\t";
			cout << transformedPoints[i].x << "," << transformedPoints[i].y << "\t";
			cout << proj.x << "," << proj.y << "\t";
			double diff = pow(pow(transformedPoints[i].x - proj.x, 2.0) + pow(transformedPoints[i].y - proj.y, 2.0), 0.5);
			cout << diff << endl;
			totalError += diff;
			totalCount++;
		}
		cout << "Average pixel error " << totalError / totalCount << " units" << endl;

		return out;
	}
	catch (Exception)
	{
		cout << Exception::what() << endl;
	}
	vector<double> empty;
	return empty;
}

const Point ProjRefToOutImg(const Point& ref, PolyProjectArgs::ProjType projType, class Tile &tile, void *userPtr)
{
	HelmertConverter converter;
	class PolyProjectArgs *args = (class PolyProjectArgs *)userPtr;
	double lat = 0.0, lon = 0.0, alt = 0.0;
	int me = 0, mn = 0;
	switch (projType)
	{
	case PolyProjectArgs::OSGB:
		if (args->mercatorOut)
		{
			converter.ConvertGbos1936ToWgs84(ref.x, ref.y, 0.0, lat, lon, alt);
		}
		else
		{
			lat = ref.y;
			lon = ref.x;
			alt = 0.0;
		}
		break;
	case PolyProjectArgs::OSGBY:
		YardsToMetres( ref.x, ref.y, me, mn );
		converter.ConvertGbos1936ToWgs84(me, mn, 0.0, lat, lon, alt);
		break;

	case PolyProjectArgs::Mercator:
		lat = ref.y;
		lon = ref.x;
		alt = 0.0;
		break;
	case PolyProjectArgs::OSI:
		converter.ConvertOsiToWgs84(ref.x, ref.y, 0.0, lat, lon, alt);
		break;
	case PolyProjectArgs::Cassini:
		converter.ConvertCasToWgs84(ref.x, ref.y, 0.0, lat, lon, alt);
		break;
	case PolyProjectArgs::BonneS:
		converter.ConvertBnSToWgs84(ref.x, ref.y, 0.0, lat, lon, alt);
		break;
	case PolyProjectArgs::BonneI:
		converter.ConvertBnIToWgs84(ref.x, ref.y, 0.0, lat, lon, alt);
		break;
	case PolyProjectArgs::BonneF:
		converter.ConvertBnFToWgs84(ref.x, ref.y, 0.0, lat, lon, alt );
		break;
	case PolyProjectArgs::WO:
		converter.ConvertWOToWgs84(ref.x, ref.y, 0.0, lat, lon, alt);
		break;
	case PolyProjectArgs::WOI:
		converter.ConvertWOIToWgs84(ref.x, ref.y, 0.0, lat, lon, alt);
		break;
	}
	if (projType != PolyProjectArgs::OSGB && !args->mercatorOut)
	{
		cout << "Error: not implemented" << endl;
		exit(0);
	}

	Point pout = tile.Project(lat, lon);
	// if(ref[0] >= 333000 && ref[1] <= 550000)
	//{
		// cout << "corner " << ref[0] << "," << ref[1] << "\t" << pout[0] << "," << pout[1] << endl;
	//}

	return pout;
}

const Point PolyProjectWithPtr(const Point& in, void *userPtr)
{
	class PolyProjectArgs *args = (class PolyProjectArgs *)userPtr;
	const Point ref = PolyProject(in, args->imgToRefPoly, args->order);
	if (!args->ptile)
		throw(0);
	/*double lat=0.0, lon=0.0, alt=0.0;
	ConvertGbos1936ToWgs84(ref[0], ref[1],0.0, lat, lon, alt);
	vector<double> pout;
	//cout << args->ptile->latmin << "\t" << args->ptile->latmax << "\t" << args->ptile->sy << endl;

	args->ptile->Project(lat, lon, pout);
	//cout << ref[0] << "\t" << ref[1] << "\t" << pout[0] << "\t" << pout[1] << endl;
	return pout;*/
	return ProjRefToOutImg(ref, args->projType, *args->ptile, userPtr);
}

void AddPointPoly(class Tile &tile, class PolyProjection &polyEst, double lat, double lon, double x, double y)
{
	// cout << "AddPointPoly " << lat << "," << lon << "," << x << "," << y << endl;
	Point p = tile.Project(lat, lon);
	polyEst.AddPoint(x, y, p);
}

void SplitGbosRef(string in, string &zone, long &easting, long &northing)
{
	// cout << in << endl;
	zone = in.substr(0, 2);
	if (in.length() != 8)
		ThrowError<logic_error>("Unexpected length of map reference", __LINE__, __FILE__);
	easting = atoi(in.substr(2, 3).c_str()) * 100;
	northing = atoi(in.substr(5, 3).c_str()) * 100;
	// cout << zone << "," << easting << "," << northing << endl;
}

void DrawCross(class ImgFrameBase &img, int x, int y, double r, double g, double b)
{
	cout << x << "," << y << endl;
	for (int i = -10; i <= 10; i++)
		for (int j = -10; j <= 10; j++)
		{
			img.SetPix(x + i, y + j, 0, r);
			img.SetPix(x + i, y + j, 1, g);
			img.SetPix(x + i, y + j, 2, b);
		}
}