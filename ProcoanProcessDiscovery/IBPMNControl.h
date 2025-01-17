#pragma once
#include "stdafx.h"
#include <list>
#include <map>

class PlayerDataFileManager;
class BPMNElement;
class BPMNDiagram;
using namespace std;

class IBPMNControl
{
public:
	virtual bool ShowEventInWindow(ULONG nEventNum) = 0;
	virtual void SetCurrentSyncronizedElement(BPMNElement *pElement, bool bSegOn) = 0;
	virtual void SetCurrentSyncronizedElement(ULONG nEventID, bool bSegOn) = 0;
	virtual void InsertItem(ULONG nEventID, long long timestamp) = 0;
	virtual void SetAnnotationDetails(string strAnnotation, string strNotes) = 0;
	virtual void SetSubProcessDetails(string strName, string strDescription) = 0;
	virtual void SetController(PlayerDataFileManager *pManager) = 0;
	virtual void SetMainList(list <BPMNElement *> *pElementList) = 0;
	virtual void CreateAndDisplayDiagram(list <BPMNElement *> *pDiagram) = 0;
	virtual void SetEventMap(map <ULONG, struct eventListElement *> *pMap) = 0;
	virtual void GetPointOffset(POINT *ptOffset) = 0;
	virtual void UpdateBPMNWindow() = 0;
	virtual BPMNDiagram *GetDiagram() = 0;
	virtual bool IsSyncElementDisplayed() = 0;
};
