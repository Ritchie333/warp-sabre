#ifndef _TILE_JOB_H
#define _TILE_JOB_H

#include <string>
#include <vector>
#include <map>
#include <boost/thread.hpp>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>
using namespace std;
#include "Tile.h"
#include "SourceKml.h"
#include "ReadDelimitedFile.h"
#include "Log.h"

static const int DEFAULT_MAX_TILES = 25;

struct RescaleParams
{
public:
	double xscale, yscale, xori, yori;
};

class TileRunner;

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

	TileJob( TileRunner* runner);
	TileJob &operator=(const TileJob &a);
	int Render();
	void operator()();

    static void Start( vector<TileJob>& jobs, const int maxTilesLoaded );
    static const int TargetThreads();

private:
    TileRunner* runner;
    static const Point RescaleTransform(const Point& in, void *userPtr);
    int GetResizedSubimage(class Tile &src, class Tile &dst, class ImgMagick &imageIn, class ImgMagick &imageOut);
};

class TileRunner
{
public:
    TileRunner( const int numThreads);
    virtual ~TileRunner();

    vector<string> inputFiles;
    string boundsFilename;
    string outFolder;
    unsigned int minZoom;
    unsigned int maxZoom;
    unsigned int maxTilesLoaded;
    bool mergeTiles;
    class Log* logger;
    
    int Init();
    void SetupTileJobs();
    void RunTileJobs();
    void Clear();
    void EndThread( boost::thread::id threadId );
    void Abort();
    const int CountJobs() const;
    const int CountFailures() const;
    int RequestTileLoading(class SourceKml*src, const size_t size, class SourceKml &toLoad, int maxTilesLoaded);
private:
    vector<TileJob> jobs;
    map<boost::thread::id, boost::thread*> threads;
    bool running;
    DelimitedFile boundsFile;
    class Tile sourceBBox;
	int sourceBBoxSet = 0;
    class SourceKml* src;
    boost::mutex statusLock;
    boost::interprocess::interprocess_semaphore semaphore;
};

#endif // TILE_JOB_H