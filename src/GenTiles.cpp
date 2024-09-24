#include "SlippyMapTile.h"
#include "gbos1936/Gbos1936.h"
#include "StringUtils.h"
#include "Tile.h"
#include "TileJob.h"
#include "ImgMagick.h"
#include "ImageWarpByFunc.h"
#include "ganzc/LatLong-OSGBconversion.h"
#include "ReadKmlFile.h"
#include "OSTN02Perl.h"
#include "ReadDelimitedFile.h"
#include "GetBounds.h"
#include "SourceKml.h"
#include "ProgramOptions.h"

// #include <wand/magick-wand.h>
// #include <wand/drawing-wand.h>
// #include <Magick++.h>
// rsync --size-only -v -r /var/www/os7 timsc@dev.openstreetmap.org:/home/ooc
// rsync --size-only -v -r /var/www/openlayers/os7 timsc@dev.openstreetmap.org:/home/timsc/public_html/openlayers

#include <list>
#include <iostream>
#include <exception>
#include <vector>
#include <map>
#include <memory>
#include <sstream>
using namespace std;
// using namespace Magick;

//***************************************

int main(int argc, char **argv)
{
	ImgMagick::Init();

	// Image imageInOut("step.jpg");
	// imageInOut.crop( Geometry(255,255,0,0) );
	// imageInOut.write("test2.png");

	/*class Tile test;
	vector<string> test2;
	string test3;
	int retOpen = ReadKmlFile("/home/tim/dev/os7files/rect/73.kml", test, test3, test2);

	exit(0);*/

	// cout << long2tile(-3.68, zoom) << "," << lat2tile(54.8333,zoom) << endl;
	// cout << long2tile(-3.04, zoom) << "," << lat2tile(55.2446,zoom) << endl;

	class DelimitedFile boundsFile;

	string outFolder = "out";
	unsigned int minZoom = 10;
	unsigned int maxZoom = 14;
	int maxTilesLoaded = DEFAULT_MAX_TILES;
	int targetNumThreads = TileJob::TargetThreads();
	bool mergeTiles = false;
	string boundsFilename = "bounds.csv";
	vector<string> inputFiles;
	string execFilename = "gentiles";
	if (argc >= 1)
		execFilename = argv[0];

	// Process program options

	std::stringstream desc;
	desc << "Allowed options" << endl;
	desc << "  --minzoom arg         Minimum zoom level" << endl;
	desc << "  --maxzoom arg         Maximum zoom level" << endl;
	desc << "  --output arg          Output folder" << endl;
	desc << "  --bounds arg          Bounds filename" << endl;
	desc << "  --maxloaded arg       Maximum number of tiles in memory" << endl;
	desc << "  --threads arg         Maximum number of threads" << endl;
	desc << "  --merge arg           Merge tiles rather than overwrite" << endl;
	desc << "  --positional arg 	 Input KML files" << endl;
	desc << "  --help                help message" << endl;

	ProgramOptions po(argc, argv);

	if (po.HasArg("help"))
	{
		cout << desc.str() << endl;
		exit(0);
	}
	if (po.HasArg("minzoom"))
		minZoom = po.GetIntArg("minzoom");
	if (po.HasArg("maxzoom"))
		maxZoom = po.GetIntArg("maxzoom");
	if (po.HasArg("maxloaded"))
		maxTilesLoaded = po.GetIntArg("maxloaded");
	if (po.HasArg("threads"))
		targetNumThreads = po.GetIntArg("threads");
	if (po.HasArg("merge"))
		mergeTiles = (po.GetIntArg("merge") == 1);
	if (po.HasArg("output"))
		outFolder = po.GetArg("output");
	if( po.HasArg("out"))
		outFolder = po.GetArg("out");
	if (po.HasArg("bounds"))
		boundsFilename = po.GetArg("bounds");

	if (po.HasArg(NULL))
	{
		inputFiles = po.GetMultiArg(NULL);
		for (unsigned int i = 0; i < inputFiles.size(); i++)
			cout << "input file " << inputFiles[i] << endl;
	}

	if (inputFiles.size() == 0)
	{
		cout << "No input KML files were specified" << endl;
		cout << "Usage: " << execFilename << " [options]" << endl;
		cout << desc.str() << endl;
		exit(0);
	}

	if (minZoom > maxZoom)
	{
		cout << "Error max zoom is lower than min zoom" << endl;
		cout << desc.str() << endl;
		exit(0);
	}

	class TileRunner tileRun( maxTilesLoaded );
	tileRun.inputFiles = inputFiles;
	tileRun.boundsFilename = boundsFilename;
	tileRun.minZoom = minZoom;
	tileRun.maxZoom = maxZoom;
	tileRun.outFolder = outFolder;
	tileRun.targetNumThreads = targetNumThreads;
	tileRun.mergeTiles = mergeTiles;

	tileRun.Init();
	tileRun.SetupTileJobs();
	tileRun.RunTileJobs();
	cout << "Number of tiles failed: " << tileRun.CountFailures() << endl;

	ImgMagick::Term();
}
