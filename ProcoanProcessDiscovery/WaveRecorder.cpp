#include "stdafx.h"
#include <mmsystem.h>
#include "WavDeviceList.h"
#include "RecorderDataFileManager.h"
#include "RecordBufferManager.h"
#include "WaveRecorder.h"
#include "spdlog/spdlog.h"

void CALLBACK waveInProc(HWAVEIN hwi, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);


WaveRecorder::WaveRecorder()
{
	m_hWaveIn = NULL;

	m_status = 0;
	pDataRecorder = NULL;
	ZeroMemory(m_WaveHeader, MAX_BUFFERS * sizeof(WAVEHDR));
	ZeroMemory(&m_wvFormat, sizeof(WAVEFORMATEX));
	bPauseRecording = false;
}

void WaveRecorder::SetDataManager(RecorderDataFileManager *pDataFileManager)
{
	pDataRecorder = pDataFileManager;
}

void WaveRecorder::SetRecorderBufferManager(RecordBufferManager *pRecBuffer)
{
	pRecordBufferManager = pRecBuffer;
}

int WaveRecorder::SelectDevice()
{
	int devCaps[] = { WAVE_FORMAT_44S16,WAVE_FORMAT_44S08,WAVE_FORMAT_44M16,WAVE_FORMAT_44M08,
					  WAVE_FORMAT_2S16,WAVE_FORMAT_2S08,WAVE_FORMAT_2M16,WAVE_FORMAT_2M08,
					  WAVE_FORMAT_1S16,WAVE_FORMAT_1S08,WAVE_FORMAT_1M16,WAVE_FORMAT_1M08,
					  -1};

	for (int i = 0; devCaps[i] != -1; i++)
	{
		m_deviceNum = m_wavDeviceList.GetDevice(devCaps[i]);
		if (m_deviceNum != -1)
			break;
	}
	if (m_deviceNum != -1)
		SetDeviceCaps();

	return m_deviceNum;
}

void WaveRecorder::SetDeviceCaps()
{
	m_wavDeviceList.GetDeviceCaps(m_deviceNum, &m_numChannels, &m_freq, &m_dataSize);
	ZeroMemory(&m_wvFormat, sizeof(WAVEFORMATEX));
	m_wvFormat.nSamplesPerSec = m_freq;
	m_wvFormat.nChannels = (WORD)m_numChannels;
	m_wvFormat.wFormatTag = WAVE_FORMAT_PCM;
	m_wvFormat.nBlockAlign = m_numChannels * m_dataSize/8;
	m_wvFormat.wBitsPerSample = m_dataSize;
	m_wvFormat.nAvgBytesPerSec = m_freq * m_numChannels * m_dataSize/8;
	pDataRecorder->SetAudioParams(m_freq, m_numChannels, WAVE_FORMAT_PCM, m_wvFormat.wBitsPerSample);
}


WaveRecorder::~WaveRecorder()
{

}

int WaveRecorder::StartRecording()
{
	spdlog::info("Initializing Audio Recording");
	MMRESULT result = waveInOpen(&m_hWaveIn, m_deviceNum, &m_wvFormat, (DWORD_PTR)waveInProc, (DWORD_PTR)this, CALLBACK_FUNCTION);
	if (result != MMSYSERR_NOERROR)
	{
		TCHAR fault[256];
		waveInGetErrorText(result, fault, 256);
		return FALSE;
	}

	for (int nIndex = 0; nIndex < MAX_BUFFERS; nIndex++)
	{
		m_WaveHeader[nIndex].lpData = (LPSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, m_wvFormat.nAvgBytesPerSec);
		m_WaveHeader[nIndex].dwBufferLength = m_wvFormat.nAvgBytesPerSec;
		m_WaveHeader[nIndex].dwUser = nIndex;
		if (waveInPrepareHeader(m_hWaveIn, &m_WaveHeader[nIndex], sizeof(WAVEHDR)) != 0)
			return FALSE;
		result = waveInAddBuffer(m_hWaveIn, &m_WaveHeader[nIndex], sizeof(WAVEHDR));
		if (result == MMSYSERR_INVALHANDLE)
			break;
		else if (result == MMSYSERR_NODRIVER)
			break;
		else if (result == MMSYSERR_NOMEM)
			break;
		else if (result == WAVERR_UNPREPARED)
			break;
	}

	if (waveInStart(m_hWaveIn) != 0)
		return FALSE;

	spdlog::info("Initializing Audio Recording Done");
	return TRUE;

}

int WaveRecorder::StopRecording()
{
	spdlog::info("Stopping Audio Recording");

	if (m_hWaveIn)
	{
		waveInStop(m_hWaveIn);
		for (int nIndex = 0; nIndex < MAX_BUFFERS;++nIndex)
		{
			if (m_WaveHeader[nIndex].lpData)
			{
				waveInUnprepareHeader(m_hWaveIn, &m_WaveHeader[nIndex], sizeof(WAVEHDR));
				HeapFree(GetProcessHeap(), 0, m_WaveHeader[nIndex].lpData);
				m_WaveHeader[nIndex].lpData = NULL;
			}
		}
		waveInClose(m_hWaveIn);
	}

	m_hWaveIn = NULL;
	spdlog::info("Audio Recording Stopped");
	return TRUE;
}


void CALLBACK waveInProc(HWAVEIN hwi, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	switch (uMsg)
	{
	case WIM_CLOSE:
		break;

	case WIM_DATA:
	{
		WaveRecorder *pRecorder = (WaveRecorder *)dwInstance;
		pRecorder->ProcessHeader((WAVEHDR *)dwParam1);
	}
	break;

	case WIM_OPEN:
		break;

	default:
		break;
	}
}

void WaveRecorder::PauseRecording()
{
	bPauseRecording = true;
}

void WaveRecorder::RestartRecording()
{
	bPauseRecording = false;
}

VOID WaveRecorder::ProcessHeader(WAVEHDR * pHdr)
{
	spdlog::info("Process Audio Segment");

	MMRESULT mRes = 0;

	//TRACE("%d", pHdr->dwUser);
	if (WHDR_DONE == (WHDR_DONE &pHdr->dwFlags))
	{
		if (!bPauseRecording)
		{
			struct bufferRecord *pBuffer = (struct bufferRecord *)pRecordBufferManager->GetFreeAudioBuffer(m_wvFormat.nAvgBytesPerSec);
			pBuffer->unBuffDets.audRec.size = pHdr->dwBytesRecorded;
			memcpy(pBuffer->buffer, pHdr->lpData, pHdr->dwBytesRecorded);
			pBuffer->timeStamp = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
			pRecordBufferManager->WriteToFile(pBuffer);
		}
		mRes = waveInAddBuffer(m_hWaveIn, pHdr, sizeof(WAVEHDR));
	//	if (mRes != 0)
	//		StoreError(mRes, TRUE, "File: %s ,Line Number:%d", __FILE__, __LINE__);
	}
	spdlog::info("Process Audio Segment Done");
}
