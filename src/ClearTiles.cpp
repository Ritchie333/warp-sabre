
#include "SlippyMapTile.h"
#include "gbos1936/Gbos1936.h"
#include "StringUtils.h"
#include "Tile.h"
#include "TileJob.h"
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

//***************************************

int main(int argc, char **argv)
{
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

	// Process program options
	std::stringstream desc;
	desc << "Allowed options" << endl;
	desc << "  --minzoom arg         Minimum zoom level" << endl;
	desc << "  --maxzoom arg         Maximum zoom level" << endl;
	desc << "  --bounds arg          Bounds filename" << endl;
	desc << "  --output arg          Output folder" << endl;
	desc << "  --positional arg   	 Input KML files" << endl;
	desc << "  --help                help message" << endl;

	ProgramOptions po(argc, argv);

	string boundsFilename = "bounds.csv";
	string outFolder = "out";
	int minZoom = 1;
	int maxZoom = 17;

	DEFINE_VERSION( po )
	DEFINE_HELP( po, desc )

	if (po.HasArg("output"))
		outFolder = po.GetArg("output");
	if (po.HasArg("out"))
		outFolder = po.GetArg("out");
	if (po.HasArg("bounds"))
		boundsFilename = po.GetArg("bounds");
	if (po.HasArg("minzoom"))
		minZoom = po.GetIntArg("minzoom");
	if (po.HasArg("maxzoom"))
		maxZoom = po.GetIntArg("maxzoom");

	vector<string> inputFiles;
	if (po.HasArg(NULL))
	{
		inputFiles = po.GetMultiArg(NULL);
		for (unsigned int i = 0; i < inputFiles.size(); i++)
			cout << "input file " << inputFiles[i] << endl;
	}

	class TileRunner tileRun( 1 );
	tileRun.inputFiles = inputFiles;
	tileRun.boundsFilename = boundsFilename;
	tileRun.minZoom = minZoom;
	tileRun.maxZoom = maxZoom;
	tileRun.outFolder = outFolder;

	tileRun.Init();
	tileRun.Clear();
}
