#include "ImgMagick.h"
#include "CopyPixels.h"
#include "Tile.h"
#include <time.h>
#include <string>

using namespace std;

class SourceKml
{
public:
	class Tile tile;
	class ImgMagick image;
	vector<string> bounds;
	string kmlFilename;
	string imgFilename;
	clock_t lastAccess;
	int maxZoomVisible;
	string projType;
	CopyPixels* copyPixels;

	SourceKml();
	virtual ~SourceKml();

	void CreateCopyPixelsObj();

private:
	SourceKml(const SourceKml &a) = delete;
	SourceKml &operator=(const SourceKml &a) = delete;
};
