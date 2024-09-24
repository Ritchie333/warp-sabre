#include <iostream>
#include <fstream>
#include <iomanip>
using namespace std;
#include "TransformPoly.h"
#include "WriteKml.h"
#include "ErrorHandling.h"
#include "ImageWarpByFunc.h"
#include "CalibrationFile.h"
#include "ProgramOptions.h"
#include "ganzc/LatLong-OSGBconversion.h" //Poor accuracy?
#include "gbos1936/Gbos1936.h"
#include "Tile.h"
#include <math.h>
#include "StringUtils.h"

//**************************************************

int main(int argc, char *argv[])
{
	string inputImageFilename = "";
	string inputPointsFilename = "os7sheet075.csv";
	int polynomialOrder = -1; //-1 is auto select
	string outputFilename = "map";
	int visualiseErrors = 0;
	int outputWidth = -1;
	int outputHeight = -1;
	int fitOnly = 0;
	int gbosOut = 0;
	int mercatorOut = 1;
	PolyProjectArgs::ProjType projType = PolyProjectArgs::OSGB;
	int forceAspectCoord = 0;
	string outproj = "mercator";
	string inproj = "gbos";
	vector<string> corners;

	ImgMagick::Init();

	/*double Lat = 54.9785 ;
	double Long = -3.1897;
	long OSGBNorthing;
	long OSGBEasting;
	char OSGBZone[4];

	cout << "Starting position(Lat, Long):  " << Lat << "   " << Long <<endl;

	LLtoOSGB(Lat, Long, OSGBEasting, OSGBNorthing, OSGBZone);
	cout << setiosflags(ios::showpoint | ios::fixed) << setprecision(6);
	cout << "Calculated OSGB position(Northing, Easting, GridSquare):  ";
	cout << OSGBZone << " " << OSGBEasting << " " << OSGBNorthing << endl;

	OSGBtoLL(OSGBNorthing, OSGBEasting, OSGBZone, Lat, Long);
	cout << "Calculated Lat, Long position(Lat, Long):  " << Lat << "   " << Long << endl <<endl;*/

	// Process program options
	std::stringstream desc;
	desc << "Allowed options:" << endl;
	desc << "  -i [ --in ] arg       input image filename" << endl;
	desc << "  -p [ --points ] arg   points to define transformation" << endl;
	desc << "  -o [ --out ] arg      output name (extension is added automatically)" << endl;
	desc << "  -v [ --vis ]          visualisation of error" << endl;
	desc << "  --fitonly             calc transform only (no rectify)" << endl;
	desc << "  --aspect              ensure output aspect ratio matches coordinate distances" << endl;
	desc << "  --inproj arg          input projection (mercator, gbos)" << endl;
	desc << "  --outproj arg         output projection (mercator, gbos)" << endl;
	desc << "  -f [ --fit ] arg      order of polynomial" << endl;
	desc << "  -w [ --width ] arg    output width" << endl;
	desc << "  -h [ --height ] arg   output height" << endl;
	desc << "  --help                help message" << endl;

	ProgramOptions po(argc, argv);
	po.AddAlias('i', "in");
	po.AddAlias('p', "points");
	po.AddAlias('o', "out");
	po.AddAlias('v', "vis");
	po.AddAlias('f', "fit");
	po.AddAlias('w', "width");
	po.AddAlias('h', "height");

	if (po.HasArg("help"))
	{
		cout << desc.str() << endl;
		exit(0);
	}
	if (po.HasArg("in"))
	{
		inputImageFilename = po.GetArg("in");
	}
	if (po.HasArg("points"))
		inputPointsFilename = po.GetArg("points");
	if (po.HasArg("out"))
		outputFilename = po.GetArg("out");
	if (po.HasArg("vis"))
		visualiseErrors = 1;
	if (po.HasArg("fit"))
		polynomialOrder = po.GetIntArg("fit");
	if (po.HasArg("width"))
		outputWidth = po.GetIntArg("width");
	if (po.HasArg("height"))
		outputHeight = po.GetIntArg("height");
	if (po.HasArg("aspect"))
		forceAspectCoord = 1;
	if (po.HasArg("fitonly"))
		fitOnly = 1;
	if (po.HasArg("corner"))
		corners = po.GetMultiArg("corner");
	if (po.HasArg("outproj"))
	{
		outproj = po.GetArg("outproj");
		if (outproj == "gbos")
		{
			gbosOut = 1;
			mercatorOut = 0;
		}
	}
	if (po.HasArg("inproj"))
	{
		inproj = po.GetArg("inproj");
		if (inproj == "mercator")
		{
			projType = PolyProjectArgs::Mercator;
		}
		if (inproj == "cassini")
		{
			projType = PolyProjectArgs::Cassini;
		}
		if (inproj == "bonnes")
		{
			projType = PolyProjectArgs::BonneS;
		}
		if (inproj == "bonnei")
		{
			projType = PolyProjectArgs::BonneI;
		}
		if( inproj == "bonnef" )
		{
			projType = PolyProjectArgs::BonneF;
		}
		if (inproj == "osi")
		{
			projType = PolyProjectArgs::OSI;
		}
		if( inproj == "wo")
		{
			projType = PolyProjectArgs::WO;
		}
		if( inproj == "woi")
		{
			projType = PolyProjectArgs::WOI;
		}
		if( inproj == "osgb")
		{
			projType = PolyProjectArgs::OSGB;
		}
		if( inproj == "osgby")
		{
			projType = PolyProjectArgs::OSGBY;
		}
	}

	if (inputImageFilename.length() == 0)
	{
		cout << desc.str() << endl;
		exit(0);
	}

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
		exit(-1);
	}
	if (outputWidth == -1)
		outputWidth = img.GetWidth();
	if (outputHeight == -1)
		outputHeight = img.GetHeight();
	tile.sx = outputWidth;
	tile.sy = outputHeight;

	class CalibrationFile pointDef;
	if (pointDef.Open(inputPointsFilename.c_str()) < 0)
	{
		cout << "File not found" << endl;
		cout << desc.str() << endl;
		exit(-1);
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
		string kmlOutFilename = outputFilename + ".kml";
		writeKml.href = mapOutFileNoPath;
		cout << "Writing KML to " << kmlOutFilename << endl;
		writeKml.WriteToFile(kmlOutFilename.c_str());
	}

	ImgMagick::Term();
}
