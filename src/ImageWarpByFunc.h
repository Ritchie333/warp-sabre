#ifndef IMAGE_WARP_BY_FUNC_H
#define IMAGE_WARP_BY_FUNC_H

#include "ImgMagick.h"
#include "Point.h"

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

	int xsize, ysize;
};

#endif // IMAGE_WARP_BY_FUNC_H
