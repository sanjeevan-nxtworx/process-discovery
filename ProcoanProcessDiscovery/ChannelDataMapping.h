#pragma once

#include "MMHeader.h"
#include <map>
#include <list>
#include <vector>
#include "DataFile.h"
#include "BitmapMerge.h"
#include <wx/wx.h>

class RecordBufferManager;
class FileContext;

class ChannelDataMapping
{
	struct channelHdr channelHeader;
	struct VideoHdr videoHdr;
	struct AudioHdr audioHdr;
	struct EventHdr eventHdr;
	DataFile *pDataFile;
	struct MonitorScreenData *pMonitorDetails;
	vector <struct ScreenData *> screenData;
	struct IndexHdr audioIndex;
	struct IndexHdr videoIndex;
	struct IndexHdr eventIndex;
	RecordBufferManager *pRecBufferManager;
	FileContext *pFileContext;

	BITMAPINFOHEADER bmInfoSrc;
	BITMAPINFOHEADER bmInfoDest;
	wxString channelKey;

	BitmapMerge *pBitmapMerge;

	void GetMonitorDetails(int noMonitors, long long fPosMonitorDetails);
	void DrawRectangleInEventBitmap(LPSTR pBuffer, int monNum, RECT r);
	void DrawHorizLine(char *pBuffer, int width, int height, int leftCol, int row, int rightCol);
	void DrawVertLine(char *pBuffer, int width, int height, int col, int topRow, int botRow);
	void ReadVideoData(bool bMainPath, map <long long, struct indexPageDescriptor *> *mapVideo, long long fPos);

public:
	ChannelDataMapping();
	~ChannelDataMapping();
	void SetupThumbBuffers();
	void ClearThumbBuffers();
	void SetFileContext(FileContext *pContext);
	void GetChannelMonitorInfo(struct MonitorHdr *pMonHeaders);
	void AddChannelMonitorInfo(struct MonitorHdr *pMonHeaders);
	void AddVideoHeader();
	void AddAudioHeader();
	void SetDataFile(DataFile *pFile);
	void SetRecordBufferManager(RecordBufferManager *pRecBuffManager);
	int GetNumMonitors();
	USHORT ReadChannelHeader(long long channelFpos);
	int ProcessVideoRecord(struct bufferRecord *pBuffer, int lastMon);
	void DrawCursorInBitmap(BitmapMerge *pBmpMerge, struct ResizeScreenBuffer *pResizeBuffer, short xPos, short yPos);
	void DrawRectanglesInBitmap(BitmapMerge *pBmpMerge, struct ResizeScreenBuffer *pResizeBuffer, RECT boundingRectControl);
	void CreateBitmapMerge(bool bActiveMonitorDisplay, int windowWidth, int windowHeight);
	void UpdateBitmapMergeWindowDimensions(bool bActiveMonitorDisplay, int windowWidth, int windowHeight);
	struct channelHdr *GetChannelHeader();
	void ReadAudioData(map <long long, struct indexPageDescriptor *> *mapAudio);
	void ReadVideoData(bool bMainPath, map <long long, struct indexPageDescriptor *> *mapVideo);
	long long GetNextChannelPosition();
	bool GetNextAudio(struct IndexHdr *pIndex);
	bool GetNextEvent(struct IndexHdr *pIndex);
	bool GetNextVideo(struct IndexHdr *pIndex, long long *posRead, map <long long, struct indexPageDescriptor *> *mapVideo);
	void GetAudioHeader(struct AudioHdr *pHeader);
	void GetVideoHeader(struct VideoHdr *pHeader);
	void WriteVideoHeader(struct VideoHdr *pHeader);
	long long GetRootVideoPosition();
	void WriteChannelHeader(long long fPos);
	struct VideoHdr *GetVideoHeader();
	struct AudioHdr *GetAudioHeader();
	DWORD GetAudioBufferSize();
	void GetVideoIndex(long long pos, struct IndexHdr *indexHdr);
	struct ScreenData *GetScreenData(HMONITOR hMon);
	HMONITOR GetScreenNumber(int xPos, int yPos);
	int GetScreenSubscript(int xPos, int yPos);

	BYTE *GetMonitorScreenBuffer(HMONITOR hMon);
	struct ScreenData *GetScreenData(int screenNum);
	void ResizeVideoImage(struct bufferRecord *pBuffer, int monNum);
	void MergePrintImage(int monNum, vector<unsigned char>& buffer, ULONG& cx, ULONG& cy, ULONG nEventIndex, vector <BPMNElement *> &pElementList);

	void GetEventImage(int monNum, vector<unsigned char>& buffer, long& cx, long& cy, ULONG nEventIndex, vector <BPMNElement *> &pElementList);
	void ResizeThumbVideoImage(struct bufferRecord *pBuffer, int monNum);
	struct bufferRecord *ProcessMonitorScreens(int *nMon);
	struct bufferRecord *ProcessMonitorScreensForThumbs(int *nMon, struct bufferRecord *pBuffer);
	void SetScreenBuffers();
	void ClearScreenBuffers();
	wxString GetChannelKey();
	void CreateScreenBuffers();
};

