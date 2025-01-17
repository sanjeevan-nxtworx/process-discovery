#pragma once

#include <vector>
#include <queue>
#include "MMHeader.h"
#include <mutex>
#include "PecanSemaphore.h"

using namespace std;


class RecordBufferManager
{
	list <struct PlayerChannelScreenBuffers *> channelScreenBuffers;
	list <struct PlayerResizeScreenBuffers *> channelResizeBuffers;

	vector <struct screenBuffer *> screenBufferList;
	mutex lockWriteBuffer;
	mutex lockAudioBuffer;
	mutex lockReadAudioBuffer;
	mutex lockResizeBuffer;
	mutex lockEventBuffer;
	mutex lockVideoBuffer;


	queue <struct bufferRecord *> writeBuffers;
	queue <struct bufferRecord *> audioBuffer;
	queue <struct bufferRecord *> readAudioBuffer;
	queue <struct bufferRecord *> readEventBuffer;
	queue <struct bufferRecord *> eventBuffer;
	queue <struct bufferRecord *> freePlayerScreenBuffers;

	queue <struct ResizeScreenBuffer *> resizeScreenBuffer;
	queue <struct ResizeScreenBuffer *> freeResizeScreenBuffer;

	time_t timeStampLastBuffer;
	unsigned fps;
	unsigned resizeWidth;
	unsigned resizeHeight;
	bool bResInitDone;
	unsigned thumbWidth;
	unsigned thumbHeight;
	PecanSemaphore writeBufferSemaphore;
	PecanSemaphore audioBufferSemaphore;

	ULONG resizeBufferSize;

	struct ResizeScreenBuffer *thumbScreenBuffer;
	struct ResizeScreenBuffer *currentThumbScreenBuffer;

public:
	char *GetChannelScreenBuffer(ULONG *size);
	void ReleaseChannelScreenBuffer(char *pBuffer, ULONG size);
	char *GetChannelResizeBuffer(ULONG *size);
	void ReleaseChannelResizeBuffer(char *pBuffer, ULONG size);
	unsigned GetResizeWidth();
	unsigned GetResizeHeight();
	bool ResizeInitDone();
	char *GetPlayerFreeScreenBuffer(struct ScreenData *screenPtr);
	void FreePlayerBuffer(struct bufferRecord *pRecBuffer);

	char *GetFreeScreenBuffer(struct ScreenData *screenPtr);
	struct ResizeScreenBuffer *GetFreeResizeScreenBuffer();
	struct ResizeScreenBuffer *GetThumbResizeScreenBuffer();
	bool IsFreeResizeScreenBufferAvailable();

	void CreateFreeResizeScreenBufferQueue(int width, int height, int framesPerSec);
	struct ThumbScreenBuffer *GetThumbBuffer(int numImage);
	void CreateVideoThumbList(int width, int height);

	void FreeResizeBuffer(struct ResizeScreenBuffer *pResScreenBuffer);
	bool ResizeScreenBufferFull();
	int GetDisplayResizeBufferSize();
	struct ResizeScreenBuffer *GetResizeBuffer();
	void DisplayResizeBuffer(struct ResizeScreenBuffer *pResScreenBuffer);
	void DisplayThumbResizeBuffer(struct ResizeScreenBuffer *pResScreenBuffer);
	void ClearThumbResizeBuffer();
	time_t GetTimeOfLastResizeBuffer();
	struct ResizeScreenBuffer *GetThumbResizeBuffer();

	char *GetFreeAudioBuffer(DWORD size);

	char *GetFreeEventBuffer();

	void WriteToFile(struct bufferRecord *pRecBuffer);
	bool IsWriteBufferAvailable();
	void SignalWriteThread();
	struct bufferRecord *GetWriteBuffer();
	void FreeBuffer(struct bufferRecord *pRecBuffer);
	void SaveInReadAudioBuffer(struct bufferRecord *pRecBuffer);
	void SaveInReadVideoBuffer(struct bufferRecord *pRecBuffer);
	void SaveInReadEventBuffer(struct bufferRecord *pRecBuffer);
	
	struct bufferRecord *GetAudioPlayRecord();
	struct bufferRecord *GetEventPlayRecord();
	//bool AudioPlayBufferFull();
	bool VideoPlayBufferFull();
	long ClearPlayBuffer();
	RecordBufferManager();
	~RecordBufferManager();
	void SetPlayerBMPDimensions(int width, int height);
};

