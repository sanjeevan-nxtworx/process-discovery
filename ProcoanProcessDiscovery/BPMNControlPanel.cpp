#include "stdafx.h"
#include "BPMNControlPanel.h"
#include "BPMNDiagram.h"
#include "PlayerDataFileManager.h"
#include "BPMNSubProcess.h"
#include "BPMNDecision.h"
#include "BPMNTask.h"
#include <chrono>
#include <algorithm>
#include "CustomEvents.h"
#include "BPMNDrawPanel.h"
#include "MainToolBarPanel.h"


wxDEFINE_EVENT(PROCOAN_UPDATE_BPMNWINDOW, wxCommandEvent);

BEGIN_EVENT_TABLE(BPMNControlPanel, wxPanel)
EVT_COMMAND(BPMN_CONTROL_VERT_SCROLL, PROCOAN_VSCROLL_MOVEABSOLUTE, BPMNControlPanel::OnVerticalScroll)
EVT_COMMAND(BPMN_CONTROL_HORIZ_SCROLL, PROCOAN_HSCROLL_MOVEABSOLUTE, BPMNControlPanel::OnHorizontalScroll)
END_EVENT_TABLE()


BPMNControlPanel::BPMNControlPanel(wxPanel *parent,ULONG id, wxPoint pt, wxSize size) :
	wxPanel(parent, id, pt, size, wxNO_BORDER)
{
	pParentWnd = parent;
	pBPMNDiagram = NULL;
	drawingHeight = 0;
	drawingWidth = 0;
	pDiagramMap = DBG_NEW BPMNDiagram();
	currentEventNumber = -1;

	bpmnDrawPanel = DBG_NEW BPMNDrawPanel(this, (ULONG)wxID_ANY, wxPoint(5, 5), wxSize(size.GetWidth() - 15, size.GetHeight() - 20));
	wxSize bpmnDrawSize = bpmnDrawPanel->GetSize();
	pHorizScroll = DBG_NEW HorizScrollBarPanel(this, BPMN_CONTROL_HORIZ_SCROLL, wxPoint(5, 5 + bpmnDrawSize.GetHeight()), wxSize(bpmnDrawSize.GetWidth(), 10));
	pHorizScroll->Hide();
	pVertScroll = DBG_NEW VertScrollBarPanel(this, BPMN_CONTROL_VERT_SCROLL, wxPoint(5 + bpmnDrawSize.GetWidth(), 5), wxSize(10, bpmnDrawSize.GetHeight()));
	pVertScroll->Hide();
	bpmnDrawPanel->SetDiagramMap(pDiagramMap);
	bpmnDrawPanel->SetControlPanel(this);
	bpmnDrawPanel->SetBackgroundColour(wxColor(0x00, 0x00, 0x0));
}

BPMNControlPanel::~BPMNControlPanel()
{
	if (pDiagramMap != NULL)
		delete pDiagramMap;
	pDiagramMap = NULL;
}

void BPMNControlPanel::RefreshDiagram()
{
	ResetDiagram();
	//bpmnDrawPanel->paintNow();
}

bool BPMNControlPanel::IsSyncElementDisplayed()
{
	return pDiagramMap->IsSyncElementDisplayed();
}

void BPMNControlPanel::PositionControls()
{
	wxSize size = GetSize();

	bpmnDrawPanel->Hide();
	pHorizScroll->Hide();
	pVertScroll->Hide();
	//switchButton->Hide();

	bpmnDrawPanel->SetPosition(wxPoint(5, 5));
	bpmnDrawPanel->SetSize(wxSize(size.GetWidth() - 15, size.GetHeight() - 20));
	UpdateBPMNControlWindow();
	//switchButton->Show();
}

void BPMNControlPanel::OnVerticalScroll(wxCommandEvent& WXUNUSED(event))
{
	bpmnDrawPanel->paintNow();
}

void BPMNControlPanel::OnHorizontalScroll(wxCommandEvent& WXUNUSED(event))
{
	bpmnDrawPanel->paintNow();
}

void BPMNControlPanel::SetMainToolbarPanel(MainToolBarPanel *pPanel)
{
	pToolBarPanel = pPanel;
}

void BPMNControlPanel::UpdateBPMNWindow()
{
	SetDrawingScrollBars();
	ShowElement();
	pDiagramMap->SetSyncElement();
	bpmnDrawPanel->Show();
	bpmnDrawPanel->paintNow();
}

void BPMNControlPanel::EnableCreateProcessButton(bool bFlg)
{
	pToolBarPanel->EnableCreateSubProcess(bFlg);
}

void BPMNControlPanel::EnableMoveToSubProcessAboveButton(bool bFlg)
{
	pToolBarPanel->EnableMoveSubProcessAbove(bFlg);
}

void BPMNControlPanel::EnableMoveToSubProcessBelowButton(bool bFlg)
{
	pToolBarPanel->EnableMoveSubProcessBelow(bFlg);
}

void BPMNControlPanel::EnableMoveToProcessAboveButton(bool bFlg)
{
	pToolBarPanel->EnableMoveProcessBelow(bFlg);
}

void BPMNControlPanel::EnableGoUpLevelButton(bool bFlg)
{
	pToolBarPanel->EnableUpLevel(bFlg);
}

void BPMNControlPanel::EnableDeleteBranchButton(bool bFlg)
{
	pToolBarPanel->EnableDeleteBranch(bFlg);
}

void BPMNControlPanel::EnableDeleteEventButton(bool bFlg)
{
	pToolBarPanel->EnableDeleteEvent(bFlg);
}

void BPMNControlPanel::EnableSyncVideoButton(bool bFlg)
{
	pToolBarPanel->EnableSyncVideo(bFlg);
}

void BPMNControlPanel::EnableBranchBottomButton(bool bFlg)
{
	pToolBarPanel->EnableBranchBottom(bFlg);
}

void BPMNControlPanel::EnableBranchRightButton(bool bFlg)
{
	pToolBarPanel->EnableBranchRight(bFlg);
}


bool BPMNControlPanel::ShowEventInWindow(ULONG nEventNum)
{
	// find if event is in current pBPMNDiagram if not then create new diagram map and Show the element
	map <ULONG, struct eventListElement *>::iterator itMapItem = pEventMap->find(nEventNum);
	if (itMapItem == pEventMap->end())
	{
		CreateNewDiagram(pMainList);
		ShowElement(0L);
		return false;
	}

	struct eventListElement *pEventListElement = itMapItem->second;
	//pEventListElement = pEventListElement->nextElement;
	//nEventNum = pEventListElement->pElement->GetEventIndex();
	BPMNElement *pElement = pEventListElement->pElement;
	list <BPMNElement *> *pElementList = GetElementList(pElement);;

	if (pElementList != pBPMNDiagram)
	{
		CreateNewDiagram(pElementList);
		ShowElement(nEventNum);
	}
	else
	{
		ShowElement(nEventNum);
		UpdateBPMNControlWindow();
	}

	pPlayerManager->SetTabFields(pEventListElement);
	return true;
}


void BPMNControlPanel::InsertItem(ULONG nEventID, long long timestamp)
{
	BPMNElement *pElement = pDiagramMap->GetCurrentSyncronizedElement();
	BPMNTask *pTaskElement = NULL;

	if (pElement != NULL && pBPMNDiagram->size() > 0)
	{
		pTaskElement = (BPMNTask *)InsertElementInList(nEventID, timestamp, pBPMNDiagram, pElement, 2, false);
	}
	else if (pBPMNDiagram->size() > 2)
	{
		list <BPMNElement *>::iterator itElement = pBPMNDiagram->begin();
		itElement++;
		if (itElement != pBPMNDiagram->end())
		{
			pElement = *itElement;
			pTaskElement = (BPMNTask *)InsertElementInList(nEventID, timestamp, pBPMNDiagram, pElement, 2, true);
		}
	}
	else
	{
		return;
	}

	CreateNewDiagram(pBPMNDiagram);
	ShowElement(nEventID);
}


void BPMNControlPanel::SetAnnotationDetails(string strAnnotation, string strNotes)
{
	if (pDiagramMap->SetAnnotationDetails(strAnnotation, strNotes))
		ResetDiagram();
}

void BPMNControlPanel::SetSubProcessDetails(string strName, string strDescription)
{
	if (pDiagramMap->SetSubProcessDetails(strName, strDescription))
		ResetDiagram();
}

void BPMNControlPanel::SetController(PlayerDataFileManager *pManager)
{
	pPlayerManager = pManager;
	pDiagramMap->SetController(pManager);
	bpmnDrawPanel->SetController(pManager);
}


void BPMNControlPanel::SetMainList(list <BPMNElement *> *pElementList)
{
	pMainList = pElementList;
}


void BPMNControlPanel::CreateAndDisplayDiagram(list <BPMNElement *> *pDiagram)
{
	CreateNewDiagram(pDiagram);
}

void BPMNControlPanel::SetEventMap(map <ULONG, struct eventListElement *> *pMap)
{
	pEventMap = pMap;
}


void BPMNControlPanel::GetPointOffset(POINT *ptOffset)
{
	wxSize size = bpmnDrawPanel->GetSize();

	HDC hdc = GetDC(NULL);
	SetMapMode(hdc, MM_HIMETRIC);
	POINT pVal;
	pVal.x = size.GetWidth();
	pVal.y = size.GetHeight();

	DPtoLP(hdc, &pVal, 1);
	size.SetWidth(pVal.x);
	size.SetHeight(-pVal.y);

	ptOffset->x = ptOffset->y = 0;
	int drawingW = GetDrawingWidth();
	if (drawingW > size.GetWidth())
		ptOffset->x = 200;
	else
		ptOffset->x = (size.GetWidth() - drawingW + COLUMN_MARGIN) / 2;
	pVal.x = GetHorizontalOffset();
	pVal.y = GetVerticalOffset();
	DPtoLP(hdc, &pVal, 1);
	DeleteDC(hdc);

	ptOffset->x = pVal.x - ptOffset->x;
	ptOffset->y = -pVal.y;
}

BPMNDiagram *BPMNControlPanel::GetDiagram()
{
	return pDiagramMap;
}

void BPMNControlPanel::GetPointScrollOffset(POINT *ptOffset)
{
	HDC hdc = GetDC(NULL);
	SetMapMode(hdc, MM_HIMETRIC);
	ptOffset->x = ptOffset->y = 0;
	int drawingW = GetDrawingWidth();
	wxSize size = GetSize();

	POINT pVal;
	pVal.x = size.GetWidth();
	pVal.y = size.GetHeight();

	DPtoLP(hdc, &pVal, 1);
	size.SetWidth(pVal.x);
	size.SetHeight(-pVal.y);

	if (drawingW > size.GetWidth())
		ptOffset->x = 200;
	else
		ptOffset->x = (size.GetWidth() - drawingW) / 2 ;

	pVal.x = GetHorizontalOffset();
	pVal.y = GetVerticalOffset();

	DPtoLP(hdc, &pVal, 1);

	ptOffset->x =  -(pVal.x - ptOffset->x);
	ptOffset->y = pVal.y;
	DeleteDC(hdc);
}


// Private Methods

list <BPMNElement *> *BPMNControlPanel::GetElementList(BPMNElement *pElement)
{
	BPMNElement *pParent = pElement->GetParentElement();
	if (pParent == NULL)
		return pMainList;
	if (pParent->GetType() == 3) // SubProcess
	{
		BPMNSubProcess *pSubProcess = (BPMNSubProcess *)pParent;
		return pSubProcess->GetSubProcessElements();
	}
	if (pParent->GetType() == 4)
	{
		// For the branch, search for the branch parent = main diagram or part of sub-process
		while (pParent != NULL && pParent->GetType() == 4)
			pParent = pParent->GetParentElement();
		if (pParent == NULL)
			return	pMainList;					//pDecision->GetDecisionElementList(pElement);
		if (pParent->GetType() == 3)
		{
			BPMNSubProcess *pSubProcess = (BPMNSubProcess *)pParent;
			return pSubProcess->GetSubProcessElements();
		}
	}
	return NULL;
}


void BPMNControlPanel::CreateNewDiagram(list <BPMNElement *> *pDiagram)
{
	pBPMNDiagram = pDiagram;
	if (pBPMNDiagram == NULL)
		return;
	bpmnDrawPanel->SetDiagram(pBPMNDiagram);

	// Get dimensions of all controls
	GetControlDimensions();
	pDiagramMap->Clear();
	pDiagramMap->FindDiagramDimensions(pBPMNDiagram);
	pDiagramMap->SetDrawPosition();
	drawingHeight = pDiagramMap->GetDrawingHeight();
	drawingWidth = pDiagramMap->GetDrawingWidth();
	UpdateBPMNControlWindow();
}

void BPMNControlPanel::UpdateBPMNControlWindow()
{
	wxCommandEvent customEvent(PROCOAN_UPDATE_BPMNWINDOW, GetId());
	customEvent.SetEventObject(this);
	// Do send it
	//ProcessWindowEvent(customEvent);
	QueueEvent(customEvent.Clone());
}

void BPMNControlPanel::SetCopySegmentFlag(bool bFlg)
{
	bpmnDrawPanel->SetCopySegmentFlag(bFlg);
}

void BPMNControlPanel::ShowElement(ULONG nEventNum)
{
	currentEventNumber = nEventNum;
}

void BPMNControlPanel::SetCurrentSyncronizedElement(BPMNElement *pElement, bool bSegOn)
{
	pDiagramMap->SetCurrentSyncronizedElement(pElement, bSegOn);
}

void BPMNControlPanel::SetCurrentSyncronizedElement(ULONG nEventID, bool bSegOn)
{
	map <ULONG, struct eventListElement *>::iterator itMapItem = pEventMap->find(nEventID);
	if (itMapItem == pEventMap->end())
	{
		return;
	}

	struct eventListElement *pEventListElement = itMapItem->second;
	BPMNElement *pElement = pEventListElement->pElement;
	pDiagramMap->SetCurrentSyncronizedElement(pElement, bSegOn);
}

void BPMNControlPanel::ClearSegmentElements()
{
	map <ULONG, struct eventListElement *>::iterator itMapItem = pEventMap->begin();
	while (itMapItem != pEventMap->end())
	{
		struct eventListElement *pEventListElement = itMapItem->second;
		BPMNElement *pElement = pEventListElement->pElement;
		pElement->SetSegElement(false);
		itMapItem++;
	}
	UpdateBPMNControlWindow();
}

void BPMNControlPanel::ShowElement()
{
	if (!pPlayerManager->IsSynchronizeBPMNDisplay())
		return;
	ULONG nEventNum = currentEventNumber;
	struct cellDef *pCell = NULL;

	pCell = pDiagramMap->FindEventInDiagram(nEventNum);
	BPMNElement *pElement = NULL;
	if (pCell == NULL || pCell->bpmnElement == NULL)
		return;
	pElement = pCell->bpmnElement;
	RECT rect, clientRect;

	POINT pointOffset;
	GetPointScrollOffset(&pointOffset);
	wxSize size = bpmnDrawPanel->GetSize();
	HDC hDC = GetDC(NULL);
	SetMapMode(hDC, MM_HIMETRIC);
	pElement->GetDisplayPosition(&rect);
	POINT ptVal;
	ptVal.x = size.GetWidth();
	ptVal.y = size.GetHeight();
	DPtoLP(hDC, &ptVal, 1);
	size.SetWidth(ptVal.x);
	size.SetHeight(-ptVal.y);

	clientRect.top = clientRect.left = 0;
	clientRect.right = size.GetWidth();
	clientRect.bottom = size.GetHeight();

	OffsetRect(&rect, pointOffset.x, pointOffset.y);
	if (pDiagramMap->IsElementInWindow(&clientRect, &rect))
		; // the element is on the screen
	else
	{
		pElement->GetDisplayPosition(&rect);
		int xNewValue = (rect.left + (rect.right - rect.left) / 2) - (clientRect.left + (clientRect.right - clientRect.left) / 2);
		int yNewValue = (rect.top + (rect.bottom - rect.top) / 2) - (clientRect.top + (clientRect.bottom - clientRect.top) / 2);

		//int xValue = pHorizScroll->GetCurrentValue();
		//int yValue = pVertScroll->GetCurrentValue(); 
		//ptVal.x = xValue;
		//ptVal.y = yValue;
		//DPtoLP(hDC, &ptVal,1);

		if (xNewValue < 0)
			xNewValue = 0;
		if (yNewValue < 0)
			yNewValue = 0;

		//xValue = ptVal.x + xNewValue;
		//yValue = -ptVal.y + yNewValue;
		// move the cursor to the position required

		ptVal.x = xNewValue;
		ptVal.y = -yNewValue;
		LPtoDP(hDC, &ptVal, 1);
		pHorizScroll->SetCurrentValue(ptVal.x);
		pVertScroll->SetCurrentValue(ptVal.y);
	}
	//pDiagramMap->SetCurrentSyncronizedElement(pElement);
	DeleteDC(hDC);
}


BPMNElement *BPMNControlPanel::InsertElementInList(ULONG nEventID, long long timestamp, list <BPMNElement *> *pElementList, BPMNElement *pElement, int type, bool insAbove)
{
	// Inserts an element below the element pointed to by pElement
	list <BPMNElement *> *pList = pElementList;

	BPMNElement *pParent = pElement->GetParentElement();
	list <BPMNElement *>::iterator itFind = pElementList->begin();

	if (pParent != NULL)
	{
		if (pParent->GetType() == 3) // Sub Process
		{
			BPMNSubProcess *pSubProcess = (BPMNSubProcess *)pParent;
			pList = pSubProcess->GetSubProcessElements();
		}
		else if (pParent->GetType() == 4)
		{
			BPMNDecision *pDecision = (BPMNDecision *)pParent;
			pList = pDecision->GetDecisionElementList(pElement);
		}
	}

	// find element in the list
	itFind = find(pList->begin(), pList->end(), pElement);
	list <BPMNElement *>::iterator itNext = itFind;
	itNext++;

	BPMNElement *pNewElement = NULL;


	if (type == 2) // Add a BPMN Task
	{
		BPMNTask *pTaskElement = NULL;

		pTaskElement = DBG_NEW BPMNTask(nEventID, -1L);
		pTaskElement->SetAnnotation("New Activity");
		string strVal = "{\"Annotation\": \"New Activity\"}";
		pTaskElement->SetNotes(strVal);
		pTaskElement->SetParentElement(pParent);
		pNewElement = (BPMNElement *)pTaskElement;
	}
	else if (type == 3)
	{
		BPMNSubProcess *pSubProcess = NULL;

		pSubProcess = DBG_NEW BPMNSubProcess(nEventID);
		pSubProcess->SetAnnotation("New SubProcess");
		string strVal = "{\"Annotation\": \"New SubProcess\"}";
		pSubProcess->SetNotes(strVal);
		pSubProcess->SetParentElement(pParent);
		pNewElement = (BPMNElement *)pSubProcess;
	}

	pNewElement->SetEventTimestamp(timestamp);
	if (!insAbove)
	{
		// here insert a new BPMNTask element after the found element
		// create a new BPMNTask object and insert into the list
		if (itNext == pElementList->end())
		{
			// add to end of list
			BPMNElement *pStopElement = pElementList->back();
			if (pStopElement->GetType() == 5) // stop element
			{
				pElementList->pop_back();
				pElementList->push_back(pNewElement);
				pElementList->push_back(pStopElement);
			}
			else
				pElementList->push_back(pNewElement);
		}
		else
		{
			pElementList->insert(itNext, pNewElement);
		}
	}
	else
		pElementList->insert(itFind, pNewElement);

	map <ULONG, struct eventListElement *>::iterator itMapElement = pEventMap->find(pElement->GetEventIndex());
	if (itMapElement != pEventMap->end())
	{
		// Insert above the first element in the list
		struct eventListElement *pMapElement = itMapElement->second;
		struct eventListElement *pNewMapElement = DBG_NEW struct eventListElement;
		struct eventListElement *pPrevMapElement = pMapElement->prevElement;
		struct eventListElement *pNextMapElement = NULL;

		pNewMapElement->pElement = pNewElement;
		if (!insAbove)
		{
			pNewMapElement->prevElement = pMapElement;
			pNewMapElement->nextElement = pMapElement->nextElement;
			pMapElement->nextElement = pNewMapElement;
		}
		else
		{
			pNewMapElement->prevElement = pPrevMapElement;
			pNewMapElement->nextElement = pMapElement;
			pMapElement->prevElement = pNewMapElement;
		}
		pEventMap->insert(pair<ULONG, struct eventListElement *>(nEventID, pNewMapElement));

		long long duration = 0L;
		if (pNewMapElement->prevElement != NULL)
		{
			long long prevEventTimestamp = pNewMapElement->prevElement->pElement->GetEventTimestamp();
			if (prevEventTimestamp != 0LL)
			{
				duration = timestamp - prevEventTimestamp;
				pNewElement->SetDuration(duration);
			}
		}
		if (pNewMapElement->nextElement != NULL)
		{
			long long nextEventTimestamp = pNewMapElement->nextElement->pElement->GetEventTimestamp();
			if (nextEventTimestamp != 0LL)
			{
				duration = nextEventTimestamp - timestamp;
				pNewMapElement->nextElement->pElement->SetDuration(duration);
			}
		}

		AddNewEventRecord(pNewMapElement);

		BPMNElement *pRootParent = pMapElement->pElement->GetParentElement();
		if (pPrevMapElement == NULL && pRootParent == NULL)
		{
			//		Write channel header or previous element record (only pointers)
			pPlayerManager->SetEventRoot(pNewMapElement);
		}
		else
		{
			if (!insAbove)
			{
				pNextMapElement = pNewMapElement->nextElement;
				if (pNextMapElement != NULL)
				{
					pNextMapElement->prevElement = pNewMapElement;
					pPlayerManager->UpdateEventRecord(pNewMapElement);
				}
			}
			else
			{
				pPrevMapElement = pNewMapElement->prevElement;
				if (pPrevMapElement != NULL)
				{
					pPrevMapElement->nextElement = pNewMapElement;
					pPlayerManager->UpdateEventRecord(pPrevMapElement);
				}
			}
		}

		pPlayerManager->UpdateEventRecord(pMapElement);

	}

	return (BPMNElement *)pNewElement;
}

BPMNElement *BPMNControlPanel::InsertElementInList(ULONG nEventID, list <BPMNElement *> *pElementList, BPMNElement *pElement, int type, bool insAbove)
{
	// Inserts an element below the element pointed to by pElement
	list <BPMNElement *> *pList = pElementList;


	BPMNElement *pParent = pElement->GetParentElement();
	list <BPMNElement *>::iterator itFind = pElementList->begin();

	if (pParent != NULL)
	{
		if (pParent->GetType() == 3) // Sub Process
		{
			BPMNSubProcess *pSubProcess = (BPMNSubProcess *)pParent;
			pList = pSubProcess->GetSubProcessElements();
		}
		else if (pParent->GetType() == 4)
		{
			BPMNDecision *pDecision = (BPMNDecision *)pParent;
			pList = pDecision->GetDecisionElementList(pElement);
		}
	}

	// find element in the list
	itFind = find(pList->begin(), pList->end(), pElement);
	list <BPMNElement *>::iterator itNext = itFind;
	itNext++;

	BPMNElement *pNewElement = NULL;


	if (type == 2) // Add a BPMN Task
	{
		BPMNTask *pTaskElement = NULL;

		pTaskElement = DBG_NEW BPMNTask(nEventID, -1L);
		pTaskElement->SetAnnotation("New Activity");
		string strVal = "{\"Annotation\": \"New Activity\"}";
		pTaskElement->SetNotes(strVal);
		pTaskElement->SetParentElement(pParent);
		pNewElement = (BPMNElement *)pTaskElement;
	}
	else if (type == 3)
	{
		BPMNSubProcess *pSubProcess = NULL;

		pSubProcess = DBG_NEW BPMNSubProcess(nEventID);
		pSubProcess->SetAnnotation("New SubProcess");
		string strVal = "{\"Annotation\": \"New SubProcess\"}";
		pSubProcess->SetNotes(strVal);
		pSubProcess->SetParentElement(pParent);
		pNewElement = (BPMNElement *)pSubProcess;
	}

	if (!insAbove)
	{
		// here insert a new BPMNTask element after the found element
		// create a new BPMNTask object and insert into the list
		if (itNext == pElementList->end())
		{
			// add to end of list
			BPMNElement *pStopElement = pElementList->back();
			if (pStopElement->GetType() == 5) // stop element
			{
				pElementList->pop_back();
				pElementList->push_back(pNewElement);
				pElementList->push_back(pStopElement);
			}
			else
				pElementList->push_back(pNewElement);
		}
		else
		{
			pElementList->insert(itNext, pNewElement);
		}
	}
	else
		pList->insert(itFind, pNewElement);

	map <ULONG, struct eventListElement *>::iterator itMapElement = pEventMap->find(pElement->GetEventIndex());
	if (itMapElement != pEventMap->end())
	{
		// Insert above the first element in the list
		struct eventListElement *pMapElement = itMapElement->second;
		struct eventListElement *pNewMapElement = DBG_NEW struct eventListElement;
		struct eventListElement *pPrevMapElement = pMapElement->prevElement;
		struct eventListElement *pNextMapElement = NULL;

		pNewMapElement->pElement = pNewElement;
		if (!insAbove)
		{
			pNewMapElement->prevElement = pMapElement;
			pNewMapElement->nextElement = pMapElement->nextElement;
			pMapElement->nextElement = pNewMapElement;
		}
		else
		{
			pNewMapElement->prevElement = pPrevMapElement;
			pNewMapElement->nextElement = pMapElement;
			pMapElement->prevElement = pNewMapElement;
		}
		pEventMap->insert(pair<ULONG, struct eventListElement *>(nEventID, pNewMapElement));

		AddNewEventRecord(pNewMapElement);

		BPMNElement *pRootParent = pMapElement->pElement->GetParentElement();
		if (pPrevMapElement == NULL && pRootParent == NULL)
		{
			//		Write channel header or previous element record (only pointers)
			pPlayerManager->SetEventRoot(pNewMapElement);
		}
		else
		{
			if (!insAbove)
			{
				pNextMapElement = pNewMapElement->nextElement;
				if (pNextMapElement != NULL)
				{
					pNextMapElement->prevElement = pNewMapElement;
					pPlayerManager->UpdateEventRecord(pNewMapElement);
				}
			}
			else
			{
				pPrevMapElement = pNewMapElement->prevElement;
				if (pPrevMapElement != NULL)
				{
					pPrevMapElement->nextElement = pNewMapElement;
					pPlayerManager->UpdateEventRecord(pPrevMapElement);
				}
			}
		}

		pPlayerManager->UpdateEventRecord(pMapElement);

	}

	return (BPMNElement *)pNewElement;
}

void BPMNControlPanel::ResetDiagram()
{
	CreateAndDisplayDiagram(pBPMNDiagram);
}

void BPMNControlPanel::GetControlDimensions()
{
	bpmnDrawPanel->GetControlDimensions();
}

void BPMNControlPanel::SetDrawingScrollBars()
{
	wxSize size = this->GetSize();
	wxSize scrollSize;

	HDC hDC = GetDC(NULL);
	SetMapMode(hDC, MM_HIMETRIC);
	POINT ptVal;
	ptVal.x = size.GetWidth();
	ptVal.y = size.GetHeight();
	DPtoLP(hDC, &ptVal, 1);
	scrollSize.SetWidth(ptVal.x);
	scrollSize.SetHeight(-ptVal.y);

	wxSize newSize = bpmnDrawPanel->GetSize();
	wxSize newScrollSize;
	ptVal.x = newSize.GetWidth();
	ptVal.y = newSize.GetHeight();
	DPtoLP(hDC, &ptVal, 1);
	newScrollSize.SetWidth(ptVal.x);
	newScrollSize.SetHeight(-ptVal.y);

	//wxSize(size.GetWidth() - 10, size.GetHeight() - 35)
	POINT ptSize;
	ptSize.x = drawingWidth;
	ptSize.y = -drawingHeight;
	LPtoDP(hDC, &ptSize, 1);
	pVertScroll->SetMaxValue(ptSize.y);
	pHorizScroll->SetMaxValue(ptSize.x);
	
	if (drawingHeight > scrollSize.GetHeight() - 400)
	{
		newSize.SetWidth(size.GetWidth() - 20);
		if (drawingWidth > newScrollSize.GetWidth())
		{
			newSize.SetHeight(size.GetHeight() - 20);
			pHorizScroll->SetPosition(wxPoint(5, 5 + newSize.GetHeight()));
			pHorizScroll->SetSize(newSize.GetWidth(), 10);
			pHorizScroll->Show();
		}
		else
		{
			pHorizScroll->SetCurrentValue(0);
			pHorizScroll->Hide();
		}
		pVertScroll->SetPosition(wxPoint(newSize.GetWidth() + 5, 5));
		pVertScroll->SetSize(10,newSize.GetHeight());
		pVertScroll->Show();
	}
	else if (drawingWidth > scrollSize.GetWidth() - 200)
	{
		newSize.SetHeight(size.GetHeight() - 20);
		pHorizScroll->SetPosition(wxPoint(5, 5 + newSize.GetHeight()));
		pHorizScroll->SetSize(newSize.GetWidth(), 10);
		pHorizScroll->Show();
		pVertScroll->SetCurrentValue(0);
		pVertScroll->Hide();
	}
	else
	{
		pVertScroll->SetCurrentValue(0);
		pHorizScroll->SetCurrentValue(0);
		pVertScroll->Hide();
		pHorizScroll->Hide();
	}
	bpmnDrawPanel->SetSize(newSize);

	ptVal.x = newScrollSize.GetWidth();
	ptVal.y = -newScrollSize.GetHeight();

	LPtoDP(hDC, &ptVal, 1);

	pHorizScroll->SetThumbSize(ptVal.x);
	pVertScroll->SetThumbSize(ptVal.y);
	DeleteDC(hDC);
}


void BPMNControlPanel::AddNewEventRecord(struct eventListElement *pEventListElement)
{
	pPlayerManager->AddNewEventRecord(pEventListElement);
}

int BPMNControlPanel::GetDrawingHeight()
{
	return drawingHeight;
}

int BPMNControlPanel::GetDrawingWidth()
{
	return drawingWidth;
}

int BPMNControlPanel::GetHorizontalOffset()
{
	return pHorizScroll->GetCurrentValue();
}

int BPMNControlPanel::GetVerticalOffset()
{
	return pVertScroll->GetCurrentValue();
}


void BPMNControlPanel::CreateSubProcess()
{
	if (pDiagramMap == NULL)
		return;

	if (pDiagramMap->CheckIfSelectedInSequence())
	{
		list <BPMNElement *> selectedList = pDiagramMap->GetSelectedItems();
		BPMNElement *pElement = selectedList.front();

		ULONG nIndex = pPlayerManager->GetChannelNextEventID(0);
		list <BPMNElement *> *pElementList = GetElementList(pElement);
		BPMNSubProcess *pSubProcess = (BPMNSubProcess *)InsertElementInList(nIndex, pElementList, pElement, 3, true);

		RemoveElementsFromList(pElementList, selectedList);
		pSubProcess->AddElementList(selectedList);

		//		Get the last element in the selected list, find the next element
		BPMNElement *pLastElement = selectedList.back();
		map <ULONG, struct eventListElement *>::iterator itEvent = pEventMap->find(pLastElement->GetEventIndex());
		if (itEvent == pEventMap->end())
			return;
		struct eventListElement *pEventListElement = itEvent->second;
		struct eventListElement *pEventListLastElement = pEventListElement;
		struct eventListElement *pNextEventListElement = pEventListElement->nextElement;

		//		get the first element of the selected list, find previous element
		itEvent = pEventMap->find(pElement->GetEventIndex());
		if (itEvent == pEventMap->end())
			return;
		pEventListElement = itEvent->second;
		struct eventListElement *pPrevEventListElement = pEventListElement->prevElement;
		// Update the SubProcess record to point to the next element after the last element in the sub-process
		pPrevEventListElement->nextElement = pNextEventListElement;
		pPlayerManager->UpdateEventRecord(pPrevEventListElement);
		// Update the next element after the last element in the subprocess to point to the subprocess
		if (pNextEventListElement != NULL)
		{
			pNextEventListElement->prevElement = pPrevEventListElement;
			pPlayerManager->UpdateEventRecord(pNextEventListElement);
		}

		//		the first element of the list and the last element of the list pointers are initialized, the rec type of each is changed
		//		rewrite all records (only pointers

		pEventListElement->prevElement = NULL;
		pEventListLastElement->nextElement = NULL;
		while (pEventListElement != NULL)
		{
			pPlayerManager->UpdateEventRecord(pEventListElement);
			pEventListElement = pEventListElement->nextElement;
		}

		// Update the list of elements within the sub-process
		itEvent = pEventMap->find(pSubProcess->GetEventIndex());
		if (itEvent == pEventMap->end())
			return;
		pEventListElement = itEvent->second;
		pPlayerManager->UpdateEventRecord(pEventListElement);

		pPlayerManager->UpdateChannelHeader(0);
		pPlayerManager->SetSubProcessTab();
		CreateNewDiagram(pElementList);
		ShowElement(nIndex);
	}
}

void BPMNControlPanel::DeleteSelectedBranch(BPMNElement *pBranch)
{
	BPMNDecision *pDecision = (BPMNDecision *)pBranch;

	list <BPMNElement *> *pBranchList = pDecision->GetBranchRightElements();
	if (pBranchList->size() > 0)
	{
		BPMNElement *pRightBranch = pBranchList->front();
		if (pRightBranch != NULL && pRightBranch->GetType() == 4)
		{
			DeleteSelectedBranch(pRightBranch);
		}

	}
	pPlayerManager->BranchDelete(pBranch->GetEventIndex(), true);
}


BPMNElement *BPMNControlPanel::RemoveElementsFromList(list <BPMNElement *> *pElementList, list <BPMNElement *> selectedList)
{
	list <BPMNElement *> *pList = pElementList;
	list <BPMNElement *>::iterator itElement = selectedList.begin();

	// Remove items in the selected list from the element list
	BPMNElement *pNextElement = NULL;
	while (itElement != selectedList.end())
	{
		BPMNElement *pElement = *itElement;
		BPMNElement *pParent = pElement->GetParentElement();
		list <BPMNElement *>::iterator itFind = pElementList->begin();

		pElement->SetParentElement(NULL);
		if (pParent != NULL)
		{
			if (pParent->GetType() == 3) // Sub Process
			{
				BPMNSubProcess *pSubProcess = (BPMNSubProcess *)pParent;
				pList = pSubProcess->GetSubProcessElements();
			}
			else if (pParent->GetType() == 4)
			{
				BPMNDecision *pDecision = (BPMNDecision *)pParent;
				pList = pDecision->GetDecisionElementList(pElement);
			}
		}

		// find element in the list
		itFind = find(pList->begin(), pList->end(), pElement);
		list <BPMNElement *>::iterator itPrev = itFind;
		list <BPMNElement *>::iterator itNext = itFind;
		itPrev--;
		itNext++;
		if (itFind != pList->end())
		{
			pList->erase(itFind);
		}

		if (itPrev != pList->begin())
		{
			pNextElement = *itPrev;
		}
		else
			if (itNext != pList->end())
			{
				pNextElement = *itNext;
			}
		itElement++;

	}
	return pNextElement;
}


BPMNElement *BPMNControlPanel::DeleteElementsFromList(list <BPMNElement *> *pElementList, list <BPMNElement *> selectedList)
{
	list <BPMNElement *> *pList = pElementList;
	list <BPMNElement *>::iterator itElement = selectedList.begin();

	// Remove items in the selected list from the element list
	BPMNElement *pNextElement = NULL;
	while (itElement != selectedList.end())
	{
		BPMNElement *pElement = *itElement;
		BPMNElement *pParent = pElement->GetParentElement();
		list <BPMNElement *>::iterator itFind = pElementList->begin();

		if (pElement->GetType() == 4) // If it is a branch then goto rightmost branch and start deleting branches right to left
		{
			DeleteSelectedBranch(pElement);
		}
		else
		{
			if (pElement->GetType() == 3)  // If subprocess then check elements at sub-process level for branch records, no need to go to next level
			{
				BPMNSubProcess *pSubProcess = (BPMNSubProcess *)pElement;
				list <BPMNElement *> *pSubProcessElements = pSubProcess->GetSubProcessElements();
				list <BPMNElement *> subProcessElementsCopy;
				subProcessElementsCopy.clear();
				subProcessElementsCopy.insert(subProcessElementsCopy.begin(), pSubProcessElements->begin(), pSubProcessElements->end());

				list <BPMNElement *>::iterator itSubProcessElements = subProcessElementsCopy.begin();
				
				while (itSubProcessElements != subProcessElementsCopy.end())
				{
					BPMNElement *pSubProcessElement = *itSubProcessElements;
					if (pSubProcessElement->GetType() == 4)  // If subprocess Element is a branch then remove top level branch
					{
						DeleteSelectedBranch(pSubProcessElement);
					}
					itSubProcessElements++;
					// find element in the list
					itFind = find(pSubProcessElements->begin(), pSubProcessElements->end(), pSubProcessElement);
					if (itFind != pSubProcessElements->end())
					{
						pSubProcessElements->erase(itFind);
					}
				}
			}
			pList = pElementList;
			if (pParent != NULL)
			{
				if (pParent->GetType() == 3) // Sub Process
				{
					BPMNSubProcess *pSubProcess = (BPMNSubProcess *)pParent;
					pList = pSubProcess->GetSubProcessElements();
				}
				else if (pParent->GetType() == 4)
				{
					BPMNDecision *pDecision = (BPMNDecision *)pParent;
					pList = pDecision->GetDecisionElementList(pElement);
				}
			}

			// find element in the list
			itFind = find(pList->begin(), pList->end(), pElement);
			list <BPMNElement *>::iterator itPrev = itFind;
			list <BPMNElement *>::iterator itNext = itFind;
			itPrev--;
			itNext++;
			if (itFind != pList->end())
			{
				pList->erase(itFind);
			}

			if (itPrev != pList->begin())
			{
				pNextElement = *itPrev;
			}
			else
				if (itNext != pList->end())
				{
					pNextElement = *itNext;
				}
		}
		itElement++;
	}
	return pNextElement;
}



void BPMNControlPanel::AddToSubProcessAbove()
{
	if (pDiagramMap == NULL)
		return;
	if (pDiagramMap->CheckIfSelectedInSequence())
	{
		list <BPMNElement *> selectedItems = pDiagramMap->GetSelectedItems();
		BPMNElement *pElementFirst = selectedItems.front();
		map <ULONG, struct eventListElement *>::iterator itEvent = pEventMap->find(pElementFirst->GetEventIndex());
		if (itEvent == pEventMap->end())
			return;
		BPMNElement *pSourceParent = pElementFirst->GetParentElement();
		BPMNElement *pDestParent = NULL;

		struct eventListElement *pEventListElement = itEvent->second;
		struct eventListElement *pEventListFirstSelectedElement = pEventListElement;
		struct eventListElement *pPrevEventListElement = pEventListElement->prevElement; // This should be the subprocess
		if (pPrevEventListElement == NULL || pPrevEventListElement->pElement->GetType() != 3)
			return;

		struct eventListElement *pSubProcessEventListElement = pPrevEventListElement;
		BPMNSubProcess *pSubProcess = (BPMNSubProcess *)pSubProcessEventListElement->pElement;
		pDestParent = (BPMNElement *)pSubProcess;
		list <BPMNElement *> *pElementList = GetElementList(pElementFirst);
		BPMNElement *pNewElement = RemoveElementsFromList(pElementList, selectedItems);

		BPMNElement *pProcessLastElement = pSubProcess->GetLastElement();
		pSubProcess->AddElementListBelow(selectedItems);

		//		Get the last element in the selected list, find the next element
		BPMNElement *pLastElement = selectedItems.back();
		itEvent = pEventMap->find(pLastElement->GetEventIndex());
		if (itEvent == pEventMap->end())
			return;
		pEventListElement = itEvent->second;
		struct eventListElement *pEventListLastElement = pEventListElement;
		struct eventListElement *pNextEventListElement = pEventListElement->nextElement;

		if (pPrevEventListElement != NULL)
		{
			pPrevEventListElement->nextElement = pNextEventListElement;
			pPlayerManager->UpdateEventRecord(pPrevEventListElement); // update the sub-process element
		}
		if (pNextEventListElement != NULL)
		{
			pNextEventListElement->prevElement = pPrevEventListElement;
			pPlayerManager->UpdateEventRecord(pNextEventListElement); // update the sub-process element
		}
		//		the first element of the list and the last element of the list pointers are initialized, the rec type of each is changed
		//		rewrite all records (only pointers

		itEvent = pEventMap->find(pProcessLastElement->GetEventIndex());
		if (itEvent == pEventMap->end())
			return;
		struct eventListElement *pLastProcessListElement = itEvent->second;

		pLastProcessListElement->nextElement = pEventListFirstSelectedElement;
		pPlayerManager->UpdateEventRecord(pLastProcessListElement);

		pEventListFirstSelectedElement->prevElement = pLastProcessListElement;
		pPlayerManager->UpdateEventRecord(pEventListFirstSelectedElement);

		itEvent = pEventMap->find(pLastElement->GetEventIndex());
		if (itEvent == pEventMap->end())
			return;
		pEventListLastElement = itEvent->second;
		pEventListLastElement->nextElement = NULL;

		pEventListElement = pEventListFirstSelectedElement;
		pEventListElement = pEventListElement->nextElement;
		while (pEventListElement != NULL)
		{
			pPlayerManager->UpdateEventRecord(pEventListElement);
			pEventListElement = pEventListElement->nextElement;
		}

		UpdateSourceAndDestinationParentRecords(pSourceParent, pDestParent);
		pPlayerManager->UpdateChannelHeader(0);

		CreateAndDisplayDiagram(pElementList);
		if (pNewElement != NULL)
		{
			ShowElement(pNewElement->GetEventIndex());
		}
	}
}

void BPMNControlPanel::AddToSubProcessBelow()
{
	if (pDiagramMap == NULL)
		return;
	if (pDiagramMap->CheckIfSelectedInSequence())
	{
		list <BPMNElement *> selectedItems = pDiagramMap->GetSelectedItems();
		BPMNElement *pElementLast = selectedItems.back();
		map <ULONG, struct eventListElement *>::iterator itEvent = pEventMap->find(pElementLast->GetEventIndex());
		if (itEvent == pEventMap->end())
			return;
		struct eventListElement *pEventListElement = itEvent->second;
		struct eventListElement *pNextEventListElement = pEventListElement->nextElement; // This should be the subprocess
		if (pNextEventListElement == NULL || pNextEventListElement->pElement->GetType() != 3)
			return;

		struct eventListElement *pSubProcessEventListElement = pNextEventListElement;
		BPMNSubProcess *pSubProcess = (BPMNSubProcess *)pSubProcessEventListElement->pElement;
		BPMNElement *pSourceParent = pElementLast->GetParentElement();
		BPMNElement *pDestParent = (BPMNElement *)pSubProcess;
		list <BPMNElement *> *pElementList = GetElementList(pElementLast);
		BPMNElement *pNewElement = RemoveElementsFromList(pElementList, selectedItems);

		//AddElementsToList(selectedItems, pSubProcess->GetFirstElement(), false);
		BPMNElement *pElementProcessFirst = pSubProcess->GetFirstElement();
		pSubProcess->AddElementListAbove(selectedItems);
		if (pElementProcessFirst == NULL)
		{
			pElementProcessFirst = pSubProcess->GetFirstElement();
		}

		//		Get the last element in the selected list, find the next element
		struct eventListElement *pEventListLastElement = pEventListElement;
		pNextEventListElement = pEventListElement->nextElement;

		//		get the first element of the selected list, find previous element
		BPMNElement *pElement = selectedItems.front();
		itEvent = pEventMap->find(pElement->GetEventIndex());
		if (itEvent == pEventMap->end())
			return;
		pEventListElement = itEvent->second;
		struct eventListElement *pPrevEventListElement = pEventListElement->prevElement;

		//		if the previous element is NULL and the previous element is not a child then the root gets updated to point to next element
		if (pPrevEventListElement == NULL && pSourceParent == NULL)
		{
			//		Write channel header or previous element record (only pointers)
			pPlayerManager->SetEventRoot(pNextEventListElement);
		}
		else
		{
			//		else the previous element next is initialized to the next element after the list
			pPrevEventListElement->nextElement = pNextEventListElement;
			pNextEventListElement->prevElement = pPrevEventListElement;
			pPlayerManager->UpdateEventRecord(pPrevEventListElement);
		}

		//		the first element of the list and the last element of the list pointers are initialized, the rec type of each is changed
		//		rewrite all records (only pointers
		itEvent = pEventMap->find(pElementProcessFirst->GetEventIndex());
		if (itEvent == pEventMap->end())
			return;
		struct eventListElement *pOldFirstElement = itEvent->second;
		pEventListLastElement->nextElement = pOldFirstElement;
		pOldFirstElement->prevElement = pEventListLastElement;

		pEventListElement->prevElement = NULL;
		while (pEventListElement != NULL)
		{
			pPlayerManager->UpdateEventRecord(pEventListElement);
			if (pEventListElement == pEventListLastElement)
				break;
			pEventListElement = pEventListElement->nextElement;
		}

		// finally update the SubProcess
		UpdateSourceAndDestinationParentRecords(pSourceParent, pDestParent);
		pPlayerManager->UpdateChannelHeader(0);
		CreateAndDisplayDiagram(pElementList);
		if (pNewElement != NULL)
		{
			ShowElement(pNewElement->GetEventIndex());
		}
	}
}


void BPMNControlPanel::UpdateSourceAndDestinationParentRecords(BPMNElement *pSource, BPMNElement *pDest)
{
	map <ULONG, struct eventListElement *>::iterator itEvent = pEventMap->end();
	struct eventListElement *pEventListElement = NULL;

	// Need to update both source and Destination parents
	if (pSource != NULL)
	{
		itEvent = pEventMap->find(pSource->GetEventIndex());
		if (itEvent == pEventMap->end())
			return;
		pEventListElement = itEvent->second;
		pPlayerManager->UpdateEventRecord(pEventListElement);
	}
	if (pDest != NULL)
	{
		itEvent = pEventMap->find(pDest->GetEventIndex());
		if (itEvent == pEventMap->end())
			return;
		pEventListElement = itEvent->second;
		pPlayerManager->UpdateEventRecord(pEventListElement);
	}
}

void BPMNControlPanel::GoUpDiagramLevel()
{
	if (pBPMNDiagram == NULL || pBPMNDiagram->size() == 0)
		return;
	BPMNElement *pElement = pBPMNDiagram->front();
	BPMNElement *pParent = pElement->GetParentElement();
	if (pParent == NULL)
		return;

	ShowEventInWindow(pParent->GetEventIndex());
/*	pBPMNDiagram = GetElementList(pParent);
	CreateAndDisplayDiagram(pBPMNDiagram);
	ShowElement(pParent->GetEventIndex());
	UpdateBPMNControlWindow();
*/
	//delete pDiagramState;
}

void BPMNControlPanel::MoveItemsLevelUp()
{
	if (pDiagramMap == NULL)
		return;
	if (pDiagramMap->CheckIfSelectedInSequence())
	{
		bool bChanged = false;
		list <BPMNElement *> selectedList;
		BPMNElement *pNewElement = NULL;
		selectedList = pDiagramMap->GetSelectedItems();
		if (selectedList.size() == 0)
			return;
		BPMNElement *pFirstElement = selectedList.front();
		BPMNElement *pParentElement = pFirstElement->GetParentElement();
		if (pParentElement == NULL)
			return; // This is the top level, so cannot move
		BPMNElement *pSelectionParent = pParentElement;
		BPMNElement *pDestinationParent = pParentElement->GetParentElement();

		map <ULONG, struct eventListElement *>::iterator itEvent = pEventMap->find(pFirstElement->GetEventIndex());
		if (itEvent == pEventMap->end())
			return;
		struct eventListElement *pEventListElement = itEvent->second;
		list <BPMNElement *> *pElementList = GetElementList(pFirstElement);
		struct eventListElement *pEventListPrevElement = NULL;

		BPMNElement *pLastElement = selectedList.back();
		itEvent = pEventMap->find(pLastElement->GetEventIndex());
		if (itEvent == pEventMap->end())
			return;
		struct eventListElement *pEventListLastSelectedElement = itEvent->second;

		if (pEventListElement->prevElement == NULL)
		{
			// selected list is at top;
			pNewElement = RemoveElementsFromList(pElementList, selectedList);

			list <BPMNElement *> *pParentElementList = GetElementList(pParentElement);
			// Get Insertion point in parent list
			list <BPMNElement *>::iterator itInsPoint = find(pParentElementList->begin(), pParentElementList->end(), pParentElement);
			if (itInsPoint == pParentElementList->end())
				return;

			pParentElementList->insert(itInsPoint, selectedList.begin(), selectedList.end());

			if (pNewElement != NULL)
			{
				itEvent = pEventMap->find(pNewElement->GetEventIndex());
				struct eventListElement *pNewEventListItem = NULL;
				if (itEvent != pEventMap->end())
				{
					pNewEventListItem = itEvent->second;
					pNewEventListItem->prevElement = NULL;
					pPlayerManager->UpdateEventRecord(pNewEventListItem);
				}

			}

			itEvent = pEventMap->find(pParentElement->GetEventIndex());
			if (itEvent == pEventMap->end())
				return;
			struct eventListElement *pEventListInsertionPointElement = itEvent->second;
			struct eventListElement *pEventListFirstElement = pEventListInsertionPointElement->prevElement;
			pEventListInsertionPointElement->prevElement = pEventListLastSelectedElement;

			// if inserted to first element then change root
			//		if the previous element is NULL and the previous element is not a child then the root gets updated to point to next element
			BPMNElement *pRootParent = pParentElement->GetParentElement();
			if (pEventListFirstElement == NULL && pRootParent == NULL)
			{
				//		Write channel header or previous element record (only pointers)
				pPlayerManager->SetEventRoot(pEventListElement);
				pEventListPrevElement = NULL;
			}
			else
			{
				//		else the previous element next is initialized to the next element after the list
				pEventListFirstElement->nextElement = pEventListElement;
				pPlayerManager->UpdateEventRecord(pEventListFirstElement);
				pEventListPrevElement = pEventListFirstElement;
			}

			if (pEventListLastSelectedElement != NULL)
			{
				pEventListLastSelectedElement->nextElement = pEventListInsertionPointElement;
			}

			list <BPMNElement *>::iterator itList = selectedList.begin();
			while (itList != selectedList.end())
			{
				BPMNElement *pBPMNListElement = *itList;
				itEvent = pEventMap->find(pBPMNListElement->GetEventIndex());
				if (itEvent == pEventMap->end())
					break;
				struct eventListElement *pEventListCurrElement = itEvent->second;
				pEventListCurrElement->prevElement = pEventListPrevElement;
				pEventListCurrElement->pElement->SetParentElement(pRootParent);
				pPlayerManager->UpdateEventRecord(pEventListCurrElement);
				pEventListPrevElement = pEventListCurrElement;
				itList++;
			}

			if (pElementList->size() > 0)
			{
				BPMNElement *pFirstElementInDeletedList = pElementList->front();
				itEvent = pEventMap->find(pFirstElementInDeletedList->GetEventIndex());
				if (itEvent != pEventMap->end())
				{
					struct eventListElement *pEventFirstInDeletedList = itEvent->second;
					pEventFirstInDeletedList->prevElement = NULL;
					pPlayerManager->UpdateEventRecord(pEventFirstInDeletedList);
				}
			}
			UpdateSourceAndDestinationParentRecords(pSelectionParent, pDestinationParent);
			pPlayerManager->UpdateChannelHeader(0);
			pDiagramMap->ClearSelectedList();
			bChanged = true;
		}
		else
		{
			pLastElement = selectedList.back();

			itEvent = pEventMap->find(pLastElement->GetEventIndex());
			if (itEvent == pEventMap->end())
				return;
			struct eventListElement *pLastEventListElement = itEvent->second;
			if (pLastEventListElement->nextElement != NULL)
				return;

			// **17-Jan** duplicate declaration
			//BPMNElement *pSelectionParent = pParentElement;
			//BPMNElement *pDestinationParent = pParentElement->GetParentElement();
			pNewElement = RemoveElementsFromList(pElementList, selectedList);

			if (pNewElement != NULL)
			{
				itEvent = pEventMap->find(pNewElement->GetEventIndex());
				struct eventListElement *pNewEventListItem = NULL;
				if (itEvent != pEventMap->end())
				{
					pNewEventListItem = itEvent->second;
					pNewEventListItem->nextElement = NULL;
					pPlayerManager->UpdateEventRecord(pNewEventListItem);
				}

			}
			// Find insertion point below the in the level above.
			list <BPMNElement *> *pParentElementList = GetElementList(pParentElement);
			// Get Insertion point in parent list
			list <BPMNElement *>::iterator itInsPoint = find(pParentElementList->begin(), pParentElementList->end(), pParentElement);
			if (itInsPoint == pParentElementList->end())
				return;
			itInsPoint++; // go to the next element and add above it
			pParentElementList->insert(itInsPoint, selectedList.begin(), selectedList.end());

			// Start with the Parent Element, update all event records till the end of the selected list + 1
			itEvent = pEventMap->find(pParentElement->GetEventIndex());
			if (itEvent == pEventMap->end())
				return;
			struct eventListElement *pEventListInsertionPointElement = itEvent->second;
			struct eventListElement *pEventInsPointElement = pEventListInsertionPointElement->nextElement;
			pEventListInsertionPointElement->nextElement = pEventListElement;
			pLastEventListElement->nextElement = pEventInsPointElement;
			pPlayerManager->UpdateEventRecord(pEventListInsertionPointElement);
			pEventListPrevElement = pEventListInsertionPointElement;

			list <BPMNElement *>::iterator itList = selectedList.begin();
			while (itList != selectedList.end())
			{
				BPMNElement *pBPMNListElement = *itList;
				itEvent = pEventMap->find(pBPMNListElement->GetEventIndex());
				if (itEvent == pEventMap->end())
					break;
				struct eventListElement *pEventListCurrElement = itEvent->second;
				pEventListCurrElement->prevElement = pEventListPrevElement;
				pEventListCurrElement->pElement->SetParentElement(pDestinationParent);
				pPlayerManager->UpdateEventRecord(pEventListCurrElement);
				pEventListPrevElement = pEventListCurrElement;
				itList++;
			}

			if (pEventInsPointElement != NULL)
			{
				pEventInsPointElement->prevElement = pEventListPrevElement;
				pPlayerManager->UpdateEventRecord(pEventInsPointElement);
			}

			if (pElementList->size() > 0)
			{
				BPMNSubProcess *pSelectedListSubProcess = (BPMNSubProcess *)pSelectionParent;
				BPMNElement *pLastElementInDeletedList = pSelectedListSubProcess->GetLastElement();
				itEvent = pEventMap->find(pLastElementInDeletedList->GetEventIndex());
				if (itEvent != pEventMap->end())
				{
					struct eventListElement *pEventLastInDeletedList = itEvent->second;
					pEventLastInDeletedList->nextElement = NULL;
					pPlayerManager->UpdateEventRecord(pEventLastInDeletedList);
				}
			}

			UpdateSourceAndDestinationParentRecords(pSelectionParent, pDestinationParent);
			pPlayerManager->UpdateChannelHeader(0);

			pDiagramMap->ClearSelectedList();
			bChanged = true;
		}
		if (bChanged)
		{
			if (pElementList->size() == 2) // All events have been deleted only the start and stop are present
			{
				// remove the sub-process
				ULONG subProcessID = pSelectionParent->GetEventIndex();

				itEvent = pEventMap->find(subProcessID);
				if (itEvent != pEventMap->end())
				{
					struct eventListElement *pSubProcessElement = itEvent->second;
					struct eventListElement *pSubProcessPrevElement = pSubProcessElement->prevElement;
					struct eventListElement *pSubProcessNextElement = pSubProcessElement->nextElement;

					pSubProcessPrevElement->nextElement = pSubProcessNextElement;
					pSubProcessNextElement->prevElement = pSubProcessPrevElement;
					pPlayerManager->UpdateEventRecord(pSubProcessPrevElement);
					pPlayerManager->UpdateEventRecord(pSubProcessNextElement);
					if (pSubProcessPrevElement != NULL)
						pNewElement = pSubProcessPrevElement->pElement;
					else if (pSubProcessNextElement != NULL)
						pNewElement = pSubProcessNextElement->pElement;
					pElementList = GetElementList(pNewElement);

					// In order to reuse the functions, we will put the subprocess into a list and then call existing functions
					list <BPMNElement *> subProcessList;
					subProcessList.clear();
					subProcessList.push_back(pSelectionParent);
					RemoveElementsFromList(pElementList, subProcessList);
				}
			}
			CreateAndDisplayDiagram(pElementList);
			if (pNewElement != NULL)
			{
				ShowElement(pNewElement->GetEventIndex());
			}
		}
	}
}

void BPMNControlPanel::BranchDelete()
{
	if (pDiagramMap == NULL)
		return;

	list <BPMNElement *> selectedList = pDiagramMap->GetSelectedItems();
	if (selectedList.size() != 1)
		return;
	BPMNElement *pElementFirst = selectedList.front();
	pPlayerManager->BranchDelete(pElementFirst->GetEventIndex(), false);
	ResetDiagram();
	pPlayerManager->InitializeVideoPath();
}


void BPMNControlPanel::DeleteItems()
{
	if (pDiagramMap == NULL)
		return;
	list <BPMNElement *> selectedList = pDiagramMap->GetSelectedItems();
	if (selectedList.size() == 0)
		return;

	pDiagramMap->ClearSelectedList();

	// here check if the selected elements belong to a sub-process and all elements of the sub-process are selected, then 
	// remove all the selected elements and add only the sub-process as the selected element
	BPMNElement *pFirstElementParent = selectedList.front()->GetParentElement();
	BPMNElement *pLastElementParent = selectedList.back()->GetParentElement();

	if (pFirstElementParent != NULL && pFirstElementParent == pLastElementParent && pFirstElementParent->GetType() == 3)
	{
		BPMNSubProcess *pSelectedSubProcess = (BPMNSubProcess *)pFirstElementParent;
		// if both elements are the same sub-process
		if (pSelectedSubProcess->GetSubProcessElements()->size() == selectedList.size() + 2)
		{
			// If all elements of the sub process are selected then clear the selected list and add the sub-process as the 
			// selected item.

			selectedList.clear();
			selectedList.push_back(pFirstElementParent);
		}
	}

	BPMNElement *pElementFirst = selectedList.front();
	list <BPMNElement *> *pElementList = GetElementList(pElementFirst);

	list <BPMNElement *>::iterator itList = selectedList.begin();
	while (itList != selectedList.end())
	{
		pElementFirst = *itList;
		itList++;
		map <ULONG, struct eventListElement *>::iterator itEvent = pEventMap->find(pElementFirst->GetEventIndex());
		if (itEvent == pEventMap->end())
			continue;
		struct eventListElement *pEventListElement = itEvent->second;
		struct eventListElement *pEventListPrevElement = pEventListElement->prevElement;
		struct eventListElement *pEventListNextElement = pEventListElement->nextElement;

		BPMNElement *pRootParent = pEventListElement->pElement->GetParentElement();
		if (pEventListPrevElement == NULL && pRootParent == NULL)
		{
			//		Write channel header or previous element record (only pointers)
			pPlayerManager->SetEventRoot(pEventListNextElement);
			pEventListNextElement->prevElement = NULL;
			pPlayerManager->UpdateEventRecord(pEventListNextElement);
		}
		else
		{
			if (pEventListPrevElement != NULL)
			{
				pEventListPrevElement->nextElement = pEventListNextElement;
				pPlayerManager->UpdateEventRecord(pEventListPrevElement);
			}
			if (pEventListNextElement != NULL)
			{
				pEventListNextElement->prevElement = pEventListPrevElement;
				pPlayerManager->UpdateEventRecord(pEventListNextElement);
			}
		}
		UpdateSourceAndDestinationParentRecords(pRootParent, NULL);
	}

	BPMNElement *pNewElement = DeleteElementsFromList(pElementList, selectedList);

	// The memory for the deleted item is not released and still retained in the map, so that it can be deleted at the end
	// remove from file
	list <BPMNElement *>::iterator itSegEvents = selectedList.begin();
	while (itSegEvents != selectedList.end())
	{
		BPMNElement *pElementFirstSeg = *itSegEvents;
		itSegEvents++;
		map <ULONG, struct eventListElement *>::iterator itEvent = pEventMap->find(pElementFirstSeg->GetEventIndex());
		if (itEvent == pEventMap->end())
			continue;
		struct eventListElement *pEventListElement = itEvent->second;
		pEventMap->erase(pElementFirstSeg->GetEventIndex());
		delete pEventListElement->pElement;
		delete pEventListElement;
	}

	pPlayerManager->InitializeVideoPath();
	CreateAndDisplayDiagram(pElementList);
	if (pNewElement != NULL)
	{
		ShowElement(pNewElement->GetEventIndex());
	}
}

void BPMNControlPanel::BranchCopy()
{
	if (pDiagramMap == NULL)
		return;
	list <BPMNElement *> selectedList = pDiagramMap->GetSelectedItems();
	if (selectedList.size() != 1)
		return;
	BPMNElement *pElementFirst = selectedList.front();
	pPlayerManager->BranchCopy(pElementFirst->GetEventIndex());
	pPlayerManager->InitializeVideoPath();
	ResetDiagram();
}

void BPMNControlPanel::BranchRight()
{
	if (pDiagramMap == NULL)
		return;
	list <BPMNElement *> selectedList = pDiagramMap->GetSelectedItems();
	if (selectedList.size() != 1)
		return;
	BPMNElement *pElementFirst = selectedList.front();
	pPlayerManager->BranchRight(pElementFirst->GetEventIndex());
	pPlayerManager->InitializeVideoPath();
	ResetDiagram();
}

void BPMNControlPanel::BranchBottom()
{
	if (pDiagramMap == NULL)
		return;
	list <BPMNElement *> selectedList = pDiagramMap->GetSelectedItems();
	if (selectedList.size() != 1)
		return;
	BPMNElement *pElementFirst = selectedList.front();
	pPlayerManager->BranchBottom(pElementFirst->GetEventIndex());
	pPlayerManager->InitializeVideoPath();
	ResetDiagram();
}

void BPMNControlPanel::SynchronizeVideo()
{
	if (pDiagramMap == NULL)
		return;
	list <BPMNElement *> selectedList = pDiagramMap->GetSelectedItems();
	if (selectedList.size() != 1)
		return;
	BPMNElement *pElementFirst = selectedList.front();
	pPlayerManager->SynchronizeVideo(pElementFirst->GetEventIndex());

}

bool BPMNControlPanel::IsSubProcessSelected(int xPos, int yPos)
{
	return pDiagramMap->IsSubProcessSelected(xPos, yPos);
}

void BPMNControlPanel::ShowSubProcessMap()
{
	BPMNSubProcess *pSubProcess = pDiagramMap->GetSubProcessSelected();
	if (pSubProcess == NULL)
	{
		return;
	}
	list <BPMNElement *> *pElementList = pSubProcess->GetSubProcessElements();
	if (pElementList != NULL)
	{
		pHorizScroll->SetCurrentValue(0);
		pVertScroll->SetCurrentValue(0);
		CreateAndDisplayDiagram(pElementList);
	}
}

bool BPMNControlPanel::MultiSelectBPMNElement(int xPos, int yPos)
{
	if (pDiagramMap->IsPlayerInPlayMode())
		return false;
	return pDiagramMap->MultiSelectBPMNElement(xPos, yPos);
}

bool BPMNControlPanel::SelectBPMNElement(int xPos, int yPos)
{
	if (pDiagramMap->IsPlayerInPlayMode())
		return false;
	return pDiagramMap->SelectBPMNElement(xPos, yPos);
}