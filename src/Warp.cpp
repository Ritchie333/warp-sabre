#include "TransformPoly.h"
#include "WriteKml.h"
#include "ErrorHandling.h"
#include "ImageWarpByFunc.h"
#include "CalibrationFile.h"
#include "StringUtils.h"
#include "Warp.h"
#include <vector>
using namespace std;

Warp::Warp()
{
    polynomialOrder = -1; //-1 is auto select
    mercatorOut = true;
    gbosOut = false;
    visualiseErrors = false;
    fitOnly = false;
    forceAspectCoord = false;
    outputWidth = -1;
    outputHeight = -1;
    projType = PolyProjectArgs::OSGB;
}

int Warp::Run()
{
    // Check basic parameters are set
	// if(inputFilename.size() == 0)
	//{cout << "Error: input sequence must be specified" << endl << desc << endl; exit(0);

	class Tile tile;
	cout << "Loading image..." << endl;
	class ImgMagick img;
	int openRet = img.Open(inputImageFilename.c_str());
	if (openRet < 0)
	{
		cout << "open " << inputImageFilename << " failed" << endl;
		return -1;
	}
    int outputWidth = -1;
	int outputHeight = -1;
	if (outputWidth == -1)
		outputWidth = img.GetWidth();
	if (outputHeight == -1)
		outputHeight = img.GetHeight();
	tile.sx = outputWidth;
	tile.sy = outputHeight;

	class CalibrationFile pointDef;
	if (pointDef.Open(inputPointsFilename.c_str()) < 0)
	{
		cout << "File not found" << inputPointsFilename << endl;
		return -1;
	}

	// Read points
	pointDef.mercatorOut = mercatorOut;
	pointDef.gbosOut = gbosOut;
	pointDef.corners = corners;
	pointDef.projType = projType;
	pointDef.ReadProjection();
	
	tile.latmin = pointDef.south;
	tile.latmax = pointDef.north;
	tile.lonmin = pointDef.west;
	tile.lonmax = pointDef.east;

	PolyProjection& srcImgToRef = pointDef.proj;

	// Auto determine polynomial order
	if (polynomialOrder == -1)
	{
		polynomialOrder = CalcOrderFitForNumConstraints(2 * srcImgToRef.originalPoints.size());
		cout << "Using " << polynomialOrder << "th order polynomial (" << srcImgToRef.originalPoints.size() << " points)" << endl;
		cout << "Need to determine " << CoeffSize(polynomialOrder) << " coeffs" << endl;
	}

	pointDef.proj.order = polynomialOrder;
	vector<double> poly = srcImgToRef.Estimate();

	double coordWidth = tile.lonmax - tile.lonmin;
	double coordHeight = tile.latmax - tile.latmin;
	// Get aspect ratio based on coordinates
	if (forceAspectCoord)
	{
		if (tile.sx < coordWidth * tile.sy / coordHeight)
			tile.sx = coordWidth * tile.sy / coordHeight;
		if (tile.sy < coordHeight * tile.sx / coordWidth)
			tile.sy = coordHeight * tile.sx / coordWidth;
	}
	cout << "Output image size " << tile.sx << " by " << tile.sy << " px" << endl;
	cout << "Output coordinates size " << coordWidth << " by " << coordHeight << endl;

	if (fitOnly)
		return 0;

	class ImgMagick endImage( tile.sx, tile.sy );
	endImage.Create();

	// vector<double> test;
	// test.push_back(333000);
	// test.push_back(550000);
	// vector<double> test2 = ProjRefToOutImg(test, tile);
	// cout << test2[0] << "," << test2[1] << endl;
	// exit(0);

	// Transform image
	class ImageWarpByFunc imageWarpByFunc( srcImgToRef.transformedPoints.size() );
	class PolyProjectArgs args;
	args.imgToRefPoly = poly;
	args.projType = projType;
	args.ptile = &tile;
	args.order = polynomialOrder;
	args.mercatorOut = mercatorOut;
	cout << "Warping image..." << endl;
	imageWarpByFunc.Warp(img, endImage, PolyProjectWithPtr, (void *)&args);

	if (visualiseErrors)
		for (unsigned int i = 0; i < srcImgToRef.transformedPoints.size(); i++)
		{
			// unsigned char col[3] = {255,0,0};
			// cout << polyEst.transformedPoints[i][0] << "," << polyEst.transformedPoints[i][1] << endl;
			DrawCross(endImage, srcImgToRef.transformedPoints[i].x, srcImgToRef.transformedPoints[i].y, 255, 0, 0);

			// endImage.draw_circle(polyEst.transformedPoints[i][0],polyEst.transformedPoints[i][1],3,col);

			// vector<double> proj = ProjRefToOutImg(polyEst.originalPoints[i], tile);
			DrawCross(endImage, srcImgToRef.originalPoints[i].x, srcImgToRef.originalPoints[i].y, 0, 0, 255);
		}

	cout << "Saving image..." << endl;
	string mapOutFilename = outputFilename + ".jpg";
	endImage.Save(mapOutFilename.c_str());

	string mapOutFileNoPath; // Don't save the path into the kml output
	mapOutFileNoPath = RemoveFilePath(mapOutFilename.c_str());

	if (mercatorOut)
	{
		class WriteKml writeKml;
		writeKml.north = tile.latmax;
		writeKml.south = tile.latmin;
		writeKml.west = tile.lonmin;
		writeKml.east = tile.lonmax;
		writeKml.description = kmlName; 
		string kmlOutFilename = outputFilename + ".kml";
		writeKml.href = mapOutFileNoPath;
		cout << "Writing KML to " << kmlOutFilename << endl;
		writeKml.WriteToFile(kmlOutFilename.c_str());
	}

	return 1;
}