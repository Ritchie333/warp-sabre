#include "TileJob.h"
#include "SourceKml.h"
#include "ImgMagick.h"
#include "ImageWarpByFunc.h"
#include "StringUtils.h"
#include "ReadKmlFile.h"
#include "GetBounds.h"
#include "SlippyMapTile.h"
#include <sys/stat.h>
#include <boost/thread.hpp>
#include <boost/thread/thread_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

boost::mutex accessTimeLock;

using namespace std;

#if defined(_MSC_VER) || defined(_UCRT)
	#include <direct.h>

	#define MKDIR(x) _mkdir((x))
#else
	#define MKDIR(x) mkdir((x), S_IRWXU)
#endif

#define OUT_TILE_WIDTH 256
#define OUT_TILE_HEIGHT 256

TileJob::TileJob( TileRunner* _runner )
{
	zoom = 0;
	verbose = 2;
	tileLon = 0;
	tileLat = 0;
	complete = 0;
	running = 0;
	failed = 0;
	enableTileLoading = 0;
	maxTilesLoaded = DEFAULT_MAX_TILES;
	srcPtr = NULL;
	srcSize = 0;
	mergeTiles = false;
	originalObj = this;
	runner = _runner;
}

TileJob &TileJob::operator=(const TileJob &a)
{
	zoom = a.zoom;
	verbose = a.verbose;
	tileLon = a.tileLon;
	tileLat = a.tileLat;
	complete = a.complete;
	running = a.running;
	failed = a.failed;
	enableTileLoading = a.enableTileLoading;
	maxTilesLoaded = a.maxTilesLoaded;
	srcPtr = a.srcPtr;
	srcSize = a.srcSize;
	mergeTiles = a.mergeTiles;
	originalObj = a.originalObj;
	runner = a.runner;
	return *this;
}

void TileJob::operator()()
{
	this->originalObj->running = 1;

	try
	{
		Render();
	}
	catch (int e)
	{
		if (verbose >= 1)
			cout << "Error: render failed (" << e << ")" << endl;
	}

	this->originalObj->running = 0;
	this->originalObj->complete = 1;
	runner->EndThread( boost::this_thread::get_id() );
}

int TileJob::Render()
{

	if (srcPtr == NULL)
	{
		cout << "Error: Bad srcPtr pointer" << endl;
		throw(4000);
	}
	class SourceKml* src = srcPtr;
	const int size = srcSize;

	class ImgMagick outImg( this->dst.sx, this->dst.sy );
	outImg.Create();

	class ImgMagick tile( 256, 256 );
	tile.Create();

	// Magick::Image image(src.GetWidth(),src.GetHeight(),"RGB",CharPixel,src.GetInternalDataConst());

	// cout << size.width() << "," << size.height() << endl;

	// For each KML source
	int countSrcTilesUsed = 0;
	for (unsigned int t = 0; t < this->kmlSrc.size(); t++)
	{
		class SourceKml &srcKml = src[this->kmlSrc[t]];

		countSrcTilesUsed++;
		if (countSrcTilesUsed == 1)
		{
			if (verbose >= 2)
				cout << "Generating tile " << this->tileLon << "," << this->tileLat << endl;
			if (verbose >= 2)
				cout << "Tile area " << this->dst.latmin << "," << this->dst.lonmin << "," << this->dst.latmax << "," << this->dst.lonmax << endl;
		}
		if (verbose >= 2)
			cout << "Copying pixels from " << srcKml.imgFilename << endl;

		// Check map is in memory
		if (!srcKml.image.Ready())
		{
			if (enableTileLoading)
			{
				int status = RequestTileLoading(src, size, srcKml, this->maxTilesLoaded);
				if (status == 0)
				{
					this->originalObj->complete = 1;
					this->originalObj->running = 0;
					this->originalObj->failed = 1;
					if (verbose >= 1)
						cout << "Error: Could not load tile" << endl;
					throw(1000);
				}
			}
			else
			{
				if (verbose >= 1)
					cout << "Error:Loading dep not satisfied" << endl;
				this->originalObj->complete = 1;
				this->originalObj->running = 0;
				this->originalObj->failed = 1;
				throw(2000);
			}
		}

		// Update access timer for this tile
		accessTimeLock.lock();
		srcKml.lastAccess = clock();
		accessTimeLock.unlock();

		// Get location in source image containing area of interest
		double xmin = (double)srcKml.tile.sx * (this->dst.lonmin - srcKml.tile.lonmin) / (srcKml.tile.lonmax - srcKml.tile.lonmin);
		double xmax = (double)srcKml.tile.sx * (this->dst.lonmax - srcKml.tile.lonmin) / (srcKml.tile.lonmax - srcKml.tile.lonmin);
		double ymin = (double)srcKml.tile.sy * (srcKml.tile.latmax - this->dst.latmax) / (srcKml.tile.latmax - srcKml.tile.latmin);
		double ymax = (double)srcKml.tile.sy * (srcKml.tile.latmax - this->dst.latmin) / (srcKml.tile.latmax - srcKml.tile.latmin);
		xmin = (int)(xmin - 100);
		xmax = (int)(xmax + 100);
		ymin = (int)(ymin - 100);
		ymax = (int)(ymax + 100);
		if (xmin < 0)
			xmin = 0;
		if (ymin < 0)
			ymin = 0;
		if (xmax > srcKml.tile.sx)
			xmax = srcKml.tile.sx;
		if (ymax > srcKml.tile.sy)
			ymax = srcKml.tile.sy;

		// cout << "a" << endl;
		// GetSubimage(image,src,temp,inter);
		// temp.write("temp.png");

		// cout << "b" << endl;
		GetResizedSubimage(srcKml.tile, this->dst, srcKml.image, tile);
		// GetResizedSubimage(inter,dst,temp);
		// cout << "c" << endl;

		if( srcKml.copyPixels ) {
			srcKml.copyPixels->Copy(tile, outImg, this->dst, this->mergeTiles);
		}
	} // End of copy KML source

// Get OS grid positions of tile corners
#if 0
	int drawGridCorners = 0;	
	if(drawGridCorners)
	{
	double eaMax=-1.0, noMax=-1.0, heMax=-1.0;
	double eaMin=-1.0, noMin=-1.0, heMin=-1.0;
	gConverter.ConvertWgs84ToGbos1936(job.dst.latmax, job.dst.lonmax, 0.0,eaMax, noMax, heMax);
	gConverter.ConvertWgs84ToGbos1936(job.dst.latmin, job.dst.lonmin, 0.0,eaMin, noMin, heMin);
	//cout << eaMax << "\t" << noMax << "\t" << heMax << endl;
	//cout << eaMin << "\t" << noMin << "\t" << heMin << endl;

	//Draw grid corners on tile
	for(int ea = floor(eaMin/1000.0);ea < ceil(eaMax/1000.0);ea+=1)
	for(int no = floor(noMin/1000.0);no < ceil(noMax/1000.0);no+=1)
	{
	//cout << ea << "," << no << endl;
		//Convert grid ref to lat lon
		double cornerLat = -1.0, cornerLon = -1.0, cornerHeight = -1.0;
		gConverter.ConvertGbos1936ToWgs84(ea*1000, no*1000, 0.0,cornerLat, cornerLon, cornerHeight);

		//Convert to pixel position
		double cornerx=-1.0, cornery=-1.0;
		job.dst.Project(cornerLat, cornerLon, cornerx, cornery);

		DrawMarker(outImg, cornerx, cornery);
	}
	}
#endif

	// outImg = tile;

	if (!dirExists(this->outFolder0.c_str()))
		MKDIR(this->outFolder0.c_str());

	if (!dirExists(this->outFolder1.c_str()))
		MKDIR(this->outFolder1.c_str());

	if (!dirExists(this->outFolder2.c_str()))
		MKDIR(this->outFolder2.c_str());

	outImg.Save(this->outFilename.c_str());
	// temp.syncPixels();
	// image.syncPixels();

	this->originalObj->complete = 1;

	return 1;
}

int TileJob::RequestTileLoading(class SourceKml*src, const size_t size, class SourceKml &toLoad, int maxTilesLoaded)
{
	// Touch this timer to prevent it being unloaded straight away
	accessTimeLock.lock(); // Reset timer
	toLoad.lastAccess = clock();
	accessTimeLock.unlock();

	if (toLoad.image.Ready())
	{
		return 2; // Don't do anything if tile already in memory
	}

	// Count how many tiles in memory
	int count = 0;
	for (unsigned int i = 0; i < size; i++)
		if (src[i].image.Ready())
			count++;
	cout << "Currently " << count << " tiles in memory" << endl;

	clock_t lowTime = 0;
	int lowTimeSet = 0;
	int lowIndex = -1;
	while (count >= maxTilesLoaded)
	{
		if (count >= maxTilesLoaded + 2)
		{
			cout << "Failing..... too many tiles loaded in memory" << endl;
			return 0;
		}

		// Need to unload a tile. Find tile that has not been recently used.
		// cout << "count " << count << ", src size " << size << endl;
		accessTimeLock.lock();
		for (unsigned int i = 0; i < size; i++)
		{
			// cout << "accessed " << src[i].imgFilename << " at " << (int)src[i].lastAccess << ", ready" << src[i].image.Ready() << endl;
			if ((src[i].lastAccess < lowTime || lowTimeSet == 0) && src[i].image.Ready())
			{
				lowTime = src[i].lastAccess;
				lowIndex = i;
				// cout << i << "," << (int)src[i].lastAccess << "," << lowTime << "," << (bool)(src[i].lastAccess < lowTime) << endl;
				// cout << "lowIndex " << lowIndex << endl;
				lowTimeSet = 1;
			}
		}
		accessTimeLock.unlock();

		// Unload tile
		if (lowIndex != -1)
		{
			cout << "Unloading " << src[lowIndex].imgFilename << endl;
			src[lowIndex].image.Clear();
		}
		else
		{
			cout << "Failed to find a tile to unload" << endl;
			return 0;
		}

		// Recount
		count = 0;
		for (unsigned int i = 0; i < size; i++)
			if (src[i].image.Ready())
				count++;
	}

	// Load required tile into mem
	cout << "Loading " << toLoad.imgFilename << "...";
	cout.flush();
	int ret = toLoad.image.Open(toLoad.imgFilename.c_str());
	if (ret < 0)
	{
		cout << "Failed to open image " << toLoad.imgFilename << endl;
		return 0;
	}
	else
		cout << "done" << endl;
	toLoad.tile.sx = toLoad.image.GetWidth();
	toLoad.tile.sy = toLoad.image.GetHeight();

	return 1;
}

const Point TileJob::RescaleTransform(const Point& in, void *userPtr)
{
	struct RescaleParams *params = (struct RescaleParams *)userPtr;
	Point out (in.x * params->xscale - params->xori, in.y * params->yscale - params->yori);
	return out;
}

int TileJob::GetResizedSubimage(class Tile &src, class Tile &dst, class ImgMagick &imageIn, class ImgMagick &imageOut)
{
	struct RescaleParams params;
	params.xscale = (src.lonmax - src.lonmin) * dst.sx / ((dst.lonmax - dst.lonmin) * src.sx);
	params.yscale = (src.latmax - src.latmin) * dst.sy / ((dst.latmax - dst.latmin) * src.sy);
	params.xori = params.xscale * src.sx * (dst.lonmin - src.lonmin) / (src.lonmax - src.lonmin);
	params.yori = params.yscale * src.sy * (src.latmax - dst.latmax) / (src.latmax - src.latmin);

	class ImageWarpByFunc warp( 10 );
	warp.Warp(imageIn, imageOut, RescaleTransform, &params);

	// cout << "scale " << xscale << "\t" << yscale << endl;
	// cout << "ori " << xori << "\t" << yori << endl;

	// double lat, lon, alt;
	// ConvertGbos1936ToWgs84(302000,588000,0.0,lat,lon, alt);
	// cout << "testA " << lat << "," << lon << endl;
	// cout << "test " << (lon - dstW) * dstWidth / (dstE - dstW) << "," << (dstN - lat) * dstHeight / (dstN - dstS) << endl;
	// cout << dst.sx<<","<<dst.sy << endl;

	return 1;
}

const int TileJob::TargetThreads()
{
    return boost::thread::hardware_concurrency();
}

//*****************************************


TileRunner::TileRunner( const int numThreads ) :
	semaphore( numThreads )
{
	src = NULL;
}
    
TileRunner::~TileRunner()
{
    delete[] src;
}
    
void TileRunner::Init()
{
    int boundsOpen = boundsFile.Open(boundsFilename.c_str());
	if (boundsOpen < 1)
		cout << "Could not read " << boundsOpen << " file" << endl;

    src = new SourceKml[ inputFiles.size() ];

    //***************************************************
	//** For each input file, parse KML into local mem
	//***************************************************

	for (unsigned int i = 0; i < inputFiles.size(); i++)
	{
		cout << "Source file '" << inputFiles[i] << "'" << endl;
		string filePath = GetFilePath(inputFiles[i].c_str());

		class SourceKml &last = src[i];
		string imgFilename;
		last.kmlFilename = inputFiles[i];
		int ret = ReadKmlFile(last.kmlFilename.c_str(), last.tile, imgFilename);
		last.imgFilename = filePath;
		last.imgFilename += "/";
		last.imgFilename += imgFilename;
		if (ret < 1)
		{
			cout << "Kml " << last.kmlFilename << " not found";
			exit(0);
		}
		cout << last.tile.latmin << "," << last.tile.lonmin << "," << last.tile.latmax << "," << last.tile.lonmax << endl;
		cout << "image filename '" << last.imgFilename << "'" << endl;

		// Update source bounding box
		if (sourceBBox.latmin > last.tile.latmin || !sourceBBoxSet)
			sourceBBox.latmin = last.tile.latmin;
		if (sourceBBox.latmax < last.tile.latmax || !sourceBBoxSet)
			sourceBBox.latmax = last.tile.latmax;
		if (sourceBBox.lonmin > last.tile.lonmin || !sourceBBoxSet)
			sourceBBox.lonmin = last.tile.lonmin;
		if (sourceBBox.lonmax < last.tile.lonmax || !sourceBBoxSet)
			sourceBBox.lonmax = last.tile.lonmax;
		sourceBBoxSet = 1;

		// int ret = last.image.Open(last.imgFilename.c_str());
		// if(ret < 0){cout << "Filed to open image" << endl;exit(0);}
		// last.bounds.push_back("NR550000");
		// last.bounds.push_back("NR950450");
		string filenameNoPath = RemoveFilePath(inputFiles[i].c_str());
		vector<string> boundsTemp;
		string projType;
		GetBounds(boundsFile, filenameNoPath.c_str(), boundsTemp, projType);

		cout << "bounds (" << boundsTemp.size() << ")";
		if (boundsTemp.size() > 0)
		{
			last.bounds = boundsTemp;
			last.projType = projType;
			for (unsigned int j = 0; j < boundsTemp.size(); j++)
				cout << projType << " : " << boundsTemp[j] << ",";
			cout << endl;
		}
		else
		{
			stringstream tl;
			tl << last.tile.latmax << ":" << last.tile.lonmin;
			stringstream br;
			br << last.tile.latmin << ":" << last.tile.lonmax;
			last.bounds.push_back(tl.str());
			last.bounds.push_back(br.str());
			last.projType = "M";
			cout << last.projType << " : " << tl.str() << "," << br.str() << endl;
		}

		last.CreateCopyPixelsObj();
	}

    cout << "Input files bounding box:" << endl;
	cout << sourceBBox.latmin << "," << sourceBBox.lonmin << "," << sourceBBox.latmax << "," << sourceBBox.lonmax << endl;

	// Read the zoom limits from a file
	map<string, int> zoomLimitMax, zoomLimitMin;
	class DelimitedFile zoomLimitsFile;
	if (zoomLimitsFile.Open("zoomlimits.txt") >= 0)
	{
		for (unsigned int i = 0; i < zoomLimitsFile.NumLines(); i++)
		{
			class DelimitedFileLine &line = zoomLimitsFile[i];
			// cout << line[0].GetVals() << endl;
			if (line.NumVals() >= 2)
				zoomLimitMin[line[0].GetVals()] = line[1].GetVali();
			if (line.NumVals() >= 3)
				zoomLimitMax[line[0].GetVals()] = line[2].GetVali();
		}
	}

	// Set zoom levels in the respective kml sources
	for (unsigned int t = 0; t < inputFiles.size(); t++)
	{
		class SourceKml &srcKml = src[t];
		string chkStr = GetFilePath(srcKml.kmlFilename.c_str());

		map<string, int>::iterator it = zoomLimitMax.find(chkStr);
		if (it != zoomLimitMax.end())
		{
			cout << "Max zoom for " << chkStr << " is " << it->second << endl;
			srcKml.maxZoomVisible = it->second;
		}
	}

}

void TileRunner::SetupTileJobs()
{
    //************************************
	//** Calculate tile jobs to do
	//************************************
	
	for (unsigned int zoom = minZoom; zoom <= maxZoom; zoom++)
	{
		int srcWtile = long2tile(sourceBBox.lonmin, zoom);
		int srcEtile = long2tile(sourceBBox.lonmax, zoom);
		int srcStile = lat2tile(sourceBBox.latmin, zoom);
		int srcNtile = lat2tile(sourceBBox.latmax, zoom);
		cout << "Planning zoom " << zoom << ", tiles covered " << srcStile << "," << srcWtile << "," << srcNtile << "," << srcEtile << endl;

		for (int tileLon = srcWtile; tileLon <= srcEtile; tileLon++)
			for (int tileLat = srcNtile; tileLat <= srcStile; tileLat++)
			{
				class TileJob job( this );

				string outFilename = outFolder;
				string outFolder0 = outFilename;
				outFilename += "/";
				outFilename += IntToString(zoom);
				string outFolder1 = outFilename;
				outFilename += "/";
				outFilename += IntToString(tileLon);
				string outFolder2 = outFilename;
				outFilename += "/";
				outFilename += IntToString(tileLat);
				outFilename += ".jpg";

				int skipExistingTiles = 1;
				if (fileExists(outFilename.c_str()) && skipExistingTiles)
				{
					cout << "Already exists: " << outFilename << endl;
					continue;
				}

				job.zoom = zoom;
				job.tileLon = tileLon;
				job.tileLat = tileLat;
				job.outFilename = outFilename;
				job.outFolder0 = outFolder0;
				job.outFolder1 = outFolder1;
				job.outFolder2 = outFolder2;
				job.srcPtr = src;
				job.srcSize = inputFiles.size();
				job.maxTilesLoaded = maxTilesLoaded;
				job.mergeTiles = mergeTiles;

				job.dst.latmax = tile2lat(job.tileLat, job.zoom);
				job.dst.lonmin = tile2long(job.tileLon, job.zoom);
				job.dst.latmin = tile2lat(job.tileLat + 1, job.zoom);
				job.dst.lonmax = tile2long(job.tileLon + 1, job.zoom);
				job.dst.sx = OUT_TILE_WIDTH;
				job.dst.sy = OUT_TILE_HEIGHT;

				// cout << "Planning tile " << tileLat << "," << tileLon << "," << zoom << endl;
				// For each KML source
				// cout << "test"<<src.size()<< endl;
				for (unsigned int t = 0; t < inputFiles.size(); t++)
				{
					class SourceKml &srcKml = src[t];

					// cout << srcKml.maxZoomVisible << endl;
					if (srcKml.maxZoomVisible >= 0 && srcKml.maxZoomVisible < (int)zoom)
						continue;

					// Check kml src actually contains useful info for this destination tile
					int noOverlap = 0;
					if (srcKml.tile.lonmax < job.dst.lonmin)
						noOverlap = 1;
					if (srcKml.tile.lonmin > job.dst.lonmax)
						noOverlap = 1;
					if (srcKml.tile.latmax < job.dst.latmin)
						noOverlap = 1;
					if (srcKml.tile.latmin > job.dst.latmax)
						noOverlap = 1;
					// cout << noOverlap << endl;
					if (!noOverlap)
					{
						job.kmlSrc.push_back(t);
						// cout << "source found: " << t << endl;
					}
				}
				if (job.kmlSrc.size() == 0)
					continue; // Don't bother with empty tiles

				// Add job to list
				jobs.push_back(job);

				// Ensure pointer is set
				// jobs[jobs.size()-1].originalObj = &jobs[jobs.size()-1];
				// cout << jobs[0].originalObj->complete << ",";
				// cout << jobs[1].originalObj->complete << ",";
			}
	}

	for (unsigned int i = 0; i < jobs.size(); i++)
	{
		jobs[i].originalObj = &jobs[i];
	}

}

void TileRunner::RunTileJobs()
{
    int running = 1;
	int renderCount = 0;
	while (running)
	{
		// Get the most complex dep
		unsigned int maxDepNum = 0;
		unsigned int countRemaining = 0;
		vector<unsigned int> maxDep;
		for (unsigned int jobNum = 0; jobNum < jobs.size(); jobNum++)
		{
			class TileJob &job = jobs[jobNum];
			int complete = job.originalObj->complete;
			// cout << complete << "," ;
			if (complete)
				continue;
			countRemaining++;
			if (job.kmlSrc.size() > maxDepNum)
			{
				maxDepNum = job.kmlSrc.size();
				maxDep = job.kmlSrc;
			}
		}

		cout << "Most complex dep (";
		for (unsigned int j = 0; j < maxDep.size(); j++)
			cout << maxDep[j] << " ";
		cout << ")" << endl;
#ifdef USE_MT_GENTILES
		int tilesLoadable = (maxDep.size() <= maxTilesLoaded);
#else
		int tilesLoadable = 0;
#endif

		// What jobs are satistifed by this dep?
		vector<class TileJob *> jobsInDep;
		for (unsigned int jobNum = 0; jobNum < jobs.size(); jobNum++)
		{
			int skipJob = 0;
			class TileJob &job = jobs[jobNum];
			if (job.originalObj->complete)
				continue;
			if (tilesLoadable)
			{
				// When we can fit the source data into memory, be less strict in checking here
				for (unsigned int i = 0; i < job.kmlSrc.size(); i++)
				{
					int match = 0;
					for (unsigned int j = 0; j < maxDep.size(); j++)
					{
						if (job.kmlSrc[i] == maxDep[j])
							match = 1;
					}
					if (match == 0)
						skipJob = 1;
				}
			}
			else // For slow loading, use strict dep checking
				if (job.kmlSrc != maxDep)
					skipJob = 1;
			if (!skipJob)
				jobsInDep.push_back(&job);
		}

		cout << "Tiles satisfied by this dep: " << jobsInDep.size() << endl;

		if (countRemaining == 0)
		{
			cout << "All done!" << endl;
			running = 0;
			continue;
		}

		cout << "tilesLoadable " << tilesLoadable << endl;
		if (tilesLoadable)
		{
			// Load appropriate tiles for this dep
			for (unsigned int t = 0; t < maxDep.size(); t++)
			{
				class SourceKml &toLoad = src[maxDep[t]];
				int status = TileJob::RequestTileLoading(src, inputFiles.size(), toLoad, maxTilesLoaded);
				if (status == 0)
					exit(0);
			}
		}

		//********************************
		//***  Produce tiles for this dep
		//********************************

		for (unsigned int jobNum = 0; jobNum < jobsInDep.size(); jobNum++)
		{

			class TileJob &job = *jobsInDep[jobNum];
			renderCount++;
			cout << "Render Tile " << renderCount << " of " << jobs.size() << " (in dep " << jobNum << " of " << jobsInDep.size() << ")" << endl;
			cout << "Depends on KML srcs ";
			for (unsigned int j = 0; j < job.kmlSrc.size(); j++)
				cout << job.kmlSrc[j] << " ";
			cout << endl;

			if (tilesLoadable) // Do processing in parallel
			{
				job.enableTileLoading = 0;
				job.verbose = 1;

				semaphore.wait();
				statusLock.lock();
				boost::thread* newThread = new boost::thread(job);
				threads[ newThread->get_id() ] = newThread;
				statusLock.unlock();
			}
			else // Do processing in serial
			{
				job.enableTileLoading = 1;
				job();
				job.enableTileLoading = 0;
			}

		} // End of job loop

		// Wait for things to finish
		cout << "Waiting for threads to finish...";
		cout.flush();
		while( threads.size() > 0 ) {
			threads.begin()->second->join();
		}
		cout << "done" << endl;

	} // End of dep depth loop

}

void TileRunner::EndThread( boost::thread::id threadId )
{
	statusLock.lock();
	boost::thread* thread = threads[ threadId ];
	threads.erase( threadId );
	statusLock.unlock();
	delete thread;
	semaphore.post();	// One less thread
}

const int TileRunner::CountFailures() const
{
    // Count failures
	int countFail = 0;
	for (unsigned int jobNum = 0; jobNum < jobs.size(); jobNum++)
	{
		const class TileJob &job = jobs[jobNum];
		if (job.originalObj->failed)
			countFail++;
	}
    return countFail;
}

void TileRunner::Clear()
{
    // int zoom = 14;
	for (unsigned int zoom = maxZoom; zoom >= minZoom; zoom--)
	{
		int srcWtile = long2tile(/*-3.68423*/ sourceBBox.lonmin, zoom);
		int srcEtile = long2tile(sourceBBox.lonmax, zoom);
		int srcStile = lat2tile(sourceBBox.latmin, zoom);
		int srcNtile = lat2tile(sourceBBox.latmax, zoom);
		// cout << "tiles covered " << srcStile << "," << srcWtile << "," << srcNtile << "," << srcEtile << endl;

		// int tileLon = 8024;
		// int tileLat = 5168;
		// For each tile to generate
		for (int tileLon = srcWtile; tileLon <= srcEtile; tileLon++)
			for (int tileLat = srcNtile; tileLat <= srcStile; tileLat++)
			{
				string outFilename = outFolder;
				string outFolder0 = outFilename;
				outFilename += "/";
				outFilename += IntToString(zoom);
				string outFolder1 = outFilename;
				outFilename += "/";
				outFilename += IntToString(tileLon);
				string outFolder2 = outFilename;
				outFilename += "/";
				outFilename += IntToString(tileLat);
				outFilename += ".jpg";

				if (fileExists(outFilename.c_str()))
				{
					cout << "Deleting: " << outFilename << endl;
					remove(outFilename.c_str());
				}

			} // End of tile loop
	} // End of zoom loop
}

//*****************************************

void DrawMarkerPix(class ImgMagick &img, int x, int y, double r, double g, double b)
{
	if (x < 0 || x >= img.GetWidth())
		return;
	if (y < 0 || y >= img.GetHeight())
		return;
	img.SetPix(x, y, 0, r);
	img.SetPix(x, y, 1, g);
	img.SetPix(x, y, 2, b);
}

void DrawMarker(class ImgMagick &img, double x, double y)
{
	// Draw to tile
	for (int i = -1; i <= 1; i++)
		for (int j = -1; j <= 1; j++)
		{
			if (i != 0 || j != 0)
				DrawMarkerPix(img, x + (double)i + 0.5, y + (double)j + 0.5, 0.0, 0.0, 0.0);
			else
				DrawMarkerPix(img, x + (double)i + 0.5, y + (double)j + 0.5, 255.0, 0.0, 255.0);
		}
}