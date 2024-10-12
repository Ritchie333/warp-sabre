#ifndef _WARP_H
#define _WARP_H

#include "TransformPoly.h"
#include "Log.h"

#include <string>
using namespace std;

class Warp
{
public:
    Warp();

    string inputImageFilename;
    string inputPointsFilename;
    string outputFilename;
    string kmlName;
    string kmlDesc;
    PolyProjectArgs::ProjType projType;
    vector<string> corners;
    int polynomialOrder;
    bool mercatorOut;
    bool gbosOut;
    bool visualiseErrors;
    bool fitOnly;
    bool forceAspectCoord;
    int outputWidth;
    int outputHeight;
    Log* logger;

    int Run();
    void ProjTypeFromName( const string& input);

    const static char* ProjectionNames[];
};

#endif