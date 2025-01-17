#pragma once
#include <string>
#include "json11.hpp"


using namespace std;
using namespace json11;

class BPMNElement
{
protected:
	SIZE szElement;
	POINT pos;
	RECT boundingRect;
	bool bDisplayed;
	bool bSyncElement;
	bool bSegElement;

	bool bEllipse;
	string strAnnotation;
	string strAutomationID;
	string strNotes;
	string strProcessName;
	string strWindowName;
	string strControlType;
	string strControlName;
	int	nEventType;
	int nControlType;
	int nEventTypeAdditionalInfo;

	ULONG nEventIndex;
	long long nEventTimestamp;
	BPMNElement *pParent;
	bool bInPath;

	Json jSonElement;

public:
	string GetKeyValueAsString(string key);
	int GetKeyValueAsInt(string key);
	void GetKeyValueAsRECT(string key, RECT *pRect);
	double GetKeyValueAsDouble(string key);

	bool GetDisplayFlag();
	void SetDisplayFlag(bool bFlg);
	void SetAutomationID(string strAutomationIDVal);
	string GetAutomationID();
	void SetAnnotation(string strAnnotationVal);
	void SetNotes(string strNotesVal);
	void SetProcessName(string strProcessNameVal);
	void SetWindowName(string strWindowNameVal);
	void SetControlType(string strControlTypeVal);
	void SetControlName(string strControlNameVal);
	void SetEventType(int nEventTypeVal);
	void SetControlTypeVal(int nControlTypeVal);
	void SetEventAdditionalInfo(int nAdditionalVal);
	void SetSyncElement(bool bFlg);
	void SetSegElement(bool bFlg);
	string GetAnnotation();
	string GetNotes();
	string GetProcessName();
	string GetWindowName();
	string GetControlType();
	string GetControlName();
	int GetEventType();
	int GetControlTypeVal();
	int GetEventAdditionalInfo();
	long GetEventIndex();
	long long GetEventTimestamp();
	void SetEventTimestamp(long long val);
	RECT *GetBoundingRect();
	void SetBoundingRect(RECT *rVal);
	virtual int GetType() = 0;
	virtual void DrawElement(HDC hDC, POINT drawPos) = 0;
	virtual void GetControlDimension(HDC hDC);
	BPMNElement *GetParentElement();
	void SetParentElement(BPMNElement *pElement);
	BPMNElement *CreateNewEvent(ULONG nEventIndexVal);
	BPMNElement *CreateNewSubProcessEvent(ULONG nEventIndexVal);
	BPMNElement *CreateNewBranchEvent(ULONG nEventIndexVal);
	bool IsInRightBranchList();
	bool IsInBottomBranchList();

	bool IsInPath();
	void SetInPath(bool bFlg);
	bool IsSynchronized();
	bool IsInSegment();
	int GetWidth();
	int GetHeight();
	bool IsPtInElement(POINT pt);
	void GetDisplayPosition(RECT *rect);
	void SetDisplayPosition(POINT position);
	void SetDuration(long long duration);

	BPMNElement();
	BPMNElement(ULONG nEventIndexVal, long long timestamp);
	virtual ~BPMNElement();
};

