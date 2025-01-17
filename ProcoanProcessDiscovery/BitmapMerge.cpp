#include "stdafx.h"
#include "MMHeader.h"
#include "BitmapMerge.h"
#include "Utility.h"
#include "ResizeBitmap.h"
#include "RecordBufferManager.h"

BITMAPINFOHEADER GetResizedImageInfo(int width, int height, int maxwidth, int maxheight);
void ResizeImage(unsigned char *buffer, BITMAPINFOHEADER bminfo, unsigned char *newbuffer, BITMAPINFOHEADER newbminfo);

int cursorBitmap[13][52] = {
	{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},
	{0xff,0xff,0xff,0xff,0x3f,0x48,0xcc,0xff,0x3f,0x48,0xcc,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},
	{0xff,0xff,0xff,0xff,0x3f,0x48,0xcc,0xff,0x3f,0x48,0xcc,0xff,0x3f,0x48,0xcc,0xff,0x2f,0x39,0xc8,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},
	{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x3f,0x48,0xcc,0xff,0xff,0xff,0xff,0xff,0x3f,0x48,0xcc,0xff,0x3f,0x48,0xcc,0xff,0x3f,0x48,0xcc,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},
	{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x3f,0x48,0xcc,0xff,0x3f,0x48,0xcc,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x3d,0x46,0xcc,0xff,0x31,0x3b,0xcc,0xff,0x3b,0x44,0xc4,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},
	{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x3f,0x48,0xcc,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x38,0x42,0xd3,0xff,0x39,0x42,0xcd,0xff,0x31,0x3b,0xcf,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},
	{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x3f,0x48,0xcc,0xff,0x3d,0x46,0xcc,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x3f,0x48,0xcc,0xff,0x3f,0x48,0xcc,0xff,0x37,0x42,0xd9,0xff,0x3f,0x48,0xcc,0xff,0xff,0xff,0xff,0xff},
	{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x34,0x3e,0xcd,0xff,0x3f,0x48,0xcc,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},
	{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x3f,0x48,0xcc,0xff,0x38,0x42,0xd0,0xff,0x3f,0x48,0xcc,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},
	{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x38,0x42,0xd3,0xff,0x3f,0x48,0xcc,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},
	{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x2e,0x38,0xd1,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},
	{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x3f,0x48,0xcc,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},
	{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},
};

BitmapMerge::BitmapMerge(int noBitmaps)
{
	nBitmaps = noBitmaps;
	nWidths = (int *)malloc(sizeof(int) * noBitmaps);
	nHeights = (int *)malloc(sizeof(int) * noBitmaps);
	pBitBuffers = (char **)malloc(sizeof(char *) * noBitmaps);

	pResizeBuffers = NULL;
	nResizeHeights = (int *)malloc(sizeof(int) * noBitmaps);
	nResizeWidths = (int *)malloc(sizeof(int) * noBitmaps);

	pThumbBuffers = NULL;
	nThumbHeights = (int *)malloc(sizeof(int) * noBitmaps);
	nThumbWidths = (int *)malloc(sizeof(int) * noBitmaps);

	ZeroMemory((char *)nWidths, sizeof(int) * noBitmaps);
	ZeroMemory((char *)nHeights, sizeof(int) * noBitmaps);
	ZeroMemory((char *)nResizeWidths, sizeof(int) * noBitmaps);
	ZeroMemory((char *)nResizeHeights, sizeof(int) * noBitmaps);
	ZeroMemory((char *)pBitBuffers, sizeof(char *) * noBitmaps);
	ZeroMemory((char *)nThumbWidths, sizeof(int) * noBitmaps);
	ZeroMemory((char *)nThumbHeights, sizeof(int) * noBitmaps);


	nResizeWindowHeight = 0;
	nResizeWindowWidth = 0;
	nBMPWidth = 0;
	nBMPHeight = 0;
	//pBMPBuffer = NULL;
	pRecBufferManager = NULL;

	bActiveMonitorDisplay = false;
	nActiveMonitorNum = -1;
}

BitmapMerge::~BitmapMerge()
{
	//if (pBMPBuffer != NULL)
	//	HeapFree(GetProcessHeap(), 0, pBMPBuffer);
	//pBMPBuffer = NULL;

	for (int i = 0; i < nBitmaps; i++)
	{
		pRecBufferManager->ReleaseChannelResizeBuffer(pResizeBuffers[i],pResizeBufferLen[i]);
	}

	if (nWidths != NULL)
		free(nWidths);
	nWidths = NULL;
	if (nHeights != NULL)
		free(nHeights);
	nHeights = NULL;
	if (pBitBuffers != NULL)
		free(pBitBuffers);
	pBitBuffers = NULL;
	if (nResizeHeights != NULL)
		free(nResizeHeights);
	nResizeHeights = NULL;
	if (nResizeWidths != NULL)
		free(nResizeWidths);
	nResizeWidths = NULL;
	if (nThumbHeights != NULL)
		free(nThumbHeights);
	nThumbHeights = NULL;
	if (nThumbWidths != NULL)
		free(nThumbWidths);
	nThumbWidths = NULL;
	if (pResizeBufferLen != NULL)
		free(pResizeBufferLen);
	pResizeBufferLen = NULL;
	if (pResizeBuffers != NULL)
	{
		free(pResizeBuffers);
	}
	pResizeBuffers = NULL;
	if (pThumbBufferLen != NULL)
		free(pThumbBufferLen);
	pThumbBufferLen = NULL;
	if (pThumbBuffers != NULL)
	{
		for (int i = 0; i < nBitmaps; i++)
			HeapFree(GetProcessHeap(), 0, pThumbBuffers[i]);
		free(pThumbBuffers);
	}
	pThumbBuffers = NULL;


}

void BitmapMerge::SetDisplayMode(bool bActiveDisplayOn)
{
	bActiveMonitorDisplay = bActiveDisplayOn;
	if (!bActiveMonitorDisplay)
		nActiveMonitorNum = -1;
}

bool BitmapMerge::GetDisplayMode()
{
	return bActiveMonitorDisplay;
}

void BitmapMerge::SetActiveMonitor(int monNum)
{
	nActiveMonitorNum = monNum;
}

void BitmapMerge::SetResizeWindowSize(int windowWidth, int windowHeight)
{
	nResizeWindowWidth = windowWidth;
	nResizeWindowHeight = windowHeight;
}

void BitmapMerge::SetNewResizeWindowSize(int windowWidth, int windowHeight)
{
	nResizeWindowWidth = windowWidth;
	nResizeWindowHeight = windowHeight;
	InitResizeMerge();
}



void BitmapMerge::SetSize(int bmpNo, int width, int height)
{
	if (bmpNo < nBitmaps)
	{
		nWidths[bmpNo] = width;
		nHeights[bmpNo] = height;
	}
	//if (pBMPBuffer != NULL)
	//	HeapFree(GetProcessHeap(), 0, pBMPBuffer);
	//pBMPBuffer = NULL;
}

void BitmapMerge::SetBuffer(int bmpNo, char *bitBuffer)
{
	if (bmpNo < nBitmaps)
	{
		pBitBuffers[bmpNo] = bitBuffer;
	}
}


void BitmapMerge::SetRecordBufferManager(RecordBufferManager *pRecManager)
{
	pRecBufferManager = pRecManager;
}

void BitmapMerge::InitResizeMerge()
{
	if (pThumbBuffers == NULL)
	{
		pThumbBuffers = (char **)malloc(sizeof(char *) * nBitmaps);
		ZeroMemory(pThumbBuffers, sizeof(char *) * nBitmaps);
		pThumbBufferLen = (ULONG *)malloc(sizeof(ULONG) * nBitmaps);
		ZeroMemory(pThumbBufferLen, sizeof(ULONG) * nBitmaps);
	}
	else
	{
		for (int i = 0; i < nBitmaps; i++)
		{
			HeapFree(GetProcessHeap(), 0, pThumbBuffers[i]);
			pThumbBuffers[i] = NULL;
		}
	}
	if (pResizeBuffers == NULL)
	{
		pResizeBuffers = (char **)malloc(sizeof(char *) * nBitmaps);
		ZeroMemory(pResizeBuffers, sizeof(char *) * nBitmaps);
		pResizeBufferLen = (ULONG*)malloc(sizeof(ULONG) * nBitmaps);
		ZeroMemory(pResizeBufferLen, sizeof(ULONG) * nBitmaps);
	}
	else
	{
		for (int i = 0; i < nBitmaps; i++)
		{
			pRecBufferManager->ReleaseChannelResizeBuffer(pResizeBuffers[i], pResizeBufferLen[i]);
		}
	}

	// remove margins included in width and height
//	int bmpW = CalcActualWidth() + ((nBitmaps - 1) / 2 + 2) * 5;
//	int bmpH = CalcActualHeight() + 15;

	int bmpW = CalcActualWidth();	// Include Margins
	int bmpH = CalcActualHeight();

	double actAspectRatio = (double)bmpW / (double)bmpH;
	int scaledWindowHeight = (int)((double)(nResizeWindowWidth - 20) / actAspectRatio);
	int scaledWindowWidth = nResizeWindowWidth - 20;

	if (scaledWindowHeight > (nResizeWindowHeight - 20))
	{
		scaledWindowWidth = (int) ((double)(nResizeWindowHeight - 20) * actAspectRatio);
		scaledWindowHeight = nResizeWindowHeight - 20;
	}

	double xFactor = (double)scaledWindowWidth / (double)bmpW;
	double yFactor = (double)scaledWindowHeight / (double)bmpH;
		
	for (int i = 0; i < nBitmaps; i++)
	{
		// Allocate memory for Resize Buffer
		BITMAPINFOHEADER bmInfoDest;
		int scaledWidth = (int)(xFactor * (double)nWidths[i]);
		int scaledHeight = (int)(yFactor * (double)nHeights[i]);

		bmInfoDest = GetResizedImageInfo(nWidths[i], nHeights[i], scaledWidth, scaledHeight);
		nResizeWidths[i] = bmInfoDest.biWidth;
		nResizeHeights[i] = bmInfoDest.biHeight;
		//		pResizeBufferLen[i] = (4L * bmInfoDest.biWidth * bmInfoDest.biHeight) + 512L;
		pResizeBufferLen[i] = (4L * nResizeWindowWidth * nResizeWindowHeight) + 512L;
		pResizeBuffers[i] = pRecBufferManager->GetChannelResizeBuffer(&pResizeBufferLen[i]);
	}

	scaledWindowHeight = (int)((double)(200.0) / actAspectRatio);
	scaledWindowWidth = 200;

	if (scaledWindowHeight > 200)
	{
		scaledWindowWidth = (int)((double)(200.0) * actAspectRatio);
		scaledWindowHeight = 150;
	}

	xFactor = (double)scaledWindowWidth / (double)bmpW;
	yFactor = (double)scaledWindowHeight / (double)bmpH;

	for (int i = 0; i < nBitmaps; i++)
	{
		// Allocate memory for Resize Buffer
		BITMAPINFOHEADER bmInfoDest;
		int scaledWidth = (int)(xFactor * (double)nWidths[i]);
		int scaledHeight = (int)(yFactor * (double)nHeights[i]);

		bmInfoDest = GetResizedImageInfo(nWidths[i], nHeights[i], scaledWidth, scaledHeight);
		nThumbWidths[i] = bmInfoDest.biWidth;
		nThumbHeights[i] = bmInfoDest.biHeight;
		pThumbBufferLen[i] = (4L * nThumbWidths[i] * nThumbHeights[i]) + 100L;
		pThumbBuffers[i] = (char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, pThumbBufferLen[i]);
		ZeroMemory(pThumbBuffers[i], pThumbBufferLen[i]);
	}
}


void BitmapMerge::DrawRectangleInBitmap(struct ResizeScreenBuffer *pResizeBuffer, int monNum, RECT r, bool inPt)
{
	if ((r.right - r.left) > 100 && (r.bottom - r.top) > 100)
		return;
	int bmpW = CalcActualWidth();	// Include Margins
	int bmpH = CalcActualHeight();

	double xFactor = 1.0f;
	double yFactor = 1.0f;

	if (!bActiveMonitorDisplay)
	{
		double actAspectRatio = (double)bmpW / (double)bmpH;
		int scaledWindowHeight = (int)((double)(nResizeWindowWidth - 20) / actAspectRatio);
		int scaledWindowWidth = nResizeWindowWidth - 20;

		if (scaledWindowHeight > (nResizeWindowHeight - 20))
		{
			scaledWindowWidth = (int)((double)(nResizeWindowHeight - 20) * actAspectRatio);
			scaledWindowHeight = nResizeWindowHeight - 20;
		}

		xFactor = (double)scaledWindowWidth / (double)bmpW;
		yFactor = (double)scaledWindowHeight / (double)bmpH;
	}
	else
	{
		bmpW = nWidths[monNum];	// Include Margins
		bmpH = nHeights[monNum];
		double actAspectRatio = (double)bmpW / (double)bmpH;
		int scaledWindowHeight = (int)((double)(nResizeWidths[monNum]) / actAspectRatio);
		int scaledWindowWidth = nResizeWidths[monNum];

		if (scaledWindowHeight > (nResizeHeights[monNum]))
		{
			scaledWindowWidth = (int)((double)(nResizeHeights[monNum]) * actAspectRatio);
			scaledWindowHeight = nResizeHeights[monNum];
		}

		xFactor = (double)scaledWindowWidth / (double)bmpW;
		yFactor = (double)scaledWindowHeight / (double)bmpH;
	}


	int xOffset = GetBitmapXOffset(monNum);
	int yOffset = GetBitmapYOffset(monNum);
	OffsetRect(&r, xOffset, yOffset);


	LONG width = (r.right - r.left) * xFactor;
	LONG height = (r.bottom - r.top) * yFactor;

	r.top = (LONG)((double)r.top * yFactor);
	r.left = (LONG)((double)r.left * xFactor);
	r.bottom = r.top + height;
	r.right = r.left + width;

	DrawHorizLine(pResizeBuffer, r.left,r.top,r.right,inPt);
//	DrawHorizLine(pResizeBuffer, r.left, r.top + 1, r.right, inPt);
	DrawHorizLine(pResizeBuffer, r.left, r.bottom, r.right, inPt);
//	DrawHorizLine(pResizeBuffer, r.left, r.bottom - 1, r.right, inPt);

	DrawVertLine(pResizeBuffer, r.left, r.top, r.bottom, inPt);
//	DrawVertLine(pResizeBuffer, r.left + 1, r.top, r.bottom, inPt);
	DrawVertLine(pResizeBuffer, r.right, r.top, r.bottom, inPt);
//	DrawVertLine(pResizeBuffer, r.right - 1, r.top, r.bottom, inPt);
}

void BitmapMerge::DrawHorizLine(struct ResizeScreenBuffer *pResizeBuffer, int leftCol, int row, int rightCol, bool inPt)
{
	UNREFERENCED_PARAMETER(inPt);

	unsigned char lineColor[4] = {(unsigned char) 0x00,(unsigned char)0x00, (unsigned char)0xff, (unsigned char)0xff };
	int height = pResizeBuffer->height;
	int width = pResizeBuffer->width;
	if (row >= height)
		return;
	//if (inPt)
	//	lineColor[1] = (char)0xff;

	int lineOffset = (row * width) * 4;
	int columnOffset = leftCol * 4;
	char *bufferPtr = (pResizeBuffer->buffer + lineOffset + columnOffset);

	for (int col = leftCol; col < rightCol; col++)
	{
		for (int i = 0; i < 4; i++)
		{
			*bufferPtr = lineColor[i];
			bufferPtr++;
		}
	}
}

void BitmapMerge::DrawVertLine(struct ResizeScreenBuffer *pResizeBuffer, int col, int topRow, int botRow, bool inPt)
{
	UNREFERENCED_PARAMETER(inPt);
	unsigned char lineColor[4] = {(unsigned char) 0x00, (unsigned char)0x00,(unsigned char)0xff,(unsigned char)0xff };

	//if (inPt)
	//	lineColor[1] = (char)0xff;
	int width = pResizeBuffer->width;
	for (int row = topRow; row <= botRow; row++)
	{
		if (row >= width)
			break;
		int lineOffset = (row * width) * 4;
		int columnOffset = col * 4;
		char *bufferPtr = (pResizeBuffer->buffer + lineOffset + columnOffset);
		for (int i = 0; i < 4; i++)
		{
			*bufferPtr = lineColor[i];
			bufferPtr++;
		}
	}
}

void BitmapMerge::DrawCursorInBitmap(struct ResizeScreenBuffer *pResizeBuffer, int monNum, RECT r)
{
	int bmpW = CalcActualWidth();	// Include Margins
	int bmpH = CalcActualHeight();
	double xFactor = 1.0f;
	double yFactor = 1.0f;

	if (!bActiveMonitorDisplay)
	{
		bmpW = CalcActualWidth();	// Include Margins
		bmpH = CalcActualHeight();
		double actAspectRatio = (double)bmpW / (double)bmpH;
		int scaledWindowHeight = (int)((double)(nResizeWindowWidth - 10) / actAspectRatio);
		int scaledWindowWidth = nResizeWindowWidth - 10;

		if (scaledWindowHeight > (nResizeWindowHeight - 10))
		{
			scaledWindowWidth = (int)((double)(nResizeWindowHeight - 10) * actAspectRatio);
			scaledWindowHeight = nResizeWindowHeight - 10;
		}

		xFactor = (double)scaledWindowWidth / (double)bmpW;
		yFactor = (double)scaledWindowHeight / (double)bmpH;
	}
	else
	{
		bmpW = nWidths[monNum];	// Include Margins
		bmpH = nHeights[monNum];
		double actAspectRatio = (double)bmpW / (double)bmpH;
		int scaledWindowHeight = (int)((double)(nResizeWidths[monNum]) / actAspectRatio);
		int scaledWindowWidth = nResizeWidths[monNum];

		if (scaledWindowHeight > (nResizeHeights[monNum]))
		{
			scaledWindowWidth = (int)((double)(nResizeHeights[monNum]) * actAspectRatio);
			scaledWindowHeight = nResizeHeights[monNum];
		}

		xFactor = (double)scaledWindowWidth / (double)bmpW;
		yFactor = (double)scaledWindowHeight / (double)bmpH;
	}


	int xOffset = GetBitmapXOffset(monNum);
	int yOffset = GetBitmapYOffset(monNum);

	OffsetRect(&r, xOffset, yOffset);
	LONG width = (r.right - r.left);
	LONG height = (r.bottom - r.top);

	r.top = (LONG)((double)r.top * yFactor);
	r.left = (LONG)((double)r.left * xFactor);
	r.bottom = r.top + height;
	r.right = r.left + width;
	width = pResizeBuffer->width;
	
	for (int row = r.top; row < r.bottom && row < (nResizeWindowHeight - 20); row++)
	{
		int lineOffset = (row * width) * 4;
		for (int col = r.left; col < r.right && col < (nResizeWindowWidth - 20); col++)
		{

			int columnOffset = col * 4;
			char *bufferPtr = (pResizeBuffer->buffer + lineOffset + columnOffset);
			int bitmapRowNum = row - r.top;
			int bitmapColNum = (col - r.left) * 4;

			if (cursorBitmap[bitmapRowNum][bitmapColNum] == 0xff &&
				cursorBitmap[bitmapRowNum][bitmapColNum + 1] == 0xff &&
				cursorBitmap[bitmapRowNum][bitmapColNum + 2] == 0xff)
			{
				;
			}
			else
			{
				char red, green, blue;
				red = cursorBitmap[(row - r.top)][(col - r.left) * 4 + 0];
				green = cursorBitmap[(row - r.top)][(col - r.left) * 4 + 1];
				blue = cursorBitmap[(row - r.top)][(col - r.left) * 4 + 2];
				*bufferPtr = blue;
				bufferPtr++;
				*bufferPtr = green;
				bufferPtr++;
				*bufferPtr = red;
				bufferPtr++;

			}
		}
	}
}

void BitmapMerge::MergeThumbResizeBitmaps(int monNum, struct ResizeScreenBuffer *pResizeScreenBuffer, struct MonitorScreenData *pMonitorDetails)
{
	if (pResizeBuffers == NULL)
	{
		InitResizeMerge();
	}

	ZeroMemory(pResizeScreenBuffer->buffer, (size_t)pResizeScreenBuffer->bufferSize);
	BITMAPINFOHEADER bmInfoSrc;
	BITMAPINFOHEADER bmInfoDest;
	if (bActiveMonitorDisplay)
	{
		// have to resize just one screen, so resize into the output buffer
		ZeroMemory(&bmInfoSrc, sizeof(BITMAPINFOHEADER));

		bmInfoSrc.biWidth = nWidths[monNum];
		bmInfoSrc.biHeight = nHeights[monNum];
		bmInfoSrc.biBitCount = 32;
		bmInfoSrc.biPlanes = 1;
		bmInfoSrc.biCompression = BI_RGB;

		ZeroMemory(&bmInfoDest, sizeof(bmInfoDest));
		bmInfoDest = GetResizedImageInfo(bmInfoSrc.biWidth, bmInfoSrc.biHeight, 200, 150);
		bmInfoDest.biBitCount = 32;
		bmInfoDest.biPlanes = 1;
		bmInfoDest.biCompression = BI_RGB;

		ResizeImage((unsigned char *)(pMonitorDetails[monNum].pThumbBuffer->pBuffer), bmInfoSrc, (unsigned char *)pResizeScreenBuffer->buffer, bmInfoDest);
		pResizeScreenBuffer->width = bmInfoDest.biWidth;
		pResizeScreenBuffer->height = bmInfoDest.biHeight;
		//time_t resizeTimeEnd = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	}
	else
	{
		// Resize all screens to the scaled sizes
		for (int i = 0; i < nBitmaps; i++)
		{
			ZeroMemory(&bmInfoSrc, sizeof(BITMAPINFOHEADER));
			bmInfoSrc.biWidth = nWidths[i];
			bmInfoSrc.biHeight = nHeights[i];
			bmInfoSrc.biBitCount = 32;
			bmInfoSrc.biPlanes = 1;
			bmInfoSrc.biCompression = BI_RGB;

			ZeroMemory(&bmInfoDest, sizeof(bmInfoDest));
			bmInfoDest.biWidth = nThumbWidths[i];
			bmInfoDest.biHeight = nThumbHeights[i];
			bmInfoDest.biBitCount = 32;
			bmInfoDest.biPlanes = 1;
			bmInfoDest.biCompression = BI_RGB;

			//ResizeImage((unsigned char *)monitorBuffer, bmInfoSrc, (unsigned char *)pThumbBuffers[i], bmInfoDest);
			ResizeImage((unsigned char *)(pMonitorDetails[i].pThumbBuffer->pBuffer), bmInfoSrc, (unsigned char *)pThumbBuffers[i], bmInfoDest);
			nThumbWidths[i] = bmInfoDest.biWidth;
			nThumbHeights[i] = bmInfoDest.biHeight;
			//time_t resizeTimeEnd = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		}

		// Copy the bitmaps to the dest buffer
		for (int i = 0; i < nBitmaps; i++)
		{
			int xOffset = GetThumbBitmapXOffset(i);
			int yOffset = GetThumbBitmapYOffset(i);
			int width = CalcThumbActualWidth();;
			for (int lineNum = 0; lineNum < nThumbHeights[i]; lineNum++)
			{
				int lineOffset = ((lineNum + yOffset) * width) * 4;
				int columnOffset = xOffset * 4;
				int srcLineOffset = lineNum * nThumbWidths[i] * 4;

				memcpy(pResizeScreenBuffer->buffer + lineOffset + columnOffset, pThumbBuffers[i] + srcLineOffset, nThumbWidths[i] * 4);
			}
		}
		pResizeScreenBuffer->width = CalcThumbActualWidth();
		pResizeScreenBuffer->height = CalcThumbActualHeight();
	}
}

void BitmapMerge::MergePrintImage(vector<unsigned char>& buffer, int monNum, ULONG& cx, ULONG& cy)
{
	char *pPrintScreenBuffer = NULL;
	int oldActiveMonitor = nActiveMonitorNum;
	if (bActiveMonitorDisplay)
	{
		SetActiveMonitor(monNum);
	}
	int width = GetBMPWidth();
	int height = GetBMPHeight();

	cx = width;
	cy = height;
	size_t sizeBuf = 4L * width * height;
	pPrintScreenBuffer = (char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (SIZE_T)sizeBuf + 1024);

	if (bActiveMonitorDisplay)
	{
		int xOffset = GetBitmapXOffset(monNum);
		int yOffset = GetBitmapYOffset(monNum);
		xOffset = yOffset = 0;
		for (int rowNum = 0; rowNum < nHeights[monNum]; rowNum++)
		{
			int lineOffset = ((rowNum + yOffset) * width) * 4L;
			int columnOffset = xOffset * 4;
			int srcLineOffset = rowNum * nWidths[monNum] * 4L;
			memcpy(pPrintScreenBuffer + lineOffset + columnOffset, (unsigned char *)pBitBuffers[monNum] + srcLineOffset, nWidths[monNum] * 4L);
		}
	}
	else
	{
		for (int i = 0; i < nBitmaps; i++)
		{
			int xOffset = GetBitmapXOffset(i);
			int yOffset = GetBitmapYOffset(i);
			for (int rowNum = 0; rowNum < nHeights[i]; rowNum++)
			{
				int lineOffset = ((rowNum + yOffset) * width) * 4L;
				int columnOffset = xOffset * 4;
				int srcLineOffset = rowNum * nWidths[i] * 4L;
				memcpy(pPrintScreenBuffer + lineOffset + columnOffset, (unsigned char *)pBitBuffers[i] + srcLineOffset, nWidths[i] * 4L);
			}

		}
	}

	buffer.resize(sizeBuf);
	CopyMemory(&buffer[0], pPrintScreenBuffer, sizeBuf);
	HeapFree(GetProcessHeap(), 0, pPrintScreenBuffer);
	SetActiveMonitor(oldActiveMonitor);

}

void BitmapMerge::ResizePrintImage(vector<unsigned char>& buffer, ULONG& cx, ULONG& cy)
{
	ULONG oldcx, oldcy;

	oldcx = cx;
	oldcy = cy;
	if (buffer.size() == 0)
		return;
	if (cx < 1000 && cy < 1000)
		return;

	double aspectRatio = (double)cy / (double)cx;

	if (cx > 1000)
	{
		cx = 1000;
		cy = cx * aspectRatio;
	}
	else
	{
		cy = 1000L;
		cx = cy / aspectRatio;
	}

	BITMAPINFOHEADER bmInfoSrc;
	BITMAPINFOHEADER bmInfoDest;

	ZeroMemory(&bmInfoSrc, sizeof(BITMAPINFOHEADER));

	bmInfoSrc.biWidth = oldcx;
	bmInfoSrc.biHeight = oldcy;
	bmInfoSrc.biBitCount = 32;
	bmInfoSrc.biPlanes = 1;
	bmInfoSrc.biCompression = BI_RGB;


	ZeroMemory(&bmInfoDest, sizeof(bmInfoDest));
	bmInfoDest = GetResizedImageInfo(bmInfoSrc.biWidth, bmInfoSrc.biHeight, cx, cy);
	bmInfoDest.biBitCount = 32;
	bmInfoDest.biPlanes = 1;
	bmInfoDest.biCompression = BI_RGB;

	ULONG width = bmInfoDest.biWidth;
	ULONG height = bmInfoDest.biHeight;
	size_t sizeBuf = 4L * width * height;
	char *pPrintScreenBuffer = NULL;
	pPrintScreenBuffer = (char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (SIZE_T)sizeBuf + 10240);
	ResizeImage((unsigned char *)pPrintScreenBuffer, bmInfoSrc, (unsigned char *)buffer.data(), bmInfoDest);
	cx = bmInfoDest.biHeight;
	cy = bmInfoDest.biWidth;


	buffer.clear();
	buffer.resize(sizeBuf);
	CopyMemory(&buffer[0], pPrintScreenBuffer, sizeBuf);
	HeapFree(GetProcessHeap(), 0, pPrintScreenBuffer);
}

void BitmapMerge::MergeResizeBitmaps(int monNum, struct ResizeScreenBuffer *pResizeScreenBuffer)
{
	if (pResizeBuffers == NULL)
	{
		InitResizeMerge();
	}

	ZeroMemory(pResizeScreenBuffer->buffer, (size_t)pResizeScreenBuffer->bufferSize);
	BITMAPINFOHEADER bmInfoSrc;
	BITMAPINFOHEADER bmInfoDest;
	if (bActiveMonitorDisplay)
	{
		// have to resize just one screen, so resize into the output buffer
		ZeroMemory(&bmInfoSrc, sizeof(BITMAPINFOHEADER));

		bmInfoSrc.biWidth = nWidths[monNum];
		bmInfoSrc.biHeight = nHeights[monNum];
		bmInfoSrc.biBitCount = 32;
		bmInfoSrc.biPlanes = 1;
		bmInfoSrc.biCompression = BI_RGB;
		int xOff = GetResizeBitmapXOffset(monNum);
		int yOff = GetResizeBitmapYOffset(monNum);

		ZeroMemory(&bmInfoDest, sizeof(bmInfoDest));
		bmInfoDest = GetResizedImageInfo(bmInfoSrc.biWidth, bmInfoSrc.biHeight, nResizeWindowWidth - xOff - xOff, nResizeWindowHeight - yOff - yOff);
		bmInfoDest.biBitCount = 32;
		bmInfoDest.biPlanes = 1;
		bmInfoDest.biCompression = BI_RGB;

		ResizeImage((unsigned char *)pBitBuffers[monNum], bmInfoSrc, (unsigned char *)pResizeBuffers[monNum], bmInfoDest);
		nResizeHeights[monNum] = bmInfoDest.biHeight;
		nResizeWidths[monNum] = bmInfoDest.biWidth;
		int xOffset = GetResizeBitmapXOffset(monNum);
		int yOffset = GetResizeBitmapYOffset(monNum);
		int width = nResizeWindowWidth;
		for (int lineNum = 0; lineNum < bmInfoDest.biHeight; lineNum++)
		{
			int lineOffset = ((lineNum + yOffset) * width) * 4;
			int columnOffset = xOffset * 4;
			int srcLineOffset = lineNum * bmInfoDest.biWidth * 4;

			memcpy(pResizeScreenBuffer->buffer + lineOffset + columnOffset, (unsigned char *)pResizeBuffers[monNum] + srcLineOffset, bmInfoDest.biWidth * 4L);
		}

		pResizeScreenBuffer->width = nResizeWindowWidth;
		pResizeScreenBuffer->height = nResizeWindowHeight;
		//time_t resizeTimeEnd = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	}
	else
	{
		// Resize all screens to the scaled sizes
		for (int i = 0; i < nBitmaps; i++)
		{
			ZeroMemory(&bmInfoSrc, sizeof(BITMAPINFOHEADER));
			bmInfoSrc.biWidth = nWidths[i];
			bmInfoSrc.biHeight = nHeights[i];
			bmInfoSrc.biBitCount = 32;
			bmInfoSrc.biPlanes = 1;
			bmInfoSrc.biCompression = BI_RGB;

			ZeroMemory(&bmInfoDest, sizeof(bmInfoDest));
			bmInfoDest.biWidth = nResizeWidths[i];
			bmInfoDest.biHeight = nResizeHeights[i];
			bmInfoDest.biBitCount = 32;
			bmInfoDest.biPlanes = 1;
			bmInfoDest.biCompression = BI_RGB;

			ResizeImage((unsigned char *)pBitBuffers[i], bmInfoSrc, (unsigned char *)pResizeBuffers[i], bmInfoDest);
			nResizeWidths[i] = bmInfoDest.biWidth;
			nResizeHeights[i] = bmInfoDest.biHeight;
			//time_t resizeTimeEnd = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		}
		// Copy the bitmaps to the dest buffer
		for (int i = 0; i < nBitmaps; i++)
		{
			int xOffset = GetResizeBitmapXOffset(i);
			int yOffset = GetResizeBitmapYOffset(i);
			int width = nResizeWindowWidth;
			for (int lineNum = 0; lineNum < nResizeHeights[i]; lineNum++)
			{
				int lineOffset = ((lineNum + yOffset) * width) * 4;
				int columnOffset = xOffset * 4;
				int srcLineOffset = lineNum * nResizeWidths[i] * 4;

				memcpy(pResizeScreenBuffer->buffer + lineOffset + columnOffset, pResizeBuffers[i] + srcLineOffset, nResizeWidths[i] * 4);
			}
		}
		pResizeScreenBuffer->width = nResizeWindowWidth;
		pResizeScreenBuffer->height = nResizeWindowHeight;
	}
}

int BitmapMerge::GetBMPWidth()
{
	int width;
	if (bActiveMonitorDisplay)
	{
		width = CalcBMPWidth();
	}
	else
		width = CalcActualWidth();
	return width;
}

int BitmapMerge::GetBMPHeight()
{
	int height = 0;
	if (bActiveMonitorDisplay)
		height = CalcBMPHeight();
	else
		height = CalcActualHeight();
	return height;
}

//char *BitmapMerge::GetBMPBuffer()
//{
//	return pBMPBuffer;
//}

int BitmapMerge::GetResizeBitmapXOffset(int numBmp)
{
	int xOffset = 5;

	if (!bActiveMonitorDisplay)
	{
		if (numBmp == 1 || numBmp == 3)
			xOffset = nResizeWidths[numBmp-1] + 5;
	}
	return xOffset;
}

int BitmapMerge::GetResizeBitmapYOffset(int numBmp)
{
	int yOffset = 5;

	if (!bActiveMonitorDisplay)
	{
		if (numBmp == 2 || numBmp == 3)
			yOffset = nResizeHeights[numBmp - 2] + 5;
	}
	return yOffset;
}

int BitmapMerge::GetThumbBitmapXOffset(int numBmp)
{
	int xOffset = 5;

	if (!bActiveMonitorDisplay)
	{
		if (numBmp == 1 || numBmp == 3)
			xOffset = nThumbWidths[numBmp - 1] + 5;
	}
	return xOffset;
}

int BitmapMerge::GetThumbBitmapYOffset(int numBmp)
{
	int yOffset = 5;

	if (!bActiveMonitorDisplay)
	{
		if (numBmp == 2 || numBmp == 3)
			yOffset = nThumbHeights[numBmp - 2] + 5;
	}
	return yOffset;
}

int BitmapMerge::CalcThumbActualWidth()
{
	int width = 5;
	int width1 = 5;
	int width2 = 5;

	switch (nBitmaps)
	{
	case 1:
		width = nThumbWidths[0];
		break;
	case 2:
		width = nThumbWidths[0] + nThumbWidths[1];
		break;
	case 3:
		width1 = nThumbWidths[0] + nThumbWidths[1];
		width2 = nThumbWidths[2];
		if (width1 > width2)
			width = width1;
		else
			width = width2;
		break;
	case 4:
		width1 = nThumbWidths[0] + nThumbWidths[1];
		width2 = nThumbWidths[2] + nThumbWidths[3];
		if (width1 > width2)
			width = width1;
		else
			width = width2;
		break;
	}

	width = width + 10;
	return width;
}

int BitmapMerge::CalcThumbActualHeight()
{
	int height = 5;
	int height1 = 0;
	int height2 = 0;


	switch (nBitmaps)
	{
	case 1:
		height = nThumbHeights[0];
		break;
	case 2:
		height1 = nThumbHeights[0];
		height2 = nThumbHeights[1];
		if (height1 > height2)
			height = height1;
		else
			height = height2;
		break;
	case 3:
		height1 = nThumbHeights[0] + nThumbHeights[2];
		height2 = nThumbHeights[1];
		if (height1 > height2)
			height = height1;
		else
			height = height2;
		break;
	case 4:
		height1 = nThumbHeights[0] + nThumbHeights[2];
		height2 = nThumbHeights[1] + nThumbHeights[3];
		if (height1 > height2)
			height = height1;
		else
			height = height2;
		break;
	}

	height = height + 10;

	return height;
}


int BitmapMerge::GetBitmapXOffset(int numBmp)
{
	int xOffset = 5;

	if (!bActiveMonitorDisplay)
	{
		switch (nBitmaps)
		{
		case 1:
		case 2:
			if (numBmp == 1)
				xOffset = nWidths[0] + 5;
			break;
		case 3:
		case 4:
			if (numBmp == 1)
				xOffset = nWidths[0] + 5;
			else if (numBmp == 3)
				xOffset = nWidths[2] + 5;
			break;
		}
	}
	return xOffset;
}

int BitmapMerge::GetBitmapYOffset(int numBmp)
{
	int yOffset = 5;

	if (!bActiveMonitorDisplay)
	{
		switch (nBitmaps)
		{
		case 1:
		case 2:
			break;
		case 3:
		case 4:
			if (numBmp > 1)
				yOffset = nHeights[numBmp - 2] + 5;

		}
	}
	return yOffset;
}

int BitmapMerge::CalcActualWidth()
{
	int width = 5;
	int width1 = 5;
	int width2 = 5;

	switch (nBitmaps)
	{
	case 1:
		width = nWidths[0];
		break;
	case 2:
		width = nWidths[0] + nWidths[1];
		break;
	case 3:
		width1 = nWidths[0] + nWidths[1];
		width2 = nWidths[2];
		if (width1 > width2)
			width = width1;
		else
			width = width2;
		break;
	case 4:
		width1 = nWidths[0] + nWidths[1];
		width2 = nWidths[2] + nWidths[3];
		if (width1 > width2)
			width = width1;
		else
			width = width2;
		break;
	}

	width = width + 10;
	return width;

}

int BitmapMerge::CalcActualHeight()
{
	int height = 5;
	int height1 = 0;
	int height2 = 0;


	switch (nBitmaps)
	{
	case 1:
		height = nHeights[0];
		break;
	case 2:
		height1 = nHeights[0];
		height2 = nHeights[1];
		if (height1 > height2)
			height = height1;
		else
			height = height2;
		break;
	case 3:
		height1 = nHeights[0] + nHeights[2];
		height2 = nHeights[1];
		if (height1 > height2)
			height = height1;
		else
			height = height2;
		break;
	case 4:
		height1 = nHeights[0] + nHeights[2];
		height2 = nHeights[1] + nHeights[3];
		if (height1 > height2)
			height = height1;
		else
			height = height2;
		break;
	}

	height = height + 10;

	return height;
}

int BitmapMerge::CalcBMPHeight()
{
	int height = 5;

	if (!bActiveMonitorDisplay)
	{
		height = CalcActualHeight();
	}
	else
		height += nHeights[nActiveMonitorNum] + 5;

	return height;

}


int BitmapMerge::CalcBMPWidth()
{
	int width = 5;

	if (!bActiveMonitorDisplay)
	{
		width = CalcActualWidth();
	}
	else
		width += nWidths[nActiveMonitorNum] + 5;

	return width;
}