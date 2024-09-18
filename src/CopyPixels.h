#include <vector>

class CopyPixels
{
private:
	bool CopyTile(const class ImgMagick &imageIn, class ImgMagick &imageOut, class Tile &tile, const int i, const int j, const int k, const bool merge);

protected:
	double gnorth, gsouth, geast, gwest;
	int boxset;
	void ThrowError(const char *what, const char *ref);
	CopyPixels();

public:
	static CopyPixels *Create(const char *type);

	virtual ~CopyPixels();

	virtual void UpdateBoundingBox(const char *mapref) = 0;
	virtual bool CheckIfInBox(double lat, double lon) = 0;
	void Copy(const class ImgMagick &imageIn, class ImgMagick &imageOut, class Tile &tile, const bool merge);
	// Like 'Copy' but only checks the outer 'n' pixels of an image.
	void FastCopy(const class ImgMagick &imageIn, class ImgMagick &imageOut, class Tile &tile, const int tileSize);
};

class CopyPixelsWithOsMask : public CopyPixels
{
public:
	CopyPixelsWithOsMask() {}
	virtual void UpdateBoundingBox(const char *mapref);
	virtual bool CheckIfInBox(double lat, double lon);
};

class CopyPixelsWithOSYMask : public CopyPixelsWithOsMask
{
public:
	CopyPixelsWithOSYMask() {}
	virtual void UpdateBoundingBox(const char *mapref);
};

class CopyPixelsWithRawMask : public CopyPixelsWithOsMask
{
public:
	CopyPixelsWithRawMask() {}
	virtual void UpdateBoundingBox(const char *mapref);
};

class CopyPixelsWithOsI : public CopyPixelsWithRawMask
{
public:
	CopyPixelsWithOsI() {}
	virtual bool CheckIfInBox(double lat, double lon);
};

class CopyPixelsWithCassini : public CopyPixels
{
public:
	CopyPixelsWithCassini() {}
	virtual void UpdateBoundingBox(const char *mapref);
	virtual bool CheckIfInBox(double lat, double lon);
};

class CopyPixelsWithBonne : public CopyPixels
{
public:
	CopyPixelsWithBonne() {}
	virtual void UpdateBoundingBox(const char *mapref);
	virtual bool CheckIfInBox(double lat, double lon);
};

class CopyPixelsWithIrishBonne : public CopyPixelsWithBonne
{
public:
	CopyPixelsWithIrishBonne() {}
	virtual bool CheckIfInBox(double lat, double lon);
};

class CopyPixelsWithFrenchBonne : public CopyPixelsWithBonne
{
public:
	CopyPixelsWithFrenchBonne() {}
	virtual bool CheckIfInBox(double lat, double lon);
};

class CopyPixelsWithUTM : public CopyPixels
{
public:
	CopyPixelsWithUTM() {}
	virtual void UpdateBoundingBox(const char *mapref);
	virtual bool CheckIfInBox(double lat, double lon);
private:
	// As a quick and simple hack, UTM zones can only be 668km,
	// so multiply the zone by 1000km and add the easting to get
	// an easy comparison
	long UTMEasting( const int zone, const int easting );
	int zwest, zeast;

};

class CopyPixelsWithWO : public CopyPixelsWithCassini
{
public:
	CopyPixelsWithWO() {}
	virtual bool CheckIfInBox(double lat, double lon);
};

class CopyPixelsWithWOI : public CopyPixelsWithCassini
{
public:
	CopyPixelsWithWOI() {}
	virtual bool CheckIfInBox(double lat, double lon);
};

class CopyPixelsWithMercator : public CopyPixels
{
public:
	CopyPixelsWithMercator() {}
	virtual void UpdateBoundingBox(const char *mapref);
	virtual bool CheckIfInBox(double lat, double lon);

protected:
	std::vector<double> gVertx;
	std::vector<double> gVerty;
};

class CopyPixelsWithParisMercator : public CopyPixelsWithMercator
{
public:
	CopyPixelsWithParisMercator() {}
	virtual void UpdateBoundingBox(const char *mapref);
};