#include "SourceKml.h"

SourceKml::SourceKml()
{
	lastAccess = 0;
	maxZoomVisible = -1;
	projType = "M";
    copyPixels = NULL;
}

SourceKml::~SourceKml()
{
    delete copyPixels;
}

void SourceKml::CreateCopyPixelsObj()
{
    copyPixels = CopyPixels::Create( projType.c_str() );
}