#include "stdafx.h"
#include <mmsystem.h>
#include "PlayerDataFileManager.h"
#include "WavePlayer.h"
#include "RecordBufferManager.h"
#include "AudioPlayerBuffer.h"
#include "Utility.h"
#include <mutex>
#include "spdlog/spdlog.h"

void CALLBACK waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
static CRITICAL_SECTION waveCriticalSection;
mutex lockWavPlayer;

WavePlayer::WavePlayer()
{
	m_hWaveOut = NULL;

	pDataRecorder = NULL;
	ZeroMemory(m_WaveHeader, MAX_READ_BUFFERS * sizeof(WAVEHDR));
	ZeroMemory(&m_wvFormat, sizeof(WAVEFORMATEX));
	waveFreeBlockCount = MAX_READ_BUFFERS;
	waveCurrentBlock = 0;
	pRecBufferManager = NULL;
	InitializeCriticalSection(&waveCriticalSection);
	PlayerMode = 0; // OFF
	pAudioPlayerBuffer = NULL;
	bStartPlay = false;
	bExitThreads = false;

}

void WavePlayer::SetStartFlag(bool bFlg)
{
	lockWavPlayer.lock();
	bStartPlay = bFlg;
	lockWavPlayer.unlock();
}

void WavePlayer::SetExitFlag(bool bFlg)
{
	lockWavPlayer.lock();
	bExitThreads = bFlg;
	lockWavPlayer.unlock();
}

void WavePlayer::SetAudioParameters(long freq, int noChannels,  int bitsPerSample)
{
	m_wvFormat.nSamplesPerSec = freq;
	m_wvFormat.nChannels = noChannels;
	m_wvFormat.wFormatTag = WAVE_FORMAT_PCM;
	m_wvFormat.nBlockAlign = noChannels * bitsPerSample / 8;
	m_wvFormat.wBitsPerSample = bitsPerSample;
	m_wvFormat.nAvgBytesPerSec = freq * noChannels * bitsPerSample / 8;
}

void WavePlayer::SetAudioPlayerBuffer(AudioPlayerBuffer *pBuffer)
{
	pAudioPlayerBuffer = pBuffer;
}

void WavePlayer::Pause()
{
	if (m_hWaveOut && 	PlayerMode == 1)
	{
		PlayerMode = 2;
		waveOutPause(m_hWaveOut);
	}
}


void WavePlayer::Restart()
{
	if (m_hWaveOut && PlayerMode == 2)
	{
		PlayerMode = 1;
		waveOutRestart(m_hWaveOut);
	}
}

WavePlayer::~WavePlayer()
{
	DeleteCriticalSection(&waveCriticalSection);
	for (int nIndex = 0; nIndex < MAX_READ_BUFFERS; nIndex++)
	{
		HeapFree(GetProcessHeap(), 0, m_WaveHeader[nIndex].lpData);
	}

}

void WavePlayer::SetDataManager(PlayerDataFileManager *pDataFileManager)
{
	pDataRecorder = pDataFileManager;
}

void WavePlayer::SetRecordBufferManager(RecordBufferManager *pRecBuff)
{
	pRecBufferManager = pRecBuff;
}

void WavePlayer::StopPlay()
{
	bExitThreads = true;
}

void WavePlayer::StartPlay(void)
{
	spdlog::info("Starting Audio Play");

	PlayerMode = 1; // Play
	waveCurrentBlock = 0;
	Init();
	WAVEHDR *current = &m_WaveHeader[waveCurrentBlock];
	// Wait for video buffer to be filled;
	while (TRUE)
	{
		if ((pRecBufferManager->ResizeInitDone() /*&& pRecBufferManager->ResizeScreenBufferFull()*/) || bStartPlay)
			break;
		Sleep(10);
	}
	int freeCount = 0;
	pAudioPlayerBuffer->SetBufferMaxSize(m_wvFormat.nAvgBytesPerSec);
	while (TRUE)
	{
		lockWavPlayer.lock();
		bool bPlay = bStartPlay;
		bool bQuit = bExitThreads;
		lockWavPlayer.unlock();
		if (bQuit)
			break;
		if (!bPlay)
		{
			Sleep(10);
			continue;
		}

		//struct bufferRecord *pBuffer = pRecBufferManager->GetAudioPlayRecord();

		EnterCriticalSection(&waveCriticalSection);
		freeCount = waveFreeBlockCount;
		LeaveCriticalSection(&waveCriticalSection);
		if (freeCount && pAudioPlayerBuffer->GetAudioBuffer())
		{
			if (current->dwFlags & WHDR_PREPARED)
			{
				waveOutUnprepareHeader(m_hWaveOut, current, sizeof(WAVEHDR));
			}
			memcpy(current->lpData, pAudioPlayerBuffer->GetBuffer(), pAudioPlayerBuffer->GetBufferLength());

			current->dwBufferLength = pAudioPlayerBuffer->GetBufferLength();
			waveOutPrepareHeader(m_hWaveOut, current, sizeof(WAVEHDR));
			waveOutWrite(m_hWaveOut, current, sizeof(WAVEHDR));
			EnterCriticalSection(&waveCriticalSection);
			waveFreeBlockCount--;
			LeaveCriticalSection(&waveCriticalSection);
			waveCurrentBlock++;
			waveCurrentBlock %= MAX_READ_BUFFERS;
			current = &m_WaveHeader[waveCurrentBlock];
			current->dwUser = 0;
			//if (!waveFreeBlockCount)
			//{
			//	while (!waveFreeBlockCount && bStartPlay)
			//		Sleep(1);
			//}
		}
		else
			Sleep(5);
	}

	/*
	* wait for all blocks to complete
	*/

	while (true)
	{
		EnterCriticalSection(&waveCriticalSection);
		freeCount = waveFreeBlockCount;
		LeaveCriticalSection(&waveCriticalSection);
		if (freeCount < (MAX_READ_BUFFERS - 1))
			Sleep(100);
		else
		{
			break;
		}
	}
	/*
	* unprepare any blocks that are still prepared
	*/
	for (int i = 0; i < MAX_READ_BUFFERS; i++)
		if (m_WaveHeader[i].dwFlags & WHDR_PREPARED)
			waveOutUnprepareHeader(m_hWaveOut, &m_WaveHeader[i], sizeof(WAVEHDR));

	Sleep(100);
	PlayerMode = 0;
	spdlog::info("End of Audio Play");
}

bool WavePlayer::IsPlayerStopped()
{
	return PlayerMode == 0;
}

void WavePlayer::Init()
{
	spdlog::info("Initializing Audio Parameters");

	for (int nIndex = 0; nIndex < MAX_READ_BUFFERS; nIndex++)
	{
		m_WaveHeader[nIndex].lpData = (LPSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, m_wvFormat.nAvgBytesPerSec * 20L);
		m_WaveHeader[nIndex].dwBufferLength = m_wvFormat.nAvgBytesPerSec * 20L;
		m_WaveHeader[nIndex].dwUser = 0;
	}

	MMRESULT result = waveOutOpen(&m_hWaveOut, WAVE_MAPPER, &m_wvFormat, (DWORD_PTR)waveOutProc, (DWORD_PTR)&waveFreeBlockCount, CALLBACK_FUNCTION);
	if (result != MMSYSERR_NOERROR)
	{
		TCHAR fault[256];
		waveInGetErrorText(result, fault, 256);
		return;
	}
	spdlog::info("Initializing Audio Parameters Done");
}

void CALLBACK waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{

	/*
	* pointer to free block counter
	*/
	int* freeBlockCounter = (int*)dwInstance;
	/*
	* ignore calls that occur due to openining and closing the
	* device.
	*/
	if (uMsg != WOM_DONE)
		return;
	EnterCriticalSection(&waveCriticalSection);
	(*freeBlockCounter)++;
	LeaveCriticalSection(&waveCriticalSection);


}


