#pragma once

#include <string>
using namespace std;

struct tagArgs
{
	string strVal;
	long lVal;
};

struct tagPoint
{
	long x;
	long y;
};

struct tagSize
{
	long cx;
	long cy;
};

struct tagRect
{
	long left;
	long top;
	long right;
	long bottom;
};

struct PictureInsertionProperties
{
	bool bFloating;
	bool bInline;
	struct tagPoint simplePosition;
	struct tagArgs relativeHPos;
	struct tagArgs relativeVPos;
	struct tagArgs relativeHSize;
	struct tagArgs relativeVSize;
	struct tagSize extent;
	struct tagRect effectiveExtent;
	string wrapSquare;
	string graphicFrameLockChangeAspect;
	bool bFillRect;
	struct tagRect shapeTransform;
	string presetGeometry;
};

struct PictureProperties
{
	string pictureName;
	string pictureDesc;
	string pictureTitle;
	string pictureFileName;
	string rID;
	string compressionState;
	string uri;
};

