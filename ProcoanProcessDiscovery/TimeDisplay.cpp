#include "stdafx.h"
#include "TimeDisplay.h"

BEGIN_EVENT_TABLE(TimeDisplay, wxWindow)
EVT_PAINT(TimeDisplay::paintEvent)
END_EVENT_TABLE()

TimeDisplay::TimeDisplay(wxWindow * parent, ULONG id, wxPoint pt, wxSize size) : wxWindow(parent, id, pt, size, wxNO_BORDER)
{
	pParent = parent;
	SetBackgroundColour(wxColour(RGB(0x00, 0x00, 0xff)));
}

void TimeDisplay::paintNow()
{
	wxClientDC dc(this);
	render(dc);
}

void TimeDisplay::SetTimeText(wxString timeVal)
{
	timeText = timeVal;
	//paintNow();
	Refresh();
}

void TimeDisplay::paintEvent(wxPaintEvent & WXUNUSED(evt))
{
	wxPaintDC dc(this);
	render(dc);
}

void TimeDisplay::render(wxDC&  dc)
{
	HDC hDC = dc.GetHDC();

	HFONT hFont = CreateFont(12, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, FIXED_PITCH | FF_MODERN, TEXT("Arial"));
	HFONT hFontOld = (HFONT)SelectObject(hDC, hFont);

	wxSize sz = GetSize();
	HBRUSH hBrush = CreateSolidBrush(RGB(0x0, 0x0, 0xff));
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);
	RECT rect;
	rect.left = 0;
	rect.top = 3;
	rect.right = sz.GetWidth();
	rect.bottom = sz.GetHeight();

	Rectangle(hDC, 0, 0, sz.GetWidth(), sz.GetHeight());
	DrawText(hDC, timeText.c_str(), -1, &rect, DT_CENTER);
	SelectObject(hDC, hOldBrush);
	SelectObject(hDC, hFontOld);
	DeleteObject(hFont);
	DeleteObject(hBrush);
}