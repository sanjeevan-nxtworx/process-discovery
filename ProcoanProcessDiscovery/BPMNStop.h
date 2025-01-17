#pragma once
#include "BPMNElement.h"
class BPMNStop :
	public BPMNElement
{
public:
	BPMNStop();
	~BPMNStop();
	virtual int GetType();
	virtual void DrawElement(HDC hDC, POINT drawPos);

};

