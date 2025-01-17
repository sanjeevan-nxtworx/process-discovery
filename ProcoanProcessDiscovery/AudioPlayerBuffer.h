#pragma once


class RecordBufferManager;

class AudioPlayerBuffer
{
	RecordBufferManager *pRecBufferManager;
	long long readTime;
	DWORD startPos;
	struct bufferRecord *pBufferRec;
	DWORD buffLength;
	DWORD buffReadLength;
	char *bufferStart;
	int lengthToWrite;
	long long overflow;

public:
	AudioPlayerBuffer();
	~AudioPlayerBuffer();
	void SetRecordBufferManager(RecordBufferManager *pBufferManager);
	void VideoPlayedTime(long long timeVal);
	bool GetAudioBuffer();
	void SetBufferMaxSize(DWORD buffSize);
	char *GetBuffer();
	void FreeAudioBuffer();
	int GetBufferLength();
};

