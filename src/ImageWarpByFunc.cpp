
#include "ImageWarpByFunc.h"
#include "Point.h"
#include <iostream>

//************************************************************

ImageWarpByFunc::ImageWarpByFunc( const int size ) :
	xsize( size ), ysize( size )
{
	sx = new double[xsize * ysize];
	sy = new double[xsize * ysize];
	ex = new double[xsize * ysize];
	ey = new double[xsize * ysize];
	slabel = new MESHLABEL_T[xsize * ysize];
}

ImageWarpByFunc::~ImageWarpByFunc()
{
	delete slabel;
	delete ey;
	delete ex;
	delete sy;
	delete sx;
}

int ImageWarpByFunc::Warp(class ImgMagick &in,
						  class ImgMagick &out,
						  ImageWarpByFunc::Func transform,
						  void *userPtr)
{
	// cout << "Creating raw image buffer" << endl;
	const CharPixelData *inBuf = (unsigned char *)in.GetInternalDataConst();

	// cout << "Creating raw image buffer" << endl;
	CharPixelData *outBuf = const_cast<CharPixelData*>(out.GetInternalDataConst());

	// Calculate mesh transform
	double sxmin = -0.0 * (double)in.GetWidth();
	double sxmax = 1.0 * (double)in.GetWidth();
	double symin = -0.0 * (double)in.GetHeight();
	double symax = 1.0 * (double)in.GetHeight();

	for (int j = 0; j < ysize; j++)
		for (int i = 0; i < xsize; i++)
		{
			sx[i + j * xsize] = (double)i * (sxmax - sxmin) / (double)(xsize - 1) + sxmin;
			sy[i + j * xsize] = (double)j * (symax - symin) / (double)(ysize - 1) + symin;
		}
	for (int i = 0; i < xsize * ysize; i++)
	{
		Point mesh( sx[i], sy[i] );
		Point meshProj = transform(mesh, userPtr);
		ex[i] = meshProj.x;
		ey[i] = meshProj.y;
		slabel[i] = 0;
	}

	MeshT startMesh;
	startMesh.nx = xsize;
	startMesh.ny = ysize;
	startMesh.x = sx;
	startMesh.y = sy;
	startMesh.label = slabel;
	startMesh.changed = 0;
	startMesh.reference_counting = 0;

	MeshT endMesh;
	endMesh.nx = xsize;
	endMesh.ny = ysize;
	endMesh.x = ex;
	endMesh.y = ey;
	endMesh.label = slabel;
	endMesh.changed = 0;
	endMesh.reference_counting = 0;

	// cout << "Performing warp" << endl;
	warp_image_a_m(inBuf, in.GetWidth(), in.GetHeight(), in.GetNumChannels(), in.GetWidth() * in.GetNumChannels(), in.GetNumChannels(),
				   outBuf, out.GetWidth(), out.GetHeight(), out.GetNumChannels(), out.GetWidth() * out.GetNumChannels(), out.GetNumChannels(),
				   &startMesh, &endMesh);
	return 1;
}
