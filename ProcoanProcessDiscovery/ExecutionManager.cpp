#include "stdafx.h"
#include "PlayerDataFileManager.h"
#include "RecorderDataFileManager.h"
#include "ExecutionManager.h"
#include "RecorderWindow.h"
#include "spdlog/spdlog.h"
#include "utility.h"

DWORD WINAPI ExecutionThreadFunction(LPVOID lpParameter);

DWORD WINAPI PlayerThreadFunction(LPVOID lpParameter);
DWORD WINAPI VideoPlayerThreadFunction(LPVOID lpParameter);
DWORD WINAPI DataFileThreadFunction(LPVOID lpParameter);
DWORD WINAPI DisplayThreadFunction(LPVOID lpParameter);
DWORD WINAPI RewindForwardThreadFunction(LPVOID lpParameter);
DWORD WINAPI MoveAbsoluteThreadFunction(LPVOID lpParameter);
DWORD WINAPI ThumbDisplayThreadFunction(LPVOID lpParameter);
DWORD WINAPI VideoRecordThreadFunction(LPVOID lpParameter);
DWORD WINAPI FileWriteRecordFunction(LPVOID lpParameter);
DWORD WINAPI AudioRecordThreadFunction(LPVOID lpParameter);
DWORD WINAPI EventRecordThreadFunction(LPVOID lpParameter);
DWORD WINAPI StepRewindForwardThreadFunction(LPVOID lpParameter);
DWORD WINAPI StopRecordingThreadFunction(LPVOID lpParameter);

ExecutionManager::ExecutionManager()
{
	bQuitExecutionManager = false;
	pPlayerDataManager = NULL;
	pRecDataManager = NULL;
	execThreadHandle = NULL;
	execThreadID = NULL;
	playerThread = NULL;
	playerThreadID = NULL;
	videoThread = NULL;
	videoThreadID = NULL;
	dataFileThread = NULL;
	dataFileThreadID = NULL;
	displayThread = NULL;
	displayThreadID = NULL;
	rewFwdThread = NULL;
	rewFwdThreadID = NULL;
	moveAbsoluteThread = NULL;
	moveAbsoluteThreadID = NULL;
	thumbDisplayThread = NULL;
	thumbDisplayThreadID = NULL;
	videoCaptureThread = NULL;
	videoCaptureThreadID = NULL;
	writeFileThread = NULL;
	writeFileThreadID = NULL;
	audioThread = NULL;
	audioThreadID = NULL;
	eventThread = NULL;
	eventThreadID = NULL;
	stopRecordingThread = NULL;
	stopRecordingThreadID = NULL;
	stepRewFwdThread = NULL;
	stepRewFwdThreadID = NULL;
	pMonitorList = NULL;
}


ExecutionManager::~ExecutionManager()
{
	bQuitExecutionManager = true;
	if (execThreadHandle != NULL)
	{
		WaitForSingleObject(execThreadHandle, INFINITE);
		CloseHandle(execThreadHandle);
		execThreadHandle = NULL;
	}
}

void ExecutionManager::SetEventRecording(bool bFlg)
{
	bEventFlg = bFlg;
}

void ExecutionManager::SetVideoRecording(bool bFlg)
{
	bVideoFlg = bFlg;
}

void ExecutionManager::SetAudioRecording(bool bFlg)
{
	bAudioFlg = bFlg;
}

void ExecutionManager::SetSelectedMonitorList(list <struct monitorDetails *> *pList)
{
	pMonitorList = pList;
}

void ExecutionManager::ShowSliderTime()
{
	//if (pPlayerDataManager != NULL)
	//	pPlayerDataManager->ShowSliderTime();
}

void ExecutionManager::CheckKillFocus(HWND hWnd)
{
	//if (pPlayerDataManager != NULL)
	//	pPlayerDataManager->CheckKillFocus(hWnd);
}

void ExecutionManager::CheckTabNotifications(NMHDR *pNMHdr)
{
	//if (pPlayerDataManager != NULL)
	//	pPlayerDataManager->CheckTabNotifications(pNMHdr);
}

void ExecutionManager::SetThumbImage(int numThumb)
{
	//if (pPlayerDataManager != NULL)
	//	pPlayerDataManager->SetThumbImageNumber(numThumb);
}

void ExecutionManager::SetInsertEvent()
{
	//if (pPlayerDataManager != NULL)
	//	pPlayerDataManager->InsertEvent();
}

void ExecutionManager::StartExecutionManager()
{
	execThreadHandle = CreateThread(0, 0,ExecutionThreadFunction, this, 0, &execThreadID);
}

void ExecutionManager::GetRecorderStatus(struct RecorderStatusRecord *recStatus)
{
	if (pRecDataManager != NULL)
	{
		pRecDataManager->GetRecorderStatus(recStatus);
	}
}

void ExecutionManager::StopExecutionManager()
{
	bQuitExecutionManager = true;
	if (execThreadHandle != NULL)
	{
		WaitForSingleObject(execThreadHandle, INFINITE);
		CloseHandle(execThreadHandle);
		execThreadHandle = NULL;
	}
}

DWORD WINAPI ExecutionThreadFunction(LPVOID lpParameter)
{
	ExecutionManager *executionManager = (ExecutionManager *)lpParameter;

	executionManager->ExecuteCommands();

	return 0;
}

void ExecutionManager::CloseAllWindows()
{
	if (pPlayerDataManager != NULL)
	{
		pPlayerDataManager->StopPlayer();
		delete pPlayerDataManager;
		pPlayerDataManager = NULL;
	}
	StopExecutionManager();
}

void ExecutionManager::StartPlayerCommand(char *filename, IPlayerWindow *pPlayerWindow)
{
	if (pPlayerDataManager == NULL)
	{
		pPlayerDataManager = DBG_NEW PlayerDataFileManager(filename);
		pPlayerDataManager->SetPlayerWindow(pPlayerWindow);
		pPlayerDataManager->StartPlayer();
	}
}


void ExecutionManager::StartRecorderCommand(char *filename)
{
	if (pRecDataManager == NULL)
	{
		pRecDataManager = DBG_NEW RecorderDataFileManager(filename);
		pRecDataManager->SetSelectedMonitorList(pMonitorList);
		pRecDataManager->SetVideoRecording(bVideoFlg);
		pRecDataManager->SetAudioRecording(bAudioFlg);
		pRecDataManager->SetEventRecording(bEventFlg);
		pRecDataManager->StartRecording();
	}
}

void ExecutionManager::PauseRecorderCommand()
{
	if (pRecDataManager != NULL)
	{
		pRecDataManager->PauseRecording();
	}
}

void ExecutionManager::RestartRecorderCommand()
{
	if (pRecDataManager != NULL)
	{
		pRecDataManager->RestartRecording();
	}
}




void ExecutionManager::StopRecorderCommand()
{
	stopRecordingThread = CreateThread(0, 0, StopRecordingThreadFunction, this, 0, &stopRecordingThreadID);
}

void ExecutionManager::StopRecorderFunction()
{
	if (pRecDataManager != NULL)
	{
		pRecDataManager->StopRecording();
		delete pRecDataManager;
		pRecDataManager = NULL;
	}
}


void ExecutionManager::StartThumbDisplayThread()
{
	lockExecutionManager.lock();
	commandBuffer.push(7);
	lockExecutionManager.unlock();
}
void ExecutionManager::StopThumbDisplayThread()
{
	lockExecutionManager.lock();
	commandBuffer.push(17);
	lockExecutionManager.unlock();
}

void ExecutionManager::StopPlayerManager()
{
	lockExecutionManager.lock();
	commandBuffer.push(100);
	lockExecutionManager.unlock();
}

void ExecutionManager::StartDataFileThread()
{
	lockExecutionManager.lock();
	commandBuffer.push(1);
	lockExecutionManager.unlock();
}

void  ExecutionManager::StartVideoThread()
{
	lockExecutionManager.lock();
	commandBuffer.push(2);
	lockExecutionManager.unlock();
}

void ExecutionManager::StartAudioThread()
{
	//lockExecutionManager.lock();
	//commandBuffer.push(3);
	//lockExecutionManager.unlock();
	playerThread = CreateThread(0, 0, PlayerThreadFunction, pPlayerDataManager, 0, &playerThreadID);

}

void ExecutionManager::StartDisplayThread()
{
	lockExecutionManager.lock();
	commandBuffer.push(4);
	lockExecutionManager.unlock();
}

void ExecutionManager::StartRewindForwardThread()
{
	//lockExecutionManager.lock();
	//commandBuffer.push(5);
	//lockExecutionManager.unlock();
	if (rewFwdThread != NULL)
	{
		StopRewindForwardThread();
	}
	else
		rewFwdThread = CreateThread(0, 0, RewindForwardThreadFunction, pPlayerDataManager, 0, &rewFwdThreadID);
}

void ExecutionManager::StartStepRewindForwardThread()
{
	//lockExecutionManager.lock();
	//commandBuffer.push(8);
	//lockExecutionManager.unlock();

	if (stepRewFwdThread != NULL)
	{
		StopStepRewindForwardThread();
	}
	else
		stepRewFwdThread = CreateThread(0, 0, StepRewindForwardThreadFunction, pPlayerDataManager, 0, &stepRewFwdThreadID);
}

void ExecutionManager::StopStepRewindForwardThread()
{
	//lockExecutionManager.lock();
	//commandBuffer.push(15);
	//lockExecutionManager.unlock();
	if (stepRewFwdThread != NULL)
	{
		WaitForSingleObject(stepRewFwdThread, INFINITE);
		CloseHandle(stepRewFwdThread);
		stepRewFwdThread = NULL;
	}
}

// Recorder Threads
void ExecutionManager::StartWriteFileThread()
{
	lockExecutionManager.lock();
	commandBuffer.push(21);
	lockExecutionManager.unlock();
}

void ExecutionManager::StartAudioRecording()
{
	lockExecutionManager.lock();
	commandBuffer.push(22);
	lockExecutionManager.unlock();
}

void ExecutionManager::StartVideoRecording()
{
	lockExecutionManager.lock();
	commandBuffer.push(23);
	lockExecutionManager.unlock();
}

void ExecutionManager::StartEventRecording()
{
	lockExecutionManager.lock();
	commandBuffer.push(24);
	lockExecutionManager.unlock();
}

void ExecutionManager::StopWriteFileThread()
{
	lockExecutionManager.lock();
	commandBuffer.push(31);
	lockExecutionManager.unlock();
}

void ExecutionManager::StopAudioRecording()
{
	lockExecutionManager.lock();
	commandBuffer.push(32);
	lockExecutionManager.unlock();
}

void ExecutionManager::StopVideoRecording()
{
	lockExecutionManager.lock();
	commandBuffer.push(33);
	lockExecutionManager.unlock();
}

void ExecutionManager::StopEventRecording()
{
	lockExecutionManager.lock();
	commandBuffer.push(34);
	lockExecutionManager.unlock();
}

void ExecutionManager::StopDataFileThread()
{
	lockExecutionManager.lock();
	commandBuffer.push(11);
	lockExecutionManager.unlock();
}

void ExecutionManager::WaitForThreadsToStop()
{
	while (playerThread != NULL || videoThread != NULL ||
		dataFileThread != NULL ||
		displayThread != NULL ||
		rewFwdThread != NULL ||
		moveAbsoluteThread != NULL ||
		thumbDisplayThread != NULL || 
		stepRewFwdThread != NULL)
		Sleep(10);

	return;
}

void  ExecutionManager::StopVideoThread()
{
	lockExecutionManager.lock();
	commandBuffer.push(12);
	lockExecutionManager.unlock();
}

void ExecutionManager::StopAudioThread()
{
	//lockExecutionManager.lock();
	//commandBuffer.push(13);
	//lockExecutionManager.unlock();
	if (playerThread != NULL)
	{
		WaitForSingleObject(playerThread, INFINITE);
		CloseHandle(playerThread);
		playerThread = NULL;
	}
}

void ExecutionManager::StopDisplayThread()
{
	lockExecutionManager.lock();
	commandBuffer.push(14);
	lockExecutionManager.unlock();
}

void ExecutionManager::StopRewindForwardThread()
{
	//lockExecutionManager.lock();
	//commandBuffer.push(15);
	//lockExecutionManager.unlock();
	if (rewFwdThread != NULL)
	{
		WaitForSingleObject(rewFwdThread, INFINITE);
		CloseHandle(rewFwdThread);
		rewFwdThread = NULL;
	}
}

void ExecutionManager::StartMoveAbsoluteThread()
{
	if (moveAbsoluteThread == NULL)
	{
		moveAbsoluteThread = CreateThread(0, 0, MoveAbsoluteThreadFunction, pPlayerDataManager, 0, &moveAbsoluteThreadID);
	}

}

void ExecutionManager::StopMoveAbsoluteThread()
{
	if (moveAbsoluteThread != NULL)
	{
		WaitForSingleObject(moveAbsoluteThread, INFINITE);
		CloseHandle(moveAbsoluteThread);
		moveAbsoluteThread = NULL;
	}
}

void ExecutionManager::ExecuteCommands()
{
	int command = -1;
	while (true)
	{
		lockExecutionManager.lock();
		bool bQuit = bQuitExecutionManager;
		lockExecutionManager.unlock();
		if (bQuit)
			break;
		command = -1;
		lockExecutionManager.lock();
		if (!commandBuffer.empty())
		{
			command = commandBuffer.front();
			commandBuffer.pop();
		}
		lockExecutionManager.unlock();
		switch (command)
		{
		case -1:
			Sleep(10);
			break;
		case 1:
			if (dataFileThread != NULL)
			{
				StopDataFileThread();
				StartDataFileThread();
			}
			else
				dataFileThread = CreateThread(0, 0, DataFileThreadFunction, pPlayerDataManager, 0, &dataFileThreadID);
			break;
		case 2:
			if (videoThread != NULL)
			{
				StopVideoThread();
				StartVideoThread();
			}
			else
				videoThread = CreateThread(0, 0, VideoPlayerThreadFunction, pPlayerDataManager, 0, &videoThreadID);
			break;
		case 3:
			if (playerThread != NULL)
			{
				StopAudioThread();
				StartAudioThread();
			}
			else
				playerThread = CreateThread(0, 0, PlayerThreadFunction, pPlayerDataManager, 0, &playerThreadID);
			break;
		case 4:
			if (displayThread != NULL)
			{
				StopDisplayThread();
				StartDisplayThread();
			}
			else
				displayThread = CreateThread(0, 0, DisplayThreadFunction, pPlayerDataManager, 0, &displayThreadID);
			break;
		case 5:
			//if (rewFwdThread != NULL)
			//{
			//	StopRewindForwardThread();
			//	StartRewindForwardThread();
			//}
			//else
			//	rewFwdThread = CreateThread(0, 0, RewindForwardThreadFunction, pPlayerDataManager, 0, &rewFwdThreadID);
			break;
		case 6:
			if (moveAbsoluteThread == NULL)
			{
				moveAbsoluteThread = CreateThread(0, 0, MoveAbsoluteThreadFunction, pPlayerDataManager, 0, &moveAbsoluteThreadID);
			}
			break;
		case 7:
			if (thumbDisplayThread == NULL)
			{
				thumbDisplayThread = CreateThread(0, 0, ThumbDisplayThreadFunction, pPlayerDataManager, 0, &thumbDisplayThreadID);
			}
			break;

		case 8:
			//if (rewFwdThread != NULL)
			//{
			//	StopRewindForwardThread();
			//	StartRewindForwardThread();
			//}
			//rewFwdThread = CreateThread(0, 0, StepRewindForwardThreadFunction, pPlayerDataManager, 0, &rewFwdThreadID);
			break;


		case 11:
			if (dataFileThread != NULL)
			{
				WaitForSingleObject(dataFileThread, INFINITE);
				CloseHandle(dataFileThread);
				dataFileThread = NULL;
			}
			break;
		case 12:
			if (videoThread != NULL)
			{
				WaitForSingleObject(videoThread, INFINITE);
				CloseHandle(videoThread);
				videoThread = NULL;
			}
			break;
		case 13:
			if (playerThread != NULL)
			{
				WaitForSingleObject(playerThread, INFINITE);
				CloseHandle(playerThread);
				playerThread = NULL;
			}
			break;
		case 14:
			if (displayThread != NULL)
			{
				WaitForSingleObject(displayThread, INFINITE);
				CloseHandle(displayThread);
				displayThread = NULL;
			}
			break;
		case 15:
			//if (rewFwdThread != NULL)
			//{
			//	WaitForSingleObject(rewFwdThread, INFINITE);
			//	CloseHandle(rewFwdThread);
			//	rewFwdThread = NULL;
			//}
			break;
		case 16:
			if (moveAbsoluteThread != NULL)
			{
				WaitForSingleObject(moveAbsoluteThread, INFINITE);
				CloseHandle(moveAbsoluteThread);
				moveAbsoluteThread = NULL;
			}
			break;
		case 17:
			if (thumbDisplayThread != NULL)
			{
				WaitForSingleObject(thumbDisplayThread, INFINITE);
				CloseHandle(thumbDisplayThread);
				thumbDisplayThread = NULL;
			}
			break;

		case 21:
			if (writeFileThread != NULL)
			{
				StopWriteFileThread();
				StartWriteFileThread();
			}
			else
				writeFileThread = CreateThread(0, 0, FileWriteRecordFunction, pRecDataManager, 0, &writeFileThreadID);
			break;

		case 22:
			if (audioThread != NULL)
			{
				StopAudioRecording();
				StartAudioRecording();
			}
			else
				audioThread = CreateThread(0, 0, AudioRecordThreadFunction, pRecDataManager, 0, &audioThreadID);
			break;

		case 23:
			if (videoCaptureThread != NULL)
			{
				StopVideoRecording();
				StartVideoRecording();
			}
			else
				videoCaptureThread = CreateThread(0, 0, VideoRecordThreadFunction, pRecDataManager, 0, &videoCaptureThreadID);
			break;

		case 24:
			if (eventThread != NULL)
			{
				StopEventRecording();
				StartEventRecording();
			}
			else
				eventThread = CreateThread(0, 0, EventRecordThreadFunction, pRecDataManager, 0, &eventThreadID);
			break;

		case 31:
			if (writeFileThread != NULL)
			{
				WaitForSingleObject(writeFileThread, INFINITE);
				CloseHandle(writeFileThread);
				thumbDisplayThread = NULL;
			}
			break;

		case 32:
			if (audioThread != NULL)
			{
				WaitForSingleObject(audioThread, INFINITE);
				CloseHandle(audioThread);
				audioThread = NULL;
			}
			break;

		case 33:
			if (videoCaptureThread != NULL)
			{
				WaitForSingleObject(videoCaptureThread, INFINITE);
				CloseHandle(videoCaptureThread);
				videoCaptureThread = NULL;
			}
			break;

		case 34:
			if (eventThread != NULL)
			{
				WaitForSingleObject(eventThread, INFINITE);
				CloseHandle(eventThread);
				eventThread = NULL;
			}
			break;
		case 100:
			if (pPlayerDataManager == NULL)
				break;
			if (dataFileThread != NULL)
			{
				StopDataFileThread();
				StopPlayerManager();
				break;
			}
			if (videoThread != NULL)
			{
				StopVideoThread();
				StopPlayerManager();
				break;
			}
			if (playerThread != NULL)
			{
				StopAudioThread();
				StopPlayerManager();
				break;
			}
			if (displayThread != NULL)
			{
				StopDisplayThread();
				StopPlayerManager();
				break;
			}
			if (rewFwdThread != NULL)
			{
				StopRewindForwardThread();
				StopPlayerManager();
				break;
			}
			//delete pPlayerDataManager;
			//pPlayerDataManager = NULL;
		}
	}
}

DWORD WINAPI VideoRecordThreadFunction(LPVOID lpParameter)
{
	RecorderDataFileManager *recFileManager = (RecorderDataFileManager *)lpParameter;
	spdlog::info("Starting Video Record Thread");
	recFileManager->RecordVideo();
	spdlog::info("Ending Video Record Thread");
	return 0;
}

DWORD WINAPI AudioRecordThreadFunction(LPVOID lpParameter)
{
	RecorderDataFileManager *recFileManager = (RecorderDataFileManager *)lpParameter;

	spdlog::info("Starting Audio Record Thread");
	recFileManager->RecordAudio();
	spdlog::info("Ending Audio Record Thread");
	return 0;
}

DWORD WINAPI EventRecordThreadFunction(LPVOID lpParameter)
{
	CoInitialize(NULL);
	spdlog::info("Starting Event Record Thread");
	RecorderDataFileManager *recFileManager = (RecorderDataFileManager *)lpParameter;
	recFileManager->RecordEvents();
	spdlog::info("Ending Event Record Thread");
	CoUninitialize();
	return 0;
}

DWORD WINAPI FileWriteRecordFunction(LPVOID lpParameter)
{
	RecorderDataFileManager *recFileManager = (RecorderDataFileManager *)lpParameter;
	spdlog::info("Starting FileWrite Thread");
	recFileManager->WriteFileThread();
	spdlog::info("Ending FileWrite Thread");
	return 0;
}

DWORD WINAPI ThumbDisplayThreadFunction(LPVOID lpParameter)
{
	PlayerDataFileManager *playerFileManager = (PlayerDataFileManager *)lpParameter;

	playerFileManager->ThumbDisplay();

	return 0;
}


DWORD WINAPI MoveAbsoluteThreadFunction(LPVOID lpParameter)
{
	PlayerDataFileManager *playerFileManager = (PlayerDataFileManager *)lpParameter;

	playerFileManager->MoveVideoAbsolute();

	return 0;
}

DWORD WINAPI PlayerThreadFunction(LPVOID lpParameter)
{
	PlayerDataFileManager *playerFileManager = (PlayerDataFileManager *)lpParameter;

	playerFileManager->PlayAudio();

	return 0;
}

DWORD WINAPI VideoPlayerThreadFunction(LPVOID lpParameter)
{
	PlayerDataFileManager *playerFileManager = (PlayerDataFileManager *)lpParameter;

	playerFileManager->PlayVideo();

	return 0;
}

DWORD WINAPI DataFileThreadFunction(LPVOID lpParameter)
{
	PlayerDataFileManager *playerFileManager = (PlayerDataFileManager *)lpParameter;

	playerFileManager->ReadDataFile();

	return 0;
}

DWORD WINAPI DisplayThreadFunction(LPVOID lpParameter)
{
	PlayerDataFileManager *playerFileManager = (PlayerDataFileManager *)lpParameter;

	playerFileManager->DisplayVideo();

	return 0;
}

DWORD WINAPI RewindForwardThreadFunction(LPVOID lpParameter)
{
	PlayerDataFileManager *playerFileManager = (PlayerDataFileManager *)lpParameter;

	playerFileManager->RewindForwardVideo();

	return 0;
}

DWORD WINAPI StepRewindForwardThreadFunction(LPVOID lpParameter)
{
	PlayerDataFileManager *playerFileManager = (PlayerDataFileManager *)lpParameter;

	playerFileManager->StepRewindForwardVideo();

	return 0;
}

DWORD WINAPI StopRecordingThreadFunction(LPVOID lpParameter)
{
	ExecutionManager *pExecManager = (ExecutionManager *)lpParameter;

	pExecManager->StopRecorderFunction();

	return 0;
}

