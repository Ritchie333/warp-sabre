
#include "ImgMagick.h"
#include <MagickWand/MagickWand.h>
#include <string.h>
#include <iostream>
#include <sstream>
using namespace std;

void ImgMagick::Init()
{
	MagickWandGenesis();
}

void ImgMagick::Term()
{
	MagickWandTerminus();
}

ImgMagick::ImgMagick()
{
	raw = 0;
	height = 0;
	width = 0;
	channels = 3;
}

ImgMagick::ImgMagick( const int w, const int h )
{
	raw = 0;
	height = h;
	width = w;
	channels = 3;
}

ImgMagick::~ImgMagick()
{
	Close();
}

void ImgMagick::Close()
{
	if (raw)
		delete raw;
	raw = 0;
}

int ImgMagick::Create( const ImgFrameBase* src )
{
	Close();
	const size_t size = width * height * channels;

	raw = new unsigned char[ size ];

	if( src ) {
		// Copy from source image of the same dimensions
		const size_t otherSize = src->GetWidth() * src->GetHeight() * src->GetNumChannels();
		if( size != otherSize ) {
			throw( std::invalid_argument("src") );
		}
		memcpy( raw, src->GetInternalDataConst(), size * sizeof( unsigned char ) );
	} else {
		memset( raw, 0, size * sizeof( unsigned char ) );
	}
	return 1;
}

int ImgMagick::Open(const char *filename)
{

	Close();

	MagickWand *wand = NewMagickWand();
	// Open File
	MagickBooleanType ret = MagickReadImage(wand, filename);
	if (!ret) {
		ExceptionType severity;
		char* error = MagickGetException( wand, &severity );
		stringstream output;
		output << severity << " " << error;
		lastError = output.str();
		return -1;
	}

	width = MagickGetImageWidth(wand);
	height = MagickGetImageHeight(wand);
	channels = 3;

	raw = new unsigned char[width * height * channels];
	MagickExportImagePixels(wand, 0, 0, width, height, "RGB", CharPixel, raw);
	DestroyMagickWand(wand);
	return 1;
}

int ImgMagick::Save(const char *filename)
{
	if (!raw)
		return -1;

	MagickWand *wand = NewMagickWand();
	MagickConstituteImage(wand,
						  width, height, "RGB",
						  CharPixel, raw);

	// Save and clean up
	MagickWriteImage(wand, filename);
	ClearMagickWand(wand);
	return 1;
}

void ImgMagick::Clear()
{
	if (raw)
		delete raw;
	raw = 0;
}

int ImgMagick::Ready() const
{
	return (raw != 0);
}

const string ImgMagick::GetLastError() const
{
	return lastError;
}

// Pixel data
const CharPixelData ImgMagick::GetPix(int x, int y, unsigned int channel) const
{
	const long off = channel + (x * channels) + (y * channels * width);
	if( off < width * height * channels ) {
		return raw[off];
	} else {
		return 0;
	}
}

void ImgMagick::SetPix(int x, int y, unsigned int channel, CharPixelData val)
{
	const long off = channel + (x * channels) + (y * channels * width);
	if( off < width * height * channels ) {
		raw[off] = val;
	}
}

// Meta data
int ImgMagick::GetWidth() const
{
	return width;
}

int ImgMagick::GetHeight() const
{
	return height;
}

int ImgMagick::GetNumChannels() const
{
	return channels;
}

int ImgMagick::SetWidth(int val)
{
	width = val;
	return 1;
}

int ImgMagick::SetHeight(int val)
{
	height = val;
	return 1;
}

int ImgMagick::SetNumChannels(int val)
{
	if (val != 3)
	{
		cout << "Only 3 channels are supported, " << val << " requested" << endl;
		throw(0);
	}
	channels = 3;
	return 1;
}

int ImgMagick::GetInternalImageType() const
{
	return IMG_FRAME_TYPE_MAGICKWAND;
}

const CharPixelData *ImgMagick::GetInternalDataConst() const
{
	return raw;
}
