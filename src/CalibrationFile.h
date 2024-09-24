#ifndef CALIBRATION_FILE_H
#define CALIBRATION_FILE_H

#include "ReadDelimitedFile.h"

class CalibrationFile : public DelimitedFile
{   
public:
    PolyProjection proj;
    PolyProjectArgs::ProjType projType;
    double north = 0.0, south = 0.0, east = 0.0, west = 0.0;
	int setBox = 0;
    bool gbosOut = false;
    bool mercatorOut = false;
    vector<string> corners;

    CalibrationFile();
	CalibrationFile(const class CalibrationFile &a);
	virtual ~CalibrationFile();
	class CalibrationFile &operator=(const class CalibrationFile &a);

    void ReadProjection();
};

#endif  // CALIBRATION_FILE_H