#include "stdafx.h"
#include "BPMNStart.h"



BPMNStart::BPMNStart()
{
}


BPMNStart::~BPMNStart()
{
}

int BPMNStart::GetType()
{
	return 1;
}

void BPMNStart::DrawElement(HDC hDC, POINT drawPos)
{
	HPEN hPen = CreatePen(PS_SOLID, 50, RGB(0, 0, 0));
	HBRUSH hBrush = CreateSolidBrush(RGB(238,90,46));
	HBRUSH oBrush = (HBRUSH)SelectObject(hDC, hBrush);

	HPEN oPen = (HPEN)SelectObject(hDC, hPen);
	int xLeft = drawPos.x;
	int xRight = xLeft + szElement.cx;
	int yTop = -drawPos.y;
	int yBottom = yTop - szElement.cy;

	Ellipse(hDC, xLeft, yTop,
		xRight, yBottom);

	SelectObject(hDC, oBrush);
	SelectObject(hDC, oPen);
	DeleteObject(hPen);
	DeleteObject(hBrush);
}