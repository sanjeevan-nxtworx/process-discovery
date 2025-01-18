#pragma once

#include <UIAutomation.h>
#include "MMHeader.h"
#include <map>
#include <string>
#include "..\PecanLib\PecanLib.h"
#include "PecanSemaphore.h"

using namespace std;

struct elementDetails
{
	string runTimeID;
	string processName;
	ULONG elementID;
	string automationID;
	RECT boundingRect;
	string className;
	CONTROLTYPEID controlType;
	string itemType;
	int    processID;

	map<int, string> nameMap;
	list <IUIAutomationElement *> parentList;
	IUIAutomationElement *pElement;
	IUIAutomationElement *pChild;
	IUIAutomationElement *pLeftSibling;
	IUIAutomationElement *pRightSibling;

	struct elementDetails *parentElement;
	struct elementDetails *childElement; // Pointer to first child element
	struct elementDetails *leftSiblingElement; 
	struct elementDetails *rightSiblingElement;

};

class EventRecorder : public IMouseEventCallback
{
	IUIAutomation *g_pAutomation;
	struct eventRecord eventRec;
	map <int, long long> messageQueue;
	map <string, struct elementDetails *> elementMap;
	list <struct elementDetails *> elementResolverList;
	map <int, string> processNameMap;
	ULONG elementIndex;

	bool bMouseMovedSinceLastCapture;
	bool bMouseOrKeyEventOccured;

	POINT ptOld;
	RecorderDataFileManager *pDataRecorder;
	RecordBufferManager *pRecordBufferManager;
	int mouseEventCode;
	int keyVKCode;
	int keyScanCode;
	IUIAutomationElement *pOldCursorElement;
	IUIAutomationElement *pOldFocusElement;
	void GetCachedElementAtPoint(POINT pt, IUIAutomationElement **ppControl);
	void GetCachedElementAtFocus(IUIAutomationElement **ppControl);
	DWORD FilterKeyCode(DWORD keyCode);
	HWND hWndHooks;
	DWORD pecanProcessID;
	PecanSemaphore eventSem;
	PecanSemaphore eventResolverSem;

	HANDLE eventResolverThread;
	DWORD eventResolverThreadID;

	HANDLE timerThread;
	DWORD  timerThreadID;
	bool	bQuitTimer;

	list <long long> eventInfo;
	void CreateTimerThread();
	void KillTimerThread();
	string GetControlInfoString(struct controlDef *pControlDef);
	string GetRuntimeID(IUIAutomationElement *pElement);
	void GetElementDetails(struct elementDetails *pElementDetails);
	bool FindElementInMap(IUIAutomationElement *pElement, struct elementDetails **ppElementDetails);
	void GetElementName(struct elementDetails *pElementDetails, int elementID);
	list <long long> keyStrokes;
	string CompileKeyStrokeString();
	void GetElementHeirarchy(struct elementDetails *pElementDetails);


public:
	EventRecorder();
	virtual ~EventRecorder();
	void RecordEvent(bool bPause);
	void EventResolver();
	bool IsEventReady();
	void SignalEvent();
	void MouseCallback(int nEventNum, DWORD pt);
	void TimerEnd();
	void KeyboardCallback(int nEventNum, WORD vk_code, WORD scanCode);
	bool IsNameRequired(CONTROLTYPEID controlTypeID);
	void GetEventDetails(struct eventRecord *eRec);
	IUIAutomationElement* GetContainingWindow(IUIAutomationElement* pChild);
	//void GetEventDetails(IUIAutomationElement *pElement, struct controlDef *pControlDetails);
	void ListDescendants(IUIAutomationElement* pParent, int indent);
	//void GetControlDefinition(IUIAutomationElement *pElement, struct controlDef *cDefn);
	bool CaptureCursorControl(IUIAutomationElement **ppElement);
	void SetDataManager(RecorderDataFileManager *pRecorder);
	void SetRecordBufferManager(RecordBufferManager *pRecManager);
	void InitializeEventRec();
	//void InitializeControlRec(struct controlDef *pControlDef);
	//void CopyControlRecBuffer(struct controlDefBuffer *pControlDefBuffer, struct controlDef *pControlDef, struct eventBufferRecord *pBuffer, int *position);
	void SetHooks();
	void SetHookHwnd(HWND hWndRecorder);
	void ResolveElement();
	void ClearHooks();
	void StartEventResolverThread();
	void StopEventResolverThread();
	string GetWindowElementJSON();
	void EscapeElementDetails(struct elementDetails *pElement);
	string EscapeJSonString(string str);
	string EscapeString(string strVal);
};
