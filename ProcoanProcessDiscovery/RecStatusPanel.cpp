#include "stdafx.h"
#include "RecStatusPanel.h"
#include "UIUtility.h"
#include "wx/graphics.h"

BEGIN_EVENT_TABLE(RecStatusPanel, wxPanel)
// some useful events
// catch paint events
EVT_PAINT(RecStatusPanel::paintEvent)
END_EVENT_TABLE()

RecStatusPanel::RecStatusPanel(wxWindow *parent, wxPoint pt, wxSize size) :
	wxPanel(parent, -1, pt, size, wxNO_BORDER)
{
	pParent = parent;
}

void RecStatusPanel::SetValue(wxString str)
{
	textVal = str;
	//paintNow();
	Refresh();
}

void RecStatusPanel::paintEvent(wxPaintEvent & WXUNUSED(evt))
{
	wxPaintDC dc(this);
	render(dc);
}

void RecStatusPanel::paintNow()
{
	wxClientDC dc(this);
	render(dc);
}

void RecStatusPanel::render(wxDC&  dc)
{
	dc.SetBrush(wxBrush(backColor));
	dc.SetPen(wxPen(wxColor(backColor)));
	dc.DrawRectangle(wxPoint(0, 0), GetSize());
	SetFont(wxFont(12, wxFONTFAMILY_SWISS, wxNORMAL, wxBOLD));
	dc.SetTextForeground(foreColor);
	dc.SetTextBackground(backColor);
	dc.DrawText(textVal, wxPoint(2, 2));
}

void RecStatusPanel::SetForeColor(wxColor wxforeColor)
{
	foreColor = wxforeColor;
}

void RecStatusPanel::SetBackColor(wxColor wxbackColor)
{
	backColor = wxbackColor;
	this->SetBackgroundColour(backColor);
}
