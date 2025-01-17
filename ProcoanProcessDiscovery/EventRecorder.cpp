#include "stdafx.h"
#include <combaseapi.h>
#include "RecordBufferManager.h"
#include "RecorderDataFileManager.h"
#include "EventRecorder.h"
#include "..\PecanLib\PecanLib.h"
#include <mutex>
#include <chrono>
#include <string>
#include <comutil.h>
#include "Utility.h"
#include "spdlog/spdlog.h"
using namespace std;

std::string ConvertBSTRToMBS(BSTR bstr);
std::string ProcessIdToName(DWORD processId);
DWORD WINAPI ExecuteTimerFunction(LPVOID lpParameter);
DWORD WINAPI EventResolverThread(LPVOID lpParameter);
mutex lockEvent;

EventRecorder::EventRecorder()
{
	pDataRecorder = NULL;
	g_pAutomation = NULL;
	elementIndex = 0L;
	HRESULT hr = CoInitialize(NULL);
	if (hr == S_OK)
	{
		hr =
			CoCreateInstance(__uuidof(CUIAutomation),
				NULL, CLSCTX_INPROC_SERVER,
				__uuidof(IUIAutomation),
				(void**)&g_pAutomation);
	}
	mouseEventCode = -1;
	bMouseOrKeyEventOccured = false;
	bMouseMovedSinceLastCapture = true;
	pecanProcessID = GetCurrentProcessId();
	InitializeEventRec();
	bQuitTimer = false;
	ptOld.x = 0;
	ptOld.y = 0;
	pOldCursorElement = NULL;
	pOldFocusElement = NULL;
	keyStrokes.clear();
	CreateTimerThread();
}

void EventRecorder::CreateTimerThread()
{
	timerThread = CreateThread(0, 0, ExecuteTimerFunction, this, 0, &timerThreadID);
}


DWORD WINAPI ExecuteTimerFunction(LPVOID lpParameter)
{
	EventRecorder *eventRecorder = (EventRecorder *)lpParameter;

	eventRecorder->TimerEnd();

	return 0;
}

void EventRecorder::KillTimerThread()
{
	bQuitTimer = true;
	Sleep(100);

	if (timerThread != NULL)
	{
		WaitForSingleObject(timerThread, INFINITE);
		CloseHandle(timerThread);
		timerThread = NULL;
	}
}

EventRecorder::~EventRecorder()
{
	KillTimerThread();
	g_pAutomation->Release();
	CoUninitialize();
}


void EventRecorder::InitializeEventRec()
{
	eventRec.timeStamp = 0L;
	eventRec.mouseX = 0;
	eventRec.mouseY = 0;
	eventRec.strDescription = "";
}

//void EventRecorder::InitializeControlRec(struct controlDef *pControlDef)
//{
//	pControlDef->automationID = "";
//	pControlDef->boundingRect.top = pControlDef->boundingRect.left = pControlDef->boundingRect.bottom = pControlDef->boundingRect.right = 0;
//	pControlDef->parentBoundingRect.top = pControlDef->parentBoundingRect.left = pControlDef->parentBoundingRect.bottom = pControlDef->parentBoundingRect.right = 0;
//	pControlDef->controlTypeID = 0L;
//	pControlDef->name = pControlDef->windowTitle = pControlDef->processName = "";
//}

void EventRecorder::TimerEnd()
{
	HANDLE hTimer = NULL;
    LARGE_INTEGER liDueTime;
	static time_t oldTime;


    liDueTime.QuadPart = -500LL; // 50 ms

    // Create an unnamed waitable timer.
    hTimer = CreateWaitableTimer(NULL, FALSE, NULL);
    if (NULL == hTimer)
    {
        return;
    }

	// Set a timer to wait for 50 ms.
	if (!SetWaitableTimer(hTimer, &liDueTime, 50, NULL, NULL, 0))
	{
		return;
	}

	while (!bQuitTimer)
	{
		if (WaitForSingleObject(hTimer, INFINITE) != WAIT_OBJECT_0)
		{
			return;
		}
		if (bQuitTimer)
			continue;

		lockEvent.lock();
		if (messageQueue.size() > 0)
		{
			//spdlog::info("In Mouse Event Processing");
			map <int, long long>::iterator itMessage = messageQueue.begin();

			while (itMessage != messageQueue.end())
			{
				long long mouseData = itMessage->second;

				mouseData++;
				if ((mouseData & 0xffff) >= 4)
					break;
				messageQueue[itMessage->first] = mouseData;
				itMessage++;
			}

			if (itMessage != messageQueue.end())
			{
				long long mouseData = itMessage->second;
				int eventID = itMessage->first;
				mouseData = (mouseData >> 16) & 0xffffffff;
				messageQueue.erase(itMessage);
				long long eventData = (long long)eventID;
				eventData = (eventData << 56) & 0xff00000000000000L;
				eventData = eventData | mouseData;
				eventInfo.push_back(eventData);
				eventSem.notify();
			}
		}
		lockEvent.unlock();
	}
	CancelWaitableTimer(hTimer);
	CloseHandle(hTimer);
}

void EventRecorder::MouseCallback(int nEventCode, DWORD pt)
{
	//spdlog::info("In Mouse Event Callback");
	lockEvent.lock();
	int searchCode = 0;
	long long mouseData = (long long)pt;
	mouseEventCode = nEventCode;
	bool bStoreEvent = false;
	//char strMessage[100];
	if (nEventCode == 1 || nEventCode == 4 || nEventCode == 7) // Double Click button
	{
		bStoreEvent = true;
	}
	if (nEventCode == 2 || nEventCode == 5 || nEventCode == 8)
	{
		// Mouse down, then check existing list of messages
		searchCode = nEventCode;
		map <int, long long>::iterator itMessage = messageQueue.find(searchCode);
		if (itMessage != messageQueue.end())
		{
			// if message found then this may be a double click.
			// get the data
			mouseData = itMessage->second;
			mouseData &= 0xffffffffffff0000L; // reset the timer counter to zero
			// remove the message from the queue
			messageQueue.erase(itMessage);
			// Insert the click message for the button
			if (nEventCode == 3)
				nEventCode = 10;
			else if (nEventCode == 6)
				nEventCode = 11;
			else if (nEventCode == 9)
				nEventCode = 12;

			messageQueue.insert(pair<int, long long>(nEventCode, mouseData));
		}
		else
		{
			mouseData = (mouseData << 16) & 0xffffffff0000L;
			messageQueue.insert(pair<int, long long>(nEventCode, mouseData));
		}
	}
	else if (nEventCode == 3 || nEventCode == 6 || nEventCode == 9)
	{
		// Mouse Up, then check existing list of messages
		searchCode = nEventCode - 1;
		map <int, long long>::iterator itMessage = messageQueue.find(searchCode);
		// if corresponding mouse down not found
		if (itMessage == messageQueue.end())
		{
			// check if corresponding click is present this becomes a double click
			if (nEventCode == 3)
				searchCode = 10;
			else if (nEventCode == 6)
				searchCode = 11;
			else if (nEventCode == 9)
				searchCode = 12;
			itMessage = messageQueue.find(searchCode);
			if (itMessage == messageQueue.end())
			{
				// do nothing, and ignore the up key
				;
			}
			else
			{
				// remove the single click, and send a double click as the message
				mouseData = itMessage->second;
				mouseData = (mouseData >> 16) & 0xffffffffL;
				messageQueue.erase(itMessage);
				mouseEventCode = nEventCode - 2; // set code to double click
				bStoreEvent = true;
			}
		}
		else
		{
			// erase the down button message and insert a click message
			mouseData = itMessage->second;
			mouseData &= 0xffffffffffff0000L;
			messageQueue.erase(itMessage);
			if (nEventCode == 3)
				nEventCode = 10;
			else if (nEventCode == 6)
				nEventCode = 11;
			else if (nEventCode == 9)
				nEventCode = 12;
			messageQueue.insert(pair<int, long long>(nEventCode, mouseData));
		}
	}
	if (bStoreEvent)
	{
		long long eventData = (long long)mouseEventCode;
		eventData = (eventData << 56) & 0xff00000000000000L;
		eventData = eventData | mouseData;
		eventInfo.push_back(eventData);
		eventSem.notify();
	}
	lockEvent.unlock();
}

void EventRecorder::KeyboardCallback(int mEventNum, WORD vk_code, WORD scanCode)
{

	lockEvent.lock();
	long long eventData = (long long)mEventNum;
	eventData = (eventData << 56) | (((long long)vk_code << 16L) & 0xffff0000L) | ((long long)scanCode & 0xffff);
	eventInfo.push_back(eventData);
	eventSem.notify();
	lockEvent.unlock();
}

void EventRecorder::SetHookHwnd(HWND hwndRecorder)
{
	hWndHooks = hwndRecorder;
}

void EventRecorder::SetHooks()
{
	pDataRecorder->SetCallBack((IMouseEventCallback *)this);
	setKeyboardHook(hWndHooks);
	setMouseHook(hWndHooks);
}

void EventRecorder::ClearHooks()
{
	clearMouseHook(hWndHooks);
	clearKeyboardHook(hWndHooks);
	pDataRecorder->SetCallBack((IMouseEventCallback *)NULL);
}

void EventRecorder::SetDataManager(RecorderDataFileManager *pDataFileManager)
{
	pDataRecorder = pDataFileManager;
}

void EventRecorder::SetRecordBufferManager(RecordBufferManager *pRecManager)
{
	pRecordBufferManager = pRecManager;
}

bool EventRecorder::IsEventReady()
{
	eventSem.wait();
	return true;
}

void EventRecorder::SignalEvent()
{
	eventSem.notify();
}

string EventRecorder::GetRuntimeID(IUIAutomationElement *pElement)
{
	SAFEARRAY *rID;
	string strReturn = "";

	strReturn.clear();
	if (SUCCEEDED(pElement->GetRuntimeId(&rID)))
	{
		VARTYPE vt;
		long minIndex, maxIndex;
		int iDim;
		long *pVal;

		iDim = SafeArrayGetDim(rID);
		SafeArrayGetUBound(rID, 1, &maxIndex);
		SafeArrayGetLBound(rID, 1, &minIndex);
		SafeArrayGetVartype(rID, &vt);
		SafeArrayAccessData(rID, (void **)&pVal);
		char str_rID[100];
		bool bFirst = true;
		for (int i = minIndex; i <= maxIndex; i++)
		{
			sprintf_s(str_rID, "%08lx", pVal[i]);
			if (!bFirst)
				strReturn = strReturn + ":";
			strReturn = strReturn + str_rID;
			bFirst = false;
		}
		SafeArrayUnaccessData(rID);
		iDim = SafeArrayGetDim(rID);
	}
	return strReturn;
}

void EventRecorder::RecordEvent(bool bPause)
{
	POINT pt;
	IUIAutomationElement *pElement = NULL;
	static IUIAutomationElement *pElementOld = NULL;

	if (eventInfo.size() == 0)
		return;
	pt.x = 0;
	pt.y = 0;
	long long eventData = eventInfo.back();
	eventInfo.pop_back();
	int eventID = (int)((eventData >> 56) & 0xff);
	static int oldEventID = 0;
	//int vkCode = 0;
	//int recordKeyScanCode = 0
	int eventProcID = 0;
	string strKeys = "";
	HRESULT hr = 0;
	bool bRepeat = false;
	while (true)
	{
		bRepeat = false;
		if (eventID < 20 && pElementOld == NULL)
		{
			// Mouse Events
			keyStrokes.clear();
			strKeys = "";
			pElementOld = NULL;
			pt.x = (int)((eventData >> 16) & 0xffffL);
			pt.y = (int)(eventData & 0xffffL);
			time_t tstart, tend;
			tstart = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
			hr = g_pAutomation->ElementFromPoint(pt, &pElement);
			tend = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
			//spdlog::info("GetElement Time {}", (UINT)((tend - tstart) / 1000));
		}
		else if (eventID < 20)
		{
			bRepeat = true;
			pElement = pElementOld;
			strKeys = CompileKeyStrokeString();
			int tempEventID = eventID;
			eventID = oldEventID;
			oldEventID = tempEventID;
			keyStrokes.clear();
			pElementOld = NULL;
		}
		else
		{
			// Keyboard Event
			//vkCode = (int)((eventData >> 16) & 0xffffL);
			//recordKeyScanCode = (int)(eventData & 0xffffL);
			GetPhysicalCursorPos(&pt);
			time_t tstart, tend;
			tstart = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
			hr = g_pAutomation->GetFocusedElement(&pElement);
			BOOL bSame = FALSE;
			if (pElementOld != NULL)
			{
				g_pAutomation->CompareElements(pElement, pElementOld, &bSame);
				if (bSame)
				{
					keyStrokes.push_back(eventData);
					pElement->Release();
					pElement = NULL;
				}
				else
				{
					IUIAutomationElement *pTempElement = pElementOld;
					strKeys = CompileKeyStrokeString();
					//int tempEventID = eventID;
					eventID = oldEventID;
					oldEventID = eventID;
					keyStrokes.clear();
					keyStrokes.push_back(eventData);
					pElementOld = pElement;
					pElement = pTempElement;
				}
			}
			else
			{
				pElementOld = pElement;
				oldEventID = eventID;
				keyStrokes.push_back(eventData);
				pElement = NULL;
			}

			tend = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		}

		if (FAILED(hr))
		{
			return;
		}
		if (pElement == NULL)
			return;
		hr = pElement->get_CurrentProcessId(&eventProcID);
		if (FAILED(hr))
		{
			pElement->Release();
			return;
		}
		if (eventProcID == (int)pecanProcessID)
		{
			pElement->Release();
			return;
		}
		struct elementDetails *pElementDetails = NULL;

		bool bAdded = FindElementInMap(pElement, &pElementDetails);

		if (bAdded)
		{
			// Add to resolveList
			// signal semaphore
			pElement->AddRef();
			//spdlog::info("Added Event to Resolver List");
			GetElementHeirarchy(pElementDetails);
			elementResolverList.push_back(pElementDetails);
			eventResolverSem.notify();
		}

		ULONG nElementID = pElementDetails->elementID;
		BSTR str = NULL;
		string name;

		static int cnt;
		cnt++;
		// Get the element's name and print it
		hr = pElement->get_CurrentName(&str);
		if (cnt == 4)
			name.clear();
		name.clear();
		if (SUCCEEDED(hr))
		{
			name = ConvertBSTRToMBS(str);
			SysFreeString(str);
			str = NULL;
		}
		hr = pElement->get_CurrentBoundingRectangle(&pElementDetails->boundingRect);
		if (!bPause)
		{
			//spdlog::info("Saving Event in File");
			eventRec.nEventCode = eventID;			// This is the event code for mouse and keyboard events.
			//eventRec.keyScanCode = recordKeyScanCode;
			//eventRec.KeyVKCode = vkCode;
			eventRec.mouseX = pt.x;
			eventRec.mouseY = pt.y;
			if (name.length() > 180)
				name = "Data Overflow";
			eventRec.strDescription = "{\"Name\":\"" + name + "\"}";
			CopyMemory(&eventRec.boundingRect, &pElementDetails->boundingRect, sizeof(RECT));

			// Get Buffer from buffer Manager
			struct bufferRecord *pRecBuffer;
			struct eventBufferRecord *pEventBuffer;
			pRecBuffer = (struct bufferRecord *)pRecordBufferManager->GetFreeEventBuffer();

			pEventBuffer = (struct eventBufferRecord *)pRecBuffer->buffer;
			pEventBuffer->nEventID = nElementID;
			pEventBuffer->timeStamp = eventRec.timeStamp;
			pEventBuffer->nEventCode = eventRec.nEventCode;
			pEventBuffer->mouseX = eventRec.mouseX;
			pEventBuffer->mouseY = eventRec.mouseY;
			//pEventBuffer->monitorNum = eventRec.monitorNum;
			CopyMemory(&pEventBuffer->boundingRect, &eventRec.boundingRect, sizeof(RECT));
			//pEventBuffer->keyScanCode = eventRec.keyScanCode;
			//pEventBuffer->keyVKCode = eventRec.KeyVKCode;
			ZeroMemory(pEventBuffer->strDescriptor, 200);
			CopyMemory(pEventBuffer->strDescriptor, eventRec.strDescription.c_str(), eventRec.strDescription.length());
			ZeroMemory(pEventBuffer->strKeyStrokes, 512);
			CopyMemory(pEventBuffer->strKeyStrokes, strKeys.c_str(), strKeys.length());
			pRecBuffer->sizeOfBuffer = sizeof(struct bufferRecord) + sizeof(struct eventBufferRecord) + 32;
			pRecordBufferManager->WriteToFile(pRecBuffer);
		}
		pElement->Release();
		if (!bRepeat)
			break;
		else
		{
			eventID = oldEventID;
		}
	}

}

void EventRecorder::GetElementHeirarchy(struct elementDetails *pElementDetails)
{
	IUIAutomationElement *pElement = pElementDetails->pElement;
	pElementDetails->leftSiblingElement = pElementDetails->rightSiblingElement = pElementDetails->parentElement = pElementDetails->childElement = NULL;
	pElementDetails->pLeftSibling = pElementDetails->pRightSibling = pElementDetails->pChild = NULL;

	pElementDetails->parentList.clear();
	if (pElement == NULL)
		return;
	// The semaphore is checked before this call
	IUIAutomationTreeWalker *pControlWalker = NULL;
	//IUIAutomationElement *pOriginalElement = pElementDetails->pElement;

	g_pAutomation->get_ControlViewWalker(&pControlWalker);
	if (pControlWalker == NULL)
	{
		return;
	}

	IUIAutomationElement *pNode = pElement;
	IUIAutomationElement *pDesktop = NULL;

	HRESULT hr = g_pAutomation->GetRootElement(&pDesktop);
	if (FAILED(hr))
	{
		pControlWalker->Release();
		return;
	}

	//struct elementDetails *pNodeElementDetails = pElementDetails;
	// Now get all the parent elements in control view settings
	IUIAutomationElement *pParent = NULL;
	while (true)
	{
		pParent = NULL;
		if (SUCCEEDED(pControlWalker->GetParentElement(pNode, &pParent)))
		{
			BOOL bAreSame = false;
			if (pParent != NULL)
			{
				g_pAutomation->CompareElements(pParent, pDesktop, &bAreSame);
			}
			if (pParent == NULL || bAreSame)
			{
				break;
			}
			pElementDetails->parentList.push_back(pParent);
			pNode = pParent;
		}
		else
		{
			pControlWalker->Release();
			return;
		}
	}

	//spdlog::info("Resolving Child Element");
	// Link to the child element
	pNode = pElementDetails->pElement;
	IUIAutomationElement *pChild = NULL;
	if (SUCCEEDED(pControlWalker->GetFirstChildElement(pNode, &pChild)))
	{
		if (pChild != NULL)
		{
			pElementDetails->pChild = pChild;
		}
	}
	spdlog::info("Resolving Left Sibling");
	IUIAutomationElement *pLeftSibling = NULL;
	pNode = pElementDetails->pElement;
	if (SUCCEEDED(pControlWalker->GetPreviousSiblingElement(pNode, &pLeftSibling)))
	{
		if (pLeftSibling != NULL)
		{
			pElementDetails->pLeftSibling = pLeftSibling;
		}
	}

	//spdlog::info("Resolving Right Sibling");
	IUIAutomationElement *pRightSibling = NULL;
	if (SUCCEEDED(pControlWalker->GetNextSiblingElement(pNode, &pRightSibling)))
	{
		if (pRightSibling != NULL)
		{
			pElementDetails->pRightSibling = pRightSibling;
		}
	}
	pControlWalker->Release();
}


string EventRecorder::CompileKeyStrokeString()
{
	string strKeys = "{\"keystrokes\":[";
	char str[33];

	list <long long>::iterator itKeyStrokes = keyStrokes.begin();
	while (itKeyStrokes != keyStrokes.end())
	{
		long long val = *itKeyStrokes;
		int vkCode = (int)((val >> 16) & 0xffffL);
		int flags = (int)(val & 0xffffL);

		if (itKeyStrokes != keyStrokes.begin())
		{
			sprintf_s(str, ",[%d,%d]", vkCode, flags);
		}
		else
		{
			sprintf_s(str, "[%d,%d]", vkCode, flags);
		}

		strKeys += str;
		if (strKeys.size() > 480) // Dont record too many characters
			break;

		itKeyStrokes++;
	}
	strKeys += "]}";
	return strKeys;
}


bool EventRecorder::FindElementInMap(IUIAutomationElement *pElement, struct elementDetails **ppElementDetails)
{
	// Get the Runtime ID of the element
	string strRID = GetRuntimeID(pElement);
	bool bIsAdded = false;

	map <string, struct elementDetails *>::iterator itMap = elementMap.find(strRID);
	if (itMap == elementMap.end())
	{
		struct elementDetails *newElement = DBG_NEW struct elementDetails;
		elementIndex++;
		newElement->elementID = elementIndex;
		newElement->runTimeID = strRID;
		newElement->controlType = 0;
		newElement->boundingRect.top = newElement->boundingRect.bottom = newElement->boundingRect.left = newElement->boundingRect.right = 0;
		newElement->processID = 0;
		newElement->childElement = newElement->parentElement = newElement->leftSiblingElement = newElement->rightSiblingElement = NULL;
		//pElement->AddRef();
		// Add to elementMap
		elementMap.insert(pair<string, struct elementDetails *>(strRID, newElement));
		*ppElementDetails = newElement;
		bIsAdded = true;
	}
	else
	{
		*ppElementDetails = itMap->second;
	}
	if (*ppElementDetails != NULL)
		(*ppElementDetails)->pElement = pElement;
	return bIsAdded;
}

void EventRecorder::ResolveElement()
{
	// New version to handle crashes while recording
	// The semaphore is checked before this call
	struct elementDetails *pElementDetails = elementResolverList.front();
	elementResolverList.pop_front();
	//IUIAutomationElement *pOriginalElement = pElementDetails->pElement;
	IUIAutomationElement *pNode = pElementDetails->pElement;
	// For debugging purposes
	//IUIAutomationElement *pOrgNode = pNode;
	if (pNode == NULL)
	{
		//spdlog::info("ERROR: Element is NULL in ResolveElement()");
		return;
	}
	GetElementDetails(pElementDetails);
	//spdlog::info("Resolving Parent Elements");
	// Go up the Parent Chain and add elements if not present
	struct elementDetails *pNodeElementDetails = pElementDetails;
	pNodeElementDetails->parentElement = NULL;

	IUIAutomationElement *pParent = NULL;
	// Now get all the parent elements in control view settings
	list <IUIAutomationElement *>::iterator itParents = pElementDetails->parentList.begin();

	while (itParents != pElementDetails->parentList.end())
	{
		pParent = *itParents;
		if (pParent != NULL)
		{
			struct elementDetails *pParentElementDetails = NULL;
			bool bAdded = FindElementInMap(pParent, &pParentElementDetails);
			pNodeElementDetails->parentElement = pParentElementDetails;
			if (bAdded)
			{
				GetElementDetails(pParentElementDetails);
			}
			GetElementName(pParentElementDetails, pElementDetails->elementID);
			pNodeElementDetails = pParentElementDetails;
			pParent->Release();
		}
		itParents++;
	}

	//spdlog::info("Resolving Child Element");
	pNodeElementDetails->childElement = NULL;
	// Link to the child element
	pNodeElementDetails = pElementDetails;
	pNode = pElementDetails->pElement;
	IUIAutomationElement *pChild = pElementDetails->pChild;
	if (pChild != NULL)
	{
		struct elementDetails *pChildElementDetails = NULL;
		bool bAdded = FindElementInMap(pChild, &pChildElementDetails);
		pNodeElementDetails->childElement = pChildElementDetails;
		if (bAdded)
		{
			GetElementDetails(pChildElementDetails);
		}
		GetElementName(pChildElementDetails, pElementDetails->elementID);
		pChild->Release();
	}
	//spdlog::info("Resolving Left Sibling");
	IUIAutomationElement *pLeftSibling = pElementDetails->pLeftSibling;
	pNode = pElementDetails->pElement;
	if (pLeftSibling != NULL)
	{
		struct elementDetails *pLeftSiblingElementDetails = NULL;
		bool bAdded = FindElementInMap(pLeftSibling, &pLeftSiblingElementDetails);
		pNodeElementDetails->leftSiblingElement = pLeftSiblingElementDetails;
		if (bAdded)
		{
			GetElementDetails(pLeftSiblingElementDetails);
		}
		GetElementName(pLeftSiblingElementDetails, pElementDetails->elementID);
		pLeftSibling->Release();
	}

		//spdlog::info("Resolving Right Sibling");
	IUIAutomationElement *pRightSibling = pElementDetails->pRightSibling;
	if (pRightSibling != NULL)
	{
		struct elementDetails *pRightSiblingElementDetails = NULL;
		bool bAdded = FindElementInMap(pRightSibling, &pRightSiblingElementDetails);
		pNodeElementDetails->rightSiblingElement = pRightSiblingElementDetails;
		if (bAdded)
		{
			GetElementDetails(pRightSiblingElementDetails);
		}
		GetElementName(pRightSiblingElementDetails, pElementDetails->elementID);
		pRightSibling->Release();
	}

	pElementDetails->pElement->Release();
	pElementDetails->pElement = NULL;
}



//void EventRecorder::ResolveElement()
//{
//	// The semaphore is checked before this call
//	try
//	{
//		IUIAutomationTreeWalker *pControlWalker = NULL;
//		g_pAutomation->get_ControlViewWalker(&pControlWalker);
//
//		struct elementDetails *pElementDetails = elementResolverList.front();
//		elementResolverList.pop_front();
//		IUIAutomationElement *pParent = NULL;
//		IUIAutomationElement *pOriginalElement = pElementDetails->pElement;
//
//		IUIAutomationElement *pNode = pElementDetails->pElement;
//		// For debugging purposes
//		IUIAutomationElement *pOrgNode = pNode;
//
//		if (pNode == NULL)
//		{
//			//spdlog::info("ERROR: Element is NULL in ResolveElement()");
//			return;
//		}
//		GetElementDetails(pElementDetails);
//		IUIAutomationElement *pDesktop = NULL;
//
//		//spdlog::info("Resolving Parent Elements");
//		// Go up the Parent Chain and add elements if not present
//		if (pControlWalker == NULL)
//		{
//			pOriginalElement->Release();
//			return;
//		}
//		HRESULT hr = g_pAutomation->GetRootElement(&pDesktop);
//		if (FAILED(hr))
//		{
//			pControlWalker->Release();
//			pOriginalElement->Release();
//			return;
//		}
//
//		struct elementDetails *pNodeElementDetails = pElementDetails;
//		pNodeElementDetails->parentElement = NULL;
//	
//		//try
//		//{
//		//	// Now get all the parent elements in control view settings
//		//	while (true)
//		//	{
//		//		pParent = NULL;
//		//		if (SUCCEEDED(pControlWalker->GetParentElement(pNode, &pParent)))
//		//		{
//		//			if (pParent == NULL || pParent == pDesktop)
//		//			{
//		//				if (pNode != pOriginalElement)
//		//					pNode->Release();
//		//				if (pParent != NULL)
//		//				{
//		//					pParent->Release();
//		//				}
//		//				break;
//		//			}
//		//			struct elementDetails *pParentElementDetails = NULL;
//		//			bool bAdded = FindElementInMap(pParent, &pParentElementDetails);
//		//			pNodeElementDetails->parentElement = pParentElementDetails;
//		//			if (bAdded)
//		//			{
//		//				GetElementDetails(pParentElementDetails);
//		//			}
//		//			GetElementName(pParentElementDetails, pElementDetails->elementID);
//		//			pNodeElementDetails = pParentElementDetails;
//		//			if (pNode != pOriginalElement)
//		//				pNode->Release();
//		//			pNode = pParent;
//		//		}
//		//		else
//		//		{
//		//			if (pNode != pOriginalElement)
//		//				pNode->Release();
//		//			pControlWalker->Release();
//		//			pOriginalElement->Release();
//		//			return;
//		//		}
//
//		//	}
//		//}
//		//catch (...)
//		//{
//		//	//spdlog::info("Exception Resolving Event Parent Info.");
//		//	;
//		//}
//
//		//spdlog::info("Resolving Child Element");
//		pNodeElementDetails->childElement = NULL;
//		// Link to the child element
//		//pNodeElementDetails = pElementDetails;
//		//pNode = pElementDetails->pElement;
//		//IUIAutomationElement *pChild = NULL;
//		//try
//		//{
//		//	if (SUCCEEDED(pControlWalker->GetFirstChildElement(pNode, &pChild)))
//		//	{
//		//		if (pChild != NULL)
//		//		{
//		//			struct elementDetails *pChildElementDetails = NULL;
//		//			bool bAdded = FindElementInMap(pChild, &pChildElementDetails);
//		//			pNodeElementDetails->childElement = pChildElementDetails;
//		//			if (bAdded)
//		//			{
//		//				GetElementDetails(pChildElementDetails);
//		//			}
//		//			GetElementName(pChildElementDetails, pElementDetails->elementID);
//		//			pChild->Release();
//		//		}
//
//		//	}
//
//		//}
//		//catch (...)
//		//{
//		//	//spdlog::info("Exception Resolving Event Child Info.");
//		//	;
//		//}
//		//spdlog::info("Resolving Left Sibling");
//		IUIAutomationElement *pLeftSibling = NULL;
//		pNode = pElementDetails->pElement;
//
//		try
//		{
//			if (SUCCEEDED(pControlWalker->GetPreviousSiblingElement(pNode, &pLeftSibling)))
//			{
//				if (pLeftSibling != NULL)
//				{
//					struct elementDetails *pLeftSiblingElementDetails = NULL;
//					bool bAdded = FindElementInMap(pLeftSibling, &pLeftSiblingElementDetails);
//					pNodeElementDetails->leftSiblingElement = pLeftSiblingElementDetails;
//					if (bAdded)
//					{
//						GetElementDetails(pLeftSiblingElementDetails);
//					}
//					GetElementName(pLeftSiblingElementDetails, pElementDetails->elementID);
//					pLeftSibling->Release();
//				}
//
//			}
//		}
//		catch (...)
//		{
//			spdlog::info("Exception Resolving Left Sibling Info.");
//			;
//		}
//
//
//		try
//		{
//			//spdlog::info("Resolving Right Sibling");
//			IUIAutomationElement *pRightSibling = NULL;
//			if (SUCCEEDED(pControlWalker->GetNextSiblingElement(pNode, &pRightSibling)))
//			{
//				if (pRightSibling != NULL)
//				{
//					struct elementDetails *pRightSiblingElementDetails = NULL;
//					bool bAdded = FindElementInMap(pRightSibling, &pRightSiblingElementDetails);
//					pNodeElementDetails->rightSiblingElement = pRightSiblingElementDetails;
//					if (bAdded)
//					{
//						GetElementDetails(pRightSiblingElementDetails);
//					}
//					GetElementName(pRightSiblingElementDetails, pElementDetails->elementID);
//					pRightSibling->Release();
//				}
//			}
//		}
//		catch (...)
//		{
//			//spdlog::info("Exception Resolving Right Sibling Info.");
//			;
//		}
//
//		try
//		{
//			pOriginalElement->Release();
//			pElementDetails->pElement = NULL;
//		}
//		catch (...)
//		{
//			//spdlog::info("Exception Releasing Events");
//			;
//		}
//		//spdlog::info("Resolving Element Done");
//
//		pControlWalker->Release();
//	}
//	catch (...)
//	{
//		//spdlog::info("Exception in Resolve Element");
//		;
//	}
//}

void EventRecorder::GetElementName(struct elementDetails *pElementDetails, int elementID)
{
	IUIAutomationElement *pElement = pElementDetails->pElement;
	HRESULT hr;
	BSTR str = NULL;

	hr = pElement->get_CurrentName(&str);
	if (SUCCEEDED(hr))
	{
		string strName = ConvertBSTRToMBS(str);
		if (strName.size() > 100)
			strName = "Text Field";
		pElementDetails->nameMap.insert(pair<int, string>(elementID, strName));
		SysFreeString(str);
		str = NULL;
	}
}

void EventRecorder::GetElementDetails(struct elementDetails *pElementDetails)
{
	IUIAutomationElement *pElement = pElementDetails->pElement;

	HRESULT hr;
	BSTR str = NULL;

	hr = pElement->get_CurrentAutomationId(&str);
	if (SUCCEEDED(hr))
	{
		pElementDetails->automationID = ConvertBSTRToMBS(str);
		SysFreeString(str);
		str = NULL;
	}

	hr = pElement->get_CurrentBoundingRectangle(&pElementDetails->boundingRect);
	hr = pElement->get_CurrentControlType(&pElementDetails->controlType);
	hr = pElement->get_CurrentProcessId(&pElementDetails->processID);
	
	hr = pElement->get_CurrentClassName(&str);
	if (SUCCEEDED(hr))
	{
		pElementDetails->className = ConvertBSTRToMBS(str);
		SysFreeString(str);
		str = NULL;
	}

	hr = pElement->get_CurrentItemType(&str);
	if (SUCCEEDED(hr))
	{
		pElementDetails->itemType = ConvertBSTRToMBS(str);
		SysFreeString(str);
		str = NULL;
	}
	map <int, string>::iterator itProc = processNameMap.find(pElementDetails->processID);
	if (itProc == processNameMap.end())
	{
		pElementDetails->processName = ProcessIdToName(pElementDetails->processID);
		processNameMap.insert(pair<int, string>(pElementDetails->processID, pElementDetails->processName));
	}
	else
	{
		pElementDetails->processName = itProc->second;
	}

}


DWORD WINAPI EventResolverThread(LPVOID lpParameter)
{
	EventRecorder *evtRecorder = (EventRecorder *)lpParameter;

	evtRecorder->EventResolver();
	return 0;
}

string EventRecorder::GetWindowElementJSON()
{
	string strJSON;

	strJSON = "[\n";
	map <string, struct elementDetails *>::iterator itMap =  elementMap.begin();

	while (itMap != elementMap.end())
	{
		struct elementDetails *pElementDetails = itMap->second;
		EscapeElementDetails(pElementDetails);
		char str[100];

		if (itMap == elementMap.begin())
			strJSON = strJSON + "\t{";
		else
			strJSON = strJSON + ",\n\t{";
		
		sprintf_s(str, "\"ElementID\":%ld,", pElementDetails->elementID);
		strJSON = strJSON + str;

		sprintf_s(str, "\"ControlType\":%ld,", (ULONG)pElementDetails->controlType);
		strJSON = strJSON + str;

		strJSON = strJSON + "\"ProcessName\":\"" + pElementDetails->processName + "\",";
		strJSON = strJSON + "\"AutomationID\":\"" + pElementDetails->automationID + "\",";

		sprintf_s(str, "\"BoundingRect\":{\"Left\":%d,\"Top\":%d,\"Right\":%d,\"Bottom\":%d}", pElementDetails->boundingRect.left, pElementDetails->boundingRect.top, pElementDetails->boundingRect.right, pElementDetails->boundingRect.bottom);
		strJSON = strJSON + str + ",";

		strJSON = strJSON + "\"ClassName\":\"" + pElementDetails->className + "\",";
		if (pElementDetails->parentElement != NULL)
		{
			sprintf_s(str, "\"Parent\":%ld,", pElementDetails->parentElement->elementID);
			strJSON = strJSON + str;
		}
		
		if (pElementDetails->childElement != NULL)
		{
			sprintf_s(str, "\"Child\":%ld,", pElementDetails->childElement->elementID);
			strJSON = strJSON + str;
		}

		if (pElementDetails->leftSiblingElement != NULL)
		{
			sprintf_s(str, "\"Left\":%ld,", pElementDetails->leftSiblingElement->elementID);
			strJSON = strJSON + str;
		}

		if (pElementDetails->rightSiblingElement != NULL)
		{
			sprintf_s(str, "\"Right\":%ld,", pElementDetails->rightSiblingElement->elementID);
			strJSON = strJSON + str;
		}

		strJSON = strJSON + "\"ItemType\":\"" + pElementDetails->itemType + "\",";

		map <int, string>::iterator itName = pElementDetails->nameMap.begin();
		strJSON = strJSON + "\"Names\": [";
		bool bFirst = true;
		while (itName != pElementDetails->nameMap.end())
		{
			if (bFirst)
				strJSON = strJSON + " { ";
			else
				strJSON = strJSON + ", { ";
			sprintf_s(str, "\"ID\": %d,\"Name\":\"", itName->first);
			string strName = EscapeJSonString(itName->second);
			strJSON = strJSON + str + strName + "\"}";

			bFirst = false;

			itName++;
		}
		strJSON = strJSON + " ] ";

		strJSON = strJSON + "}";
		itMap++;
	}

	strJSON = strJSON + "\n]\n";

	return strJSON;
}

string EventRecorder::EscapeJSonString(string str)
{
	// Replace double quotes with single quotes
	size_t found = str.find('\"');
	if (found != string::npos)
	{
		string strRes = str;

		found = strRes.find("\"");

		// Repeat till end is reached
		while (found != string::npos)
		{
			// Replace this occurrence of Sub String
			strRes.replace(found, 1, "\\\"");
			// Get the next occurrence from the current position
			found = strRes.find('\"', found + 1);
		}

		return strRes;
	}
	return str;

}

void EventRecorder::EscapeElementDetails(struct elementDetails *pElementDetails)
{
	pElementDetails->automationID = EscapeString(pElementDetails->automationID);
	pElementDetails->runTimeID = EscapeString(pElementDetails->runTimeID);
	pElementDetails->processName = EscapeString(pElementDetails->processName);
	pElementDetails->className = EscapeString(pElementDetails->className);
	pElementDetails->itemType = EscapeString(pElementDetails->itemType);

	map<int, string>::iterator itNameMap = pElementDetails->nameMap.begin();
	while (itNameMap != pElementDetails->nameMap.end())
	{
		itNameMap->second = EscapeString(itNameMap->second);
		itNameMap++;
	}
}

string EventRecorder::EscapeString(string strVal)
{
	size_t found = strVal.find('\\');
	if (found != string::npos)
	{
		string strRes = strVal;

		found = strRes.find("\\");

		// Repeat till end is reached
		while (found != string::npos)
		{
			// Replace this occurrence of Sub String
			strRes.replace(found, 1, "\\\\");
			// Get the next occurrence from the current position
			found = strRes.find("\\", found + 2);
		}

		return strRes;
	}
	return strVal;
}

void EventRecorder::EventResolver()
{
	while (true)
	{
		eventResolverSem.wait();
		if (elementResolverList.size() == 0)
		{
			return;
		}
		//spdlog::info("Resolving Event");
		ResolveElement();
	}
}

void EventRecorder::StartEventResolverThread()
{
	eventResolverThread = CreateThread(0, 0, EventResolverThread, this, 0, &eventResolverThreadID);
}

void EventRecorder::StopEventResolverThread()
{
	while (elementResolverList.size() > 0)
		Sleep(100);
	eventResolverSem.notify();
	Sleep(1000);
}




//string EventRecorder::GetControlInfoString(struct controlDef *pControl)
//{
//	string strMain;
//	string strField;
//
//	char str[200];
//	sprintf_s(str, "<I>%d</I><T>%d</T><R>%d,%d,%d,%d</R><PR>%d,%d,%d,%d</PR>", pControl->pID, (int)pControl->controlTypeID,
//		pControl->boundingRect.left, pControl->boundingRect.top, pControl->boundingRect.right, pControl->boundingRect.bottom,
//		pControl->parentBoundingRect.left, pControl->parentBoundingRect.top, pControl->parentBoundingRect.right, pControl->parentBoundingRect.bottom);
//	strMain = "<A>" + pControl->automationID + "</A>";
//	strMain = strMain + "<N>" + pControl->name + "</N>";
//	strMain = strMain + "<PN>" + pControl->processName + "</PN>";
//	strMain = strMain + "<WN>" + pControl->windowTitle + "</WN>";
//	strMain = strMain + str;
//	return strMain;
//}

DWORD EventRecorder::FilterKeyCode(DWORD kCode)
{
	DWORD retCode = kCode;

	if ((kCode >= '0' && kCode <= '9') || (kCode >= 'A' && kCode <= 'Z'))
		retCode = 0xff;
	return retCode;
}

//void EventRecorder::CopyControlRecBuffer(struct controlDefBuffer *pControlDefBuffer, struct controlDef *pControlDef, struct eventBufferRecord *pBuffer, int *position)
//{
//	pControlDefBuffer->boundingRect = pControlDef->boundingRect;
//	pControlDefBuffer->parentBoundingRect = pControlDef->parentBoundingRect;
//	pControlDefBuffer->controlTypeID = pControlDef->controlTypeID;
//
//	char *pWriteBuffer = (char *)pBuffer + sizeof(struct eventBufferRecord) + 5;
//	ZeroMemory(pWriteBuffer+*position, 405);
//	pControlDefBuffer->autoIDLen = pControlDef->automationID.length() + 1;
//	if (pControlDefBuffer->autoIDLen > 100)
//		pControlDefBuffer->autoIDLen = 100;
//	pControlDefBuffer->nameLen = pControlDef->name.length() + 1;
//	if (pControlDefBuffer->nameLen > 100)
//		pControlDefBuffer->nameLen = 100;
//	pControlDefBuffer->processNameLen = pControlDef->processName.length() + 1;
//	if (pControlDefBuffer->processNameLen > 100)
//		pControlDefBuffer->processNameLen = 100;
//	pControlDefBuffer->windowTitleLen = pControlDef->windowTitle.length() + 1;
//	if (pControlDefBuffer->windowTitleLen > 100)
//		pControlDefBuffer->windowTitleLen = 100;
//	
//
//	pControlDefBuffer->automationID = pWriteBuffer + *position;
//	memcpy(pControlDefBuffer->automationID, (void *)(pControlDef->automationID.c_str()), pControlDefBuffer->autoIDLen-1);
//	*position += pControlDefBuffer->autoIDLen;
//
//	pControlDefBuffer->name = pWriteBuffer + *position;
//	memcpy(pControlDefBuffer->name, (void *)(pControlDef->name.c_str()), pControlDefBuffer->nameLen - 1);
//	*position += pControlDefBuffer->nameLen;
//
//	pControlDefBuffer->processName = pWriteBuffer + *position;
//	memcpy(pControlDefBuffer->processName, (void *)(pControlDef->processName.c_str()), pControlDefBuffer->processNameLen - 1);
//	*position += pControlDefBuffer->processNameLen;
//
//	pControlDefBuffer->windowTitle = pWriteBuffer + *position;
//	memcpy(pControlDefBuffer->windowTitle, (void *)(pControlDef->windowTitle.c_str()), pControlDefBuffer->windowTitleLen - 1);
//	*position += pControlDefBuffer->windowTitleLen;
//	*position += 5;
//}

bool EventRecorder::CaptureCursorControl(IUIAutomationElement **ppElement)
{
	// Get the element under the cursor
	// Use GetPhysicalCursorPos to interact properly with
	// High DPI
	bool bResult = false;
	POINT pt;
	time_t timeStampNew = 0L;
	static time_t timeStampLast = 0L;


	GetPhysicalCursorPos(&pt);

	lockEvent.lock();
	eventRec.mouseX = pt.x;
	eventRec.mouseY = pt.y;
	//eventRec.monitorNum = (HMONITOR)NULL;//monNum;
	lockEvent.unlock();
	if (pt.x != ptOld.x || pt.y != ptOld.y)
	{
		bResult = false;
		bMouseMovedSinceLastCapture = true;
		timeStampLast = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	}
	else
	{
		timeStampNew = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		if((timeStampNew - timeStampLast) > 150000L && bMouseMovedSinceLastCapture)
		{
			bMouseMovedSinceLastCapture = false;
			bResult = true;
		}
	}
	ptOld = pt;
	*ppElement = NULL;
	if (bResult)
	{
		time_t tstart, tend;

		tstart = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		if (FAILED(g_pAutomation->ElementFromPoint(pt, ppElement)))
		{
			bResult = false;
			*ppElement = NULL;
		}
		tend = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

		//GetCachedElementAtPoint(pt, ppElement);
	}
	return bResult;
}

void EventRecorder::GetCachedElementAtPoint(POINT pt, IUIAutomationElement **ppControl)
{
	time_t tstart, tend;

	tstart = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	IUIAutomationCacheRequest* pCacheRequest = NULL;
	HRESULT hr;
	*ppControl = NULL;
	hr = g_pAutomation->CreateCacheRequest(&pCacheRequest);
	if (FAILED(hr))
	{
		goto cleanup;
	}
	hr = pCacheRequest->AddProperty(UIA_NamePropertyId);
	if (FAILED(hr))
	{
		pCacheRequest->Release();
		goto cleanup;
	}
	hr = pCacheRequest->AddProperty(UIA_AutomationIdPropertyId);
	if (FAILED(hr))
	{
		pCacheRequest->Release();
		goto cleanup;
	}		
	hr = pCacheRequest->AddProperty(UIA_BoundingRectanglePropertyId);
	if (FAILED(hr))
	{
		pCacheRequest->Release();
		goto cleanup;
	}

	hr = pCacheRequest->AddProperty(UIA_ControlTypePropertyId);
	if (FAILED(hr))
	{
		pCacheRequest->Release();
		goto cleanup;
	}

	hr = pCacheRequest->AddProperty(UIA_ProcessIdPropertyId);
	if (FAILED(hr))
	{
		pCacheRequest->Release();
		goto cleanup;
	}

	time_t t1start, t1end;
	t1start = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	hr = g_pAutomation->ElementFromPointBuildCache(pt, pCacheRequest, ppControl);
	t1end = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	pCacheRequest->Release();
	if (FAILED(hr))
	{
		goto cleanup;
	}
cleanup:
	hr = S_OK;
	tend = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

void EventRecorder::GetCachedElementAtFocus(IUIAutomationElement **ppControl)
{
	IUIAutomationCacheRequest* pCacheRequest = NULL;
	HRESULT hr;
	*ppControl = NULL;

	time_t tstart, tend;

	tstart = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

	hr = g_pAutomation->CreateCacheRequest(&pCacheRequest);
	if (FAILED(hr))
	{
		goto cleanup;
	}
	hr = pCacheRequest->AddProperty(UIA_NamePropertyId);
	if (FAILED(hr))
	{
		pCacheRequest->Release();
		goto cleanup;
	}
	hr = pCacheRequest->AddProperty(UIA_AutomationIdPropertyId);
	if (FAILED(hr))
	{
		pCacheRequest->Release();
		goto cleanup;
	}
	hr = pCacheRequest->AddProperty(UIA_BoundingRectanglePropertyId);
	if (FAILED(hr))
	{
		pCacheRequest->Release();
		goto cleanup;
	}

	hr = pCacheRequest->AddProperty(UIA_ControlTypePropertyId);
	if (FAILED(hr))
	{
		pCacheRequest->Release();
		goto cleanup;
	}

	hr = pCacheRequest->AddProperty(UIA_ProcessIdPropertyId);
	if (FAILED(hr))
	{
		pCacheRequest->Release();
		goto cleanup;
	}

	time_t t1start, t1end;
	t1start = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	hr = g_pAutomation->GetFocusedElementBuildCache(pCacheRequest, ppControl);
	t1end = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	pCacheRequest->Release();
	if (FAILED(hr))
	{
		goto cleanup;
	}
cleanup:
	hr = S_OK;
	tend = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

//void EventRecorder::GetEventDetails(IUIAutomationElement *pElement, struct controlDef *pControlDetails)
//{
//	time_t tstart, tend;
//
//	tstart = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
//
//	IUIAutomationElement *pParentWindow = GetContainingWindow(pElement);
//	struct controlDef cParentDefn;
//	InitializeControlRec(&cParentDefn);
//
//	if (pParentWindow != NULL)
//	{
//		GetControlDefinition(pParentWindow, &cParentDefn);
//		pParentWindow->Release();
//	}
//	 
//	struct controlDef cDefn;
//	InitializeControlRec(&cDefn);
//	GetControlDefinition(pElement, &cDefn);
//	InitializeControlRec(pControlDetails);
//	cDefn.processName = ProcessIdToName(cDefn.pID);
//	lockEvent.lock();
//	pControlDetails->automationID = cDefn.automationID;
//	pControlDetails->boundingRect = cDefn.boundingRect;
//	pControlDetails->controlTypeID = cDefn.controlTypeID;
//	pControlDetails->name = cDefn.name;
//	pControlDetails->pID = cDefn.pID;
//	pControlDetails->processName = cDefn.processName;
//	pControlDetails->windowTitle = cParentDefn.name;
//	pControlDetails->parentBoundingRect = cParentDefn.boundingRect;
//	lockEvent.unlock();
//	tend = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
//
//}
//
//void EventRecorder::GetControlDefinition(IUIAutomationElement *pElement, struct controlDef *cDefn)
//{
//	// Get the elements Control ID
//	HRESULT hr;
//	BSTR str = NULL;
//
//	hr = pElement->get_CurrentAutomationId(&str);
//	if (SUCCEEDED(hr))
//	{
//		cDefn->automationID = ConvertBSTRToMBS(str);
//		SysFreeString(str);
//		str = NULL;
//	}
//	hr = pElement->get_CurrentControlType(&cDefn->controlTypeID);
//	if (SUCCEEDED(hr))
//	{
//		if (IsNameRequired(cDefn->controlTypeID))
//		{
//			// Get the element's name and print it
//			hr = pElement->get_CurrentName(&str);
//			if (SUCCEEDED(hr))
//			{
//				cDefn->name = ConvertBSTRToMBS(str);
//				SysFreeString(str);
//				str = NULL;
//			}
//
//		}
//		hr = pElement->get_CurrentProcessId(&cDefn->pID);
//		hr = pElement->get_CurrentBoundingRectangle(&cDefn->boundingRect);
//
//	}
//}

void EventRecorder::GetEventDetails(struct eventRecord *eRec)
{
	lockEvent.lock();
	eRec->mouseX = eventRec.mouseX;
	eRec->mouseY = eventRec.mouseY;
	lockEvent.unlock();
}

IUIAutomationElement* EventRecorder::GetContainingWindow(IUIAutomationElement* pChild)
{
	if (pChild == NULL)
		return NULL;

	IUIAutomationElement* pDesktop = NULL;
	HRESULT hr = g_pAutomation->GetRootElement(&pDesktop);
	if (FAILED(hr))
	{
		return NULL;
	}
	BOOL same;
	g_pAutomation->CompareElements(pChild, pDesktop, &same);
	if (same)
	{
		return NULL; // No parent, so return NULL.
	}

	IUIAutomationElement* pParent = NULL;
	IUIAutomationElement* pNode = pChild;
	IUIAutomationTreeWalker* pWalker = NULL;
	hr = g_pAutomation->get_ControlViewWalker(&pWalker);
	if (FAILED(hr))
	{
		goto cleanup;
	}
	if (pWalker == NULL)
		goto cleanup;

	// Walk up the tree.
	while (TRUE)
	{

		hr = pWalker->GetParentElement(pNode, &pParent);
		//hr = pWalker->GetParentElementBuildCache(pNode, pCacheRequest, &pParent);

		if (FAILED(hr) || pParent == NULL)
		{
			break;
		}

		hr = g_pAutomation->CompareElements(pParent, pDesktop, &same);
		if (FAILED(hr))
		{
			goto cleanup;
		}
		if (same)
		{
			if (pDesktop != pChild)
				pDesktop->Release();
			if (pParent != pChild)
				pParent->Release();
			pWalker->Release();
			// Reached desktop, so return next element below it.
			return pNode;
		}
		if (pNode != pChild) // Do not release the in-param.
			pNode->Release();

		CONTROLTYPEID controlType;
		hr = pParent->get_CurrentControlType(&controlType);

		if (SUCCEEDED(hr))
		{
			if (controlType == UIA_WindowControlTypeId)	// parent window
			{
				if (pDesktop != pChild)
					pDesktop->Release();
				pWalker->Release();
				return pParent;
			}
		}
		else
		{
			goto cleanup;
		}
		pNode = pParent;
	}

cleanup:
	if ((pNode != NULL) && (pNode != pChild))
		pNode->Release();

	if (pDesktop != NULL && pDesktop != pChild)
		pDesktop->Release();

	if (pWalker != NULL)
		pWalker->Release();

	if (pParent != NULL && pParent != pChild)
		pParent->Release();

	return NULL;
}


// CAUTION: Do not pass in the root (desktop) element. Traversing the entire subtree
// of the desktop could take a very long time and even lead to a stack overflow.
void EventRecorder::ListDescendants(IUIAutomationElement* pParent, int indent)
{
	if (pParent == NULL)
		return;
	IUIAutomationTreeWalker* pDescendantControlWalker = NULL;
	IUIAutomationElement* pNode = NULL;

	HRESULT hr = g_pAutomation->get_ControlViewWalker(&pDescendantControlWalker);
	if (hr != S_OK)
	{
		goto cleanup;
	}
	if (pDescendantControlWalker == NULL)
		goto cleanup;
	

	hr = pDescendantControlWalker->GetFirstChildElement(pParent, &pNode);
	if (hr != S_OK)
	{
		goto cleanup;
	}
	if (pNode == NULL)
		goto cleanup;

	while (pNode)
	{
		BSTR desc;
		hr = pNode->get_CurrentLocalizedControlType(&desc);
		if (hr != S_OK)
		{
			goto cleanup;
		}
		for (int x = 0; x <= indent; x++)
		{
			std::wcout << L"   ";
		}
		std::wcout << desc << L"\n";
		SysFreeString(desc);

		ListDescendants(pNode, indent + 1);
		IUIAutomationElement* pNext;
		hr = pDescendantControlWalker->GetNextSiblingElement(pNode, &pNext);
		if (hr != S_OK)
		{
			goto cleanup;
		}
		hr = pNode->Release();
		if (hr != S_OK)
		{
			goto cleanup;
		}
		pNode = pNext;
	}

cleanup:
	if (pDescendantControlWalker != NULL)
		pDescendantControlWalker->Release();

	if (pNode != NULL)
		pNode->Release();
	return;
}

bool EventRecorder::IsNameRequired(CONTROLTYPEID controlTypeID)
{
	bool retVal = false;

	switch (controlTypeID)
	{
	case UIA_ButtonControlTypeId:
	case UIA_CheckBoxControlTypeId:
	case UIA_HyperlinkControlTypeId:
	case UIA_ListItemControlTypeId:
	case UIA_MenuBarControlTypeId:
	case UIA_MenuItemControlTypeId:
	case UIA_RadioButtonControlTypeId:
	case UIA_TabItemControlTypeId:
	case UIA_TreeItemControlTypeId:
	case UIA_DataItemControlTypeId:
	case UIA_HeaderItemControlTypeId:
	case UIA_WindowControlTypeId:
		retVal = true;
		break;
	case UIA_CalendarControlTypeId:
	case UIA_ComboBoxControlTypeId:
	case UIA_EditControlTypeId:
	case UIA_ImageControlTypeId:
	case UIA_ListControlTypeId:
	case UIA_MenuControlTypeId:
	case UIA_ProgressBarControlTypeId:
	case UIA_ScrollBarControlTypeId:
	case UIA_SliderControlTypeId:
	case UIA_SpinnerControlTypeId:
	case UIA_StatusBarControlTypeId:
	case UIA_TabControlTypeId:
	case UIA_TextControlTypeId:
	case UIA_ToolBarControlTypeId:
	case UIA_ToolTipControlTypeId:
	case UIA_TreeControlTypeId:
	case UIA_CustomControlTypeId:
	case UIA_ThumbControlTypeId:
	case UIA_DataGridControlTypeId:
	case UIA_DocumentControlTypeId:
	case UIA_SplitButtonControlTypeId:
	case UIA_PaneControlTypeId:
	case UIA_HeaderControlTypeId:
	case UIA_TableControlTypeId:
	case UIA_TitleBarControlTypeId:
	case UIA_SeparatorControlTypeId:
	case UIA_SemanticZoomControlTypeId:
	case UIA_AppBarControlTypeId:
		retVal = false;
		break;
	default:
		retVal = false;
		break;
	}
	return retVal;
}

