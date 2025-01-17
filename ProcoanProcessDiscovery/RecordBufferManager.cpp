#include "stdafx.h"
#include "RecordBufferManager.h"

RecordBufferManager::RecordBufferManager()
{
	fps = 0;
	resizeWidth = resizeHeight = 0;
	bResInitDone = false;
	timeStampLastBuffer = 0L;
	fps = 0;
	resizeBufferSize = 0L;
	bResInitDone = false;
	thumbWidth = thumbHeight = 0;
	thumbScreenBuffer = NULL;
	currentThumbScreenBuffer = NULL;
}

unsigned RecordBufferManager::GetResizeWidth()
{
	return resizeWidth;
}

unsigned RecordBufferManager::GetResizeHeight()
{
	return resizeHeight;
}

RecordBufferManager::~RecordBufferManager()
{
	ClearPlayBuffer();
	//list <struct PlayerChannelScreenBuffers *> channelScreenBuffers;

	list <struct PlayerChannelScreenBuffers *>::iterator itScreenList = channelScreenBuffers.begin();

	while (itScreenList != channelScreenBuffers.end())
	{
		struct PlayerChannelScreenBuffers *ptrVal = *itScreenList;
		if (ptrVal->pBuffer != NULL)
			HeapFree(GetProcessHeap(), 0, ptrVal->pBuffer);
		delete ptrVal;
		itScreenList++;
	}

	//list <struct PlayerResizeScreenBuffers *> channelResizeBuffers;
	list <struct PlayerResizeScreenBuffers *>::iterator itchannelResizeList = channelResizeBuffers.begin();

	while (itchannelResizeList != channelResizeBuffers.end())
	{
		struct PlayerResizeScreenBuffers *ptrVal = *itchannelResizeList;
		if (ptrVal != NULL)
		{
			if (ptrVal->pBuffer != NULL)
			{
				HeapFree(GetProcessHeap(), 0, ptrVal->pBuffer);
			}
			delete ptrVal;
		}
		
		itchannelResizeList++;
	}

	//vector <struct screenBuffer *> screenBufferList;
	//queue <struct bufferRecord *> writeBuffers;
	//queue <struct bufferRecord *> audioBuffer;
	while (audioBuffer.size() > 0)
	{
		struct bufferRecord *pBuffer = NULL;
		pBuffer = audioBuffer.front();
		audioBuffer.pop();
		HeapFree(GetProcessHeap(), 0, pBuffer);
	}

	//queue <struct bufferRecord *> readAudioBuffer;
	//queue <struct bufferRecord *> readEventBuffer;
	//queue <struct bufferRecord *> eventBuffer;
	while (eventBuffer.size() > 0)
	{
		struct bufferRecord *pBuffer = NULL;
		pBuffer = eventBuffer.front();
		eventBuffer.pop();
		HeapFree(GetProcessHeap(), 0, pBuffer);
	}

	//queue <struct bufferRecord *> freePlayerScreenBuffers;
	while (freePlayerScreenBuffers.size() > 0)
	{
		struct bufferRecord *pBuffer = NULL;
		pBuffer = freePlayerScreenBuffers.front();
		freePlayerScreenBuffers.pop();
		HeapFree(GetProcessHeap(), 0, pBuffer);
	}

	//queue <struct ResizeScreenBuffer *> resizeScreenBuffer;
	//queue <struct ResizeScreenBuffer *> freeResizeScreenBuffer;
	while (freeResizeScreenBuffer.size() > 0)
	{
		struct ResizeScreenBuffer *pResizeBuffer = NULL;
		pResizeBuffer = freeResizeScreenBuffer.front();
		freeResizeScreenBuffer.pop();
		char *pBuffer = pResizeBuffer->buffer;
		if (pBuffer != NULL)
			HeapFree(GetProcessHeap(), 0, pBuffer);
		delete pResizeBuffer;
	}

	if (thumbScreenBuffer != NULL)
	{
		if (thumbScreenBuffer->buffer != NULL)
			HeapFree(GetProcessHeap(), 0, thumbScreenBuffer->buffer);
		thumbScreenBuffer->buffer = NULL;
		delete thumbScreenBuffer;
		thumbScreenBuffer = NULL;
	}

	// Called by recorder
	//struct screenBuffer *screenBuffer = NULL;
	//bool bFound = false;

	for (std::vector<struct screenBuffer *>::iterator it = screenBufferList.begin(); it != screenBufferList.end(); ++it)
	{
		struct screenBuffer *screenBuffPtr = (struct screenBuffer *) *it;

		while (screenBuffPtr->freeBuffers.size() > 0)
		{
			struct bufferRecord *pBuffer = screenBuffPtr->freeBuffers.front();
			screenBuffPtr->freeBuffers.pop();
			if (pBuffer != NULL)
				HeapFree(GetProcessHeap(), 0, pBuffer);
		}
		delete screenBuffPtr;
	}
	screenBufferList.clear();
}

char *RecordBufferManager::GetChannelScreenBuffer(ULONG *size)
{
	list <struct PlayerChannelScreenBuffers *>::iterator itScreenBuffers = channelScreenBuffers.begin();

	char *retBuffValue = NULL;
	if (itScreenBuffers != channelScreenBuffers.end())
	{
		struct PlayerChannelScreenBuffers *pScreenBuffer = channelScreenBuffers.front();
		channelScreenBuffers.pop_front();
		if (pScreenBuffer->size < *size)
		{
			pScreenBuffer->pBuffer = (char *)HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, pScreenBuffer->pBuffer, *size);
		}
		retBuffValue = pScreenBuffer->pBuffer;
		*size = pScreenBuffer->size;
		delete pScreenBuffer;
		return retBuffValue;
	}

	retBuffValue = (char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, *size);
	return retBuffValue;
}

void RecordBufferManager::ReleaseChannelScreenBuffer(char *pBuffer, ULONG size)
{
	struct PlayerChannelScreenBuffers *pScreenBuffer = DBG_NEW struct PlayerChannelScreenBuffers;
	pScreenBuffer->size = size;
	pScreenBuffer->pBuffer = pBuffer;
	channelScreenBuffers.push_back(pScreenBuffer);
}

char *RecordBufferManager::GetChannelResizeBuffer(ULONG *size)
{
	list <struct PlayerResizeScreenBuffers *>::iterator itResizeBuffers = channelResizeBuffers.begin();

	char *retBuffValue = NULL;
	if (itResizeBuffers != channelResizeBuffers.end())
	{
		struct PlayerResizeScreenBuffers *pResizeBuffer = channelResizeBuffers.front();
		channelResizeBuffers.pop_front();
		if (pResizeBuffer->size < *size)
		{
			HeapFree(GetProcessHeap(), 0, pResizeBuffer->pBuffer);
			pResizeBuffer->pBuffer = NULL;
			pResizeBuffer->pBuffer = (char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, *size);
		}
		retBuffValue = pResizeBuffer->pBuffer;
		*size = pResizeBuffer->size;
		delete pResizeBuffer;
		return retBuffValue;
	}

	retBuffValue = (char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, *size);
	return retBuffValue;
}

void RecordBufferManager::ReleaseChannelResizeBuffer(char *pBuffer, ULONG size)
{
	struct PlayerResizeScreenBuffers *pResizeBuffer = DBG_NEW struct PlayerResizeScreenBuffers;
	pResizeBuffer->size = size;
	pResizeBuffer->pBuffer = pBuffer;
	channelResizeBuffers.push_back(pResizeBuffer);
}

void RecordBufferManager::WriteToFile(struct bufferRecord *pRecBuffer)
{
	lockWriteBuffer.lock();
	writeBuffers.push(pRecBuffer);
	writeBufferSemaphore.notify();
	lockWriteBuffer.unlock();

}

bool RecordBufferManager::ResizeInitDone()
{
	bool bResult = false;
	lockResizeBuffer.lock();
	bResult = bResInitDone;
	lockResizeBuffer.unlock();
	return bResult;
}

//void RecordBufferManager::CreateVideoThumbList(int width, int height)
//{
//	if (thumbScreenBufferList.size() != 0)
//		return;
//	lockResizeBuffer.lock();
//	for (int i = 0; i < 100; i++)
//	{
//		struct ThumbScreenBuffer *pThumbBuffer = new struct ThumbScreenBuffer;
//		pThumbBuffer->buffer = (char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, width * height * 4);
//		thumbScreenBufferList.push_back(pThumbBuffer);
//	}
//	
//	thumbWidth = width;
//	thumbHeight = height;
//	lockResizeBuffer.unlock();
//}

//struct ThumbScreenBuffer *RecordBufferManager::GetThumbBuffer(int numImage)
//{
//	if (numImage >= 0 && numImage < 100)
//		return thumbScreenBufferList[numImage];
//	return NULL;
//}

void RecordBufferManager::CreateFreeResizeScreenBufferQueue(int width, int height, int framesPerSec)
{
	lockResizeBuffer.lock();
	if (resizeBufferSize == 0L)
	{
		resizeBufferSize = 4L * width * height;

		for (int i = 0; i < framesPerSec; i++)
		{
			struct ResizeScreenBuffer *pResBuffer = DBG_NEW struct ResizeScreenBuffer;
			pResBuffer->t1FileStart = 0L;
			pResBuffer->bufferSize = resizeBufferSize;
			pResBuffer->width = (short)width;
			pResBuffer->height = (short)height;
			pResBuffer->buffer = (char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (SIZE_T)pResBuffer->bufferSize);
			freeResizeScreenBuffer.push(pResBuffer);
		}
	}
	else
	{
		ULONG newResizeBufferSize = 4L * width * height;
		if (newResizeBufferSize != resizeBufferSize)
		{
			for (int i = 0; i < framesPerSec; i++)
			{
				struct ResizeScreenBuffer *pResBuffer = freeResizeScreenBuffer.front();
				freeResizeScreenBuffer.pop();
				pResBuffer->t1FileStart = 0L;
				pResBuffer->bufferSize = resizeBufferSize;
				pResBuffer->width = (short)width;
				pResBuffer->height = (short)height;
				if (newResizeBufferSize > resizeBufferSize)
				{
					pResBuffer->bufferSize = newResizeBufferSize;
					pResBuffer->buffer = (char *)HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, pResBuffer->buffer, (SIZE_T)pResBuffer->bufferSize);
				}
				freeResizeScreenBuffer.push(pResBuffer);
			}
			if (newResizeBufferSize > resizeBufferSize)
				resizeBufferSize = newResizeBufferSize;
		}
	}
	fps = framesPerSec;
	resizeWidth = width;
	resizeHeight = height;
	bResInitDone = true;
	lockResizeBuffer.unlock();
}

void RecordBufferManager::SetPlayerBMPDimensions(int width, int height)
{
	resizeWidth = width;
	resizeHeight = height;
}

bool RecordBufferManager::IsFreeResizeScreenBufferAvailable()
{
	bool bResizeBufferAvailable = false;

	lockResizeBuffer.lock();
	if (freeResizeScreenBuffer.size() > 0)
	{
		bResizeBufferAvailable = true;
	}
	lockResizeBuffer.unlock();
	return bResizeBufferAvailable;

}

struct ResizeScreenBuffer *RecordBufferManager::GetFreeResizeScreenBuffer()
{
	struct ResizeScreenBuffer *pResizeBuffer = NULL;

	lockResizeBuffer.lock();
	if (freeResizeScreenBuffer.size() > 0)
	{
		pResizeBuffer = freeResizeScreenBuffer.front();
		freeResizeScreenBuffer.pop();
	}
	lockResizeBuffer.unlock();
	return pResizeBuffer;
}


struct ResizeScreenBuffer *RecordBufferManager::GetThumbResizeScreenBuffer()
{
	lockResizeBuffer.lock();
	if (thumbScreenBuffer == NULL)
	{
		thumbScreenBuffer = DBG_NEW struct ResizeScreenBuffer;
		thumbScreenBuffer->t1FileStart = 0L;
		thumbScreenBuffer->bufferSize = 4L * 220 * 170;
		thumbScreenBuffer->width = 200;
		thumbScreenBuffer->height = 150;
		thumbScreenBuffer->buffer = (char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (SIZE_T)thumbScreenBuffer->bufferSize);
	}

	lockResizeBuffer.unlock();
	return thumbScreenBuffer;
}




void RecordBufferManager::FreeResizeBuffer(struct ResizeScreenBuffer *pResScreenBuffer)
{
	lockResizeBuffer.lock();
	freeResizeScreenBuffer.push(pResScreenBuffer);
	lockResizeBuffer.unlock();
}

bool RecordBufferManager::ResizeScreenBufferFull()
{
	bool result = false;
	lockResizeBuffer.lock();
	if (resizeScreenBuffer.size() >= fps)
	{
		result = true;
	}
	lockResizeBuffer.unlock();
	return result;
}

int RecordBufferManager::GetDisplayResizeBufferSize()
{
	int sizeBuffer = 0;
	lockResizeBuffer.lock();
	sizeBuffer = resizeScreenBuffer.size();
	lockResizeBuffer.unlock();
	return sizeBuffer;
}

struct ResizeScreenBuffer *RecordBufferManager::GetResizeBuffer()
{
	struct ResizeScreenBuffer *pRecBuffer = NULL;

	lockResizeBuffer.lock();

	if (!resizeScreenBuffer.empty())
	{
		pRecBuffer = resizeScreenBuffer.front();
		resizeScreenBuffer.pop();
	}
	lockResizeBuffer.unlock();
	return pRecBuffer;
}

void RecordBufferManager::DisplayResizeBuffer(struct ResizeScreenBuffer *pResScreenBuffer)
{
	lockResizeBuffer.lock();
	resizeScreenBuffer.push(pResScreenBuffer);
	timeStampLastBuffer = pResScreenBuffer->t1FileStart;
	lockResizeBuffer.unlock();
}

void RecordBufferManager::ClearThumbResizeBuffer()
{
	lockResizeBuffer.lock();
	currentThumbScreenBuffer = NULL;
	lockResizeBuffer.unlock();
}

struct ResizeScreenBuffer *RecordBufferManager::GetThumbResizeBuffer()
{
	return currentThumbScreenBuffer;
}

void RecordBufferManager::DisplayThumbResizeBuffer(struct ResizeScreenBuffer *pResScreenBuffer)
{
	lockResizeBuffer.lock();
	currentThumbScreenBuffer = thumbScreenBuffer;
	timeStampLastBuffer = pResScreenBuffer->t1FileStart;
	lockResizeBuffer.unlock();
}

time_t RecordBufferManager::GetTimeOfLastResizeBuffer()
{
	time_t timeStamp;

	lockResizeBuffer.lock();
	timeStamp = timeStampLastBuffer;
	lockResizeBuffer.unlock();

	return timeStamp;
}

bool RecordBufferManager::IsWriteBufferAvailable()
{
	writeBufferSemaphore.wait();
	return true;
}

void RecordBufferManager::SignalWriteThread()
{
	writeBufferSemaphore.notify();
}

struct bufferRecord *RecordBufferManager::GetWriteBuffer()
{
	struct bufferRecord *pRecBuffer = NULL;

	lockWriteBuffer.lock();

	if (!writeBuffers.empty())
	{
		pRecBuffer = writeBuffers.front();
		writeBuffers.pop();
	}
	lockWriteBuffer.unlock();
	return pRecBuffer;
}

void RecordBufferManager::FreeBuffer(struct bufferRecord *pRecBuffer)
{
	if (pRecBuffer->buffType == 1)	// Video Record
	{
		struct screenBuffer *screenBuffer = NULL;
		bool bFound = false;
		for (std::vector<struct screenBuffer *>::iterator it = screenBufferList.begin(); it != screenBufferList.end(); ++it)
		{
			struct screenBuffer *screenBuffPtr = (struct screenBuffer *) *it;

			if (screenBuffPtr->hMon == pRecBuffer->unBuffDets.scrRec.hMon)
			{
				bFound = true;
				screenBuffer = screenBuffPtr;
				break;
			}
		}

		if (!bFound)
		{
			//Should never happen
			HeapFree(GetProcessHeap(), 0, pRecBuffer);
			return;
		}

		screenBuffer->lockFreeBuffer.lock();
		screenBuffer->freeBuffers.push(pRecBuffer);
		screenBuffer->lockFreeBuffer.unlock();
	}
	else if (pRecBuffer->buffType == 2)
	{
		lockAudioBuffer.lock();
		audioBuffer.push(pRecBuffer);
		lockAudioBuffer.unlock();
	}
	else if (pRecBuffer->buffType == 3)
	{
		lockEventBuffer.lock();
		eventBuffer.push(pRecBuffer);
		lockEventBuffer.unlock();
	}
}

void RecordBufferManager::FreePlayerBuffer(struct bufferRecord *pRecBuffer)
{
	if (pRecBuffer->buffType == 1)	// Video Record
	{
		lockVideoBuffer.lock();
		freePlayerScreenBuffers.push(pRecBuffer);
		lockVideoBuffer.unlock();
	}
	else if (pRecBuffer->buffType == 2)
	{
		lockAudioBuffer.lock();
		audioBuffer.push(pRecBuffer);
		lockAudioBuffer.unlock();
	}
	else if (pRecBuffer->buffType == 3)
	{
		lockEventBuffer.lock();
		eventBuffer.push(pRecBuffer);
		lockEventBuffer.unlock();
	}
}


void RecordBufferManager::SaveInReadEventBuffer(struct bufferRecord *pRecBuffer)
{
	lockEventBuffer.lock();
	readEventBuffer.push(pRecBuffer);
	lockEventBuffer.unlock();
}

void RecordBufferManager::SaveInReadAudioBuffer(struct bufferRecord *pRecBuffer)
{
	lockReadAudioBuffer.lock();
	readAudioBuffer.push(pRecBuffer);
	audioBufferSemaphore.notify();
	lockReadAudioBuffer.unlock();
}

void RecordBufferManager::SaveInReadVideoBuffer(struct bufferRecord *pRecBuffer)
{
	lockWriteBuffer.lock();
	writeBuffers.push(pRecBuffer);
	lockWriteBuffer.unlock();
}

//bool RecordBufferManager::AudioPlayBufferFull()
//{
//	bool retVal = false;
//
//	lockReadAudioBuffer.lock();
//	if (readAudioBuffer.size() >= 5)
//		retVal = true;
//	lockReadAudioBuffer.unlock();
//	return retVal;
//}

long RecordBufferManager::ClearPlayBuffer()
{
	long timeInBuffer = 0L;
	struct bufferRecord *pBuffer = NULL;
	lockReadAudioBuffer.lock();
	while (!readAudioBuffer.empty())
	{
		pBuffer = readAudioBuffer.front();
		readAudioBuffer.pop();
		audioBuffer.push(pBuffer);
	}
	lockReadAudioBuffer.unlock();

	lockEventBuffer.lock();
	while (!readEventBuffer.empty())
	{
		pBuffer = readEventBuffer.front();
		readEventBuffer.pop();
		eventBuffer.push(pBuffer);
	}
	lockEventBuffer.unlock();

	lockResizeBuffer.lock();
	struct ResizeScreenBuffer *pRecBuffer = NULL;
	while (!resizeScreenBuffer.empty())
	{
		pRecBuffer = resizeScreenBuffer.front();
		resizeScreenBuffer.pop();
		timeInBuffer = timeInBuffer + (long)pRecBuffer->recDurationus;
		freeResizeScreenBuffer.push(pRecBuffer);
	}
	lockResizeBuffer.unlock();

	lockWriteBuffer.lock();
	while (!writeBuffers.empty())
	{
		pBuffer = writeBuffers.front();
		writeBuffers.pop();
		timeInBuffer = timeInBuffer + (long)pBuffer->recDurationus;
		FreePlayerBuffer(pBuffer);

	}
	lockWriteBuffer.unlock();

	return timeInBuffer;
}


bool RecordBufferManager::VideoPlayBufferFull()
{
	bool retVal = false;

	lockWriteBuffer.lock();
	if (writeBuffers.size() >= 5)
		retVal = true;
	lockWriteBuffer.unlock();
	return retVal;
}

struct bufferRecord *RecordBufferManager::GetAudioPlayRecord()
{
	struct bufferRecord *pBuffer = NULL;
	if (!audioBufferSemaphore.try_wait())
		return NULL;
	lockReadAudioBuffer.lock();
	if (!readAudioBuffer.empty())
	{
		pBuffer = readAudioBuffer.front();
		readAudioBuffer.pop();
	}
	lockReadAudioBuffer.unlock();
	return pBuffer;
}

struct bufferRecord *RecordBufferManager::GetEventPlayRecord()
{
	struct bufferRecord *pBuffer = NULL;

	lockEventBuffer.lock();
	if (!readEventBuffer.empty())
	{
		pBuffer = readEventBuffer.front();
		readEventBuffer.pop();
	}
	lockEventBuffer.unlock();
	return pBuffer;
}

//char *RecordBufferManager::GetPlayerFreeScreenBuffer(struct ScreenData *screenPtr)
//{
//	// Called by Player
//	int width, height;
//
//	width = screenPtr->rcMonitor.right - screenPtr->rcMonitor.left;
//	height = screenPtr->rcMonitor.bottom - screenPtr->rcMonitor.top;
//	lockVideoBuffer.lock();
//	DWORD lineSize = (width * 4) / BLOCKSIZE;
//	DWORD compSizeBuffer = ((lineSize + 1) * (height + 1) * sizeof(struct blockDetails)) + sizeof(struct CompareBuffer);
//	DWORD totalBuffSize = compSizeBuffer + sizeof(struct bufferRecord) + 16;
//	struct bufferRecord *pBufferRecord = NULL;
//	if (freePlayerScreenBuffers.empty())
//	{
//		pBufferRecord = (struct bufferRecord *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, totalBuffSize);
//		pBufferRecord->buffType = 1; // Video Record
//		pBufferRecord->sizeOfBuffer = totalBuffSize;
//		pBufferRecord->buffer = (char *)pBufferRecord + sizeof(struct bufferRecord) + 8;
//		pBufferRecord->unBuffDets.scrRec.hMon = screenPtr->hMonitor;
//		pBufferRecord->unBuffDets.scrRec.size = compSizeBuffer;
//		freePlayerScreenBuffers.push(pBufferRecord);
//	}
//
//	pBufferRecord = freePlayerScreenBuffers.front();
//	freePlayerScreenBuffers.pop();
//
//	if (pBufferRecord->sizeOfBuffer < totalBuffSize)
//	{
//		// The free buffer size is less than the size of the required buffer
//		// Then reallocate and reassign the data 
//		pBufferRecord = (struct bufferRecord *)HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, pBufferRecord, totalBuffSize);
//		pBufferRecord->buffType = 1; // Video Record
//		pBufferRecord->sizeOfBuffer = totalBuffSize;
//		pBufferRecord->buffer = (char *)pBufferRecord + sizeof(struct bufferRecord) + 8;
//		pBufferRecord->unBuffDets.scrRec.hMon = screenPtr->hMonitor;
//		pBufferRecord->unBuffDets.scrRec.size = compSizeBuffer;
//	}
//
//	char *retBuff = (char *)pBufferRecord;
//	lockVideoBuffer.unlock();
//	return retBuff;
//}


char *RecordBufferManager::GetPlayerFreeScreenBuffer(struct ScreenData *screenPtr)
{
	// Called by Player
	int width, height;

	width = screenPtr->rcMonitor.right - screenPtr->rcMonitor.left;
	height = screenPtr->rcMonitor.bottom - screenPtr->rcMonitor.top;
	lockVideoBuffer.lock();
	DWORD compSizeBuffer =  ((DWORD)width * (DWORD)height * 4L) + sizeof(struct CompareBuffer);
	DWORD totalBuffSize = compSizeBuffer + sizeof(struct bufferRecord) + 16L;
	struct bufferRecord *pBufferRecord = NULL;
	if (freePlayerScreenBuffers.empty())
	{
		pBufferRecord = (struct bufferRecord *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, totalBuffSize);
		pBufferRecord->buffType = 1; // Video Record
		pBufferRecord->sizeOfBuffer = totalBuffSize;
		pBufferRecord->buffer = (char *)pBufferRecord + sizeof(struct bufferRecord) + 8;
		pBufferRecord->unBuffDets.scrRec.hMon = screenPtr->hMonitor;
		pBufferRecord->unBuffDets.scrRec.size = compSizeBuffer;
		freePlayerScreenBuffers.push(pBufferRecord);
	}

	pBufferRecord = freePlayerScreenBuffers.front();
	freePlayerScreenBuffers.pop();

	if (pBufferRecord->sizeOfBuffer < totalBuffSize)
	{
		// The free buffer size is less than the size of the required buffer
		// Then reallocate and reassign the data 
		pBufferRecord = (struct bufferRecord *)HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, pBufferRecord, totalBuffSize);
		pBufferRecord->buffType = 1; // Video Record
		pBufferRecord->sizeOfBuffer = totalBuffSize;
		pBufferRecord->buffer = (char *)pBufferRecord + sizeof(struct bufferRecord) + 8;
		pBufferRecord->unBuffDets.scrRec.hMon = screenPtr->hMonitor;
		pBufferRecord->unBuffDets.scrRec.size = compSizeBuffer;
	}

	char *retBuff = (char *)pBufferRecord;
	lockVideoBuffer.unlock();
	return retBuff;
}

//char *RecordBufferManager::GetFreeScreenBuffer(struct ScreenData *screenPtr)
//{
//	// Called by recorder
//	struct screenBuffer *screenBuffer = NULL;
//	bool bFound = false;
//
//	for (std::vector<struct screenBuffer *>::iterator it = screenBufferList.begin(); it != screenBufferList.end(); ++it)
//	{
//		struct screenBuffer *screenBuffPtr = (struct screenBuffer *) *it;
//		
//		if (screenBuffPtr->hMon == screenPtr->hMonitor)
//		{
//			bFound = true;
//			//plogFile->WriteLibLog((char *)"ALERT: Found Free Screen Buffer");
//			screenBuffer = screenBuffPtr;
//			break;
//		}
//	}
//
//	if (!bFound)
//	{
//		//plogFile->WriteLibLog((char *)"ALERT: Creating DBG_NEW screen buffer");
//		screenBuffer = DBG_NEW struct screenBuffer;
//		screenBuffer->hMon = screenPtr->hMonitor;
//		screenBufferList.push_back(screenBuffer);
//	}
//	int width, height;
//
//	width = screenPtr->rcMonitor.right - screenPtr->rcMonitor.left;
//	height = screenPtr->rcMonitor.bottom - screenPtr->rcMonitor.top;
//
//	screenBuffer->lockFreeBuffer.lock();
//	DWORD compSizeBuffer = ((DWORD)width * (DWORD)height * 4L) + sizeof(struct CompareBuffer);
//	DWORD totalBuffSize = compSizeBuffer + sizeof(struct bufferRecord) + 16;
//	struct bufferRecord *pBufferRecord = NULL;
//	if (screenBuffer->freeBuffers.empty())
//	{
//		pBufferRecord = (struct bufferRecord *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, totalBuffSize);
//		pBufferRecord->buffType = 1; // Video Record
//		pBufferRecord->sizeOfBuffer = totalBuffSize;
//		pBufferRecord->buffer = (char *)pBufferRecord + sizeof(struct bufferRecord) + 8;
//		pBufferRecord->unBuffDets.scrRec.hMon = screenPtr->hMonitor;
//		pBufferRecord->unBuffDets.scrRec.size = compSizeBuffer;
//		screenBuffer->freeBuffers.push(pBufferRecord);
//	}
//
//	pBufferRecord = screenBuffer->freeBuffers.front();
//	screenBuffer->freeBuffers.pop();
//	char *retBuff = (char *)pBufferRecord;
//	if (pBufferRecord->sizeOfBuffer != totalBuffSize)
//	{
//		pBufferRecord = (struct bufferRecord *)HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, pBufferRecord, totalBuffSize);
//		pBufferRecord->buffType = 1; // Video Record
//		pBufferRecord->sizeOfBuffer = totalBuffSize;
//		pBufferRecord->buffer = (char *)pBufferRecord + sizeof(struct bufferRecord) + 8;
//		pBufferRecord->unBuffDets.scrRec.hMon = screenPtr->hMonitor;
//		pBufferRecord->unBuffDets.scrRec.size = compSizeBuffer;
//		retBuff = (char *)pBufferRecord;
//	}
//	screenBuffer->lockFreeBuffer.unlock();
//	return retBuff;
//}


char *RecordBufferManager::GetFreeScreenBuffer(struct ScreenData *screenPtr)
{
	// Called by recorder
	struct screenBuffer *screenBuffer = NULL;
	bool bFound = false;

	for (std::vector<struct screenBuffer *>::iterator it = screenBufferList.begin(); it != screenBufferList.end(); ++it)
	{
		struct screenBuffer *screenBuffPtr = (struct screenBuffer *) *it;

		if (screenBuffPtr->hMon == screenPtr->hMonitor)
		{
			bFound = true;
			//plogFile->WriteLibLog((char *)"ALERT: Found Free Screen Buffer");
			screenBuffer = screenBuffPtr;
			break;
		}
	}

	if (!bFound)
	{
		//plogFile->WriteLibLog((char *)"ALERT: Creating DBG_NEW screen buffer");
		screenBuffer = DBG_NEW struct screenBuffer;
		screenBuffer->hMon = screenPtr->hMonitor;
		screenBufferList.push_back(screenBuffer);
	}
	int width, height;

	width = screenPtr->rcMonitor.right - screenPtr->rcMonitor.left;
	height = screenPtr->rcMonitor.bottom - screenPtr->rcMonitor.top;

	screenBuffer->lockFreeBuffer.lock();
	bool bIsBufferEmpty = screenBuffer->freeBuffers.empty();
	screenBuffer->lockFreeBuffer.unlock();
	DWORD compSizeBuffer = ((DWORD)width * (DWORD)height * 4L) + sizeof(struct CompareBuffer);
	DWORD totalBuffSize = compSizeBuffer + sizeof(struct bufferRecord) + 16;
	struct bufferRecord *pBufferRecord = NULL;
	char *retBuff = NULL;
	if (bIsBufferEmpty)
	{
		pBufferRecord = (struct bufferRecord *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, totalBuffSize);
		pBufferRecord->buffType = 1; // Video Record
		pBufferRecord->sizeOfBuffer = totalBuffSize;
		pBufferRecord->buffer = (char *)pBufferRecord + sizeof(struct bufferRecord) + 8;
		pBufferRecord->unBuffDets.scrRec.hMon = screenPtr->hMonitor;
		pBufferRecord->unBuffDets.scrRec.size = compSizeBuffer;
		retBuff = (char *)pBufferRecord;
	}
	else
	{
		screenBuffer->lockFreeBuffer.lock();
		pBufferRecord = screenBuffer->freeBuffers.front();
		screenBuffer->freeBuffers.pop();
		screenBuffer->lockFreeBuffer.unlock();
		retBuff = (char *)pBufferRecord;
		if (pBufferRecord->sizeOfBuffer < totalBuffSize)
		{
			pBufferRecord = (struct bufferRecord *)HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, pBufferRecord, totalBuffSize);
			pBufferRecord->buffType = 1; // Video Record
			pBufferRecord->sizeOfBuffer = totalBuffSize;
			pBufferRecord->buffer = (char *)pBufferRecord + sizeof(struct bufferRecord) + 8;
			pBufferRecord->unBuffDets.scrRec.hMon = screenPtr->hMonitor;
			pBufferRecord->unBuffDets.scrRec.size = compSizeBuffer;
			retBuff = (char *)pBufferRecord;
		}
	}
	return retBuff;
}

char *RecordBufferManager::GetFreeAudioBuffer(DWORD size)
{
	DWORD totalBuffSize = size + sizeof(struct bufferRecord) + 16;
	struct bufferRecord *pBufferRecord = NULL;

	lockAudioBuffer.lock();
	bool bIsEmpty = audioBuffer.empty();
	if (!bIsEmpty)
	{
		pBufferRecord = audioBuffer.front();
		audioBuffer.pop();
	}
	lockAudioBuffer.unlock();
	if (bIsEmpty)
	{
		pBufferRecord = (struct bufferRecord *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, totalBuffSize);
		pBufferRecord->unBuffDets.audRec.origSize = totalBuffSize;
	}

	if (pBufferRecord->unBuffDets.audRec.origSize < totalBuffSize)
	{
		pBufferRecord = (struct bufferRecord *)HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, pBufferRecord, totalBuffSize);
		pBufferRecord->unBuffDets.audRec.origSize = totalBuffSize;
	}

	pBufferRecord->sizeOfBuffer = totalBuffSize;
	pBufferRecord->buffType = 2; // Audio Record
	pBufferRecord->unBuffDets.audRec.size = size;
	pBufferRecord->buffer = (char *)pBufferRecord + sizeof(struct bufferRecord) + 8;
	char *retBuff = (char *)pBufferRecord;
	return retBuff;
}

char *RecordBufferManager::GetFreeEventBuffer()
{
	lockEventBuffer.lock();
	if (eventBuffer.empty())
	{ // if free buffer is empty, create a new buffer
		DWORD totalBuffSize = sizeof(struct bufferRecord) + 16 + 5024;
		struct bufferRecord *pBufferRecord = (struct bufferRecord *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, totalBuffSize);
		pBufferRecord->buffType = 3; // Event Record
		pBufferRecord->sizeOfBuffer = totalBuffSize;
		pBufferRecord->unBuffDets.eventRec.size = totalBuffSize;
		pBufferRecord->buffer = (char *)pBufferRecord + sizeof(struct bufferRecord) + 8;
		eventBuffer.push(pBufferRecord);
	}
	char *retBuff = (char *)eventBuffer.front();
	eventBuffer.pop();
	lockEventBuffer.unlock();
	return retBuff;
}

