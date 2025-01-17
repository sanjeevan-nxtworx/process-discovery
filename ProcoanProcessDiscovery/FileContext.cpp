#include "stdafx.h"
#include "FileContext.h"
#include "DataFile.h"
#include "ChannelDataMapping.h"
#include "AudioPlayerBuffer.h"
#include "RecordBufferManager.h"
#include "BPMNFacade.h"
#include "BPMNElement.h"
#include "BPMNDecision.h"
#include "BPMNSubProcess.h"
#include "Utility.h"
#include <wx/progdlg.h>
#include "spdlog/spdlog.h"
#include "ScenarioDefintion.h"

FileContext::FileContext()
{
	pDataFile = NULL;
	pEventListRoot = NULL;
	pBPMNFacade = NULL;
	mainChannelNumber = USHRT_MAX;
	currContextChannel = USHRT_MAX;
	pRecBufferManager = NULL;
	pAudioPlayerBuffer = DBG_NEW AudioPlayerBuffer();
	nFileContextNum = 0;
	pEventMap = NULL;

	savedContext.numVideo = 0;
	ZeroMemory(&savedContext.mainVideoIndex, sizeof(struct IndexHdr));
	savedContext.bDataReadThreadWait = false;
	savedContext.bVideoThreadWait = false;
	savedContext.bRewindThreadWait = false;
	savedContext.bMoveAbsoluteThreadWait = false;
	savedContext.bForwardThreadWait = false;
	savedContext.bStartPlay = false;
	savedContext.bExitThreads = false;
	savedContext.bHeaderReads = false;
	savedContext.bRewind = false;
	savedContext.bForward = false;
	savedContext.bStepRewind = false;
	savedContext.bStepForward = false;
	savedContext.bMoveAbsolute = false;
	savedContext.bForwardThreadIsRunning = false;
	savedContext.bRewindThreadIsRunning = false;
	savedContext.bThumbsDone = false;
	ZeroMemory(&savedContext.currPositionMarker, sizeof(struct positionMarker));
	savedContext.rewfwdMultiplier = 0L;
	savedContext.sliderPosition = 0;
	savedContext.fps = 0;
	savedContext.bSegInProgress = false;
	savedContext.bSegPresent = false;
	savedContext.bSegCopied = false;
	savedContext.startPos = 0;
	savedContext.endPos = 0;

	maxVideoBufferSize = 0L;
	maxAudioBufferSize = 0L;

	bActiveMonitorOn = true;
	playerWindowWidth = 0;
	playerWindowHeight = 0;
	parentIndexMapping.clear();
}

void FileContext::SetActiveMonitorDisplay(bool bFlg)
{
	bActiveMonitorOn = bFlg;
}

void FileContext::SetPlayerWindowWidth(int nWidth)
{
	playerWindowWidth = nWidth;
}

void FileContext::SetPlayerWindowHeight(int nHeight)
{
	playerWindowHeight = nHeight;
}


string FileContext::ReadProcessValue()
{
	string processValue = "";
	if (pDataFile != NULL)
	{
		pDataFile->ReadFromFile((char *)&rootRecord, sizeof(struct rootHdr), 0L);

		if (rootRecord.sizeOfProcessInfo != 0 && rootRecord.posProcessInfo != 0L)
		{
			char *processInfo = (char *)malloc((size_t)rootRecord.sizeOfProcessInfo + 1024);
			pDataFile->ReadDeflateBlockFromFile(processInfo, rootRecord.sizeOfProcessInfo, rootRecord.sizeOfCompProcessInfo,rootRecord.posProcessInfo);
			if (processInfo != NULL)
			{
				processValue = processInfo;
				free(processInfo);
			}
		}
	}
	return processValue;
}

void FileContext::WriteProcessValue(string val)
{
	if (pDataFile != NULL)
	{
		pDataFile->ReadFromFile((char *)&rootRecord, sizeof(struct rootHdr), 0L);
		rootRecord.sizeOfProcessInfo = val.size() + 1;
		rootRecord.posProcessInfo = pDataFile->AppendDeflateBlockToFile((char *)val.c_str(), rootRecord.sizeOfProcessInfo, &rootRecord.sizeOfCompProcessInfo);
		pDataFile->WriteToFile(0L, (char *)&rootRecord, sizeof(struct rootHdr));
	}
}

FileContext::~FileContext()
{
	delete pDataFile;
	delete pBPMNFacade;
	delete pAudioPlayerBuffer;
	
	map <long long, struct indexPageDescriptor *>::iterator itMap = mapAudioPages.begin();

	while (itMap != mapAudioPages.end())
	{
		struct indexPageDescriptor *pDescriptor = itMap->second;
		delete pDescriptor;
		itMap->second = NULL;
		itMap++;
	}

	itMap = mapVideoPages.begin();
	while (itMap != mapVideoPages.end())
	{
		struct indexPageDescriptor *pDescriptor = itMap->second;
		if (pDescriptor->nPageType == 1 || pDescriptor->nPageType == 4)
			delete pDescriptor->pageDescriptor.pVideo;
		else if (pDescriptor->nPageType == 2)
			delete pDescriptor->pageDescriptor.pBranch;
		else if (pDescriptor->nPageType == 3)
			delete pDescriptor->pageDescriptor.pJoin;
		else if (pDescriptor->nPageType == 5)
			delete pDescriptor->pageDescriptor.pChannel;

		delete pDescriptor;
		itMap->second = NULL;
		itMap++;
	}

	mapAudioPages.clear();
	mapVideoPages.clear();

	map <USHORT, ChannelDataMapping *>::iterator itChannelData =  channelDataMap.begin(); 
	while (itChannelData != channelDataMap.end())
	{
		ChannelDataMapping *pChannelData = itChannelData->second;
		delete pChannelData;
		itChannelData++;
	}
}

struct fileContext *FileContext::GetContext()
{
	return &savedContext;
}

DataFile *FileContext::GetDataFile()
{
	return pDataFile;
}

void FileContext::SetFileContextNumber(int num)
{
	nFileContextNum = num;
}

int FileContext::GetFileContextNumber()
{
	return nFileContextNum;
}

int FileContext::ProcessVideoRecord(struct bufferRecord *pBuffer, int currChannelNum, int lastMon)
{
	ChannelDataMapping *pChannelData = GetChannelDataMapping(currChannelNum);
	if (pChannelData != NULL)
	{
		return pChannelData->ProcessVideoRecord(pBuffer, lastMon);
	}
	return 0;
}

AudioPlayerBuffer *FileContext::GetAudioPlayerBuffer()
{
	return pAudioPlayerBuffer;
}

RecordBufferManager *FileContext::GetRecordBufferManager()
{
	return pRecBufferManager;
}

void FileContext::SetRecordBufferManager(RecordBufferManager *pRecBuffManager)
{
	pRecBufferManager = pRecBuffManager;
	SetChannelBufferManager(pRecBufferManager);
	pAudioPlayerBuffer->SetRecordBufferManager(pRecBufferManager);
}

void FileContext::CreateBitmapMerge()
{
	map <USHORT, ChannelDataMapping *>::iterator itChannel = channelDataMap.begin();
	while (itChannel != channelDataMap.end())
	{
		ChannelDataMapping *pChannelData = itChannel->second;
		if (pChannelData != NULL)
			pChannelData->CreateBitmapMerge(bActiveMonitorOn, playerWindowWidth, playerWindowHeight);
		itChannel++;
	}
}

void FileContext::CreateScreenResizeBuffer(bool bActiveMonitorDisplay, int windowWidth, int windowHeight)
{
	ChannelDataMapping *pChannelData = GetChannelDataMapping(currContextChannel);
	if (pChannelData != NULL)
		pChannelData->UpdateBitmapMergeWindowDimensions(bActiveMonitorDisplay, windowWidth, windowHeight);
}

ChannelDataMapping *FileContext::GetChannelDataMapping(USHORT channelNum)
{
	map<USHORT, ChannelDataMapping *>::iterator itFind = channelDataMap.find(channelNum);
	if (itFind != channelDataMap.end())
	{
		ChannelDataMapping *channel = itFind->second;
		return channel;
	}
	return NULL;
}

void FileContext::SetChannelBufferManager(RecordBufferManager *pRecBuffManager)
{
	map<USHORT, ChannelDataMapping *>::iterator itFind = channelDataMap.begin();
	if (itFind != channelDataMap.end())
	{
		ChannelDataMapping *channel = itFind->second;
		channel->SetRecordBufferManager(pRecBuffManager);
		itFind++;
	}
}

void FileContext::GetAudioHeaderData(struct AudioHdr *pAudioHdr)
{
	ZeroMemory(pAudioHdr, sizeof(struct AudioHdr));

	ChannelDataMapping *pChannelData = GetChannelDataMapping(currContextChannel);
	if (pChannelData != NULL)
		pChannelData->GetAudioHeader(pAudioHdr);
}

void FileContext::GetVideoHeaderData(struct VideoHdr *pVideoHdr)
{
	ZeroMemory(pVideoHdr, sizeof(struct AudioHdr));

	ChannelDataMapping *pChannelData = GetChannelDataMapping(currContextChannel);
	if (pChannelData != NULL)
		pChannelData->GetVideoHeader(pVideoHdr);
}

void FileContext::CreateFileContext(char *fileName)
{
	pBPMNFacade = DBG_NEW BPMNFacade();
	CopyRecordingFile(fileName);
	pDataFile = DBG_NEW DataFile(fileName, FALSE);
	CreateChannelDataMapping();
}

void FileContext::CopyRecordingFile(char *fileName)
{
	DataFile *pOldFile = NULL;
	DataFile *pNewFile = NULL;

	spdlog::info("Copying Recording File");
	size_t len = strlen(fileName);
	char *backupFileName = (char *)malloc(len + 32);
	ZeroMemory(backupFileName, len + 32);
	for (size_t i = 0; i < len; i++)
	{
		if (fileName[i] == '.')
			break;
		backupFileName[i] = fileName[i];
	}
	strcat_s(backupFileName,len+31, ".bak");
	remove(backupFileName);
	rename(fileName, backupFileName);

	pOldFile = DBG_NEW DataFile(backupFileName, FALSE);
	pNewFile = DBG_NEW DataFile(fileName, TRUE);

	struct rootHdr rootRec;
	struct AudioHdr audioHeader;
	struct VideoHdr videoHeader;
	struct EventHdr eventHeader;
	long long fPosAudioHeader = 0L;
	long long fPosVideoHeader = 0L;
	long long fPosEventHeader = 0L;
	struct IndexHdr audioIndex;
	struct IndexHdr videoIndex;
	struct IndexHdr eventIndex;
	long long branchPosition = 0L;
	int screenWidth, screenHeight;
	
	ZeroMemory(&audioIndex, sizeof(IndexHdr));
	ZeroMemory(&videoIndex, sizeof(IndexHdr));
	ZeroMemory(&eventIndex, sizeof(IndexHdr));

	ZeroMemory(&rootRec, sizeof(struct rootHdr));
	ZeroMemory(&audioHeader, sizeof(struct AudioHdr));
	ZeroMemory(&videoHeader, sizeof(struct VideoHdr));
	
	branchesCopied.clear();

	pOldFile->ReadFromFile((char *)&rootRec, sizeof(struct rootHdr), 0L);
	if (rootRec.RecType != 'R' || strcmp(rootRec.signature, "PECAN1.0"))
	{
		return;
	}
	pNewFile->AppendToFile((char *)&rootRec, sizeof(struct rootHdr));

	char *pProcessInfo = NULL;
	if (rootRec.posProcessInfo != NULL && rootRec.sizeOfProcessInfo != 0)
	{
		pProcessInfo = (char *)malloc((size_t)rootRec.sizeOfProcessInfo + 1024);
		pOldFile->ReadDeflateBlockFromFile(pProcessInfo, rootRec.sizeOfProcessInfo, rootRec.sizeOfCompProcessInfo, rootRec.posProcessInfo);
		
		rootRec.posProcessInfo = pNewFile->AppendDeflateBlockToFile(pProcessInfo, rootRec.sizeOfProcessInfo, &rootRec.sizeOfCompProcessInfo);
		pNewFile->WriteToFile(0L, (char *)&rootRec, sizeof(struct rootHdr));
		free(pProcessInfo);
	}

	char *pScenarioInfo = NULL;
	//rootRec.posScenarioInfo = NULL;
	//rootRec.sizeOfCompScenarioInfo = rootRec.sizeOfScenarioInfo = 0;
	//pNewFile->WriteToFile(0L, (char *)&rootRec, sizeof(struct rootHdr));
	if (rootRec.posScenarioInfo != NULL && rootRec.sizeOfScenarioInfo != 0)
	{
		pScenarioInfo = (char *)malloc((size_t)rootRec.sizeOfScenarioInfo + 1024);
		pOldFile->ReadDeflateBlockFromFile(pScenarioInfo, rootRec.sizeOfScenarioInfo, rootRec.sizeOfCompScenarioInfo, rootRec.posScenarioInfo);

		rootRec.posScenarioInfo = pNewFile->AppendDeflateBlockToFile(pScenarioInfo, rootRec.sizeOfScenarioInfo, &rootRec.sizeOfCompScenarioInfo);
		pNewFile->WriteToFile(0L, (char *)&rootRec, sizeof(struct rootHdr));
		free(pScenarioInfo);
	}

	long long channelHeaderFPos = rootRec.posChannelRecords;
	struct channelHdr channelHeader;
	struct channelHdr prevChannelHeader;
	struct channelHdr *prevChannelHdr = NULL;
	long long fChannelPos, fChannelPrevPos;

	fChannelPos = 0L;
	fChannelPrevPos = 0L;
	long buffSize = 400000000L;
	ULONG indexReadCnt = 0;
	ULONG maxIndex = 0L;
	wxString dialogTitle = wxString("Copying File ") + wxString(fileName);

	pOldFile->ReadFromFile((char *)&channelHeader, sizeof(struct channelHdr), channelHeaderFPos);
	maxIndex = channelHeader.lastIndexNumber / 60;

	wxProgressDialog wxReadProgressDialog(dialogTitle, wxString("Copying Indexes"), maxIndex);
	
	// First copy all the channel records with the audio and video records

	while (channelHeaderFPos != NULL)
	{
		// Copy the Channel Header
		pOldFile->ReadFromFile((char *)&channelHeader, sizeof(struct channelHdr), channelHeaderFPos);
		channelHeaderFPos = channelHeader.fPosNextChannel;
		fChannelPos = pNewFile->AppendToFile((char *)&channelHeader, sizeof(struct channelHdr));
		channelHeader.posCurrRec = fChannelPos;
		if (prevChannelHdr == NULL)
		{
			rootRec.posChannelRecords = fChannelPos;
			pNewFile->WriteToFile(0L, (char *)&rootRec, sizeof(struct rootHdr));
		}
		else
		{
			prevChannelHdr->fPosNextChannel = fChannelPos;
			pNewFile->WriteToFile(fChannelPrevPos, (char *)prevChannelHdr, sizeof(struct channelHdr));
		}

		if (channelHeader.posAudioRoot != NULL)
		{
			pOldFile->ReadFromFile((char *)&audioHeader, sizeof(struct AudioHdr), channelHeader.posAudioRoot);
			audioHeader.fPosRoot = NULL;
			channelHeader.posAudioRoot = pNewFile->AppendToFile((char *)&audioHeader, sizeof(struct AudioHdr));
		}

		if (channelHeader.posVideoRoot != NULL)
		{
			pOldFile->ReadFromFile((char *)&videoHeader, sizeof(struct VideoHdr), channelHeader.posVideoRoot);
			channelHeader.posVideoRoot = pNewFile->AppendToFile((char *)&videoHeader, sizeof(struct VideoHdr));

			for (int i = 0; i < videoHeader.noMonitors; i++)
			{
				struct MonitorHdr mDets;

				if (i == 0)
				{
					pOldFile->ReadFromFile((char *)&mDets, sizeof(struct MonitorHdr), videoHeader.fPosMonitorDetails);
					videoHeader.fPosMonitorDetails = pNewFile->AppendToFile((char *)&mDets, sizeof(struct MonitorHdr));
				}
				else
				{
					pOldFile->ReadFromFile((char *)&mDets, sizeof(struct MonitorHdr), -1L);
					pNewFile->AppendToFile((char *)&mDets, sizeof(struct MonitorHdr));
				}
				screenWidth = mDets.right - mDets.left;
				screenHeight = mDets.bottom - mDets.top;

				long tempBuffSize = (screenHeight) * (screenWidth) * 5;
				if (tempBuffSize > buffSize)
					buffSize = tempBuffSize;
			}

			pNewFile->WriteToFile(channelHeader.posVideoRoot, (char *)&videoHeader, sizeof(struct VideoHdr));
		}

		pNewFile->WriteToFile(fChannelPos, (char *)&channelHeader, sizeof(struct channelHdr));
		fChannelPrevPos = fChannelPos;
		prevChannelHdr = &prevChannelHeader;
		CopyMemory(prevChannelHdr, &channelHeader, sizeof(struct channelHdr));
	}

	maxVideoBufferSize = buffSize;
	maxAudioBufferSize = buffSize;
	// Retrieve the first channel record, and video and audio headers, this will have the main recording data
	channelHeaderFPos = rootRec.posChannelRecords;
	pNewFile->ReadFromFile((char *)&channelHeader, sizeof(struct channelHdr), channelHeaderFPos);
	if (channelHeader.posAudioRoot != NULL)
	{
		pNewFile->ReadFromFile((char *)&audioHeader, sizeof(struct AudioHdr), channelHeader.posAudioRoot);
	}

	if (channelHeader.posVideoRoot != NULL)
	{
		pNewFile->ReadFromFile((char *)&videoHeader, sizeof(struct VideoHdr), channelHeader.posVideoRoot);
	}

	struct bufferRecord *pRecBuffer = NULL;
	char *audioBuffer = NULL;
	if (audioHeader.fPosRoot != NULL || videoHeader.fPosRoot != NULL || channelHeader.posTaskListRoot != NULL)
	{
		pRecBuffer = (struct bufferRecord *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, buffSize);
		audioBuffer = (char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, buffSize);
	}

	if (audioHeader.fPosRoot != NULL)
	{
		// Read the Index Record for Audio
		fPosAudioHeader = audioHeader.fPosRoot;
		pOldFile->ReadFromFile((char *)&audioIndex, sizeof(struct IndexHdr), fPosAudioHeader);
		int numAudio = 0;
		struct IndexHdr prevAudioIndex;
		long long prevAudioIndexPos = 0L;
		while (true)
		{
			pOldFile->ReadCompressedBlockFromFile((char *)pRecBuffer, audioIndex.dataRecs[numAudio].compSize, audioIndex.dataRecs[numAudio].fPosData);
			audioIndex.dataRecs[numAudio].fPosData = pNewFile->AppendCompressedBlockToFile((char *)pRecBuffer, audioIndex.dataRecs[numAudio].compSize);
			numAudio++;
			if (numAudio > audioIndex.numRecs)
			{
				long long pos = audioIndex.fPosNextRecord;
				audioIndex.fPosNextRecord = NULL;
				if (prevAudioIndexPos == NULL)
				{
					audioIndex.fPosPrevRecord = NULL;
					audioHeader.fPosRoot = pNewFile->AppendToFile((char *)&audioIndex, sizeof(struct IndexHdr));
					prevAudioIndexPos = audioHeader.fPosRoot;
				}
				else
				{
					audioIndex.fPosPrevRecord = prevAudioIndexPos;
					prevAudioIndex.fPosNextRecord = pNewFile->AppendToFile((char *)&audioIndex, sizeof(struct IndexHdr));
					pNewFile->WriteToFile(prevAudioIndexPos, (char *)&prevAudioIndex, sizeof(struct IndexHdr));
					prevAudioIndexPos = prevAudioIndex.fPosNextRecord;
				}
				CopyMemory(&prevAudioIndex, &audioIndex, sizeof(struct IndexHdr));

				if (pos == 0L)
				{
					// end of file
					break;
				}
				else
				{
					pOldFile->ReadFromFile((char *)&audioIndex, sizeof(struct IndexHdr), pos);
					numAudio = 0;
				}
			}
		}
		pNewFile->WriteToFile(channelHeader.posAudioRoot, (char *)&audioHeader, sizeof(struct AudioHdr));
	}

	long long fPosJoin = 0LL;

	if (videoHeader.fPosRoot)
	{
		// Read the Index Record
		fPosVideoHeader = videoHeader.fPosRoot;
		struct IndexHdr prevVideoIndex;
		long long prevVideoIndexPos = 0L;
		pOldFile->ReadFromFile((char *)&videoIndex, sizeof(struct IndexHdr), fPosVideoHeader);
		int numVideo = 0;
		int indexNumCount = 0;
		while (true)
		{
			indexNumCount++;
			if (indexNumCount >= 60)
			{
				indexNumCount = 0;
				indexReadCnt++;
				if (indexReadCnt > maxIndex)
				{
					indexReadCnt = maxIndex-1;
					char str[100];
					sprintf_s(str, "Progress Count > maxIndex %ld", maxIndex);
					spdlog::info(str);
				}
				wxReadProgressDialog.Update(indexReadCnt, wxString::Format("Copying Indexes %d of %d", indexReadCnt, maxIndex));
			}
			if (videoIndex.RecType == 'P')
			{
				pOldFile->ReadCompressedBlockFromFile((char *)pRecBuffer, videoIndex.dataRecs[numVideo].compSize, videoIndex.dataRecs[numVideo].fPosData);
				// Write Audio Record
				if (videoIndex.dataRecs[numVideo].fPosAudioData != NULL)
				{
					pOldFile->ReadCompressedBlockFromFile(audioBuffer, videoIndex.dataRecs[numVideo].compAudioSize, videoIndex.dataRecs[numVideo].fPosAudioData);
					videoIndex.dataRecs[numVideo].fPosAudioData = pNewFile->AppendCompressedBlockToFile(audioBuffer, videoIndex.dataRecs[numVideo].compAudioSize);
				}
				videoIndex.dataRecs[numVideo].fPosData = pNewFile->AppendCompressedBlockToFile((char *)pRecBuffer, videoIndex.dataRecs[numVideo].compSize);
			}
			else if (videoIndex.RecType == 'B')
			{
				long long pos = videoIndex.fPosNextRecord;
				videoIndex.fPosNextRecord = NULL;
				if (prevVideoIndexPos == NULL)
				{
					videoIndex.fPosPrevRecord = NULL;
					videoHeader.fPosRoot = pNewFile->AppendToFile((char *)&videoIndex, sizeof(struct IndexHdr));
					prevVideoIndexPos = videoHeader.fPosRoot;
				}
				else
				{
					videoIndex.fPosPrevRecord = prevVideoIndexPos;
					prevVideoIndex.fPosNextRecord = pNewFile->AppendToFile((char *)&videoIndex, sizeof(struct IndexHdr));
					pNewFile->WriteToFile(prevVideoIndexPos, (char *)&prevVideoIndex, sizeof(struct IndexHdr));
					prevVideoIndexPos = prevVideoIndex.fPosNextRecord;
				}
				branchPosition = prevVideoIndexPos;
				branchesCopied.push_back(prevVideoIndexPos);
				fPosJoin = CopyRightBranchToFile(pNewFile, pOldFile, &videoIndex, pRecBuffer, audioBuffer, &prevVideoIndex);

				CopyMemory(&prevVideoIndex, &videoIndex, sizeof(struct IndexHdr));
				if (pos == 0L)
				{
					// end of file
					break;
				}
				else
				{
					pOldFile->ReadFromFile((char *)&videoIndex, sizeof(struct IndexHdr), pos);
					numVideo = -1;
				}

			}
			else if (videoIndex.RecType == 'J')
			{
				long long pos = videoIndex.fPosNextRecord;

//
				long long posBranch = 0LL;
				if (branchesCopied.size() > 0)
					posBranch = branchesCopied.back();
				if (posBranch == branchPosition)
				{
					branchesCopied.pop_back(); // pop to signify that the first copy is written
					videoIndex.dataRecs[0].fPosData = fPosJoin;
					videoIndex.fPosPrevRecord = prevVideoIndexPos;
					prevVideoIndex.fPosNextRecord = pNewFile->AppendToFile((char *)&videoIndex, sizeof(struct IndexHdr));
					pNewFile->WriteToFile(prevVideoIndexPos, (char *)&prevVideoIndex, sizeof(struct IndexHdr));
					prevVideoIndexPos = prevVideoIndex.fPosNextRecord;
					CopyMemory(&prevVideoIndex, &videoIndex, sizeof(struct IndexHdr));
					if (fPosJoin != 0LL)
					{
						pNewFile->ReadFromFile((char *)&videoIndex, sizeof(struct IndexHdr), fPosJoin); // Read the right join
						videoIndex.fPosNextRecord = prevVideoIndexPos;
						pNewFile->WriteToFile(fPosJoin, (char *)&videoIndex, sizeof(struct IndexHdr));
					}
				}
				else
				{
					pNewFile->ReadFromFile((char *)&videoIndex, sizeof(struct IndexHdr), fPosJoin); // Read the right join
					videoIndex.fPosPrevRecord = prevVideoIndexPos;
					pNewFile->WriteToFile(fPosJoin, (char *)&videoIndex, sizeof(struct IndexHdr));
					prevVideoIndex.fPosNextRecord = fPosJoin;
					pNewFile->WriteToFile(prevVideoIndexPos, (char *)&prevVideoIndex, sizeof(struct IndexHdr));
					prevVideoIndexPos = fPosJoin;
					CopyMemory(&prevVideoIndex, &videoIndex, sizeof(struct IndexHdr));
				}
				
				if (pos == 0L)
				{
					// end of file
					break;
				}
				else
				{
					pOldFile->ReadFromFile((char *)&videoIndex, sizeof(struct IndexHdr), pos);
					numVideo = -1;
				}
			}
			numVideo++;
			if (numVideo > videoIndex.numRecs)
			{
				long long pos = videoIndex.fPosNextRecord;
				videoIndex.fPosNextRecord = NULL;
				if (prevVideoIndexPos == NULL)
				{
					videoIndex.fPosPrevRecord = NULL;
					videoHeader.fPosRoot = pNewFile->AppendToFile((char *)&videoIndex, sizeof(struct IndexHdr));
					prevVideoIndexPos = videoHeader.fPosRoot;
				}
				else
				{
					videoIndex.fPosPrevRecord = prevVideoIndexPos;
					prevVideoIndex.fPosNextRecord = pNewFile->AppendToFile((char *)&videoIndex, sizeof(struct IndexHdr));
					pNewFile->WriteToFile(prevVideoIndexPos, (char *)&prevVideoIndex, sizeof(struct IndexHdr));
					prevVideoIndexPos = prevVideoIndex.fPosNextRecord;
				}
				CopyMemory(&prevVideoIndex, &videoIndex, sizeof(struct IndexHdr));

				if (pos == 0L)
				{
					// end of file
					break;
				}
				else
				{
					pOldFile->ReadFromFile((char *)&videoIndex, sizeof(struct IndexHdr), pos);
					numVideo = 0;
				}
			}
		}
		wxReadProgressDialog.Update(indexReadCnt, wxString::Format("Copying Indexes %d of %d", maxIndex, maxIndex));
		pNewFile->WriteToFile(channelHeader.posVideoRoot, (char *)&videoHeader, sizeof(struct VideoHdr));
	}

	struct eventDetails eventDets;
	struct eventFileRecord eventRecNew, eventRecOld;
	ZeroMemory(&eventRecNew, sizeof(struct eventFileRecord));
	ZeroMemory(&eventRecOld, sizeof(struct eventFileRecord));
	long long posTaskList = NULL;
	long long posTaskListOld = NULL;

	if (channelHeader.posTaskListRoot == NULL && channelHeader.posEventRoot != NULL)
	{
		string strPath = getcurrentpath() + "resources\\ARRules.xml";

		pBPMNFacade->GetAnnotationRules((char *)strPath.c_str());
		if (channelHeader.posElementJSON != NULL)
		{
			struct elementJSON jSONRec;
			pOldFile->ReadFromFile((char *)&jSONRec, sizeof(struct elementJSON), channelHeader.posElementJSON);
			pOldFile->ReadDeflateBlockFromFile((char *)pRecBuffer, jSONRec.size, jSONRec.compSize, jSONRec.fPosJSON);
			jSONRec.fPosJSON = pNewFile->AppendDeflateBlockToFile((char *)pRecBuffer, jSONRec.size, &jSONRec.compSize);
			channelHeader.posElementJSON = pNewFile->AppendToFile((char *)&jSONRec, sizeof(struct elementJSON));
			pBPMNFacade->CreateWidgetTree((char *)pRecBuffer);
			// After this we need to do the following
			// Resolve all BoundingRectangle co-ordinates from Window to Client 
			// Resolve all names for editable controls relevant names if possible.
		}

		// Read events and create event List
		pOldFile->ReadFromFile((char *)&eventHeader, sizeof(struct EventHdr), channelHeader.posEventRoot);
		// Read the Index Record
		fPosEventHeader = eventHeader.fPosRoot;
		channelHeader.posEventRoot = NULL;
		if (fPosEventHeader != NULL)
		{
			//long long prevEventIndexPos = 0L;

			pOldFile->ReadFromFile((char *)&eventIndex, sizeof(struct IndexHdr), fPosEventHeader);
			int numEvent = 0;
			while (true)
			{
				pOldFile->ReadDeflateBlockFromFile((char *)pRecBuffer, eventIndex.dataRecs[numEvent].size, eventIndex.dataRecs[numEvent].compSize, eventIndex.dataRecs[numEvent].fPosData);
				pRecBuffer->buffer = (char *)pRecBuffer + sizeof(struct bufferRecord) + 8;
				struct eventBufferRecord *pEventBuffer = (struct eventBufferRecord *)pRecBuffer->buffer;
				pBPMNFacade->CreateRecorderEvent(&eventDets, pEventBuffer, eventIndex.dataRecs[numEvent].videoIndexNumber);
				// remove event if no annotation is possible.
				if (eventDets.strAnnotation != "")
				{
					PackEventData(&eventRecNew, &eventDets, (char *)pRecBuffer);

					eventRecNew.fPosPrevEvent = posTaskListOld;
					eventRecNew.nParentEventCode = 0L;
					eventRecNew.nEventType = 1; // This is a Task Event
					eventRecNew.dataPos = pNewFile->AppendDeflateBlockToFile((char *)pRecBuffer, eventRecNew.size, &eventRecNew.compSize);
					posTaskList = pNewFile->AppendToFile((char *)&eventRecNew, sizeof(struct eventFileRecord));

					if (posTaskListOld != NULL)
					{
						struct eventDetails oldEventDets;
						pNewFile->ReadDeflateBlockFromFile((char *)pRecBuffer, eventRecOld.size, eventRecOld.compSize, eventRecOld.dataPos);
						//UnpackEventData(&oldEventDets, &eventRecOld, (char *)pRecBuffer);
						//long long tnM = eventDets.timestamp - oldEventDets.timestamp;
						//char strTnM[100];
						//sprintf_s(strTnM, "%lld", tnM);
						//string strTnMval = strTnM;
						//std::size_t found = oldEventDets.strNotes.find("%D");
						//oldEventDets.strNotes.replace(found, 2, strTnM);
						//PackEventData(&eventRecOld, &oldEventDets, (char *)pRecBuffer);
						eventRecOld.dataPos = pNewFile->AppendDeflateBlockToFile((char *)pRecBuffer, eventRecOld.size, &eventRecOld.compSize);
						eventRecOld.fPosNextEvent = posTaskList;
						pNewFile->WriteToFile(posTaskListOld, (char *)&eventRecOld, sizeof(struct eventFileRecord));
					}
					else
					{
						channelHeader.posTaskListRoot = posTaskList;
					}
					posTaskListOld = posTaskList;
					CopyMemory(&eventRecOld, &eventRecNew, sizeof(struct eventFileRecord));
				}
				numEvent++;
				if (numEvent > eventIndex.numRecs)
				{
					long long pos = eventIndex.fPosNextRecord;
					if (pos == 0L)
					{
						// end of file
						break;
					}
					else
					{
						pOldFile->ReadFromFile((char *)&eventIndex, sizeof(struct IndexHdr), pos);
						numEvent = 0;
					}
				}

			}
		}
	}
	else if (channelHeader.posTaskListRoot != NULL)
	{
		posTaskList = channelHeader.posTaskListRoot;
		if (channelHeader.posElementJSON != NULL)
		{
			struct elementJSON jSONRec;
			pOldFile->ReadFromFile((char *)&jSONRec, sizeof(struct elementJSON), channelHeader.posElementJSON);
			pOldFile->ReadDeflateBlockFromFile((char *)pRecBuffer, jSONRec.size, jSONRec.compSize, jSONRec.fPosJSON);
			jSONRec.fPosJSON = pNewFile->AppendDeflateBlockToFile((char *)pRecBuffer, jSONRec.size, &jSONRec.compSize);
			channelHeader.posElementJSON = pNewFile->AppendToFile((char *)&jSONRec, sizeof(struct elementJSON));
		}

		branchRecordUpdateList.clear();
		channelHeader.posTaskListRoot = CopyEventRecords(posTaskList, (char *)pRecBuffer, pOldFile, pNewFile, &channelHeader, 0L);
	}
	
	fChannelPos = rootRec.posChannelRecords;
	pNewFile->WriteToFile(fChannelPos, (char *)&channelHeader, sizeof(struct channelHdr));
	HeapFree(GetProcessHeap(), 0, pRecBuffer);
	HeapFree(GetProcessHeap(), 0, audioBuffer);

	delete pOldFile;
	delete pNewFile;
	free(backupFileName);
}

long long FileContext::CopyRightBranchToFile(DataFile *pNewFile, DataFile *pOldFile, struct IndexHdr *pVideoIndex, struct bufferRecord *pRecBuffer, char *audioBuffer, struct IndexHdr *pBranchPrevIndex)
{
	long long fPos = pVideoIndex->dataRecs[0].fPosData;
	if (fPos == NULL)
		return(0);

	struct IndexHdr videoIndex;
	struct IndexHdr prevVideoIndex;
	long long prevVideoIndexPos = 0L;
	pOldFile->ReadFromFile((char *)&videoIndex, sizeof(struct IndexHdr), fPos);
	pVideoIndex->dataRecs[0].fPosData = NULL;
	int numVideo = 0;
	int indexReadCnt = 0;
	long long fPosJoin = 0L;
	//long long branchPos = 0LL;
	long long branchPosition = 0LL;

	while (true)
	{
		if (videoIndex.RecType == 'P')
		{
			pOldFile->ReadCompressedBlockFromFile((char *)pRecBuffer, videoIndex.dataRecs[numVideo].compSize, videoIndex.dataRecs[numVideo].fPosData);
			// Write Audio Record
			if (videoIndex.dataRecs[numVideo].fPosAudioData != NULL)
			{
				pOldFile->ReadCompressedBlockFromFile(audioBuffer, videoIndex.dataRecs[numVideo].compAudioSize, videoIndex.dataRecs[numVideo].fPosAudioData);
				videoIndex.dataRecs[numVideo].fPosAudioData = pNewFile->AppendCompressedBlockToFile(audioBuffer, videoIndex.dataRecs[numVideo].compAudioSize);
			}
			videoIndex.dataRecs[numVideo].fPosData = pNewFile->AppendCompressedBlockToFile((char *)pRecBuffer, videoIndex.dataRecs[numVideo].compSize);
		}
		else if (videoIndex.RecType == 'B')
		{
			long long pos = videoIndex.fPosNextRecord;
			videoIndex.fPosNextRecord = NULL;
			if (prevVideoIndexPos == NULL)
			{
				if (pBranchPrevIndex->RecType != 'B')
					videoIndex.fPosPrevRecord = pBranchPrevIndex->fPosNextRecord;
				else
					videoIndex.fPosPrevRecord = pBranchPrevIndex->dataRecs[0].fPosData;
				pVideoIndex->dataRecs[0].fPosData = pNewFile->AppendToFile((char *)&videoIndex, sizeof(struct IndexHdr));
				prevVideoIndexPos = pVideoIndex->dataRecs[0].fPosData;
				pNewFile->WriteToFile(videoIndex.fPosPrevRecord, (char *)pVideoIndex, sizeof(struct IndexHdr));
				branchPosition = prevVideoIndexPos;
				branchesCopied.push_back(prevVideoIndexPos);
				fPosJoin = CopyRightBranchToFile(pNewFile, pOldFile, &videoIndex, pRecBuffer, audioBuffer, pVideoIndex);
			}
			else
			{
				videoIndex.fPosPrevRecord = prevVideoIndexPos;
				prevVideoIndex.fPosNextRecord = pNewFile->AppendToFile((char *)&videoIndex, sizeof(struct IndexHdr));
				pNewFile->WriteToFile(prevVideoIndexPos, (char *)&prevVideoIndex, sizeof(struct IndexHdr));
				prevVideoIndexPos = prevVideoIndex.fPosNextRecord;
				branchPosition = prevVideoIndexPos;
				branchesCopied.push_back(prevVideoIndexPos);
				fPosJoin = CopyRightBranchToFile(pNewFile, pOldFile, &videoIndex, pRecBuffer, audioBuffer, &prevVideoIndex);
			}

			CopyMemory(&prevVideoIndex, &videoIndex, sizeof(struct IndexHdr));
			if (pos == 0L)
			{
				// end of file
				break;
			}
			else
			{
				pOldFile->ReadFromFile((char *)&videoIndex, sizeof(struct IndexHdr), pos);
				numVideo = 0;
			}
		}
		else if (videoIndex.RecType == 'J')
		{
			long long posBranch = branchesCopied.back();
			if (posBranch == branchPosition || fPosJoin == 0LL)
			{
				branchesCopied.pop_back(); // pop to signify that the first copy is written
				if (fPosJoin != 0LL)
				{
					videoIndex.dataRecs[0].fPosData = fPosJoin;
				}
				else
				{
					videoIndex.dataRecs[0].fPosData = prevVideoIndexPos;
				}
				videoIndex.fPosPrevRecord = prevVideoIndexPos;
				prevVideoIndex.fPosNextRecord = pNewFile->AppendToFile((char *)&videoIndex, sizeof(struct IndexHdr));
				pNewFile->WriteToFile(prevVideoIndexPos, (char *)&prevVideoIndex, sizeof(struct IndexHdr));
				prevVideoIndexPos = prevVideoIndex.fPosNextRecord;
				CopyMemory(&prevVideoIndex, &videoIndex, sizeof(struct IndexHdr));

				if (fPosJoin != 0LL)
				{
					pNewFile->ReadFromFile((char *)&videoIndex, sizeof(struct IndexHdr), fPosJoin); // Read the right join
					videoIndex.fPosNextRecord = prevVideoIndexPos;
					pNewFile->WriteToFile(fPosJoin, (char *)&videoIndex, sizeof(struct IndexHdr));
				}
			}
			else
			{
				pNewFile->ReadFromFile((char *)&videoIndex, sizeof(struct IndexHdr), fPosJoin); // Read the right join
				videoIndex.fPosPrevRecord = prevVideoIndexPos;
				pNewFile->WriteToFile(fPosJoin, (char *)&videoIndex, sizeof(struct IndexHdr));
				prevVideoIndex.fPosNextRecord = fPosJoin;
				pNewFile->WriteToFile(prevVideoIndexPos, (char *)&prevVideoIndex, sizeof(struct IndexHdr));
				prevVideoIndexPos = fPosJoin;
			}

			return prevVideoIndexPos;

		}
		numVideo++;
		if (numVideo > videoIndex.numRecs)
		{
			indexReadCnt++;
			long long pos = videoIndex.fPosNextRecord;
			videoIndex.fPosNextRecord = NULL;
			if (prevVideoIndexPos == NULL)
			{
				videoIndex.fPosPrevRecord = NULL;
				pVideoIndex->dataRecs[0].fPosData = pNewFile->AppendToFile((char *)&videoIndex, sizeof(struct IndexHdr));
				prevVideoIndexPos = pVideoIndex->dataRecs[0].fPosData;
			}
			else
			{
				videoIndex.fPosPrevRecord = prevVideoIndexPos;
				prevVideoIndex.fPosNextRecord = pNewFile->AppendToFile((char *)&videoIndex, sizeof(struct IndexHdr));
				pNewFile->WriteToFile(prevVideoIndexPos, (char *)&prevVideoIndex, sizeof(struct IndexHdr));
				prevVideoIndexPos = prevVideoIndex.fPosNextRecord;
			}
			CopyMemory(&prevVideoIndex, &videoIndex, sizeof(struct IndexHdr));

			if (pos == 0L)
			{
				// end of file
				break;
			}
			else
			{
				pOldFile->ReadFromFile((char *)&videoIndex, sizeof(struct IndexHdr), pos);
				numVideo = 0;
			}
		}
	}

	return indexReadCnt;
}

long long FileContext::CopyEventRecords(long long fPos, char *pRecBuffer, DataFile *pOldFile, DataFile *pNewFile, struct channelHdr *pChannelHdr, ULONG branchIndex)
{
	long long posTaskListDest = 0L;
	long long posTaskListOld = NULL;
	struct eventFileRecord eventRecNew, eventRecOld;
	ZeroMemory(&eventRecNew, sizeof(struct eventFileRecord));
	ZeroMemory(&eventRecOld, sizeof(struct eventFileRecord));
	long long posTaskListFirst = NULL;

	spdlog::info("Copying Event Records");
	while (fPos != NULL)
	{
		pOldFile->ReadFromFile((char *)&eventRecNew, sizeof(struct eventFileRecord), fPos);
		if (eventRecNew.nParentEventCode != branchIndex)
		{
			branchRecordUpdateList.push_back(posTaskListOld);
			break;
		}
		pOldFile->ReadDeflateBlockFromFile((char *)pRecBuffer, eventRecNew.size, eventRecNew.compSize, eventRecNew.dataPos);
		fPos = eventRecNew.fPosNextEvent;
		eventRecNew.dataPos = pNewFile->AppendDeflateBlockToFile((char *)pRecBuffer, eventRecNew.size, &eventRecNew.compSize);
		eventRecNew.fPosNextEvent = NULL;
		eventRecNew.fPosPrevEvent = posTaskListOld;
		posTaskListDest = pNewFile->AppendToFile((char *)&eventRecNew, sizeof(struct eventFileRecord));

		if (posTaskListOld == NULL)
		{
			posTaskListFirst = posTaskListDest;
		}
		else
		{
			if (branchRecordUpdateList.size() > 0)
			{
				list <long long>::iterator itBranchList = branchRecordUpdateList.begin();
				while (itBranchList != branchRecordUpdateList.end())
				{
					long long branchPos = *itBranchList;
					struct eventFileRecord branchEventRec;
					pNewFile->ReadFromFile((char *)&branchEventRec,sizeof(struct eventFileRecord), branchPos);
					branchEventRec.fPosNextEvent = posTaskListDest;
					pNewFile->WriteToFile(branchPos, (char *)&branchEventRec, sizeof(struct eventFileRecord));
					itBranchList++;
				}
				branchRecordUpdateList.clear();
			}
			eventRecOld.fPosNextEvent = posTaskListDest;
			pNewFile->WriteToFile(posTaskListOld, (char *)&eventRecOld, sizeof(struct eventFileRecord));
		}

		if (eventRecNew.fPosBottom != 0)
		{
			eventRecNew.fPosBottom = CopyEventRecords(eventRecNew.fPosBottom, pRecBuffer, pOldFile, pNewFile, pChannelHdr, eventRecNew.eventIndex);
			pNewFile->WriteToFile(posTaskListDest, (char *)&eventRecNew, sizeof(struct eventFileRecord));
		}
		if (eventRecNew.fPosRight != 0)
		{
			eventRecNew.fPosRight = CopyEventRecords(eventRecNew.fPosRight, pRecBuffer, pOldFile, pNewFile, pChannelHdr, eventRecNew.eventIndex);
			pNewFile->WriteToFile(posTaskListDest, (char *)&eventRecNew, sizeof(struct eventFileRecord));
		}
		posTaskListOld = posTaskListDest;
		CopyMemory(&eventRecOld, &eventRecNew, sizeof(struct eventFileRecord));
	}

	return posTaskListFirst;
}

void FileContext::AddNewEventRecord(struct eventListElement *pEventListElement)
{
	spdlog::info("Add New Event Record");
	char *pRecBuffer = (char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 10240);
	struct eventDetails eventDets;
	struct eventFileRecord eventRec;
	ZeroMemory(&eventRec, sizeof(struct eventFileRecord));
	CreateEventDetails(&eventDets, pEventListElement->pElement);
	PackEventData(&eventRec, &eventDets, pRecBuffer);
	eventRec.fPosBottom = eventRec.fPosRight = 0L;
	if (pEventListElement->nextElement != NULL)
		eventRec.fPosNextEvent = pEventListElement->nextElement->fPosCurr;
	if (pEventListElement->prevElement != NULL)
		eventRec.fPosPrevEvent = pEventListElement->prevElement->fPosCurr;
	
	eventRec.dataPos = pDataFile->AppendDeflateBlockToFile((char *)pRecBuffer, eventRec.size, &eventRec.compSize);
	pEventListElement->fPosCurr = pDataFile->AppendToFile((char *)&eventRec, sizeof(struct eventFileRecord));

	HeapFree(GetProcessHeap(), NULL, pRecBuffer);
}

void FileContext::SaveEventDetails(ULONG eventIndex)
{
	map<ULONG, struct eventListElement *> *pEventMapLocal = pBPMNFacade->GetElementMap();
	map<ULONG, struct eventListElement *>::iterator itEvent = pEventMapLocal->find(eventIndex);
	if (itEvent != pEventMapLocal->end())
	{
		struct eventListElement *pEventList = itEvent->second;
		RewriteEventRecord(pEventList);
	}
}

void FileContext::RewriteEventRecord(struct eventListElement *pEventListElement)
{
	spdlog::info("Rewrite Event Records");

	char *pRecBuffer = (char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 10240);
	struct eventDetails eventDets;
	struct eventFileRecord eventRec;
	ZeroMemory(&eventRec, sizeof(struct eventFileRecord));
	CreateEventDetails(&eventDets, pEventListElement->pElement);
	PackEventData(&eventRec, &eventDets, pRecBuffer);
/*-------------------------------------------------------------------------------------------*/
	struct eventListElement *pBottomListElement = NULL;
	struct eventListElement *pRightListElement = NULL;

	if (pEventListElement->pElement->GetType() == 3)
	{
		BPMNSubProcess *pSubProcess = (BPMNSubProcess *)pEventListElement->pElement;
		BPMNElement *pElement = pSubProcess->GetFirstElement();
		if (pElement != NULL)
		{
			map <ULONG, struct eventListElement *>::iterator itEvent = pEventMap->find(pElement->GetEventIndex());
			if (itEvent != pEventMap->end())
			{
				pBottomListElement = itEvent->second;
			}
		}
	}
	if (pEventListElement->pElement->GetType() == 4)
	{
		BPMNDecision *pDecision = (BPMNDecision *)pEventListElement->pElement;
		BPMNElement *pElement = pDecision->GetFirstBottomElement();
		if (pElement != NULL)
		{
			map <ULONG, struct eventListElement *>::iterator itEvent = pEventMap->find(pElement->GetEventIndex());
			if (itEvent != pEventMap->end())
			{
				pBottomListElement = itEvent->second;
			}
		}

		pElement = pDecision->GetFirstRightElement();
		if (pElement != NULL)
		{
			map <ULONG, struct eventListElement *>::iterator itEvent = pEventMap->find(pElement->GetEventIndex());
			if (itEvent != pEventMap->end())
			{
				pRightListElement = itEvent->second;
			}
		}
	}

	struct eventListElement *pPrevEventListElement = pEventListElement->prevElement;
	struct eventListElement *pNextEventListElement = pEventListElement->nextElement;
	eventRec.fPosNextEvent = NULL;
	eventRec.fPosPrevEvent = NULL;
	if (pPrevEventListElement != NULL)
	{
		eventRec.fPosPrevEvent = pPrevEventListElement->fPosCurr;
	}
	if (pNextEventListElement != NULL)
	{
		eventRec.fPosNextEvent = pNextEventListElement->fPosCurr;
	}
	if (pBottomListElement != NULL)
	{
		eventRec.fPosBottom = pBottomListElement->fPosCurr;
	}
	if (pRightListElement != NULL)
	{
		eventRec.fPosRight = pRightListElement->fPosCurr;
	}

/*------------------------------------------------------------------------------------------*/
	eventRec.dataPos = pDataFile->AppendDeflateBlockToFile((char *)pRecBuffer, eventRec.size, &eventRec.compSize);
	pDataFile->WriteToFile(pEventListElement->fPosCurr,(char *)&eventRec, sizeof(struct eventFileRecord));
	HeapFree(GetProcessHeap(), NULL, pRecBuffer);
}

void FileContext::UpdateEventRecord(struct eventListElement *pEventListElement, VideoSegmentClass *pCopySegment)
{
	spdlog::info("Update Event Records");
	struct eventFileRecord eventRec;

	FileContext *pContextFile = NULL;
	if (pCopySegment != NULL)
		pContextFile = pCopySegment->GetFileContext();
	else
		pContextFile = this;

	pContextFile->ReadFromFile((char *)&eventRec, sizeof(struct eventFileRecord), pEventListElement->fPosCurr);
	if (pEventListElement->pElement->GetParentElement() == NULL)
		eventRec.nParentEventCode = 0;
	else
		eventRec.nParentEventCode = pEventListElement->pElement->GetParentElement()->GetEventIndex();
	eventRec.nEventType = GetElementRecordType(pEventListElement->pElement);
	struct eventListElement *pBottomListElement = NULL;
	struct eventListElement *pRightListElement = NULL;

	if (pEventListElement->pElement->GetType() == 3)
	{
		BPMNSubProcess *pSubProcess = (BPMNSubProcess *)pEventListElement->pElement;
		BPMNElement *pElement = pSubProcess->GetFirstElement();
		if (pElement != NULL)
		{
			map <ULONG, struct eventListElement *>::iterator itEvent = pEventMap->find(pElement->GetEventIndex());
			if (itEvent != pEventMap->end())
			{
				pBottomListElement = itEvent->second;
			}
		}
	}
	if (pEventListElement->pElement->GetType() == 4)
	{
		BPMNDecision *pDecision = (BPMNDecision *)pEventListElement->pElement;
		BPMNElement *pElement = pDecision->GetFirstBottomElement();
		if (pElement != NULL)
		{
			map <ULONG, struct eventListElement *>::iterator itEvent = pEventMap->find(pElement->GetEventIndex());
			if (itEvent != pEventMap->end())
			{
				pBottomListElement = itEvent->second;
			}
		}

		pElement = pDecision->GetFirstRightElement();
		if (pElement != NULL)
		{
			map <ULONG, struct eventListElement *>::iterator itEvent = pEventMap->find(pElement->GetEventIndex());
			if (itEvent != pEventMap->end())
			{
				pRightListElement = itEvent->second;
			}
		}
	}

	struct eventListElement *pPrevEventListElement = pEventListElement->prevElement;
	struct eventListElement *pNextEventListElement = pEventListElement->nextElement;
	eventRec.fPosNextEvent = NULL;
	eventRec.fPosPrevEvent = NULL;
	if (pPrevEventListElement != NULL)
	{
		eventRec.fPosPrevEvent = pPrevEventListElement->fPosCurr;
	}
	if (pNextEventListElement != NULL)
	{
		eventRec.fPosNextEvent = pNextEventListElement->fPosCurr;
	}
	if (pBottomListElement != NULL)
	{
		eventRec.fPosBottom = pBottomListElement->fPosCurr;
	}
	if (pRightListElement != NULL)
	{
		eventRec.fPosRight = pRightListElement->fPosCurr;
	}
	pDataFile->WriteToFile(pEventListElement->fPosCurr, (char *)&eventRec, sizeof(struct eventFileRecord));
}

void FileContext::SetEventRoot(struct eventListElement *pEventListElement)
{
	pEventListRoot = pEventListElement;
	ChannelDataMapping *pChannelData = GetChannelDataMapping(mainChannelNumber);
	struct channelHdr *pChannelHdr = pChannelData->GetChannelHeader();
	pChannelHdr->posTaskListRoot = pEventListElement->fPosCurr;
	// Rewrite main channel header
	pDataFile->WriteToFile(rootRecord.posChannelRecords, (char *)pChannelHdr, sizeof(struct channelHdr));
}

void FileContext::UpdateChannelHeader(int channelNum)
{
	ChannelDataMapping *pChannelData = GetChannelDataMapping(channelNum);
	struct channelHdr *pChannelHdr = pChannelData->GetChannelHeader();
	// Rewrite main channel header
	pDataFile->WriteToFile(pChannelHdr->posCurrRec, (char *)pChannelHdr, sizeof(struct channelHdr));
}

int FileContext::GetElementRecordType(BPMNElement *pElement)
{
	int recType = 0;

	int type = pElement->GetType();
	if (type == 2) // task
		recType = 1;
	else if (type == 3)
		recType = 2;
	else if (type == 4)
		recType = 4;

	BPMNElement *pParent = pElement->GetParentElement();
	if (pParent != NULL)
	{
		type = pParent->GetType();
		if (type == 3)
			recType = recType | 0x40; // this is a sub-process child
		else if (type == 4)
		{
			BPMNDecision *pDecision = (BPMNDecision *)pParent;
			recType = recType | pDecision->GetElementRecordType(pElement);
		}
	}
	return recType;
}

void FileContext::CreateEventDetails(struct eventDetails *pEventDetails, BPMNElement *pElement)
{
	pEventDetails->nEventRecType = GetElementRecordType(pElement);
	pEventDetails->timestamp = pElement->GetEventTimestamp();
	pEventDetails->nEventCode = pElement->GetEventType();
	if (pElement->GetParentElement() != NULL)
		pEventDetails->nParentEventIndex = pElement->GetParentElement()->GetEventIndex();
	else
		pEventDetails->nParentEventIndex = 0;
	CopyMemory(&pEventDetails->boundingRect, pElement->GetBoundingRect(), sizeof(RECT));
	//pEventDetails->nVK_Code = pElement->GetEventAdditionalInfo();
	//pEventDetails->nScan_Code = 0;
	pEventDetails->eventIndex = pElement->GetEventIndex();
	pEventDetails->strAnnotation = pElement->GetAnnotation();
	pEventDetails->strNotes = pElement->GetNotes();
}

void FileContext::PackEventData(struct eventFileRecord *pEventRecord, struct eventDetails *pEventDetails, char *pRecBuffer)
{
	pEventRecord->nEventType = pEventDetails->nEventRecType;
	pEventRecord->timestamp = pEventDetails->timestamp;
	pEventRecord->nEventCode = pEventDetails->nEventCode;
	
	//pEventRecord->nVK_Code = pEventDetails->nVK_Code;
	//pEventRecord->nScan_Code = pEventDetails->nScan_Code;
	pEventRecord->eventIndex = pEventDetails->eventIndex;
	pEventRecord->nParentEventCode = pEventDetails->nParentEventIndex;
	CopyMemory(&pEventRecord->boundingRect, &pEventDetails->boundingRect, sizeof(RECT));
	pEventRecord->size = 0;
	pEventRecord->compSize = 0;
	pEventRecord->fPosNextEvent = 0L;
	pEventRecord->fPosPrevEvent = 0L;
	
	char *pBuffer = pRecBuffer;

	pEventRecord->stringStart[0] = 0;
	pEventRecord->stringLengths[0] = pEventDetails->strAnnotation.size() + 1;
	CopyMemory(pBuffer + pEventRecord->stringStart[0], pEventDetails->strAnnotation.c_str(), pEventRecord->stringLengths[0]);

	pEventRecord->stringStart[1] = pEventRecord->stringStart[0] + pEventRecord->stringLengths[0] + 1;
	pEventRecord->stringLengths[1] = pEventDetails->strNotes.size() + 1;
	CopyMemory(pBuffer + pEventRecord->stringStart[1], pEventDetails->strNotes.c_str(), pEventRecord->stringLengths[1]);

	pEventRecord->stringStart[2] = pEventRecord->stringStart[1] + pEventRecord->stringLengths[1] + 1;
	pEventRecord->stringLengths[2] = pEventDetails->strKeyStrokes.size() + 1;
	CopyMemory(pBuffer + pEventRecord->stringStart[2], pEventDetails->strKeyStrokes.c_str(), pEventRecord->stringLengths[2]);

	pEventRecord->size = pEventRecord->stringStart[1] + pEventRecord->stringLengths[1] + pEventRecord->stringStart[2];
}

void FileContext::UnpackEventData(struct eventDetails *pEventDetails, struct eventFileRecord *pEventRecord,  char *pRecBuffer)
{
	pEventDetails->nEventRecType = pEventRecord->nEventType;
	pEventDetails->timestamp = pEventRecord->timestamp;
	//pEventDetails->nEventCode = pEventRecord->nEventCode;
	//pEventDetails->nVK_Code = pEventRecord->nVK_Code;
	pEventDetails->eventIndex = pEventRecord->eventIndex;
	pEventDetails->timestamp = pEventRecord->timestamp;
	pEventDetails->nParentEventIndex = pEventRecord->nParentEventCode;
	CopyMemory(&pEventDetails->boundingRect, &pEventRecord->boundingRect, sizeof(RECT));
	char *pBuffer = pRecBuffer;
	pBuffer = pBuffer + pEventRecord->stringStart[0];
	pEventDetails->strAnnotation = pBuffer;

	pBuffer = pRecBuffer + pEventRecord->stringStart[1];
	pEventDetails->strNotes = pBuffer;

	pBuffer = pRecBuffer + pEventRecord->stringStart[2];
	pEventDetails->strKeyStrokes = pBuffer;
}


RECT *FileContext::GetBoundingRect(int eventIndex)
{
	return pBPMNFacade->GetBoundingRect(eventIndex);
}

void FileContext::CreateChannelDataMapping()
{
	spdlog::info("Create Channel Data Mapping");

	channelDataMap.clear();

	// Load Audio & Video Headers and then start players Player
	pDataFile->ReadFromFile((char *)&rootRecord, sizeof(struct rootHdr), 0L);
	if (rootRecord.RecType != 'R' || strcmp(rootRecord.signature, "PECAN1.0"))
	{
		return;
	}

	long long channelFpos = rootRecord.posChannelRecords;
	mainChannelNumber = -1;
	for (int i = 0; i < rootRecord.noOfChannels; i++)
	{
		ChannelDataMapping *pChannelDataMapping = DBG_NEW ChannelDataMapping;

		pChannelDataMapping->SetDataFile(pDataFile);
		pChannelDataMapping->SetRecordBufferManager(pRecBufferManager);
		pChannelDataMapping->SetFileContext(this);

		USHORT channelNum = pChannelDataMapping->ReadChannelHeader(channelFpos);
		if (channelNum == USHRT_MAX)
			break;
		if (mainChannelNumber == USHRT_MAX)
			mainChannelNumber = channelNum;
		if (currContextChannel == USHRT_MAX)
			currContextChannel = channelNum;
		channelDataMap.insert(pair<USHORT, ChannelDataMapping *>(channelNum, pChannelDataMapping));
		if (mainChannelNumber == channelNum)
			pChannelDataMapping->ReadVideoData(true, &mapVideoPages);
		//else
		//	pChannelDataMapping->ReadVideoData(false, &mapVideoPages);
		// pChannelDataMapping->ReadAudioData(&mapAudioPages);
		channelFpos = pChannelDataMapping->GetNextChannelPosition();
	}
	CreateBPMNList();
}

void FileContext::UpdateIndexPageDescriptor(long long keyVal, struct IndexHdr *value)
{

	spdlog::info("Update Event Index Descriptor");

	map <long long, struct indexPageDescriptor *>::iterator itFind = mapVideoPages.find(keyVal);
	if (itFind != mapVideoPages.end())
	{
		struct indexPageDescriptor *pDescriptor = itFind->second;
		if (value->RecType == 'P')
		{
			struct videoIndexDescriptor *pVideo = pDescriptor->pageDescriptor.pVideo;
			pDescriptor->numRecs = value->numRecs;
			pVideo->timespan = 0;
			pVideo->nextFpos = value->fPosNextRecord;
			pVideo->prevFpos = value->fPosPrevRecord;

		}
		else if (value->RecType == 'J')
		{
			struct joinIndexDescriptor *pJoin = pDescriptor->pageDescriptor.pJoin;
			pDescriptor->numRecs = value->numRecs;
			pJoin->nextFpos = value->fPosNextRecord;
			pJoin->prevTopFpos = value->fPosPrevRecord;
			pJoin->prevRightFpos = value->dataRecs[0].fPosData;

		}
		else if (value->RecType == 'B')
		{
			struct branchIndexDescriptor *pBranch = pDescriptor->pageDescriptor.pBranch;
			pDescriptor->numRecs = value->numRecs;
			pBranch->nextBottomFPos = value->fPosNextRecord;
			pBranch->prevFpos = value->fPosPrevRecord;
			pBranch->nextRightFPos = value->dataRecs[0].fPosData;

		}
		else if (value->RecType == 'H')
		{
			struct channelIndexDescriptor *pChannelIndex = pDescriptor->pageDescriptor.pChannel;
			pDescriptor->numRecs = value->numRecs;
			pChannelIndex->newChannel = value->dataRecs[0].size;
			pChannelIndex->prevChannel = value->dataRecs[0].compSize;
			pChannelIndex->nextFPos = value->fPosNextRecord;
			pChannelIndex->prevFpos = value->fPosPrevRecord;

		}
		
		for (int i = 0; i <= value->numRecs; i++)
		{
			pDescriptor->dataRecs[i].indexNum = value->dataRecs[i].videoIndexNumber;
			pDescriptor->dataRecs[i].recDuration = (long)value->dataRecs[i].recDurationus;
			if (value->RecType == 'P')
				pDescriptor->pageDescriptor.pVideo->timespan += value->dataRecs[i].recDurationus;
		}
		pDescriptor->next = NULL;
		itFind = mapVideoPages.find(value->fPosNextRecord);
		if (itFind != mapVideoPages.end())
		{
			pDescriptor->next = itFind->second;
		}
	}
}



int FileContext::AddNewChannel(ChannelDataMapping *pChannelSrc)
{
	struct channelHdr *pChannelHdr = AddNewChannel(); // Add channel to the channels in the destination
	CopyVideoAndAudioHeaders(pChannelHdr, pChannelSrc);
	return pChannelHdr->channelNum;
}

struct channelHdr *FileContext::AddNewChannel()
{
	spdlog::info("Add New Channel");
	ChannelDataMapping *pChannelDataMapping = NULL;
	int lastChannelNum = -1;
	struct channelHdr *pChannel = NULL;
	struct channelHdr *pPrevChannel = NULL;
	ChannelDataMapping *pPrevChannelDataMapping = NULL;

	// Find last channel Number
	map<USHORT, ChannelDataMapping *>::iterator itFind = channelDataMap.begin();

	while (itFind != channelDataMap.end())
	{
		pChannelDataMapping = itFind->second;
		pChannel = pChannelDataMapping->GetChannelHeader();
		if ((int)pChannel->channelNum > lastChannelNum)
		{
			lastChannelNum = pChannel->channelNum;
			pPrevChannelDataMapping = pChannelDataMapping;
		}
		itFind++;
	}

	lastChannelNum++;
	rootRecord.noOfChannels++;
	pChannelDataMapping = DBG_NEW ChannelDataMapping;

	pChannelDataMapping->SetDataFile(pDataFile);
	pChannelDataMapping->SetRecordBufferManager(pRecBufferManager);
	pChannelDataMapping->SetFileContext(this);

	channelDataMap.insert(pair<USHORT, ChannelDataMapping *>(lastChannelNum, pChannelDataMapping));
	pPrevChannel = pPrevChannelDataMapping->GetChannelHeader();
	pChannel = pChannelDataMapping->GetChannelHeader();
	ZeroMemory(pChannel, sizeof(struct channelHdr));

	pChannel->channelNum = lastChannelNum;
	pChannel->RecType = 'C';

	pPrevChannel->fPosNextChannel = pDataFile->AppendToFile((char *)pChannel, sizeof(struct channelHdr));
	pChannel->posCurrRec = pPrevChannel->fPosNextChannel;
	pDataFile->WriteToFile(pPrevChannel->posCurrRec, (char *)pPrevChannel, sizeof(struct channelHdr));
	pDataFile->WriteToFile(pChannel->posCurrRec, (char *)pChannel, sizeof(struct channelHdr));
	pDataFile->WriteToFile(0L, (char *)&rootRecord, sizeof(struct rootHdr));
	return pChannel;
}


void FileContext::CopyVideoAndAudioHeaders(struct channelHdr *pChannelHdr, ChannelDataMapping *pChannelSrc)
{
	// Read Source Video Header
	//struct channelHdr *pChannelHdrSource = pChannelSrc->GetChannelHeader();
	struct VideoHdr *pVHeader, *pVHeaderNew;
	struct AudioHdr *pAHeader, *pAHeaderNew;
	ChannelDataMapping *pSourceChannel = pChannelSrc;
	ChannelDataMapping *pNewChannel = GetChannelDataMapping(pChannelHdr->channelNum);

	pVHeader = pSourceChannel->GetVideoHeader();
	pVHeaderNew = pNewChannel->GetVideoHeader();

	ZeroMemory(pVHeaderNew, sizeof(struct VideoHdr));
	pVHeaderNew->fps = pVHeader->fps;
	pVHeaderNew->noMonitors = pVHeader->noMonitors;
	pVHeaderNew->RecType = pVHeader->RecType;

	// Read Monitor Info from the old channel
	struct MonitorHdr *pMonHeaders = DBG_NEW struct MonitorHdr[pVHeader->noMonitors];
	pSourceChannel->GetChannelMonitorInfo(pMonHeaders);
	pNewChannel->AddChannelMonitorInfo(pMonHeaders);
	delete [] pMonHeaders;
	pNewChannel->AddVideoHeader();

	// Copy and Write Audio Header
	pAHeader = pSourceChannel->GetAudioHeader();
	pAHeaderNew = pNewChannel->GetAudioHeader();
	ZeroMemory(pAHeaderNew, sizeof(struct AudioHdr));
	pAHeaderNew->bitsPerSample = pAHeader->bitsPerSample;
	pAHeaderNew->freq = pAHeader->freq;
	pAHeaderNew->noChannels = pAHeader->noChannels;
	pAHeaderNew->RecType = pAHeader->RecType;
	pAHeaderNew->wFormatTag = pAHeader->wFormatTag;
	pNewChannel->AddAudioHeader();
	UpdateChannelHeader(pChannelHdr->channelNum);
	pNewChannel->CreateScreenBuffers();
	pNewChannel->CreateBitmapMerge(bActiveMonitorOn, playerWindowWidth, playerWindowHeight);
}

bool FileContext::GetNextAudioIndex(struct IndexHdr *pIndex)
{
	ChannelDataMapping *pChannelData = GetChannelDataMapping(currContextChannel);
	return pChannelData->GetNextAudio(pIndex);
}

bool FileContext::GetNextVideoIndex(struct IndexHdr *pIndex)
{
	ChannelDataMapping *pChannelData = GetChannelDataMapping(currContextChannel);
	if (pIndex->RecType == 0) // Request for first record
	{
		currVideoFPos = pChannelData->GetRootVideoPosition();
	}
	else
	{
		currVideoFPos = pIndex->fPosNextRecord;
	}
//	return pChannelData->GetNextVideo(pIndex, &currVideoFPos, &mapVideoPages);

	while (currVideoFPos != 0LL)
	{
		pDataFile->ReadFromFile((char *)pIndex, sizeof(struct IndexHdr), currVideoFPos);
		if (pIndex->RecType == 'P')
			break;
		if (pIndex->RecType == 'H')
		{
			currContextChannel = pIndex->dataRecs[0].size;
			currVideoFPos = pIndex->fPosNextRecord;		// Perform a Channel Switch operation
		}
		else if (pIndex->RecType == 'B')
		{
			// Search for IndexDescriptor record for this branch, Check the branch direction
			// Then get the next branch record
			map <long long, struct indexPageDescriptor *>::iterator itMap = mapVideoPages.find(currVideoFPos);
			if (itMap == mapVideoPages.end())
			{
				currVideoFPos = 0LL;
				break;
			}
			struct indexPageDescriptor *pDescriptor = itMap->second;
			if (pDescriptor->nPageType != 2)
			{
				currVideoFPos = 0LL;
				break;
			}
			struct branchIndexDescriptor *pBranch = pDescriptor->pageDescriptor.pBranch;
			if (pBranch->bPathDir == true)
			{
				currVideoFPos = pIndex->dataRecs[0].fPosData;
			}
			else
			{
				currVideoFPos = pIndex->fPosNextRecord;
			}

		}
		else if (pIndex->RecType == 'J')
		{
			currVideoFPos = pIndex->fPosNextRecord;
		}
	}
	if (currVideoFPos == 0LL)
	{
		return true; // no more records
	}
	return false;

}

long long FileContext::InitializeVideoPath()
{
	// Read Main Channel Header file position, search in map
	// start time = 0;
	// read index header, if index header is video then add time of index header to total time, go to next record
	// if branch then search the next index in the path
	// if joint then search the next index in the path

	long long startTime = 0L;
	long long fPosPrev = 0L;

	struct indexPageDescriptor *currIndexPage = NULL;

	ChannelDataMapping *pChannelData = GetChannelDataMapping(mainChannelNumber);
	struct VideoHdr videoHdr;
	long long fPos = 0L;
	pChannelData->GetVideoHeader(&videoHdr);
	fPos = videoHdr.fPosRoot;

	map <long long, struct indexPageDescriptor *>::iterator itFind = mapVideoPages.find(fPos);

	while (itFind != mapVideoPages.end())
	{
		currIndexPage = itFind->second;
		if (currIndexPage->nPageType == 1)
		{
			struct videoIndexDescriptor *pVideo = currIndexPage->pageDescriptor.pVideo;
			pVideo->inPath = false;
		}
		itFind++;
	}

	itFind = mapVideoPages.find(fPos);
	if (itFind != mapVideoPages.end())
	{
		currIndexPage = itFind->second;
	}
	while (currIndexPage != NULL)
	{
		if (currIndexPage->nPageType == 2) // Branch index
		{
			struct branchIndexDescriptor *pBranch = currIndexPage->pageDescriptor.pBranch;
			currIndexPage = NULL;
			fPosPrev = fPos;
			if (pBranch->bPathDir) // branches to the right
				fPos = pBranch->nextRightFPos;
			else
				fPos = pBranch->nextBottomFPos;

			itFind = mapVideoPages.find(fPos);
			if (itFind != mapVideoPages.end())
				currIndexPage = itFind->second;
		}
		else if (currIndexPage->nPageType == 3) // Join Index
		{
			//struct joinIndexDescriptor *pJoin = currIndexPage->pageDescriptor.pJoin;
			fPosPrev = fPos;
			currIndexPage = currIndexPage->next;
		}
		else if (currIndexPage->nPageType == 1) // Video Page
		{
			struct videoIndexDescriptor *pVideo = currIndexPage->pageDescriptor.pVideo;
			pVideo->startTime = startTime;
			startTime += pVideo->timespan;
			fPosPrev = fPos;
			currIndexPage = currIndexPage->next;
			pVideo->inPath = true;
		}
		else if (currIndexPage->nPageType == 5) // Channel Page
		{
			struct channelIndexDescriptor *pChannel = currIndexPage->pageDescriptor.pChannel;
			pChannel->startTime = startTime;
			fPosPrev = fPos;
			currIndexPage = currIndexPage->next;
		}

		if (currIndexPage != NULL)
		{
			if (currIndexPage->nPageType == 1)
			{
				struct videoIndexDescriptor *pVideo = currIndexPage->pageDescriptor.pVideo;
				fPos = pVideo->currFPos;
			}
			else if (currIndexPage->nPageType == 5)
			{
				struct channelIndexDescriptor *pChannel = currIndexPage->pageDescriptor.pChannel;
				fPos = pChannel->currFPos;
			}
		}
	}
	ClearEventsInPath();
	if (pEventListRoot != NULL)
		SetDescriptorEvents();
	
	return startTime;
}


void FileContext::GetScenarioData(struct scenarioReportData *pScenarioData, ScenarioDefintion *pScenarioDef)
{
	// Read Main Channel Header file position, search in map
	// start time = 0;
	// read index header, if index header is video then add time of index header to total time, go to next record
	// if branch then search the next index in the path
	// if joint then search the next index in the path

	long long fPosPrev = 0L;
	long long startTime = 0L;
	UINT numSteps = 0L;

	struct indexPageDescriptor *currIndexPage = NULL;
	ChannelDataMapping *pChannelData = GetChannelDataMapping(mainChannelNumber);
	struct VideoHdr videoHdr;
	long long fPos = 0L;
	pChannelData->GetVideoHeader(&videoHdr);
	fPos = videoHdr.fPosRoot;

	map <long long, struct indexPageDescriptor *>::iterator itFind = mapVideoPages.find(fPos);
	if (itFind != mapVideoPages.end())
	{
		currIndexPage = itFind->second;
	}
	while (currIndexPage != NULL)
	{
		if (currIndexPage->nPageType == 2) // Branch index
		{
			struct branchIndexDescriptor *pBranch = currIndexPage->pageDescriptor.pBranch;
			fPosPrev = fPos;
			ULONG nIndex = currIndexPage->dataRecs[0].indexNum;
			short nPath = 0;
			if (pScenarioDef != NULL)
				nPath = pScenarioDef->GetPath(nIndex);
			if (nPath == 0)
			{
				fPos = pBranch->nextBottomFPos;
			}
			else
			{
				fPos = pBranch->nextRightFPos;
			}
			itFind = mapVideoPages.find(fPos);
			if (itFind != mapVideoPages.end())
				currIndexPage = itFind->second;
		}
		else if (currIndexPage->nPageType == 3) // Join Index
		{
			//struct joinIndexDescriptor *pJoin = currIndexPage->pageDescriptor.pJoin;
			fPosPrev = fPos;
			currIndexPage = currIndexPage->next;
		}
		else if (currIndexPage->nPageType == 1) // Video Page
		{
			//struct videoIndexDescriptor *pVideo = currIndexPage->pageDescriptor.pVideo;
			fPosPrev = fPos;
			startTime += currIndexPage->pageDescriptor.pVideo->timespan;
			for (int numI = 0; numI < currIndexPage->numRecs; numI++)
			{
				struct eventListElement *pEventList = NULL;
				map <ULONG, struct eventListElement *>::iterator itEventMap = pEventMap->find(currIndexPage->dataRecs[numI].indexNum);
				if (itEventMap != pEventMap->end())
				{
					pEventList = itEventMap->second;
					if (pEventList->pElement->GetType() == 2) // Task Event
					{
						numSteps++;
					}
				}
			}
			currIndexPage = currIndexPage->next;
		}
		else if (currIndexPage->nPageType == 5) // Channel Page
		{
			//struct channelIndexDescriptor *pChannel = currIndexPage->pageDescriptor.pChannel;
			fPosPrev = fPos;
			currIndexPage = currIndexPage->next;
		}

		if (currIndexPage != NULL)
		{
			if (currIndexPage->nPageType == 1)
			{
				struct videoIndexDescriptor *pVideo = currIndexPage->pageDescriptor.pVideo;
				fPos = pVideo->currFPos;
			}
			else if (currIndexPage->nPageType == 5)
			{
				struct channelIndexDescriptor *pChannel = currIndexPage->pageDescriptor.pChannel;
				fPos = pChannel->currFPos;
			}
		}
	}

	pScenarioData->noOfSteps = numSteps;
	pScenarioData->timeInSecs = (double)startTime / 1000000.0;
}

void FileContext::SaveOrGetScenario(bool bSave, ScenarioDefintion *pScenario)
{
	// Read Main Channel Header file position, search in map
	// start time = 0;
	// read index header, if index header is video then add time of index header to total time, go to next record
	// if branch then search the next index in the path
	// if joint then search the next index in the path

	long long fPosPrev = 0L;

	struct indexPageDescriptor *currIndexPage = NULL;
	ChannelDataMapping *pChannelData = GetChannelDataMapping(mainChannelNumber);
	struct VideoHdr videoHdr;
	long long fPos = 0L;
	pChannelData->GetVideoHeader(&videoHdr);
	fPos = videoHdr.fPosRoot;

	map <long long, struct indexPageDescriptor *>::iterator itFind = mapVideoPages.find(fPos);
	if (itFind != mapVideoPages.end())
	{
		currIndexPage = itFind->second;
	}
	while (currIndexPage != NULL)
	{
		if (currIndexPage->nPageType == 2) // Branch index
		{
			struct branchIndexDescriptor *pBranch = currIndexPage->pageDescriptor.pBranch;
			fPosPrev = fPos;
			ULONG nIndex = currIndexPage->dataRecs[0].indexNum;
			short nPath = 0;
			if (!bSave) // If setting the branch path
			{
				if (pScenario == NULL)
					nPath = 0;
				else
					nPath = pScenario->GetPath(nIndex);
				if (nPath == 0)
					pBranch->bPathDir = false;
				else
					pBranch->bPathDir = true;
			}
			else
			{
				if (pBranch->bPathDir)
					nPath = 1;
				pScenario->SetPath(nIndex, nPath);
			}
			
			SaveOrGetBranchScenario(pBranch->nextRightFPos, bSave, pScenario);
			fPos = pBranch->nextBottomFPos;
			itFind = mapVideoPages.find(fPos);
			if (itFind != mapVideoPages.end())
				currIndexPage = itFind->second;
		}
		else if (currIndexPage->nPageType == 3) // Join Index
		{
			//struct joinIndexDescriptor *pJoin = currIndexPage->pageDescriptor.pJoin;
			fPosPrev = fPos;
			currIndexPage = currIndexPage->next;
		}
		else if (currIndexPage->nPageType == 1) // Video Page
		{
			//struct videoIndexDescriptor *pVideo = currIndexPage->pageDescriptor.pVideo;
			fPosPrev = fPos;
			currIndexPage = currIndexPage->next;
		}
		else if (currIndexPage->nPageType == 5) // Channel Page
		{
			//struct channelIndexDescriptor *pChannel = currIndexPage->pageDescriptor.pChannel;
			fPosPrev = fPos;
			currIndexPage = currIndexPage->next;
		}

		if (currIndexPage != NULL)
		{
			if (currIndexPage->nPageType == 1)
			{
				struct videoIndexDescriptor *pVideo = currIndexPage->pageDescriptor.pVideo;
				fPos = pVideo->currFPos;
			}
			else if (currIndexPage->nPageType == 5)
			{
				struct channelIndexDescriptor *pChannel = currIndexPage->pageDescriptor.pChannel;
				fPos = pChannel->currFPos;
			}
		}
	}
}

void FileContext::SaveOrGetBranchScenario(long long brancRightFPos, bool bSave, ScenarioDefintion *pScenario)
{
	long long fPosPrev = 0L;

	struct indexPageDescriptor *currIndexPage = NULL;
	long long fPos = brancRightFPos;

	map <long long, struct indexPageDescriptor *>::iterator itFind = mapVideoPages.find(fPos);
	if (itFind != mapVideoPages.end())
	{
		currIndexPage = itFind->second;
	}
	while (currIndexPage != NULL)
	{
		if (currIndexPage->nPageType == 2) // Branch index
		{
			struct branchIndexDescriptor *pBranch = currIndexPage->pageDescriptor.pBranch;
			fPosPrev = fPos;
			ULONG nIndex = currIndexPage->dataRecs[0].indexNum;
			short nPath = 0;
			if (!bSave) // If setting the branch path
			{
				if (pScenario == NULL)
					nPath = 0;
				else
					nPath = pScenario->GetPath(nIndex);
				if (nPath == 0)
					pBranch->bPathDir = false;
				else
					pBranch->bPathDir = true;
			}
			else
			{
				if (pBranch->bPathDir)
					nPath = 1;
				pScenario->SetPath(nIndex, nPath);
			}
			SaveOrGetBranchScenario(pBranch->nextRightFPos, bSave, pScenario);
			fPos = pBranch->nextBottomFPos;
			itFind = mapVideoPages.find(fPos);
			if (itFind != mapVideoPages.end())
				currIndexPage = itFind->second;
		}
		else if (currIndexPage->nPageType == 3) // Join Index
		{
			//struct joinIndexDescriptor *pJoin = currIndexPage->pageDescriptor.pJoin;
			fPosPrev = fPos;
			currIndexPage = currIndexPage->next;
		}
		else if (currIndexPage->nPageType == 1) // Video Page
		{
			//struct videoIndexDescriptor *pVideo = currIndexPage->pageDescriptor.pVideo;
			fPosPrev = fPos;
			currIndexPage = currIndexPage->next;
		}
		else if (currIndexPage->nPageType == 5) // Channel Page
		{
			//struct channelIndexDescriptor *pChannel = currIndexPage->pageDescriptor.pChannel;
			fPosPrev = fPos;
			currIndexPage = currIndexPage->next;
		}

		if (currIndexPage != NULL)
		{
			if (currIndexPage->nPageType == 1)
			{
				struct videoIndexDescriptor *pVideo = currIndexPage->pageDescriptor.pVideo;
				fPos = pVideo->currFPos;
			}
			else if (currIndexPage->nPageType == 5)
			{
				struct channelIndexDescriptor *pChannel = currIndexPage->pageDescriptor.pChannel;
				fPos = pChannel->currFPos;
			}
		}
	}

}


string FileContext::GetScenarioJSON()
{
	pDataFile->ReadFromFile((char *)&rootRecord, sizeof(struct rootHdr), 0L);
	if (rootRecord.posScenarioInfo == NULL)
		return "";
	ULONG size = rootRecord.sizeOfScenarioInfo;
	ULONG compSize = rootRecord.sizeOfCompScenarioInfo;
	char *pBuffer = (char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (size_t)size + 100);
	pDataFile->ReadDeflateBlockFromFile(pBuffer, size, compSize, rootRecord.posScenarioInfo);
	if (pBuffer != NULL)
	{
		string strScenario = pBuffer;
		HeapFree(GetProcessHeap(), 0, pBuffer);
		return strScenario;
	}
	else
		return "";
}

void FileContext::SetScenarioJSON(string sceneJSON)
{
	pDataFile->ReadFromFile((char *)&rootRecord, sizeof(struct rootHdr), 0L);
	int len = sceneJSON.size() + 1;
	rootRecord.posScenarioInfo = pDataFile->AppendDeflateBlockToFile((char *)sceneJSON.c_str(), len, &rootRecord.sizeOfCompScenarioInfo);
	rootRecord.sizeOfScenarioInfo = len;
	pDataFile->WriteToFile(0L, (char *)&rootRecord, sizeof(struct rootHdr));
}

void FileContext::ClearEventsInPath()
{
	map <ULONG, struct eventListElement *>::iterator itEvent = pEventMap->begin();
	while (itEvent != pEventMap->end())
	{
		struct eventListElement *pEventListElement = itEvent->second;

		if (pEventListElement != NULL && pEventListElement->pElement != NULL)
			pEventListElement->pElement->SetInPath(false);
		itEvent++;
	}
}


void FileContext::SetBranchRight(ULONG nEventIndex)
{
	spdlog::info("Branch Right");
	struct eventListElement *pEventList = NULL;
	map <ULONG, struct eventListElement *>::iterator itEventMap = pEventMap->find(nEventIndex);
	if (itEventMap != pEventMap->end())
	{
		pEventList = itEventMap->second;
		BPMNElement *pElement = pEventList->pElement;
		if (pElement->GetType() != 4)
			return;
		struct indexPageDescriptor *pIndexPageDescriptor = FindIndexPageDescriptorForBranch(nEventIndex);
		if (pIndexPageDescriptor == NULL || pIndexPageDescriptor->nPageType != 2)
			return;
		pIndexPageDescriptor->pageDescriptor.pBranch->bPathDir = true;
	}

}

void FileContext::SetBranchBottom(ULONG nEventIndex)
{
	spdlog::info("Branch Bottom");
	struct eventListElement *pEventList = NULL;
	map <ULONG, struct eventListElement *>::iterator itEventMap = pEventMap->find(nEventIndex);
	if (itEventMap != pEventMap->end())
	{
		pEventList = itEventMap->second;
		BPMNElement *pElement = pEventList->pElement;
		if (pElement->GetType() != 4)
			return;
		struct indexPageDescriptor *pIndexPageDescriptor = FindIndexPageDescriptorForBranch(nEventIndex);
		if (pIndexPageDescriptor == NULL || pIndexPageDescriptor->nPageType != 2)
			return;
		pIndexPageDescriptor->pageDescriptor.pBranch->bPathDir = false;
	}

}

long long FileContext::SynchronizeVideo(ULONG nEventIndex)
{
	// Video is stopped
	// First find if event is a subprocess, then find first video event index
	// then find the indexDescriptor record and numIndex
	// show video screen for the video
	spdlog::info("Synchronize Video");
	long long nTime = -1L;
	struct eventListElement *pEventList = NULL;
	map <ULONG, struct eventListElement *>::iterator itEventMap = pEventMap->find(nEventIndex);
	if (itEventMap != pEventMap->end())
	{
		pEventList = itEventMap->second;
		pEventList = GetFirstEventListElement(pEventList->pElement);
		if (pEventList == NULL || pEventList->pElement == NULL)
			return -1L;
		BPMNElement *pElement = pEventList->pElement;
		ULONG nEventIndexNew = pElement->GetEventIndex();
		struct indexPageDescriptor *pIndexPageDescriptor = FindIndexPageDescriptorForEvent(nEventIndexNew);
		if (pIndexPageDescriptor == NULL || pIndexPageDescriptor->nPageType != 1)
			return -1;
		int numIndex = 0;
		for (numIndex = 0; numIndex <= pIndexPageDescriptor->numRecs; numIndex++)
		{
			if (pIndexPageDescriptor->dataRecs[numIndex].indexNum == nEventIndexNew)
				break;
		}
		// Show Video at this location;
		if (numIndex > 0)
			numIndex--;
		struct videoIndexDescriptor *pVideo = pIndexPageDescriptor->pageDescriptor.pVideo;
		nTime = GetVideoIndexTime(numIndex, pVideo->currFPos);
	}

	return nTime;

}

bool FileContext::IsBranchRight(ULONG nEventIndex)
{
	struct branchIndexDescriptor *pBranch = NULL;

	ChannelDataMapping *pChannelData = GetChannelDataMapping(mainChannelNumber);
	struct VideoHdr videoHdr;
	long long fPos = 0L;
	pChannelData->GetVideoHeader(&videoHdr);
	fPos = videoHdr.fPosRoot;
	pBranch = FindBranchDescriptor(fPos, nEventIndex, &fPos);
	if (pBranch != NULL)
		return pBranch->bPathDir;
	return false;
	
}

struct branchIndexDescriptor *FileContext::FindBranchDescriptor(long long fPos, ULONG nIndex, long long *fPosNext)
{
	struct branchIndexDescriptor *pBranch = NULL;

	while (fPos != NULL)
	{
		map <long long, struct indexPageDescriptor *>::iterator itFind = mapVideoPages.find(fPos);
		if (itFind == mapVideoPages.end())
			return NULL;
		struct indexPageDescriptor *pIndexDescriptor = itFind->second;
		if (pIndexDescriptor->nPageType == 2)
		{
			// This is a Branch Record
			if (pIndexDescriptor->dataRecs[0].indexNum == nIndex)
				return pIndexDescriptor->pageDescriptor.pBranch;
			// Now go to right branch
			struct branchIndexDescriptor *pBranchIndex = pIndexDescriptor->pageDescriptor.pBranch;
		
			// Then go to bottom branch
			long long joinFPos = 0L;
			pBranch = FindBranchDescriptor(pBranchIndex->nextRightFPos, nIndex, &joinFPos);
			if (pBranch != NULL)
				return pBranch;
			// continue from the join record
			pBranch = FindBranchDescriptor(pBranchIndex->nextBottomFPos, nIndex, &joinFPos);
			if (pBranch != NULL)
				return pBranch;
			fPos = joinFPos;
		}
		else if (pIndexDescriptor->nPageType == 3)
		{
			*fPosNext = pIndexDescriptor->pageDescriptor.pJoin->nextFpos;
			return NULL;
		}
		else if (pIndexDescriptor->nPageType == 1 || pIndexDescriptor->nPageType == 4)
		{
			fPos = pIndexDescriptor->pageDescriptor.pVideo->nextFpos;
		}
		else if (pIndexDescriptor->nPageType == 5)
		{
			fPos = pIndexDescriptor->pageDescriptor.pChannel->nextFPos;
		}

	}
	return NULL;
}

ULONG FileContext::FindEventIndex(long long fPosVideoPlayed, int videoIndexNum)
{
	ULONG eventIndex = 0L;
	map <long long, struct indexPageDescriptor *>::iterator itVideoMap =  mapVideoPages.find(fPosVideoPlayed);
	if (itVideoMap != mapVideoPages.end())
	{
		struct indexPageDescriptor *pDescriptor = itVideoMap->second;
		eventIndex = pDescriptor->dataRecs[videoIndexNum].eventIndexNum;
	}
	return eventIndex;
}

void FileContext::SetDescriptorEvents()
{
	ULONG nEventIndex = 0L;
	ULONG oEventIndex = 0L;
	struct eventListElement *pEventList = pEventListRoot;
	ChannelDataMapping *pChannelDataMap = GetChannelDataMapping(mainChannelNumber);
	struct VideoHdr *pVideoHdr = pChannelDataMap->GetVideoHeader();
	long long fPos = pVideoHdr->fPosRoot;
	map <long long, struct indexPageDescriptor *>::iterator itFind = mapVideoPages.find(fPos);
	if (itFind == mapVideoPages.end())
		return;
	struct indexPageDescriptor *pIndexDescriptor = itFind->second;
	// create a list of branch indexes that can be popped on joins
	list <struct eventListElement *> branchEventElements;

	//BPMNElement *pNextElement = NULL;
	bool bStartFlg = true;
	if (pEventList->pElement != NULL)
	{
		pEventList = GetFirstEventListElement(pEventList->pElement);
		if (pEventList == NULL || pEventList->pElement == NULL)
			return;
		pEventList->pElement->SetInPath(true);
		nEventIndex = pEventList->pElement->GetEventIndex();
		oEventIndex = nEventIndex;
	}

	while (pIndexDescriptor != NULL)
	{
		for (int i = 0; i <= pIndexDescriptor->numRecs; i++)
		{
			if (bStartFlg && pIndexDescriptor->dataRecs[i].indexNum != nEventIndex)
			{
				pIndexDescriptor->dataRecs[i].eventIndexNum = 0;
			}
			else
			{
				if (bStartFlg)
				{
					pIndexDescriptor->dataRecs[i].eventIndexNum = nEventIndex;
					pEventList = GetNextEventListElement(pEventList);
					if (pEventList != NULL && pEventList->pElement != NULL)
					{
						pEventList->pElement->SetInPath(true);
						oEventIndex = nEventIndex;
						nEventIndex = pEventList->pElement->GetEventIndex();
					}

					bStartFlg = false;
					continue;
				}
				if (pIndexDescriptor->dataRecs[i].indexNum != nEventIndex)
				{
					pIndexDescriptor->dataRecs[i].eventIndexNum = oEventIndex;
				}
				else
				{
					pIndexDescriptor->dataRecs[i].eventIndexNum = nEventIndex;
					if (pIndexDescriptor->nPageType != 2 && pIndexDescriptor->nPageType != 3)
					{
						pEventList = GetNextEventListElement(pEventList);
						if (pEventList != NULL && pEventList->pElement != NULL)
						{
							pEventList->pElement->SetInPath(true);
							oEventIndex = nEventIndex;
							nEventIndex = pEventList->pElement->GetEventIndex();
						}
						else
						{
							oEventIndex = nEventIndex;
							nEventIndex = 0xffffffff;
						}
					}
				}
			}
		}
		if (pIndexDescriptor->nPageType == 2) // If Branch Descriptor
		{
			struct branchIndexDescriptor *pBranch = pIndexDescriptor->pageDescriptor.pBranch;
			//branchEventElements.push_back(pEventList);
			BPMNElement *pCurrElement = pEventList->pElement;
			if (pCurrElement->GetType() == 4)
			{
				BPMNDecision *pDecision = (BPMNDecision *)pCurrElement;
				BPMNElement *pNextEvent = NULL;
				if (pBranch->bPathDir)
				{
					pNextEvent = pDecision->GetFirstRightElement();
				}
				else
				{
					pNextEvent = pDecision->GetFirstBottomElement();
				}
				if (pNextEvent != NULL)
				{
					map <ULONG, struct eventListElement *>::iterator itEventMap = pEventMap->find(pNextEvent->GetEventIndex());
					if (itEventMap != pEventMap->end())
					{
						pEventList = itEventMap->second;
						pEventList->pElement->SetInPath(true);
						oEventIndex = nEventIndex;
						nEventIndex = pEventList->pElement->GetEventIndex();
					}
				}

			}
			if (pBranch->bPathDir) // branches to the right
			{
				fPos = pBranch->nextRightFPos;
			}
			else
			{
				fPos = pBranch->nextBottomFPos;
			}
			pIndexDescriptor = NULL;
			itFind = mapVideoPages.find(fPos);
			if (itFind != mapVideoPages.end())
			{
				pIndexDescriptor = itFind->second;
			}

		}
		//else if (pIndexDescriptor->nPageType == 3)
		//{
		//	pIndexDescriptor = pIndexDescriptor->next;
		//	if (branchEventElements.size() > 0)
		//	{
		//		pEventList = branchEventElements.back();
		//		branchEventElements.pop_back();
		//		pEventList = GetNextEventListElement(pEventList);
		//		if (pEventList != NULL && pEventList->pElement != NULL)
		//		{
		//			pEventList->pElement->SetInPath(true);
		//			oEventIndex = nEventIndex;
		//			nEventIndex = pEventList->pElement->GetEventIndex();
		//		}
		//	}
		//}
		else 
		{
			pIndexDescriptor = pIndexDescriptor->next;
		}
	}
}

struct eventListElement *FileContext::GetNextEventListElement(struct eventListElement *pEventListElement)
{
	// This function return the next BPMNTask element
	// if the element is a SubProcess then get the subprocess list
	// if no element found then find the EventList object and go to next, till no more event list objects
	if (pEventListElement == NULL)
		return NULL;
	struct eventListElement *pEventListNextElement = pEventListElement->nextElement;
	
	if (pEventListElement->pElement->GetType() == 4) // if it is a branch
	{
		return pEventListElement;
	}

	if (pEventListNextElement == NULL)
	{
		BPMNElement *pParentElement = pEventListElement->pElement->GetParentElement();
		if (pParentElement != NULL && (pParentElement->GetType() == 3 || pParentElement->GetType() == 4))
		{
			ULONG eventIndex = pParentElement->GetEventIndex();
			map <ULONG, struct eventListElement *>::iterator itEventMap = pEventMap->find(eventIndex);
			if (itEventMap != pEventMap->end())
			{
				struct eventListElement *pEventList = itEventMap->second;
				pEventListNextElement = pEventList->nextElement;
				if (pEventListNextElement == NULL)
				{
					return GetNextEventListElement(pEventList);
				}
			}
		}
		else
			return NULL;
	}
	return GetFirstEventListElement(pEventListNextElement->pElement);
}

struct eventListElement *FileContext::GetFirstEventListElement(BPMNElement *pElement)
{
	// This function return the first BPMNTask element
	// if the element is a SubProcess then get the subprocess list
	// if no element found then find the EventList object and go to next, till no more event list objects

	if (pElement == NULL)
		return NULL;

	struct eventListElement *pEventList = NULL;

	if (pElement->GetType() != 3)
	{
		map <ULONG, struct eventListElement *>::iterator itEventMap = pEventMap->find(pElement->GetEventIndex());
		if (itEventMap != pEventMap->end())
		{
			pEventList = itEventMap->second;
			return pEventList;
		}
	}
	else
	{
		BPMNSubProcess *pSubProcess = (BPMNSubProcess *)pElement;
		list <BPMNElement *> *pSubProcessList = pSubProcess->GetSubProcessElements();
		list <BPMNElement *>::iterator itList = pSubProcessList->begin();
		while (itList != pSubProcessList->end())
		{
			BPMNElement *pSubProcessElement = *itList;
			int nElementType = pSubProcessElement->GetType();
			if (nElementType != 1 && nElementType != 5) // skip start and stop elements
				pEventList = GetFirstEventListElement(pSubProcessElement);
			if (pEventList != NULL)
				return pEventList;
			itList++;
		}
	}

	map <ULONG, struct eventListElement *>::iterator itEventMap = pEventMap->find(pElement->GetEventIndex());
	if (itEventMap != pEventMap->end())
	{
		pEventList = itEventMap->second;
		pEventList = pEventList->nextElement;
		if (pEventList != NULL)
			pEventList = GetFirstEventListElement(pEventList->pElement);
	}

	return pEventList;

}

long long FileContext::GetVideoIndexTime(int nIndexNum, long long fPos)
{
	struct indexPageDescriptor *currIndexPage = NULL;
	map <long long, struct indexPageDescriptor *>::iterator itFind = mapVideoPages.find(fPos);
	if (itFind == mapVideoPages.end())
		return 0L;

	currIndexPage = itFind->second;
	struct videoIndexDescriptor *pVideo = currIndexPage->pageDescriptor.pVideo;

	long long timeVal = pVideo->startTime;
	for (int i = 0; i <= nIndexNum; i++)
	{
		timeVal += currIndexPage->dataRecs[i].recDuration;
	}

	return timeVal;
}

int FileContext::StepVideo(int stepNum, IndexHdr *videoIndex, int *numVideo, long long *pos)
{
	USHORT newChannel = currContextChannel;
	*numVideo = *numVideo + stepNum;
	int nextIndex = *numVideo;
	if (nextIndex >= 0 && nextIndex <= videoIndex->numRecs)
		return newChannel;
	if (nextIndex < 0)
	{
		*pos = videoIndex->fPosPrevRecord;
	}
	if (nextIndex > videoIndex->numRecs)
	{
		*pos = videoIndex->fPosNextRecord;
	}
	if (*pos == NULL)
	{
		*numVideo = 0;
		return newChannel;
	}

	pDataFile->ReadFromFile((char *)videoIndex, sizeof(struct IndexHdr), *pos);
	while (videoIndex->RecType != 'P')
	{
		//ULONG nIndex = 0L;
		struct indexPageDescriptor *currIndexPage = NULL;
		map <long long, struct indexPageDescriptor *>::iterator itFind = mapVideoPages.find(*pos);
		if (itFind == mapVideoPages.end())
			break;

		currIndexPage = itFind->second;
		if (videoIndex->RecType == 'B')
		{
			struct branchIndexDescriptor *pBranch = currIndexPage->pageDescriptor.pBranch;
			if (stepNum > 0)
			{
				if (pBranch->bPathDir)
				{
					*pos = pBranch->nextRightFPos;
				}
				else
				{
					*pos = pBranch->nextBottomFPos;
				}
			}
			else
			{
				*pos = pBranch->prevFpos;
			}
		}
		else if (videoIndex->RecType == 'J')
		{
			struct joinIndexDescriptor *pJoin = currIndexPage->pageDescriptor.pJoin;
			if (stepNum > 0)
			{
				*pos = pJoin->nextFpos;
			}
			else
			{
				if (pJoin->bPathDir)
					*pos = pJoin->prevRightFpos;
				else
					*pos = pJoin->prevTopFpos;
			}
		}
		else if (videoIndex->RecType == 'H')
		{
			// here we need to switch channel context and go next or  previous
			struct channelIndexDescriptor *pChannel = currIndexPage->pageDescriptor.pChannel;
			
			if (stepNum > 0)
			{
				newChannel = pChannel->newChannel;
				*pos = pChannel->nextFPos;
			}
			else
			{
				newChannel = pChannel->prevChannel;
				*pos = pChannel->prevFpos;
			}
		}
		pDataFile->ReadFromFile((char *)videoIndex, sizeof(struct IndexHdr), *pos);
	}
	if (*pos == NULL)
	{
		*numVideo = 0;
		return 0;
	}
	// Read the index record from pos again

	if (stepNum > 0)
	{
		*numVideo = 0;
	}
	else
	{
		*numVideo = videoIndex->numRecs;
	}

	return newChannel;

}

int FileContext::GetVideoAtTime(long timePos, IndexHdr *videoIndex, int *numVideo, long long *pos)
{
	return GetVideoAtTimeWithFlg(timePos, videoIndex, numVideo, pos, true);
}


int FileContext::GetThumbVideoAtTime(long timePos, IndexHdr *videoIndex, int *numVideo, long long *pos)
{
	return GetVideoAtTimeWithFlg(timePos, videoIndex, numVideo, pos, false);
}


int FileContext::GetVideoAtTimeWithFlg(long timePos, IndexHdr *videoIndex, int *numVideo, long long *pos, bool bUpdateFlg)
{
	ChannelDataMapping *pChannelData = GetChannelDataMapping(mainChannelNumber);
	//long long startTime = 0L;
	struct VideoHdr videoHdr;
	long long fPos = 0L;
	pChannelData->GetVideoHeader(&videoHdr);
	fPos = videoHdr.fPosRoot;
	int numIndex = 0;
	int newChannelNum = mainChannelNumber;
	long long searchTime = (long long)timePos * 1000L;

	struct indexPageDescriptor *currIndexPage = NULL;

	map <long long, struct indexPageDescriptor *>::iterator itFind = mapVideoPages.find(fPos);
	if (itFind != mapVideoPages.end())
		currIndexPage = itFind->second;

	while (currIndexPage != NULL)
	{
		if (currIndexPage->nPageType == 2) // Branch index
		{
			struct branchIndexDescriptor *pBranch = currIndexPage->pageDescriptor.pBranch;
			currIndexPage = NULL;
			if (pBranch->bPathDir) // branches to the right
				fPos = pBranch->nextRightFPos;
			else
				fPos = pBranch->nextBottomFPos;
			itFind = mapVideoPages.find(fPos);
			if (itFind != mapVideoPages.end())
				currIndexPage = itFind->second;
		}
		else if (currIndexPage->nPageType == 3) // Join Index
		{
			struct joinIndexDescriptor *pJoin = currIndexPage->pageDescriptor.pJoin;
			fPos = pJoin->nextFpos;
			itFind = mapVideoPages.find(fPos);
			if (itFind != mapVideoPages.end())
				currIndexPage = itFind->second;
		}
		else if (currIndexPage->nPageType == 1) // Video Page
		{
			struct videoIndexDescriptor *pVideo = currIndexPage->pageDescriptor.pVideo;
			long long videoTime = pVideo->startTime + pVideo->timespan;
			if (videoTime >= searchTime)
			{
				break;
			}
			if (currIndexPage->next != NULL)
				currIndexPage = currIndexPage->next;
			else
				break;
		}
		else if (currIndexPage->nPageType == 5) // Channel Page
		{
			struct channelIndexDescriptor *pChannel = currIndexPage->pageDescriptor.pChannel;
			newChannelNum = pChannel->newChannel;
			if (currIndexPage->next != NULL)
				currIndexPage = currIndexPage->next;
			else
				break;
		}
	}

	if (currIndexPage != NULL && currIndexPage->nPageType == 1)
	{
		struct videoIndexDescriptor *pVideo = currIndexPage->pageDescriptor.pVideo;
		long long videoTime = pVideo->startTime + pVideo->timespan;
		if (videoTime == searchTime)
		{
			numIndex = videoHdr.noMonitors;
		}
		else
		{
			pVideo = currIndexPage->pageDescriptor.pVideo;
			videoTime = pVideo->startTime;
			numIndex = 0;
			while (numIndex <= currIndexPage->numRecs)
			{
				videoTime = videoTime + currIndexPage->dataRecs[numIndex].recDuration;
				if (videoTime >= searchTime)
					break;
				numIndex++;
			}
			if (numIndex > currIndexPage->numRecs)
				numIndex = currIndexPage->numRecs;
			// Current Index Page is the index Page descriptor, numIndex is the index Number
			pVideo = currIndexPage->pageDescriptor.pVideo;
			pDataFile->ReadFromFile((char *)videoIndex, sizeof(struct IndexHdr), pVideo->currFPos);
			*numVideo = numIndex;
			*pos = pVideo->currFPos;
			if (bUpdateFlg)
				currVideoFPos = pVideo->currFPos;
			newChannelNum = currIndexPage->channelNum;
		}
	}
	else
	{
		// Should never come over here
		if (bUpdateFlg)
			currVideoFPos = videoHdr.fPosRoot;
		*pos = videoHdr.fPosRoot;
		numIndex = videoHdr.noMonitors;
		*numVideo = numIndex;
		pDataFile->ReadFromFile((char *)&videoIndex, sizeof(struct IndexHdr), videoHdr.fPosRoot);
	}

	return newChannelNum;
}

void FileContext::GetEventImage(int nChannel, IndexHdr *videoIndex, int numVideo, long long pos, vector<unsigned char>& buffer, ULONG& cx, ULONG& cy, ULONG nEventIndex, vector <BPMNElement *> &pElementList)
{
	if (currContextChannel != nChannel)
	{
		// do a channel context switch out here
		currContextChannel = nChannel;
	}

	// for the index page, get all process all records till num index
	// then display the screen
	ChannelDataMapping *pChannelData = GetChannelDataMapping(currContextChannel);
	if (pChannelData == NULL)
		return;

	int numDisplay = pChannelData->GetNumMonitors();
	if (numDisplay < numVideo)
		numDisplay = numVideo;

	int monNum = 0;
	struct bufferRecord *pBuffer = NULL;

	for (int i = 0; i <= numDisplay; i++)
	{
		if (GetScreenVideoBuffer(videoIndex, &i))
			break;
		pBuffer = pChannelData->ProcessMonitorScreens(&monNum);
	}

	if (pBuffer != NULL)
	{
		monNum = pChannelData->GetScreenSubscript(pBuffer->mouseX, pBuffer->mouseY);
		pChannelData->MergePrintImage(monNum, buffer, cx, cy, nEventIndex, pElementList);
	}

	
	//HMONITOR screenNum = GetScreenOfEvent(videoIndex, numVideo);
	//for (int i = 0; i <= numVideo; i++)
	//{
	//	if (GetScreenVideoBuffer(videoIndex, &i, screenNum))
	//		break;
	//	pChannelData->ProcessMonitorScreens(&monNum);
	//}
	//pChannelData->GetEventImage(monNum, buffer, cx, cy , nEventIndex, pElementList);

}

void FileContext::CreateVideoImage(int nChannel, struct IndexHdr *videoIndex, int numVideo, long long pos)
{
	if (currContextChannel != nChannel)
	{
		// do a channel context switch out here
		currContextChannel = nChannel;
	}

	// for the index page, get all process all records till num index
	// then display the screen
	ChannelDataMapping *pChannelData = GetChannelDataMapping(currContextChannel);
	if (pChannelData == NULL)
		return;

	int numDisplay = pChannelData->GetNumMonitors();
	if (numDisplay < numVideo)
		numDisplay = numVideo;

	struct bufferRecord *pBuffer = NULL;
	int monNum = 0;
	
	for (int i = 0; i <= numDisplay;i++)
	{
		if (GetScreenVideoBuffer(videoIndex, &i))
			break;
		pBuffer = pChannelData->ProcessMonitorScreens(&monNum);
	}
	pBuffer->indexNum = numVideo;
	pBuffer->fPosIndex = pos;
	pChannelData->ResizeVideoImage(pBuffer, monNum);
}

void FileContext::CreateThumbImage(int nChannel, struct IndexHdr *videoIndex, int numVideo, long long pos)
{
	if (currContextChannel != nChannel)
	{
		// do a channel context switch out here
		currContextChannel = nChannel;
	}

	// for the index page, get all process all records till num index
	// then display the screen
	ChannelDataMapping *pChannelData = GetChannelDataMapping(currContextChannel);
	if (pChannelData == NULL)
		return;

	struct bufferRecord *pBuffer = NULL;
	int monNum = 0;
	int numDisplay = pChannelData->GetNumMonitors();
	if (numDisplay < numVideo)
		numDisplay = numVideo;
	
	pChannelData->SetupThumbBuffers();
	for (int i = 0; i <= numDisplay; i++)
	{
		if (GetScreenThumbVideoBuffer(videoIndex, &i, &pBuffer))
			break;
		pBuffer = pChannelData->ProcessMonitorScreensForThumbs(&monNum, pBuffer);
	}
	pBuffer->indexNum = numVideo;
	pBuffer->fPosIndex = pos;
	pChannelData->ResizeThumbVideoImage(pBuffer, monNum);
	pChannelData->ClearThumbBuffers();
}


void FileContext::GetChannelVideoIndex(int chNum, long long pos, struct IndexHdr *indexHdr)
{
	ChannelDataMapping *pChannelData = GetChannelDataMapping(currContextChannel);
	if (pChannelData != NULL)
		pChannelData->GetVideoIndex(pos, indexHdr);
}

void FileContext::CreateBPMNList()
{
	// Create BPMN List only from the main Channel Number
	ChannelDataMapping *pChannelData = GetChannelDataMapping(mainChannelNumber);

	if (pChannelData == NULL)
		return;
	
	char *pRecBuffer = (char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 10480);

		// here create list from the task list channel
	long long posTaskList = 0L;
	posTaskList = pChannelData->GetChannelHeader()->posTaskListRoot;
	ReadEventRecords(posTaskList, pRecBuffer);
	HeapFree(GetProcessHeap(), 0, pRecBuffer);
}

struct eventListElement *FileContext::ReadEventRecords(long long pos, char *pRecBuffer)
{
	struct eventFileRecord eventFileRec;
	ZeroMemory(&eventFileRec, sizeof(struct eventFileRecord));
	struct eventDetails eventDets;
	struct eventListElement *pEventAdded = NULL;
	struct eventListElement *pPrevEventAdded = NULL;
	struct eventListElement *pRightEventAdded = NULL;
	struct eventListElement *pBottomEventAdded = NULL;


	while (pos != NULL)
	{
		pDataFile->ReadFromFile((char *)&eventFileRec, sizeof(struct eventFileRecord), pos);
		pDataFile->ReadDeflateBlockFromFile((char *)pRecBuffer, eventFileRec.size, eventFileRec.compSize, eventFileRec.dataPos);
		UnpackEventData(&eventDets, &eventFileRec, (char *)pRecBuffer);
		pPrevEventAdded = pEventAdded;
		pEventAdded = pBPMNFacade->AddEventInDiagram(&eventFileRec, &eventDets, pos);
	
		if (pEventAdded != NULL)
		{
			if (pEventListRoot == NULL)
				pEventListRoot = pEventAdded;
			if (pRightEventAdded != NULL)
			{
				pPrevEventAdded->nextElement = pRightEventAdded->nextElement = pEventAdded;
				pRightEventAdded = NULL;
			}
			if (pBottomEventAdded != NULL)
			{
				pPrevEventAdded->nextElement = pBottomEventAdded->nextElement = pEventAdded;
				pBottomEventAdded = NULL;
			}

			if (eventFileRec.fPosBottom != NULL)
			{
				pBottomEventAdded = ReadEventRecords(eventFileRec.fPosBottom, pRecBuffer);
			}
			if (eventFileRec.fPosRight != NULL)
			{
				pRightEventAdded = ReadEventRecords(eventFileRec.fPosRight, pRecBuffer);
			}
		}
		else
		{
			if (pRightEventAdded != NULL || pBottomEventAdded != NULL)
			{
				pEventAdded = pBPMNFacade->GetEventInDiagram(eventDets.eventIndex);
			}

			if (pEventAdded != NULL)
			{
				if (pRightEventAdded != NULL)
				{
					pPrevEventAdded->nextElement = pRightEventAdded->nextElement = pEventAdded;
					pRightEventAdded = NULL;
				}
				if (pBottomEventAdded != NULL)
				{
					pPrevEventAdded->nextElement = pBottomEventAdded->nextElement = pEventAdded;
					pBottomEventAdded = NULL;
				}
			}
		}
		pos = eventFileRec.fPosNextEvent;
	}
	return pEventAdded;

}

list <BPMNElement *> *FileContext::GetBPMNDiagram()
{
	return pBPMNFacade->GetBPMNDiagram();
}

map <ULONG, struct eventListElement *> *FileContext::GetElementMap()
{
	return pBPMNFacade->GetElementMap();
}

bool FileContext::GetVideoBuffer(struct IndexHdr *pIndex, int *indexNum)
{
	ChannelDataMapping *pChannelData = GetChannelDataMapping(currContextChannel);
	bool bRetValue = false;
	struct ScreenData *screen = pChannelData->GetScreenData(pIndex->dataRecs[*indexNum].screenNum);
	if (screen == NULL)
		return true; // change to return true;
	struct bufferRecord *pRecBuffer = (struct bufferRecord *)pRecBufferManager->GetPlayerFreeScreenBuffer(screen);
	pDataFile->ReadDeflateBlockFromFile((char *)pRecBuffer, pIndex->dataRecs[*indexNum].size, pIndex->dataRecs[*indexNum].compSize, pIndex->dataRecs[*indexNum].fPosData);

	USHORT *pFileContextNumPosition = (USHORT *)((char *)pRecBuffer + sizeof(struct bufferRecord) + 2);
	USHORT *pFileContextChannelNumPosition = (USHORT *)((char *)pRecBuffer + sizeof(struct bufferRecord) + 2 + sizeof(USHORT));
	*pFileContextNumPosition = nFileContextNum;
	*pFileContextChannelNumPosition = currContextChannel;

	pRecBuffer->buffer = (char *)pRecBuffer + sizeof(struct bufferRecord) + 8;
	pRecBuffer->eventIndex = pIndex->dataRecs[*indexNum].videoIndexNumber;
	pRecBuffer->fPosIndex = currVideoFPos;
	pRecBuffer->recDurationus = pIndex->dataRecs[*indexNum].recDurationus;
	pRecBuffer->timeStamp = pIndex->dataRecs[*indexNum].timestamp;
	pRecBuffer->indexNum = *indexNum;
	//struct CompareBuffer *pCompBuffer = (struct CompareBuffer *)pRecBuffer->buffer;
	pRecBufferManager->SaveInReadVideoBuffer(pRecBuffer);
	if (pIndex->dataRecs[*indexNum].fPosAudioData != NULL)
	{
		struct bufferRecord *pVideoBuffer = pRecBuffer;
		pRecBuffer = (struct bufferRecord *)pRecBufferManager->GetFreeAudioBuffer(pIndex->dataRecs[*indexNum].sizeAudio+100);
		pRecBuffer->buffer = (char *)pRecBuffer + sizeof(struct bufferRecord) + 8;
		pDataFile->ReadDeflateBlockFromFile((char *)pRecBuffer->buffer, pIndex->dataRecs[*indexNum].sizeAudio,
			pIndex->dataRecs[*indexNum].compAudioSize, pIndex->dataRecs[*indexNum].fPosAudioData);
		pRecBuffer->buffType = 2;
		pRecBuffer->unBuffDets.audRec.size = pIndex->dataRecs[*indexNum].sizeAudio;
		pRecBufferManager->SaveInReadAudioBuffer(pRecBuffer);
		pRecBuffer = pVideoBuffer;
	}


	*indexNum = *indexNum + 1;

	if (*indexNum > pIndex->numRecs)
	{
		currVideoFPos = pIndex->fPosNextRecord;
		*indexNum = 0;
		while (currVideoFPos != NULL)
		{
			pDataFile->ReadFromFile((char *)pIndex, sizeof(struct IndexHdr), currVideoFPos);

			if (pIndex->RecType == 'P')
				break;
			if (pIndex->RecType == 'H')
			{
				currContextChannel = pIndex->dataRecs[0].size;
				currVideoFPos = pIndex->fPosNextRecord;		// Perform a Channel Switch operation
			}
			else if (pIndex->RecType == 'B')
			{
				// Search for IndexDescriptor record for this branch, Check the branch direction
				// Then get the next branch record
				map <long long, struct indexPageDescriptor *>::iterator itMap = mapVideoPages.find(currVideoFPos);
				if (itMap == mapVideoPages.end())
				{
					bRetValue = false;
					break;
				}
				struct indexPageDescriptor *pDescriptor = itMap->second;
				if (pDescriptor->nPageType != 2)
				{
					bRetValue = false;
					break;
				}
				struct branchIndexDescriptor *pBranch = pDescriptor->pageDescriptor.pBranch;
				if (pBranch->bPathDir == true)
				{
					currVideoFPos = pIndex->dataRecs[0].fPosData;
				}
				else
				{
					currVideoFPos = pIndex->fPosNextRecord;
				}

			}
			else if (pIndex->RecType == 'J')
			{
				currVideoFPos = pIndex->fPosNextRecord;
			}
		}
	}

	if (currVideoFPos == NULL)
		bRetValue = true;
	//if (*indexNum > pIndex->numRecs)
	//{
	//	currVideoFPos = pIndex->fPosNextRecord;
	//	*indexNum = 0;
	//	//while (true)
	//	//{
	//		bRetValue = pChannelData->GetNextVideo(pIndex, &currVideoFPos, &mapVideoPages);
	//		//if (pIndex->RecType == 'P')
	//		//	break;
	//		//if (pIndex->RecType == 'H')
	//		//{
	//		//	currVideoFPos = pIndex->fPosNextRecord;		// Perform a Channel Switch operation
	//		//}
	//		//else if (pIndex->RecType == 'B')
	//		//{
	//		//	// Search for IndexDescriptor record for this branch, Check the branch direction
	//		//	// Then get the next branch record
	//		//	map <long long, struct indexPageDescriptor *>::iterator itMap = mapVideoPages.find(currVideoFPos);
	//		//	if (itMap == mapVideoPages.end())
	//		//	{
	//		//		bRetValue = false;
	//		//		break;
	//		//	}
	//		//	struct indexPageDescriptor *pDescriptor = itMap->second;
	//		//	if (pDescriptor->nPageType != 2)
	//		//	{
	//		//		bRetValue = false;
	//		//		break;
	//		//	}
	//		//	struct branchIndexDescriptor *pBranch = pDescriptor->pageDescriptor.pBranch;
	//		//	if (pBranch->bPathDir == true)
	//		//	{
	//		//		currVideoFPos = pIndex->dataRecs[0].fPosData;
	//		//	}
	//		//	else
	//		//	{
	//		//		currVideoFPos = pIndex->fPosNextRecord;
	//		//	}

	//		//}
	//		//else if (pIndex->RecType == 'J')
	//		//{
	//		//	currVideoFPos = pIndex->fPosNextRecord;
	//		//}
	//	//}
	//}
	return bRetValue;
}

bool FileContext::GetScreenVideoBuffer(struct IndexHdr *pIndex, int *indexNum)
{
	// if the user has rewind / fwd / move absolute on the video or edited the video then take index and index Num data 
	// from here also channel may have changed

	// Otherwise just get the next record in the index and store in buffer
	ChannelDataMapping *pChannelData = GetChannelDataMapping(currContextChannel);
	bool bRetValue = false;
	struct ScreenData *screen = pChannelData->GetScreenData(pIndex->dataRecs[*indexNum].screenNum);
	if (screen == NULL)
		return true; // change to return true;

	struct bufferRecord *pRecBuffer = (struct bufferRecord *)pRecBufferManager->GetPlayerFreeScreenBuffer(screen);
	pDataFile->ReadDeflateBlockFromFile((char *)pRecBuffer, pIndex->dataRecs[*indexNum].size, pIndex->dataRecs[*indexNum].compSize, pIndex->dataRecs[*indexNum].fPosData);

	USHORT *pFileContextNumPosition = (USHORT *)((char *)pRecBuffer + sizeof(struct bufferRecord) + 2);
	USHORT *pFileContextChannelNumPosition = (USHORT *)((char *)pRecBuffer + sizeof(struct bufferRecord) + 2 + sizeof(USHORT));
	*pFileContextNumPosition = nFileContextNum;
	*pFileContextChannelNumPosition = currContextChannel;

	pRecBuffer->buffer = (char *)pRecBuffer + sizeof(struct bufferRecord) + 8;
	pRecBuffer->eventIndex = pIndex->dataRecs[*indexNum].videoIndexNumber;
	pRecBuffer->fPosIndex = currVideoFPos;
	pRecBuffer->indexNum = *indexNum;
	//struct CompareBuffer *pCompBuffer = (struct CompareBuffer *)pRecBuffer->buffer;

	pRecBufferManager->SaveInReadVideoBuffer(pRecBuffer);
	return bRetValue;
}

bool FileContext::GetScreenVideoBuffer(struct IndexHdr *pIndex, int *indexNum, HMONITOR hMon)
{
	// if the user has rewind / fwd / move absolute on the video or edited the video then take index and index Num data 
	// from here also channel may have changed

	// Otherwise just get the next record in the index and store in buffer
	ChannelDataMapping *pChannelData = GetChannelDataMapping(currContextChannel);
	bool bRetValue = false;
	while (pIndex->dataRecs[*indexNum].screenNum != hMon && *indexNum <= pIndex->numRecs)
	{
		*indexNum += 1;
	}
	if (*indexNum > pIndex->numRecs)
		return true;

	struct ScreenData *screen = pChannelData->GetScreenData(pIndex->dataRecs[*indexNum].screenNum);
	if (screen == NULL)
		return true; // change to return true;

	struct bufferRecord *pRecBuffer = (struct bufferRecord *)pRecBufferManager->GetPlayerFreeScreenBuffer(screen);
	pDataFile->ReadDeflateBlockFromFile((char *)pRecBuffer, pIndex->dataRecs[*indexNum].size, pIndex->dataRecs[*indexNum].compSize, pIndex->dataRecs[*indexNum].fPosData);

	USHORT *pFileContextNumPosition = (USHORT *)((char *)pRecBuffer + sizeof(struct bufferRecord) + 2);
	USHORT *pFileContextChannelNumPosition = (USHORT *)((char *)pRecBuffer + sizeof(struct bufferRecord) + 2 + sizeof(USHORT));
	*pFileContextNumPosition = nFileContextNum;
	*pFileContextChannelNumPosition = currContextChannel;

	pRecBuffer->buffer = (char *)pRecBuffer + sizeof(struct bufferRecord) + 8;
	pRecBuffer->eventIndex = pIndex->dataRecs[*indexNum].videoIndexNumber;
	pRecBuffer->fPosIndex = currVideoFPos;
	pRecBuffer->indexNum = *indexNum;
	//struct CompareBuffer *pCompBuffer = (struct CompareBuffer *)pRecBuffer->buffer;

	pRecBufferManager->SaveInReadVideoBuffer(pRecBuffer);
	return bRetValue;
}


HMONITOR FileContext::GetScreenOfEvent(IndexHdr *videoIndex, int nIndex)
{
	ChannelDataMapping *pChannelData = GetChannelDataMapping(currContextChannel);
	//bool bRetValue = false;
	struct ScreenData *screen = pChannelData->GetScreenData(videoIndex->dataRecs[nIndex].screenNum);
	if (screen == NULL)
		return (HMONITOR)NULL; // change to return true;

	struct bufferRecord *pRecBuffer = (struct bufferRecord *)pRecBufferManager->GetPlayerFreeScreenBuffer(screen);
	pDataFile->ReadDeflateBlockFromFile((char *)pRecBuffer, videoIndex->dataRecs[nIndex].size, videoIndex->dataRecs[nIndex].compSize, videoIndex->dataRecs[nIndex].fPosData);
	HMONITOR hMon = pChannelData->GetScreenNumber(pRecBuffer->mouseX, pRecBuffer->mouseY);
	pRecBufferManager->FreePlayerBuffer(pRecBuffer);
	return hMon;

}



bool FileContext::GetScreenThumbVideoBuffer(struct IndexHdr *pIndex, int *indexNum, struct bufferRecord **pBuffer)
{
	// if the user has rewind / fwd / move absolute on the video or edited the video then take index and index Num data 
	// from here also channel may have changed

	// Otherwise just get the next record in the index and store in buffer
	ChannelDataMapping *pChannelData = GetChannelDataMapping(currContextChannel);
	bool bRetValue = false;
	struct ScreenData *screen = pChannelData->GetScreenData(pIndex->dataRecs[*indexNum].screenNum);
	if (screen == NULL)
		return true; // change to return true;

	struct bufferRecord *pRecBuffer = (struct bufferRecord *)pRecBufferManager->GetPlayerFreeScreenBuffer(screen);
	pDataFile->ReadDeflateBlockFromFile((char *)pRecBuffer, pIndex->dataRecs[*indexNum].size, pIndex->dataRecs[*indexNum].compSize, pIndex->dataRecs[*indexNum].fPosData);

	USHORT *pFileContextNumPosition = (USHORT *)((char *)pRecBuffer + sizeof(struct bufferRecord) + 2);
	USHORT *pFileContextChannelNumPosition = (USHORT *)((char *)pRecBuffer + sizeof(struct bufferRecord) + 2 + sizeof(USHORT));
	*pFileContextNumPosition = nFileContextNum;
	*pFileContextChannelNumPosition = currContextChannel;

	pRecBuffer->buffer = (char *)pRecBuffer + sizeof(struct bufferRecord) + 8;
	pRecBuffer->eventIndex = pIndex->dataRecs[*indexNum].videoIndexNumber;
	pRecBuffer->fPosIndex = currVideoFPos;
	pRecBuffer->indexNum = *indexNum;
	//struct CompareBuffer *pCompBuffer = (struct CompareBuffer *)pRecBuffer->buffer;

	*pBuffer = pRecBuffer;
	return bRetValue;
}

/*****************************************************************************************************/
// Start of Video Editing functions
/*****************************************************************************************************/

void FileContext::StartSegment(struct positionMarker *posMarker)
{
	
	startPositionMarker.channelNum = posMarker->channelNum;
	startPositionMarker.fileNum = posMarker->fileNum;
	startPositionMarker.fPosIndex = posMarker->fPosIndex;
	startPositionMarker.indexNum = posMarker->indexNum;
	
}

void FileContext::EndSegment(struct positionMarker *posMarker)
{
	endPositionMarker.channelNum = posMarker->channelNum;
	endPositionMarker.fileNum = posMarker->fileNum;
	endPositionMarker.fPosIndex = posMarker->fPosIndex;
	endPositionMarker.indexNum = posMarker->indexNum;
}

void FileContext::DestinationSegment(struct positionMarker *posMarker)
{
	destPositionMarker.channelNum = posMarker->channelNum;
	destPositionMarker.fileNum = posMarker->fileNum;
	destPositionMarker.fPosIndex = posMarker->fPosIndex;
	destPositionMarker.indexNum = posMarker->indexNum;

}

long long FileContext::CutSegment(VideoSegmentClass *pCopySegment)
{
	long long startTime = 0L;

	pRecBufferManager->ClearPlayBuffer();
	if (startPositionMarker.fileNum != 1)// Cut can only be done on the main file;
		return startTime;
	if (PrepareForOperation(true, true))		// Originally false, true
	{
		long long fPosStartIndex = startPositionMarker.fPosIndex;
		long long fPosEndIndex = endPositionMarker.fPosIndex;
		startTime = GetMarkerTime(&startPositionMarker);
		CopySegmentDetails();
		copySegmentEvents.clear();
		if (segmentEvents.size() > 0)
		{
			copySegmentEvents.insert(copySegmentEvents.begin(), segmentEvents.begin(), segmentEvents.end());
			// Delete events from event list
			DeleteEventsInList(&segmentEvents);
		}
		DeleteVideoSegment();
		ClearSegment(pCopySegment);
		pCopySegment->SetFileContext(this);
		pCopySegment->SetChannelNum(currContextChannel);
		pCopySegment->SetTopIndexHeader(&copyTopIndexHdr);
		pCopySegment->SetBottomIndexHeader(&copyBottomIndexHdr);
		pCopySegment->SetEventList(&copySegmentEvents);
		pCopySegment->SetStartPosition(fPosStartIndex);
		pCopySegment->SetEndPosition(fPosEndIndex);
		copySegmentEvents.clear();
	}

	return startTime;

}

void FileContext::CopySegmentDetails()
{
	struct VideoHdr vHeader;
	struct IndexHdr indexPage;
	
	ChannelDataMapping *pChannelData = GetChannelDataMapping(startPositionMarker.channelNum);
	pChannelData->GetVideoHeader(&vHeader);
	int numMon = vHeader.noMonitors;

	ZeroMemory(&copyTopIndexHdr, sizeof(struct IndexHdr));
	ZeroMemory(&copyBottomIndexHdr, sizeof(struct IndexHdr));
	ZeroMemory(&indexPage, sizeof(struct IndexHdr));

	if (startPositionMarker.fPosIndex == endPositionMarker.fPosIndex)
	{
		// The top Index is only the end top index hdr value

		CopyMemory(&copyTopIndexHdr, &startBottomIndexHdr, sizeof(struct IndexHdr));
		copyTopIndexHdr.fPosNextRecord = copyTopIndexHdr.fPosPrevRecord = NULL;
		return;
	}

	if (startBottomIndexHdr.numRecs > numMon)
	{
		CopyMemory(&copyTopIndexHdr, &startBottomIndexHdr, sizeof(struct IndexHdr));
	}
	else
	{
		long long fPosNext = startTopIndexHdr.fPosNextRecord;
		pDataFile->ReadFromFile((char *)&indexPage, sizeof(struct IndexHdr), fPosNext);
		CopyMemory(&copyTopIndexHdr, &indexPage, sizeof(struct IndexHdr));
	}

	if (endTopIndexHdr.numRecs > numMon)
	{
		CopyMemory(&copyBottomIndexHdr, &endTopIndexHdr, sizeof(struct IndexHdr));
	}
	else
	{
		long long fPosPrev = endTopIndexHdr.fPosPrevRecord;
		pDataFile->ReadFromFile((char *)&indexPage, sizeof(struct IndexHdr), fPosPrev);
		CopyMemory(&copyBottomIndexHdr, &indexPage, sizeof(struct IndexHdr));
	}
}


void FileContext::ClearSegment(VideoSegmentClass *pCopySegment)
{
	ZeroMemory(&startPositionMarker, sizeof(struct positionMarker));
	ZeroMemory(&endPositionMarker, sizeof(struct positionMarker));
	ZeroMemory(&destPositionMarker, sizeof(struct positionMarker));
	ZeroMemory(&startTopIndexHdr, sizeof(struct IndexHdr));
	ZeroMemory(&startBottomIndexHdr, sizeof(struct IndexHdr));
	ZeroMemory(&endTopIndexHdr, sizeof(struct IndexHdr));
	ZeroMemory(&endBottomIndexHdr, sizeof(struct IndexHdr));
	segmentEvents.clear();
	pCopySegment->Clear();
}

void FileContext::CopySegment(VideoSegmentClass *pCopySegment)
{
	struct IndexHdr segTopIndexHdr;

	pRecBufferManager->ClearPlayBuffer();
	if (PrepareForOperation(true, true))  // Originally false, false
	{
/// Original Code
		long long fPosStartIndex = startPositionMarker.fPosIndex;
		long long fPosEndIndex = endPositionMarker.fPosIndex;
		GetMarkerTime(&startPositionMarker);
		CopySegmentDetails();
		copySegmentEvents.clear();
		if (segmentEvents.size() > 0)
		{
			copySegmentEvents.insert(copySegmentEvents.begin(), segmentEvents.begin(), segmentEvents.end());
		}
		ClearSegment(pCopySegment);
		pCopySegment->SetSegmentTopIndexHeader(&segTopIndexHdr);
		pCopySegment->SetTopIndexHeader(&copyTopIndexHdr);
		pCopySegment->SetBottomIndexHeader(&copyBottomIndexHdr);
		pCopySegment->SetEventList(&copySegmentEvents);
		pCopySegment->SetFileContext(this);
		pCopySegment->SetChannelNum(currContextChannel);
		pCopySegment->SetStartPosition(fPosStartIndex);
		pCopySegment->SetEndPosition(fPosEndIndex);
		copySegmentEvents.clear();
	}
}

long long FileContext::PasteSegment(VideoSegmentClass *pCopySegment)
{
	// Should be done after a Cut or a Copy
	pRecBufferManager->ClearPlayBuffer();
	pCopySegment->GetTopIndexHeader(&copyTopIndexHdr);
	pCopySegment->GetBottomIndexHeader(&copyBottomIndexHdr);
	pCopySegment->GetEventList(&copySegmentEvents);
	segmentEvents.clear();
	segmentEvents.insert(segmentEvents.begin(), copySegmentEvents.begin(), copySegmentEvents.end());

	if (copyTopIndexHdr.numRecs == 0 && copyBottomIndexHdr.numRecs == 0)
		return 0L;
	long long startTime = GetMarkerTime(&destPositionMarker);
	if (destPositionMarker.fileNum != 1)// Paste and Branch can only be done on the main file;
		return startTime;

// Here we can prepare the Video starting records and then initialize the index Header structures and event array
	//PrepareVideoIndex(pCopySegment);
	if (PrepareDestinationForOperation(pCopySegment))
	{
		InsertEventsInList(pCopySegment);
	}
	
	return startTime;
}

void FileContext::GetBranchPositions(long long *fPosBranch)
{
	;
}

void FileContext::BranchDelete(ULONG nEventIndex, bool bDelEntire)
{
	spdlog::info("Branch Delete");
	struct branchIndexDescriptor *pBranch1, *pBranch2, *pBranch3;
	pBranch1 = pBranch2 = pBranch3 = NULL;
	struct joinIndexDescriptor *pJoin1, *pJoin2, *pJoin3;
	long long posRight1, posRight2, posRight3;
	posRight1 = posRight2 = posRight3 = 0LL;


	struct indexPageDescriptor *pRightIndexDescriptor1, *pRightIndexDescriptor2, *pRightIndexDescriptor3;
	pRightIndexDescriptor1 = pRightIndexDescriptor2 = pRightIndexDescriptor3 = NULL;

	struct eventListElement *pEventList = NULL;
	map <ULONG, struct eventListElement *>::iterator itEventMap = pEventMap->find(nEventIndex);
	if (itEventMap == pEventMap->end())
		return;

	pEventList = itEventMap->second;
	if (pEventList == NULL || pEventList->pElement == NULL || pEventList->pElement->GetType() != 4)
		return;

	//BPMNElement *pElement = pEventList->pElement;
	struct indexPageDescriptor *pIndexPageDescriptor = FindIndexPageDescriptorForBranch(nEventIndex);

	// This is the Branch and Join to be deleted
	pBranch1 = pIndexPageDescriptor->pageDescriptor.pBranch;
	pJoin1 = GetJoinDescriptor(pBranch1);

	posRight1 = pBranch1->nextRightFPos;
	if (posRight1 != NULL)
	{
		map <long long, struct indexPageDescriptor *>::iterator itMap = mapVideoPages.find(posRight1);
		if (itMap == mapVideoPages.end())
		{
			return;
		}

		pRightIndexDescriptor1 = itMap->second;
		// Check if the right branch connects to another branch
		if (pRightIndexDescriptor1->nPageType == 2)
		{
			// Branch on Right
			pBranch2 = pRightIndexDescriptor1->pageDescriptor.pBranch;
			pJoin2 = GetJoinDescriptor(pBranch2);
			posRight2 = pBranch2->nextRightFPos;
			if (posRight2 != NULL)
			{
				itMap = mapVideoPages.find(posRight2);
				if (itMap == mapVideoPages.end())
				{
					return;
				}
				pRightIndexDescriptor2 = itMap->second;
				if (pRightIndexDescriptor2->nPageType == 2)
				{
					// Branch on Right
					pBranch3 = pRightIndexDescriptor2->pageDescriptor.pBranch;
					pJoin3 = GetJoinDescriptor(pBranch3);
				}
			}
		}
	}

	struct IndexHdr prevTopIndexHdr, nextTopIndexHdr, prevBottomIndexHdr, nextBottomIndexHdr, nextRightIndexHdr, currJoinIndexHdr;
	ZeroMemory(&prevTopIndexHdr, sizeof(struct IndexHdr));
	ZeroMemory(&nextTopIndexHdr, sizeof(struct IndexHdr));
	ZeroMemory(&prevBottomIndexHdr, sizeof(struct IndexHdr));
	ZeroMemory(&nextBottomIndexHdr, sizeof(struct IndexHdr));
	ZeroMemory(&nextRightIndexHdr, sizeof(struct IndexHdr));
	ZeroMemory(&currJoinIndexHdr, sizeof(struct IndexHdr));

	pDataFile->ReadFromFile((char *)&prevTopIndexHdr, sizeof(struct IndexHdr), pBranch1->prevFpos);
	pDataFile->ReadFromFile((char *)&nextTopIndexHdr, sizeof(struct IndexHdr), pBranch1->nextBottomFPos);
	pDataFile->ReadFromFile((char *)&nextRightIndexHdr, sizeof(struct IndexHdr), pBranch1->nextRightFPos);
	pDataFile->ReadFromFile((char *)&prevBottomIndexHdr, sizeof(struct IndexHdr), pJoin1->prevTopFpos);
	pDataFile->ReadFromFile((char *)&nextBottomIndexHdr, sizeof(struct IndexHdr), pJoin1->nextFpos);
	pDataFile->ReadFromFile((char *)&currJoinIndexHdr, sizeof(struct IndexHdr), pJoin1->currFPos);
	
	if (prevTopIndexHdr.RecType != 'B' && nextRightIndexHdr.RecType != 'B')
	{
		prevTopIndexHdr.fPosNextRecord = pBranch1->nextBottomFPos;
		pDataFile->WriteToFile(pBranch1->prevFpos, (char *)&prevTopIndexHdr, sizeof(struct IndexHdr));
		UpdateIndexPageDescriptor(pBranch1->prevFpos, &prevTopIndexHdr);

		nextTopIndexHdr.fPosPrevRecord = pBranch1->prevFpos;
		pDataFile->WriteToFile(pBranch1->nextBottomFPos, (char *)&nextTopIndexHdr, sizeof(struct IndexHdr));
		UpdateIndexPageDescriptor(pBranch1->nextBottomFPos, &nextTopIndexHdr);

		prevBottomIndexHdr.fPosNextRecord = pJoin1->nextFpos;
		pDataFile->WriteToFile(pJoin1->prevTopFpos, (char *)&prevBottomIndexHdr, sizeof(struct IndexHdr));
		UpdateIndexPageDescriptor(pJoin1->prevTopFpos, &prevBottomIndexHdr);

		nextBottomIndexHdr.fPosPrevRecord = pJoin1->prevTopFpos;
		pDataFile->WriteToFile(pJoin1->nextFpos, (char *)&nextBottomIndexHdr, sizeof(struct IndexHdr));
		UpdateIndexPageDescriptor(pJoin1->nextFpos, &nextBottomIndexHdr);
	}
	else if (prevTopIndexHdr.RecType != 'B' && nextRightIndexHdr.RecType == 'B')
	{
		prevTopIndexHdr.fPosNextRecord = pBranch1->nextRightFPos;
		pDataFile->WriteToFile(pBranch1->prevFpos, (char *)&prevTopIndexHdr, sizeof(struct IndexHdr));
		UpdateIndexPageDescriptor(pBranch1->prevFpos, &prevTopIndexHdr);

		nextRightIndexHdr.fPosPrevRecord = pBranch1->prevFpos;
		nextRightIndexHdr.fPosNextRecord = pBranch1->nextBottomFPos;
		pDataFile->WriteToFile(pBranch1->nextRightFPos, (char *)&nextRightIndexHdr, sizeof(struct IndexHdr));
		UpdateIndexPageDescriptor(pBranch1->nextRightFPos, &nextRightIndexHdr);

		currJoinIndexHdr.dataRecs[0].fPosData = pJoin2->prevRightFpos;
		pDataFile->WriteToFile(pJoin1->currFPos, (char *)&currJoinIndexHdr, sizeof(struct IndexHdr));
		UpdateIndexPageDescriptor(pJoin1->currFPos, &currJoinIndexHdr);

		struct IndexHdr rightBottomIndexHdr;
		pDataFile->ReadFromFile((char *)&rightBottomIndexHdr, sizeof(struct IndexHdr), pJoin2->prevRightFpos);
		rightBottomIndexHdr.fPosNextRecord = pJoin1->currFPos;
		pDataFile->WriteToFile(pJoin2->prevRightFpos, (char *)&rightBottomIndexHdr, sizeof(struct IndexHdr));
		UpdateIndexPageDescriptor(pJoin2->prevRightFpos, &rightBottomIndexHdr);
	}
	else if (prevTopIndexHdr.RecType == 'B' && nextRightIndexHdr.RecType == 'B')
	{
		prevTopIndexHdr.dataRecs[0].fPosData = pBranch1->nextRightFPos;
		pDataFile->WriteToFile(pBranch1->prevFpos, (char *)&prevTopIndexHdr, sizeof(struct IndexHdr));
		UpdateIndexPageDescriptor(pBranch1->prevFpos, &prevTopIndexHdr);

		nextRightIndexHdr.fPosPrevRecord = pBranch1->prevFpos;
		nextRightIndexHdr.fPosNextRecord = pBranch1->nextBottomFPos;
		pDataFile->WriteToFile(pBranch1->nextRightFPos, (char *)&nextRightIndexHdr, sizeof(struct IndexHdr));
		UpdateIndexPageDescriptor(pBranch1->nextRightFPos, &nextRightIndexHdr);

		currJoinIndexHdr.dataRecs[0].fPosData = pJoin2->prevRightFpos;
		pDataFile->WriteToFile(pJoin1->currFPos, (char *)&currJoinIndexHdr, sizeof(struct IndexHdr));
		UpdateIndexPageDescriptor(pJoin1->currFPos, &currJoinIndexHdr);

		struct IndexHdr rightBottomIndexHdr;
		pDataFile->ReadFromFile((char *)&rightBottomIndexHdr, sizeof(struct IndexHdr), pJoin2->prevRightFpos);
		rightBottomIndexHdr.fPosNextRecord = pJoin1->currFPos;
		pDataFile->WriteToFile(pJoin2->prevRightFpos, (char *)&rightBottomIndexHdr, sizeof(struct IndexHdr));
		UpdateIndexPageDescriptor(pJoin2->prevRightFpos, &rightBottomIndexHdr);
	}
	else if (prevTopIndexHdr.RecType == 'B' && nextRightIndexHdr.RecType != 'B')
	{
		prevTopIndexHdr.dataRecs[0].fPosData = pBranch1->nextBottomFPos;
		pDataFile->WriteToFile(pBranch1->prevFpos, (char *)&prevTopIndexHdr, sizeof(struct IndexHdr));
		UpdateIndexPageDescriptor(pBranch1->prevFpos, &prevTopIndexHdr);

		nextTopIndexHdr.fPosPrevRecord = pBranch1->prevFpos;
		pDataFile->WriteToFile(pBranch1->nextBottomFPos, (char *)&nextTopIndexHdr, sizeof(struct IndexHdr));
		UpdateIndexPageDescriptor(pBranch1->nextBottomFPos, &nextTopIndexHdr);

		// Get Next Join header and sent next join headers->right record to pJoin1-prevRightFpos
		nextBottomIndexHdr.dataRecs[0].fPosData = pJoin1->prevRightFpos;
		pDataFile->WriteToFile(pJoin1->nextFpos, (char *)&nextBottomIndexHdr, sizeof(struct IndexHdr));
		UpdateIndexPageDescriptor(pJoin1->nextFpos, &nextBottomIndexHdr);

		struct IndexHdr rightBottomIndexHdr;
		pDataFile->ReadFromFile((char *)&rightBottomIndexHdr, sizeof(struct IndexHdr), pJoin1->prevTopFpos);
		rightBottomIndexHdr.fPosNextRecord = pJoin1->nextFpos;
		pDataFile->WriteToFile(pJoin1->prevTopFpos, (char *)&rightBottomIndexHdr, sizeof(struct IndexHdr));
		UpdateIndexPageDescriptor(pJoin1->prevTopFpos, &rightBottomIndexHdr);
	}


	// Now Handle the events
	map <ULONG, struct eventListElement *>::iterator itEvent = pEventMap->find(nEventIndex);
	if (itEvent == pEventMap->end())
	{
		return;
	}

	struct eventListElement *pBranchEvent = itEvent->second;
	struct eventListElement *pNextElementAfterBranch = pBranchEvent->nextElement;

	BPMNDecision *pBranchElement = (BPMNDecision *)pBranchEvent->pElement;
	BPMNElement *pBranchParent = pBranchElement->GetParentElement();
	BPMNElement *pBranchPrev = NULL;

	pBranchPrev = pBranchParent;
	if (pBranchPrev != NULL && pBranchPrev->GetType() == 4)
	{
		;
	}
	else
	{
		if (pBranchEvent->prevElement != NULL)
			pBranchPrev = pBranchEvent->prevElement->pElement;
		else
			pBranchPrev = NULL;
	}


	BPMNElement *pBranchNext = pBranchElement->GetBranchRightElements()->front();
	list <BPMNElement *> deleteList;
	list <BPMNElement *> deleteListBottom;
	list <BPMNElement *> copyList;
	deleteListBottom.clear();
	deleteList.clear();
	copyList.clear();

	if (pBranchNext->GetType() == 4) // Right is a decision box
	{
		BPMNDecision *pRightBranch = (BPMNDecision *)pBranchNext;
		list <BPMNElement *> *branchList = pRightBranch->GetBranchBottomElements();
		deleteList.insert(deleteList.begin(), branchList->begin(), branchList->end());
		branchList->clear();
		branchList = pBranchElement->GetBranchBottomElements();
		copyList.insert(copyList.begin(), branchList->begin(), branchList->end());

		if (pBranchPrev != NULL)
			pRightBranch->SetParentElement(pBranchPrev->GetParentElement());
		else
			pRightBranch->SetParentElement(NULL);
		pRightBranch->AddBottomBranchList(copyList); // changed indexes
		UpdateEventList(&copyList);

		if (pBranchPrev != NULL && pBranchPrev->GetType() == 4)
		{
			BPMNDecision *pLeftBranch = (BPMNDecision *)pBranchPrev;
			list <BPMNElement *> *rbranchList = pBranchElement->GetBranchRightElements();
			list <BPMNElement *> *prevBranchList = pLeftBranch->GetBranchRightElements();
			prevBranchList->clear();
			copyList.clear();
			copyList.insert(copyList.begin(), rbranchList->begin(), rbranchList->end());
			pLeftBranch->AddRightBranchList(copyList);
			UpdateEventList(&copyList);
		}
		else
		{
			ReplaceEventsInList(pBranchElement, pRightBranch);
			// Get branch previous element and link with pRightBranch
		}
	}
	else if (pBranchPrev != NULL && pBranchPrev->GetType() == 4) // Previous is a decision box
	{
		BPMNDecision *pLeftBranch = (BPMNDecision *)pBranchPrev;
		list <BPMNElement *> *branchList = pBranchElement->GetBranchRightElements();
		deleteList.insert(deleteList.begin(), branchList->begin(), branchList->end());
		branchList->clear();
		branchList = pBranchElement->GetBranchBottomElements();
		copyList.insert(copyList.begin(), branchList->begin(), branchList->end());
		pLeftBranch->GetBranchRightElements()->clear(); // clear current right elements
		pLeftBranch->AddRightBranchList(copyList); // changed indexes
		UpdateEventList(&copyList);
	}
	else
	{
		// Both previous and next are not branches
		list <BPMNElement *> *branchList = pBranchElement->GetBranchRightElements();
		deleteList.insert(deleteList.begin(), branchList->begin(), branchList->end());
		branchList->clear();
		branchList = pBranchElement->GetBranchBottomElements();
		copyList.insert(copyList.begin(), branchList->begin(), branchList->end());
		branchList->clear();

		list <BPMNElement *> *pBPMNEventList = pBPMNFacade->GetBPMNDiagram();
		list <BPMNElement *>::iterator itEventLocal = pBPMNEventList->begin();
		// Erase the branch element

		itEventLocal = FindEventIteratorInList(&pBPMNEventList, pBranchElement->GetEventIndex(), true);
		BPMNElement *pEventElement = *itEventLocal;
		if (!bDelEntire)
		{
			// Commented on 6/1 to fix issues with deleting a branch in a subprocess
			BPMNElement *pNewParentElement = NULL;
			//ULONG nBranchIndex = 0L;
			if (pBranchPrev != NULL)
			{
				pNewParentElement = pBranchPrev->GetParentElement();
			//	nBranchIndex = pBranchPrev->GetEventIndex();
			}
			else
			{
				pNewParentElement = pBranchElement->GetParentElement();
			//	if (pNewParentElement != NULL)
			//	{
			//		nBranchIndex = pNewParentElement->GetEventIndex();
			//	}
			}

			//if (nBranchIndex != 0L)
			//{
			//	itEvent = FindEventIteratorInList(&pBPMNEventList, pBranchPrev->GetEventIndex(), true);
			//}
			//else
			//{
			//	itEvent = pBPMNEventList->begin();
			//}

			// pEventElement contains point of insertion
			pBPMNEventList->insert(itEventLocal, copyList.begin(), copyList.end());
			list <BPMNElement *>::iterator itNewEvent = copyList.begin();
			while (itNewEvent != copyList.end())
			{
				BPMNElement *pNewEventElement = *itNewEvent;
				pNewEventElement->SetParentElement(pNewParentElement);
				InsertEventInEventMap(pEventElement, pNewEventElement, false);
				itNewEvent++;
				pEventElement = pNewEventElement;
			}

			pEventElement = *itEventLocal;
			itEventMap = pEventMap->find(pEventElement->GetEventIndex());
			if (itEventMap == pEventMap->end())
				return;
			struct eventListElement *pEventListElement = itEventMap->second;
			struct eventListElement *pPrevEventListElement = pEventListElement->prevElement;
			BPMNElement *pParentDest = pEventListElement->pElement->GetParentElement();

			if (pPrevEventListElement == NULL && pParentDest == NULL)
				;
			else if (pPrevEventListElement == NULL)
			{
				;
			}
			else if (pPrevEventListElement != NULL)
			{
				;
			}

			itNewEvent = copyList.begin();
			while (itNewEvent != copyList.end())
			{
				pEventElement = *itNewEvent;
				itEventMap = pEventMap->find(pEventElement->GetEventIndex());
				if (itEventMap == pEventMap->end())
					return;
				pEventListElement = itEventMap->second;
				pEventListElement->prevElement = pPrevEventListElement;
				UpdateEventRecord(pEventListElement, NULL);
				if (pPrevEventListElement != NULL)
				{
					pPrevEventListElement->nextElement = pEventListElement;
					UpdateEventRecord(pPrevEventListElement, NULL);
				}
				else if (pPrevEventListElement == NULL && pParentDest == NULL)
					SetEventRoot(pEventListElement);
				else if (pPrevEventListElement == NULL)
				{
					map <ULONG, struct eventListElement *>::iterator itParentMap = pEventMap->find(pParentDest->GetEventIndex());
					struct eventListElement *pParentListElement = itParentMap->second;
					UpdateEventRecord(pParentListElement, NULL);
				}
				pPrevEventListElement = pEventListElement;
				itNewEvent++;
			}

			pEventListElement->nextElement = pNextElementAfterBranch;
			UpdateEventRecord(pEventListElement, NULL);
			if (pNextElementAfterBranch != NULL)
			{
				pNextElementAfterBranch->prevElement = pEventListElement;
				UpdateEventRecord(pNextElementAfterBranch, NULL);
			}
		}
		else
		{
			branchList = pBranchElement->GetBranchBottomElements();
			deleteListBottom.insert(deleteListBottom.begin(), branchList->begin(), branchList->end());
			branchList->clear();
			itEventMap = pEventMap->find(pBranchElement->GetEventIndex());
			if (itEventMap == pEventMap->end())
				return;
			struct eventListElement *pEventListElement = itEventMap->second;
			struct eventListElement *pPrevEventListElement = pEventListElement->prevElement;
			BPMNElement *pParentDest = pEventListElement->pElement->GetParentElement();

			if (pPrevEventListElement == NULL && pParentDest == NULL)
				SetEventRoot(pNextElementAfterBranch);
			else if (pPrevEventListElement == NULL)
			{
				if (pParentDest->GetType() == 3) //if branch is the child of sub-process
				{
					itEventMap = pEventMap->find(pParentDest->GetEventIndex());
					if (itEventMap != pEventMap->end())
					{
						pPrevEventListElement = itEventMap->second;
						pPrevEventListElement->nextElement = pNextElementAfterBranch;
						UpdateEventRecord(pPrevEventListElement, NULL);
					}
				}
			}
			else if (pPrevEventListElement != NULL)
			{
				pPrevEventListElement->nextElement = pNextElementAfterBranch;
				UpdateEventRecord(pPrevEventListElement, NULL);
			}
			pNextElementAfterBranch->prevElement = pPrevEventListElement;
			UpdateEventRecord(pNextElementAfterBranch, NULL);
		}

		pBPMNEventList = pBPMNFacade->GetBPMNDiagram();
		// Erase the branch element
		itEventLocal = FindEventIteratorInList(&pBPMNEventList, pBranchElement->GetEventIndex(), true);
		if (itEventLocal != pBPMNEventList->end())
		{
			pBPMNEventList->erase(itEventLocal);
		}
	}

	if (pBranchNext->GetType() != 4 && (pBranchPrev == NULL || pBranchPrev->GetType() != 4)) // Right and left is not a decision box
	{
		;
	}
	else
	{
		// First find the new insertion point
		struct eventListElement *pBranchRightEvent = NULL;
		itEvent = pEventMap->find(pBranchNext->GetEventIndex());
		if (itEvent == pEventMap->end())
			return;

		pBranchRightEvent = itEvent->second;

		struct eventListElement *pBranchPrevEvent = NULL;
		itEvent = pEventMap->find(pBranchPrev->GetEventIndex());
		if (itEvent == pEventMap->end())
			return;
		pBranchPrevEvent = itEvent->second;
		if (pBranchPrev->GetType() == 4)
		{
			pBranchPrevEvent->nextElement = pNextElementAfterBranch;
		}
		else
		{
			pBranchPrevEvent->nextElement = pBranchRightEvent;
		}
		UpdateEventRecord(pBranchPrevEvent, NULL);

		pBranchRightEvent->prevElement = pBranchPrevEvent;
		pBranchRightEvent->nextElement = pNextElementAfterBranch;
		UpdateEventRecord(pBranchRightEvent, NULL);
	}
	pBranchElement->GetBranchRightElements()->clear();
	pBranchElement->GetBranchBottomElements()->clear();

	//deleteList.push_back(pBranchElement);
	if (deleteListBottom.size() > 0)
	{
		RemoveEventsInList(&deleteListBottom);  // Remove from Diagram
		DeleteEventsFromEventList(&deleteListBottom);
	}

	if (deleteList.size() > 0)
	{
		RemoveEventsInList(&deleteList);  // Remove from Diagram
		deleteList.push_back(pBranchElement);
		DeleteEventsFromEventList(&deleteList);
	}

}


long long FileContext::BranchCopy(ULONG nEventIndex, VideoSegmentClass *pCopySegment)
{
	spdlog::info("Branch Copy");
	// Initialize the Copy segment details
	pCopySegment->GetTopIndexHeader(&copyTopIndexHdr);
	pCopySegment->GetBottomIndexHeader(&copyBottomIndexHdr);
	pCopySegment->GetEventList(&copySegmentEvents);
	segmentEvents.clear();
	segmentEvents.insert(segmentEvents.begin(), copySegmentEvents.begin(), copySegmentEvents.end());
	insertSegmentEvents.clear();
	parentIndexMapping.clear();

	// Initialize Channel details
	struct IndexHdr channelIndexHdr;
	FileContext *pSrcContext = pCopySegment->GetFileContext();
	ChannelDataMapping *pSrcChannelData = pSrcContext->GetChannelDataMapping(pCopySegment->GetChannelNum());

	struct branchIndexDescriptor *pBranch1, *pBranch2, *pBranch3;
	pBranch1 = pBranch2 = pBranch3 = NULL;
	struct joinIndexDescriptor *pJoin1, *pJoin2, *pJoin3;
	pJoin1 = pJoin2 = pJoin3 = NULL;

	long long posRight1, posRight2, posRight3;
	posRight1 = posRight2 = posRight3 = 0LL;

	struct indexPageDescriptor *pRightIndexDescriptor1, *pRightIndexDescriptor2, *pRightIndexDescriptor3;
	pRightIndexDescriptor1 = pRightIndexDescriptor2 = pRightIndexDescriptor3 = NULL;

	// Should be done after a Cut or a Copy
	if (copyTopIndexHdr.numRecs == 0 && copyBottomIndexHdr.numRecs == 0)
		return 0L;

	// Insert the segment records out here

	struct eventListElement *pEventList = NULL;
	map <ULONG, struct eventListElement *>::iterator itEventMap = pEventMap->find(nEventIndex);
	if (itEventMap == pEventMap->end())
		return 0L;

	pEventList = itEventMap->second;
	if (pEventList == NULL || pEventList->pElement == NULL || pEventList->pElement->GetType() != 4)
		return -1L;

	//BPMNElement *pElement = pEventList->pElement;
	struct indexPageDescriptor *pIndexPageDescriptor = FindIndexPageDescriptorForBranch(nEventIndex);

	pBranch1 = pIndexPageDescriptor->pageDescriptor.pBranch;

	pJoin1 = GetJoinDescriptor(pBranch1);

	posRight1 = pBranch1->nextRightFPos;
	if (posRight1 != NULL)
	{
		map <long long, struct indexPageDescriptor *>::iterator itMap = mapVideoPages.find(posRight1);
		if (itMap == mapVideoPages.end())
		{
			return -1L;
		}

		pRightIndexDescriptor1 = itMap->second;
		// Check if the right branch connects to another branch
		if (pRightIndexDescriptor1->nPageType == 2)
		{
			// Branch on Right
			pBranch2 = pRightIndexDescriptor1->pageDescriptor.pBranch;
			pJoin2 = GetJoinDescriptor(pBranch2);
			posRight2 = pBranch2->nextRightFPos;
			if (posRight2 != NULL)
			{
				itMap = mapVideoPages.find(posRight2);
				if (itMap == mapVideoPages.end())
				{
					return -1L;
				}
				pRightIndexDescriptor2 = itMap->second;
				if (pRightIndexDescriptor2->nPageType == 2)
				{
					// Branch on Right
					pBranch3 = pRightIndexDescriptor2->pageDescriptor.pBranch;
					pJoin3 = GetJoinDescriptor(pBranch3);
				}
			}
		}
	}

	struct VideoHdr vHeader;
	ZeroMemory(&vHeader, sizeof(struct VideoHdr));
	short channelNum = pBranch1->branchChannelNum;
	ChannelDataMapping *pChannelData = GetChannelDataMapping(channelNum);
	pChannelData->GetVideoHeader(&vHeader);
	int numMon = vHeader.noMonitors;
	FileContext *fContext = pCopySegment->GetFileContext();

	// Add the New Branch
	struct IndexHdr newBranchIndex;
	ZeroMemory(&newBranchIndex, sizeof(struct IndexHdr));
	struct IndexHdr oldBranchIndex;
	struct IndexHdr newIndexHdr;
	struct IndexHdr oldIndexHdr;
	ZeroMemory(&newIndexHdr, sizeof(struct IndexHdr));
	ZeroMemory(&oldIndexHdr, sizeof(struct IndexHdr));
	ZeroMemory(&oldBranchIndex, sizeof(struct IndexHdr));

	pDataFile->ReadFromFile((char *)&oldBranchIndex, sizeof(struct IndexHdr), pBranch1->currFPos);

	newBranchIndex.RecType = 'B';
	long long posBranch1Right = pBranch1->nextRightFPos;
	long long posNewBranch = NULL;
	long long fPosPrev = NULL;

	fPosPrev = InsertRootBranchIndexRecord(&channelNum, &newBranchIndex, &oldBranchIndex, pBranch1->currFPos, numMon, fContext, true, pBranch2);

	posNewBranch = fPosPrev;

	CopyMemory(&oldIndexHdr, &newBranchIndex, sizeof(struct IndexHdr));

	// Now setup in insertion point for the new segment.
	if (pBranch2 != NULL)
	{
		// insertion point is the 2nd branch bottom
		pDataFile->ReadFromFile((char *)&oldIndexHdr, sizeof(struct IndexHdr), posBranch1Right);

		// Read New Branch added and set bottom branch to right Branches bottom branch
		pDataFile->ReadFromFile((char *)&newBranchIndex, sizeof(struct IndexHdr), posNewBranch);
		newBranchIndex.fPosNextRecord = oldIndexHdr.fPosNextRecord;
		pDataFile->WriteToFile(posNewBranch, (char *)&newBranchIndex, sizeof(struct IndexHdr));
		UpdateIndexPageDescriptor(posNewBranch, &newBranchIndex);

		oldIndexHdr.fPosPrevRecord = posNewBranch;
		pDataFile->WriteToFile(posBranch1Right, (char *)&oldIndexHdr, sizeof(struct IndexHdr));
		UpdateIndexPageDescriptor(posBranch1Right, &oldIndexHdr);
		fPosPrev = posBranch1Right;
	}
	//else
	//{
	//	// Read New Branch added and set bottom branch to right Branches bottom branch
	//	long long oldRightPos = pBranch1->nextRightFPos;
	//	newBranchIndex.fPosNextRecord = oldRightPos;
	//	pDataFile->WriteToFile(posNewBranch, (char *)&newBranchIndex, sizeof(struct IndexHdr));
	//	UpdateIndexPageDescriptor(posNewBranch, &newBranchIndex);

	//	struct IndexHdr bottomTopIndexHdr;
	//	pDataFile->ReadFromFile((char *)&bottomTopIndexHdr, sizeof(struct IndexHdr), oldRightPos);
	//	bottomTopIndexHdr.fPosPrevRecord = posNewBranch;
	//	pDataFile->WriteToFile(oldRightPos, (char *)&bottomTopIndexHdr, sizeof(struct IndexHdr));
	//}

 	// Now add the new segement either to the right of the new branch (if pBranch2 == NULL) or to the bottom of pBranch2
	int nChannelNum = -1;
	bool bChannelAdded = false;
	if (pSrcChannelData->GetChannelKey() != pChannelData->GetChannelKey())
	{
		nChannelNum = FindChannelInVideo(pSrcChannelData->GetChannelKey());
		if (nChannelNum != pBranch1->branchChannelNum)
		{
			bChannelAdded = true;
			// Add a channel in the destination file along with monitor details
			if (nChannelNum == -1)
				nChannelNum = AddNewChannel(pSrcChannelData);
			// Insert a channel context Index record;
			ZeroMemory(&channelIndexHdr, sizeof(struct IndexHdr));
			channelIndexHdr.RecType = 'H';
			channelIndexHdr.numRecs = 0;
			channelIndexHdr.recDurationus = 0L;
			channelIndexHdr.timestamp = copyTopIndexHdr.timestamp;
			channelNum = nChannelNum; //pCopySegment->GetChannelNum();
			pChannelData->GetChannelHeader()->lastIndexNumber++;
			channelIndexHdr.dataRecs[0].videoIndexNumber = pChannelData->GetChannelHeader()->lastIndexNumber;
			channelIndexHdr.dataRecs[0].size = nChannelNum;
			channelIndexHdr.dataRecs[0].compSize = destPositionMarker.channelNum;
			if (pBranch2 == NULL)
			{
				fPosPrev = InsertRightIndexRecord(&channelNum, &channelIndexHdr, &oldIndexHdr, fPosPrev, numMon, fContext);
			}
			else
			{
				fPosPrev = InsertIndexRecord(&channelNum, &channelIndexHdr, &oldIndexHdr, fPosPrev, numMon, fContext);
			}
			GetInsertEventsAndVideoInSegment(&copyTopIndexHdr, &newIndexHdr, pSrcContext);
			// followed by copy top index
			fPosPrev = InsertIndexRecord(&channelNum, &newIndexHdr, &channelIndexHdr, fPosPrev, pSrcChannelData->GetNumMonitors(), pSrcContext);
		}
		else
		{
			// Insert a copy top index after start Top
			GetInsertEventsAndVideoInSegment(&copyTopIndexHdr, &newIndexHdr, pSrcContext);
			// This function re-writes the destTopIndexHdr, and appends the indexNewHdr
			if (pBranch2 == NULL)
			{
				fPosPrev = InsertRightIndexRecord(&channelNum, &newIndexHdr, &oldIndexHdr, fPosPrev, pSrcChannelData->GetNumMonitors(), pSrcContext);
			}
			else
			{
				fPosPrev = InsertIndexRecord(&channelNum, &newIndexHdr, &oldIndexHdr, fPosPrev, pSrcChannelData->GetNumMonitors(), pSrcContext);
			}
		}
	}
	else
	{
		// Insert a copy top index after start Top
		GetInsertEventsAndVideoInSegment(&copyTopIndexHdr, &newIndexHdr, pSrcContext);
		// This function re-writes the destTopIndexHdr, and appends the indexNewHdr
		if (pBranch2 == NULL)
		{
			fPosPrev = InsertRightIndexRecord(&channelNum, &newIndexHdr, &oldIndexHdr, fPosPrev, pSrcChannelData->GetNumMonitors(), pSrcContext);
		}
		else
		{
			fPosPrev = InsertIndexRecord(&channelNum, &newIndexHdr, &oldIndexHdr, fPosPrev, pSrcChannelData->GetNumMonitors(), pSrcContext);
		}
	}

	CopyMemory(&oldIndexHdr, &newIndexHdr, sizeof(struct IndexHdr));

	long long fPosNextIndex = copyTopIndexHdr.fPosNextRecord;

	struct IndexHdr indexHdr;
	ZeroMemory(&indexHdr, sizeof(struct IndexHdr));
	/*---------------------------------------------------------*/
	// Write remaining index records in the copy segment
	if (fPosNextIndex != NULL && copyTopIndexHdr.fPosNextRecord != copyBottomIndexHdr.fPosNextRecord)
	{
		pSrcContext->ReadFromFile((char *)&indexHdr, sizeof(struct IndexHdr), fPosNextIndex);
		while (indexHdr.fPosNextRecord != NULL && indexHdr.fPosNextRecord != copyBottomIndexHdr.fPosNextRecord && indexHdr.fPosPrevRecord != copyBottomIndexHdr.fPosPrevRecord)
		{
			CopyMemory(&oldIndexHdr, &newIndexHdr, sizeof(struct IndexHdr));
			if (indexHdr.RecType == 'P' || indexHdr.RecType == 'B')
				GetInsertEventsAndVideoInSegment(&indexHdr, &newIndexHdr, pSrcContext);
			else
				CopyMemory(&newIndexHdr, &indexHdr, sizeof(struct IndexHdr));
			fPosPrev = InsertIndexRecord(&channelNum, &newIndexHdr, &oldIndexHdr, fPosPrev, pSrcChannelData->GetNumMonitors(), pSrcContext);
			if (newIndexHdr.RecType == 'J')
			{
				fPosNextIndex = oldIndexHdr.fPosNextRecord;
			}
			else
			{
				fPosNextIndex = indexHdr.fPosNextRecord;
			}
			if (fPosNextIndex != NULL)
				pSrcContext->ReadFromFile((char *)&indexHdr, sizeof(struct IndexHdr), fPosNextIndex);
		}
	}
	CopyMemory(&oldIndexHdr, &newIndexHdr, sizeof(struct IndexHdr));
	if (copyBottomIndexHdr.numRecs > numMon)
	{
		// Write the copy Bottom index
		GetInsertEventsAndVideoInSegment(&copyBottomIndexHdr, &newIndexHdr, pSrcContext);
		fPosPrev = InsertIndexRecord(&channelNum, &newIndexHdr, &oldIndexHdr, fPosPrev, numMon, pSrcContext);
		CopyMemory(&oldIndexHdr, &newIndexHdr, sizeof(struct IndexHdr));
	}

	// Write a channel change if required
	if (bChannelAdded)
	{
		// Insert a channel context Index record;
		ZeroMemory(&channelIndexHdr, sizeof(struct IndexHdr));
		channelIndexHdr.RecType = 'H';
		channelIndexHdr.numRecs = 0;
		channelIndexHdr.recDurationus = 0L;
		channelIndexHdr.timestamp = startTopIndexHdr.timestamp;
		channelNum = nChannelNum;
		pChannelData->GetChannelHeader()->lastIndexNumber++;
		channelIndexHdr.dataRecs[0].videoIndexNumber = pChannelData->GetChannelHeader()->lastIndexNumber;
		channelIndexHdr.dataRecs[0].size = destPositionMarker.channelNum; //pCopySegment->GetChannelNum();
		channelIndexHdr.dataRecs[0].compSize = nChannelNum;

		fPosPrev = InsertIndexRecord(&channelNum, &channelIndexHdr, &oldIndexHdr, fPosPrev, numMon, pSrcContext);
		CopyMemory(&oldIndexHdr, &channelIndexHdr, sizeof(struct IndexHdr));
	}

	struct IndexHdr newJoinIndex;
	ZeroMemory(&newJoinIndex, sizeof(struct IndexHdr));
	newJoinIndex.RecType = 'J';
	pChannelData->GetChannelHeader()->lastIndexNumber++;
	newJoinIndex.dataRecs[0].videoIndexNumber = pChannelData->GetChannelHeader()->lastIndexNumber;
	if (pJoin2 == NULL)
		newJoinIndex.dataRecs[0].fPosData = fPosPrev;
	else
	{
		if (pJoin3 == NULL)
		{
			newJoinIndex.dataRecs[0].fPosData = pJoin2->prevRightFpos;
		}
		else
		{
			newJoinIndex.dataRecs[0].fPosData = pJoin3->currFPos;
		}
	}

	long long posLastNewEvent = fPosPrev;
	fPosPrev = InsertJoinIndexRecord(&channelNum, &newJoinIndex, &oldIndexHdr, fPosPrev, numMon);
	if (pJoin2 == NULL)
	{
		newJoinIndex.fPosNextRecord = pJoin1->currFPos;
		newJoinIndex.fPosPrevRecord = pJoin1->prevRightFpos;
	}
	else
	{
		newJoinIndex.fPosNextRecord = pJoin2->currFPos;
		newJoinIndex.fPosPrevRecord = posLastNewEvent;
	}
	pDataFile->WriteToFile(fPosPrev, (char *)&newJoinIndex, sizeof(struct IndexHdr));
	UpdateIndexPageDescriptor(fPosPrev, &newJoinIndex);

	if (pJoin2 != NULL)
	{
		// Read the bottom last record and update next to new Join
		pDataFile->ReadFromFile((char *)&newIndexHdr, sizeof(struct IndexHdr), newJoinIndex.dataRecs[0].fPosData);
		newIndexHdr.fPosNextRecord = fPosPrev;
		pDataFile->WriteToFile(newJoinIndex.dataRecs[0].fPosData, (char *)&newIndexHdr, sizeof(struct IndexHdr));
		UpdateIndexPageDescriptor(newJoinIndex.dataRecs[0].fPosData, &newIndexHdr);
	}

	// At this point we have a new Branch Record with the Copy segment either copied to the right or bottom of pBranch2
	// Now reset the pointers as required

	struct IndexHdr joinIndexHdr;
	//long long fPosOldRightJoin = 0LL;

	if (pJoin2 != NULL)
	{
		pDataFile->ReadFromFile((char *)&joinIndexHdr, sizeof(struct IndexHdr), pJoin2->currFPos);
		joinIndexHdr.dataRecs[0].fPosData = fPosPrev;
		pDataFile->WriteToFile(pJoin2->currFPos, (char *)&joinIndexHdr, sizeof(struct IndexHdr));
		UpdateIndexPageDescriptor(pJoin2->currFPos, &joinIndexHdr);
	}

	//if (pJoin3 != NULL)
	//{
	//	pDataFile->ReadFromFile((char *)&joinIndexHdr, sizeof(struct IndexHdr), pJoin2->currFPos);
	//	joinIndexHdr.fPosNextRecord = fPosPrev;
	//	pDataFile->WriteToFile(pJoin3->currFPos, (char *)&joinIndexHdr, sizeof(struct IndexHdr));
	//	UpdateIndexPageDescriptor(pJoin3->currFPos, &joinIndexHdr);
	//}


	// Write Channel Header Information
	pDataFile->ReadFromFile((char *)&rootRecord, sizeof(struct rootHdr), 0L);
	long long channelFpos = rootRecord.posChannelRecords;
	pChannelData->WriteChannelHeader(channelFpos);

	// Insert Events Here
	InsertEventsInNewBranch(pCopySegment, posNewBranch, fPosPrev);
	return fPosPrev;
}

long long FileContext::InsertJoinIndexRecord(short *channelNum, struct IndexHdr *pCurrIndex, struct IndexHdr *pPrevIndex, long long posPrevIndex, int numMon)
{
	long long posAdded = 0L;

	// Need to handle insert at top of channel
	pCurrIndex->fPosPrevRecord = posPrevIndex;
	// writing to the root record
	ChannelDataMapping *pChannelData = GetChannelDataMapping(*channelNum);
	if (pChannelData == NULL)
		return 0L;
	struct VideoHdr *pVHeader;
	pVHeader = pChannelData->GetVideoHeader();

	pCurrIndex->fPosNextRecord = pPrevIndex->fPosNextRecord;
	pPrevIndex->fPosNextRecord = pDataFile->AppendToFile((char *)pCurrIndex, (ULONG)sizeof(struct IndexHdr));
	posAdded = pPrevIndex->fPosNextRecord;
	pDataFile->WriteToFile(posPrevIndex, (char *)pPrevIndex, (ULONG)sizeof(struct IndexHdr));

	struct indexPageDescriptor *indexPageDesc = DBG_NEW struct indexPageDescriptor;
	ZeroMemory(indexPageDesc, sizeof(struct indexPageDescriptor));
	indexPageDesc->next = NULL;
	indexPageDesc->channelNum = *channelNum;
	map<long long, struct indexPageDescriptor *>::iterator itMapNextDescriptor = mapVideoPages.find(pCurrIndex->fPosNextRecord);
	if (itMapNextDescriptor != mapVideoPages.end())
	{
		indexPageDesc->next = itMapNextDescriptor->second;
	}
	indexPageDesc->nPageType = 3;
	indexPageDesc->numRecs = 0;
	indexPageDesc->pageDescriptor.pJoin = DBG_NEW struct joinIndexDescriptor;
	indexPageDesc->pageDescriptor.pJoin->currFPos = posAdded;
	indexPageDesc->pageDescriptor.pJoin->prevTopFpos = posPrevIndex;
	indexPageDesc->pageDescriptor.pJoin->nextFpos = pCurrIndex->fPosNextRecord;
	indexPageDesc->pageDescriptor.pJoin->bPathDir = false;
	for (int i = 0; i <= 0; i++)
	{
		indexPageDesc->dataRecs[i].indexNum = pCurrIndex->dataRecs[i].videoIndexNumber;
		indexPageDesc->dataRecs[i].recDuration = (long)pCurrIndex->dataRecs[i].recDurationus;
		indexPageDesc->pageDescriptor.pVideo->timespan += pCurrIndex->dataRecs[i].recDurationus;
	}

	mapVideoPages.insert(pair<long long, struct indexPageDescriptor *>(posAdded, indexPageDesc));
	// Look up previous Index Hdr
	UpdatePreviousIndexHdr(posPrevIndex, posAdded, pPrevIndex, indexPageDesc);

	return posAdded; // pPrevIndex->fPosNextRecord;
}

void FileContext::UpdatePreviousIndexHdr(long long posPrevIndex, long long posAdded, struct IndexHdr *pPrevIndex, struct indexPageDescriptor *indexPageDesc)
{
	if (posPrevIndex != NULL)
	{
		UpdateIndexPageDescriptor(posPrevIndex, pPrevIndex);
		map<long long, struct indexPageDescriptor *>::iterator itMapDescriptor = mapVideoPages.find(posPrevIndex);
		if (itMapDescriptor != mapVideoPages.end())
		{
			struct indexPageDescriptor *pIndexDescriptor = itMapDescriptor->second;
			if (pIndexDescriptor->nPageType == 1 || pIndexDescriptor->nPageType == 4)
			{
				pIndexDescriptor->pageDescriptor.pVideo->nextFpos = posAdded;
			}
			else if (pIndexDescriptor->nPageType == 5)
			{
				pIndexDescriptor->pageDescriptor.pChannel->nextFPos = posAdded;
			}
			else if (pIndexDescriptor->nPageType == 2)
			{
				pIndexDescriptor->pageDescriptor.pBranch->nextBottomFPos = posAdded;
			}
			else if (pIndexDescriptor->nPageType == 3)
			{
				pIndexDescriptor->pageDescriptor.pJoin->nextFpos = posAdded;
			}
			pIndexDescriptor->next = indexPageDesc;
		}
	}
}

void FileContext::InsertEventsInNewBranch(VideoSegmentClass *pCopySegment, long long posNewBranch, long long posNewJoin)
{
	struct indexPageDescriptor *pBranchIndex = NULL;
	struct indexPageDescriptor *pBranchNextIndex = NULL;
	struct indexPageDescriptor *pBranchLeftIndex = NULL;
	struct indexPageDescriptor *pBranchRightIndex = NULL;
	struct indexPageDescriptor *pBranchJoinIndex = NULL;

	map <long long, struct indexPageDescriptor *>::iterator itMap = mapVideoPages.find(posNewBranch);
	if (itMap == mapVideoPages.end())
		return;
	pBranchIndex = itMap->second;

	itMap = mapVideoPages.find(pBranchIndex->pageDescriptor.pBranch->prevFpos);
	if (itMap != mapVideoPages.end())
	{
		pBranchLeftIndex = itMap->second;
	}

	itMap = mapVideoPages.find(pBranchIndex->pageDescriptor.pBranch->nextRightFPos);
	if (itMap != mapVideoPages.end())
	{
		pBranchRightIndex = itMap->second;
	}


	// Create new Decision Element and Add its bottom elements
	BPMNDecision *pDecisionElement = DBG_NEW BPMNDecision(pBranchIndex->dataRecs[0].indexNum);
	pDecisionElement->SetBranchRightChannel(pBranchIndex->channelNum);
	pDecisionElement->SetBranchBottomChannel(pBranchIndex->channelNum);
	pDecisionElement->SetAnnotation("New Branch");
	pDecisionElement->SetEventType(50);

	list <BPMNElement *> *pEventList = pBPMNFacade->GetBPMNDiagram();

	// Get the index page for the next video element after join
	itMap = mapVideoPages.find(posNewJoin);
	if (itMap == mapVideoPages.end())
		return;
	pBranchJoinIndex = itMap->second;
	pBranchNextIndex = pBranchJoinIndex->next;
	if (pBranchNextIndex->nPageType != 1) // if not video page then get next video page
	{
		pBranchNextIndex = GetNextVideoIndexDescriptor(pBranchNextIndex);
	}

	ULONG nEventIndex = 0L;
	list <BPMNElement *>::iterator itFind = pEventList->end();
	BPMNDecision *pBranchElement = NULL;
	list <BPMNElement *> *pBranchElementList = NULL;
	list <BPMNElement *> branchElements;
	branchElements.clear();
	// Here add the correct Bottom Branch Elements to the new Branch element
	if (pBranchRightIndex->nPageType != 2) // If Right Branch is not a Branch then add Left Branch's right elements to new branch bottom
	{
		nEventIndex = GetInsertionElementIndex(pBranchLeftIndex);
		itFind = FindEventIteratorInList(&pEventList, nEventIndex, false);
		pBranchElement = (BPMNDecision *)*itFind;
		pBranchElementList = pBranchElement->GetBranchRightElements();
	}
	else
	{  // Else if Right branch is a branch then add the right branches bottom branch element to new branch bottom element
		nEventIndex = GetInsertionElementIndex(pBranchRightIndex);
		itFind = FindEventIteratorInList(&pEventList, nEventIndex, false);
		pBranchElement = (BPMNDecision *)*itFind;
		pBranchElementList = pBranchElement->GetBranchBottomElements();
	}

	if (pBranchElementList->size() > 0)
	{
		branchElements.insert(branchElements.begin(), pBranchElementList->begin(), pBranchElementList->end());
		pBranchElementList->clear();
	}
	pDecisionElement->AddBottomBranchList(branchElements); // no change indexes
	UpdateEventList(&branchElements);

	// Adding a new eventListElement and new event Record
	map <ULONG, struct eventListElement *>::iterator itMapElement = pEventMap->end();
	struct eventListElement *pNewMapElement = NULL;
	struct eventListElement *pNextMapElement = NULL;

	pNewMapElement = DBG_NEW struct eventListElement;
	ZeroMemory(pNewMapElement, sizeof(struct eventListElement));

	pNewMapElement->pElement = (BPMNElement *)pDecisionElement;
	itMapElement = pEventMap->find(pBranchLeftIndex->dataRecs[0].indexNum);
	if (itMapElement != pEventMap->end())
	{
		pNewMapElement->prevElement = itMapElement->second;
		pNewMapElement->nextElement = pNewMapElement->prevElement->nextElement;
	}
	pNextMapElement = pNewMapElement->nextElement;
	pEventMap->insert(pair<ULONG, struct eventListElement *>(pDecisionElement->GetEventIndex(), pNewMapElement));
	AddNewEventRecord(pNewMapElement);

	// Now Add the Decision Element to the right side of the Prev Element

	nEventIndex = GetInsertionElementIndex(pBranchLeftIndex);
	pEventList = pBPMNFacade->GetBPMNDiagram();
	pBranchElement = (BPMNDecision *)FindEventInList(pEventList, nEventIndex);
	pBranchElementList = pBranchElement->GetBranchRightElements();
	pBranchElementList->clear();
	pBranchElement->AddRightElement(pDecisionElement);

	//if (pBranchRightIndex->nPageType != 2) // If Right Branch is not a Branch then add decision box to left branch's right 
	//	pBranchElement->AddRightElement(pDecisionElement);
	//else
	//{
	//	list <BPMNElement *> *pRightBranchList = pBranchElement->GetBranchRightElements();
	//	pRightBranchList->pop_front();
	//	pRightBranchList->push_front(pDecisionElement);
	//}
	UpdateEventList(pBranchElement->GetBranchRightElements());

	// Update New Branch Event to reflect next pointer correctly
	itMapElement = pEventMap->find(pBranchLeftIndex->dataRecs[0].indexNum);
	if (itMapElement != pEventMap->end())
	{
		pNewMapElement->prevElement = itMapElement->second;
		pNewMapElement->nextElement = pNewMapElement->prevElement->nextElement;
		pNextMapElement = pNewMapElement->nextElement;
	}
	UpdateEventRecord(pNewMapElement, NULL);


	if (pBranchRightIndex->nPageType != 2) // If Right Branch is not a Branch then add Left Branch's right elements to current bottom, and new elements to the right
	{
		// Insertion point for segment events
		pDecisionElement->AddRightBranchList(insertSegmentEvents); // changed indexes
		UpdateEventList(&insertSegmentEvents);
	}
	else
	{
		nEventIndex = GetInsertionElementIndex(pBranchRightIndex);
		pEventList = pBPMNFacade->GetBPMNDiagram();
		pBranchElement = (BPMNDecision *)FindEventInList(pEventList, nEventIndex);
		pDecisionElement->AddRightElement(pBranchElement);
		UpdateEventList(pDecisionElement->GetBranchRightElements());
		// UpdateEventList sets next pointer to NULL so reset it to what is needed
		
		map <ULONG, struct eventListElement *>::iterator itEvent = pEventMap->find(pBranchElement->GetEventIndex());
		struct eventListElement *pEventListElement = NULL;
		if (itEvent != pEventMap->end())
		{
			pEventListElement = itEvent->second;
			pEventListElement->nextElement = pNextMapElement;
			pEventListElement->prevElement = pNewMapElement;
			UpdateEventRecord(pEventListElement, NULL);
		}
		// Insertion point for segment events
		pBranchElement->AddBottomBranchList(insertSegmentEvents);
		UpdateEventList(&insertSegmentEvents);
	}

}

struct joinIndexDescriptor *FileContext::GetJoinDescriptor(struct branchIndexDescriptor *pBranch)
{
	long long fPosNext = pBranch->nextBottomFPos;

	while (true)
	{
		map <long long, struct indexPageDescriptor *>::iterator itMap = mapVideoPages.find(fPosNext);
		if (itMap == mapVideoPages.end())
		{
			return NULL;
		}
		struct indexPageDescriptor *pIndexDescriptor = itMap->second;
		if (pIndexDescriptor->nPageType == 1 || pIndexDescriptor->nPageType == 4)
		{
			fPosNext = pIndexDescriptor->pageDescriptor.pVideo->nextFpos;
		}
		else  if (pIndexDescriptor->nPageType == 2)
		{
			fPosNext = pIndexDescriptor->pageDescriptor.pBranch->nextBottomFPos;
		}
		else if (pIndexDescriptor->nPageType == 5)
			fPosNext = pIndexDescriptor->pageDescriptor.pChannel->nextFPos;
		else
			return(pIndexDescriptor->pageDescriptor.pJoin);
	}

	return NULL;
}

long long FileContext::InsertRightIndexRecord(short *channelNum, struct IndexHdr *pCurrIndex, struct IndexHdr *pPrevIndex, long long posPrevIndex, int numMon, FileContext *fContext)
{
	long long posAdded = 0L;
	//struct IndexHdr lastIndexHdr;

	// Need to handle insert at top of channel
	pCurrIndex->fPosPrevRecord = posPrevIndex;
	// writing to the root record
	ChannelDataMapping *pChannelData = GetChannelDataMapping(*channelNum);
	if (pChannelData == NULL)
		return 0L;
	if (fContext != this)
	{
		// if Copying from a different context, then copy the datarecs of the current Index in the secondary file to the current file
		long long fSizeVideo = fContext->GetVideoBufferSize();
		//long long fSizeAudio = fContext->GetAudioBufferSize();
		if (fSizeVideo > maxVideoBufferSize)
			maxVideoBufferSize = fSizeVideo;
		//if (fSizeAudio > maxAudioBufferSize)
		//	maxAudioBufferSize = fSizeAudio;

		char *fBuffer = (char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, fSizeVideo);
		//char *fAudioBuffer = (char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, fSizeAudio);
		DataFile *pSrcDataFile = fContext->GetDataFile();
		for (int i = 0; i <= pCurrIndex->numRecs; i++)
		{
			pSrcDataFile->ReadCompressedBlockFromFile(fBuffer, pCurrIndex->dataRecs[i].compSize, pCurrIndex->dataRecs[i].fPosData);
			pCurrIndex->dataRecs[i].fPosData = pDataFile->AppendCompressedBlockToFile(fBuffer, pCurrIndex->dataRecs[i].compSize);
			if (pCurrIndex->dataRecs[i].fPosAudioData != 0L)
			{
				pSrcDataFile->ReadCompressedBlockFromFile(fBuffer, pCurrIndex->dataRecs[i].compAudioSize, pCurrIndex->dataRecs[i].fPosAudioData);
				pCurrIndex->dataRecs[i].fPosAudioData = pDataFile->AppendCompressedBlockToFile(fBuffer, pCurrIndex->dataRecs[i].compAudioSize);
			}
		}
		HeapFree(GetProcessHeap(), 0, fBuffer);
	}


	pPrevIndex->dataRecs[0].fPosData = pDataFile->AppendToFile((char *)pCurrIndex, (ULONG)sizeof(struct IndexHdr));
	posAdded = pPrevIndex->dataRecs[0].fPosData;
	pDataFile->WriteToFile(posPrevIndex, (char *)pPrevIndex, (ULONG)sizeof(struct IndexHdr));

	struct indexPageDescriptor *indexPageDesc = DBG_NEW struct indexPageDescriptor;
	ZeroMemory(indexPageDesc, sizeof(struct indexPageDescriptor));
	indexPageDesc->next = NULL;
	indexPageDesc->channelNum = *channelNum;
	map<long long, struct indexPageDescriptor *>::iterator itMapNextDescriptor = mapVideoPages.find(pCurrIndex->fPosNextRecord);
	if (itMapNextDescriptor != mapVideoPages.end())
	{
		indexPageDesc->next = itMapNextDescriptor->second;
	}
	if (pCurrIndex->RecType == 'P')
	{
		indexPageDesc->nPageType = 1;
		indexPageDesc->pageDescriptor.pVideo = DBG_NEW struct videoIndexDescriptor;
		indexPageDesc->numRecs = pCurrIndex->numRecs;
		indexPageDesc->pageDescriptor.pVideo->currFPos = posAdded;
		indexPageDesc->pageDescriptor.pVideo->prevFpos = posPrevIndex;
		indexPageDesc->pageDescriptor.pVideo->nextFpos = pCurrIndex->fPosNextRecord; 
		indexPageDesc->pageDescriptor.pVideo->startTime = 0L;
		indexPageDesc->pageDescriptor.pVideo->timespan = 0;
		indexPageDesc->pageDescriptor.pVideo->inPath = true;
		int monNum = pChannelData->GetNumMonitors() - 1;
		for (int i = 0; i <= pCurrIndex->numRecs; i++)
		{
			indexPageDesc->dataRecs[i].indexNum = pCurrIndex->dataRecs[i].videoIndexNumber;
			indexPageDesc->dataRecs[i].recDuration = (long)pCurrIndex->dataRecs[i].recDurationus;
			if (i >= monNum)
				indexPageDesc->pageDescriptor.pVideo->timespan += pCurrIndex->dataRecs[i].recDurationus;
		}
	}
	mapVideoPages.insert(pair<long long, struct indexPageDescriptor *>(posAdded, indexPageDesc));

	map<long long, struct indexPageDescriptor *>::iterator itMapDescriptor = mapVideoPages.find(posPrevIndex);
	if (itMapDescriptor != mapVideoPages.end())
	{
		struct indexPageDescriptor *pIndexDescriptor = itMapDescriptor->second;
		if (pIndexDescriptor->nPageType == 2)
		{
			pIndexDescriptor->pageDescriptor.pBranch->nextRightFPos = posAdded;
		}
		pIndexDescriptor->next = indexPageDesc;
	}
	return posAdded; // pPrevIndex->fPosNextRecord;
}


long long FileContext::InsertRootBranchIndexRecord(short *channelNum, struct IndexHdr *pCurrIndex, struct IndexHdr *pPrevIndex, long long posPrevIndex, int numMon, FileContext *fContext, bool bFlg, struct branchIndexDescriptor *pBranchRight)
{
	long long posAdded = 0L;
	ChannelDataMapping *pChannelData = GetChannelDataMapping(*channelNum);  // changed on 12/28 from mainChannelNumber
	if (pChannelData == NULL)
		return 0L;

	pChannelData->GetChannelHeader()->lastIndexNumber++;
	pCurrIndex->dataRecs[0].videoIndexNumber = pChannelData->GetChannelHeader()->lastIndexNumber;
	// Need to handle insert at top of channel
	pCurrIndex->fPosPrevRecord = posPrevIndex;
	if (bFlg)
	{
		if (pBranchRight)
			pCurrIndex->dataRecs[0].fPosData = pPrevIndex->dataRecs[0].fPosData;
		else
			pCurrIndex->fPosNextRecord = pPrevIndex->dataRecs[0].fPosData;
	}
	else
		pCurrIndex->fPosNextRecord = pPrevIndex->fPosNextRecord;  // This should be set in calling function
	// writing to the root record
	posAdded = pDataFile->AppendToFile((char *)pCurrIndex, (ULONG)sizeof(struct IndexHdr));
	if (bFlg) // if branch to be added to right
		pPrevIndex->dataRecs[0].fPosData = posAdded;
	else
		pPrevIndex->fPosNextRecord = posAdded;
	pDataFile->WriteToFile(posPrevIndex, (char *)pPrevIndex, sizeof(struct IndexHdr));

	struct indexPageDescriptor *indexPageDesc = DBG_NEW struct indexPageDescriptor;
	ZeroMemory(indexPageDesc, sizeof(struct indexPageDescriptor));
	indexPageDesc->next = NULL;
	indexPageDesc->channelNum = *channelNum;
	indexPageDesc->nPageType = 2;
	indexPageDesc->numRecs = 0;
	indexPageDesc->pageDescriptor.pBranch = DBG_NEW struct branchIndexDescriptor;
	indexPageDesc->pageDescriptor.pBranch->currFPos = posAdded;
	indexPageDesc->pageDescriptor.pBranch->prevFpos = posPrevIndex;
	indexPageDesc->pageDescriptor.pBranch->nextBottomFPos = pCurrIndex->fPosNextRecord;
	indexPageDesc->pageDescriptor.pBranch->nextRightFPos = pCurrIndex->dataRecs[0].fPosData;
	indexPageDesc->pageDescriptor.pBranch->bPathDir = false;
	indexPageDesc->pageDescriptor.pBranch->branchChannelNum = *channelNum;
	for (int i = 0; i <= 0; i++)
	{
		indexPageDesc->dataRecs[i].indexNum = pCurrIndex->dataRecs[i].videoIndexNumber;
		indexPageDesc->dataRecs[i].recDuration = (long)pPrevIndex->dataRecs[i].recDurationus;
		indexPageDesc->pageDescriptor.pVideo->timespan += pPrevIndex->dataRecs[i].recDurationus;
	}

	mapVideoPages.insert(pair<long long, struct indexPageDescriptor *>(posAdded, indexPageDesc));
	UpdateIndexPageDescriptor(posAdded, pCurrIndex);
	// Look up previous Index Hdr
	UpdateIndexPageDescriptor(posPrevIndex, pPrevIndex);

	return posAdded; // pPrevIndex->fPosNextRecord;
}



long long FileContext::CreateBranchInsertEventAndVideoList(VideoSegmentClass *pCopySegment, bool bRight)
{
	struct IndexHdr indexHdr;
	struct IndexHdr indexNewHdr, indexOldHdr;
	long long fPosPrev = 0L;
	short channelNum = destPositionMarker.channelNum;
	insertSegmentEvents.clear();
	parentIndexMapping.clear();
	struct IndexHdr channelIndexHdr;
	long long fPosNextIndex = copyTopIndexHdr.fPosNextRecord;
	ChannelDataMapping *pChannelData = GetChannelDataMapping(channelNum);  // changed on 12/28 from mainChannelNumber
	if (pChannelData == NULL)
		return 0L;
	FileContext *pSrcContext = pCopySegment->GetFileContext();
	ChannelDataMapping *pSrcChannelData = pSrcContext->GetChannelDataMapping(pCopySegment->GetChannelNum());
	int nChannelNum = -1;
	bool bChannelAdded = false;
	if (pSrcChannelData->GetChannelKey() != pChannelData->GetChannelKey())
	{
		nChannelNum = FindChannelInVideo(pSrcChannelData->GetChannelKey());
		if (nChannelNum != destPositionMarker.channelNum)
		{
			bChannelAdded = true;
			// Add a channel in the destination file along with monitor details
			if (nChannelNum == -1)
				nChannelNum = AddNewChannel(pSrcChannelData);
			// Insert a channel context Index record;
			ZeroMemory(&channelIndexHdr, sizeof(struct IndexHdr));
			channelIndexHdr.RecType = 'H';
			channelIndexHdr.numRecs = 0;
			channelIndexHdr.recDurationus = 0L;
			channelIndexHdr.timestamp = copyTopIndexHdr.timestamp;
			channelNum = nChannelNum; //pCopySegment->GetChannelNum();
			pChannelData->GetChannelHeader()->lastIndexNumber++;
			channelIndexHdr.dataRecs[0].videoIndexNumber = pChannelData->GetChannelHeader()->lastIndexNumber;
			channelIndexHdr.dataRecs[0].size = nChannelNum;
			channelIndexHdr.dataRecs[0].compSize = destPositionMarker.channelNum;
			// Insert a Channel change after Start Top index
			fPosPrev = InsertIndexRecord(&channelNum, &channelIndexHdr, &destTopIndexHdr, destPositionMarker.fPosIndex, pSrcChannelData->GetNumMonitors(), pSrcContext);
			GetInsertEventsAndVideoInSegment(&copyTopIndexHdr, &indexNewHdr, pSrcContext);
			// followed by copy top index
			fPosPrev = InsertIndexRecord(&channelNum, &indexNewHdr, &channelIndexHdr, fPosPrev, pSrcChannelData->GetNumMonitors(), pSrcContext);
		}
		else
		{
			// Insert a copy top index after start Top
			GetInsertEventsAndVideoInSegment(&copyTopIndexHdr, &indexNewHdr, pSrcContext);
			// This function re-writes the destTopIndexHdr, and appends the indexNewHdr
			fPosPrev = InsertIndexRecord(&channelNum, &indexNewHdr, &destTopIndexHdr, destPositionMarker.fPosIndex, pSrcChannelData->GetNumMonitors(), pSrcContext);
		}
	}
	else
	{
		// Insert a copy top index after start Top
		GetInsertEventsAndVideoInSegment(&copyTopIndexHdr, &indexNewHdr, pSrcContext);
		// This function re-writes the destTopIndexHdr, and appends the indexNewHdr
		fPosPrev = InsertIndexRecord(&channelNum, &indexNewHdr, &destTopIndexHdr, destPositionMarker.fPosIndex, pSrcChannelData->GetNumMonitors(), pSrcContext);
	}
	// Write remaining index records in the copy segment
	if (fPosNextIndex != NULL && copyTopIndexHdr.fPosNextRecord != copyBottomIndexHdr.fPosNextRecord)
	{
		pSrcContext->ReadFromFile((char *)&indexHdr, sizeof(struct IndexHdr), fPosNextIndex);
		while (indexHdr.fPosNextRecord != NULL && indexHdr.fPosNextRecord != copyBottomIndexHdr.fPosNextRecord && indexHdr.fPosPrevRecord != copyBottomIndexHdr.fPosPrevRecord)
		{
			CopyMemory(&indexOldHdr, &indexNewHdr, sizeof(struct IndexHdr));
			if (indexHdr.RecType == 'P' || indexHdr.RecType == 'B')
				GetInsertEventsAndVideoInSegment(&indexHdr, &indexNewHdr, pSrcContext);
			else
				CopyMemory(&indexNewHdr, &indexHdr, sizeof(struct IndexHdr));
			fPosPrev = InsertIndexRecord(&channelNum, &indexNewHdr, &indexOldHdr, fPosPrev, pSrcChannelData->GetNumMonitors(), pSrcContext);
			if (indexNewHdr.RecType == 'J')
			{
				fPosNextIndex = indexOldHdr.fPosNextRecord;
			}
			else
			{
				fPosNextIndex = indexHdr.fPosNextRecord;
			}
			if (fPosNextIndex != NULL)
				pSrcContext->ReadFromFile((char *)&indexHdr, sizeof(struct IndexHdr), fPosNextIndex);
		}
	}
	CopyMemory(&indexOldHdr, &indexNewHdr, sizeof(struct IndexHdr));
	struct VideoHdr vHeader;

	ZeroMemory(&vHeader, sizeof(struct VideoHdr));
	pChannelData->GetVideoHeader(&vHeader);
	int numMon = vHeader.noMonitors;

	if (copyBottomIndexHdr.numRecs > numMon)
	{
		// Write the copy Bottom index
		GetInsertEventsAndVideoInSegment(&copyBottomIndexHdr, &indexNewHdr, pSrcContext);
		fPosPrev = InsertIndexRecord(&channelNum, &indexNewHdr, &indexOldHdr, fPosPrev, numMon, pSrcContext);
		CopyMemory(&indexOldHdr, &indexNewHdr, sizeof(struct IndexHdr));
	}

	// Write a channel change if required
	if (bChannelAdded)
	{
		// Insert a channel context Index record;
		ZeroMemory(&channelIndexHdr, sizeof(struct IndexHdr));
		channelIndexHdr.RecType = 'H';
		channelIndexHdr.numRecs = 0;
		channelIndexHdr.recDurationus = 0L;
		channelIndexHdr.timestamp = startTopIndexHdr.timestamp;
		channelNum = nChannelNum;
		pChannelData->GetChannelHeader()->lastIndexNumber++;
		channelIndexHdr.dataRecs[0].videoIndexNumber = pChannelData->GetChannelHeader()->lastIndexNumber;
		channelIndexHdr.dataRecs[0].size = destPositionMarker.channelNum; //pCopySegment->GetChannelNum();
		channelIndexHdr.dataRecs[0].compSize = nChannelNum;

		fPosPrev = InsertIndexRecord(&channelNum, &channelIndexHdr, &indexOldHdr, fPosPrev, numMon, pSrcContext);
		CopyMemory(&indexOldHdr, &channelIndexHdr, sizeof(struct IndexHdr));
	}
	// Write the start bottom index record.
	if (destTopIndexHdr.numRecs <= numMon)
	{
		struct IndexHdr lastRec;
		// This means that the video has been inserted, and the next record does not need to be inserted only the
		// record pointers to be setup
		pDataFile->ReadFromFile((char *)&lastRec, sizeof(struct IndexHdr), indexOldHdr.fPosNextRecord);
		lastRec.fPosPrevRecord = fPosPrev;
		pDataFile->WriteToFile(indexOldHdr.fPosNextRecord, (char *)&lastRec, sizeof(struct IndexHdr));
		UpdateIndexPageDescriptor(indexOldHdr.fPosNextRecord, &lastRec);
	}
	else if (destBottomIndexHdr.numRecs > numMon)
	{
		struct IndexHdr lastRec;

		fPosPrev = InsertIndexRecord(&channelNum, &destBottomIndexHdr, &indexOldHdr, fPosPrev, numMon, this);
		UpdateIndexPageDescriptor(fPosPrev, &destBottomIndexHdr);
		pDataFile->ReadFromFile((char *)&lastRec, sizeof(struct IndexHdr), destBottomIndexHdr.fPosNextRecord);
		lastRec.fPosPrevRecord = fPosPrev;
		pDataFile->WriteToFile(destBottomIndexHdr.fPosNextRecord, (char *)&lastRec, sizeof(struct IndexHdr));
		UpdateIndexPageDescriptor(destBottomIndexHdr.fPosNextRecord, &lastRec);
	}
	else
	{
		indexOldHdr.fPosNextRecord = destBottomIndexHdr.fPosNextRecord;
		pDataFile->WriteToFile(fPosPrev, (char *)&indexOldHdr, sizeof(struct IndexHdr));
		UpdateIndexPageDescriptor(fPosPrev, &indexOldHdr);
		if (destBottomIndexHdr.fPosNextRecord != NULL)
		{
			struct IndexHdr lastRec;

			pDataFile->ReadFromFile((char *)&lastRec, sizeof(struct IndexHdr), destBottomIndexHdr.fPosNextRecord);
			lastRec.fPosPrevRecord = fPosPrev;
			pDataFile->WriteToFile(destBottomIndexHdr.fPosNextRecord, (char *)&lastRec, sizeof(struct IndexHdr));
			UpdateIndexPageDescriptor(destBottomIndexHdr.fPosNextRecord, &lastRec);
		}
	}

	// Write Channel Header Information
	pDataFile->ReadFromFile((char *)&rootRecord, sizeof(struct rootHdr), 0L);
	long long channelFpos = rootRecord.posChannelRecords;
	pChannelData->WriteChannelHeader(channelFpos);
	return fPosPrev;
}



long long FileContext::CreateBranch(VideoSegmentClass *pCopySegment)
{
	pCopySegment->GetTopIndexHeader(&copyTopIndexHdr);
	pCopySegment->GetBottomIndexHeader(&copyBottomIndexHdr);
	pCopySegment->GetEventList(&copySegmentEvents);
	segmentEvents.clear();
	segmentEvents.insert(segmentEvents.begin(), copySegmentEvents.begin(), copySegmentEvents.end());


	// Should be done after a Cut or a Copy
	if (copyTopIndexHdr.numRecs == 0 && copyBottomIndexHdr.numRecs == 0)
		return 0L;

	if (startPositionMarker.fPosIndex == 0L && endPositionMarker.fPosIndex == 0L)
		return 0L;

	DestinationSegment(&startPositionMarker);
	long long startTime = GetMarkerTime(&startPositionMarker);
	if (destPositionMarker.fileNum != 1)// Paste and Branch can only be done on the main file;
		return startTime;

	if (PrepareForOperation(true, true))
	{
		ChannelDataMapping *pChannelData = GetChannelDataMapping(destPositionMarker.channelNum);
		if (pChannelData == NULL)
			return 0L;
		int channelNum = destPositionMarker.channelNum;
		//if (pCopySegment->GetChannelNum() != destPositionMarker.channelNum)
		//	channelNum = CopyAudioAndVideoToChannel(pChannelData->GetChannelHeader());
		CopyMemory(&destTopIndexHdr, &startTopIndexHdr, sizeof(struct IndexHdr));
		CopyMemory(&destBottomIndexHdr, &startBottomIndexHdr, sizeof(struct IndexHdr));
		if (destTopIndexHdr.numRecs == -1 && destTopIndexHdr.fPosPrevRecord != NULL)
		{
			// read previous record
			startPositionMarker.fPosIndex = destTopIndexHdr.fPosPrevRecord;
			destPositionMarker.fPosIndex = destTopIndexHdr.fPosPrevRecord;
			pDataFile->ReadFromFile((char *)&destTopIndexHdr, sizeof(struct IndexHdr), destTopIndexHdr.fPosPrevRecord);
		}
		if (destBottomIndexHdr.numRecs == 1 && destBottomIndexHdr.fPosNextRecord != NULL)  // To be changed to number monitors
		{
			startPositionMarker.fPosIndex = destBottomIndexHdr.fPosNextRecord;
			destPositionMarker.fPosIndex = destBottomIndexHdr.fPosNextRecord;
			pDataFile->ReadFromFile((char *)&destBottomIndexHdr, sizeof(struct IndexHdr), destBottomIndexHdr.fPosNextRecord);
		}
		destPositionMarker.channelNum = channelNum;
		long long fPosBranchInsPoint = 0L;
		//long long fPosRightBranchLastRecord = CreateInsertEventAndVideoList(&fPosBranchInsPoint, pCopySegment); // Check
		long long fPosBranchPos = AddBranchIndex(fPosBranchInsPoint);
		long long fPosRecordAfterJoin = CreateBottomBranchIndex(fPosBranchInsPoint);
		InsertEventsToBranch(channelNum, fPosBranchPos, fPosRecordAfterJoin);
		UpdateChannelHeader(channelNum);
	}
	return startTime;
}

bool FileContext::PrepareForOperation(bool bCopy, bool bCopyEnd)
{
	// This function will split the start and end index headers into 2 parts
	// Create a list of events within the segment in the list segmentEvents
	// finds the time of the segment in microseconds
	struct VideoHdr vHeader;

	ZeroMemory(&startTopIndexHdr, sizeof(struct IndexHdr));
	ZeroMemory(&startBottomIndexHdr, sizeof(struct IndexHdr));
	ZeroMemory(&endTopIndexHdr, sizeof(struct IndexHdr));
	ZeroMemory(&endBottomIndexHdr, sizeof(struct IndexHdr));
	ZeroMemory(&vHeader, sizeof(struct VideoHdr));

	long long startTime = GetMarkerTime(&startPositionMarker);
	long long endTime = GetMarkerTime(&endPositionMarker);
	if (startTime == endTime)
		return false;
	if (startTime > endTime)
	{
		// Swap start and end Markers
		struct positionMarker tempMarker;
		long long tempTime;

		CopyMemory(&tempMarker, &startPositionMarker, sizeof(struct positionMarker));
		CopyMemory(&startPositionMarker, &endPositionMarker, sizeof(struct positionMarker));
		CopyMemory(&endPositionMarker, &tempMarker, sizeof(struct positionMarker));

		tempTime = startTime;
		startTime = endTime;
		endTime = tempTime;
	}
	
	ChannelDataMapping *pChannelData = GetChannelDataMapping(startPositionMarker.channelNum);
	pChannelData->GetVideoHeader(&vHeader);
	if (startPositionMarker.indexNum < vHeader.noMonitors)
		startPositionMarker.indexNum = 0;
	if (endPositionMarker.indexNum < vHeader.noMonitors)
		endPositionMarker.indexNum = 0;

	if (startPositionMarker.fPosIndex >= 0L && endPositionMarker.fPosIndex >= 0L)
	{
		struct IndexHdr startIndex, endIndex;

		pDataFile->ReadFromFile((char *)&startIndex, sizeof(struct IndexHdr), startPositionMarker.fPosIndex);
		SplitIndex(startPositionMarker.fPosIndex, startPositionMarker.channelNum,&startIndex, &startTopIndexHdr, &startBottomIndexHdr, startPositionMarker.indexNum, bCopy, vHeader.noMonitors);
		startBottomIndexHdr.fPosPrevRecord = startTopIndexHdr.fPosPrevRecord = startIndex.fPosPrevRecord;
		startBottomIndexHdr.fPosNextRecord = startTopIndexHdr.fPosNextRecord = startIndex.fPosNextRecord;

		pDataFile->ReadFromFile((char *)&endIndex, sizeof(struct IndexHdr), endPositionMarker.fPosIndex);
		SplitIndex(endPositionMarker.fPosIndex, endPositionMarker.channelNum, &endIndex, &endTopIndexHdr, &endBottomIndexHdr, endPositionMarker.indexNum, bCopyEnd, vHeader.noMonitors);
		endBottomIndexHdr.fPosPrevRecord = endTopIndexHdr.fPosPrevRecord = endIndex.fPosPrevRecord;
		endBottomIndexHdr.fPosNextRecord = endTopIndexHdr.fPosNextRecord = endIndex.fPosNextRecord;

		if (startPositionMarker.fPosIndex == endPositionMarker.fPosIndex)
		{
			startBottomIndexHdr.numRecs = endPositionMarker.indexNum - startPositionMarker.indexNum;
			endTopIndexHdr.numRecs = -1;
		}
		CreateSegmentEventListAndTime();
		return true;
	}
	return false;
}

bool FileContext::PrepareDestinationForOperation(VideoSegmentClass *pCopySegment)
{
	struct VideoHdr vHeader;
	ZeroMemory(&vHeader, sizeof(struct VideoHdr));

	ChannelDataMapping *pChannelData = GetChannelDataMapping(destPositionMarker.channelNum);
	if (pChannelData == NULL)
		return false;
	pChannelData->GetVideoHeader(&vHeader);
	if (destPositionMarker.indexNum < vHeader.noMonitors)
		destPositionMarker.indexNum = 0;

	struct IndexHdr startIndex;

	if (destPositionMarker.fPosIndex > 0L)
	{
		pDataFile->ReadFromFile((char *)&startIndex, sizeof(struct IndexHdr), destPositionMarker.fPosIndex);
		SplitIndex(destPositionMarker.fPosIndex, destPositionMarker.channelNum, &startIndex, &destTopIndexHdr, &destBottomIndexHdr, destPositionMarker.indexNum, true, vHeader.noMonitors);
		destBottomIndexHdr.fPosPrevRecord = destTopIndexHdr.fPosPrevRecord = startIndex.fPosPrevRecord;
		destBottomIndexHdr.fPosNextRecord = destTopIndexHdr.fPosNextRecord = startIndex.fPosNextRecord;
	}
	else
	{
		ZeroMemory(&destTopIndexHdr, sizeof(struct IndexHdr));
		ZeroMemory(&destBottomIndexHdr, sizeof(struct IndexHdr));
		return false;
	}
	CreateInsertEventAndVideoList(NULL, pCopySegment);
	return true;
}

long long FileContext::GetMarkerTime(struct positionMarker *pMarker)
{
	// Calculates the absolute time for a position marker, steps followed are
	// 1. Find the indexPageDescriptor for the position marker fPOs
	// 2. starting from the start time for the page, add recDuration for each index pages till indexNum

	long long timeVal = 0L;

	map<long long, struct indexPageDescriptor *>::iterator itFind = mapVideoPages.find(pMarker->fPosIndex);

	if (itFind != mapVideoPages.end())
	{
		struct indexPageDescriptor *pDescriptor = itFind->second;

		if (pDescriptor->nPageType == 1)
		{
			struct videoIndexDescriptor *pVideo = pDescriptor->pageDescriptor.pVideo;
			timeVal = pVideo->startTime;

			for (int i = 0; i <= pMarker->indexNum; i++)
			{
				timeVal += pDescriptor->dataRecs[i].recDuration;
			}
		}
	}

	return timeVal;
}

void FileContext::PrepareVideoIndex(VideoSegmentClass *pCopySegment)
{
	FileContext *pSrcFileContext = pCopySegment->GetFileContext();
	struct IndexHdr startIndex;
	struct IndexHdr destStartIndex;
	struct IndexHdr destIndexFromCopySegment;

	ZeroMemory(&startIndex, sizeof(struct IndexHdr));
	ZeroMemory(&destStartIndex, sizeof(struct IndexHdr));
	ZeroMemory(&destIndexFromCopySegment, sizeof(struct IndexHdr));
	pCopySegment->GetSegmentTopIndexHeader(&startIndex);
	pCopySegment->GetTopIndexHeader(&destIndexFromCopySegment);
	ChannelDataMapping *pChannelData = pSrcFileContext->GetChannelDataMapping(pCopySegment->GetChannelNum());
	if (pChannelData == NULL)
		return;
	ChannelDataMapping *pDestChannelData = GetChannelDataMapping(mainChannelNumber);

	// create screen image records for each monitor and store in the file & bottom half
	int numMon = pSrcFileContext->GetNumMonitors(pCopySegment->GetChannelNum());
	int mouseX = 0;
	int mouseY = 0;

	int monNum = 0;
	for (int numVideo = 0; numVideo <= startIndex.numRecs; numVideo++)
	{
		if (pSrcFileContext->GetScreenVideoBuffer(&startIndex, &numVideo))
			break;
		pChannelData->ProcessMonitorScreens(&monNum);
	}

	ULONG compressedLen = 0;
	for (int i = 0; i < numMon; i++)
	{
		// create DataRec records, write in file and store in destBottom

		struct ScreenData *screen = pChannelData->GetScreenData(i);
		if (screen == NULL)
			return;

		BYTE *screenBuffer = NULL;
		struct CompareBuffer *pCompBuffer = NULL;
		screenBuffer = screen->lpScreen;
		struct bufferRecord *pRecBuffer;
		int screenWidth, screenHeight;
		screenWidth = screen->rcMonitor.right - screen->rcMonitor.left;
		screenHeight = screen->rcMonitor.bottom - screen->rcMonitor.top;

		pRecBuffer = (struct bufferRecord *)pRecBufferManager->GetFreeScreenBuffer(screen);
		pRecBuffer->timeStamp = startIndex.dataRecs[i].timestamp;
		pCompBuffer = (struct CompareBuffer *)pRecBuffer->buffer;
		pCompBuffer->screenNum = screen->hMonitor;
		pRecBuffer->sizeOfBuffer = screenHeight * screenWidth * 4L;
		pCompBuffer->numOfBlocks = -1;
		BYTE *compBufferFullScreen = (BYTE *)&pCompBuffer->blockData;
		//			BYTE *compBufferFullScreen = pCompBuffer->blockData[0].dataBuffer; New algo
		BYTE *monitorScreenBuffer = pChannelData->GetMonitorScreenBuffer(screen->hMonitor);
		pCompBuffer->numBytes = PackData((long *)compBufferFullScreen, (long *)monitorScreenBuffer, screenHeight, screenWidth);
		pRecBuffer->unBuffDets.scrRec.size = pRecBuffer->sizeOfBuffer;
		pRecBuffer->unBuffDets.scrRec.hMon = screen->hMonitor;
		pRecBuffer->recDurationus = startIndex.dataRecs[i].recDurationus;
		pRecBuffer->buffType = 1;
		pRecBuffer->eventIndex = 0;
		pRecBuffer->indexNum = i;
		pRecBuffer->fPosIndex = 0L;
		pRecBuffer->mouseX = mouseX;
		pRecBuffer->mouseY = mouseY;
	
		//WriteImageBMP(screenWidth, screenHeight, pRecBuffer->buffer);

		destStartIndex.dataRecs[i].RecType = 'P';
		destStartIndex.dataRecs[i].size = pRecBuffer->sizeOfBuffer;
		destStartIndex.dataRecs[i].screenNum = pRecBuffer->unBuffDets.scrRec.hMon;
		destStartIndex.dataRecs[i].fPosData = pDataFile->AppendDeflateBlockToFile((char *)pRecBuffer, pRecBuffer->sizeOfBuffer, &compressedLen);
		destStartIndex.dataRecs[i].compSize = compressedLen;

		pDestChannelData->GetChannelHeader()->lastIndexNumber++;
		destStartIndex.dataRecs[i].videoIndexNumber = destStartIndex.dataRecs[i].audioIndexNumber = pDestChannelData->GetChannelHeader()->lastIndexNumber;
		destStartIndex.dataRecs[i].eventIndexNumber = startIndex.dataRecs[startIndex.numRecs].eventIndexNumber;
		destStartIndex.dataRecs[i].timestamp = startIndex.dataRecs[startIndex.numRecs].timestamp + i + 1;
		destStartIndex.dataRecs[i].recStatus = 'A';	
		destStartIndex.dataRecs[i].recDurationus = 1L;
		destStartIndex.recDurationus += 1L;
	}
	for (int i = 0; i <= destIndexFromCopySegment.numRecs; i++)
	{
		CopyMemory(&destStartIndex.dataRecs[i + numMon], &destIndexFromCopySegment.dataRecs[i], sizeof(struct dataHdr));
		destStartIndex.recDurationus += destIndexFromCopySegment.dataRecs[i].recDurationus;
	}
	destStartIndex.bFullImageIndex = destIndexFromCopySegment.bFullImageIndex;
	destStartIndex.fPosNextRecord = destIndexFromCopySegment.fPosNextRecord;
	destStartIndex.fPosPrevRecord = destIndexFromCopySegment.fPosPrevRecord;
	destStartIndex.numRecs = destIndexFromCopySegment.numRecs + numMon;
	destStartIndex.RecType = destIndexFromCopySegment.RecType;
	destStartIndex.timestamp = destIndexFromCopySegment.timestamp;
	pCopySegment->SetTopIndexHeader(&destStartIndex);
}

int FileContext::GetNumMonitors(int chNum)
{
	ChannelDataMapping *pChannelData = GetChannelDataMapping(chNum);
	if (pChannelData == NULL)
		return(0);
	return pChannelData->GetNumMonitors();
}

int FileContext::GetMaxNumberOfMonitors()
{
	map <USHORT, ChannelDataMapping *>::iterator itChannelMap = channelDataMap.begin();
	int maxMonitors = 0;
	while (itChannelMap != channelDataMap.end())
	{
		ChannelDataMapping *pChannelData = itChannelMap->second;
		if (pChannelData != NULL)
		{
			int num = pChannelData->GetNumMonitors();
			if (num > maxMonitors)
				maxMonitors = num;
		}
		itChannelMap++;
	}
	return maxMonitors;
}


void FileContext::SplitIndex(long long fPos, int channelNum, struct IndexHdr *source, struct IndexHdr *destTop, struct IndexHdr *destBottom, int indexNum, bool bCreateScreens, int numMon)
{
	ZeroMemory(destTop, sizeof(struct IndexHdr));
	ZeroMemory(destBottom, sizeof(struct IndexHdr));

	ChannelDataMapping *pChannelData = GetChannelDataMapping(channelNum);
	if (pChannelData == NULL)
		return;

	destTop->RecType = source->RecType;
	destTop->timestamp = source->timestamp;
	destTop->numRecs = indexNum - 1;
	destTop->recDurationus = 0L;

	for (int i = 0; i <= destTop->numRecs; i++)
	{
		memcpy(&destTop->dataRecs[i], &source->dataRecs[i], sizeof(struct dataHdr));
		destTop->recDurationus += source->dataRecs[i].recDurationus;
	}

	destBottom->RecType = source->RecType;
	destBottom->timestamp = source->timestamp + destTop->recDurationus;
	destBottom->numRecs = source->numRecs - destTop->numRecs - 1;
	destBottom->recDurationus = 0L;
	int numHalf = source->numRecs - destTop->numRecs - 1;
	int numScreenIndex = 0;
	if (bCreateScreens && indexNum != 0 && numHalf >= 0)
	{
		destBottom->numRecs += numMon;
		// create screen image records for each monitor and store in the file & bottom half
		numScreenIndex = numMon;
		int mouseX = 0;
		int mouseY = 0;

		int monNum = 0;
		for (int numVideo = 0; numVideo <= destTop->numRecs; numVideo++)
		{
			if (GetScreenVideoBuffer(source, &numVideo))
				break;
			pChannelData->ProcessMonitorScreens(&monNum);
		}
		ULONG compressedLen = 0;
		for (int i = 0; i < numMon; i++)
		{
			// create DataRec records, write in file and store in destBottom

			struct ScreenData *screen = pChannelData->GetScreenData(i);
			if (screen == NULL)
				return;

			BYTE *screenBuffer = NULL;
			struct CompareBuffer *pCompBuffer = NULL;
			screenBuffer = screen->lpScreen;
			struct bufferRecord *pRecBuffer;
			int screenWidth, screenHeight;
			screenWidth = screen->rcMonitor.right - screen->rcMonitor.left;
			screenHeight = screen->rcMonitor.bottom - screen->rcMonitor.top;

			pRecBuffer = (struct bufferRecord *)pRecBufferManager->GetFreeScreenBuffer(screen);
			pRecBuffer->timeStamp = source->dataRecs[i].timestamp;
			pCompBuffer = (struct CompareBuffer *)pRecBuffer->buffer;
			pCompBuffer->screenNum = screen->hMonitor;
			pRecBuffer->sizeOfBuffer = screenHeight * screenWidth * 4L;
			pCompBuffer->numOfBlocks = -1;
			BYTE *compBufferFullScreen = (BYTE *)&pCompBuffer->blockData;
			BYTE *monitorScreenBuffer = pChannelData->GetMonitorScreenBuffer(screen->hMonitor);
			pCompBuffer->numBytes = PackData((long *)compBufferFullScreen, (long *)monitorScreenBuffer, screenHeight, screenWidth);
			pRecBuffer->unBuffDets.scrRec.size = pRecBuffer->sizeOfBuffer;
			pRecBuffer->unBuffDets.scrRec.hMon = screen->hMonitor;
			pRecBuffer->recDurationus = 1L;
			pRecBuffer->buffType = 1;
			pRecBuffer->eventIndex = 0;
			pRecBuffer->indexNum = i;
			pRecBuffer->fPosIndex = 0L;
			pRecBuffer->mouseX = mouseX;
			pRecBuffer->mouseY = mouseY;
			//WriteImageBMP(screenWidth, screenHeight, pRecBuffer->buffer);

			destBottom->dataRecs[i].RecType = 'P';
			destBottom->dataRecs[i].size = pRecBuffer->sizeOfBuffer;
			destBottom->dataRecs[i].screenNum = pRecBuffer->unBuffDets.scrRec.hMon;
			destBottom->dataRecs[i].fPosData = pDataFile->AppendDeflateBlockToFile((char *)pRecBuffer, pRecBuffer->sizeOfBuffer, &compressedLen);
			destBottom->dataRecs[i].compSize = compressedLen;
			destBottom->dataRecs[i].videoIndexNumber = source->dataRecs[destTop->numRecs].videoIndexNumber;;
			destBottom->dataRecs[i].audioIndexNumber = source->dataRecs[destTop->numRecs].audioIndexNumber;;
			destBottom->dataRecs[i].eventIndexNumber = source->dataRecs[destTop->numRecs].eventIndexNumber;
			destBottom->dataRecs[i].timestamp = source->dataRecs[destTop->numRecs].timestamp + i + 1;
			destBottom->dataRecs[i].recStatus = 'A';
			destBottom->dataRecs[i].recDurationus = 1L;
			pRecBufferManager->FreeBuffer(pRecBuffer);
		}
	}
	for (int i = 0; i <= numHalf; i++)
	{
		CopyMemory(&destBottom->dataRecs[i + numScreenIndex], &source->dataRecs[i + destTop->numRecs + 1], sizeof(struct dataHdr));
		destBottom->recDurationus = source->dataRecs[i + destTop->numRecs + 1].recDurationus;
	}
}

void FileContext::WriteImageBMP(int width, int height, char *pBuffer)
{
	static int numImageBmp = 0;
	BITMAPINFO bmi;

	int bpp = 32;

	/* Create a DIB and select it into the dest_hdc */
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = width;
	bmi.bmiHeader.biHeight = -height;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = bpp;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = 0;
	bmi.bmiHeader.biXPelsPerMeter = 0;
	bmi.bmiHeader.biYPelsPerMeter = 0;
	bmi.bmiHeader.biClrUsed = 0;
	bmi.bmiHeader.biClrImportant = 0;

	// Part 2
	int headerSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) +
		(bpp <= 8 ? (1 << bpp) : 0) * sizeof(RGBQUAD) /* palette size */;
	int frameSize = width * height * 4;

	BITMAPFILEHEADER bfh;
	int file_size = headerSize + frameSize;

	bfh.bfType = 0x4d42; /* "BM" in little-endian */
	bfh.bfSize = file_size;
	bfh.bfReserved1 = 0;
	bfh.bfReserved2 = 0;
	bfh.bfOffBits = headerSize;

	// Part 3

	char fileName[100];

	sprintf_s(fileName, "c:\\Recorder\\ScreenBitmaps\\Screen-%06d.bmp", numImageBmp);
	wchar_t wcstring[100];
	numImageBmp++;

	//// Convert char* string to a wchar_t* string.  
	size_t convertedChars = 0;
	mbstowcs_s(&convertedChars, wcstring, strlen(fileName) + 1, fileName, _TRUNCATE);

	// A file is created, this is where we will save the screen capture.
	HANDLE hFile = CreateFile(wcstring,
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);

	// Add the size of the headers to the size of the bitmap to get the total file size
	//DWORD dwSizeofDIB = file_size;

	DWORD dwBytesWritten = 0;
	WriteFile(hFile, (LPSTR)&bfh, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (LPSTR)&bmi.bmiHeader, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (LPSTR)pBuffer, frameSize, &dwBytesWritten, NULL);
	//Close the handle for the file that was created
	CloseHandle(hFile);
}

long long FileContext::CreateSegmentEventListAndTime()
{
	long long indexTime = 0L;
	segmentEvents.clear();
	struct IndexHdr indexHdr;
	// keep track of all channel and branch switches
	int channelNum = startPositionMarker.channelNum;
	long long fPosNextIndex = startBottomIndexHdr.fPosNextRecord;
	indexTime += GetEventsAndTimeInSegment(channelNum, &startBottomIndexHdr);
	if (fPosNextIndex != NULL && startPositionMarker.fPosIndex != endPositionMarker.fPosIndex)
	{
		pDataFile->ReadFromFile((char *)&indexHdr, sizeof(struct IndexHdr), fPosNextIndex);
		while (indexHdr.fPosNextRecord != NULL && indexHdr.fPosNextRecord != endTopIndexHdr.fPosNextRecord && indexHdr.fPosPrevRecord != endTopIndexHdr.fPosPrevRecord)
		{
			map<long long, struct indexPageDescriptor *>::iterator itMapDescriptor = mapVideoPages.find(fPosNextIndex);
			if (itMapDescriptor == mapVideoPages.end())
				break;
			struct indexPageDescriptor *pIndexDescriptor = itMapDescriptor->second;
			if (indexHdr.RecType == 'P') // a video index record
			{
				indexTime += GetEventsAndTimeInSegment(channelNum, &indexHdr);
				fPosNextIndex = indexHdr.fPosNextRecord;
				pDataFile->ReadFromFile((char *)&indexHdr, sizeof(struct IndexHdr), fPosNextIndex);
			}
			else if (indexHdr.RecType == 'H') // channel Header
			{
				channelNum = indexHdr.dataRecs[0].size;
				fPosNextIndex = indexHdr.fPosNextRecord;
				pDataFile->ReadFromFile((char *)&indexHdr, sizeof(struct IndexHdr), fPosNextIndex);
			}
			else if (indexHdr.RecType == 'B') // Branch
			{
				struct branchIndexDescriptor *pBranch = pIndexDescriptor->pageDescriptor.pBranch;
				if (pBranch->bPathDir)
					fPosNextIndex = pBranch->nextRightFPos;
				else
					fPosNextIndex = pBranch->nextBottomFPos;
				pDataFile->ReadFromFile((char *)&indexHdr, sizeof(struct IndexHdr), fPosNextIndex);
			}
			else if (indexHdr.RecType == 'J') // Join
			{
				fPosNextIndex = indexHdr.fPosNextRecord;
				pDataFile->ReadFromFile((char *)&indexHdr, sizeof(struct IndexHdr), fPosNextIndex);
			}

		}
	}
	indexTime += GetEventsAndTimeInSegment(channelNum, &endTopIndexHdr);
	return indexTime;
}

long long FileContext::GetEventsAndTimeInSegment(int channelNum, struct IndexHdr *pIndexHdr)
{
	list <BPMNElement *> *pEventList = pBPMNFacade->GetBPMNDiagram();
	long long indexTime = 0L;
	for (int i = 0; i <= pIndexHdr->numRecs; i++)
	{
		ULONG eventIndex = pIndexHdr->dataRecs[i].videoIndexNumber;
		indexTime += pIndexHdr->dataRecs[i].recDurationus;
		BPMNElement *pEventElement = FindEventInList(pEventList, eventIndex);
		if (pEventElement != NULL)
			segmentEvents.push_back(pEventElement);
	}
	return indexTime;
}

BPMNElement *FileContext::FindEventInList(list <BPMNElement *> *pElementList, ULONG eventIndex)
{
	struct eventListElement *pEventElement = NULL;
	map <ULONG, struct eventListElement *>::iterator itEventFind = pEventMap->find(eventIndex);
	if (itEventFind == pEventMap->end())
		return NULL;
	pEventElement = itEventFind->second;
	return pEventElement->pElement;
}

void FileContext::DeleteEventsInList(list <BPMNElement *> *pElementList)
{
	// The memory for the deleted item is not released and still retained in the map, so that it can be deleted at the end
	// remove from file
	list <BPMNElement *>::iterator itSegEvents = pElementList->begin();
	//list <BPMNElement *> *listEvents = pBPMNFacade->GetBPMNDagrami();
	while (itSegEvents != pElementList->end())
	{
		BPMNElement *pElementFirst = *itSegEvents;
		itSegEvents++;
		map <ULONG, struct eventListElement *>::iterator itEvent = pEventMap->find(pElementFirst->GetEventIndex());
		if (itEvent == pEventMap->end())
			continue;
		struct eventListElement *pEventListElement = itEvent->second;
		struct eventListElement *pEventListPrevElement = pEventListElement->prevElement;
		struct eventListElement *pEventListNextElement = pEventListElement->nextElement;

		BPMNElement *pRootParent = pEventListElement->pElement->GetParentElement();
		if (pEventListPrevElement == NULL && pRootParent == NULL)
		{
			//		Write channel header or previous element record (only pointers)
			SetEventRoot(pEventListNextElement);
			pEventListNextElement->prevElement = NULL;
			UpdateEventRecord(pEventListNextElement, NULL);
		}
		else
		{
			if (pEventListPrevElement != NULL)
			{
				pEventListPrevElement->nextElement = pEventListNextElement;
				UpdateEventRecord(pEventListPrevElement, NULL);
			}
			if (pEventListNextElement != NULL)
			{
				pEventListNextElement->prevElement = pEventListPrevElement;
				UpdateEventRecord(pEventListNextElement, NULL);
			}
		}

		UpdateSourceAndDestinationParentRecords(pRootParent, NULL);
	}

	RemoveEventsInList(pElementList);  // Remove from Diagram
	DeleteEventsFromEventList(pElementList);		// Remove from event list
}

void FileContext::DeleteEventsFromEventList(list <BPMNElement *> *pElementList)
{
	// The memory for the deleted item is not released and still retained in the map, so that it can be deleted at the end
	// remove from file
	list <BPMNElement *>::iterator itSegEvents = pElementList->begin();
	while (itSegEvents != pElementList->end())
	{
		BPMNElement *pElementFirst = *itSegEvents;
		itSegEvents++;
		if (pElementFirst->GetType() == 4)
		{
			BPMNDecision *pDecision = (BPMNDecision *)pElementFirst;
			DeleteEventsFromEventList(pDecision->GetBranchRightElements());
		}

		map <ULONG, struct eventListElement *>::iterator itEvent = pEventMap->find(pElementFirst->GetEventIndex());
		if (itEvent == pEventMap->end())
			continue;
		struct eventListElement *pEventListElement = itEvent->second;
		pEventMap->erase(pElementFirst->GetEventIndex());
		delete pEventListElement->pElement;
		delete pEventListElement;
	}
}

void FileContext::ReplaceEventsInList(BPMNElement *pOldElement, BPMNElement *pNewElement)
{
	list <BPMNElement *> *listEvents = pBPMNFacade->GetBPMNDiagram();
	listEvents = pBPMNFacade->GetElementList(pOldElement);
	if (listEvents != NULL)
	{
		list <BPMNElement *>::iterator itElement = find(listEvents->begin(), listEvents->end(), pOldElement);
		if (itElement != listEvents->end())
			*itElement = pNewElement;
	}
}


void FileContext::RemoveEventsInList(list <BPMNElement *> *pElementList)
{
	// This function only remove events from the list, but keeps it in the map, and file. This is for the bottom branch
	// events in a Branch operation
	// The memory for the deleted item is not released and still retained in the map, so that it can be deleted at the end
	list <BPMNElement *>::iterator itSegEvents = pElementList->begin();
	list <BPMNElement *> *listEvents = pBPMNFacade->GetBPMNDiagram();
	while (itSegEvents != pElementList->end())
	{
		BPMNElement *pElementFirst = *itSegEvents;
		itSegEvents++;
		listEvents = pBPMNFacade->GetElementList(pElementFirst);
		if (listEvents != NULL)
		{
			list <BPMNElement *>::iterator itElement = FindEventIteratorInList(&listEvents, pElementFirst->GetEventIndex(), true);
				//find(listEvents->begin(), listEvents->end(), pElementFirst);
			if (itElement != listEvents->end())
				listEvents->erase(itElement);
		}
	}
}


void FileContext::UpdateSourceAndDestinationParentRecords(BPMNElement *pSource, BPMNElement *pDest)
{
	map <ULONG, struct eventListElement *>::iterator itEvent = pEventMap->end();
	struct eventListElement *pEventListElement = NULL;

	// Need to update both source and Destination parents
	if (pSource != NULL)
	{
		itEvent = pEventMap->find(pSource->GetEventIndex());
		if (itEvent == pEventMap->end())
			return;
		pEventListElement = itEvent->second;
		UpdateEventRecord(pEventListElement, NULL);
	}
	if (pDest != NULL)
	{
		itEvent = pEventMap->find(pDest->GetEventIndex());
		if (itEvent == pEventMap->end())
			return;
		pEventListElement = itEvent->second;
		UpdateEventRecord(pEventListElement, NULL);
	}
}

void FileContext::DeleteVideoSegment()
{
	struct IndexHdr indexPage;
	struct VideoHdr vHeader;

	ZeroMemory(&vHeader, sizeof(struct VideoHdr));

	ChannelDataMapping *pChannelData = GetChannelDataMapping(startPositionMarker.channelNum);
	pChannelData->GetVideoHeader(&vHeader);
	int numMon = vHeader.noMonitors;
	if (startTopIndexHdr.numRecs <= numMon)
	{
		// If Cut operation starts at top of Index
		// At top of index page
		if (startPositionMarker.fPosIndex > 0)
		{
			// Read the start index page
			pDataFile->ReadFromFile((char *)&indexPage, sizeof(struct IndexHdr), startPositionMarker.fPosIndex);
			if (endBottomIndexHdr.numRecs > numMon)
			{
				// If endBottomIndex Hdr contains data then link to startIndex record previous record
				endBottomIndexHdr.fPosPrevRecord = indexPage.fPosPrevRecord;
				pDataFile->WriteToFile(startPositionMarker.fPosIndex, (char *)&endBottomIndexHdr, (ULONG)sizeof(struct IndexHdr));
				UpdateIndexPageDescriptor(startPositionMarker.fPosIndex, &endBottomIndexHdr);
			}
			else
			{
				// the entire Index Hdr record has been cut
				pDataFile->ReadFromFile((char *)&indexPage, sizeof(struct IndexHdr), endPositionMarker.fPosIndex);
				if (indexPage.fPosNextRecord != NULL)
				{
					// Set the Next records prevous record to point to previous record of Start
					long long fPosNext = indexPage.fPosNextRecord;
					pDataFile->ReadFromFile((char *)&indexPage, sizeof(struct IndexHdr), fPosNext);
					indexPage.fPosPrevRecord = startTopIndexHdr.fPosPrevRecord;
					pDataFile->WriteToFile(fPosNext, (char *)&indexPage, (ULONG)sizeof(struct IndexHdr));
					UpdateIndexPageDescriptor(fPosNext, &indexPage);
					long long fPosPrev = startTopIndexHdr.fPosPrevRecord;
					if (fPosPrev != NULL)
					{
						// If previous Record exists then point previous record to next record
						pDataFile->ReadFromFile((char *)&indexPage, sizeof(struct IndexHdr), fPosPrev);
						indexPage.fPosNextRecord = fPosNext;
						pDataFile->WriteToFile(fPosPrev, (char *)&indexPage, (ULONG)sizeof(struct IndexHdr));
						UpdateIndexPageDescriptor(fPosPrev, &indexPage);
					}
					else
					{
						// If no previous record exists then get root to point to next record;
						vHeader.fPosRoot = fPosNext;
						pChannelData->WriteVideoHeader(&vHeader);
					}
				}
				else
				{
					// Here we need to update the next record pointer of the record prior to Start to NULL
					long long fPosNext = startTopIndexHdr.fPosPrevRecord;
					if (fPosNext != NULL)
					{
						// Go to the previous record and set next record to NULL
						pDataFile->ReadFromFile((char *)&indexPage, sizeof(struct IndexHdr), fPosNext);
						indexPage.fPosNextRecord = NULL;
						pDataFile->WriteToFile(fPosNext, (char *)&indexPage, (ULONG)sizeof(struct IndexHdr));
						UpdateIndexPageDescriptor(fPosNext, &indexPage);
					}
					else
					{
						// All records are deleted, Highly Unlikely - set Video Header root to NULL
						// Update Video Header
						vHeader.fPosRoot = NULL;
						pChannelData->WriteVideoHeader(&vHeader);
					}
				}

			}
		}
	}
	else if (startPositionMarker.fPosIndex == endPositionMarker.fPosIndex)
	{
		// both indexes are in the same index record
		if (endBottomIndexHdr.numRecs > numMon)
		{
			// if end bottom index header contains data
			pDataFile->ReadFromFile((char *)&indexPage, sizeof(struct IndexHdr), startPositionMarker.fPosIndex);
			endBottomIndexHdr.fPosPrevRecord = startPositionMarker.fPosIndex;
			endBottomIndexHdr.fPosNextRecord = indexPage.fPosNextRecord;
			startTopIndexHdr.fPosNextRecord = InsertIndexRecord(&startPositionMarker.channelNum, &endBottomIndexHdr, &startTopIndexHdr, startPositionMarker.fPosIndex, numMon, this);
			//startTopIndexHdr.fPosNextRecord = pDataFile->AppendToFile((char *)&endBottomIndexHdr, (ULONG)sizeof(struct IndexHdr));
			startTopIndexHdr.fPosPrevRecord = indexPage.fPosPrevRecord;
			pDataFile->WriteToFile(startPositionMarker.fPosIndex, (char *)&startTopIndexHdr, (ULONG)sizeof(struct IndexHdr));
			UpdateIndexPageDescriptor(startPositionMarker.fPosIndex, &startTopIndexHdr);
			if (endBottomIndexHdr.fPosNextRecord != NULL)
			{
				// Update the end Bottom index record to point to start top index record
				pDataFile->ReadFromFile((char *)&indexPage, sizeof(struct IndexHdr), endBottomIndexHdr.fPosNextRecord);
				indexPage.fPosPrevRecord = startTopIndexHdr.fPosNextRecord;
				pDataFile->WriteToFile(endBottomIndexHdr.fPosNextRecord, (char *)&indexPage, sizeof(struct IndexHdr));
				UpdateIndexPageDescriptor(endBottomIndexHdr.fPosNextRecord, &indexPage);
			}
		}
		else
		{
			// If end bottom index header does not contain any data, the update start Top index to point to next record after end Position marker
			pDataFile->ReadFromFile((char *)&indexPage, sizeof(struct IndexHdr), startPositionMarker.fPosIndex);
			startTopIndexHdr.fPosPrevRecord = indexPage.fPosPrevRecord;
			pDataFile->ReadFromFile((char *)&indexPage, sizeof(struct IndexHdr), endPositionMarker.fPosIndex);
			startTopIndexHdr.fPosNextRecord = indexPage.fPosNextRecord;
			pDataFile->WriteToFile(startPositionMarker.fPosIndex, (char *)&startTopIndexHdr, (ULONG)sizeof(struct IndexHdr));
			UpdateIndexPageDescriptor(startPositionMarker.fPosIndex, &startTopIndexHdr);
		}
	}
	else
	{
		// segment spans multiple Index Pages
		pDataFile->ReadFromFile((char *)&indexPage, sizeof(struct IndexHdr), startPositionMarker.fPosIndex);
		startTopIndexHdr.fPosPrevRecord = indexPage.fPosPrevRecord;
		if (endBottomIndexHdr.numRecs > numMon)
			startTopIndexHdr.fPosNextRecord = endPositionMarker.fPosIndex;
		else
		{
			pDataFile->ReadFromFile((char *)&indexPage, sizeof(struct IndexHdr), endPositionMarker.fPosIndex);
			startTopIndexHdr.fPosNextRecord = indexPage.fPosNextRecord;
		}
		pDataFile->WriteToFile(startPositionMarker.fPosIndex, (char *)&startTopIndexHdr, (ULONG)sizeof(struct IndexHdr));
		UpdateIndexPageDescriptor(startPositionMarker.fPosIndex, &startTopIndexHdr);

		if (endBottomIndexHdr.numRecs > numMon)
		{
			pDataFile->ReadFromFile((char *)&indexPage, sizeof(struct IndexHdr), endPositionMarker.fPosIndex);
			endBottomIndexHdr.fPosPrevRecord = startPositionMarker.fPosIndex;
			endBottomIndexHdr.fPosNextRecord = indexPage.fPosNextRecord;
			pDataFile->WriteToFile(endPositionMarker.fPosIndex, (char *)&endBottomIndexHdr, (ULONG)sizeof(struct IndexHdr));
			UpdateIndexPageDescriptor(endPositionMarker.fPosIndex, &endBottomIndexHdr);
		}
	}
}

void FileContext::SetEventMap(map <ULONG, struct eventListElement *> *pMap)
{
	pEventMap = pMap;
}

long long FileContext::CreateInsertEventAndVideoList(long long *fPosBranchInsPoint, VideoSegmentClass *pCopySegment)
{
	struct IndexHdr indexHdr;
	struct IndexHdr indexNewHdr, indexOldHdr;
	long long fPosPrev = 0L;
	short channelNum = destPositionMarker.channelNum;
	insertSegmentEvents.clear();
	parentIndexMapping.clear();
	struct IndexHdr channelIndexHdr; 
	long long fPosNextIndex = copyTopIndexHdr.fPosNextRecord;
	ChannelDataMapping *pChannelData = GetChannelDataMapping(channelNum);  // changed on 12/28 from mainChannelNumber
	if (pChannelData == NULL)
		return 0L;
	FileContext *pSrcContext = pCopySegment->GetFileContext();
	ChannelDataMapping *pSrcChannelData = pSrcContext->GetChannelDataMapping(pCopySegment->GetChannelNum());
	int nChannelNum = -1;
	bool bChannelAdded = false;
	if (pSrcChannelData->GetChannelKey() != pChannelData->GetChannelKey())
	{
		nChannelNum = FindChannelInVideo(pSrcChannelData->GetChannelKey());
		if (nChannelNum != destPositionMarker.channelNum)
		{
			bChannelAdded = true;
			// Add a channel in the destination file along with monitor details
			if (nChannelNum == -1)
				nChannelNum = AddNewChannel(pSrcChannelData);
			// Insert a channel context Index record;
			ZeroMemory(&channelIndexHdr, sizeof(struct IndexHdr));
			channelIndexHdr.RecType = 'H';
			channelIndexHdr.numRecs = 0;
			channelIndexHdr.recDurationus = 0L;
			channelIndexHdr.timestamp = copyTopIndexHdr.timestamp;
			pChannelData->GetChannelHeader()->lastIndexNumber++;
			channelIndexHdr.dataRecs[0].videoIndexNumber = pChannelData->GetChannelHeader()->lastIndexNumber;
			channelIndexHdr.dataRecs[0].size = nChannelNum;
			channelIndexHdr.dataRecs[0].compSize = destPositionMarker.channelNum;
			// Insert a Channel change after Start Top index
			fPosPrev = InsertIndexRecord(&channelNum, &channelIndexHdr, &destTopIndexHdr, destPositionMarker.fPosIndex, pSrcChannelData->GetNumMonitors(), pSrcContext);
			GetInsertEventsAndVideoInSegment(&copyTopIndexHdr, &indexNewHdr, pSrcContext);
			channelNum = nChannelNum;
			// followed by copy top index
			fPosPrev = InsertIndexRecord(&channelNum, &indexNewHdr, &channelIndexHdr, fPosPrev, pSrcChannelData->GetNumMonitors(), pSrcContext);
		}
		else
		{
			// Insert a copy top index after start Top
			GetInsertEventsAndVideoInSegment(&copyTopIndexHdr, &indexNewHdr, pSrcContext);
			// This function re-writes the destTopIndexHdr, and appends the indexNewHdr
			fPosPrev = InsertIndexRecord(&channelNum, &indexNewHdr, &destTopIndexHdr, destPositionMarker.fPosIndex, pSrcChannelData->GetNumMonitors(), pSrcContext);
		}
		channelNum = nChannelNum;
	}
	else
	{
		// Insert a copy top index after start Top
		GetInsertEventsAndVideoInSegment(&copyTopIndexHdr, &indexNewHdr, pSrcContext);
		// This function re-writes the destTopIndexHdr, and appends the indexNewHdr
		fPosPrev = InsertIndexRecord(&channelNum, &indexNewHdr, &destTopIndexHdr, destPositionMarker.fPosIndex, pSrcChannelData->GetNumMonitors(), pSrcContext);
	}
	// Write remaining index records in the copy segment
	if (fPosNextIndex != NULL && copyTopIndexHdr.fPosNextRecord != copyBottomIndexHdr.fPosNextRecord)
	{
		pSrcContext->ReadFromFile((char *)&indexHdr, sizeof(struct IndexHdr), fPosNextIndex);
		while (indexHdr.fPosNextRecord != NULL && indexHdr.fPosNextRecord != copyBottomIndexHdr.fPosNextRecord && indexHdr.fPosPrevRecord != copyBottomIndexHdr.fPosPrevRecord)
		{
			CopyMemory(&indexOldHdr, &indexNewHdr, sizeof(struct IndexHdr));
			if (indexHdr.RecType == 'P' || indexHdr.RecType == 'B')
				GetInsertEventsAndVideoInSegment(&indexHdr, &indexNewHdr, pSrcContext);
			else
				CopyMemory(&indexNewHdr, &indexHdr, sizeof(struct IndexHdr));
			fPosPrev = InsertIndexRecord(&channelNum, &indexNewHdr, &indexOldHdr, fPosPrev, pSrcChannelData->GetNumMonitors(), pSrcContext);
			if (indexNewHdr.RecType == 'J')
			{
				fPosNextIndex = indexOldHdr.fPosNextRecord;
			}
			else
			{
				fPosNextIndex = indexHdr.fPosNextRecord;
			}
			if (fPosNextIndex != NULL)
				pSrcContext->ReadFromFile((char *)&indexHdr, sizeof(struct IndexHdr), fPosNextIndex);
		}
	}
	CopyMemory(&indexOldHdr, &indexNewHdr, sizeof(struct IndexHdr));
	struct VideoHdr vHeader;

	ZeroMemory(&vHeader, sizeof(struct VideoHdr));
	pChannelData->GetVideoHeader(&vHeader);
	int numMon = vHeader.noMonitors;

	if (copyBottomIndexHdr.numRecs > numMon)
	{
		// Write the copy Bottom index
		GetInsertEventsAndVideoInSegment(&copyBottomIndexHdr, &indexNewHdr, pSrcContext);
		fPosPrev = InsertIndexRecord(&channelNum, &indexNewHdr, &indexOldHdr, fPosPrev, numMon, pSrcContext);
		CopyMemory(&indexOldHdr, &indexNewHdr, sizeof(struct IndexHdr));
	}

	// Write a channel change if required
	if (bChannelAdded)
	{
		// Insert a channel context Index record;
		ZeroMemory(&channelIndexHdr, sizeof(struct IndexHdr));
		channelIndexHdr.RecType = 'H';
		channelIndexHdr.numRecs = 0;
		channelIndexHdr.recDurationus = 0L;
		channelIndexHdr.timestamp = startTopIndexHdr.timestamp;
		channelNum = nChannelNum;
		pChannelData->GetChannelHeader()->lastIndexNumber++;
		channelIndexHdr.dataRecs[0].videoIndexNumber = pChannelData->GetChannelHeader()->lastIndexNumber;
		channelIndexHdr.dataRecs[0].size = destPositionMarker.channelNum; //pCopySegment->GetChannelNum();
		channelIndexHdr.dataRecs[0].compSize = nChannelNum;

		fPosPrev = InsertIndexRecord(&channelNum, &channelIndexHdr, &indexOldHdr, fPosPrev, numMon, pSrcContext);
		CopyMemory(&indexOldHdr, &channelIndexHdr, sizeof(struct IndexHdr));
	}
	if (fPosBranchInsPoint != NULL)
		*fPosBranchInsPoint = fPosPrev;
	// Write the start bottom index record.
	if (destTopIndexHdr.numRecs <= numMon)
	{
		struct IndexHdr lastRec;
		// This means that the video has been inserted, and the next record does not need to be inserted only the
		// record pointers to be setup
		pDataFile->ReadFromFile((char *)&lastRec, sizeof(struct IndexHdr), indexOldHdr.fPosNextRecord);
		lastRec.fPosPrevRecord = fPosPrev;
		pDataFile->WriteToFile(indexOldHdr.fPosNextRecord, (char *)&lastRec, sizeof(struct IndexHdr));
		UpdateIndexPageDescriptor(indexOldHdr.fPosNextRecord, &lastRec);
	}
	else if (destBottomIndexHdr.numRecs > numMon)
	{
		struct IndexHdr lastRec;

		fPosPrev = InsertIndexRecord(&channelNum, &destBottomIndexHdr, &indexOldHdr, fPosPrev, numMon, this);
		UpdateIndexPageDescriptor(fPosPrev, &destBottomIndexHdr);
		pDataFile->ReadFromFile((char *)&lastRec, sizeof(struct IndexHdr), destBottomIndexHdr.fPosNextRecord);
		lastRec.fPosPrevRecord = fPosPrev;
		pDataFile->WriteToFile(destBottomIndexHdr.fPosNextRecord, (char *)&lastRec,sizeof(struct IndexHdr));
		UpdateIndexPageDescriptor(destBottomIndexHdr.fPosNextRecord, &lastRec);
	}
	else
	{
		indexOldHdr.fPosNextRecord = destBottomIndexHdr.fPosNextRecord;
		pDataFile->WriteToFile(fPosPrev, (char *)&indexOldHdr, sizeof(struct IndexHdr));
		UpdateIndexPageDescriptor(fPosPrev, &indexOldHdr);
		if (destBottomIndexHdr.fPosNextRecord != NULL)
		{
			struct IndexHdr lastRec;

			pDataFile->ReadFromFile((char *)&lastRec, sizeof(struct IndexHdr), destBottomIndexHdr.fPosNextRecord);
			lastRec.fPosPrevRecord = fPosPrev;
			pDataFile->WriteToFile(destBottomIndexHdr.fPosNextRecord, (char *)&lastRec, sizeof(struct IndexHdr));
			UpdateIndexPageDescriptor(destBottomIndexHdr.fPosNextRecord, &lastRec);
		}
	}

	// Write Channel Header Information
	pDataFile->ReadFromFile((char *)&rootRecord, sizeof(struct rootHdr), 0L);
	long long channelFpos = rootRecord.posChannelRecords;
	pChannelData->WriteChannelHeader(channelFpos);
	return fPosPrev;
}

int FileContext::FindChannelInVideo(wxString strChannelKey)
{
	int newChannelNum = -1;

	map<USHORT, ChannelDataMapping *>::iterator itFind = channelDataMap.begin();
	while (itFind != channelDataMap.end())
	{
		ChannelDataMapping *channel = itFind->second;

		if (channel->GetChannelKey() == strChannelKey)
			break;

		itFind++;
	}

	if (itFind != channelDataMap.end())
	{
		ChannelDataMapping *channel = itFind->second;
		struct channelHdr *pChannelHdr = channel->GetChannelHeader();
		newChannelNum = pChannelHdr->channelNum;
	}
	return newChannelNum;
}

struct indexPageDescriptor *FileContext::GetIndexDescriptor(long long fPos)
{
	map<long long, struct indexPageDescriptor *>::iterator itMapDescriptor = mapVideoPages.find(fPos);
	if (itMapDescriptor == mapVideoPages.end())
		return NULL;
	return itMapDescriptor->second;
}

void FileContext::ReadFromFile(char *buffer, int size, long long fPos)
{
	pDataFile->ReadFromFile(buffer, size, fPos);
}

long long FileContext::GetVideoBufferSize()
{
	return maxVideoBufferSize;
}

long long FileContext::GetAudioBufferSize()
{
	return maxAudioBufferSize;
}


long long FileContext::InsertIndexRecord(short *channelNum, struct IndexHdr *pCurrIndex, struct IndexHdr *pPrevIndex, long long posPrevIndex, int numMon, FileContext *fContext)
{
	long long pos = 0L;
	if (pCurrIndex->RecType == 'B')
		pos = InsertBranchIndexRecord(channelNum, pCurrIndex, pPrevIndex, posPrevIndex, numMon, fContext);
	else if (pCurrIndex->RecType == 'H')
		pos = InsertChannelIndexRecord(channelNum, pCurrIndex, pPrevIndex, posPrevIndex, numMon, fContext);
	else if (pCurrIndex->RecType == 'P')
		pos =  InsertVideoIndexRecord(channelNum, pCurrIndex, pPrevIndex, posPrevIndex, numMon, fContext);

	return pos;
}

long long FileContext::InsertBranchIndexRecord(short *channelNum, struct IndexHdr *pCurrIndex, struct IndexHdr *pPrevIndex, long long posPrevIndex, int numMon, FileContext *fContext)
{
	long long posAdded = 0L;
	//bool bAddedToRoot = false;
	//struct IndexHdr lastIndexHdr;

	// Need to handle insert at top of channel
	pCurrIndex->fPosPrevRecord = posPrevIndex;
	// writing to the root record
	ChannelDataMapping *pChannelData = GetChannelDataMapping(*channelNum);
	if (pChannelData == NULL)
		return 0L;
	struct VideoHdr *pVHeader;
	pVHeader = pChannelData->GetVideoHeader();
	struct IndexHdr branchIndex;
	struct IndexHdr prevIndex;
	long long fPosRightBranchEnd = NULL;
	long long fPosBottomBranchEnd = NULL;
	long long fPosRightBranchStart = pCurrIndex->dataRecs[0].fPosData;
	long long fPosBottomBranchStart = pCurrIndex->fPosNextRecord;

	if (fContext != this)
	{
		;
	}

	if (posPrevIndex == NULL)
	{
		pVHeader->fPosRoot = pDataFile->AppendToFile((char *)pCurrIndex, (ULONG)sizeof(struct IndexHdr));
		pDataFile->WriteToFile(pChannelData->GetChannelHeader()->posVideoRoot, (char *)pVHeader, sizeof(struct VideoHdr));
		posAdded = pVHeader->fPosRoot;
	}
	else 
	{
		pCurrIndex->fPosNextRecord = pPrevIndex->fPosNextRecord;
		pPrevIndex->fPosNextRecord = pDataFile->AppendToFile((char *)pCurrIndex, (ULONG)sizeof(struct IndexHdr));
		posAdded = pPrevIndex->fPosNextRecord;
		pDataFile->WriteToFile(posPrevIndex, (char *)pPrevIndex, (ULONG)sizeof(struct IndexHdr));
	}
	
	long long fPosJoin = NULL;
	if (pCurrIndex->RecType == 'B')
	{
		CopyMemory(&branchIndex, pCurrIndex, sizeof(struct IndexHdr));
		CopyMemory(&prevIndex, pCurrIndex, sizeof(struct IndexHdr));  // pCurrIndex is copied to prevIndex, since pCurrIndex will be initialized to latest record written in next function
		branchIndex.dataRecs[0].fPosData = InsertRightOrBottomBranchIndexRecords(channelNum, pCurrIndex, &prevIndex, posAdded, numMon, fContext, fPosRightBranchStart, &fPosRightBranchEnd, &fPosJoin);
		CopyMemory(pCurrIndex, &branchIndex, sizeof(struct IndexHdr));
		CopyMemory(&prevIndex, &branchIndex, sizeof(struct IndexHdr));
		branchIndex.fPosNextRecord = InsertRightOrBottomBranchIndexRecords(channelNum, pCurrIndex, &prevIndex, posAdded, numMon, fContext, fPosBottomBranchStart, &fPosBottomBranchEnd, &fPosJoin);
	}

	pDataFile->WriteToFile(posAdded, (char *)&branchIndex, sizeof(struct IndexHdr));

	struct indexPageDescriptor *indexPageDesc = DBG_NEW struct indexPageDescriptor;
	ZeroMemory(indexPageDesc, sizeof(struct indexPageDescriptor));
	indexPageDesc->next = NULL;
	indexPageDesc->channelNum = *channelNum;
	map<long long, struct indexPageDescriptor *>::iterator itMapNextDescriptor = mapVideoPages.find(branchIndex.fPosNextRecord);
	if (itMapNextDescriptor != mapVideoPages.end())
	{
		indexPageDesc->next = itMapNextDescriptor->second;
	}
	indexPageDesc->nPageType = 2;
	indexPageDesc->pageDescriptor.pBranch = DBG_NEW struct branchIndexDescriptor;
	indexPageDesc->pageDescriptor.pBranch->bPathDir = false;
	indexPageDesc->pageDescriptor.pBranch->currFPos = posAdded;
	indexPageDesc->pageDescriptor.pBranch->branchChannelNum = *channelNum;
	indexPageDesc->pageDescriptor.pBranch->nextBottomFPos = branchIndex.fPosNextRecord;
	indexPageDesc->pageDescriptor.pBranch->nextRightFPos = branchIndex.dataRecs[0].fPosData;
	indexPageDesc->pageDescriptor.pBranch->prevFpos = posPrevIndex;
	indexPageDesc->pageDescriptor.pBranch->branchChannelNum = *channelNum;
	for (int i = 0; i <= 0; i++)
	{
		indexPageDesc->dataRecs[i].eventIndexNum = indexPageDesc->dataRecs[i].indexNum = pCurrIndex->dataRecs[i].videoIndexNumber;
		indexPageDesc->dataRecs[i].recDuration = (long)branchIndex.dataRecs[i].recDurationus;
		indexPageDesc->pageDescriptor.pVideo->timespan += branchIndex.dataRecs[i].recDurationus;
	}

	mapVideoPages.insert(pair<long long, struct indexPageDescriptor *>(posAdded, indexPageDesc));

	UpdatePreviousIndexHdr(posPrevIndex, posAdded, pPrevIndex, indexPageDesc);


	posPrevIndex = fPosBottomBranchEnd;
	pDataFile->ReadFromFile((char *)pPrevIndex, sizeof(struct IndexHdr), posPrevIndex);
	fContext->ReadFromFile((char *)pCurrIndex, sizeof(struct IndexHdr), fPosJoin);
	struct IndexHdr joinIndex;
	CopyMemory((char *)&joinIndex, pCurrIndex, sizeof(struct IndexHdr));

	joinIndex.dataRecs[0].fPosData = fPosRightBranchEnd;
	joinIndex.fPosPrevRecord = fPosBottomBranchEnd;
	joinIndex.fPosNextRecord = pPrevIndex->fPosNextRecord;
	posAdded = pDataFile->AppendToFile((char *)&joinIndex, sizeof(struct IndexHdr));
	struct IndexHdr bottomHdrs;

	pDataFile->ReadFromFile((char *)&bottomHdrs, sizeof(struct IndexHdr), fPosRightBranchEnd);
	bottomHdrs.fPosNextRecord = posAdded;
	pDataFile->WriteToFile(fPosRightBranchEnd, (char *)&bottomHdrs, sizeof(struct IndexHdr));
	UpdateIndexPageDescriptor(fPosRightBranchEnd, &bottomHdrs);

	pDataFile->ReadFromFile((char *)&bottomHdrs, sizeof(struct IndexHdr), fPosBottomBranchEnd);
	bottomHdrs.fPosNextRecord = posAdded;
	pDataFile->WriteToFile(fPosBottomBranchEnd, (char *)&bottomHdrs, sizeof(struct IndexHdr));
	UpdateIndexPageDescriptor(fPosBottomBranchEnd, &bottomHdrs);


	posPrevIndex = fPosBottomBranchEnd;
	CopyMemory(pPrevIndex, &bottomHdrs, sizeof(struct IndexHdr));

	indexPageDesc = DBG_NEW struct indexPageDescriptor;
	ZeroMemory(indexPageDesc, sizeof(struct indexPageDescriptor));
	indexPageDesc->next = NULL;
	indexPageDesc->channelNum = *channelNum;
	itMapNextDescriptor = mapVideoPages.find(joinIndex.fPosNextRecord);
	if (itMapNextDescriptor != mapVideoPages.end())
	{
		indexPageDesc->next = itMapNextDescriptor->second;
	}
	indexPageDesc->nPageType = 3;
	indexPageDesc->pageDescriptor.pJoin = DBG_NEW struct joinIndexDescriptor;
	indexPageDesc->pageDescriptor.pJoin->bPathDir = false;
	indexPageDesc->pageDescriptor.pJoin->currFPos = posAdded;
	indexPageDesc->pageDescriptor.pJoin->prevRightFpos = fPosRightBranchEnd;
	indexPageDesc->pageDescriptor.pJoin->prevTopFpos = fPosBottomBranchEnd;
	indexPageDesc->pageDescriptor.pJoin->nextFpos = joinIndex.fPosNextRecord;
	for (int i = 0; i <= 0; i++)
	{
		indexPageDesc->dataRecs[i].eventIndexNum = indexPageDesc->dataRecs[i].indexNum = joinIndex.dataRecs[i].videoIndexNumber;
		indexPageDesc->dataRecs[i].recDuration = (long)joinIndex.dataRecs[i].recDurationus;
		indexPageDesc->pageDescriptor.pVideo->timespan += joinIndex.dataRecs[i].recDurationus;
	}

	mapVideoPages.insert(pair<long long, struct indexPageDescriptor *>(posAdded, indexPageDesc));
	// Look up previous Index Hdr
	UpdatePreviousIndexHdr(posPrevIndex, posAdded, pPrevIndex, indexPageDesc);

	pDataFile->ReadFromFile((char *)pPrevIndex, sizeof(struct IndexHdr), fPosJoin);
	fContext->ReadFromFile((char *)pCurrIndex, sizeof(struct IndexHdr), posAdded);
	return posAdded; // pPrevIndex->fPosNextRecord;
}

long long FileContext::InsertRightOrBottomBranchIndexRecords(short *channelNum, struct IndexHdr *pCurrIndex, struct IndexHdr *pPrevIndex, long long posPrevIndex, int numMon, FileContext *fContext, long long fPosStart, long long *fPosEnd, long long *fPosJoin)
{
	long long fPosNextIndex = fPosStart;
	struct IndexHdr indexHdr;
	struct IndexHdr indexNewHdr, indexOldHdr;
	long long fPosPrev = posPrevIndex;
	ChannelDataMapping *pSrcChannelData = fContext->GetChannelDataMapping(*channelNum);
	long long fPosStartLeg = NULL;
	CopyMemory(&indexNewHdr, pPrevIndex, sizeof(struct IndexHdr));
	if (fPosNextIndex != NULL)
	{
		fContext->ReadFromFile((char *)&indexHdr, sizeof(struct IndexHdr), fPosNextIndex);
		while (indexHdr.fPosNextRecord != NULL && indexHdr.RecType != 'J')
		{
			CopyMemory(&indexOldHdr, &indexNewHdr, sizeof(struct IndexHdr));
			if (indexHdr.RecType == 'P' || indexHdr.RecType == 'B')
				GetInsertEventsAndVideoInSegment(&indexHdr, &indexNewHdr, fContext);
			else
				CopyMemory(&indexNewHdr, &indexHdr, sizeof(struct IndexHdr));
			fPosPrev = InsertIndexRecord(channelNum, &indexNewHdr, &indexOldHdr, fPosPrev, pSrcChannelData->GetNumMonitors(), fContext);
			if (indexNewHdr.RecType == 'J')
			{
				fPosNextIndex = indexNewHdr.fPosNextRecord;
			}
			else
			{
				fPosNextIndex = indexHdr.fPosNextRecord;
			}
			*fPosJoin = fPosNextIndex;
			*fPosEnd = fPosPrev;
			if (fPosStartLeg == NULL)
				fPosStartLeg = fPosPrev;
			CopyMemory(pCurrIndex, &indexNewHdr, sizeof(struct IndexHdr));
			CopyMemory(pPrevIndex, &indexOldHdr, sizeof(struct IndexHdr));
			fContext->ReadFromFile((char *)&indexHdr, sizeof(struct IndexHdr), fPosNextIndex);
		}
	}
	return fPosStartLeg;
}

long long FileContext::InsertChannelIndexRecord(short *channelNum, struct IndexHdr *pCurrIndex, struct IndexHdr *pPrevIndex, long long posPrevIndex, int numMon, FileContext *fContext)
{
	long long posAdded = 0L;
	//bool bAddedToRoot = false;

	// Need to handle insert at top of channel
	pCurrIndex->fPosPrevRecord = posPrevIndex;
	// writing to the root record
	ChannelDataMapping *pChannelData = GetChannelDataMapping(pCurrIndex->dataRecs[0].compSize);
	if (pChannelData == NULL)
		return 0L;
	struct VideoHdr *pVHeader;
	pVHeader = pChannelData->GetVideoHeader();

	//if (fContext != this)
	//{
	//	ChannelDataMapping *pSrcChannelData = fContext->GetChannelDataMapping(pCurrIndex->dataRecs[0].size);

	//	int nChannelNum = -1;
	//	nChannelNum = FindChannelInVideo(pSrcChannelData->GetChannelKey());
	//	if (nChannelNum == -1) // If channel is not found in main video
	//	{
	//		nChannelNum = AddNewChannel(pSrcChannelData);
	//	}
	//	pCurrIndex->dataRecs[0].size = nChannelNum;
	//	pCurrIndex->dataRecs[0].compSize = *channelNum;
	//	*channelNum = nChannelNum;
	//}
	if (posPrevIndex == NULL)
	{
		pVHeader->fPosRoot = pDataFile->AppendToFile((char *)pCurrIndex, (ULONG)sizeof(struct IndexHdr));
		pDataFile->WriteToFile(pChannelData->GetChannelHeader()->posVideoRoot, (char *)pVHeader, sizeof(struct VideoHdr));
		posAdded = pVHeader->fPosRoot;
	}
	else 
	{
		pCurrIndex->fPosNextRecord = pPrevIndex->fPosNextRecord;
		pPrevIndex->fPosNextRecord = pDataFile->AppendToFile((char *)pCurrIndex, (ULONG)sizeof(struct IndexHdr));
		posAdded = pPrevIndex->fPosNextRecord;
		pDataFile->WriteToFile(posPrevIndex, (char *)pPrevIndex, (ULONG)sizeof(struct IndexHdr));
	}
	
	struct indexPageDescriptor *indexPageDesc = DBG_NEW struct indexPageDescriptor;
	ZeroMemory(indexPageDesc, sizeof(struct indexPageDescriptor));
	indexPageDesc->next = NULL;
	indexPageDesc->channelNum = pCurrIndex->dataRecs[0].size;
	map<long long, struct indexPageDescriptor *>::iterator itMapNextDescriptor = mapVideoPages.find(pCurrIndex->fPosNextRecord);
	if (itMapNextDescriptor != mapVideoPages.end())
	{
		indexPageDesc->next = itMapNextDescriptor->second;
	}
	indexPageDesc->nPageType = 5;
	indexPageDesc->numRecs = 0;
	indexPageDesc->pageDescriptor.pChannel = DBG_NEW struct channelIndexDescriptor;
	indexPageDesc->pageDescriptor.pChannel->currFPos = posAdded;
	indexPageDesc->pageDescriptor.pChannel->prevFpos = posPrevIndex;
	indexPageDesc->pageDescriptor.pChannel->nextFPos = pCurrIndex->fPosNextRecord;
	indexPageDesc->pageDescriptor.pChannel->startTime = 0L;
	indexPageDesc->pageDescriptor.pChannel->inPath = true;
	indexPageDesc->pageDescriptor.pChannel->newChannel = (int)pCurrIndex->dataRecs[0].size;
	indexPageDesc->pageDescriptor.pChannel->prevChannel = (int)pCurrIndex->dataRecs[0].compSize;
	for (int i = 0; i <= 0; i++)
	{
		indexPageDesc->dataRecs[i].indexNum = pCurrIndex->dataRecs[i].videoIndexNumber;
		indexPageDesc->dataRecs[i].recDuration = (long)pCurrIndex->dataRecs[i].recDurationus;
		indexPageDesc->pageDescriptor.pVideo->timespan += pCurrIndex->dataRecs[i].recDurationus;
	}

	mapVideoPages.insert(pair<long long, struct indexPageDescriptor *>(posAdded, indexPageDesc));
	// Look up previous Index Hdr
	UpdatePreviousIndexHdr(posPrevIndex, posAdded, pPrevIndex, indexPageDesc);

	return posAdded; // pPrevIndex->fPosNextRecord;
}


long long FileContext::InsertVideoIndexRecord(short *channelNum, struct IndexHdr *pCurrIndex, struct IndexHdr *pPrevIndex, long long posPrevIndex, int numMon, FileContext *fContext)
{
	long long posAdded = 0L;
	bool bAddedToRoot = false;
	//struct IndexHdr lastIndexHdr;

	// Need to handle insert at top of channel
	pCurrIndex->fPosPrevRecord = posPrevIndex;
	// writing to the root record
	ChannelDataMapping *pChannelData = GetChannelDataMapping(*channelNum);
	if (pChannelData == NULL)
		return 0L;
	struct VideoHdr *pVHeader;
	pVHeader = pChannelData->GetVideoHeader();

	if (fContext != this)
	{
		// if Copying from a different context, then copy the datarecs of the current Index in the secondary file to the current file
		long long fSizeVideo = fContext->GetVideoBufferSize();
		//long long fSizeAudio = fContext->GetAudioBufferSize();
		if (fSizeVideo > maxVideoBufferSize)
			maxVideoBufferSize = fSizeVideo;
		//if (fSizeAudio > maxAudioBufferSize)
		//	maxAudioBufferSize = fSizeAudio;

		char *fBuffer = (char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, fSizeVideo);
		//char *fAudioBuffer = (char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, fSizeAudio);
		DataFile *pSrcDataFile = fContext->GetDataFile();
		for (int i = 0; i <= pCurrIndex->numRecs; i++)
		{
			pSrcDataFile->ReadCompressedBlockFromFile(fBuffer, pCurrIndex->dataRecs[i].compSize, pCurrIndex->dataRecs[i].fPosData);
			pCurrIndex->dataRecs[i].fPosData = pDataFile->AppendCompressedBlockToFile(fBuffer, pCurrIndex->dataRecs[i].compSize);
			if (pCurrIndex->dataRecs[i].fPosAudioData != 0L)
			{
				pSrcDataFile->ReadCompressedBlockFromFile(fBuffer, pCurrIndex->dataRecs[i].compAudioSize, pCurrIndex->dataRecs[i].fPosAudioData);
				pCurrIndex->dataRecs[i].fPosAudioData = pDataFile->AppendCompressedBlockToFile(fBuffer, pCurrIndex->dataRecs[i].compAudioSize);
			}
		}
		HeapFree(GetProcessHeap(), 0, fBuffer);
	}

	if (posPrevIndex == NULL)
	{
		pVHeader->fPosRoot = pDataFile->AppendToFile((char *)pCurrIndex, (ULONG)sizeof(struct IndexHdr));
		pDataFile->WriteToFile(pChannelData->GetChannelHeader()->posVideoRoot, (char *)pVHeader, sizeof(struct VideoHdr));
		posAdded = pVHeader->fPosRoot;
	}
	else
	{
		if (pPrevIndex->numRecs > numMon || pPrevIndex->RecType != 'P')
		{
			pCurrIndex->fPosNextRecord = pPrevIndex->fPosNextRecord;
			pPrevIndex->fPosNextRecord = pDataFile->AppendToFile((char *)pCurrIndex, (ULONG)sizeof(struct IndexHdr));
			posAdded = pPrevIndex->fPosNextRecord;
			pDataFile->WriteToFile(posPrevIndex, (char *)pPrevIndex, (ULONG)sizeof(struct IndexHdr));
		}
		else if (pPrevIndex->fPosPrevRecord == NULL)
		{
			bAddedToRoot = true;
			pCurrIndex->fPosPrevRecord = NULL;
			pCurrIndex->fPosNextRecord = posPrevIndex;
			pVHeader->fPosRoot = pDataFile->AppendToFile((char *)pCurrIndex, (ULONG)sizeof(struct IndexHdr));
			pDataFile->WriteToFile(pChannelData->GetChannelHeader()->posVideoRoot, (char *)pVHeader, sizeof(struct VideoHdr));
			posAdded = pVHeader->fPosRoot;
			posPrevIndex = NULL;
		}
		else
		{
			posPrevIndex = pPrevIndex->fPosPrevRecord;
			pDataFile->ReadFromFile((char *)pPrevIndex, sizeof(struct IndexHdr), posPrevIndex);
			pCurrIndex->fPosNextRecord = pPrevIndex->fPosNextRecord;
			pPrevIndex->fPosNextRecord = pDataFile->AppendToFile((char *)pCurrIndex, (ULONG)sizeof(struct IndexHdr));
			posAdded = pPrevIndex->fPosNextRecord;
			pDataFile->WriteToFile(posPrevIndex, (char *)pPrevIndex, (ULONG)sizeof(struct IndexHdr));
		}
	}

	struct indexPageDescriptor *indexPageDesc = DBG_NEW struct indexPageDescriptor;
	ZeroMemory(indexPageDesc, sizeof(struct indexPageDescriptor));
	indexPageDesc->next = NULL;
	indexPageDesc->channelNum = *channelNum;
	map<long long, struct indexPageDescriptor *>::iterator itMapNextDescriptor = mapVideoPages.find(pCurrIndex->fPosNextRecord);
	if (itMapNextDescriptor != mapVideoPages.end())
	{
		indexPageDesc->next = itMapNextDescriptor->second;
	}
	if (pCurrIndex->RecType == 'P')
	{
		indexPageDesc->nPageType = 1;
		indexPageDesc->pageDescriptor.pVideo = DBG_NEW struct videoIndexDescriptor;
		indexPageDesc->numRecs = pCurrIndex->numRecs;
		indexPageDesc->pageDescriptor.pVideo->currFPos = posAdded;
		if (!bAddedToRoot)
		{
			indexPageDesc->pageDescriptor.pVideo->prevFpos = posPrevIndex;
			indexPageDesc->pageDescriptor.pVideo->nextFpos = pCurrIndex->fPosNextRecord;
		}
		else
		{
			
			indexPageDesc->pageDescriptor.pVideo->prevFpos = 0L;
			indexPageDesc->pageDescriptor.pVideo->nextFpos = pCurrIndex->fPosNextRecord; // There is no page before
		}
		indexPageDesc->pageDescriptor.pVideo->startTime = 0L;
		indexPageDesc->pageDescriptor.pVideo->timespan = 0;
		indexPageDesc->pageDescriptor.pVideo->inPath = true;
		int monNum = pChannelData->GetNumMonitors() - 1;
		for (int i = 0; i <= pCurrIndex->numRecs; i++)
		{
			indexPageDesc->dataRecs[i].indexNum = pCurrIndex->dataRecs[i].videoIndexNumber;
			indexPageDesc->dataRecs[i].recDuration = (long)pCurrIndex->dataRecs[i].recDurationus;
			if (i >= monNum)
				indexPageDesc->pageDescriptor.pVideo->timespan += pCurrIndex->dataRecs[i].recDurationus;
		}
	}
	mapVideoPages.insert(pair<long long, struct indexPageDescriptor *>(posAdded, indexPageDesc));
	// Look up previous Index Hdr
	UpdatePreviousIndexHdr(posPrevIndex, posAdded, pPrevIndex, indexPageDesc);


	return posAdded; // pPrevIndex->fPosNextRecord;
}

long long FileContext::GetInsertEventsAndVideoInSegment(struct IndexHdr *pIndexHdr, struct IndexHdr *pDestIndexHdr, FileContext *fContext)
{
	ChannelDataMapping *pChannelData = GetChannelDataMapping(mainChannelNumber);
	if (pChannelData == NULL)
		return 0L;

	CopyMemory(pDestIndexHdr, pIndexHdr, sizeof(struct IndexHdr));

	list <BPMNElement *> *pEventList = &segmentEvents;
	long long indexTime = 0L;
	for (int i = 0; i <= pIndexHdr->numRecs; i++)
	{
		// This is the old event Index Number
		ULONG eventIndex = pIndexHdr->dataRecs[i].videoIndexNumber;
		pChannelData->GetChannelHeader()->lastIndexNumber++;
		pDestIndexHdr->dataRecs[i].videoIndexNumber = pChannelData->GetChannelHeader()->lastIndexNumber;
		indexTime += pIndexHdr->dataRecs[i].recDurationus;
		BPMNElement *pEventElement = fContext->FindEventInList(pEventList, eventIndex);
		ULONG oldEventIndex = eventIndex;
		if (pEventElement != NULL)
		{
			// Commented to test if sub-process is created when the branch is within a sub-process

			if (pEventElement->GetType() == 2) // Task events
			{
				// This is the new event Index number
			//	BPMNElement *pOldEventParent = pEventElement->GetParentElement();
			//	if (pOldEventParent != NULL && pOldEventParent->GetType() == 3)
			//	{
			//		map<ULONG, ULONG>::iterator itParentIndexMap = parentIndexMapping.find(pOldEventParent->GetEventIndex());
			//		if (itParentIndexMap == parentIndexMapping.end())
			//		{
			//			pChannelData->GetChannelHeader()->lastIndexNumber++;
			//			ULONG parentEventIndex = pChannelData->GetChannelHeader()->lastIndexNumber;
			//			ULONG oldParentEventIndex = pOldEventParent->GetEventIndex();
			//			parentIndexMapping.insert(pair<ULONG, ULONG>(oldParentEventIndex, parentEventIndex));

			//			// Create a new Event Element
			//			BPMNElement *pNewSubProcessEvent = pOldEventParent->CreateNewSubProcessEvent(parentEventIndex);
			//			AddToEventList(&insertSegmentEvents, pNewSubProcessEvent, pOldEventParent, pOldEventParent->GetParentElement());
			//			if (fContext != this)
			//			{
			//				// Copy Event Record details
			//				UpdateEventDetailsFromContext(parentEventIndex, oldParentEventIndex, fContext);
			//			}
			//		}
			//	}

				eventIndex = pDestIndexHdr->dataRecs[i].videoIndexNumber;
				// Create a new Event Element
				BPMNElement *pNewEvent = pEventElement->CreateNewEvent(eventIndex);

				AddToEventList(&insertSegmentEvents, pNewEvent, pEventElement, pEventElement->GetParentElement());
				if (fContext != this)
				{
					// Copy Event Record details
					UpdateEventDetailsFromContext(eventIndex, oldEventIndex, fContext);
				}
			}
			else if (pEventElement->GetType() == 3) // Sub-Process Events
			{
				// This is the new event Index number
				// This code will never be called because sub-process is not assigned a valid video index, it is attached to the child elements
				eventIndex = pDestIndexHdr->dataRecs[i].videoIndexNumber;
				parentIndexMapping.insert(pair<ULONG, ULONG>(oldEventIndex, eventIndex));
				// Create a new Event Element
				BPMNElement *pNewEvent = pEventElement->CreateNewSubProcessEvent(eventIndex);
				AddToEventList(&insertSegmentEvents, pNewEvent, pEventElement, pEventElement->GetParentElement());
				if (fContext != this)
				{
					// Copy Event Record details
					UpdateEventDetailsFromContext(eventIndex, oldEventIndex, fContext);
				}
			}
			else if (pEventElement->GetType() == 4) // Branch Events
			{
				// This is the new event Index number
				eventIndex = pDestIndexHdr->dataRecs[i].videoIndexNumber;
				// Create a new Event Element
				BPMNElement *pNewEvent = pEventElement->CreateNewBranchEvent(eventIndex);
				parentIndexMapping.insert(pair<ULONG, ULONG>(oldEventIndex, eventIndex));
				AddToEventList(&insertSegmentEvents, pNewEvent, pEventElement, pEventElement->GetParentElement());
				if (fContext != this)
				{
					// Copy Event Record details
					UpdateEventDetailsFromContext(eventIndex, oldEventIndex, fContext);
				}
			}
		}
	}
	return indexTime;
}

bool FileContext::GetEventDetail(ULONG nIndex, struct eventFileRecord *pEventRec, char *pBuffer)
{
	struct eventListElement *pEventElement = NULL;
	map <ULONG, struct eventListElement *>::iterator itEventFind = pEventMap->find(nIndex);
	if (itEventFind == pEventMap->end())
		return false;
	pEventElement = itEventFind->second;
	pDataFile->ReadFromFile((char *)pEventRec, sizeof(struct eventFileRecord), pEventElement->fPosCurr);
	pDataFile->ReadDeflateBlockFromFile((char *)pBuffer, pEventRec->size, pEventRec->compSize, pEventRec->dataPos);
	return true;
}

void FileContext::UpdateEventDetailsFromContext(ULONG nIndex, ULONG oIndex, FileContext *fContext)
{
	map <ULONG, struct eventListElement *>::iterator itEventFind = pEventMap->find(nIndex);
	if (itEventFind == pEventMap->end())
		return;
	struct eventListElement *pEventElement = itEventFind->second;

	char *pRecBuffer = (char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 10240);
	struct eventFileRecord oldEventRec;
	struct eventFileRecord newEventRec;

	ZeroMemory((char *)&oldEventRec, sizeof(struct eventFileRecord));
	ZeroMemory((char *)&newEventRec, sizeof(struct eventFileRecord));

	if (fContext->GetEventDetail(oIndex, &oldEventRec, pRecBuffer))
	{
		pDataFile->ReadFromFile((char *)&newEventRec, sizeof(struct eventFileRecord), pEventElement->fPosCurr);
		newEventRec.nEventCode = oldEventRec.nEventCode;
		newEventRec.size = oldEventRec.size;
		newEventRec.dataPos = pDataFile->AppendDeflateBlockToFile((char *)pRecBuffer, newEventRec.size, &newEventRec.compSize);
		pDataFile->WriteToFile(pEventElement->fPosCurr, (char *)&newEventRec, sizeof(struct eventFileRecord));
	}
	
	HeapFree(GetProcessHeap(), NULL, pRecBuffer);
}

void FileContext::AddToEventList(list <BPMNElement *> *pEventList, BPMNElement *pElement, BPMNElement *pOldElement, BPMNElement *pSourceElement)
{
	// Push back event
	// Get previous element
	BPMNElement *pPrevEvent = NULL;
	if (pSourceElement != NULL)
	{
		ULONG nParentID = pSourceElement->GetEventIndex();
		ULONG newParentID = 0L;
		BPMNElement *pParentEventElement = NULL;
		map <ULONG, ULONG>::iterator itParentEvent = parentIndexMapping.find(nParentID);
		if (itParentEvent != parentIndexMapping.end())
		{
			newParentID = itParentEvent->second;
			pParentEventElement = FindEventInList(pEventList, newParentID);
			pElement->SetParentElement(pParentEventElement);
			if (pParentEventElement->GetType() == 3)
			{
				BPMNSubProcess *pSubProcess = (BPMNSubProcess *)pParentEventElement;
				list <BPMNElement *> *pSubProcessList = pSubProcess->GetSubProcessElements();
				if (pSubProcessList->size() > 0)
				{
					pPrevEvent = pSubProcess->GetLastElement();
				}
				pSubProcess->AddElement(pElement);
			}
			else if (pParentEventElement->GetType() == 4)
			{
				BPMNDecision *pDecision = (BPMNDecision *)pParentEventElement;
				list <BPMNElement *> *pBranchList = NULL;
				if (pOldElement->IsInRightBranchList())
				{
					pBranchList = pDecision->GetBranchRightElements();
					if (pBranchList->size() > 0)
					{
						pPrevEvent = pBranchList->back();
					}
					else
					{
						pPrevEvent = pParentEventElement;
					}
					pDecision->AddRightElement(pElement);
				}

				if (pOldElement->IsInBottomBranchList())
				{
					pBranchList = pDecision->GetBranchBottomElements();
					if (pBranchList->size() > 0)
					{
						pPrevEvent = pBranchList->back();
					}
					else
					{
						pPrevEvent = pParentEventElement;
					}
					pDecision->AddBottomElement(pElement);
				}
			}
		}
		else
		{
			if (pEventList->size() > 0)
			{
				pPrevEvent = pEventList->back();
			}
			pEventList->push_back(pElement);
		}
	}
	else
	{
		if (pEventList->size() > 0)
		{
			pPrevEvent = pEventList->back();
		}
		pEventList->push_back(pElement);
	}

	struct eventListElement * pEventListElement = DBG_NEW struct eventListElement;
	pEventListElement->fPosCurr = NULL;
	pEventListElement->nextElement = pEventListElement->prevElement = NULL;
	pEventListElement->pElement = pElement;
	pEventMap->insert(pair<ULONG, struct eventListElement *>(pElement->GetEventIndex(), pEventListElement));
	struct eventListElement *pPrevEventListElement = NULL;

	if (pPrevEvent != NULL)
	{
		map <ULONG, struct eventListElement *>::iterator itMap = pEventMap->find(pPrevEvent->GetEventIndex());
		if (itMap != pEventMap->end())
		{
			pPrevEventListElement = itMap->second;
			pPrevEventListElement->nextElement = pEventListElement;
			pEventListElement->prevElement = pPrevEventListElement;
		}
	}
	// Add the event record in the file.
	AddNewEventRecord(pEventListElement);
	if (pPrevEventListElement != NULL)
		UpdateEventRecord(pPrevEventListElement, NULL);
}

void FileContext::InsertEventsInList(VideoSegmentClass *pCopySegment)
{
	if (insertSegmentEvents.size() == 0)
		return;
	struct IndexHdr indexHdr;
	CopyMemory(&indexHdr, &destBottomIndexHdr, sizeof(struct IndexHdr));
	list <BPMNElement *> *pEventList = pBPMNFacade->GetBPMNDiagram();
	BPMNElement *pEventElement = NULL;
	list <BPMNElement *>::iterator itEvent = pEventList->begin();

	// Find insertion Point
	while (true)
	{
		pEventElement = NULL;
		for (int i = 0; i <= indexHdr.numRecs; i++)
		{
			ULONG eventIndex = indexHdr.dataRecs[i].videoIndexNumber;
			itEvent = FindEventIteratorInList(&pEventList, eventIndex, true);
			if (itEvent != pEventList->end())
			{
				pEventElement = *itEvent;
				break;
			}
		}
		if (pEventElement == NULL)
		{
			if (indexHdr.fPosNextRecord != NULL)
			{
				pDataFile->ReadFromFile((char *)&indexHdr, sizeof(struct IndexHdr), indexHdr.fPosNextRecord);
				continue;
			}
			else
				break;
		}
		else
			break;
	}

	bool bAddAbove = true;
	if (pEventElement == NULL)
	{
		// Insert list to end of list
		// The last event in the event list is the Stop event so point to the second last event
		itEvent = pEventList->end();
		itEvent--;
		itEvent--;
		pEventElement = *itEvent;
		bAddAbove = false;
		itEvent++;
	}
	// pEventElement contains point of insertion
	pEventList->insert(itEvent, insertSegmentEvents.begin(), insertSegmentEvents.end());
	list <BPMNElement *>::iterator itNewEvent = insertSegmentEvents.begin();
	while (itNewEvent != insertSegmentEvents.end())
	{
		BPMNElement *pNewEventElement = *itNewEvent;
				
		InsertEventInEventMap(pEventElement, pNewEventElement, bAddAbove);
		itNewEvent++;
		if (!bAddAbove)
			pEventElement = pNewEventElement;
	}

	pEventElement = insertSegmentEvents.front();
	map <ULONG, struct eventListElement *>::iterator itEventMap = pEventMap->find(pEventElement->GetEventIndex());
	if (itEventMap == pEventMap->end())
		return;
	struct eventListElement *pEventListElement = itEventMap->second;
	struct eventListElement *pPrevEventListElement = pEventListElement->prevElement;
	BPMNElement *pParentDest = pEventListElement->pElement->GetParentElement();
		
	if (pPrevEventListElement == NULL && pParentDest == NULL)
		SetEventRoot(pEventListElement);
	else if (pPrevEventListElement != NULL)
		UpdateEventRecord(pPrevEventListElement, NULL);
	itNewEvent = insertSegmentEvents.begin();
	while (itNewEvent != insertSegmentEvents.end())
	{
		pEventElement = *itNewEvent;
		itEventMap = pEventMap->find(pEventElement->GetEventIndex());
		if (itEventMap == pEventMap->end())
			return;
		pEventListElement = itEventMap->second;
		UpdateEventRecord(pEventListElement, NULL);
		itNewEvent++;
	}
	pEventListElement = pEventListElement->nextElement;
	if (pEventListElement != NULL)
		UpdateEventRecord(pEventListElement, NULL);
	UpdateSourceAndDestinationParentRecords(pParentDest, NULL);
}

void FileContext::InsertEventInEventMap(BPMNElement *pInsPoint, BPMNElement *pNewEvent, bool bAddAbove)
{
	pNewEvent->SetParentElement(pInsPoint->GetParentElement());
	map <ULONG, struct eventListElement *>::iterator itEventMap = pEventMap->find(pInsPoint->GetEventIndex());
	if (itEventMap != pEventMap->end())
	{
		struct eventListElement *pInsEventListElement = itEventMap->second;
		struct eventListElement *pPrevEventListElement = pInsEventListElement->prevElement;

		struct eventListElement *pNewEventListElement = NULL;
		map <ULONG, struct eventListElement *>::iterator itEventMapNewElement = pEventMap->find(pNewEvent->GetEventIndex());
		if (itEventMapNewElement == pEventMap->end())
		{
			pNewEventListElement = DBG_NEW struct eventListElement;
			pEventMap->insert(pair<ULONG, eventListElement *>(pNewEvent->GetEventIndex(), pNewEventListElement));
			pNewEventListElement->pElement = pNewEvent;
		}
		else
			pNewEventListElement = itEventMapNewElement->second;

		if (bAddAbove)
		{
			pNewEventListElement->nextElement = pInsEventListElement;
			pNewEventListElement->prevElement = pPrevEventListElement;
			if (pPrevEventListElement != NULL)
			{
				pPrevEventListElement->nextElement = pNewEventListElement;
			}
			if (pInsEventListElement != NULL)
				pInsEventListElement->prevElement = pNewEventListElement;
		}
		else
		{
			// Add to end
			pNewEventListElement->prevElement = pInsEventListElement;
			pNewEventListElement->nextElement = NULL;
			pInsEventListElement->nextElement = pNewEventListElement;
		}
	}

}

list <BPMNElement *>::iterator FileContext::FindEventIteratorInList(list <BPMNElement *> **pElementList, ULONG eventIndex, bool bUpdateListArg)
{
	list <BPMNElement *> *pListElement = *pElementList;

	list <BPMNElement *>::iterator itEvent = pListElement->end();
	struct eventListElement *pEventElement = NULL;
	map <ULONG, struct eventListElement *>::iterator itEventFind = pEventMap->find(eventIndex);
	if (itEventFind == pEventMap->end())
		return itEvent;
	pEventElement = itEventFind->second;
	BPMNElement *pElement = pEventElement->pElement;
	BPMNElement *pParent = pElement->GetParentElement();
	list <BPMNElement *> *pList = *pElementList;

	if (pParent != NULL)
	{
		if (pParent->GetType() == 3)
		{
			BPMNSubProcess *pSubProcess = (BPMNSubProcess *)pParent;
			pList = pSubProcess->GetSubProcessElements();
		}
		else if (pParent->GetType() == 4)
		{
			BPMNDecision *pDecision = (BPMNDecision *)pParent;
			pList = pDecision->GetDecisionElementList(pElement);
		}
	}
	if (bUpdateListArg)
	{
		*pElementList = pList;
	}
	itEvent = find(pList->begin(), pList->end(), pElement);
	if (itEvent == pList->end())
		itEvent = (*pElementList)->end(); // Make sure the iterator is pointing to the end of the list;
	return itEvent;
}

int FileContext::CopyAudioAndVideoToChannel(struct channelHdr *pChannelHdrSource)
{

	struct channelHdr *pChannelHdr = AddNewChannel();
	int channelNum = pChannelHdr->channelNum;

	//CopyVideoAndAudioHeaders(pChannelHdr, pChannelHdrSource);
	return channelNum;
}


void FileContext::InsertEventsToBranch(int nChannel, long long fPos, long long fPosNext)
{
	struct indexPageDescriptor *pBranchIndex = NULL;
	struct indexPageDescriptor *pBranchNextIndex = NULL;
	struct indexPageDescriptor *pBranchPrevIndex = NULL;
	
	map <long long, struct indexPageDescriptor *>::iterator itMap = mapVideoPages.find(fPos);
	if (itMap == mapVideoPages.end())
		return;
	pBranchIndex = itMap->second;
	
	itMap = mapVideoPages.find(pBranchIndex->pageDescriptor.pBranch->prevFpos);
	if (itMap != mapVideoPages.end())
	{
		pBranchPrevIndex = itMap->second;
	}

	// if prev index page descriptor not found then the branch will be added to root of the tree. TO BE DONE
	ULONG prevBranchIndexNum = 0L;
	if (pBranchPrevIndex != NULL)
	{
		prevBranchIndexNum = pBranchPrevIndex->dataRecs[pBranchPrevIndex->numRecs].eventIndexNum;
	}

	BPMNDecision *pDecisionElement = DBG_NEW BPMNDecision(pBranchIndex->dataRecs[0].indexNum);
	pDecisionElement->SetBranchRightChannel(nChannel);
	pDecisionElement->SetBranchBottomChannel(startPositionMarker.channelNum);
	pDecisionElement->SetAnnotation("New Branch");

	// Insert Branch Segment
	list <BPMNElement *> *pEventList = pBPMNFacade->GetBPMNDiagram();

	// Get the index page for the next video element after join
	itMap = mapVideoPages.find(fPosNext);
	if (itMap == mapVideoPages.end())
		return;
	pBranchNextIndex = itMap->second;
	if (pBranchNextIndex->nPageType != 1) // if not video page then get next video page
	{
		pBranchNextIndex = GetNextVideoIndexDescriptor(pBranchNextIndex);
	}
	
	ULONG nEventIndex = 0L;
	list <BPMNElement *>::iterator itFind = pEventList->end();

	if (pBranchNextIndex != NULL)
	{
		nEventIndex = GetInsertionElementIndex(pBranchNextIndex);
		itFind = FindEventIteratorInList(&pEventList, nEventIndex, true);
	}

	BPMNElement *pElement = NULL;
	map <ULONG, struct eventListElement *>::iterator itMapElement = pEventMap->end();
	if (itFind != pEventList->end())
	{
		pElement = *itFind;
		// Now set the parent of the decision element
		pDecisionElement->SetParentElement(pElement->GetParentElement());
		pEventList->insert(itFind, (BPMNElement *)pDecisionElement);
		itMapElement = pEventMap->find(pElement->GetEventIndex());
	}
	else
	{
		BPMNElement *pElementStop = pEventList->back();
		pEventList->pop_back();
		pEventList->push_back((BPMNElement *)pDecisionElement);
		pEventList->push_back(pElementStop);
	}

	struct eventListElement *pMapElement = NULL;
	struct eventListElement *pNewMapElement = NULL;
	struct eventListElement *pPrevMapElement = NULL;

	if (itMapElement != pEventMap->end())
	{
		// Insert above the first element in the list
		pMapElement = itMapElement->second;
	}
	pNewMapElement = DBG_NEW struct eventListElement;
	pPrevMapElement = NULL;
	if (prevBranchIndexNum != NULL)
	{
		itMapElement = pEventMap->find(prevBranchIndexNum);
		if (itMapElement != pEventMap->end())
			pPrevMapElement = itMapElement->second;
	}
	pNewMapElement->pElement = (BPMNElement *)pDecisionElement;
	pNewMapElement->prevElement = pPrevMapElement;
	pNewMapElement->nextElement = pMapElement;
	if (pMapElement != NULL)
		pMapElement->prevElement = pNewMapElement;
	pEventMap->insert(pair<ULONG, struct eventListElement *>(pDecisionElement->GetEventIndex(), pNewMapElement));
	AddNewEventRecord(pNewMapElement); 
	BPMNElement *pRootParent = NULL;
	if (pMapElement != NULL)
	{
		pRootParent = pMapElement->pElement->GetParentElement();
	}

	if (pPrevMapElement == NULL && pRootParent == NULL)
	{
		//		Write channel header or previous element record (only pointers)
		SetEventRoot(pNewMapElement);
	}
	else if (pPrevMapElement != NULL)
	{
		pPrevMapElement->nextElement = pNewMapElement;
		UpdateEventRecord(pPrevMapElement, NULL);
	}

	if (pMapElement != NULL)
		UpdateEventRecord(pMapElement, NULL);
	if (segmentEvents.size() > 0)
	{
		RemoveEventsInList(&segmentEvents);
		pDecisionElement->AddBottomBranchList(segmentEvents); // no change indexes
		UpdateEventList(&segmentEvents);
	}
	pDecisionElement->AddRightBranchList(insertSegmentEvents); // changed indexes
	UpdateEventList(&insertSegmentEvents);
}

ULONG FileContext::GetInsertionElementIndex(struct indexPageDescriptor *pIndexDescriptor)
{
	ULONG eventIndex = 0L;
	struct indexPageDescriptor *pIndex = pIndexDescriptor;

	while (true)
	{
		for (int i = 0; i <= pIndex->numRecs; i++)
		{
			eventIndex = pIndex->dataRecs[i].indexNum;
			map <ULONG, struct eventListElement *>::iterator itEvent = pEventMap->find(eventIndex);
			if (itEvent != pEventMap->end())
				break;
			eventIndex = 0L;
		}
		if (eventIndex != 0L)
			break;
		pIndex = pIndex->next;
		pIndex = GetNextVideoIndexDescriptor(pIndex);
		if (pIndex == NULL)
		{
			eventIndex = 0L;
			break;
		}
	}
	return eventIndex;
}


struct indexPageDescriptor *FileContext::GetNextVideoIndexDescriptor(struct indexPageDescriptor *pIndexDescriptor)
{
	while (pIndexDescriptor != NULL && pIndexDescriptor->nPageType != 1 && pIndexDescriptor->nPageType != 2)
	{
		pIndexDescriptor = pIndexDescriptor->next;
	}

	return pIndexDescriptor;
}

void FileContext::UpdateEventList(list<BPMNElement *> *pList)
{
	list <BPMNElement *>::iterator itList = pList->begin();
	struct eventListElement *pEventListLastElement = NULL;
	while (itList != pList->end())
	{
		BPMNElement *pElement = *itList;

		map <ULONG, struct eventListElement *>::iterator itEvent = pEventMap->find(pElement->GetEventIndex());
		struct eventListElement *pEventListElement = NULL;
		if (itEvent == pEventMap->end())
			pEventListElement = DBG_NEW struct eventListElement;
		else
			pEventListElement = itEvent->second;
		pEventListElement->pElement = pElement;
		pEventListElement->nextElement = NULL;
		pEventListElement->prevElement = pEventListLastElement;
		if (itEvent == pEventMap->end())
			pEventMap->insert(pair<ULONG, struct eventListElement *>(pElement->GetEventIndex(), pEventListElement));
		UpdateEventRecord(pEventListElement, NULL);
		if (pEventListLastElement == NULL && pElement->GetParentElement() == NULL)
		{
			SetEventRoot(pEventListElement);
		}
		else if (pEventListLastElement != NULL)
		{
			pEventListLastElement->nextElement = pEventListElement;
			UpdateEventRecord(pEventListLastElement, NULL);
		}
		pEventListLastElement = pEventListElement;
		itList++;
	}
	if (pList->size() > 0)
	{
		BPMNElement *pFirstElement = pList->front();
		UpdateSourceAndDestinationParentRecords(pFirstElement->GetParentElement(), NULL);
	}
}

long long FileContext::CreateBottomBranchIndex(long long fPosRightBottom)
{
	long long fPosPrev = 0L;
	short channelNum = endPositionMarker.channelNum;
	struct IndexHdr indexOldHdr;

	// Write the endTopIndexHdr in position and Insert endBottomIndexHdr
	if (endTopIndexHdr.fPosPrevRecord == NULL && endTopIndexHdr.numRecs == -1)
	{
		pDataFile->WriteToFile(startPositionMarker.fPosIndex, (char *)&startBottomIndexHdr, (ULONG)sizeof(struct IndexHdr));
		CopyMemory(&indexOldHdr, &startBottomIndexHdr, sizeof(struct IndexHdr));
		UpdateIndexPageDescriptor(startPositionMarker.fPosIndex, &startBottomIndexHdr);
	}
	else
	{
		pDataFile->WriteToFile(endPositionMarker.fPosIndex, (char *)&endTopIndexHdr, (ULONG)sizeof(struct IndexHdr));
		CopyMemory(&indexOldHdr, &endTopIndexHdr, sizeof(struct IndexHdr));
		UpdateIndexPageDescriptor(endPositionMarker.fPosIndex, &endTopIndexHdr);
	}
	ChannelDataMapping *pChannelData = GetChannelDataMapping(mainChannelNumber);
	if (pChannelData == NULL)
		return 0L;

	struct channelHdr *pChannelHdr = pChannelData->GetChannelHeader();
	pChannelHdr->lastIndexNumber++;

	// Add Join Index record
	struct IndexHdr joinIndex;
	ZeroMemory(&joinIndex, sizeof(struct IndexHdr));
	joinIndex.RecType = 'J';
	if (endTopIndexHdr.fPosPrevRecord == NULL && endTopIndexHdr.numRecs == -1)
		joinIndex.fPosPrevRecord = startPositionMarker.fPosIndex;
	else
		joinIndex.fPosPrevRecord = endPositionMarker.fPosIndex;
	joinIndex.dataRecs[0].videoIndexNumber = pChannelHdr->lastIndexNumber; 
	joinIndex.dataRecs[0].fPosData = fPosRightBottom; // poisition to last record in the right branch
	joinIndex.fPosNextRecord = fPosPrev;
	if (endTopIndexHdr.fPosPrevRecord == NULL && endTopIndexHdr.numRecs == -1)
		fPosPrev = InsertJoinIndexRecord(&channelNum, &joinIndex, &indexOldHdr, startPositionMarker.fPosIndex, pChannelData->GetNumMonitors());
	else
		fPosPrev = InsertJoinIndexRecord(&channelNum, &joinIndex, &indexOldHdr, endPositionMarker.fPosIndex, pChannelData->GetNumMonitors());
	CopyMemory(&indexOldHdr, &joinIndex, sizeof(struct IndexHdr));
	
	// Read the last right hand record and initialize to point to the Join record
	struct IndexHdr rightBottomRec;
	pDataFile->ReadFromFile((char *)&rightBottomRec, sizeof(struct IndexHdr), fPosRightBottom);
	rightBottomRec.fPosNextRecord = fPosPrev;
	pDataFile->WriteToFile(fPosRightBottom, (char *)&rightBottomRec, sizeof(struct IndexHdr));
	UpdateIndexPageDescriptor(fPosRightBottom, &rightBottomRec);

	endBottomIndexHdr.fPosPrevRecord = fPosPrev;
	fPosPrev = InsertIndexRecord(&channelNum, &endBottomIndexHdr, &indexOldHdr, fPosPrev, pChannelData->GetNumMonitors(), this);
	if (endBottomIndexHdr.fPosNextRecord != NULL)
	{
		struct IndexHdr endBottomNextHdr;
		pDataFile->ReadFromFile((char *)&endBottomNextHdr, sizeof(struct IndexHdr), endBottomIndexHdr.fPosNextRecord);
		endBottomNextHdr.fPosPrevRecord = fPosPrev;
		pDataFile->WriteToFile(endBottomIndexHdr.fPosNextRecord, (char *)&endBottomNextHdr, sizeof(struct IndexHdr));
		UpdateIndexPageDescriptor(endBottomIndexHdr.fPosNextRecord, &endBottomNextHdr);
	}
	return fPosPrev;
}

long long FileContext::AddBranchIndex(long long fPosBranchInsPoint)
{
	// Insert a branch Index between startTop and start Bottom
	ChannelDataMapping *pChannelData = GetChannelDataMapping(mainChannelNumber);
	if (pChannelData == NULL)
		return(0L);

	struct channelHdr *pChannelHdr = pChannelData->GetChannelHeader();
	struct VideoHdr *pVHeader;
	pVHeader = pChannelData->GetVideoHeader();

	struct IndexHdr topIndexHdr, bottomIndexHdr;
	struct IndexHdr insIndexHdr;

	// Check if added at root
	if (startTopIndexHdr.fPosPrevRecord == NULL && startTopIndexHdr.numRecs == -1)
		pDataFile->ReadFromFile((char *)&topIndexHdr, sizeof(struct IndexHdr), pVHeader->fPosRoot);
	else
		pDataFile->ReadFromFile((char *)&topIndexHdr, sizeof(struct IndexHdr), startPositionMarker.fPosIndex);

	pDataFile->ReadFromFile((char *)&insIndexHdr, sizeof(struct IndexHdr), fPosBranchInsPoint);
	pDataFile->ReadFromFile((char *)&bottomIndexHdr, sizeof(struct IndexHdr), insIndexHdr.fPosNextRecord);

	struct IndexHdr branchIndexHdr;
	ZeroMemory(&branchIndexHdr, sizeof(struct IndexHdr));
	branchIndexHdr.numRecs = 0;
	branchIndexHdr.RecType = 'B';
	if (startTopIndexHdr.fPosPrevRecord == NULL && startTopIndexHdr.numRecs == -1)
		branchIndexHdr.dataRecs[0].fPosData = pVHeader->fPosRoot; // currently the next record after the start Top is the copy top which is the right branch
	else
		branchIndexHdr.dataRecs[0].fPosData = topIndexHdr.fPosNextRecord; // currently the next record after the start Top is the copy top which is the right branch

	pChannelHdr->lastIndexNumber++;
	branchIndexHdr.dataRecs[0].videoIndexNumber = pChannelHdr->lastIndexNumber;
	// initialize Right Branch top records previous pointer to NULL
	struct IndexHdr rightTopIndex;

	if (startTopIndexHdr.fPosPrevRecord == NULL && startTopIndexHdr.numRecs == -1)
	{
		pDataFile->ReadFromFile((char *)&rightTopIndex, sizeof(struct IndexHdr), pVHeader->fPosRoot);
		rightTopIndex.fPosPrevRecord = NULL;
		pDataFile->WriteToFile(pVHeader->fPosRoot, (char *)&rightTopIndex, sizeof(struct IndexHdr));
		UpdateIndexPageDescriptor(pVHeader->fPosRoot, &rightTopIndex);
	}
	else
	{
		pDataFile->ReadFromFile((char *)&rightTopIndex, sizeof(struct IndexHdr), topIndexHdr.fPosNextRecord);
		rightTopIndex.fPosPrevRecord = NULL;
		pDataFile->WriteToFile(topIndexHdr.fPosNextRecord, (char *)&rightTopIndex, sizeof(struct IndexHdr));
		UpdateIndexPageDescriptor(topIndexHdr.fPosNextRecord, &rightTopIndex);
	}

	long long fPosPrev = 0L;

	if (startTopIndexHdr.numRecs < 0 && startTopIndexHdr.fPosPrevRecord == NULL)
	{
		branchIndexHdr.fPosNextRecord = insIndexHdr.fPosNextRecord;
		branchIndexHdr.fPosPrevRecord = NULL;
		fPosPrev = InsertRootBranchIndexRecord(&destPositionMarker.channelNum, &branchIndexHdr, NULL, NULL, pChannelData->GetNumMonitors(), this, false, NULL);
	}
	else
	{
		// insert branch below top index header
		topIndexHdr.fPosNextRecord = insIndexHdr.fPosNextRecord;
		fPosPrev = InsertRootBranchIndexRecord(&destPositionMarker.channelNum, &branchIndexHdr, &topIndexHdr, startPositionMarker.fPosIndex, pChannelData->GetNumMonitors(), this, false, NULL);
	}
	// Right Branch Top record to point to Branch
	rightTopIndex.fPosPrevRecord = fPosPrev;
	pDataFile->WriteToFile(branchIndexHdr.dataRecs[0].fPosData, (char *)&rightTopIndex, sizeof(struct IndexHdr));
	UpdateIndexPageDescriptor(branchIndexHdr.dataRecs[0].fPosData, &rightTopIndex);

	bottomIndexHdr.fPosPrevRecord = fPosPrev;
	pDataFile->WriteToFile(insIndexHdr.fPosNextRecord, (char *)&bottomIndexHdr, sizeof(struct IndexHdr));
	UpdateIndexPageDescriptor(insIndexHdr.fPosNextRecord, &bottomIndexHdr);
	return fPosPrev; // return the postion of the BranchIndexHdr
}

struct indexPageDescriptor *FileContext::FindIndexPageDescriptorForEvent(ULONG nEventID)
{
	struct indexPageDescriptor *pIndexDescriptor = NULL;
	struct indexPageDescriptor *pIndexDescriptorFound = NULL;
	//struct indexPageDescriptor *pLastVideoIndexDescriptor = NULL;

	map <long long, struct indexPageDescriptor *>::iterator itMap = mapVideoPages.begin();
	while (itMap != mapVideoPages.end())
	{
		pIndexDescriptor = itMap->second;
		if (pIndexDescriptor->nPageType == 1 || pIndexDescriptor->nPageType == 2)
		{
			for (int i = 0; i <= pIndexDescriptor->numRecs; i++)
			{
				if (pIndexDescriptor->dataRecs[i].indexNum == nEventID)
				{
					pIndexDescriptorFound = pIndexDescriptor;
					break;
				}
			}
			if (pIndexDescriptorFound != NULL)
				break;
		}
		itMap++;
	}

	if (pIndexDescriptorFound != NULL && pIndexDescriptorFound->nPageType == 2)
	{
		// to previous VideoIndex Descriptor
		pIndexDescriptorFound = FindPreviousVideoIndexDescriptor(pIndexDescriptorFound);
	}

	return pIndexDescriptorFound;
}

struct indexPageDescriptor *FileContext::FindIndexPageDescriptorForBranch(ULONG nEventID)
{
	struct indexPageDescriptor *pIndexDescriptor = NULL;
	struct indexPageDescriptor *pIndexDescriptorFound = NULL;
	//struct indexPageDescriptor *pLastVideoIndexDescriptor = NULL;

	map <long long, struct indexPageDescriptor *>::iterator itMap = mapVideoPages.begin();
	while (itMap != mapVideoPages.end())
	{
		pIndexDescriptor = itMap->second;
		if (pIndexDescriptor->nPageType == 1 || pIndexDescriptor->nPageType == 2)
		{
			for (int i = 0; i <= pIndexDescriptor->numRecs; i++)
			{
				if (pIndexDescriptor->dataRecs[i].indexNum == nEventID)
				{
					pIndexDescriptorFound = pIndexDescriptor;
					break;
				}
			}
			if (pIndexDescriptorFound != NULL)
				break;
		}
		itMap++;
	}

	return pIndexDescriptorFound;
}

struct indexPageDescriptor *FileContext::FindPreviousVideoIndexDescriptor(struct indexPageDescriptor *pIndexDescriptor)
{
	long long fPos = 0;
	while (pIndexDescriptor != NULL && pIndexDescriptor->nPageType != 1)
	{
		if (pIndexDescriptor->nPageType == 2)
		{
			// Branch
			struct branchIndexDescriptor *pBranch = pIndexDescriptor->pageDescriptor.pBranch;
			fPos = pBranch->prevFpos;
		}
		else if (pIndexDescriptor->nPageType == 3)
		{
			// Join
			struct joinIndexDescriptor *pJoin = pIndexDescriptor->pageDescriptor.pJoin;
			if (pJoin->bPathDir)
				fPos = pJoin->prevRightFpos;
			else
				fPos = pJoin->prevTopFpos;
		}
		else if (pIndexDescriptor->nPageType == 5)
		{
			// Channel
			struct channelIndexDescriptor *pChannel = pIndexDescriptor->pageDescriptor.pChannel;

			fPos = pChannel->prevFpos;
		}
		
		pIndexDescriptor = NULL;
		map <long long, struct indexPageDescriptor *>::iterator itMap = mapVideoPages.find(fPos);
		if (itMap != mapVideoPages.end())
		{
			pIndexDescriptor = itMap->second;
		}
	}

	return pIndexDescriptor;
}
