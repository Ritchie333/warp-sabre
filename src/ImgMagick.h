
#ifndef IMG_MAGICK_H
#define IMG_MAGICK_H

#include "ImgFrameBase.h"
#define IMG_FRAME_TYPE_MAGICKWAND 101

class ImgMagick : public ImgFrameBase
{
public:
	ImgMagick();
	ImgMagick( const int width, const int height );
	virtual ~ImgMagick();

	virtual int Open(const char *filename);
	virtual int Create(const ImgFrameBase* src = 0);
	virtual int Save(const char *filename);
	virtual void Clear();
	virtual void Close();
	virtual int Ready() const;

	// Pixel data
	virtual double GetPix(int x, int y, unsigned int channel) const;
	virtual void SetPix(int x, int y, unsigned int channel, double val);

	// Meta data
	virtual int GetWidth() const;
	virtual int GetHeight() const;
	virtual int GetNumChannels() const;

	virtual int SetWidth(int val);
	virtual int SetHeight(int val);
	virtual int SetNumChannels(int val);

	virtual int GetInternalImageType() const;
	virtual const void *GetInternalDataConst() const;

	static void Init();
	static void Term();

protected:
	// The raw pixel data. (width * height * channels) bytes long.
	unsigned char *raw;
	int height;
	int width;
	int channels;

private:
	// Don't allow copying as there is a large amount of allocated data
	ImgMagick(const ImgMagick &a) = delete;
	ImgMagick &operator=(const ImgMagick &a) = delete;
};

#endif // IMG_MAGICK_H
