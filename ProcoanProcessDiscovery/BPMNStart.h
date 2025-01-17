#pragma once
#include "BPMNElement.h"
class BPMNStart :
	public BPMNElement
{
public:
	BPMNStart();
	~BPMNStart();
	virtual int GetType();
	virtual void DrawElement(HDC hDC, POINT drawPos);

};

