#pragma once
#include "BPMNElement.h"
class BPMNTask :
	public BPMNElement
{
public:
	BPMNTask(long nEventIndexVal, long long timestamp);
	~BPMNTask();
	virtual int GetType();
	virtual void DrawElement(HDC hDC, POINT drawPos);
};

