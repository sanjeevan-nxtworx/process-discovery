#include "stdafx.h"
#include "EventCaptureURL.h"
#include <combaseapi.h>
#include <comutil.h>
#include <mutex>
#include "utility.h"

using namespace std;

DWORD WINAPI ExecuteCaptureTimerFunction(LPVOID lpParameter);
DWORD WINAPI ExecuteCaptureUIAThread(LPVOID lpParamenter);
IUIAutomation* g_pCaptureUIA = NULL;
mutex captureLockEvent;
static LRESULT CALLBACK TransparentWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void EventCaptureURL::MouseCallback(int nEventCode, ULONG lParam)
{
	if (nEventCode == 30)
	{
		captureLockEvent.lock();
		if (!moveWaitFlg)
		{
			moveTimer = 50;
		}
		captureLockEvent.unlock();
	}
}

void EventCaptureURL::KeyboardCallback(int mEventNum, WORD vk_code, WORD scanCode)
{
	if (!moveWaitFlg && mEventNum == 21)
	{
		if (vk_code == VK_LCONTROL)
		{
			// Capture the URL and data of this control, and populate the dialog with the data
			// Then the user can enter the data and save
			moveWaitFlg = true;
			UIAMessages.push_back(2);
			eventUIASem.notify();
		}
	}
}

bool EventCaptureURL::IsNewEventCaptured()
{
	return newCaptureFlg;
}

void EventCaptureURL::ClearCapturedEvent()
{
	if (pCapturedElementData != NULL)
		delete pCapturedElementData;
	pCapturedElementData = NULL;
}


void EventCaptureURL::StartEventProcessing()
{
	captureLockEvent.lock();
	moveWaitFlg = true;
	newCaptureFlg = false;
	captureLockEvent.unlock();
}

void EventCaptureURL::EndEventProcessing()
{
	captureLockEvent.lock();
	moveWaitFlg = false;
	
	captureLockEvent.unlock();
}

CaptureElementData* EventCaptureURL::GetCapturedEvent()
{
	return pCapturedElementData;
}

void EventCaptureURL::TimerEnd()
{
	HANDLE hTimer = NULL;
	LARGE_INTEGER liDueTime;
	static time_t oldTime;
	static int eventRepeatCount;
	__int64 qwDueTime;

	qwDueTime = -200000LL; // 20 ms

	liDueTime.LowPart = (DWORD)(qwDueTime & 0xFFFFFFFF);
	liDueTime.HighPart = (LONG)(qwDueTime >> 32);

	// Create an unnamed waitable timer.
	hTimer = CreateWaitableTimer(NULL, FALSE, NULL);
	if (NULL == hTimer)
	{
		return;
	}

	// Set a timer to wait for 20 ms.
	if (!SetWaitableTimer(hTimer, &liDueTime, 20, NULL, NULL, 0))
	{
		return;
	}

	while (true)
	{
		captureLockEvent.lock();
		bool bFlg = bQuitCapture;
		int mTimeCnt = moveTimer;
		captureLockEvent.unlock();
		if (bFlg)
			break;
		if (WaitForSingleObject(hTimer, INFINITE) != WAIT_OBJECT_0)
		{
			return;
		}
		if (mTimeCnt > 0)
		{
			mTimeCnt--;

			captureLockEvent.lock();
			if (mTimeCnt == 0)
			{
				moveWaitFlg = true;
				UIAMessages.push_back(1);
				eventUIASem.notify();
				// Here capture the element under the mouse and draw rectangle on the screen
			}
			moveTimer = mTimeCnt;
			captureLockEvent.unlock();
		}
	}
	CancelWaitableTimer(hTimer);
	CloseHandle(hTimer);
}

EventCaptureURL::EventCaptureURL() : timerThread(NULL), UIAThread(NULL), timerThreadID(NULL),
					UIAThreadID(NULL), moveWaitFlg(false), moveTimer(0), bQuitCapture(false),
					rectElement(), pCapturedElement(nullptr), overLapWindow(NULL), pCapturedElementData(nullptr),
					newCaptureFlg(false)
{
	// Create windows
	overLapWindow = CreateTransparentWindow()->GetTransparentWindowHandle();
}


EventCaptureURL::~EventCaptureURL()
{
	// Delete window
	if (pCapturedElement != NULL)
		pCapturedElement->Release();
	ShowWindow(overLapWindow, SW_HIDE);
	//DestroyWindow(overLapWindow);
}

void EventCaptureURL::QuitCapture()
{
	bQuitCapture = true;
	Sleep(1000);
}

void EventCaptureURL::KillUIAThread()
{
	eventUIASem.notify();
	Sleep(100);
	if (UIAThread != NULL)
	{
		WaitForSingleObject(UIAThread, INFINITE);
		CloseHandle(UIAThread);
		UIAThread = NULL;
	}

}

void EventCaptureURL::KillTimerThread()
{
	Sleep(100);
	if (timerThread != NULL)
	{
		WaitForSingleObject(timerThread, INFINITE);
		CloseHandle(timerThread);
		timerThread = NULL;
	}
}

void EventCaptureURL::UIAThreadFunction()
{

	// This is the main function, it starts processing when eventUIASem is unlocked and will handle the meesage queue
	// Elements in the message queue have 2 states, 1 to get data if the event is deciphered and 2 to resolve and put in queue


	while (true)
	{
		eventUIASem.wait();
		bool bFlg;
		captureLockEvent.lock();
		bFlg = bQuitCapture;
		captureLockEvent.unlock();
		if (bFlg)
		{
			break;
		}
		if (UIAMessages.size() > 0)
		{
			captureLockEvent.lock();
			unsigned int func = UIAMessages.front();
			UIAMessages.pop_front();
			captureLockEvent.unlock();
			if (func == 1)  // Capture the control data at mouse position
			{
				CaptureElementAtPoint();

			}
			else if (func == 2) // Capture function data at mouse position
			{
				CaptureElementDataAtPoint();
			}
		}	
		Sleep(500);
	}
}

void EventCaptureURL::CaptureElementDataAtPoint()
{
	// If chrome or explorer then get URL else get windows title
	// Get the control and its parents, right and left siblings
	// get name
	// show in dialog
	if (pCapturedElement == NULL)
	{
		moveWaitFlg = false;
		return;
	}

	int pid;
	int captureType = 0;

	pCapturedElement->get_CachedProcessId(&pid);
	string processName = ProcessIdToName(pid);
	vector <string> tokens = split(processName, "\\");
	string lastToken = tokens.back();
	string windowName = "";
	if (lastToken == "msedge.exe" || lastToken == "chrome.exe")
	{
		// Here get the URL
		windowName = GetEdgeURL(pCapturedElement);
		captureType = 2;
	}
	else
	{
		// Go up the captured element hierarchy until a window is reached, then get the title of the window
		windowName = GetWindowTitle(pCapturedElement);
		captureType = 1;
	}
	processName = lastToken;
	CaptureElementData *pCaptureData = GetControlData(pCapturedElement);

	if (pCaptureData != NULL)
	{
		pCaptureData->SetProcessName(processName);
		pCaptureData->SetWindowName(windowName);
		pCaptureData->SetCaptureType(captureType);
		if (pCapturedElementData != NULL)
			delete pCapturedElementData;
		pCapturedElementData = pCaptureData;
		captureLockEvent.lock();
		newCaptureFlg = true;
		captureLockEvent.unlock();
	}
	moveWaitFlg = false;
}

CaptureElementData* EventCaptureURL::GetControlData(IUIAutomationElement* pElement)
{
	if (pElement == NULL)
		return NULL;

	CaptureElementData* pCaptureElementData = DBG_NEW CaptureElementData;

	GetCachedElementDetails(pCaptureElementData->GetElementData(), pElement);

	IUIAutomationTreeWalker* pControlWalker = NULL;

	g_pCaptureUIA->get_ControlViewWalker(&pControlWalker);
	if (pControlWalker == NULL)
	{
		return NULL;
	}

	IUIAutomationElement* pNode = pElement;
	IUIAutomationElement* pDesktop = NULL;

	HRESULT hr = g_pCaptureUIA->GetRootElement(&pDesktop);
	if (FAILED(hr))
	{
		pControlWalker->Release();
		return NULL;
	}

	pNode->AddRef();
	IUIAutomationElement* pParent = NULL;
	int num = 0;
	// only take 5 relatives of each side
	while (true)
	{
		pParent = NULL;
		if (SUCCEEDED(pControlWalker->GetParentElement(pNode, &pParent)))
		{
			BOOL bAreSame = false;
			if (pParent != NULL)
			{
				g_pCaptureUIA->CompareElements(pParent, pDesktop, &bAreSame);
			}
			if (pParent == NULL || bAreSame)
			{
				break;
			}
			ElementData* pElementData = DBG_NEW ElementData;
			GetCurrentElementDetails(pElementData, pParent);
			pCaptureElementData->GetParentList()->push_back(pElementData);
			pNode->Release();
			pNode = pParent;
		}
		else
		{
			break;
		}
		num++;
		if (num > 5)
			break;
	}

	pNode = pElement;
	pNode->AddRef();
	num = 0;
	while (true)
	{
		IUIAutomationElement* pLeftSibling = NULL;
		if (SUCCEEDED(pControlWalker->GetPreviousSiblingElement(pNode, &pLeftSibling)))
		{
			if (pLeftSibling == NULL)
			{
				break;
			}
			ElementData* pElementData = DBG_NEW ElementData;
			GetCurrentElementDetails(pElementData, pLeftSibling);
			pCaptureElementData->GetLeftSiblingList()->push_back(pElementData);
			pNode->Release();
			pNode = pLeftSibling;
		}
		else
			break;
		num++;
		if (num > 5)
			break;
	}

	pNode = pElement;
	pNode->AddRef();
	IUIAutomationElement* pRightSibling = NULL;
	num = 0;
	while (true)
	{
		if (SUCCEEDED(pControlWalker->GetNextSiblingElement(pNode, &pRightSibling)))
		{
			if (pRightSibling == NULL)
			{
				break;
			}
			ElementData* pElementData = DBG_NEW ElementData;
			GetCurrentElementDetails(pElementData, pRightSibling);
			pCaptureElementData->GetRightSiblingList()->push_back(pElementData);
			pNode->Release();
			pNode = pRightSibling;
		}
		else
			break;
		num++;
		if (num > 5)
			break;
	}
	pControlWalker->Release();
	return pCaptureElementData;
}

void EventCaptureURL::GetCachedElementDetails(ElementData* pElementDetails, IUIAutomationElement* pElement)
{
	HRESULT hr;
	BSTR str = NULL;
	string autoID;
	string className;
	string itemType;
	string name;
	RECT boundingRect;
	CONTROLTYPEID controlType;
	int processID;

	hr = pElement->get_CachedAutomationId(&str);
	if (SUCCEEDED(hr))
	{
		autoID = ConvertBSTRToMBS(str);
		SysFreeString(str);
		str = NULL;
	}
	hr = pElement->get_CachedBoundingRectangle(&boundingRect);
	hr = pElement->get_CachedControlType(&controlType);
	hr = pElement->get_CachedProcessId(&processID);
	hr = pElement->get_CachedClassName(&str);
	if (SUCCEEDED(hr))
	{
		className = ConvertBSTRToMBS(str);
		SysFreeString(str);
		str = NULL;
	}

	hr = pElement->get_CachedItemType(&str);
	if (SUCCEEDED(hr))
	{
		itemType = ConvertBSTRToMBS(str);
		SysFreeString(str);
		str = NULL;
	}
	hr = pElement->get_CachedName(&str);
	if (SUCCEEDED(hr))
	{
		name = ConvertBSTRToMBS(str);
		SysFreeString(str);
		str = NULL;
	}
	pElementDetails->SetAutomationID(autoID);
	pElementDetails->SetBoundingRect(&boundingRect);
	pElementDetails->SetClassName(className);
	pElementDetails->SetControlName(name);
	pElementDetails->SetControlType(controlType);
	pElementDetails->SetItemType(itemType);
}

void EventCaptureURL::GetCurrentElementDetails(ElementData* pElementDetails, IUIAutomationElement* pElement)
{
	HRESULT hr;
	BSTR str = NULL;
	string autoID;
	string className;
	string itemType;
	string name;
	RECT boundingRect;
	CONTROLTYPEID controlType;
	int processID;

	hr = pElement->get_CurrentAutomationId(&str);
	if (SUCCEEDED(hr))
	{
		autoID = ConvertBSTRToMBS(str);
		SysFreeString(str);
		str = NULL;
	}
	hr = pElement->get_CurrentBoundingRectangle(&boundingRect);
	hr = pElement->get_CurrentControlType(&controlType);
	hr = pElement->get_CurrentProcessId(&processID);
	hr = pElement->get_CurrentClassName(&str);
	if (SUCCEEDED(hr))
	{
		className = ConvertBSTRToMBS(str);
		SysFreeString(str);
		str = NULL;
	}

	hr = pElement->get_CurrentItemType(&str);
	if (SUCCEEDED(hr))
	{
		itemType = ConvertBSTRToMBS(str);
		SysFreeString(str);
		str = NULL;
	}
	hr = pElement->get_CurrentName(&str);
	if (SUCCEEDED(hr))
	{
		name = ConvertBSTRToMBS(str);
		SysFreeString(str);
		str = NULL;
	}
	pElementDetails->SetAutomationID(autoID);
	pElementDetails->SetBoundingRect(&boundingRect);
	pElementDetails->SetClassName(className);
	pElementDetails->SetControlName(name);
	pElementDetails->SetControlType(controlType);
	pElementDetails->SetItemType(itemType);
}


string EventCaptureURL::GetWindowTitle(IUIAutomationElement* pElement)
{
	IUIAutomationTreeWalker* pWalker = NULL;
	HRESULT hr = g_pCaptureUIA->get_ControlViewWalker(&pWalker);
	if (FAILED(hr))
		return "";

	IUIAutomationElement* pNodeElement = pElement;
	pNodeElement->AddRef();
	while (true)
	{
		IUIAutomationElement* pParent = NULL;
//		IUIAutomationElement* pTestParent = NULL;

		hr = pWalker->GetParentElement(pNodeElement, &pParent);
		if (FAILED(hr) || pParent == NULL)
		{
			break;
		}
		CONTROLTYPEID type;

		hr = pParent->get_CurrentControlType(&type);
		if (SUCCEEDED(hr) && type == UIA_WindowControlTypeId)
		{
			BSTR str = NULL;
			string strName = "";
			hr = pParent->get_CurrentName(&str);
			if (SUCCEEDED(hr))
			{
				int wslen = ::SysStringLen(str);
				strName = ConvertWCSToMBS((wchar_t*)str, wslen);
				SysFreeString(str);
				str = NULL;
				pParent->Release();
				pWalker->Release();
				return (strName);
			}
		}
		pNodeElement->Release();
		pNodeElement = pParent;
	}
	pWalker->Release();
	return "";
}

string EventCaptureURL::GetEdgeURL(IUIAutomationElement *pElement)
{

	IUIAutomationTreeWalker* pWalker = NULL;
	HRESULT hr = g_pCaptureUIA->get_ControlViewWalker(&pWalker);
	if (FAILED(hr))
		return "";

	IUIAutomationElement* pNodeElement = pElement;
	pNodeElement->AddRef();
	while (true)
	{
		IUIAutomationElement* pParent = NULL;

		hr = pWalker->GetParentElement(pNodeElement, &pParent);
		if (FAILED(hr) || pParent == NULL)
		{
			pWalker->Release();
			break;
		}
		BSTR str = NULL;
		string strName = "";
		hr = pParent->get_CurrentName(&str);
		if (SUCCEEDED(hr))
		{
			int wslen = ::SysStringLen(str);
			strName = ConvertWCSToMBS((wchar_t*)str, wslen);
			SysFreeString(str);
			str = NULL;
		}

		string strClassName = "";
		hr = pParent->get_CurrentClassName(&str);
		if (SUCCEEDED(hr))
		{
			int wslen = ::SysStringLen(str);
			strClassName = ConvertWCSToMBS((wchar_t*)str, wslen);
			SysFreeString(str);
			str = NULL;
		}

		if (strName == "Microsoft Edge" || strClassName == "Chrome_WidgetWin_1")
		{
			strClassName = GetBrowserEdgeURLName(pParent);
			pParent->Release();
			pWalker->Release();
			return strClassName;
		}

		pNodeElement->Release();
		pNodeElement = pParent;
	}

	pWalker->Release();
	return "";
}


string EventCaptureURL::GetBrowserEdgeURLName(IUIAutomationElement* pElement)
{
	if (g_pCaptureUIA == NULL)
		return "";

	IUIAutomationCondition* pControlNameCondition = NULL;
	IUIAutomationElement* pFound = NULL;
	// Create a property condition for the button control type.
	VARIANT varProp;
	string urlName = "";

	varProp.vt = VT_BSTR;
	varProp.bstrVal = SysAllocString(L"Search or enter web address");
	g_pCaptureUIA->CreatePropertyCondition(UIA_NamePropertyId, varProp, &pControlNameCondition);
	if (pControlNameCondition != NULL)
	{
		pElement->FindFirst(TreeScope_Descendants, pControlNameCondition, &pFound);
	}
	if (pControlNameCondition != NULL)
		pControlNameCondition->Release();
	if (pFound == NULL)
	{
		// Then search for Microsoft Edge Chromium
		VARIANT varProp2;
		varProp2.vt = VT_BSTR;
		varProp2.bstrVal = SysAllocString(L"Address and search bar");
		g_pCaptureUIA->CreatePropertyCondition(UIA_NamePropertyId, varProp2, &pControlNameCondition);
		if (pControlNameCondition != NULL)
		{
			pElement->FindFirst(TreeScope_Descendants, pControlNameCondition, &pFound);
		}
		if (pControlNameCondition != NULL)
			pControlNameCondition->Release();
		VariantClear(&varProp2);
	}
	VariantClear(&varProp);

	if (pFound != NULL)
	{
		IUIAutomationElement* pURLElement = pFound;
		VARIANT varValue;
		VariantInit(&varValue);
		pURLElement->GetCurrentPropertyValue(UIA_ValueValuePropertyId, &varValue);
		if (varValue.vt == VT_BSTR && varValue.bstrVal != NULL)
		{
			wstring wString(varValue.bstrVal);
			urlName = ws2s(wString);
		}
		pFound->Release();
	}
	return urlName;
}


void EventCaptureURL::CaptureElementAtPoint()
{

	POINT pt;
	GetPhysicalCursorPos(&pt);

	DWORD processID = NULL;
	GetWindowThreadProcessId(WindowFromPoint(pt), &processID);
	if (processID == GetCurrentProcessId())
	{
		moveWaitFlg = false;
		return;
	}

	IUIAutomationCacheRequest* pCacheRequest = NULL;

	IUIAutomationElement* pElement = NULL;
	GetCachedElementAtPoint(pt, &pElement, &pCacheRequest);
	if (pElement == NULL)
	{
		moveWaitFlg = false;
		return;
	}
	// Here get the rectangle and pElement return
	if (pCapturedElement != NULL)
		pCapturedElement->Release();
	pCapturedElement = pElement;
	pCapturedElement->get_CachedBoundingRectangle(&rectElement);
	SetWindowPos(overLapWindow, HWND_TOPMOST, rectElement.left, rectElement.top, rectElement.right - rectElement.left, rectElement.bottom - rectElement.top, SWP_SHOWWINDOW);
	moveWaitFlg = false;

}

void EventCaptureURL::GetCachedElementAtPoint(POINT pt, IUIAutomationElement** ppControl, IUIAutomationCacheRequest** pCacheRequest)
{
	*ppControl = NULL;

	IUIAutomationCacheRequest* pCacheRequestLocal = NULL;

	HRESULT hr = CreateCacheRequest(&pCacheRequestLocal);
	if (hr != S_OK)
		return;

	hr = g_pCaptureUIA->ElementFromPointBuildCache(pt, pCacheRequestLocal, ppControl);
	if (SUCCEEDED(hr))
	{
		// Now drill down to child elements and get the element that falls in the pt
		IUIAutomationTreeWalker* pControlWalker = NULL;
		g_pCaptureUIA->get_ControlViewWalker(&pControlWalker);
		if (pControlWalker == NULL)
		{
			*pCacheRequest = pCacheRequestLocal;
			return;
		}

		IUIAutomationElement* pNode = *ppControl;
		IUIAutomationElement* pDesktop = NULL;

		hr = g_pCaptureUIA->GetRootElement(&pDesktop);
		if (FAILED(hr))
		{
			pControlWalker->Release();
			*pCacheRequest = pCacheRequestLocal;
			return;
		}

		BOOL bAreSame = false;
		g_pCaptureUIA->CompareElements(pNode, pDesktop, &bAreSame);
		if (bAreSame)
		{
			pControlWalker->Release();
			pDesktop->Release();
			*pCacheRequest = pCacheRequestLocal;
			return;
		}

		IUIAutomationElement* pElementAtPoint = GetElementAtPoint(pt, pNode, pCacheRequest, pControlWalker);

		if (pElementAtPoint != NULL)
		{
			CONTROLTYPEID controlType;

			pElementAtPoint->get_CachedControlType(&controlType);
			if (IsControlTypeActionable(controlType))
			{
				pNode->Release();
				pCacheRequestLocal->Release();
				*ppControl = pElementAtPoint;
			}
			else
			{
				pCacheRequestLocal->Release();
			}
		}
		else
		{
			*pCacheRequest = pCacheRequestLocal;
		}
		pControlWalker->Release();
		pDesktop->Release();
	}
	else
		pCacheRequestLocal->Release();
}



IUIAutomationElement* EventCaptureURL::GetElementAtPoint(POINT pt, IUIAutomationElement* pControl, IUIAutomationCacheRequest** pCacheRequest, IUIAutomationTreeWalker* pControlWalker)
{
	// Here get First Child, check if pt is within rectangle then call this function recursively, 
	//		if call returns not null then use the returned element and cache request, else
	//		use this the current cache and value
	// if pt is not in rectangle, then go next sibling, if pt is within rectangle then call this function recursively
	//		if call returns not null then use the returned element and cache request, else
	//		use this the current cache and value
	// if not found then return null

	IUIAutomationCacheRequest* pCacheRequestLocal = NULL;

	HRESULT hr = CreateCacheRequest(&pCacheRequestLocal);
	if (hr != S_OK)
		return NULL;

	IUIAutomationElement* pNode = NULL;

	hr = pControlWalker->GetFirstChildElementBuildCache(pControl, pCacheRequestLocal, &pNode);
	if (SUCCEEDED(hr) && pNode != NULL)
	{
		RECT rectVal;

		pNode->get_CachedBoundingRectangle(&rectVal);
		if (PtInRect(&rectVal, pt))
		{
			// If the child is in the point then go down to its children
			IUIAutomationElement* pChildElement = GetElementAtPoint(pt, pNode, pCacheRequest, pControlWalker);
			if (pChildElement != NULL)
			{
				// Here check if it is a actionalble element then update the required variables
				CONTROLTYPEID controlType;

				pChildElement->get_CachedControlType(&controlType);
				if (IsControlTypeActionable(controlType))
				{
					pNode->Release();
					pCacheRequestLocal->Release();
					return pChildElement;
				}
				else
				{
					*pCacheRequest = pCacheRequestLocal;
					return pNode;
				}
			}
			else
			{
				*pCacheRequest = pCacheRequestLocal;
				return pNode;
			}
		}
	}
	else
	{
		pCacheRequestLocal->Release();
		return NULL;
	}
	while (true)
	{
		IUIAutomationElement* pSibling = NULL;
		hr = pControlWalker->GetNextSiblingElementBuildCache(pNode, pCacheRequestLocal, &pSibling);
		if (FAILED(hr) || pSibling == NULL)
			break;

		RECT rectVal;

		pSibling->get_CachedBoundingRectangle(&rectVal);
		if (PtInRect(&rectVal, pt))
		{
			// If the child is in the point then go down to its children
			IUIAutomationElement* pChildElement = GetElementAtPoint(pt, pSibling, pCacheRequest, pControlWalker);
			if (pChildElement != NULL)
			{
				// Here check if it is a actionalble element then update the required variables
				CONTROLTYPEID controlType;

				pChildElement->get_CachedControlType(&controlType);
				pNode->Release();
				if (IsControlTypeActionable(controlType))
				{
					pSibling->Release();
					pCacheRequestLocal->Release();
					return pChildElement;
				}
				else
				{
					*pCacheRequest = pCacheRequestLocal;
					return pSibling;
				}
			}
			else
			{
				pNode->Release();
				*pCacheRequest = pCacheRequestLocal;
				return pSibling;
			}
		}
		pNode->Release();
		pNode = pSibling;
	}

	return NULL;
}


bool EventCaptureURL::IsControlTypeActionable(CONTROLTYPEID controlTypeID)
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
		retVal = true;
		break;

	case UIA_PaneControlTypeId:
	case UIA_StatusBarControlTypeId:
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


HRESULT EventCaptureURL::CreateCacheRequest(IUIAutomationCacheRequest** pRequest)
{
	*pRequest = NULL;
	HRESULT hr;
	IUIAutomationCacheRequest* pCacheRequest = NULL;
	hr = g_pCaptureUIA->CreateCacheRequest(&pCacheRequest);
	if (FAILED(hr))
	{
		return hr;
	}
	hr = pCacheRequest->AddProperty(UIA_NamePropertyId);
	if (FAILED(hr))
	{
		pCacheRequest->Release();
		return hr;
	}
	hr = pCacheRequest->AddProperty(UIA_AutomationIdPropertyId);
	if (FAILED(hr))
	{
		pCacheRequest->Release();
		return hr;
	}
	hr = pCacheRequest->AddProperty(UIA_BoundingRectanglePropertyId);
	if (FAILED(hr))
	{
		pCacheRequest->Release();
		return hr;
	}

	hr = pCacheRequest->AddProperty(UIA_ControlTypePropertyId);
	if (FAILED(hr))
	{
		pCacheRequest->Release();
		return hr;
	}

	hr = pCacheRequest->AddProperty(UIA_ProcessIdPropertyId);
	if (FAILED(hr))
	{
		pCacheRequest->Release();
		return hr;
	}

	hr = pCacheRequest->AddProperty(UIA_ClassNamePropertyId);
	if (FAILED(hr))
	{
		pCacheRequest->Release();
		return hr;
	}

	hr = pCacheRequest->AddProperty(UIA_ItemTypePropertyId);
	if (FAILED(hr))
	{
		pCacheRequest->Release();
		return hr;
	}

	*pRequest = pCacheRequest;
	return S_OK;
}

void EventCaptureURL::CreateTimerThread()
{
	timerThread = CreateThread(0, 0, ExecuteCaptureTimerFunction, this, 0, &timerThreadID);
}

void EventCaptureURL::CreateUIAThread()
{
	UIAThread = CreateThread(0, 0, ExecuteCaptureUIAThread, this, 0, &UIAThreadID);
}

DWORD WINAPI ExecuteCaptureTimerFunction(LPVOID lpParameter)
{
	EventCaptureURL* eventRecorder = (EventCaptureURL*)lpParameter;
	eventRecorder->TimerEnd();
	return 0;
}

DWORD WINAPI ExecuteCaptureUIAThread(LPVOID lpParameter)
{
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	EventCaptureURL* eventRecorder = (EventCaptureURL*)lpParameter;
	CoCreateInstance(__uuidof(CUIAutomation),
		NULL, CLSCTX_INPROC_SERVER,
		__uuidof(IUIAutomation),
		(void**)&g_pCaptureUIA);
	eventRecorder->UIAThreadFunction();
	g_pCaptureUIA->Release();
	g_pCaptureUIA = NULL;
	CoUninitialize();
	return 0;
}
