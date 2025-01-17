#pragma once
#include "..\PecanLib\PecanLib.h"
#include <list>
#include <string>
#include "..\ProcoanProcessDiscovery\PecanSemaphore.h"
#include <UIAutomationClient.h>
#include <CaptureElementData.h>

using namespace std;

class EventCaptureURL : public IMouseEventCallback
{
	HANDLE	timerThread;
	HANDLE UIAThread;
	DWORD timerThreadID;
	DWORD UIAThreadID;
	bool moveWaitFlg;
	unsigned moveTimer;
	bool bQuitCapture;
	list <unsigned int> UIAMessages;
	PecanSemaphore eventUIASem;
	RECT rectElement;
	IUIAutomationElement* pCapturedElement;
	HWND overLapWindow;

	CaptureElementData* pCapturedElementData;
	bool newCaptureFlg;
	void CaptureElementDataAtPoint();
	void CaptureElementAtPoint();
	void GetCachedElementAtPoint(POINT pt, IUIAutomationElement** ppControl, IUIAutomationCacheRequest** pCacheRequest);
	IUIAutomationElement* GetElementAtPoint(POINT pt, IUIAutomationElement* pControl, IUIAutomationCacheRequest** pCacheRequest, IUIAutomationTreeWalker* pControlWalker);
	HRESULT CreateCacheRequest(IUIAutomationCacheRequest** pRequest);
	bool IsControlTypeActionable(CONTROLTYPEID controlTypeID);
	string GetWindowTitle(IUIAutomationElement* pElement);
	string GetEdgeURL(IUIAutomationElement* pElement);
	string GetBrowserEdgeURLName(IUIAutomationElement* pElement);
	CaptureElementData* GetControlData(IUIAutomationElement* pElement);
	void GetCachedElementDetails(ElementData* pElementDetails, IUIAutomationElement* pElement);
	void GetCurrentElementDetails(ElementData* pElementDetails, IUIAutomationElement* pElement);

public:
	EventCaptureURL();
	~EventCaptureURL();
	bool IsNewEventCaptured();
	void ClearCapturedEvent();

	void StartEventProcessing();
	void EndEventProcessing();
	CaptureElementData* GetCapturedEvent();

	virtual void MouseCallback(int mEventNum, DWORD pt);
	virtual void KeyboardCallback(int mEventNum, WORD vk_code, WORD scanCode);
	virtual void TimerEnd();
	void CreateTimerThread();
	void UIAThreadFunction();
	void CreateUIAThread();
	void KillUIAThread();
	void KillTimerThread();
	void QuitCapture();
};

