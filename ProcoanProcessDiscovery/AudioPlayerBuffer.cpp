#include "stdafx.h"
#include "AudioPlayerBuffer.h"
#include "RecordBufferManager.h"
#include <mutex>

using namespace std;

static mutex lockAudioBuffer;
#define NUMPARTS 10
#define TIMEMAX 100000L

AudioPlayerBuffer::AudioPlayerBuffer()
{
	pRecBufferManager = NULL;
	readTime = 0L;
	startPos = 0L;
	pBufferRec = NULL;
	buffLength = 0;
	bufferStart = NULL;
	lengthToWrite = 0;
	buffReadLength = 0;
	overflow = TIMEMAX * 2;
}


AudioPlayerBuffer::~AudioPlayerBuffer()
{
}

void AudioPlayerBuffer::SetBufferMaxSize(DWORD sizeBuffer)
{
	buffLength = sizeBuffer;
}

bool AudioPlayerBuffer::GetAudioBuffer()
{
	pBufferRec = pRecBufferManager->GetAudioPlayRecord();
	if (!pBufferRec)
		return false;
	lengthToWrite = pBufferRec->unBuffDets.audRec.size;
	return true;
}

void AudioPlayerBuffer::FreeAudioBuffer()
{
	if (pBufferRec != NULL)
		pRecBufferManager->FreePlayerBuffer(pBufferRec);
}

char *AudioPlayerBuffer::GetBuffer()
{
	return pBufferRec->buffer;
}

int AudioPlayerBuffer::GetBufferLength()
{
	return lengthToWrite;
}

void AudioPlayerBuffer::VideoPlayedTime(long long timeVal)
{
	lockAudioBuffer.lock();
	readTime += timeVal;
	lockAudioBuffer.unlock();
}

void AudioPlayerBuffer::SetRecordBufferManager(RecordBufferManager *pBufferManager)
{
	pRecBufferManager = pBufferManager;
}
