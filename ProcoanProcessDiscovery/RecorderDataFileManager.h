//#pragma once

#include "MMHeader.h"

class WaveRecorder;
class ScreenRecorder;
class RecordBufferManager;
class EventRecorder;
class RecorderWindow;
class IMouseEventCallback;

#include "DataFile.h"
class RecorderDataFileManager
{
private:
	struct IndexHdr audioIndex;
	struct IndexHdr videoIndex;

	struct rootHdr rootRecord;
	struct channelHdr mainChannelRecord;
	struct AudioHdr audioHeader;
	struct VideoHdr videoHeader;
	struct EventHdr eventHeader;
	struct IndexHdr eventIndex;

	long indexValue;
	long audioIndexValue;
	long videoIndexValue;
	long eventIndexValue;
	unsigned short nAudioValue;   /// Audio value to display in recorder
	struct RecorderStatusRecord recStatus;

	long long pauseTime;
	long long pauseStartTime;
	long long pauseEndTime;

	long long lastEventIndexPosition;
	long long eventIndexPos;

	long long lastAudioIndexPosition;
	long long audioIndexPos;

	long long lastVideoIndexPosition;
	long long videoIndexPos;

	HANDLE videoCaptureThread;
	DWORD videoCaptureThreadID;
	HANDLE videoCompareThread;
	DWORD videoCompareThreadID;
	HANDLE writeFileThread;
	DWORD writeFileThreadID;
	HANDLE audioThread;
	DWORD audioThreadID;
	HANDLE eventThread;
	DWORD eventThreadID;
	int videoIndexNum;

	DataFile *pDataFile;
	WaveRecorder *pWavRecorder;
	ScreenRecorder *pScreenRecorder;
	RecordBufferManager *pRecordBufferManager;
	EventRecorder *pEventRecorder;
	RecorderWindow *pRecorderWindow;
	HWND hRecorderHWND;
	string recFileName;

	// These are all protected flags
	bool bRecordingOn;
	bool bRecordingStop;
	bool bVideoStopped;
	bool bAudioStopped;
	bool bEventStopped;
	bool bRecordingPaused;

	bool bEventRecordInProcess;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	SOCKET ConnectSocket;

	int fps;
	bool bRecordEvents;
	bool bRecordSound;
	bool bRecordVideo;
	HWND hWndPlayer;
	bool GetAudioBuffer(char *destBuffer, char *srcBuffer, struct IndexHdr *indexPage, int *nIndex, int sizeOneSec,
		ULONG *lenDestBuffer, long long timeUs,
		ULONG *lenSrcBuffer, ULONG *lenSrcRead, int alignByteNum,  DataFile *pOldFile);
	list <struct monitorDetails *> *pMonitorList;
	bool SetupSocket();
	void CloseEventProcess();
	void EndEventProcess();
	bool SendEventProcessCommand(char *str, ULONG recvLen);
	void CreateEventRecordsFromEventProcess();
	void WriteEventRecordsFromProcess(char *jSONRec);
	LONG GetVideoIndexNumber(time_t tsVal, bool bFirst);
	void WriteProcessEventsInFile(struct bufferRecord * pData, ULONG vIndex);


public:
	void SaveRecordingFile(char *fileName);
	void StartRecording();
	void StopRecording();
	void PauseRecording();
	void RestartRecording();
	void StartAudioRecording();
	void StartVideoRecording();
	void StartEventRecording();
	void StopAudioRecording();
	void StopVideoRecording();
	void StopWriteDataFile();
	void StartVideoCaptureThread();
	void WriteFileThread();
	void StartWriteFileThread();
	void StartAudioThread();
	void StartEventThread();
	void StopEventRecording();
	void RecordVideo();
	void RecordAudio();
	void RecordEvents();
	void GetEventRecord(struct eventRecord *eRec);
	void GetRecorderStatus(struct RecorderStatusRecord *pRecStatus);
	void SetHWnd(HWND hWnd)
	{
		hWndPlayer = hWnd;
	}

	void SetRecorderWnd(RecorderWindow *pRecorder);
	void SetSelectedMonitorList(list <struct monitorDetails *> *pList);
	void WriteAudioFile(struct bufferRecord * pData);
	void WriteVideoFile(struct bufferRecord *pRecBuffer);
	void WriteEventFile(struct bufferRecord *pRecBuffer);
	void WriteMonitorData(HMONITOR hMon, LPRECT lprcMonitor);
	void CaptureScreenDataTest();
	void CaptureScreenData();
	void SetCallBack(IMouseEventCallback *callBack);
	void SetVideoRecording(bool bFlg);
	void SetAudioRecording(bool bFlg);
	void SetEventRecording(bool bFlg);
	RecorderDataFileManager(char *fileName);
	void SetAudioParams(int freq, int noChannels, int waveFormat, int bitsPerSample);
	HMONITOR GetMonitorNum(POINT pt);
	~RecorderDataFileManager();
};

