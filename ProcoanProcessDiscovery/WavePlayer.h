#pragma once

#define MAX_READ_BUFFERS	4
#include <mmsystem.h>
class RecordBufferManager;
class AudioPlayerBuffer;

class WavePlayer
{
	WAVEFORMATEX m_wvFormat;
	HWAVEOUT m_hWaveOut;
	WAVEHDR m_WaveHeader[MAX_READ_BUFFERS];
	int waveCurrentBlock;

	RecordBufferManager *pRecBufferManager;
	PlayerDataFileManager *pDataRecorder;
	AudioPlayerBuffer *pAudioPlayerBuffer;

	int waveFreeBlockCount;
	int PlayerMode;

	bool	bStartPlay;
	bool	bExitThreads;

public:
	void Pause();
	void Restart();
	void SetDataManager(PlayerDataFileManager *pRecorder);
	void SetRecordBufferManager(RecordBufferManager *pRecBuff);
	void SetAudioParameters(long freq, int noChannels, int format, int bitsPerSample);
	void SetAudioPlayerBuffer(AudioPlayerBuffer *pBuffer);
	void StartPlay(long long fPos);
	void StopPlay();
	void SetStartFlag(bool bFlg);
	void SetExitFlag(bool bFlg);
	void RestartAudio();
	bool IsPlayerStopped();
	void Init();
	WavePlayer();
	~WavePlayer();
};

