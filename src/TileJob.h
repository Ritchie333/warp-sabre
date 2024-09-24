#ifndef _TILE_JOB_H
#define _TILE_JOB_H

#include <string>
#include <vector>
using namespace std;
#include "Tile.h"
#include "SourceKml.h"
#include "ReadDelimitedFile.h"

static const int DEFAULT_MAX_TILES = 25;

struct RescaleParams
{
public:
	double xscale, yscale, xori, yori;
};

class TileJob
{
public:
	int zoom, verbose;
	int tileLon, tileLat;
	string outFilename;
	string outFolder0, outFolder1, outFolder2;
	vector<unsigned int> kmlSrc;
	class Tile dst;
	int complete, running, failed;
	int enableTileLoading, maxTilesLoaded;
	class SourceKml *srcPtr;
	size_t srcSize;
	bool mergeTiles;
	class TileJob *originalObj;

	TileJob();
	TileJob &operator=(const TileJob &a);
	int Render();
	void operator()();

    static void Start( vector<TileJob>& jobs, const int maxTilesLoaded );
    static int RequestTileLoading(class SourceKml*src, const size_t size, class SourceKml &toLoad, int maxTilesLoaded);
    static const int TargetThreads();

private:
    
    static const Point RescaleTransform(const Point& in, void *userPtr);
    int GetResizedSubimage(class Tile &src, class Tile &dst, class ImgMagick &imageIn, class ImgMagick &imageOut);
};

class TileRunner
{
public:
    TileRunner();
    virtual ~TileRunner();

    vector<string> inputFiles;
    string boundsFilename;
    string outFolder;
    unsigned int minZoom;
    unsigned int maxZoom;
    unsigned int maxTilesLoaded;
    bool mergeTiles;
    int targetNumThreads;
    
    void Init();
    void SetupTileJobs();
    void RunTileJobs();
    void Clear();
    const int CountFailures() const;
private:
    vector<TileJob> jobs;
    DelimitedFile boundsFile;
    class Tile sourceBBox;
	int sourceBBoxSet = 0;
    class SourceKml* src;
};

#endif // TILE_JOB_H