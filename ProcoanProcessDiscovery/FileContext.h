#pragma once
#include "MMHeader.h"
#include "VideoSegmentClass.h"
#include <wx/wx.h>
class DataFile;
class ChannelDataMapping;
class RecordBufferManager;
class AudioPlayerBuffer;
class BPMNElement;
class BPMNDecision;
class BPMNFacade;
class BPMNSubProcess;
class ScenarioDefintion;
struct eventFileRecord;


class FileContext
{

	bool bActiveMonitorOn;
	int playerWindowWidth;
	int playerWindowHeight;

	struct fileContext savedContext;

	USHORT nFileContextNum;
	DataFile *pDataFile;
	BPMNFacade *pBPMNFacade;

	map <USHORT, ChannelDataMapping *> channelDataMap;
	map <ULONG, ULONG> parentIndexMapping;

	list <BPMNElement *>segmentEvents;
	list <BPMNElement *>copySegmentEvents;
	list <BPMNElement *>branchRightEvents;
	list <BPMNElement *>insertSegmentEvents;
	list <long long> branchesCopied;
	list <long long> branchRecordUpdateList;

	map <ULONG, struct eventListElement *> *pEventMap;
	map <long long, struct indexPageDescriptor *> mapVideoPages;
	map <long long, struct indexPageDescriptor *> mapAudioPages;

	struct eventListElement *pEventListRoot;


	SHORT mainChannelNumber;
	SHORT currContextChannel;

	long long currVideoFPos;
	long long maxVideoBufferSize;
	long long maxAudioBufferSize;

	struct IndexHdr startTopIndexHdr;
	struct IndexHdr startBottomIndexHdr;
	struct IndexHdr endTopIndexHdr;
	struct IndexHdr endBottomIndexHdr;
	struct IndexHdr copyTopIndexHdr;
	struct IndexHdr copyBottomIndexHdr;
	struct IndexHdr destTopIndexHdr;
	struct IndexHdr destBottomIndexHdr;

	struct rootHdr rootRecord;
	AudioPlayerBuffer *pAudioPlayerBuffer;
	RecordBufferManager *pRecBufferManager;

	struct positionMarker startPositionMarker;
	struct positionMarker endPositionMarker;
	struct positionMarker destPositionMarker;
	void WriteImageBMP(int width, int height, char *pBuffer);
	void CreateChannelDataMapping();
	void SetChannelBufferManager(RecordBufferManager *pRecBuffManager);
	void CreateBPMNList();
	//Helper functions for video editing
	bool PrepareForOperation(bool bCopy, bool bCopyEnd);
	bool PrepareDestinationForOperation(VideoSegmentClass *pCopySegment);
	long long GetMarkerTime(struct positionMarker *pMarker);
	void PrepareVideoIndex(VideoSegmentClass *pCopySegment);
	void SplitIndex(long long fPos, int channelNum, struct IndexHdr *source, struct IndexHdr *destTop, struct IndexHdr *destBottom, int indexNum, bool bCreateScreens, int numMon);
	long long CreateSegmentEventListAndTime();
	long long GetEventsAndTimeInSegment(int channelNum, struct IndexHdr *pIndexHdr);
	void InsertEventInEventMap(BPMNElement *pInsPoint, BPMNElement *pNewEvent, bool bAddAbove);
	BPMNElement *FindEventInList(list <BPMNElement *> *pElementList, ULONG eventIndex);
	void DeleteEventsInList(list <BPMNElement *> *pElementList);
	void RemoveEventsInList(list <BPMNElement *> *pElementList);
	void ReplaceEventsInList(BPMNElement *pOldElement, BPMNElement *pNewElement);
	void DeleteVideoSegment();
	void UpdateIndexPageDescriptor(long long keyVal, struct IndexHdr *value);
	long long CreateInsertEventAndVideoList(long long *fPosBranchInsPoint, VideoSegmentClass *pCopySegment);
	long long CreateBottomBranchIndex(long long fPosRightBottom);
	long long InsertVideoIndexRecord(short *channelNum, struct IndexHdr *pCurrIndex, struct IndexHdr *pPrevIndex, long long posPrevIndex, int numMon, FileContext *fContext);
	long long InsertBranchIndexRecord(short *channelNum, struct IndexHdr *pCurrIndex, struct IndexHdr *pPrevIndex, long long posPrevIndex, int numMon, FileContext *fContext);
	long long InsertRightOrBottomBranchIndexRecords(short *channelNum, struct IndexHdr *pCurrIndex, struct IndexHdr *pPrevIndex, long long posPrevIndex, int numMon, FileContext *fContext, long long fPosStart, long long *fPosEnd, long long *fPosJoin);
	long long InsertChannelIndexRecord(short *channelNum, struct IndexHdr *pCurrIndex, struct IndexHdr *pPrevIndex, long long posPrevIndex, int numMon, FileContext *fContext);
	long long InsertIndexRecord(short *channelNum, struct IndexHdr *pCurrIndex, struct IndexHdr *pPrevIndex, long long posPrevIndex, int numMon, FileContext *fContext);
	long long InsertJoinIndexRecord(short *channelNum, struct IndexHdr *pCurrIndex, struct IndexHdr *pPrevIndex, long long posPrevIndex, int numMon);
	void ReadFromFile(char *buffer, int size, long long fPos);
	struct indexPageDescriptor *GetIndexDescriptor(long long fPos);
	int FindChannelInVideo(wxString strChannelKey);
	void InsertEventsInList(VideoSegmentClass *pCopySegment);
	void AddToEventList(list <BPMNElement *> *pEventList, BPMNElement *pElement, BPMNElement *pOldElement, BPMNElement *pSourceElement);
	long long GetInsertEventsAndVideoInSegment(struct IndexHdr *pIndexHdr, struct IndexHdr *pDestIndexHdr, FileContext *fContext);
	list <BPMNElement *>::iterator FindEventIteratorInList(list <BPMNElement *> **pElementList, ULONG eventIndex, bool bUpdateListArg);
	int CopyAudioAndVideoToChannel(struct channelHdr *pChannelHdrSource);
	//void CopyVideoAndAudioHeaders(struct channelHdr *pChannelHdr, struct channelHdr *pChannelHdrSource);
	void CopyVideoAndAudioHeaders(struct channelHdr *pChannelHdr, ChannelDataMapping *pChannelSrc);
	void InsertEventsToBranch(int nChannel, long long fPos, long long fPosNext);
	long long AddBranchIndex(long long fPosBranchInsPoint);
	void SetDescriptorEvents();
	void CopyRecordingFile(char *fileName);
	void PackEventData(struct eventFileRecord *pEventRecord, struct eventDetails *pEventDetails, char *pRecBuffer);
	void UnpackEventData(struct eventDetails *pEventDetails, struct eventFileRecord *pEventRecord, char *pRecBuffer);
	void CreateEventDetails(struct eventDetails *pEventDetails, BPMNElement *pElement);
	int GetElementRecordType(BPMNElement *pElement);
	struct eventListElement *ReadEventRecords(long long pos, char *pRecBuffer);
	struct eventListElement *ReadBranchEventRecords(long long pos, char *pRecBuffer, list <struct eventDetails *> *pLastEventList);

	long long CopyEventRecords(long long pos, char *pRecBuffer, DataFile *pOldFile, DataFile *pNewFile, struct channelHdr *pChannelHdr, ULONG branchIndex);
	void UpdateSourceAndDestinationParentRecords(BPMNElement *pSource, BPMNElement *pDest);
	void UpdateEventList(list<BPMNElement *> *pList);
	struct eventListElement *GetFirstEventListElement(BPMNElement *pElement);
	struct eventListElement *GetNextEventListElement(struct eventListElement *pEventListElement);
	struct indexPageDescriptor *GetNextVideoIndexDescriptor(struct indexPageDescriptor *pIndexDescriptor);
	ULONG GetInsertionElementIndex(struct indexPageDescriptor *pIndexDescriptor);
	struct branchIndexDescriptor *FindBranchDescriptor(long long fPos, ULONG nIndex, long long *fPosNext);
	void ClearEventsInPath();
	struct indexPageDescriptor *FindIndexPageDescriptorForEvent(ULONG nEventID);
	struct indexPageDescriptor *FindIndexPageDescriptorForBranch(ULONG nEventID);
	struct indexPageDescriptor *FindPreviousVideoIndexDescriptor(struct indexPageDescriptor *pIndexDescriptor);
	int AddNewChannel(ChannelDataMapping *pChannelSrc);
	HMONITOR GetScreenOfEvent(IndexHdr *videoIndex, int nIndex);
	bool GetScreenVideoBuffer(struct IndexHdr *pIndex, int *indexNum, HMONITOR hMon);
	void CopySegmentDetails();
	int GetVideoAtTimeWithFlg(long timePos, IndexHdr *videoIndex, int *numVideo, long long *pos, bool bUpdateFlg);
	long long CopyRightBranchToFile(DataFile *pNewFile, DataFile *pOldFile, struct IndexHdr *pVideoIndex, struct bufferRecord *pRecBuffer, char *audioBuffer, struct IndexHdr *pBranchPrevIndex);
	long long GetVideoBufferSize();
	long long GetAudioBufferSize();
	void UpdateEventDetailsFromContext(ULONG nIndex, ULONG oIndex, FileContext *fContext);
	bool GetEventDetail(ULONG nIndex, struct eventFileRecord *pEventRec, char *pBuffer);
	void GetBranchPositions(long long *fPosBranch);
	long long CreateBranchInsertEventAndVideoList(VideoSegmentClass *pCopySegment, bool bRight);
	long long InsertRootBranchIndexRecord(short *channelNum, struct IndexHdr *pCurrIndex, struct IndexHdr *pPrevIndex, long long posPrevIndex, int numMon, FileContext *fContext, bool bFlg, struct branchIndexDescriptor *pBranchRight);
	long long InsertRightIndexRecord(short *channelNum, struct IndexHdr *pCurrIndex, struct IndexHdr *pPrevIndex, long long posPrevIndex, int numMon, FileContext *fContext);
	struct joinIndexDescriptor *GetJoinDescriptor(struct branchIndexDescriptor *pBranch);
	void InsertEventsInNewBranch(VideoSegmentClass *pCopySegment, long long posNewBranch, long long posNewJoin);
	void UpdatePreviousIndexHdr(long long posPrevIndex, long long posAdded, struct IndexHdr *pPrevIndex, struct indexPageDescriptor *indexPageDesc);
	void DeleteEventsFromEventList(list <BPMNElement *> *pElementList);
public:
	FileContext();
	~FileContext();

	void SetActiveMonitorDisplay(bool bFlg);
	void SetPlayerWindowWidth(int nWidth);
	void SetPlayerWindowHeight(int nHeight);

	struct fileContext *GetContext();

	DataFile *GetDataFile();
	AudioPlayerBuffer *GetAudioPlayerBuffer();
	RecordBufferManager *GetRecordBufferManager();
	void SetRecordBufferManager(RecordBufferManager *pRecBuffManager);
	void SetEventMap(map <ULONG, struct eventListElement *> *pMap);
	void CreateBitmapMerge();
	void CreateScreenResizeBuffer(bool bActiveMonitorDisplay, int windowWidth, int windowHeight);
	RECT *GetBoundingRect(int eventIndex);

	void GetAudioHeaderData(struct AudioHdr *pAudioHdr);
	void GetVideoHeaderData(struct VideoHdr *pVideoHdr);
	bool GetNextAudioIndex(struct IndexHdr *pIndex);
	bool GetNextVideoIndex(struct IndexHdr *pIndex);
	void CreateFileContext(char *fileName);
	list <BPMNElement *> *GetBPMNDiagram();
	map <ULONG, struct eventListElement *> *GetElementMap();
	bool GetAudioBuffer(struct IndexHdr *pIndex, int *indexNum);
	bool GetVideoBuffer(struct IndexHdr *pIndex, int *indexNum);
	bool GetScreenVideoBuffer(struct IndexHdr *pIndex, int *indexNum);
	bool GetScreenThumbVideoBuffer(struct IndexHdr *pIndex, int *indexNum, struct bufferRecord **pBuffer);

	void GetScenarioData(struct scenarioReportData *pScenarioData, ScenarioDefintion *pScenarioDef);
	struct channelHdr *AddNewChannel();
	void SetFileContextNumber(int num);
	int GetFileContextNumber();
	int ProcessVideoRecord(struct bufferRecord *pBuffer, int currChannelNum, int lastMon);
	void GetChannelVideoIndex(int chNum, long long pos, struct IndexHdr *indexHdr);
	long long InitializeVideoPath();
	int GetVideoAtTime(long timePos, IndexHdr *videoIndex, int *numVideo, long long *pos);
	int GetThumbVideoAtTime(long timePos, IndexHdr *videoIndex, int *numVideo, long long *pos);
	void CreateVideoImage(int nChannel, IndexHdr *videoIndex, int numVideo, long long pos);
	void GetEventImage(int nChannel, IndexHdr *videoIndex, int numVideo, long long pos, vector<unsigned char>& buffer, ULONG& cx, ULONG& cy, ULONG nEventIndex, vector <BPMNElement *> &pElementList);
	void CreateThumbImage(int nChannel, IndexHdr *videoIndex, int numVideo, long long pos);
	int StepVideo(int stepNum, IndexHdr *videoIndex, int *numVideo, long long *pos);
	long long GetVideoIndexTime(int nIndexNum, long long fPos);
	void StartSegment(struct positionMarker *posMarker);
	void EndSegment(struct positionMarker *posMarker);
	void DestinationSegment(struct positionMarker *posMarker);
	long long CutSegment(VideoSegmentClass *pCopySegment);
	void CopySegment(VideoSegmentClass *pCopySegment);
	void ClearSegment(VideoSegmentClass *pCopySegment);
	long long PasteSegment(VideoSegmentClass *pCopySegment);
	long long CreateBranch(VideoSegmentClass *pCopySegment);

	ChannelDataMapping *GetChannelDataMapping(USHORT channelNum);

	void AddNewEventRecord(struct eventListElement *pEventListElement);
	void RewriteEventRecord(struct eventListElement *pEventListElement);
	void SetEventRoot(struct eventListElement *pEventListElement);
	void UpdateEventRecord(struct eventListElement *pEventListElement, VideoSegmentClass *pCopySegment);
	void UpdateChannelHeader(int channelNum);
	ULONG FindEventIndex(long long fPosVideoPlayed, int videoIndexNum);
	bool IsBranchRight(ULONG nEventIndex);
	long long SynchronizeVideo(ULONG nEventIndex);
	int GetNumMonitors(int chNum);
	void SaveEventDetails(ULONG eventIndex);
	void SetBranchRight(ULONG nEventIndex);
	void SetBranchBottom(ULONG nEventIndex);
	long long BranchCopy(ULONG nEventIndex, VideoSegmentClass *pCopySegment);
	void BranchDelete(ULONG nEventIndex, bool bDelEntire);
	string ReadProcessValue();
	void WriteProcessValue(string val);
	int GetMaxNumberOfMonitors();
	void SaveOrGetScenario(bool bSave, ScenarioDefintion *pScenario);
	void SaveOrGetBranchScenario(long long brancRightFPos, bool bSave, ScenarioDefintion *pScenario);
	string GetScenarioJSON();
	void SetScenarioJSON(string sceneJSON);

};

