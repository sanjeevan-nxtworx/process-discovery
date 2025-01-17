#pragma once
#include "SpyWindowData.h"
#include "json11.hpp"
#include <string>
#include <map>
#include <vector>

using namespace std;
using namespace json11;

class SpyMaskData
{
	DWORD ID;
	string maskName;
	string maskType;
	BITMAPFILEHEADER bfh;
	BITMAPINFO bmi;
	char *bitmapData;

	RECT image;
	RECT image1;
	RECT image2;
	RECT image3;
	RECT image4;
	RECT image5;
	RECT image6;
	RECT maskRect;

	long long fPos;
	long long fPosNext;

public:
	SpyMaskData();
	~SpyMaskData();

	void SetMaskName(string value);
	void SetMaskType(string value);
	void SetMaskImage(BITMAPFILEHEADER *pBfh, BITMAPINFOHEADER *pBmiHeader, char *pBuffer);
	void SetMaskRect(RECT value);
	void SetImageRect(RECT value);
	void SetImage1Rect(RECT value);
	void SetImage2Rect(RECT value);
	void SetImage3Rect(RECT value);
	void SetImage4Rect(RECT value);
	void SetImage5Rect(RECT value);
	void SetImage6Rect(RECT value);
	void SetFilePosition(long long value);
	void SetFileNextMaskPosition(long long value);

	string GetMaskName();
	string GetMaskType();
	char *GetMaskImage(BITMAPFILEHEADER *pBfh, BITMAPINFOHEADER *pBmiHeader);
	RECT GetImageRect();
	RECT GetImage1Rect();
	RECT GetImage2Rect();
	RECT GetImage3Rect();
	RECT GetImage4Rect();
	RECT GetImage5Rect();
	RECT GetImage6Rect();
	RECT GetMaskRect();
	long long GetFilePosition();
	long long GetFileNextMaskPosition();

};

