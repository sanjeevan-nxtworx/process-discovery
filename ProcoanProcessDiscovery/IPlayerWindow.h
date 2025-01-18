#pragma once
#include "stdafx.h"

#include <list>
#include <map>
using namespace std;

class PlayerDataFileManager;
class AudioPlayerBuffer;
class BPMNElement;
class RecordBufferManager;
class BPMNDiagram;

class IPlayerWindow
{
public:
	virtual void SetSliderPosition(long position) = 0;
	virtual void StopPlay() = 0;
	virtual void SetExitFlag(bool flg) = 0;
	virtual int GetWindowWidth() = 0;
	virtual int GetWindowHeight() = 0;
	virtual bool GetActiveMonitorDisplayFlag() = 0;
	virtual void SetDataManager(PlayerDataFileManager *pRecorder) = 0;
	virtual long VideoSegmentStartTime() = 0;
	virtual void ResetSegmentMarker() = 0;
	virtual long VideoSegmentEndTime() = 0;
	virtual void GetPointOffset(POINT *pPoint) = 0;
	virtual void SetPlayerEndTime(ULONG endTime) = 0;
	virtual void SetPlayerTimeWindow() = 0;
	virtual void SetDiagram(list<BPMNElement *> *pDiagram) = 0;
	virtual void SetEventMap(map<ULONG, struct eventListElement *> *pEventMap) = 0;
	virtual void InsertEvent(ULONG nEventID, long long timestamp) = 0;
	virtual void SetStartFlag(bool bFlg) = 0;
	virtual void StartPlay(void) = 0;
	virtual void StartDisplay() = 0;
	virtual long GetSliderPosition() = 0;
	virtual void DisplayImage() = 0;
	virtual void BPMNElementSelected(struct cellDef *pCell) = 0;
	virtual void SetFramesPerSecond(int fpsCount) = 0;
	virtual int GetFramesPerSecond() = 0;
	//virtual void GetMonitorDetails(int noMonitors, long long fPosMonitorDetails) = 0;
	virtual void SetRecordBufferManager(RecordBufferManager *pRecBuffer) = 0;
	virtual bool IsVideoThreadWaiting() = 0;
	virtual ULONG GetThumbTime() = 0;
	virtual bool GetSegInProgress() = 0;
	virtual bool GetSegPresent() = 0;
	virtual ULONG GetSegStartPos() = 0;
	virtual ULONG GetSegEndPos() = 0;
	virtual void SetSegmentDetails(bool bInProgress, bool bPresent, ULONG startVal, ULONG endVal) = 0;
	virtual void SetTabFields(struct cellDef *pSelected) = 0;
	virtual void SetTabFields(struct eventListElement *pEventListElement) = 0;
};
