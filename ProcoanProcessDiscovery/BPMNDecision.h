#pragma once
#include "BPMNElement.h"
#include <list>

using namespace std;

class BPMNDecision :
	public BPMNElement
{
	list <BPMNElement *> branchRightList;
	list <BPMNElement *> branchBottomList;
	int branchRightChannel;
	int branchBottomChannel;

public:
	BPMNDecision();
	BPMNDecision(ULONG index);
	~BPMNDecision();
	list <BPMNElement *> * GetBranchRightElements();
	list <BPMNElement *> * GetBranchBottomElements();
	BPMNElement *GetLastRightElement();
	BPMNElement *GetLastBottomElement();
	BPMNElement *GetFirstRightElement();
	BPMNElement *GetFirstBottomElement();

	void AddRightElement(BPMNElement *pElement);
	void AddBottomElement(BPMNElement *pElement);
	void AddRightBranchList(list <BPMNElement *> elementList);
	void AddBottomBranchList(list <BPMNElement *> elementList);

	void DeleteElement(BPMNElement * pElement);
	void DeleteElementList(list <BPMNElement *> elementList);
	virtual void GetControlDimension(HDC hDC);
	void SetElementIndex(ULONG index);
	virtual int GetType();
	virtual void DrawElement(HDC hDC, POINT offset);
	void SetBranchRightChannel(int nChannel);
	int GetBranchRightChannel();
	void SetBranchBottomChannel(int nChannel);
	int GetBranchBottomChannel();
	list <BPMNElement *> *GetDecisionElementList(BPMNElement *pElement);
	int GetElementRecordType(BPMNElement *pElement);
	bool IsInRightBranchList(BPMNElement *pElement);
	bool IsInBottomBranchList(BPMNElement *pElement);
};

