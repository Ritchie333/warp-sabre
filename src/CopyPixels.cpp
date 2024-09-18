#include "gbos1936/Gbos1936.h"
#include "ImgMagick.h"
#include <vector>
#include <iostream>
#include <string>
#include <stdio.h>
#include "CopyPixels.h"
#include "Tile.h"
#include "ganzc/LatLong-OSGBconversion.h"

using namespace std;

class HelmertConverter gConverter;

#define BEGIN_MASK_MAP(type)	string sType = type;
#define	MASK_ENTRY(name,class)	if( sType == name ) { mask = new class; }
#define END_MASK_MAP()

CopyPixels *CopyPixels::Create(const char *type)
{
	CopyPixels *mask = NULL;
	
	BEGIN_MASK_MAP(type)
	MASK_ENTRY("G", CopyPixelsWithOsMask )
	MASK_ENTRY("O", CopyPixelsWithRawMask )
	MASK_ENTRY("OS", CopyPixelsWithRawMask )
	MASK_ENTRY("OSY", CopyPixelsWithOSYMask )
	MASK_ENTRY("C", CopyPixelsWithCassini )
	MASK_ENTRY("CAS", CopyPixelsWithCassini )
	MASK_ENTRY("B", CopyPixelsWithBonne )
	MASK_ENTRY("SB", CopyPixelsWithBonne )
	MASK_ENTRY("I", CopyPixelsWithOsI )
	MASK_ENTRY("R", CopyPixelsWithIrishBonne )
	MASK_ENTRY("IB", CopyPixelsWithIrishBonne )
	MASK_ENTRY("FB", CopyPixelsWithFrenchBonne )
	MASK_ENTRY("PM", CopyPixelsWithParisMercator )
	MASK_ENTRY("M", CopyPixelsWithMercator )
	MASK_ENTRY("UTM", CopyPixelsWithUTM )
	MASK_ENTRY("WO", CopyPixelsWithWO )
	MASK_ENTRY("WOI", CopyPixelsWithWOI )
	END_MASK_MAP()

	if( !mask ) {
		cout << "Invalid projection type " << type << endl;
		throw 3001;
	}
	return mask;
}

CopyPixels::CopyPixels()
{
	gnorth = 0;
	gsouth = 0;
	geast = 0;
	gwest = 0;
	boxset = 0;
}

CopyPixels::~CopyPixels()
{
}

void CopyPixels::ThrowError(const char *what, const char *ref)
{
	cout << "Error : " << what << " " << ref << endl;
	throw 3000;
}

void CopyPixelsWithOsMask::UpdateBoundingBox(const char *mapref)
{
	int dEasting = 0, dNorthing = 0;
	if (2 == sscanf(mapref, "%d:%d", &dEasting, &dNorthing))
	{
		if (!boxset || gsouth > dNorthing)
			gsouth = dNorthing;
		if (!boxset || gnorth < dNorthing)
			gnorth = dNorthing;
		if (!boxset || geast < dEasting)
			geast = dEasting;
		if (!boxset || gwest > dEasting)
			gwest = dEasting;
		boxset = 1;
		// double lat=-1.0, lon=-1.0, alt = -1.0;
		// ConvertGbos1936ToWgs84(dEasting, dNorthing,0.0, lat, lon, alt);
	}
}

void CopyPixelsWithOSYMask::UpdateBoundingBox(const char *mapref)
{
	int dEasting = 0, dNorthing = 0;
	if (2 == sscanf(mapref, "%d:%d", &dEasting, &dNorthing))
	{
		dEasting *= METRES_IN_YARD;
		dNorthing *= METRES_IN_YARD;
		if (!boxset || gsouth > dNorthing)
			gsouth = dNorthing;
		if (!boxset || gnorth < dNorthing)
			gnorth = dNorthing;
		if (!boxset || geast < dEasting)
			geast = dEasting;
		if (!boxset || gwest > dEasting)
			gwest = dEasting;
		boxset = 1;
	}
}

long CopyPixelsWithUTM::UTMEasting( const int zone, const int easting )
{
	return ( zone * 1000000 ) + easting;
}

void CopyPixelsWithUTM::UpdateBoundingBox(const char *mapref)
{
	int dZone = 0, dEasting = 0, dNorthing = 0;
	if( 3 == sscanf(mapref, "%d:%d:%d", &dZone, &dEasting, &dNorthing ))
	{
		long fullEasting = UTMEasting( dZone, dEasting ); 
		long fullWest = UTMEasting( zwest, gwest ); 
		long fullEast = UTMEasting( zeast, geast ); 

		if (!boxset || gsouth > dNorthing) {
			gsouth = dNorthing;
		}
		if (!boxset || gnorth < dNorthing) {
			gnorth = dNorthing;
		}
		if (!boxset || fullEast < fullEasting) {
			geast = dEasting;
			zeast = dZone;
		}
		if (!boxset || fullWest > fullEasting) {
			gwest = dEasting;
			zwest = dZone;
		}

		boxset = 1;
	}
}

bool
CopyPixelsWithWO::CheckIfInBox(double lat, double lon)
{
	double pnorth, peast;
	gConverter.ConvertWgs84ToWO(lat, lon, 0.0, peast, pnorth);
	if (pnorth < gsouth)
		return false;
	if (pnorth > gnorth)
		return false;
	if (peast < gwest)
		return false;
	if (peast > geast)
		return false;
	return true;
}

bool
CopyPixelsWithWOI::CheckIfInBox(double lat, double lon)
{
	double pnorth, peast;
	gConverter.ConvertWgs84ToWOI(lat, lon, 0.0, peast, pnorth);
	if (pnorth < gsouth)
		return false;
	if (pnorth > gnorth)
		return false;
	if (peast < gwest)
		return false;
	if (peast > geast)
		return false;
	return true;
}

void CopyPixelsWithRawMask::UpdateBoundingBox(const char *mapref)
{
	int dEasting = 0, dNorthing = 0;
	if (2 == sscanf(mapref, "%d:%d", &dEasting, &dNorthing))
	{
		if (!boxset || gsouth > dNorthing)
			gsouth = dNorthing;
		if (!boxset || gnorth < dNorthing)
			gnorth = dNorthing;
		if (!boxset || geast < dEasting)
			geast = dEasting;
		if (!boxset || gwest > dEasting)
			gwest = dEasting;
		boxset = 1;
	}
	else
	{
		ThrowError("Invalid raw OS map reference", mapref);
	}
}

void CopyPixelsWithCassini::UpdateBoundingBox(const char *mapref)
{
	double dEasting = 0, dNorthing = 0;
	if (2 == sscanf(mapref, "%lf:%lf", &dEasting, &dNorthing))
	{
		if (!boxset || gsouth > dNorthing)
			gsouth = dNorthing;
		if (!boxset || gnorth < dNorthing)
			gnorth = dNorthing;
		if (!boxset || geast < dEasting)
			geast = dEasting;
		if (!boxset || gwest > dEasting)
			gwest = dEasting;
		boxset = 1;
	}
	else
	{
		ThrowError("Invalid Cassini map reference", mapref);
	}
}

void CopyPixelsWithBonne::UpdateBoundingBox(const char *mapref)
{
	double dEasting = 0, dNorthing = 0;
	if (2 == sscanf(mapref, "%lf:%lf", &dEasting, &dNorthing))
	{
		if (!boxset || gsouth > dNorthing)
			gsouth = dNorthing;
		if (!boxset || gnorth < dNorthing)
			gnorth = dNorthing;
		if (!boxset || geast < dEasting)
			geast = dEasting;
		if (!boxset || gwest > dEasting)
			gwest = dEasting;
		boxset = 1;
	}
	else
	{
		ThrowError("Invalid Bonne map reference", mapref);
	}
}

void CopyPixelsWithMercator::UpdateBoundingBox(const char *mapref)
{
	double dLng = 0, dLat = 0;
	if (2 == sscanf(mapref, "%lf:%lf", &dLat, &dLng))
	{
		if (!boxset || gsouth > dLat)
			gsouth = dLat;
		if (!boxset || gnorth < dLat)
			gnorth = dLat;
		if (!boxset || geast < dLng)
			geast = dLng;
		if (!boxset || gwest > dLng)
			gwest = dLng;
		gVertx.clear();
		gVerty.clear();

		gVertx.push_back(gwest);
		gVertx.push_back(geast);
		gVertx.push_back(geast);
		gVertx.push_back(gwest);

		gVerty.push_back(gnorth);
		gVerty.push_back(gnorth);
		gVerty.push_back(gsouth);
		gVerty.push_back(gsouth);

		boxset = 1;
	}
	else
	{
		ThrowError("Invalid mercator reference", mapref);
	}
}

void CopyPixelsWithParisMercator::UpdateBoundingBox(const char *mapref)
{
	double dgLng = 0, dgLat = 0, dLng = 0, dLat = 0;
	if (2 == sscanf(mapref, "%lf:%lf", &dgLat, &dgLng))
	{
		gConverter.ConvertParisToWgs84( dgLat / 0.9, ( dgLng / 0.9 ) - PARIS_CENTRE_LON, dLat, dLng );

		if (!boxset || gsouth > dLat)
			gsouth = dLat;
		if (!boxset || gnorth < dLat)
			gnorth = dLat;
		if (!boxset || geast < dLng)
			geast = dLng;
		if (!boxset || gwest > dLng)
			gwest = dLng;
		gVertx.clear();
		gVerty.clear();

		gVertx.push_back(gwest);
		gVertx.push_back(geast);
		gVertx.push_back(geast);
		gVertx.push_back(gwest);

		gVerty.push_back(gnorth);
		gVerty.push_back(gnorth);
		gVerty.push_back(gsouth);
		gVerty.push_back(gsouth);

		boxset = 1;
	}
	else
	{
		ThrowError("Invalid mercator reference", mapref);
	}
}

bool CopyPixelsWithOsMask::CheckIfInBox(double lat, double lon)
{
	double pnorth, peast, palt;
	gConverter.ConvertWgs84ToGbos1936(lat, lon, 0.0, peast, pnorth, palt);
	if (pnorth < gsouth)
		return false;
	if (pnorth > gnorth)
		return false;
	if (peast < gwest)
		return false;
	if (peast > geast)
		return false;

	return true;
}

bool CopyPixelsWithUTM::CheckIfInBox(double lat, double lon)
{
	double pnorth, peast;
	int pzone;
	gConverter.ConvertWgs84ToUTM50( lat, lon, peast, pnorth, pzone );

	long fullEasting = UTMEasting( pzone, peast );
	long fullWest = UTMEasting( zwest, gwest );
	long fullEast = UTMEasting( zeast, geast );

	if (pnorth < gsouth)
		return false;
	if (pnorth > gnorth)
		return false;
	if (fullEasting < fullWest )
		return false;
	if (fullEasting > fullEast )
		return false;
	return true;
}

bool CopyPixelsWithOsI::CheckIfInBox(double lat, double lon)
{
	double pnorth, peast, palt;
	gConverter.ConvertWgs84ToOsi(lat, lon, 0.0, peast, pnorth, palt);
	if (pnorth < gsouth)
		return false;
	if (pnorth > gnorth)
		return false;
	if (peast < gwest)
		return false;
	if (peast > geast)
		return false;

	return true;
}

bool CopyPixelsWithCassini::CheckIfInBox(double lat, double lon)
{
	double pnorth, peast;
	gConverter.ConvertWgs84ToCas(lat, lon, 0.0, peast, pnorth);
	if (pnorth < gsouth)
		return false;
	if (pnorth > gnorth)
		return false;
	if (peast < gwest)
		return false;
	if (peast > geast)
		return false;
	return true;
}

bool CopyPixelsWithBonne::CheckIfInBox(double lat, double lon)
{
	double pnorth, peast;
	gConverter.ConvertWgs84ToBnS(lat, lon, 0.0, peast, pnorth);
	if (pnorth < gsouth)
		return false;
	if (pnorth > gnorth)
		return false;
	if (peast < gwest)
		return false;
	if (peast > geast)
		return false;
	return true;
}

bool CopyPixelsWithIrishBonne::CheckIfInBox(double lat, double lon)
{
	double pnorth, peast;
	gConverter.ConvertWgs84ToBnI(lat, lon, 0.0, peast, pnorth);
	if (pnorth < gsouth)
		return false;
	if (pnorth > gnorth)
		return false;
	if (peast < gwest)
		return false;
	if (peast > geast)
		return false;
	return true;
}

bool CopyPixelsWithFrenchBonne::CheckIfInBox(double lat, double lon)
{
	double pnorth, peast;
	gConverter.ConvertWgs84ToBnF(lat, lon, 0.0, peast, pnorth);
	if (pnorth < gsouth)
		return false;
	if (pnorth > gnorth)
		return false;
	if (peast < gwest)
		return false;
	if (peast > geast)
		return false;
	return true;
}

bool CopyPixelsWithMercator::CheckIfInBox(double lat, double lon)
{
	int c = 0, i = 0, j = 0;
	for (i = 0, j = 3; i < 4; j = i++)
	{
		if (((gVerty[i] > lat) != (gVerty[j] > lat)) &&
			(lon < (gVertx[j] - gVertx[i]) * (lat - gVerty[i]) / (gVerty[j] - gVerty[i]) + gVertx[i]))
		{
			c = !c;
		}
	}
	return c;
}

void CopyPixels::FastCopy(const class ImgMagick &imageIn, class ImgMagick &imageOut, class Tile &tile, const int tileSize)
{
	int width = imageIn.GetWidth();
	int height = imageIn.GetHeight();
	int chans = imageIn.GetNumChannels();

	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			for (int k = 0; k < chans; k++)
			{
				if (i < tileSize ||
					j < tileSize ||
					i > (width - tileSize) ||
					j > (height - tileSize))
				{
					CopyTile(imageIn, imageOut, tile, i, j, k, false /* no merge */);
				}
			}
		}
	}
}

bool CopyPixels::CopyTile(const class ImgMagick &imageIn, class ImgMagick &imageOut, class Tile &tile, const int i, const int j, const int k, const bool merge)
{
	double lat, lon;
	tile.UnProject(i, j, lat, lon);
	double val = imageIn.GetPix(i, j, k);
	bool inMask = this->CheckIfInBox(lat, lon);
	if (inMask)
	{
		if (!merge || (val > 0))
		{
			imageOut.SetPix(i, j, k, val);
			return true;
		}
	}
	return false;
}

void CopyPixels::Copy(const class ImgMagick &imageIn, class ImgMagick &imageOut, class Tile &tile, const bool merge)
{
	int width = imageIn.GetWidth();
	int height = imageIn.GetHeight();
	int chans = imageIn.GetNumChannels();

	// Check corners (quicker to check then exhaustively check each pixel)
	double lat, lon;
	tile.UnProject(0, 0, lat, lon);
	bool cornerCheck = this->CheckIfInBox(lat, lon);
	tile.UnProject(width, 0, lat, lon);
	cornerCheck = cornerCheck && this->CheckIfInBox(lat, lon);
	tile.UnProject(width, height, lat, lon);
	cornerCheck = cornerCheck && this->CheckIfInBox(lat, lon);
	tile.UnProject(0, height, lat, lon);
	cornerCheck = cornerCheck && this->CheckIfInBox(lat, lon);
	// cout << cornerCheck << endl;
	if (cornerCheck || boxset == 0) // Entire tile is in mask
	{

		// Copy every pixel
		for (int i = 0; i < width; i++)
			for (int j = 0; j < height; j++)
				for (int k = 0; k < chans; k++)
				{
					double val = imageIn.GetPix(i, j, k);
					if (!merge || val)
					{
						imageOut.SetPix(i, j, k, val);
					}
				}
	}
	else
	{
		// Check each pixel and copy if inside mask
		for (int i = 0; i < width; i++)
			for (int j = 0; j < height; j++)
				for (int k = 0; k < chans; k++)
				{
					CopyTile(imageIn, imageOut, tile, i, j, k, merge);
				}
	}
}
