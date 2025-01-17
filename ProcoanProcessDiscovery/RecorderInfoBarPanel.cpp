#include "stdafx.h"
#include "RecorderInfoBarPanel.h"
#include "UIUtility.h"

BEGIN_EVENT_TABLE(RecorderInfoBarPanel, wxPanel)
// catch paint events
EVT_PAINT(RecorderInfoBarPanel::paintEvent)

END_EVENT_TABLE()



RecorderInfoBarPanel::RecorderInfoBarPanel(wxWindow *parent, wxSize size) :
	wxPanel(parent, -1, wxPoint(0, 37), wxSize(size), wxNO_BORDER)
{
	pParent = parent;
}

void RecorderInfoBarPanel::paintEvent(wxPaintEvent & WXUNUSED(evt))
{
	wxPaintDC dc(this);
	render(dc);
}

void RecorderInfoBarPanel::paintNow()
{
	wxClientDC dc(this);
	render(dc);
}

void RecorderInfoBarPanel::SetDirectory(wxString dir)
{
	directory = dir;
}

void RecorderInfoBarPanel::SetFilename(wxString fname)
{
	filename = fname;
}

void RecorderInfoBarPanel::render(wxDC&  dc)
{
	wxSize winSize = this->GetSize();
	RECT rect;
	rect.left = 10;
	rect.top = 4;
	rect.right = winSize.GetWidth();
	rect.bottom = winSize.GetHeight();

	DrawBevel(dc, wxColor(0xc0, 0xc0, 0xc0), wxColor(0xc0, 0xc0, 0xc0), wxPoint(0, 0), winSize, 2, true);

	HDC hDC = dc.GetHDC();

	HFONT hFont = CreateFont(16, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, FIXED_PITCH | FF_MODERN, TEXT("Arial"));
	HFONT hFontOld = (HFONT)SelectObject(hDC, hFont);
	int oMode = SetBkMode(hDC, TRANSPARENT);
	wxString text = directory + filename;
	COLORREF oColor = SetTextColor(hDC, RGB(50, 50, 50));
	DrawText(hDC, text.c_str(), -1, &rect, DT_LEFT);
	SelectObject(hDC, hFontOld);
	DeleteObject(hFont);
	SetBkMode(hDC, oMode);
	SetTextColor(hDC, oColor);
}