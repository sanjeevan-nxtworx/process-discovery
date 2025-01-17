#include "stdafx.h"
#include "BPMNStop.h"



BPMNStop::BPMNStop()
{
}


BPMNStop::~BPMNStop()
{
}

int BPMNStop::GetType()
{
	return 5;
}

void BPMNStop::DrawElement(HDC hDC, POINT drawPos)
{
	HPEN hPen = CreatePen(PS_SOLID, 50, RGB(0, 0, 0));
	HPEN hPen2 = CreatePen(PS_SOLID, 20, RGB(0, 0, 0));
	HPEN oPen = (HPEN)SelectObject(hDC, hPen);
	int xLeft = drawPos.x;
	int xRight = xLeft + szElement.cx;
	int yTop = -drawPos.y;
	int yBottom = yTop - szElement.cy;

	Ellipse(hDC, xLeft, yTop,
		xRight, yBottom);

	hPen = (HPEN)SelectObject(hDC, hPen2);
	HBRUSH hBrush = CreateSolidBrush(RGB(127, 127, 127));
	HBRUSH oBrush = (HBRUSH)SelectObject(hDC, hBrush);
	Ellipse(hDC, xLeft + 100, yTop - 100, xRight - 100, yBottom + 100);
	SelectObject(hDC, oBrush);
	SelectObject(hDC, oPen);

	DeleteObject(hPen2);
	DeleteObject(hPen);
	DeleteObject(hBrush);
}

