#include "stdafx.h"
#include "BPMNDrawPanel.h"
#include "CustomEvents.h"
#include "BPMNElement.h"
#include "BPMNDiagram.h"
#include "PlayerDataFileManager.h"
#include "BPMNDecision.h"
#include "BPMNControlPanel.h"

BEGIN_EVENT_TABLE(BPMNDrawPanel, wxPanel)
// catch paint events

EVT_MOTION(BPMNDrawPanel::OnMouseMove)
EVT_LEFT_DOWN(BPMNDrawPanel::OnLeftDown)
EVT_LEFT_DCLICK(BPMNDrawPanel::OnLeftDblClick)
EVT_RIGHT_DCLICK(BPMNDrawPanel::OnRightDblClick)
EVT_RIGHT_DOWN(BPMNDrawPanel::OnRightDown)
EVT_LEFT_UP(BPMNDrawPanel::OnLeftUp)
EVT_PAINT(BPMNDrawPanel::paintEvent)
END_EVENT_TABLE()


#define FONT_SIZE 10

BPMNDrawPanel::BPMNDrawPanel(wxPanel *parent, ULONG id, wxPoint pt, wxSize size) :
	wxPanel(parent, id, pt, size, wxNO_BORDER)
{
	pDiagramMap = NULL;
	pBPMNDiagram = NULL;
	pPlayerManager = NULL;
	pControlPanel = NULL;
	bCopySegmentFlg = false;
}


BPMNDrawPanel::~BPMNDrawPanel()
{
}


void BPMNDrawPanel::paintEvent(wxPaintEvent & WXUNUSED(event))
{
	wxPaintDC dc(this);
	render(dc);
}

void BPMNDrawPanel::paintNow()
{
	wxClientDC dc(this);
	render(dc);
}

void BPMNDrawPanel::render(wxDC&  dc)
{
	list <BPMNElement *> *pDiagram;

	HDC hdc = dc.GetHDC();
	SetMapMode(hdc, MM_HIMETRIC);
	pDiagram = pBPMNDiagram;
	
	if (pDiagram == NULL)
		return;
	wxSize size = GetSize();

	int cx = size.GetWidth();
	int cy = size.GetHeight();
	HDC backhdc = CreateCompatibleDC(NULL);
	SetMapMode(backhdc, MM_HIMETRIC);
	POINT pVal;
	pVal.x = cx;
	pVal.y = cy;

	DPtoLP(backhdc, &pVal, 1);
	cx = pVal.x;
	cy = -pVal.y;

	HBITMAP backbmp = CreateBitmap(cx, cy, 1, 32, NULL);
	HBITMAP oldbmp = (HBITMAP)SelectObject(backhdc, backbmp);

	BitBlt(backhdc, 0, 0, cx, -cy, 0, 0, 0, WHITENESS); // Clears the screen
	HFONT hFont = CreateFont(382, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, FIXED_PITCH | FF_MODERN, TEXT("Courier New"));

	//HFONT hFont = CreateFont(14, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
	//	CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Arial"));

	HFONT hOldFont = (HFONT)SelectObject(backhdc, hFont); // <-- add this
	SetTextColor(backhdc, RGB(0, 0, 0));
	SetBkMode(backhdc, TRANSPARENT);
	BPMNDiagram *pDiagMap = pDiagramMap;
	POINT pointOffset;
	pControlPanel->GetPointScrollOffset(&pointOffset);
	pDiagMap->DrawDiagram(backhdc, cx, cy, pointOffset);

	BitBlt(hdc, 0, 0, cx, -cy, backhdc, 0, 0, SRCCOPY);

	SelectObject(backhdc, hOldFont);
	SelectObject(backhdc, oldbmp);
	DeleteObject(backbmp);
	DeleteObject(hFont);
	DeleteDC(backhdc);
}



void BPMNDrawPanel::OnLeftUp(wxMouseEvent &WXUNUSED(event))
{
}

void BPMNDrawPanel::SetCopySegmentFlag(bool bFlg)
{
	bCopySegmentFlg = bFlg;
}

void BPMNDrawPanel::OnLeftDown(wxMouseEvent &event)
{
	if (pPlayerManager->GetRecorderStatus())
		return;
	bool bSelected = false;
	int xPos = event.GetX();
	int yPos = event.GetY();

	HDC dc = GetDC(NULL);

	SetMapMode(dc, MM_HIMETRIC);
	POINT pVal;
	pVal.x = xPos;
	pVal.y = yPos;

	DPtoLP(dc, &pVal, 1);
	xPos = pVal.x;
	yPos = -pVal.y;
	DeleteDC(dc);

	if (event.ControlDown()) // control is pressed
		bSelected = pControlPanel->MultiSelectBPMNElement(xPos, yPos);
	else
		bSelected = pControlPanel->SelectBPMNElement(xPos, yPos);

	if (bSelected)
	{
		//paintNow();
		Refresh(false);
	}
	UpdateButtons();

}

void BPMNDrawPanel::OnMouseMove(wxMouseEvent &WXUNUSED(event))
{
}

void BPMNDrawPanel::OnLeftDblClick(wxMouseEvent &event)
{
	if (pPlayerManager->GetRecorderStatus())
		return;
	int xPos = event.GetX();
	int yPos = event.GetY();

	HDC dc = GetDC(NULL);
	SetMapMode(dc, MM_HIMETRIC);
	POINT pVal;
	pVal.x = xPos;
	pVal.y = yPos;

	DPtoLP(dc, &pVal, 1);
	xPos = pVal.x;
	yPos = -pVal.y;
	DeleteDC(dc);

	if (pControlPanel->IsSubProcessSelected(xPos, yPos))
	{
		pControlPanel->ShowSubProcessMap();
	}
	UpdateButtons();
}

void BPMNDrawPanel::OnRightDblClick(wxMouseEvent &WXUNUSED(event))
{
	if (pPlayerManager->GetRecorderStatus())
		return;
	pControlPanel->GoUpDiagramLevel();
	UpdateButtons();
}

void BPMNDrawPanel::SetDiagram(list <BPMNElement *> *pDiagram)
{
	pBPMNDiagram = pDiagram;
}

void BPMNDrawPanel::SetDiagramMap(BPMNDiagram *pMap)
{
	pDiagramMap = pMap;
}

void BPMNDrawPanel::SetController(PlayerDataFileManager *pManager)
{
	pPlayerManager = pManager;
}

void BPMNDrawPanel::SetControlPanel(BPMNControlPanel *pBPMNPanel)
{
	pControlPanel = pBPMNPanel;
}


void BPMNDrawPanel::OnPopupClick(wxCommandEvent &evt)
{
	//void *data = static_cast<wxMenu *>(evt.GetEventObject())->GetClientData();
	switch (evt.GetId()) {
	case IDM_CREATE_SUB_PROCESS:
		pControlPanel->CreateSubProcess();
		break;
	case IDM_MOVE_SUB_PROCESS_ABOVE:
		pControlPanel->AddToSubProcessAbove();
		break;
	case IDM_MOVE_SUB_PROCESS_BELOW:
		pControlPanel->AddToSubProcessBelow();
		break;
	case IDM_MOVE_PROCESS_ABOVE:
		pControlPanel->MoveItemsLevelUp();
		break;
	case IDM_MOVE_LEVEL_UP:
		pControlPanel->GoUpDiagramLevel();
		break;
	case IDM_BRANCH_DELETE:
		pControlPanel->BranchDelete();
		break;
	case IDM_EVENT_DELETE:
		pControlPanel->DeleteItems();
		break;
	case IDM_EVENT_SYNC:
		pControlPanel->SynchronizeVideo();
		break;
	case IDM_BRANCH_BOTTOM:
		pControlPanel->BranchBottom();
		break;
	case IDM_BRANCH_RIGHT:
		pControlPanel->BranchRight();
		break;
	case IDM_BRANCH_COPY:
		pControlPanel->BranchCopy();
		break;

	}

	UpdateButtons();
}

void BPMNDrawPanel::UpdateButtons()
{
	if (IsItemSelected() || IsInSubprocess())
	{
		if (IsItemSelected())
		{
			pControlPanel->EnableCreateProcessButton(true);
			if (IsSubProcessAboveSelected())
				pControlPanel->EnableMoveToSubProcessAboveButton(true);
			else
				pControlPanel->EnableMoveToSubProcessAboveButton(false);
			if (IsSubProcessBelowSelected())
				pControlPanel->EnableMoveToSubProcessBelowButton(true);
			else
				pControlPanel->EnableMoveToSubProcessBelowButton(false);
			if (IsSelectedInSubProcess() && (IsOnTopOfList() || IsOnBottomOfList()))
				pControlPanel->EnableMoveToProcessAboveButton(true);
			else
				pControlPanel->EnableMoveToProcessAboveButton(false);
		}
		else
		{
			pControlPanel->EnableCreateProcessButton(false);
			pControlPanel->EnableMoveToSubProcessAboveButton(false);
			pControlPanel->EnableMoveToSubProcessBelowButton(false);
			pControlPanel->EnableMoveToProcessAboveButton(false);
		}
		if (IsInSubprocess())
			pControlPanel->EnableGoUpLevelButton(true);
		else
			pControlPanel->EnableGoUpLevelButton(false);
		if (IsItemSelected())
		{
			if (IsBranchSelected())
			{
				pControlPanel->EnableDeleteBranchButton(true);
				pControlPanel->EnableDeleteEventButton(false);
			}
			else
			{
				pControlPanel->EnableDeleteBranchButton(false);
				pControlPanel->EnableDeleteEventButton(true);
			}
			if (IsOneItemSelected())
			{
				pControlPanel->EnableSyncVideoButton(true);
				if (IsBranchSelected())
				{
					if (IsBranchRight())
					{
						pControlPanel->EnableBranchBottomButton(true);
						pControlPanel->EnableBranchRightButton(false);
					}
					else
					{
						pControlPanel->EnableBranchBottomButton(false);
						pControlPanel->EnableBranchRightButton(true);
					}
				}
				else
				{
					pControlPanel->EnableBranchBottomButton(false);
					pControlPanel->EnableBranchRightButton(false);
				}
			}
			else
			{
				pControlPanel->EnableSyncVideoButton(false);
				pControlPanel->EnableBranchBottomButton(false);
				pControlPanel->EnableBranchRightButton(false);
			}
		}
		else
		{
			pControlPanel->EnableDeleteBranchButton(false);
			pControlPanel->EnableDeleteEventButton(false);
			pControlPanel->EnableSyncVideoButton(false);
			pControlPanel->EnableBranchBottomButton(false);
			pControlPanel->EnableBranchRightButton(false);
		}
	}

	else
	{
		pControlPanel->EnableDeleteBranchButton(false);
		pControlPanel->EnableDeleteEventButton(false);
		pControlPanel->EnableSyncVideoButton(false);
		pControlPanel->EnableBranchBottomButton(false);
		pControlPanel->EnableBranchRightButton(false);
		pControlPanel->EnableGoUpLevelButton(false);
		pControlPanel->EnableCreateProcessButton(false);
		pControlPanel->EnableMoveToSubProcessAboveButton(false);
		pControlPanel->EnableMoveToSubProcessBelowButton(false);
		pControlPanel->EnableMoveToProcessAboveButton(false);
	}
}

void BPMNDrawPanel::OnRightDown(wxMouseEvent &WXUNUSED(event))
{
	if (pPlayerManager->GetRecorderStatus())
		return;

	wxMenu mnu;

	if (pPlayerManager->GetPlayerMode())
	{
		if (IsOneItemSelected())
		{
			mnu.Append(IDM_EVENT_SYNC, "Sync Video");
			if (IsBranchSelected())
			{
				mnu.Append(wxID_SEPARATOR, "");
				if (IsBranchRight())
					mnu.Append(IDM_BRANCH_BOTTOM, "Branch Bottom");
				else
					mnu.Append(IDM_BRANCH_RIGHT, "Branch Right");
			}
		}
		mnu.Connect(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(BPMNDrawPanel::OnPopupClick), NULL, this);
		PopupMenu(&mnu);
		return;
	}

	//The popupmenu will be displayed when the user right clicks 
	if (IsItemSelected() || IsInSubprocess())
	{
		if (IsItemSelected())
		{
			mnu.Append(IDM_CREATE_SUB_PROCESS, "Create Sub-Process");
			if (IsSubProcessAboveSelected())
				mnu.Append(IDM_MOVE_SUB_PROCESS_ABOVE, "Move to Sub-Process above");
			if (IsSubProcessBelowSelected())
				mnu.Append(IDM_MOVE_SUB_PROCESS_BELOW, "Move to Sub-Process below");
			if (IsSelectedInSubProcess() && (IsOnTopOfList() || IsOnBottomOfList()))
			{
				mnu.Append(wxID_SEPARATOR, "");
				mnu.Append(IDM_MOVE_PROCESS_ABOVE, "Move Activity to Previous Level");
				mnu.Append(wxID_SEPARATOR, "");
			}
		}
		if (IsInSubprocess())
			mnu.Append(IDM_MOVE_LEVEL_UP, "Go Up Level");
		if (IsItemSelected())
		{
			mnu.Append(wxID_SEPARATOR, "");
			if (IsBranchSelected()) {
				mnu.Append(IDM_BRANCH_DELETE, "Delete Branch");
				if (bCopySegmentFlg)
					mnu.Append(IDM_BRANCH_COPY, "Copy Segment To Branch");
			}
			else if (IsSubProcessSelected())
				mnu.Append(IDM_EVENT_DELETE, "Delete Sub-Process");
			else
				mnu.Append(IDM_EVENT_DELETE, "Delete Activity");
			mnu.Append(wxID_SEPARATOR, "");
			if (IsOneItemSelected())
			{
				mnu.Append(IDM_EVENT_SYNC, "Sync Video");
				if (IsBranchSelected())
				{
					mnu.Append(wxID_SEPARATOR, "");
					if (IsBranchRight())
						mnu.Append(IDM_BRANCH_BOTTOM, "Branch Bottom");
					else
						mnu.Append(IDM_BRANCH_RIGHT, "Branch Right");
				}
			}
		}
		mnu.Connect(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(BPMNDrawPanel::OnPopupClick), NULL, this);
		PopupMenu(&mnu);
	}

}


bool BPMNDrawPanel::IsItemSelected()
{
	return pDiagramMap->IsItemSelected();
}

bool BPMNDrawPanel::IsInSubprocess()
{
	return pDiagramMap->IsInSubprocess();
}

bool BPMNDrawPanel::IsSubProcessAboveSelected()
{
	return pDiagramMap->IsSubProcessAboveSelected();
}

bool BPMNDrawPanel::IsSubProcessBelowSelected()
{
	return pDiagramMap->IsSubProcessBelowSelected();
}

bool BPMNDrawPanel::IsSubProcessSelected()
{
	return pDiagramMap->IsSubProcessSelected();
}

bool BPMNDrawPanel::IsSelectedInSubProcess()
{
	return pDiagramMap->IsSelectedInSubProcess();
}

bool BPMNDrawPanel::IsOnTopOfList()
{
	return pDiagramMap->IsOnTopOfList();
}

bool BPMNDrawPanel::IsOnBottomOfList()
{
	return pDiagramMap->IsOnBottomOfList();
}


bool BPMNDrawPanel::IsOneItemSelected()
{
	return pDiagramMap->IsOneItemSelected();
}

bool BPMNDrawPanel::IsBranchSelected()
{
	return pDiagramMap->IsBranchSelected();
}

bool BPMNDrawPanel::IsBranchRight()
{
	BPMNDecision *pDecision = pDiagramMap->GetSelectedBranch();
	if (pDecision != NULL)
	{
		return pPlayerManager->IsBranchRight(pDecision->GetEventIndex());
	}
	return false;
}

void BPMNDrawPanel::GetControlDimensions()
{
	list<BPMNElement *>::iterator it;
	wxClientDC dc(this);
	HDC hDC = dc.GetHDC();
	SetMapMode(hDC, MM_HIMETRIC);
	//SetMapMode(hDC, MM_LOENGLISH);
	HFONT hFont = CreateFont(382, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, FIXED_PITCH | FF_MODERN, TEXT("Courier New"));
	HFONT hFontOld = (HFONT)SelectObject(hDC, hFont);

	it = pBPMNDiagram->begin();

	while (it != pBPMNDiagram->end())
	{
		BPMNElement *pElement = *it;
		pElement->GetControlDimension(hDC);
		it++;
	}
	SelectObject(hDC, hFontOld);
	DeleteObject(hFont);
}
