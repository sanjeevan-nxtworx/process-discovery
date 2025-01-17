#include "stdafx.h"
#include "LEDDisplayPanel.h"
#include "UIUtility.h"
#include "wx/graphics.h"

BEGIN_EVENT_TABLE(LEDDisplayPanel, wxPanel)
// some useful events
// catch paint events
EVT_PAINT(LEDDisplayPanel::paintEvent)
END_EVENT_TABLE()

LEDDisplayPanel::LEDDisplayPanel(wxWindow *parent, wxPoint pt, wxSize size) :
	wxPanel(parent, -1, pt, size, wxNO_BORDER)
{
	pParent = parent;
}

void LEDDisplayPanel::paintEvent(wxPaintEvent & WXUNUSED(evt))
{
	wxPaintDC dc(this);
	render(dc);
}

void LEDDisplayPanel::paintNow()
{
	wxClientDC dc(this);
	render(dc);
}

void LEDDisplayPanel::render(wxDC&  dc)
{
	if (!textValue.IsEmpty())
		DrawLEDString(dc, textValue.char_str(), foreColor, 5, 5, thickness, height, gap);
}

void LEDDisplayPanel::SetHeight(int nHeight)
{
	height = nHeight;
}

void LEDDisplayPanel::SetThickness(int nThickness)
{
	thickness = nThickness;
}

void LEDDisplayPanel::SetGap(int nGap)
{
	gap = nGap;
}

void LEDDisplayPanel::SetForeColor(wxColor wxforeColor)
{
	foreColor = wxforeColor;
}

void LEDDisplayPanel::SetBackColor(wxColor wxbackColor)
{
	backColor = wxbackColor;
	this->SetBackgroundColour(backColor);
}

void LEDDisplayPanel::SetText(wxString value)
{
	textValue = value;
}