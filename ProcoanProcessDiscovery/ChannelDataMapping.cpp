#include "stdafx.h"
#include "ChannelDataMapping.h"
#include "RecordBufferManager.h"
#include <chrono>
#include "Utility.h"
#include "FileContext.h"
#include "BPMNElement.h"

BITMAPINFOHEADER GetResizedImageInfo(int width, int height, int maxwidth, int maxheight);
void ResizeImage(unsigned char *buffer, BITMAPINFOHEADER bminfo, unsigned char *newbuffer, BITMAPINFOHEADER newbminfo);


ChannelDataMapping::ChannelDataMapping()
{
	ZeroMemory(&channelHeader, sizeof(struct channelHdr));
	ZeroMemory(&videoHdr, sizeof(struct IndexHdr));
	ZeroMemory(&audioHdr, sizeof(struct IndexHdr));
	pDataFile = NULL;
	pMonitorDetails = NULL;
	pFileContext = NULL;
	pBitmapMerge = NULL;
}

void ChannelDataMapping::SetFileContext(FileContext *pContext)
{
	pFileContext = pContext;
}

ChannelDataMapping::~ChannelDataMapping()
{
	ClearScreenBuffers();
	if (pMonitorDetails != NULL)
	{
		for (int i = 0; i < videoHdr.noMonitors; i++)
		{
			//HeapFree(GetProcessHeap(), 0, pMonitorDetails[i].pMonitorBuffer->pBuffer);
			if (pMonitorDetails[i].pThumbBuffer != NULL)
				delete pMonitorDetails[i].pThumbBuffer;
			if (pMonitorDetails[i].pMonitorBuffer != NULL)
				delete pMonitorDetails[i].pMonitorBuffer;
		}
		delete pMonitorDetails;
	}
	pMonitorDetails = NULL;
	if (pBitmapMerge != NULL)
		delete pBitmapMerge;
	pBitmapMerge = NULL;
	screenData.clear();
}

void ChannelDataMapping::AddChannelMonitorInfo(struct MonitorHdr *pMonHeaders)
{
	// Write Monitor Info to the new Channel
	for (int i = 0; i < videoHdr.noMonitors; i++)
	{
		if (i == 0)
			videoHdr.fPosMonitorDetails = pDataFile->AppendToFile((char *)&pMonHeaders[i], sizeof(struct MonitorHdr));
		else
			pDataFile->AppendToFile((char *)&pMonHeaders[i], sizeof(struct MonitorHdr));
	}
}

void ChannelDataMapping::GetChannelMonitorInfo(struct MonitorHdr *pMonHeaders)
{
	for (int i = 0; i < videoHdr.noMonitors; i++)
	{
		if (i == 0)
			pDataFile->ReadFromFile((char *)&pMonHeaders[i], sizeof(struct MonitorHdr), videoHdr.fPosMonitorDetails);
		else
			pDataFile->ReadFromFile((char *)&pMonHeaders[i], sizeof(struct MonitorHdr), -1L);
	}
}

void ChannelDataMapping::AddVideoHeader()
{
	// Write Video Root
	channelHeader.posVideoRoot = pDataFile->AppendToFile((char *)&videoHdr, sizeof(struct VideoHdr));
}

void ChannelDataMapping::AddAudioHeader()
{
	// Write Audio Header
	channelHeader.posAudioRoot = pDataFile->AppendToFile((char *)&audioHdr, sizeof(struct AudioHdr));
}

void ChannelDataMapping::SetDataFile(DataFile *pFile)
{
	pDataFile = pFile;
}

int ChannelDataMapping::GetNumMonitors()
{
	return videoHdr.noMonitors;
}

void ChannelDataMapping::SetRecordBufferManager(RecordBufferManager *pRecBuffManager)
{
	pRecBufferManager = pRecBuffManager;
}

USHORT ChannelDataMapping::ReadChannelHeader(long long channelFpos)
{
	pDataFile->ReadFromFile((char *)&channelHeader, sizeof(struct channelHdr), channelFpos);
	if (channelHeader.RecType != 'C') // 'C'
	{
		return USHRT_MAX;
	}
	channelHeader.posCurrRec = channelFpos;
	pDataFile->ReadFromFile((char *)&audioHdr, sizeof(struct AudioHdr), channelHeader.posAudioRoot);
	if (audioHdr.RecType != 'A')
	{
		return USHRT_MAX;
	}
	pDataFile->ReadFromFile((char *)&videoHdr, sizeof(struct VideoHdr), channelHeader.posVideoRoot);
	if (videoHdr.RecType != 'V')
	{
		return USHRT_MAX;
	}

	pDataFile->ReadFromFile((char *)&eventHdr, sizeof(struct EventHdr), channelHeader.posEventRoot);


	GetMonitorDetails(videoHdr.noMonitors, videoHdr.fPosMonitorDetails);
	SetScreenBuffers();
	return channelHeader.channelNum;
}


void ChannelDataMapping::CreateScreenBuffers()
{
	GetMonitorDetails(videoHdr.noMonitors, videoHdr.fPosMonitorDetails);
	SetScreenBuffers();
}

struct bufferRecord *ChannelDataMapping::ProcessMonitorScreensForThumbs(int *nMon, struct bufferRecord *pBuffer)
{
	if (pBuffer == NULL)
		return NULL;
	struct CompareBuffer *pCompareBuffer = (struct CompareBuffer *)pBuffer->buffer;

	// Get Monitor
	int monNum = 0;
	for (monNum = 0; monNum < videoHdr.noMonitors; monNum++)
	{
		if (pMonitorDetails[monNum].MonitorDetails.MonitorNumber == pCompareBuffer->screenNum)
			break;
	}
	if (monNum == videoHdr.noMonitors)
	{
		// This is an error, can never happen
		*nMon = 0;
		return NULL;
	}
	DWORD width, height;
	// Whole screen
	width = pMonitorDetails[monNum].MonitorDetails.right - pMonitorDetails[monNum].MonitorDetails.left;
	height = pMonitorDetails[monNum].MonitorDetails.bottom - pMonitorDetails[monNum].MonitorDetails.top;

	if (pCompareBuffer->numOfBlocks == -1)
	{
		UnpackData((long *)pMonitorDetails[monNum].pThumbBuffer->pBuffer, (long *)&pCompareBuffer->blockData, height, width);
	}
	else
	{
		UnpackCompareData((long *)pMonitorDetails[monNum].pThumbBuffer->pBuffer, (long *)&pCompareBuffer->blockData, width, pCompareBuffer->numOfBlocks);
	}

	pRecBufferManager->FreePlayerBuffer(pBuffer);
	*nMon = monNum;
	return pBuffer;
}

struct bufferRecord *ChannelDataMapping::ProcessMonitorScreens(int *nMon)
{
	struct bufferRecord *pBuffer = pRecBufferManager->GetWriteBuffer();
	struct CompareBuffer *pCompareBuffer = (struct CompareBuffer *)pBuffer->buffer;

	// Get Monitor
	int monNum = 0;
	for (monNum = 0; monNum < videoHdr.noMonitors; monNum++)
	{
		if (pMonitorDetails[monNum].MonitorDetails.MonitorNumber == pCompareBuffer->screenNum)
			break;
	}
	if (monNum == videoHdr.noMonitors)
	{
		// This is an error, can never happen
		*nMon = 0;
		return NULL;
	}
	DWORD width, height;
	// Whole screen
	width = pMonitorDetails[monNum].MonitorDetails.right - pMonitorDetails[monNum].MonitorDetails.left;
	height = pMonitorDetails[monNum].MonitorDetails.bottom - pMonitorDetails[monNum].MonitorDetails.top;

	if (pCompareBuffer->numOfBlocks == -1)
	{
		UnpackData((long *)pMonitorDetails[monNum].pMonitorBuffer->pBuffer, (long *)&pCompareBuffer->blockData, height, width);
	}
	else
	{
		UnpackCompareData((long *)pMonitorDetails[monNum].pMonitorBuffer->pBuffer, (long *)&pCompareBuffer->blockData, width, pCompareBuffer->numOfBlocks);
	}

	pRecBufferManager->FreePlayerBuffer(pBuffer);

	*nMon = monNum;
	return pBuffer;
}

void ChannelDataMapping::GetEventImage(int monNum, vector<unsigned char>& buffer, long& cx, long& cy, ULONG nEventIndex, vector <BPMNElement *> &pElementList)
{
	RECT boundingRect;
	ZeroMemory(&boundingRect, sizeof(RECT));
	RECT *pEventRect = pFileContext->GetBoundingRect(nEventIndex);
	if (pEventRect != NULL)
	{
		InflateRect(pEventRect, 1, 1);
		CopyMemory(&boundingRect, pEventRect, sizeof(RECT));
	}

	DrawRectangleInEventBitmap(pMonitorDetails[monNum].pMonitorBuffer->pBuffer, monNum, boundingRect);
	vector <BPMNElement *>::iterator itElementList = pElementList.begin();
	while (itElementList != pElementList.end())
	{
		BPMNElement *pElement = *itElementList;
		boundingRect = *(pElement->GetBoundingRect());
		DrawRectangleInEventBitmap(pMonitorDetails[monNum].pMonitorBuffer->pBuffer, monNum, boundingRect);
		itElementList++;
	}
	int width = pMonitorDetails[monNum].MonitorDetails.virRect.right - pMonitorDetails[monNum].MonitorDetails.virRect.left;
	int height = pMonitorDetails[monNum].MonitorDetails.virRect.bottom - pMonitorDetails[monNum].MonitorDetails.virRect.top;

	
	BITMAPINFO bpmInfo;
	ZeroMemory(&bpmInfo, sizeof(BITMAPINFO));
	int bpp = 32;
	bpmInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bpmInfo.bmiHeader.biWidth = width;
	bpmInfo.bmiHeader.biHeight = height;
	bpmInfo.bmiHeader.biPlanes = 1; // we are assuming that there is only one plane
	bpmInfo.bmiHeader.biBitCount = bpp;
	bpmInfo.bmiHeader.biCompression = BI_RGB;
	bpmInfo.bmiHeader.biSizeImage = (width * bpp  * height)/8;
	bpmInfo.bmiHeader.biClrImportant = 0;

	BITMAPFILEHEADER hdr;
	ZeroMemory(&hdr, sizeof(BITMAPFILEHEADER));
	// type == BM
	hdr.bfType = 0x4d42;

	hdr.bfSize = (sizeof(BITMAPFILEHEADER) + bpmInfo.bmiHeader.biSize + bpmInfo.bmiHeader.biClrUsed * sizeof(RGBQUAD) + bpmInfo.bmiHeader.biSizeImage);
	hdr.bfReserved1 = 0;
	hdr.bfReserved2 = 0;
	hdr.bfOffBits = sizeof(BITMAPFILEHEADER) + bpmInfo.bmiHeader.biSize + bpmInfo.bmiHeader.biClrUsed * sizeof(RGBQUAD);

	LPBYTE lpBits = (LPBYTE)pMonitorDetails[monNum].pMonitorBuffer->pBuffer;

	size_t sizeBmp = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (bpmInfo.bmiHeader.biClrUsed * sizeof(RGBQUAD)) + bpmInfo.bmiHeader.biSizeImage;
	buffer.resize((size_t)sizeBmp);
	CopyMemory(&buffer[0], &hdr, sizeof(BITMAPFILEHEADER));
	CopyMemory(&buffer[0 + sizeof(BITMAPFILEHEADER)], &bpmInfo, sizeof(BITMAPINFOHEADER) + bpmInfo.bmiHeader.biClrUsed * sizeof(RGBQUAD));
	size_t sizeHeader = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (bpmInfo.bmiHeader.biClrUsed * sizeof(RGBQUAD));
	CopyMemory(&buffer[sizeHeader], lpBits, bpmInfo.bmiHeader.biSizeImage);

	HDC dc = GetDC(NULL);

	int noPixX = GetDeviceCaps(dc, LOGPIXELSX);
	int noPixY = GetDeviceCaps(dc, LOGPIXELSY);

	cx = (long)(width * 914400L) / noPixX;
	cy = (long)(height * 914400L) / noPixY;
}

void ChannelDataMapping::DrawRectangleInEventBitmap(LPSTR pBuffer, int monNum, RECT r)
{
	
	if (GetNumMonitors() > 1 && !pBitmapMerge->GetDisplayMode())
	{
		;
	}
	else
	{
		r.left -= pMonitorDetails[monNum].MonitorDetails.virRect.left;
		r.top -= pMonitorDetails[monNum].MonitorDetails.virRect.top;
		r.right -= pMonitorDetails[monNum].MonitorDetails.virRect.left;
		r.bottom -= pMonitorDetails[monNum].MonitorDetails.virRect.top;
	}
	int width, height;

	width = pBitmapMerge->GetBMPWidth();
	height = pBitmapMerge->GetBMPHeight();

	if (r.right + 3 >= width)
		r.right = width - 3;
	if (r.bottom + 3 >= height)
		r.bottom = height - 3;
	if (r.top <= 3)
		r.top = 3;
	if (r.left <= 3)
		r.left = 3;
	DrawHorizLine(pBuffer, width, height, r.left, r.top, r.right);
	DrawHorizLine(pBuffer, width, height, r.left, r.top-1, r.right);
	DrawHorizLine(pBuffer, width, height, r.left, r.top - 2, r.right);

	DrawHorizLine(pBuffer, width, height, r.left, r.bottom, r.right);
	DrawHorizLine(pBuffer, width, height, r.left, r.bottom + 1, r.right);
	DrawHorizLine(pBuffer, width, height, r.left, r.bottom + 2, r.right);

	DrawVertLine(pBuffer, width, height, r.left, r.top, r.bottom);
	DrawVertLine(pBuffer, width, height, r.left - 1, r.top, r.bottom);
	DrawVertLine(pBuffer, width, height, r.left - 2, r.top, r.bottom);

	DrawVertLine(pBuffer, width, height, r.right, r.top, r.bottom);
	DrawVertLine(pBuffer, width, height, r.right + 1, r.top, r.bottom);
	DrawVertLine(pBuffer, width, height, r.right + 2, r.top, r.bottom);
}


void ChannelDataMapping::DrawHorizLine(char *pBuffer, int width, int height, int leftCol, int row, int rightCol)
{
	unsigned char lineColor[4] = { (unsigned char)0x00,(unsigned char)0x00, (unsigned char)0xff, (unsigned char)0xff };
	if (row >= height)
		return;

	int lineOffset = (row * width) * 4;
	int columnOffset = leftCol * 4;
	char *bufferPtr = (pBuffer + lineOffset + columnOffset);

	for (int col = leftCol; col < rightCol; col++)
	{
		for (int i = 0; i < 4; i++)
		{
			*bufferPtr = lineColor[i];
			bufferPtr++;
		}
	}
}

void ChannelDataMapping::DrawVertLine(char *pBuffer, int width, int height, int col, int topRow, int botRow)
{
	unsigned char lineColor[4] = { (unsigned char)0x00, (unsigned char)0x00,(unsigned char)0xff,(unsigned char)0xff };

	for (int row = topRow; row <= botRow; row++)
	{
		if (row >= width)
			break;
		int lineOffset = (row * width) * 4;
		int columnOffset = col * 4;
		char *bufferPtr = (pBuffer + lineOffset + columnOffset);
		for (int i = 0; i < 4; i++)
		{
			*bufferPtr = lineColor[i];
			bufferPtr++;
		}
	}
}

void ChannelDataMapping::MergePrintImage(int monNum, vector<unsigned char>& buffer, ULONG& cx, ULONG& cy, ULONG nEventIndex, vector <BPMNElement *> &pElementList)
{
	//time_t mergeTimeStart = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	vector <unsigned char> mergeBuffer;

	mergeBuffer.clear();
	pBitmapMerge->MergePrintImage(mergeBuffer, monNum, cx, cy);

	RECT boundingRect;
	ZeroMemory(&boundingRect, sizeof(RECT));
	RECT *pEventRect = pFileContext->GetBoundingRect(nEventIndex);
	if (pEventRect != NULL)
	{
		InflateRect(pEventRect, 1, 1);
		CopyMemory(&boundingRect, pEventRect, sizeof(RECT));
	}

	DrawRectangleInEventBitmap((LPSTR)mergeBuffer.data(), monNum, boundingRect);
	vector <BPMNElement *>::iterator itElementList = pElementList.begin();
	while (itElementList != pElementList.end())
	{
		BPMNElement *pElement = *itElementList;
		boundingRect = *(pElement->GetBoundingRect());
		DrawRectangleInEventBitmap((LPSTR)mergeBuffer.data(), monNum, boundingRect);
		itElementList++;
	}

	//pBitmapMerge->ResizePrintImage(mergeBuffer, cx, cy);

	int width = cx;
	int height = cy;


	BITMAPINFO bpmInfo;
	ZeroMemory(&bpmInfo, sizeof(BITMAPINFO));
	int bpp = 32;
	bpmInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bpmInfo.bmiHeader.biWidth = width;
	bpmInfo.bmiHeader.biHeight = height;
	bpmInfo.bmiHeader.biPlanes = 1; // we are assuming that there is only one plane
	bpmInfo.bmiHeader.biBitCount = bpp;
	bpmInfo.bmiHeader.biCompression = BI_RGB;
	bpmInfo.bmiHeader.biSizeImage = (width * bpp  * height) / 8;
	bpmInfo.bmiHeader.biClrImportant = 0;

	BITMAPFILEHEADER hdr;
	ZeroMemory(&hdr, sizeof(BITMAPFILEHEADER));
	// type == BM
	hdr.bfType = 0x4d42;

	hdr.bfSize = (sizeof(BITMAPFILEHEADER) + bpmInfo.bmiHeader.biSize + bpmInfo.bmiHeader.biClrUsed * sizeof(RGBQUAD) + bpmInfo.bmiHeader.biSizeImage);
	hdr.bfReserved1 = 0;
	hdr.bfReserved2 = 0;
	hdr.bfOffBits = sizeof(BITMAPFILEHEADER) + bpmInfo.bmiHeader.biSize + bpmInfo.bmiHeader.biClrUsed * sizeof(RGBQUAD);

	size_t sizeBmp = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (bpmInfo.bmiHeader.biClrUsed * sizeof(RGBQUAD)) + bpmInfo.bmiHeader.biSizeImage;
	buffer.resize((size_t)sizeBmp);
	CopyMemory(&buffer[0], &hdr, sizeof(BITMAPFILEHEADER));
	CopyMemory(&buffer[0 + sizeof(BITMAPFILEHEADER)], &bpmInfo, sizeof(BITMAPINFOHEADER) + bpmInfo.bmiHeader.biClrUsed * sizeof(RGBQUAD));
	size_t sizeHeader = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (bpmInfo.bmiHeader.biClrUsed * sizeof(RGBQUAD));
	CopyMemory(&buffer[sizeHeader], mergeBuffer.data(),cx * cy * 4L);

	HDC dc = GetDC(NULL);

	int noPixX = GetDeviceCaps(dc, LOGPIXELSX);
	int noPixY = GetDeviceCaps(dc, LOGPIXELSY);

	cx = (long)(width * 914400L) / noPixX;
	cy = (long)(height * 914400L) / noPixY;

}



void ChannelDataMapping::ResizeVideoImage(struct bufferRecord *pBuffer, int monNum)
{
	//time_t mergeTimeStart = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	USHORT *pFileContextNumPosition = (USHORT *)((char *)pBuffer + sizeof(struct bufferRecord) + 2);
	USHORT *pFileContextChannelNumPosition = (USHORT *)((char *)pBuffer + sizeof(struct bufferRecord) + 2 + sizeof(USHORT));

	USHORT currFileNum = *pFileContextNumPosition;
	USHORT currChannelNum = *pFileContextChannelNumPosition;

	struct ResizeScreenBuffer *pResizeScreenBuffer = NULL;
	// First get the Buffer;
	while (pResizeScreenBuffer == NULL)
	{
		pResizeScreenBuffer = pRecBufferManager->GetFreeResizeScreenBuffer();
	}

	pBitmapMerge->MergeResizeBitmaps(monNum, pResizeScreenBuffer);
	DrawCursorInBitmap(pBitmapMerge, pResizeScreenBuffer, pBuffer->mouseX, pBuffer->mouseY);
	POINT mousePoint;
	mousePoint.x = pBuffer->mouseX;
	mousePoint.y = pBuffer->mouseY;
	//if (PtInRect(&eventDetails.controlAtPoint.boundingRect, mousePoint))
	//	DrawRectanglesInBitmap(pBitmapMerge, eventDetails.controlInFocus.boundingRect, eventDetails.controlAtPoint.boundingRect);
	//time_t mergeTimeEnd = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	//if (monNum == 0)
	//	WriteBMPFile(bmInfoDest, pResizeScreenBuffer->buffer);
	pResizeScreenBuffer->fileNumber = currFileNum;
	pResizeScreenBuffer->channelNumber = currChannelNum;
	pResizeScreenBuffer->t1FileStart = pBuffer->timeStamp;
	pResizeScreenBuffer->recDurationus = pBuffer->recDurationus;
	pResizeScreenBuffer->fPosIndexHeader = pBuffer->fPosIndex;
	pResizeScreenBuffer->indexNum = pBuffer->indexNum;
	pResizeScreenBuffer->eventIndex = pBuffer->eventIndex;

	pRecBufferManager->DisplayResizeBuffer(pResizeScreenBuffer);

}

void ChannelDataMapping::ResizeThumbVideoImage(struct bufferRecord *pBuffer, int monNum)
{
	//time_t mergeTimeStart = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

	//struct CompareBuffer *pCompareBuffer = (struct CompareBuffer *)pBuffer->buffer;
	USHORT *pFileContextNumPosition = (USHORT *)((char *)pBuffer + sizeof(struct bufferRecord) + 2);
	USHORT *pFileContextChannelNumPosition = (USHORT *)((char *)pBuffer + sizeof(struct bufferRecord) + 2 + sizeof(USHORT));

	USHORT currFileNum = *pFileContextNumPosition;
	USHORT currChannelNum = *pFileContextChannelNumPosition;

	struct ResizeScreenBuffer *pResizeScreenBuffer = NULL;
	// First get the Buffer;
	pResizeScreenBuffer = pRecBufferManager->GetThumbResizeScreenBuffer();
	pBitmapMerge->MergeThumbResizeBitmaps(monNum, pResizeScreenBuffer, pMonitorDetails);
	POINT mousePoint;
	mousePoint.x = pBuffer->mouseX;
	mousePoint.y = pBuffer->mouseY;
	//time_t mergeTimeEnd = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	pResizeScreenBuffer->fileNumber = currFileNum;
	pResizeScreenBuffer->channelNumber = currChannelNum;
	pResizeScreenBuffer->t1FileStart = pBuffer->timeStamp;
	pResizeScreenBuffer->recDurationus = pBuffer->recDurationus;
	pResizeScreenBuffer->fPosIndexHeader = pBuffer->fPosIndex;
	pResizeScreenBuffer->indexNum = pBuffer->indexNum;
	pResizeScreenBuffer->eventIndex = pBuffer->eventIndex;
	pRecBufferManager->DisplayThumbResizeBuffer(pResizeScreenBuffer);
}

int ChannelDataMapping::ProcessVideoRecord(struct bufferRecord *pBuffer, int lastMon)
{
	struct CompareBuffer *pCompareBuffer = (struct CompareBuffer *)pBuffer->buffer;
	// Get Buffer channel Number and File Number
	USHORT *pFileContextNumPosition = (USHORT *)((char *)pBuffer + sizeof(struct bufferRecord) + 2);
	USHORT *pFileContextChannelNumPosition = (USHORT *)((char *)pBuffer + sizeof(struct bufferRecord) + 2 + sizeof(USHORT));

	USHORT currFileNum = *pFileContextNumPosition;
	USHORT currChannelNum = *pFileContextChannelNumPosition;
	//if (pBuffer->timeStamp == 60134423884LL)
	//{
	//	int x = 10;
	//	x++;
	//}
	// Get Monitor
	int monNum = 0;
	for (monNum = 0; monNum < videoHdr.noMonitors; monNum++)
	{
		if (pMonitorDetails[monNum].MonitorDetails.MonitorNumber == pCompareBuffer->screenNum)
			break;
	}
	if (monNum == videoHdr.noMonitors)
	{
		// This is an error, can never happen
		return 0;
	}
	DWORD width, height;
	// Whole screen
	width = pMonitorDetails[monNum].MonitorDetails.right - pMonitorDetails[monNum].MonitorDetails.left;
	height = pMonitorDetails[monNum].MonitorDetails.bottom - pMonitorDetails[monNum].MonitorDetails.top;

	if (pCompareBuffer->numOfBlocks == -1)
	{
		UnpackData((long *)pMonitorDetails[monNum].pMonitorBuffer->pBuffer, (long *)&pCompareBuffer->blockData, height, width);
	}
	else
	{
		UnpackCompareData((long *)pMonitorDetails[monNum].pMonitorBuffer->pBuffer, (long *)&pCompareBuffer->blockData, width, pCompareBuffer->numOfBlocks);
	}
	//WriteImageBMP(monNum);

	if (lastMon >= monNum)
	{
		lastMon = monNum;
		//time_t mergeTimeStart = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

		struct ResizeScreenBuffer *pResizeScreenBuffer = NULL;
		// First get the Buffer;
		//while (pResizeScreenBuffer == NULL)
		//{
		pResizeScreenBuffer = pRecBufferManager->GetFreeResizeScreenBuffer();
		//}

		pBitmapMerge->MergeResizeBitmaps(monNum, pResizeScreenBuffer);
		DrawCursorInBitmap(pBitmapMerge, pResizeScreenBuffer, pBuffer->mouseX, pBuffer->mouseY);
		POINT mousePoint;
		mousePoint.x = pBuffer->mouseX;
		mousePoint.y = pBuffer->mouseY;
		static RECT boundingRect;
		RECT *pEventRect = pFileContext->GetBoundingRect(pBuffer->eventIndex);
		if (pEventRect != NULL)
		{
			InflateRect(pEventRect, 1, 1);
			CopyMemory(&boundingRect, pEventRect, sizeof(RECT));
		}
		if (PtInRect(&boundingRect, mousePoint))
			DrawRectanglesInBitmap(pBitmapMerge, pResizeScreenBuffer, boundingRect);
		//time_t mergeTimeEnd = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		//if (monNum == 0)
		//	WriteBMPFile(bmInfoDest, pResizeScreenBuffer->buffer);
		pResizeScreenBuffer->fileNumber = currFileNum;
		pResizeScreenBuffer->channelNumber = currChannelNum;
		pResizeScreenBuffer->t1FileStart = pBuffer->timeStamp;
		pResizeScreenBuffer->recDurationus = pBuffer->recDurationus;
		pResizeScreenBuffer->fPosIndexHeader = pBuffer->fPosIndex;
		pResizeScreenBuffer->indexNum = pBuffer->indexNum;
		pResizeScreenBuffer->eventIndex = pBuffer->eventIndex;

		pRecBufferManager->DisplayResizeBuffer(pResizeScreenBuffer);
	}

	return monNum;
}

void ChannelDataMapping::DrawRectanglesInBitmap(BitmapMerge *pBmpMerge, struct ResizeScreenBuffer *pResizeBuffer, RECT boundingRectControl)
{
	POINT pt;
	int monNum = 0;

	// Get Monitor
	for (monNum = 0; monNum < videoHdr.noMonitors; monNum++)
	{
		pt.x = boundingRectControl.left;
		pt.y = boundingRectControl.top;
		if (PtInRect(&pMonitorDetails[monNum].MonitorDetails.virRect, pt))
		{
			pt.x = boundingRectControl.right;
			pt.y = boundingRectControl.bottom;
			if (PtInRect(&pMonitorDetails[monNum].MonitorDetails.virRect, pt))
			{
				break;
			}
		}
	}
	if (monNum == videoHdr.noMonitors)
		return;

	boundingRectControl.left -= pMonitorDetails[monNum].MonitorDetails.virRect.left;
	boundingRectControl.top -= pMonitorDetails[monNum].MonitorDetails.virRect.top;
	boundingRectControl.right -= pMonitorDetails[monNum].MonitorDetails.virRect.left;
	boundingRectControl.bottom -= pMonitorDetails[monNum].MonitorDetails.virRect.top;

	//if (boundingRectControl.left < 0)
	//	boundingRectControl.left = 0;
	//if (boundingRectControl.right > (pMonitorDetails[monNum].MonitorDetails.virRect.right - pMonitorDetails[monNum].MonitorDetails.virRect.left))
	//	boundingRectControl.right = pMonitorDetails[monNum].MonitorDetails.virRect.right - pMonitorDetails[monNum].MonitorDetails.virRect.left;
	//if (boundingRectControl.top < 0)
	//	boundingRectControl.top = 0;
	//if (boundingRectControl.bottom > (pMonitorDetails[monNum].MonitorDetails.virRect.bottom - pMonitorDetails[monNum].MonitorDetails.top))
	//	boundingRectControl.bottom = pMonitorDetails[monNum].MonitorDetails.virRect.bottom - pMonitorDetails[monNum].MonitorDetails.virRect.top;
	pBmpMerge->DrawRectangleInBitmap(pResizeBuffer, monNum, boundingRectControl, true);


}


void ChannelDataMapping::DrawCursorInBitmap(BitmapMerge *pBmpMerge, struct ResizeScreenBuffer *pResizeBuffer, short xPos, short yPos)
{

	POINT pt;
	pt.x = xPos;
	pt.y = yPos;

	int monNum = 0;

	// Get Monitor
	for (monNum = 0; monNum < videoHdr.noMonitors; monNum++)
	{
		if (PtInRect(&pMonitorDetails[monNum].MonitorDetails.virRect, pt))
			break;
	}
	if (monNum == videoHdr.noMonitors)
		return;
	if (monNum == 1)
	{
		int i = 0;
		i++;
	}
	pt.x = pt.x - pMonitorDetails[monNum].MonitorDetails.virRect.left;
	pt.y = pt.y - pMonitorDetails[monNum].MonitorDetails.virRect.top;
	RECT r;
	r.left = pt.x;
	r.right = pt.x + 13;
	r.top = pt.y;
	r.bottom = pt.y + 13;

	if (r.left < 0)
		r.left = 0;
	if (r.right > (pMonitorDetails[monNum].MonitorDetails.virRect.right - pMonitorDetails[monNum].MonitorDetails.virRect.left))
		r.right = pMonitorDetails[monNum].MonitorDetails.virRect.right - pMonitorDetails[monNum].MonitorDetails.virRect.left;
	if (r.top < 0)
		r.top = 0;
	if (r.bottom > (pMonitorDetails[monNum].MonitorDetails.virRect.bottom - pMonitorDetails[monNum].MonitorDetails.virRect.top))
		r.bottom = pMonitorDetails[monNum].MonitorDetails.virRect.bottom - pMonitorDetails[monNum].MonitorDetails.virRect.top;
	pBmpMerge->DrawCursorInBitmap(pResizeBuffer, monNum, r);
}

void ChannelDataMapping::UpdateBitmapMergeWindowDimensions(bool bActiveMonitorDisplay, int windowWidth, int windowHeight)
{
	if (pBitmapMerge == NULL)
		return;

	pBitmapMerge->SetDisplayMode(bActiveMonitorDisplay);
	pBitmapMerge->SetNewResizeWindowSize(windowWidth, windowHeight);
}

void ChannelDataMapping::CreateBitmapMerge(bool bActiveMonitorDisplay, int windowWidth, int windowHeight)
{
	if (pBitmapMerge != NULL)
		delete pBitmapMerge;

	pBitmapMerge = NULL;
	pBitmapMerge = DBG_NEW BitmapMerge(videoHdr.noMonitors);
	pBitmapMerge->SetRecordBufferManager(pRecBufferManager);
	pBitmapMerge->SetResizeWindowSize(windowWidth, windowHeight);

	for (int monCnt = 0; monCnt < videoHdr.noMonitors; monCnt++)
	{
		int width, height;

		width = pMonitorDetails[monCnt].MonitorDetails.right - pMonitorDetails[monCnt].MonitorDetails.left;
		height = pMonitorDetails[monCnt].MonitorDetails.bottom - pMonitorDetails[monCnt].MonitorDetails.top;

		pBitmapMerge->SetSize(monCnt, width, height);
		pBitmapMerge->SetBuffer(monCnt, pMonitorDetails[monCnt].pMonitorBuffer->pBuffer);
	}

	// Initialize for the entire screen all monitors
	pBitmapMerge->InitResizeMerge();
	// Then set the flag for resize monitor
	pBitmapMerge->SetDisplayMode(bActiveMonitorDisplay);
	//if (bActiveMonitorDisplay)
	//	pBitmapMerge->SetActiveMonitor(videoHdr.noMonitors - 1);
}

struct channelHdr *ChannelDataMapping::GetChannelHeader()
{
	return &channelHeader;
}


long long ChannelDataMapping::GetNextChannelPosition()
{
	return channelHeader.fPosNextChannel;
}

void ChannelDataMapping::GetAudioHeader(struct AudioHdr *pHeader)
{
	CopyMemory(pHeader, &audioHdr, sizeof(struct AudioHdr));
}

DWORD ChannelDataMapping::GetAudioBufferSize()
{
	return ((audioHdr.freq * audioHdr.noChannels * audioHdr.bitsPerSample) / 8);
}

void ChannelDataMapping::GetVideoHeader(struct VideoHdr *pHeader)
{
	CopyMemory(pHeader, &videoHdr, sizeof(struct VideoHdr));
}

void ChannelDataMapping::WriteVideoHeader(struct VideoHdr *pHeader)
{
	CopyMemory(&videoHdr, pHeader, sizeof(struct VideoHdr));
	pDataFile->WriteToFile(channelHeader.posVideoRoot, (char *)&videoHdr, (ULONG)sizeof(struct VideoHdr));
}

void ChannelDataMapping::WriteChannelHeader(long long fPos)
{
	pDataFile->WriteToFile(fPos, (char *)&channelHeader, (ULONG)sizeof(struct channelHdr));
}

struct VideoHdr *ChannelDataMapping::GetVideoHeader()
{
	return &videoHdr;
}

struct AudioHdr *ChannelDataMapping::GetAudioHeader()
{
	return &audioHdr;
}

void ChannelDataMapping::ReadAudioData(map <long long, struct indexPageDescriptor *> *mapAudio)
{
	long long fPosIndex = audioHdr.fPosRoot;
	struct indexPageDescriptor *pPrevDescriptor = NULL;

	while (fPosIndex != 0L)
	{
		pDataFile->ReadFromFile((char *)&audioIndex, sizeof(struct IndexHdr), fPosIndex);
		struct indexPageDescriptor *pDescriptor = DBG_NEW struct indexPageDescriptor;
		if (videoIndex.RecType == 'P')
		{
			struct videoIndexDescriptor *pAudioDescriptor = DBG_NEW struct videoIndexDescriptor;
			pAudioDescriptor->nextFpos = audioIndex.fPosNextRecord;
			pAudioDescriptor->prevFpos = audioIndex.fPosPrevRecord;
			pAudioDescriptor->startTime = 0L;
			pAudioDescriptor->inPath = false;
			pAudioDescriptor->timespan = 0L;
			pAudioDescriptor->currFPos = fPosIndex;
			pDescriptor->nPageType = 4;
			pDescriptor->pageDescriptor.pVideo = pAudioDescriptor; // Video and Audio has same structure
			if (pPrevDescriptor != NULL)
				pPrevDescriptor->next = pDescriptor;
			pDescriptor->next = NULL;
			
			pPrevDescriptor = pDescriptor;
			mapAudio->insert(pair<long long, struct indexPageDescriptor *>(fPosIndex, pDescriptor));
			pDescriptor->numRecs = audioIndex.numRecs;
			for (int i = 0; i <= audioIndex.numRecs; i++)
			{
				pDescriptor->dataRecs[i].indexNum = audioIndex.dataRecs[i].videoIndexNumber;
				pDescriptor->dataRecs[i].recDuration = (long)audioIndex.dataRecs[i].recDurationus;
				pAudioDescriptor->timespan += audioIndex.dataRecs[i].recDurationus;
			}
		}
		fPosIndex = audioIndex.fPosNextRecord;
	}
}

void ChannelDataMapping::ReadVideoData(bool bMainPath, map <long long, struct indexPageDescriptor *> *mapVideo)
{
	ReadVideoData(bMainPath, mapVideo, videoHdr.fPosRoot);
}

void ChannelDataMapping::ReadVideoData(bool bMainPath, map <long long, struct indexPageDescriptor *> *mapVideo, long long fPos)
{
	long long fPosIndex = fPos;
	struct indexPageDescriptor *pPrevDescriptor = NULL;
	struct IndexHdr vIndex;
	ZeroMemory(&vIndex, sizeof(struct IndexHdr));
	int channelNum = 0;
	while (fPosIndex != 0L)
	{
		pDataFile->ReadFromFile((char *)&vIndex, sizeof(struct IndexHdr), fPosIndex);
		struct indexPageDescriptor *pDescriptor = DBG_NEW struct indexPageDescriptor;
		pDescriptor->channelNum = channelNum;
		if (vIndex.RecType == 'H') // 'C'
		{
			struct channelIndexDescriptor *pChannelDescriptor = DBG_NEW struct channelIndexDescriptor;

			pChannelDescriptor->inPath = bMainPath;
			pChannelDescriptor->currFPos = fPosIndex;
			pChannelDescriptor->newChannel = vIndex.dataRecs[0].size;
			pChannelDescriptor->prevChannel = vIndex.dataRecs[0].compSize;
			pChannelDescriptor->nextFPos = vIndex.fPosNextRecord;
			pChannelDescriptor->prevFpos = vIndex.fPosPrevRecord;
			pChannelDescriptor->startTime = 0L;

			channelNum = pChannelDescriptor->newChannel;
			pDescriptor->nPageType = 5;
			pDescriptor->numRecs = 0;
			pDescriptor->next = NULL;
			pDescriptor->pageDescriptor.pChannel = pChannelDescriptor;
			mapVideo->insert(pair<long long, struct indexPageDescriptor *>(fPosIndex, pDescriptor));
			for (int i = 0; i <= vIndex.numRecs; i++)
			{
				pDescriptor->dataRecs[i].indexNum = vIndex.dataRecs[i].videoIndexNumber;
				pDescriptor->dataRecs[i].recDuration = 0L; // (long)vIndex.dataRecs[i].recDurationus;
			}

		}
		else if (vIndex.RecType == 'J')
		{
			map <long long, struct indexPageDescriptor *>::iterator itMap = mapVideo->find(fPosIndex);

			if (itMap == mapVideo->end())
			{
				struct joinIndexDescriptor *pJoinDescriptor = DBG_NEW struct joinIndexDescriptor;
				pJoinDescriptor->bPathDir = false;
				pJoinDescriptor->currFPos = fPosIndex;
				pJoinDescriptor->nextFpos = vIndex.fPosNextRecord;
				pJoinDescriptor->prevTopFpos = vIndex.fPosPrevRecord;
				pJoinDescriptor->prevRightFpos = vIndex.dataRecs[0].fPosData;
				pDescriptor->nPageType = 3;
				pDescriptor->numRecs = 0;
				pDescriptor->next = NULL;
				pDescriptor->pageDescriptor.pJoin = pJoinDescriptor;
				// if not found insert
				mapVideo->insert(pair<long long, struct indexPageDescriptor *>(fPosIndex, pDescriptor));
				for (int i = 0; i <= vIndex.numRecs; i++)
				{
					pDescriptor->dataRecs[i].indexNum = vIndex.dataRecs[i].videoIndexNumber;
					pDescriptor->dataRecs[i].recDuration = 0L; //(long)vIndex.dataRecs[i].recDurationus;
				}
				if (pPrevDescriptor != NULL)
					pPrevDescriptor->next = pDescriptor;
				pPrevDescriptor = pDescriptor;
				break;
			}
			else
			{
				delete pDescriptor;
				pDescriptor = itMap->second;
			}
		}
		else if (vIndex.RecType == 'B')
		{
			struct branchIndexDescriptor *pBranchDescriptor = DBG_NEW struct branchIndexDescriptor;
			pBranchDescriptor->bPathDir = false;
			pBranchDescriptor->currFPos = fPosIndex;
			pBranchDescriptor->nextBottomFPos = vIndex.fPosNextRecord;
			pBranchDescriptor->nextRightFPos = vIndex.dataRecs[0].fPosData;
			pBranchDescriptor->prevFpos  = vIndex.fPosPrevRecord;
			pBranchDescriptor->branchChannelNum = channelNum;
			pDescriptor->nPageType = 2;
			pDescriptor->next = NULL;
			pDescriptor->numRecs = 0;
			pDescriptor->pageDescriptor.pBranch = pBranchDescriptor;
			mapVideo->insert(pair<long long, struct indexPageDescriptor *>(fPosIndex, pDescriptor));
			for (int i = 0; i <= vIndex.numRecs; i++)
			{
				pDescriptor->dataRecs[i].indexNum = vIndex.dataRecs[i].videoIndexNumber;
				pDescriptor->dataRecs[i].recDuration = (long)vIndex.dataRecs[i].recDurationus;
			}
			ReadVideoData(false, mapVideo, pBranchDescriptor->nextRightFPos);
		}
		else if (vIndex.RecType == 'P')
		{
			struct videoIndexDescriptor *pVideoDescriptor = DBG_NEW struct videoIndexDescriptor;
			pVideoDescriptor->nextFpos = vIndex.fPosNextRecord;
			pVideoDescriptor->prevFpos = vIndex.fPosPrevRecord;
			pVideoDescriptor->startTime = 0L;
			pVideoDescriptor->inPath = bMainPath;
			pVideoDescriptor->timespan = 0L;
			pVideoDescriptor->currFPos = fPosIndex;
			pDescriptor->nPageType = 1;
			pDescriptor->pageDescriptor.pVideo = pVideoDescriptor;
			pDescriptor->next = NULL;
			mapVideo->insert(pair<long long, struct indexPageDescriptor *>(fPosIndex, pDescriptor));
			pDescriptor->numRecs = vIndex.numRecs;
			int monNum = GetNumMonitors() - 1;
			for (int i = 0; i <= vIndex.numRecs; i++)
			{
				pDescriptor->dataRecs[i].indexNum = vIndex.dataRecs[i].videoIndexNumber;
				pDescriptor->dataRecs[i].recDuration = (long)vIndex.dataRecs[i].recDurationus;
				if (i >= monNum)
					pVideoDescriptor->timespan += vIndex.dataRecs[i].recDurationus;
			}
		}
		if (pPrevDescriptor != NULL)
			pPrevDescriptor->next = pDescriptor;
		pPrevDescriptor = pDescriptor;
		fPosIndex = vIndex.fPosNextRecord;
	}
}

void ChannelDataMapping::GetMonitorDetails(int noMonitors, long long fPosMonitorDetails)
{
	pMonitorDetails = DBG_NEW struct MonitorScreenData[noMonitors];
	ZeroMemory((char *)pMonitorDetails, sizeof(struct MonitorScreenData) * noMonitors);
	wxString strKey = "";

	strKey = wxString::Format("%02d", noMonitors);
	for (int i = 0; i < noMonitors; i++)
	{
		if (i == 0)
			pDataFile->ReadFromFile((char *)&pMonitorDetails[i].MonitorDetails, sizeof(struct MonitorHdr), fPosMonitorDetails);
		else
			pDataFile->ReadFromFile((char *)&pMonitorDetails[i].MonitorDetails, sizeof(struct MonitorHdr), -1L);
		struct ScreenData *screen = &pMonitorDetails[i].screen;
		screen->hMonitor = pMonitorDetails[i].MonitorDetails.MonitorNumber;
		//screen->g_pd3dDevice = NULL;
		//screen->g_pSurface = NULL;
		screen->bCapture = false;
		screen->bFullScreen = false;
		screen->rcMonitor.left = pMonitorDetails[i].MonitorDetails.left;
		screen->rcMonitor.right = pMonitorDetails[i].MonitorDetails.right;
		screen->rcMonitor.top = pMonitorDetails[i].MonitorDetails.top;
		screen->rcMonitor.bottom = pMonitorDetails[i].MonitorDetails.bottom;

		int width = pMonitorDetails[i].MonitorDetails.right - pMonitorDetails[i].MonitorDetails.left;
		int height = pMonitorDetails[i].MonitorDetails.bottom - pMonitorDetails[i].MonitorDetails.top;
		screen->bmInfo.biBitCount = 32;
		screen->bmInfo.biHeight = height;
		screen->bmInfo.biWidth = width;
		strKey += wxString::Format("%08X%05dx%05d", (ULONG)screen->hMonitor, width, height);
		screen->bmInfo.biSize = sizeof(BITMAPINFOHEADER);
		screen->bmInfo.biPlanes = 1;
		screen->bmInfo.biCompression = BI_RGB;
		screen->bmInfo.biSizeImage = 0;
		screen->bmInfo.biXPelsPerMeter = 0;
		screen->bmInfo.biYPelsPerMeter = 0;
		screen->bmInfo.biClrUsed = 0;
		screen->bmInfo.biClrImportant = 0;
		screenData.push_back(screen);

		struct MonitorScreenBuffer *pMonitorBuffer = DBG_NEW struct MonitorScreenBuffer;
		pMonitorBuffer->sizeOfBuffer = 4L * height * width;
		pMonitorBuffer->pBuffer = NULL;
		pMonitorDetails[i].pMonitorBuffer = pMonitorBuffer;
		pMonitorBuffer = DBG_NEW struct MonitorScreenBuffer;
		pMonitorBuffer->sizeOfBuffer = 4L * height * width;
		pMonitorBuffer->pBuffer = NULL;
		pMonitorDetails[i].pThumbBuffer = pMonitorBuffer;

	}
	channelKey = strKey;
}

void ChannelDataMapping::SetScreenBuffers()
{
	for (size_t i = 0; i < screenData.size(); i++)
	{
		struct MonitorScreenBuffer *pMonitorBuffer = pMonitorDetails[i].pMonitorBuffer;
		int width = pMonitorDetails[i].MonitorDetails.right - pMonitorDetails[i].MonitorDetails.left;
		int height = pMonitorDetails[i].MonitorDetails.bottom - pMonitorDetails[i].MonitorDetails.top;
		pMonitorBuffer->sizeOfBuffer = 4L * height * width;
		pMonitorBuffer->pBuffer = pRecBufferManager->GetChannelScreenBuffer(&pMonitorBuffer->sizeOfBuffer);
	}
}

void ChannelDataMapping::SetupThumbBuffers()
{
	for (size_t i = 0; i < screenData.size(); i++)
	{
		struct MonitorScreenBuffer *pThumbBuffer = pMonitorDetails[i].pThumbBuffer;
		int width = pMonitorDetails[i].MonitorDetails.right - pMonitorDetails[i].MonitorDetails.left;
		int height = pMonitorDetails[i].MonitorDetails.bottom - pMonitorDetails[i].MonitorDetails.top;
		pThumbBuffer->sizeOfBuffer = 4L * height * width;
		pThumbBuffer->pBuffer = pRecBufferManager->GetChannelScreenBuffer(&pThumbBuffer->sizeOfBuffer);
	}
}

void ChannelDataMapping::ClearThumbBuffers()
{
	for (size_t i = 0; i < screenData.size(); i++)
	{
		struct MonitorScreenBuffer *pThumbBuffer = pMonitorDetails[i].pThumbBuffer;
		int width = pMonitorDetails[i].MonitorDetails.right - pMonitorDetails[i].MonitorDetails.left;
		int height = pMonitorDetails[i].MonitorDetails.bottom - pMonitorDetails[i].MonitorDetails.top;
		pThumbBuffer->sizeOfBuffer = 4L * height * width;
		pRecBufferManager->ReleaseChannelResizeBuffer(pThumbBuffer->pBuffer, pThumbBuffer->sizeOfBuffer);
		pThumbBuffer->pBuffer = NULL;

	}
}

wxString ChannelDataMapping::GetChannelKey()
{
	return channelKey;
}

void ChannelDataMapping::ClearScreenBuffers()
{
	for (size_t i = 0; i < screenData.size(); i++)
	{
		struct MonitorScreenBuffer *pMonitorBuffer = pMonitorDetails[i].pMonitorBuffer;

		pRecBufferManager->ReleaseChannelScreenBuffer(pMonitorBuffer->pBuffer, pMonitorBuffer->sizeOfBuffer);
		pMonitorBuffer->pBuffer = NULL;
	}
}


struct ScreenData *ChannelDataMapping::GetScreenData(int screenNum)
{
	struct ScreenData *screen = NULL;

	if (screenNum <= videoHdr.noMonitors)
		screen = &pMonitorDetails[screenNum].screen;
	return screen;
}

struct ScreenData *ChannelDataMapping::GetScreenData(HMONITOR hMon)
{
	struct ScreenData *screen = NULL;

	for (int i = 0; i < videoHdr.noMonitors; i++)
	{
		struct ScreenData *screenPtr = &pMonitorDetails[i].screen;
		if (screenPtr->hMonitor == hMon)
		{
			screen = screenPtr;
			break;
		}
	}

	return screen;
}


HMONITOR ChannelDataMapping::GetScreenNumber(int xPos, int yPos)
{
	HMONITOR hMon = NULL;
	POINT pt;
	pt.x = xPos;
	pt.y = yPos;

	for (int i = 0; i < videoHdr.noMonitors; i++)
	{
		struct ScreenData *screenPtr = &pMonitorDetails[i].screen;
		if (PtInRect(&pMonitorDetails[i].MonitorDetails.virRect, pt))
		{
			hMon = screenPtr->hMonitor;
			break;
		}
	}

	return hMon;
}

int ChannelDataMapping::GetScreenSubscript(int xPos, int yPos)
{
	int hMon = -1;
	POINT pt;
	pt.x = xPos;
	pt.y = yPos;

	for (int i = 0; i < videoHdr.noMonitors; i++)
	{
		if (PtInRect(&pMonitorDetails[i].MonitorDetails.virRect, pt))
		{
			hMon = i;
			break;
		}
	}

	return hMon;
}

BYTE *ChannelDataMapping::GetMonitorScreenBuffer(HMONITOR hMon)
{
	int monNum = 0;

	// Get Monitor
	for (monNum = 0; monNum < videoHdr.noMonitors; monNum++)
	{
		if (pMonitorDetails[monNum].MonitorDetails.MonitorNumber == hMon)
			break;
	}
	if (monNum == videoHdr.noMonitors)
	{
		// This is an error, can never happen
		return NULL;
	}
	return (BYTE *)pMonitorDetails[monNum].pMonitorBuffer->pBuffer;
}

bool ChannelDataMapping::GetNextAudio(struct IndexHdr *pIndex)
{
	long long fPosIndex = 0L;
	if (pIndex->RecType == 0) // Request for first record
	{
		fPosIndex = audioHdr.fPosRoot;
	}
	else
	{
		fPosIndex = pIndex->fPosNextRecord;
	}
	if (fPosIndex != 0L)
	{
		pDataFile->ReadFromFile((char *)pIndex, sizeof(struct IndexHdr), fPosIndex);
	}
	else
	{
		return true; // no more records
	}
	return false;
}

long long ChannelDataMapping::GetRootVideoPosition()
{
	return videoHdr.fPosRoot;
}

bool ChannelDataMapping::GetNextVideo(struct IndexHdr *pIndex, long long *posRead, map <long long, struct indexPageDescriptor *> *mapVideo)
{
	long long fPosIndex = *posRead;
	//if (pIndex->RecType == 0) // Request for first record
	//{
	//	fPosIndex = videoHdr.fPosRoot;
	
	//else
	//{
	//	fPosIndex = pIndex->fPosNextRecord;
	//}
	while (fPosIndex != 0LL)
	{
		pDataFile->ReadFromFile((char *)pIndex, sizeof(struct IndexHdr), fPosIndex);
		if (pIndex->RecType == 'P')
			break;
		if (pIndex->RecType == 'H')
		{

			fPosIndex = pIndex->fPosNextRecord;		// Perform a Channel Switch operation
		}
		else if (pIndex->RecType == 'B')
		{
			// Search for IndexDescriptor record for this branch, Check the branch direction
			// Then get the next branch record
			map <long long, struct indexPageDescriptor *>::iterator itMap = mapVideo->find(fPosIndex);
			if (itMap == mapVideo->end())
			{
				fPosIndex = 0LL;
				break;
			}
			struct indexPageDescriptor *pDescriptor = itMap->second;
			if (pDescriptor->nPageType != 2)
			{
				fPosIndex = 0LL;
				break;
			}
			struct branchIndexDescriptor *pBranch = pDescriptor->pageDescriptor.pBranch;
			if (pBranch->bPathDir == true)
			{
				fPosIndex = pIndex->dataRecs[0].fPosData;
			}
			else
			{
				fPosIndex = pIndex->fPosNextRecord;
			}

		}
		else if (pIndex->RecType == 'J')
		{
			fPosIndex = pIndex->fPosNextRecord;
		}
	}
	*posRead = fPosIndex;
	if (fPosIndex == 0LL)
	{
		return true; // no more records
	}
	return false;
}



bool ChannelDataMapping::GetNextEvent(struct IndexHdr *pIndex)
{
	long long fPosIndex = 0L;
	if (pIndex->RecType == 0) // Request for first record
	{
		fPosIndex = eventHdr.fPosRoot;
	}
	else
	{
		fPosIndex = pIndex->fPosNextRecord;
	}
	if (fPosIndex != 0L)
	{
		pDataFile->ReadFromFile((char *)pIndex, sizeof(struct IndexHdr), fPosIndex);
	}
	else
	{
		return true; // no more records
	}
	return false;
}

void ChannelDataMapping::GetVideoIndex(long long pos, struct IndexHdr *indexHdr)
{
	pDataFile->ReadFromFile((char *)indexHdr, sizeof(struct IndexHdr), pos);
}
