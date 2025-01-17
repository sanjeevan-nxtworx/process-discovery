#include "stdafx.h"
#include "BPMNDecision.h"

BPMNDecision::BPMNDecision()
{
	branchRightChannel = 0;
	branchBottomChannel = 0;
}

BPMNDecision::BPMNDecision(ULONG index)
{
	nEventIndex = index;
}

BPMNDecision::~BPMNDecision()
{
}

int BPMNDecision::GetType()
{
	return 4;
}

void BPMNDecision::DrawElement(HDC hDC, POINT drawPos)
{
	HPEN hPen = CreatePen(PS_SOLID, 50, RGB(0, 0, 0));
	HPEN oPen = (HPEN)SelectObject(hDC, hPen);

	POINT pt[4];

	pt[0].x = drawPos.x + szElement.cx / 2;
	pt[0].y = -drawPos.y;

	pt[1].x = drawPos.x + szElement.cx;
	pt[1].y = -drawPos.y - szElement.cy / 2;

	pt[2].x = drawPos.x + szElement.cx / 2;
	pt[2].y = -drawPos.y - szElement.cy;

	pt[3].x = drawPos.x;
	pt[3].y = -drawPos.y - szElement.cy / 2;

	Polygon(hDC, pt, 4);


	SelectObject(hDC, oPen);
	DeleteObject(hPen);

}

list <BPMNElement *> * BPMNDecision::GetBranchRightElements()
{
	return &branchRightList;
}

list <BPMNElement *> * BPMNDecision::GetBranchBottomElements()
{
	return &branchBottomList;
}

BPMNElement *BPMNDecision::GetLastRightElement()
{
	if (branchRightList.size() == 0)
		return NULL;
	return branchRightList.back();
}

BPMNElement *BPMNDecision::GetLastBottomElement()
{
	if (branchBottomList.size() == 0)
		return NULL;
	return branchBottomList.back();
}

BPMNElement *BPMNDecision::GetFirstRightElement()
{
	if (branchRightList.size() == 0)
		return NULL;
	return branchRightList.front();
}

BPMNElement *BPMNDecision::GetFirstBottomElement()
{
	if (branchBottomList.size() == 0)
		return NULL;
	return branchBottomList.front();
}

void BPMNDecision::AddRightElement(BPMNElement *pElement)
{
	pElement->SetParentElement((BPMNElement *)this);
	branchRightList.push_back(pElement);
}

void BPMNDecision::AddBottomElement(BPMNElement *pElement)
{
	pElement->SetParentElement((BPMNElement *)this);
	branchBottomList.push_back(pElement);
}


void BPMNDecision::AddRightBranchList(list <BPMNElement *> elementList)
{
	list <BPMNElement *>::iterator itElements = elementList.begin();
	BPMNElement *pElement = NULL;

	while (itElements != elementList.end())
	{
		pElement = *itElements;
		pElement->SetParentElement((BPMNElement *)this);
		branchRightList.push_back(pElement);
		itElements++;
	}
}

void BPMNDecision::AddBottomBranchList(list <BPMNElement *> elementList)
{
	list <BPMNElement *>::iterator itElements = elementList.begin();
	BPMNElement *pElement = NULL;

	while (itElements != elementList.end())
	{
		pElement = *itElements;
		pElement->SetParentElement((BPMNElement *)this);
		branchBottomList.push_back(pElement);
		itElements++;
	}
}

void BPMNDecision::SetBranchRightChannel(int nChannel)
{
	branchRightChannel = nChannel;
}

int BPMNDecision::GetBranchRightChannel()
{
	return branchRightChannel;
}

void BPMNDecision::SetBranchBottomChannel(int nChannel)
{
	branchBottomChannel = nChannel;
}

int BPMNDecision::GetBranchBottomChannel()
{
	return branchBottomChannel;
}


void BPMNDecision::DeleteElement(BPMNElement * pElement)
{
	// handle unused parameter
	pElement++;
}

int BPMNDecision::GetElementRecordType(BPMNElement *pElement)
{
	int recType = 0;

	list <BPMNElement *>::iterator itFind = find(branchRightList.begin(), branchRightList.end(), pElement);
	if (itFind != branchRightList.end())
		recType = 0x10;

	itFind = find(branchBottomList.begin(), branchBottomList.end(), pElement);
	if (itFind != branchBottomList.end())
		recType = 0x20;

	return recType;

}

bool BPMNDecision::IsInBottomBranchList(BPMNElement *pElement)
{
	list <BPMNElement *>::iterator itFind = find(branchBottomList.begin(), branchBottomList.end(), pElement);
	if (itFind != branchBottomList.end())
		return true;
	return false;
}

bool BPMNDecision::IsInRightBranchList(BPMNElement *pElement)
{
	list <BPMNElement *>::iterator itFind = find(branchRightList.begin(), branchRightList.end(), pElement);
	if (itFind != branchRightList.end())
		return true;
	return false;
}


list <BPMNElement *> *BPMNDecision::GetDecisionElementList(BPMNElement *pElement)
{
	list <BPMNElement *>::iterator itFind = find(branchRightList.begin(), branchRightList.end(), pElement);
	list <BPMNElement *> *pList = NULL;
	if (itFind != branchRightList.end())
		pList = &branchRightList;

	itFind = find(branchBottomList.begin(), branchBottomList.end(), pElement);
	if (itFind != branchBottomList.end())
		pList = &branchBottomList;

	return pList;
}


void BPMNDecision::DeleteElementList(list <BPMNElement *> elementList)
{

}

void BPMNDecision::SetElementIndex(ULONG index)
{
	nEventIndex = index;
}

void BPMNDecision::GetControlDimension(HDC hDC)
{
	szElement.cx = 500;
	szElement.cy = 500;

	list<BPMNElement *>::iterator it;

	it = branchRightList.begin();
	while (it != branchRightList.end())
	{
		BPMNElement *pElement = *it;
		pElement->GetControlDimension(hDC);
		it++;
	}

	it = branchBottomList.begin();
	while (it != branchBottomList.end())
	{
		BPMNElement *pElement = *it;
		pElement->GetControlDimension(hDC);
		it++;
	}

}
