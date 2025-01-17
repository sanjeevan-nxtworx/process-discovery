#pragma once

#ifndef MMHEADER
#define MMHEADER

//#include <d3d9.h>
//#include <d3dx9.h>
#include <chrono>
#include <list>
#include <map>
#include <queue>
#include <mutex>
#include <UIAutomation.h>
#include <string>
#include "json11.hpp"

//#ifdef _WIN32
//	#define BUILD32BIT
//#endif

using namespace std;


class BPMNElement;

struct scenarioReportData
{
	string scenarioName;
	string scenarioDesc;
	double timeInSecs;
	short noOfSteps;
};

struct linkReportSections
{
	BPMNElement *pElement;
	LONG link;
	string linkSNo;
};

struct PlayerChannelScreenBuffers
{
	ULONG size;
	
	char *pBuffer;
};

struct PlayerResizeScreenBuffers
{
	ULONG size;

	char *pBuffer;
};

struct RecorderStatusRecord
{
	mutex lockStruct;
	short currStatus;
	long long recordTime;
	unsigned short audioValue;
	ULONG numIndex;
	ULONG indexWritten;
	list <DWORD> audioValues;
};


struct eventListElement
{
	BPMNElement *pElement;
	RECT boundingRect;
	struct eventListElement *prevElement;
	struct eventListElement *nextElement;
	long long fPosCurr;
};

#define BLOCKSIZE 128


#pragma pack(push,1)

#define NUM_DATA_POINTS 60
#define WRITE_AUDIO 1

struct widgetDetails
{
	ULONG elementID;

	string processName;
	string automationID;
	RECT boundingRect;
	string className;
	CONTROLTYPEID controlType;
	string itemType;
	map<int, string> nameMap;

	ULONG parentID;
	ULONG childID;
	ULONG leftID;
	ULONG rightID;
};

struct keyStrokes {
	short keyVal;
	short keyFlg;
	short numPressed;
};

struct eventDetails
{
	short nEventRecType; // Type of record, 0x1 (task), 0x2 (sub Process), 0x3 (decision), 0x10 (Right child), 0x20 (bottom child), 0x40 (sub process Child)
	ULONG nParentEventIndex;
	long long timestamp;
	short nEventCode;
	short elementID;
	//int nVK_Code;
	//int nScan_Code;
	long eventIndex;
	POINT mousePt;
	RECT boundingRect;
	json11::Json jSonElement;
	string strAnnotation;
	string strNotes;
	string strKeyStrokes;
};

struct eventFileRecord
{
	long long timestamp;
	ULONG nEventCode;
	long eventIndex;
	RECT boundingRect;
	ULONG nParentEventCode;
	short nEventType; // 1 - Task, 2 for Sub Process, 4 for Branch, 256 for Branch Right Child, 512 for Branch Left Child, 1024 for Process Child
	long long fPosBottom; // position of first record in case of a SubProcess or bottom of a Branch
	long long fPosRight;  // position of right record in case a a branch.
	unsigned short stringLengths[3];
	unsigned short stringStart[3];
	ULONG size;
	ULONG compSize;

	long long fPosPrevEvent;
	long long fPosNextEvent;
	long long dataPos;  // This will hold the Annotation
};

struct eventBufferRecord
{
	time_t timeStamp;
	short mouseX;
	short mouseY;
#ifdef BUILD32BIT
	HMONITOR monitorNum;
	HMONITOR monitorNumExt;
#else
	HMONITOR monitorNum;
#endif

	short nEventCode;
	ULONG nEventID;
	RECT boundingRect;
	char strDescriptor[2000];
	char strKeyStrokes[512];
};


struct channelIndexDescriptor
{
	long long prevFpos;
	long long nextFPos;
	long long currFPos;
	long long startTime;
	short prevChannel;
	short newChannel;
	bool inPath;
};

struct videoIndexDescriptor
{
	long long prevFpos;
	long long nextFpos;
	long long timespan;
	long long currFPos;
	long long startTime;
	bool inPath;
};

struct branchIndexDescriptor
{
	long long prevFpos;
	long long nextRightFPos;
	long long nextBottomFPos;
	long long currFPos;
	short branchChannelNum;
	bool bPathDir;		// True means flow branches to the right
};

struct joinIndexDescriptor
{
	long long prevTopFpos;
	long long prevRightFpos;
	long long nextFpos;
	long long currFPos;
	bool bPathDir;	// true means flow comes in from the right
};

struct indexDataHdr
{
	ULONG indexNum;
	ULONG eventIndexNum;
	long recDuration;
};

struct indexPageDescriptor
{
	short nPageType; // 1 = VideoIndex, 2 = Branch Video 3 = joinPage 4 = audioPage, 5 = Channel Switch
	short channelNum;
	short numRecs;
	struct indexDataHdr dataRecs[NUM_DATA_POINTS];
	union pageData
	{
		struct videoIndexDescriptor *pVideo;
		struct branchIndexDescriptor *pBranch;
		struct joinIndexDescriptor *pJoin;
		struct channelIndexDescriptor *pChannel;
	} pageDescriptor;
	struct indexPageDescriptor *next;
};

struct headerDef
{
	short num;
	ULONG size;
};

struct cellDef
{
	BPMNElement *bpmnElement;
	bool bSelected;
	struct headerDef *row;
	struct headerDef *col;
	list <struct cellDef *> from;
	list <struct cellDef *> to;
};


struct rootHdr
{
	char RecType;     // 'R' for root
	char signature[64];
	short  noOfChannels;
	long long posChannelRecords;
	ULONG noOfVideoIndexPages;
	ULONG sizeOfProcessInfo;
	ULONG sizeOfCompProcessInfo;
	long long posProcessInfo;
	long long posScenarioInfo;
	ULONG sizeOfScenarioInfo;
	ULONG sizeOfCompScenarioInfo;
	char bit64;
	char filler[91];
};


struct SOPRootHdr
{
	char RecType;     // 'R' for root
	char signature[64];
	ULONG sizeOfProcessInfo;
	ULONG sizeOfCompProcessInfo;
	long long posProcessInfo;
	long long posScenarioInfo;
	ULONG sizeOfScenarioInfo;
	ULONG sizeOfCompScenarioInfo;
	long long posVideoRoot;
	long long posEventRoot;
	long long posTaskListRoot;
	long long posElementJSON;
	char filler[128];
};

struct SOPDataHeader
{
	char RecType;
	ULONG eventIndexNumber;
	ULONG width;
	ULONG height;
	ULONG compSize;
	long long recDurations;
	long long fPosData; // contain the right Branch position record in case of Branch Record
	char filler[16];
};

struct SOPIndexHdr
{
	char RecType;
	long long fPosNextRecord;
	int numRecs;
	struct SOPDataHeader dataRecs[NUM_DATA_POINTS];
	char filler[16];
};


struct positionMarker
{
	long long fPosIndex;
	unsigned short indexNum;
	short channelNum;
	short fileNum;
};

struct elementJSON
{
	ULONG compSize;
	ULONG size;
	long long fPosJSON;
};


struct channelHdr
{
	char RecType;		// 'C' for Channel Header
	time_t timestampStart;
	long long posVideoRoot;
	long long posAudioRoot;
	long long posEventRoot;
	long long posTaskListRoot;
	long long posElementJSON;

	time_t timestampEnd;
	long long recDurationus;
	//
	long long timePaused;
	long long posCurrRec;

	unsigned short channelNum;
	ULONG lastIndexNumber;
	long long fPosNextChannel;
	char filler[82];
};

struct dataHdr
{
	char RecType;	// 'D' for data record
	long long fPosData; // contain the right Branch position record in case of Branch Record
	ULONG size;		// for channel Index records will contain newChannel Number
	ULONG compSize;	// for channel Index records will contain old channel number

	long long fPosAudioData; // Contain the Audio Data record
	ULONG sizeAudio;
	ULONG compAudioSize;	

#ifdef BUILD32BIT
	HMONITOR screenNum;
	HMONITOR screenNumExt;
#else
	HMONITOR screenNum;
#endif
	ULONG audioIndexNumber;
	ULONG videoIndexNumber;
	ULONG eventIndexNumber;
	time_t timestamp;
	SHORT branchFlg;
	long long recDurationus; 
	char recStatus; /* Deleted (D), Inserted (I) etc.*/
};

// For Video Index Headers
// the Channel Switch Index hdr will have the channel number in numRecs
// the Branch Switch Index will have dataRecs[0].fPosData - Right Branch, and fPosNextRecord - Bottom Brancch
// The Join Index will have dataRecs[0].fPosData - right Branch join, fPosPrevRecord - top Branch Join
// The FposNext or fPosPrev will be set as required in the main index record

struct IndexHdr
{
	char RecType;	// 'P' for Page data, H for Channel Switch, B for Branch Switch, J for Join
	time_t timestamp;
	long long fPosNextRecord;
	long long fPosPrevRecord;
	short numRecs;
	char bFullImageIndex;
	long long recDurationus;
	struct dataHdr dataRecs[NUM_DATA_POINTS];
};

struct AudioHdr
{
	char RecType;  // 'A' for Audio Header
	long freq;
	short noChannels;
	short bitsPerSample;
	short wFormatTag;
	long long fPosRoot;
};

struct VideoHdr
{
	char RecType;	// 'V' for Video Header
	short noMonitors; 
	short fps;
	long long fPosMonitorDetails;
	long long fPosRoot;
};

struct EventHdr
{
	short RecType; // 'E' for Event Header
	long long fPosRoot;
};


struct MonitorHdr
{
	char RecType;	// 'M' for Monitor Hdr
#ifdef BUILD32BIT
	HMONITOR MonitorNumber;
	HMONITOR MonitorNumberExt;
#else
	HMONITOR MonitorNumber;
#endif
	LONG top;
	LONG left;
	LONG right;
	LONG bottom;
	RECT virRect;
};

struct blockDetails
{
	SHORT lineNum;
	SHORT structPos;
	LONG  numPixels;
};

struct CompareBuffer
{
#ifdef BUILD32BIT
	HMONITOR screenNum;
	HMONITOR screenNumExt;
#else
	HMONITOR screenNum;
#endif
	long numOfBlocks;
	long numBytes;
	struct blockDetails blockData;
};

struct ScreenData
{
	HMONITOR hMonitor;
	BITMAPINFOHEADER bmInfo;
	BYTE *lpScreen;
	BYTE *lpScreenNew;
	bool bCapture;
	bool bFullScreen;
	bool bScreenSelected;
	RECT rcMonitor;
	//IDirect3DDevice9*	g_pd3dDevice;
	//IDirect3DSurface9*	g_pSurface;
	//D3DDISPLAYMODE	ddm;
};

struct screenBufferRecord
{
#ifdef BUILD32BIT
	HMONITOR hMon;
	HMONITOR hMonExt;
#else
	HMONITOR hMon;
#endif
	DWORD size;
};

struct audioBufferRecord
{
	DWORD size;
	DWORD origSize;
};


union buffRec
{
	struct screenBufferRecord scrRec;
	struct audioBufferRecord  audRec;
	struct audioBufferRecord eventRec;  // use audio Buffer Record because of name clash 
};

struct bufferRecord
{
	char buffType; // Screen, Audio or Event
	time_t timeStamp;
	ULONG eventIndex;
	long long recDurationus;
	short mouseX;
	short mouseY;
	DWORD sizeOfBuffer;
	long long fPosIndex;
	LONG indexNum;
	union buffRec unBuffDets;
#ifdef BUILD32BIT
	char *buffer;
	char *bufferExt;
#else
	char *buffer;
#endif
};

struct screenBuffer
{
	HMONITOR hMon;
	mutex lockFreeBuffer;
	queue <struct bufferRecord *> freeBuffers;
};

struct MonitorScreenBuffer
{
	ULONG sizeOfBuffer;
	LPSTR pBuffer;
};

struct MonitorScreenData
{
	struct MonitorHdr MonitorDetails;
	struct MonitorScreenBuffer *pMonitorBuffer;
	struct MonitorScreenBuffer *pThumbBuffer;
	struct ScreenData screen;
};



//struct controlDef
//{
//	string automationID;
//	string name;
//	string processName;
//	string windowTitle;
//	CONTROLTYPEID controlTypeID;
//	int pID;
//	RECT boundingRect;
//	RECT parentBoundingRect;
//};
//
//struct controlDefBuffer
//{
//	int autoIDLen;
//	int nameLen;
//	int processNameLen;
//	int windowTitleLen;
//	char *automationID;
//	char *name;
//	char *processName;
//	char *windowTitle;
//	CONTROLTYPEID controlTypeID;
//	int pID;
//	RECT boundingRect;
//	RECT parentBoundingRect;
//};

struct eventRecord
{
	time_t timeStamp;
	short mouseX;
	short mouseY;
	ULONG nEventCode;
	string keyCodes;
	//DWORD KeyVKCode;
	//DWORD keyScanCode;
	string strDescription;
	RECT boundingRect;
};

struct ResizeScreenBuffer
{
	long long bufferSize;
	ULONG eventIndex;
	short width;
	short height;
	long long fPosIndexHeader;
	LONG indexNum;
	USHORT fileNumber;
	USHORT channelNumber;
	time_t t1FileStart;
	time_t recDurationus;
	char *buffer;
};

struct ThumbScreenBuffer
{
	BITMAPINFOHEADER bmInfo;
	char *buffer;
};

struct indexRecordData
{
	short channelNum;
	long long fPosCurr;
	long long fPosNextRecord;
	long long fPosPrevRecord;
	short numRecs;
	long long recDurationus;
};

struct indexPageData
{
	list <struct IndexRecordData *> indexPageList;
	map <long long, struct indexRecordData *> indexPageMap;
};

union pathNode
{
	struct indexPageData *pIndexPageData;
	struct branchData *pBranchData;
	struct joinData *pJoinData;
};

struct pathData
{
	short nodeType;
	union pathNode nextPath;
};

struct branchData
{
	ULONG branchIndex;
	struct pathData *pRightBranch;
	struct pathData *pDownBrach;
	struct pathData *currPath;
};

struct joinData
{
	ULONG joinIndex;
	struct pathData *pJoinBranch1;
	struct pathData *pJoinBranch2;
	struct pathData *pJoinBranchExit;
};


struct fileContext
{
	short numVideo;
	struct IndexHdr mainVideoIndex;
	bool	bDataReadThreadWait;
	bool    bVideoThreadWait;
	bool	bRewindThreadWait;
	bool	bMoveAbsoluteThreadWait;
	bool	bForwardThreadWait;
	bool	bStartPlay;
	bool	bExitThreads;
	bool	bHeaderReads;
	bool	bRewind;
	bool	bForward;
	bool	bStepRewind;
	bool	bStepForward;
	bool	bMoveAbsolute;
	bool	bForwardThreadIsRunning;
	bool	bRewindThreadIsRunning;
	bool	bThumbsDone;
	struct positionMarker currPositionMarker;
	long rewfwdMultiplier;
	ULONG	sliderPosition;
	short		fps;
	bool	bSegInProgress;
	bool	bSegPresent;
	bool	bSegCopied;
	ULONG	startPos;
	ULONG	endPos;
	string  scenarioName;
};




#pragma pack(pop)

#endif






		
		