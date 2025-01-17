//#pragma once
#include "WavDeviceList.h"

#define MAX_BUFFERS	5

class WaveRecorder
{
	WavDeviceList m_wavDeviceList;
	int m_deviceNum;
	WORD m_status; /* 0-Stopped, 1- Recording, 2-Paused */
	WORD m_numChannels; /* 1-Mono, 2-Stereo */
	WORD m_freq;
	WORD m_dataSize;
	WORD m_devCap;
	WAVEFORMATEX m_wvFormat;
	HWAVEIN m_hWaveIn;
	WAVEHDR m_WaveHeader[MAX_BUFFERS];
	RecorderDataFileManager *pDataRecorder;
	RecordBufferManager *pRecordBufferManager;
	bool bPauseRecording;

public:
	void SetDataManager(RecorderDataFileManager *pRecorder);
	void SetRecorderBufferManager(RecordBufferManager *pRecBuffer);
	int SelectDevice();
	void SetDeviceCaps();
	int StartRecording();
	int StopRecording();
	void PauseRecording();
	void RestartRecording();

	VOID ProcessHeader(WAVEHDR * pHdr);
	WaveRecorder();
	~WaveRecorder();
};

