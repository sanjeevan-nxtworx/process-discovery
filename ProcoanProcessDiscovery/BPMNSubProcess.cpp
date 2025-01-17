#include "stdafx.h"
#include "BPMNSubProcess.h"
#include "BPMNStart.h"
#include "BPMNStop.h"



BPMNSubProcess::BPMNSubProcess(ULONG nIndex)
{
	nEventIndex = nIndex;
	subProcessElements.clear();
}

BPMNSubProcess::~BPMNSubProcess()
{
	while (subProcessElements.size() > 0)
	{
		BPMNElement *pElement = subProcessElements.front();
		subProcessElements.pop_front();
		delete pElement;
	}
	subProcessElements.clear();
}

int BPMNSubProcess::GetType()
{
	return 3;
}

list <BPMNElement *> *BPMNSubProcess::GetSubProcessElements()
{
	return &subProcessElements;
}

BPMNElement *BPMNSubProcess::GetFirstElement()
{
	BPMNElement *pElement = NULL;
	if (subProcessElements.size() > 0)
	{
		BPMNElement *pStartElement = NULL;
		pStartElement = subProcessElements.front();
		subProcessElements.pop_front();
		pElement = subProcessElements.front();
		subProcessElements.push_front(pStartElement);
	}
	return pElement;
}

BPMNElement *BPMNSubProcess::GetLastElement()
{
	BPMNElement *pElement = NULL;
	if (subProcessElements.size() > 0)
	{
		BPMNElement *pEndElement = NULL;
		pEndElement = subProcessElements.back();
		subProcessElements.pop_back();
		pElement = subProcessElements.back();
		subProcessElements.push_back(pEndElement);
	}
	return pElement;
}


void BPMNSubProcess::AddElementListAbove(list <BPMNElement *> elementList)
{
	if (subProcessElements.size() == 0)
	{
		AddElementList(elementList);
		return;
	}
	BPMNElement *pElementStart = subProcessElements.front();
	subProcessElements.pop_front();
	list <BPMNElement *>::reverse_iterator itFrontElement = elementList.rbegin();
	while (itFrontElement != elementList.rend())
	{
		BPMNElement *pElement = *itFrontElement;
		pElement->SetParentElement((BPMNElement *)this);
		subProcessElements.push_front(pElement);
		itFrontElement++;
	}
	subProcessElements.push_front(pElementStart);
}

void BPMNSubProcess::AddElement(BPMNElement *pNewElement)
{
	BPMNElement *pElement = NULL;
	pNewElement->SetParentElement((BPMNElement *)this);	
	if (subProcessElements.size() == 0)
	{
		pElement = (BPMNElement *) DBG_NEW BPMNStart();
		pElement->SetParentElement((BPMNElement *)this);
		subProcessElements.push_back(pElement);
		subProcessElements.push_back(pNewElement);
		pElement = (BPMNElement *) DBG_NEW BPMNStop();
		subProcessElements.push_back(pElement);
	}
	else
	{
		pElement = subProcessElements.back();
		subProcessElements.pop_back();
		subProcessElements.push_back(pNewElement);
		subProcessElements.push_back(pElement);
	}
}


void BPMNSubProcess::AddElementListBelow(list <BPMNElement *> elementList)
{
	BPMNElement *pElementStop = subProcessElements.back();
	subProcessElements.pop_back();
	list <BPMNElement *>::iterator itBackElement = elementList.begin();
	while (itBackElement != elementList.end())
	{
		BPMNElement *pElement = *itBackElement;
		pElement->SetParentElement((BPMNElement *)this);
		subProcessElements.push_back(pElement);
		itBackElement++;
	}
	subProcessElements.push_back(pElementStop);
}

void BPMNSubProcess::AddElementList(list <BPMNElement *> elementList)
{
	list <BPMNElement *>::iterator itElements = elementList.begin();
	BPMNElement *pElement = NULL;

	if ((*itElements)->GetType() != 1) // if no start element the add one
	{
		pElement = (BPMNElement *) DBG_NEW BPMNStart();
		pElement->SetParentElement((BPMNElement *)this);
		subProcessElements.push_back(pElement);
	}

	while (itElements != elementList.end())
	{
		pElement = *itElements;
		pElement->SetParentElement((BPMNElement *)this);
		subProcessElements.push_back(pElement);
		itElements++;
	}

	if (pElement == NULL || pElement->GetType() != 5) // if no Stop element the add one
	{
		pElement = (BPMNElement *) DBG_NEW BPMNStop();
		pElement->SetParentElement((BPMNElement *)this);
		subProcessElements.push_back(pElement);
	}
}

void BPMNSubProcess::DrawElement(HDC hDC, POINT drawPos)
{
	HPEN hPen = CreatePen(PS_SOLID, 50, RGB(0, 0, 0));
	HPEN hPen2 = CreatePen(PS_SOLID, 20, RGB(0, 0, 0));
	HPEN oPen = (HPEN)SelectObject(hDC, hPen);
	int xLeft = drawPos.x;
	int xRight = xLeft + szElement.cx;
	int yTop = -drawPos.y;
	int yBottom = yTop - szElement.cy;

	RECT rect;
	rect.left = xLeft + 200;
	rect.right = xRight - 200;
	rect.top = -drawPos.y - 200;
	rect.bottom = yTop - szElement.cy - 200;

	RoundRect(hDC, xLeft, yTop,
		xRight, yBottom,
		300, 300);

	wstring wideString = wstring(strAnnotation.begin(), strAnnotation.end());
	DrawText(hDC, wideString.c_str(), -1, &rect, DT_CENTER | DT_WORDBREAK);
	//int topY = ((-yBottom - yTop) - (rect.bottom - rect.top)) / 2;
	//rect.left = xLeft + 5;
	//rect.right = xRight - 5;
	//rect.top = yTop + topY;
	//rect.bottom = yBottom - 5;
	//DrawText(hDC, wideString.c_str(), -1, &rect, DT_CENTER | DT_WORDBREAK);

	HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 0));
	HBRUSH oBrush = (HBRUSH)SelectObject(hDC, hBrush);

	SelectObject(hDC, hPen2);
	rect.left = rect.left + (rect.right - rect.left) / 2 - 200;
	rect.right = rect.left + 400;
	rect.bottom = yBottom;
	rect.top = rect.bottom + 200;
	Rectangle(hDC, rect.left, rect.top, rect.right, rect.bottom);
	
	SelectObject(hDC, oBrush);
	HPEN hCrossPen = CreatePen(PS_SOLID, 2, RGB(255, 255, 255));
	SelectObject(hDC, hCrossPen);
	MoveToEx(hDC, rect.left + 200, rect.top, NULL);
	LineTo(hDC, rect.left + 200, rect.bottom);
	MoveToEx(hDC, rect.left, rect.bottom + 100, NULL);
	LineTo(hDC, rect.right, rect.bottom + 100);

	SelectObject(hDC, oPen);
	DeleteObject(hPen2);
	DeleteObject(hPen);
	DeleteObject(hBrush);
	DeleteObject(hCrossPen);
}