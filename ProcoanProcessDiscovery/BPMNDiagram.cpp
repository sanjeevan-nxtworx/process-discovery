#include "stdafx.h"
#include "BPMNDiagram.h"
#include "BPMNElement.h"
#include "BPMNSubProcess.h"
#include "BPMNDecision.h"
#include "PlayerDataFileManager.h"
#include "Utility.h"
#include <algorithm>
#include "LineConnectionInfo.h"

BPMNDiagram::BPMNDiagram()
{
	pPlayerManager = NULL;
	pLastSelected = NULL;
	pSyncElement = NULL;
	nCurColumn = -1;
	nCurRow = -1;
	notInPathList.clear();
	inPathList.clear();
}

BPMNDiagram::~BPMNDiagram()
{
	Clear();
	nCurColumn = -1;
	nCurRow = -1;
}

void BPMNDiagram::SetController(PlayerDataFileManager *pManager)
{
	pPlayerManager = pManager;
}

BPMNElement *BPMNDiagram::GetCurrentSyncronizedElement()
{
	return pSyncElement;
}

void BPMNDiagram::SetCurrentSyncronizedElement(BPMNElement *pElement, bool bSegOn)
{
	if (pSyncElement != NULL)
		pSyncElement->SetSyncElement(false);
	if (pElement != NULL)
	{
		pElement->SetSyncElement(true);
		pElement->SetSegElement(bSegOn);
	}

	pSyncElement = pElement;
}

static bool CompareCellID(struct cellDef *first, struct cellDef *second);

bool BPMNDiagram::SelectedListIsAtBottom()
{
	list <struct cellDef *>::reverse_iterator itSelected = selectedList.rbegin();
	struct cellDef *pCell = *itSelected;

	struct cellDef *pCellStop = FindCellInDiagram(pCell->col->num, pCell->row->num + 1);
	if (pCellStop != NULL && pCellStop->bpmnElement != NULL && pCellStop->bpmnElement->GetType() == 5)
		return true;
	return false;
}


bool BPMNDiagram::SelectedListIsAtTop()
{
	list <struct cellDef *>::iterator itSelected = selectedList.begin();
	struct cellDef *pCell = *itSelected;

	if (pCell->row->num == 1)
		return true;
	return false;
}


bool BPMNDiagram::CheckIfSelectedInSequence()
{
	if (selectedList.size() == 0)
		return false;
	// Check if in sequence and if same parent
	selectedList.sort(CompareCellID);
	list <struct cellDef *>::iterator itSelected = selectedList.begin();
	struct cellDef *pCell = *itSelected;
	BPMNElement *pElement = pCell->bpmnElement;
	BPMNElement *pParent = pElement->GetParentElement();
	bool bIsInSequence = true;

	itSelected++;
	while (itSelected != selectedList.end())
	{
		struct cellDef *pCell2 = *itSelected;
		if (pCell2->col->num == pCell->col->num && pCell2->row->num == pCell->row->num + 1 && pCell2->bpmnElement->GetParentElement() == pParent)
		{
			pCell = *itSelected;
		}
		else
		{
			bIsInSequence = false;
			break;
		}
		itSelected++;
	}

	return bIsInSequence;
}

list <BPMNElement *>BPMNDiagram::GetSelectedItems()
{
	list <BPMNElement *> listItems;
	list <struct cellDef *>::iterator itSelected = selectedList.begin();

	while (itSelected != selectedList.end())
	{
		struct cellDef *pCell = *itSelected;

		listItems.push_back(pCell->bpmnElement);
		itSelected++;
	}

	return listItems;
}

void BPMNDiagram::ClearSelectedList()
{
	list <struct cellDef *>::iterator itCell = selectedList.begin();
	while (itCell != selectedList.end())
	{
		struct cellDef *pCell = *itCell;
		if (pCell != NULL && pCell->bpmnElement != NULL)
			pCell->bSelected = false;
		itCell++;
	}
	selectedList.clear();
	SelectLastSelected(NULL);
}

void BPMNDiagram::GetSubProcessesInDiagram(list <BPMNElement *> *pSubProcesses)
{
	vector <vector <struct cellDef *> *>::iterator itColumn = diagram.begin();

	while (itColumn != diagram.end())
	{
		vector <struct cellDef *> *pRow = *itColumn;

		vector <struct cellDef *>::iterator itRow = pRow->begin();
		while (itRow != pRow->end())
		{
			struct cellDef *pCell = *itRow;
			if (pCell != NULL && pCell->bpmnElement != NULL && pCell->bpmnElement->GetType() == 3)
				pSubProcesses->push_back(pCell->bpmnElement);
			itRow++;
		}
		itColumn++;
	}
}

void BPMNDiagram::SetSelectedCell(struct cellDef *pCell)
{
	pCell->bSelected = true;
	SelectLastSelected(pCell);
}

void BPMNDiagram::DeleteCell(struct cellDef *pCell)
{
	if (pCell->bpmnElement->GetType() != 4) // treat decisions differently
	{
		int colNum = pCell->col->num;
		vector <struct cellDef *> *pColumn = diagram[colNum];
		vector <struct cellDef *>::iterator itCell = find(pColumn->begin(), pColumn->end(), pCell);
		if (itCell != pColumn->end())
		{
			vector <struct cellDef *>::iterator itCellNext = itCell + 1;
			while(itCellNext != pColumn->end())
			{
				struct cellDef *pCellNext = *itCellNext;
				pCellNext->row--; // decrement row number
				itCellNext++;
			}
			pColumn->erase(itCell);
		}
	}
}


bool CompareCellID(struct cellDef *first, struct cellDef *second)
{
	if (first->col->num <= second->col->num && first->row->num < second->row->num)
		return true;
	return false;
}

bool BPMNDiagram::SetAnnotationDetails(string strAnnotation, string strAnnotationNotes)
{
	if (pLastSelected)
	{
		int oldLen = pLastSelected->bpmnElement->GetAnnotation().length();
		int newLen = strAnnotation.length();

		pLastSelected->bpmnElement->SetAnnotation(strAnnotation);
		pLastSelected->bpmnElement->SetNotes(strAnnotationNotes);

		if (oldLen != newLen)
		{
			return true;
		}
	}
	return false;
}

bool BPMNDiagram::SetSubProcessDetails(string strName, string strDescription)
{
	if (pLastSelected && pLastSelected->bpmnElement->GetType() == 3)
	{
		int oldLen = pLastSelected->bpmnElement->GetAnnotation().length();
		int newLen = strName.length();

		pLastSelected->bpmnElement->SetAnnotation(strName);
		pLastSelected->bpmnElement->SetNotes(strDescription);

		if (oldLen != newLen)
		{
			return true;
		}
	}
	return false;
}

void BPMNDiagram::SetCurrentColumn(int nCol)
{
	nCurColumn = nCol;
}

void BPMNDiagram::SetCurrentRow(int nRow)
{
	nCurRow = nRow;
}

int BPMNDiagram::GetCurrentColumn()
{
	return nCurColumn;
}

int BPMNDiagram::GetCurrentRow()
{
	return nCurRow;
}

//void  BPMNDiagram::InsertNewColumn()
//{
//	// Insert new column and add's empty cell elements till the current row
//	// renumbers all columns to the right
//	vector <struct headerDef *>::iterator itColumn = columnHeader.begin();
//	while (itColumn != columnHeader.end())
//	{
//		struct headerDef *pColumn = *itColumn;
//		if (pColumn->num == nCurColumn)
//			break;
//		itColumn++;
//	}
//
//	if (itColumn != columnHeader.end())
//	{
//		struct headerDef *pColumn = new struct headerDef;
//		pColumn->size = 0;
//		columnHeader.insert(itColumn, pColumn);
//		nCurColumn++;
//		int numRowsInColumn = GetColumnRows(nCurColumn);
//		for (int i = numRowsInColumn; i < nCurRow; i++)
//		{
//			AddRowToCurrentColumn(NULL);
//		}
//	}
//
//	// Renumber column headers
//	for (size_t i = 0; i < columnHeader.size(); i++)
//	{
//		struct headerDef *pColumn = columnHeader[i];
//		pColumn->num = i;
//	}
//}

int BPMNDiagram::GetColumnRows(int colNum)
{
	if (colNum >= (int)diagram.size())
		return -1;
	vector <struct cellDef *> *rows = diagram[colNum];
	return rows->size();
}

void BPMNDiagram::AddRowToCurrentColumn(BPMNElement *pElement)
{
	vector <struct cellDef *> *rows = diagram[nCurColumn];
	struct cellDef *cell = DBG_NEW struct cellDef;
	cell->bpmnElement = pElement;
	cell->bSelected = false;
	rows->push_back(cell);
	struct headerDef *pHeaderDef = NULL;
	if (rowHeader.size() < rows->size())
	{
		pHeaderDef = DBG_NEW struct headerDef;
		pHeaderDef->size = 0;
		rowHeader.push_back(pHeaderDef);
		pHeaderDef->num = rowHeader.size() - 1;
	}
	else
	{
		pHeaderDef = rowHeader[rows->size() - 1];
	}

	cell->row = pHeaderDef;
	if (pElement != NULL)
	{
		if (pHeaderDef->size < (ULONG)pElement->GetHeight())
			pHeaderDef->size = pElement->GetHeight();
	}

	nCurRow = pHeaderDef->num;

	pHeaderDef = columnHeader[nCurColumn];

	if (pElement != NULL)
	{
		if (pHeaderDef->size < (ULONG)pElement->GetWidth())
			pHeaderDef->size = pElement->GetWidth();
	}
	cell->col = pHeaderDef;
}

void BPMNDiagram::AddColumn()
{
	vector <struct cellDef *> *rows = DBG_NEW vector<struct cellDef *>;
	diagram.push_back(rows);
	nCurColumn = diagram.size() - 1;
}

void BPMNDiagram::SetColumn(int colNum)
{
	struct headerDef *pHeaderDef;
	
	if (columnHeader.size() <= 0)
		return;
	pHeaderDef = columnHeader[colNum];
	nCurColumn = colNum;
}

void BPMNDiagram::CreateLevelDiagram(list <BPMNElement *> *pElementList)
{
	GetControlDimensions(pElementList);
	Clear();
	FindDiagramDimensions(pElementList);
	SetDrawPosition();
}

void BPMNDiagram::GetControlDimensions(list <BPMNElement *> *pElementList)
{
	list<BPMNElement *>::iterator it;
	HDC hDC = GetDC(NULL);
	SetMapMode(hDC, MM_HIMETRIC);
	HFONT hFont = CreateFont(382, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, FIXED_PITCH | FF_MODERN, TEXT("Courier New"));
	HFONT hFontOld = (HFONT)SelectObject(hDC, hFont);

	it = pElementList->begin();

	while (it != pElementList->end())
	{
		BPMNElement *pElement = *it;
		pElement->GetControlDimension(hDC);
		it++;
	}
	SelectObject(hDC, hFontOld);
	DeleteObject(hFont);
	DeleteDC(hDC);
}

void BPMNDiagram::ResetDiagram()
{
	vector <vector <struct cellDef *> *>::iterator itColumn = diagram.begin();

	while (itColumn != diagram.end())
	{
		vector <struct cellDef *> *pRow = *itColumn;

		// go through each row and calculate dimensions and position in diagram
		vector <struct cellDef *>::iterator itRow = pRow->begin();
		while (itRow != pRow->end())
		{
			struct cellDef *pCell = *itRow;
			BPMNElement *pElement = pCell->bpmnElement;
			if (pElement != NULL)
			{
				struct headerDef *pHeaderDef = NULL;

				pHeaderDef = pCell->row;
				if (pHeaderDef->size < (ULONG)pElement->GetWidth())
					pHeaderDef->size = pElement->GetWidth();

				pHeaderDef = pCell->col;
				if (pHeaderDef->size < (ULONG)pElement->GetHeight())
					pHeaderDef->size = pElement->GetHeight();
			}
			itRow++;
		}
		itColumn++;
	}
}

int BPMNDiagram::GetDrawingHeight()
{
	vector <struct headerDef *>::iterator it;
	int size = 0;
	it = rowHeader.begin();

	while (it != rowHeader.end())
	{
		struct headerDef *pHeader = *it;

		size += pHeader->size;
		size += ROW_MARGIN;
		it++;
	}
	return size;
}

void BPMNDiagram::SelectLastSelected(struct cellDef *pSelected)
{
	pLastSelected = pSelected;
	//if (pSelected == NULL || pSelected->bpmnElement == NULL)
	//	pPlayerManager->SetTabFields(0);
	//else
	pPlayerManager->SetTabFields(pSelected);
}

bool BPMNDiagram::MultiSelectBPMNElement(int xPos, int yPos)
{
	struct cellDef *pCell = NULL;
	pCell = FindCell(xPos, yPos);

	if (pCell != NULL)
	{
		bool bSelected = pCell->bSelected;

		if (!bSelected)
		{
			pCell->bSelected = true;
			selectedList.push_back(pCell);
			SelectLastSelected(pCell);
		}
		else
		{
			pCell->bSelected = false;
			selectedList.remove(pCell);
			SelectLastSelected(NULL);
		}

		return true;
	}
	else
		SelectLastSelected(NULL);

	return false;
}

bool BPMNDiagram::IsSubProcessSelected(int xPos, int yPos)
{
	if (SelectBPMNElement(xPos, yPos, false))
	{
		if (pLastSelected != NULL && pLastSelected->bpmnElement->GetType() == 3)
			return true;
	}

	return false;
}

struct cellDef *BPMNDiagram::GetSubProcessToAdd(int direction)
{
	struct cellDef *lastCellSelected = NULL;
	struct cellDef *pCell = NULL;
	if (direction > 0)
		lastCellSelected = selectedList.back();
	else
		lastCellSelected = selectedList.front();

	if (lastCellSelected != NULL)
	{
		pCell = FindCellInDiagram(lastCellSelected->col->num, lastCellSelected->row->num + direction);
	}
	return pCell;

}


BPMNSubProcess *BPMNDiagram::GetSubProcessSelected()
{
	return (BPMNSubProcess *)pLastSelected->bpmnElement;
}

bool BPMNDiagram::SelectBPMNElement(int xPos, int yPos, bool bTest)
{
	struct cellDef *pCell = NULL;
	struct cellDef *pCellSelected = NULL;

	list <struct cellDef *>::iterator it;

	pCell = FindCell(xPos, yPos);

	if (pCell != NULL)
	{
		bool bSelected = pCell->bSelected;
		it = selectedList.begin();
		while (it != selectedList.end())
		{
			pCellSelected = *it;
			pCellSelected->bSelected = false;
			it++;
		}

		selectedList.clear();


		if (!bSelected || !bTest)
		{
			pCell->bSelected = true;
			selectedList.push_back(pCell);
			pPlayerManager->BPMNElementSelected(pCell);
		}
		SelectLastSelected(pCell);
		return true;
	}
	else
		SelectLastSelected(NULL);

	return false;
	
}

struct cellDef *BPMNDiagram::FindCellInDiagram(int col, int row)
{
	if (col >= (int)diagram.size())
		return NULL;

	vector <struct cellDef *> *pColumn = diagram[col];
	vector <struct cellDef *>::iterator itRow = pColumn->begin();
	struct cellDef *pCell = NULL;
	while (itRow != pColumn->end())
	{
		pCell = *itRow;

		if (pCell->row->num == row)
			break;
		itRow++;
	}

	return pCell;
}

struct cellDef *BPMNDiagram::FindEventInDiagram(ULONG nEventNum)
{
	if (diagram.size() == 0)
		return NULL;

	vector <vector <struct cellDef *> *>::iterator itColumn = diagram.begin();
	
	while (itColumn != diagram.end())
	{
		vector <struct cellDef *> *rows = *itColumn;
		vector <struct cellDef *>::iterator itRow = rows->begin();

		while (itRow != rows->end())
		{
			struct cellDef *pCell = *itRow;

			if (pCell->bpmnElement != NULL && pCell->bpmnElement->GetEventIndex() == (long)nEventNum)
				return pCell;
			itRow++;
		}
		itColumn++;
	}

	return NULL;
}


struct cellDef *BPMNDiagram::FindCell(int xPos, int yPos)
{
	vector <vector <struct cellDef *> *>::iterator itColumn = diagram.begin();
	POINT pt;
	
	pPlayerManager->GetPointOffset(&pt);
	pt.x += xPos;
	pt.y += yPos;

	while (itColumn != diagram.end())
	{
		vector <struct cellDef *> *rows = *itColumn;
		vector <struct cellDef *>::iterator itRow = rows->begin();

		while (itRow != rows->end())
		{
			struct cellDef *pCell = *itRow;
		
			if (pCell->bpmnElement != NULL && pCell->bpmnElement->IsPtInElement(pt))
				return pCell;
			itRow++;
		}
		itColumn++;
	}

	return NULL;

}

int BPMNDiagram::GetDrawingWidth()
{
	vector <struct headerDef *>::iterator it;
	int size = 0;
	it = columnHeader.begin();

	while (it != columnHeader.end())
	{
		struct headerDef *pHeader = *it;

		size += pHeader->size;
		size += COLUMN_MARGIN;
		it++;
	}
	return size;
}

void BPMNDiagram::SetDrawPosition()
{
	vector <vector <struct cellDef *> *>::iterator itColumn = diagram.begin();
	vector <struct headerDef *>::iterator itColHdr = columnHeader.begin();
	int xPos, yPos;

	//xPos = offset.x;
	xPos = 0;
	while (itColumn != diagram.end())
	{
		int columnWidth = (*itColHdr)->size + COLUMN_MARGIN;
		//yPos = offset.y;
		yPos = 0;
		vector <struct cellDef *> *rows = *itColumn;
		if (rows->size() > 0)
		{
			vector <struct cellDef *>::iterator itRow = rows->begin();
			vector <struct headerDef *>::iterator itRowHdr = rowHeader.begin();

			while (itRow != rows->end())
			{
				struct cellDef *pCell = *itRow;
				POINT ptTopLeft;
				int rowHeight = (*itRowHdr)->size + ROW_MARGIN;

				BPMNElement *pElement = pCell->bpmnElement;
				if (pElement != NULL)
				{
					ptTopLeft.x = xPos + (columnWidth - pElement->GetWidth())/2;
					ptTopLeft.y = yPos + (rowHeight - pElement->GetHeight()) / 2;
					pElement->SetDisplayPosition(ptTopLeft);
				}
				yPos += rowHeight;
				itRow++;
				itRowHdr++;
			}
		}
		xPos += (*itColHdr)->size + COLUMN_MARGIN;
		itColHdr++;
		itColumn++;
	}
}

bool BPMNDiagram::IsElementInWindow(BPMNElement *pElement, LPRECT pWindowRect, LPRECT pElementRect)
{
	POINT topLeft, topRight, botLeft, botRight;
	topLeft.x = pElementRect->left;
	topLeft.y = pElementRect->top;
	topRight.x = pElementRect->right;
	topRight.y = pElementRect->top;
	botLeft.x = pElementRect->left;
	botLeft.y = pElementRect->bottom;
	botRight.x = pElementRect->right;
	botRight.y = pElementRect->bottom;

	// If element is completely in window
	if (PtInRect(pWindowRect, topLeft) && PtInRect(pWindowRect, topRight)
		&& PtInRect(pWindowRect, botLeft) && PtInRect(pWindowRect, botRight))
		return true;
	// if the box width is bigger than the window width
	if (topLeft.x < pWindowRect->left && topRight.x > pWindowRect->right &&
		(topLeft.y > pWindowRect->top && topLeft.y < pWindowRect->bottom) &&
		(botLeft.y > pWindowRect->top && botLeft.y < pWindowRect->bottom))
		return true;

	return false;

}

bool BPMNDiagram::IsElementPartialInWindow(BPMNElement *pElement, LPRECT pWindowRect, LPRECT pElementRect)
{
	POINT topLeft, topRight, botLeft, botRight;
	topLeft.x = pElementRect->left;
	topLeft.y = pElementRect->top;
	topRight.x = pElementRect->right;
	topRight.y = pElementRect->top;
	botLeft.x = pElementRect->left;
	botLeft.y = pElementRect->bottom;
	botRight.x = pElementRect->right;
	botRight.y = pElementRect->bottom;

	// If element is completely in window
	if (PtInRect(pWindowRect, topLeft) || PtInRect(pWindowRect, topRight)
		|| PtInRect(pWindowRect, botLeft) || PtInRect(pWindowRect, botRight))
		return true;
	// if the box width is bigger than the window width
	if (topLeft.x < pWindowRect->left && topRight.x > pWindowRect->right &&
		(topLeft.y > pWindowRect->top && topLeft.y < pWindowRect->bottom) &&
		(botLeft.y > pWindowRect->top && botLeft.y < pWindowRect->bottom))
		return true;

	return false;

}

wxSize BPMNDiagram::GetDiagramExtents(HDC hdc)
{
	vector <vector <struct cellDef *> *>::iterator itColumn = diagram.begin();
	vector <struct headerDef *>::iterator itColHdr = columnHeader.begin();
	RECT windowRect;
	windowRect.top = windowRect.left = 0;
	windowRect.right = windowRect.bottom = 0;

	wxSize sz;
	sz.SetWidth(0);
	sz.SetHeight(0);
	
	while (itColumn != diagram.end())
	{
		//int columnWidth = (*itColHdr)->size;
		vector <struct cellDef *> *rows = *itColumn;
		if (rows->size() > 0)
		{
			vector <struct cellDef *>::iterator itRow = rows->begin();
			vector <struct headerDef *>::iterator itRowHdr = rowHeader.begin();

			while (itRow != rows->end())
			{
				struct cellDef *pCell = *itRow;
				BPMNElement *pElement = pCell->bpmnElement;
				if (pElement != NULL)
				{
					RECT rect;
					pElement->GetDisplayPosition(&rect);
					OffsetRect(&rect, 500, 500);
					if (rect.bottom > sz.GetHeight())
						sz.SetHeight(rect.bottom);
					if (rect.right > sz.GetWidth())
						sz.SetWidth(rect.right);

					list <struct cellDef *>::iterator itConnections = pCell->to.begin();
					while (itConnections != pCell->to.end())
					{
						struct cellDef *pCellTo = *itConnections;
						BPMNElement *pElementTo = pCellTo->bpmnElement;
						RECT rectFrom, rectTo;
						pElement->GetDisplayPosition(&rectFrom);
						OffsetRect(&rectFrom, 500, 500);
						pElementTo->GetDisplayPosition(&rectTo);
						OffsetRect(&rectTo, 500, 500);
						if (rectFrom.bottom > sz.GetHeight())
							sz.SetHeight(rectFrom.bottom);
						if (rectFrom.right > sz.GetWidth())
							sz.SetWidth(rectFrom.right);

						if (rectTo.bottom > sz.GetHeight())
							sz.SetHeight(rectTo.bottom);
						if (rectTo.right > sz.GetWidth())
							sz.SetWidth(rectTo.right);

						itConnections++;
					}
				}

				itRow++;
				itRowHdr++;
			}
		}
		itColHdr++;
		itColumn++;
	}

	sz.SetHeight(sz.GetHeight() + 500);
	sz.SetWidth(sz.GetWidth() + 500);

	return sz;

}

void BPMNDiagram::GetEventBitmap(vector<unsigned char>& buffer, ULONG& cx, ULONG& cy, ULONG nEventID, vector <BPMNElement *> &pElementList)
{
	ULONG eventID = 0;
	size_t sizeList = pElementList.size();
	BPMNElement *pElement = pElementList[sizeList - 1];

	eventID = pElement->GetEventIndex();

	long long nTime = pPlayerManager->GetEventVideoTime(eventID);
	pPlayerManager->GetEventImage(nTime, buffer, cx, cy, nEventID, pElementList);
}

void BPMNDiagram::CreateBPMNBitmap(vector<unsigned char>& buffer, SIZE *szBmp)
{
	HDC dc = GetDC(NULL);
	
	SetMapMode(dc, MM_HIMETRIC);

	wxSize sz = GetDiagramExtents(dc);

	int cx = sz.GetWidth();
	int cy = sz.GetHeight();
	HDC backhdc = CreateCompatibleDC(dc);
	SetMapMode(backhdc, MM_HIMETRIC);
	POINT pxVal;
	pxVal.x = cx;
	pxVal.y = -cy;

	LPtoDP(backhdc, &pxVal, 1);
	int bmpWidth, bmpHeight;
	bmpWidth = pxVal.x;
	bmpHeight = pxVal.y;
	//cx = pxVal.x;
	//cy = -pxVal.y;
	//HBITMAP backbmp = CreateBitmap(cx, cy, 1, 32, NULL);
	HBITMAP backbmp = CreateCompatibleBitmap(dc, bmpWidth, bmpHeight);
	HBITMAP oldbmp = (HBITMAP)SelectObject(backhdc, backbmp);

	BitBlt(backhdc, 0, 0, cx, -cy, 0, 0, 0, WHITENESS); // Clears the screen
	HFONT hFont = CreateFont(382, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, FIXED_PITCH | FF_MODERN, TEXT("Courier New"));

	HFONT hOldFont = (HFONT)SelectObject(backhdc, hFont); // <-- add this
	SetTextColor(backhdc, RGB(0, 0, 0));
	//SetBkMode(backhdc, TRANSPARENT);
	POINT pointOffset;
	pointOffset.x = 1000;
	pointOffset.y = 1000;
	DrawDiagramForPrint(backhdc, cx, cy, pointOffset);
	
	SelectObject(backhdc, hOldFont);
	backbmp = (HBITMAP)SelectObject(backhdc, oldbmp);

	BITMAP bmpHdr;
	BITMAPINFO bpmInfo;
	ZeroMemory(&bpmInfo, sizeof(BITMAPINFO));
	GetObject(backbmp, sizeof(BITMAP), &bmpHdr);

	szBmp->cx = bmpHdr.bmWidth;
	szBmp->cy = bmpHdr.bmHeight;
	bpmInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bpmInfo.bmiHeader.biWidth = bmpHdr.bmWidth;
	bpmInfo.bmiHeader.biHeight = bmpHdr.bmHeight;
	bpmInfo.bmiHeader.biPlanes = bmpHdr.bmPlanes; // we are assuming that there is only one plane
	bpmInfo.bmiHeader.biBitCount = bmpHdr.bmBitsPixel;
	bpmInfo.bmiHeader.biCompression = BI_RGB;
	bpmInfo.bmiHeader.biSizeImage = bmpHdr.bmWidth * 4L * bmpHdr.bmHeight;
	bpmInfo.bmiHeader.biClrImportant = 0;
	
	BITMAPFILEHEADER hdr;
	ZeroMemory(&hdr, sizeof(BITMAPFILEHEADER));
	// type == BM
	hdr.bfType = 0x4d42;

	hdr.bfSize = (sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + bpmInfo.bmiHeader.biClrUsed * sizeof(RGBQUAD) + (bmpHdr.bmWidth * 4L * bmpHdr.bmHeight));
	hdr.bfReserved1 = 0;
	hdr.bfReserved2 = 0;
	hdr.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + bpmInfo.bmiHeader.biClrUsed * sizeof(RGBQUAD);

	LPBYTE lpBits;
	lpBits = (LPBYTE)GlobalAlloc(GMEM_FIXED, (bmpHdr.bmWidth * 4L * bmpHdr.bmHeight));

	if (!lpBits)
	{
		return; // could not allocate bitmap
	}

	GetDIBits(dc, backbmp, 0, (WORD)bpmInfo.bmiHeader.biHeight, lpBits, &bpmInfo, DIB_RGB_COLORS);

	size_t sizeBmp = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (bpmInfo.bmiHeader.biClrUsed * sizeof(RGBQUAD)) + (bmpHdr.bmWidth * 4L * bmpHdr.bmHeight);
	buffer.resize((size_t)sizeBmp);
	CopyMemory(&buffer[0], &hdr, sizeof(BITMAPFILEHEADER));
	CopyMemory(&buffer[0 + sizeof(BITMAPFILEHEADER)], &bpmInfo, sizeof(BITMAPINFOHEADER) + bpmInfo.bmiHeader.biClrUsed * sizeof(RGBQUAD));
	size_t sizeHeader = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (bpmInfo.bmiHeader.biClrUsed * sizeof(RGBQUAD));
	CopyMemory(&buffer[sizeHeader], lpBits, (bmpHdr.bmWidth * 4L * bmpHdr.bmHeight));


	int noPixX = GetDeviceCaps(dc, LOGPIXELSX);
	int noPixY = GetDeviceCaps(dc, LOGPIXELSY);

	long val = (long)(bmpWidth * 914400L) / noPixX;
	szBmp->cx = val;
	val = (long)(bmpHeight * 914400L) / noPixY;
	szBmp->cy = val;

	//// Write BMP File
	//wstring fileName = _T("C:\\Recorder\\screen.bmp");

	//// A file is created, this is where we will save the screen capture.
	//HANDLE hFile = CreateFile(fileName.c_str(),
	//	GENERIC_WRITE,
	//	0,
	//	NULL,
	//	CREATE_ALWAYS,
	//	FILE_ATTRIBUTE_NORMAL, NULL);

	//// Add the size of the headers to the size of the bitmap to get the total file size

	//DWORD dwBytesWritten = 0;
	//WriteFile(hFile, (LPSTR)&buffer[0], sizeBmp, &dwBytesWritten, NULL);
	////Close the handle for the file that was created
	//CloseHandle(hFile);


	GlobalFree((HGLOBAL)lpBits);
	DeleteObject(backbmp);
	DeleteObject(hFont);
	DeleteDC(dc);
	DeleteDC(backhdc);
}


void BPMNDiagram::DrawDiagram(HDC hdc, int width, int height, POINT offset)
{
	vector <vector <struct cellDef *> *>::iterator itColumn = diagram.begin();
	vector <struct headerDef *>::iterator itColHdr = columnHeader.begin();
	RECT windowRect;
	windowRect.top = windowRect.left = 0;
	windowRect.right = width;
	windowRect.bottom = height;

	HBRUSH hBrush = CreateSolidBrush(RGB(107, 164, 206));
	HBRUSH hBrushGrey = CreateSolidBrush(RGB(128, 128, 128));
	HBRUSH hSyncBrush = CreateSolidBrush(RGB(0, 200, 0));
	HBRUSH hSegBrush = CreateSolidBrush(RGB(200, 0, 0));

	HBRUSH oBrush;

	notInPathList.clear();
	inPathList.clear();
	while (itColumn != diagram.end())
	{
		//int columnWidth = (*itColHdr)->size;
		vector <struct cellDef *> *rows = *itColumn;
		if (rows->size() > 0)
		{
			vector <struct cellDef *>::iterator itRow = rows->begin();
			vector <struct headerDef *>::iterator itRowHdr = rowHeader.begin();

			while (itRow != rows->end())
			{
				struct cellDef *pCell = *itRow;
				BPMNElement *pElement = pCell->bpmnElement;
				if (pElement != NULL)
				{
					RECT rect;
					pElement->GetDisplayPosition(&rect);
					OffsetRect(&rect, offset.x, offset.y);
					if (IsElementPartialInWindow(pElement, &windowRect, &rect))
					{
						if (pCell->bSelected)
							oBrush = (HBRUSH)SelectObject(hdc, hBrushGrey);
						else if (pCell->bpmnElement->IsInSegment())
							oBrush = (HBRUSH)SelectObject(hdc, hSegBrush);
						else if (pCell->bpmnElement->IsSynchronized())
							oBrush = (HBRUSH)SelectObject(hdc, hSyncBrush);
						else
							oBrush = (HBRUSH)SelectObject(hdc, hBrush);
						POINT ptTopLeft;
						ptTopLeft.x = rect.left;
						ptTopLeft.y = rect.top;
						pElement->DrawElement(hdc, ptTopLeft);
						SelectObject(hdc, oBrush);
					}

					list <struct cellDef *>::iterator itConnections = pCell->to.begin();
					while (itConnections != pCell->to.end())
					{
						struct cellDef *pCellTo = *itConnections;
						BPMNElement *pElementTo = pCellTo->bpmnElement;
						RECT rectFrom, rectTo;
						pElement->GetDisplayPosition(&rectFrom);
						OffsetRect(&rectFrom, offset.x, offset.y);
						pElementTo->GetDisplayPosition(&rectTo);
						OffsetRect(&rectTo, offset.x, offset.y);

						COLORREF connectionColor = RGB(255, 0, 0);

						if (pElement->IsInPath() && pElementTo->IsInPath())
							connectionColor = RGB(0, 255, 0);
						else if (pElement->GetType() == 1 && pElementTo->IsInPath())
							connectionColor = RGB(0, 255, 0);
						else if (pElement->IsInPath() && pElementTo->GetType() == 5)
							connectionColor = RGB(0, 255, 0);
						else if ((pElement->IsInPath() || pElement->GetType() == 1) && pElementTo->GetType() == 3)
							connectionColor = RGB(0, 255, 0);
						else if (pElement->GetType() == 3 && pElementTo->IsInPath())
							connectionColor = RGB(0, 255, 0);

						LineConnectionInfo *pConnectionInfo = DBG_NEW LineConnectionInfo();
						pConnectionInfo->SetFromRect(&rectFrom);
						pConnectionInfo->SetToRect(&rectTo);
						pConnectionInfo->SetWndRect(&windowRect);
						if (connectionColor == RGB(0, 255, 0))
						{
							inPathList.push_back(pConnectionInfo);
						}
						else
						{
							notInPathList.push_back(pConnectionInfo);
						}
						itConnections++;
					}
				}

				itRow++;
				itRowHdr++;
			}
		}
		itColHdr++;
		itColumn++;
	}

	if (notInPathList.size() > 0)
	{
		vector <LineConnectionInfo *>::iterator itConnection = notInPathList.begin();
		HPEN nPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
		HPEN oPen = (HPEN)SelectObject(hdc, nPen);
		HBRUSH nBrush = CreateSolidBrush(RGB(255, 0, 0));
		oBrush = (HBRUSH)SelectObject(hdc, nBrush);
		while (itConnection != notInPathList.end())
		{
			LineConnectionInfo *pConnectionInfo = *itConnection;
			DrawConnectingLines(hdc, pConnectionInfo->GetFromRect(), pConnectionInfo->GetToRect(), pConnectionInfo->GetWndRect());
			itConnection++;
			delete pConnectionInfo;
		}
		SelectObject(hdc, oPen);
		SelectObject(hdc, oBrush);
		DeleteObject(nPen);
		DeleteObject(nBrush);
	}

	if (inPathList.size() > 0)
	{
		vector <LineConnectionInfo *>::iterator itConnection = inPathList.begin();
		HPEN nPen = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
		HPEN oPen = (HPEN)SelectObject(hdc, nPen);
		HBRUSH nBrush = CreateSolidBrush(RGB(0, 255, 0));
		oBrush = (HBRUSH)SelectObject(hdc, nBrush);
		while (itConnection != inPathList.end())
		{
			LineConnectionInfo *pConnectionInfo = *itConnection;
			DrawConnectingLines(hdc, pConnectionInfo->GetFromRect(), pConnectionInfo->GetToRect(), pConnectionInfo->GetWndRect());
			itConnection++;
			delete pConnectionInfo;
		}
		SelectObject(hdc, oPen);
		SelectObject(hdc, oBrush);
		DeleteObject(nPen);
		DeleteObject(nBrush);
	}

	inPathList.clear();
	notInPathList.clear();
	DeleteObject(hBrush);
	DeleteObject(hBrushGrey);
	DeleteObject(hSyncBrush);
	DeleteObject(hSegBrush);
}

void BPMNDiagram::DrawDiagramForPrint(HDC hdc, int width, int height, POINT offset)
{
	vector <vector <struct cellDef *> *>::iterator itColumn = diagram.begin();
	vector <struct headerDef *>::iterator itColHdr = columnHeader.begin();
	RECT windowRect;
	windowRect.top = windowRect.left = 0;
	windowRect.right = width;
	windowRect.bottom = height;

	HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
	HBRUSH oBrush;
	

	while (itColumn != diagram.end())
	{
		//int columnWidth = (*itColHdr)->size;
		vector <struct cellDef *> *rows = *itColumn;
		if (rows->size() > 0)
		{
			vector <struct cellDef *>::iterator itRow = rows->begin();
			vector <struct headerDef *>::iterator itRowHdr = rowHeader.begin();

			while (itRow != rows->end())
			{
				struct cellDef *pCell = *itRow;
				BPMNElement *pElement = pCell->bpmnElement;
				if (pElement != NULL)
				{
					RECT rect;
					pElement->GetDisplayPosition(&rect);
					OffsetRect(&rect, offset.x, offset.y);
					oBrush = (HBRUSH)SelectObject(hdc, hBrush);
					POINT ptTopLeft;
					ptTopLeft.x = rect.left;
					ptTopLeft.y = rect.top;
					pElement->DrawElement(hdc, ptTopLeft);
					SelectObject(hdc, oBrush);

					list <struct cellDef *>::iterator itConnections = pCell->to.begin();
					while (itConnections != pCell->to.end())
					{
						struct cellDef *pCellTo = *itConnections;
						BPMNElement *pElementTo = pCellTo->bpmnElement;
						RECT rectFrom, rectTo;
						pElement->GetDisplayPosition(&rectFrom);
						OffsetRect(&rectFrom, offset.x, offset.y);
						pElementTo->GetDisplayPosition(&rectTo);
						OffsetRect(&rectTo, offset.x, offset.y);
						COLORREF connectionColor = RGB(0, 0, 0);
						HPEN nPen = CreatePen(PS_SOLID, 1, connectionColor);
						HPEN oPen = (HPEN)SelectObject(hdc, nPen);
						HBRUSH nBrush = CreateSolidBrush(connectionColor);
						HBRUSH oConnBrush;

						oConnBrush = (HBRUSH)SelectObject(hdc, nBrush);
						DrawConnectingLines(hdc, &rectFrom, &rectTo, &windowRect);

						SelectObject(hdc, oPen);
						SelectObject(hdc, oConnBrush);
						DeleteObject(nPen);
						DeleteObject(nBrush);

						itConnections++;
					}
					SelectObject(hdc, oBrush);
				}

				itRow++;
				itRowHdr++;
			}
		}
		itColHdr++;
		itColumn++;
	}

	DeleteObject(hBrush);
}


void BPMNDiagram::DrawConnectingLines(HDC hdc, LPRECT rectFrom, LPRECT rectTo, LPRECT wndRect)
{
	//if (!RectInRect(wndRect, rectFrom) && RectInRect(wndRect, rectTo))
	//	return;

	POINT pt[4];
	if (rectTo->left > rectFrom->right && rectTo->top > rectFrom->bottom)
	{
		// Draw line from middle of right side to middle of top side
		
		POINT ptStart, ptEnd, ptMid;
		ptStart.x = rectFrom->right;
		ptStart.y = rectFrom->top + (rectFrom->bottom - rectFrom->top) / 2;

		ptMid.x = rectTo->left + (rectTo->right - rectTo->left) / 2;
		ptMid.y = rectFrom->top + (rectFrom->bottom - rectFrom->top) / 2;

		ptEnd.x = rectTo->left + (rectTo->right - rectTo->left) / 2;
		ptEnd.y = rectTo->top;
		//bool inWindow = false;
		//if (rectFrom->right < wndRect->left && rectTo->left > wndRect->right && ((rectFrom->top || rectTo->top) > wndRect->top) && (rectFrom->bottom || rectTo->bottom) < wndRect->bottom)
		//	inWindow = true;

		

		//if (PtInRect(wndRect, ptStart) || PtInRect(wndRect, ptEnd) || PtInRect(wndRect, ptMid))
		//{
			MoveToEx(hdc, ptStart.x, -ptStart.y, NULL);
			LineTo(hdc, ptEnd.x, -ptStart.y);
			LineTo(hdc, ptEnd.x, -ptEnd.y);

			pt[0].x = rectTo->left + (rectTo->right - rectTo->left) / 2;
			pt[0].y = -rectTo->top;

			pt[1].x = pt[0].x - 100;
			pt[1].y = pt[0].y + 100;

			pt[2].x = pt[1].x + 200;
			pt[2].y = pt[1].y;

			Polygon(hdc, pt, 3);
		//}
	}
	else if (rectTo->right < rectFrom->left && rectTo->top > rectFrom->bottom)
	{
		POINT ptStart, ptEnd, ptMid;
		ptStart.x = rectFrom->left + (rectFrom->right - rectFrom->left) / 2;
		ptStart.y = rectFrom->bottom;

		ptMid.x = rectFrom->left + (rectFrom->right - rectFrom->left) / 2;
		ptMid.y = rectTo->top - 300;

		ptEnd.x = rectTo->right - (rectTo->right - rectTo->left)/2;
		ptEnd.y = ptMid.y;

		//if (PtInRect(wndRect, ptStart) || PtInRect(wndRect, ptEnd) || PtInRect(wndRect, ptMid))
		//{
			MoveToEx(hdc,ptStart.x , -ptStart.y, NULL);
			LineTo(hdc, ptMid.x, -ptMid.y);
			LineTo(hdc, ptEnd.x, -ptEnd.y);

			pt[0].x = ptEnd.x;
			pt[0].y = -ptMid.y;

			pt[1].x = pt[0].x + 100;
			pt[1].y = pt[0].y + 100;

			pt[2].x = pt[1].x;
			pt[2].y = pt[1].y - 200;

			Polygon(hdc, pt, 3);
//		}
	}
	else if (rectTo->top > rectFrom->bottom)
	{
		POINT ptStart, ptEnd;
		ptStart.x = rectFrom->left + (rectFrom->right - rectFrom->left) / 2;
		ptStart.y = rectFrom->bottom;

		ptEnd.x = rectFrom->left + (rectFrom->right - rectFrom->left) / 2;
		ptEnd.y = rectTo->top;

		//if (PtInRect(wndRect, ptStart) || PtInRect(wndRect, ptEnd) || 
		//	(ptStart.x >= wndRect->left && ptStart.x <= wndRect->right && ptStart.y < wndRect->top && ptEnd.y > wndRect->bottom))
		//{
			MoveToEx(hdc, ptStart.x, -ptStart.y, NULL);
			LineTo(hdc, ptEnd.x, -ptEnd.y);

			pt[0].x = rectTo->left + (rectTo->right - rectTo->left) / 2;
			pt[0].y = -rectTo->top;

			pt[1].x = pt[0].x - 100;
			pt[1].y = pt[0].y + 100;

			pt[2].x = pt[1].x + 200;
			pt[2].y = pt[1].y;

			Polygon(hdc, pt, 3);
//		}
	}
}
void BPMNDiagram::FindDiagramDimensions(list <BPMNElement *> *pElementList)
{
	int row = -1;
	int col = -1;
	int maxCol = -1;

	FindElementListRowAndColumn(pElementList, &row, &col, &maxCol);

	// Add column header structure
	for (int colNum = 0; colNum <= maxCol; colNum++)
	{
		struct headerDef *pHeader = DBG_NEW struct headerDef;
		pHeader->num = colNum;
		pHeader->size = 0;
		columnHeader.push_back(pHeader);
	}

	// Add row header structure
	for (int rowNum = 0; rowNum <= row; rowNum++)
	{
		struct headerDef *pHeader = DBG_NEW struct headerDef;
		pHeader->num = rowNum;
		pHeader->size = 0;
		rowHeader.push_back(pHeader);
	}

	// Create Diagram cells with empty details
	for (int colNum = 0; colNum <= maxCol; colNum++)
	{
		AddColumn();
		vector<struct cellDef *> *pColumn = diagram[colNum];

		for (int rowNum = 0; rowNum <= row; rowNum++)
		{
			struct cellDef *pCell = DBG_NEW struct cellDef;
			pCell->bpmnElement = NULL;
			pCell->bSelected = false;
			pCell->row = rowHeader[rowNum];
			pCell->col = columnHeader[colNum];
			pColumn->push_back(pCell);
		}
	}

	row = -1;
	col = -1;
	maxCol = -1;

	SetElementsInDiagram(pElementList, &row, &col, &maxCol, NULL, NULL);
}

bool BPMNDiagram::IsSyncElementDisplayed()
{
	if (pSyncElement == NULL)
		return true;

	vector <vector <struct cellDef *> *>::iterator itColumn = diagram.begin();

	while (itColumn != diagram.end())
	{
		vector <struct cellDef *> *rows = *itColumn;
		vector <struct cellDef *>::iterator itRow = rows->begin();

		while (itRow != rows->end())
		{
			struct cellDef *pCell = *itRow;

			if (pCell->bpmnElement != NULL && pCell->bpmnElement == pSyncElement)
				return true;
			itRow++;
		}
		itColumn++;
	}

	return false;

}

void BPMNDiagram::SetSyncElement()
{
	if (pSyncElement != NULL)
		pSyncElement->SetSyncElement(true);
}

struct cellDef *BPMNDiagram::SetElementsInDiagram(list <BPMNElement *> *pElementList, int *pRow, int *pCol, int *maxCol, struct cellDef *pCellPrev, list <struct cellDef *> *pBranchEndElements)
{
	list <BPMNElement *>::iterator itElement = pElementList->begin();
	static list <struct cellDef *> branchList;
	list <struct cellDef *> branchEndList;
	struct cellDef *rightBottomEvent = NULL;
	branchEndList.clear();
	*pCol = *pCol + 1;
	if (*pCol > *maxCol)
		*maxCol = *pCol;

	while (itElement != pElementList->end())
	{
		BPMNElement *pElement = *itElement;
		*pRow = *pRow + 1; // add row for new element

		vector<struct cellDef *> *pColumn = diagram[*pCol];
		
		struct cellDef *pCell = pColumn->at(*pRow);
		pCell->bpmnElement = pElement;
		if (pCellPrev != NULL)
		{
			pCell->from.push_back(pCellPrev);
			pCellPrev->to.push_back(pCell);
		}
		if (branchEndList.size() > 0)
		{
			list <struct cellDef *>::iterator itBranchEnd = branchEndList.begin();
			while (itBranchEnd != branchEndList.end())
			{
				struct cellDef *pBranchEnd = *itBranchEnd;
				pCell->from.push_back(pBranchEnd);
				pBranchEnd->to.push_back(pCell);
				itBranchEnd++;
			}
			branchEndList.clear();
		}

		pCellPrev = pCell;

		if (*pCol > 0)
		{
			// if in a branch then add to branch list
			branchList.push_back(pCell);
		}
		else
		{
			// set the size for the first column
			struct headerDef *pHeader = rowHeader.at(*pRow);
			int size = pElement->GetHeight();
			if (size > (int)pHeader->size)
				pHeader->size = size;
			pHeader = columnHeader.at(*pCol);
			size = pElement->GetWidth();
			if (size > (int)pHeader->size)
				pHeader->size = size;

		}
		if (pElement->GetType() == 4)
		{
			int branchRow;
			branchRow = *pRow;
			BPMNDecision *pDecision = (BPMNDecision *)pElement;
			list <BPMNElement *> *pBranchList = NULL;
			UpdateBranchCells(&branchList, *pCol+1);
			pBranchList = pDecision->GetBranchRightElements();
			rightBottomEvent = SetElementsInDiagram(pBranchList, pRow, pCol, maxCol, pCellPrev, &branchEndList);
			branchEndList.push_back(rightBottomEvent);
			*pCol = *pCol - 2;
			pBranchList = pDecision->GetBranchBottomElements();
			pCellPrev = SetElementsInDiagram(pBranchList, &branchRow, pCol, maxCol, pCellPrev, &branchEndList);
			branchEndList.push_back(pCellPrev);
			// Branch row is used to make sure that both the legs of the branch are the same size
			if (*pRow < branchRow)
				*pRow = branchRow;
			if (*pCol == 0)
			{
				UpdateBranchCellSizes(&branchList);
				branchList.clear();
			}
		}
		itElement++;
	}

	// now copy all unprocessed branch ends to calling function
	if (branchEndList.size() > 0)
	{
		if (pBranchEndElements != NULL)
		{
			list <struct cellDef *>::iterator itBranchEnd = branchEndList.begin();
			while (itBranchEnd != branchEndList.end())
			{
				struct cellDef *pBranchEnd = *itBranchEnd;
				pBranchEndElements->push_back(pBranchEnd);
				itBranchEnd++;
			}
			branchEndList.clear();
		}
	}

	if (pBranchEndElements != NULL)
	{
		pBranchEndElements->push_back(pCellPrev);
	}

	return pCellPrev;
}

void BPMNDiagram::UpdateBranchCells(list <struct cellDef *> *pBranchElementList, int startCol)
{
	list <struct cellDef *>::iterator itCell = pBranchElementList->begin();
	// Move all cell column headers from startCol onwards one column to the right 
	while (itCell != pBranchElementList->end())
	{
		struct cellDef *pCell = *itCell;
		struct headerDef *pHeader = pCell->col;
		int colNum = pHeader->num;
		if (colNum >= startCol)
		{
			pCell->col = columnHeader[colNum + 1];
		}
		itCell++;
	}

}

void BPMNDiagram::UpdateBranchCellSizes(list <struct cellDef *> *pBranchElementList)
{
	list <struct cellDef *>::iterator itCell = pBranchElementList->begin();
	// Update Row and Column Header sizes for all the branch sizes
	while (itCell != pBranchElementList->end())
	{
		struct cellDef *pCell = *itCell;
		BPMNElement *pElement = pCell->bpmnElement;
		if (pElement != NULL)
		{
			struct headerDef *pHeader = pCell->row;
			int size = pElement->GetHeight();
			if (size > (int)pHeader->size)
				pHeader->size = size;
			pHeader = pCell->col;
			size = pElement->GetWidth();
			if (size > (int)pHeader->size)
				pHeader->size = size;

		}
		itCell++;
	}
}



void BPMNDiagram::FindElementListRowAndColumn(list <BPMNElement *> *pElementList, int *pRow, int *pCol, int *maxCol)
{
	list <BPMNElement *>::iterator itElement = pElementList->begin();

	*pCol = *pCol + 1;
	if (*pCol > *maxCol)
		*maxCol = *pCol;
	while (itElement != pElementList->end())
	{
		BPMNElement *pElement = *itElement;

		*pRow = *pRow + 1; // add row for new element
		if (pElement->GetType() == 4)
		{
			int branchRow;
			branchRow = *pRow;
			BPMNDecision *pDecision = (BPMNDecision *)pElement;
			list <BPMNElement *> *pBranchList = NULL;
			pBranchList = pDecision->GetBranchRightElements();
			FindElementListRowAndColumn(pBranchList, pRow, pCol, maxCol);
			*pCol = *pCol - 2;
			pBranchList = pDecision->GetBranchBottomElements();
			FindElementListRowAndColumn(pBranchList, &branchRow, pCol, maxCol);
			// Branch row is used to make sure that both the legs of the branch are the same size
			if (*pRow < branchRow)
				*pRow = branchRow;

		}
		itElement++;
	}
}

void BPMNDiagram::Clear()
{
	vector<vector<struct cellDef *> *>::iterator it;

	it = diagram.begin();

	while (it != diagram.end())
	{
		vector<struct cellDef *> *pColumnCells = *it;
		vector<struct cellDef *>::iterator itRows = pColumnCells->begin();
		while (itRows != pColumnCells->end())
		{
			struct cellDef *pCell = *itRows;
			delete pCell;
			itRows++;
		}
		pColumnCells->clear();
		delete pColumnCells;
		it++;
	}

	diagram.clear();

	vector<struct headerDef *>::iterator itHeader = columnHeader.begin();
	while (itHeader != columnHeader.end())
	{
		struct headerDef *pHeader = *itHeader;
		delete pHeader;
		itHeader++;
	}
	columnHeader.clear();

	itHeader = rowHeader.begin();
	while (itHeader != rowHeader.end())
	{
		struct headerDef *pHeader = *itHeader;
		delete pHeader;
		itHeader++;
	}
	rowHeader.clear();
	selectedList.clear();
	//SetCurrentSyncronizedElement(NULL);
}

bool BPMNDiagram::IsItemSelected()
{
	if (selectedList.size() > 0)
		return true;
	return false;
}

bool BPMNDiagram::IsInSubprocess()
{
	struct cellDef *pCell = FindCellInDiagram(0, 1); // Find the first cell after the start element in the diagram
	if (pCell == NULL)
		return false;
	BPMNElement *pElement = pCell->bpmnElement;

	if (pElement == NULL)
		return false;
	BPMNElement *pParent = pElement->GetParentElement();
	if (pParent != NULL && pParent->GetType() == 3)
		return true;
	return false;
}

bool BPMNDiagram::IsSubProcessAboveSelected()
{
	struct cellDef *pCell = selectedList.front();
	if (pCell != NULL && pCell->from.size() > 0)
	{
		struct cellDef *pCellTop = pCell->from.front();
		if (pCellTop != NULL)
		{
			BPMNElement *pElement = pCellTop->bpmnElement;
			if (pElement != NULL && pElement->GetType() == 3)
				return true;
		}
	}
	return false;

}

bool BPMNDiagram::IsSubProcessSelected()
{
	if (selectedList.size() != 1)
		return false;
	struct cellDef *pCell = selectedList.front();
	if (pCell != NULL)
	{
		BPMNElement *pElement = pCell->bpmnElement;
		if (pElement != NULL && pElement->GetType() == 3)
			return true;
	}
	return false;
}

bool BPMNDiagram::IsSubProcessBelowSelected()
{
	struct cellDef *pCell = selectedList.back();
	if (pCell != NULL && pCell->from.size() > 0)
	{
		struct cellDef *pCellTop = pCell->to.front();
		if (pCellTop != NULL)
		{
			BPMNElement *pElement = pCellTop->bpmnElement;
			if (pElement != NULL && pElement->GetType() == 3)
				return true;
		}
	}
	return false;
}

bool BPMNDiagram::IsSelectedInSubProcess()
{
	struct cellDef *pCell = selectedList.front();
	BPMNElement *pElement = pCell->bpmnElement;

	if (pElement == NULL)
		return false;
	BPMNElement *pParent = pElement->GetParentElement();
	if (pParent != NULL && pParent->GetType() == 3)
		return true;
	return false;
}

bool BPMNDiagram::IsOnTopOfList()
{
	struct cellDef *pCell = selectedList.front();
	if (pCell != NULL && pCell->from.size() > 0)
	{
		struct cellDef *pCellTop = pCell->from.front();
		if (pCellTop != NULL)
		{
			BPMNElement *pElement = pCellTop->bpmnElement;
			if (pElement != NULL && pElement->GetType() == 1)
				return true;
		}
	}
	return false;
}

bool BPMNDiagram::IsOnBottomOfList()
{
	struct cellDef *pCell = selectedList.back();
	if (pCell != NULL && pCell->to.size() > 0)
	{
		struct cellDef *pCellBottom = pCell->to.front();
		if (pCellBottom != NULL)
		{
			BPMNElement *pElement = pCellBottom->bpmnElement;
			if (pElement != NULL && pElement->GetType() == 5)
				return true;
		}
	}
	return false;
}


bool BPMNDiagram::IsOneItemSelected()
{
	if (selectedList.size() == 1)
		return true;
	return false;
}

int BPMNDiagram::GetNoOfItems()
{
	return 0;
}

bool BPMNDiagram::IsPlayerInPlayMode()
{
	return pPlayerManager->GetRecorderStatus();
}

bool BPMNDiagram::IsBranchSelected()
{
	if (IsOneItemSelected())
	{
		struct cellDef *pCell = selectedList.front();
		BPMNElement *pElement = pCell->bpmnElement;
		if (pElement == NULL)
			return false;
		if (pElement->GetType() == 4)
			return true;

	}
	return false;
}

BPMNDecision *BPMNDiagram::GetSelectedBranch()
{
	if (IsBranchSelected())
	{
		struct cellDef *pCell = selectedList.front();
		return (BPMNDecision *)pCell->bpmnElement;
	}
	return NULL;
}

