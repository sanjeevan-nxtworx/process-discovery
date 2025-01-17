#include "stdafx.h"
#include "BPMNFacade.h"
#include "BPMNStart.h"
#include "BPMNStop.h"
#include "BPMNSubProcess.h"
#include "BPMNTask.h"
#include "BPMNDecision.h"
#include "json11.hpp"
#include "spdlog/spdlog.h"
#include "Utility.h"
using namespace json11;

BPMNFacade::BPMNFacade()
{
	pAnnotationRules = NULL;
	eventMap.clear();
	widgetMap.clear();
}


BPMNFacade::~BPMNFacade()
{
	if (pAnnotationRules != NULL)
		delete pAnnotationRules;


	map <ULONG, struct eventListElement *>::iterator itEvent = eventMap.begin();
	while (itEvent != eventMap.end())
	{
		struct eventListElement *pEventList = itEvent->second;
		BPMNElement *pElement = pEventList->pElement;
		if (pElement != NULL)
		{
			delete pElement;
			pEventList->pElement = NULL;
		}
		delete pEventList;
		itEvent++;
	}

	eventMap.clear();
	map <ULONG, struct widgetDetails *>::iterator itWidget = widgetMap.begin();
	while (itWidget != widgetMap.end())
	{
		struct widgetDetails *pWidget = itWidget->second;
		pWidget->nameMap.clear();
		delete pWidget;
		itWidget++;
	}


}

list <BPMNElement *> *BPMNFacade::GetBPMNDiagram()
{
	return &bpmnDiagram;
}

map <ULONG, struct eventListElement *> *BPMNFacade::GetElementMap()
{
	return &eventMap;
}

RECT *BPMNFacade::GetBoundingRect(int eventIndex)
{
	map <ULONG, struct eventListElement *>::iterator itEventCheck = eventMap.find(eventIndex);
	if (itEventCheck == eventMap.end())
		return NULL; // already exists;
	struct eventListElement *pEventList = itEventCheck->second;
	return &pEventList->boundingRect;

}

void BPMNFacade::GetAnnotationRules(char *fileName)
{
	pAnnotationRules = DBG_NEW AnnotationRuleDefinitions;
	pAnnotationRules->ReadRules(fileName);
	pAnnotationRules->ParseRules();
}

void BPMNFacade::CreateRecorderEvent(struct eventDetails *pEvent, struct eventBufferRecord *pEventBuffer, ULONG eventIndexVal)
{
	static long long durationAccumulator = 0LL;
	long long durationVal = 0LL;

	pEvent->timestamp = pEventBuffer->timeStamp;
	pEvent->nEventCode = pEventBuffer->nEventCode;
	//pEvent->nVK_Code = pEventBuffer->keyVKCode;
	//pEvent->nScan_Code = pEventBuffer->keyScanCode;
	pEvent->eventIndex = eventIndexVal;
	pEvent->elementID = pEventBuffer->nEventID;
	pEvent->nParentEventIndex = 0L;
	pEvent->nEventRecType = 0;
	pEvent->mousePt.x = pEventBuffer->mouseX;
	pEvent->mousePt.y = pEventBuffer->mouseY;
	CopyMemory(&pEvent->boundingRect, &pEventBuffer->boundingRect, sizeof(RECT));
	// Get Widget ID for this controls
	pEvent->strAnnotation = "";
	pEvent->strNotes = "";
	pEvent->strKeyStrokes = pEventBuffer->strKeyStrokes;
	string err;
	pEvent->jSonElement = Json::parse(pEventBuffer->strDescriptor, err);
	string strAnnotation  = pAnnotationRules->GetAnnotationForEvent(pEvent, &widgetMap);
	string strInfo = pAnnotationRules->GetAdditionalInfo(pEvent, &widgetMap);
	string strJSon = "{\n";
	strAnnotation = EscapeJSonAnnotationString(strAnnotation);
	// strNotes has the following definitions
	// 1. Annotation
	// 2. CType - Control Type
	// 3. PName - Process Name
	// 4. Class - Class Name
	// 5. WName - Window Name
	// 6. WRect - Window Rect
	// 7. DocName - Name
	// 8. Name - Element Name
	// 9. Duration - Duration

	strJSon += "\"Annotation\": \"" + strAnnotation + "\",\n";
	strJSon += strInfo;
	if (pEvent->jSonElement.is_object())
	{
		json11::Json descriptor = pEvent->jSonElement.object_items();
		
		strJSon += "\"DocName\": \"" + EscapeJSonAnnotationString(descriptor["Document"].string_value()) + "\",\n";
		strJSon += "\"Name\": \"" + EscapeJSonAnnotationString(descriptor["Name"].string_value()) + "\",\n";
		char str[100];
		durationVal = (long long)(descriptor["Duration"].int_value()) + durationAccumulator;
		sprintf_s(str, "\"Duration\": %lld\n", durationVal);
		strJSon += str;
	}
	strJSon += "}";
	pEvent->strNotes = strJSon;
	pEvent->strAnnotation = strAnnotation; // to keep compatability with older versions
	spdlog::info("Adding Event Annotation {}", strAnnotation);
	if (strAnnotation == "")
		durationAccumulator = durationVal;
	else
		durationAccumulator = 0LL;
}

list <BPMNElement *> *BPMNFacade::GetElementList(BPMNElement *pElement)
{
	BPMNElement *pParent = pElement->GetParentElement();
	if (pParent == NULL)
		return &bpmnDiagram;
	if (pParent->GetType() == 3) // SubProcess
	{
		BPMNSubProcess *pSubProcess = (BPMNSubProcess *)pParent;
		return pSubProcess->GetSubProcessElements();
	}
	if (pParent->GetType() == 4)
	{
		BPMNDecision *pDecision = (BPMNDecision *)pParent;
		return pDecision->GetDecisionElementList(pElement);
	}
	return NULL;
}

struct eventListElement *BPMNFacade::GetEventInDiagram(ULONG nIndex)
{

	map <ULONG, struct eventListElement *>::iterator itEventCheck = eventMap.find(nIndex);
	if (itEventCheck == eventMap.end())
		return NULL; // already exists;

	return itEventCheck->second;
}

struct eventListElement *BPMNFacade::AddEventInDiagram(struct eventFileRecord *pEventRecord, struct eventDetails *pEvent, long long pos)
{
	BPMNElement *pElement = NULL;
	BPMNElement *pPrevElement = NULL;

	map <ULONG, struct eventListElement *>::iterator itEventCheck = eventMap.find(pEvent->eventIndex);
	if (itEventCheck != eventMap.end())
		return NULL; // already exists;
	if (bpmnDiagram.size() == 0)
	{
		pElement = DBG_NEW BPMNStart();
		bpmnDiagram.push_back(pElement);
	}
	char strIndexNum[20];
	sprintf_s(strIndexNum, "%ld", pEvent->eventIndex);

	struct eventListElement *prevEventListElement = NULL;
	if ((pEventRecord->nEventType & 0x04) == 0x04) // Decision
	{
		pElement = (BPMNElement *)DBG_NEW BPMNDecision(pEvent->eventIndex);
		pElement->SetAnnotation(pEvent->strAnnotation + strIndexNum);
		pElement->SetNotes(pEvent->strNotes);
		pElement->SetEventTimestamp(pEvent->timestamp);
		pElement->SetBoundingRect(&pEvent->boundingRect);

	}
	else if ((pEventRecord->nEventType & 0x02) == 0x02) // SubProcess
	{
		pElement = (BPMNElement *)DBG_NEW BPMNSubProcess(pEvent->eventIndex);
		pElement->SetAnnotation(pEvent->strAnnotation + strIndexNum);
		pElement->SetNotes(pEvent->strNotes);
		pElement->SetEventTimestamp(pEvent->timestamp);
		pElement->SetBoundingRect(&pEvent->boundingRect);
	}
	else if ((pEventRecord->nEventType & 0x01) == 1) // Task
	{
		pElement = DBG_NEW BPMNTask(pEvent->eventIndex, pEvent->timestamp);

		//pElement->SetEventAdditionalInfo(pEvent->nVK_Code);
		pElement->SetAnnotation(pEvent->strAnnotation + strIndexNum);
		pElement->SetNotes(pEvent->strNotes);
		pElement->SetEventTimestamp(pEvent->timestamp);
		pElement->SetBoundingRect(&pEvent->boundingRect);
	}

	struct eventListElement *pEventListElement = DBG_NEW struct eventListElement;
	pEventListElement->pElement = pElement;
	CopyMemory(&pEventListElement->boundingRect, &pEvent->boundingRect, sizeof(RECT));
	pEventListElement->fPosCurr = pos;
	pEventListElement->nextElement = pEventListElement->prevElement = NULL;
	eventMap.insert(pair<ULONG, struct eventListElement *>(pEvent->eventIndex, pEventListElement));

	if ((pEventRecord->nEventType & 0x10) == 0x10 ||	// Branch Right Child
		(pEventRecord->nEventType & 0x20) == 0x20 ||   // Branch Left Child
		(pEventRecord->nEventType & 0x40) == 0x40)		// Process Child
	{
		// Search for Branch ID
		map <ULONG, struct eventListElement *>::iterator itEventFind = eventMap.find(pEventRecord->nParentEventCode);
		if (itEventFind != eventMap.end())
		{
			struct eventListElement *pParentEvent = itEventFind->second;

			if ((pEventRecord->nEventType & 0x10) == 0x10)
			{
				BPMNDecision *pDecision = (BPMNDecision *)pParentEvent->pElement;
				if (pDecision != NULL)
				{
					pPrevElement = pDecision->GetLastRightElement();
					pDecision->AddRightElement(pElement);
				}
			}
			else if ((pEventRecord->nEventType & 0x20) == 0x20)
			{
				BPMNDecision *pDecision = (BPMNDecision *)pParentEvent->pElement;
				if (pDecision != NULL)
				{
					pPrevElement = pDecision->GetLastBottomElement();
					pDecision->AddBottomElement(pElement);
				}
			}
			else if ((pEventRecord->nEventType & 0x40) == 0x40)
			{
				BPMNSubProcess *pSubProcess = (BPMNSubProcess *)pParentEvent->pElement;
				if (pSubProcess != NULL)
				{
				pPrevElement = pSubProcess->GetLastElement();
				pSubProcess->AddElement(pElement);
				}
			}
		}
	}
	else
	{
		BPMNElement *pStop = NULL;
		if (bpmnDiagram.size() > 1)
		{
			pStop = bpmnDiagram.back();
			bpmnDiagram.pop_back();
			pPrevElement = bpmnDiagram.back();
		}
		bpmnDiagram.push_back(pElement);
		if (pStop == NULL)
		{
			pStop = DBG_NEW BPMNStop();
		}
		bpmnDiagram.push_back(pStop);
	}

	if (pPrevElement != NULL)
	{
		map <ULONG, struct eventListElement *>::iterator itEventFind = eventMap.find(pPrevElement->GetEventIndex());
		if (itEventFind != eventMap.end())
		{
			prevEventListElement = itEventFind->second;
			prevEventListElement->nextElement = pEventListElement;
			pEventListElement->prevElement = prevEventListElement;
		}

	}

	return pEventListElement;

}

struct eventListElement *BPMNFacade::ProcessEventList()
{
	list<struct eventDetails *>::iterator it;
	struct eventListElement *pEventRoot = NULL;
	it = eventList.begin();

	BPMNElement *pElement = DBG_NEW BPMNStart();
	bpmnDiagram.push_back(pElement);
	struct eventListElement *prevEventListElement = NULL;
	while (it != eventList.end())
	{
		struct eventDetails *pEventDetails = *it;
		// Check if event index is the same, then dont add it,
		// it should never happen when click, and double clicks are handle in the hooks
		map <ULONG, struct eventListElement *>::iterator itEventListMap = eventMap.find(pEventDetails->eventIndex);
		if (itEventListMap != eventMap.end())
		{
			it++;
			continue;
		}
	
		pElement = DBG_NEW BPMNTask(pEventDetails->eventIndex, pEventDetails->timestamp);

		struct eventListElement *pEventListElement = DBG_NEW struct eventListElement;
		pEventListElement->pElement = pElement;
		if (prevEventListElement != NULL)
			prevEventListElement->nextElement = pEventListElement;
		prevEventListElement = pEventListElement;
		pEventListElement->prevElement = prevEventListElement;
		pEventListElement->nextElement = NULL;
		eventMap.insert(pair<ULONG, struct eventListElement *>(pEventDetails->eventIndex, pEventListElement));
		if (pEventRoot == NULL)
			pEventRoot = pEventListElement;
		//string strName = pEventDetails->focusProcessName;
		//// Split the string to find the last token that will contain the process name8
		//size_t pos = 0;
		//string token;
		//string delimiter = "\\";
		//while ((pos = strName.find(delimiter)) != std::string::npos) {
		//	token = strName.substr(0, pos);
		//	strName.erase(0, pos + delimiter.length());
		//}
		//if (token == "Error OpenProcess")
		//{
		//	it++;
		//	continue;
		//}
		//// The process name is in token
		//pElement->SetProcessName(token);
		//pElement->SetWindowName(pEventDetails->focusWindowName);
		//pElement->SetControlName(pEventDetails->focusControlName);
		////pElement->SetControlType(pEventDetails->focusAutomationType);
		//pElement->SetAutomationID(pEventDetails->pointAutomationID);
		//pElement->SetControlTypeVal(pEventDetails->focusControlTypeID);
		pElement->SetEventType(pEventDetails->nEventCode);
		//pElement->SetEventAdditionalInfo(pEventDetails->nVK_Code);
		string strAnnotation = "";
		if (pEventDetails->nEventCode < 10)
		{
			strAnnotation = "In " + pElement->GetProcessName() + " " + pElement->GetWindowName() + "window, Mouse Clicked on " + pElement->GetControlName() + ".";
			pElement->SetAnnotation(strAnnotation);
		}
		bpmnDiagram.push_back(pElement);
		it++;
	}
	pElement = DBG_NEW BPMNStop();
	bpmnDiagram.push_back(pElement);
	return pEventRoot;
}

void BPMNFacade::CreateWidgetTree(char *jSONRec)
{
	string err;
	Json jsonElements = Json::parse(jSONRec, err);
	spdlog::info("Widget Tree is {}", jSONRec);
	widgetMap.clear();
	
	if (jsonElements.is_array())
	{
		vector <Json> widgets = jsonElements.array_items();
		for (size_t i = 0; i < widgets.size(); i++)
		{
			struct widgetDetails *pWidgetDetails = DBG_NEW struct widgetDetails;
			
			Json widget = widgets[i];
			pWidgetDetails->elementID = widget["ElementID"].int_value();
			pWidgetDetails->processName = widget["ProcessName"].string_value();
			Json rectWidget = widget["BoundingRect"];
			pWidgetDetails->boundingRect.top = rectWidget["Top"].int_value();
			pWidgetDetails->boundingRect.left = rectWidget["Left"].int_value();
			pWidgetDetails->boundingRect.bottom = rectWidget["Bottom"].int_value();
			pWidgetDetails->boundingRect.right = rectWidget["Right"].int_value();
			pWidgetDetails->automationID = widget["AutomationID"].string_value();
			pWidgetDetails->className = widget["ClassName"].string_value();
			pWidgetDetails->controlType = widget["ControlType"].int_value();
			pWidgetDetails->itemType = widget["ItemType"].string_value();
			pWidgetDetails->parentID = pWidgetDetails->childID = pWidgetDetails->leftID = pWidgetDetails->rightID = 0L;
			pWidgetDetails->parentID = widget["Parent"].int_value();
			pWidgetDetails->childID = widget["Child"].int_value();
			pWidgetDetails->leftID = widget["Left"].int_value();
			pWidgetDetails->rightID = widget["Right"].int_value();

			vector<Json> names = widget["Names"].array_items();
			for (size_t iName = 0; iName < names.size(); iName++)
			{
				Json name = names[iName];
				int idName = name["ID"].int_value();
				string strName = name["Name"].string_value();
				pWidgetDetails->nameMap.insert(pair<int, string>(idName, strName));
			}

			ULONG keyVal = pWidgetDetails->elementID;
			widgetMap.insert(pair<ULONG, struct widgetDetails *>(keyVal, pWidgetDetails));
			spdlog::info("Inserting element into widget map");
		}
	}


}