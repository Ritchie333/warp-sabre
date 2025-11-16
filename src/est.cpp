#include "gbos1936/Gbos1936.h"
#include <stdio.h>
#include <iostream>
#include <math.h>

using namespace std;

int main(int argc, char* argv[])
{
    HelmertConverter gConverter;

    if( argc > 3 ) {
        double bonfx, bonfy;
        double gradx;       
        sscanf(argv[1],"%lf",&bonfx);
        sscanf(argv[2],"%lf",&bonfy);
        sscanf(argv[3],"%lf",&gradx);

        double step = 0.0001;
        double curx = bonfx;

        double lat = 0, lon = 0, he = 0, glat = 0, glon = 0;
        
        while( glon < gradx ) {           
            curx += step;
            gConverter.ConvertBnFToWgs84(curx, bonfy, 0, lat, lon, he);            
            gConverter.ConvertWgs84ToParis(lat, lon, glat, glon);
            glon = ( glon - PARIS_CENTRE_LON ) / 0.9;
            glat = ( glat / 0.9 );
        }
        cout << curx - step << endl;
    }
}