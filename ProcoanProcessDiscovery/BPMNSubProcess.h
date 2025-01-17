#pragma once
#include "BPMNElement.h"
#include <list>
using namespace std;

class BPMNSubProcess :
	public BPMNElement
{
	list <BPMNElement *> subProcessElements;

public:
	BPMNSubProcess(ULONG nEventIndex);
	~BPMNSubProcess();
	list <BPMNElement *> * GetSubProcessElements();
	void AddElementList(list <BPMNElement *> elementList);
	void AddElementListAbove(list <BPMNElement *> elementList);
	void AddElement(BPMNElement *pNewElement);
	void AddElementListBelow(list <BPMNElement *> elementList);
	BPMNElement *GetFirstElement();
	BPMNElement *GetLastElement();
	virtual int GetType();
	virtual void DrawElement(HDC hDC, POINT drawPos);
};

