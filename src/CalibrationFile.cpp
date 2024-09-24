#include "gbos1936/Gbos1936.h"
#include "ganzc/LatLong-OSGBconversion.h" 
#include "CalibrationFile.h"

CalibrationFile::CalibrationFile()
{
}

CalibrationFile::CalibrationFile(const class CalibrationFile &a)
{
    operator=(a);
}

class CalibrationFile& CalibrationFile::operator=(const class CalibrationFile &a)
{
    lines = a.lines;
	delimiter = a.delimiter;
	return *this;
}

CalibrationFile::~CalibrationFile()
{

}

void CalibrationFile::ReadProjection()
{
	class HelmertConverter converter;

	for (unsigned int i = 0; i < lines.size(); i++)
	{
		class DelimitedFileLine &line = lines[i];
		// cout << line.NumVals() << endl;

		if (line.NumVals() == 5)
		{
			if (strcmp(line[0].GetVals(), "p") == 0)
			{
				double imgX = line[3].GetVald();
				double imgY = line[4].GetVald();
				double ix = line[2].GetVald();
				double iy = line[1].GetVald();

				if (iy < south || !setBox)
					south = iy;
				if (iy > north || !setBox)
					north = iy;
				if (ix < west || !setBox)
					west = ix;
				if (ix > east || !setBox)
					east = ix;

				if (projType != PolyProjectArgs::Mercator)
				{
					cout << "Cannot take lat lon input when using OS as input projection" << endl;
					exit(0);
				}
				else
					proj.AddPoint(imgX, imgY, ix, iy);

				setBox = 1;
			}
			if (strcmp(line[0].GetVals(), "g") == 0)	// French maps are in grads
			{
				double imgX = line[3].GetVald();
				double imgY = line[4].GetVald();
				double ix = ( line[2].GetVald() * 0.9 ) + PARIS_CENTRE_LON;
				double iy = line[1].GetVald() * 0.9;
				double lat = 0.0, lon = 0.0;
				converter.ConvertParisToWgs84( iy, ix, lat, lon );

				if (lat < south || !setBox)
					south = lat;
				if (lat > north || !setBox)
					north = lat;
				if (lon < west || !setBox)
					west = lon;
				if (lon > east || !setBox)
					east = lon;
				setBox = 1;

				if (projType != PolyProjectArgs::Mercator)
				{
					cout << "Cannot take lat lon input when using OS as input projection" << endl;
					exit(0);
				}
				else
					proj.AddPoint(imgX, imgY, ix, iy);

				setBox = 1;
			}
		}
		if (line.NumVals() == 4) // Read in GB OS national grid data
		{
			int dEasting = 0, dNorthing = 0;
			double lat = -1.0, lon = -1.0, alt = -1.0;
			double imgX = line[2].GetVald();
			double imgY = line[3].GetVald();
			if (strcmp(line[0].GetVals(), "os") == 0)
			{
				string mapref = line[1].GetVals();
				sscanf(mapref.c_str(), "%d:%d", &dEasting, &dNorthing);
				// Add point to transform constraints
				converter.ConvertGbos1936ToWgs84(dEasting, dNorthing, 0.0, lat, lon, alt);
			}
			if (strcmp(line[0].GetVals(), "osy") == 0)
			{
				string mapref = line[1].GetVals();
				sscanf(mapref.c_str(), "%d:%d", &dEasting, &dNorthing);
				int me = 0, mn = 0;
				YardsToMetres( dEasting, dNorthing, me, mn );
				// Add point to transform constraints
				converter.ConvertGbos1936ToWgs84(me, mn, 0.0, lat, lon, alt);
			}
			if (strcmp(line[0].GetVals(), "osi") == 0)
			{
				string mapref = line[1].GetVals();
				sscanf(mapref.c_str(), "%d:%d", &dEasting, &dNorthing);
				converter.ConvertOsiToWgs84(dEasting, dNorthing, 0.0, lat, lon, alt);
			}
			if (strcmp(line[0].GetVals(), "wo") == 0)
			{
				string mapref = line[1].GetVals();
				sscanf(mapref.c_str(), "%d:%d", &dEasting, &dNorthing);
				converter.ConvertWOToWgs84(dEasting, dNorthing, 0.0, lat, lon, alt);
			}
			if (strcmp(line[0].GetVals(), "woi") == 0)
			{
				string mapref = line[1].GetVals();
				sscanf(mapref.c_str(), "%d:%d", &dEasting, &dNorthing);
				converter.ConvertWOIToWgs84(dEasting, dNorthing, 0.0, lat, lon, alt);
			}
			if (lat != -1.0 && lon != -1.0)
			{
				if (projType != PolyProjectArgs::Mercator)
					proj.AddPoint(imgX, imgY, dEasting, dNorthing);
				else
					proj.AddPoint(imgX, imgY, lon, lat);

				if (mercatorOut)
				{
					if (lat < south || !setBox)
						south = lat;
					if (lat > north || !setBox)
						north = lat;
					if (lon < west || !setBox)
						west = lon;
					if (lon > east || !setBox)
						east = lon;
					setBox = 1;
				}
				if (gbosOut)
				{
					if (dNorthing < south || !setBox)
						south = dNorthing;
					if (dNorthing > north || !setBox)
						north = dNorthing;
					if (dEasting < west || !setBox)
						west = dEasting;
					if (dEasting > east || !setBox)
						east = dEasting;
					setBox = 1;
				}
			}
		}

		if (line.NumVals() == 5) // Read in GB OS national grid data
		{
			double dEasting = 0, dNorthing = 0;
			double imgX = line[3].GetVald();
			double imgY = line[4].GetVald();
			double lat = -1.0, lon = -1.0, alt = -1.0;
			if (strcmp(line[0].GetVals(), "gbos1936") == 0)
			{
				dEasting = line[1].GetVald();
				dNorthing = line[2].GetVald();

				converter.ConvertGbos1936ToWgs84(dEasting, dNorthing, 0.0, lat, lon, alt);
			}
			if (strcmp(line[0].GetVals(), "osi") == 0)
			{
				dEasting = line[1].GetVald();
				dNorthing = line[2].GetVald();

				converter.ConvertOsiToWgs84(dEasting, dNorthing, 0.0, lat, lon, alt);
			}
			if (strcmp(line[0].GetVals(), "wo") == 0)
			{
				dEasting = line[1].GetVald();
				dNorthing = line[2].GetVald();

				converter.ConvertWOToWgs84(dEasting, dNorthing, 0.0, lat, lon, alt);
			}
			if (strcmp(line[0].GetVals(), "woi") == 0)
			{
				dEasting = line[1].GetVald();
				dNorthing = line[2].GetVald();

				converter.ConvertWOIToWgs84(dEasting, dNorthing, 0.0, lat, lon, alt);
			}
			if (strcmp(line[0].GetVals(), "wp") == 0)
			{
				dEasting = line[1].GetVald();
				dNorthing = line[2].GetVald();

				converter.ConvertWOToWgs84(dEasting, dNorthing, 0.0, lat, lon, alt);
			}
			if (strcmp(line[0].GetVals(), "cas") == 0)
			{
				dEasting = line[1].GetVald();
				dNorthing = line[2].GetVald();
				converter.ConvertCasToWgs84(dEasting, dNorthing, 0.0, lat, lon, alt);
			}
			if (strcmp(line[0].GetVals(), "bns") == 0)
			{
				dEasting = line[1].GetVald();
				dNorthing = line[2].GetVald();
				converter.ConvertBnSToWgs84(dEasting, dNorthing, 0.0, lat, lon, alt);
			}
			if (strcmp(line[0].GetVals(), "bni") == 0)
			{
				dEasting = line[1].GetVald();
				dNorthing = line[2].GetVald();
				converter.ConvertBnIToWgs84(dEasting, dNorthing, 0.0, lat, lon, alt);
			}
			if (strcmp(line[0].GetVals(), "bnf") == 0)
			{
				dEasting = line[1].GetVald();
				dNorthing = line[2].GetVald();
				converter.ConvertBnFToWgs84(dEasting, dNorthing, 0.0, lat, lon, alt);
			}
			if (lat != -1.0 && lon != -1.0)
			{

				cout << "conv " << dEasting << "," << dNorthing << "->" << lat << "," << lon << endl;

				// Add point to transform constraints
				if (projType != PolyProjectArgs::Mercator)
					proj.AddPoint(imgX, imgY, dEasting, dNorthing);
				else
					proj.AddPoint(imgX, imgY, lon, lat);

				if (mercatorOut)
				{
					if (lat < south || !setBox)
						south = lat;
					if (lat > north || !setBox)
						north = lat;
					if (lon < west || !setBox)
						west = lon;
					if (lon > east || !setBox)
						east = lon;
					setBox = 1;
				}
				if (gbosOut)
				{
					if (dNorthing < south || !setBox)
						south = dNorthing;
					if (dNorthing > north || !setBox)
						north = dNorthing;
					if (dEasting < west || !setBox)
						west = dEasting;
					if (dEasting > east || !setBox)
						east = dEasting;
					setBox = 1;
				}
				// cout << "gbos " << dEasting << "," << dNorthing << "\t" << lat << "," << lon << endl;
			}
		}

		if (line.NumVals() == 6) // Read in UTM data
		{
			int zone = 0;
			int dEasting = 0, dNorthing = 0;
			double imgX = line[4].GetVald();
			double imgY = line[5].GetVald();
			double lat = -1.0, lon = -1.0;
			if (strcmp(line[0].GetVals(), "utm") == 0)
			{
				zone = line[1].GetVali();
				dEasting = line[2].GetVald();
				dNorthing = line[3].GetVald();

				converter.ConvertUTM50ToWgs84( zone, dEasting, dNorthing, lat, lon);
			}
			if (lat != -1.0 && lon != -1.0)
			{

				cout << "conv " << zone << ":" << dEasting << "," << dNorthing << "->" << lat << "," << lon << endl;

				// Add point to transform constraints
				if (projType != PolyProjectArgs::Mercator)
					proj.AddPoint(imgX, imgY, dEasting, dNorthing);
				else
					proj.AddPoint(imgX, imgY, lon, lat);

				if (mercatorOut)
				{
					if (lat < south || !setBox)
						south = lat;
					if (lat > north || !setBox)
						north = lat;
					if (lon < west || !setBox)
						west = lon;
					if (lon > east || !setBox)
						east = lon;
					setBox = 1;
				}
				if (gbosOut)
				{
					if (dNorthing < south || !setBox)
						south = dNorthing;
					if (dNorthing > north || !setBox)
						north = dNorthing;
					if (dEasting < west || !setBox)
						west = dEasting;
					if (dEasting > east || !setBox)
						east = dEasting;
					setBox = 1;
				}
				// cout << "gbos " << dEasting << "," << dNorthing << "\t" << lat << "," << lon << endl;
			}
		}

	}

    if (corners.size() == 0)
	{
		cout << "Approx bounding box N " << north << ", S " << south << ", E " << east << ", W " << west << endl;
	}
	else
	{
		setBox = 0;
		for (unsigned int i = 0; i < corners.size(); i++)
		{
			int dEasting = 0, dNorthing = 0;
			OSGBGridRefToRefCoords(corners[i].c_str(), dEasting, dNorthing);

			// Add point to transform constraints
			double lat = -1.0, lon = -1.0, alt = -1.0;
			converter.ConvertGbos1936ToWgs84(dEasting, dNorthing, 0.0, lat, lon, alt);

			if (mercatorOut)
			{
				if (lat < south || !setBox)
					south = lat;
				if (lat > north || !setBox)
					north = lat;
				if (lon < west || !setBox)
					west = lon;
				if (lon > east || !setBox)
					east = lon;
				setBox = 1;
			}
			if (gbosOut)
			{
				if (dNorthing < south || !setBox)
					south = dNorthing;
				if (dNorthing > north || !setBox)
					north = dNorthing;
				if (dEasting < west || !setBox)
					west = dEasting;
				if (dEasting > east || !setBox)
					east = dEasting;
				setBox = 1;
			}
			setBox = 1;
		}
		cout << "Manually set boundary " << north << "," << south << "," << east << "," << west << endl;
	}
}