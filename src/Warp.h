#ifndef _WARP_H
#define _WARP_H

#include "TransformPoly.h"

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

    int Run();
};

#endif