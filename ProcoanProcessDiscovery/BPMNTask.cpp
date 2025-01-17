#include "stdafx.h"
#include "BPMNTask.h"

BPMNTask::BPMNTask(long nEventIndexVal, long long timestamp): BPMNElement(nEventIndexVal, timestamp)
{
}


BPMNTask::~BPMNTask()
{
}

int BPMNTask::GetType()
{
	return 2;
}

void BPMNTask::DrawElement(HDC hDC, POINT drawPos)
{
	HPEN hPen = CreatePen(PS_SOLID, 50, RGB(0, 0, 0));
	HPEN oPen = (HPEN)SelectObject(hDC, hPen);
	int xLeft = drawPos.x;
	int xRight = xLeft + szElement.cx;
	int yTop = -drawPos.y;
	int yBottom = yTop - szElement.cy;

	RECT rect;
	rect.left = xLeft + 200;
	rect.right = xRight - 200;
	rect.top = -drawPos.y - 200;
	rect.bottom = yTop - szElement.cy  - 200;
	
	string activityType = GetKeyValueAsString("Group");
	//activityType = "";
	HBRUSH oBrush = (HBRUSH)0;
	HBRUSH hBrushManual = (HBRUSH)0;
	if (activityType == "Manual")
	{
		hBrushManual = CreateSolidBrush(RGB(0x73, 0xc2, 0xfb));
		oBrush = (HBRUSH)SelectObject(hDC, hBrushManual);
	}
	else
	if (activityType == "Cognitive")
	{
		hBrushManual = CreateSolidBrush(RGB(0x99, 0xba, 0xdd));
		oBrush = (HBRUSH)SelectObject(hDC, hBrushManual);
	}
	else
	if (activityType == "Intermediate")
	{
		hBrushManual = CreateSolidBrush(RGB(0x00, 0xbf, 0xff));
		oBrush = (HBRUSH)SelectObject(hDC, hBrushManual);
	}

	xLeft -= 250;
	xRight += 350;
	RoundRect(hDC, xLeft, yTop,
		xRight, yBottom,
		300, 300);

	RECT mRect;
	mRect.left = xRight - 500;
	mRect.top = yTop - 50;
	mRect.right = xRight - 50;
	mRect.bottom = yTop - 500;

	if (activityType == "Manual")
	{
		Ellipse(hDC, mRect.left, mRect.top, mRect.right, mRect.bottom);
		DrawText(hDC, L"M", -1, &mRect, DT_CENTER);
	}
	else if (activityType == "Intermediate")
	{
		Ellipse(hDC, mRect.left, mRect.top, mRect.right, mRect.bottom);
		DrawText(hDC, L"I", -1, &mRect, DT_CENTER);
	}
	else if (activityType == "Cognitive")
	{
		Ellipse(hDC, mRect.left, mRect.top, mRect.right, mRect.bottom);
		DrawText(hDC, L"C", -1, &mRect, DT_CENTER);
	}

	wstring wideString = wstring(strAnnotation.begin(), strAnnotation.end());
	DrawText(hDC, wideString.c_str(), -1, &rect, DT_CENTER | DT_WORDBREAK);
	//int topY = ((yBottom - yTop) - (rect.bottom - rect.top)) / 2;
	//rect.left = xLeft + 5;
	//rect.right = xRight - 5;
	//rect.top = yTop + topY;
	//rect.bottom = yBottom - 5;
	//DrawText(hDC, wideString.c_str(), -1, &rect,  DT_CENTER | DT_WORDBREAK);
	SelectObject(hDC, oPen);
	if (activityType == "Manual" || activityType == "Intermediate" || activityType == "Cognitive")
	{
		SelectObject(hDC, oBrush);
		DeleteObject(hBrushManual);
	}

	DeleteObject(hPen);
}

