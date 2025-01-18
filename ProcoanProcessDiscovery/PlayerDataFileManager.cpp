#include "stdafx.h"
#include "PlayerDataFileManager.h"
#include "WavePlayer.h"
#include "IPlayerWindow.h"
#include "RecordBufferManager.h"
#include "AudioPlayerBuffer.h"
#include "ExecutionManager.h"
#include "BPMNElement.h"
#include "BPMNDecision.h"
#include "BPMNSubProcess.h"
#include "Utility.h"
#include <mutex>
#include "spdlog/spdlog.h"
#include "ResizeBitmap.h"
#include "wx/wx.h"
#include "CustomEvents.h"
#include "ScenarioDefintion.h"
#include "ScenarioFacade.h"

static mutex lockPlayerManager;
#define THUMB_WIDTH	160
#define THUMB_HEIGHT 95

PlayerDataFileManager::PlayerDataFileManager(char *fileName)
{
	currentFile = fileName;
	curContext = DBG_NEW FileContext;
	fileNumber = 1;
	bDataReadThreadWait = true;
	bVideoThreadWait = true;
	bRewindThreadWait = true;
	bMoveAbsoluteThreadWait = true;
	bMoveAbsoluteDone = false;
	bForwardThreadWait = true;
	ZeroMemory(&mainVideoIndex, sizeof(struct IndexHdr));
	scenarioName = "Default";
	numVideo = 0;
	fps = 30;
	bStopPlayerAtEnd = false;
	curContext->SetFileContextNumber(fileNumber);
	pRecBufferManager = DBG_NEW RecordBufferManager();
	fileContextMap.insert(pair<string, FileContext *>(currentFile, curContext));
	curContext->SetRecordBufferManager(pRecBufferManager);
	curContext->CreateFileContext(fileName);
	playerWindow = NULL;
	bThumbsDone = false;
	bRewind = false;
	bStepForward = false;
	bStepRewind = false;
	bForward = false;
	bStartPlay = false;
	bMoveAbsolute = false;
	bExitThreads = false;
	bHeaderReads = false;
	pWavPlayer = NULL;
	//bForwardThreadIsRunning = false;
	//bRewindThreadIsRunning = false;
	rewfwdMultiplier = 0;
	nPictureDisplayEventIndex = 0;
	bStepRewFwdThreadOn = false;
	bRewFwdThreadOn = false;
	bPlayerMode = false;
}

void PlayerDataFileManager::SetPlayerMode(bool value)
{
	bPlayerMode = value;
}

bool PlayerDataFileManager::GetPlayerMode()
{
	return bPlayerMode;
}

string PlayerDataFileManager::ReadProcessValue()
{
	return curContext->ReadProcessValue();
}

void PlayerDataFileManager::WriteProcessValue(string val)
{
	curContext->WriteProcessValue(val);
}

void PlayerDataFileManager::AddNewFileContext(char *fileName)
{
	spdlog::info("Adding New File Context");

	SaveContext(curContext);
	currentFile = fileName;
	curContext = DBG_NEW FileContext;
	fileNumber++;

	numVideo = 0;
	ZeroMemory(&mainVideoIndex, sizeof(struct IndexHdr));
	bDataReadThreadWait = false;
	bVideoThreadWait = false;
	bRewindThreadWait = true;
	bMoveAbsoluteThreadWait = true;
	bForwardThreadWait = true;
	bStartPlay = false;
	bExitThreads = false;
	bHeaderReads = false;
	bRewind = false;
	bForward = false;
	bStepRewind = false;
	bStepForward = false;
	bMoveAbsolute = false;
	//bForwardThreadIsRunning = false;
	//bRewindThreadIsRunning = false;
	bThumbsDone = false;
	ZeroMemory(&currPositionMarker, sizeof(struct positionMarker));
	rewfwdMultiplier = 0;
	curContext->SetFileContextNumber(fileNumber);
	fileContextMap.insert(pair<string, FileContext *>(currentFile, curContext));
	curContext->SetRecordBufferManager(pRecBufferManager);
	curContext->CreateFileContext(fileName);

	// This is initialization from the ReadData Thread
	numVideo = 0;
	struct VideoHdr videoHeader;

	ZeroMemory(&videoHeader, sizeof(struct VideoHdr));
	curContext->GetVideoHeaderData(&videoHeader);
	fps = videoHeader.fps;
	playerWindow->SetFramesPerSecond(fps);

	// Read the Video Index Record
	curContext->GetNextVideoIndex(&mainVideoIndex);
	bHeaderReads = true; // Set flag for Header reads so that other threads can start;
	list <BPMNElement *> *pDiagram = curContext->GetBPMNDiagram();
	playerWindow->SetDiagram(pDiagram);
	map <ULONG, struct eventListElement *> *pEventMap = curContext->GetElementMap();
	playerWindow->SetEventMap(pEventMap);
	curContext->SetEventMap(pEventMap);

	long long totalTime = curContext->InitializeVideoPath();
	playerWindow->SetPlayerEndTime((ULONG)(totalTime / 1000L));
	playerWindow->SetPlayerTimeWindow();
	curContext->SetActiveMonitorDisplay(playerWindow->GetActiveMonitorDisplayFlag());
	curContext->SetPlayerWindowHeight(playerWindow->GetWindowHeight());
	curContext->SetPlayerWindowWidth(playerWindow->GetWindowWidth());
	curContext->CreateBitmapMerge();
	//MoveAbsolute();
	playerWindow->SetSliderPosition(0L);
	playerWindow->SetSegmentDetails(false, false, 0L, 0L);
	SaveContext(curContext);
	RestoreContext(curContext);
	spdlog::info("Adding New File Context Done");
	
}

void PlayerDataFileManager::SaveContext(FileContext *context)
{
	spdlog::info("Saving File Context");
	struct fileContext *ctxData = context->GetContext();

	ctxData->numVideo = numVideo;
	CopyMemory(&ctxData->mainVideoIndex, &mainVideoIndex, sizeof(struct IndexHdr));
	ctxData->bDataReadThreadWait =                           bDataReadThreadWait;
	ctxData->bVideoThreadWait =							bVideoThreadWait;
	ctxData->bRewindThreadWait = 							bRewindThreadWait;
	ctxData->bMoveAbsoluteThreadWait = 					bMoveAbsoluteThreadWait;
	ctxData->bForwardThreadWait =							bForwardThreadWait;
	ctxData->bStartPlay =									bStartPlay;
	ctxData->bExitThreads =								bExitThreads;
	ctxData->bHeaderReads =								bHeaderReads;
	ctxData->bRewind =									bRewind;
	ctxData->bForward =									bForward;
	ctxData->bStepRewind =								bStepRewind;
	ctxData->bStepForward =								bStepForward;
	ctxData->bMoveAbsolute =								bMoveAbsolute;
	CopyMemory(&ctxData->currPositionMarker, &currPositionMarker, sizeof(struct positionMarker));
	ctxData->rewfwdMultiplier = rewfwdMultiplier;
	ctxData->sliderPosition = playerWindow->GetSliderPosition();
	ctxData->fps = playerWindow->GetFramesPerSecond();
	ctxData->bSegInProgress = playerWindow->GetSegInProgress();
	ctxData->bSegPresent = playerWindow->GetSegPresent();
	ctxData->startPos = playerWindow->GetSegStartPos();
	ctxData->endPos = playerWindow->GetSegEndPos();
	ctxData->scenarioName = scenarioName;
	spdlog::info("Saving File Context Done");
}

void PlayerDataFileManager::RestoreContext(FileContext *context)
{
	struct fileContext *ctxData = context->GetContext();
	spdlog::info("Restoring File Context");

	numVideo = ctxData->numVideo;
	CopyMemory(&mainVideoIndex, &ctxData->mainVideoIndex, sizeof(struct IndexHdr));
	bDataReadThreadWait = ctxData->bDataReadThreadWait;
	bVideoThreadWait = ctxData->bVideoThreadWait;
	bRewindThreadWait = ctxData->bRewindThreadWait;
	bMoveAbsoluteThreadWait = ctxData->bMoveAbsoluteThreadWait;
	bForwardThreadWait = ctxData->bForwardThreadWait;
	bStartPlay = ctxData->bStartPlay;
	bExitThreads = ctxData->bExitThreads;
	bHeaderReads = ctxData->bHeaderReads;
	bRewind = ctxData->bRewind;
	bForward = ctxData->bForward;
	bStepRewind = ctxData->bStepRewind;
	bStepForward = ctxData->bStepForward;
	bMoveAbsolute = ctxData->bMoveAbsolute;
	scenarioName  = ctxData->scenarioName;
	//bForwardThreadIsRunning = ctxData->bForwardThreadIsRunning;
	//bRewindThreadIsRunning = ctxData->bRewindThreadIsRunning;
	CopyMemory(&currPositionMarker, &ctxData->currPositionMarker, sizeof(struct positionMarker));
	rewfwdMultiplier = ctxData->rewfwdMultiplier;
	playerWindow->SetSliderPosition(ctxData->sliderPosition);
	playerWindow->SetFramesPerSecond(ctxData->fps);
	playerWindow->SetSegmentDetails(ctxData->bSegInProgress, ctxData->bSegPresent, ctxData->startPos, ctxData->endPos);

	spdlog::info("Restoring File Context Done");
}

PlayerDataFileManager::~PlayerDataFileManager()
{
	if (bStartPlay)
		StopPlayer();
	if (pWavPlayer != NULL)
		delete pWavPlayer;

	// Delete the file context map
	map<string, FileContext *>::iterator itFileContext = fileContextMap.begin();
	while (itFileContext != fileContextMap.end())
	{
		FileContext *pFileContext = itFileContext->second;
		delete pFileContext;
		itFileContext++;
	}
	fileContextMap.clear();

	delete pRecBufferManager;
	pRecBufferManager = NULL;
	ClearGamma();
}

bool PlayerDataFileManager::IsSegmentSelectedInOtherContexts(int num)
{
	map <string, FileContext *>::iterator itContextMap = fileContextMap.begin();

	while (itContextMap != fileContextMap.end())
	{
		FileContext *pContext = itContextMap->second;
		fileContext *pfContext = pContext->GetContext();
		if (pContext->GetFileContextNumber() != num)
		{
			if (pfContext->bSegPresent)
				return true;
		}
		itContextMap++;
	}
	return false;
}

bool PlayerDataFileManager::IsSegmentSelectedInSameContext(int num)
{
	map <string, FileContext *>::iterator itContextMap = fileContextMap.begin();

	while (itContextMap != fileContextMap.end())
	{
		FileContext *pContext = itContextMap->second;
		fileContext *pfContext = pContext->GetContext();
		if (pContext->GetFileContextNumber() == num)
		{
			if (pfContext->bSegPresent)
				return true;
		}
		itContextMap++;
	}
	return false;
}

bool PlayerDataFileManager::IsSegmentCopiedInAnyContext()
{
	map <string, FileContext *>::iterator itContextMap = fileContextMap.begin();

	while (itContextMap != fileContextMap.end())
	{
		FileContext *pContext = itContextMap->second;
		fileContext *pfContext = pContext->GetContext();
		if (pfContext->bSegCopied)
			return true;
		itContextMap++;
	}
	return false;
}

int PlayerDataFileManager::ProcessVideoRecord(struct bufferRecord *pBuffer, int currFileNum, int currChannelNum, int lastMon)
{
	// Search the File context object for the file number

	FileContext *fileContext = GetFileContext(currFileNum);
	if (fileContext != NULL)
		return fileContext->ProcessVideoRecord(pBuffer, currChannelNum, lastMon);
	return 0;

}

void PlayerDataFileManager::UpdateChannelHeader(int channelNum)
{
	curContext->UpdateChannelHeader(channelNum);
}

bool PlayerDataFileManager::IsBranchRight(ULONG nEventIndex)
{
	return curContext->IsBranchRight(nEventIndex);
}

long long PlayerDataFileManager::GetEventVideoTime(ULONG nEventIndex)
{
	long long nTime = curContext->SynchronizeVideo(nEventIndex);
	return nTime;
}

void PlayerDataFileManager::SynchronizeVideo(ULONG nEventIndex)
{
	spdlog::info("Synchronizing Video");
	SetRecorderStop();
	long long nTime = curContext->SynchronizeVideo(nEventIndex);
	if (nTime > 0L)
	{
		playerWindow->SetSliderPosition((ULONG)(nTime/1000L));
		MoveAbsolute();
	}
	spdlog::info("Synchronizing Video Done");
}

void PlayerDataFileManager::BranchBottom(ULONG nEventIndex)
{
	SetRecorderStop();
	curContext->SetBranchBottom(nEventIndex);
}

void PlayerDataFileManager::BranchRight(ULONG nEventIndex)
{
	SetRecorderStop();
	curContext->SetBranchRight(nEventIndex);
}


void PlayerDataFileManager::SwitchFileContext(int oldIndex, int newIndex)
{
	spdlog::info("Switching File Context");

	if (oldIndex == newIndex)
		return;

	FileContext *oldFileContext = GetFileContext(oldIndex + 1);
	FileContext *newFileContext = GetFileContext(newIndex + 1);

	if (oldFileContext == NULL || newFileContext == NULL)
		return;

	SaveContext(curContext);
	curContext = newFileContext;
	RestoreContext(curContext);

	list <BPMNElement *> *pDiagram = curContext->GetBPMNDiagram();
	playerWindow->SetDiagram(pDiagram);
	map <ULONG, struct eventListElement *> *pEventMap = curContext->GetElementMap();
	playerWindow->SetEventMap(pEventMap);
	curContext->SetEventMap(pEventMap);
	long long totalTime = curContext->InitializeVideoPath();
	playerWindow->SetPlayerEndTime((ULONG)(totalTime / 1000L));
	playerWindow->SetPlayerTimeWindow();
	spdlog::info("Switch File Context Done");
}

int PlayerDataFileManager::GetContextFileNumber()
{
	return curContext->GetFileContextNumber();
}

bool PlayerDataFileManager::GetContextSegOn()
{
	fileContext *pFContext = curContext->GetContext();
	return pFContext->bSegInProgress;
}


bool PlayerDataFileManager::GetContextSegCopy()
{
	fileContext *pFContext = curContext->GetContext();
	return pFContext->bSegCopied;
}

FileContext *PlayerDataFileManager::GetFileContext(int currFileNum)
{
	map<string, FileContext *>::iterator it = fileContextMap.begin();
	FileContext *fileContext = NULL;

	while (it != fileContextMap.end())
	{
		fileContext = it->second;
		if (fileContext->GetFileContextNumber() == currFileNum)
			break;
		else
			fileContext = NULL;
		it++;
	}
	
	return fileContext;
}

void PlayerDataFileManager::StopPlayer()
{
	spdlog::info("Stopping Player");

	lockPlayerManager.lock();
	bExitThreads = true;
	bStartPlay = false;
	bRewind = false;
	bForward = false;
	bStepRewind = false;
	bStepForward = false;
	lockPlayerManager.unlock();
	playerWindow->StopPlay();
	playerWindow->SetExitFlag(true);
	pWavPlayer->SetExitFlag(true);

	Sleep(1000);
	ExecutionManager *pExecManager = GetExecutionManager();
	pExecManager->StopVideoThread();
	pExecManager->StopAudioThread();
	pExecManager->StopDataFileThread();
	pExecManager->StopDisplayThread();
	pExecManager->WaitForThreadsToStop();
	delete pWavPlayer;
	pWavPlayer = NULL;
	spdlog::info("Player Stopped");

}

void PlayerDataFileManager::StartPlayer()
{
	spdlog::info("Starting Player");
	pWavPlayer = DBG_NEW WavePlayer();
	lockPlayerManager.lock();
	bExitThreads = false;
	bStartPlay = false;
	bRewind = false;
	bForward = false;
	bStepRewind = false;
	bStepForward = false;
	lockPlayerManager.unlock();

	pRecBufferManager->CreateFreeResizeScreenBufferQueue(playerWindow->GetWindowWidth(), playerWindow->GetWindowHeight(), 30);
	curContext->SetRecordBufferManager(pRecBufferManager);
	curContext->SetActiveMonitorDisplay(playerWindow->GetActiveMonitorDisplayFlag());
	curContext->SetPlayerWindowHeight(playerWindow->GetWindowHeight());
	curContext->SetPlayerWindowWidth(playerWindow->GetWindowWidth());
	curContext->CreateBitmapMerge();

	playerWindow->SetDataManager(this);
//	playerWindow->SetAudioPlayerBuffer(curContext->GetAudioPlayerBuffer());
	ExecutionManager *pExecManager = GetExecutionManager();
	pExecManager->StartDataFileThread();
	while (true)
	{
		lockPlayerManager.lock();
		bool bRead = bHeaderReads;
		lockPlayerManager.unlock();
		if (bRead)
			break;
		Sleep(100);
	}
	pExecManager->StartVideoThread();
	pExecManager->StartAudioThread();
	pExecManager->StartDisplayThread();
	
	spdlog::info("Player Started");
}

bool PlayerDataFileManager::IsSegmentOn()
{
	fileContext *pFContext = curContext->GetContext();
	return pFContext->bSegInProgress;
}

void PlayerDataFileManager::SetCurrentPositionMarker(long long lastfPosVideoPlayed, ULONG lastVideoIndexNum, USHORT fileNumr, USHORT channelNum)
{
	currPositionMarker.fPosIndex = lastfPosVideoPlayed;
	currPositionMarker.indexNum = lastVideoIndexNum;
	currPositionMarker.channelNum = channelNum;
	currPositionMarker.fileNum = fileNumr;
}

bool PlayerDataFileManager::IsEventDataPresent()
{
	map <ULONG, struct eventListElement *> *pEventMap = curContext->GetElementMap();
	if (pEventMap->size() == 0)
		return false;

	return true;
}

void PlayerDataFileManager::StartSegment()
{
	spdlog::info("Start New Segment");

	lockPlayerManager.lock();
	bool bIsPlaying = bStartPlay;
	lockPlayerManager.unlock();
	if (!bIsPlaying)
	{
		ClearAllContextSegments();
		fileContext *pFContext = curContext->GetContext();
		pFContext->bSegInProgress = true;
		playerWindow->VideoSegmentStartTime();
		// Save the recording position
		curContext->StartSegment(&currPositionMarker);
	}
}

void PlayerDataFileManager::ClearAllContextSegments()
{
	map <string, FileContext *>::iterator itContextMap = fileContextMap.begin();

	while (itContextMap != fileContextMap.end())
	{
		FileContext *pContext = itContextMap->second;
		fileContext *pfContext = pContext->GetContext();
		pfContext->bSegInProgress = pfContext->bSegPresent = false;
		pfContext->bSegCopied = false;
		pfContext->startPos = pfContext->endPos = 0L;
		itContextMap++;
	}
}

void PlayerDataFileManager::EndSegment()
{
	spdlog::info("End Segment");
	lockPlayerManager.lock();
	bool bIsPlaying = bStartPlay;
	lockPlayerManager.unlock();
	if (!bIsPlaying)
	{
		playerWindow->VideoSegmentEndTime();
		// Save the recording position
		curContext->EndSegment(&currPositionMarker);
		fileContext *pFContext = curContext->GetContext();
		pFContext->bSegInProgress = false;

	}
}

void PlayerDataFileManager::SetPlayerToStart()
{
	playerWindow->SetSliderPosition(0L);
}

void PlayerDataFileManager::InitializeVideoPath()
{
	long long totalTime = curContext->InitializeVideoPath();
	playerWindow->SetPlayerEndTime((ULONG)(totalTime / 1000L));
	playerWindow->SetPlayerTimeWindow();
	MoveAbsolute();
}

void PlayerDataFileManager::StartAudio()
{
	spdlog::info("Start Audio Player");
	pWavPlayer = DBG_NEW WavePlayer();
	struct AudioHdr audioHeader;
	pWavPlayer->SetDataManager(this);
	pWavPlayer->SetRecordBufferManager(pRecBufferManager);
	curContext->GetAudioHeaderData(&audioHeader);
	pWavPlayer->SetAudioParameters(audioHeader.freq,
		audioHeader.noChannels,
		audioHeader.bitsPerSample);
	pWavPlayer->SetAudioPlayerBuffer(curContext->GetAudioPlayerBuffer());
	GetExecutionManager()->StartAudioThread();
	pWavPlayer->SetStartFlag(true);
	spdlog::info("Start Audio Player Done");
}

void PlayerDataFileManager::StopAudio()
{
	spdlog::info("Stop Audio Player");

	if (pWavPlayer != NULL)
	{
		pWavPlayer->SetExitFlag(true);
		while (!pWavPlayer->IsPlayerStopped())
			;
		GetExecutionManager()->StopAudioThread();
		delete pWavPlayer;
	}
	pWavPlayer = NULL;
	spdlog::info("Stop Audio Player Done");

}

void PlayerDataFileManager::ClearSegment()
{
	spdlog::info("Clear Segment");
	fileContext *pFContext = curContext->GetContext();
	pFContext->bSegCopied = false;
	curContext->ClearSegment(&copySegmentInfo);
	playerWindow->ResetSegmentMarker();
}

void PlayerDataFileManager::CutSegment()
{
	spdlog::info("Cut Segment");
	lockPlayerManager.lock();
	bool bIsPlaying = bStartPlay;
	lockPlayerManager.unlock();

	if (bIsPlaying)
		return;
	long long startTime = curContext->CutSegment(&copySegmentInfo);
	fileContext *pFContext = curContext->GetContext();
	pFContext->bSegCopied = false;
	playerWindow->SetSliderPosition((ULONG)(startTime/1000L));
	ResetPlayerScreen();
}

void PlayerDataFileManager::CopySegment()
{
	spdlog::info("Copy Segment");
	lockPlayerManager.lock();
	bool bIsPlaying = bStartPlay;
	lockPlayerManager.unlock();

	if (bIsPlaying)
		return;
	fileContext *pFContext = curContext->GetContext();
	pFContext->bSegCopied = true;
	curContext->CopySegment(&copySegmentInfo);
}

void PlayerDataFileManager::PasteSegment()
{
	spdlog::info("Paste Segment");
	lockPlayerManager.lock();
	bool bIsPlaying = bStartPlay;
	lockPlayerManager.unlock();
	if (!bIsPlaying)
	{
		// Save the recording position
		fileContext *pFContext = curContext->GetContext();
		pFContext->bSegCopied = false;
		curContext->DestinationSegment(&currPositionMarker);
		long long startTime = curContext->PasteSegment(&copySegmentInfo);
		playerWindow->SetSliderPosition((ULONG)(startTime / 1000L));
		ResetPlayerScreen();
		

	}
}

void PlayerDataFileManager::BranchDelete(ULONG nEventIndex, bool bDelEntire)
{
	SetRecorderStop();
	fileContext *pFContext = curContext->GetContext();
	pFContext->bSegCopied = false;

	curContext->BranchDelete(nEventIndex, bDelEntire);
	if (!bDelEntire)
		ResetPlayerScreen();
}

void PlayerDataFileManager::BranchCopy(ULONG nEventIndex)
{
	SetRecorderStop();
	fileContext *pFContext = curContext->GetContext();
	pFContext->bSegCopied = false;

	curContext->BranchCopy(nEventIndex, &copySegmentInfo);
//	playerWindow->SetSliderPosition((ULONG)(startTime / 1000L));
	ResetPlayerScreen();
}

void PlayerDataFileManager::CreateBranch()
{
	spdlog::info("Create Branch");
	lockPlayerManager.lock();
	bool bIsPlaying = bStartPlay;
	lockPlayerManager.unlock();

	if (bIsPlaying)
		return;
	fileContext *pFContext = curContext->GetContext();
	pFContext->bSegCopied = false;

	long long startTime = curContext->CreateBranch(&copySegmentInfo);
	playerWindow->SetSliderPosition((ULONG)(startTime / 1000L));
	ResetPlayerScreen();
}

void PlayerDataFileManager::AddNewEventRecord(struct eventListElement *pEventListElement)
{
	curContext->AddNewEventRecord(pEventListElement);
}

//void PlayerDataFileManager::RewriteEventRecord(struct eventListElement *pEventListElement)\
//{
//	curContext->RewriteEventRecord(pEventListElement);
//}

void PlayerDataFileManager::SetEventRoot(struct eventListElement *pEventListElement)
{
	curContext->SetEventRoot(pEventListElement);
}

ULONG PlayerDataFileManager::FindEventIndex(long long fPosVideoPlayed, int videoIndexNum)
{
	return curContext->FindEventIndex(fPosVideoPlayed, videoIndexNum);
}

void PlayerDataFileManager::UpdateEventRecord(struct eventListElement *pEventListElement)
{
	curContext->UpdateEventRecord(pEventListElement, NULL);
}

LONG PlayerDataFileManager::GetChannelNextEventID(int channelNum)
{
	ChannelDataMapping *pChannel = curContext->GetChannelDataMapping(channelNum);
	if (pChannel == NULL)
		return 0L;
	struct channelHdr *pChannelHdr = GetChannelHeader(0);
	if (pChannelHdr == NULL)
		return -1L;
	pChannelHdr->lastIndexNumber++;
	ULONG indexVal =  pChannelHdr->lastIndexNumber;
	return indexVal;
}


struct channelHdr * PlayerDataFileManager::GetChannelHeader(int channelNum)
{
	ChannelDataMapping *pChannel = curContext->GetChannelDataMapping(channelNum);
	if (pChannel == NULL)
		return NULL;

	struct channelHdr *pChannelHdr = pChannel->GetChannelHeader();
	return pChannelHdr;
}

void PlayerDataFileManager::SaveEventDetails(ULONG eventIndex)
{
	curContext->SaveEventDetails(eventIndex);
}

void PlayerDataFileManager::GetPointOffset(POINT *pPoint)
{
	if (playerWindow != NULL)
		playerWindow->GetPointOffset(pPoint);
}

void PlayerDataFileManager::ResetPlayerScreen()
{
	// Show Message
	// recalculate the total time
	long long totalTime = curContext->InitializeVideoPath();
	playerWindow->SetPlayerEndTime((ULONG)(totalTime / 1000L));
	playerWindow->ResetSegmentMarker();
	
	// Reset Static
	playerWindow->SetPlayerTimeWindow();
	// Reset BPMNControl
	list <BPMNElement *> *pDiagram = curContext->GetBPMNDiagram();
	map <ULONG, struct eventListElement *> *pEventMap = curContext->GetElementMap();
	playerWindow->SetDiagram(pDiagram);
	playerWindow->SetEventMap(pEventMap);
	curContext->SetEventMap(pEventMap);
	// Reset Tab Control
	// Reset Segment Data
	ZeroMemory(&currPositionMarker, sizeof(struct positionMarker));
	MoveAbsolute();
}


void PlayerDataFileManager::CreateScreenResizeBuffer(int width, int height)
{
	curContext->CreateScreenResizeBuffer(playerWindow->GetActiveMonitorDisplayFlag(), width, height);
}



void PlayerDataFileManager::InsertEvent()
{
	spdlog::info("Insert New Event");
	struct IndexHdr currPosIndex;
	curContext->GetChannelVideoIndex(currPositionMarker.channelNum, currPositionMarker.fPosIndex, &currPosIndex);
	int indexNum = currPosIndex.dataRecs[currPositionMarker.indexNum].videoIndexNumber + 1;
	long long timeStamp = currPosIndex.dataRecs[currPositionMarker.indexNum].timestamp;
	playerWindow->InsertEvent(indexNum, timeStamp);
}

bool PlayerDataFileManager::GetRecorderStatus()
{
	return bStartPlay;
}

void PlayerDataFileManager::SetRecorderPlay()
{
	spdlog::info("Play File");

	SetRecorderStop(); // First Stop and then start play
	lockPlayerManager.lock();
	bStartPlay = true;
	lockPlayerManager.unlock();
	if (pWavPlayer != NULL)
		pWavPlayer->SetStartFlag(true);
	if (playerWindow != NULL)
		playerWindow->SetStartFlag(true);
}

//void PlayerDataFileManager::WaitForThumbsDone()
//{
//	while (true)
//	{
//		lockPlayerManager.lock();
//		bool bThumbsDoneFlg = bThumbsDone;
//		lockPlayerManager.unlock();
//		if (bThumbsDoneFlg)
//			break;
//	}
//}

void PlayerDataFileManager::SetRecorderStop()
{
	spdlog::info("Stop Play");
	lockPlayerManager.lock();
	bStartPlay = false;
	bool bRewindFlag = bRewind;
	//bool bFwdFlag = bForward;
	//bool bStepRewindFlag = bStepRewind;
	//bool bStepFwdFlag = bStepForward;
	bool bRewFwdThreadFlg = bRewFwdThreadOn;
	bool bStepRewFwdThreadFlg = bStepRewFwdThreadOn;
	bRewind = false;
	bForward = false;
	bStepRewind = false;
	bStepForward = false;
	bRewFwdThreadOn = false;
	bStepRewFwdThreadOn = false;
	rewfwdMultiplier = 0;
	lockPlayerManager.unlock();

	if (bRewFwdThreadFlg)
	{
		ExecutionManager *pExecManager = GetExecutionManager();
		pExecManager->StopRewindForwardThread();
	}

	if (bStepRewFwdThreadFlg)
	{
		ExecutionManager *pExecManager = GetExecutionManager();
		pExecManager->StopStepRewindForwardThread();
	}


	if (pWavPlayer != NULL)
		pWavPlayer->SetStartFlag(false);
	if (playerWindow != NULL)
		playerWindow->SetStartFlag(false);
	while (true)
	{
		lockPlayerManager.lock();
		bool bDataReadThreadWaitChk = bDataReadThreadWait;
		bool bVideoThreadWaitChk = playerWindow->IsVideoThreadWaiting();
		bool bRewindThreadWaitChk = bRewindThreadWait;
		bool bMoveAbsoluteThreadWaitChk = bMoveAbsoluteThreadWait;
		bool bForwardThreadWaitChk = bForwardThreadWait;
		lockPlayerManager.unlock();
		if (bDataReadThreadWaitChk && bVideoThreadWaitChk && bRewindThreadWaitChk && bMoveAbsoluteThreadWaitChk && bForwardThreadWaitChk)
			break;
	}

	bRewindFlag = false;

}

void PlayerDataFileManager::PlayAudio()
{
	spdlog::info("Start Audio Play");

	lockPlayerManager.lock();
	bool bPlay = bStartPlay;
	bool bQuit = bExitThreads;
	lockPlayerManager.unlock();

	pWavPlayer->SetStartFlag(bPlay);
	pWavPlayer->SetExitFlag(bQuit);

	struct AudioHdr audioHeader;
	ZeroMemory(&audioHeader, sizeof(struct AudioHdr));
	curContext->GetAudioHeaderData(&audioHeader);
	pWavPlayer->StartPlay();
}

void PlayerDataFileManager::PlayVideo()
{
	spdlog::info("Start Video Play");
	lockPlayerManager.lock();
	bool bPlay = bStartPlay;
	bool bQuit = bExitThreads;
	lockPlayerManager.unlock();

	playerWindow->SetStartFlag(bPlay);
	playerWindow->SetExitFlag(bQuit);

	struct VideoHdr videoHeader;
	ZeroMemory(&videoHeader, sizeof(struct VideoHdr));
	curContext->GetVideoHeaderData(&videoHeader);
	fps = videoHeader.fps;
	playerWindow->SetFramesPerSecond(fps);
	//playerWindow->StartPlay(videoHeader.fPosRoot);
	//playerWindow->StartPlay(videoHeader.fPosRoot);
	playerWindow->StartPlay();
}

void PlayerDataFileManager::SetSubProcessTab()
{
	//->SetSubProcessTab();
}

void PlayerDataFileManager::SetTaskTab()
{
	//playerWindow->SetTaskTab();
}

void PlayerDataFileManager::DisplayVideo()
{
	lockPlayerManager.lock();
	bool bPlay = bStartPlay;
	bool bQuit = bExitThreads;
	lockPlayerManager.unlock();

	playerWindow->SetStartFlag(bPlay);
	playerWindow->SetExitFlag(bQuit);
	playerWindow->StartDisplay();
}

void PlayerDataFileManager::MoveAbsolute()
{
	spdlog::info("Start of Move Video Absolute");

	lockPlayerManager.lock();
	bool bPlay = bStartPlay;
	lockPlayerManager.unlock();

	SetRecorderStop();
	bMoveAbsolute = true;
	bMoveAbsoluteThreadWait = false;
	playerWindow->SetPlayerTimeWindow();
	ExecutionManager *pExecManager = GetExecutionManager();
	pExecManager->StartMoveAbsoluteThread();
	pExecManager->StopMoveAbsoluteThread();
	bMoveAbsoluteThreadWait = true;
	if (bPlay)
	{
		SetRecorderPlay();
	}
	bMoveAbsolute = false;
	bMoveAbsoluteDone = true;
	spdlog::info("Move Absolute Done");
}

void PlayerDataFileManager::GetEventImage(long long nTime, vector<unsigned char>& buffer, ULONG& cx, ULONG& cy, ULONG nEventIndex, vector <BPMNElement *> &pElementList)
{
	long long fPos = 0L;
	int nVideo = 0;
	IndexHdr videoIndex;

	ZeroMemory(&videoIndex, sizeof(IndexHdr));

	int nChannel = curContext->GetVideoAtTime(nTime/1000, &videoIndex, &nVideo, &fPos);
	curContext->GetEventImage(nChannel, &videoIndex, nVideo, fPos, buffer, cx, cy, nEventIndex, pElementList);


}


void PlayerDataFileManager::MoveVideoAbsolute()
{
	spdlog::info("Move Absolute Thread Function");
	Sleep(100);
	//WaitForThumbsDone();
	ULONG absPos = playerWindow->GetSliderPosition();
	if (absPos < 0L)
		absPos = 0L;
	if (pRecBufferManager != NULL)
		pRecBufferManager->ClearPlayBuffer();
	long long fPos = 0L;
	int nChannel = curContext->GetVideoAtTime(absPos, &mainVideoIndex, &numVideo, &fPos);

	curContext->CreateVideoImage(nChannel, &mainVideoIndex, numVideo, fPos);
	playerWindow->DisplayImage();
	playerWindow->SetSliderPosition(absPos);
	//playerWindow->SetPlayerTimeWindow();
	spdlog::info("End of Move Absolute Thread Function");
}

void PlayerDataFileManager::ForwardStepVideo()
{
	spdlog::info("Step Forward Video");
	lockPlayerManager.lock();
	bool bStepForwardFlag = bStepForward;
	lockPlayerManager.unlock();

	if (!bStepForwardFlag)
	{
		SetRecorderStop();
		Sleep(100); // wait for 100 ms
		lockPlayerManager.lock();
		bStepForward = true;
		bStepRewFwdThreadOn = true;
		//bForwardThreadIsRunning = true;
		lockPlayerManager.unlock();
		if (pRecBufferManager != NULL) {
			pRecBufferManager->ClearPlayBuffer();
		}
		ExecutionManager *pExecManager = GetExecutionManager();
		pExecManager->StartStepRewindForwardThread();
	}
	spdlog::info("End of Step Forward Video");

}

void PlayerDataFileManager::ReverseStepVideo()
{
	lockPlayerManager.lock();
	bool bStepRewindFlag = bStepRewind;
	lockPlayerManager.unlock();

	spdlog::info("Step Reverse Video");

	if (!bStepRewindFlag)
	{
		SetRecorderStop();
		Sleep(100); // wait for 100 ms
		lockPlayerManager.lock();
		bStepRewind = true;
		bStepRewFwdThreadOn = true;
		lockPlayerManager.unlock();
		if (pRecBufferManager != NULL) {
			pRecBufferManager->ClearPlayBuffer();
			MoveToCurrentVideoIndexNumber();
		}
		ExecutionManager *pExecManager = GetExecutionManager();
		pExecManager->StartStepRewindForwardThread();
	}
	spdlog::info("End of Step Reverse Video");

}


void PlayerDataFileManager::StepRewindForwardVideo()
{
	//char strMessage[100];
	//long long prevTimeDiff = 0;
	//long timeDiffVal = 0L;
	lockPlayerManager.lock();
	//bRewindThreadIsRunning = true;
	bRewindThreadWait = false;
	bForwardThreadWait = false;
	lockPlayerManager.unlock();
	rewfwdMultiplier = 0;
	long long maxPosition = curContext->InitializeVideoPath() / 1000L;
	spdlog::info("Step Video Thread Function");


	long long fPos = 0L;
	long absPos = playerWindow->GetSliderPosition();
	ULONG newTime = absPos;
	int nChannel = curContext->GetVideoAtTime(newTime, &mainVideoIndex, &numVideo, &fPos);
	while (true)
	{
		lockPlayerManager.lock();
		bool bRewindFlag = bStepRewind;
		bool bForwardFlag = bStepForward;
		lockPlayerManager.unlock();
		if (!bRewindFlag && !bForwardFlag)
			break;
		if (bRewindFlag)
		{
			nChannel = curContext->StepVideo(-1, &mainVideoIndex, &numVideo, &fPos);
		}
		else if (bForwardFlag)
		{
			nChannel = curContext->StepVideo(1, &mainVideoIndex, &numVideo, &fPos);
		}
		curContext->CreateVideoImage(nChannel, &mainVideoIndex, numVideo, fPos);
		long long imageTime = curContext->GetVideoIndexTime(numVideo, fPos);
		newTime = (ULONG)(imageTime / 1000L);
		playerWindow->DisplayImage();
		playerWindow->SetSliderPosition(newTime);
		playerWindow->SetPlayerTimeWindow();
		Sleep(200);
		if (newTime == 0L || newTime == maxPosition)
		{
			lockPlayerManager.lock();
			bStepRewind = false;
			bStepForward = false;
			bStopPlayerAtEnd = true;
			lockPlayerManager.unlock();
			CheckIfStopPlayer();
			//SendStopPlayMessage();
		}
	}
	lockPlayerManager.lock();
	//bRewindThreadIsRunning = false;
	bRewindThreadWait = true;
	bForwardThreadWait = true;
	lockPlayerManager.unlock();

	spdlog::info("End of Step Video Thread Function");
}


void PlayerDataFileManager::RewindForwardVideo()
{
	//long long prevTimeDiff = 0;
	long timeDiffVal = 0L;
	lockPlayerManager.lock();
	bRewindThreadWait = false;
	bForwardThreadWait = false;
	//bRewindThreadIsRunning = true;
	lockPlayerManager.unlock();
	rewfwdMultiplier = 0;
	//WaitForThumbsDone();

	long long maxPosition = curContext->InitializeVideoPath()/1000L;
	spdlog::info("Rewind / Forward Thread Function");

	while (true)
	{
	
		lockPlayerManager.lock();
		timeDiffVal = 5000L/fps * ((rewfwdMultiplier & 0x03) + 1);
		//bool bRewindFlag = bRewind;
		//bool bForwardFlag = bForward;
		lockPlayerManager.unlock();
		if (!bForward && !bRewind)
			break;

		long absPos = playerWindow->GetSliderPosition();
		ULONG newTime = 0;
		if (bRewind)
		{
			if (absPos > timeDiffVal)
				newTime = absPos - timeDiffVal;
			else
				newTime = 0L;
		}
		else if (bForward)
		{
			newTime = absPos + timeDiffVal;
			if (newTime > maxPosition)
				newTime = maxPosition;
		}
		long long fPos = 0L;
		int nChannel = curContext->GetVideoAtTime(newTime, &mainVideoIndex, &numVideo, &fPos);
		curContext->CreateVideoImage(nChannel, &mainVideoIndex, numVideo, fPos);
		playerWindow->DisplayImage();
		playerWindow->SetSliderPosition(newTime);
		playerWindow->SetPlayerTimeWindow();
		Sleep(100);
		if (newTime == 0L || newTime == maxPosition)
		{
			lockPlayerManager.lock();
			bRewind = false;
			bForward = false;
			bStopPlayerAtEnd = true;
			lockPlayerManager.unlock();
			CheckIfStopPlayer();
			// SetSSetRecorderStop();
		}

	}
	lockPlayerManager.lock();
	//bRewindThreadIsRunning = false;
	bRewindThreadWait = true;
	bForwardThreadWait = true;

	lockPlayerManager.unlock();
	spdlog::info("End of Rewind / Forward Thread Function");

}

void PlayerDataFileManager::RewindVideo()
{
	lockPlayerManager.lock();
	bool bRewindFlag = bRewind;
	lockPlayerManager.unlock();
	spdlog::info("Rewind Video");

	if (!bRewindFlag)
	{
		SetRecorderStop();
		Sleep(100); // wait for 100 ms

		lockPlayerManager.lock();
		bRewind = true;
		bRewFwdThreadOn = true;
		//bRewindThreadIsRunning = true;
		lockPlayerManager.unlock();
		if (pRecBufferManager != NULL) {
			pRecBufferManager->ClearPlayBuffer();
			MoveToCurrentVideoIndexNumber();
		}
		ExecutionManager *pExecManager = GetExecutionManager();
		pExecManager->StartRewindForwardThread();
	}
	else
	{
		rewfwdMultiplier++;
		rewfwdMultiplier &= 0x3;
		spdlog::info("Rewind Multiplier {}", rewfwdMultiplier);
	}
	spdlog::info("End of Rewind Video");
}

void PlayerDataFileManager::FastForwardVideo()
{
	lockPlayerManager.lock();
	bool bForwardFlag = bForward;
	lockPlayerManager.unlock();

	spdlog::info("Forward Video");

	if (!bForwardFlag)
	{
		SetRecorderStop();
		Sleep(100); // wait for 100 ms
		lockPlayerManager.lock();
		bForward = true;
		bRewFwdThreadOn = true;
		//bForwardThreadIsRunning = true;
		lockPlayerManager.unlock();
		if (pRecBufferManager != NULL) {
			pRecBufferManager->ClearPlayBuffer();
		}
		ExecutionManager *pExecManager = GetExecutionManager();
		pExecManager->StartRewindForwardThread();
	}
	else
	{
		rewfwdMultiplier++;
		rewfwdMultiplier &= 0x3;
		spdlog::info("Forward Multiplier {}", rewfwdMultiplier);
	}
	spdlog::info("End of Forward Video");

}

void PlayerDataFileManager::ClearThumb()
{
	pRecBufferManager->ClearThumbResizeBuffer();
}

bool PlayerDataFileManager::IsVideoImageMoving()
{
	if (bRewind ||
		bStepForward ||
		bStepRewind ||
		bForward ||
		bStartPlay
		)
		return true;
	return false;
}

void PlayerDataFileManager::SetCurrentEventDisplay(ULONG nEventIndex)
{
	nPictureDisplayEventIndex = nEventIndex;
}

void PlayerDataFileManager::ShowThumb()
{
	struct IndexHdr videoIndex;
	int nVideo = 0;
	long long fPos = 0L;

	ZeroMemory(&videoIndex, sizeof(struct IndexHdr));

	ULONG thumbTime = playerWindow->GetThumbTime();
	int nChannel = curContext->GetThumbVideoAtTime(thumbTime, &videoIndex, &nVideo, &fPos);
	curContext->CreateThumbImage(nChannel, &videoIndex, nVideo, fPos);
}

//void PlayerDataFileManager::SetThumbImageNumber(int thumbNum)
//{
////	playerWindow->SetThumbImageNumber(thumbNum);
//}

void PlayerDataFileManager::ThumbDisplay()
{
//	playerWindow->ThumbDisplay();
}

void PlayerDataFileManager::ShowSliderTime()
{
	playerWindow->SetPlayerTimeWindow();
}

//void PlayerDataFileManager::CheckTabNotifications(NMHDR *pNMHdr)
//{
////	playerWindow->CheckTabNotifications(pNMHdr);
//}

//void PlayerDataFileManager::CheckKillFocus(HWND hWnd)
//{
//	playerWindow->CheckKillFocus(hWnd);
//}

void PlayerDataFileManager::SetTabFields(struct cellDef *pSelected)
{
	playerWindow->SetTabFields(pSelected);
}

void PlayerDataFileManager::SetTabFields(struct eventListElement *pEventListElement)
{
	playerWindow->SetTabFields(pEventListElement);
}

void PlayerDataFileManager::BPMNElementSelected(struct cellDef *pCell)
{
	playerWindow->BPMNElementSelected(pCell);
}


void PlayerDataFileManager::MoveToCurrentVideoIndexNumber()
{
	// Get video Index header & the current Position displayed
	FileContext *fileContext = GetFileContext(currPositionMarker.fileNum);
	if (fileContext != NULL)
	{
		fileContext->GetChannelVideoIndex(currPositionMarker.channelNum, currPositionMarker.fPosIndex, &mainVideoIndex);
		numVideo = currPositionMarker.indexNum;
		// Show the image at the current index here
	}
}

bool PlayerDataFileManager::IsPlaying()
{
	return bStartPlay;
}

bool PlayerDataFileManager::IsSynchronizeBPMNDisplay()
{
	if (IsVideoImageMoving() || bMoveAbsoluteDone)
	{
		bMoveAbsoluteDone = false;
		return true;
	}

	return false;
}



void PlayerDataFileManager::ReadDataFile()
{
	numVideo = 0;
	struct AudioHdr audioHeader;
	struct VideoHdr videoHeader;
	spdlog::info("Read Datafile Thread");

	ZeroMemory(&audioHeader, sizeof(struct AudioHdr));
	ZeroMemory(&videoHeader, sizeof(struct VideoHdr));

	pWavPlayer->SetDataManager(this);
	pWavPlayer->SetRecordBufferManager(pRecBufferManager);
	curContext->GetAudioHeaderData(&audioHeader);
	pWavPlayer->SetAudioParameters(audioHeader.freq,
									audioHeader.noChannels,
									audioHeader.bitsPerSample);
	pWavPlayer->SetAudioPlayerBuffer(curContext->GetAudioPlayerBuffer());
	curContext->GetVideoHeaderData(&videoHeader);
	fps = videoHeader.fps;
	playerWindow->SetFramesPerSecond(fps);

	playerWindow->SetRecordBufferManager(pRecBufferManager);

	playerWindow->SetFramesPerSecond(videoHeader.fps);

	//playerWindow->GetMonitorDetails(videoHeader.noMonitors, videoHeader.fPosMonitorDetails);
	bool audioDoneFlg = false;
	bool videoDoneFlg = false;

	// Read the Video Index Record
	ZeroMemory(&mainVideoIndex, sizeof(IndexHdr));
	videoDoneFlg = curContext->GetNextVideoIndex(&mainVideoIndex);

	bHeaderReads = true; // Set flag for Header reads so that other threads can start;
	Sleep(200); //Wait for 500 ms and then create thumb index
	//CreateThumbIndex(&videoHeader);
	list <BPMNElement *> *pDiagram = curContext->GetBPMNDiagram();
	playerWindow->SetDiagram(pDiagram);
	map <ULONG, struct eventListElement *> *pEventMap = curContext->GetElementMap();
	playerWindow->SetEventMap(pEventMap);
	curContext->SetEventMap(pEventMap);

	long long totalTime = curContext->InitializeVideoPath();
	playerWindow->SetPlayerEndTime((ULONG)(totalTime / 1000L));
	playerWindow->SetPlayerTimeWindow();

	lockPlayerManager.lock();
	bThumbsDone = true;
	lockPlayerManager.unlock();
	ExecutionManager *pExecManager = GetExecutionManager();
	MoveAbsolute();

	//pExecManager->StartThumbDisplayThread();
	while (true)
	{
		lockPlayerManager.lock();
		bool bPlay = bStartPlay;
		bool bQuit = bExitThreads;
		if (!bPlay || bQuit)
			bDataReadThreadWait = true;
		else
			bDataReadThreadWait = false;
		lockPlayerManager.unlock();
		if (bQuit)
			break;
		if (!bPlay)
		{
			Sleep(10);
			continue;
		}

		// Switch File context out here
		//if (audioDoneFlg)
		//{
		//	if (mainAudioIndex.fPosNextRecord != NULL || mainAudioIndex.numRecs < numAudio)
		//		audioDoneFlg = false;
		//}

		if (videoDoneFlg)
		{
			if (mainVideoIndex.fPosNextRecord != NULL || mainVideoIndex.numRecs < numVideo)
				videoDoneFlg = audioDoneFlg = false;
		}
		// Fill read buffers with data
		//if (!audioDoneFlg && !pRecBufferManager->AudioPlayBufferFull())
		//{
		//	audioDoneFlg = curContext->GetAudioBuffer(&mainAudioIndex, &numAudio);
		//}
		if (!videoDoneFlg && !pRecBufferManager->VideoPlayBufferFull())
		{
			spdlog::info("Read Video Buffer");
			audioDoneFlg = videoDoneFlg = curContext->GetVideoBuffer(&mainVideoIndex, &numVideo);
			if (videoDoneFlg)
			{
				bStopPlayerAtEnd = true;
			}
			spdlog::info("Read Video Buffer Done");
		}
	}
	pExecManager->StopThumbDisplayThread();
	spdlog::info("End of Read Datafile Thread");
}

list <BPMNElement *> *PlayerDataFileManager::GetBPMNDiagram()
{
	return curContext->GetBPMNDiagram();
}


//void PlayerDataFileManager::CreateThumbIndex(struct VideoHdr *vHeader)
//{
//	long long recordingTimeSpan = mainChannelRecord.recDurationus - mainChannelRecord.timeDeleted - mainChannelRecord.timePaused + mainChannelRecord.timeAdded;
//	struct IndexHdr videoIndex;
////	int oldPlayingNumVideo = playingNumVideo;
//	long long curVideoPos = currVideoFPos;
//
//	playerWindow->SetThumbListInProgress(true);
////	memcpy((char *)&videoIndex, (char *)&playingVideoIndex, sizeof(struct IndexHdr));
//
//	pRecBufferManager->CreateVideoThumbList(THUMB_WIDTH, THUMB_HEIGHT);
//	// Read the Video Index Record
//	long long fPosVideoHeader = videoHeader.fPosRoot;
////	pDataFile->ReadFromFile((char *)&playingVideoIndex, sizeof(struct IndexHdr), fPosVideoHeader);
////	playingNumVideo = 0;
//	for (int i = 0; i < 100; i++)
//	{
//		//GetNextSecondData((long)(recordingTimeSpan / 100L));
//		// Read the final one second image and put in buffer
//		struct ScreenData *screen = playerWindow->GetScreenData(playingVideoIndex.dataRecs[playingNumVideo].screenNum);
//		struct bufferRecord *pRecBuffer = (struct bufferRecord *)pRecBufferManager->GetFreeScreenBuffer(screen);
//		pDataFile->ReadBlockFromFile((char *)pRecBuffer, playingVideoIndex.dataRecs[playingNumVideo].size, playingVideoIndex.dataRecs[playingNumVideo].compSize, playingVideoIndex.dataRecs[playingNumVideo].fPosData);
//		pRecBuffer->buffer = (char *)pRecBuffer + sizeof(struct bufferRecord) + 8;
//		pRecBuffer->eventIndex = -1;
//		struct CompareBuffer *pCompBuffer = (struct CompareBuffer *)pRecBuffer->buffer;
//		playerWindow->CreateRewFwdScreenWindow();
//		pRecBufferManager->SaveInReadVideoBuffer(pRecBuffer);
//		playerWindow->CreateThumbImage(i);
//	}
//	playingNumVideo = oldPlayingNumVideo;
//	memcpy((char *)&playingVideoIndex, (char *)&videoIndex, sizeof(struct IndexHdr));
//	playerWindow->SetThumbListInProgress(false);
//	currVideoFPos = curVideoPos;
//}

void PlayerDataFileManager::ReadBlockFromFile(char *buffer, ULONG len, ULONG compressedLength, long long pos)
{
	curContext->GetDataFile()->ReadDeflateBlockFromFile(buffer, len, compressedLength, pos);
}

void PlayerDataFileManager::ReadFromFile(char *buffer, ULONG len, long long pos)
{
	curContext->GetDataFile()->ReadFromFile(buffer, len, pos);
}

void PlayerDataFileManager::Pause()
{
	pWavPlayer->Pause();
}

void PlayerDataFileManager::Restart()
{
	pWavPlayer->Restart();
}

void PlayerDataFileManager::SendStopPlayMessage()
{
	wxCommandEvent event(PROCOAN_BUTTON_DOWN);
	event.SetId(EDITOR_STOP);
	wxPostEvent(pPicControlPanel, event);
}

void PlayerDataFileManager::CheckIfStopPlayer()
{
	if (bStopPlayerAtEnd)
	{
		bStopPlayerAtEnd = false;
		SendStopPlayMessage();
	}
}

int PlayerDataFileManager::GetMaxMonitors()
{

	return curContext->GetMaxNumberOfMonitors();
}

void PlayerDataFileManager::GetScenarioData(struct scenarioReportData *pScenarioData, ScenarioDefintion *pScenarioDef)
{
	curContext->GetScenarioData(pScenarioData, pScenarioDef);
}

void PlayerDataFileManager::SaveOrGetScenario(bool bSave, ScenarioDefintion *pScenario)
{
	curContext->SaveOrGetScenario(bSave, pScenario);
}

string PlayerDataFileManager::GetScenarioJSON()
{
	return curContext->GetScenarioJSON();
}

vector <string> PlayerDataFileManager::GetScenarioNames()
{
	string scenarioJSON = GetScenarioJSON();
	ScenarioFacade scenarios(scenarioJSON);
	return scenarios.GetScenarioNames();
}

void PlayerDataFileManager::SetScenarioJSON(string sceneJSON)
{
	curContext->SetScenarioJSON(sceneJSON);
}

void PlayerDataFileManager::SetScenarioName(string value)
{
	scenarioName = value;
}

string PlayerDataFileManager::GetScenarioName()
{
	return scenarioName;
}

void PlayerDataFileManager::SetScenario(string value)
{
	string scenarioJSON = GetScenarioJSON();
	ScenarioFacade scenarios(scenarioJSON);
	ScenarioDefintion *pScenarioDef = scenarios.GetScenario(value);
	SaveOrGetScenario(false, pScenarioDef);
	ResetPlayerScreen();
}

void PlayerDataFileManager::PlayScenario(string value)
{
	string scenarioJSON = GetScenarioJSON();
	ScenarioFacade scenarios(scenarioJSON);
	ScenarioDefintion *pScenarioDef = scenarios.GetScenario(value);
	if (pScenarioDef != NULL)
	{
		SaveOrGetScenario(false, pScenarioDef);
		SetPlayerToStart();
		ResetPlayerScreen();
		SetRecorderPlay();
	}

}

void PlayerDataFileManager::SetPlayerBitmap(bool bActiveFlg)
{
	// for each context set the bitmap merge data
	map<string, FileContext *>::iterator itFileContext = fileContextMap.begin();
	while (itFileContext != fileContextMap.end())
	{
		FileContext *pFileContext = itFileContext->second;
		pFileContext->SetActiveMonitorDisplay(bActiveFlg);
		pFileContext->CreateBitmapMerge();
		itFileContext++;
	}
}

void PlayerDataFileManager::PrintDataFile()
{
	;
}

