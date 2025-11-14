#ifndef IMAGE_WARP_BY_FUNC_H
#define IMAGE_WARP_BY_FUNC_H

#include "ImgMagick.h"
#include "Point.h"

extern "C"
{
#include "libmorph/warp2.h"
}

class ImageWarpByFunc
{
public:

	typedef const Point(*Func)(const Point& in, void* userPtr);

	ImageWarpByFunc( const int size );
	virtual ~ImageWarpByFunc();
	
	int Warp(class ImgMagick &in,
			 class ImgMagick &out,
			 Func transform,
			 void *userPtr);

	const int xsize, ysize;

	double* sx;
	double* sy;
	double* ex;
	double* ey;
	MESHLABEL_T* slabel;	

};

#endif // IMAGE_WARP_BY_FUNC_H
