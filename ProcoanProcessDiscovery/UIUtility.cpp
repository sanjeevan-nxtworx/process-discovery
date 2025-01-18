#include "stdafx.h"
#include "UIUtility.h"
#include "wx/graphics.h"
#include <list>
#include <Windows.h>


static BOOL CALLBACK CreateMonitorDataEnum(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor, LPARAM pData);


void DrawLEDString(wxDC& dc, char *text, wxColor color, int left, int top , int thickness, int height, int gap)
{

	HDC hDC = dc.GetHDC();
	int num = 0;
	bool seg1, seg2, seg3, seg4, seg5, seg6, seg7;
	int leftX = left;
	int topY = top;
	//wxColor backColor = RGB(0x29 + 0x10, 0x37+ 0x10, 0x49+0x10);
	COLORREF backColor = RGB(0x29 + 0x10, 0x37 + 0x10, 0x49 + 0x10);
	COLORREF foreColor = RGB(color.Red(), color.Green(), color.Blue());

	HBRUSH oBrush, hBrush, hBrushBack;
	HPEN oPen, hPen, hPenBack;

	hBrush = CreateSolidBrush(foreColor);
	hPen = CreatePen(PS_SOLID, 1, foreColor);
	hBrushBack = CreateSolidBrush(backColor);
	hPenBack = CreatePen(PS_SOLID, 1, backColor);


	while (*text != NULL)
	{
		if (*text == ':')
		{
			num = 10;
			oBrush = (HBRUSH)SelectObject(hDC, hBrush);
			oPen = (HPEN)SelectObject(hDC, hPen);
			int eLeft = leftX + height / 2;
			int eTop = topY + height;

			Ellipse(hDC, eLeft, eTop, eLeft + (height / 2), eTop + (height / 2));
			eTop += height;
			Ellipse(hDC, eLeft, eTop, eLeft + (height/2), eTop + (height / 2));
			SelectObject(hDC, oBrush);
			SelectObject(hDC, oPen);
			leftX += height + height/2;
		}
		else
		{
			num = *text - '0';
			seg1 = seg2 = seg3 = seg4 = seg5 = seg6 = seg7 = false;

			switch (num)
			{
			case 0: seg1 = seg2 = seg4 = seg5 = seg6 = seg3 = true;	break;
			case 1:	seg2 = seg3 = true; break;
			case 2: seg1 = seg2 = seg7 = seg5 = seg4 = true; break;
			case 3: seg1 = seg2 = seg3 = seg4 = seg7 = true; break;
			case 4: seg2 = seg3 = seg6 = seg7 = true;	break;
			case 5: seg1 = seg3 = seg4 = seg7 = seg6 = true; break;
			case 6: seg3 = seg4 = seg5 = seg6 = seg7 = true; break;
			case 7: seg1 = seg2 = seg3 = true; break;
			case 8: seg1 = seg2 = seg3 = seg4 = seg5 = seg6 = seg7 = true; break;
			case 9: seg1 = seg2 = seg3 = seg6 = seg7 = true; break;
			}

			POINT poly[7];
			//int width = 6;
			//int right = 24;

			poly[0].x = leftX + thickness + gap;			poly[0].y = topY + thickness;
			poly[1].x = poly[0].x + thickness;				poly[1].y = poly[0].y - thickness;
			poly[2].x = poly[1].x + height;					poly[2].y = poly[1].y;
			poly[3].x = poly[2].x + thickness;				poly[3].y = poly[2].y + thickness;
			poly[4].x = poly[2].x;							poly[4].y = poly[3].y + thickness;
			poly[5].x = poly[1].x;							poly[5].y = poly[4].y;
			poly[6].x = poly[0].x;						    poly[6].y = poly[0].y;

			if (seg1)
			{
				oBrush = (HBRUSH)SelectObject(hDC, hBrush);
				oPen = (HPEN)SelectObject(hDC, hPen);
			}
			else
			{
				oBrush = (HBRUSH)SelectObject(hDC, hBrushBack);
				oPen = (HPEN)SelectObject(hDC, hPenBack);
			}
			Polygon(hDC, poly, 7);
			SelectObject(hDC, oBrush);
			SelectObject(hDC, oPen);

			poly[0].x = leftX + height + gap + gap + thickness + thickness + thickness;	poly[0].y = topY + thickness;
			poly[1].x = poly[0].x + thickness;											poly[1].y = poly[0].y + thickness;
			poly[2].x = poly[1].x;														poly[2].y = poly[1].y + height;
			poly[3].x = poly[1].x - thickness;											poly[3].y = poly[2].y + thickness;
			poly[4].x = poly[3].x - thickness;											poly[4].y = poly[3].y - thickness;
			poly[5].x = poly[4].x;														poly[5].y = poly[1].y;
			poly[6].x = poly[0].x;														poly[6].y = poly[0].y;

			if (seg2)
			{
				oBrush = (HBRUSH)SelectObject(hDC, hBrush);
				oPen = (HPEN)SelectObject(hDC, hPen);
			}
			else
			{
				oBrush = (HBRUSH)SelectObject(hDC, hBrushBack);
				oPen = (HPEN)SelectObject(hDC, hPenBack);
			}
			Polygon(hDC, poly, 7);
			SelectObject(hDC, oBrush);
			SelectObject(hDC, oPen);

			poly[0].x = leftX + height + gap + gap + thickness + thickness + thickness;	poly[0].y = topY + thickness + height + gap + thickness + thickness;
			poly[1].x = poly[0].x + thickness;											poly[1].y = poly[0].y + thickness;
			poly[2].x = poly[1].x;														poly[2].y = poly[1].y + height;
			poly[3].x = poly[1].x - thickness;											poly[3].y = poly[2].y + thickness;
			poly[4].x = poly[3].x - thickness;											poly[4].y = poly[3].y - thickness;
			poly[5].x = poly[4].x;														poly[5].y = poly[1].y;
			poly[6].x = poly[0].x;														poly[6].y = poly[0].y;

			if (seg3)
			{
				oBrush = (HBRUSH)SelectObject(hDC, hBrush);
				oPen = (HPEN)SelectObject(hDC, hPen);
			}
			else
			{
				oBrush = (HBRUSH)SelectObject(hDC, hBrushBack);
				oPen = (HPEN)SelectObject(hDC, hPenBack);
			}
			Polygon(hDC, poly, 7);
			SelectObject(hDC, oBrush);
			SelectObject(hDC, oPen);

			poly[0].x = leftX + thickness + gap;			poly[0].y = topY + thickness + thickness + height + height + thickness + gap + gap + thickness + thickness;
			poly[1].x = poly[0].x + thickness;				poly[1].y = poly[0].y - thickness;
			poly[2].x = poly[1].x + height;					poly[2].y = poly[1].y;
			poly[3].x = poly[2].x + thickness;				poly[3].y = poly[2].y + thickness;
			poly[4].x = poly[2].x;							poly[4].y = poly[3].y + thickness;
			poly[5].x = poly[1].x;							poly[5].y = poly[4].y;
			poly[6].x = poly[0].x;						    poly[6].y = poly[0].y;

			if (seg4)
			{
				oBrush = (HBRUSH)SelectObject(hDC, hBrush);
				oPen = (HPEN)SelectObject(hDC, hPen);
			}
			else
			{
				oBrush = (HBRUSH)SelectObject(hDC, hBrushBack);
				oPen = (HPEN)SelectObject(hDC, hPenBack);
			}
			Polygon(hDC, poly, 7);
			SelectObject(hDC, oBrush);
			SelectObject(hDC, oPen);

			poly[0].x = leftX + thickness;								poly[0].y = topY + thickness;
			poly[1].x = poly[0].x + thickness;							poly[1].y = poly[0].y + thickness;
			poly[2].x = poly[1].x;										poly[2].y = poly[1].y + height;
			poly[3].x = poly[1].x - thickness;							poly[3].y = poly[2].y + thickness;
			poly[4].x = poly[3].x - thickness;							poly[4].y = poly[3].y - thickness;
			poly[5].x = poly[4].x;										poly[5].y = poly[1].y;
			poly[6].x = poly[0].x;										poly[6].y = poly[0].y;

			if (seg6)
			{
				oBrush = (HBRUSH)SelectObject(hDC, hBrush);
				oPen = (HPEN)SelectObject(hDC, hPen);
			}
			else
			{
				oBrush = (HBRUSH)SelectObject(hDC, hBrushBack);
				oPen = (HPEN)SelectObject(hDC, hPenBack);
			}
			Polygon(hDC, poly, 7);
			SelectObject(hDC, oBrush);
			SelectObject(hDC, oPen);

			poly[0].x = leftX + thickness;								poly[0].y = topY + thickness + height + thickness + gap + thickness;
			poly[1].x = poly[0].x + thickness;							poly[1].y = poly[0].y + thickness;
			poly[2].x = poly[1].x;										poly[2].y = poly[1].y + height;
			poly[3].x = poly[1].x - thickness;							poly[3].y = poly[2].y + thickness;
			poly[4].x = poly[3].x - thickness;							poly[4].y = poly[3].y - thickness;
			poly[5].x = poly[4].x;										poly[5].y = poly[1].y;
			poly[6].x = poly[0].x;										poly[6].y = poly[0].y;

			if (seg5)
			{
				oBrush = (HBRUSH)SelectObject(hDC, hBrush);
				oPen = (HPEN)SelectObject(hDC, hPen);
			}
			else
			{
				oBrush = (HBRUSH)SelectObject(hDC, hBrushBack);
				oPen = (HPEN)SelectObject(hDC, hPenBack);
			}
			Polygon(hDC, poly, 7);
			SelectObject(hDC, oBrush);
			SelectObject(hDC, oPen);

			poly[0].x = leftX + thickness + gap;			poly[0].y = topY + height + thickness + thickness + thickness;
			poly[1].x = poly[0].x + thickness;				poly[1].y = poly[0].y - thickness;
			poly[2].x = poly[1].x + height;					poly[2].y = poly[1].y;
			poly[3].x = poly[2].x + thickness;				poly[3].y = poly[2].y + thickness;
			poly[4].x = poly[2].x;							poly[4].y = poly[3].y + thickness;
			poly[5].x = poly[1].x;							poly[5].y = poly[4].y;
			poly[6].x = poly[0].x;						    poly[6].y = poly[0].y;

			if (seg7)
			{
				oBrush = (HBRUSH)SelectObject(hDC, hBrush);
				oPen = (HPEN)SelectObject(hDC, hPen);
			}
			else
			{
				oBrush = (HBRUSH)SelectObject(hDC, hBrushBack);
				oPen = (HPEN)SelectObject(hDC, hPenBack);
			}
			Polygon(hDC, poly, 7);
			SelectObject(hDC, oBrush);
			SelectObject(hDC, oPen);
			leftX += height + thickness + thickness + gap + gap + height;
		}
		text++;
	}

	DeleteObject(hBrush);
	DeleteObject(hPen);
	DeleteObject(hBrushBack);
	DeleteObject(hPenBack);

}

void DrawLED(wxDC& dc, wxColor color, int left, int top, int width)
{
	HDC hDC = dc.GetHDC();

	HBRUSH hBrush, oBrush;
	hBrush = CreateSolidBrush(RGB(color.Red(), color.Green(), color.Blue()));
	oBrush = (HBRUSH)SelectObject(hDC, hBrush);
	Ellipse(hDC, left, top, left + width, top + width);
	SelectObject(hDC, oBrush);
	DeleteObject(hBrush);
}

void DrawVideo(wxDC& dc, wxColor color, int left, int top)
{
	HDC hDC = dc.GetHDC();

	HBRUSH hBrush, oBrush;

	hBrush = CreateSolidBrush(RGB(color.Red(), color.Green(), color.Blue()));
	oBrush = (HBRUSH)SelectObject(hDC, hBrush);

	POINT video[23];
	video[0].x = left + 3;  video[0].y = top;
	video[1].x = left + 12; video[1].y = top;
	video[2].x = left + 13; video[2].y = top + 1;
	video[3].x = left + 14; video[3].y = top + 2;
	video[4].x = left + 15; video[4].y = top + 3;
	video[5].x = left + 15; video[5].y = top + 4;
	video[6].x = left + 18; video[6].y = top + 1;
	video[7].x = left + 20; video[7].y = top + 1;
	video[8].x = left + 20; video[8].y = top + 9;
	video[9].x = left + 18; video[9].y = top + 9;
	video[10].x = left + 15; video[10].y = top + 5;
	video[11].x = left + 15; video[11].y = top + 7;
	video[12].x = left + 14; video[12].y = top + 8;
	video[13].x = left + 13; video[13].y = top + 9;
	video[14].x = left + 12; video[14].y = top + 10;
	video[15].x = left + 3; video[15].y = top + 10;
	video[16].x = left + 2; video[16].y = top + 9;
	video[17].x = left + 1; video[17].y = top + 8;
	video[18].x = left; video[18].y = top + 7;
	video[19].x = left; video[19].y = top + 3;
	video[20].x = left + 1; video[20].y = top + 2;
	video[21].x = left + 2; video[21].y = top + 1;
	video[22].x = left + 3; video[22].y = top;

	Polygon(hDC, video, 23);

	SelectObject(hDC, oBrush);

	DeleteObject(hBrush);
}

void DrawMultipleMonitors(wxDC& dc, wxColor color, int left, int top)
{
	HDC hDC = dc.GetHDC();

	HPEN hPen, oPen;
	COLORREF clr = RGB(color.Red(), color.Green(), color.Blue());
	hPen = CreatePen(PS_SOLID, 1, clr);
	oPen = (HPEN)SelectObject(hDC, hPen);

	MoveToEx(hDC, left, top + 4, NULL);
	LineTo(hDC, left + 6, top + 4);
	LineTo(hDC, left + 6, top + 9);
	LineTo(hDC, left, top + 9);
	LineTo(hDC, left, top + 4);

	MoveToEx(hDC, left + 4, top + 1, NULL);
	LineTo(hDC, left + 10, top + 1);
	LineTo(hDC, left + 10, top + 6);
	LineTo(hDC, left + 4, top + 6);
	LineTo(hDC, left + 4, top + 1);

	MoveToEx(hDC, left + 8, top + 4, NULL);
	LineTo(hDC, left + 14, top + 4);
	LineTo(hDC, left + 14, top + 9);
	LineTo(hDC, left + 8, top + 9);
	LineTo(hDC, left + 8, top + 4);

	SelectObject(hDC, oPen);
	DeleteObject(hPen);

}

void DrawSingleMonitor(wxDC& dc, wxColor color, int left, int top)
{
	HDC hDC = dc.GetHDC();

	HPEN hPen, oPen;
	COLORREF clr = RGB(color.Red(), color.Green(), color.Blue());
	hPen = CreatePen(PS_SOLID, 1, clr);
	oPen = (HPEN)SelectObject(hDC, hPen);

	MoveToEx(hDC, left + 4, top + 1, NULL);
	LineTo(hDC, left + 12, top + 1);
	LineTo(hDC, left + 12, top + 8);
	LineTo(hDC, left + 4, top + 8);
	LineTo(hDC, left + 4, top + 1);

	SelectObject(hDC, oPen);
	DeleteObject(hPen);
}



void DrawAudio(wxDC& dc, wxColor color, int left, int top)
{
	HDC hDC = dc.GetHDC();

	COLORREF foreColor = RGB(color.Red(), color.Green(), color.Blue());

	HBRUSH oBrush, hBrush;
	HPEN oPen, hPen;

	hBrush = CreateSolidBrush(foreColor);
	hPen = CreatePen(PS_SOLID, 1, foreColor);

	oBrush = (HBRUSH)SelectObject(hDC, hBrush);
	oPen = (HPEN)SelectObject(hDC, hPen);

	POINT audio[8];
	audio[0].x = left; audio[0].y = top + 5;
	audio[1].x = left + 3; audio[1].y = top + 5;
	audio[2].x = left + 7; audio[2].y = top + 2;
	audio[3].x = left + 9; audio[3].y = top + 2;
	audio[4].x = left + 9; audio[4].y = top + 10;
	audio[5].x = left + 7; audio[5].y = top + 10;
	audio[6].x = left + 3; audio[6].y = top + 8;
	audio[7].x = left; audio[7].y = top + 8;

	Polygon(hDC, audio, 8);

	MoveToEx(hDC, left + 12, top + 6, NULL);
	LineTo(hDC, left + 12, top + 10);

	MoveToEx(hDC, left + 14, top + 4, NULL);
	LineTo(hDC, left + 14, top + 12);

	SelectObject(hDC, oBrush);
	SelectObject(hDC, oPen);
	DeleteObject(hBrush);
	DeleteObject(hPen);

}

void DrawBell(wxDC& dc, wxColor color, int left, int top)
{
	HDC hDC = dc.GetHDC();

	COLORREF foreColor = RGB(color.Red(), color.Green(), color.Blue());

	HBRUSH oBrush, hBrush;
	HPEN oPen, hPen;

	hBrush = CreateSolidBrush(foreColor);
	hPen = CreatePen(PS_SOLID, 1, foreColor);

	oBrush = (HBRUSH)SelectObject(hDC, hBrush);
	oPen = (HPEN)SelectObject(hDC, hPen);

	POINT bell[19];
	bell[0].x = left + 7; bell[0].y = top;
	bell[1].x = left + 8; bell[1].y = top;
	bell[2].x = left + 9; bell[2].y = top + 1;
	bell[3].x = left + 9; bell[3].y = top + 2;
	bell[4].x = left + 11; bell[4].y = top + 2;
	bell[5].x = left + 13; bell[5].y = top + 5;
	bell[6].x = left + 13; bell[6].y = top + 9;
	bell[7].x = left + 15; bell[7].y = top + 12;
	bell[8].x = left + 10; bell[8].y = top + 12;
	bell[9].x = left + 9; bell[9].y = top + 13;
	bell[10].x = left + 6; bell[10].y = top + 13;
	bell[11].x = left + 5; bell[11].y = top + 12;
	bell[12].x = left + 0; bell[12].y = top + 12;
	bell[13].x = left + 2; bell[13].y = top + 9;
	bell[14].x = left + 2; bell[14].y = top + 5;
	bell[15].x = left + 4; bell[15].y = top + 2;
	bell[16].x = left + 6; bell[16].y = top + 2;
	bell[17].x = left + 6; bell[17].y = top + 1;
	bell[18].x = left + 7; bell[18].y = top;

	Polygon(hDC, bell, 19);

	SelectObject(hDC, oBrush);
	SelectObject(hDC, oPen);
	DeleteObject(hBrush);
	DeleteObject(hPen);

}


void DrawBevel(wxDC& dc, wxColor color, wxColor borderColor, wxPoint top, wxSize size, int depth, bool raisedFlg)
{
	//int rTopDiff, rBotDiff, gTopDiff, gBotDiff, bTopDiff, bBotDiff;
	//int rRightDiff, gRightDiff, bRightDiff;
	//int rLeftDiff, gLeftDiff, bLeftDiff;

	//int rTop, gTop, bTop;
	//int rRight, gRight, bRight;
	//int rLeft, gLeft, bLeft;
	//int rBottom, gBottom, bBottom;

	//if (raisedFlg)
	//{
	//	rTop = gTop = bTop = 0xff;
	//	rRight = gRight = bRight = 0x7f;
	//	rLeft = gLeft = bLeft = 0x3f;
	//	rBottom = bBottom = gBottom = 0;
	//}
	//else
	//{
	//	rTop = gTop = bTop = 0;
	//	rRight = gRight = bRight = 0x3f;
	//	rLeft = gLeft = bLeft = 0x7f;
	//	rBottom = bBottom = gBottom = 0xff;

	//}

	//rTopDiff = (rTop - color.Red()) / depth;
	//rLeftDiff = (rLeft - color.Red()) / depth;
	//rRightDiff = (rRight - color.Red()) / depth;
	//rBotDiff = (rBottom - color.Red()) / depth;

	//gTopDiff = (gTop - color.Green()) / depth;
	//gLeftDiff = (gLeft - color.Green()) / depth;
	//gRightDiff = (gRight - color.Green()) / depth;
	//gBotDiff = (gBottom - color.Green()) / depth;

	//bTopDiff = (bTop - color.Blue()) / depth;
	//bLeftDiff = (bLeft - color.Blue()) / depth;
	//bRightDiff = (bRight - color.Blue()) / depth;
	//bBotDiff = (bBottom - color.Blue()) / depth;

	wxRect rect;

	rect.SetTopLeft(top);
	rect.SetSize(size);

	HDC hDC = dc.GetHDC();
	int lineWidth = (int)(depth * 0.2f);
	if (lineWidth == 0)
		lineWidth = 1;
	unsigned char rNew, gNew, bNew;
	rNew = (unsigned char)(borderColor.Red() * 0.8);
	gNew = (unsigned char)(borderColor.Green() * 0.8);
	bNew = (unsigned char)(borderColor.Blue() * 0.8);
	HPEN hDarkPen = CreatePen(PS_SOLID, lineWidth, RGB(rNew,gNew,bNew));
	rNew = (unsigned char)(borderColor.Red() + (0.8 * (255 - color.Red())));
	gNew = (unsigned char)(borderColor.Green() + (0.8 * (255 - color.Green())));
	bNew = (unsigned char)(borderColor.Blue() + (0.8 * (255 - color.Blue())));
	HPEN hLightPen = CreatePen(PS_SOLID, lineWidth, RGB(rNew, gNew, bNew));
	rNew = (unsigned char)(borderColor.Red() + (0.95 * (255 - color.Red())));
	gNew = (unsigned char)(borderColor.Green() + (0.95 * (255 - color.Green())));
	bNew = (unsigned char)(borderColor.Blue() + (0.95 * (255 - color.Blue())));
	HPEN hMaxLightPen = CreatePen(PS_SOLID, lineWidth, RGB(rNew, gNew, bNew));
	HPEN oldPen = (HPEN)SelectObject(hDC, hLightPen);
	MoveToEx(hDC, rect.GetLeft(), rect.GetTop(), NULL);
	LineTo(hDC, rect.GetRight(), rect.GetTop());
	LineTo(hDC, rect.GetRight(), rect.GetBottom());
	SelectObject(hDC, hDarkPen);
	LineTo(hDC, rect.GetLeft(), rect.GetBottom());
	LineTo(hDC, rect.GetLeft(), rect.GetTop());
	SelectObject(hDC, oldPen);
	rNew = (unsigned char)(borderColor.Red() * 0.9);
	gNew = (unsigned char)(borderColor.Green() * 0.9);
	bNew = (unsigned char)(borderColor.Blue() * 0.9);
	HPEN hDarkPenBorder = CreatePen(PS_SOLID, lineWidth, RGB(rNew, gNew, bNew));
	rNew = (unsigned char)(borderColor.Red() + (0.9 * (255 - color.Red())));
	gNew = (unsigned char)(borderColor.Green() + (0.9 * (255 - color.Green())));
	bNew = (unsigned char)(borderColor.Blue() + (0.9 * (255 - color.Blue())));
	HPEN hLightPenBorder = CreatePen(PS_SOLID, lineWidth, RGB(rNew, gNew, bNew));
	int rectOffset = 1;
	if (depth > 5)
	{
		oldPen = (HPEN)SelectObject(hDC, hLightPenBorder);
		MoveToEx(hDC, rect.GetLeft() + lineWidth, rect.GetTop() + lineWidth, NULL);
		LineTo(hDC, rect.GetRight() - lineWidth, rect.GetTop() + lineWidth);
		LineTo(hDC, rect.GetRight() - lineWidth, rect.GetBottom() - lineWidth);
		SelectObject(hDC, hDarkPenBorder);
		LineTo(hDC, rect.GetLeft() + lineWidth, rect.GetBottom() - lineWidth);
		LineTo(hDC, rect.GetLeft() + lineWidth, rect.GetTop() + lineWidth - lineWidth);
		rectOffset = lineWidth + lineWidth + lineWidth;
		MoveToEx(hDC, rect.GetLeft() + rectOffset, rect.GetTop() + rectOffset, NULL);
		LineTo(hDC, rect.GetRight() - rectOffset, rect.GetTop() + rectOffset);
		LineTo(hDC, rect.GetRight() - rectOffset, rect.GetBottom() - rectOffset);
		SelectObject(hDC, hLightPenBorder);
		LineTo(hDC, rect.GetLeft() + rectOffset, rect.GetBottom() - rectOffset);
		LineTo(hDC, rect.GetLeft() + rectOffset, rect.GetTop() + rectOffset - lineWidth);
	}
	if (depth > 2)
	{
		SelectObject(hDC, hMaxLightPen);
		rectOffset = lineWidth + lineWidth;
		MoveToEx(hDC, rect.GetLeft() + rectOffset, rect.GetTop() + rectOffset, NULL);
		LineTo(hDC, rect.GetRight() - rectOffset, rect.GetTop() + rectOffset);
		LineTo(hDC, rect.GetRight() - rectOffset, rect.GetBottom() - rectOffset);
		LineTo(hDC, rect.GetLeft() + rectOffset, rect.GetBottom() - rectOffset);
		LineTo(hDC, rect.GetLeft() + rectOffset, rect.GetTop() + rectOffset - lineWidth);
	}
	if (depth > 5)
		rectOffset = lineWidth + lineWidth + lineWidth + lineWidth;
	else if (depth > 2)
		rectOffset = lineWidth + lineWidth;
	else
		rectOffset = lineWidth;

	SelectObject(hDC, hDarkPen);
	MoveToEx(hDC, rect.GetLeft() + rectOffset, rect.GetTop() + rectOffset, NULL);
	LineTo(hDC, rect.GetRight() - rectOffset, rect.GetTop() + rectOffset);
	LineTo(hDC, rect.GetRight() - rectOffset, rect.GetBottom() - rectOffset);
	SelectObject(hDC, hLightPen);
	LineTo(hDC, rect.GetLeft() + rectOffset, rect.GetBottom() - rectOffset);
	LineTo(hDC, rect.GetLeft() + rectOffset, rect.GetTop() + rectOffset - lineWidth);
	SelectObject(hDC, oldPen);

	DeleteObject(hDarkPen);
	DeleteObject(hLightPen);
	DeleteObject(hDarkPenBorder);
	DeleteObject(hLightPenBorder);
	DeleteObject(hMaxLightPen);


	//for (int i = 0; i < depth - 1; i++)
	//{
	//	dc.SetPen(wxPen(RGB(rLeft, gLeft, bLeft)));
	//	dc.DrawLine(rect.GetTopLeft(), rect.GetBottomLeft());
	//	dc.SetPen(wxPen(RGB(rBottom, gBottom, bBottom)));
	//	dc.DrawLine(rect.GetBottomLeft(), rect.GetBottomRight());
	//	dc.SetPen(wxPen(RGB(rTop, gTop, bTop)));
	//	dc.DrawLine(rect.GetTopLeft(), rect.GetTopRight());
	//	dc.SetPen(wxPen(RGB(rRight, gRight, bRight)));
	//	dc.DrawLine(rect.GetTopRight(), rect.GetBottomRight());
	//	rect = rect.Deflate(wxSize(1, 1));
	//	if (raisedFlg)
	//	{
	//		rBottom -= rBotDiff;
	//		gBottom -= gBotDiff;
	//		bBottom -= bBotDiff;
	//		rTop -= rTopDiff;
	//		gTop -= gTopDiff;
	//		bTop -= bTopDiff;
	//		rLeft -= rLeftDiff;
	//		gLeft -= gLeftDiff;
	//		bLeft -= bLeftDiff;
	//		rRight -= rRightDiff;
	//		gRight -= gRightDiff;
	//		bRight -= bRightDiff;
	//	}
	//	else
	//	{
	//		rBottom += rBotDiff;
	//		gBottom += gBotDiff;
	//		bBottom += bBotDiff;
	//		rTop += rTopDiff;
	//		gTop += gTopDiff;
	//		bTop += bTopDiff;
	//		rLeft += rLeftDiff;
	//		gLeft += gLeftDiff;
	//		bLeft += bLeftDiff;
	//		rRight += rRightDiff;
	//		gRight += gRightDiff;
	//		bRight += bRightDiff;
	//	}
	//}
	rect = rect.Deflate(wxSize(depth, depth-1));
	HPEN rectPen = CreatePen(PS_SOLID, 1, RGB(color.Red(), color.Green(), color.Blue()));
	oldPen = (HPEN)SelectObject(hDC, rectPen);
	rect.SetLeft(rect.GetLeft() - 1);
	rect.SetRight(rect.GetRight() + 2);

	HBRUSH rectBrush = CreateSolidBrush(RGB(color.Red(), color.Green(), color.Blue()));
	HBRUSH oldBrush = (HBRUSH)SelectObject(hDC, rectBrush);
	//dc.SetPen(wxPen(color));
	//dc.SetBrush(wxBrush(color));
	//dc.DrawRectangle(rect);
	Rectangle(hDC, rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());

	SelectObject(hDC, oldPen);
	SelectObject(hDC, oldBrush);
	DeleteObject(rectBrush);
	DeleteObject(rectPen);
}

void GetScreenDataDetails(std::list <struct monitorDetails *> *pMonitorList)
{
	HDC hDC = GetDC(NULL);
	EnumDisplayMonitors(hDC, 0, CreateMonitorDataEnum, (LPARAM)pMonitorList);
	DeleteDC(hDC);
}

static BOOL CALLBACK CreateMonitorDataEnum(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor, LPARAM pData)
{
	std::list <struct monitorDetails *> *pMonitorList = reinterpret_cast <std::list < struct monitorDetails *> *>(pData);

	struct monitorDetails *pMonitor = DBG_NEW struct monitorDetails;
	ZeroMemory(pMonitor, sizeof(struct monitorDetails));
	pMonitor->hMon = hMon;
	pMonitor->monitorInfo.cbSize = sizeof(MONITORINFOEX);
	GetMonitorInfo(hMon, &pMonitor->monitorInfo);
	pMonitor->bSelect = true;
	pMonitorList->push_back(pMonitor);
	return true;
}

