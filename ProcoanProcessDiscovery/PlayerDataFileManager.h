#pragma once

#include "MMHeader.h"
class WavePlayer;
#include "DataFile.h"
#include "ChannelDataMapping.h"
#include <map>
#include <list>
#include "FileContext.h"
#include "IPlayerWindow.h"
#include "VideoSegmentClass.h"

class RecordBufferManager;
class AudioPlayerBuffer;
class PlayerWindow;
class BPMNFacade;
class BPMNElement;
class BPMNDiagram;
class ScenarioDefintion;

class PlayerDataFileManager
{
private:

	map <string, FileContext *> fileContextMap;
	FileContext *curContext;
	string currentFile;
	int fileNumber;
	VideoSegmentClass copySegmentInfo;

	int numVideo;
	struct IndexHdr mainVideoIndex;

	RecordBufferManager *pRecBufferManager;

	bool	bPlayerMode;
	bool	bDataReadThreadWait;
	bool    bVideoThreadWait;
	bool	bRewindThreadWait;
	bool	bMoveAbsoluteThreadWait;
	bool	bForwardThreadWait;
	bool	bStopPlayerAtEnd;
	bool	bStartPlay;
	bool	bExitThreads;
	bool	bHeaderReads;
	bool	bRewind;
	bool	bStepRewind;
	bool	bStepForward;
	bool	bStepRewFwdThreadOn;
	bool	bRewFwdThreadOn;
	bool	bForward;
	bool	bMoveAbsolute;
	bool	bMoveAbsoluteDone;
	//bool	bForwardThreadIsRunning;
	//bool	bRewindThreadIsRunning;
	bool	bThumbsDone;
	ULONG	nPictureDisplayEventIndex;
	string  scenarioName;
	WavePlayer *pWavPlayer;
	IPlayerWindow *playerWindow;
	int		fps;

	struct positionMarker currPositionMarker;
	long rewfwdMultiplier;
	//void WaitForThumbsDone();
	struct channelHdr *GetChannelHeader(int channelNum);
	FileContext *GetFileContext(int currFileNum);
	void SaveContext(FileContext *context);
	void RestoreContext(FileContext *context);
	void SendStopPlayMessage();
	void ClearAllContextSegments();

public:
	LONG GetChannelNextEventID(int channelNum);
	void ResetPlayerScreen();
	vector <string> GetScenarioNames();

	void InsertEvent();
	void StartSegment();
	void EndSegment();
	void ClearSegment();
	void CutSegment();
	void PasteSegment();
	void CopySegment();
	void CreateBranch();

	void SetRecorderPlay();
	void SetRecorderStop();
	void RewindVideo();
	bool GetRecorderStatus();

	void SetPlayerMode(bool value);
	bool GetPlayerMode();

	void MoveAbsolute();
	void FastForwardVideo();
	void RewindForwardVideo();
	void StepRewindForwardVideo();
	void ForwardStepVideo();
	void ReverseStepVideo();

	list <BPMNElement *> *GetBPMNDiagram();
	void AddNewEventRecord(struct eventListElement *pEventListElement);
	//void RewriteEventRecord(struct eventListElement *pEventListElement);
	void SetEventRoot(struct eventListElement *pEventListElement);
	void UpdateEventRecord(struct eventListElement *pEventListElement);
	ULONG FindEventIndex(long long fPosVideoPlayed, int videoIndexNum);
	void MoveToCurrentVideoIndexNumber();
	void MoveVideoAbsolute();
	void ThumbDisplay();
	void SetCurrentPositionMarker(long long lastfPosVideoPlayed, ULONG lastVideoIndexNum, USHORT fileNum, USHORT channelNum);
	bool IsEventDataPresent();
	bool IsSegmentOn();
	//void SetThumbImageNumber(int thumbNum);
	void ShowSliderTime();
	//void CheckTabNotifications(NMHDR *pNMHdr);
	//void CheckKillFocus(HWND hWnd);
	void BPMNElementSelected(struct cellDef *pCell);
	void Pause();
	void Restart();
	void StartPlayer();
	void StopPlayer();
	void PlayAudio();
	void PlayVideo();
	void DisplayVideo();
	bool IsPlaying();
	bool IsSynchronizeBPMNDisplay();
	void CreateScreenResizeBuffer(int width, int height);
	void ReadDataFile();
	void CreateThumbIndex(struct VideoHdr *vHeader);
	void SetSubProcessTab();
	void SetTaskTab();
	void SetTabFields(struct cellDef *pSelected);
	void SetTabFields(struct eventListElement *pEventListElement);
	void GetPointOffset(POINT *pPoint);
	void SaveEventDetails(ULONG eventIndex);
	bool IsSegmentSelectedInOtherContexts(int num);
	bool IsSegmentSelectedInSameContext(int num);
	bool IsSegmentCopiedInAnyContext();

	void SetPlayerWindow(IPlayerWindow *playerWnd)
	{
		playerWindow = playerWnd;
	}

	void ReadBlockFromFile(char *buffer, ULONG len, ULONG compressedLength, long long pos);
	void ReadFromFile(char *buffer, ULONG len, long long pos);
	void PrintDataFile();
	PlayerDataFileManager(char *fileName);
	~PlayerDataFileManager();
	int ProcessVideoRecord(struct bufferRecord *pBuffer, int currFileNum, int currChannelNum, int lastMon);
	void UpdateChannelHeader(int channelNum);
	bool IsBranchRight(ULONG nEventIndex);
	void SynchronizeVideo(ULONG nEventIndex);
	void BranchRight(ULONG nEventIndex);
	void BranchBottom(ULONG nEventIndex);
	void BranchCopy(ULONG nEventIndex);
	void BranchDelete(ULONG nEventIndex, bool bDelEntire);

	long long GetEventVideoTime(ULONG nEventIndex);
	void AddNewFileContext(char *fileName);
	void SwitchFileContext(int oldIndex, int newIndex);
	int GetContextFileNumber();
	bool GetContextSegOn();
	bool GetContextSegCopy();
	void StartAudio();
	void StopAudio();
	void ShowThumb();
	void ClearThumb();
	bool IsVideoImageMoving();
	void SetCurrentEventDisplay(ULONG nEventIndex);
	void GetEventImage(long long nTime, vector<unsigned char>& buffer, ULONG& cx, ULONG& cy, ULONG nEventIndex, vector <BPMNElement *> &pElementList);
	string ReadProcessValue();
	void WriteProcessValue(string val);
	void InitializeVideoPath();
	void SetPlayerToStart();
	void CheckIfStopPlayer();
	int GetMaxMonitors();
	void SetPlayerBitmap(bool bActiveFlg);
	void GetScenarioData(struct scenarioReportData *pScenarioData, ScenarioDefintion *pScenarioDef);
	void SaveOrGetScenario(bool bSave, ScenarioDefintion *pScenario);
	string GetScenarioJSON();
	void SetScenarioJSON(string sceneJSON);
	void SetScenarioName(string value);
	void SetScenario(string value);
	void PlayScenario(string value);
	string GetScenarioName();
};

