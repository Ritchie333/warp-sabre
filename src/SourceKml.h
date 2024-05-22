#include "ImgMagick.h"
#include <time.h>

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

	SourceKml()
	{
		lastAccess = 0;
		maxZoomVisible = -1;
		projType = "M";
	}
	virtual ~SourceKml() {}
	
private:
	SourceKml(const SourceKml &a) = delete;
	SourceKml &operator=(const SourceKml &a) = delete;
};
