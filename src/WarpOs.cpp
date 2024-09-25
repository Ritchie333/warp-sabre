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
#include "Warp.h"

PolyProjectArgs::ProjType GetProjType( const string& input )
{
	const char* names[] = {
		"osgb",
		"mercator",
		"cassini",
		"bonnes",
		"bonnei",
		"bonnef",
		"osi",
		"wo",
		"woi",
		"osgby",
		NULL
	};

	for( int i = 0; ; i++ ) {
		if( !names[i] ) {
			// entry not found
			return PolyProjectArgs::OSGB;
		}
		if( input == names[i] ) {
			return ( PolyProjectArgs::ProjType) i;
		}
	}
}

//**************************************************

int main(int argc, char *argv[])
{
	class Warp warp;
	string outproj = "mercator";
	string inproj = "gbos";

	ImgMagick::Init();

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
	po.AddAlias('n', "name");

	if (po.HasArg("help"))
	{
		cout << desc.str() << endl;
		exit(0);
	}
	if (po.HasArg("in"))
	{
		warp.inputImageFilename = po.GetArg("in");
		warp.kmlName = warp.inputImageFilename;
	}
	if (po.HasArg("points"))
		warp.inputPointsFilename = po.GetArg("points");
	if (po.HasArg("out"))
		warp.outputFilename = po.GetArg("out");
	if (po.HasArg("vis"))
		warp.visualiseErrors = true;
	if (po.HasArg("fit"))
		warp.polynomialOrder = po.GetIntArg("fit");
	if (po.HasArg("width"))
		warp.outputWidth = po.GetIntArg("width");
	if (po.HasArg("height"))
		warp.outputHeight = po.GetIntArg("height");
	if (po.HasArg("aspect"))
		warp.forceAspectCoord = 1;
	if (po.HasArg("fitonly"))
		warp.fitOnly = true;
	if (po.HasArg("corner"))
		warp.corners = po.GetMultiArg("corner");
	if (po.HasArg("name"))
		warp.kmlName = po.GetArg("name");
	if (po.HasArg("outproj"))
	{
		outproj = po.GetArg("outproj");
		if (outproj == "gbos")
		{
			warp.gbosOut = true;
			warp.mercatorOut = false;
		}
	}
	if (po.HasArg("inproj"))
	{
		warp.projType = GetProjType ( po.GetArg("inproj") );
	}

	if (warp.inputImageFilename.length() == 0)
	{
		cout << desc.str() << endl;
		exit(0);
	}

	int result = warp.Run();

	ImgMagick::Term();

	return result;
}
