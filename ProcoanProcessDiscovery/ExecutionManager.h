#pragma once

#include <mutex>
#include <queue>
#include "mmHeader.h"
#include "IPlayerWindow.h"

class PlayerDataFileManager;
class RecorderDataFileManager;

using namespace std;

class ExecutionManager
{
	queue <int> commandBuffer;
	PlayerDataFileManager *pPlayerDataManager;
	RecorderDataFileManager *pRecDataManager;
	mutex lockExecutionManager;
	bool bQuitExecutionManager;

	HANDLE execThreadHandle;
	DWORD  execThreadID;

	HANDLE playerThread;
	DWORD  playerThreadID;

	HANDLE videoThread;
	DWORD  videoThreadID;

	HANDLE dataFileThread;
	DWORD  dataFileThreadID;

	HANDLE displayThread;
	DWORD  displayThreadID;

	HANDLE rewFwdThread;
	DWORD  rewFwdThreadID;

	HANDLE stepRewFwdThread;
	DWORD  stepRewFwdThreadID;

	HANDLE moveAbsoluteThread;
	DWORD  moveAbsoluteThreadID;

	HANDLE thumbDisplayThread;
	DWORD  thumbDisplayThreadID;

	//Recorder Threads
	HANDLE videoCaptureThread;
	DWORD videoCaptureThreadID;

	HANDLE writeFileThread;
	DWORD writeFileThreadID;

	HANDLE audioThread;
	DWORD audioThreadID;

	HANDLE eventThread;
	DWORD eventThreadID;

	HANDLE stopRecordingThread;
	DWORD stopRecordingThreadID;

	list <struct monitorDetails *> *pMonitorList;

	bool bEventFlg;
	bool bAudioFlg;
	bool bVideoFlg;

public:
	ExecutionManager();
	~ExecutionManager();
	void StartExecutionManager();
	void StopExecutionManager();
	void ExecuteCommands();
	void StartPlayerCommand(char *filename, IPlayerWindow *pPanelWindow);
	void StartRecorderCommand(char *filename);
	void PauseRecorderCommand();
	void RestartRecorderCommand();
	void StopRecorderFunction();
	void StopRecorderCommand();
	void StopPlayerManager();
	void StopThumbDisplayThread();
	void StartThumbDisplayThread();

	void StartDataFileThread();
	void StartVideoThread();
	void StartAudioThread();
	void StartDisplayThread();
	void StartRewindForwardThread();
	void StartStepRewindForwardThread();

	void StopDataFileThread();
	void StopVideoThread();
	void StopAudioThread();
	void StopDisplayThread();
	void StopRewindForwardThread();
	void StartMoveAbsoluteThread();
	void StopMoveAbsoluteThread();
	void StopStepRewindForwardThread();
	void WaitForThreadsToStop();
	void SetInsertEvent();

	// Recorder Threads
	void StartWriteFileThread();
	void StartAudioRecording();
	void StartVideoRecording();
	void StartEventRecording();

	void StopWriteFileThread();
	void StopAudioRecording();
	void StopVideoRecording();
	void StopEventRecording();

	void SetThumbImage(int num);
	void ShowSliderTime();
	void CheckTabNotifications(NMHDR *pNMHdr);
	void CheckKillFocus(HWND hWnd);
	void SetSelectedMonitorList(list <struct monitorDetails *> *pList);
	void SetEventRecording(bool bFlg);
	void SetVideoRecording(bool bFlg);
	void SetAudioRecording(bool bFlg);

	void CloseAllWindows();
	void GetRecorderStatus(struct RecorderStatusRecord *recStatus);
};

