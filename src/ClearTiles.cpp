
#include "SlippyMapTile.h"
#include "gbos1936/Gbos1936.h"
#include "StringUtils.h"
#include "Tile.h"
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

	class Tile dst;
	// cout << long2tile(-3.68, zoom) << "," << lat2tile(54.8333,zoom) << endl;
	// cout << long2tile(-3.04, zoom) << "," << lat2tile(55.2446,zoom) << endl;

	// Process program options
	std::stringstream desc;
	desc << "Allowed options" << endl;
	desc << "  --bounds arg          Bounds filename" << endl;
	desc << "  --output arg          Output folder" << endl;
	desc << "  --positional arg   	 Input KML files" << endl;
	desc << "  --help                help message" << endl;

	ProgramOptions po(argc, argv);

	string boundsFilename = "bounds.csv";
	string outFolder = "out";
	if (po.HasArg("help"))
	{
		cout << desc.str() << endl;
		exit(0);
	}
	if (po.HasArg("output"))
		outFolder = po.GetArg("output");
	if (po.HasArg("out"))
		outFolder = po.GetArg("out");
	if (po.HasArg("bounds"))
		boundsFilename = po.GetArg("bounds");

	vector<string> inputFiles;
	if (po.HasArg(NULL))
	{
		inputFiles = po.GetMultiArg(NULL);
		for (unsigned int i = 0; i < inputFiles.size(); i++)
			cout << "input file " << inputFiles[i] << endl;
	}

	class DelimitedFile boundsFile;
	int boundsOpen = boundsFile.Open(boundsFilename.c_str());
	if (boundsOpen < 1)
		cout << "Could not read bounds file " << boundsFilename << endl;

	class Tile sourceBBox;
	int sourceBBoxSet = 0;

	SourceKml* src = new SourceKml[ inputFiles.size() ];

	// For each input file, parse KML into local mem
	for (int i = 0; i < static_cast<int>(inputFiles.size()); i++)
	{
		cout << "Source file '" << inputFiles[i] << "'" << endl;
		string filePath = GetFilePath(inputFiles[i].c_str());

		class SourceKml &last = src[i];
		string imgFilename;
		int ret = ReadKmlFile(inputFiles[i].c_str(), last.tile, imgFilename);
		last.imgFilename = filePath;
		last.imgFilename += "/";
		last.imgFilename += imgFilename;
		if (ret < 1)
		{
			cout << "Kml " << inputFiles[i] << " not found";
			exit(0);
		}
		cout << last.tile.latmin << "," << last.tile.lonmin << "," << last.tile.latmax << "," << last.tile.lonmax << endl;
		cout << "image filename '" << last.imgFilename << "'" << endl;

		// Update source bounding box
		if (sourceBBox.latmin > last.tile.latmin || !sourceBBoxSet)
			sourceBBox.latmin = last.tile.latmin;
		if (sourceBBox.latmax < last.tile.latmax || !sourceBBoxSet)
			sourceBBox.latmax = last.tile.latmax;
		if (sourceBBox.lonmin > last.tile.lonmin || !sourceBBoxSet)
			sourceBBox.lonmin = last.tile.lonmin;
		if (sourceBBox.lonmax < last.tile.lonmax || !sourceBBoxSet)
			sourceBBox.lonmax = last.tile.lonmax;
		sourceBBoxSet = 1;

		// int ret = last.image.Open(last.imgFilename.c_str());
		// if(ret < 0){cout << "Filed to open image" << endl;exit(0);}
		// last.bounds.push_back("NR550000");
		// last.bounds.push_back("NR950450");
		string filenameNoPath = RemoveFilePath(filePath.c_str());
		vector<string> boundsTemp;
		string projType;
		GetBounds(boundsFile, filenameNoPath.c_str(), boundsTemp, projType);
		cout << "bounds (" << boundsTemp.size() << ")";
		for (unsigned int j = 0; j < boundsTemp.size(); j++)
			cout << projType << " : " << boundsTemp[j] << ",";
		cout << endl;
		last.bounds = boundsTemp;
	}

	cout << "Input files bounding box:" << endl;
	cout << sourceBBox.latmin << "," << sourceBBox.lonmin << "," << sourceBBox.latmax << "," << sourceBBox.lonmax << endl;

	// int zoom = 14;
	for (unsigned int zoom = 17; zoom >= 2; zoom--)
	{
		int srcWtile = long2tile(/*-3.68423*/ sourceBBox.lonmin, zoom);
		int srcEtile = long2tile(sourceBBox.lonmax, zoom);
		int srcStile = lat2tile(sourceBBox.latmin, zoom);
		int srcNtile = lat2tile(sourceBBox.latmax, zoom);
		// cout << "tiles covered " << srcStile << "," << srcWtile << "," << srcNtile << "," << srcEtile << endl;

		// int tileLon = 8024;
		// int tileLat = 5168;
		// For each tile to generate
		for (int tileLon = srcWtile; tileLon <= srcEtile; tileLon++)
			for (int tileLat = srcNtile; tileLat <= srcStile; tileLat++)
			{
				string outFilename = outFolder;
				string outFolder0 = outFilename;
				outFilename += "/";
				outFilename += IntToString(zoom);
				string outFolder1 = outFilename;
				outFilename += "/";
				outFilename += IntToString(tileLon);
				string outFolder2 = outFilename;
				outFilename += "/";
				outFilename += IntToString(tileLat);
				outFilename += ".jpg";

				if (fileExists(outFilename.c_str()))
				{
					cout << "Deleting: " << outFilename << endl;
					remove(outFilename.c_str());
				}

			} // End of tile loop
	} // End of zoom loop

	delete[] src;

}
