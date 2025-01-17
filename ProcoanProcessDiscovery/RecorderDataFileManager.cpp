#include "stdafx.h"

#include "RecorderDataFileManager.h"
#include "WaveRecorder.h"
#include "ScreenRecorder.h"
#include "EventRecorder.h"
#include "RecordBufferManager.h"
#include <chrono>
#include <mutex>
#include "..\PecanLib\PecanLib.h"
#include "ConfigClass.h"
#include "RecorderWindow.h"
#include "ExecutionManager.h"
#include "Utility.h"
#include <unordered_map>
#include "spdlog/spdlog.h"
#include "json11.hpp"
using namespace json11;

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

using namespace std;

mutex lockDataManager;

DWORD WINAPI VideoThreadCaptureFunction(LPVOID lpParameter);
DWORD WINAPI ThreadWriteFileFunction(LPVOID lpParameter);
DWORD WINAPI AudioThreadFunction(LPVOID lpParameter);
DWORD WINAPI EventThreadFunction(LPVOID lpParameter);
int receiveall(int s, char *buf, ULONG *len);
extern HANDLE hSemaphore;

//DWORD WINAPI CaptureScreenDataThreadFunction(LPVOID lpParameter);

RecorderDataFileManager::RecorderDataFileManager(char *fileName)
{
	ZeroMemory(&rootRecord, sizeof(struct rootHdr));
	ZeroMemory(&mainChannelRecord, sizeof(struct channelHdr));
	ZeroMemory(&audioIndex, sizeof(struct IndexHdr));
	ZeroMemory(&videoIndex, sizeof(struct IndexHdr));
	ZeroMemory(&audioHeader, sizeof(struct AudioHdr));
	ZeroMemory(&videoHeader, sizeof(struct AudioHdr));
	ZeroMemory(&eventIndex, sizeof(struct IndexHdr));
	ZeroMemory(&eventHeader, sizeof(struct EventHdr));
	recStatus.audioValue = 0;
	recStatus.audioValues.clear();
	recStatus.currStatus = 0;
	recStatus.indexWritten = 0;
	recStatus.numIndex = 0;
	recStatus.recordTime = 0L;
	ConnectSocket = INVALID_SOCKET;

	recFileName = fileName;
	bRecordSound = true;
	bRecordEvents = true;
	bRecordingPaused = false;
	rootRecord.RecType = 'R';
	strcpy_s(rootRecord.signature, "PECAN1.0");
	rootRecord.noOfChannels = 1;
	
	mainChannelRecord.RecType = 'C';
	
	audioIndex.RecType = 'P';
	videoIndex.RecType = 'P';
	eventIndex.RecType = 'P';

	audioHeader.RecType = 'A';
	videoHeader.RecType = 'V';
	eventHeader.RecType = 'E';

	pDataFile = DBG_NEW DataFile(fileName);
	lastAudioIndexPosition = 0;
	audioIndexPos = 0L;
	lastVideoIndexPosition = 0;
	videoIndexPos = 0L;
	lastEventIndexPosition = 0L;
	eventIndexPos = 0L;

	pWavRecorder = NULL;
	pScreenRecorder = NULL;
	videoCaptureThread = NULL;
	videoCaptureThreadID = NULL;
	eventThread = NULL;
	eventThreadID = NULL;
	pEventRecorder = NULL;
	writeFileThread = NULL;
	writeFileThreadID = NULL;
	audioThread = NULL;
	audioThreadID = NULL;
	bRecordingOn = false;
	bRecordingStop = false;
	bVideoStopped = false;
	bAudioStopped = false;
	bEventStopped = false;
	bEventRecordInProcess = true;

	indexValue = 0L;
	
	fps = 10;
	bRecordVideo = true;
	ConfigClass *pConfigClass = GetConfigClass();
	if (pConfigClass != NULL)
	{
		fps = pConfigClass->getCaptureFrameRate();
		bRecordEvents = pConfigClass->getCaptureEvents();
		bRecordSound = pConfigClass->getCaptureAudio();
		bRecordVideo = pConfigClass->getCaptureVideo();
	}
	
	hRecorderHWND = GetRecorderWnd()->GetRecorderWindowHandle();
	pMonitorList = NULL;
}

void RecorderDataFileManager::SetCallBack(IMouseEventCallback *callBack)
{
	pRecorderWindow->SetCallBack(callBack);
}


void RecorderDataFileManager::SetVideoRecording(bool bFlg)
{
	bRecordVideo = bFlg;
}

void RecorderDataFileManager::SetAudioRecording(bool bFlg)
{
	bRecordSound = bFlg;
}

void RecorderDataFileManager::SetEventRecording(bool bFlg)
{
	bRecordEvents = bFlg;
}

void RecorderDataFileManager::GetRecorderStatus(struct RecorderStatusRecord *pRecStatus)
{
	pRecStatus->lockStruct.lock();
	recStatus.lockStruct.lock();
	pRecStatus->currStatus = recStatus.currStatus;
	pRecStatus->indexWritten = recStatus.indexWritten;
	pRecStatus->numIndex = recStatus.numIndex;
	pRecStatus->recordTime = recStatus.recordTime;
	//if (recStatus.audioValues.size() > 0)
	//{ 
	//	pRecStatus->audioValues.insert(pRecStatus->audioValues.end(), recStatus.audioValues.begin(), recStatus.audioValues.end());
	//	recStatus.audioValues.clear();
	//}
	recStatus.lockStruct.unlock();
	pRecStatus->lockStruct.unlock();

}

void RecorderDataFileManager::SetSelectedMonitorList(list <struct monitorDetails *> *pList)
{
	pMonitorList = pList;
}

void RecorderDataFileManager::GetEventRecord(struct eventRecord *eRec)
{
	if (pEventRecorder != NULL)
	{
		pEventRecorder->GetEventDetails(eRec);
	}
}

RecorderDataFileManager::~RecorderDataFileManager()
{
	if (pWavRecorder != NULL)
		StopRecording();
	delete pWavRecorder;
	delete pScreenRecorder;
	delete pEventRecorder;
	recStatus.lockStruct.lock();
	recStatus.currStatus = 4; // Done
	recStatus.lockStruct.unlock();
	Sleep(1000);
	DeleteConfigClass();
	// review if threads have to be closed

}

void RecorderDataFileManager::StartRecording()
{
	spdlog::info("Start Recording");
	indexValue = 0L;
	pRecordBufferManager = DBG_NEW RecordBufferManager();
	pWavRecorder = DBG_NEW WaveRecorder();
	pWavRecorder->SetDataManager(this);
	pWavRecorder->SetRecorderBufferManager(pRecordBufferManager);
	pWavRecorder->SelectDevice();
	pScreenRecorder = DBG_NEW ScreenRecorder();
	pScreenRecorder->SetDataManager(this);
	pScreenRecorder->SetRecordBufferManager(pRecordBufferManager);
	pScreenRecorder->SetSelectedMonitorList(pMonitorList);
	pScreenRecorder->GetScreenDataDetails();
	pScreenRecorder->SetIndexInterval(fps > 10 ? 10 : fps);
	ExecutionManager *pExecManager = GetExecutionManager();
	pauseTime = 0L;
	if (pExecManager != NULL)
	{
		pExecManager->StartWriteFileThread();
		Sleep(10);
		if (bRecordSound)
		{
			spdlog::info("Start Audio Recording");
			pExecManager->StartAudioRecording();
			Sleep(10);
		}
		else
			spdlog::info("No Audio Recording");
		//pExecManager->SetVideoRecording(bRecordVideo);
		//if (bRecordVideo)
		//{
			spdlog::info("Start Video Recording");
			pExecManager->StartVideoRecording();
			Sleep(10);
		//}
		//else
		//	spdlog::info("No Video Recording");

		//bRecordEvents = false;
		if (bRecordEvents)
		{
			spdlog::info("Start Event Recording");
			if (!bEventRecordInProcess)
			{
				pEventRecorder = DBG_NEW EventRecorder();
				pEventRecorder->SetDataManager(this);
				pEventRecorder->SetRecordBufferManager(pRecordBufferManager);
				pEventRecorder->SetHookHwnd(hRecorderHWND);
				pEventRecorder->SetHooks();
				pExecManager->StartEventRecording();
				Sleep(10);
			}
			else
			{
				// start Event Recorder process
				string eventProcessPath = getcurrentpath();
				eventProcessPath += "NxtScopeEventProcessor.exe";
				wstring eventPath(eventProcessPath.begin(), eventProcessPath.end());
				// set the size of the structures
				ZeroMemory(&si, sizeof(si));
				si.cb = sizeof(si);
				ZeroMemory(&pi, sizeof(pi));
				startup(eventPath.c_str(), &si, &pi);
				SetupSocket();
			}
		}
		else
			spdlog::info("No Event Recording");

	}

	lockDataManager.lock();
	bRecordingOn = true;
	bRecordingStop = false;
	bRecordingPaused = false;
	lockDataManager.unlock();
	recStatus.lockStruct.lock();
	recStatus.currStatus = 1; // Recording
	recStatus.indexWritten = 0;
	recStatus.lockStruct.unlock();
	spdlog::info("Recording Started");
}

bool RecorderDataFileManager::SetupSocket()
{
	WSADATA wsaData;
	ConnectSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL, *ptr = NULL;
	struct addrinfo hints;

	int iResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		return false;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo("localhost", DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		WSACleanup();
		return false;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			WSACleanup();
			return false;
		}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}
	freeaddrinfo(result);
	ULONG lenRecv = 8L;
	SendEventProcessCommand("START",lenRecv);
	return true;
}

int receiveall(int s, char *buf, ULONG *len)
{
	ULONG total = 0;        // how many bytes we've received
	ULONG bytesleft = *len; // how many we have left to receive
	LONG n = -1;

	while (total < *len) {
		n = recv(s, buf + total, bytesleft, 0);
		if (n <= 0) { break; }
		total += n;
		bytesleft -= n;
	}

	*len = total; // return number actually received here

	return (n <= 0) ? -1 : 0; // return -1 on failure, 0 on success
}

bool RecorderDataFileManager::SendEventProcessCommand(char *str, ULONG recvLen)
{
	if (ConnectSocket == INVALID_SOCKET) {
		WSACleanup();
		return false;
	}
	int iResult;

	string strCmd = str;

	iResult = send(ConnectSocket, strCmd.c_str(), (int)strCmd.size(), 0);
	if (iResult == SOCKET_ERROR) {
		closesocket(ConnectSocket);
		WSACleanup();
		return false;
	}

	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	ZeroMemory(recvbuf, recvbuflen);
	// Receive until the peer closes the connection
	receiveall(ConnectSocket, recvbuf, &recvLen);
	strCmd += "ACK";
	string strRecv = recvbuf;
	if (strCmd != strRecv)
		return false;

	return true;
}

void RecorderDataFileManager::PauseRecording()
{
	spdlog::info("Start Recording Pause");
	pauseStartTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	recStatus.lockStruct.lock();
	recStatus.currStatus = 2; // Paused
	recStatus.lockStruct.unlock();
	lockDataManager.lock();
	bRecordingPaused = true;
	lockDataManager.unlock();
	ULONG lenRecv = 8L;
	if (!bEventRecordInProcess)
		SendEventProcessCommand("PAUSE", lenRecv);
	if (pWavRecorder != NULL)
		pWavRecorder->PauseRecording();
	spdlog::info("Recording Paused");
}

void RecorderDataFileManager::RestartRecording()
{
	spdlog::info("Start Restart Recording");
	recStatus.lockStruct.lock();
	recStatus.currStatus = 1; // Recording
	recStatus.lockStruct.unlock();
	pauseEndTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	pauseTime += (pauseEndTime - pauseStartTime);
	lockDataManager.lock();
	bRecordingPaused = false;
	lockDataManager.unlock();
	ULONG lenRecv = 10L;
	if (!bEventRecordInProcess)
		SendEventProcessCommand("RESTART", lenRecv);
	if (pWavRecorder != NULL)
		pWavRecorder->RestartRecording();
	spdlog::info("Recording Restarted");
}

void RecorderDataFileManager::StopRecording()
{
	spdlog::info("Stop Recording");
	if (recStatus.currStatus == 2)
	{
		pauseEndTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		pauseTime += (pauseEndTime - pauseStartTime);
	}
	lockDataManager.lock();
	LONG dwCount = 0;
	bAudioStopped = false;
	bVideoStopped = false;
	bRecordingPaused = false;
	bRecordingStop = true;
	lockDataManager.unlock();
	recStatus.lockStruct.lock();
	recStatus.currStatus = 3; // Stopped
	recStatus.lockStruct.unlock();
	ReleaseSemaphore(hSemaphore, 1, &dwCount);

	ExecutionManager *pExecManager = GetExecutionManager();
	if (pExecManager != NULL)
	{
		if (bRecordEvents)
		{
			if (!bEventRecordInProcess)
			{
				lockDataManager.lock();
				bEventStopped = false;
				lockDataManager.unlock();
				pEventRecorder->SignalEvent();
				pEventRecorder->ClearHooks();
				while (true)
				{
					lockDataManager.lock();
					if (bEventStopped)
					{
						lockDataManager.unlock();
						break;
					}
					lockDataManager.unlock();
					Sleep(10);
				}
				//TerminateThread(eventThread, 0);
				pExecManager->StopEventRecording();
				Sleep(100);
			}
			else
			{
				CloseEventProcess();
				lockDataManager.lock();
				bEventStopped = true;
				lockDataManager.unlock();
			}
		}
		if (!bRecordSound)
		{
			lockDataManager.lock();
			bAudioStopped = true;
			lockDataManager.unlock();
		}
		if (!bRecordVideo)
		{
			lockDataManager.lock();
			bVideoStopped = true;
			lockDataManager.unlock();
		}

		while (true)
		{
			lockDataManager.lock();
			bool bFlg = bVideoStopped;
			lockDataManager.unlock();
			if (bFlg)
				break;
			Sleep(10);
		}

		while (true)
		{
			lockDataManager.lock();
			bool bFlg = bEventStopped;
			lockDataManager.unlock();
			if (bFlg)
				break;
			Sleep(10);
		}
		while (true)
		{
			lockDataManager.lock();
			bool bFlg = bRecordingStop;
			lockDataManager.unlock();
			if (bFlg)
				break;
			Sleep(10);
		}
		while (true)
		{
			lockDataManager.lock();
			bool bFlg = bAudioStopped;
			lockDataManager.unlock();
			if (bFlg)
				break;
			Sleep(10);
		}
		spdlog::info("All threads stopped");
		spdlog::flush_on(spdlog::level::info);
		pExecManager->StopAudioRecording();
		Sleep(100);
		pExecManager->StopVideoRecording();
		Sleep(100);
		pRecordBufferManager->SignalWriteThread();
		pExecManager->StopWriteFileThread();
		Sleep(100);
	}
	spdlog::info("All threads stopped");
	spdlog::flush_on(spdlog::level::info);
	if (bEventRecordInProcess)
	{
		CreateEventRecordsFromEventProcess();
	}
	spdlog::info("Event Processes done");
	delete pWavRecorder;
	delete pRecordBufferManager;
	delete pEventRecorder;
	pEventRecorder = NULL;
	pWavRecorder = NULL;
	delete pDataFile;
	pDataFile = NULL;
	EndEventProcess();
	SaveRecordingFile((char *)recFileName.c_str());
	spdlog::info("Recording Stopped");
}

void RecorderDataFileManager::CreateEventRecordsFromEventProcess()
{
	if (ConnectSocket == INVALID_SOCKET) {
		WSACleanup();
		return;
	}
	int iResult;

	string strCmd = "EVENTSIZE";
	spdlog::info("Getting Size of Events");
	spdlog::flush_on(spdlog::level::info);
	iResult = send(ConnectSocket, strCmd.c_str(), (int)strCmd.size(), 0);
	if (iResult == SOCKET_ERROR) {
		closesocket(ConnectSocket);
		WSACleanup();
		return;
	}

	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	ZeroMemory(recvbuf, recvbuflen);
	// Receive until the peer closes the connection
	ULONG lenRecv = 19L;
	receiveall(ConnectSocket, recvbuf, &lenRecv);
	string strRetVal = recvbuf;
	ULONG sizeBuff = 0;
	if (strRetVal.substr(0, 9) == "EVENTSIZE")
	{
		sizeBuff = atol(recvbuf + 10);
	}
	else
		return;

	char *pBuffer = (char *)malloc((sizeof(char) * sizeBuff) + 1024L);

	spdlog::info("Getting Events");
	spdlog::flush_on(spdlog::level::info);

	strCmd = "GETEVENTS";
	iResult = send(ConnectSocket, strCmd.c_str(), (int)strCmd.size(), 0);
	if (iResult == SOCKET_ERROR) {
		closesocket(ConnectSocket);
		WSACleanup();
		return;
	}
	lenRecv = sizeBuff + 13;
	ZeroMemory(pBuffer, sizeBuff + 1024);
	receiveall(ConnectSocket, pBuffer, &lenRecv);
	strRetVal = pBuffer;
	if (strRetVal.substr(0, 12) == "GETEVENTSACK")
	{
		strRetVal = pBuffer + 13;
	}
	else
		return;
	free(pBuffer);
	pBuffer = NULL;
	// Parse the event list JSON
	spdlog::info("Writing Events");
	spdlog::flush_on(spdlog::level::info);
	WriteEventRecordsFromProcess((char *)strRetVal.c_str());

	strCmd = "WIDGETSIZE";
	spdlog::info("Getting Widget Details");
	spdlog::flush_on(spdlog::level::info);

	iResult = send(ConnectSocket, strCmd.c_str(), (int)strCmd.size(), 0);
	if (iResult == SOCKET_ERROR) {
		closesocket(ConnectSocket);
		WSACleanup();
		return;
	}
	lenRecv = 20;
	ZeroMemory(recvbuf, recvbuflen);
	// Receive until the peer closes the connection
	receiveall(ConnectSocket, recvbuf, &lenRecv);
	strRetVal = recvbuf;
	sizeBuff = 0;
	if (strRetVal.substr(0, 10) == "WIDGETSIZE")
	{
		sizeBuff = atol(recvbuf + 11);
	}
	else
		return;

	pBuffer = (char *)malloc((sizeof(char) * sizeBuff) + 1024);
	strCmd = "GETWIDGETS";
	iResult = send(ConnectSocket, strCmd.c_str(), (int)strCmd.size(), 0);
	if (iResult == SOCKET_ERROR) {
		closesocket(ConnectSocket);
		WSACleanup();
		return;
	}
	lenRecv = sizeBuff + 14;
	ZeroMemory(pBuffer, sizeBuff + 1024);
	receiveall(ConnectSocket, pBuffer, &lenRecv);
	strRetVal = pBuffer;
	if (strRetVal.substr(0, 13) == "GETWIDGETSACK")
	{
		strRetVal = pBuffer + 14;
	}
	else
		return;

	spdlog::info(strRetVal.c_str());
	free(pBuffer);


	// Update Widget Map Data
	pDataFile->ReadFromFile((char *)&rootRecord, sizeof(struct rootHdr), 0L);
	long long channelFpos = rootRecord.posChannelRecords;
	pDataFile->ReadFromFile((char *)&mainChannelRecord, sizeof(struct channelHdr), channelFpos);
	struct elementJSON jSONRec;
	jSONRec.size = strRetVal.length() + 1;
	jSONRec.fPosJSON = pDataFile->AppendDeflateBlockToFile((char *)strRetVal.c_str(), jSONRec.size, &jSONRec.compSize);
	mainChannelRecord.posElementJSON = pDataFile->AppendToFile((char *)&jSONRec, sizeof(struct elementJSON));
	pDataFile->WriteToFile(channelFpos, (char *)&mainChannelRecord, sizeof(struct channelHdr));

	spdlog::info("Shutting Down Event Process");
	spdlog::flush_on(spdlog::level::info);

	strCmd = "END";
	iResult = send(ConnectSocket, strCmd.c_str(), (int)strCmd.size(), 0);
	if (iResult == SOCKET_ERROR) {
		closesocket(ConnectSocket);
		WSACleanup();
		return;
	}
	lenRecv = 6;
	ZeroMemory(recvbuf, recvbuflen);
	// Receive until the peer closes the connection
	receiveall(ConnectSocket, recvbuf, &lenRecv);
	strRetVal = recvbuf;
	sizeBuff = 0;
	if (strRetVal.substr(0, 6) == "ENDACK")
	{
		;
	}

}


void RecorderDataFileManager::WriteEventRecordsFromProcess(char *jSONRec)
{
	string err;
	Json jsonElements = Json::parse(jSONRec, err);
	spdlog::info("Event List Tree is {}", jSONRec);
	bool bFirstItem = true;

	if (jsonElements.is_array())
	{
		struct bufferRecord *pRecBuffer;
		struct eventBufferRecord *pEventBuffer;
		pRecBuffer = (struct bufferRecord *)pRecordBufferManager->GetFreeEventBuffer();
		lastEventIndexPosition = NULL;
		vector <Json> events = jsonElements.array_items();
		for (size_t i = 0; i < events.size(); i++)
		{
			spdlog::info("Writing Captured Events");
			Json eventDetail = events[i];
			pEventBuffer = (struct eventBufferRecord *)pRecBuffer->buffer;
			pEventBuffer->nEventID = eventDetail["EventID"].int_value();
			ULONG timeHi = eventDetail["TimestampH"].int_value();
			ULONG timeLo = eventDetail["TimestampL"].int_value();
			pEventBuffer->timeStamp = ((long long)timeHi * (long long)4000000000L) + (long long)timeLo;
			pRecBuffer->timeStamp = pEventBuffer->timeStamp;
			pEventBuffer->nEventCode = eventDetail["EventCode"].int_value();
			pEventBuffer->mouseX = eventDetail["MouseX"].int_value();
			pEventBuffer->mouseY = eventDetail["MouseY"].int_value();
			pEventBuffer->monitorNum = (HMONITOR)eventDetail["Monitor"].int_value();
			pEventBuffer->boundingRect.left = eventDetail["RectLeft"].int_value();
			pEventBuffer->boundingRect.top = eventDetail["RectTop"].int_value();
			pEventBuffer->boundingRect.right = eventDetail["RectRight"].int_value();
			pEventBuffer->boundingRect.bottom = eventDetail["RectBottom"].int_value();
			string val = eventDetail["Descriptor"].string_value();
			ZeroMemory(pEventBuffer->strDescriptor, 2000);
			CopyMemory(pEventBuffer->strDescriptor, val.c_str(), val.size());
			val = eventDetail["Keys"].string_value();
			ZeroMemory(pEventBuffer->strKeyStrokes, 512);
			CopyMemory(pEventBuffer->strKeyStrokes, val.c_str(), val.size());
			pRecBuffer->sizeOfBuffer = sizeof(struct bufferRecord) + sizeof(struct eventBufferRecord) + 32;
			ULONG vNum = GetVideoIndexNumber(pEventBuffer->timeStamp, bFirstItem);
			bFirstItem = false;
			if (vNum != 0L)
				WriteProcessEventsInFile(pRecBuffer, vNum);
		}

		// Write the last event index in file.
		eventIndex.numRecs--;
		long long fPos = 0L;
		if (eventIndex.numRecs >= 0)
		{
			fPos = pDataFile->AppendToFile((char *)&eventIndex, sizeof(struct IndexHdr));
			if (lastEventIndexPosition != 0L)
			{
				pDataFile->WriteToFile(lastEventIndexPosition + sizeof(time_t) + sizeof(char), (char *)&fPos, sizeof(long long));
			}
			else
			{
				// this the first Event record being written, this is the root
				eventHeader.fPosRoot = fPos;
			}
			lastEventIndexPosition = fPos;
		}

		ZeroMemory(&eventIndex, sizeof(struct IndexHdr));
		eventIndex.RecType = 'P';
		eventIndexPos = pDataFile->AppendToFile((char *)&eventHeader, sizeof(struct EventHdr));
		mainChannelRecord.posEventRoot = eventIndexPos;
		pDataFile->WriteToFile(rootRecord.posChannelRecords, (char *)&mainChannelRecord, sizeof(struct channelHdr));
		pRecordBufferManager->FreeBuffer(pRecBuffer);
	}
}

LONG RecorderDataFileManager::GetVideoIndexNumber(time_t tsVal, bool bFirst)
{
	if (bFirst)
	{
		// Read the first record and store index record and index num = 0;
		pDataFile->ReadFromFile((char *)&rootRecord, sizeof(struct rootHdr), 0L);
		long long channelFpos = rootRecord.posChannelRecords;
		pDataFile->ReadFromFile((char *)&mainChannelRecord, sizeof(struct channelHdr), channelFpos);
		ZeroMemory(&videoHeader, sizeof(struct VideoHdr));
		ZeroMemory(&eventIndex, sizeof(struct IndexHdr));
		lastEventIndexPosition = eventIndexPos = 0L;
		if (mainChannelRecord.posVideoRoot != NULL)
		{
			pDataFile->ReadFromFile((char *)&videoHeader, sizeof(struct VideoHdr), mainChannelRecord.posVideoRoot);
			videoIndexPos = videoHeader.fPosRoot;
			if (videoIndexPos != NULL)
				pDataFile->ReadFromFile((char *)&videoIndex, sizeof(struct IndexHdr), videoIndexPos);
			videoIndexNum = 0;
		}
	}
	// For current index record and index num
	// if video record timestamp >= tsval then set index numbers
	// else inc index num and get index record etc till end of file.
	if (videoIndexPos != NULL)
	{
		while (true)
		{
			if (videoIndex.dataRecs[videoIndexNum].timestamp >= tsVal)
			{
				videoIndexNum++;
				// found index value;
				return videoIndex.dataRecs[videoIndexNum - 1].videoIndexNumber;
			}
			if (videoIndexNum >= videoIndex.numRecs)
			{
				videoIndexNum = -1;
				videoIndexPos = videoIndex.fPosNextRecord;
				if (videoIndexPos == 0L)
				{
					videoIndexNum = 0;
					break;
				}
				pDataFile->ReadFromFile((char *)&videoIndex, sizeof(struct IndexHdr), videoIndexPos);
			}
			videoIndexNum++;
		}
	}
	else
		videoIndexNum = -1;
	return videoIndexNum;
}

void RecorderDataFileManager::CloseEventProcess()
{
	long lenRecv = 7L;
	SendEventProcessCommand("STOP", lenRecv);
}

void RecorderDataFileManager::EndEventProcess()
{
	int iResult;
	long lenRecv = 6L;
	SendEventProcessCommand("END", lenRecv);
	iResult = shutdown(ConnectSocket, SD_SEND);
	closesocket(ConnectSocket);
	WSACleanup();
}

void RecorderDataFileManager::SaveRecordingFile(char *fileName)
{
	DataFile *pOldFile = NULL;
	DataFile *pNewFile = NULL;
	
	spdlog::info("Start Save Recording");
	int len = strlen(fileName);
	char *backupFileName = (char *)malloc(len + 32);
	ZeroMemory(backupFileName, len + 32);
	for (int i = 0; i < len; i++)
	{
		if (fileName[i] == '.')
			break;
		backupFileName[i] = fileName[i];
	}
	strcat_s(backupFileName, len + 31, "REC.bak");
	remove(backupFileName);
	rename(fileName, backupFileName);

	pOldFile = DBG_NEW DataFile(backupFileName, FALSE);
	pNewFile = DBG_NEW DataFile(fileName, TRUE);
//	pOldFile->SetZipMode(false);
//	pNewFile->SetZipMode(true);
	

	struct rootHdr saveRootRecord;
	struct AudioHdr saveAudioHeader;
	struct VideoHdr saveVideoHeader;
	struct EventHdr saveEventHeader;
	long long fPosAudioHeader = 0L;
	long long fPosVideoHeader = 0L;
	long long fPosEventHeader = 0L;
	struct IndexHdr saveAudioIndex;
	struct IndexHdr saveVideoIndex;
	struct IndexHdr saveEventIndex;

	int screenWidth, screenHeight;

	ZeroMemory(&saveAudioHeader, sizeof(struct AudioHdr));
	ZeroMemory(&saveVideoHeader, sizeof(struct VideoHdr));
	ZeroMemory(&saveEventHeader, sizeof(struct EventHdr));

	ZeroMemory(&saveAudioIndex, sizeof(struct IndexHdr));
	ZeroMemory(&saveVideoIndex, sizeof(struct IndexHdr));
	ZeroMemory(&saveEventIndex, sizeof(struct IndexHdr));

	pOldFile->ReadFromFile((char *)&saveRootRecord, sizeof(struct rootHdr), 0L);
	if (saveRootRecord.RecType != 'R' || strcmp(saveRootRecord.signature, "PECAN1.0"))
	{
		return;
	}
	recStatus.lockStruct.lock();
	recStatus.numIndex = saveRootRecord.noOfVideoIndexPages;
	recStatus.indexWritten = 0L;
	recStatus.lockStruct.unlock();

	pNewFile->AppendToFile((char *)&saveRootRecord, sizeof(struct rootHdr));

	long long channelHeaderFPos = saveRootRecord.posChannelRecords;
	struct channelHdr channelHeader;
	struct channelHdr prevChannelHeader;
	struct channelHdr *prevChannelHdr = NULL;
	long long fChannelPos, fChannelPrevPos;

	fChannelPos = 0L;
	fChannelPrevPos = 0L;
	while (channelHeaderFPos != NULL)
	{
		spdlog::info("Writing Channel Header");
		// Copy the Channel Header
		pOldFile->ReadFromFile((char *)&channelHeader, sizeof(struct channelHdr), channelHeaderFPos);
		channelHeaderFPos = channelHeader.fPosNextChannel;

		fChannelPos = pNewFile->AppendToFile((char *)&channelHeader, sizeof(struct channelHdr));
		channelHeader.posCurrRec = fChannelPos;
		if (prevChannelHdr == NULL)
		{
			saveRootRecord.posChannelRecords = fChannelPos;
			pNewFile->WriteToFile(0L, (char *)&saveRootRecord, sizeof(struct rootHdr));
		}
		else
		{
			prevChannelHdr->fPosNextChannel = fChannelPos;
			pNewFile->WriteToFile(fChannelPrevPos, (char *)prevChannelHdr, sizeof(struct channelHdr));
		}

		if (channelHeader.posAudioRoot != NULL)
		{
			pOldFile->ReadFromFile((char *)&saveAudioHeader, sizeof(struct AudioHdr), channelHeader.posAudioRoot);
			channelHeader.posAudioRoot = pNewFile->AppendToFile((char *)&saveAudioHeader, sizeof(struct AudioHdr));
		}

		if (channelHeader.posEventRoot != NULL)
		{
			pOldFile->ReadFromFile((char *)&saveEventHeader, sizeof(struct EventHdr), channelHeader.posEventRoot);
			channelHeader.posEventRoot = pNewFile->AppendToFile((char *)&saveEventHeader, sizeof(struct EventHdr));
		}

		long buffSize = 0L;
		spdlog::info("Writing Monitor Info");
		if (channelHeader.posVideoRoot != NULL)
		{
			pOldFile->ReadFromFile((char *)&saveVideoHeader, sizeof(struct VideoHdr), channelHeader.posVideoRoot);
			channelHeader.posVideoRoot = pNewFile->AppendToFile((char *)&saveVideoHeader, sizeof(struct VideoHdr));

			for (int i = 0; i < saveVideoHeader.noMonitors; i++)
			{
				struct MonitorScreenData mDets;

				if (i == 0)
				{
					pOldFile->ReadFromFile((char *)&mDets, sizeof(struct MonitorHdr), saveVideoHeader.fPosMonitorDetails);
					saveVideoHeader.fPosMonitorDetails = pNewFile->AppendToFile((char *)&mDets, sizeof(struct MonitorHdr));
				}
				else
				{
					pOldFile->ReadFromFile((char *)&mDets, sizeof(struct MonitorHdr), -1L);
					pNewFile->AppendToFile((char *)&mDets, sizeof(struct MonitorHdr));
				}
				screenWidth = mDets.MonitorDetails.right - mDets.MonitorDetails.left;
				screenHeight = mDets.MonitorDetails.bottom - mDets.MonitorDetails.top;

				long tempBuffSize = (screenHeight) * (screenWidth) * 5;
				if (tempBuffSize > buffSize)
					buffSize = tempBuffSize;
			}
		}

		int blockAlign = 0;
		struct bufferRecord *pRecBuffer = NULL;
		char *audioBuffer = NULL;
		char *oldAudioBuffer = NULL;
		struct bufferRecord *pOldAudioBuffer = NULL;

		pRecBuffer = (struct bufferRecord *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, buffSize);
		//struct bufferRecord *pCompRecBuffer = NULL;
		if (saveAudioHeader.fPosRoot != NULL || saveVideoHeader.fPosRoot != NULL || channelHeader.posTaskListRoot != NULL)
		{
			//pCompRecBuffer = (struct bufferRecord *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, buffSize);
			if (saveAudioHeader.fPosRoot != NULL)
			{
				buffSize = (saveAudioHeader.freq * saveAudioHeader.noChannels * saveAudioHeader.bitsPerSample) / 8L;
				buffSize += sizeof(struct bufferRecord) + 16;
				blockAlign = (saveAudioHeader.noChannels * saveAudioHeader.bitsPerSample) / 8;
				pOldAudioBuffer = (struct bufferRecord *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, buffSize);
				pOldAudioBuffer->buffer = (char *)pOldAudioBuffer + sizeof(struct bufferRecord) + 8;

				audioBuffer = (char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, buffSize * 60L);
				oldAudioBuffer = (char *)pOldAudioBuffer;
			}
		}

		// Split Audio records as per Video Requirements
		fPosAudioHeader = saveAudioHeader.fPosRoot;
		ULONG audioBufferLen = 0L;
		ULONG audioBufferLenRead = 0L;
		if (saveVideoHeader.fPosRoot != NULL)
		{
			int numVideo = 0;
			int numAudioRead = 0;
			if (saveAudioHeader.fPosRoot != NULL)
			{
				// Read the Index Record for Audio
				pOldFile->ReadFromFile((char *)&saveAudioIndex, sizeof(struct IndexHdr), saveAudioHeader.fPosRoot);
			}
			// Read the Index Record
			fPosVideoHeader = saveVideoHeader.fPosRoot;
			struct IndexHdr prevVideoIndex;
			long long prevVideoIndexPos = 0L;
			ULONG audioDestBuffLen = 0L;
			int sizeOfOneSec = (saveAudioHeader.freq * saveAudioHeader.noChannels * saveAudioHeader.bitsPerSample) / 8;
			bool bDoneFlg = false;
			pOldFile->ReadFromFile((char *)&saveVideoIndex, sizeof(struct IndexHdr), fPosVideoHeader);
			if (fPosAudioHeader != NULL)
			{
				audioBufferLen = saveAudioIndex.dataRecs[numAudioRead].size;
				audioBufferLenRead = 0L;
				pOldFile->ReadDeflateBlockFromFile((char *)pOldAudioBuffer, audioBufferLen, saveAudioIndex.dataRecs[numAudioRead].compSize, saveAudioIndex.dataRecs[numAudioRead].fPosData);
				audioBufferLen -= sizeof(struct bufferRecord) + 16;
			}
			recStatus.lockStruct.lock();
			recStatus.indexWritten = 0;
			recStatus.lockStruct.unlock();

			while (true)
			{
				spdlog::info("Writing Audio & Video Records");
				pOldFile->ReadCompressedBlockFromFile((char *)pRecBuffer, saveVideoIndex.dataRecs[numVideo].compSize, saveVideoIndex.dataRecs[numVideo].fPosData);
//				pOldFile->ReadDeflateBlockFromFile((char *)pRecBuffer, saveVideoIndex.dataRecs[numVideo].size, saveVideoIndex.dataRecs[numVideo].compSize, saveVideoIndex.dataRecs[numVideo].fPosData);
//				pRecBuffer->buffer = (char *)pRecBuffer + sizeof(struct bufferRecord) + 8;
//				struct CompareBuffer *pCompBuffer = (struct CompareBuffer *)pRecBuffer->buffer;
				
				if (fPosAudioHeader != NULL && !bDoneFlg)
				{
					audioDestBuffLen = 0L;
					bDoneFlg = GetAudioBuffer(audioBuffer, oldAudioBuffer, &saveAudioIndex, &numAudioRead, sizeOfOneSec,
						&audioDestBuffLen, saveVideoIndex.dataRecs[numVideo].recDurationus,
						&audioBufferLen, &audioBufferLenRead, blockAlign, pOldFile);
					saveVideoIndex.dataRecs[numVideo].sizeAudio = audioDestBuffLen;
					saveVideoIndex.dataRecs[numVideo].fPosAudioData = NULL;
					if (audioDestBuffLen != 0)
					{
						saveVideoIndex.dataRecs[numVideo].fPosAudioData = pNewFile->AppendDeflateBlockToFile((char *)audioBuffer, audioDestBuffLen, &saveVideoIndex.dataRecs[numVideo].compAudioSize);
					}
				}

				saveVideoIndex.dataRecs[numVideo].fPosData = pNewFile->AppendCompressedBlockToFile((char *)pRecBuffer, saveVideoIndex.dataRecs[numVideo].compSize);
				//saveVideoIndex.dataRecs[numVideo].fPosData = pNewFile->AppendDeflateBlockToFile((char *)pRecBuffer, saveVideoIndex.dataRecs[numVideo].size, &saveVideoIndex.dataRecs[numVideo].compSize);
				numVideo++;
				if (numVideo > saveVideoIndex.numRecs)
				{
					long long pos = saveVideoIndex.fPosNextRecord;
					saveVideoIndex.fPosNextRecord = NULL;
					if (prevVideoIndexPos == NULL)
					{
						saveVideoIndex.fPosPrevRecord = NULL;
						saveVideoHeader.fPosRoot = pNewFile->AppendToFile((char *)&saveVideoIndex, sizeof(struct IndexHdr));
						prevVideoIndexPos = saveVideoHeader.fPosRoot;
					}
					else
					{
						saveVideoIndex.fPosPrevRecord = prevVideoIndexPos;
						prevVideoIndex.fPosNextRecord = pNewFile->AppendToFile((char *)&saveVideoIndex, sizeof(struct IndexHdr));
						pNewFile->WriteToFile(prevVideoIndexPos, (char *)&prevVideoIndex, sizeof(struct IndexHdr));
						prevVideoIndexPos = prevVideoIndex.fPosNextRecord;
					}
					CopyMemory(&prevVideoIndex, &saveVideoIndex, sizeof(struct IndexHdr));
					if (pos == 0L)
					{
						// end of file
						break;
					}
					else
					{
						pOldFile->ReadFromFile((char *)&saveVideoIndex, sizeof(struct IndexHdr), pos);
						numVideo = 0;
					}
					recStatus.lockStruct.lock();
					recStatus.indexWritten++;
					recStatus.lockStruct.unlock();
				}
			}
		}
		pNewFile->WriteToFile(channelHeader.posVideoRoot, (char *)&saveVideoHeader, sizeof(struct VideoHdr));

		if (saveEventHeader.fPosRoot != NULL)
		{
			// Read events
			fPosEventHeader = saveEventHeader.fPosRoot;
			struct IndexHdr prevEventIndex;
			long long prevEventIndexPos = 0L;

			pOldFile->ReadFromFile((char *)&saveEventIndex, sizeof(struct IndexHdr), fPosEventHeader);
			int numEvent = 0;
			while (true)
			{
				pOldFile->ReadDeflateBlockFromFile((char *)pRecBuffer, saveEventIndex.dataRecs[numEvent].size, saveEventIndex.dataRecs[numEvent].compSize, saveEventIndex.dataRecs[numEvent].fPosData);
				saveEventIndex.dataRecs[numEvent].fPosData = pNewFile->AppendDeflateBlockToFile((char *)pRecBuffer, saveEventIndex.dataRecs[numEvent].size, &saveEventIndex.dataRecs[numEvent].compSize);
				numEvent++;
				if (numEvent > saveEventIndex.numRecs)
				{
					long long pos = saveEventIndex.fPosNextRecord;
					saveEventIndex.fPosNextRecord = NULL;
					if (prevEventIndexPos == NULL)
					{
						saveEventIndex.fPosPrevRecord = NULL;
						saveEventHeader.fPosRoot = pNewFile->AppendToFile((char *)&saveEventIndex, sizeof(struct IndexHdr));
						prevEventIndexPos = saveEventHeader.fPosRoot;
					}
					else
					{
						saveEventIndex.fPosPrevRecord = prevEventIndexPos;
						prevEventIndex.fPosNextRecord = pNewFile->AppendToFile((char *)&saveEventIndex, sizeof(struct IndexHdr));
						pNewFile->WriteToFile(prevEventIndexPos, (char *)&prevEventIndex, sizeof(struct IndexHdr));
						prevEventIndexPos = prevEventIndex.fPosNextRecord;
					}
					CopyMemory(&prevEventIndex, &saveEventIndex, sizeof(struct IndexHdr));

					if (pos == 0L)
					{
						// end of file
						break;
					}
					else
					{
						pOldFile->ReadFromFile((char *)&saveEventIndex, sizeof(struct IndexHdr), pos);
						numEvent = 0;
					}
				}
			}
		}
		pNewFile->WriteToFile(channelHeader.posEventRoot, (char *)&saveEventHeader, sizeof(struct EventHdr));

		if (channelHeader.posElementJSON != NULL)
		{
			struct elementJSON jSONRec;
			pOldFile->ReadFromFile((char *)&jSONRec, sizeof(struct elementJSON), channelHeader.posElementJSON);

			pOldFile->ReadCompressedBlockFromFile((char *)pRecBuffer, jSONRec.compSize, jSONRec.fPosJSON);
			//pOldFile->ReadDeflateBlockFromFile((char *)pRecBuffer, jSONRec.size, jSONRec.compSize, jSONRec.fPosJSON);
			jSONRec.fPosJSON = pNewFile->AppendCompressedBlockToFile((char *)pRecBuffer, jSONRec.compSize);
			//jSONRec.fPosJSON = pNewFile->AppendDeflateBlockToFile((char *)pRecBuffer, jSONRec.size, &jSONRec.compSize);
			channelHeader.posElementJSON = pNewFile->AppendToFile((char *)&jSONRec, sizeof(struct elementJSON));

		}

		pNewFile->WriteToFile(fChannelPos, (char *)&channelHeader, sizeof(struct channelHdr));
		fChannelPrevPos = fChannelPos;
		prevChannelHdr = &prevChannelHeader;
		CopyMemory(prevChannelHdr, &channelHeader, sizeof(struct channelHdr));
		HeapFree(GetProcessHeap(), 0, pRecBuffer);
		pRecBuffer = NULL;
		if (audioBuffer != NULL)
			HeapFree(GetProcessHeap(), 0, audioBuffer);
		audioBuffer = NULL;
		if (oldAudioBuffer != NULL)
			HeapFree(GetProcessHeap(), 0, oldAudioBuffer);
		oldAudioBuffer = NULL;
	}

	delete pOldFile;
	delete pNewFile;
	delete backupFileName;
	Sleep(1000);
	spdlog::info("Recording Saved");
}

bool RecorderDataFileManager::GetAudioBuffer(char *destBuffer, char *srcBuffer, struct IndexHdr *indexPage, int *nIndex, int sizeOneSec,
	ULONG *lenDestBuffer, long long timeUs,
	ULONG *lenSrcBuffer, ULONG *lenSrcRead, int alignByteNum,
	DataFile *pOldFile)
{

	spdlog::info("Get Audio Buffer");

	static int overflow = 0;

	*lenDestBuffer = 0L;
	bool bDoneFlg = false;
	if (timeUs == 0)
		return bDoneFlg;

	long long timeVal = sizeOneSec * timeUs;
	
	ULONG bufferLen = (timeVal / 1000000L) + overflow;
	ULONG lenAlign = bufferLen % alignByteNum;
	bufferLen -= lenAlign; // subtract the mod to get the actual value of buffer length aligned as per PCM
	overflow = lenAlign;

	ULONG lenRead = *lenSrcRead;
	ULONG lenSrc = *lenSrcBuffer;
	if ((lenRead + bufferLen) > lenSrc)
	{
		ULONG copyLen = lenSrc - lenRead;
		CopyMemory(destBuffer, srcBuffer + sizeof(struct bufferRecord) + 8 + (*lenSrcRead), copyLen);
		bufferLen -= copyLen;
		*lenDestBuffer += copyLen;
		int numAudio = *nIndex;
		numAudio++;
		if (numAudio > indexPage->numRecs)
		{
			long long pos = indexPage->fPosNextRecord;
			if (pos != 0L)
			{
				pOldFile->ReadFromFile((char *)indexPage, sizeof(struct IndexHdr), pos);
				numAudio = 0;
			}
			else
				bDoneFlg = true;
		}
		if (!bDoneFlg)
		{
			while (true)
			{
				*lenSrcBuffer = indexPage->dataRecs[numAudio].size;
				pOldFile->ReadDeflateBlockFromFile((char *)srcBuffer, *lenSrcBuffer, indexPage->dataRecs[numAudio].compSize, indexPage->dataRecs[numAudio].fPosData);
				*lenSrcBuffer -= (sizeof(struct bufferRecord) + 16);
				*lenSrcRead = 0L;
				copyLen = bufferLen;
				if (copyLen > *lenSrcBuffer)
				{
					copyLen = *lenSrcBuffer;
				}
				CopyMemory(destBuffer + (*lenDestBuffer), srcBuffer + sizeof(struct bufferRecord) + 8, copyLen);
				*lenDestBuffer += copyLen;
				*lenSrcRead += copyLen;
				bufferLen -= copyLen;
				if (bufferLen == 0 || (copyLen + (*lenDestBuffer)) >= ((ULONG)sizeOneSec * 60L))
					break;
				numAudio++;
				if (numAudio > indexPage->numRecs)
				{
					long long pos = indexPage->fPosNextRecord;
					if (pos != 0L)
					{
						pOldFile->ReadFromFile((char *)indexPage, sizeof(struct IndexHdr), pos);
						numAudio = 0;
					}
					else
						bDoneFlg = true;
				}
				if (bDoneFlg)
				{
					break;
				}
			}
		}
		*nIndex = numAudio;
	}
	else
	{
		CopyMemory(destBuffer, srcBuffer + sizeof(struct bufferRecord) + 8 + (*lenSrcRead), bufferLen);
		*lenSrcRead += bufferLen;
		*lenDestBuffer = bufferLen;
	}
	spdlog::info("Get Audio Buffer Done");
	return bDoneFlg;
}

void RecorderDataFileManager::StartAudioRecording()
{
	StartAudioThread();
}	

void RecorderDataFileManager::StartEventRecording()
{
	StartEventThread();
}

void RecorderDataFileManager::StartVideoRecording()
{
	StartVideoCaptureThread();
}

void RecorderDataFileManager::StartWriteFileThread()
{
	writeFileThread = CreateThread(0, 0, ThreadWriteFileFunction, this, 0, &writeFileThreadID);
}

void RecorderDataFileManager::StartVideoCaptureThread()
{
	videoCaptureThread = CreateThread(0, 0, VideoThreadCaptureFunction, this, 0, &videoCaptureThreadID);
}

void RecorderDataFileManager::StartAudioThread()
{
	audioThread = CreateThread(0, 0, AudioThreadFunction, this, 0, &audioThreadID);
}

void RecorderDataFileManager::StartEventThread()
{
	eventThread = CreateThread(0, 0, EventThreadFunction, this, 0, &eventThreadID);
}

DWORD WINAPI VideoThreadCaptureFunction(LPVOID lpParameter)
{
	RecorderDataFileManager *recFileManager = (RecorderDataFileManager *)lpParameter;

	recFileManager->RecordVideo();

	return 0;
}

DWORD WINAPI AudioThreadFunction(LPVOID lpParameter)
{
	RecorderDataFileManager *recFileManager = (RecorderDataFileManager *)lpParameter;

	recFileManager->RecordAudio();
	return 0;
}

DWORD WINAPI EventThreadFunction(LPVOID lpParameter)
{
	CoInitialize(NULL);

	RecorderDataFileManager *recFileManager = (RecorderDataFileManager *)lpParameter;
	recFileManager->RecordEvents();

	CoUninitialize();
	return 0;
}

DWORD WINAPI ThreadWriteFileFunction(LPVOID lpParameter)
{
	RecorderDataFileManager *recFileManager = (RecorderDataFileManager *)lpParameter;

	recFileManager->WriteFileThread();

	return 0;
}

void RecorderDataFileManager::WriteFileThread()
{
	lastAudioIndexPosition = 0;
	audioIndexPos = 0L;
	lastVideoIndexPosition = 0;
	videoIndexPos = 0L;
	lastEventIndexPosition = 0L;
	eventIndexPos = 0L;

	while (true)
	{
		lockDataManager.lock();
		bool bFlg = bRecordingOn;
		lockDataManager.unlock();
		if (bFlg)
			break;
		spdlog::info("Waiting for Recording to Start");
		Sleep(10);
	}
	mainChannelRecord.timestampStart = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	pDataFile->AppendToFile((char *)&rootRecord, sizeof(struct rootHdr));

	while (true)
	{
		spdlog::info("Checking for Data to Write");
		if (pRecordBufferManager->IsWriteBufferAvailable())
		{
			spdlog::info("Writing Data Record");
			struct bufferRecord *pRecordBuffer = pRecordBufferManager->GetWriteBuffer();
			if (pRecordBuffer == NULL)
			{
				// Check if thread needs to close then break;
				//pLogFile->WriteLibLog((char *)"ALERT: No record to write");
				lockDataManager.lock();
				bool bFlg = bRecordingStop && bVideoStopped && bAudioStopped && bEventStopped;
				lockDataManager.unlock();
				if (bFlg)
					break;
			}
			else
			{
				// else write the buffer;
				spdlog::info("Writing Record Buffer");
				if (pRecordBuffer->buffType == 2) // Audio Write Record
				{
					WriteAudioFile(pRecordBuffer);
				}
				else if (pRecordBuffer->buffType == 1)
				{
					WriteVideoFile(pRecordBuffer);
				}
				else if (pRecordBuffer->buffType == 3)
				{
					WriteEventFile(pRecordBuffer);
				}
				pRecordBufferManager->FreeBuffer(pRecordBuffer);
				spdlog::info("Writing Record Buffer Done");
			}
		}
	}
	mainChannelRecord.timestampEnd = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	mainChannelRecord.recDurationus = mainChannelRecord.timestampEnd - mainChannelRecord.timestampStart - pauseTime;
	
	// Write the last audio index in file.
	audioIndex.numRecs--;
	long long fPos = 0L;
	if (audioIndex.numRecs >= 0)
	{
		fPos = pDataFile->AppendToFile((char *)&audioIndex, sizeof(struct IndexHdr));
		if (lastAudioIndexPosition != 0L)
		{
			pDataFile->WriteToFile(lastAudioIndexPosition + sizeof(time_t) + sizeof(char), (char *)&fPos, sizeof(long long));
		}
		else
		{
			// this the first Audio record being written, this is the root
			audioHeader.fPosRoot = fPos;
		}
		lastAudioIndexPosition = fPos;
	}

	spdlog::info("Writing Remaining Recording Data");

	ZeroMemory(&audioIndex, sizeof(struct IndexHdr));
	audioIndex.RecType = 'P';
	audioIndexPos = pDataFile->AppendToFile((char *)&audioHeader, sizeof(struct AudioHdr));
	mainChannelRecord.posAudioRoot = audioIndexPos;

	videoIndex.numRecs--;
	if (videoIndex.numRecs >= 0)
	{
		fPos = pDataFile->AppendToFile((char *)&videoIndex, sizeof(struct IndexHdr));
		if (lastVideoIndexPosition != 0L)
		{
			pDataFile->WriteToFile(lastVideoIndexPosition + sizeof(time_t) + sizeof(char), (char *)&fPos, sizeof(long long));
		}
		else
		{
			// this the first Audio record being written, this is the root
			videoHeader.fPosRoot = fPos;
		}
		lastVideoIndexPosition = fPos;
	}
	// Write the last event index in file.
	eventIndex.numRecs--;
	if (eventIndex.numRecs >= 0)
	{
		fPos = pDataFile->AppendToFile((char *)&eventIndex, sizeof(struct IndexHdr));
		if (lastEventIndexPosition != 0L)
		{
			pDataFile->WriteToFile(lastEventIndexPosition + sizeof(time_t) + sizeof(char), (char *)&fPos, sizeof(long long));
		}
		else
		{
			// this the first Event record being written, this is the root
			eventHeader.fPosRoot = fPos;
		}
		lastEventIndexPosition = fPos;
	}

	ZeroMemory(&eventIndex, sizeof(struct IndexHdr));
	eventIndex.RecType = 'P';
	eventIndexPos = pDataFile->AppendToFile((char *)&eventHeader, sizeof(struct EventHdr));
	mainChannelRecord.posEventRoot = eventIndexPos;

	videoHeader.RecType = 'V';
	videoHeader.fps = fps;
	videoHeader.noMonitors = 0;
	videoHeader.fPosMonitorDetails = 0L;

	pScreenRecorder->WriteMonitorDetails();
	if (pEventRecorder)
	{
		string strElementJSON = pEventRecorder->GetWindowElementJSON();
		struct elementJSON jSONRec;
		jSONRec.size = strElementJSON.length() + 1;
		jSONRec.fPosJSON = pDataFile->AppendDeflateBlockToFile((char *)strElementJSON.c_str(), jSONRec.size, &jSONRec.compSize);
		mainChannelRecord.posElementJSON = pDataFile->AppendToFile((char *)&jSONRec, sizeof(struct elementJSON));
	}
	videoIndexPos = pDataFile->AppendToFile((char *)&videoHeader, sizeof(struct VideoHdr));
	mainChannelRecord.posVideoRoot = videoIndexPos;
	mainChannelRecord.lastIndexNumber = indexValue;
	long long IndexPos = pDataFile->AppendToFile((char *)&mainChannelRecord, sizeof(struct channelHdr));
	rootRecord.posChannelRecords = IndexPos;
	pDataFile->WriteToFile(0L, (char *)&rootRecord, sizeof(struct rootHdr));
	spdlog::info("Datafile Write Done");
}


void RecorderDataFileManager::RecordAudio()
{
	spdlog::info("Start Audio Recording");
	// Add code to synchronize start of thread
	HANDLE hTimer = CreateWaitableTimer(NULL, TRUE, NULL);
	LARGE_INTEGER liDueTime;
	__int64         qwDueTime;

	while (true)
	{
		lockDataManager.lock();
		bool bFlg = bRecordingOn;
		lockDataManager.unlock();
		if (bFlg)
			break;
		qwDueTime = -100000LL; // wait for 10 ms
		// Copy the relative time into a LARGE_INTEGER.
		liDueTime.LowPart = (DWORD)(qwDueTime & 0xFFFFFFFF);
		liDueTime.HighPart = (LONG)(qwDueTime >> 32);

		if (!SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0))
		{
			spdlog::info("Failed creating Audio Timer");
		}
		if (WaitForSingleObject(hTimer, INFINITE) != WAIT_OBJECT_0)
		{
			spdlog::info("Failed waiting for Audio Timer");
		}
		CancelWaitableTimer(hTimer);
	}
	// Create an unnamed waitable timer.

	audioIndex.timestamp = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	pWavRecorder->StartRecording();
	while (true)
	{
		lockDataManager.lock();
		bool bRecFlg = bRecordingOn;
		bool bStopFlg = bRecordingStop;
		lockDataManager.unlock();
		if (bRecFlg && !bStopFlg)
		{
			qwDueTime = -1000000LL; // wait for 100 ms
			// Copy the relative time into a LARGE_INTEGER.
			liDueTime.LowPart = (DWORD)(qwDueTime & 0xFFFFFFFF);
			liDueTime.HighPart = (LONG)(qwDueTime >> 32);

			if (!SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0))
			{
				spdlog::info("Failed creating Audio Timer");
			}
			if (WaitForSingleObject(hTimer, INFINITE) != WAIT_OBJECT_0)
			{
				spdlog::info("Failed waiting for Audio Timer");
			}
			CancelWaitableTimer(hTimer);
		}
		else
			break;
	}

	CloseHandle(hTimer);
	lockDataManager.lock();
	bAudioStopped = true;
	lockDataManager.unlock();
	pWavRecorder->StopRecording();
	spdlog::info("Audio Recording Stopped");
}

void RecorderDataFileManager::RecordEvents()
{
	//pEventRecorder->SetHooks();
	spdlog::info("Start of Record Event");
	pEventRecorder->StartEventResolverThread();
	while (true)
	{
		spdlog::info("Waiting for Event");
		if (pEventRecorder->IsEventReady()) // This will block if no event is ready.
		{
			lockDataManager.lock();
			bool bFlg = bRecordingStop;
			bool bPause = bRecordingPaused;
			lockDataManager.unlock();
			if (bFlg)
			{
				break;
			}
			spdlog::info("Event detected");

			pEventRecorder->RecordEvent(bPause);
		}
	}

	pEventRecorder->StopEventResolverThread();
	//pEventRecorder->ClearHooks();
	// Kill the Event Resolver thread here

	lockDataManager.lock();
	bEventStopped = true;
	lockDataManager.unlock();
	spdlog::info("End of Record Event");
}

HMONITOR RecorderDataFileManager::GetMonitorNum(POINT pt)
{
	return pScreenRecorder->GetMonitorNum(pt);
}


void RecorderDataFileManager::RecordVideo()
{
	spdlog::info("Start of Record Video");

	if (bRecordVideo)
	{
		// Add code to sync recording
		while (true)
		{
			lockDataManager.lock();
			bool bFlg = bRecordingOn;
			lockDataManager.unlock();
			if (bFlg)
				break;
			Sleep(10);
		}
		spdlog::info("Recording Started");
		videoIndex.timestamp = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		long timeInMS = 1000000 / fps;
		long timeInt = 0L;
		bool bFirst = true;
		time_t t1 = 0L;
		time_t t2 = 0L;
		time_t t3 = 0L;
		// Create an unnamed waitable timer.
		HANDLE hTimer = CreateWaitableTimer(NULL, TRUE, NULL);
		LARGE_INTEGER liDueTime;
		bool bPauseFirst = true;
		bool bWaitToggleFlag = false;

		while (true)
		{
			lockDataManager.lock();
			bool bFlg = bRecordingStop;
			bool bRecPausedFlg = bRecordingPaused;
			lockDataManager.unlock();
			if (bFlg)
				break;
			spdlog::info("Taking a snapshot");
			t3 = t1;
			t1 = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
			if (pRecordBufferManager->VideoPlayBufferFull() & !bWaitToggleFlag)
			{
				bWaitToggleFlag = true;

			}
			else if (!bRecPausedFlg && !bPauseFirst)
			{
				bWaitToggleFlag = false;
				recStatus.lockStruct.lock();
				recStatus.recordTime += (t1 - t3);
				recStatus.lockStruct.unlock();
				pScreenRecorder->SetRecDuration(t1 - t3);
				pScreenRecorder->CaptureScreens(bFirst);
				bFirst = false;
				bPauseFirst = false;
			}
			else if (!bRecPausedFlg)
			{
				bWaitToggleFlag = false;
				pScreenRecorder->SetRecDuration(0L);
				pScreenRecorder->CaptureScreens(bFirst);
				bFirst = false;
				bPauseFirst = false;
			}
			else
			{
				bWaitToggleFlag = false;
				bPauseFirst = true;
			}
			t2 = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
			timeInt = (long)(t2 - t1);
			long delayTime = 0L;
			if (bWaitToggleFlag)
			{
				delayTime = 100000L;
			}
			else
				if (timeInt > timeInMS)
				{
					delayTime = 100000L;
				}
				else
				{
					delayTime = timeInMS - timeInt;
				}

			if (NULL == hTimer)
			{
				Sleep(delayTime / 1000L);
				continue;
			}
			liDueTime.QuadPart = (delayTime) * -10; // time in 100 nano sec intervals
			if (!SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0))
			{
				Sleep(delayTime / 1000L);
				continue;
			}
			if (WaitForSingleObject(hTimer, INFINITE) != WAIT_OBJECT_0)
			{
				continue;
			}
			CancelWaitableTimer(hTimer);

		}
		CloseHandle(hTimer);
	}
	else
	{
		while (true)
		{
			lockDataManager.lock();
			bool bFlg = bRecordingOn;
			lockDataManager.unlock();
			if (bFlg)
				break;
			Sleep(10);
		}
		spdlog::info("Recording Started");
		videoIndex.timestamp = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		time_t t1 = videoIndex.timestamp;
		time_t t3 = 0L;

		//bool bFirst = true;
		while (true)
		{
			if (WaitForSingleObject(hSemaphore, INFINITE) != WAIT_OBJECT_0)
			{
				continue;
			}
			lockDataManager.lock();
			bool bFlg = bRecordingStop;
			//bool bRecPausedFlg = bRecordingPaused;
			lockDataManager.unlock();
			t3 = t1;
			t1 = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

			if (bFlg)
				break;
			spdlog::info("Taking a snapshot");
			pScreenRecorder->SetRecDuration(t1 - t3);
			pScreenRecorder->CaptureScreens(false);
		}
	}
	lockDataManager.lock();
	bVideoStopped = true;
	lockDataManager.unlock();
	spdlog::info("Record Video Done");
}

void RecorderDataFileManager::StopVideoRecording()
{
	WaitForSingleObject(videoCaptureThread, INFINITE);
	CloseHandle(videoCaptureThread);
}

void RecorderDataFileManager::StopWriteDataFile()
{
	WaitForSingleObject(writeFileThread, INFINITE);
	CloseHandle(writeFileThread);
}

void RecorderDataFileManager::WriteMonitorData(HMONITOR hMon, LPRECT lprcMonitor)
{
	spdlog::info("Writing Monitor Data");
	struct MonitorHdr monHdr;
	ZeroMemory((char *)&monHdr, sizeof(struct MonitorHdr));

	struct ScreenData *screen = NULL;
	screen = pScreenRecorder->GetScreenData(hMon);
	MONITORINFO monitorInfo;

	monHdr.RecType = 'M';
	monHdr.MonitorNumber = hMon;
	monHdr.top = 0;
	monHdr.left = 0;
	monHdr.bottom = screen->rcMonitor.bottom - screen->rcMonitor.top;
	monHdr.right = screen->rcMonitor.right - screen->rcMonitor.left;
	monitorInfo.cbSize = sizeof(MONITORINFO);
	//GetMonitorInfo(hMon, &monitorInfo);
	monHdr.virRect = screen->rcMonitor;
	long long pos = pDataFile->AppendToFile((char *)&monHdr, sizeof(struct MonitorHdr));
	if (videoHeader.fPosMonitorDetails == 0L)
	{
		videoHeader.fPosMonitorDetails = pos;
	}

	videoHeader.noMonitors++;

	spdlog::info("Monitor Left {}", lprcMonitor->left);
	spdlog::info("Monitor Top {}", lprcMonitor->top);
	spdlog::info("Monitor Right {}", lprcMonitor->right);
	spdlog::info("Monitor Bottom {}", lprcMonitor->bottom);

	spdlog::info("Writing Monitor Data Done");
}

void RecorderDataFileManager::StopAudioRecording()
{
	WaitForSingleObject(audioThread, INFINITE);
	CloseHandle(audioThread);
}

void RecorderDataFileManager::StopEventRecording()
{
	WaitForSingleObject(eventThread, INFINITE);
	CloseHandle(eventThread);
}

void RecorderDataFileManager::WriteVideoFile (struct bufferRecord *pRecBuffer)
{
	spdlog::info("Writing Video File");

	ULONG compressedLen = 0;
	struct CompareBuffer *pCompBuffer = (struct CompareBuffer *)pRecBuffer->buffer;
	DWORD len = 0;
	static bool bFirst = true;
	static time_t oldTimeStamp;	
	static time_t indexTimeDuration = 0L;
	
	if (pCompBuffer->numOfBlocks == -1)
		len = pCompBuffer->numBytes + sizeof(struct bufferRecord) + sizeof(struct CompareBuffer) + 16;
	else if (pCompBuffer->numOfBlocks == 0)
		len = sizeof(struct CompareBuffer) + sizeof(struct bufferRecord) + 16;
	else
		len = pCompBuffer->numBytes + sizeof(struct bufferRecord) + sizeof(struct CompareBuffer) + 16;

	pRecBuffer->unBuffDets.scrRec.size = len;

	videoIndex.recDurationus += pRecBuffer->recDurationus;

	if (pRecBuffer->buffType == 'I')
	{ 
		videoIndex.bFullImageIndex = 1;
		pRecBuffer->buffType = 1; // Restore Buff type to 1;
	}
	//pRecBuffer->timeStamp = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	long long fPosData = pDataFile->AppendDeflateBlockToFile((char *)pRecBuffer, len, &compressedLen);
	//lockDataManager.lock(); // No need to lock since it is used exclusively
	int i = videoIndex.numRecs;
	videoIndex.dataRecs[i].RecType = 'D';
	videoIndex.dataRecs[i].size = len;
	videoIndex.dataRecs[i].screenNum = pRecBuffer->unBuffDets.scrRec.hMon;
	videoIndex.dataRecs[i].fPosData = fPosData;
	videoIndex.dataRecs[i].compSize = compressedLen;
	indexValue++;
	videoIndex.dataRecs[i].videoIndexNumber = indexValue;
	videoIndex.dataRecs[i].audioIndexNumber = audioIndexValue;
	videoIndex.dataRecs[i].eventIndexNumber = eventIndexValue;
	videoIndex.dataRecs[i].timestamp = pRecBuffer->timeStamp;
	videoIndex.dataRecs[i].recStatus = 'A';
	videoIndex.dataRecs[i].recDurationus = pRecBuffer->recDurationus;
	videoIndexValue = indexValue;
	//lockDataManager.unlock();

	i++;
	if (i == NUM_DATA_POINTS)
	{
		rootRecord.noOfVideoIndexPages++;
		long long fPos = pDataFile->AppendToFile((char *)&videoIndex, sizeof(struct IndexHdr));
		
		if (lastVideoIndexPosition != 0L)
		{
			pDataFile->WriteToFile(lastVideoIndexPosition + sizeof(time_t) + sizeof(char), (char *)&fPos, sizeof(long long));
		}
		else
		{
			// this the first Video record being written, this is the root
			videoHeader.fPosRoot = fPos;
		}
		lastVideoIndexPosition = fPos;
		//lockDataManager.lock();
		ZeroMemory(&videoIndex, sizeof(struct IndexHdr));
		videoIndex.bFullImageIndex = 0;
		videoIndex.RecType = 'P';
		videoIndex.timestamp = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		videoIndex.numRecs = 0;
		videoIndex.fPosPrevRecord = lastVideoIndexPosition;
		//lockDataManager.unlock();
	}
	else
	{
		//lockDataManager.lock();
		videoIndex.numRecs++;
		//lockDataManager.unlock();
	}

	spdlog::info("Write Video File Done");
}

void RecorderDataFileManager::WriteAudioFile(struct bufferRecord * pData)
{
	spdlog::info("Writing Audio File");
	DWORD len = pData->sizeOfBuffer;

	ULONG compressedLen;
	pData->mouseX = 0;
	pData->mouseY = 0;
	int i = audioIndex.numRecs;

	//now draw this point
	long long fPosData = pDataFile->AppendDeflateBlockToFile((char *)pData, len, &compressedLen);
	//lockDataManager.lock();
	audioIndex.dataRecs[i].RecType = 'D';
	audioIndex.dataRecs[i].fPosData = fPosData;
	audioIndex.dataRecs[i].size = len;
	audioIndex.dataRecs[i].compSize = compressedLen;
	audioIndex.dataRecs[i].screenNum = (HMONITOR)0;
	audioIndex.dataRecs[i].timestamp = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	indexValue++;
	audioIndex.dataRecs[i].videoIndexNumber = videoIndexValue;
	audioIndex.dataRecs[i].audioIndexNumber = indexValue;
	audioIndex.dataRecs[i].eventIndexNumber = eventIndexValue;
	audioIndexValue = indexValue;
	//lockDataManager.unlock();
	i++;
	if (i == NUM_DATA_POINTS)
	{
		long long fPos = pDataFile->AppendToFile((char *)&audioIndex, sizeof(struct IndexHdr));
		if (lastAudioIndexPosition != 0L)
		{	
			pDataFile->WriteToFile(lastAudioIndexPosition + sizeof(time_t) + sizeof(char), (char *)&fPos, sizeof(long long));
		}
		else
		{
			// this the first Audio record being written, this is the root
			audioHeader.fPosRoot = fPos;
		}
		lastAudioIndexPosition = fPos;
		//lockDataManager.lock();
		ZeroMemory(&audioIndex, sizeof(struct IndexHdr));
		audioIndex.RecType = 'P';
		audioIndex.numRecs = 0;
		audioIndex.timestamp = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		audioIndex.fPosPrevRecord = lastAudioIndexPosition;
		//lockDataManager.unlock();
	}
	else
	{
		//lockDataManager.lock();
		audioIndex.numRecs++;
		//lockDataManager.unlock();
	}

	spdlog::info("Writing Audio File Done");
}

void RecorderDataFileManager::WriteEventFile(struct bufferRecord * pData)
{
	spdlog::info("Writing Event File");
	DWORD len = pData->sizeOfBuffer;
	ULONG compressedLen;

	pData->mouseX = pData->mouseY = 0;
	int i = eventIndex.numRecs;
	long long fPosData = pDataFile->AppendDeflateBlockToFile((char *)pData, len, &compressedLen);
	//lockDataManager.lock();
	eventIndex.dataRecs[i].RecType = 'D';
	eventIndex.dataRecs[i].fPosData = fPosData;
	eventIndex.dataRecs[i].size = len;
	eventIndex.dataRecs[i].compSize = compressedLen;
	eventIndex.dataRecs[i].screenNum = (HMONITOR)0;
	eventIndex.dataRecs[i].timestamp = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	indexValue++;
	eventIndex.dataRecs[i].audioIndexNumber = audioIndexValue;
	eventIndex.dataRecs[i].eventIndexNumber = indexValue;
	eventIndex.dataRecs[i].videoIndexNumber = videoIndexValue;
	eventIndexValue = indexValue;
	//lockDataManager.unlock();
	i++;
	if (i == NUM_DATA_POINTS)
	{
		long long fPos = pDataFile->AppendToFile((char *)&eventIndex, sizeof(struct IndexHdr));
		if (lastEventIndexPosition != 0L)
		{
			pDataFile->WriteToFile(lastEventIndexPosition + sizeof(time_t) + sizeof(char), (char *)&fPos, sizeof(long long));
		}
		else
		{
			// this the first Audio record being written, this is the root
			eventHeader.fPosRoot = fPos;
		}
		lastEventIndexPosition = fPos;
		//lockDataManager.lock();
		ZeroMemory(&eventIndex, sizeof(struct IndexHdr));
		eventIndex.RecType = 'P';
		eventIndex.numRecs = 0;
		eventIndex.timestamp = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		eventIndex.fPosPrevRecord = lastEventIndexPosition;
		//lockDataManager.unlock();
	}
	else
	{
		//lockDataManager.lock();
		eventIndex.numRecs++;
		//lockDataManager.unlock();
	}

	spdlog::info("Writing Event File Done");
}


void RecorderDataFileManager::WriteProcessEventsInFile(struct bufferRecord * pData, ULONG vIndex)
{
	spdlog::info("Writing Event File");
	DWORD len = pData->sizeOfBuffer;
	ULONG compressedLen;

	pData->mouseX = pData->mouseY = 0;
	long long fPosData = pDataFile->AppendDeflateBlockToFile((char *)pData, len, &compressedLen);
	int i = eventIndex.numRecs;
	//lockDataManager.lock();
	eventIndex.dataRecs[i].RecType = 'D';
	eventIndex.dataRecs[i].fPosData = fPosData;
	eventIndex.dataRecs[i].size = len;
	eventIndex.dataRecs[i].compSize = compressedLen;
	eventIndex.dataRecs[i].screenNum = (HMONITOR)0;
	eventIndex.dataRecs[i].timestamp = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	eventIndex.dataRecs[i].audioIndexNumber = vIndex;
	eventIndex.dataRecs[i].eventIndexNumber = vIndex;
	eventIndex.dataRecs[i].videoIndexNumber = vIndex;
	//lockDataManager.unlock();
	i++;
	if (i == NUM_DATA_POINTS)
	{
		long long fPos = pDataFile->AppendToFile((char *)&eventIndex, sizeof(struct IndexHdr));
		if (lastEventIndexPosition != 0L)
		{
			pDataFile->WriteToFile(lastEventIndexPosition + sizeof(time_t) + sizeof(char), (char *)&fPos, sizeof(long long));
		}
		else
		{
			// this the first Audio record being written, this is the root
			eventHeader.fPosRoot = fPos;
		}
		lastEventIndexPosition = fPos;
		//lockDataManager.lock();
		ZeroMemory(&eventIndex, sizeof(struct IndexHdr));
		eventIndex.RecType = 'P';
		eventIndex.numRecs = 0;
		eventIndex.timestamp = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		eventIndex.fPosPrevRecord = lastEventIndexPosition;
		//lockDataManager.unlock();
	}
	else
	{
		//lockDataManager.lock();
		eventIndex.numRecs++;
		//lockDataManager.unlock();
	}

	spdlog::info("Writing Event File Done");
}


void RecorderDataFileManager::SetAudioParams(int freq, int noChannels, int waveFormat, int bitsPerSample)
{
	audioHeader.RecType = 'A';
	audioHeader.freq = freq;
	audioHeader.noChannels = noChannels;
	audioHeader.wFormatTag = waveFormat;
	audioHeader.bitsPerSample = bitsPerSample;
}


