
#ifndef GBOS1936_H
#define GBOS1936_H

#include <string.h>
#include <math.h>
// Helmert approximation of GBOS1936 to WGS84
// Accurate to 6 metres approx
// Based on that VB Excel OS ProjectionandTransformationCalculations.xls
// Probably based on OS Crown Copyright
// Ported to C++ by Tim Sheerman-Chase 2009

// Other conversions by Ed Fielden
// Ported to C++ By Ritchie Swann

#define PARIS_CENTRE_LAT 48.836439
#define PARIS_CENTRE_LON 2.336506

const double METRES_IN_MILE = 1609.347955248;
const double METRES_IN_YARD = 0.9144022473;

struct Ellip
{
	double a;
	double b;
	double e2;
	double se;

	Ellip( double _a, double _b, double _e2, double _se )
	{
		a = _a;
		b = _b;
		e2 = _e2;
		se = _se;
	}

	Ellip( double _a, double _b )
	{
		a = _a;
		b = _b;
		e2 = (1-(pow(b/a,2)));
		se = (pow((a/b)*(a/b)-1,0.5));
	}

	bool operator==( const struct Ellip& other ) const {
		return ( a == other.a && b == other.b && e2 == other.e2 && se == other.se );
	}
};

struct Datum
{
	Ellip ellipsoid;	// S
	double dX;
	double dY;
	double dZ;
	double rX;
	double rY;
	double rZ;
	double sf;

	bool operator==( const struct Datum& other ) const {
		return ( ellipsoid == other.ellipsoid &&
			dX == other.dX &&
			dY == other.dY &&
			dZ == other.dZ &&
			rX == other.rX &&
			rY == other.rY &&
			rZ == other.rZ &&
			sf == other.sf );
	}
};

struct GridData
{
	Ellip ellip;
	double F0;
	double Lat0;
	double Lon0;
	double Unit;
	double FE;
	double FN;

	GridData() : ellip( 0, 0 ) {}
};

double sech( double x );
double arsinh( double x );
double artanh( double x );
double sec2rad( double x );

double Helmert_X(double X, double Y, double Z, double DX, double Y_Rot, double Z_Rot, double s);
double Helmert_Y(double X, double Y, double Z, double DY, double X_Rot, double Z_Rot, double s);
double Helmert_Z(double X, double Y, double Z, double DZ, double X_Rot, double Y_Rot, double s);
double XYZ_to_Lat(double X, double Y, double Z, double a, double b);
double XYZ_to_Long(double X, double Y);
double XYZ_to_H(double X, double Y, double Z, double a, double b);
double Lat_Long_H_to_X(double PHI, double LAM, double H, double a, double b);
double Lat_Long_H_to_Y(double PHI, double LAM, double H, double a, double b);
double Lat_H_to_Z(double PHI, double LAM, double H, double a, double b);
double E_N_to_Lat(double East, double North, double a, double b, double e0, double n0, double f0, double PHI0, double LAM0);
double E_N_to_Long(double East, double North, double a, double b, double e0, double n0, double f0, double PHI0, double LAM0);
double calc_M(long double latdiff, long double latsum, long double nn, long double eb, double F0);

void TM2Geo( double East, double North, const GridData& TMgrid, double& latOut, double& lngOut );
void Geo2TM( double lat, double lon, const GridData& TMgrid, double &eaOut, double &noOut );
void Geo2Geo( double lat, double lon, const struct Datum& datum_a, const struct Datum& datum_b, double& latOut, double &lngOut );

void ConvertGbos1936ToWgs84(double ea, double no, double he,
							double &latOut, double &lonOut, double &heOut);
void ConvertOsiToWgs84(double ea, double no, double he,
					   double &latOut, double &lonOut, double &heOut);
void ConvertCasToWgs84(double ea, double no, double he,
					   double &latOut, double &lonOut, double &heOut,
					   const GridData &CSGrid, const Datum& datum);
void ConvertBnToMercator(const struct Ellip& ellip, const double sf, double orglat, double orglon, double ea, double no, double he,
					  double &latOut, double &lonOut, double &heOut);
void ConvertWgs84ToGbos1936(double lat, double lon, double he,
							double &eaOut, double &noOut, double &heOut);
void ConvertWgs84ToOsi(double lat, double lon, double he,
					   double &eaOut, double &noOut, double &heOut);
void ConvertWgs84ToCas(double lat, double lon, double he, double &eaoOut, double &noOut,
	const GridData &CSGrid, const Datum& datum);
void ConvertMercatorToBn(const struct Ellip& ellip, const double sf, const double orglat, const double orglon,
	double lat, double lon, double he, double &eaoOut, double &noOut);
void ConvertWgs84ToGbos1936LatLng(double lat, double lon, double he,
								  double &latOut, double &lngOut);
void ConvertGbos1936LatLngToWgs84(double gboslat, double gboslon, double he,
								  double &latOut, double &lngOut, double &heOut);
void ConvertOsi65ToWgs84(double osilat, double osilon, double he, double &latOut, double &lngOut, double &heOut);
void ConvertWgs84ToOsi65(double lat, double lon, double he, double &latOut, double &lonOut, double &heOut);
void GetOsiShift(double lat, double lon, double &latOut, double &lonOut);
void ConvertParisToWgs84(double glat, double glon, double &latOut, double &lonOut );
void ConvertWgs84ToParis(double lat, double lon, double &glatOut, double &glonOut );
void GetParisOrigin( double& latOut, double& lonOut );

int TestGbos1936();

class HelmertConverter
{
public:
	void ConvertGbos1936ToWgs84(double ea, double no, double he,
								double &latOut, double &lonOut, double &heOut);
	void ConvertOsiToWgs84(double ea, double no, double he,
						   double &latOut, double &lonOut, double &heOut);
	void ConvertCasToWgs84(double ea, double no, double he,
						   double &latOut, double &lonOut, double &heOut);
	void ConvertWOToWgs84(double ea, double no, double he,
							double &latOut, double &lonOut, double &heOut);
	void ConvertWOIToWgs84(double ea, double no, double he,
							double &latOut, double &lonOut, double &heOut);
	void ConvertBnSToWgs84(double ea, double no, double he,
						   double &latOut, double &lonOut, double &heOut);
	void ConvertBnIToWgs84(double ea, double no, double he,
						   double &latOut, double &lonOut, double &heOut);
	void ConvertBnFToWgs84(double ea, double no, double he,
							double &latOut, double &lonOut, double &heOut );
	void ConvertWgs84ToGbos1936(double lat, double lon, double he,
								double &eaOut, double &noOut, double &heOut);
	void ConvertWgs84ToOsi(double lat, double lon, double he,
						   double &eaOut, double &noOut, double &heOut);
	void ConvertWgs84ToCas(double lat, double lon, double he, double &eaOut, double &noOut);
	void ConvertWgs84ToWO(double lat, double lon, double he, double &eaOut, double &noOut);
	void ConvertWgs84ToWOI(double lat, double lon, double he, double &eaOut, double &noOut);
	void ConvertWgs84ToBnS(double lat, double lon, double he, double &eaOut, double &noOut);
	void ConvertWgs84ToBnI(double lat, double lon, double he, double &eaOut, double &noOut);
	void ConvertWgs84ToBnF(double lat, double lon, double he, double &eaOut, double &noOut);
	void ConvertUTM50ToWgs84(const int zone, double ea, double no, double &latOut, double &lonOut);
	void ConvertWgs84ToUTM50(double lat, double lon, double &eaOut, double &noOut, int &zOut);
	void ConvertParisToWgs84(double glat, double glon, double &latOut, double &lonOut );
	void ConvertWgs84ToParis(double lat, double lon, double &glatOut, double &glonOut );
	void GetTransformName(char *buffer, int maxLen)
	{
		strncpy(buffer, "Helmert Gbos1936", maxLen);
	}
};

#endif // GBOS1936_H
