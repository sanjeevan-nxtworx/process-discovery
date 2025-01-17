#include "stdafx.h"
#include "HorizScrollBarPanel.h"
#include "UIUtility.h"
#include "CustomEvents.h"

wxDEFINE_EVENT(PROCOAN_HSCROLL_MOVEABSOLUTE, wxCommandEvent);

BEGIN_EVENT_TABLE(HorizScrollBarPanel, wxPanel)
// catch paint events

EVT_MOTION(HorizScrollBarPanel::OnMouseMove)
EVT_LEFT_DOWN(HorizScrollBarPanel::OnLeftDown)
EVT_LEFT_UP(HorizScrollBarPanel::OnLeftUp)
EVT_PAINT(HorizScrollBarPanel::paintEvent)

END_EVENT_TABLE()



HorizScrollBarPanel::HorizScrollBarPanel(wxPanel *parent, ULONG id, wxPoint pt, wxSize size) :
	wxPanel(parent, id, pt, size, wxNO_BORDER)
{
	controlID = id;
	pParent = parent;
	backColor = wxColor(RGB(0x7f, 0x7f, 0x7f));
	knobColor = wxColor(RGB(0, 0, 0xff));
	segColor = wxColor(RGB(0x7f, 0, 0));
	maxVal = 100L;
	currVal = 0L;
	thumbSize = 5;
}

void HorizScrollBarPanel::paintEvent(wxPaintEvent & WXUNUSED(evt))
{
	wxPaintDC dc(this);
	render(dc);
}

void HorizScrollBarPanel::paintNow()
{
	wxClientDC dc(this);
	render(dc);
}


void HorizScrollBarPanel::OnLeftDown(wxMouseEvent & event)
{
	wxSize winSize = this->GetSize();
	int currPos = (currVal * winSize.GetWidth()) / maxVal;

	int sliderTop = winSize.GetHeight() / 2 - 3;
	wxPoint pos = event.GetPosition();

	if (wxRect(currPos - 6, sliderTop, 12, 6).Contains(pos))
	{
		CaptureMouse();
	}


}

void HorizScrollBarPanel::OnLeftUp(wxMouseEvent &event)
{
	if (HasCapture())
	{
		ReleaseMouse();
	}
	else
	{
		wxPoint pt = event.GetPosition();
		wxSize winSize = this->GetSize();
		currVal = (pt.x * maxVal) / winSize.GetWidth();
		//paintNow();
		Refresh();
	}
	wxCommandEvent customEvent(PROCOAN_HSCROLL_MOVEABSOLUTE, GetId());
	customEvent.SetEventObject(this);

	// Do send it
	ProcessWindowEvent(customEvent);
}

void HorizScrollBarPanel::OnMouseMove(wxMouseEvent & event)
{
	wxPoint pt = event.GetPosition();
	wxSize winSize = this->GetSize();

	if (HasCapture())
	{
		currVal = (pt.x 	* maxVal) / winSize.GetWidth();
		//paintNow();
		Refresh();
		wxCommandEvent customEvent(PROCOAN_HSCROLL_MOVEABSOLUTE, GetId());
		customEvent.SetEventObject(this);

		// Do send it
		ProcessWindowEvent(customEvent);
	}
}


void HorizScrollBarPanel::render(wxDC&  dc)
{
	wxSize winSize = this->GetSize();
	dc.SetBrush(wxBrush(backColor));
	dc.SetPen(wxPen(backColor));
	dc.DrawRectangle(wxPoint(0, 0), winSize);
	if (maxVal == 0)
		return;
	int currPos = (currVal * winSize.GetWidth()) / maxVal;
	int rightPos = (thumbSize * thumbSize) / maxVal;
	dc.SetBrush(wxBrush(knobColor));
	dc.SetPen(wxPen(knobColor));

	dc.DrawRoundedRectangle(wxPoint(currPos, 1), wxSize(rightPos, winSize.GetHeight() - 2), 2);
}

void HorizScrollBarPanel::SetBackColor(wxColor color)
{
	backColor = color;
	//paintNow();
	Refresh();
}

wxColor HorizScrollBarPanel::GetBackColor(void)
{
	return backColor;
}

void HorizScrollBarPanel::SetKnobColor(wxColor color)
{
	knobColor = color;
	//paintNow();
	Refresh();
}

wxColor HorizScrollBarPanel::GetKnobColor(void)
{
	return knobColor;
}

void HorizScrollBarPanel::SetSegmentColor(wxColor color)
{
	segColor = color;
	//paintNow();
	Refresh();
}

wxColor HorizScrollBarPanel::GetSegmentColor(void)
{
	return segColor;
}

void HorizScrollBarPanel::SetMaxValue(ULONG value)
{
	maxVal = value;
	//paintNow();
	Refresh();
}

ULONG HorizScrollBarPanel::GetMaxValue(void)
{
	return maxVal;
}

void HorizScrollBarPanel::SetCurrentValue(ULONG value)
{
	currVal = value;
	//paintNow();
	Refresh();
}

ULONG HorizScrollBarPanel::GetCurrentValue(void)
{
	return currVal;
}

void HorizScrollBarPanel::SetThumbSize(ULONG value)
{
	thumbSize = value;
	//paintNow();
	Refresh();
}

ULONG HorizScrollBarPanel::GetThumbSize(void)
{
	return thumbSize;
}

