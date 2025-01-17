#include "stdafx.h"
#include "SpyMaskData.h"


SpyMaskData::SpyMaskData()
{
	fPos = 0LL;
	fPosNext = 0LL;
	bitmapData = NULL;
}


SpyMaskData::~SpyMaskData()
{
	if (bitmapData != NULL)
		HeapFree(GetProcessHeap(), 0, bitmapData);

}

void SpyMaskData::SetMaskName(string value)
{
	maskName = value;
}

void SpyMaskData::SetMaskType(string value)
{
	maskType = value;
}

void SpyMaskData::SetMaskImage(BITMAPFILEHEADER *pBfh, BITMAPINFOHEADER *pBmiHeader, char *pBuffer)
{
	CopyMemory(&bfh, pBfh, sizeof(BITMAPFILEHEADER));
	CopyMemory(&bmi, pBmiHeader, sizeof(BITMAPINFOHEADER));
	bitmapData = pBuffer;
}

void SpyMaskData::SetImage1Rect(RECT value)
{
	CopyMemory(&image1, &value, sizeof(RECT));
}

void SpyMaskData::SetImage2Rect(RECT value)
{
	CopyMemory(&image2, &value, sizeof(RECT));
}

void SpyMaskData::SetImage3Rect(RECT value)
{
	CopyMemory(&image3, &value, sizeof(RECT));
}

void SpyMaskData::SetImage4Rect(RECT value)
{
	CopyMemory(&image4, &value, sizeof(RECT));
}


void SpyMaskData::SetImage5Rect(RECT value)
{
	CopyMemory(&image5, &value, sizeof(RECT));
}

void SpyMaskData::SetImage6Rect(RECT value)
{
	CopyMemory(&image6, &value, sizeof(RECT));
}

void SpyMaskData::SetImageRect(RECT value)
{
	CopyMemory(&image, &value, sizeof(RECT));
}

void SpyMaskData::SetMaskRect(RECT value)
{
	CopyMemory(&maskRect, &value, sizeof(RECT));
}

void SpyMaskData::SetFilePosition(long long value)
{
	fPos = value;
}

void SpyMaskData::SetFileNextMaskPosition(long long value)
{
	fPosNext = value;
}


string SpyMaskData::GetMaskName()
{
	return maskName;
}

string SpyMaskData::GetMaskType()
{
	return maskType;
}

char *SpyMaskData::GetMaskImage(BITMAPFILEHEADER *pBfh, BITMAPINFOHEADER *pBmiHeader)
{
	CopyMemory( pBfh, &bfh, sizeof(BITMAPFILEHEADER));
	CopyMemory( pBmiHeader, &bmi, sizeof(BITMAPINFOHEADER));
	return bitmapData;
}

RECT SpyMaskData::GetImage1Rect()
{
	return image1;
}

RECT SpyMaskData::GetImage2Rect()
{
	return image2;
}

RECT SpyMaskData::GetImage3Rect()
{
	return image3;
}

RECT SpyMaskData::GetImage4Rect()
{
	return image4;
}

RECT SpyMaskData::GetImage5Rect()
{
	return image5;
}

RECT SpyMaskData::GetImage6Rect()
{
	return image6;
}

RECT SpyMaskData::GetImageRect()
{
	return image;
}

RECT SpyMaskData::GetMaskRect()
{
	return maskRect;
}

long long SpyMaskData::GetFilePosition()
{
	return fPos;
}

long long SpyMaskData::GetFileNextMaskPosition()
{
	return fPosNext;
}



